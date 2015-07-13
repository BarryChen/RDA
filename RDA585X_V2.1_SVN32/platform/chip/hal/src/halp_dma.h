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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_dma.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_dma.h                                                          //
/// That file provides the private interface of the DMA driver.               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef  _HALP_DMA_H_
#define  _HALP_DMA_H_


#include "cs_types.h"

// =============================================================================
//  FUNCTIONS
// =============================================================================



// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
// hal_DmaIrqHandler
// -----------------------------------------------------------------------------
/// DMA module IRQ handler
/// Clear IRQ and call the IRQ handler user function.
// =============================================================================
PROTECTED VOID hal_DmaIrqHandler(UINT8 dmaIrqId);




#endif //  _HAL_DMA_H_ 

