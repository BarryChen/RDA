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
/// @file tgt_memd_cfg.h                                                      //
/// That file provides the TGT API related to MEMD configuration.             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_MEMD_CFG_H_
#define _TGT_MEMD_CFG_H_

#include "cs_types.h"
#include "memd_config.h"



// =============================================================================
// tgt_GetMemdFlashConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Flash configuration structure.
// =============================================================================
PUBLIC CONST MEMD_FLASH_CONFIG_T* tgt_GetMemdFlashConfig(VOID);



// =============================================================================
// tgt_GetMemdRamConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Ram configuration structure.
// =============================================================================
PUBLIC CONST MEMD_RAM_CONFIG_T* tgt_GetMemdRamConfig(VOID);

#endif // _TGT_MEMD_CFG_H_

