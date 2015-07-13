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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_boot_sector_reload.c $
//  $Author: admin $
//  $Date: 2012-07-13 13:54:10 +0800 (星期五, 13 七月 2012) $
//  $Revision: 16308 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file hal_boot_sector_driver.c
///
/// HAL Boot Sector driver.
/// 
/// Provides functions to allow HAL EBC and HAL Host Monitor to write some
/// informations in the Boot Sector structure and to validate this structure.
/// 
/// The name is very long to avoid to be cached by the *boot_sector.o pattern
/// of the linker script.
//
////////////////////////////////////////////////////////////////////////////////

#include "chip_id.h"
#include "global_macros.h"

#include "cs_types.h"

#include "hal_mem_map.h"

#include "boot_sector.h"
#include "halp_debug.h"



// =============================================================================
// MACROS                                                                       
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================

/// Boot Sector reload structure.
/// This structure contains information written by HAL during the normal
/// execution of the code and used by the Boot Sector to reload the ram image
/// when the phone boots. 
PRIVATE BOOT_SECTOR_RELOAD_STRUCT_T 
    HAL_BOOT_SECTOR_RELOAD_STRUCT_SECTION g_halBootSectorReloadStruct;



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
PUBLIC BOOL hal_BootSectorFillReloadCtx(BOOT_SECTOR_RELOAD_CTX_T *ctx, BOOT_SECTOR_RELOAD_ID_T id)
{
    if(id >= BOOT_SECTOR_RELOAD_MAX_NUM)
    {
        HAL_ASSERT(FALSE,"boot sector reload: unkown id %d",id);
        return FALSE;
    }

    g_halBootSectorReloadStruct.ctx[id].storeAddress = ctx->storeAddress;
    g_halBootSectorReloadStruct.ctx[id].mapAddress = ctx->mapAddress;
    g_halBootSectorReloadStruct.ctx[id].size = ctx->size;

    return TRUE;
}

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
PUBLIC VOID hal_BootSectorReloadEnable(VOID)
{
    g_halBootSectorReloadStruct.validTag = BOOT_SECTOR_RELOAD_VALID_TAG;
    g_halBootSectorReloadStruct.checkSum = ~BOOT_SECTOR_RELOAD_VALID_TAG;
    *boot_BootSectorGetReloadStructPointer() = &g_halBootSectorReloadStruct;
}


