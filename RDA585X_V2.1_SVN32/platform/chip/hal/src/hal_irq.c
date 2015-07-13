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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_irq.c $ //
//    $Author: admin $                                                        // 
//    $Date: 2012-07-17 19:30:16 +0800 (周二, 17 七月 2012) $                     //   
//    $Revision: 16368 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_irq.c
/// IRQ module driver implementation
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "global_macros.h"
#include "sys_irq.h"

#include "chip_id.h"

#include "halp_irq.h"
#include "halp_gpio.h"
#include "halp_key.h"
#include "halp_timers.h"
#include "halp_calendar.h"
#include "halp_fint.h"
#include "halp_tcu.h"
#include "halp_uart.h"
#include "halp_dma.h"
#include "halp_sys_ifc.h"
#include "halp_sim.h"
#include "halp_spi.h"
#include "halp_lps.h"
#include "halp_comregs.h"
#include "halp_rfspi.h"
#include "halp_voc.h"
#include "halp_os_stub.h"
#include "halp_aif.h"
#include "halp_debug.h"
#include "halp_profile.h"
#include "halp_usb.h"
#include "halp_camera.h"
#include "halp_gouda.h"
#include "pmd_m.h"
//#include "halp_sdio.h"

#include "hal_debug.h"
#include "hal_mem_map.h"

// =============================================================================
//  MACROS
// =============================================================================

/// Highest priority 0 - no IRQ are re-enabled during treatment
#define MASK_PRIO_0_IRQS            (0)

#define PRIO_MASK_LPS_IRQ           (MASK_PRIO_0_IRQS)

#define PRIO_0_IRQS                 (SYS_IRQ_SYS_IRQ_LPS)

/// priority 1 - Priority 0 IRQs re-enabled
#define MASK_PRIO_1_IRQS            (MASK_PRIO_0_IRQS|PRIO_0_IRQS)

#ifdef RFSPI_IFC_WORKAROUND
// Allow RFSPI IFC config (TCU1 IRQ) during MCP locking time (TCU0 IRQ),
// so that RFSPI IFC config will never be delayed across the real FINT (TCU wrap event).
// Otherwise RFSPI cmds might NOT be sent to XCV strictly before the first rx win starts at
// next frame, resulting in data lost and wrong data timing. 
#define PRIO_MASK_TCU1_IRQ          (MASK_PRIO_1_IRQS)
#define PRIO_1_IRQS_OPTION_TCU1     (SYS_IRQ_SYS_IRQ_TCU1)
#else // !RFSPI_IFC_WORKAROUND
#define PRIO_1_IRQS_OPTION_TCU1     (0)
#endif // !RFSPI_IFC_WORKAROUND
#define PRIO_MASK_USB_IRQ           (MASK_PRIO_1_IRQS)
#define PRIO_1_IRQS                 (PRIO_1_IRQS_OPTION_TCU1|SYS_IRQ_SYS_IRQ_USBC)

/// priority 2 - Priority 1 and higher IRQs re-enabled
#define MASK_PRIO_2_IRQS            (MASK_PRIO_1_IRQS|PRIO_1_IRQS)

#if defined(PAL_WINDOWS_LOCK_MCP_ON_RX) || defined(DCDC_FREQ_DIV_WORKAROUND)
#define PRIO_MASK_TCU0_IRQ          (MASK_PRIO_2_IRQS)
#define PRIO_2_IRQS_OPTION_TCU0     (SYS_IRQ_SYS_IRQ_TCU0)
#else // !(PAL_WINDOWS_LOCK_MCP_ON_RX || DCDC_FREQ_DIV_WORKAROUND)
#define PRIO_2_IRQS_OPTION_TCU0     (0)
#endif // !(PAL_WINDOWS_LOCK_MCP_ON_RX || DCDC_FREQ_DIV_WORKAROUND)

#define PRIO_2_IRQS                 (PRIO_2_IRQS_OPTION_TCU0)

/// priority 3 - Priority 2 and higher IRQs re-enabled
#define MASK_PRIO_3_IRQS            (MASK_PRIO_2_IRQS|PRIO_2_IRQS)

#define PRIO_MASK_COM0_IRQ          (MASK_PRIO_3_IRQS)

#define PRIO_3_IRQS                 (SYS_IRQ_SYS_IRQ_COM0)

/// priority 4 - Priority 3 and higher IRQs re-enabled
#define MASK_PRIO_4_IRQS            (MASK_PRIO_3_IRQS|PRIO_3_IRQS)

#define PRIO_MASK_FRAME_IRQ         (MASK_PRIO_4_IRQS)

#define PRIO_4_IRQS                 (SYS_IRQ_SYS_IRQ_FRAME)

/// priority 5 - Priority 4 and higher IRQs re-enabled
#define MASK_PRIO_5_IRQS            (MASK_PRIO_4_IRQS|PRIO_4_IRQS)

#define PRIO_MASK_BBIFC0_IRQ        (MASK_PRIO_5_IRQS)
#define PRIO_MASK_BBIFC1_IRQ        (MASK_PRIO_5_IRQS)
#define PRIO_MASK_VOC_IRQ           (MASK_PRIO_5_IRQS)

#define PRIO_5_IRQS                 (SYS_IRQ_SYS_IRQ_BBIFC0| \
                                     SYS_IRQ_SYS_IRQ_BBIFC1| \
                                     SYS_IRQ_SYS_IRQ_VOC)

/// priority 6 - Priority 5 and higher IRQs re-enabled
#define MASK_PRIO_6_IRQS            (MASK_PRIO_5_IRQS|PRIO_5_IRQS)

#define PRIO_MASK_DMA_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_GPIO_IRQ          (MASK_PRIO_6_IRQS)
#define PRIO_MASK_TIMERS_IRQ        (MASK_PRIO_6_IRQS)
#define PRIO_MASK_OS_TIMER_IRQ      (MASK_PRIO_6_IRQS)
#define PRIO_MASK_CALENDAR_IRQ      (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SPI_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_TRACE_UART_IRQ    (MASK_PRIO_6_IRQS)
#define PRIO_MASK_UART_IRQ          (MASK_PRIO_6_IRQS)
#define PRIO_MASK_I2C_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SIM_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_KEYPAD_IRQ        (MASK_PRIO_6_IRQS)
#define PRIO_MASK_COM1_IRQ          (MASK_PRIO_6_IRQS)
#define PRIO_MASK_RTC_TIMER_IRQ     (MASK_PRIO_6_IRQS)
#define PRIO_MASK_RF_SPI_IRQ        (MASK_PRIO_6_IRQS)

#ifndef PRIO_MASK_TCU0_IRQ
#define PRIO_MASK_TCU0_IRQ          (MASK_PRIO_6_IRQS)
#endif // !PRIO_MASK_TCU0_IRQ

#ifndef PRIO_MASK_TCU1_IRQ
#define PRIO_MASK_TCU1_IRQ          (MASK_PRIO_6_IRQS)
#endif // !PRIO_MASK_TCU1_IRQ

//#define PRIO_MASK_USB_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SDMMC_DMA_TX_IRQ  (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SDMMC_DMA_RX_IRQ  (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SDMMC_IRQ         (MASK_PRIO_6_IRQS)
#define PRIO_MASK_GOUDA_IRQ         (MASK_PRIO_6_IRQS)
#define PRIO_MASK_CAMERA_IRQ        (MASK_PRIO_6_IRQS)
#define PRIO_MASK_PMU               (MASK_PRIO_6_IRQS)
#if (!CHIP_HAS_ASYNC_TCU)
#define PRIO_MASK_GPADC_IRQ         (MASK_PRIO_6_IRQS)
#else // (CHIP_HAS_ASYNC_TCU)
#define PRIO_MASK_SDMMC2_IRQ         (MASK_PRIO_6_IRQS)
#endif // (!CHIP_HAS_ASYNC_TCU)


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// g_halHwModuleIrqHandler
// -----------------------------------------------------------------------------
///     Physical interrupts mapping 
///     This order must match the order of the physical interrupts.
// =============================================================================
PRIVATE HAL_MODULE_IRQ_HANDLER_T g_halHwModuleIrqHandler[SYS_IRQ_QTY]
  = 
{
    hal_TcuIrqHandler,
    hal_TcuIrqHandler,
    NULL, //hal_FintIrqHandler,
    NULL, //hal_ComregsIrqHandler,
    NULL, //hal_ComregsIrqHandler,
    hal_VocIrqHandler,
    hal_DmaIrqHandler,
    hal_GpioIrqHandler,//0x80
    hal_KeyIrqHandler,
    NULL, //hal_TimTimersIrqHandler,
    hal_TimTickIrqHandler,
    hal_TimRtcIrqHandler,
    NULL, //hal_SpiIrqHandler,      // SPI   
    NULL, //hal_SpiIrqHandler,      // SPI2
    NULL, //hal_SpiIrqHandler,      // SPI3
    NULL, // No irq on hst_trace uart
    hal_UartIrqHandler,
    hal_UartIrqHandler,
    NULL, // reserved for i2c, no IRQ for it
    NULL, // reserved for i2c2, no IRQ for it
    NULL, // reserved for i2c3, no IRQ for it
    NULL, //hal_SimIrqHandler,
    NULL, //hal_RfspiIrqHandler,
    hal_LpsIrqHandler,
    hal_AifIrqHandler, // BBIFC0 handler
    hal_AifIrqHandler, // BBIFC1 handler
    hal_UsbIrqHandler,
#if (CHIP_HAS_GOUDA == 1)
    hal_GoudaIrqHandler, // GOUDA
#else
    NULL,
#endif
    NULL, // SDMMC
    NULL, //hal_CameraIrqHandler,
#if (!CHIP_HAS_ASYNC_TCU)
    NULL, // GPADC
    pmd_IrqHandler, // PMU
#else // (CHIP_HAS_ASYNC_TCU)
    pmd_IrqHandler, // PMU
    NULL, //hal_SdioIrqHandler, // SDMMC2
#endif // (CHIP_HAS_ASYNC_TCU)
};



// =============================================================================
// g_halIrqPriorityMask
// -----------------------------------------------------------------------------
/// Priority mask defining which IRQs have higher priorities than a given one.
// =============================================================================
PRIVATE CONST UINT32 g_halIrqPriorityMask[SYS_IRQ_QTY] =
//     =
{  //  LOGICAL IRQ 
    PRIO_MASK_TCU0_IRQ,             // TCU0 
    PRIO_MASK_TCU1_IRQ,             // TCU1 
    PRIO_MASK_FRAME_IRQ,            // FINT 
    PRIO_MASK_COM0_IRQ,             // comreg0/DSP/EQUALISER 
    PRIO_MASK_COM1_IRQ,             // comreg1/AUDIO/MSG 
    PRIO_MASK_VOC_IRQ,              // voc 
    PRIO_MASK_DMA_IRQ,              // dma 
    PRIO_MASK_GPIO_IRQ,             // GPIO 
    PRIO_MASK_KEYPAD_IRQ,           // KEYPAD 
    PRIO_MASK_TIMERS_IRQ,           // TICK_TIMER 
    PRIO_MASK_OS_TIMER_IRQ,         // ostick 
    PRIO_MASK_CALENDAR_IRQ,         // calendar/RTC timer
    PRIO_MASK_SPI_IRQ,              // SPI 
    PRIO_MASK_SPI_IRQ,              // SPI 2
    PRIO_MASK_SPI_IRQ,              // SPI 3
    PRIO_MASK_TRACE_UART_IRQ,       // trace uart 
    PRIO_MASK_UART_IRQ,             // UART
    PRIO_MASK_UART_IRQ,             // UART2
    PRIO_MASK_I2C_IRQ,              // I2C
    PRIO_MASK_I2C_IRQ,              // I2C 2
    PRIO_MASK_I2C_IRQ,              // I2C 3
    PRIO_MASK_SIM_IRQ,              // SIM 
    PRIO_MASK_RF_SPI_IRQ,           // RF-SPI 
    PRIO_MASK_LPS_IRQ,              // lps,
    PRIO_MASK_BBIFC0_IRQ,           // BBIFC0
    PRIO_MASK_BBIFC1_IRQ,           // BBIFC1
    PRIO_MASK_USB_IRQ,              // USB
    PRIO_MASK_GOUDA_IRQ,            // GOUDA
    PRIO_MASK_SDMMC_IRQ,            // SDMMC
    PRIO_MASK_CAMERA_IRQ,           // CAMERA
#if (!CHIP_HAS_ASYNC_TCU)
    PRIO_MASK_GPADC_IRQ,            // GPADC
    PRIO_MASK_PMU,                  // PMU
#else // (CHIP_HAS_ASYNC_TCU)
    PRIO_MASK_PMU,                  // PMU
    PRIO_MASK_SDMMC2_IRQ,           // SDMMC2
#endif // (CHIP_HAS_ASYNC_TCU)
};


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_IrqDispatch
// -----------------------------------------------------------------------------
/// IRQ dispatcher. Calls the interrupt handler according to their priorities
/// defined in the relevant macros and array.
// =============================================================================
PROTECTED VOID HAL_FUNC_INTERNAL hal_IrqDispatch(VOID)
{
    UINT32 cause = hwp_sysIrq->Cause;
    UINT32 bit = 1;
    UINT8 i;
    UINT32 unused __attribute__ ((unused));
    UINT32 mask;

    sxr_EnterScSchedule();
    
    // gestion Mask
    // save current Mask
    mask = hwp_sysIrq->Mask_Set;
    // Mask all IT
    hwp_sysIrq->Mask_Clear = SYS_IRQ_MASK_SET_MASK;
    // Re-enable irq    
    hwp_sysIrq->SC = 1;

    for(i=0;i<SYS_IRQ_QTY;i++)
    {
        if (cause & bit)
        {
            HAL_PROFILE_IRQ_ENTER(i);
            // clear irq bit of reenabled higher prio irqs as those will be treated 
            // by an other interrupt call
            cause &= ~g_halIrqPriorityMask[i];
            // Setup priority mask for this irq
            hwp_sysIrq->Mask_Set = g_halIrqPriorityMask[i];

            if(g_halHwModuleIrqHandler[i])
            {
                (g_halHwModuleIrqHandler[i])(i);
            }

            // Mask all IT
            hwp_sysIrq->Mask_Clear =SYS_IRQ_MASK_SET_MASK;
            
            HAL_PROFILE_IRQ_EXIT(i);
        }
        bit = bit << 1;
    }

    // Disable irq
    unused = hwp_sysIrq->SC;
    // Restore previous mask
    hwp_sysIrq->Mask_Set = mask; 

    sxr_ExitScSchedule();
}


// =============================================================================
// hal_XCpuSleep
// -----------------------------------------------------------------------------
/// Put the XCpu in sleep mode until next irq.
/// @todo see how mike used that one and named it...
// =============================================================================
PROTECTED VOID hal_XCpuSleep()
{
    hwp_sysIrq->WakeUp_Mask = hwp_sysIrq->Mask_Set;
    //hwp_sysIrq->Cpu_Sleep = (SYS_IRQ_SLEEP);
    {
        UINT32 flush_wr_buff __attribute__((unused)) = hwp_sysIrq->Cpu_Sleep;
    }
}

