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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_irq.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file   halp_irq.h                                               
/// Prototypes for private IRQ functions and defines                 
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef  _HALP_IRQ_H_
#define  _HALP_IRQ_H_

#include "cs_types.h"




/// 
///     Function pointer type used to set module irq handlers
/// 
typedef VOID (*HAL_MODULE_IRQ_HANDLER_T)(UINT8);



// Global Var

/// 
///     The registry array 
/// 


extern HAL_MODULE_IRQ_HANDLER_T hal_HwModuleIrqHandler[];

/// 
///     Irq id array
/// 
extern UINT8 hal_HwModuleIrqId[];



// API Functions
//  ---------- Init Irq ----------------- 
/// 
///     Irq init (used only for test without sx)
/// 
VOID hal_IrqInit(BOOL irq_in_rom, UINT8 mask);




#endif //  HAL_IRQ_H 


