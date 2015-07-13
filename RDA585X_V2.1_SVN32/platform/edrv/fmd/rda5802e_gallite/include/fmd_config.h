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
/// the FMD structure stored in TGT for RDA5802 driver.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "hal_spi.h"
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
#define FMD_CONFIG_STRUCT_T FMD_CONFIG_RDA5802E_GALLITE_STRUCT_T

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
// FMD_CONFIG_RDA5802E_GALLITE_STRUCT_T
// -----------------------------------------------------------------------------
/// Describes the board specific configuration of the RDA5802 FM chip
// ============================================================================
struct FMD_CONFIG_RDA5802E_GALLITE_STRUCT_T
{
    /// Sleep time between power on and access
    UINT32                  powerOnTime;

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


