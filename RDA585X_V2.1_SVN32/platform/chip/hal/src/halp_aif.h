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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_aif.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_aif.h                                                          //
/// That file presents the HAL AIF driver private API.                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _HALP_AIF_H_
#define _HALP_AIF_H_

#include "cs_types.h"
#include "hal_aif.h"

// =============================================================================
//  MACROS
// =============================================================================
/// Indices for the user handler array.
#define RECORD  0
#define PLAY    1

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_AIF_XFER_HANDLER_T
// -----------------------------------------------------------------------------
/// Type privately used to store both handler for each direction
// =============================================================================
typedef struct
{
    HAL_AIF_HANDLER_T halfHandler;
    HAL_AIF_HANDLER_T endHandler;
} HAL_AIF_XFER_HANDLER_T;






// =============================================================================
// hal_AifIrqHandler
// -----------------------------------------------------------------------------
/// Handler called by the IRQ module when a BB-IFC interrupt occurs.
/// 
// =============================================================================
PROTECTED VOID hal_AifIrqHandler(UINT8 interruptId);


#endif //_HALP_AIF_H_

