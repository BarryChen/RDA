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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/include/boot_uart.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                        //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_uart.h                                                         //
/// That file contains types and defines used to exchange with the            //
/// uart for auto-bauding, monitoring and remote execution.                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _BOOT_UART_H_
#define _BOOT_UART_H_

/// @defgroup boot_uart BOOT Uart Port Driver/Client
/// That group describes briefly the exchange interface 
/// to the romed boot uart monitor.
/// @{
///

#include "cs_types.h"
#include "hal_uart.h"

// =============================================================================
// boot_UartMonitorGetBaudRate
// -----------------------------------------------------------------------------
/// Used to recover the value of the detected monitor uart
/// @param baudRate pointer where the detected value, if any, will be 
/// written.
/// @return \c TRUE If a connected Uart was detected and the baudrate
/// discover, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL boot_UartMonitorGetBaudRate(HAL_UART_BAUD_RATE_T* rate);


// =============================================================================
// boot_HstUartDump
// -----------------------------------------------------------------------------
/// Send a buffer of data in a dump packet through the host.
/// @param data Pointer to the data to send.
/// @param length Lenght of the buffer to send.
/// @param access Access width in bytes (1,2 or 4).
/// @return #BOOT_UM_ERR_NO or #BOOT_UM_ERR_TX_FAILED.
// =============================================================================
PUBLIC UINT32 boot_HstUartDump(UINT8* data, UINT32 length, UINT32 access);


/// @} // <-- End of the boot_uart group.

#endif // _BOOT_UART_H_




