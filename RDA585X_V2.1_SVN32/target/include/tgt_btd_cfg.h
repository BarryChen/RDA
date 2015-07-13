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
//
/// @file tgt_btd_cfg.h.h
/// That file provides the TGT API related to BTD configuration.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_BTD_CFG_H_
#define _TGT_BTD_CFG_H_

#include "cs_types.h"


typedef struct BTD_CONFIG_STRUCT_T TGT_BTD_CONFIG_T;

// =============================================================================
// tgt_GetBtdConfig
// -----------------------------------------------------------------------------
/// This function is used by BTD to get access to its configuration structure.
/// This is the only way BTD can get this information.
// =============================================================================
PUBLIC CONST TGT_BTD_CONFIG_T* tgt_GetBtdConfig(VOID);


#endif // _TGT_BTD_CFG_H_
