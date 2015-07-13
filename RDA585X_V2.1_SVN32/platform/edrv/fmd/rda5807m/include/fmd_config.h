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
//                                                                            //
/// @file fmd_config.h
/// That file describes the configuration of the structure used to set up
/// the FMD structure stored in TGT for RDA5870 driver.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "hal_i2c.h"
#include "hal_gpio.h"


#ifndef _FMD_CONFIG_H_
#define _FMD_CONFIG_H_

// =============================================================================
// FMD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type FMD_CONFIG_T is defined in tgt_fmd_cfg.h using the structure
/// #FMD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include fmd_config.h before fmd_m.h or tgt_fmd_cfg.h !
// =============================================================================
#define FMD_CONFIG_STRUCT_T FMD_CONFIG_RDA5876_STRUCT_T

#include "fmd_m.h"


/// @defgroup fmd FM Driver
/// @{


// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// FMD_CHANNEL_SPACING_T
// -----------------------------------------------------------------------------
/// This type describes the possible value for the FM channels spacing
// =============================================================================
typedef enum
{
    /// channels every 100kHz
    FMD_CHANNEL_SPACE_100 = 0,
    /// channels every 200kHz
    FMD_CHANNEL_SPACE_200,
    /// channels every 50kHz
    FMD_CHANNEL_SPACE_50,

    FMD_CHANNEL_SPACING_QTY
} FMD_CHANNEL_SPACING_T;


// =============================================================================
// FMD_CONFIG_RDA5870_STRUCT_T
// -----------------------------------------------------------------------------
/// Describes the board specific configuration of the RDA5870 FM chip
// ============================================================================
struct FMD_CONFIG_RDA5876_STRUCT_T
{
    /// I2C bus used to configure the chip
    HAL_I2C_BUS_ID_T 		i2cBusId;
    /// I2C bus used to configure the chip
    HAL_I2C_BPS_T           i2cBps;
    /// Sleep time between power on and access
    UINT32                  powerOnTime;

    /// Volume conversion table: the FM chip \c VOLUME_DAC value for each
    /// logical levels in #FMD_ANA_LEVEL_T.
    /// ( #FMD_ANA_MUTE is always mute regardless of this array ) 
    UINT8                   volumeVal[FMD_ANA_VOL_QTY];

    /// Channels spacing
    FMD_CHANNEL_SPACING_T   channelSpacing;

    /// RSSI threshold for automatic seek
    UINT8                   seekRSSIThreshold;

};


// =============================================================================
//  FUNCTIONS
// =============================================================================

///  @} <- End of the fmd group 

#endif // _FMD_CONFIG_H_


