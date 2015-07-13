////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2008, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
//  $HeadURL: http://10.10.100.14/svn/developing/Sources/svc/branches/Total_BJ/umss/include/umssp_debug.h $
//  $Author: shug $
//  $Date$
//  $Revision: 26488 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file umssp_debug.h
///
/// umss debug macro
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _UMSSP_DEBUG_H_
#define _UMSSP_DEBUG_H_

#include "cs_types.h"
//#include "svcp_debug.h"

// =============================================================================
// MACROS                                                                       
// =============================================================================

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
// =============================================================================
#define UMSS_WARN_TRC  5 // SVC_WARN_TRC

/// Informational trace interesting for AVRS only
#define UMSS_INFO_TRC   5 //SVC_UMSS_TRC

/// reserved for debug (can be very verbose, but should probably not stay in code)
#define UMSS_DBG_TRC    5 //SVC_DBG_TRC

#ifndef UMSS_NO_PRINTF
  #define UMSS_TRACE(level, tsmap, format, ...)   CSW_TRACE(level,format, ##__VA_ARGS__)
#else
  #define UMSS_TRACE(level, tsmap, fmt, ...)
#endif // UMSS_NO_PRINTF

// =============================================================================
// TYPES                                                                        
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================


#endif // _UMSSP_DEBUG_H_
