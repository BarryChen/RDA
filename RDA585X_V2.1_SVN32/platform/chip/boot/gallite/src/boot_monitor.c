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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/gallite/src/boot_monitor.c $ //
//  $Author: huazeng $                                                         //
//  $Date: 2011-11-30 18:24:46 +0800 (����, 30 ʮһ�� 2011) $                     //
//  $Revision: 12233 $                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_monitor.c                                                      //
///     RMC monitor
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "boot_sector_chip.h"
#include "boot_sector.h"

#include "global_macros.h"
#include "timer.h"
#include "uart.h"
#include "debug_host.h"
#include "debug_uart.h"
#include "debug_port.h"
#include "cfg_regs.h"
#include "cp0.h"
#include "sys_ctrl.h"

#include "boot_host.h"
#include "boot.h"
#include "bootp_mode.h"
#include "bootp_host.h"
#include "boot_uart.h"
#include "boot_ispi.h"
#include "boot_usb_monitor.h"
#include "bootp_uart_monitor.h"
#include "bootp_debug.h"

#include "hal_debug.h"


// Forward declaration.
PROTECTED VOID boot_MonitorDoMonitoring(VOID);

// =============================================================================
// boot_Monitor
// -----------------------------------------------------------------------------
/// Entry point for the boot monitor. This is the first C function called out
/// of boot. When exiting this function, we jump into flash with an almost not
/// altered stack pointer.
// =============================================================================
PROTECTED VOID boot_Monitor(VOID)
{
    // Enable the host from within. (Forced)
    // Set clock
    // Formula from the uart driver
    // Divider mode is 4
    // There is no PLL during monitoring
    // Rq: the plus 4/2 * rate is a subtle way to get the upper integer
    UINT32 divider = ((BOOT_HST_SYSTEM_CLOCK + 2 * BOOT_HST_HOST_RATE) / ( 4 * BOOT_HST_HOST_RATE)) - 2;
    hwp_sysCtrl->Cfg_Clk_Uart[0] = SYS_CTRL_UART_DIVIDER(divider)
        | SYS_CTRL_UART_SEL_PLL_SLOW;
    
    // Force on.
    hwp_debugHost->mode = DEBUG_HOST_FORCE_ON;


    VOID(*flashEntry)(UINT32);
    {
        flashEntry = (VOID(*)(UINT32)) BOOT_SECTOR_ENTRY_POINT;
    }

    // Open the ISPI to access and configure stuff, using the default,romed
    // configuration.
    // FIXME There just to link boot_ispi into the ROM.
    boot_IspiOpen(NULL);

    // Do monitoring
    boot_MonitorDoMonitoring();

    // Boot into flash.
    BOOT_PROFILE_PULSE(BOOT_DIRECT_IN_FLASH);
    flashEntry(BOOT_SECTOR_STANDARD_MAIN);
}


// =============================================================================
// boot_MonitorDoMonitoring
// -----------------------------------------------------------------------------
/// This function actually implement the monitoring operations.
// =============================================================================
PROTECTED VOID boot_MonitorDoMonitoring(VOID)
{
    BOOT_HOST_USB_MONITOR_CTX_T hostUsbContext;
    BOOT_MONITOR_OP_STATUS_T    monitorLoopStatus = BOOT_MONITOR_OP_STATUS_NONE;
    BOOL flashProgrammed = FALSE;

    hostUsbContext.MonitorMode = BOOT_HOST_USB_MODE_BOOT;

    // Get current time to calculate time-out exit
    UINT32  enterTime = hwp_timer->HWTimer_CurVal;

    // Configure the IOMux to let the PXTS exit.
#ifdef BOOT_WITH_IOMUX_PROFILE
    hwp_debugPort->Debug_Port_Mode |= DEBUG_PORT_PXTS_EXL_CLOCK_POL
        | DEBUG_PORT_MODE_CONFIG_MODE_PXTS;
    hwp_debugPort->Pxts_Exl_Cfg    |= DEBUG_PORT_ENABLE_PXTS_TAG(1<<HAL_DBG_PXTS_BOOT);
    hwp_configRegs->Alt_mux_select &= ~CFG_REGS_DEBUG_PORT_MASK;
#endif // BOOT_WITH_IOMUX_PROFILE

    BOOT_PROFILE_PULSE(BOOT_POWER_STATE_DONE);

    // Initializes the uart baudrate detection variables.
    g_bootUartMonitorBrDetected = FALSE;
    g_bootUartMonitorBaudRate   = 0;

    {
        // Check if NOR flash is programmed
        if (*(BOOT_FLASH_PROGRAMMED_PATTERN_ADDRESS) == BOOT_FLASH_PROGRAMMED_PATTERN)
        {
            flashProgrammed = TRUE;
        }
    }

    // We enter the monitor if access is forced through 
    // boot mode or there is no code on CS0.
    if ((g_bootBootMode & BOOT_MODE_FORCE_MONITOR) || (!flashProgrammed))
    {
        BOOT_PROFILE_PULSE(BOOT_ENTER_MONITOR_LOOP);
        
        // If we are out of reset, set the status to 'waiting for a command'.
        // Otherwise, keep the status as it was probably set by the boot sector
        // and carries a specific meaning.
        if (hwp_debugHost->p2h_status == BOOT_HST_STATUS_NONE)
        {
            hwp_debugHost->p2h_status = BOOT_HST_STATUS_WAITING_FOR_COMMAND;
        }                                                                       
                                                                                
        // Open the UART monitor, only when the boot mode requires it.
        if (g_bootBootMode & BOOT_MODE_UART_MONITOR_ENABLE)
        {
            boot_UartMonitorOpen();
        }

        // Open the USB monitor, except when to boot mode prevents it.
        if (!(g_bootBootMode & BOOT_MODE_USB_MONITOR_DISABLE))
        {
            boot_UsbInitVar();
            boot_HostUsbClockEnable();
            boot_UsbOpen(boot_HostUsbDeviceCallback);
            // We never exit this function, so the hostUsbContext
            // is valid (On the stack).
            boot_HostUsbOpen(&hostUsbContext, 0, 0);
        }

        while (1)
        {
            // We are in the monitor. We are indefinitely trying to 
            // execute command from any (available) monitor. This
            // is done through a dedicated function per monitor
            // (boot_HstMonitor for the host monitor,
            // boot_UartMonitor for the uart monitor,
            // boot_HostUsbRecv for the USB monitor.)
            // When any of them receive a "Exit the monitor"
            // BOOT_HST_MONITOR_END_CMD command, it
            // returns BOOT_MONITOR_OP_STATUS_EXIT, and we check that to leave the 
            // monitor loop.
            monitorLoopStatus = BOOT_MONITOR_OP_STATUS_NONE;


            // Host monitor always enabled. 
            monitorLoopStatus |= boot_HstMonitor();
            
            // Process the UART monitor, only when to boot mode requires it.
            if (g_bootBootMode & BOOT_MODE_UART_MONITOR_ENABLE)
            {
                monitorLoopStatus |= boot_UartMonitor();
            }
            
            // Process the USB monitor, except when to boot mode prevents it.
            if (!(g_bootBootMode & BOOT_MODE_USB_MONITOR_DISABLE))
            {
                // Receive a USB packet, parse it and execute 
                // the command if any.
                // Return BOOT_MONITOR_OP_STATUS_EXIT if the exit monitor command is received.
                monitorLoopStatus |= boot_HostUsbRecv();

                // If a read command was received, send a packet
                // with the read data.
                boot_HostUsbSend();
            }

            // Check the status returned by the monitors.
            if (monitorLoopStatus & BOOT_MONITOR_OP_STATUS_EXIT)
            {
                // Exit the monitoring loop
                break;
            }

            // Check we received a command, and update the timeout
            // FIXME Fixe here to implement the behaviour about the timeout
            // (No command or after the command)
            if (monitorLoopStatus & BOOT_MONITOR_OP_STATUS_CONTINUE)
            {
                // Update the timeout or set a variable bypassing the
                // timeout stuff.
                enterTime = hwp_timer->HWTimer_CurVal;
            }

            if (monitorLoopStatus == BOOT_MONITOR_OP_STATUS_NONE)
            {
                // No command received.
                // Exit the loop if valid flash and timeout.
                if ((*(BOOT_FLASH_PROGRAMMED_PATTERN_ADDRESS) == BOOT_FLASH_PROGRAMMED_PATTERN)
                 && ((hwp_timer->HWTimer_CurVal - enterTime) > (BOOT_IDLE_TIMEOUT*16384)))
                {
                    // If no command was sent, we have to tell we're not ready
                    // anymore to handle new commands.
                    hwp_debugHost->p2h_status = BOOT_HST_STATUS_NONE;
                    break;
                }
            }

        }

        // Close the UART monitor, only when the boot mode requires it.
        if (g_bootBootMode & BOOT_MODE_UART_MONITOR_ENABLE)
        {
            boot_UartMonitorClose();
        }

        // Close the USB monitor, except when to boot mode prevents it.
        if (!(g_bootBootMode & BOOT_MODE_USB_MONITOR_DISABLE))
        {
            boot_HostUsbClose();
            boot_UsbClose();
        }
    }
}

