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
//  $HeadURL: http://10.10.100.14/svn/developing/Sources/svc/branches/Total_BJ/uctls/include/uctls_callback.h $
//  $Author: lilian $
//  $Date$
//  $Revision: 13080 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file uctls_callback.h
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _UCTLS_CALLBACK_H_
#define _UCTLS_CALLBACK_H_

#include "cs_types.h"

#include "umss_m.h"
//#include "uvideos_m.h"

/// @file uctls_callback.h
///
/// @addtogroup uctls
/// @{
///

// =============================================================================
// MACROS                                                                       
// =============================================================================


// =============================================================================
// TYPES                                                                        
// =============================================================================

// =============================================================================
// UCTLS_SERVICE_CFG_T
// -----------------------------------------------------------------------------
/// Service configuration data
// =============================================================================
typedef union {
    CONST UMSS_CFG_T mss;
//	CONST UVIDEOS_CFG_T videos;
} UCTLS_SERVICE_CFG_T;

// =============================================================================
// UCTLS_SERVICE_CALLBACK_OPEN_T
// -----------------------------------------------------------------------------
/// Open service callback
/// @param config service configure structure
/// @return USB list interface descriptor
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T**
(*UCTLS_SERVICE_CALLBACK_OPEN_T)(CONST UCTLS_SERVICE_CFG_T* config);

// =============================================================================
// UCTLS_SERVICE_CALLBACK_CLOSE_T
// -----------------------------------------------------------------------------
/// Close service callback
// =============================================================================
typedef VOID
(*UCTLS_SERVICE_CALLBACK_CLOSE_T)(VOID);

typedef VOID
(*UCTLS_SERVICE_CALLBACK_PROCESS_T)(UINT32);

// =============================================================================
// UCTLS_SERVICE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback service structure
// =============================================================================
typedef struct {
    // Open
    CONST UCTLS_SERVICE_CALLBACK_OPEN_T  open;
    // Close
    CONST UCTLS_SERVICE_CALLBACK_CLOSE_T close;
    // Process event
    CONST UCTLS_SERVICE_CALLBACK_PROCESS_T process;
} UCTLS_SERVICE_CALLBACK_T;

// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================

/// @} <- End of the uctls group

#endif // _UCTLS_CALLBACK_H_

