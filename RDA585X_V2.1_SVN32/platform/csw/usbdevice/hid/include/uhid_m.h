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

#ifndef _UHID_M_H_
#define _UHID_M_H_

#include "cs_types.h"

#include "hal_usb.h"

/// @file uhid_m.h
/// @mainpage USB HID service
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
// uHid_Open
// -----------------------------------------------------------------------------
/// Open USB HID service
PUBLIC VOID uhid_Open(UINT8 *desc, UINT16 length);

// =============================================================================
// uHid_TransportEventProcess
// -----------------------------------------------------------------------------
/// Process hid event
PUBLIC VOID uhid_TransportEventProcess(UINT32 nParam1);

// =============================================================================
// uHid_Close
// -----------------------------------------------------------------------------
/// Close USB HID service
PUBLIC VOID uhid_Close(void);

// =============================================================================
// uHid_SendKeycode
// -----------------------------------------------------------------------------
/// Send HID key code
PUBLIC VOID uhid_SendKeycode(UINT8 *keycode, UINT8 length);


///  @} <- End of the uaud group

#endif // _UHID_M_H_
