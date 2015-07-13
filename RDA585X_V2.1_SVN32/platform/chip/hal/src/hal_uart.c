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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_uart.c $ //
//    $Author: admin $                                                        // 
//    $Date: 2012-07-10 11:26:59 +0800 (星期二, 10 七月 2012) $                     //   
//    $Revision: 16264 $                                                        //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_uart.c                                                          //
/// This file implements the UART driver.                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"

#include "chip_id.h"

#include "global_macros.h"

#include "uart.h"
#include "sys_ifc.h"
#include "sys_irq.h"

#include "hal_uart.h"
#include "hal_gpio.h"

#include "halp_sys_ifc.h"
#include "halp_sys.h"
#include "halp_irq.h"
#include "halp_uart.h"
#include "halp_debug.h"


//  ---------- Some Internal defines. ---------- 
#define UART_LOOP_BIT   2
#define AFC_MODE_MASK   0x3f

#define NEED_IRQ(val) (val & 0x1)
#define IS_DMA(val) (val & 0x2)

/// Type use to store all the information related to 
/// one instance of the UART driver. An array of them
/// will be used for all the instances. 
/// As the trace_uart (considered as UART0) is not
/// handled by this driver, we will shift by 1 and
/// affect case 0 for UART 1, case 1 for UART 2, usw...
typedef struct
{
    HAL_UART_IRQ_STATUS_T       irqMask;
    /// Variable to store the irq handler for the uart
    HAL_UART_IRQ_HANDLER_T      irqHandler;
    HAL_UART_TRANSFERT_MODE_T   rxMode;
    HAL_UART_TRANSFERT_MODE_T   txMode;
    HAL_IFC_REQUEST_ID_T        rxRequestId;
    HAL_IFC_REQUEST_ID_T        txRequestId;
    VOLATILE UINT8              rxIfcCh;
    VOLATILE UINT8              txIfcCh;
    /// Assume this is initialized to 0 by the BSS.
    BOOL                        forcingPins;
    /// Whether to wakeup system via UART break interrupt
    BOOL                        breakIntWakeup;
    /// For resource managment
    BOOL                        opened;
    BOOL                        actived;
} HAL_UART_PROP_T;

PRIVATE HAL_UART_PROP_T g_halUartPropArray[HAL_UART_QTY-1];

// Array of pointers towards the UART HW modules
PRIVATE HWP_UART_T* g_halUartHwpArray[HAL_UART_QTY-1] = 
#if (CHIP_STD_UART_QTY == 1)
    {hwp_uart};
#else
    {hwp_uart, hwp_uart2};
#endif

// Simple send byte / get byte Macros
#define hal_SendByte(hwp_uart, byte_to_send) hwp_uart->rxtx_buffer = byte_to_send;
#define hal_GetByte(hwp_uart) hwp_uart->rxtx_buffer


// ============================================================================
// hal_UartBreakIntWakeup
// ----------------------------------------------------------------------------
/// This function configures whether the system can be waked up 
/// by Uart break interrupt.
/// It must be configured before calling hal_UartOpen and hal_UartClose
/// to take effect.
///
/// @param id Identifier of the UART for which the function is called.
/// @param on TRUE to enable breakIntWakeup mode, FALSE otherwise.
// ============================================================================
PUBLIC VOID hal_UartBreakIntWakeup(HAL_UART_ID_T id, BOOL on)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    g_halUartPropArray[id-1].breakIntWakeup = on;

    if (on)
    {
        // Clean FIFO
        hal_UartFifoFlush(id);
    }
}


// ============================================================================
// hal_UartIrqEnableBreakInt
// ----------------------------------------------------------------------------
/// This function enables or disables UART break IRQ.
///
/// @param id Identifier of the UART for which the function is called.
/// @param enable TRUE to enable break IRQ, FALSE otherwise.
// ============================================================================
PUBLIC VOID hal_UartIrqEnableBreakInt(HAL_UART_ID_T id, BOOL enable)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);

    // Uart not open
    if (!((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE))
    {
        return;
    }

    // Never update g_halUartPropArray[id-1].irqMask here

    // Set/clear IRQ mask
    if (enable)
    {
        g_halUartHwpArray[id-1]->irq_mask |= UART_RX_LINE_ERR;
    }
    else
    {
        g_halUartHwpArray[id-1]->irq_mask &= ~(UART_RX_LINE_ERR);
    }
}


// ============================================================================
// hal_UartOpen
// ----------------------------------------------------------------------------
/// This function enables the Uart in the mode defined by \c uartCfg
///
/// @param id Identifier of the UART for which the function is called.
/// @param uartCfg Uart configuration structure
// ============================================================================
PUBLIC VOID hal_UartOpen(HAL_UART_ID_T id, CONST HAL_UART_CFG_T* uartCfg)
{
    HAL_ASSERT(id >= HAL_UART_1 && id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    UINT32 uartClockDivisorMode = 4;
    UINT32 uartClockDivisor = 0;
    UINT32 fs, fsSys, mode;
    UINT32 clockToUse;
    register UINT32 uartConfig;

    // Initialize the UART configurable line to their
    // reset value, and reset Fifos.
    g_halUartHwpArray[id-1]->CMD_Clr = UART_RI
                                     | UART_DCD
                                     | UART_DSR 
                                     | UART_TX_BREAK_CONTROL
                                     | UART_TX_FINISH_N_WAIT
                                     | UART_RTS;

    // In breakIntWakeup mode, UART is always enabled to detect break condition,
    // so the FIFO might contain valid data
    if (!g_halUartPropArray[id-1].breakIntWakeup)
    {
        g_halUartHwpArray[id-1]->CMD_Set = UART_RX_FIFO_RESET
                                         | UART_TX_FIFO_RESET;
    }

    g_halUartPropArray[id-1].rxMode      = uartCfg->rx_mode;
    g_halUartPropArray[id-1].txMode      = uartCfg->tx_mode;
    g_halUartPropArray[id-1].rxIfcCh     = HAL_UNKNOWN_CHANNEL;
    g_halUartPropArray[id-1].txIfcCh     = HAL_UNKNOWN_CHANNEL;

    /// xfer id are consecutive  tx1-rx1-tx2-rx2, etc... therefore
    /// we can get all of them with the  2 x (uartid - 1), realtively]
    /// to uart1 transfer ids.
    g_halUartPropArray[id-1].rxRequestId = HAL_IFC_UART1_RX + 2*(id - 1);
    g_halUartPropArray[id-1].txRequestId = HAL_IFC_UART1_TX + 2*(id - 1);

    // In breakIntWakeup mode, UART is always enabled to detect break condition.
    if (!g_halUartPropArray[id-1].breakIntWakeup)
    {
        // Check that UART is not already opened or in force pins mode.
        // Then disable the force pins mode.
        HAL_ASSERT( (!(g_halUartHwpArray[id-1]->ctrl & UART_ENABLE) ||
                     (TRUE == g_halUartPropArray[id-1].forcingPins)),
                    "Uart already open");
    }

    g_halUartPropArray[id-1].forcingPins = FALSE;

    // Open without any direction enabled.
    HAL_ASSERT(!((uartCfg->rx_mode==HAL_UART_TRANSFERT_MODE_OFF)
        && (uartCfg->tx_mode==HAL_UART_TRANSFERT_MODE_OFF)),
        "Open without any direction enabled");
    
    //  set the resource as active.
    hal_SysRequestFreq(HAL_SYS_FREQ_UART, HAL_SYS_FREQ_26M, NULL);
    g_halUartPropArray[id-1].opened = TRUE;
    g_halUartPropArray[id-1].actived = TRUE;
    
    //  ------------------------------ 
    //  Build the configuration word. 
    //  ------------------------------ 
    //  Implicit in this config
    //         UART_TX_BREAK_CONTROL   = 0
    //         UART_RX_LOCK_ERR        = DISABLED
    //     
    uartConfig = UART_ENABLE | 
        UART_DMA_MODE | UART_RX_BREAK_LENGTH(13);
    
    //  Hidden feature to get the loopback mode. 
    if (uartCfg->afc & HAL_UART_AFC_LOOP_BACK)
    {
        uartConfig |= UART_LOOP_BACK_MODE;
    }
    
    if (uartCfg->data == HAL_UART_7_DATA_BITS)
        uartConfig |= UART_DATA_BITS_7_BITS;
    else
        uartConfig |= UART_DATA_BITS_8_BITS;
    
    if (uartCfg->stop == HAL_UART_1_STOP_BIT)
        uartConfig |= UART_TX_STOP_BITS_1_BIT;
    else
        uartConfig |= UART_TX_STOP_BITS_2_BITS;
    
    switch(uartCfg->parity)
    {
        case HAL_UART_ODD_PARITY:
            uartConfig |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_ODD;
            break;
        case HAL_UART_EVEN_PARITY:
            uartConfig |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_EVEN;
            break;
        case HAL_UART_SPACE_PARITY:
            uartConfig |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_SPACE;
            break;
        case HAL_UART_MARK_PARITY:
            uartConfig |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_MARK;
            break;
        case HAL_UART_NO_PARITY: 
        default:
            uartConfig |= UART_PARITY_ENABLE_NO;
            break;
    }
    
    if ((uartCfg->afc & AFC_MODE_MASK) == HAL_UART_AFC_MODE_DISABLE)
    {
        uartConfig |= UART_AUTO_FLOW_CONTROL_DISABLE;
    }
    else 
    {
        // Afc trigger level 
        uartConfig |= UART_AUTO_FLOW_CONTROL_ENABLE;
        g_halUartHwpArray[id-1]->triggers = UART_AFC_LEVEL(uartCfg->afc);
    }
    
    // New way to deal with rx and tx fifo trigger level, in
    // a dedicated 'trigger' register.
    g_halUartHwpArray[id-1]->triggers |= (UART_RX_TRIGGER(uartCfg->rx_trigger)
        | UART_TX_TRIGGER(uartCfg->tx_trigger));

    
    //  Baudrate with the system clock (uses a global variable). 
    if (uartCfg->irda == HAL_UART_IRDA_MODE_ENABLE)
    {
        
        //  Using mode divisor = 16 
        uartConfig |= UART_DIVISOR_MODE | UART_IRDA_ENABLE;
        uartClockDivisorMode = 16;
    }
    else
    {
        switch(uartCfg->rate)
        {
            
            //  Using mode divisor = 16 
            case HAL_UART_BAUD_RATE_2400:
            case HAL_UART_BAUD_RATE_4800:
                uartConfig |= UART_DIVISOR_MODE;
                uartClockDivisorMode = 16;
                break;
                //  Using mode divisor = 4 
            default:
                uartConfig &= ~UART_DIVISOR_MODE;
                uartClockDivisorMode = 4;
                break;
        }
    }
    
    //  Select which clock will be used. 
    switch(uartCfg->rate) 
    {
        
        //  Using the fast clock at 52MHz. 
    case HAL_UART_BAUD_RATE_1843200:
        fsSys = 156000000;
        clockToUse = SYS_CTRL_UART_SEL_PLL_PLL;
        break;
        
        //  Using the slow clock. 
    case HAL_UART_BAUD_RATE_921600:
    default:
        //  It is 13MHz or 26MHz. 
        fsSys = 26000000;
        clockToUse = SYS_CTRL_UART_SEL_PLL_SLOW;
        break;
    }
    
    //  Calculate the clock divider. 
    fs = uartCfg->rate;
    mode = uartClockDivisorMode;
    uartClockDivisor = ( (fsSys + ((mode / 2) * fs)) / (mode * fs) ) - 2;
    
    //  Configure the clock register. 
    hwp_sysCtrl->Cfg_Clk_Uart[id] = SYS_CTRL_UART_DIVIDER(uartClockDivisor) |
        clockToUse;
    
    //  Write the config word.
    g_halUartHwpArray[id-1]->ctrl = uartConfig;

    g_halUartHwpArray[id-1]->irq_mask = 0;
}


// ============================================================================
// hal_UartClose
// ----------------------------------------------------------------------------
/// This function closes the selected UART
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartClose(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);

    // In breakIntWakeup mode, UART is always enabled to detect break condition.
    if (g_halUartPropArray[id-1].breakIntWakeup)
    {
        if (g_halUartPropArray[id-1].irqMask.rxLineErr)
        {
            // Mask all UART IRQs except for break interrupt
            g_halUartHwpArray[id-1]->irq_mask = UART_RX_LINE_ERR;
        }
    }
    else
	{
        //  Turn off the UART. 
        g_halUartHwpArray[id-1]->ctrl      = 0;
        g_halUartHwpArray[id-1]->irq_mask  = 0;
        g_halUartHwpArray[id-1]->irq_cause = 0xffff;
    }

    //  Disable the Rx IFC Channel. 
    if ((IS_DMA(g_halUartPropArray[id-1].rxMode) != 0) &&
        (g_halUartPropArray[id-1].rxIfcCh != HAL_UNKNOWN_CHANNEL)) 
    {
        hal_IfcChannelRelease(g_halUartPropArray[id-1].rxRequestId,
            g_halUartPropArray[id-1].rxIfcCh);
    }

    //  Disable the Tx IFC Channel. 
    if ((IS_DMA(g_halUartPropArray[id-1].txMode) != 0) &&
        (g_halUartPropArray[id-1].txIfcCh != HAL_UNKNOWN_CHANNEL)) 
    {
        hal_IfcChannelRelease(g_halUartPropArray[id-1].txRequestId,
            g_halUartPropArray[id-1].txIfcCh);
    }

    // Re-init the driver variables.
    g_halUartPropArray[id-1].rxMode  = HAL_UART_TRANSFERT_MODE_OFF;
    g_halUartPropArray[id-1].txMode  = HAL_UART_TRANSFERT_MODE_OFF;

    g_halUartPropArray[id-1].rxIfcCh = HAL_UNKNOWN_CHANNEL;
    g_halUartPropArray[id-1].txIfcCh = HAL_UNKNOWN_CHANNEL;

    g_halUartPropArray[id-1].opened  = FALSE;
    g_halUartPropArray[id-1].actived = FALSE;

    {
        HAL_UART_ID_T i;
        BOOL enable = FALSE;
        for(i=0; i<HAL_UART_QTY-1; i++)
        {
            if(g_halUartPropArray[i].actived)
            {
                enable = TRUE;
                break;
            }
			else if (g_halUartPropArray[i].breakIntWakeup)
			{
				// Configure a fastest clock divider so as to detect
				// break condition when system is running in 32K clock
				hwp_sysCtrl->Cfg_Clk_Uart[id] = SYS_CTRL_UART_DIVIDER(0) |
					SYS_CTRL_UART_SEL_PLL_SLOW;
			}
        }
        if(!enable)
        {
            //  Release resource.
            hal_SysRequestFreq(HAL_SYS_FREQ_UART, HAL_SYS_FREQ_32K, NULL);
        }
    }

    if (!g_halUartPropArray[id-1].breakIntWakeup)
    {
        // Clear Irq Handler.
        g_halUartPropArray[id-1].irqMask.txModemStatus   = 0;
        g_halUartPropArray[id-1].irqMask.rxDataAvailable = 0;
        g_halUartPropArray[id-1].irqMask.txDataNeeded    = 0;
        g_halUartPropArray[id-1].irqMask.rxTimeout       = 0;
        g_halUartPropArray[id-1].irqMask.rxLineErr       = 0;
        g_halUartPropArray[id-1].irqMask.txDmaDone       = 0;
        g_halUartPropArray[id-1].irqMask.rxDmaDone       = 0;
        g_halUartPropArray[id-1].irqMask.rxDmaTimeout    = 0;

        g_halUartPropArray[id-1].irqHandler              = 0;
    }
}


// ============================================================================
// hal_UartSendData
// ----------------------------------------------------------------------------
/// This functions sends \c length bytes of data starting from the address 
/// \c data. The number returned is the number of bytes actually sent. In 
/// DMA mode, this function returns 0 when no DMA channel is available. 
/// Otherwise, it returns \c length.
///
/// @param id Identifier of the UART for which the function is called.
/// @param data Pointer on the buffer of data to send.
/// @param length Number of bytes to send.
/// @return Number of sent bytes or 0 if no DMA channel is available in case of
/// a DMA transfer.
// ============================================================================
PUBLIC UINT32 hal_UartSendData(HAL_UART_ID_T id, CONST UINT8* data, UINT32 length)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    UINT16 i;
    UINT32 free_room;

    if (hal_SysGetRequestFreq(HAL_SYS_FREQ_UART) == HAL_SYS_FREQ_32K)
    {
        //HAL_ASSERT(FALSE, "UART Send Data when resource not active");
        hal_SysRequestFreq(HAL_SYS_FREQ_UART, HAL_SYS_FREQ_26M, NULL);
    }
    
    //  Check the address pointer. 
    HAL_ASSERT((data != NULL) ,"Unitialized addr");
    
    //  Check if the UART is open 
    HAL_ASSERT(g_halUartHwpArray[id-1]->ctrl & UART_ENABLE ,"Uart not Open");
    
    if (!IS_DMA(g_halUartPropArray[id-1].txMode))
    {
        // -----------------------------
        // DIRECT TRANSFER
        // -----------------------------
        free_room = GET_BITFIELD(g_halUartHwpArray[id-1]->status,
            UART_TX_FIFO_SPACE);
        
        if (free_room > length)
        {
            free_room = length;
        }
        
        //  Send data byte by byte. 
        for (i = 0; i < free_room; i++)
        {
            hal_SendByte(g_halUartHwpArray[id-1], *(data + i));
        }
        
        return free_room;
    } 
    else  
    {
        // -----------------------------
        // DMA TRANSFER
        // -----------------------------
        //  Transfer size too big 
        HAL_ASSERT(length<EXP2(SYS_IFC_TC_LEN), "Transfer size too big");
        
        g_halUartPropArray[id-1].txIfcCh =
            hal_IfcTransferStart(g_halUartPropArray[id-1].txRequestId,
            (UINT8*)data, length, HAL_IFC_SIZE_8_MODE_AUTO);
        
        // get IFC channel and start if any available
        if (g_halUartPropArray[id-1].txIfcCh == HAL_UNKNOWN_CHANNEL)
        {
            // No channel available
            // No data received
            return 0;
        }
        else
        {
            // all data will be fetched
            return length;
        }
    }
}


// ============================================================================
// hal_UartTxDmaDone
// ----------------------------------------------------------------------------
/// This function checks if the DMA transmission is finished. 
/// Before sending new data in DMA mode, the previous DMA transfer must be 
/// finished, hence the use of this function for polling.
///
/// Note that the DMA transfer can be finished with an unempty Tx FIFO. Before 
/// shutting down the Uart, one must check that the uart FIFO is empty and that 
/// the last byte has been completely sent by using #hal_UartTxFinished. It
/// is not necessary to check the emptiness of the Tx FIFO to start a new DMA
/// transfer. A new DMA transfer is possible once the previous DMA transfer 
/// is over (And even is the Tx FIFO is not empty).
///
/// @param id Identifier of the UART for which the function is called.
/// @return \c TRUE if the last DMA transfer is finished. \c NO otherwise.
// ============================================================================
PUBLIC BOOL hal_UartTxDmaDone(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    //  Check that the DMA mode is active. 
    HAL_ASSERT(IS_DMA(g_halUartPropArray[id-1].txMode), "Uart not in dma mode");
    
    //  Check if the Uart is enabled. 
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE),
        "Uart not enabled");
    
    if (g_halUartPropArray[id-1].txIfcCh == HAL_UNKNOWN_CHANNEL)
    {
        return TRUE;
    }
    
    if ( g_halUartHwpArray[id-1]->irq_cause & (UART_TX_DMA_DONE_U) )
    {
        // clear done
        g_halUartPropArray[id-1].txIfcCh = HAL_UNKNOWN_CHANNEL;
        g_halUartHwpArray[id-1]->irq_cause = UART_TX_DMA_DONE;
        return TRUE;
    } 
    else 
    {
        return FALSE;
    }
}


// ============================================================================
// hal_UartTxFinished
// ----------------------------------------------------------------------------
/// This functions checks if the data transfer is completely finished before 
/// closing.
/// This function returns \c TRUE when the transmit FIFO is empty and when the 
/// last byte is completely sent. It should be called before closing the Uart 
/// if the last bytes of the transfer are important. 
///
/// This function should not be called between transfers, in direct or DMA mode.
/// The \link #hal_UartTxFifoAvailable FIFO available \endlink for direct 
/// mode and the \link #hal_UartTxDmaDone DMA done indication \endlink for 
/// DMA allow for a more optimized transmission.
///
/// @param id Identifier of the UART for which the function is called.
/// @return \c TRUE if the Tx FIFO is empty. \c NO otherwise
// ============================================================================
PUBLIC BOOL hal_UartTxFinished(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    //  Check if the Uart is enabled. 
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE),
        "Uart not enabled");
    
    UINT32 status = g_halUartHwpArray[id-1]->status;
   
    //  Check if there is no more data to be sent 
    if ((status & UART_TX_ACTIVE) ||
        (UART_TX_FIFO_SIZE - GET_BITFIELD(status,UART_TX_FIFO_SPACE) > 0))
    {
        return FALSE;
    }
    else 
    {
        return TRUE;
    }
}


// ============================================================================
// hal_UartGetData
// ----------------------------------------------------------------------------
/// This function gets \c length bytes from the Uart and stores them starting 
/// from the address \c destAddress. The number returned is the number of bytes 
/// actually received.
/// In DMA mode, this function returns 0 when no DMA channel is available. It 
/// returns length otherwise.
/// If you use the Rx Uart (to get some data from the Uart) in DMA mode, you 
/// will have to call the #hal_SysInvalidateCache function to invalidate the 
/// cache on your reception buffer. Please refer to the <B> Application Note 
/// 0019 "Cache and DMA Modules" </B> for more details.
///
/// @param id Identifier of the UART for which the function is called.
/// @param destAddress Pointer to a buffer to store the received data
/// @param length Number of byte to receive
/// @return The number of actually received bytes, or 0 when no DMA channel is 
/// available in case of a DMA transfer.
// ============================================================================
PUBLIC UINT32 hal_UartGetData(HAL_UART_ID_T id, UINT8* destAddress, UINT32 length)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    UINT32 i;
    UINT32 nbAvailable;

    if (hal_SysGetRequestFreq(HAL_SYS_FREQ_UART) == HAL_SYS_FREQ_32K)
    {
        //HAL_ASSERT(FALSE, "UART Get Data when resource not active");
        hal_SysRequestFreq(HAL_SYS_FREQ_UART, HAL_SYS_FREQ_26M, NULL);
    }
    
    //  Check the address pointer. 
    HAL_ASSERT((destAddress != NULL) , "Uninitialized dest addr");
    
    //  Check if the uart is open 
    HAL_ASSERT(g_halUartHwpArray[id-1]->ctrl & UART_ENABLE,
               "Uart has not been opened");
    
    if (!IS_DMA(g_halUartPropArray[id-1].rxMode))
    {
        // -----------------------------
        // DIRECT TRANSFER
        // -----------------------------
        nbAvailable = GET_BITFIELD(g_halUartHwpArray[id-1]->status,
                                   UART_RX_FIFO_LEVEL);
        
        if (nbAvailable > length)
        {
            nbAvailable = length;
        }
        
        //  Get data byte by byte.
        //  Workaround:
        //  Always read a byte from h/w to clear the IRQ, as
        //  h/w cannot handle the read timeout correctly.
        i = 0;
        do
        {
            destAddress[i] = hal_GetByte(g_halUartHwpArray[id-1]);
        } while (++i < nbAvailable);
        
        return nbAvailable;
    }
    else
    {
        // -----------------------------
        // DMA TRANSFER
        // -----------------------------
        //  Transfer size too big 
        HAL_ASSERT(length < EXP2(SYS_IFC_TC_LEN), "Transfer size too big");
        
        UINT32 status = hal_SysEnterCriticalSection();

        // (Re)Start transfert, this will reset the timeout counter.
        // Do this before clearing the mask and cause, to prevent 
        // a previous unwanted timeout interrupt to occur right between the 
        // two clears and then the restart transfert.

        g_halUartPropArray[id-1].rxIfcCh =
            hal_IfcTransferStart(g_halUartPropArray[id-1].rxRequestId,
            destAddress, length, HAL_IFC_SIZE_8_MODE_AUTO);
        
        // Enable the rxDmaTimeout IRQ, if wanted by the user.
        // Clear any pending rxDmaTimeout IRQ
        if (g_halUartPropArray[id-1].irqMask.rxDmaTimeout == 1)
        {
            g_halUartHwpArray[id-1]->irq_cause = UART_RX_DMA_TIMEOUT;
            g_halUartHwpArray[id-1]->irq_mask |= UART_RX_DMA_TIMEOUT;
        }
            
        hal_SysExitCriticalSection(status);

        // check if we got an IFC channel
        if (g_halUartPropArray[id-1].rxIfcCh == HAL_UNKNOWN_CHANNEL)
        {
            // No channel available
            // No data received
            return 0;
        }
        else
        {
            // all data will be fetched
            return length;
        }
    }
}


// ============================================================================
// hal_UartRxFifoLevel
// ----------------------------------------------------------------------------
/// This function returns the number of bytes in the Rx FIFO
///
/// @param id Identifier of the UART for which the function is called.
/// @returns The number of bytes in the Rx FIFO
// ============================================================================
PUBLIC UINT8 hal_UartRxFifoLevel(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    UINT8 rxLevel;
    
    //  Check if the Uart is enabled. 
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE), "Uart not enabled");
    
    //  Get level 
    rxLevel = GET_BITFIELD(g_halUartHwpArray[id-1]->status,UART_RX_FIFO_LEVEL);
    
    return rxLevel;
}


// ============================================================================
// hal_UartRxDmaDone
// ----------------------------------------------------------------------------
/// Check if the reception is finished. 
///
/// Before being able to receive new data in DMA mode, the previous transfer
/// must be finished, hence the use of this function for polling.
///
/// @param id Identifier of the UART for which the function is called.
/// @returns \c TRUE if the last DMA transfer is finished. \c NO otherwise
// ============================================================================
PUBLIC BOOL hal_UartRxDmaDone(HAL_UART_ID_T id)
{
    // Check that the UART id exists, is in DMA mode and is enabled.
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    HAL_ASSERT(IS_DMA(g_halUartPropArray[id-1].rxMode), "Uart not in dma mode");
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE), "Uart not enabled");
    
    if (g_halUartPropArray[id-1].rxIfcCh == HAL_UNKNOWN_CHANNEL)
    {
        return TRUE;
    }
    
    if (g_halUartHwpArray[id-1]->irq_cause & ( UART_RX_DMA_DONE_U))
    {
        // Clear done.
        g_halUartPropArray[id-1].rxIfcCh = HAL_UNKNOWN_CHANNEL;
        g_halUartHwpArray[id-1]->irq_cause = UART_RX_DMA_DONE;
        return TRUE;
    } 
    else 
    {
        return FALSE;
    }
}


// ============================================================================
// hal_UartRxDmaStop
// ----------------------------------------------------------------------------
/// This functions terminates the current Rx transfer and releases the
/// current Rx IFC DMA channel used by identified UART.
/// If the IFC channel fifo is not empty the transfer is not terminated.
/// This is useful when the DMA Rx Timeout rises.
///
/// @param id Identifier of the UART for which the function is called.
/// @return \c TRUE if the transfer has been released or if the current
/// UART doesn't have an IFC DMA channel attributed. \c FALSE if the
/// transfer cannot be released, i.e. the IFC DMA channel fifo is not empty.  
// ============================================================================
PUBLIC BOOL hal_UartRxDmaStop(HAL_UART_ID_T id)
{
    // Check that the UART id exists, is in DMA mode and is enabled.
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    HAL_ASSERT(IS_DMA(g_halUartPropArray[id-1].rxMode), "Uart not in dma mode");
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE), "Uart not enabled");
    
    // If the IFC channel fifo is not empty we don't stop the transfer.
    if (FALSE == hal_IfcChannelIsFifoEmpty(g_halUartPropArray[id-1].rxRequestId,
        g_halUartPropArray[id-1].rxIfcCh))
    {
        return FALSE;
    }

    // Release the IFC channel linked to the UART.
    hal_IfcChannelRelease(g_halUartPropArray[id-1].rxRequestId,
        g_halUartPropArray[id-1].rxIfcCh);
    
    // Disable the DMA Timeout IRQ from the UART only, not in the global 
    // variable so that the IRQ mask can be set again when starting a
    // new Rx DMA transfer.
    g_halUartHwpArray[id-1]->irq_mask &= ~UART_RX_DMA_TIMEOUT;

    g_halUartPropArray[id-1].rxIfcCh = HAL_UNKNOWN_CHANNEL;
    // Clear the rx DMA done cause
    g_halUartHwpArray[id-1]->irq_cause = UART_RX_DMA_DONE;
    
    return TRUE;
}    

// ============================================================================
// hal_UartRxDmaLevel
// ----------------------------------------------------------------------------
/// This function returns the number of free room remaining in the IFC DMA Rx
/// buffer. It can be useful to know when you can get data from the buffer. 
/// 
/// @param id Identifier of the UART for which the function is called.
/// @return Number of data (in bytes) remaining in the DMA buffer.
// ============================================================================
PUBLIC UINT32 hal_UartRxDmaLevel(HAL_UART_ID_T id)
{
    // Check that the UART id exists, is in DMA mode and is enabled.
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    HAL_ASSERT(IS_DMA(g_halUartPropArray[id-1].rxMode), "Uart not in dma mode");
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE), "Uart not enabled");
    
    // Get the TC from the current IFC channel.
    return hal_IfcGetTc(g_halUartPropArray[id-1].rxRequestId,
        g_halUartPropArray[id-1].rxIfcCh);
}


// ============================================================================
// hal_UartRxDmaFifoEmpty
// ----------------------------------------------------------------------------
/// This function returns \c TRUE when the fifo of the DMA is empty.
///
/// @param id Identifier of the UART for which the function is called.
/// @returns \c TRUE when the fifo of the DMA is empty.
// ============================================================================
PUBLIC BOOL hal_UartRxDmaFifoEmpty(HAL_UART_ID_T id)
{
    // Check that the UART id exists, is in DMA mode and is enabled.
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    HAL_ASSERT(IS_DMA(g_halUartPropArray[id-1].rxMode), "Uart not in dma mode");
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE), "Uart not enabled");
    
    // Return whether the TC from the current IFC channel is empty.
    return hal_IfcChannelIsFifoEmpty(g_halUartPropArray[id-1].rxRequestId,
        g_halUartPropArray[id-1].rxIfcCh);
}


// ============================================================================
// hal_UartRxDmaPurgeFifo
// ----------------------------------------------------------------------------
/// Sends all the remaining data in the IFC fifo to the RAM buffer.
///
/// After the IFC DMA channel has been purged, it is NOT released.
/// After calling this function, the software should wait for the
/// actual end of the purging the fifo, by calling the function
/// hal_UartRxDmaFifoEmpty().
/// After the IFC DMA channel has been purged (or is being purged),
/// the data from the UART will not be read by the IFC DMA channel
/// anymore. This means that after calling this function, the user must
/// stop the current transfer and restart a new transfer.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartRxDmaPurgeFifo(HAL_UART_ID_T id)
{
    // Check that the UART id exists, is in DMA mode and is enabled.
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    HAL_ASSERT(IS_DMA(g_halUartPropArray[id-1].rxMode), "Uart not in dma mode");
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE), "Uart not enabled");
    
    // Flush the fifo of the current IFC channel.
    hal_IfcChannelFlush(g_halUartPropArray[id-1].rxRequestId,
        g_halUartPropArray[id-1].rxIfcCh);
}


// ============================================================================
// hal_UartSetBreak
// ----------------------------------------------------------------------------
/// Enable or disable the break (hold the Tx line low). 
///
/// With this function, it is possible to generate a break: when the booLean 
/// parameter \c enable is set to \c TRUE, the Uart_Tx line is held low, thus 
/// generating a break on the line.  It is held low until this function is 
/// called again with the parameter \c enable set to \c FALSE.
///
/// @param id Identifier of the UART for which the function is called.
/// @param enable When set to \c TRUE, enables a break. When set to \c FALSE, 
/// disables a break.
// ============================================================================
PUBLIC VOID hal_UartSetBreak(HAL_UART_ID_T id, BOOL enable)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    //  Set the break mode. 
    if (enable)
    {
        g_halUartHwpArray[id-1]->CMD_Set = UART_TX_BREAK_CONTROL;
    }
    else
    {
        g_halUartHwpArray[id-1]->CMD_Clr = UART_TX_BREAK_CONTROL;
    }
}


// ============================================================================
// hal_UartGetErrorStatus
// ----------------------------------------------------------------------------
/// Gives the status of the UART. 
/// This function returns the line error status. The \c UINT16 value returned 
/// is a bitfield containing the errors detected during the transfer. The
/// errors can be: #HAL_ERR_UART_RX_OVERFLOW, #HAL_ERR_UART_TX_OVERFLOW, 
/// #HAL_ERR_UART_PARITY, #HAL_ERR_UART_FRAMING, or #HAL_ERR_UART_BREAK_INT.
///
/// @param id Identifier of the UART for which the function is called.
/// @return The line error status
// ============================================================================
PUBLIC HAL_ERR_T hal_UartGetErrorStatus(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    //  Return the status. 
    return (UINT16)( (g_halUartHwpArray[id-1]->status & (UART_RX_OVERFLOW_ERR |
        UART_TX_OVERFLOW_ERR | UART_RX_PARITY_ERR |
        UART_RX_FRAMING_ERR | UART_RX_BREAK_INT)) >> 8 );

}


// ============================================================================
// hal_UartClearErrorStatus
// ----------------------------------------------------------------------------
/// This function clears all the line error status. The error status is cleared 
/// automatically by the interrupt dispatcher code, but if you do not use the 
/// interrupt, the error status is not cleared. This means that if you check 
/// the error status manually (by polling it with the #hal_UartGetErrorStatus 
/// function), you'll need to call the #hal_UartClearErrorStatus function to 
/// clear it, manually.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartClearErrorStatus(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    // Clear the status. 
    // Any value written in the status register clear it
    g_halUartHwpArray[id-1]->status = 0x1;
}


// ============================================================================
// hal_UartSetRts
// ----------------------------------------------------------------------------
/// This function controls the RTS line of the UART. When the function is 
/// called with a \c ready parameter set to \c FALSE, that means that the CPU 
/// is not ready to handle the data of the UART. The remote device will stop 
/// sending data to the UART.
///
/// To resume the data transfer (i.e. to let the remote device send data to the 
/// UART again), call this function with a \c ready parameter set to \c TRUE.
///
/// If the hardware flow control is enabled, the RTS line will be controlled as 
/// follows: RTS Line Ready = CPU Ready AND UART Hardware Flow Control Ready. 
/// It means that if the UART or the CPU is not ready, the RTS line will ask 
/// the remote device to stop sending data.
///
/// When the flow control is handled by the #hal_UartSetRts function, it is 
/// called software flow control. The software flow control can be used in 
/// parallel to the hardware flow control. Please refer to previous chapters for 
/// details about @link #HAL_UART_AFC_MODE_T hardware flow control @endlink.
///
/// @param id Identifier of the UART for which the function is called.
/// @param ready Ready to receive data from the remote 
/// device (<CODE> ready = TRUE </CODE>) or not (<CODE> ready = FALSE </CODE>)
// ============================================================================
PUBLIC VOID hal_UartSetRts(HAL_UART_ID_T id, BOOL ready)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    //  Control the RTS. 
    if (ready)
    {
        g_halUartHwpArray[id-1]->CMD_Set = UART_RTS;
    }
    else
    {
        g_halUartHwpArray[id-1]->CMD_Clr = UART_RTS;
    }
}


// ============================================================================
// hal_UartGetCts
// ----------------------------------------------------------------------------
/// This function returns the value of the CTS line of the UART. When the 
/// function returns \c TRUE, that means that the remote peripheral is  
/// ready to receive data on its serial interface. The UART should stop sending 
/// data.
/// 
/// If the hardware flow control is enabled and the CTS line says that the 
/// remote device is not ready to receive data, the UART hardware will 
/// automatically stop sending data.
/// 
/// When the flow control is handled by the #hal_UartGetCts, it is called 
/// software flow control. The software flow control can be used in parallel to 
/// the hardware flow control. Please refer to previous chapters for details 
/// about @link #HAL_UART_AFC_MODE_T hardware flow control @endlink.
/// 
/// @param id Identifier of the UART for which the function is called.
/// @returns Value of the CTS line : \c TRUE if the remote device is ready to
/// receive data, \c FALSE otherwise.
// ============================================================================
PUBLIC BOOL hal_UartGetCts(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    //  Get the RTS. 
    return ((g_halUartHwpArray[id-1]->status & UART_CTS) == UART_CTS);
}


// ============================================================================
// hal_UartUartIsIdle
// ----------------------------------------------------------------------------
/// Check if an uart is busy before putting it to sleep 
/// (used once in pal...)
// ============================================================================
BOOL hal_UartUartIsIdle(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    UINT32 status = g_halUartHwpArray[id-1]->status;
    
    if ((status | UART_RX_FIFO_LEVEL_MASK | UART_TX_ACTIVE | UART_RX_ACTIVE) == 0 &&
        (UART_TX_FIFO_SIZE - GET_BITFIELD(status,UART_TX_FIFO_SPACE)) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// ============================================================================
// hal_UartIrqSetHandler
// ----------------------------------------------------------------------------
/// Set the user handler called when an Irq is triggered for the 
/// defined UART.
/// @param id Identifier of the UART for which the function is called.
/// @param handler User function called in case of an interrupt on this UART
/// occurs.
// ============================================================================
PUBLIC VOID hal_UartIrqSetHandler(HAL_UART_ID_T id, HAL_UART_IRQ_HANDLER_T handler)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    g_halUartPropArray[id-1].irqHandler = handler;
}


// ============================================================================
// hal_UartIrqGetHandler
// ----------------------------------------------------------------------------
/// Get the user handler
// ============================================================================
HAL_UART_IRQ_HANDLER_T hal_UartIrqGetHandler(HAL_UART_ID_T id) 
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    return g_halUartPropArray[id-1].irqHandler;
}


// ============================================================================
// hal_UartIrqSetMask
// ----------------------------------------------------------------------------
/// Set the IRQ mask for the specified uart
/// @param id Identifier of the UART for which the function is called.
/// @param mask The mask to apply
// ============================================================================
PUBLIC VOID hal_UartIrqSetMask(HAL_UART_ID_T id, HAL_UART_IRQ_STATUS_T mask)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    union {
        UINT32 reg; // used to write the full reg
        HAL_UART_IRQ_STATUS_T bitfield;
    } u;
    
    // Uart not open
    if (!((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE))
    {
        return;
    }
    
    g_halUartPropArray[id-1].irqMask  = mask;

    u.bitfield                        = mask;
    g_halUartHwpArray[id-1]->irq_mask = u.reg;
}


// ============================================================================
// hal_UartIrqGetMask
// ----------------------------------------------------------------------------
/// Get the IRQ mask set for an UART
/// @param id Identifier of the UART for which the function is called.
/// @return The Irq mask of this uart
// ============================================================================
PUBLIC HAL_UART_IRQ_STATUS_T hal_UartIrqGetMask(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    union {
        UINT32 reg; // used to write the full reg
        HAL_UART_IRQ_STATUS_T bitfield;
    } u;
    
    u.reg = g_halUartHwpArray[id-1]->irq_mask;
    return (u.bitfield);
}


// ============================================================================
// hal_UartIrqHandler
// ----------------------------------------------------------------------------
/// UART module IRQ handler
/// Clear the IRQ and call the IRQ handler user function
/// @param interruptId The interruption id
// ============================================================================
PROTECTED VOID hal_UartIrqHandler(UINT8 interruptId)
{
    // Thinking about it, this formula is true:
    // if interruptid is the one of UART1, we have 1, 
    // and for UART2, it is 2, etc
    HAL_UART_ID_T id = interruptId -  SYS_IRQ_UART + 1;

    // TODO: set according to the interruptId
    
    union {
        UINT32 reg; // used to write the full reg
        HAL_UART_IRQ_STATUS_T bitfield;
    } cause;
    
    union {
        UINT32 reg;
        HAL_UART_ERROR_STATUS_T bitfield;
    } status;
    
    // Get IRQ cause
    cause.reg   = g_halUartHwpArray[id-1]->irq_cause;
    // Get Error Status
    status.reg  = g_halUartHwpArray[id-1]->status;
    // Clear the cause
    g_halUartHwpArray[id-1]->irq_cause = cause.reg;
    // Clear the Error status
    // Writing any value in this register will clear them all
    g_halUartHwpArray[id-1]->status = 0x1;
    
    if (cause.bitfield.rxDmaDone)
    {
        g_halUartPropArray[id-1].rxIfcCh = HAL_UNKNOWN_CHANNEL;
        // Mask DMA timeout IRQ and clear the reporting
        g_halUartHwpArray[id-1]->irq_mask &= ~(UART_RX_DMA_TIMEOUT);
        cause.bitfield.rxDmaTimeout=0;
    }

    if (cause.bitfield.txDmaDone)
    {
        g_halUartPropArray[id-1].txIfcCh = HAL_UNKNOWN_CHANNEL;
    }

    if(g_halUartPropArray[id-1].irqHandler)
    {
        g_halUartPropArray[id-1].irqHandler(cause.bitfield,status.bitfield);
    }
}


// ============================================================================
// hal_UartAllowSleep
// ----------------------------------------------------------------------------
/// That function is called to signify that the uart needs allow or not 
/// the system to go into lowpower.
/// @param id Identifier of the UART for which the function is called.
/// @param allow If \c TRUE, the system is allowed to go into lowpower mode. \n
///              If \c FALSE, this prevents the system from entering in
///              lowpower mode.
// ============================================================================
PUBLIC VOID hal_UartAllowSleep(HAL_UART_ID_T id, BOOL allow)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);

    g_halUartPropArray[id-1].actived = !allow;

    if(!allow)
        hal_SysRequestFreq(HAL_SYS_FREQ_UART, HAL_SYS_FREQ_26M, NULL);
    else
    {
        HAL_UART_ID_T i;
        BOOL enable = FALSE;
        for(i=0; i<HAL_UART_QTY-1; i++)
        {
            if(g_halUartPropArray[i].actived)
            {
                enable = TRUE;
                break;
            }
        }
        if(!enable)
            hal_SysRequestFreq(HAL_SYS_FREQ_UART, HAL_SYS_FREQ_32K, NULL);
     }
}


// ============================================================================
// hal_UartSetRi
// ----------------------------------------------------------------------------
/// Sets the RI bit to the value given as a parameter
///
/// @param id Identifier of the UART for which the function is called.
/// @param value If \c TRUE, the RI bit is set.
///              If \c FALSE, the RI bit is cleared.
// ============================================================================
PUBLIC VOID hal_UartSetRi(HAL_UART_ID_T id, BOOL value)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    if (value == TRUE) 
    {
        g_halUartHwpArray[id-1]->CMD_Set = UART_RI;
    }
    else
    {
        g_halUartHwpArray[id-1]->CMD_Clr = UART_RI;
    }
}


// ============================================================================
// hal_UartSetDsr
// ----------------------------------------------------------------------------
/// Sets the DSR bit to the value given as a parameter
///
/// @param id Identifier of the UART for which the function is called.
/// @param value If \c TRUE, the DSR bit is set.
///              If \c FALSE, the DSR bit is cleared.
// ============================================================================
PUBLIC VOID hal_UartSetDsr(HAL_UART_ID_T id, BOOL value)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    if (value == TRUE) 
    {
        g_halUartHwpArray[id-1]->CMD_Set = UART_DSR;
    }
    else
    {
        g_halUartHwpArray[id-1]->CMD_Clr = UART_DSR;
    }
}


// ============================================================================
// hal_UartSetDcd
// ----------------------------------------------------------------------------
/// Sets the DCD bit to the value given as a parameter
///
/// @param id Identifier of the UART for which the function is called.
/// @param value If \c TRUE, the DCD bit is set.
///              If \c FALSE, the DCD bit is cleared.
// ============================================================================
PUBLIC VOID hal_UartSetDcd(HAL_UART_ID_T id, BOOL value)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    if (value == TRUE) 
    {
        g_halUartHwpArray[id-1]->CMD_Set = UART_DCD;
    }
    else
    {
        g_halUartHwpArray[id-1]->CMD_Clr = UART_DCD;
    }
}


// ============================================================================
// hal_UartGetDtr
// ----------------------------------------------------------------------------
/// Read the current value of the DTR line
/// @param id Identifier of the UART for which the function is called.
/// @return The current value of the DTR line. (\c TRUE or \c FALSE)
// ============================================================================
PUBLIC BOOL hal_UartGetDtr(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    if (g_halUartHwpArray[id-1]->status & UART_DTR)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// ============================================================================
// hal_UartTxFifoAvailable
// ----------------------------------------------------------------------------
/// This function returns the number of bytes available in the TX FIFO
/// 
/// @param id Identifier of the UART for which the function is called.
/// @return The number of available bytes in the TX FIFO.
// ============================================================================
PUBLIC UINT8 hal_UartTxFifoAvailable(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    UINT8 txLevel;
    
    //  Check if the Uart is enabled. 
    HAL_ASSERT(((g_halUartHwpArray[id-1]->ctrl) & UART_ENABLE), "Uart not enabled");
    
    //  Get level 
    txLevel = GET_BITFIELD(g_halUartHwpArray[id-1]->status,UART_TX_FIFO_SPACE);
    
    return txLevel;
}


// ============================================================================
// hal_UartFifoFlush
// ----------------------------------------------------------------------------
/// This function resets the reception and the transmission FIFOs. All bytes 
/// that are waiting to be sent in the Tx FIFO and that have been received, 
/// waiting to be read in the Rx FIFO, will be removed.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartFifoFlush(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    g_halUartHwpArray[id-1]->CMD_Set = UART_RX_FIFO_RESET | UART_TX_FIFO_RESET;
}


// ============================================================================
// hal_UartForcePinsLow
// ----------------------------------------------------------------------------
/// This function force all output pins of the UART to low. This can be
/// called for any UART it will only change the state of the pins actually
/// configured in UART mode (IO config).
///
/// This function must only be called when the UART is closed.
///
/// @param id Identifier of the UART for which the function is called.
// ============================================================================
PUBLIC VOID hal_UartForcePinsLow(HAL_UART_ID_T id)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);
    
    HAL_ASSERT(!(g_halUartHwpArray[id-1]->ctrl & UART_ENABLE),
             "%s cannot be called while uart %d is enabled",
             __FUNCTION__, id);

    // Set the GPIO-like pins to low.
    g_halUartHwpArray[id-1]->CMD_Clr = (UART_RI | UART_DCD | UART_DSR);
    // RTS is active low
    g_halUartHwpArray[id-1]->CMD_Set = (UART_RTS);

    // Enable the UART and set it in break mode to force the TX line to low.
    g_halUartHwpArray[id-1]->ctrl = UART_ENABLE;
        
    // Enable the TX break.
    g_halUartHwpArray[id-1]->CMD_Set = UART_TX_BREAK_CONTROL | 
        UART_RX_FIFO_RESET | UART_TX_FIFO_RESET;
    
    // Keeping the force pins status.
    g_halUartPropArray[id-1].forcingPins = TRUE;
}


