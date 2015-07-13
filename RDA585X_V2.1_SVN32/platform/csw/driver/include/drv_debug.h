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
//  $HeadURL: http://svn.rdamicro.com/svn/developing/Sources/csw/branches/Total_BJ/include/DRV_debug.h $ //
//	$Author: shug $                                                        // 
//	$Date$                     //   
//	$Revision: 36105 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file drv_debug.h                                                          //
/// That file provides assert, trace and profiling macros for use in          //
/// CSW implementation                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hal_debug.h"
#include "sxs_io.h"
#include "csw_profile_codes.h"
#include "chip_id.h"

#ifndef DRV_DEBUG_H
#define DRV_DEBUG_H



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
// =============================================================================
#ifndef DRV_NO_PROFILING


// =============================================================================
//  DRV_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define DRV_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  DRV_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define DRV_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  DRV_PROFILE_WINDOW_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define DRV_PROFILE_WINDOW_ENTRY(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  DRV_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define DRV_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  DRV_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define DRV_PROFILE_PULSE(pulseName)  \
                hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_CSW, (CP_ ## pulseName))


#else // DRV_NO_PROFILING

#define DRV_PROFILE_FUNCTION_ENTER(eventName)
#define DRV_PROFILE_FUNCTION_EXIT(eventName)
#define DRV_PROFILE_WINDOW_ENTRY(eventName)
#define DRV_PROFILE_WINDOW_EXIT(eventName)
#define DRV_PROFILE_PULSE(pulseName)

#endif // DRV_NO_PROFILING

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define DRV_WARN_TRC        (_CSW | TLEVEL(1))
/// BAL specific trace
#define DRV_BAL_TRC         (_CSW | TLEVEL(2))
/// PM specific trace
#define DRV_PM_TRC          (_CSW | TLEVEL(3))
/// @todo add levels and use them !
// add more here
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define DRV_DBG_TRC         (_CSW | TLEVEL(16))

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string 
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
// Conditional Printf Usage
/*
#ifndef DRV_NO_PRINTF
#define DRV_TRACE(level, tstmap, format, ...) \
     hal_DbgTrace(_CSW |(level),tstmap,format, ##__VA_ARGS__)
#else
#define DRV_TRACE(level, tsmap, fmt, ...)
#endif
*/

#endif // DRV_DBG_H







