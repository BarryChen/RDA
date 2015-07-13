/*************************************************************************
 *
 * MODULE NAME:    GPIO_detect.c
 * PROJECT CODE:   rda
 * DESCRIPTION:    rda gpio detect drv.
 * MAINTAINER:     cj
 * CREATION DATE:  16/04/10
 *
 * SOURCE CONTROL: $Id: gpio_linein.c 1857 2010-09-01 11:25:00Z huazeng $
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
#include "cos.h"
#include "event.h"
#include "gpio_edrv.h"

extern const GPIO_CFG_CONFIG_T *g_gpio_config;


PUBLIC UINT32 gpio_detect_linein(void)
{
    if(g_gpio_config->gpio_lineIn == HAL_GPIO_NONE)
        return GPIO_INVALID;
    if(hal_GpioGet(g_gpio_config->gpio_lineIn) == g_gpio_config->lineinPlugHigh)
        return GPIO_DETECTED;
    return GPIO_NOT_DETECTED;
}

PROTECTED VOID gpio_IrqLineinHandler()
{
    COS_EVENT ev;
    ev.nEventId  = EV_LINE_IN;
    if(hal_GpioGet(g_gpio_config->gpio_lineIn) == g_gpio_config->lineinPlugHigh)
        ev.nParam1  = 1;
    else
        ev.nParam1  = 0;
//    ev.nParam2  = 0;//for delete nParam2
//    ev.nParam3  = 0;//for delete nParam3	

    COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

PUBLIC VOID gpio_IrqLineinInit()
{
    HAL_GPIO_CFG_T tfCfg;
    tfCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    tfCfg.irqHandler = gpio_IrqLineinHandler;
    tfCfg.irqMask.rising=TRUE;
    tfCfg.irqMask.falling = TRUE;
    tfCfg.irqMask.debounce = TRUE;
    tfCfg.irqMask.level=FALSE;
    hal_GpioOpen(g_gpio_config->gpio_lineIn,&tfCfg);
}

// =============================================================================
//  Earphone detection stuff
// =============================================================================
UINT8 ear_time_handle;

VOID gpio_EarDetectDelay(VOID)
{
    HAL_GPIO_CFG_T eraCfg;
    HAL_GPIO_GPIO_ID_T gpio = g_gpio_config->earpieceDetectGpio;

   // if (ear_handler != NULL)
    {
    	COS_EVENT ev;
        if (hal_GpioGet(gpio) == g_gpio_config->earpiecePlugHigh)
        {
            //(*ear_handler)(FALSE);
            ev.nParam1 = 1;
        }
        else
        {
            //(*ear_handler)(TRUE);
            ev.nParam1 = 0;
        }
		ev.nEventId = EV_DM_EAR_PEACE_DETECT_IND;
		COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        
    }
    eraCfg.irqMask.rising = TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(gpio);
    ear_time_handle = 0;
}

VOID gpio_EarDetectHandler(VOID)
{
    HAL_GPIO_GPIO_ID_T gpio = g_gpio_config->earpieceDetectGpio;
    HAL_GPIO_CFG_T eraCfg;
    eraCfg.irqMask.rising = FALSE;
    eraCfg.irqMask.falling = FALSE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(gpio);
    //sxr_StopFunctionTimer(gpio_EarDetectDelay);
    COS_KillTimer(ear_time_handle);
    ear_time_handle = COS_SetTimer(100, gpio_EarDetectDelay, NULL, COS_TIMER_MODE_SINGLE);
}

VOID gpio_EarpDetectGpioInit(void)
{
    HAL_GPIO_CFG_T eraCfg;
    eraCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    eraCfg.irqHandler = gpio_EarDetectHandler;
    eraCfg.irqMask.rising=TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level=FALSE;
    hal_GpioOpen(g_gpio_config->earpieceDetectGpio,&eraCfg);
}

PUBLIC UINT32 gpio_detect_earpiece(VOID)
{
    if (g_gpio_config->earpieceDetectGpio == HAL_GPIO_NONE)
    {
        return g_gpio_config->earpiecePlugin;
    }
    else
    {
        return hal_GpioGet(g_gpio_config->earpieceDetectGpio ) == g_gpio_config->earpiecePlugHigh;
    }
}

#ifdef MCD_TFCARD_SUPPORT
static UINT8 g_timer_id_gpio_card = 0;
void gpio_CardDetectTimerHandler(VOID)
{
    COS_EVENT ev;
    ev.nEventId = EV_FS_TFLASH_DETECT;
    ev.nParam1  = mcd_CardDetectUpdateStatus();
//    ev.nParam2  = 0;//for delete nParam2
//    ev.nParam3  = 0;//for delete nParam3
    COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    g_timer_id_gpio_card = 0;
}
#endif
VOID gpio_CardDetectHandler(VOID)
{
#ifdef MCD_TFCARD_SUPPORT

	extern UINT8 g_tflash_detect_flag;
	if(1 == g_tflash_detect_flag)
	{
		hal_HstSendEvent(SYS_EVENT, 0x14061444);
		return;
	}
    COS_EVENT ev;
    BOOL CardPresent = mcd_CardDetectUpdateStatus();

//    if(CardPresent == TRUE)
    {
        if(g_timer_id_gpio_card == 0)
        {
            g_timer_id_gpio_card = COS_SetTimer(100, gpio_CardDetectTimerHandler, NULL, COS_TIMER_MODE_SINGLE);
        }
        else
        {
            COS_ChangeTimer(g_timer_id_gpio_card, 100);
        }

        return;
    }
    
    ev.nEventId = EV_FS_TFLASH_DETECT;
    ev.nParam1  = CardPresent;
//    ev.nParam2  = 0;//for delete nParam2
//    ev.nParam3  = 0;//for delete nParam3
    COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
#endif
}

/*VOID pm_USBDiskDetect(VOID)
{
    COS_EVENT ev;

    ev.nEventId = EV_FS_USBDISK_DETECT;
    ev.nParam1  = cardPlugged;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}*/

VOID gpio_IrqCardDetectInit(VOID)
{
#ifdef MCD_TFCARD_SUPPORT
    HAL_GPIO_CFG_T cardCfg;
    cardCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    cardCfg.irqHandler = gpio_CardDetectHandler;
    cardCfg.irqMask.rising = TRUE;
    cardCfg.irqMask.falling = TRUE;
    cardCfg.irqMask.debounce = TRUE;
    cardCfg.irqMask.level = FALSE;
    hal_GpioOpen(g_gpio_config->cardDetectGpio,&cardCfg);
#endif
}


PUBLIC UINT32 gpio_detect_tcard(VOID)
{
#ifdef MCD_TFCARD_SUPPORT
    if(g_gpio_config->cardDetectGpio == HAL_GPIO_NONE)
        return GPIO_INVALID;
    if(hal_GpioGet(g_gpio_config->cardDetectGpio) == g_gpio_config->gpioCardDetectHigh)
        return GPIO_DETECTED;
#endif
    return GPIO_NOT_DETECTED;
}

PUBLIC UINT32 gpio_detect_usbdisk(VOID)
{

}

#ifdef UART2_SUPPORT
VOID gpio_Uart2DetectHandler(VOID)
{
    uart2_wakeup();
}

VOID gpio_Uart2DetectGpioInit(void)
{
    HAL_GPIO_CFG_T uart2Cfg;
    uart2Cfg.direction = HAL_GPIO_DIRECTION_INPUT;
    uart2Cfg.irqHandler = gpio_Uart2DetectHandler;
    uart2Cfg.irqMask.rising= TRUE;
    uart2Cfg.irqMask.falling = TRUE;
    uart2Cfg.irqMask.debounce = TRUE;
    uart2Cfg.irqMask.level = FALSE;
    hal_GpioOpen(g_gpio_config->uart2_wake,&uart2Cfg);
}
#endif

BOOL gpio_detect_usb(void)
{
    if(g_gpio_config->gpioUSBDetectEnable)
        return (hal_GpioGet(g_gpio_config->gpioUSBDetect)==g_gpio_config->gpioUSBPlugHigh);
    else
        return (pmd_GetChargerHwStatus() == 2 /*CHR_STATUS_AC_ON*/);
}


VOID gpio_USBDetectGpioInit(void)
{
    HAL_GPIO_CFG_T usbgpioCfg;
    usbgpioCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    usbgpioCfg.irqHandler = NULL;
    usbgpioCfg.irqMask.rising= FALSE;
    usbgpioCfg.irqMask.falling = FALSE;
    usbgpioCfg.irqMask.debounce = TRUE;
    usbgpioCfg.irqMask.level = FALSE;
    hal_GpioOpen(g_gpio_config->gpioUSBDetect,&usbgpioCfg);
}

PUBLIC VOID gpio_DetectInit(void)
{
    if(g_gpio_config->gpio_lineIn != HAL_GPIO_NONE)
        gpio_IrqLineinInit();

    if(g_gpio_config->cardDetectGpio != HAL_GPIO_NONE)
        gpio_IrqCardDetectInit();

    if(g_gpio_config->earpieceDetectGpio != HAL_GPIO_NONE)
        gpio_EarpDetectGpioInit();

#ifdef UART2_SUPPORT
    if(g_gpio_config->uart2_wake!= HAL_GPIO_NONE)
        gpio_Uart2DetectGpioInit();
#endif

    if(g_gpio_config->gpioUSBDetectEnable)
        gpio_USBDetectGpioInit();
}


