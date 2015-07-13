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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_key.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///                                                                           //
///     @file   halp_key.h 
///     
///     Keypad module irq handler
///                                                                           //
///                                                                           //
//////////////////////////////////////////////////////////////////////////////// 

#ifndef  _HALP_KEY_H_
#define  _HALP_KEY_H_

#include "cs_types.h"

// ============================================================================
//      hal_KeyIrqHandler
// ----------------------------------------------------------------------------
///     Internal interrupt handler for the keypad physical interruptions.
///     
///     It handles the edge detection by starting the timer IRQ.
///     It handles the key timer interruptions by debouncing and calling the user
///     callback function for each key event (up, down or pressed).
///     It manages the hw interrupts of the keypad (clears the IRQ in the module).
///     
///     It handles also the interrupt for the GPIO used for the on-off button.
///     About the interrupt for the on-off button: Physically, this button is
///     mapped on a GPIO. The GPIO HAL handler will call this function when an
///     interrupt is detected on the corresponding GPIO. When this rising edge is
///     detected, the behavior is the same as the one that occurs when a normal
///     key is pressed: it disable the edge IRQ and starts the key timer IRQ.
///
///     @param interruptId The Id of the interrruption
// ============================================================================
PROTECTED VOID hal_KeyIrqHandler(UINT8 interruptId);



#endif //  _HALP_KEY_H_ 
