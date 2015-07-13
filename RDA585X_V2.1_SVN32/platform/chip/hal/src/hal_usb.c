////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2008, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/hal/src/hal_usb.c $
//  $Author: jiancui $
//  $Date: 2011-12-29 17:17:20 +0800 (������, 29 ʮ���� 2011) $
//  $Revision: 12962 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file hal_usb.c
///
/// Hal USB public function
//
////////////////////////////////////////////////////////////////////////////////

#if (CHIP_HAS_USB == 1)

#if defined(HAL_USB_PRINTF) && defined(HAL_TRACE_FORCE_USB)
  #error "You can t use HAL_USB_PRINTF with HAL_TRACE_FORCE_USB"
#endif

#ifdef HAL_USB_PRINTF
  #define HAL_USB_TRACE HAL_TRACE
#else
  #define HAL_USB_TRACE(level, tsmap, fmt, ...)
#endif /* HAL_USB_PRINTF */

#include "pmd_m.h"

#include "sxr_ops.h"
#include "halp_sys.h"

#ifdef HAL_USB_DRIVER_DONT_USE_ROMED_CODE

#include "halp_usb.h"
#include "halp_debug.h"
//#include "halp_version.h"
#include "hal_usb.h"

#include "boot_usb.h"

#include "string.h"

#if ((CHIP_HAS_ACCO_PLL) || (CHIP_HAS_ACCO_AUDIO))
#include "ana_acco.h"
#endif

#include "cfg_regs.h"
#include "global_macros.h"
#include "usbc.h"
#include "sys_ctrl.h"
#include "event.h"
#include "cos.h"


// =============================================================================
// MACRO
// =============================================================================

#define EPIN       0x01
#define EPOUT      0x02
#define EPINOUT    0x00

#define EPDIR(n)   (((hwp_usbc->GHWCFG1)>>(2*n))&0x03)

#define SINGLE     0
#define INCR       1
#define INCR4      3
#define HAL_USB_AHB_MODE    INCR4


#define RXFIFOSIZE 64
#define TXFIFOSIZE 48

#ifndef USB_MAX_STRING
#define USB_MAX_STRING 8
#endif /* USB_MAX_STRING */

#define HAL_USB_OPERATE_TIME 1638

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// HAL_USB_REQUEST_DESTINATION_T
// -----------------------------------------------------------------------------
/// Destination of USB command
// =============================================================================
typedef enum
{
    HAL_USB_REQUEST_DESTINATION_DEVICE    = 0,
    HAL_USB_REQUEST_DESTINATION_INTERFACE = 1,
    HAL_USB_REQUEST_DESTINATION_EP        = 2
} HAL_USB_REQUEST_DESTINATION_T ;

// =============================================================================
// HAL_USB_REQUEST_DEVICE_T
// -----------------------------------------------------------------------------
/// List of device request
// =============================================================================
typedef enum
{
    HAL_USB_REQUEST_DEVICE_SETADDR = 0x05,
    HAL_USB_REQUEST_DEVICE_GETDESC = 0x06,
    HAL_USB_REQUEST_DEVICE_SETCONF = 0x09
} HAL_USB_REQUEST_DEVICE_T ;

// =============================================================================
// HAL_USB_DEVICE_DESCRIPTOR_REAL_T
// -----------------------------------------------------------------------------
/// Device descriptor structure
// =============================================================================
typedef struct
{
    UINT8  size;
    UINT8  type;
    UINT16 bcdUsb;
    UINT8  usbClass;
    UINT8  usbSubClass;
    UINT8  usbProto;
    UINT8  ep0Mps;
    UINT16 vendor;
    UINT16 product;
    UINT16 release;
    UINT8  iManufacturer;
    UINT8  iProduct;
    UINT8  iSerial;
    UINT8  nbConfig;
} PACKED HAL_USB_DEVICE_DESCRIPTOR_REAL_T;

// =============================================================================
// HAL_USB_CONFIG_DESCRIPTOR_REAL_T
// -----------------------------------------------------------------------------
/// Config descriptor structure
// =============================================================================
typedef struct
{
    UINT8  size;
    UINT8  type;
    UINT16 totalLength;
    UINT8  nbInterface;
    UINT8  configIndex;
    UINT8 iDescription;
    UINT8  attrib;
    UINT8  maxPower;
}  PACKED HAL_USB_CONFIG_DESCRIPTOR_REAL_T;
// =============================================================================
// HAL_USB_CONFIG_DESCRIPTOR_REAL_T
// -----------------------------------------------------------------------------
/// Config descriptor structure
// =============================================================================
typedef struct
{
    UINT8  size;
    UINT8  type;
    UINT16 totalLength;
    UINT8  nbInterface;	
	UINT8  bConfigurationValue;
    UINT8  configIndex;
    UINT8  attrib;
    UINT8  maxPower;
}  PACKED HAL_USBA_CONFIG_DESCRIPTOR_REAL_T;
// =============================================================================
// HAL_USB_INTERFACE_DESCRIPTOR_REAL_T
// -----------------------------------------------------------------------------
/// Interface descriptor structure
// =============================================================================
typedef struct
{
    UINT8  size;
    UINT8  type;
    UINT8  interfaceIndex;
    UINT8  alternateSetting;
    UINT8  nbEp;
    UINT8  usbClass;
    UINT8  usbSubClass;
    UINT8  usbProto;
    UINT8  iDescription;
} PACKED HAL_USB_INTERFACE_DESCRIPTOR_REAL_T;
// =============================================================================
// HAL_USB_ISO_EP_DESCRIPTOR_REAL_T
// -----------------------------------------------------------------------------
/// ISO Ep descriptor structure
// =============================================================================
typedef struct
{
   UINT8  size;
   UINT8  type;
   UINT8  ep;
   UINT8  attributes;
   UINT16 mps;
   UINT8  interval;
   UINT8 bRefreash;
   UINT8 bSynchAddr;
} PACKED HAL_USB_ISO_EP_DESCRIPTOR_REAL_T;
// =============================================================================
// EP0_STATE_T
// -----------------------------------------------------------------------------
/// State of ep0
// =============================================================================
typedef enum
{
    EP0_STATE_IDLE,
    EP0_STATE_IN,
    EP0_STATE_OUT,
    EP0_STATE_STATUS_IN,
    EP0_STATE_STATUS_OUT
} EP0_STATE_T ;

// =============================================================================
// HAL_USB_TRANSFERT_T
// -----------------------------------------------------------------------------
/// Structure containt the transfert parameter
// =============================================================================
typedef struct
{
    INT32  sizeRemaining;
    INT32  sizeTransfered;
} HAL_USB_TRANSFERT_T;

// =============================================================================
// HAL_USB_GLOBAL_VAR_T
// -----------------------------------------------------------------------------
/// Structure with all global var
// =============================================================================
typedef struct
{
    HAL_USB_GETDESCRIPTOR_CALLBACK_T DeviceCallback;
    HAL_USB_DEVICE_DESCRIPTOR_T*     Desc;
    HAL_USB_CALLBACK_T               EpInCallback [DIEP_NUM];
    HAL_USB_CALLBACK_T               EpOutCallback[DOEP_NUM];
    UINT32                           EpFlag;
    UINT32                           EpInterruptMask;
    UINT8*                           String       [USB_MAX_STRING];

    // Transfert data
    HAL_USB_TRANSFERT_T             InTransfert [DIEP_NUM+1];
    HAL_USB_TRANSFERT_T             OutTransfert[DOEP_NUM+1];

    // Ep0 State
    HAL_USB_SETUP_REQUEST_DESC_T     RequestDesc;
    UINT8                            Ep0State;
    UINT8                            Ep0Index;

    UINT8                            Config;
    UINT8                            NbEp;
    UINT8                            NbInterface;
    UINT8                            NbString;
} HAL_USB_GLOBAL_VAR_T;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

PRIVATE HAL_USB_GLOBAL_VAR_T g_HalUsbVar =
{
    .DeviceCallback = 0             ,
    .Desc           = 0             ,
    .NbEp           = 0             ,
    .NbInterface    = 0             ,
    .NbString       = 0             ,
    .Ep0State       = EP0_STATE_IDLE,
    .Config         = 0xFF
};

// Ep0 Buffer
PRIVATE UINT8 USB_UCBSS_INTERNAL g_HalUsbBufferEp0Out[HAL_USB_MPS];
PRIVATE UINT8 USB_UCBSS_INTERNAL g_HalUsbBufferEp0In [HAL_USB_MPS*4];
PRIVATE UINT8 g_UsbMode = 0;
void(*g_pfDisconnCb)(BOOL isHost)  = NULL;

// =============================================================================
// PRIVATE FUNCTION PROTOTYPE
// =============================================================================

PRIVATE        VOID hal_UsbConfigureEp(HAL_USB_EP_DESCRIPTOR_T* ep);
PRIVATE        VOID hal_UsbDecodeEp0Packet(VOID);
PRIVATE        VOID hal_getSetupPacket(VOID);
PRIVATE        VOID hal_UsbEnableEp(UINT8 ep, HAL_USB_EP_TYPE_T type);
PRIVATE        VOID hal_UsbDisableEp(UINT8 ep);
PRIVATE        VOID hal_UsbClrConfig(VOID);
INLINE         VOID hal_UsbStatusIn(VOID);
INLINE         VOID hal_UsbStatusOut(VOID);
INLINE         HAL_USB_CALLBACK_RETURN_T
hal_UsbCallbackEp(UINT8                      ep,
                  HAL_USB_CALLBACK_EP_TYPE_T type,
                  HAL_USB_SETUP_T*           setup);
INLINE         HAL_USB_CALLBACK_RETURN_T
hal_UsbCallbackInterface(UINT8                       ep,
                         HAL_USB_CALLBACK_EP_TYPE_T  type,
                         HAL_USB_SETUP_T*            setup);
PRIVATE UINT16 hal_generateDescConfig(HAL_USB_CONFIG_DESCRIPTOR_T* cfg,
                                      UINT8*                       buffer,
                                      UINT8                        num);
PRIVATE UINT16
hal_generateDescEp(HAL_USB_EP_DESCRIPTOR_T* ep,
                   UINT8*                   buffer);
PRIVATE UINT16
hal_generateDescInterface(HAL_USB_INTERFACE_DESCRIPTOR_T* interface,
                          UINT8*                          buffer,
                          UINT8                           num);

PRIVATE VOID hal_UsbCancelTransfert(UINT8 ep);

// =============================================================================
// hal_UsbFlushRxFifo
// -----------------------------------------------------------------------------
/// Flux reception USB fifo
// =============================================================================
PRIVATE VOID hal_UsbFlushRxFifo(VOID);

// =============================================================================
// hal_UsbFlushTxFifo
// -----------------------------------------------------------------------------
/// Flush transmition USB FIFO
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
// =============================================================================
PRIVATE VOID hal_UsbFlushTxFifo(UINT8 ep);

// =============================================================================
// hal_UsbFlushAllTxFifos
// -----------------------------------------------------------------------------
/// Flush all transmition USB FIFOS
// =============================================================================
PRIVATE VOID hal_UsbFlushAllTxFifos(VOID);

// =============================================================================
// hal_usbAsciiToUtf8
// -----------------------------------------------------------------------------
/// Convert string to utf8
/// @param utf8 destination string
/// @param acsii source string
// =============================================================================
PRIVATE VOID hal_usbAsciiToUtf8(UINT8 *utf8, UINT8 *acsii);

// =============================================================================
// PRIVATE FUNCTION
// =============================================================================

PRIVATE VOID hal_usbAsciiToUtf8(UINT8 *utf8, UINT8 *acsii)
{
    while(*acsii)
    {
        *utf8 = *acsii;
        utf8++;
        *utf8 = 0;
        utf8++;
        acsii++;
    }
}

PRIVATE VOID hal_UsbFlushRxFifo(VOID)
{
    UINT32 second_time,first_time;
    first_time = hal_TimGetUpTime();
    second_time = hal_TimGetUpTime();

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbFlushRxFifo);

    hwp_usbc->DCTL    |= USBC_SGOUTNAK;
    while((!(hwp_usbc->DCTL & USBC_GOUTNAKSTS)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
    {
        second_time = hal_TimGetUpTime();
    }

    hwp_usbc->GRSTCTL |= USBC_RXFFLSH;

    hwp_usbc->DCTL    |= USBC_CGOUTNAK;

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbFlushRxFifo);
}

PRIVATE VOID hal_UsbFlushTxFifo(UINT8 ep)
{
    UINT8 epNum;
    UINT32 second_time,first_time;
    first_time = hal_TimGetUpTime();
    second_time = hal_TimGetUpTime();

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbFlushTxFifo);

    epNum              = HAL_USB_EP_NUM(ep);
    hwp_usbc->DCTL    |= USBC_SGNPINNAK;
    while((!(hwp_usbc->DCTL & USBC_GNPINNAKSTS)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
    {
        second_time = hal_TimGetUpTime();
    }

    hwp_usbc->GRSTCTL  = USBC_TXFNUM(epNum) | USBC_TXFFLSH;

    hwp_usbc->DCTL    |= USBC_CGNPINNAK;

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbFlushTxFifo);
}

PRIVATE VOID hal_UsbFlushAllTxFifos(VOID)
{
    UINT32 second_time,first_time;
    first_time = hal_TimGetUpTime();
    second_time = hal_TimGetUpTime();

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbFlushAllTxFifos);

    hwp_usbc->DCTL    |= USBC_SGNPINNAK;
    while((!(hwp_usbc->DCTL & USBC_GNPINNAKSTS)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
    {
        second_time = hal_TimGetUpTime();
    }

    hwp_usbc->GRSTCTL  =  USBC_TXFNUM(0x10) | USBC_TXFFLSH;

    hwp_usbc->DCTL    |= USBC_CGNPINNAK;

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbFlushAllTxFifos);
}

PRIVATE VOID hal_UsbResetTransfert(VOID)
{
    UINT8 i;

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbResetTransfert);

    for(i = 0; i < DIEP_NUM+1; ++i)
    {
        g_HalUsbVar.InTransfert[i].sizeRemaining   = -1;
        g_HalUsbVar.InTransfert[i].sizeTransfered  =  0;
    }
    for(i = 0; i < DOEP_NUM+1; ++i)
    {
        g_HalUsbVar.OutTransfert[i].sizeRemaining  = -1;
        g_HalUsbVar.OutTransfert[i].sizeTransfered =  0;
    }

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbResetTransfert);
}

extern UINT8                        g_uaudIsoOutEp;
extern UINT8                        g_uaudIsoInEp;
PRIVATE INT32 hal_UsbGetCurrentTransfertSize(UINT8 ep)
{
    UINT8                epNum;
    INT32                sizeMax;
    INT32                size;
    HAL_USB_TRANSFERT_T* transfert;
    UINT32               nbPacket;

    epNum = HAL_USB_EP_NUM(ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        transfert = &g_HalUsbVar.InTransfert[epNum];

        // Select register for the EP and the max transfert size
        if(epNum == 0)
        {
            sizeMax = (HAL_USB_MPS*((1<< 3)-1) < ((1<< 7)-HAL_USB_MPS)) ? HAL_USB_MPS*((1<< 3)-1) : (1<< 7)-HAL_USB_MPS;
        }
        else
        {
            sizeMax = (HAL_USB_MPS*((1<<10)-1) < ((1<<19)-HAL_USB_MPS)) ? HAL_USB_MPS*((1<<10)-1) : (1<<19)-HAL_USB_MPS;
        }
    }
    else
    {
        transfert = &g_HalUsbVar.OutTransfert[epNum];

        // Select register for the EP
        if(epNum == 0)
        {
            sizeMax = (HAL_USB_MPS*((1<< 1)-1) < ((1<< 7)-HAL_USB_MPS)) ? HAL_USB_MPS*((1<< 1)-1) : (1<< 7)-HAL_USB_MPS;
        }
        else
        {
            sizeMax = (HAL_USB_MPS*((1<<10)-1) < ((1<<19)-HAL_USB_MPS)) ? HAL_USB_MPS*((1<<10)-1) : (1<<19)-HAL_USB_MPS;
        }
    }

    size = transfert->sizeRemaining;
    if(transfert->sizeRemaining > sizeMax)
    {
        size = sizeMax;
    }
#ifdef USB_AUDIO_SUPPORT
	if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
	{
            if(ep != g_uaudIsoInEp)//especial handle for audio input endpoint
            {
                if(size > HAL_USB_MPS)
                {
                    nbPacket = size     / HAL_USB_MPS;
                    size     = nbPacket * HAL_USB_MPS;
                }
            }
         }
        else
        {
            if(size > HAL_USB_MPS)
            {
                nbPacket = size     / HAL_USB_MPS;
                size     = nbPacket * HAL_USB_MPS;
            }
        }
#else
    if(size > HAL_USB_MPS)
    {
        nbPacket = size     / HAL_USB_MPS;
        size     = nbPacket * HAL_USB_MPS;
    }
#endif

    return size;
}

volatile UINT32 USB_AUDIO_LAST_TICK;
extern UINT8  g_uhidEp;

PRIVATE VOID hal_UsbNewTransfert(UINT8 ep)
{
    UINT8                epNum;
    REG32*               regSize;
    REG32*               regCtl;
    INT32                size;
    UINT32               nbPacket;

    epNum = HAL_USB_EP_NUM(ep);

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbNewTransfert);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        // Select register for the EP and the max transfert size
        if(epNum == 0)
        {
            regSize = &hwp_usbc->DIEPTSIZ0;
            regCtl  = &hwp_usbc->DIEPCTL0;
        }
        else
        {
            regSize = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPTSIZ;
            regCtl  = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        }
    }
    else
    {
        // Select register for the EP
        if(epNum == 0)
        {
            regSize = &hwp_usbc->DOEPTSIZ0;
            regCtl  = &hwp_usbc->DOEPCTL0;
        }
        else
        {
            regSize = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPTSIZ;
            regCtl  = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
        }
    }

    size = hal_UsbGetCurrentTransfertSize(ep);
    //hal_HstSendEvent(USB_EVENT,0xb0b00000);
    //hal_HstSendEvent(USB_EVENT,epNum);
    //hal_HstSendEvent(USB_EVENT,size);
    // Set size of DMA transfert
    if(size < HAL_USB_MPS)
    {
        nbPacket = 1;
    }
    else
    {
        nbPacket = size/HAL_USB_MPS;
    }
#ifdef USB_AUDIO_SUPPORT
	
//	hal_HstSendEvent(USB_EVENT,0x67676767);
//	hal_HstSendEvent(USB_EVENT,hal_UsbGetMode());
	if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
	{
	   *regSize  = USBC_OEPXFERSIZE(size) | USBC_OEPPKTCNT(1);
 //            hal_HstSendEvent(USB_EVENT,0x13051302);
//             hal_HstSendEvent(USB_EVENT,size);
	}
    else
#endif
    *regSize  = USBC_OEPXFERSIZE(size) | USBC_OEPPKTCNT(nbPacket);
    *regCtl  |= USBC_EPENA | USBC_CNAK;

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbNewTransfert);
}

PRIVATE UINT8 hal_UsbStartTransfert(UINT8 ep, VOID *data, UINT16 size,
                                    UINT32 flag)
{
    UINT8                epNum;
    REG32*               regDma;
    HAL_USB_TRANSFERT_T* transfert;

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbStartTransfert);

	UINT32 csStatus = hal_SysEnterCriticalSection();

    epNum = HAL_USB_EP_NUM(ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        // In (Device->PC)
        transfert = &g_HalUsbVar.InTransfert[epNum];

        // Select DMA register for the EP
        if(epNum == 0)
        {
            regDma  = &hwp_usbc->DIEPDMA0;
        }
        else
        {
            regDma  = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPDMA;
        }
    }
    else
    {
        // Out (PC->device)
#if 0
		HAL_ASSERT((size%HAL_USB_MPS) == 0,
			   "Out transfert (size = %i) is not a multiple "
			   "of Max Packet Size %i", size, HAL_USB_MPS);
#endif        

        transfert = &g_HalUsbVar.OutTransfert[epNum];

        // Select DMA register for the EP
        if(epNum == 0)
        {
            regDma  = &hwp_usbc->DOEPDMA0;
        }
        else
        {
            regDma  = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPDMA;
        }
    }

//    hal_HstSendEvent(USB_EVENT, 0x13032702);
//    hal_HstSendEvent(USB_EVENT, transfert->sizeRemaining);

    if(transfert->sizeRemaining != -1)
    {
        // EP already use for another transfert
        hal_SysExitCriticalSection(csStatus);
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbStartTransfert);
        return 1;
    }

    g_HalUsbVar.EpFlag &= ~(1<<epNum);
    if(flag)
    {
        g_HalUsbVar.EpFlag |= 1<<epNum;
    }

    HAL_USB_TRACE(HAL_USB_TRC, 0,
                  "Start transfert EP=%i size=%i addr=0x%08x\n",
                  ep, size, data);

    transfert->sizeRemaining  = size;
    transfert->sizeTransfered = 0;

    // Program DMA transfert
    *regDma  = (REG32) data;

    // Program the EP
    hal_UsbNewTransfert(ep);

	hal_SysExitCriticalSection(csStatus);

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbStartTransfert);

    return 0;
}

PRIVATE VOID hal_UsbCancelTransfert(UINT8 ep)
{
    UINT8               epNum;

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbCancelTransfert);

    epNum = HAL_USB_EP_NUM(ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        HAL_USB_TRACE(HAL_USB_TRC, 0,
                      "Cancel transfert EP%i IN",
                      epNum);
        g_HalUsbVar.InTransfert[epNum].sizeRemaining   = -1;
        g_HalUsbVar.InTransfert[epNum].sizeTransfered  =  0;
    }
    else
    {
        HAL_USB_TRACE(HAL_USB_TRC, 0,
                      "Cancel transfert EP%i OUT",
                      epNum);
        g_HalUsbVar.OutTransfert[epNum].sizeRemaining   = -1;
        g_HalUsbVar.OutTransfert[epNum].sizeTransfered  =  0;
    }

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbCancelTransfert);
}

extern UINT32 g_uAudActualPacketSize;
PRIVATE UINT8 hal_UsbContinueTransfert(UINT8 ep)
{
    UINT8                epNum;
    REG32*               regSize;
    REG32*               regCtl;
    HAL_USB_TRANSFERT_T* transfert;
    INT32                size;

    epNum = HAL_USB_EP_NUM(ep);

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbContinueTransfert);

	UINT32 csStatus = hal_SysEnterCriticalSection();

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        // In (Device->PC)

        transfert = &g_HalUsbVar.InTransfert[epNum];

        // Select register for the EP and the max transfert size
        if(epNum == 0)
        {
            regSize = &hwp_usbc->DIEPTSIZ0;
            regCtl  = &hwp_usbc->DIEPCTL0;
        }
        else
        {
            regSize = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPTSIZ;
            regCtl  = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        }
    }
    else
    {
        // Out (PC->device)

        transfert = &g_HalUsbVar.OutTransfert[epNum];

        // Select register for the EP
        if(epNum == 0)
        {
            regSize = &hwp_usbc->DOEPTSIZ0;
            regCtl  = &hwp_usbc->DOEPCTL0;
        }
        else
        {
            regSize = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPTSIZ;
            regCtl  = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
        }
    }

    if(transfert->sizeRemaining == -1)
    {
    	hal_SysExitCriticalSection(csStatus);
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbContinueTransfert);
        return 1;
    }

    size  = hal_UsbGetCurrentTransfertSize(ep);
//    hal_HstSendEvent(USB_EVENT, 0x09220001);
//    hal_HstSendEvent(USB_EVENT, size);
    size -= GET_BITFIELD(*regSize, USBC_OEPXFERSIZE);
//    hal_HstSendEvent(USB_EVENT, size);

    if(ep == g_uaudIsoOutEp)
    {
        g_uAudActualPacketSize = size;
    }

    transfert->sizeRemaining  -= size;
    transfert->sizeTransfered += size;

//    hal_HstSendEvent(USB_EVENT, 0x09210001);
//    hal_HstSendEvent(USB_EVENT, transfert->sizeRemaining);
//    hal_HstSendEvent(USB_EVENT, transfert->sizeTransfered);
//    hal_HstSendEvent(USB_EVENT, size);
    

    if(GET_BITFIELD(*regSize, USBC_OEPXFERSIZE) != 0)
    {
//        hal_HstSendEvent(USB_EVENT, 0x09210002);
        transfert->sizeRemaining = -1;
		hal_SysExitCriticalSection(csStatus);
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbContinueTransfert);
        return 1;
    }

    if(transfert->sizeRemaining == 0)
    {
 //        hal_HstSendEvent(USB_EVENT, 0x09210003);
        if(size != HAL_USB_MPS || !(g_HalUsbVar.EpFlag & (1<<epNum)) ||
           !HAL_USB_IS_EP_DIRECTION_IN(ep))
        {
  //          hal_HstSendEvent(USB_EVENT, 0x09210004);
            // End of transfert
            transfert->sizeRemaining = -1;
  			hal_SysExitCriticalSection(csStatus);
            HAL_PROFILE_FUNCTION_EXIT(hal_UsbContinueTransfert);
            return 1;
        }
    }

//    hal_HstSendEvent(USB_EVENT, 0x09210005);

    hal_UsbNewTransfert(ep);
	hal_SysExitCriticalSection(csStatus);

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbContinueTransfert);
    return 0;
}


PRIVATE VOID hal_UsbConfigureEp(HAL_USB_EP_DESCRIPTOR_T* ep)
{
    UINT8 epNum;

    epNum = HAL_USB_EP_NUM(ep->ep);

    hal_HstSendEvent(USB_EVENT, 0x11121005);
    hal_HstSendEvent(USB_EVENT, epNum);

    HAL_ASSERT(epNum < 16 && epNum != 0, "usbConfigure: Endpoint not valid");

    if(HAL_USB_IS_EP_DIRECTION_IN(ep->ep))
    {
        HAL_USB_TRACE(HAL_USB_TRC, 0, "Configure EP%i IN\n", epNum);
        g_HalUsbVar.EpInCallback[epNum-1]  = ep->callback;
        hal_UsbEnableEp(HAL_USB_EP_DIRECTION_IN(epNum), ep->type);
    }
    else
    {
        HAL_USB_TRACE(HAL_USB_TRC, 0, "Configure EP%i OUT\n", epNum);
        g_HalUsbVar.EpOutCallback[epNum-1] = ep->callback;
        hal_UsbEnableEp(HAL_USB_EP_DIRECTION_OUT(epNum), ep->type);
    }
}

// =============================================================================
// boot_UsbInit
// -----------------------------------------------------------------------------
/// (Re)Initializes the USB state machine, clear the IRQ and stop the transfers.
// =============================================================================
PRIVATE VOID hal_UsbInit(VOID)
{
    // Flush fifo
    hal_UsbFlushAllTxFifos();
    hal_UsbFlushRxFifo();

    // Reset EP0 state
    g_HalUsbVar.Ep0State        = EP0_STATE_IDLE;
    g_HalUsbVar.NbString        = 0;

    hal_UsbClrConfig();
    g_HalUsbVar.Desc            = 0;

    hal_UsbResetTransfert();
}

PRIVATE VOID hal_UsbConfig(VOID)
{
    UINT8                i;
    UINT16               addr;
    UINT8                dir;
    UINT32 second_time,first_time;
    first_time = hal_TimGetUpTime();
    second_time = hal_TimGetUpTime();

    // Nb EP
    g_HalUsbVar.NbEp = GET_BITFIELD(hwp_usbc->GHWCFG2, USBC_NUMDEVEPS);
    g_HalUsbVar.NbEp++;
    HAL_USB_TRACE(HAL_USB_TRC, 0, "NbEP %i\n", g_HalUsbVar.NbEp);

	UINT32 csStatus = hal_SysEnterCriticalSection();

    hwp_usbc->GRSTCTL   = USBC_CSFTRST;

	while((!(hwp_usbc->GRSTCTL&USBC_AHBIDLE)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
	{
		second_time = hal_TimGetUpTime();
	}

    if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
    {
        hwp_usbc->GRXFSIZ    = 48+22;
        addr                 = 48+22;

        // EP direction and Tx fifo configuration
        hwp_usbc->GNPTXFSIZ  = USBC_NPTXFSTADDR(addr) | USBC_NPTXFDEPS(32);
        addr                += 32;
        hwp_usbc->DIEPTXF[0].DIEnPTXF  = USBC_IENPNTXFSTADDR(addr) | USBC_INEPNTXFDEP(45);
                addr                            += 45;
  //       hwp_usbc->DIEPTXF[2].DIEnPTXF  = USBC_IENPNTXFSTADDR(addr) | USBC_INEPNTXFDEP(1);
#if 1
        for(i = 2; i < g_HalUsbVar.NbEp; i++)
        {
            dir = EPDIR(i);
            if(dir == EPIN || dir == EPINOUT)
            {
                hwp_usbc->DIEPTXF[i-1].DIEnPTXF  =
                    USBC_IENPNTXFSTADDR(addr) | USBC_INEPNTXFDEP(48);
                addr                            += 48;
            }
            HAL_USB_TRACE(HAL_USB_TRC, 0, "EP%i %i\n", i, dir);
        }
#endif
    }
    else
    {
        // Rx Fifo Size
        hwp_usbc->GRXFSIZ    = RXFIFOSIZE;
        addr                 = RXFIFOSIZE;

        // EP direction and Tx fifo configuration
        hwp_usbc->GNPTXFSIZ  = USBC_NPTXFSTADDR(addr) | USBC_NPTXFDEPS(TXFIFOSIZE);
        addr                += TXFIFOSIZE;

        for(i = 1; i < g_HalUsbVar.NbEp; i++)
        {
            dir = EPDIR(i);
            if(dir == EPIN || dir == EPINOUT)
            {
                hwp_usbc->DIEPTXF[i-1].DIEnPTXF  =
                    USBC_IENPNTXFSTADDR(addr) | USBC_INEPNTXFDEP(TXFIFOSIZE);
                addr                            += TXFIFOSIZE;
            }
            HAL_USB_TRACE(HAL_USB_TRC, 0, "EP%i %i\n", i, dir);
        }
    }

    // Enable DMA mode
    hwp_usbc->GAHBCFG  = USBC_DMAEN | USBC_HBSTLEN(HAL_USB_AHB_MODE);
    hwp_usbc->GAHBCFG |= USBC_GLBLINTRMSK;
    hwp_usbc->GUSBCFG |= USBC_PHYIF | USBC_USBTRDTIM(5);

    HAL_USB_TRACE(HAL_USB_TRC, 0, "Base config\n");

    hwp_usbc->DCFG    &= ~(USBC_DEVSPD_MASK  | USBC_PERFRINT_MASK);
   // hwp_usbc->DCFG    &= ~(USBC_DEVSPD_MASK  | (0<<11));

    // Configure FS USB 1.1 Phy
    hwp_usbc->DCFG    |= USBC_DEVSPD(3);
#ifdef USB_AUDIO_SUPPORT
//	hal_HstSendEvent(USB_EVENT,0x67676767);
//	hal_HstSendEvent(USB_EVENT,hal_UsbGetMode());
    if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
    {
    hwp_usbc->GINTMSK |= USBC_USBRST | USBC_ENUMDONE|USBC_SOF
        | USBC_ERLYSUSP | USBC_USBSUSP|USBC_ISOOUTDROP|USBC_INCOMPLSOIN|USBC_INCOMPIP;
   	   //open ISO thread
       hwp_usbc->DTHRCTL |= USBC_ISOTHREN;
       hwp_usbc->DTHRCTL |= USBC_NONISOTHREN;
    }
	else
#endif
	{
	   hwp_usbc->GINTMSK |= USBC_USBRST | USBC_ENUMDONE
           | USBC_ERLYSUSP | USBC_USBSUSP;   
	}
	
	//open ISO thread
    hwp_usbc->DTHRCTL |= USBC_ISOTHREN;
    hwp_usbc->DTHRCTL |= USBC_NONISOTHREN;

    hal_UsbFlushRxFifo();
    hal_UsbFlushAllTxFifos();
    //hal_HstSendEvent(USB_EVENT,0x04260050);
    //hal_HstSendEvent(USB_EVENT,hwp_usbc->GINTMSK);
	hal_SysExitCriticalSection(csStatus);
}

// =============================================================================
// hal_UsbDisableEp
// -----------------------------------------------------------------------------
/// Disable an endpoint.
///
/// Please note that there is a particular behaviour for endpoint 0.
/// @param ep Endpoint to disable.
// ============================================================================= 
PRIVATE VOID hal_UsbDisableEp(UINT8 ep)
{
    UINT8   epNum;
    UINT8   offset;
    REG32*  diepctl;
    REG32*  doepctl;
    REG32*  ctl;
    REG32*  diepint;
    REG32*  doepint;
    REG32*  epint;
    UINT32 second_time,first_time;
    first_time = hal_TimGetUpTime();


    HAL_PROFILE_FUNCTION_ENTER(hal_UsbDisableEp);

    epNum = HAL_USB_EP_NUM(ep);
    HAL_ASSERT(epNum < 16, "hal_UsbDisableEp: Ep number invalid");

    // Select ctl register
    if(epNum == 0)
    {
        diepctl   = &hwp_usbc->DIEPCTL0;
        doepctl   = &hwp_usbc->DOEPCTL0;
        diepint   = &hwp_usbc->DIEPINT0;
        doepint   = &hwp_usbc->DOEPINT0;
    }
    else
    {
        diepctl   = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        doepctl   = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
        diepint   = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPINT;
        doepint   = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPINT;
    }

    // Select direction
    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        offset   = 0;
        ctl      = diepctl;
        epint    = diepint;
    }
    else
    {
        offset   = 16;
        ctl      = doepctl;
        epint    = doepint;
    }

    // Clear the interruption cause, as we don't care
    // about the past anymore.
    if(epNum == 0)
    {
        // EP0 must always be enabled, so we don't stop
        // its transfer.
        if(*ctl & USBC_EPENA)
        {
            *ctl   = USBC_EPDIS | USBC_SNAK;
        }
        else
        {
            // set nake on EP
            *ctl   = USBC_SNAK;
        }
        *epint = USBC_XFERCOMPL | USBC_SETUPMK | USBC_TIMEOUT;
    }
    else
    {
        if(*ctl & USBC_EPENA)
        {
            // Stop current transfert
            if(HAL_USB_IS_EP_DIRECTION_IN(ep))
            {
                second_time = hal_TimGetUpTime();
                while((!(*diepint & USBC_INEPNAKEFF)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
                {
                    *diepctl |= USBC_SNAK;
                    second_time = hal_TimGetUpTime();
                }
            }
            else
            {
                second_time = hal_TimGetUpTime();
                while((!(hwp_usbc->GINTSTS & USBC_GOUTNAKEFF)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
                {
                    hwp_usbc->DCTL  |= USBC_SGOUTNAK;
                    second_time = hal_TimGetUpTime();
                }
            }
            *ctl             = USBC_EPDIS | USBC_SNAK;
            second_time = hal_TimGetUpTime();
            while((!(*epint & USBC_EPDISBLD)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
            {
                    second_time = hal_TimGetUpTime();
            }
            *epint           = USBC_EPDISBLD;

            if(HAL_USB_IS_EP_DIRECTION_IN(ep))
            {
                hal_UsbFlushTxFifo(epNum);
            }
            else
            {
                hwp_usbc->DCTL  |= USBC_CGOUTNAK;
            }
        }
        else
        {
            // set nake on EP
            *ctl             = USBC_SNAK;
        }
    }

    hal_UsbFlushTxFifo(epNum);

    hal_UsbCancelTransfert(ep);

    // We don't disable EP0 interrupt as it is driving the USB
    // stuff. Closing the USB will disable them though.
    if (epNum != 0)
    {
        // Disable interrupt mask for this EP
        hwp_usbc->DAINTMSK          &= ~(1<<(epNum+offset));
        g_HalUsbVar.EpInterruptMask &= ~(1<<(epNum+offset));
    }

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbDisableEp);
}

PRIVATE VOID hal_UsbEnableEp(UINT8 ep, HAL_USB_EP_TYPE_T type)
{
    UINT8   epNum;
    UINT8   offset;
    REG32*  diepctl;
    REG32*  doepctl;
    REG32*  ctl;
    REG32*  diepint;
    REG32*  doepint;
    REG32*  epint;

    epNum = HAL_USB_EP_NUM(ep);
    HAL_ASSERT(epNum < 16, "hal_UsbEnableEp: Ep number invalid");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbEnableEp);

	UINT32 csStatus = hal_SysEnterCriticalSection();

    // Select ctl register
    if(epNum == 0)
    {
        diepctl = &hwp_usbc->DIEPCTL0;
        doepctl = &hwp_usbc->DOEPCTL0;
        diepint = &hwp_usbc->DIEPINT0;
        doepint = &hwp_usbc->DOEPINT0;
    }
    else
    {
        diepctl = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        doepctl = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
        diepint = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPINT;
        doepint = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPINT;
    }

    // Select direction
    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        offset = 0;
        ctl    = diepctl;
        epint  = diepint;
    }
    else
    {
        offset = 16;
        ctl    = doepctl;
        epint  = doepint;
    }

    // Clear pending interrupt
    *epint = *epint;

/*     if(*ctl & USBC_USBACTEP) */
/*     { */
        //add this if branch for audio,cause audio packet could be larger than HAL_USB_MPS,
        //add there is some special configuration.
        if(type == HAL_USB_EP_TYPE_ISO)
        {
         
         if(HAL_USB_IS_EP_DIRECTION_IN(ep))
         {
           *ctl = USBC_EPN_MPS(HAL_USB_ISO_IN_MPS) | USBC_EPTYPE(type) | USBC_USBACTEP
                | USBC_EPTXFNUM(epNum) |  USBC_NEXTEP(epNum);
         }
         else
         {
            *ctl = USBC_EPN_MPS(HAL_USB_ISO_OUT_MPS) | USBC_EPTYPE(type) | USBC_USBACTEP
                | USBC_EPTXFNUM(epNum)| USBC_SETD0PID|  USBC_NEXTEP(epNum);
         }
        }
#if 0
		else if(epNum == 3)
		{
            *ctl = USBC_EPN_MPS(HAL_USB_MPS) | USBC_EPTYPE(type) | USBC_USBACTEP
                | USBC_EPTXFNUM(0) | USBC_SETD0PID | USBC_NEXTEP(epNum);
		}
#endif
		else
        {
            *ctl = USBC_EPN_MPS(HAL_USB_MPS) | USBC_EPTYPE(type) | USBC_USBACTEP
//                | USBC_EPTXFNUM(0) | USBC_SETD0PID | USBC_NEXTEP(epNum);
			| USBC_EPTXFNUM(epNum) | USBC_SETD0PID | USBC_NEXTEP(epNum);
        }
/*     } */

    // Enable interrupt mask for this EP
    hwp_usbc->DAINTMSK          |= (1<<(epNum+offset));
    g_HalUsbVar.EpInterruptMask |= (1<<(epNum+offset));

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbEnableEp);

    hal_SysExitCriticalSection(csStatus);

}

INLINE         UINT8 hal_UsbAddString(UINT8 *str)
{
    UINT8 i;

    if(str == 0)
    {
        return(0);
    }
    for(i = 0; i < g_HalUsbVar.NbString; ++i)
    {
        if(g_HalUsbVar.String[i] == str)
        {
            return(i + 1);
        }
    }
    HAL_ASSERT(g_HalUsbVar.NbString < USB_MAX_STRING,
               "Usb: Too string descriptor defined");
    g_HalUsbVar.String[g_HalUsbVar.NbString] = str;
    g_HalUsbVar.NbString++;
    return(g_HalUsbVar.NbString);
}

PRIVATE VOID hal_generateDescDevice(HAL_USB_DEVICE_DESCRIPTOR_T* dev,
                                    UINT8*                       buffer)
{
    HAL_USB_DEVICE_DESCRIPTOR_REAL_T* desc;
    UINT8                             i;

    desc = (HAL_USB_DEVICE_DESCRIPTOR_REAL_T*) buffer;
    desc->size          = sizeof(HAL_USB_DEVICE_DESCRIPTOR_REAL_T);
    // Device type = 1
    desc->type          = 1;
    desc->bcdUsb        = 0x0110;
    desc->usbClass      = dev->usbClass;
    desc->usbSubClass   = dev->usbSubClass;
    desc->usbProto      = dev->usbProto;
    desc->ep0Mps        = HAL_USB_MPS;
    if(dev->vendor == 0)
    {
        desc->vendor    = BOOT_USB_VENDOR_ID;
    }
    else
    {
        desc->vendor    = dev->vendor;
    }
    if(dev->product == 0)
    {
        desc->product   = BOOT_USB_PRODUCT_ID(dev->usbMode);
    }
    else
    {
        desc->product   = dev->product;
    }
    if(dev->version == 0)
    {
        desc->release   = BOOT_USB_REVISION_ID;
    }
    else
    {
        desc->release   = dev->version;
    }
    desc->iManufacturer = hal_UsbAddString((UINT8*)"RDA Microelectronics");
    desc->iProduct      = hal_UsbAddString(dev->description);
    desc->iSerial       = hal_UsbAddString(dev->serialNumber);
    i = 0;
    if(dev->configList)
    {
        for(i = 0; dev->configList[i]; ++i);
    }
    desc->nbConfig             = i;
}

PRIVATE UINT16 hal_generateDescConfig(HAL_USB_CONFIG_DESCRIPTOR_T* cfg,
                                      UINT8*                       buffer,
                                      UINT8                        num)
{

    UINT32                            i;
    HAL_USB_CONFIG_DESCRIPTOR_REAL_T* desc;
    HAL_USBA_CONFIG_DESCRIPTOR_REAL_T* descA;
    UINT16                            size;	
//	hal_HstSendEvent(USB_EVENT,0x67676767);
//	hal_HstSendEvent(USB_EVENT,hal_UsbGetMode());
#ifdef USB_AUDIO_SUPPORT

	if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
	{
       descA = (HAL_USBA_CONFIG_DESCRIPTOR_REAL_T*) buffer;
       descA->size         = sizeof(HAL_USBA_CONFIG_DESCRIPTOR_REAL_T);
       size               = sizeof(HAL_USBA_CONFIG_DESCRIPTOR_REAL_T);
       descA->type         = 2;
       descA->nbInterface  = 0;
    	if(cfg->iad != NULL)
    	{
    		memcpy(&buffer[size],cfg->iad,sizeof(HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T));
    		size += sizeof(HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T);
    	}
       if(cfg->interfaceList)
       {
           for(i = 0; cfg->interfaceList[i]; ++i)
           {
               size += hal_generateDescInterface(cfg->interfaceList[i],
                                                 &buffer[size], descA->nbInterface);
            if((cfg->interfaceList[i]->interfaceIdx != 0xFF)&&
            	((cfg->interfaceList[i]->interfaceIdx != 0xF0))
              )
            {	 
                  ++descA->nbInterface;
               }
           }
       }
   	   descA->bConfigurationValue = 0x01;
       descA->configIndex  = cfg->configIdx;
       descA->attrib       = cfg->attrib;
       descA->maxPower     = cfg->maxPower;
       descA->totalLength  = size;
       HAL_USB_TRACE(HAL_USB_TRC, 0, "Configuration descriptor size %i\n", size);
       HAL_ASSERT(size <= sizeof(g_HalUsbBufferEp0In), "Configuration descriptor too big %i", size);

       hal_HstSendEvent(USB_EVENT, 0x11060001);
       return(size);
	}
	else
#endif
	{
		desc = (HAL_USB_CONFIG_DESCRIPTOR_REAL_T*) buffer;
		desc->size		   = sizeof(HAL_USB_CONFIG_DESCRIPTOR_REAL_T);
		size			   = sizeof(HAL_USB_CONFIG_DESCRIPTOR_REAL_T);
		desc->type		   = 2;
		desc->nbInterface  = 0;
		if(cfg->iad != NULL)
		{
			memcpy(&buffer[size],cfg->iad,sizeof(HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T));
			size += sizeof(HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T);
		}
		if(cfg->interfaceList)
		{
			for(i = 0; cfg->interfaceList[i]; ++i)
			{
				size += hal_generateDescInterface(cfg->interfaceList[i],
												  &buffer[size], desc->nbInterface);
				if(cfg->interfaceList[i]->interfaceIdx != 0xFF)
				{
                    ++desc->nbInterface;
                }
            }
        }
        desc->configIndex= cfg->configIdx;
        desc->iDescription = hal_UsbAddString(cfg->description);
        desc->attrib       = cfg->attrib;
        desc->maxPower     = cfg->maxPower;
        desc->totalLength  = size;

        HAL_USB_TRACE(HAL_USB_TRC, 0, "Configuration descriptor size %i\n", size);

        HAL_ASSERT(size <= sizeof(g_HalUsbBufferEp0In), "Configuration descriptor too big %i", size);

		return(size);
	}
}

PRIVATE UINT16
hal_generateDescInterface(HAL_USB_INTERFACE_DESCRIPTOR_T* interface,
                          UINT8*                          buffer,
                          UINT8                           num)
{
    UINT8                                j;
    HAL_USB_INTERFACE_DESCRIPTOR_REAL_T* desc;
    UINT16                               size;
    HAL_USB_CS_INTERFACE_DESCRIPTOR_T*   csDesc;

    csDesc = (HAL_USB_CS_INTERFACE_DESCRIPTOR_T*)   interface;
    desc   = (HAL_USB_INTERFACE_DESCRIPTOR_REAL_T*) buffer;
//    static UINT8 alternateSetting = 0;
    if(interface->interfaceIdx == 0xFF)
    { // CS_INTERFACE,and other descriptor
        size                    = csDesc->constructor(buffer);
    }
    else
    {
        desc->size              = sizeof(HAL_USB_INTERFACE_DESCRIPTOR_REAL_T);
        size                    = sizeof(HAL_USB_INTERFACE_DESCRIPTOR_REAL_T);
        // Interface type = 4
        desc->type              = 4;
        desc->interfaceIndex    = num;

#ifndef USB_AUDIO_SUPPORT      
      desc->alternateSetting = 0;      
#else
      //for Audio Class
      if(interface->interfaceIdx == 0x0F)
      {
         desc->alternateSetting = 1;
      }
      else
      {
         desc->alternateSetting = 0;
      }
#endif
      //for HID interface     
      if(interface->usbClass == 0x03)
      {
    	  desc->nbEp			  = 1;
      }
      else
      {
         j = 0;
    	     //hal_HstSendEvent(0x19920708);
         if(interface->epList)
         {
            for(j = 0; interface->epList[j]; ++j)
            {
               size += hal_generateDescEp(interface->epList[j],&buffer[size]);
               hal_HstSendEvent(USB_EVENT, 0x11121002);
            }
         }
    	 desc->nbEp 			 = j;
    	 hal_HstSendEvent(USB_EVENT, 0x11121003);
    	 hal_HstSendEvent(USB_EVENT, j);
      }
        desc->usbClass          = interface->usbClass   ;
        desc->usbSubClass       = interface->usbSubClass;
        desc->usbProto          = interface->usbProto   ;
        desc->iDescription      = hal_UsbAddString(interface->description);
    }

    return(size);
}

PRIVATE UINT16
hal_generateDescEp(HAL_USB_EP_DESCRIPTOR_T* ep,
                   UINT8*                   buffer)
{
   HAL_USB_EP_DESCRIPTOR_REAL_T* desc;
   HAL_USB_ISO_EP_DESCRIPTOR_REAL_T* descIso;
   if(ep->type == HAL_USB_EP_TYPE_ISO)
   {
      descIso = (HAL_USB_ISO_EP_DESCRIPTOR_REAL_T*) buffer;
      descIso->size       = sizeof(HAL_USB_ISO_EP_DESCRIPTOR_REAL_T);
      descIso->attributes = ep->type|0x00;	  
      descIso->type       = 5;
      descIso->ep         = ep->ep;
      hal_HstSendEvent(USB_EVENT, 0x11130003);
      hal_HstSendEvent(USB_EVENT, ep->ep);
	  
      if(HAL_USB_IS_EP_DIRECTION_IN(ep->ep))
      {
         descIso->mps = HAL_USB_ISO_IN_MPS;
      }
      else
      {
         descIso->mps = HAL_USB_ISO_OUT_MPS;
      }
      descIso->interval = ep->interval;
      descIso->bRefreash = 0x00;
      descIso->bSynchAddr = 0x00;
      return (descIso->size);

   }
   else
   {
      desc = (HAL_USB_EP_DESCRIPTOR_REAL_T*) buffer;     
      desc->size       = sizeof(HAL_USB_EP_DESCRIPTOR_REAL_T);
      // EP type = 5
      desc->type       = 5;
      desc->ep         = ep->ep;
      hal_HstSendEvent(USB_EVENT, 0x11130004);
      hal_HstSendEvent(USB_EVENT, ep->ep);
      desc->attributes = ep->type;
      desc->mps        = HAL_USB_MPS;
      desc->interval   = ep->interval;
      // EP Type (Cmd, Bulk, Iso, Int)
      return(desc->size);
   }
}

PRIVATE VOID hal_getSetupPacket(VOID)
{
    // Enable EP0 to receive a new setup packet

    HAL_USB_TRACE(HAL_USB_TRC, 0, "Hal USB Setup\n");
    hwp_usbc->DOEPTSIZ0  =  USBC_SUPCNT(3) |
        USBC_OEPXFERSIZE0(8) | USBC_OEPPKTCNT0;
    hwp_usbc->DOEPDMA0   =  (UINT32) g_HalUsbBufferEp0Out;
    hwp_usbc->DOEPCTL0  |= USBC_CNAK | USBC_EPENA;
}

INLINE         VOID hal_UsbStatusIn(VOID)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_UsbStatusIn);

    // Enable EP0 to receive a status IN

    g_HalUsbVar.Ep0State = EP0_STATE_STATUS_IN;
    hal_UsbSend(0, g_HalUsbBufferEp0In, 0, 0);
//    hal_getSetupPacket();

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbStatusIn);
}

INLINE         VOID hal_UsbStatusOut(VOID)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_UsbStatusOut);

    // Enable EP0 to receive a status OUT

    /* NEED use hwp_usbc->DCFG |= USBC_NZSTSOUTHSHK; for set negative status */
    g_HalUsbVar.Ep0State = EP0_STATE_STATUS_OUT;
    hal_UsbRecv(0, g_HalUsbBufferEp0Out, 0, 0);

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbStatusOut);
}

// =============================================================================
// hal_UsbClrConfig
// -----------------------------------------------------------------------------
/// Clear the USB config. Stop the transfer on the endpoint (but ep0) and clear
/// the interrupts (but ep0).
// =============================================================================
PRIVATE VOID hal_UsbClrConfig(VOID)
{
    UINT8 i;

    if(g_HalUsbVar.Desc)
    {
        if(g_HalUsbVar.Config != 0xFF &&
           g_HalUsbVar.Desc->configList[g_HalUsbVar.Config] &&
           g_HalUsbVar.Desc->configList[g_HalUsbVar.Config]->interfaceList)
        {
            for(i = 0;
                g_HalUsbVar.Desc->configList[g_HalUsbVar.Config]
                    ->interfaceList[i];
                ++i)
            {
                if(g_HalUsbVar.Desc->configList[g_HalUsbVar.Config]
                   ->interfaceList[i]->callback)
                {
                    g_HalUsbVar.Desc->configList[g_HalUsbVar.Config]
                        ->interfaceList[i]->callback
                        (HAL_USB_CALLBACK_TYPE_DISABLE, 0);
                }
            }
        }
        g_HalUsbVar.Config = 0xFF;
    }

    // Disable all EP
    for(i = 0; i < DIEP_NUM; ++i)
    {
        g_HalUsbVar.EpInCallback[i]  = 0;
        hal_UsbDisableEp(HAL_USB_EP_DIRECTION_IN (i+1));
    }
    for(i = 0; i < DOEP_NUM; ++i)
    {
        g_HalUsbVar.EpOutCallback[i] = 0;
        hal_UsbDisableEp(HAL_USB_EP_DIRECTION_OUT(i+1));
    }

    // Endpoint 0 doesn't have endpoint callbacks other
    // than the IRQ handlers...
    hal_UsbDisableEp(HAL_USB_EP_DIRECTION_IN(0));
    hal_UsbDisableEp(HAL_USB_EP_DIRECTION_OUT(0));
}

INLINE         HAL_USB_CALLBACK_RETURN_T
hal_UsbCallbackEp(UINT8                      ep,
                  HAL_USB_CALLBACK_EP_TYPE_T type,
                  HAL_USB_SETUP_T*           setup)
{
    UINT8                     epNum;

    epNum = HAL_USB_EP_NUM(ep);

    if(epNum < 16 && epNum != 0)
    {
        if(HAL_USB_IS_EP_DIRECTION_IN(ep))
        {
            HAL_USB_TRACE(HAL_USB_TRC, 0, "CallBackEp IN EP%i\n", epNum);
            if(g_HalUsbVar.EpInCallback[epNum-1])
            {
                return(g_HalUsbVar.EpInCallback[epNum-1](type, setup));
            }
        }
        else
        {
            HAL_USB_TRACE(HAL_USB_TRC, 0, "CallBackEp OUT EP%i\n", epNum);
            if(g_HalUsbVar.EpOutCallback[epNum-1])
            {
              //  hal_HstSendEvent(USB_EVENT, 0x0925000e);
              //  hal_HstSendEvent(USB_EVENT, g_HalUsbVar.EpOutCallback[epNum-1]);
                return(g_HalUsbVar.EpOutCallback[epNum-1](type, setup));
            }
        }
    }
    
    return(HAL_USB_CALLBACK_RETURN_KO);
}

INLINE         HAL_USB_CALLBACK_RETURN_T
hal_UsbCallbackInterface(UINT8                      interface,
                         HAL_USB_CALLBACK_EP_TYPE_T type,
                         HAL_USB_SETUP_T*           setup)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T* interfaceDesc = 0;
    UINT32                          i;
    UINT32                          nonCsItfIdx = 0;

    if(g_HalUsbVar.Desc == 0)
    {
        return HAL_USB_CALLBACK_RETURN_KO;
    }
    if(g_HalUsbVar.Config == 0xFF)
    {
        return HAL_USB_CALLBACK_RETURN_KO;
    }
    if(g_HalUsbVar.Desc->configList[g_HalUsbVar.Config] == 0)
    {
        return HAL_USB_CALLBACK_RETURN_KO;
    }
    for(i = 0;
        (interfaceDesc = g_HalUsbVar.Desc->configList[g_HalUsbVar.Config]->interfaceList[i]);
        ++i)
    {
        if((interfaceDesc->interfaceIdx != 0xFF)&&(interfaceDesc->interfaceIdx != 0xF0))
        {
            if(nonCsItfIdx == interface)
            {
                break;
            }
            nonCsItfIdx++;
        }
    }
    if(interfaceDesc == 0)
    {
        return HAL_USB_CALLBACK_RETURN_KO;
    }
    if(interfaceDesc->callback == 0)
    {
        return HAL_USB_CALLBACK_RETURN_KO;
    }
    return interfaceDesc->callback(type, setup);
}

PRIVATE VOID hal_UsbSetConfig(UINT8 num)
{
    UINT8 i = 0;
    UINT8 j;

    // Disable old config
    hal_UsbClrConfig();

    g_HalUsbVar.Config      = 0xFF;
    g_HalUsbVar.NbInterface = 0;

    // Search config index
//	hal_HstSendEvent(USB_EVENT,0x67676767);
#ifdef USB_AUDIO_SUPPORT
	if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
	{
	    for(i = 0; g_HalUsbVar.Desc->configList[i]; i++) 
	    {
           if(g_HalUsbVar.Desc->configList[i]->configIdx == (num-1))
           {            
			   hal_HstSendEvent(USB_EVENT,0xfafa0006);
               break;
           }
        }
	}
	else
#endif
	{
	    for(i = 0; g_HalUsbVar.Desc->configList[i]; ++i) 
	    {
           if(g_HalUsbVar.Desc->configList[i]->configIdx == num)
           {            
			   hal_HstSendEvent(USB_EVENT,0xfafa0006);
			   hal_HstSendEvent(USB_EVENT,i);
			   hal_HstSendEvent(USB_EVENT,num);
               break;
           }
        }
	}
    //hal_HstSendEvent(USB_EVENT,0xfafa0001);
    //hal_HstSendEvent(USB_EVENT,i);
    if(g_HalUsbVar.Desc->configList[i] == 0)
    {
        return;
    }
    // Save config index
    g_HalUsbVar.Config = i;
    num                = i;

    // Configure all interface end EP of this config
    if(g_HalUsbVar.Desc->configList[num]->interfaceList)
    {
        for(i = 0; g_HalUsbVar.Desc->configList[num]->interfaceList[i]; ++i)
        {
            if(g_HalUsbVar.Desc->configList[num]->interfaceList[i]->interfaceIdx != 0xFF)
            {
//				hal_HstSendEvent(USB_EVENT,0x67676767);
//				hal_HstSendEvent(USB_EVENT,hal_UsbGetMode());
				if(g_HalUsbVar.Desc->configList[num]->interfaceList[i]->epList)
				{
                      for(j = 0;
                      g_HalUsbVar.Desc->configList[num]->interfaceList[i]->epList[j];
                      ++j)
                      {                       
                         hal_UsbConfigureEp(g_HalUsbVar.Desc->configList[num]->
                         interfaceList[i]->epList[j]);
                         hal_HstSendEvent(USB_EVENT,0x11121001);
				        // hal_HstSendEvent(USB_EVENT,hal_UsbGetMode());
                      }
                }
            }
            if(g_HalUsbVar.Desc->configList[num]->interfaceList[i]->callback)
            {
                g_HalUsbVar.Desc->configList[num]->
                    interfaceList[i]->callback(HAL_USB_CALLBACK_TYPE_ENABLE, 0);
            }
        }
        g_HalUsbVar.NbInterface = i;
    }
    hal_HstSendEvent(USB_EVENT, 0x11060002);    
}

PRIVATE VOID hal_UsbDecodeEp0Packet(VOID)
{
    HAL_USB_SETUP_T* setup;
    UINT16           size;
    UINT8            setup_completed = 0;

    setup =
        (HAL_USB_SETUP_T*) HAL_SYS_GET_UNCACHED_ADDR(g_HalUsbBufferEp0Out);
   if(setup->requestDesc.requestType)
   {
      //hal_HstSendEvent(USB_EVENT,0xfcfcfcfc);
   }
#if 0
   hal_HstSendEvent(USB_EVENT, 0x11090004);
   hal_HstSendEvent(USB_EVENT, g_HalUsbVar.Ep0State);
   hal_HstSendEvent(USB_EVENT, setup->requestDesc);
   hal_HstSendEvent(USB_EVENT, setup->request);
   hal_HstSendEvent(USB_EVENT, setup->value);
   hal_HstSendEvent(USB_EVENT, setup->index);
   hal_HstSendEvent(USB_EVENT, setup->lenght);
#endif
	UINT32 csStatus = hal_SysEnterCriticalSection();

    switch(g_HalUsbVar.Ep0State)
    {
    case EP0_STATE_IDLE  :
        // New setup packet

        // Change endian less
        setup->value     = HAL_ENDIAN_LITTLE_16(setup->value );
        setup->index     = HAL_ENDIAN_LITTLE_16(setup->index );
        setup->lenght    = HAL_ENDIAN_LITTLE_16(setup->lenght);

        g_HalUsbVar.RequestDesc = setup->requestDesc;
        g_HalUsbVar.Ep0Index    = setup->index&0xff;

        HAL_USB_TRACE(HAL_USB_TRC, 0, "Setup %02x %02x %04x %04x %04x\n",
                  setup->requestDesc, setup->request,
                  setup->value, setup->index, setup->lenght);
        if(setup->lenght == 0)
        {
            // No data
            if(setup->requestDesc.requestDirection)
            {
                g_HalUsbVar.Ep0State = EP0_STATE_STATUS_OUT;
            }
            else
            {
                g_HalUsbVar.Ep0State = EP0_STATE_STATUS_IN;
            }
        }
        else
        {
            // Command with data
            if(setup->requestDesc.requestDirection)
            {
                g_HalUsbVar.Ep0State = EP0_STATE_IN;
            }
            else
            {
                g_HalUsbVar.Ep0State = EP0_STATE_OUT;
            }
        }

        switch(setup->requestDesc.requestDest)
        {
        case HAL_USB_REQUEST_DESTINATION_DEVICE:
            switch(setup->request)
            {
            case HAL_USB_REQUEST_DEVICE_SETADDR:
                // Set device addr
                hwp_usbc->DCFG |= USBC_DEVADDR(setup->value);
                HAL_USB_TRACE(HAL_USB_TRC, 0, "Set addr 0x%08x\n",setup->value);
                setup_completed = 1;
                break;
            case HAL_USB_REQUEST_DEVICE_SETCONF:
                // Select one config
                if(g_HalUsbVar.Desc == 0)
                {
                    break;
                }
                HAL_USB_TRACE(HAL_USB_TRC, 0, "Set config\n");

//                 if((setup->value&0xFF)  <= g_HalUsbVar.NbConfig)
//                 {
                    setup_completed = 1;
                    hal_UsbSetConfig(setup->value&0xFF);
                    hal_HstSendEvent(USB_EVENT, 0x11150001);
                    HAL_USB_TRACE(HAL_USB_TRC, 0, "Set config %i OK\n",
                                  setup->value&0xFF);

//                 }
                break;
            case HAL_USB_REQUEST_DEVICE_GETDESC:
                // Fetch device descriptor                

                if(g_HalUsbVar.Desc == 0)
                {
                    break;
                }
                size = 0;
                switch(setup->value>>8)
                {
                case 1: /* Device */
                    HAL_USB_TRACE(HAL_USB_TRC, 0, "Device desc\n");
                    hal_generateDescDevice(g_HalUsbVar.Desc,
                                           g_HalUsbBufferEp0In);
                    size = sizeof(HAL_USB_DEVICE_DESCRIPTOR_REAL_T);
                  //  hal_HstSendEvent(USB_EVENT, 0x11090003);
                    break;
                case 2: /* Config */
                    HAL_USB_TRACE(HAL_USB_TRC, 0, "Config desc %i\n",
                              setup->value&0xFF);
                 //   hal_HstSendEvent(USB_EVENT, 0x11024440);
                  //  hal_HstSendEvent(USB_EVENT, setup->value);
                 //   hal_HstSendEvent(USB_EVENT, setup->lenght);
                    size = hal_generateDescConfig(
                        g_HalUsbVar.Desc->configList[(setup->value&0xFF)],
                        g_HalUsbBufferEp0In, setup->value&0xFF);
                 //   hal_HstSendEvent(USB_EVENT, 0x11130006);
                    hal_HstSendEvent(USB_EVENT, size);
                    break;
                case 3: /* String */
                    HAL_USB_TRACE(HAL_USB_TRC, 0, "String desc %i %i\n",
                                  (setup->value&0xFF), g_HalUsbVar.NbString);
                    if((setup->value&0xFF) == 0)
                    {
                        // Set caracter table to 0x0409
                        size                   = 0x04;
                        g_HalUsbBufferEp0In[0] = 0x04;
                        g_HalUsbBufferEp0In[1] = 0x03;
                        g_HalUsbBufferEp0In[2] = 0x09;
                        g_HalUsbBufferEp0In[3] = 0x04;
                    }
                    else
                    {
                        // Select the string and send it
                        size = 0;
                        if((setup->value&0xFF) <= g_HalUsbVar.NbString)
                        {
                                     //hal_HstSendEvent(USB_EVENT,0xfcfc1337);
                                     HAL_USB_TRACE(HAL_USB_TRC, 0,"String desc %i %i\n", (setup->value&0xFF), g_HalUsbVar.NbString);
                                     size = strlen((INT8 *)g_HalUsbVar.String[(setup->value&0xFF)-1])*2 + 2;
                            g_HalUsbBufferEp0In[0] = size;
                            g_HalUsbBufferEp0In[1] = 0x03;
                                     hal_usbAsciiToUtf8(&g_HalUsbBufferEp0In[2],g_HalUsbVar.String[(setup->value&0xFF)-1]);
                        }
                    }
                    break;
                }
                if(setup->lenght < size)
                {
                    size = setup->lenght;
                }
                HAL_USB_TRACE(HAL_USB_TRC, 0, "Get descriptor %i\n", size);
                /* Data in */
                hal_UsbSend(0, g_HalUsbBufferEp0In, size, 0);
                setup_completed = 1;
                break;
            }
            break;
        case HAL_USB_REQUEST_DESTINATION_INTERFACE:
       //     hal_HstSendEvent(USB_EVENT, 0x1103aaaa);
            switch(hal_UsbCallbackInterface(setup->index&0xFF,HAL_USB_CALLBACK_TYPE_CMD, setup))
            {
            case HAL_USB_CALLBACK_RETURN_OK:
                setup_completed = 1;
                break;
            case HAL_USB_CALLBACK_RETURN_RUNNING:
                setup_completed = 2;
                break;
            case HAL_USB_CALLBACK_RETURN_KO:
                break;
            }
            break;
        case HAL_USB_REQUEST_DESTINATION_EP:
            switch(setup->request)
            {
            case HAL_USB_REQUEST_EP_GET_STATUS   :
                break;
            case HAL_USB_REQUEST_EP_CLEAR_FEATURE:
                HAL_USB_TRACE(HAL_USB_TRC, 0, "Clear feature\n");
                if(setup->value == 0 || setup->value & 0x01)
                {
                	//the implementation of hal_UsbEpStall has bad effect on CLEAR_FEATURE command
                	//remove this function will not effect current umss and uvideos
                	//TODO:implement hal_UsbEpStall in right way
                    //hal_UsbEpStall(setup->index&0xFF, FALSE);
                }
                setup_completed = 1;
                break;
            case HAL_USB_REQUEST_EP_SET_FEATURE  :
                break;
            default:
                break;
            }
                   switch(hal_UsbCallbackEp(setup->index&0xFF, HAL_USB_CALLBACK_TYPE_CMD, setup))
            {
            case HAL_USB_CALLBACK_RETURN_OK:
                setup_completed = 1;
                break;
            case HAL_USB_CALLBACK_RETURN_RUNNING:
                setup_completed = 2;
                break;
            case HAL_USB_CALLBACK_RETURN_KO:
                break;
            }
            break;
        default:
            break;
        }
        if(setup_completed == 0 || setup_completed == 1)
        {
            // Command finish. Ready to receive status.
            if(setup->lenght == 0)
            {
                if(setup->requestDesc.requestDirection)
                {
                    hal_UsbStatusOut();
                }
                else
                {
                    hal_UsbStatusIn();
                }
            }
        }
        if(setup_completed == 0)
        {
            if(setup->lenght != 0)
            {
                // Command success and data must be send

                if(setup->requestDesc.requestDirection)
                {
                    hal_UsbSend(0, g_HalUsbBufferEp0In, 0, 0);
                }
                else
                {
                    hal_UsbRecv(0, g_HalUsbBufferEp0Out, HAL_USB_MPS, 0);
                }
            }
        }
        break;
    case EP0_STATE_IN    :
        HAL_USB_TRACE(HAL_USB_TRC, 0, "State IN Data\n");
        /* Transfert finish */
        if(hal_UsbContinueTransfert(HAL_USB_EP_DIRECTION_IN(0)))
        {
            switch(g_HalUsbVar.RequestDesc.requestDest)
            {
            case HAL_USB_REQUEST_DESTINATION_DEVICE:
                break;
            case HAL_USB_REQUEST_DESTINATION_INTERFACE:
                    hal_UsbCallbackInterface(g_HalUsbVar.Ep0Index&0xFF, HAL_USB_CALLBACK_TYPE_DATA_CMD, setup);
                break;
            case HAL_USB_REQUEST_DESTINATION_EP:
                hal_UsbCallbackEp(g_HalUsbVar.Ep0Index&0xFF,
                                  HAL_USB_CALLBACK_TYPE_DATA_CMD, setup);
                break;
            }
            HAL_USB_TRACE(HAL_USB_TRC, 0, "State IN Data complete\n");
            hal_UsbStatusOut();
        }
        break;
    case EP0_STATE_OUT   :
        HAL_USB_TRACE(HAL_USB_TRC, 0, "State OUT Data\n");
        /* Transfert finish */
        if(hal_UsbContinueTransfert(HAL_USB_EP_DIRECTION_OUT(0)))
        {
            switch(g_HalUsbVar.RequestDesc.requestDest)
            {
            case HAL_USB_REQUEST_DESTINATION_DEVICE:
                break;
            case HAL_USB_REQUEST_DESTINATION_INTERFACE:
                hal_UsbCallbackInterface(g_HalUsbVar.Ep0Index&0xFF,
                                         HAL_USB_CALLBACK_TYPE_DATA_CMD,
                                         setup);
                break;
            case HAL_USB_REQUEST_DESTINATION_EP:
                hal_UsbCallbackEp(g_HalUsbVar.Ep0Index&0xFF,
                                  HAL_USB_CALLBACK_TYPE_DATA_CMD, setup);
                break;
            }
            HAL_USB_TRACE(HAL_USB_TRC, 0, "State OUT Data complete\n");
            hal_UsbStatusIn();
        }
        break;
    case EP0_STATE_STATUS_IN:
        HAL_USB_TRACE(HAL_USB_TRC, 0, "State IN Status\n");
        hal_UsbContinueTransfert(HAL_USB_EP_DIRECTION_IN(0));
        g_HalUsbVar.Ep0State = EP0_STATE_IDLE;
        hal_getSetupPacket();
        break;
    case EP0_STATE_STATUS_OUT:
        HAL_USB_TRACE(HAL_USB_TRC, 0, "State OUT Status\n");
        hal_UsbContinueTransfert(HAL_USB_EP_DIRECTION_OUT(0));
        g_HalUsbVar.Ep0State = EP0_STATE_IDLE;
        hal_getSetupPacket();
        break;
    }

	hal_SysExitCriticalSection(csStatus);
}

// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================

extern BOOL  g_usb_connected;
extern volatile UINT32 keycode_input;
UINT8 active_ep_flag = 0;
//UINT8 usb_first_frame = 0;
PROTECTED VOID hal_UsbIrqHandler(UINT8 interruptId)
{
    UINT32 status;
    UINT32 statusEp;
    UINT32 data;
    UINT8  i;
    UINT32 framestatus;
    COS_EVENT ev;
 //   UINT32 ticks;
//    ticks = hal_TimGetUpTime();
    HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandler);

    // To keep the compiler warning (unused variable)
    interruptId = interruptId;

    // Store interrupt flag and reset it
    status             = hwp_usbc->GINTSTS;
//    hal_HstSendEvent(USB_EVENT,0x11020004);
//    hal_HstSendEvent(USB_EVENT,status);
#if 0
    if(!usb_first_frame)
    {
        if(status&USBC_SOF)
        {
            ev.nEventId = EV_DM_USB_PLUG_IN;
            COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
            hal_HstSendEvent(USB_EVENT,0x11024004);
           // usb_first_frame = 1;
            //return;
        }
    } 
#endif

#ifdef USB_AUDIO_SUPPORT
//	hal_HstSendEvent(USB_EVENT,0x67676767);
//	hal_HstSendEvent(USB_EVENT,hal_UsbGetMode());
	if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
	{
        status            &= USBC_USBRST | USBC_ENUMDONE|USBC_ISOOUTDROP|USBC_SOF|USBC_EOPF
        | USBC_IEPINT | USBC_OEPINT | USBC_USBSUSP | USBC_ERLYSUSP|USBC_INCOMPLSOIN|USBC_INCOMPIP ;//|USBC_NPTXFEMP|USBC_PTXFEMP|USBC_GINNAKEFF
	}
	else
#endif
	{	
       status            &= USBC_USBRST | USBC_ENUMDONE
        | USBC_IEPINT | USBC_OEPINT | USBC_USBSUSP | USBC_ERLYSUSP;//|USBC_NPTXFEMP|USBC_PTXFEMP|USBC_GINNAKEFF
	}
    hwp_usbc->GINTSTS |= status;

    HAL_USB_TRACE(HAL_USB_TRC, 0, "UsbIrqHandler sts=0x%08x\n", status);

//    hal_HstSendEvent(USB_EVENT, 0x09200417);

    if(status & USBC_USBRST)
    {
        // Usb reset
 //       hal_HstSendEvent(USB_EVENT,0x04170007);
        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerReset);
        //hal_UsbInit();

        hwp_usbc->DCFG = USBC_DEVSPD(3);

        // Enable DMA INCR4
        hwp_usbc->GINTMSK   |= USBC_OEPINT | USBC_IEPINT | USBC_CONLDSTSCHNG;
        hwp_usbc->GAHBCFG   |= USBC_DMAEN  | USBC_HBSTLEN(HAL_USB_AHB_MODE);

        // EP interrupt EP0 IN/OUT
        hwp_usbc->DAINTMSK   = 1 | (1<<16);

        hwp_usbc->DOEPMSK    =  USBC_SETUPMK    | USBC_XFERCOMPLMSK | USBC_AHBERRMSK|(1<<1);
        hwp_usbc->DIEPMSK    =  USBC_TIMEOUTMSK | USBC_XFERCOMPLMSK | USBC_AHBERRMSK;

        // We got a reset, and reseted the soft state machine: Discard all other
        // interrupt causes.
        status &= USBC_ENUMDONE|USBC_USBSUSP;
        
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerReset);
        HAL_USB_TRACE(HAL_USB_TRC, 0, "UsbIrqHandler Reset EP0 config\n");
    }

    if(status & USBC_ENUMDONE)
    {
        // Enumeration done
   //     hal_HstSendEvent(USB_EVENT,0x04170008);

        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerEnum);

        // Config max packet size
        hwp_usbc->DIEPCTL0  = USBC_EP0_MPS(0);
        hwp_usbc->DOEPCTL0  = USBC_EP0_MPS(0);

        // Get usb description
        if(g_HalUsbVar.DeviceCallback != 0)
        {
            //hal_HstSendEvent(USB_EVENT,0x11130001);
            g_HalUsbVar.Desc = g_HalUsbVar.DeviceCallback(TRUE);
        }

        // Config EP0
        hal_getSetupPacket();

        HAL_USB_TRACE(HAL_USB_TRC, 0, "UsbIrqHandler Enum done\n");
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerEnum);
    }

    if(status & USBC_IEPINT) // TX
    {
   //     hal_HstSendEvent(USB_EVENT,0x04170009);
        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerI);
        data = hwp_usbc->DAINT & hwp_usbc->DAINTMSK;
        for(i = 0; i < g_HalUsbVar.NbEp; ++i)
        {
            // EP IN i have interrupt event
            if((data>>i)&1)
            {
                HAL_USB_TRACE(HAL_USB_TRC, 0, "UsbIrqHandler IEP%i int\n", i);
                if(i == 0)
                {
                    // EP0

                    statusEp            = hwp_usbc->DIEPINT0;
                  //  hal_HstSendEvent(USB_EVENT, statusEp);
                    if(statusEp & USBC_TIMEOUTMSK)
                    {
                        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerIto);

                        HAL_USB_TRACE(HAL_USB_TRC, 0,
                                  "UsbIrqHandler Timeout int\n");
                        // Clear Timeout interupt
                        hwp_usbc->DIEPINT0  = USBC_SETUPMK;
                        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerIto);
                    }
                    if(statusEp & USBC_XFERCOMPLMSK)
                    {
                        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerIcp);
                        hwp_usbc->DIEPINT0  = USBC_XFERCOMPLMSK;
                        if(g_HalUsbVar.Ep0State != EP0_STATE_IN ||
                           hal_UsbContinueTransfert(
                               HAL_USB_EP_DIRECTION_IN(i)))
                        {
                            HAL_USB_TRACE(HAL_USB_TRC, 0,
                                          "XFer complete\n");
                            // Clear transfert completed interrupt
                            // Decode Ep0 command
                            hal_UsbDecodeEp0Packet();
                        }
                        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerIcp);
                    }
                    if(statusEp & USBC_AHBERRMSK)
                    {
                        HAL_PROFILE_PULSE(hal_UsbAhbError);
                    }
                }
                else
                {
                    statusEp = hwp_usbc->DIEPnCONFIG[i-1].DIEPINT;
                    HAL_USB_TRACE(HAL_USB_TRC, 0, "Status 0x%08x\n", statusEp);

                    // Interrupt EP mode
                    if(g_HalUsbVar.EpInterruptMask & (1<<i))
                    {
                        hwp_usbc->DIEPnCONFIG[i-1].DIEPINT = statusEp;
                        if(statusEp & USBC_XFERCOMPLMSK)
                        {                        
                            HAL_USB_TRACE(HAL_USB_TRC, 0, "XFer complete\n");
                            if(hal_UsbContinueTransfert(
                                   HAL_USB_EP_DIRECTION_IN(i)))
                            {
								//hal_HstSendEvent(USB_EVENT,0x04171219);
                                hal_UsbCallbackEp
                                    (HAL_USB_EP_DIRECTION_IN(i),
                                     HAL_USB_CALLBACK_TYPE_TRANSMIT_END, 0);
#if 0
#ifdef USB_AUDIO_SUPPORT
                                if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
                                 // if(hal_UsbGetEpAttr(i) == HAL_USB_EP_TYPE_ISO)
                             	{
                                   if(hwp_usbc->DIEPnCONFIG[i-1].DIEPCTL&(1<<16))
                                      hwp_usbc->DIEPnCONFIG[i-1].DIEPCTL |= (1<<28);
                                   else
                                      hwp_usbc->DIEPnCONFIG[i-1].DIEPCTL |= (1<<29);
                             	}
#endif
#endif
                            }
                        }
                        if(statusEp & USBC_TIMEOUTMSK)
                        {
                            HAL_PROFILE_PULSE(hal_UsbTimeOutIn);
                            HAL_USB_TRACE(HAL_USB_TRC, 0,
                                          "UsbIrqHandler Timeout int\n");
                            // Clear Timeout interupt
                            hwp_usbc->DIEPnCONFIG[i].DIEPINT = USBC_SETUPMK;
                        }
                    }
                    else
                    {
                        if(statusEp & USBC_XFERCOMPLMSK || statusEp & USBC_TIMEOUTMSK)
                        {
                            hwp_usbc->DAINTMSK &= ~(1<<i);
                        }
                    }
                    if(statusEp & USBC_AHBERRMSK)
                    {
                        HAL_PROFILE_PULSE(hal_UsbAhbError);
                    }
                }
            }
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerI);
    }

    if(status & USBC_ERLYSUSP)
    {
  //      hal_HstSendEvent(USB_EVENT,0x04170010);
        HAL_USB_TRACE(HAL_USB_TRC, 0, "UsbIrqHandler ERLY Susp\n");
    }

    if(status & USBC_USBSUSP)
    {
  //       hal_HstSendEvent(USB_EVENT,0x04170011);

        if(g_HalUsbVar.DeviceCallback != 0)
        {
            //hal_HstSendEvent(USB_EVENT,0x11130002);
            g_HalUsbVar.Desc = g_HalUsbVar.DeviceCallback(FALSE);
        }
        HAL_USB_TRACE(HAL_USB_TRC, 0, "UsbIrqHandler USB Susp\n");
    }

    if(status & USBC_OEPINT) // RX
    {
        g_usb_connected = 1;
 //       hal_HstSendEvent(USB_EVENT,0x04170012);
        data = hwp_usbc->DAINT & hwp_usbc->DAINTMSK;
 //       hal_HstSendEvent(USB_EVENT,data);
        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerO);
        for(i = 0; i < g_HalUsbVar.NbEp; ++i)
        {
            // EP OUT i have interrupt event
            if((data>>(i+16))&1)
            {
                HAL_USB_TRACE(HAL_USB_TRC, 0, "UsbIrqHandler OEP%i int\n", i);
                if(i == 0)
                {
                    statusEp            = hwp_usbc->DOEPINT0;

                    if(statusEp & USBC_SETUPMK)
                    {
 //                       hal_HstSendEvent(USB_EVENT, 0x11090001);
                        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerOsp);
                        HAL_USB_TRACE(HAL_USB_TRC, 0,
                                      "UsbIrqHandler SETUP %i int\n",
                                      statusEp & USBC_INEPNAKEFF);
                        // Decode Ep0 command
                        hal_UsbDecodeEp0Packet();
                        // Clear Setup interrupt
                        hwp_usbc->DOEPINT0 = USBC_SETUPMK;
                        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerOsp);
                    }
                    if(statusEp & USBC_XFERCOMPLMSK)
                    {
  //                      hal_HstSendEvent(USB_EVENT, 0x11090002);
                        HAL_PROFILE_FUNCTION_ENTER(hal_UsbIrqHandlerOcp);
                        // Clear transfert completed interrupt
                        hwp_usbc->DOEPINT0 = USBC_XFERCOMPLMSK;

                        if(g_HalUsbVar.Ep0State != EP0_STATE_OUT ||
                           hal_UsbContinueTransfert(
                               HAL_USB_EP_DIRECTION_OUT(i)))
                        {
                            HAL_USB_TRACE(HAL_USB_TRC, 0,
                                          "UsbIrqHandler Xfer complete int\n");
                            // Decode Ep0 command
                            hal_UsbDecodeEp0Packet();
                        }

                        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerOcp);
                    }
                }
                else
                {
                    statusEp = hwp_usbc->DOEPnCONFIG[i-1].DOEPINT;
                    // Interrupt EP mode
        //            hal_HstSendEvent(USB_EVENT,0x04170031);
                    if(g_HalUsbVar.EpInterruptMask & (1<<(i+16)))
                    {
                        hwp_usbc->DOEPnCONFIG[i-1].DOEPINT = statusEp;
   //                     hal_HstSendEvent(USB_EVENT,0x04170032);
   //                     hal_HstSendEvent(USB_EVENT, statusEp);
                        
                        if(statusEp & USBC_XFERCOMPLMSK)
                        {
         //                   hal_HstSendEvent(USB_EVENT,0x04170022);
                            if(hal_UsbContinueTransfert(
                                   HAL_USB_EP_DIRECTION_OUT(i)))
                            {
                   //             hal_HstSendEvent(USB_EVENT,0x04170023);
                                HAL_USB_TRACE(HAL_USB_TRC, 0,
                                              "XFer complete\n");
                                hal_UsbCallbackEp
                                    (HAL_USB_EP_DIRECTION_OUT(i),
                                     HAL_USB_CALLBACK_TYPE_RECEIVE_END, 0);

#ifdef USB_AUDIO_SUPPORT
                           //     hal_HstSendEvent(USB_EVENT, 0x11050003);
                           //     hal_HstSendEvent(USB_EVENT, hal_UsbGetMode()); 
                           //  	hal_HstSendEvent(USB_EVENT, hal_UsbGetEpAttr(i));
                                if(HAL_USB_MODE_AUDIO == hal_UsbGetMode())
                                 // if(hal_UsbGetEpAttr(i) == HAL_USB_EP_TYPE_ISO)
                             	{
                                   if(hwp_usbc->DOEPnCONFIG[i-1].DOEPCTL&(1<<16))
                                      hwp_usbc->DOEPnCONFIG[i-1].DOEPCTL |= (1<<28);
                                   else
                                      hwp_usbc->DOEPnCONFIG[i-1].DOEPCTL |= (1<<29);
                             	}
#endif
                            }
                        }
                    }
                    else
                    {
             //           hal_HstSendEvent(USB_EVENT,0x04170033);
                        if(statusEp & USBC_XFERCOMPLMSK)
                        {
   //                         hal_HstSendEvent(USB_EVENT,0x04170034);
                            hwp_usbc->DAINTMSK &= ~(1<<(i+16));
                        }
                    }
                }
            }
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandlerO);
    }
#ifdef USB_AUDIO_SUPPORT
    if(status & USBC_ISOOUTDROP)
    {
      //hal_HstSendEvent(USB_EVENT, 0x11111111);   
    }
    if(status & USBC_INCOMPLSOIN)
    {
      hal_HstSendEvent(USB_EVENT, 0x22222222);
//       hal_HstSendEvent(USB_EVENT,hwp_usbc->DIEPnCONFIG[0].DIEPCTL);
    }
    if(status & USBC_SOF)
    {
       if(active_ep_flag != 0)
       {
//            hal_UsbEnableEp(g_uaudIsoOutEp, HAL_USB_EP_TYPE_ISO);
//            hal_HstSendEvent(USB_EVENT, 0x33333333);
//            hal_HstSendEvent(USB_EVENT, hwp_usbc->DOEPnCONFIG[1].DOEPCTL);         
           // hal_HstSendEvent(USB_EVENT, active_ep_flag);
           // active_ep_flag = 0;
       }
      //hal_HstSendEvent(USB_EVENT, 0x33333333);   
    }
    if(status & USBC_EOPF)
    {
     //hal_HstSendEvent(USB_EVENT, 0x44444444);
    }
    if(status & USBC_NPTXFEMP)
    {
      //hal_HstSendEvent(USB_EVENT, 0x55555555);
    }
    if(status & USBC_INCOMPIP)
    {
      //  hal_HstSendEvent(USB_EVENT, 0x12250001);
//        hal_UsbFlushRxFifo();
//        hal_UsbDisableEp(g_uaudIsoOutEp);
//        hal_HstSendEvent(USB_EVENT, hwp_usbc->DOEPnCONFIG[1].DOEPCTL);
//        active_ep_flag = 1;
    //    hal_HstSendEvent(USB_EVENT, hwp_usbc->DSTS);
    //    hal_HstSendEvent(USB_EVENT, hwp_usbc->DOEPnCONFIG[0].DOEPCTL);
    }

  //  hal_HstSendEvent(USB_EVENT, 0x12270001);
  //  hal_HstSendEvent(USB_EVENT, hal_TimGetUpTime() - ticks);
#endif
    HAL_PROFILE_FUNCTION_EXIT(hal_UsbIrqHandler);
}

PROTECTED VOID hal_UsbHandleIrqInGdb(VOID)
{
    if (hal_GdbIsInGdbLoop())
    {
        if ((hwp_usbc->GINTSTS & hwp_usbc->GINTMSK) != 0)
        {
            hal_UsbIrqHandler(SYS_IRQ_USBC);
        }
    }
}

PUBLIC VOID hal_UsbOpen(HAL_USB_GETDESCRIPTOR_CALLBACK_T callback)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbOpen\n");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbOpen);

    // resource will automatically enable the PLL on greenstone

    hal_SysRequestFreq(HAL_SYS_FREQ_USB, HAL_SYS_FREQ_104M, 0);

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    pmd_EnablePower(PMD_POWER_USB, TRUE);
#endif
    hwp_sysCtrl->REG_DBG          = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Other_Enable = SYS_CTRL_ENABLE_OC_USBPHY;
    hwp_sysCtrl->REG_DBG          = SYS_CTRL_PROTECT_LOCK;
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8808)
    pmd_EnablePower(PMD_POWER_USB, TRUE);
#endif

    hal_UsbInit();
    g_HalUsbVar.DeviceCallback = callback;
    hal_UsbConfig();

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbOpen);
}

PUBLIC VOID hal_UsbEpStall(UINT8 ep, BOOL stall)
{
    UINT8   epNum;
    REG32*  diepctl;
    REG32*  doepctl;

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbEpStall);

    hal_HstSendEvent(USB_EVENT, 0x1112eeee);

    epNum = HAL_USB_EP_NUM(ep);
    HAL_ASSERT(epNum < 16, "hal_UsbEpStall: Ep number invalid");

    HAL_USB_TRACE(HAL_USB_TRC, 0, "Stall EP 0x%02x stall %i\n", ep, stall);

    // Select ctl register
    if(epNum == 0)
    {
        diepctl = &hwp_usbc->DIEPCTL0;
        doepctl = &hwp_usbc->DOEPCTL0;
    }
    else
    {
        diepctl = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        doepctl = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
    }

    // Select direction
    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        if(stall == TRUE)
        {
            if(*diepctl & USBC_EPENA)
            {
                *diepctl |= USBC_STALL | USBC_EPDIS;
            }
            else
            {
                *diepctl |= USBC_STALL;
            }
        }
        else
        {
            *diepctl &= ~(USBC_STALL | USBC_EPDIS | USBC_EPENA | USBC_USBACTEP);
            *diepctl |=   USBC_SETD0PID;
        }
    }
    else
    {
        if(stall == TRUE)
        {
            *doepctl |= USBC_STALL;
        }
        else
        {
            *doepctl &= ~(USBC_STALL | USBC_EPDIS | USBC_EPENA | USBC_USBACTEP);
        }
    }

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbEpStall);
}



// =============================================================================
// hal_UsbRecv
// -----------------------------------------------------------------------------
/// Recv an usb packet
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param buffer Pointer on the data buffer to be send.
/// @param size Number of bytes to be send.
/// @param flag No flag you must set this parameter to zero
/// @return Number of sent bytes or -1 if error
// =============================================================================
PUBLIC INT32 hal_UsbRecv(UINT8 ep, UINT8* buffer, UINT16 size, UINT32 flag)
{
    UINT8   epNum;
    UINT32  activeEp;

    HAL_ASSERT(buffer != 0, "hal_UsbRecv: buffer null");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbRecv);

    epNum = HAL_USB_EP_NUM(ep);
    if(epNum == 0)
    {
        activeEp = USBC_USBACTEP;
    }
    else
    {
        activeEp = hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL & USBC_USBACTEP;
    }

    if(activeEp &&hal_UsbStartTransfert(HAL_USB_EP_DIRECTION_OUT(ep), buffer, size, flag)== 0)
    {
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbRecv);
        return(size);
    }
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbRecv Error ??");

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbRecv);
    return(-1);
}


// =============================================================================
// boot_UsbSend
// -----------------------------------------------------------------------------
/// Send an usb packet
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param buffer Pointer on the date buffer to be send.
/// @param size Number of bytes to be send.
/// @param flag For partial send set the flag to 1
/// @return Number of sent bytes or -1 if error
// =============================================================================
PUBLIC INT32 hal_UsbSend(UINT8 ep, UINT8* buffer, UINT16 size, UINT32 flag)
{
    UINT8   epNum;
    UINT32  activeEp;

    HAL_ASSERT(buffer != 0, "hal_UsbSend: buffer null");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbSend);

    epNum = HAL_USB_EP_NUM(ep);
    if(epNum == 0)
    {
        activeEp = USBC_USBACTEP;
    }
    else
    {
        activeEp = hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL & USBC_USBACTEP;
    }

    if(activeEp &&
       hal_UsbStartTransfert(HAL_USB_EP_DIRECTION_IN(ep), buffer, size, flag)
       == 0)
    {
        HAL_PROFILE_FUNCTION_EXIT(hal_UsbSend);
        return(size);
    }

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbSend);
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbSend Error ??");
    return(-1);
}

PUBLIC VOID hal_UsbCompletedCommand(VOID)
{
    switch(g_HalUsbVar.Ep0State)
    {
    case EP0_STATE_STATUS_OUT:
        hal_UsbStatusOut();
        break;
    case EP0_STATE_STATUS_IN:
        hal_UsbStatusIn();
        break;
    default:
        break;

    }
}

PUBLIC VOID hal_UsbEpEnableInterrupt(UINT8 ep, BOOL enable)
{
    UINT8   epNum;
    epNum = HAL_USB_EP_NUM(ep);

    HAL_ASSERT(epNum < 16, "hal_UsbEpEnableInterrupt: Ep number invalid");

    if(enable == TRUE)
    {
        if(HAL_USB_IS_EP_DIRECTION_IN(ep))
        {
            g_HalUsbVar.EpInterruptMask    |= (1<<(epNum+0));
        }
        else
        {
            g_HalUsbVar.EpInterruptMask    |= (1<<(epNum+16));
        }
    }
    else
    {
        if(HAL_USB_IS_EP_DIRECTION_IN(ep))
        {
            g_HalUsbVar.EpInterruptMask    &= ~(1<<(epNum+0));
        }
        else
        {
            g_HalUsbVar.EpInterruptMask    &= ~(1<<(epNum+16));
        }
    }
}

PUBLIC UINT16 hal_UsbEpRxTransferedSize(UINT8 ep)
{
    UINT8   epNum;

    epNum = HAL_USB_EP_NUM(ep);

    HAL_ASSERT(epNum < 16 || HAL_USB_IS_EP_DIRECTION_IN(ep),
               "hal_UsbEpRxTransferedSize: Ep number invalid");

    // When the transfer is done, the size value is
    // inverted. Consequently, when this function
    // is called after a transfer is done, size
    // must be inverted.
    return g_HalUsbVar.OutTransfert[epNum].sizeTransfered;
}

PUBLIC BOOL hal_UsbEpTransfertDone(UINT8 ep)
{
    UINT8   epNum;
    UINT8   offset;
    BOOL    value = FALSE;
    REG32*  reg;

    epNum = HAL_USB_EP_NUM(ep);

    HAL_ASSERT(epNum < 16, "hal_UsbEpTransfertDone: Ep number invalid");

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        HAL_ASSERT(!(g_HalUsbVar.EpInterruptMask & (1<<(epNum+0))),
                   "hal_UsbEpTransfertDone: IN EP%i is in interrupt mode", epNum);
    }
    else
    {
        HAL_ASSERT(!(g_HalUsbVar.EpInterruptMask & (1<<(epNum+16))),
                   "hal_UsbEpTransfertDone: OUT EP%i is in interrupt mode", epNum);
    }

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        offset = 0;
        if(g_HalUsbVar.InTransfert[epNum].sizeRemaining == -1)
        {
            return(TRUE);
        }

        if(epNum == 0)
        {
            reg = &hwp_usbc->DIEPINT0;
        }
        else
        {
            reg = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPINT;
        }
        if(*reg&USBC_TIMEOUTMSK)
        {
            *reg  = USBC_TIMEOUTMSK;
            hal_UsbCancelTransfert(ep);
            HAL_PROFILE_PULSE(hal_UsbTimeOutIn);
            value = TRUE;
        }
    }
    else
    {
        offset = 16;
        if(epNum == 0)
        {
            reg = &hwp_usbc->DOEPINT0;
        }
        else
        {
            reg = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPINT;
        }
    }
    if(*reg&USBC_XFERCOMPL)
    {
        *reg  = USBC_XFERCOMPL;
        value = (hal_UsbContinueTransfert(ep) == 1);
    }

    hwp_usbc->DAINTMSK |= 1<<(epNum+offset);

    return(value);
}

PUBLIC VOID hal_UsbCloseSoftware(VOID)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbCloseSoftware\n");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbCloseSoftware);

    hal_UsbClrConfig();
    hal_UsbSetMode(0);

    g_HalUsbVar.Ep0State        = EP0_STATE_IDLE;
    g_HalUsbVar.NbString        = 0;

    g_HalUsbVar.DeviceCallback  = 0;
    g_HalUsbVar.Desc            = 0;
    hal_UsbResetTransfert();

//    hal_SysRequestFreq(HAL_SYS_FREQ_USB, HAL_SYS_FREQ_32K, 0);

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbCloseSoftware);
}

PUBLIC VOID hal_UsbClose(VOID)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbClose\n");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbClose);

    hal_HstSendEvent(USB_EVENT, 0x13070101);

    hal_UsbClrConfig();

    g_HalUsbVar.Ep0State        = EP0_STATE_IDLE;
    g_HalUsbVar.NbString        = 0;
    hwp_usbc->GAHBCFG           = 0;
    hwp_usbc->GUSBCFG           = 0;
    g_HalUsbVar.DeviceCallback  = 0;
    hal_UsbResetTransfert();
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8808)
    pmd_EnablePower(PMD_POWER_USB, FALSE);
#endif	
    g_HalUsbVar.Desc            = 0;
    // resource will automatically disable the PLL on greenstone
    hwp_sysCtrl->REG_DBG           = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Other_Disable = SYS_CTRL_DISABLE_OC_USBPHY;
    hwp_sysCtrl->REG_DBG           = SYS_CTRL_PROTECT_LOCK;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    pmd_EnablePower(PMD_POWER_USB, FALSE);
#endif
    hal_SysRequestFreq(HAL_SYS_FREQ_USB, HAL_SYS_FREQ_32K, 0);

    HAL_PROFILE_FUNCTION_EXIT(hal_UsbClose);
}


// =============================================================================
// hal_UsbReset
// -----------------------------------------------------------------------------
/// This function is use to reset USB and/or change configuration of usb
/// For reset you must do hal_UsbReset(TRUE); sleep 0.5s; hal_UsbReset(FALSE);
/// @param reset If is true, the function is clear usb and power down usb else
/// the function power up usb
// =============================================================================
PUBLIC VOID hal_UsbReset(BOOL reset)
{
    UINT32 second_time,first_time;
    first_time = hal_TimGetUpTime();
    second_time = hal_TimGetUpTime();

    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbReset\n");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbReset);

    if(reset == TRUE)
    {
        //hal_HstSendEvent(USB_EVENT,0x04180001);
        //hal_HstSendEvent(USB_EVENT,hwp_usbc->GAHBCFG);
        if(hwp_usbc->GAHBCFG & USBC_DMAEN)
        {
            //hal_HstSendEvent(USB_EVENT,0x04170021);
            // Usb enable
            hwp_usbc->GRSTCTL   = USBC_CSFTRST;
            while((!(hwp_usbc->GRSTCTL&USBC_AHBIDLE)) && (second_time - first_time < HAL_USB_OPERATE_TIME))
            {
                second_time = hal_TimGetUpTime();
            }
            hal_UsbInit();
            hal_UsbConfig();
            pmd_EnablePower(PMD_POWER_USB, FALSE);
        }
    }
    else
    {
        pmd_EnablePower(PMD_POWER_USB, TRUE);
    }
    HAL_PROFILE_FUNCTION_EXIT(hal_UsbReset);
}

BOOL hal_UsbIsWorkAsHost()
{
        //hal_HstSendEvent(USB_EVENT,hwp_usbc->GOTGCTL);
	if((hwp_usbc->GOTGCTL&USBC_CONLDSTS))
		return FALSE;
	else
		return TRUE;	
}

void hal_UsbSetDisconnCb(void (*Cb)(BOOL isHost))
{
	g_pfDisconnCb = Cb;
}

PUBLIC HAL_USB_EP_TYPE_T hal_UsbGetEpAttr(UINT8 ep)
{    
    UINT32 epCtrl = 0;
    UINT8 epNum = HAL_USB_EP_NUM(ep);
    
    if (epNum == 0)
    {
        return HAL_USB_EP_TYPE_CONTROL;
    } 

    if (HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        epCtrl = hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
    }else
    {
        epCtrl = hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
    }

    return (HAL_USB_EP_TYPE_T)((epCtrl >> 18)&0x3);
}
PUBLIC void hal_UsbSetMode(HAL_USB_MODE_T Mode)
{
   g_UsbMode = Mode;
}
PUBLIC HAL_USB_MODE_T hal_UsbGetMode(void)
{
   return g_UsbMode;
}
#else // HAL_USB_DRIVER_DONT_USE_ROMED_CODE

#include "halp_usb.h"
#include "halp_debug.h"

#include "hal_usb.h"

#include "boot_usb.h"

PUBLIC VOID hal_UsbOpen(HAL_USB_GETDESCRIPTOR_CALLBACK_T callback)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbOpen\n");
    // resource will automatically enable the PLL on greenstone
    hal_SysRequestFreq(HAL_SYS_FREQ_USB, HAL_SYS_FREQ_39M, 0);
    hwp_sysCtrl->REG_DBG          = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Other_Enable = SYS_CTRL_ENABLE_OC_USBPHY;
    hwp_sysCtrl->REG_DBG          = SYS_CTRL_PROTECT_LOCK;
    pmd_EnablePower(PMD_POWER_USB, TRUE);

    boot_UsbInitVar();
    boot_UsbOpen(callback);
}

PUBLIC VOID hal_UsbClose(VOID)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbClose\n");
    // FIXME USB Romed global variables being uninitialized,
    // next function will ultimately crash if hal_UsbOpen
    // has never been called before.
    boot_UsbClose();
    pmd_EnablePower(PMD_POWER_USB, FALSE);

    // resource will automatically disable the PLL on greenstone
    hwp_sysCtrl->REG_DBG           = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Other_Disable = SYS_CTRL_DISABLE_OC_USBPHY;
    hwp_sysCtrl->REG_DBG           = SYS_CTRL_PROTECT_LOCK;
    hal_SysRequestFreq(HAL_SYS_FREQ_USB, HAL_SYS_FREQ_32K, 0);
}

PUBLIC VOID hal_UsbReset(BOOL reset)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbReset\n");

    HAL_PROFILE_FUNCTION_ENTER(hal_UsbReset);

    if(reset == TRUE)
    {
        boot_UsbReset();
        pmd_EnablePower(PMD_POWER_USB, FALSE);
    }
    else
    {
        pmd_EnablePower(PMD_POWER_USB, TRUE);
    }
    HAL_PROFILE_FUNCTION_EXIT(hal_UsbReset);
}

PUBLIC INT32 hal_UsbSend(UINT8  ep, UINT8* buffer, UINT16 size, UINT32 flag)
{
    HAL_ASSERT(buffer != 0, "hal_UsbSend: buffer null");
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbSend EP%i buffer=0x%08x size=%i\n",
              HAL_USB_EP_NUM(ep), buffer, size);
    return boot_UsbSend(ep, buffer, size, flag);
}

PUBLIC INT32 hal_UsbRecv(UINT8  ep, UINT8* buffer, UINT16 size, UINT32 flag)
{
    HAL_ASSERT(buffer != 0, "hal_UsbRecv: buffer null");
    HAL_ASSERT((size % HAL_USB_MPS) == 0, "hal_UsbRecv: size");
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_usbRecv EP%i buffer=0x%08x size=%i\n",
              HAL_USB_EP_NUM(ep), buffer, size);
    return boot_UsbRecv(ep, buffer, size, flag);
}

PUBLIC VOID hal_UsbCompletedCommand(VOID)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbCompletedCommand\n");
    boot_UsbCompletedCommand();
}

PUBLIC VOID hal_UsbEpStall(UINT8 ep, BOOL stall)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0, "hal_UsbEpStall EP=0x%02 stall=%i\n",
                  ep, stall);
    boot_UsbEpStall(ep, stall);
}

PUBLIC VOID hal_UsbEpEnableInterrupt(UINT8 ep, BOOL enable)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0,
              "hal_UsbEpEnableInterrupt EP=0x%02 enable=%i\n", ep, enable);
    boot_UsbEpEnableInterrupt(ep, enable);
}

PUBLIC UINT16 hal_UsbEpRxTransferedSize(UINT8 ep)
{
    HAL_USB_TRACE(HAL_USB_TRC, 0,
              "hal_UsbEpRxTranferedSize EP%i\n", HAL_USB_EP_NUM(ep));
    return boot_UsbEpRxTransferedSize(ep);
}

PUBLIC BOOL hal_UsbEpTransfertDone(UINT8 ep)
{
    return boot_UsbEpTransfertDone(ep);
}

PROTECTED VOID hal_UsbHandleIrqInGdb(VOID)
{
    if (hal_GdbIsInGdbLoop())
    {
        if ((hwp_usbc->GINTSTS & hwp_usbc->GINTMSK) != 0)
        {
            boot_UsbIrqHandler(SYS_IRQ_USBC);
        }
    }
}

#endif // !HAL_USB_DRIVER_DONT_USE_ROMED_CODE

#endif // CHIP_HAS_USB


