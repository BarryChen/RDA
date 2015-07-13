//==============================================================================
//                                                                              
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.              
//                            All Rights Reserved                               
//                                                                              
//      This source code is the property of Coolsand Technologies and is        
//      confidential.  Any  modification, distribution,  reproduction or        
//      exploitation  of  any content of this file is totally forbidden,        
//      except  with the  written permission  of  Coolsand Technologies.        
//                                                                              
//==============================================================================
//                                                                              
//    THIS FILE WAS GENERATED FROM ITS CORRESPONDING XML VERSION WITH COOLXML.  
//                                                                              
//                       !!! PLEASE DO NOT EDIT !!!                             
//                                                                              
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/regs/gallite/include/sys_irq.h $                                                                   
//  $Author: huazeng $                                                                    
//  $Date: 2011-12-02 15:56:10 +0800 (周五, 02 十二月 2011) $                                                                      
//  $Revision: 12275 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _SYS_IRQ_H_
#define _SYS_IRQ_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'sys_irq'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// XCPU_IRQ_LINES_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
/// XCPU Irq Lines
    XCPU_MAIN_IRQ_LINE                          = 0x00000000,
    XCPU_PAGE_SPY_IRQ_LINE                      = 0x00000003,
    XCPU_DEBUG_IRQ_LINE                         = 0x00000004,
    XCPU_HOST_IRQ_LINE                          = 0x00000005
} XCPU_IRQ_LINES_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SYS_IRQ_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_SYS_IRQ_BASE            0x01A01000

typedef volatile struct
{
    /// If cause is not null and interrupt are enabled then the interrupt line 0
    /// is driven on the system CPU. 
    /// The cause for the Irq sources, one bit for each module's irq source. 
    /// The cause is the actual Irq source masked by the mask register.
    REG32                          Cause;                        //0x00000000
    /// The status for the level Irq sources, one bit for each module's irq source.
    /// 
    /// The status reflect the actual Irq source.
    REG32                          Status;                       //0x00000004
    /// Writing '1' sets the corresponding bit in the mask register to '1'. 
    /// Reading gives the value of the mask register.
    REG32                          Mask_Set;                     //0x00000008
    /// Writing '1' clears the corresponding bit in the mask register to '0'. 
    /// Reading gives the value of the mask register.
    REG32                          Mask_Clear;                   //0x0000000C
    REG32                          NonMaskable;                  //0x00000010
    REG32                          SC;                           //0x00000014
    /// Each bit to '1' in that registers allows the correcponding interrupt to wake
    /// up the System CPU (i.e.: Reenable it's clock, see CLOCK_SYS_ENABLE and CLOCK_SYS_DISABLE
    /// registers in sys_ctrl registers section)
    REG32                          WakeUp_Mask;                  //0x00000018
    REG32                          Cpu_Sleep;                    //0x0000001C
    /// Writing '1' sets the corresponding bit in the mask register to '1'. 
    /// Reading gives the value of the mask register.
    REG32                          Pulse_Mask_Set;               //0x00000020
    /// Writing '1' clears the corresponding bit in the mask register to '0'. 
    /// Reading gives the value of the mask register.
    REG32                          Pulse_Mask_Clr;               //0x00000024
    /// Writing '1' clears the corresponding Pulse IRQ. 
    /// Pulse IRQ are set by the modules and cleared here.
    REG32                          Pulse_Clear;                  //0x00000028
    /// The status for the Pulse Irq sources, one bit for each module's irq source.
    /// 
    /// The status reflect the actual Irq source.
    REG32                          Pulse_Status;                 //0x0000002C
} HWP_SYS_IRQ_T;

#define hwp_sysIrq                  ((HWP_SYS_IRQ_T*) KSEG1(REG_SYS_IRQ_BASE))


//Cause
#define SYS_IRQ_SYS_IRQ_TCU0        (1<<0)
#define SYS_IRQ_SYS_IRQ_TCU1        (1<<1)
#define SYS_IRQ_SYS_IRQ_FRAME       (1<<2)
#define SYS_IRQ_SYS_IRQ_COM0        (1<<3)
#define SYS_IRQ_SYS_IRQ_COM1        (1<<4)
#define SYS_IRQ_SYS_IRQ_VOC         (1<<5)
#define SYS_IRQ_SYS_IRQ_DMA         (1<<6)
#define SYS_IRQ_SYS_IRQ_GPIO        (1<<7)
#define SYS_IRQ_SYS_IRQ_KEYPAD      (1<<8)
#define SYS_IRQ_SYS_IRQ_TIMERS      (1<<9)
#define SYS_IRQ_SYS_IRQ_OS_TIMER    (1<<10)
#define SYS_IRQ_SYS_IRQ_CALENDAR    (1<<11)
#define SYS_IRQ_SYS_IRQ_SPI1        (1<<12)
#define SYS_IRQ_SYS_IRQ_SPI2        (1<<13)
#define SYS_IRQ_SYS_IRQ_SPI3        (1<<14)
#define SYS_IRQ_SYS_IRQ_DEBUG_UART  (1<<15)
#define SYS_IRQ_SYS_IRQ_UART        (1<<16)
#define SYS_IRQ_SYS_IRQ_UART2       (1<<17)
#define SYS_IRQ_SYS_IRQ_I2C         (1<<18)
#define SYS_IRQ_SYS_IRQ_I2C2        (1<<19)
#define SYS_IRQ_SYS_IRQ_I2C3        (1<<20)
#define SYS_IRQ_SYS_IRQ_SCI         (1<<21)
#define SYS_IRQ_SYS_IRQ_RF_SPI      (1<<22)
#define SYS_IRQ_SYS_IRQ_LPS         (1<<23)
#define SYS_IRQ_SYS_IRQ_BBIFC0      (1<<24)
#define SYS_IRQ_SYS_IRQ_BBIFC1      (1<<25)
#define SYS_IRQ_SYS_IRQ_USBC        (1<<26)
#define SYS_IRQ_SYS_IRQ_GOUDA       (1<<27)
#define SYS_IRQ_SYS_IRQ_SDMMC       (1<<28)
#define SYS_IRQ_SYS_IRQ_CAMERA      (1<<29)
#define SYS_IRQ_SYS_IRQ_GPADC       (1<<30)
#define SYS_IRQ_SYS_IRQ_PMU         (1<<31)
#define SYS_IRQ_CAUSE(n)            (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_CAUSE_MASK          (0xFFFFFFFF<<0)
#define SYS_IRQ_CAUSE_SHIFT         (0)

//Status
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0      (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1      (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC       (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA       (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO      (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD    (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS    (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER  (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR  (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1      (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2      (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3      (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART      (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2     (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C       (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2      (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3      (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI       (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI    (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS       (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0    (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1    (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC      (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA     (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC     (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA    (1<<29)
//#define SYS_IRQ_SYS_IRQ_GPADC     (1<<30)
//#define SYS_IRQ_SYS_IRQ_PMU       (1<<31)
#define SYS_IRQ_STATUS(n)           (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_STATUS_MASK         (0xFFFFFFFF<<0)
#define SYS_IRQ_STATUS_SHIFT        (0)

//Mask_Set
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0      (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1      (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC       (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA       (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO      (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD    (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS    (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER  (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR  (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1      (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2      (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3      (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART      (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2     (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C       (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2      (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3      (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI       (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI    (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS       (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0    (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1    (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC      (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA     (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC     (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA    (1<<29)
//#define SYS_IRQ_SYS_IRQ_GPADC     (1<<30)
//#define SYS_IRQ_SYS_IRQ_PMU       (1<<31)
#define SYS_IRQ_MASK_SET(n)         (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_MASK_SET_MASK       (0xFFFFFFFF<<0)
#define SYS_IRQ_MASK_SET_SHIFT      (0)

//Mask_Clear
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0      (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1      (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC       (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA       (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO      (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD    (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS    (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER  (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR  (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1      (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2      (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3      (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART      (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2     (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C       (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2      (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3      (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI       (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI    (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS       (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0    (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1    (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC      (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA     (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC     (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA    (1<<29)
//#define SYS_IRQ_SYS_IRQ_GPADC     (1<<30)
//#define SYS_IRQ_SYS_IRQ_PMU       (1<<31)
#define SYS_IRQ_MASK_CLR(n)         (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_MASK_CLR_MASK       (0xFFFFFFFF<<0)
#define SYS_IRQ_MASK_CLR_SHIFT      (0)

//NonMaskable
#define SYS_IRQ_MAIN_IRQ            (1<<10)
#define SYS_IRQ_PAGE_SPY_IRQ        (1<<13)
#define SYS_IRQ_DEBUG_IRQ           (1<<14)
#define SYS_IRQ_HOST_IRQ            (1<<15)
#define SYS_IRQ_INTENABLE_STATUS    (1<<31)

//SC
#define SYS_IRQ_INTENABLE           (1<<0)

//WakeUp_Mask
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
//#define SYS_IRQ_SYS_IRQ_COM0      (1<<3)
//#define SYS_IRQ_SYS_IRQ_COM1      (1<<4)
//#define SYS_IRQ_SYS_IRQ_VOC       (1<<5)
//#define SYS_IRQ_SYS_IRQ_DMA       (1<<6)
//#define SYS_IRQ_SYS_IRQ_GPIO      (1<<7)
//#define SYS_IRQ_SYS_IRQ_KEYPAD    (1<<8)
//#define SYS_IRQ_SYS_IRQ_TIMERS    (1<<9)
//#define SYS_IRQ_SYS_IRQ_OS_TIMER  (1<<10)
//#define SYS_IRQ_SYS_IRQ_CALENDAR  (1<<11)
//#define SYS_IRQ_SYS_IRQ_SPI1      (1<<12)
//#define SYS_IRQ_SYS_IRQ_SPI2      (1<<13)
//#define SYS_IRQ_SYS_IRQ_SPI3      (1<<14)
//#define SYS_IRQ_SYS_IRQ_DEBUG_UART (1<<15)
//#define SYS_IRQ_SYS_IRQ_UART      (1<<16)
//#define SYS_IRQ_SYS_IRQ_UART2     (1<<17)
//#define SYS_IRQ_SYS_IRQ_I2C       (1<<18)
//#define SYS_IRQ_SYS_IRQ_I2C2      (1<<19)
//#define SYS_IRQ_SYS_IRQ_I2C3      (1<<20)
//#define SYS_IRQ_SYS_IRQ_SCI       (1<<21)
//#define SYS_IRQ_SYS_IRQ_RF_SPI    (1<<22)
//#define SYS_IRQ_SYS_IRQ_LPS       (1<<23)
//#define SYS_IRQ_SYS_IRQ_BBIFC0    (1<<24)
//#define SYS_IRQ_SYS_IRQ_BBIFC1    (1<<25)
//#define SYS_IRQ_SYS_IRQ_USBC      (1<<26)
//#define SYS_IRQ_SYS_IRQ_GOUDA     (1<<27)
//#define SYS_IRQ_SYS_IRQ_SDMMC     (1<<28)
//#define SYS_IRQ_SYS_IRQ_CAMERA    (1<<29)
//#define SYS_IRQ_SYS_IRQ_GPADC     (1<<30)
//#define SYS_IRQ_SYS_IRQ_PMU       (1<<31)
#define SYS_IRQ_WAKEUP_MASK(n)      (((n)&0xFFFFFFFF)<<0)
#define SYS_IRQ_WAKEUP_MASK_MASK    (0xFFFFFFFF<<0)
#define SYS_IRQ_WAKEUP_MASK_SHIFT   (0)

//Cpu_Sleep
#define SYS_IRQ_SLEEP               (1<<0)

//Pulse_Mask_Set
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
#define SYS_IRQ_PULSE_MASK_SET(n)   (((n)&7)<<0)
#define SYS_IRQ_PULSE_MASK_SET_MASK (7<<0)
#define SYS_IRQ_PULSE_MASK_SET_SHIFT (0)

//Pulse_Mask_Clr
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
#define SYS_IRQ_PULSE_MASK_CLR(n)   (((n)&7)<<0)
#define SYS_IRQ_PULSE_MASK_CLR_MASK (7<<0)
#define SYS_IRQ_PULSE_MASK_CLR_SHIFT (0)

//Pulse_Clear
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
#define SYS_IRQ_PULSE_CLR(n)        (((n)&7)<<0)
#define SYS_IRQ_PULSE_CLR_MASK      (7<<0)
#define SYS_IRQ_PULSE_CLR_SHIFT     (0)

//Pulse_Status
//#define SYS_IRQ_SYS_IRQ_TCU0      (1<<0)
//#define SYS_IRQ_SYS_IRQ_TCU1      (1<<1)
//#define SYS_IRQ_SYS_IRQ_FRAME     (1<<2)
//#define SYS_IRQ_STATUS(n)         (((n)&7)<<0)
//#define SYS_IRQ_STATUS_MASK       (7<<0)
//#define SYS_IRQ_STATUS_SHIFT      (0)





#endif

