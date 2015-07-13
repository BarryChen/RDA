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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_config.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_config.h                                                       //
/// That file is the private header for the configuration of HAL in its aspect
/// that are the more likely to be implementation dependent...                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _HALP_CONFIG_H_
#define _HALP_CONFIG_H_

#include "cs_types.h"
#include "hal_config.h"


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

EXPORT CONST HAL_CFG_CONFIG_T* g_halCfg;


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_BoardSetup
// -----------------------------------------------------------------------------
/// Apply board dependent configuration to HAL
/// @param halCfg Pointer to HAL configuration structure (from the target
/// module).
// =============================================================================
PROTECTED VOID hal_BoardSetup(CONST HAL_CFG_CONFIG_T* halCfg);


#endif // _HALP_CONFIG_H_

