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
//  $HeadURL: svn://atlas/SVN_MIRROR/dev_mirror/modem2G/trunk/platform/edrv/mcd/include/mcd_m.h$ //
//  $Author: caoxh $                                                          //
//  $Date: 2011-04-27 19:33:19 +0800 (星期三, 27 四月 2011) $                                                                    //
//  $Revision: 7282 $                                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/// @file mcd_m.h                                                             //
////////////////////////////////////////////////////////////////////////////////


#ifndef _MCD_USB_H_
#define _MCD_USB_H_

#include "cs_types.h"

/// @file mcd_usb.h

/// This document describes the characteristics of the  USB Disk 
/// Driver and how to use it via its API. 

#ifdef __cplusplus
extern "C"
{
#endif

#define MCDUSB_SUCCESS	0x00000000
#define MCDUSB_FAILED	0x00000001


// =============================================================================
//  Functions
// =============================================================================

// =============================================================================
// mcd_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================
PUBLIC MCD_ERR_T mcd_SetUSBDiskDetectHandler(MCD_CARD_DETECT_HANDLER_T handler);


typedef struct tagUSBClientDriver
{
	int	InterfaceClass;
	int InterfaceSubClass;
	void (*UsbDeviceAttach)(void *configuration, uint8 index);
	void (*UsbDeviceDetach)(void*);
}USBCLIENTDRIVER;

int Mcd_RegisterClientDriver(USBCLIENTDRIVER *pdriverInst);


#ifdef __cplusplus
};
#endif



/// @} <- End of the mmc group

#endif // _MCD_USB_H_

