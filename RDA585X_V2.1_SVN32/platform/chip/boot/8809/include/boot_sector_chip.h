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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/8809/include/boot_sector_chip.h $
//  $Author: huazeng $
//  $Date: 2011-12-31 20:41:51 +0800 (星期六, 31 十二月 2011) $
//  $Revision: 12997 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BOOT_SECTOR_CHIP_H_
#define _BOOT_SECTOR_CHIP_H_


#include "global_macros.h"
#ifdef CT_ASM
#include "mem_bridge_asm.h"
#else
#include "mem_bridge.h"
#endif


//=============================================================================
// BOOT_SECTOR_ENTRY_POINT
//-----------------------------------------------------------------------------
/// First address where to jump in flash, to execute code
//=============================================================================
#define BOOT_SECTOR_ENTRY_POINT (KSEG0(REG_CS0_BASE)+0x10)


//=============================================================================
// BOOT_FLASH_PROGRAMMED_PATTERN_ADDRESS
//-----------------------------------------------------------------------------
/// Address of the programmed flash pattern.
//=============================================================================
#define BOOT_FLASH_PROGRAMMED_PATTERN_ADDRESS ((UINT32*)KSEG0(REG_CS0_BASE))


#endif // _BOOT_SECTOR_CHIP_H_


