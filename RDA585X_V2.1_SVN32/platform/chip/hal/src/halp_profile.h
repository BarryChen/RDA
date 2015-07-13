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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_profile.h $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-07-08 16:27:00 +0800 (星期四, 08 七月 2010) $                     //   
//    $Revision: 301 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_profile.h                                                      //
/// That file provides the private API for the CoolProfiling.                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef  _HALP_PROFILE_H_
#define  _HALP_PROFILE_H_

#include "global_macros.h"
#include "debug_port.h"

#include "hal_debug.h"
#include "hal_host.h"
#include "hal_profile_codes.h"

#if defined(HAL_PROFILE_NOT_ON_ROMULATOR)

#include "hal_timers.h"
#include "sxs_rmt.h"
#include "sxs_rmc.h"
#include "string.h"

#include "hal_map.h"
#include "hal_sys.h"


#define HAL_PROFILE_ON_RAM_SAMPLE_SIZE  (8)

// =============================================================================
// hal_DbgPxtsMallocRamBuffer
// -----------------------------------------------------------------------------
/// Allocate the RAM buffer used by to record PXTS tags in RAM, and fill the 
/// appropriate global variables. If the buffer is already allocated (ie the
/// \c startAddr field is not 0), the previously allocated buffer is kept and
/// no more memory is allocated.
///
/// @param size Size of the buffer to allocate.
/// @return Pointer to the allocated buffer
// =============================================================================
PROTECTED VOID* hal_DbgPxtsMallocRamBuffer(UINT32 size);


// =============================================================================
// hal_DbgPxtsFreeRamBuffer
// -----------------------------------------------------------------------------
/// Free the RAM buffer used by to record PXTS tags in RAM, and clear the 
/// appropriate global variables. If the buffer is already freed (ie the
/// \c startAddr field is not 0), nothing is done.
// =============================================================================
PROTECTED VOID hal_DbgPxtsFreeRamBuffer(VOID);

#endif // HAL_PROFILE_NOT_ON_ROMULATOR


#if defined(HAL_PROFILE_NOT_ON_ROMULATOR) || defined(HAL_PROFILE_ON_BUFFER)
// =============================================================================
// hal_DbgPxtsOpen
// -----------------------------------------------------------------------------
/// Initializes the global variable (Part of HAL Map structure) used to enable
/// the feature of the recording the PXTS tags in a RAM buffer, etc.
///
/// This function must be called by hal_Open before doing profiling.
// =============================================================================
PROTECTED VOID hal_DbgPxtsOpen(VOID);

#endif // defined(HAL_PROFILE_NOT_ON_ROMULATOR) || defined(HAL_PROFILE_ON_BUFFER)


#if defined(HAL_PROFILE_ON_BUFFER)

#if defined(HAL_PROFILE_NOT_ON_ROMULATOR)
#error "HAL_PROFILE_NOT_ON_ROMULATOR and HAL_PROFILE_ON_BUFFER are mutually exclusive!"
#endif

#include "hal_timers.h"
#include "hal_map.h"
#include "halp_debug.h"

#define HAL_PROFILE_ON_RAM_SAMPLE_SIZE  (4)

#endif // defined(HAL_PROFILE_ON_BUFFER)

// =============================================================================
// hal_DbgPxtsSendCode
// -----------------------------------------------------------------------------
/// Profiling tool (reserved for Platform profiling). To use this feature, 
/// the PXTS device must have been enabled on the signal spy (#hal_DbgPortSetup)
/// This function is reserved for platform profiling. It is used to send 
/// a serie of event to a profiling tool (like CSBJ SA Tool) through the
/// romulator. \n
/// When this function is called, it sends an event represented by its \c code
/// to the PXTS Fifo. This Fifo is read by en external tool. \n
/// \n
/// (Nota: SA tool and Coolprofile cannot be used at the same time)
/// 
/// @param code The event code to send to the profiling tool
/// @param level PXTS level on which to send the code.
// =============================================================================
INLINE VOID hal_DbgPxtsSendCode(HAL_DBG_PXTS_LEVEL_T level, UINT16 code)
{
// FIXME Merge on trace into the other
// FIXME Optimize :)
#ifdef HAL_PROFILE_NOT_ON_ROMULATOR
    UINT32 reg = 0;
    HAL_PROFILE_CONTROL_T* pControl = (HAL_PROFILE_CONTROL_T*) HAL_SYS_GET_CACHED_ADDR(&g_halMapAccess.profileControl);
    // Check there is a buffer allocated.
    if (pControl->startAddr != 0)
    {
        reg = HAL_PROFILE_CONTROL_GLOBAL_ENABLE_RAM
                   | ( 1 << (level + HAL_PROFILE_CONTROL_ENABLE_PXTS_TAG_SHIFT));
        // check that the main enable and the level is there.
        if ((pControl->config & reg) == reg)
        {
            // Atomically write time stamp and tag
            UINT32 status = hal_SysEnterCriticalSection();

            // Check availability in the buffer. (A sample is 8 bytes)
            if (pControl->writePointer <=
                pControl->startAddr + pControl->size - HAL_PROFILE_ON_RAM_SAMPLE_SIZE)
            {
                UINT32 time = hal_TimGetUpTime();
#if 0
                UINT8* writePtr = (UINT8*)pControl->writePointer;
                    memcpy(&writePtr[0], &time, 4);
                    memcpy(&writePtr[4], &code, 2);
#else
                // profile buffer is malloc-ed and should be 4-byte aligned
                UINT32* writePtr = (UINT32*)pControl->writePointer;
                writePtr[0] = time;
                writePtr[1] = (UINT32)code;
#endif
            }

            // Update pointers and status.
            // TODO Implement read pointer management.
            pControl->writePointer += HAL_PROFILE_ON_RAM_SAMPLE_SIZE;
            pControl->remainingSize =   pControl->startAddr
                                                        + pControl->size
                                                        - pControl->writePointer;
            if (pControl->writePointer >=
                pControl->startAddr + pControl->size)
            {
                pControl->status |= HAL_PROFILE_CONTROL_WRAPPED;
                pControl->writePointer = pControl->startAddr;
            }
            hal_SysExitCriticalSection(status);

        }
    }

    reg = HAL_PROFILE_CONTROL_GLOBAL_ENABLE_TRACE
                | ( 1 << (level + HAL_PROFILE_CONTROL_ENABLE_PXTS_TAG_SHIFT));
    // check that the main enable and the level is there.
    if ((pControl->config & reg) == reg)
    {
        // Atomically write time stamp and tag
        UINT32 status = hal_SysEnterCriticalSection();
        UINT32 time = hal_TimGetUpTime();
        UINT8 *packet;
        if ((packet = sxs_SendToRemote(SXS_PROFILE_RMC, NIL, 6)) != NIL)
        {
            memcpy( &packet[0], &time, 4);
            memcpy( &packet[4], &code, 2);
        }
        hal_SysExitCriticalSection(status);
    }
#else // ! HAL_PROFILE_NOT_ON_ROMULATOR

#ifdef HAL_PROFILE_ON_BUFFER
    HAL_PROFILE_CONTROL_T* pControl = (HAL_PROFILE_CONTROL_T*) HAL_SYS_GET_CACHED_ADDR(&g_halMapAccess.profileControl);
    // Check there is a buffer allocated.
    if (pControl->startAddr != 0)
    {
        UINT32 Status = hal_SysEnterCriticalSection(); 

        // Check availability in the buffer. (A sample is 4 bytes)
        if (pControl->writePointer <=
            pControl->startAddr + pControl->size - HAL_PROFILE_ON_RAM_SAMPLE_SIZE)
        {
            UINT32 time = hal_TimGetUpTime();
            UINT32* writePtr = (UINT32*)pControl->writePointer;
            *writePtr = ((time & 0xffff) << 16) | code;
        }

        // Update pointers and status.
        // TODO Implement read pointer management.
        pControl->writePointer += HAL_PROFILE_ON_RAM_SAMPLE_SIZE;
        if (pControl->writePointer >=
            pControl->startAddr + pControl->size)
        {
            pControl->writePointer = pControl->startAddr;
        }
        
        hal_SysExitCriticalSection(Status);
    }
#else // ! HAL_PROFILE_ON_BUFFER

#ifndef CHIP_HAS_PXTS_LEVELS // Granite test chip      
    hwp_debugPort->Pxts_tag_cfg = code;
#else
    hwp_debugPort->Pxts_tag[level] = code;
#endif

#endif //!HAL_PROFILE_ON_BUFFER

#endif // HAL_PROFILE_NOT_ON_ROMULATOR
}



// TODO: Maybe we want to profile allways the IRQ, even when HAL_PROFILING is
// disable. Except when we use SA profile...
//#ifdef HAL_PROFILING


#define CPMASK     0x3fff
#define CPEXITFLAG 0x8000

#define CPIRQSPACESTART 0x3f20
#define CPIRQSPACEMASK 0x1f


// =============================================================================
//  HAL_PROFILE_IRQ_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the start of a IRQ treatment.
/// This is private as the IRQ management is done by HAL.
/// Implemented that way (not through a "DbgPxtsIrqEnter" function) to avoid
/// slowing down the IRQ management.
// =============================================================================
#define HAL_PROFILE_IRQ_ENTER(n)   \
        hal_DbgPxtsSendCode(HAL_DBG_PXTS_HAL, \
                            CPIRQSPACESTART |((n) & CPIRQSPACEMASK))


// =============================================================================
//  HAL_PROFILE_IRQ_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the start of a IRQ treatment.
/// This is private as the IRQ management is done by HAL.
/// Implemented that way (not through a "DbgPxtsIrqEnter" function) to avoid
/// slowing down the IRQ management.
// =============================================================================
#define HAL_PROFILE_IRQ_EXIT(n)   \
        hal_DbgPxtsSendCode(HAL_DBG_PXTS_HAL, \
                            CPIRQSPACESTART | ((n) & CPIRQSPACEMASK) | CPEXITFLAG)


//#else // HAL_PROFILING

//#define HAL_PROFILE_IRQ_ENTER(n)
//#define HAL_PROFILE_IRQ_EXIT(n)

//#endif // HAL_PROFILING




#endif //_HALP_PROFILE_H_


