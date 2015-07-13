////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_fint.c $
//    $Author: admin $
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $
//    $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file hal_fint.c
/// Manage the fint irq: handler set, masking, etc
//
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "global_macros.h"
#include "sys_irq.h"

#include "hal_fint.h"
#include "hal_mem_map.h"
#include "halp_fint.h"
#include "halp_irq.h"
#include "halp_sys.h"
#include "halp_lps.h"



#define PULSE_ID_2_BITMASK(id) (1<<((id)-SYS_IRQ_TCU0))

// Variable to store the user handler
PRIVATE HAL_FINT_IRQ_HANDLER_T g_fintRegistry = NULL;


// =============================================================================
// hal_FintIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user function called in case of FINT
/// @param handler Handler function to set
// =============================================================================
PUBLIC VOID hal_FintIrqSetHandler(HAL_FINT_IRQ_HANDLER_T handler)
{
    g_fintRegistry = handler;
}

// =============================================================================
// hal_FintIrqSetMask
// -----------------------------------------------------------------------------
/// Set the mask for the FINT
/// @param mask Mask to set. If \c TRUE, FINT are enabled. If \c FALSE,
/// they don't occur.
// =============================================================================
PUBLIC VOID hal_FintIrqSetMask(BOOL mask)
{
    if (mask)
    {
        hwp_sysIrq->Pulse_Mask_Set = SYS_IRQ_SYS_IRQ_FRAME;
    }
    else
    {
        hwp_sysIrq->Pulse_Mask_Clr = SYS_IRQ_SYS_IRQ_FRAME;
    }
}

// =============================================================================
// hal_FintIrqGetMask
// -----------------------------------------------------------------------------
/// This function is used to recover the FINT mask
/// @return The Fint mask: \c TRUE if FINT can happen, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_FintIrqGetMask(VOID)
{
    if (hwp_sysIrq->Pulse_Mask_Clr & SYS_IRQ_SYS_IRQ_FRAME)
    {
        return TRUE; // mask set
    }
    else 
    {
        return FALSE; // mask not set
    }
}

// =============================================================================
// hal_FintIrqGetStatus
// -----------------------------------------------------------------------------
/// Check the Frame interrupt status.
/// @return \c TRUE if the FINT had occured \n
///         \c FALSE otherwise
// =============================================================================
PUBLIC BOOL HAL_FUNC_INTERNAL hal_FintIrqGetStatus(VOID)
{
    if (hwp_sysIrq->Status & SYS_IRQ_SYS_IRQ_FRAME)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// =============================================================================
// hal_FintIrqHandler
// -----------------------------------------------------------------------------
/// Handles IRQ.
// =============================================================================
PROTECTED VOID HAL_FUNC_INTERNAL hal_FintIrqHandler(UINT8 interruptId)
{
    UINT32 tmpRead;
    hwp_sysIrq->Pulse_Clear = PULSE_ID_2_BITMASK(interruptId);
    tmpRead = hwp_sysIrq->Pulse_Clear;

#if (!CHIP_HAS_ASYNC_TCU)
    // TODO somewhere there: do a frequency change there if
    // one was attempted during g_halLpsCtx.lpsSkipFrameNumber != HAL_LPS_UNDEF_VALUE
    UINT32 status=hal_SysEnterCriticalSection();

    if (g_halSysLpsLock != HAL_SYS_LPS_LOCK_UNLOCKED)
    {
        g_halSysLpsLock&=~HAL_SYS_LPS_LOCK_FINT;
        if(g_halSysLpsLock == HAL_SYS_LPS_LOCK_UNLOCKED)
        {
            hal_SysUpdateSystemFrequency();
        }
    }
    hal_SysExitCriticalSection(status);
#endif

    // Call user handler
    hal_FintUserHandler();
}

// =============================================================================
// hal_FintUserHandler
// -----------------------------------------------------------------------------
/// called by hal_FintIrqHandler()
// =============================================================================
PROTECTED VOID HAL_FUNC_INTERNAL hal_FintUserHandler(VOID)
{
    // Call user handler
    if (g_fintRegistry)
        g_fintRegistry();
}

