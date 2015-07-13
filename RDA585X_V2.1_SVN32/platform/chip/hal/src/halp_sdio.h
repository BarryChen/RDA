////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2012, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_sdio.h $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2012-03-14 17:44:31 +0800 (周三, 14 三月 2012) $                     //   
//    $Revision: 14120 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_sdio.h
/// SDIO private API. It holds the IRQ handling functions, types and mechanisms
/// to enable the SDIO interrupt on the System Side. If they appear to be useful,
/// put them in the public interface.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef _HALP_SDIO_H_
#define _HALP_SDIO_H_



// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// ============================================================================
// hal_SdioIrqHandler
// ----------------------------------------------------------------------------
/// SDIO IRQ handler, clearing the IRQ cause regiter and calling the user handler
/// defined by hal_SdioIrqSetHandler.
/// @param interruptId Id with which the handler is entered
// ============================================================================
PROTECTED VOID hal_SdioIrqHandler(UINT8 interruptId);



#endif
