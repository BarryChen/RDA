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

#ifndef _GPIO_EDRV_H_
#define _GPIO_EDRV_H_

enum {
    GPIO_INVALID            = -1,
    GPIO_NOT_DETECTED       = 0,
    GPIO_DETECTED           = 1,
};


PUBLIC UINT32 gpio_initialise(VOID);

PUBLIC UINT32 gpio_detect_linein(VOID);
PUBLIC UINT32 gpio_detect_earpiece(VOID);
PUBLIC UINT32 gpio_detect_tcard(VOID);
PUBLIC UINT32 gpio_detect_usbdisk(VOID);

PUBLIC VOID gpio_SetLED(int index, int on);
PUBLIC VOID gpio_SetMute(BOOL mute_flag);

#endif // #ifndef _GPIO_EDRV_H_


