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

#ifndef _GPIO_CONFIG_H_
#define _GPIO_CONFIG_H_

#include "cs_types.h"

#include "hal_gpio.h"



// =============================================================================
//  
// -----------------------------------------------------------------------------
///
/// Make sure to include gpioic_config.h before  
// =============================================================================
#define HAL_GPIO_MAX_LED                8
#define HAL_GPIO_MAX_GPIOKEYS           8

#define GPIO_CONFIG_STRUCT_T GPIO_CONFIG_STRUCT_T

// =============================================================================
//  
// -----------------------------------------------------------------------------
///  
// ============================================================================


typedef struct GPIO_CONFIG_STRUCT_T
{
    // gpio for irc
    HAL_GPIO_GPIO_ID_T gpio_irc;

    // leds config
    UINT8 led_count;
    /// when set to TRUE, the GPIO value 1 is for open the led light
    BOOL ledHighLight;
    UINT8 leds[HAL_GPIO_MAX_LED]; // PMD_LEVEL_ID_T

    // gpio key config
    UINT8 key_count;
    BOOL  key_interrupt_mode; // true if use interrupt mode, else use scan mode
    BOOL  high_keydown; // true if high for key down
    HAL_GPIO_GPIO_ID_T gpio_keys[HAL_GPIO_MAX_GPIOKEYS];

    HAL_GPIO_GPIO_ID_T gpio_lineIn;
    /// when set to TRUE, the GPIO value 1 is deal as linein plugin
    BOOL lineinPlugHigh;

    /// Gpio Connected to socket to detect card insertion/removal .
    /// (set to #HAL_GPIO_NONE) if not available.
    HAL_GPIO_GPIO_ID_T  cardDetectGpio;
    /// Define the polarity of the above GPIO: 
    /// \c TRUE GPIO is high when card is present,
    /// \c FALSE GPIO is low when card is present.
    BOOL                gpioCardDetectHigh;

    /// The GPIO pin to detect earpiece
    HAL_GPIO_GPIO_ID_T earpieceDetectGpio;
    /// when set to TRUE, the GPIO value 1 is deal as earpice plugin
    BOOL earpiecePlugHigh;
    /// If no GPIO check, earpiece plugin or not
    BOOL earpiecePlugin;

    // the GPIO pin to mute
    HAL_APO_ID_T gpioMute;
    // enable gpio mute function
    BOOL gpioMuteEnable;
    // when set to TRUE, set the GPIO value 1 to mute
    BOOL gpioMuteHigh;

    // the GPIO for wake up uart2
    HAL_GPIO_GPIO_ID_T uart2_wake;

    /// The GPIO pin to detect USB
    HAL_GPIO_GPIO_ID_T gpioUSBDetect;
    /// when set to TRUE, the GPIO value 1 is deal as usb plugin
    BOOL gpioUSBPlugHigh;
    /// If detect usb by GPIO or not
    BOOL gpioUSBDetectEnable;

} GPIO_CFG_CONFIG_T;


PUBLIC CONST GPIO_CFG_CONFIG_T* tgt_GetGPIOCfg(VOID);

#endif // 


