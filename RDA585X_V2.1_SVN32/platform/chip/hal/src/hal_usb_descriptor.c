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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_usb_descriptor.c $
//  $Author: admin $
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $
//  $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file hal_usb_descriptor.c
///
/// Function for generate USB Descriptor
//
////////////////////////////////////////////////////////////////////////////////


#include "hal_usb_descriptor.h"


#include "sxr_ops.h"

#include <string.h>


PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewCmdEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback)
{
    UINT8 epNum;

    HAL_USB_EP_DESCRIPTOR_T* ep_desc;

    epNum   = HAL_USB_EP_NUM(ep);
    if(epNum >= 16 || epNum == 0)
    {
        return 0;
    }

    ep_desc = (HAL_USB_EP_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T));

    ep_desc->ep         = ep;
    ep_desc->interval   = 0;
    ep_desc->callback   = callback;
    ep_desc->type       = HAL_USB_EP_TYPE_CONTROL;

    return ep_desc;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewBulkEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback)
{
    UINT8 epNum;

    HAL_USB_EP_DESCRIPTOR_T* ep_desc;

    epNum   = HAL_USB_EP_NUM(ep);
    if(epNum >= 16 || epNum == 0)
    {
        return 0;
    }

    ep_desc = (HAL_USB_EP_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T));

    ep_desc->ep         = ep;
    ep_desc->interval   = 0;
    ep_desc->callback   = callback;
    ep_desc->type       = HAL_USB_EP_TYPE_BULK;

    return ep_desc;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewInterruptEpDescriptor(UINT8 ep, UINT8 interval,
                               HAL_USB_CALLBACK_T callback)
{
    UINT8 epNum;

    HAL_USB_EP_DESCRIPTOR_T* ep_desc;

    epNum   = HAL_USB_EP_NUM(ep);
    if(epNum >= 16 || epNum == 0)
    {
        return 0;
    }

    ep_desc = (HAL_USB_EP_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T));

    ep_desc->ep         = ep;
    ep_desc->interval   = interval;
    ep_desc->callback   = callback;
    ep_desc->type       = HAL_USB_EP_TYPE_INTERRUPT;

    return ep_desc;
}

PUBLIC HAL_USB_EP_DESCRIPTOR_T*
hal_UsbDescriptorNewIsoEpDescriptor(UINT8 ep, HAL_USB_CALLBACK_T callback)
{
    UINT8 epNum;

    HAL_USB_EP_DESCRIPTOR_T* ep_desc;

    epNum   = HAL_USB_EP_NUM(ep);
    if(epNum >= 16 || epNum == 0)
    {
        return 0;
    }

    ep_desc = (HAL_USB_EP_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T));

    ep_desc->ep         = ep;
    ep_desc->interval   = 1;
    ep_desc->callback   = callback;
    ep_desc->type       = HAL_USB_EP_TYPE_ISO;

    return ep_desc;
}

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorNewInterfaceDescriptor(UINT8                            usbClass,
                                        UINT8                            usbSubClass,
                                        UINT8                            usbProto,
                                        UINT8                            interfaceIdx,
                                        HAL_USB_EP_DESCRIPTOR_T**        epList,
                                        UINT8*                           description,
                                        HAL_USB_CALLBACK_T               callback)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T* interface_desc;

    interface_desc = (HAL_USB_INTERFACE_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T));

    interface_desc->usbClass     = usbClass;
    interface_desc->usbSubClass  = usbSubClass;
    interface_desc->usbProto     = usbProto;
    interface_desc->interfaceIdx = interfaceIdx;
    interface_desc->epList       = epList;
    interface_desc->description  = description;
    interface_desc->callback     = callback;

    return interface_desc;
}

PUBLIC HAL_USB_CS_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorNewCSInterfaceDescriptor(HAL_USB_CS_CONSTRUCTOR_T constructor,
                                          HAL_USB_CALLBACK_T       callback)
{
    HAL_USB_CS_INTERFACE_DESCRIPTOR_T* interface_desc;

    interface_desc = (HAL_USB_CS_INTERFACE_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T));

    interface_desc->reserved[0]  = 0;
    interface_desc->reserved[1]  = 0;
    interface_desc->constructor  = constructor;
    interface_desc->interfaceIdx = 0xFF;
    interface_desc->callback     = callback;

    return interface_desc;
}

PUBLIC HAL_USB_CONFIG_DESCRIPTOR_T*
hal_UsbDescriptorNewConfigDescriptor(UINT8 maxPower, UINT8 attributes,
                          HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceList,
                          UINT8*                           description)
{
    HAL_USB_CONFIG_DESCRIPTOR_T* config_desc;

    config_desc = (HAL_USB_CONFIG_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_CONFIG_DESCRIPTOR_T));

    config_desc->attrib        = attributes;
    config_desc->maxPower      = maxPower;
    config_desc->interfaceList = interfaceList;
    config_desc->description   = description;

    return config_desc;
}

PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T*
hal_UsbDescriptorNewDeviceDescriptor(UINT8  usbClass, UINT8  usbSubClass,
                          UINT8  usbProto, UINT16 vendor,
                          UINT16 product,  UINT16 version,
                          HAL_USB_CONFIG_DESCRIPTOR_T* configList[],
                          UINT8*                       serialnumber,
                          UINT8*                       description)
{
    HAL_USB_DEVICE_DESCRIPTOR_T* device_desc;

    device_desc = (HAL_USB_DEVICE_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_DEVICE_DESCRIPTOR_T));


    device_desc->usbClass         = usbClass;
    device_desc->usbSubClass      = usbSubClass;
    device_desc->usbProto         = usbProto;
    device_desc->vendor           = vendor;
    device_desc->product          = product;
    device_desc->version          = version;
    device_desc->configList       = configList;
    device_desc->description      = description;
    device_desc->serialNumber     = serialnumber;
    device_desc->usbMode          = 0;

    return device_desc;
}

PRIVATE VOID
hal_UsbDescriptorCleanEpListDescriptor(HAL_USB_EP_DESCRIPTOR_T** eps)
{
    UINT32 i;

    if(eps)
    {
        for(i = 0; eps[i] != 0; ++i) {
            sxr_Free(eps[i]);
        }
        sxr_Free(eps);
    }
}

PUBLIC VOID
hal_UsbDescriptorCleanInterfaceListDescriptor(HAL_USB_INTERFACE_DESCRIPTOR_T** interfaces)
{
    UINT32 i;

    if(interfaces)
    {
        for(i = 0; interfaces[i] != 0; ++i)
        {
            hal_UsbDescriptorCleanEpListDescriptor(interfaces[i]->epList);
            sxr_Free(interfaces[i]);
            interfaces[i] = 0;
        }
        sxr_Free(interfaces);
    }
}

PRIVATE VOID
hal_UsbDescriptorCleanConfigListDescriptor(HAL_USB_CONFIG_DESCRIPTOR_T** configs)
{
    UINT32 i;

    if(configs)
    {
        for(i = 0; configs[i] != 0; ++i)
        {
            hal_UsbDescriptorCleanInterfaceListDescriptor(configs[i]->interfaceList);
            sxr_Free(configs[i]);
        }
        sxr_Free(configs);
    }
}

PUBLIC VOID
hal_UsbDescriptorCleanDeviceDescriptor(HAL_USB_DEVICE_DESCRIPTOR_T* dev)
{
    if(dev)
    {
        hal_UsbDescriptorCleanConfigListDescriptor(dev->configList);
        sxr_Free(dev);
    }
}

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T*
hal_UsbDescriptorCopyInterfaceDescriptor(CONST HAL_USB_INTERFACE_DESCRIPTOR_T* interface)
{
    UINT32                          nbEp;
    UINT32                          i;
    HAL_USB_INTERFACE_DESCRIPTOR_T* newInterface;

    newInterface = (HAL_USB_INTERFACE_DESCRIPTOR_T*)
        sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T));
    memcpy(newInterface, interface, sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T));

    for(nbEp = 0; interface->epList[nbEp]; ++nbEp);

    newInterface->epList = (HAL_USB_EP_DESCRIPTOR_T**)
        sxr_Malloc((nbEp+1)*sizeof(HAL_USB_EP_DESCRIPTOR_T*));

    newInterface->epList[nbEp] = 0;

    for(i = 0; i < nbEp; ++i)
    {
        newInterface->epList[i] = (HAL_USB_EP_DESCRIPTOR_T*)
            sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T));
        memcpy(newInterface->epList[i], interface->epList[i],
               sizeof(HAL_USB_EP_DESCRIPTOR_T));
    }

    return newInterface;
}
