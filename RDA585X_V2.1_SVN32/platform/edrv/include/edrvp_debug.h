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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/include/edrvp_debug.h $ //
//	$Author: huazeng $                                                        // 
//	$Date: 2010-08-23 13:47:34 +0800 (星期一, 23 八月 2010) $                     //   
//	$Revision: 1532 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file audp_debug.h                                                          //
/// Debug features to be used only inside the module (protected).
/// Is this file is used for all the debug needs of the module, it will be
/// easier to disable those debug features for the whole module (by changing
/// the value of a define while compiling).
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hal_debug.h"
#include "sxs_io.h"

#ifndef _EDRVP_DEBUG_H_
#define _EDRVP_DEBUG_H_

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define EDRV_WARN_TRC       (_EDRV | TLEVEL(1))
/// PMD specific trace
#define EDRV_PMD_TRC        (_EDRV | TLEVEL(2))
/// MEMD specific trace
#define EDRV_MEMD_TRC       (_EDRV | TLEVEL(3))
/// RFD specific trace
#define EDRV_RFD_TRC        (_EDRV | TLEVEL(4))
/// AUD specific trace
#define EDRV_AUD_TRC        (_EDRV | TLEVEL(5))
/// LCDD specific trace
#define EDRV_LCDD_TRC       (_EDRV | TLEVEL(6))
/// MCD specific trace
#define EDRV_MCD_TRC        (_EDRV | TLEVEL(7))
/// CAMD specific trace
#define EDRV_CAMD_TRC       (_EDRV | TLEVEL(8))
/// FMD specific trace
#define EDRV_FMD_TRC        (_EDRV | TLEVEL(9))
/// BTD specific trace
#define EDRV_BTD_TRC        (_EDRV | TLEVEL(10))
/// TSD specific trace
#define EDRV_TSD_TRC        (_EDRV | TLEVEL(11))
/// GSS specific trace  
#define EDRV_GSS_TRC        (_EDRV | TLEVEL(12))
/// GPIOI2C specific trace  
#define EDRV_GI2C_TRC       (_EDRV | TLEVEL(13))
/// UART specific trace
#define EDRV_UART_TRC       (_EDRV | TLEVEL(14))
// add more here
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define EDRV_DBG_TRC        (_EDRV | TLEVEL(16))

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string 
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
// Conditional Printf Usage
#ifndef EDRV_NO_TRACE
#define EDRV_TRACE(level, tstmap, format, ...)  hal_DbgTrace(_EDRV | level,tstmap,format, ##__VA_ARGS__)
#else
#define EDRV_TRACE(level, tstmap, format, ...)  
#endif //EDRV_NO_PRINTF



#endif // _EDRVP_DEBUG_H_

