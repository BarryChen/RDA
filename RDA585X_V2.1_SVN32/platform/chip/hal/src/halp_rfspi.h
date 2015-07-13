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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_rfspi.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_rfspi.h                                                        //
/// That file provides the private interface for the RFSPI driver.            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "hal_rfspi.h"
#include "hal_sys.h"


#ifndef _HALP_RFSPI_H_
#define _HALP_RFSPI_H_


// =============================================================================
// hal_RfspiIrqHandler
// -----------------------------------------------------------------------------
/// RF-SPI module IRQ handler.
/// This function is used by the private IRQ module to clear the IRQ and call 
/// the user handler with the proper status.
// =============================================================================
PROTECTED VOID hal_RfspiIrqHandler(UINT8 interruptId);

#endif // _HALP_RFSPI_H_




