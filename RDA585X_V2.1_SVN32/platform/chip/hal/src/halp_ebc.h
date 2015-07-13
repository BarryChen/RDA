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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_ebc.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2012-07-13 13:54:10 +0800 (星期五, 13 七月 2012) $                     //   
//    $Revision: 16308 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_ebc.h                                                          //
/// Prototypes for private EBC functions                                      //
//                                                                            //
//////////////////////////////////////////////////////////////////////////////// 


#ifndef  _HALP_EBC_H_
#define  _HALP_EBC_H_

#include "cs_types.h"
#include "global_macros.h"
#include "mem_bridge.h"
#include "hal_sys.h"
#include "hal_ebc.h"


#ifdef CHIP_HAS_EBC_CS2_BUG
/// Use to remember the CS0 timing configuration, as the register
/// cannot be read again.
extern PROTECTED UINT32 g_halCs0TimeReg ;
/// Use to remember the CS0 mode configuration, as the register
/// cannot be read again.
extern PROTECTED UINT32 g_halCs0ModeReg ;
#endif

/// 
///     Array of pointers to the different
///     CS configuration.
///     
///     \c NULL in the i-th place means
///     the CS i is not enabled/used @todo Write that properly.
/// 
extern HAL_EBC_CS_CFG_T* g_halEbcCsConfigArray [EBC_NB_CS];

#endif // HAL_EBC_H
