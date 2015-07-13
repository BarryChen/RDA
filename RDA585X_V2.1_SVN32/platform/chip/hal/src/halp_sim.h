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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_sim.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_sim.h                                                          //
/// Prototypes for private SIM functions                     
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef  _HALP_SIM_H_
#define  _HALP_SIM_H_

#include "cs_types.h"
#include "hal_sys.h"



// ============================================================================
// hal_SimIrqHandler
// ----------------------------------------------------------------------------
/// SIM module irq handler for the IRQ module.
///
/// Clear the IRQ and call the IRQ handler user function.
/// @param interruptId Identifier of the interrupt.
// ============================================================================
PROTECTED VOID hal_SimIrqHandler(UINT8 interruptId);



// ============================================================================
// hal_SimCheckClockStatus
// ----------------------------------------------------------------------------
/// This function checks the clock status of the SIM modules. When it is not
/// needed, the request for a clock is given up (ie request for no clock
/// is done.)
// ============================================================================
PROTECTED VOID hal_SimCheckClockStatus(VOID);



#endif //  _HALP_SIM_H_ 


