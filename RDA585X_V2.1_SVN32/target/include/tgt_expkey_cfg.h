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
/// @file tgt_expkey_cfg.h                                                    //
/// That file provides the TGT API related to expand key configuration.       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_EXPKEY_CFG_H_
#define _TGT_EXPKEY_CFG_H_

#include "cs_types.h"
#include "hal_gpio.h"


// =============================================================================
// TGT_CFG_EXPKEY_T
// -----------------------------------------------------------------------------
/// This structure describes the expand key configuration for a given target.
/// The first field identify if expand key is used.
/// The second and third field is the mapped pin when expand key flag is set.
// =============================================================================
typedef struct
{
    /// \c TRUE if expand key is used
    BOOL            expKeyUsed;
    /// The mapped GPO when expKeyUsed is true
    HAL_APO_ID_T    expKeyOut6;
    /// The mapped GPO when expKeyUsed is true
    HAL_APO_ID_T    expKeyOut7;
} TGT_CFG_EXPKEY_T;


// =============================================================================
// tgt_GetExpKeyConfig
// -----------------------------------------------------------------------------
/// This function is used by HAL to get access to expand key configuration.
// =============================================================================
PUBLIC CONST TGT_CFG_EXPKEY_T* tgt_GetExpKeyConfig(VOID);


#endif // _TGT_CAMS_CFG_H_

