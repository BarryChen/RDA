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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/boot/gallite/src/bootp_uart_monitor.h $
//  $Author: admin $
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $
//  $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file bootp_uart_monitor.h
///
/// Uart monitor function
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BOOTP_UART_MONITOR_H_
#define _BOOTP_UART_MONITOR_H_

#include "cs_types.h"
#include "hal_uart.h"
#include "boot.h"
// =============================================================================
// MACROS                                                                       
// =============================================================================


// =============================================================================
// TYPES                                                                        
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================
// =============================================================================
// g_bootUartMonitorBaudRate;
// -----------------------------------------------------------------------------
/// Holds the detected baud rate if any, or 0.
// =============================================================================
EXPORT PROTECTED HAL_UART_BAUD_RATE_T g_bootUartMonitorBaudRate;



// =============================================================================
// g_bootUartMonitorBrDetected
// -----------------------------------------------------------------------------
/// Is true if, and only if, a communication baudrate has been detected.
// =============================================================================
EXPORT PROTECTED BOOL g_bootUartMonitorBrDetected;


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================

PROTECTED VOID boot_UartMonitorOpen(VOID);

PROTECTED VOID boot_UartMonitorClose(VOID);

PROTECTED BOOT_MONITOR_OP_STATUS_T boot_UartMonitor(VOID);

#endif // _BOOTP_UART_MONITOR_H_
