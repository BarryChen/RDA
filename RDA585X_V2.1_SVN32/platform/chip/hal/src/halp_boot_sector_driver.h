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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_boot_sector_driver.h $
//  $Author: admin $
//  $Date: 2012-07-13 13:54:10 +0800 (星期五, 13 七月 2012) $
//  $Revision: 16308 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file halp_boot_sector_driver.h
///
/// HAL Boot Sector driver.
/// 
/// Provides functions to allow HAL EBC and HAL Host Monitor to write some
/// informations in the Boot Sector structure and to validate this structure.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _HALP_BOOT_SECTOR_DRIVER_H_
#define _HALP_BOOT_SECTOR_DRIVER_H_

#include "cs_types.h"

#include "chip_id.h"

#include "boot_sector.h"
#include "hal_sys.h"

// =============================================================================
// FUNCTIONS                                                            
// =============================================================================


// =============================================================================
// hal_BootSectorSetCommand
// -----------------------------------------------------------------------------
/// Set the command which must be executed by the Boot Sector.
/// Then validate the Boot Sector structure.
/// 
/// @param valid Command used during the next boot.
// =============================================================================
PROTECTED VOID hal_BootSectorSetCommand(BOOT_SECTOR_CMD_T command);


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
/// This function is *forced* inline because it will be used by HAL and by
/// the Boot Sector and the linkage of HAL and Boot Sector can be independent.
/// 
/// @param timings EBC RAM chip select timing configuration as defined by
/// the EBC CS_Time_Write register.
/// @param mode EBC RAM chip select mode configuration as defined by the
/// EBC CS_Mode register.
// =============================================================================
PROTECTED VOID hal_BootSectorSetEbcConfig(UINT32 timings, UINT32 mode);


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
PROTECTED VOID hal_BootSectorSetEbcFreq(HAL_SYS_MEM_FREQ_T ebcFreq);


// =============================================================================
// hal_BootSectorGetEbcFreq
// -----------------------------------------------------------------------------
/// Get the EBC frequency in the EBC extension configuration.
/// 
/// @return the EBC frequency.
// =============================================================================
PROTECTED HAL_SYS_MEM_FREQ_T hal_BootSectorGetEbcFreq(VOID);



#endif // _HALP_BOOT_SECTOR_DRIVER_H_
