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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_boot_sector_reload.h $
//  $Author: huazeng $
//  $Date: 2010-07-21 20:06:00 +0800 (星期三, 21 七月 2010) $
//  $Revision: 640 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file hal_boot_sector_reload.h
///
/// HAL Boot Sector loader.
/// 
/// Provides functions to allow HAL ro reload the ram image when in boot sector
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_BOOT_SECTOR_RELOAD_H_
#define _HAL_BOOT_SECTOR_RELOAD_H_

#include "cs_types.h"

#include "chip_id.h"

#include "boot_sector.h"


// =============================================================================
// FUNCTIONS                                                            
// =============================================================================


// =============================================================================
// hal_BootSectorFillReloadCtx
// -----------------------------------------------------------------------------
/// Fill the boot sector reload context.
/// ctx: pointer to the boot sector reload context.
/// id: boot sector reload id
// =============================================================================
PUBLIC BOOL hal_BootSectorFillReloadCtx(BOOT_SECTOR_RELOAD_CTX_T *ctx, BOOT_SECTOR_RELOAD_ID_T id);


// =============================================================================
// hal_BootSectorReloadEnable
// -----------------------------------------------------------------------------
/// Validate the Boot Sector reload structure. This is a way to tell the Boot Sector
/// that the structure contains correct ram image and that it must be loaded
/// and executed when restart.
/// 
/// This is done by putting the address of the Boot Sector reload structure in
/// the Boot Sector reload structure pointer and enabling a flag in it.
// =============================================================================
PUBLIC VOID hal_BootSectorReloadEnable(VOID);


#endif // _HAL_BOOT_SECTOR_RELOAD_H_
