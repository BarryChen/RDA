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
/// @file tgt_mcd_cfg.h                                                       //
/// That file provides the TGT API related to MCD configuration.              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_MCD_CFG_H_
#define _TGT_MCD_CFG_H_

#include "cs_types.h"

#ifdef DUAL_TFLASH_SUPPORT
#include "mcd_config.h"

EXTERN_C_START

// =============================================================================
// tgt_GetMcdConfig
// -----------------------------------------------------------------------------
/// This function is used by MCD to get access to its configuration structure.
/// This is the only way MCD can get this information.
// =============================================================================
PUBLIC CONST MCD_CONFIG_STRUCT_T* tgt_GetMcdConfig(VOID);
EXTERN_C_END
#else
typedef struct MCD_CONFIG_STRUCT_T TGT_MCD_CONFIG_T;


// =============================================================================
// tgt_GetMcdConfig
// -----------------------------------------------------------------------------
/// This function is used by MCD to get access to its configuration structure.
/// This is the only way MCD can get this information.
// =============================================================================
PUBLIC CONST TGT_MCD_CONFIG_T* tgt_GetMcdConfig(VOID);
#endif

#endif // _TGT_MCD_CFG_H_

