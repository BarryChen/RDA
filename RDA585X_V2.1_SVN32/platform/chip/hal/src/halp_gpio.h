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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_gpio.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_gpio.h                                                         //
/// This file contains Granite's GPIO driver implementation                   //
//                                                                            //
//////////////////////////////////////////////////////////////////////////////// 


#ifndef  _HALP_GPIO_H_
#define  _HALP_GPIO_H_

#include "hal_gpio.h"

// =============================================================================
// MACROS
// =============================================================================

// =============================================================================
// HAL_GPIO_BIT
// -----------------------------------------------------------------------------
/// This macro is used by internal code to convert gpio number to bit.
/// It masks the upper bit so it can be used directly with #HAL_GPIO_GPIO_ID_T.
// =============================================================================
#define HAL_GPIO_BIT(n)    (1<<((n)&0x3f))

// =============================================================================
// HAL_GPO_BIT
// -----------------------------------------------------------------------------
/// This macro is used by internal code to convert gpio number to bit.
/// It masks the upper bit so it can be used directly with #HAL_GPIO_GPO_ID_T.
// =============================================================================
#define HAL_GPO_BIT(n)    (1<<((n)&0x3f))




// =============================================================================
// hal_GpioIrqHandler
// -----------------------------------------------------------------------------
/// GPIO module IRQ handler
/// 
///     Clear the IRQ and call the IRQ handler
///     user function
///     @param interruptId The interruption ID
// =============================================================================  
PROTECTED VOID hal_GpioIrqHandler(UINT8 interruptId);


#endif //HAL_GPIO_H
