/*************************************************************************
 *
 * MODULE NAME:    GPIO_leds.c
 * PROJECT CODE:   rda
 * DESCRIPTION:    rda leds drv.
 * MAINTAINER:     cj
 * CREATION DATE:  16/04/10
 *
 * SOURCE CONTROL: $Id: gpio_leds.c 1857 2010-09-01 11:25:00Z huazeng $
 *
 * LICENSE:
 *     This source code is copyright (c) 2010-2015 rda.
 *     All rights reserved.
 *
 * NOTES TO USERS:
 *   None.
 *
 *************************************************************************/

#include "cs_types.h"
#include "gpio_config.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "cos.h"
#include "event.h"
#include "edrvp_debug.h"


extern const GPIO_CFG_CONFIG_T *g_gpio_config;

#if 0
PUBLIC VOID gpio_LEDinit()
{
    int i;
    HAL_GPIO_CFG_T tfCfg;
    tfCfg.direction = HAL_GPIO_DIRECTION_OUTPUT;
    tfCfg.irqHandler = NULL;
    tfCfg.irqMask.rising=FALSE;
    tfCfg.irqMask.falling = FALSE;
    tfCfg.irqMask.debounce = FALSE;
    tfCfg.irqMask.level=FALSE;
    for(i=0; i<g_gpio_config->led_count; i++)
    {
        if(g_gpio_config->leds[i].type == HAL_GPIO_TYPE_IO)
            hal_GpioOpen(g_gpio_config->leds[i].gpioId,&tfCfg);
    }
}
#endif

PUBLIC VOID gpio_SetLED(int index, int on)
{
    if(on^g_gpio_config->ledHighLight)
    {
        //hal_GpioClr(g_gpio_config->leds[i]);
        pmd_SetLevel(g_gpio_config->leds[index],0);
    }
    else
    {
        //hal_GpioSet(g_gpio_config->leds[i]);
        pmd_SetLevel(g_gpio_config->leds[index],1);
    }
}


