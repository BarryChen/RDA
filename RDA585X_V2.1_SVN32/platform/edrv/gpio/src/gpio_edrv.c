/*************************************************************************
 *
 * MODULE NAME:    GPIO_edrv.c
 * PROJECT CODE:   rda
 * DESCRIPTION:    rda gpio drv.
 * MAINTAINER:     cj
 * CREATION DATE:  16/04/10
 *
 * SOURCE CONTROL: $Id: gpio_edrv.c 1857 2010-09-01 11:25:00Z huazeng $
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
#include "hal_timers.h"
#include "sxr_tim.h"
#include "sxr_tls.h"

const GPIO_CFG_CONFIG_T *g_gpio_config;
static BOOL gpio_mute_save=FALSE;//warkey 2.0


PRIVATE VOID gpio_Muteinit(void)
{
    int i;
    HAL_GPIO_CFG_T tfCfg;
    HAL_APO_ID_T gpioMute = g_gpio_config->gpioMute;

    if (FALSE == g_gpio_config->gpioMuteEnable)
        return ;
    tfCfg.direction = HAL_GPIO_DIRECTION_OUTPUT;
    tfCfg.irqHandler = NULL;
    tfCfg.irqMask.rising=FALSE;
    tfCfg.irqMask.falling = FALSE;
    tfCfg.irqMask.debounce = FALSE;
    tfCfg.irqMask.level=FALSE;

    if(gpioMute.type == HAL_GPIO_TYPE_IO)
        hal_GpioOpen(gpioMute.gpioId,&tfCfg);

    gpio_SetMute(TRUE);
}


PUBLIC VOID gpio_set_tone_status(BOOL status)//warkey 2.0  //TRUE---提示音开始FALSE---提示音结束
{
	static BOOL gpio_pa_save=FALSE;
	if(status==1)
	{
		hal_HstSendEvent(APP_EVENT,0xad101111);
		if(gpio_mute_save==TRUE)
		{
			gpio_pa_save=gpio_mute_save;
			gpio_SetMute(FALSE);
		}
	}
	else if(status==0)
	{
		hal_HstSendEvent(APP_EVENT,0xad100000);
		if(gpio_pa_save==TRUE)
		{
			gpio_pa_save=FALSE;
			gpio_SetMute(TRUE);	
		}
	}
}

PUBLIC VOID gpio_SetMute_Pop(BOOL mute_flag)
{
	return;
    HAL_APO_ID_T gpioMute = g_gpio_config->gpioMute;

   if (FALSE == g_gpio_config->gpioMuteEnable)
       return ;

    if(mute_flag)
    {
        if(g_gpio_config->gpioMuteHigh)
            hal_GpioSet(gpioMute);
        else
            hal_GpioClr(gpioMute);
        hal_HstSendEvent(APP_EVENT,0x09031348);
    }
    else
    {
        if(g_gpio_config->gpioMuteHigh)
            hal_GpioClr(gpioMute);
        else
            hal_GpioSet(gpioMute);
        hal_HstSendEvent(APP_EVENT,0x09031349);
    }
}

PUBLIC VOID gpio_SetMute(BOOL mute_flag)
{
	//return;
    HAL_APO_ID_T gpioMute = g_gpio_config->gpioMute;

   if (FALSE == g_gpio_config->gpioMuteEnable)
       return ;
   #if 1//warkey 2.0
    if(mute_flag==gpio_mute_save)
		return;
    gpio_mute_save=mute_flag;
    #endif
    if(mute_flag)
    {

        if(g_gpio_config->gpioMuteHigh)
            hal_GpioSet(gpioMute);
        else
            hal_GpioClr(gpioMute);
        hal_HstSendEvent(APP_EVENT,0x09031348);
    }
    else
    {
        if(g_gpio_config->gpioMuteHigh)
            hal_GpioClr(gpioMute);
        else
            hal_GpioSet(gpioMute);
        hal_HstSendEvent(APP_EVENT,0x09031349);
    }
}


PUBLIC UINT32 gpio_initialise(VOID)
{
    g_gpio_config = tgt_GetGPIOCfg();
    HAL_APO_ID_T gpioMute = g_gpio_config->gpioMute;

    if(g_gpio_config->gpio_irc != HAL_GPIO_NONE)
        gpio_IRCinit(g_gpio_config->gpio_irc);

    //if(g_gpio_config->led_count > 0)
    //    gpio_LEDinit();

    if(g_gpio_config->key_count > 0)
        gpio_Keyinit();

    if (gpioMute.gpoId != HAL_GPO_NONE)
        gpio_Muteinit(); //2012-9-27

    gpio_DetectInit();
}


