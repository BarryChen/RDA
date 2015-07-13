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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/src/bootp_sector.h $
//  $Author: admin $
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $
//  $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file bootp_sector.h
///
/// Boot sector variable
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BOOTP_SECTOR_H_
#define _BOOTP_SECTOR_H_

#include "cs_types.h"


// =============================================================================
// MACROS                                                                       
// =============================================================================


/// Value of the event sent by the Boot Sector when it configures the EBC
/// CS RAM (useful for RAM in burst mode, after a soft reset).
#define BOOT_SECTOR_EVENT_CONFIG_RAM    0x00000055


/// /// Value of the event sent by the Boot Sector when it detects that it
/// must enter in the boot monitor.
#define BOOT_SECTOR_EVENT_ENTER_MONITOR 0x00000056


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================


// =============================================================================
// boot_SectorEnterBootMonitor
// -----------------------------------------------------------------------------
/// From the Boot Sector, enter in the Boot Monitor.
/// And, depending on the chip, make the modifications required to have a
/// fully working monitor.
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
PROTECTED VOID boot_SectorEnterBootMonitor(VOID);

// =============================================================================
// boot_SectorFixRom
// -----------------------------------------------------------------------------
/// Use to fix some startup missing feature (like usb clock on greenstone)
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
PROTECTED VOID boot_SectorFixRom(VOID);


#endif // _BOOTP_SECTOR_H_
