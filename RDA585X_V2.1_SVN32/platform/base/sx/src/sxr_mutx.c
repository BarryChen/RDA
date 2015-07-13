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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/base/sx/src/sxr_mutx.c $
//	$Author: licheng $
//	$Date: 2012-06-11 10:48:24 +0800 (星期一, 11 六月 2012) $
//	$Revision: 15863 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file sxs_mutx.c
/// Systeme executif task mutual exclusion management.
//
////////////////////////////////////////////////////////////////////////////////

#include "sxs_type.h"
#include "sxr_mutx.h"
#include "sxp_mutx.h"
#include "sxr_sbx.h"
#include "sxp_debug.h"
#include "sxr_tksd.hp"



// =============================================================================
// sxr_TakeMutex
// -----------------------------------------------------------------------------
/// Take the mutex.
/// @param Id mutex Id.
/// @return UserId to give to sxr_ReleaseMutex()
// =============================================================================
u8    sxr_TakeMutex (u8 Id)
{
    u32   Status;
    u32   CallerAdd;
    u32   K1;
    u8    TaskId;

    SXS_GET_RA(&CallerAdd);

    // ensure mutex is allocated
    SX_MUTX_ASSERT(Id < SXR_NB_MAX_MUTEX, "sxr_ReleaseMutex Invalid mutex ID %d", Id);
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED, "sxr_ReleaseMutex mutex is not allocated!");

    /// @todo move that to hal
    asm volatile("move %0, $27":"=d"(K1));
    SX_MUTX_ASSERT(0 == K1, "sxr_TakeMutex Must be called from a Task!");
    
    Status = sxr_EnterSc ();
    TaskId = sxr_Task.Active;
    // current task already has the mutex ?
    if (sxr_Mutex.Queue [Id].TaskId != TaskId)
    {
        // other task
        // the task switch that might occur here will restore the Sc state.
        sxr_TakeSemaphore (sxr_Mutex.Queue [Id].SemaId);
        sxr_Mutex.Queue [Id].TaskId = TaskId;
        sxr_Mutex.Queue [Id].CallerAdd = CallerAdd;
    }
    sxr_Mutex.Queue [Id].Count++;

    sxr_ExitSc (Status);
    return TaskId;
}

// =============================================================================
// sxr_ReleaseMutex
// -----------------------------------------------------------------------------
/// Release a mutex.
/// @param Id mutex Id.
/// @param UserId from sxr_TakeMutex()
// =============================================================================
void  sxr_ReleaseMutex (u8 Id, u8 UserId)
{
    u32   K1;
    u32   Status = sxr_EnterSc ();
    u8    TaskId = sxr_Task.Active;
    // ensure mutex is allocated
    SX_MUTX_ASSERT(Id < SXR_NB_MAX_MUTEX, "sxr_ReleaseMutex Invalid mutex ID %d", Id);
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED, "sxr_ReleaseMutex mutex is not allocated!");
    // ensure mutex is taken
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Count != 0, "sxr_ReleaseMutex while not taken!");
    /// @todo move that to hal
    asm volatile("move %0, $27":"=d"(K1));
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].TaskId == UserId, "sxr_ReleaseMutex Must be released by the UserId that took it!");
    /// @todo 0x10000000 is from halp_irq_handler.h
    SX_MUTX_ASSERT(0x01000000 > K1, "sxr_ReleaseMutex called from a Job!");
    // allow IRQ to free a mutex for a task...
    SX_MUTX_ASSERT((K1 != 0) || (TaskId == UserId), "sxr_ReleaseMutex Must be released by the Task that took it!");


    sxr_Mutex.Queue [Id].Count--;

    if (sxr_Mutex.Queue [Id].Count == 0)
    {
        sxr_Mutex.Queue [Id].TaskId = SXR_NO_TASK;
        sxr_ReleaseSemaphore (sxr_Mutex.Queue [Id].SemaId);
    }
    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_MutexHot
// -----------------------------------------------------------------------------
/// Check if a mutex is free.
/// @param Id mutex Id.
/// @return \c TRUE when the mutex is free, FALSE when it is hot.
// =============================================================================
u8    sxr_MutexHot (u8 Id)
{
    // ensure mutex is allocated
    SX_MUTX_ASSERT(Id < SXR_NB_MAX_MUTEX, "sxr_ReleaseMutex Invalid mutex ID %d", Id);
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED, "sxr_ReleaseMutex mutex is not allocated!");
    return sxr_SemaphoreHot(sxr_Mutex.Queue [Id].SemaId);
}

