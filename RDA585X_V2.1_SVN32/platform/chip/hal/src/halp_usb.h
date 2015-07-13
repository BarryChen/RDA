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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_usb.h $
//  $Author: huazeng $
//  $Date: 2012-01-13 11:41:14 +0800 (星期五, 13 一月 2012) $
//  $Revision: 13209 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file halp_usb.h
///
/// This document describes the HAL USB private function
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _HALP_USB_H_
#define _HALP_USB_H_

#include "cs_types.h"

#if CHIP_HAS_USB == 1
#include "boot_usb.h"
#endif

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

#if CHIP_HAS_USB == 1
#ifdef HAL_USB_DRIVER_DONT_USE_ROMED_CODE
PROTECTED VOID hal_UsbIrqHandler(UINT8 interruptId);
#else
#define hal_UsbIrqHandler boot_UsbIrqHandler
#endif // HAL_USB_DRIVER_DONT_USE_ROMED_CODE
PROTECTED VOID hal_UsbHandleIrqInGdb(VOID);
#endif /* CHIP_HAS_USB */

#endif // _HALP_USB_H_

