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
/// @file tgt_fmd_cfg.h                                                       //
/// That file provides the TGT API related to FMD configuration.              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_FMD_CFG_H_
#define _TGT_FMD_CFG_H_

#include "cs_types.h"

typedef struct FMD_CONFIG_STRUCT_T TGT_FMD_CONFIG_T;

EXTERN_C_START

// =============================================================================
// tgt_GetFmdConfig
// -----------------------------------------------------------------------------
/// This function is used by FMD to get access to its configuration structure.
/// This is the only way FMD can get this information.
// =============================================================================
PUBLIC CONST TGT_FMD_CONFIG_T* tgt_GetFmdConfig(VOID);

EXTERN_C_END

#endif // _TGT_FMD_CFG_H_

