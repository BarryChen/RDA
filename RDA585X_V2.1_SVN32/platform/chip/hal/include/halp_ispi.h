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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/halp_ispi.h $
//  $Author: admin $                                                      
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                   //   
//  $Revision: 269 $                                                      //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_ispi.c                                                         //
/// Implementation of the Internal ISPI romed Driver.                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _HALP_ISPI_H_
#define _HALP_ISPI_H_

#include "cs_types.h"

#include "chip_id.h"

#if (CHIP_ISPI_CS_QTY != 0) || (CHIP_SPI3_CS_QTY != 0)
#include "hal_ispi.h"
#include "hal_sys.h"

// =============================================================================
// hal_IspiUpdateDivider
// -----------------------------------------------------------------------------
/// Update the ISPI divider so that the maximum frequency defined is not
/// overpassed.
/// 
/// @param sysFreq Newly applied system frequency
// =============================================================================
PROTECTED VOID hal_IspiUpdateDivider(HAL_SYS_FREQ_T sysFreq);


#endif // CHIP_SPI3_CS_QTY

#endif // _HALP_ISPI_H_


