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
/// @file boot_sector_chip.c
///
/// Contains the implementation of the chip dependent function of
/// the Boot Sector.
/// 
/// Important: all the source of this file is mapped in the Boot Sector (i.e.
/// first section of the Flash).
//
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"

#include "bootp_sector.h"
#include "boot.h"
#include "global_macros.h"
#include "bootp_host.h"
#include "bootp_mode.h"


// =============================================================================
// MACROS                                                                       
// =============================================================================

// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================


/// Entry of the Boot Monitor in ROM.
EXPORT PROTECTED VOID boot_Monitor(VOID);


// =============================================================================
// boot_SectorEnterBootMonitor
// -----------------------------------------------------------------------------
/// From the Boot Sector, enter in the Boot Monitor.
/// And, depending on the chip, make the modifications required to have a
/// fully working monitor.
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
PROTECTED VOID boot_SectorEnterBootMonitor(VOID)
{
    /// Force entering the boot monitor.
    /// This function is at a fixed location, in ROM.
    g_bootBootMode |= BOOT_MODE_FORCE_MONITOR;
    boot_Monitor();
}

// =============================================================================
// boot_SectorFixRom
// -----------------------------------------------------------------------------
/// Use to fix some startup missing feature:
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
VOID boot_SectorFixRom(VOID)
{
}


