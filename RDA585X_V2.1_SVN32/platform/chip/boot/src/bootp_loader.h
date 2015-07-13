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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/src/bootp_loader.h $
//  $Author: admin $
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $
//  $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file bootp_loader.h
///
/// Private declaration of boot_loader
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BOOTP_LOADER_H_
#define _BOOTP_LOADER_H_

#include "cs_types.h"

// =============================================================================
// MACROS                                                                       
// =============================================================================


/// Value of the event sent by the Boot Loader when it starts the main() of
/// the code.
#define BOOT_LOADER_EVENT_START_CODE 0x00000057


/// Value of the event sent by the Boot Loader when the version of the ROM of
/// the chip mismatch the version of the ROM against which the code is linked.
#define BOOT_LOADER_EVENT_ROM_ERROR 0x00000054


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================
// =============================================================================
//  boot_LoaderEnter
// -----------------------------------------------------------------------------
/// The parameter "param" has a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID boot_LoaderEnter(UINT32 param);

#endif // _BOOTP_LOADER_H_



