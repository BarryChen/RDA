#include "cs_types.h"
#include "cos.h"
#include "usb.h"
#include "event.h"

#include "sxr_mem.h"

#include "usb_hcd.h"
#include "hal_usb.h"
#include "event.h"


static USBHCD gHcdInst;
static volatile int VendorCmdFinish = 0;
static volatile int BulkTransFinish = 0;
void VendorCmdCallBack(void* pData, uint8 status)
{
	VendorCmdFinish = 1;

	/*
	COS_EVENT ev;
	
	ev.nEventId = EV_FS_USB_INIT;
	ev.nParam1  = (uint32)pData;
	ev.nParam2  = status;
	ev.nParam3  = 0;
	COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	*/
}

void BulkTransferCompleteCallBack(HANDLE handle, uint8 status)
{
	BulkTransFinish = 1;
	/*
	COS_EVENT ev;
	
	ev.nEventId = EV_FS_USB_TRANSFERRED;
	ev.nParam1  = handle;
	ev.nParam2  = status;
	ev.nParam3  = 0;
	COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	*/
}

USBOPERATION *USB_HcdInit()
{	
	VendorCmdFinish = 0;
	BulkTransFinish = 0;
	memset(&gHcdInst, 0, sizeof(USBHCD));
	return &gHcdInst.usbops;
}

int USB_HcdDeinit()
{
	int i = 0;
	
	if( gHcdInst.pdeviceDesc )
	{
		sxr_Free(gHcdInst.pdeviceDesc);
	}	

	if( gHcdInst.pconfig != NULL)
	{
		sxr_Free(gHcdInst.pconfig);
	}

	for(; i<3; i++)
	{
		if( gHcdInst.pConfigDesc[i] )
		{
			sxr_Free(gHcdInst.pConfigDesc[i]);
		}
	}
	VendorCmdFinish = 1;
	BulkTransFinish = 1;
	gHcdInst.flag = USB_HCD_DEVICE_DETACHED;
	return 0;
}

int USB_SetAddress(void (*Cb)(void*pData, uint8 status))
{
	USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
	memset(pReq, 0, sizeof(USBCONTROLREQUEST));

	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
	{
		return USBHCD_DEVICE_DISCONNECTED;
	}
	
	gHcdInst.address ++;
	if( gHcdInst.address == 0)
	{
		gHcdInst.address  = 1;
	}

	pReq->completeCb =  Cb;
	pReq->stdRequest.bRequest = USB_SET_ADDRESS;
	pReq->stdRequest.wValue = gHcdInst.address;
	
	if(gHcdInst.usbops.ControlTransfer)
	{
		return gHcdInst.usbops.ControlTransfer(pReq);
	}
	else
	{
		return  -1;
	}
}

int USB_SetConfiguration(uint8 bValue, void (*Cb)(void*pData, uint8 status))
{
	USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
	memset(pReq, 0, sizeof(USBCONTROLREQUEST));

	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
	{
		return -1;
	}
		
	pReq->completeCb =  Cb;
	pReq->stdRequest.wValue = bValue;
	pReq->stdRequest.bRequest = USB_SET_CONFIGURATION;

	if( gHcdInst.usbops.ControlTransfer)
	{
		return gHcdInst.usbops.ControlTransfer( pReq );
	}
	else
	{
		return -1;
	}
}

int USB_GetDescriptor(uint8 type, uint16 wValue, uint16 wIndex, void (*Cb)(void*pData, uint8 status))
{
	uint8 desType = wValue>>8;
	uint8 desIndex = wValue;
	USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
	memset(pReq, 0, sizeof(USBCONTROLREQUEST));

	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
	{
		return USBHCD_DEVICE_DISCONNECTED;
	}
		
	pReq->stdRequest.bmRequestType = 0x80;
	pReq->stdRequest.bRequest = USB_GET_DESCRIPTOR;
	pReq->stdRequest.wValue = wValue;

	if( desType == USB_DES_DEVICE )
	{
		gHcdInst.pdeviceDesc = (USBDEVICEDESC*)sxr_Malloc(sizeof(USBDEVICEDESC));
		pReq->nBufLen = pReq->stdRequest.wLength = sizeof(USBDEVICEDESC); 
		pReq->pBuf = (uint8*)gHcdInst.pdeviceDesc;
	}
	else if( desType == USB_DES_CONFIGURATION )
	{
		//the first time we read a specific configuration
		if( gHcdInst.pconfig == NULL)
		{
			gHcdInst.pconfig = (USBCONFIGURATIONDESC*)sxr_Malloc(sizeof(USBCONFIGURATIONDESC));
			memset(gHcdInst.pconfig, 0, sizeof(USBCONFIGURATIONDESC));
		}
		
		if(gHcdInst.pconfig->wTotalLength == 0)
		{
			hal_HstSendEvent(SYS_EVENT,0x09060003);
			pReq->stdRequest.wLength = sizeof(USBCONFIGURATIONDESC);
			pReq->pBuf = (uint8*)gHcdInst.pconfig;
		}
		else
		{
			hal_HstSendEvent(SYS_EVENT,0x09060004);
			gHcdInst.pConfigDesc[desIndex]= sxr_Malloc( gHcdInst.pconfig->wTotalLength);
			if( gHcdInst.pConfigDesc[desIndex] == NULL)
			{
				return USBHCD_NO_RESOURCE;
			}
			
			pReq->stdRequest.wLength = gHcdInst.pconfig->wTotalLength;
			pReq->pBuf = (uint8*)gHcdInst.pConfigDesc[desIndex];
			
			//memset(g_pHcdInst->pconfig, 0, sizeof(USBCONFIGURATIONDESC));
		}
	}
	else 
	{
		return USBHCD_INVALID_PARAM;
	}
	
	pReq->stdRequest.wIndex = wIndex;
	pReq->completeCb =  Cb;

	if(gHcdInst.usbops.ControlTransfer)
	{
		return  gHcdInst.usbops.ControlTransfer(pReq);
	}
	else
	{
		return USBHCD_ERROR_FAILED;
	}
	
}

int USB_SetInterface(uint16 bAlternateSetting, uint16 Interface, void (*Cb)(void*pData, uint8 status))
{
	USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
	memset(pReq, 0, sizeof(USBCONTROLREQUEST));

	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
	{
		return USBHCD_DEVICE_DISCONNECTED;
	}
		
	pReq->stdRequest.bmRequestType = 0x01;	/*dest: interface */ 
	pReq->stdRequest.bRequest = USB_SET_INTERFACE;
	pReq->stdRequest.wIndex = Interface;
	pReq->stdRequest.wValue = bAlternateSetting;

	pReq->completeCb = Cb;

	if(gHcdInst.usbops.ControlTransfer)
	{
		return  gHcdInst.usbops.ControlTransfer(pReq);
	}
	else
	{
		return USBHCD_ERROR_FAILED;
	}
	
}

USB_PIPE USB_OpenPipe(USBENDPOINTDESC *pEndDes)
{
	if(gHcdInst.usbops.OpenPipe)
	{
		return gHcdInst.usbops.OpenPipe(pEndDes);
	}
	return NULL;
}

int USB_VendorTransfer(USBSTDREQUEST *pStdRequest, void *pBuf, uint32* pnDataTransferred, uint32 timeout)
{
	USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
	memset(pReq, 0, sizeof(USBCONTROLREQUEST));

	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
	{
		return USBHCD_DEVICE_DISCONNECTED;
	}
		
	pReq->stdRequest = *pStdRequest;
	pReq->completeCb = VendorCmdCallBack;
	pReq->pBuf = pBuf;
	pReq->nBufLen = pStdRequest->wLength;

	if(gHcdInst.usbops.ControlTransfer)
	{
		if( gHcdInst.usbops.ControlTransfer(pReq) != USB_NO_ERROR )
		{
			return USBHCD_ERROR_FAILED;
		}
	}
	else
	{
		return USBHCD_ERROR_FAILED;
	}

	while(1) //timeout
	{
		timeout--;
		//sxr_Sleep(1 MS_WAITING);
		if( VendorCmdFinish > 0)
			break;
	}
	VendorCmdFinish = 0;
	return 0;
}

int USB_BulkOrInterruptTransfer(USB_PIPE pipe, void* pBuf, uint32 BufLen, uint32* pnBytesTransferred, uint32 timeout)
{
	uint32 status = 0;
	USBBULKREQUEST *pReq = &gHcdInst.bulkReq;
	
	memset(pReq, 0, sizeof(USBBULKREQUEST));

	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
	{
		return USBHCD_DEVICE_DISCONNECTED;
	}
		
	pReq->nBufLen = BufLen;
	pReq->pBuf = pBuf;
	pReq->pipe = pipe;
	pReq->pnByteTransferred = pnBytesTransferred;
	pReq->completeCb = BulkTransferCompleteCallBack;
	pReq->pstatus = &status;
	//pReq->handle = COS_CreateSemaphore(1);

	if( gHcdInst.usbops.BulkTransfer )
	{
		if( gHcdInst.usbops.BulkTransfer(pReq) != USB_NO_ERROR )
		{
			return USBHCD_ERROR_FAILED;
		}	
	}
	else
	{
		return USBHCD_ERROR_FAILED;
	}

WAIT_FRAME_PERIOD:
	while(1)	//timeout
	{
		timeout --;
		if( BulkTransFinish > 0)
		{
			break;
		}
	}
	//hal_HstSendEvent(SYS_EVENT,0x55555555);
	BulkTransFinish = 0;
	
	if( status == USB_STALL_ERROR)
	{
		//clear feature
		return USB_STALL_ERROR;
	}
	else if(status == USB_RECEIVED_NAK)
	{
		int RDAUsb_PacketTransfer(int chn);
		RDAUsb_PacketTransfer(pipe);
		// try to send again;
		goto WAIT_FRAME_PERIOD;
	}
	else if( status == USBHCD_DEVICE_DISCONNECTED )
	{
		hal_HstSendEvent(SYS_EVENT,0x88888888);
		gHcdInst.flag = USB_HCD_DEVICE_DETACHED;
		return USBHCD_DEVICE_DISCONNECTED;
	}

	return USB_NO_ERROR;
	
}

int USB_ResetDefaultEndpoint()
{
	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
		return USBHCD_DEVICE_DISCONNECTED;
	else
		return 0;
}

int USB_ResetEndpoint(void *pipe, uint32 timeout)
{
	if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
		return USBHCD_DEVICE_DISCONNECTED;
	else
		return 0;
}




