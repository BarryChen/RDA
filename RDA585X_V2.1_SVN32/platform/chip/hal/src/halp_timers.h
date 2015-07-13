///////////////////////////////////////////////////////////////////////////////
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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_timers.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_timers.h
/// That file contains the timers'module private API
//                                                                            //
//////////////////////////////////////////////////////////////////////////////// 
#ifndef  _HALP_TIMERS_H_
#define  _HALP_TIMERS_H_

#include "cs_types.h"
#include "global_macros.h"
#include "timer.h"

#include "hal_debug.h"



// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// =============================================================================
//  TYPES
// =============================================================================


// ============================================================================
// HAL_TIM_MODE_T
// ----------------------------------------------------------------------------
/// Timer modes
/// All the timer can operate in three different modes :
/// - single
/// - repetitive
/// - wrap.
// ============================================================================
typedef enum  {
    HAL_TIM_SINGLE_MODE = 0,   ///< Single mode
    HAL_TIM_REPEAT_MODE  = 1,///< Repetitive mode
    HAL_TIM_WRAP_MODE   = 2      ///< Wrap mode
} HAL_TIM_MODE_T;



// ============================================================================
// HAL_TIM_WD_IRQ_HANDLER_T
// ----------------------------------------------------------------------------
/// Type of the user handler function for the Watchdog timer
// ============================================================================
typedef VOID (*HAL_TIM_WD_IRQ_HANDLER_T)(VOID);



// ============================================================================
// HAL_TIM_TICK_IRQ_HANDLER_T
// ----------------------------------------------------------------------------
/// Type of the user handler function for the OS timer
// ============================================================================
typedef VOID (*HAL_TIM_TICK_IRQ_HANDLER_T)(VOID);



// ============================================================================
// 
// ----------------------------------------------------------------------------
// ============================================================================

// ============================================================================
//  FUNCTIONS
// ============================================================================

// ============================================================================
// hal_TimWdIrqSetHandler
// ----------------------------------------------------------------------------
/// Set the function called in case of interrupt after a Watchdog.
/// @param handler Function called when a WatchDog interrupt occurs
// ============================================================================
PROTECTED VOID hal_TimWdIrqSetHandler(HAL_TIM_WD_IRQ_HANDLER_T handler);



// =============================================================================
// hal_TimWdIrqSetMask
// -----------------------------------------------------------------------------
/// @param mask Mask of the WatchDog interrupt. (\c TRUE to have the interrupt).
// ============================================================================-
PROTECTED VOID hal_TimWdIrqSetMask(BOOL mask);



// ============================================================================
// hal_TimWdIrqGetMask
// ----------------------------------------------------------------------------
/// @return The mask for the WatchDog IRQ.
// ============================================================================
PROTECTED BOOL hal_TimWdIrqGetMask(VOID);



// ============================================================================
// hal_TimTimersIrqHandler
// ----------------------------------------------------------------------------
/// Timers module IRQ handler.
// ============================================================================
PROTECTED VOID hal_TimTimersIrqHandler(UINT8 interruptId);




// =============================================================================
// hal_TimWatchDogStop
// -----------------------------------------------------------------------------
/// Stop the watchdog timer 
/// This function stops the watchdog timer. A reset will not occur because
/// of the watchdog, even if #hal_TimWatchDogKeepAlive is not called. The 
/// watchdog can be re-enabled by a call to #hal_TimWatchDogKeepAlive.
// =============================================================================
PROTECTED VOID hal_TimWatchDogStop(VOID);



// ============================================================================
// hal_TickInit
// ----------------------------------------------------------------------------
/// Initializes the OS tick timer and configure its driver.
/// This function is called by hal_Open.
// ============================================================================
PROTECTED VOID hal_TimTickOpen(VOID);



// ============================================================================
// hal_TimTickIrqHandler
// ----------------------------------------------------------------------------
/// OS tick IRQ handler, used by the IRQ module to clear the cause and call 
/// the 'user' function.
// ============================================================================ 
PROTECTED VOID hal_TimTickIrqHandler(UINT8 interruptId);





#endif //_HALP_TIMERS_H_

