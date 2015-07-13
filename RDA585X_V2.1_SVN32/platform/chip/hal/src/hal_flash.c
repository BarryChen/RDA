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
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_flash.c $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_flash.c                                                     //
///                                                                           //
///     That files contains this function in order not to include             //
///     too much stuff in the flashprogrammer                                 //
//                                                                            //
//////////////////////////////////////////////////////////////////////////////// 

#include "cs_types.h"

#include "global_macros.h"
#include "mem_bridge.h"
#include "hal_ebc.h"
#include "halp_ebc.h"
#include "halp_sys.h"

UINT32 g_halCs0WriteSetting  = 0;

#ifdef CHIP_HAS_EBC_CS2_BUG
PROTECTED UINT32 g_halCs0TimeReg  = 0;
PROTECTED UINT32 g_halCs0ModeReg  = 0;
#endif

// ============================================================================
// hal_EbcFlashWriteEnable
// ----------------------------------------------------------------------------
/// Enables or disables writing for the flash memory accessible through the 
/// Chip Select 0.
/// This function is normally reserved for the implementation of flash driver
///
/// @param enable If \c TRUE, writing on the CS0 is allowed \n
///               If \c FALSE, the CS0 is read-only
// ============================================================================
PUBLIC VOID hal_EbcFlashWriteEnable(BOOL enable)
{

#ifndef CHIP_HAS_EBC_CS2_BUG
    UINT32 reg;
#endif

    if (enable)
    {
        g_halCs0WriteSetting = MEM_BRIDGE_WRITEALLOW;
    }
    else 
    {
        g_halCs0WriteSetting = 0;
    }
#ifdef CHIP_HAS_EBC_CS2_BUG
    hwp_memBridge->CS_Time_Write = g_halCs0TimeReg;
    hwp_memBridge->CS_Config[0].CS_Mode = (g_halCs0ModeReg & ~MEM_BRIDGE_WRITEALLOW)
                        | g_halCs0WriteSetting;
#else
    reg = hwp_memBridge->CS_Config[0].CS_Time;
    hwp_memBridge->CS_Time_Write = reg;

    reg = hwp_memBridge->CS_Config[0].CS_Mode & ~MEM_BRIDGE_WRITEALLOW; 
    hwp_memBridge->CS_Config[0].CS_Mode = reg | g_halCs0WriteSetting;

#endif
}

// ============================================================================
// hal_EbcFlashWriteEnabled
// ----------------------------------------------------------------------------
/// Test if the flash write is enable
///
/// @return If \c TRUE, writing on the CS0 is allowed \n
///               If \c FALSE, the CS0 is read-only
// ============================================================================
PUBLIC BOOL hal_EbcFlashWriteEnabled(VOID)
{
    // If g_halCs0WriteSetting is 0, the bit allowing
    // to write in CS0 is not set.
    return (g_halCs0WriteSetting != 0);
}








