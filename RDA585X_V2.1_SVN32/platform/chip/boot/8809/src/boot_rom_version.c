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


// =============================================================================
// Muted fram data arrays
// -----------------------------------------------------------------------------
/// Used by HAL SPEECH to construct mute frames.
/// These variables are copied from spc_tables.c, and the latter is linked to BCPU ROM.
// =============================================================================
CONST UINT32 MutedFrame_AMR[2]={0x16c1e364, 0x00000000};
CONST UINT32 MutedFrame_HR[4]={0x97712c80, 0xffffffff, 0xffffffff, 0x2d41ffff};
CONST UINT32 MutedFrame_EFR[8]={
0x9c4bfcdb, 0xffff600d, 0xc000001f, 0x007ffffd, 0xfff00000, 0x00000fff, 0x3ff3fff0, 0x2fa00000};
CONST UINT32 MutedFrame_FR[9]={
0xd6174820, 0x0000284, 0x80000000, 0x00000002, 0x2800000, 0x00000000, 0x0028000, 0x00000000, 0x1d3f0000};

