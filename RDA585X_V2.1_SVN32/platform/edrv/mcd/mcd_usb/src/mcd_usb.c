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
//
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/mcd/mcd_sdmmc/src/mcd_sdmmc.c $
//  $Author: caoxh $
//  $Date: 2011-04-27 19:33:19 +0800 (星期三, 27 四月 2011) $
//  $Revision: 7282 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file mcd_sdmmc.c
///
/// Implementation of the MCD external driver for the sdmmc protocol.
///
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"
#include "cos.h"
//#include "mcd_config.h"
//#include "tgt_mcd_cfg.h"

#include "hal_gpio.h"
#include "mcd_m.h"
#include "mcdp_debug.h"
//#include "mcdp_sdmmc.h"
#include "usb.h"
#include "mcd_usb.h"

#include "hal_sdmmc.h"
#include "hal_sys.h"

#include "stdlib.h"
#include "stdio.h"
#include "sxr_tls.h"
#include "event.h"

#include "sxs_io.h"
#include "sxr_sbx.h"
#include "usb_hcd.h"


#define MAX_USB_CLIENT		2

enum 
{
	USB_HOST_SCH_INIT,
	USB_HOST_SCH_RESET,
	USB_HOST_SCH_GET_DEVICE_DES,	
	USB_HOST_SCH_SET_ADDRESS,
	USB_HOST_SCH_GET_CONFIGURATION1,	/*get configuration descriptor itself */
	USB_HOST_SCH_GET_CONFIGURATION2,	/*get a whole configuration info*/
	USB_HOST_SCH_SET_CONFIGURATION,
	USB_HOST_SCH_INIT_END
};


PRIVATE MCD_STATUS_T      g_mcdUSBStatus = MCD_STATUS_NOTOPEN_PRESENT;
PRIVATE MCD_CARD_DETECT_HANDLER_T g_mcdUSBDiskDetectHandler;

typedef struct tagMCDUSBHOST
{
	USBDEVICEDESC *pDeviceDesc;
	USBCONFIGURATIONDESC *pConfigDes;
	uint32	UsbSchOp;
	uint8	nClientNum;
	int 		nCurrentDev;
	USBCLIENTDRIVER ClientDriver[MAX_USB_CLIENT];
	
}MCDUSBHOST;

MCDUSBHOST g_McdUsbInst;


//extern volatile int gUsbParam; 
//void *gUsbData = NULL;
//uint8 gUsbStatus = 0;

UINT8 mcd_usbHostInitOpCompleteCb_status = 0;//added for delete nParam2
// this callback fuction runs in ISR
static void mcd_usbHostInitOpCompleteCb(void *pData, uint8 status)
{
	
	COS_EVENT ev;
	
	ev.nEventId = EV_FS_USB_INIT;
	ev.nParam1  = (uint32)pData;
         mcd_usbHostInitOpCompleteCb_status = status;
//	ev.nParam2  = status;//for delete nParam2
//	ev.nParam3  = 0;//for delete nParam3
	COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	
    //gUsbParam  = 1;
    //gUsbData = pData;
    //gUsbStatus = status;
	
}

int RDA_UsbReset(void (*CompleteCb)(void* pData, uint8 status));

int Mcd_USBHostSchOperation(void *pData, uint8 status)
{
	static uint8 index = 0;
	static uint8 count = 0;
	int 	handled = 1;

    //pData = gUsbData;
    //status = gUsbStatus;
    
	if( status == 0)
	{
		count = 0;
		g_McdUsbInst.UsbSchOp++;
	}
	else
	{
		count ++;
		if( count < 3)
		{
			// let's try again... 
			
			if( g_McdUsbInst.UsbSchOp > USB_HOST_SCH_GET_DEVICE_DES)
			{
				//g_McdUsbInst.UsbSchOp = USB_HOST_SCH_RESET;
				//RDA_UsbReset(mcd_usbHostInitOpCompleteCb);
				return 0;	
			}
			else
			{
				RDA_UsbReset(NULL);	
			}	
		}
		else
		{
			return 0;
		}
	}
	
	switch( g_McdUsbInst.UsbSchOp)
	{
	case USB_HOST_SCH_RESET:
		//hal_HstSendEvent(SYS_EVENT,0x09170010);
		RDA_UsbReset(mcd_usbHostInitOpCompleteCb);
		break;
	case USB_HOST_SCH_GET_DEVICE_DES:
		//hal_HstSendEvent(SYS_EVENT,0x09170011);
		USB_GetDescriptor(USB_GET_DESCRIPTOR, USB_DES_DEVICE<<8, 0, mcd_usbHostInitOpCompleteCb);
		break;
	case USB_HOST_SCH_SET_ADDRESS:
		//hal_HstSendEvent(SYS_EVENT,0x09170012);
		if( pData != NULL)
		{
			g_McdUsbInst.pDeviceDesc = (USBDEVICEDESC*)pData;
			index = 0;
		}
		USB_SetAddress(mcd_usbHostInitOpCompleteCb);
		break;
	case USB_HOST_SCH_GET_CONFIGURATION1:
	case USB_HOST_SCH_GET_CONFIGURATION2:
		//hal_HstSendEvent(SYS_EVENT,0x08260003);
		USB_GetDescriptor(USB_GET_DESCRIPTOR, USB_DES_CONFIGURATION<<8|index, 0, mcd_usbHostInitOpCompleteCb);
		break;
	case USB_HOST_SCH_SET_CONFIGURATION:
		{
			//hal_HstSendEvent(SYS_EVENT,0x08260005);
			//hal_HstSendEvent(SYS_EVENT,pData);
			if( pData != NULL)
			{
				USBCONFIGURATIONDESC * pConfig  = (USBCONFIGURATIONDESC*)pData;
				uint8 * ptemp = (uint8*)pData;
				USBINTERFACEDESC *pInterface = (USBINTERFACEDESC*)(ptemp+sizeof(USBCONFIGURATIONDESC));
				int j = 0;

				index = pConfig->bConfigurationValue;
			
				for(j = 0; j<g_McdUsbInst.nClientNum; j++)
				{
					if( pInterface->bInterfaceClass == g_McdUsbInst.ClientDriver[j].InterfaceClass 
						&& pInterface->bInterfaceSubClass == g_McdUsbInst.ClientDriver[j].InterfaceSubClass)
					{
						g_McdUsbInst.nCurrentDev = j;
						g_McdUsbInst.pConfigDes = pConfig;
						USB_SetConfiguration(index, mcd_usbHostInitOpCompleteCb);
						break;
					}
				}
			}
			else
			{
				/* try again? */
			}
		}
		break;
	case USB_HOST_SCH_INIT_END:
		{
			//hal_HstSendEvent(SYS_EVENT,0x08090003);
			if(g_McdUsbInst.ClientDriver[g_McdUsbInst.nCurrentDev].UsbDeviceAttach)
			{
				g_McdUsbInst.ClientDriver[g_McdUsbInst.nCurrentDev].UsbDeviceAttach(g_McdUsbInst.pConfigDes, index);
			}
		}
		break;
	default:
		handled = 0;
		break;
	}
    //hal_HstSendEvent(SYS_EVENT,0x08260015);
	return handled;
}

int Mcd_USBHostInit()
{
	USBOPERATION *pOps = USB_HcdInit();

	g_McdUsbInst.UsbSchOp = USB_HOST_SCH_INIT;
	g_McdUsbInst.nCurrentDev = -1;
	
	if( pOps != NULL)
	{
		//hal_HstSendEvent(SYS_EVENT,0x08150004);
		RDA_UsbHostInit(pOps, mcd_usbHostInitOpCompleteCb);
	}
}

int Mcd_UsbHostDeInit(uint8 port)
{
	USB_HcdDeinit();
	
	//since that we only register client driver when power on, don't delete driver info
	if( g_McdUsbInst.nCurrentDev < 0)
	{
		return 0;
	}
	
	if( g_McdUsbInst.ClientDriver[g_McdUsbInst.nCurrentDev].UsbDeviceDetach)
	{
		g_McdUsbInst.ClientDriver[g_McdUsbInst.nCurrentDev].UsbDeviceDetach(NULL);
	}
	g_McdUsbInst.nCurrentDev = -1;
	hal_HstSendEvent(SYS_EVENT,0x09260002);
	
	return 0;
}

int Mcd_RegisterClientDriver(USBCLIENTDRIVER *pdriverInst)
{
	if( g_McdUsbInst.nClientNum >= MAX_USB_CLIENT  || pdriverInst == NULL)
	{
		return MCDUSB_FAILED;
	}

	g_McdUsbInst.ClientDriver[g_McdUsbInst.nClientNum++] = *pdriverInst;

	return MCDUSB_SUCCESS;
}


/// Update g_mcdStatus
/// @return TRUE is card present (only exact when GPIO is used for card detect.)
PRIVATE BOOL mcd_USBDetectUpdateStatus(VOID)
{
   return TRUE;
}

PRIVATE VOID mcd_USBDiskDetectHandler(VOID)
{
    BOOL CardPresent = mcd_USBDetectUpdateStatus();

    g_mcdUSBDiskDetectHandler(CardPresent);
}

// =============================================================================
// mcd_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================
PUBLIC MCD_ERR_T mcd_SetUSBDiskDetectHandler(MCD_CARD_DETECT_HANDLER_T handler)
{
/*    if(NULL == g_mcdConfig)
    {
        g_mcdConfig = tgt_GetMcdConfig();
    }

    if(g_mcdConfig->cardDetectGpio == HAL_GPIO_NONE)
    {
        return MCD_ERR_NO_HOTPLUG;
    }

    if(NULL != handler)*/
    {
        HAL_GPIO_CFG_T cfg  = 
            {
            .direction      = HAL_GPIO_DIRECTION_INPUT,
            .irqMask        = 
                {
                .rising     = TRUE,
                .falling    = TRUE,
                .debounce   = TRUE,
                .level      = FALSE
                },
            .irqHandler     = mcd_USBDiskDetectHandler
            };

        hal_GpioOpen(HAL_GPIO_6, &cfg);
        g_mcdUSBDiskDetectHandler = handler;
    }
/*    else
    {
        hal_GpioClose(g_mcdConfig->cardDetectGpio);
        g_mcdCardDetectHandler = NULL;
    }*/
    
    return MCD_ERR_NO;
}


