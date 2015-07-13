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
//
//  $HeadURL: http://10.10.100.14/svn/developing/Sources/svc/branches/Total_BJ/uaud/include/uaud_m.h $                                                                   
//  $Author: lilian $
//  $Date$
//  $Revision: 13125 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file uaud_m.h
///
/// USB Mass Storage Service
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _UAUD_M_H_
#define _UAUD_M_H_

#include "cs_types.h"

#include "hal_usb.h"

#include "uaud_config.h"

/// @file uaud_m.h
/// @mainpage USB Mass Storage service
///
/// This service provides Mass Storage service on usb
///
/// This document is composed of:
/// - @ref uaud
///
/// @addtogroup uaud USB Mass Storage Service (uaud)
/// @{
///

// =============================================================================
// MACROS                                                                       
// =============================================================================

// =============================================================================
// TYPES                                                                        
// =============================================================================

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// uaud_Open
// -----------------------------------------------------------------------------
/// Open USB audio play service
PUBLIC VOID uaud_Open(void);

// =============================================================================
// uaud_TransportEventProcess
// -----------------------------------------------------------------------------
/// Process audio event
PUBLIC VOID uaud_TransportEventProcess(UINT32 nParam1);

// =============================================================================
// uaud_Close
// -----------------------------------------------------------------------------
/// Close USB audio play service
PUBLIC VOID uaud_Close(void);

PUBLIC UINT32 uAudGetPlayStatus(void);

///  @} <- End of the uaud group

#endif // _UAUD_M_H_
