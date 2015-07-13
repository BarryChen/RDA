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
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file tgt_hal_cfg.h                                                       //
/// That file provides the TGT API related to HAL configuration.              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _TGT_HAL_CFG_H
#define _TGT_HAL_CFG_H

#include "cs_types.h"
#include "hal_config.h"


// =============================================================================
// tgt_GetHalConfig
// -----------------------------------------------------------------------------
/// This function is used by HAL to get access to its configuration structure.
/// This is the only way HAL can get this information.
// =============================================================================
PUBLIC CONST HAL_CFG_CONFIG_T* tgt_GetHalConfig(VOID);


#endif // _TGT_HAL_CFG_H

