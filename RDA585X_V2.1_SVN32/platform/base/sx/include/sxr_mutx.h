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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/base/sx/include/sxr_mutx.h $
//	$Author: admin $
//	$Date: 2010-07-07 20:26:37 +0800 (星期三, 07 七月 2010) $
//	$Revision: 268 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file sxr_mutx.h
/// Systeme executif task mutual exclusion header file.
//
////////////////////////////////////////////////////////////////////////////////

/// @defgroup sx_mutex SX Simple Mutex
/// @par Simple Mutex
/// mutex are binary semaphores used to implement mutual exclusion at task level.
/// Compared to semaphores, the mutex can be taken more than once by the same 
/// task, in that case it must be released as many times to be actually released.
/// 
/// @{

#ifndef __SXR_MUTX_H__
#define __SXR_MUTX_H__

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// sxr_MutexInit
// -----------------------------------------------------------------------------
/// Mutual exclusion data context initialization.
// =============================================================================
void  sxr_MutexInit (void);


// =============================================================================
// sxr_NewMutex
// -----------------------------------------------------------------------------
/// provide a free mutex.
/// @return mutex Id.
// =============================================================================
u8    sxr_NewMutex (void);
// =============================================================================
// sxr_FreeMutex
// -----------------------------------------------------------------------------
/// Free a previously allocated mutex.
/// @param Id mutex Id.
// =============================================================================
void  sxr_FreeMutex (u8 Id);

// =============================================================================
// sxr_TakeMutex
// -----------------------------------------------------------------------------
/// Take the mutex.
/// @param Id mutex Id.
/// @return UserId to give to sxr_ReleaseMutex()
// =============================================================================
u8    sxr_TakeMutex (u8 Id);

// =============================================================================
// sxr_ReleaseMutex
// -----------------------------------------------------------------------------
/// Release a mutex.
/// @param Id mutex Id.
/// @param UserId from sxr_TakeMutex()
// =============================================================================
void  sxr_ReleaseMutex (u8 Id, u8 UserId);

// =============================================================================
// sxr_MutexHot
// -----------------------------------------------------------------------------
/// Check if a mutex is free.
/// @param Id mutex Id.
/// @return \c TRUE when the mutex is free, FALSE when it is hot.
// =============================================================================
u8    sxr_MutexHot (u8 Id);

// =============================================================================
// sxr_ChkMutex
// -----------------------------------------------------------------------------
/// Display debug information about mutexes
// =============================================================================
void  sxr_CheckMutex (void);

// =============================================================================
// sxr_ChkMutex
// -----------------------------------------------------------------------------
/// Display debug information about mutex
/// @param Id mutex Id.
// =============================================================================
void  sxr_ChkMutex (u8 Id);


#ifdef __cplusplus
}
#endif

#endif
/// @} // <-- End of sx_mutex group

