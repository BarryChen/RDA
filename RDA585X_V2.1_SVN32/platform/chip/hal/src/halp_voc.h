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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_voc.h $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-09-06 12:06:26 +0800 (星期一, 06 九月 2010) $                     //   
//    $Revision: 1958 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_voc_irq.h
///     Definition of the private IRQ functions of Garnet's VoC Driver
///     Caution: This file is strictly confidential!!!
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#ifndef  _HALP_VOC_IRQ_H_
#define  _HALP_VOC_IRQ_H_


#include "cs_types.h"

// ============================================================================
// TYPES
// ============================================================================



// ============================================================================
// HAL_VOC_STATE_T
// ----------------------------------------------------------------------------
/// Describes the VoC state.
// ============================================================================
typedef enum
{
    HAL_VOC_CLOSE_DONE,
    HAL_VOC_CLOSE_STARTED,
    HAL_VOC_OPEN_STARTED,
    HAL_VOC_OPEN_IN_PROGRESS,
    HAL_VOC_OPEN_FAILED,
    HAL_VOC_OPEN_DONE,
} HAL_VOC_STATE_T;


// ============================================================================
// FUNCTIONS
// ============================================================================

// ============================================================================
// hal_VocIrqHandler
// ----------------------------------------------------------------------------
/// VoC IRQ handler, clearing the IRQ cause regiter and calling the user handler
/// defined by hal_VocIrqSetHandler. It also checks if the open procedure is finished
/// when the user chooses to confirm the end of the open procedure with an VoC DMA 
/// interrupt
/// @param interruptId Id with which the handler is entered
// ============================================================================
PROTECTED VOID hal_VocIrqHandler(UINT8 interruptId);


#endif //  HAL_VOC_IRQ_H 

