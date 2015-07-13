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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_uart.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_uart.h 
///     Definition of the private functions of HAL's UART Driver. 
///     Caution: This file is strictly confidential!!!
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef  _HALP_UART_H_
#define  _HALP_UART_H_

#include "cs_types.h"
#include "hal_uart.h"

// ============================================================================
// hal_UartIrqHandler
// ----------------------------------------------------------------------------
/// UART module IRQ handler
/// Clear the IRQ and call the IRQ handler user function
// ============================================================================
PROTECTED VOID hal_UartIrqHandler(UINT8);


#endif //  HAL_UART_IRQ_H 

