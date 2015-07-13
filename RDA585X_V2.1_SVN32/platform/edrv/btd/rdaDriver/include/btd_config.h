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
/// @file btd_config.h
/// That file describes the configuration of the structure used to set up
/// the BTD structure stored in TGT for blue-tooth driver.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"
#include "hal_i2c.h"
#include "hal_gpio.h"


#ifndef _BTD_CONFIG_H_
#define _BTD_CONFIG_H_

// =============================================================================
// BTD_CONFIG_STRUCT_T
// -----------------------------------------------------------------------------
/// The type BTD_CONFIG_STRUCT_T is defined in tgt_btd_cfg.h using the structure
/// #BTD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
// =============================================================================
#define BTD_CONFIG_STRUCT_T BTD_CONFIG_RDABT_STRUCT_T


// =============================================================================
// BTD_CONFIG_ASC3600_STRUCT_T
// -----------------------------------------------------------------------------
/// Describes the board specific configuration of the ASC3600 BT chip
// ============================================================================
struct BTD_CONFIG_RDABT_STRUCT_T
{
    HAL_I2C_BUS_ID_T i2cBusId;
    HAL_APO_ID_T pinReset;
    HAL_APO_ID_T pinWakeUp;
    HAL_APO_ID_T pinSleep;
};


// =============================================================================
//  FUNCTIONS
// =============================================================================

///  @} <- End of the btd group 

#endif // _BTD_CONFIG_H_


