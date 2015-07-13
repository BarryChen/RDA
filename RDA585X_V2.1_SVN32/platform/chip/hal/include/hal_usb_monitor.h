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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_usb_monitor.h $
//  $Author: admin $
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $
//  $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file hal_usb_monitor.h
///
/// Global variable for usb monitor
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_USB_MONITOR_H_
#define _HAL_USB_MONITOR_H_

#include "cs_types.h"

#include "hal_usb.h"


// =============================================================================
// MACROS                                                                       
// =============================================================================

#define HAL_HOST_USB_FIFO_READ_SIZE              (32)
#define HAL_HOST_USB_BUFFER_OUT_SIZE             (2048)
#define HAL_HOST_USB_BUFFER_IN_SIZE              (1032)

// =============================================================================
// TYPES                                                                        
// =============================================================================

typedef enum {
    HAL_HOST_USB_MODE_BOOT                      = 0x00000001,
    HAL_HOST_USB_MODE_SX                        = 0x00000002
} HAL_HOST_USB_MODE_T;

// =============================================================================
// HAL_HOST_USB_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback for decoding trame
// =============================================================================
typedef VOID (*HAL_HOST_USB_CALLBACK_T)(VOID *buffer, UINT16 size);

// ============================================================================
// HAL_HOST_USB_MONITOR_FIFO_ELEM_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================


typedef struct
{
    UINT32                         Addr;                         //0x00000000
    UINT8                          Rid;                          //0x00000004
    UINT16                         Size;                         //0x00000006
} HAL_HOST_USB_MONITOR_FIFO_ELEM_T; //Size : 0x8



// ============================================================================
// HAL_HOST_USB_MONITOR_BUFFER_IN_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================


typedef struct
{
    UINT8                          Buffer[HAL_HOST_USB_BUFFER_IN_SIZE]; //0x00000000
} HAL_HOST_USB_MONITOR_BUFFER_IN_T; //Size : 0x408



// ============================================================================
// HAL_HOST_USB_MONITOR_CTX_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================


typedef struct
{
    HAL_HOST_USB_MODE_T            MonitorMode;                  //0x00000000
    UINT8                          BufferOut[HAL_HOST_USB_BUFFER_OUT_SIZE]; //0x00000004
    HAL_HOST_USB_MONITOR_BUFFER_IN_T BufferIn[2];                //0x0x00000804
    HAL_HOST_USB_MONITOR_FIFO_ELEM_T Fifo[HAL_HOST_USB_FIFO_READ_SIZE]; //0x0x00001014
} HAL_HOST_USB_MONITOR_CTX_T; //Size : 0x1114

// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================

// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================

// =============================================================================
// hal_HostUsbOpen
// -----------------------------------------------------------------------------
/// Configure USB host
// =============================================================================
PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T* hal_HostUsbOpen(VOID);

// =============================================================================
// hal_HostUsbClose
// -----------------------------------------------------------------------------
/// Close host usb
// =============================================================================
PUBLIC VOID hal_HostUsbClose(VOID);

#endif // _HAL_USB_MONITOR_H_
