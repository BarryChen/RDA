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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_boot_sector_driver.c $
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
#include "halp_boot_sector_driver.h"

#include "boot_sector.h"


// =============================================================================
// MACROS                                                                       
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================

/// Boot Sector structure pointer.
/// 
/// This structure contains information written by HAL during the normal
/// execution of the code and used by the Boot Sector when the phone boots.
/// 
/// It is mapped in the Internal SRAM because we don't want the Boot
/// Sector code to access the External RAM. And it is mapped at a fixed
/// location (at the place of the last patch) because we don't want the RAMRUN
/// to overwrite it.
PRIVATE BOOT_SECTOR_STRUCT_T 
    HAL_BOOT_SECTOR_STRUCT_SECTION g_halBootSectorStruct;


PRIVATE INT32 (*g_halBootBeforeMonCallback)(VOID) = NULL;


// =============================================================================
// FUNCTIONS                                                            
// =============================================================================


// =============================================================================
// hal_BootSectorValidateStruct
// -----------------------------------------------------------------------------
/// Validate the Boot Sector structure. This is a way to tell the Boot Sector
/// that the structure contains correct data and that it must be read and
/// that the commands that it contains must be executed.
/// 
/// This is done by putting the address of the Boot Sector structure in
/// the Boot Sector structure pointer and enabling a bit in the Boot Mode
/// register.
// =============================================================================
PRIVATE VOID HAL_BOOT_FUNC hal_BootSectorValidateStruct(VOID)
{
    *boot_BootSectorGetStructPointer() = &g_halBootSectorStruct;
    BOOT_SECTOR_SET_STRUCT_VALIDITY();
}


// =============================================================================
// hal_BootSectorSetCommand
// -----------------------------------------------------------------------------
/// Set the command which must be executed by the Boot Sector.
/// Then validate the Boot Sector structure.
/// 
/// @param valid Command used during the next boot.
// =============================================================================
PROTECTED VOID hal_BootSectorSetCommand(BOOT_SECTOR_CMD_T command)
{
    g_halBootSectorStruct.command = command;
    hal_BootSectorValidateStruct();
}


// =============================================================================
// hal_BootSectorSetEbcConfig
// -----------------------------------------------------------------------------
/// Set the configuration of the EBC for the RAM chip select. This
/// configuration will be written by the Boot Sector code to be sure the
/// RAM configured properly after a reset (in this case, the RAM chip
/// might not get the system reset). Without this, a RAM configured in burst
/// mode could be in an unknown state.
/// 
/// Then validate the EBC configuration and the Boot Sector structure.
/// 
/// @param timings EBC RAM chip select timing configuration as defined by
/// the EBC CS_Time_Write register.
/// @param mode EBC RAM chip select mode configuration as defined by the
/// EBC CS_Mode register.
// =============================================================================
PROTECTED VOID HAL_BOOT_FUNC hal_BootSectorSetEbcConfig(UINT32 timings, UINT32 mode)
{
    g_halBootSectorStruct.ebcConfigRamTimings = timings;
    g_halBootSectorStruct.ebcConfigRamMode    = mode;
    g_halBootSectorStruct.ebcConfigValidTag   = BOOT_SECTOR_EBC_VALID_TAG;
    hal_BootSectorValidateStruct();
}


// =============================================================================
// hal_BootSectorSetEbcFreq
// -----------------------------------------------------------------------------
/// Save EBC frequency for the RAM chip select. This
/// configuration will be written by the Boot Sector code to be sure the
/// RAM configured properly after a reset (in this case, the RAM chip
/// might not get the system reset). Without this, a RAM configured in burst
/// mode could be in an unknown state.
/// 
/// Then validate the EBC extension configuration.
/// 
/// @param ebcFreq the EBC frequency.
// =============================================================================
PROTECTED VOID HAL_BOOT_FUNC hal_BootSectorSetEbcFreq(HAL_SYS_MEM_FREQ_T ebcFreq)
{
    g_halBootSectorStruct.ebcFreq = ebcFreq;
    g_halBootSectorStruct.ebcExtConfigValidTag = BOOT_SECTOR_EBC_EXT_VALID_TAG;
}


// =============================================================================
// hal_BootSectorGetEbcFreq
// -----------------------------------------------------------------------------
/// Get the EBC frequency in the EBC extension configuration.
/// 
/// @return the EBC frequency.
// =============================================================================
PROTECTED HAL_SYS_MEM_FREQ_T hal_BootSectorGetEbcFreq(VOID)
{
    return g_halBootSectorStruct.ebcFreq;
}


// =============================================================================
// hal_BootSectorSetEbcRamId
// -----------------------------------------------------------------------------
/// Save the external RAM ID to select a correct timing.
/// 
/// @param ramId the external RAM ID.
// =============================================================================
PUBLIC VOID HAL_BOOT_FUNC hal_BootSectorSetEbcRamId(UINT8 ramId)
{
    g_halBootSectorStruct.ramId = ramId;
}


// =============================================================================
// hal_BootSectorGetEbcRamId
// -----------------------------------------------------------------------------
/// Get the external RAM ID.
/// 
/// @return the external RAM ID.
// =============================================================================
PUBLIC UINT8 HAL_BOOT_FUNC hal_BootSectorGetEbcRamId(VOID)
{
    return g_halBootSectorStruct.ramId;
}


// =============================================================================
// hal_BootEnterMonitor
// -----------------------------------------------------------------------------
/// Call this function to enter the boot monitor.
// =============================================================================
PUBLIC VOID hal_BootEnterMonitor(VOID)
{
    // We want to reboot now, and not get interrupted by any trivial stuff.
    UINT32 __attribute__((unused)) status = hal_SysEnterCriticalSection();

	if (g_halBootBeforeMonCallback != NULL)
	{
		(*g_halBootBeforeMonCallback)();
	}

    // To restart in the Boot Monitor, by going through
    // the Boot Sector.
    hal_BootSectorSetCommand(BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR);

    hal_SysRestart();
}


// =============================================================================
// hal_BootRegisterBeforeMonCallback
// -----------------------------------------------------------------------------
/// Call this function to register a callback function which is run
/// before entering the boot monitor.
// =============================================================================
PUBLIC VOID hal_BootRegisterBeforeMonCallback(INT32 (*funcPtr)(VOID))
{
	g_halBootBeforeMonCallback = funcPtr;
}


