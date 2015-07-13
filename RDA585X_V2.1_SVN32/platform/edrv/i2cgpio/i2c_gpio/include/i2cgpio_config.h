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
/// @file  
/// That file describes the configuration of the structure used to set up
///  
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _GPIOI2C_CONFIG_H_
#define _GPIOI2C_CONFIG_H_

#include "cs_types.h"

#include "gpio_i2c.h"
#include "hal_gpio.h"



// =============================================================================
//  
// -----------------------------------------------------------------------------
///
/// Make sure to include gpioic_config.h before  
// =============================================================================

#define GPIOI2C_CONFIG_STRUCT_T GPIOI2C_CONFIG_STRUCT_T



// =============================================================================
//  
// -----------------------------------------------------------------------------
///  
// ============================================================================



struct GPIOI2C_CONFIG_STRUCT_T
{
    /// I2C bus used to configure the chip
    GPIO_I2C_BPS_T           i2c_gpio_Bps;
    
    HAL_GPIO_GPIO_ID_T      scl_i2c_gpio;  // if do not used the gpio SCL ,set it HAL_GPIO_NONE

    HAL_GPIO_GPO_ID_T       scl_i2c_gpo;   // if do not used gpo for SCL ,set it HAL_GPO_NONE
    
    HAL_GPIO_GPIO_ID_T      sda_i2c;    
};

#endif // 


