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

#ifndef _GPIOSPI_CONFIG_H_
#define _GPIOSPI_CONFIG_H_

#include "cs_types.h"

#include "gpio_spi.h"
#include "hal_gpio.h"



// =============================================================================
//  
// -----------------------------------------------------------------------------
///
/// Make sure to include gpioic_config.h before  
// =============================================================================

#define GPIOSPI_CONFIG_STRUCT_T GPIO_SPI_CONFIG_STRUCT_T



// =============================================================================
// 
// -----------------------------------------------------------------------------
///  
// ============================================================================



struct GPIO_SPI_CONFIG_STRUCT_T
{
    /// SPI bus used to configure the chip

    HAL_APO_ID_T      scs_spi;        

    HAL_APO_ID_T      scl_spi;  
    
    HAL_APO_ID_T      sdi_spi;    

    HAL_APO_ID_T      sdo_spi;    

};

#endif // 


