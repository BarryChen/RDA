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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_open.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_open.h                                                         //
/// Contains private init related header of Ruby's hal driver
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __HAL_OPEN_H__
#define __HAL_OPEN_H__

#include "cs_types.h"
#include "hal_config.h"

// =============================================================================
// hal_Open
// -----------------------------------------------------------------------------
/// Initializes the HAL layer. 
/// This function also do the open of the PMD driver and set the idle-mode.
// =============================================================================
PUBLIC VOID hal_Open(CONST HAL_CFG_CONFIG_T*);

#endif // __HAL_OPEN_H__


