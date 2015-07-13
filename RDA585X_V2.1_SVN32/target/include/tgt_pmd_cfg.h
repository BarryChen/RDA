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
/// @file tgt_pmd_cfg.h                                                       //
/// That file provides the TGT API related to PMD configuration.              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_PMD_CFG_H
#define _TGT_PMD_CFG_H

#include "cs_types.h"


// =============================================================================
// PMD_CONFIG_T
// -----------------------------------------------------------------------------
/// This is the anonymous declaration of the structure used to describes the 
/// configuration of the PMD driver. The actual content is specifc to the pmd 
/// driver of the actual PM Chip
// =============================================================================
typedef struct PMD_CONFIG_STRUCT_T TGT_PMD_CONFIG_T;


// =============================================================================
// tgt_GetPmdConfig
// -----------------------------------------------------------------------------
/// This function is used by Aud to get access to its configuration structure.
/// This is the only way Aud can get this information.
// =============================================================================
PUBLIC CONST TGT_PMD_CONFIG_T* tgt_GetPmdConfig(VOID);


#endif // _TGT_PMD_CFG_H

