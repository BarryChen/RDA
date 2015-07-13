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
/// @file                                                                     //
/// That file contains the global variable ROM version and is mapped at
/// a fixed location in ROM.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"


// =============================================================================
//  MACROS
// =============================================================================


/// Linker section where the Boot Sector main function must be mapped:
/// Beginning of the first sector of the flash.
#define BOOT_ROM_VERSION_SECTION  __attribute__((section (".boot_rom_version_number")))


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// g_bootRomVersionNumber
// -----------------------------------------------------------------------------
/// Contains the version of the ROM.
/// This constant is mapped at a fixed location at the end of the ROM.
// =============================================================================
CONST UINT32 g_bootRomVersionNumber BOOT_ROM_VERSION_SECTION = ROM_VERSION_NUMBER;


