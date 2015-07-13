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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/8808/src/bootp_host.h $
//  $Author: admin $
//  $Date: 2010-07-07 20:28:03 +0800 (周三, 07 七月 2010) $
//  $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file bootp_host.h
///
/// Boot host protected function
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BOOTP_HOST_H_
#define _BOOTP_HOST_H_

#include "cs_types.h"
#include "boot.h"

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

PROTECTED VOID mon_Event(UINT32 evt);

PROTECTED VOID boot_HstMonitorX(VOID);

PROTECTED BOOT_MONITOR_OP_STATUS_T boot_HstMonitor(VOID);

#endif // _BOOTP_HOST_H_
