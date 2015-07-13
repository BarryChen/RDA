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
/// @file tgt_uctl_cfg.h                                                      //
/// That file provides the TGT API related to Usb Controller configuration.   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_UCTLS_CFG_H_
#define _TGT_UCTLS_CFG_H_

#include "cs_types.h"
#include "uctls_m.h"
#include "umss_m.h"
//#include "utraces_m.h"
#include "umss_callback.h"
#include "umss_config.h"
//#include "uvideos_callback.h"
//#include "uvideos_config.h"

// =============================================================================
// UCTLS_CONFIG_T
// -----------------------------------------------------------------------------
/// Rename type for uctls config
// =============================================================================
typedef UCTLS_SERVICE_LIST_T UCTLS_CONFIG_T;

// =============================================================================
// tgt_GetUctlsConfig
// -----------------------------------------------------------------------------
/// This function is used by UCTL to get access to its configuration structure.
/// This is the only way UCTLS can get this information.
// =============================================================================
PUBLIC CONST UCTLS_CONFIG_T* tgt_GetUctlsConfig(VOID);

#endif // _TGT_UCTLS_CFG_H_
