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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_spi.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file  hal_spi.h                                                          //
/// That file provides the SPI driver private API.                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef  _HALP_SPI_H_
#define  _HALP_SPI_H_

#include "hal_spi.h"




// svn propset svn:keywords "HeadURL Author Date Revision" dummy.h



// =============================================================================
//  MACROS
// =============================================================================

/// Those defines are directly related to the ways the macros representing
/// the transferts are defined : IRQ needing ones are odd. DMA needing ones
/// are 3 or 4 (second bit is then one ...)
#define NEED_IRQ(val) (val & 0x1)
#define IS_DMA(val) (val & 0x2)



// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_SpiIrqHandler
// -----------------------------------------------------------------------------
/// Handler called in case of SPI interrupt
/// 
/// @param interruptId  Will be spi Hw module interrupt 
/// id when used by the IRQ driver.
// =============================================================================
PROTECTED VOID hal_SpiIrqHandler(UINT8 interruptId);

// =============================================================================
// hal_SpiUpdateDivider
// -----------------------------------------------------------------------------
/// Update the SPI divider so that the maximum frequency defined for the currently
/// active CS is not overpassed.
/// @param id Identifier of the SPI for which the function is called.
/// @param sysFreq Newly applied system frequency
// =============================================================================
PROTECTED VOID hal_SpiUpdateDivider(HAL_SPI_ID_T id, HAL_SYS_FREQ_T sysFreq);

#endif //_HALP_SPI_H_


