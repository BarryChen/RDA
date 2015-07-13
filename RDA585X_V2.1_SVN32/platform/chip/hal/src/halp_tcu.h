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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_tcu.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_tcu.h
/// TCU private API. It holds the IRQ handling functions, types and mechanisms
/// to enable the TCU interrupt on the System Side. If they appear to be useful,
/// put them in the public interface.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef _HALP_TCU_H_
#define _HALP_TCU_H_


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_TCU_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// User IRQ handler type
// =============================================================================
typedef VOID (*HAL_TCU_IRQ_HANDLER_T)(VOID);



// =============================================================================
// HAL_TCU_IRQ_ID_T
// -----------------------------------------------------------------------------
/// Use  to discriminate the tcu irqs
// =============================================================================
typedef enum{
    HAL_TCU0_IRQ = 2,
    HAL_TCU1_IRQ = 3
} HAL_TCU_IRQ_ID_T;





// =============================================================================
//  FUNCTIONS
// =============================================================================




// =============================================================================
// hal_TcuIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user function to call when a tcu interrupt occurs.
/// @tcu Choose the TCU interrupt that will call this user handler.
/// @handler Handler called by this TCU interrupt.
// =============================================================================
PROTECTED VOID hal_TcuIrqSetHandler(HAL_TCU_IRQ_ID_T tcu, HAL_TCU_IRQ_HANDLER_T handler);


// =============================================================================
// hal_TcuIrqSetMask
// -----------------------------------------------------------------------------
/// Set the interruption mask for the specified TCU IRQ.
/// @param tcu Id of the TCU  IRQ
/// @mask If \c TRUE, the IRQ can be triggered. Oterwise, it can't occur.
// =============================================================================
PROTECTED VOID hal_TcuIrqSetMask(HAL_TCU_IRQ_ID_T tcu, BOOL mask);



// =============================================================================
// hal_TcuIrqGetMask
// -----------------------------------------------------------------------------
//  Gets the interrupt mask of a TCU  IRQ
/// @param tcu A TCU IRQ Id.
/// @return The mask
// =============================================================================
BOOL hal_TcuIrqGetMask(HAL_TCU_IRQ_ID_T tcu);


// =============================================================================
// hal_TcuIrqHandler
// -----------------------------------------------------------------------------
/// TCU IRQ handler called by the IRQ module
/// @param interruptId of the IRQ calling this handler
// =============================================================================
PROTECTED VOID hal_TcuIrqHandler(UINT8 interruptId);


#endif



