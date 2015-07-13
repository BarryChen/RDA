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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/regs/gallite/include/sys_ctrl.h $                                                                   
//  $Author: huazeng $                                                                    
//  $Date: 2011-12-02 15:56:10 +0800 (周五, 02 十二月 2011) $                                                                      
//  $Revision: 12275 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _SYS_CTRL_H_
#define _SYS_CTRL_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'sys_ctrl'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// CPU_ID_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
/// CPU IDs
    XCPU                                        = 0x00000000,
    BCPU                                        = 0x00000001
} CPU_ID_T;


// ============================================================================
// SYS_CLKS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
/// System side System clocks
    SYS_XCPU                                    = 0x00000000,
    SYS_XCPU_INT                                = 0x00000001,
    SYS_PCLK_CONF                               = 0x00000002,
    SYS_PCLK_DATA                               = 0x00000003,
    SYS_AMBA                                    = 0x00000004,
    SYS_DMA                                     = 0x00000005,
    SYS_GOUDA                                   = 0x00000006,
    SYS_EBC                                     = 0x00000007,
    SYS_IFC_CH0                                 = 0x00000008,
    SYS_IFC_CH1                                 = 0x00000009,
    SYS_IFC_CH2                                 = 0x0000000A,
    SYS_IFC_CH3                                 = 0x0000000B,
    SYS_IFC_CH4                                 = 0x0000000C,
    SYS_IFC_CH5                                 = 0x0000000D,
    SYS_IFC_CH6                                 = 0x0000000E,
    SYS_IFC_CH7                                 = 0x0000000F,
    SYS_IFC_DBG                                 = 0x00000010,
    SYS_A2A                                     = 0x00000011,
    SYS_COM_REGS                                = 0x00000012,
    SYS_DEBUG_UART                              = 0x00000013,
    SYS_DBGHST                                  = 0x00000014,
    SYS_GPADC                                   = 0x00000015,
/// System side divided clock (either divided by module or by sys_ctrl)
    SYSD_SCI                                    = 0x00000016,
    SYSD_RF_SPI                                 = 0x00000017,
    SYSD_OSC                                    = 0x00000018,
    SYSD_PWM                                    = 0x00000019,
/// the following don't have an auto enable
    SYS_GPIO                                    = 0x0000001A,
    SYS_IRQ                                     = 0x0000001B,
    SYS_TCU                                     = 0x0000001C,
    SYS_TIMER                                   = 0x0000001D,
/// the following are sharing their enable
    SYS_SCI                                     = 0x0000001E,
/// keep last
    SYS_NOGATE                                  = 0x0000001F
} SYS_CLKS_T;

#define NB_SYS_CLK_XCPU                          (2)
#define NB_SYS_CLK_AEN                           (26)
#define NB_SYS_CLK_EN                            (30)
#define NB_SYS_CLK                               (32)

// ============================================================================
// PER_CLKS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
/// System side System clocks for Peripherals
    PER_UART                                    = 0x00000000,
    PER_UART2                                   = 0x00000001,
    PER_I2C                                     = 0x00000002,
    PER_I2C2                                    = 0x00000003,
    PER_I2C3                                    = 0x00000004,
    PER_SDMMC                                   = 0x00000005,
    PER_CAMERA                                  = 0x00000006,
/// System side divided clock (either divided by module or by sys_ctrl)
    PERD_SPI1                                   = 0x00000007,
    PERD_SPI2                                   = 0x00000008,
    PERD_SPI3                                   = 0x00000009,
    PER_USBC                                    = 0x0000000A,
    PER_SPY                                     = 0x0000000B,
    PER_TEST                                    = 0x0000000C
} PER_CLKS_T;

#define NB_PER_CLK_AEN                           (10)
#define NB_PER_CLK_EN                            (13)
#define NB_PER_CLK                               (13)

// ============================================================================
// BB_CLKS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
/// Baseband side System clocks
    BB_BCPU                                     = 0x00000000,
    BB_BCPU_INT                                 = 0x00000001,
    BB_AMBA                                     = 0x00000002,
    BB_PCLK_CONF                                = 0x00000003,
    BB_PCLK_DATA                                = 0x00000004,
    BB_VOC_AHB                                  = 0x00000005,
    BB_IFC_CH0                                  = 0x00000006,
    BB_IFC_CH1                                  = 0x00000007,
    BB_IFC_CH2                                  = 0x00000008,
    BB_IFC_CH3                                  = 0x00000009,
    BB_SRAM                                     = 0x0000000A,
    BB_A2A                                      = 0x0000000B,
    BB_ITLV                                     = 0x0000000C,
    BB_VITERBI                                  = 0x0000000D,
    BB_CIPHER                                   = 0x0000000E,
    BB_RF_IF                                    = 0x0000000F,
    BB_COPRO                                    = 0x00000010,
    BB_CP2_REG                                  = 0x00000011,
    BB_XCOR                                     = 0x00000012,
    BB_AIF                                      = 0x00000013,
/// Baseband side divided clock
/// divided BCK clock for DAI
    BBD_DAIS_208K                               = 0x00000014,
/// the following don't have an auto enable
    BB_IRQ                                      = 0x00000015
} BB_CLKS_T;

#define NB_BB_CLK_AEN                            (21)
#define NB_BB_CLK_EN                             (22)
#define NB_BB_CLK                                (22)
/// Other clocks
/// clocks with auto enble
/// the debug host clock auto enable is not used in host mode, only in uart mode
#define OC_HOST_UART                             (0)

// ============================================================================
// OTHER_CLKS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
    OC_DEBUG_UART                               = 0x00000000,
    OC_26M_GPADC                                = 0x00000001,
    OC_BCK                                      = 0x00000002,
    OC_UART                                     = 0x00000003,
    OC_UART2                                    = 0x00000004,
    OC_RF_RX                                    = 0x00000005,
    OC_RF_TX                                    = 0x00000006,
    OC_MEM_BRIDGE                               = 0x00000007,
    OC_VOC_CORE                                 = 0x00000008,
    OC_VOC_AHB                                  = 0x00000009,
    OC_VOC_DAI                                  = 0x0000000A,
/// the following don't have an auto enable
    OC_VOC                                      = 0x0000000B,
    OC_LPS                                      = 0x0000000C,
    OC_GPIO                                     = 0x0000000D,
    OC_USBPHY                                   = 0x0000000E,
    OC_PIX                                      = 0x0000000F,
    OC_CLK_OUT                                  = 0x00000010,
    OC_MEM_CLK_OUT                              = 0x00000011,
    OC_DBG                                      = 0x00000012
} OTHER_CLKS_T;

#define NB_OTHER_CLK_VOC_AEN_SYNC                (8)
#define NB_OTHER_CLK_AEN                         (11)
#define NB_OTHER_CLK_EN                          (19)
#define NB_OTHER_CLK                             (19)

// ============================================================================
// RESETS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
/// System side resets
    RST_XCPU                                    = 0x00000000,
    RST_SYS_IRQ                                 = 0x00000001,
    RST_SYS_A2A                                 = 0x00000002,
    RST_ABB                                     = 0x00000003,
    RST_DMA                                     = 0x00000004,
    RST_GOUDA                                   = 0x00000005,
    RST_TIMER                                   = 0x00000006,
    RST_TCU                                     = 0x00000007,
    RST_GPIO                                    = 0x00000008,
    RST_KEYPAD                                  = 0x00000009,
    RST_PWM                                     = 0x0000000A,
    RST_UART                                    = 0x0000000B,
    RST_UART2                                   = 0x0000000C,
    RST_SPI1                                    = 0x0000000D,
    RST_SPI2                                    = 0x0000000E,
    RST_SPI3                                    = 0x0000000F,
    RST_RF_SPI                                  = 0x00000010,
    RST_SCI                                     = 0x00000011,
    RST_SPY                                     = 0x00000012,
    RST_I2C                                     = 0x00000013,
    RST_I2C2                                    = 0x00000014,
    RST_I2C3                                    = 0x00000015,
    RST_MEM_BRIDGE                              = 0x00000016,
    RST_GPADC                                   = 0x00000017,
    RST_EXT_APB                                 = 0x00000018,
    RST_SDMMC                                   = 0x00000019,
    RST_CAMERA                                  = 0x0000001A,
    RST_USBC                                    = 0x0000001B,
    RST_USBPHY                                  = 0x0000001C,
    RST_DBG                                     = 0x0000001D,
    RST_BCPU                                    = 0x0000001E,
    RST_BB_IRQ                                  = 0x0000001F,
    RST_BB_A2A                                  = 0x00000020,
    RST_BB_IFC                                  = 0x00000021,
    RST_BB_SRAM                                 = 0x00000022,
    RST_ITLV                                    = 0x00000023,
    RST_VITERBI                                 = 0x00000024,
    RST_CIPHER                                  = 0x00000025,
    RST_XCOR                                    = 0x00000026,
    RST_COPRO                                   = 0x00000027,
    RST_RF_IF                                   = 0x00000028,
    RST_VOC                                     = 0x00000029,
    RST_AIF                                     = 0x0000002A,
    RST_DAI                                     = 0x0000002B,
    RST_BB_FULL                                 = 0x0000002C,
    RST_SYS_FULL                                = 0x0000002D
} RESETS_T;

#define NB_SRST                                  (30)
/// Baseband side resets
#define BOUND_BRST_FIRST                         (30)
#define BOUND_BRST_AFTER                         (44)
/// The following reset does not have register
#define NR_RST_REG                               (45)
#define NB_RST                                   (46)
#define NB_BRST                                  (BOUND_BRST_AFTER-BOUND_BRST_FIRST)

// ============================================================================
// RESET_OTHERS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
/// Reset Other : resync on corresponding clock other
    RSTO_DBG_HOST                               = 0x00000000,
    RSTO_GPADC                                  = 0x00000001,
    RSTO_BCK                                    = 0x00000002,
    RSTO_UART                                   = 0x00000003,
    RSTO_UART2                                  = 0x00000004,
    RSTO_RF_RX                                  = 0x00000005,
    RSTO_RF_TX                                  = 0x00000006,
    RSTO_MEM_BRIDGE                             = 0x00000007,
    RSTO_DBG                                    = 0x00000008,
    RSTO_VOC_DAI                                = 0x00000009,
    RSTO_VOC                                    = 0x0000000A,
    RSTO_LPS                                    = 0x0000000B,
    RSTO_GPIO                                   = 0x0000000C,
    RSTO_WDTIMER                                = 0x0000000D,
    RSTO_USBPHY                                 = 0x0000000E
} RESET_OTHERS_T;

#define BOUND_RSTO_RF_FIRST                      (5)
#define BOUND_RSTO_RF_AFTER                      (7)
#define NB_RSTO                                  (15)
/// For REG_DBG protect lock/unlock value
#define SYS_CTRL_PROTECT_LOCK                    (0XA50000)
#define SYS_CTRL_PROTECT_UNLOCK                  (0XA50001)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SYS_CTRL_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_SYS_CTRL_BASE           0x01A00000

typedef volatile struct
{
    /// <strong>This register is used to Lock and Unlock the protected registers.</strong>
    REG32                          REG_DBG;                      //0x00000000
    /// This register is protected.
    REG32                          Sys_Rst_Set;                  //0x00000004
    REG32                          Sys_Rst_Clr;                  //0x00000008
    /// This register is protected.
    REG32                          BB_Rst_Set;                   //0x0000000C
    REG32                          BB_Rst_Clr;                   //0x00000010
    REG32                          Clk_Sys_Mode;                 //0x00000014
    REG32                          Clk_Sys_Enable;               //0x00000018
    /// This register is protected.
    REG32                          Clk_Sys_Disable;              //0x0000001C
    REG32                          Clk_Per_Mode;                 //0x00000020
    REG32                          Clk_Per_Enable;               //0x00000024
    /// This register is protected.
    REG32                          Clk_Per_Disable;              //0x00000028
    REG32                          Clk_BB_Mode;                  //0x0000002C
    REG32                          Clk_BB_Enable;                //0x00000030
    /// This register is protected.
    REG32                          Clk_BB_Disable;               //0x00000034
    REG32                          Clk_Other_Mode;               //0x00000038
    REG32                          Clk_Other_Enable;             //0x0000003C
    /// This register is protected.
    REG32                          Clk_Other_Disable;            //0x00000040
    /// Register protected by Write_Unlocked_H.
    REG32                          Pll_Ctrl;                     //0x00000044
    /// This register is protected.
    REG32                          Sel_Clock;                    //0x00000048
    REG32                          Cfg_Clk_Sys;                  //0x0000004C
    REG32                          Cfg_Clk_Mem_Bridge;           //0x00000050
    REG32                          Cfg_Clk_Voc;                  //0x00000054
    /// This register is protected.
    REG32                          Cfg_Clk_Out;                  //0x00000058
    REG32                          Cfg_Clk_GPADC;                //0x0000005C
    REG32                          Cfg_Clk_AudioBCK_Div;         //0x00000060
    REG32                          Cfg_Aif_Tx_Stb_Div;           //0x00000064
    REG32                          Cfg_Clk_Uart[3];              //0x00000068
    REG32                          Cfg_Clk_PWM;                  //0x00000074
    REG32                          Cfg_Clk_Auxclk;               //0x00000078
    REG32                          Cfg_Clk_Dbg_Div;              //0x0000007C
    REG32                          Cfg_Clk_Camera_Out;           //0x00000080
    /// This register is protected.
    REG32                          Cfg_AHB;                      //0x00000084
    /// This register is protected. Used to unsplit masters manualy.
    REG32                          Ctrl_AHB;                     //0x00000088
    REG32                          XCpu_Dbg_BKP;                 //0x0000008C
    REG32                          XCpu_Dbg_Addr;                //0x00000090
    REG32                          BCpu_Dbg_BKP;                 //0x00000094
    REG32                          BCpu_Dbg_Addr;                //0x00000098
    REG32                          Cfg_Cpus_Cache_Ram_Disable;   //0x0000009C
    REG32                          Reset_Cause;                  //0x000000A0
    /// This register is protected.
    REG32                          WakeUp;                       //0x000000A4
    /// This register is protected.
    REG32                          Ignore_Charger;               //0x000000A8
} HWP_SYS_CTRL_T;

#define hwp_sysCtrl                 ((HWP_SYS_CTRL_T*) KSEG1(REG_SYS_CTRL_BASE))


//REG_DBG
#define SYS_CTRL_SCRATCH(n)         (((n)&0xFFFF)<<0)
#define SYS_CTRL_WRITE_UNLOCK_STATUS (1<<30)
#define SYS_CTRL_WRITE_UNLOCK       (1<<31)

//Sys_Rst_Set
#define SYS_CTRL_SET_RST_XCPU       (1<<0)
#define SYS_CTRL_SET_RST_SYS_IRQ    (1<<1)
#define SYS_CTRL_SET_RST_SYS_A2A    (1<<2)
#define SYS_CTRL_SET_RST_ABB        (1<<3)
#define SYS_CTRL_SET_RST_DMA        (1<<4)
#define SYS_CTRL_SET_RST_GOUDA      (1<<5)
#define SYS_CTRL_SET_RST_TIMER      (1<<6)
#define SYS_CTRL_SET_RST_TCU        (1<<7)
#define SYS_CTRL_SET_RST_GPIO       (1<<8)
#define SYS_CTRL_SET_RST_KEYPAD     (1<<9)
#define SYS_CTRL_SET_RST_PWM        (1<<10)
#define SYS_CTRL_SET_RST_UART       (1<<11)
#define SYS_CTRL_SET_RST_UART2      (1<<12)
#define SYS_CTRL_SET_RST_SPI1       (1<<13)
#define SYS_CTRL_SET_RST_SPI2       (1<<14)
#define SYS_CTRL_SET_RST_SPI3       (1<<15)
#define SYS_CTRL_SET_RST_RF_SPI     (1<<16)
#define SYS_CTRL_SET_RST_SCI        (1<<17)
#define SYS_CTRL_SET_RST_SPY        (1<<18)
#define SYS_CTRL_SET_RST_I2C        (1<<19)
#define SYS_CTRL_SET_RST_I2C2       (1<<20)
#define SYS_CTRL_SET_RST_I2C3       (1<<21)
#define SYS_CTRL_SET_RST_MEM_BRIDGE (1<<22)
#define SYS_CTRL_SET_RST_GPADC      (1<<23)
#define SYS_CTRL_SET_RST_EXT_APB    (1<<24)
#define SYS_CTRL_SET_RST_SDMMC      (1<<25)
#define SYS_CTRL_SET_RST_CAMERA     (1<<26)
#define SYS_CTRL_SET_RST_USBC       (1<<27)
#define SYS_CTRL_SET_RST_USBPHY     (1<<28)
#define SYS_CTRL_SET_RST_DBG        (1<<29)
#define SYS_CTRL_SET_RST_OUT        (1<<30)
#define SYS_CTRL_SOFT_RST           (1<<31)
#define SYS_CTRL_SET_SYS_RST(n)     (((n)&0x3FFFFFFF)<<0)
#define SYS_CTRL_SET_SYS_RST_MASK   (0x3FFFFFFF<<0)
#define SYS_CTRL_SET_SYS_RST_SHIFT  (0)

//Sys_Rst_Clr
#define SYS_CTRL_CLR_RST_XCPU       (1<<0)
#define SYS_CTRL_CLR_RST_SYS_IRQ    (1<<1)
#define SYS_CTRL_CLR_RST_SYS_A2A    (1<<2)
#define SYS_CTRL_CLR_RST_ABB        (1<<3)
#define SYS_CTRL_CLR_RST_DMA        (1<<4)
#define SYS_CTRL_CLR_RST_GOUDA      (1<<5)
#define SYS_CTRL_CLR_RST_TIMER      (1<<6)
#define SYS_CTRL_CLR_RST_TCU        (1<<7)
#define SYS_CTRL_CLR_RST_GPIO       (1<<8)
#define SYS_CTRL_CLR_RST_KEYPAD     (1<<9)
#define SYS_CTRL_CLR_RST_PWM        (1<<10)
#define SYS_CTRL_CLR_RST_UART       (1<<11)
#define SYS_CTRL_CLR_RST_UART2      (1<<12)
#define SYS_CTRL_CLR_RST_SPI1       (1<<13)
#define SYS_CTRL_CLR_RST_SPI2       (1<<14)
#define SYS_CTRL_CLR_RST_SPI3       (1<<15)
#define SYS_CTRL_CLR_RST_RF_SPI     (1<<16)
#define SYS_CTRL_CLR_RST_SCI        (1<<17)
#define SYS_CTRL_CLR_RST_SPY        (1<<18)
#define SYS_CTRL_CLR_RST_I2C        (1<<19)
#define SYS_CTRL_CLR_RST_I2C2       (1<<20)
#define SYS_CTRL_CLR_RST_I2C3       (1<<21)
#define SYS_CTRL_CLR_RST_MEM_BRIDGE (1<<22)
#define SYS_CTRL_CLR_RST_GPADC      (1<<23)
#define SYS_CTRL_CLR_RST_EXT_APB    (1<<24)
#define SYS_CTRL_CLR_RST_SDMMC      (1<<25)
#define SYS_CTRL_CLR_RST_CAMERA     (1<<26)
#define SYS_CTRL_CLR_RST_USBC       (1<<27)
#define SYS_CTRL_CLR_RST_USBPHY     (1<<28)
#define SYS_CTRL_CLR_RST_DBG        (1<<29)
#define SYS_CTRL_CLR_RST_OUT        (1<<30)
#define SYS_CTRL_CLR_SYS_RST(n)     (((n)&0x3FFFFFFF)<<0)
#define SYS_CTRL_CLR_SYS_RST_MASK   (0x3FFFFFFF<<0)
#define SYS_CTRL_CLR_SYS_RST_SHIFT  (0)

//BB_Rst_Set
#define SYS_CTRL_SET_RST_BCPU       (1<<0)
#define SYS_CTRL_SET_RST_BB_IRQ     (1<<1)
#define SYS_CTRL_SET_RST_BB_A2A     (1<<2)
#define SYS_CTRL_SET_RST_BB_IFC     (1<<3)
#define SYS_CTRL_SET_RST_BB_SRAM    (1<<4)
#define SYS_CTRL_SET_RST_ITLV       (1<<5)
#define SYS_CTRL_SET_RST_VITERBI    (1<<6)
#define SYS_CTRL_SET_RST_CIPHER     (1<<7)
#define SYS_CTRL_SET_RST_XCOR       (1<<8)
#define SYS_CTRL_SET_RST_COPRO      (1<<9)
#define SYS_CTRL_SET_RST_RF_IF      (1<<10)
#define SYS_CTRL_SET_RST_VOC        (1<<11)
#define SYS_CTRL_SET_RST_AIF        (1<<12)
#define SYS_CTRL_SET_RST_DAI        (1<<13)
#define SYS_CTRL_SET_RST_BB_FULL    (1<<31)
#define SYS_CTRL_SET_BB_RST(n)      (((n)&0x3FFF)<<0)
#define SYS_CTRL_SET_BB_RST_MASK    (0x3FFF<<0)
#define SYS_CTRL_SET_BB_RST_SHIFT   (0)

//BB_Rst_Clr
#define SYS_CTRL_CLR_RST_BCPU       (1<<0)
#define SYS_CTRL_CLR_RST_BB_IRQ     (1<<1)
#define SYS_CTRL_CLR_RST_BB_A2A     (1<<2)
#define SYS_CTRL_CLR_RST_BB_IFC     (1<<3)
#define SYS_CTRL_CLR_RST_BB_SRAM    (1<<4)
#define SYS_CTRL_CLR_RST_ITLV       (1<<5)
#define SYS_CTRL_CLR_RST_VITERBI    (1<<6)
#define SYS_CTRL_CLR_RST_CIPHER     (1<<7)
#define SYS_CTRL_CLR_RST_XCOR       (1<<8)
#define SYS_CTRL_CLR_RST_COPRO      (1<<9)
#define SYS_CTRL_CLR_RST_RF_IF      (1<<10)
#define SYS_CTRL_CLR_RST_VOC        (1<<11)
#define SYS_CTRL_CLR_RST_AIF        (1<<12)
#define SYS_CTRL_CLR_RST_DAI        (1<<13)
#define SYS_CTRL_CLR_RST_BB_FULL    (1<<31)
#define SYS_CTRL_CLR_BB_RST(n)      (((n)&0x3FFF)<<0)
#define SYS_CTRL_CLR_BB_RST_MASK    (0x3FFF<<0)
#define SYS_CTRL_CLR_BB_RST_SHIFT   (0)

//Clk_Sys_Mode
#define SYS_CTRL_MODE_SYS_XCPU      (1<<0)
#define SYS_CTRL_MODE_SYS_XCPU_INT_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_SYS_XCPU_INT_MANUAL (1<<1)
#define SYS_CTRL_MODE_SYS_PCLK_CONF_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_SYS_PCLK_CONF_MANUAL (1<<2)
#define SYS_CTRL_MODE_SYS_PCLK_DATA_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_SYS_PCLK_DATA_MANUAL (1<<3)
#define SYS_CTRL_MODE_SYS_AMBA_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_SYS_AMBA_MANUAL (1<<4)
#define SYS_CTRL_MODE_SYS_DMA_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_SYS_DMA_MANUAL (1<<5)
#define SYS_CTRL_MODE_SYS_GOUDA_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_SYS_GOUDA_MANUAL (1<<6)
#define SYS_CTRL_MODE_SYS_EBC_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_SYS_EBC_MANUAL (1<<7)
#define SYS_CTRL_MODE_SYS_IFC_CH0_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_SYS_IFC_CH0_MANUAL (1<<8)
#define SYS_CTRL_MODE_SYS_IFC_CH1_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_SYS_IFC_CH1_MANUAL (1<<9)
#define SYS_CTRL_MODE_SYS_IFC_CH2_AUTOMATIC (0<<10)
#define SYS_CTRL_MODE_SYS_IFC_CH2_MANUAL (1<<10)
#define SYS_CTRL_MODE_SYS_IFC_CH3_AUTOMATIC (0<<11)
#define SYS_CTRL_MODE_SYS_IFC_CH3_MANUAL (1<<11)
#define SYS_CTRL_MODE_SYS_IFC_CH4_AUTOMATIC (0<<12)
#define SYS_CTRL_MODE_SYS_IFC_CH4_MANUAL (1<<12)
#define SYS_CTRL_MODE_SYS_IFC_CH5_AUTOMATIC (0<<13)
#define SYS_CTRL_MODE_SYS_IFC_CH5_MANUAL (1<<13)
#define SYS_CTRL_MODE_SYS_IFC_CH6_AUTOMATIC (0<<14)
#define SYS_CTRL_MODE_SYS_IFC_CH6_MANUAL (1<<14)
#define SYS_CTRL_MODE_SYS_IFC_CH7_AUTOMATIC (0<<15)
#define SYS_CTRL_MODE_SYS_IFC_CH7_MANUAL (1<<15)
#define SYS_CTRL_MODE_SYS_IFC_DBG_AUTOMATIC (0<<16)
#define SYS_CTRL_MODE_SYS_IFC_DBG_MANUAL (1<<16)
#define SYS_CTRL_MODE_SYS_A2A_AUTOMATIC (0<<17)
#define SYS_CTRL_MODE_SYS_A2A_MANUAL (1<<17)
#define SYS_CTRL_MODE_SYS_COM_REGS_AUTOMATIC (0<<18)
#define SYS_CTRL_MODE_SYS_COM_REGS_MANUAL (1<<18)
#define SYS_CTRL_MODE_SYS_DEBUG_UART_AUTOMATIC (0<<19)
#define SYS_CTRL_MODE_SYS_DEBUG_UART_MANUAL (1<<19)
#define SYS_CTRL_MODE_SYS_DBGHST_AUTOMATIC (0<<20)
#define SYS_CTRL_MODE_SYS_DBGHST_MANUAL (1<<20)
#define SYS_CTRL_MODE_SYS_GPADC_AUTOMATIC (0<<21)
#define SYS_CTRL_MODE_SYS_GPADC_MANUAL (1<<21)
#define SYS_CTRL_MODE_SYSD_SCI_AUTOMATIC (0<<22)
#define SYS_CTRL_MODE_SYSD_SCI_MANUAL (1<<22)
#define SYS_CTRL_MODE_SYSD_RF_SPI_AUTOMATIC (0<<23)
#define SYS_CTRL_MODE_SYSD_RF_SPI_MANUAL (1<<23)
#define SYS_CTRL_MODE_SYSD_OSC_AUTOMATIC (0<<24)
#define SYS_CTRL_MODE_SYSD_OSC_MANUAL (1<<24)
#define SYS_CTRL_MODE_SYSD_PWM_AUTOMATIC (0<<25)
#define SYS_CTRL_MODE_SYSD_PWM_MANUAL (1<<25)
#define SYS_CTRL_MODE_CLK_SYS(n)    (((n)&0x1FFFFFF)<<1)
#define SYS_CTRL_MODE_CLK_SYS_MASK  (0x1FFFFFF<<1)
#define SYS_CTRL_MODE_CLK_SYS_SHIFT (1)

//Clk_Sys_Enable
#define SYS_CTRL_ENABLE_SYS_XCPU    (1<<0)
#define SYS_CTRL_ENABLE_SYS_XCPU_INT (1<<1)
#define SYS_CTRL_ENABLE_SYS_PCLK_CONF (1<<2)
#define SYS_CTRL_ENABLE_SYS_PCLK_DATA (1<<3)
#define SYS_CTRL_ENABLE_SYS_AMBA    (1<<4)
#define SYS_CTRL_ENABLE_SYS_DMA     (1<<5)
#define SYS_CTRL_ENABLE_SYS_GOUDA   (1<<6)
#define SYS_CTRL_ENABLE_SYS_EBC     (1<<7)
#define SYS_CTRL_ENABLE_SYS_IFC_CH0 (1<<8)
#define SYS_CTRL_ENABLE_SYS_IFC_CH1 (1<<9)
#define SYS_CTRL_ENABLE_SYS_IFC_CH2 (1<<10)
#define SYS_CTRL_ENABLE_SYS_IFC_CH3 (1<<11)
#define SYS_CTRL_ENABLE_SYS_IFC_CH4 (1<<12)
#define SYS_CTRL_ENABLE_SYS_IFC_CH5 (1<<13)
#define SYS_CTRL_ENABLE_SYS_IFC_CH6 (1<<14)
#define SYS_CTRL_ENABLE_SYS_IFC_CH7 (1<<15)
#define SYS_CTRL_ENABLE_SYS_IFC_DBG (1<<16)
#define SYS_CTRL_ENABLE_SYS_A2A     (1<<17)
#define SYS_CTRL_ENABLE_SYS_COM_REGS (1<<18)
#define SYS_CTRL_ENABLE_SYS_DEBUG_UART (1<<19)
#define SYS_CTRL_ENABLE_SYS_DBGHST  (1<<20)
#define SYS_CTRL_ENABLE_SYS_GPADC   (1<<21)
#define SYS_CTRL_ENABLE_SYSD_SCI    (1<<22)
#define SYS_CTRL_ENABLE_SYSD_RF_SPI (1<<23)
#define SYS_CTRL_ENABLE_SYSD_OSC    (1<<24)
#define SYS_CTRL_ENABLE_SYSD_PWM    (1<<25)
#define SYS_CTRL_ENABLE_SYS_GPIO    (1<<26)
#define SYS_CTRL_ENABLE_SYS_IRQ     (1<<27)
#define SYS_CTRL_ENABLE_SYS_TCU     (1<<28)
#define SYS_CTRL_ENABLE_SYS_TIMER   (1<<29)
#define SYS_CTRL_ENABLE_CLK_SYS(n)  (((n)&0x3FFFFFFF)<<0)
#define SYS_CTRL_ENABLE_CLK_SYS_MASK (0x3FFFFFFF<<0)
#define SYS_CTRL_ENABLE_CLK_SYS_SHIFT (0)

//Clk_Sys_Disable
#define SYS_CTRL_DISABLE_SYS_XCPU   (1<<0)
#define SYS_CTRL_DISABLE_SYS_XCPU_INT (1<<1)
#define SYS_CTRL_DISABLE_SYS_PCLK_CONF (1<<2)
#define SYS_CTRL_DISABLE_SYS_PCLK_DATA (1<<3)
#define SYS_CTRL_DISABLE_SYS_AMBA   (1<<4)
#define SYS_CTRL_DISABLE_SYS_DMA    (1<<5)
#define SYS_CTRL_DISABLE_SYS_GOUDA  (1<<6)
#define SYS_CTRL_DISABLE_SYS_EBC    (1<<7)
#define SYS_CTRL_DISABLE_SYS_IFC_CH0 (1<<8)
#define SYS_CTRL_DISABLE_SYS_IFC_CH1 (1<<9)
#define SYS_CTRL_DISABLE_SYS_IFC_CH2 (1<<10)
#define SYS_CTRL_DISABLE_SYS_IFC_CH3 (1<<11)
#define SYS_CTRL_DISABLE_SYS_IFC_CH4 (1<<12)
#define SYS_CTRL_DISABLE_SYS_IFC_CH5 (1<<13)
#define SYS_CTRL_DISABLE_SYS_IFC_CH6 (1<<14)
#define SYS_CTRL_DISABLE_SYS_IFC_CH7 (1<<15)
#define SYS_CTRL_DISABLE_SYS_IFC_DBG (1<<16)
#define SYS_CTRL_DISABLE_SYS_A2A    (1<<17)
#define SYS_CTRL_DISABLE_SYS_COM_REGS (1<<18)
#define SYS_CTRL_DISABLE_SYS_DEBUG_UART (1<<19)
#define SYS_CTRL_DISABLE_SYS_DBGHST (1<<20)
#define SYS_CTRL_DISABLE_SYS_GPADC  (1<<21)
#define SYS_CTRL_DISABLE_SYSD_SCI   (1<<22)
#define SYS_CTRL_DISABLE_SYSD_RF_SPI (1<<23)
#define SYS_CTRL_DISABLE_SYSD_OSC   (1<<24)
#define SYS_CTRL_DISABLE_SYSD_PWM   (1<<25)
#define SYS_CTRL_DISABLE_SYS_GPIO   (1<<26)
#define SYS_CTRL_DISABLE_SYS_IRQ    (1<<27)
#define SYS_CTRL_DISABLE_SYS_TCU    (1<<28)
#define SYS_CTRL_DISABLE_SYS_TIMER  (1<<29)
#define SYS_CTRL_DISABLE_CLK_SYS(n) (((n)&0x3FFFFFFF)<<0)
#define SYS_CTRL_DISABLE_CLK_SYS_MASK (0x3FFFFFFF<<0)
#define SYS_CTRL_DISABLE_CLK_SYS_SHIFT (0)

//Clk_Per_Mode
#define SYS_CTRL_MODE_PER_UART_AUTOMATIC (0<<0)
#define SYS_CTRL_MODE_PER_UART_MANUAL (1<<0)
#define SYS_CTRL_MODE_PER_UART2_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_PER_UART2_MANUAL (1<<1)
#define SYS_CTRL_MODE_PER_I2C_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_PER_I2C_MANUAL (1<<2)
#define SYS_CTRL_MODE_PER_I2C2_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_PER_I2C2_MANUAL (1<<3)
#define SYS_CTRL_MODE_PER_I2C3_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_PER_I2C3_MANUAL (1<<4)
#define SYS_CTRL_MODE_PER_SDMMC_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_PER_SDMMC_MANUAL (1<<5)
#define SYS_CTRL_MODE_PER_CAMERA_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_PER_CAMERA_MANUAL (1<<6)
#define SYS_CTRL_MODE_PERD_SPI1_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_PERD_SPI1_MANUAL (1<<7)
#define SYS_CTRL_MODE_PERD_SPI2_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_PERD_SPI2_MANUAL (1<<8)
#define SYS_CTRL_MODE_PERD_SPI3_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_PERD_SPI3_MANUAL (1<<9)
#define SYS_CTRL_MODE_CLK_PER(n)    (((n)&0x3FF)<<0)
#define SYS_CTRL_MODE_CLK_PER_MASK  (0x3FF<<0)
#define SYS_CTRL_MODE_CLK_PER_SHIFT (0)

//Clk_Per_Enable
#define SYS_CTRL_ENABLE_PER_UART    (1<<0)
#define SYS_CTRL_ENABLE_PER_UART2   (1<<1)
#define SYS_CTRL_ENABLE_PER_I2C     (1<<2)
#define SYS_CTRL_ENABLE_PER_I2C2    (1<<3)
#define SYS_CTRL_ENABLE_PER_I2C3    (1<<4)
#define SYS_CTRL_ENABLE_PER_SDMMC   (1<<5)
#define SYS_CTRL_ENABLE_PER_CAMERA  (1<<6)
#define SYS_CTRL_ENABLE_PERD_SPI1   (1<<7)
#define SYS_CTRL_ENABLE_PERD_SPI2   (1<<8)
#define SYS_CTRL_ENABLE_PERD_SPI3   (1<<9)
#define SYS_CTRL_ENABLE_PER_USBC    (1<<10)
#define SYS_CTRL_ENABLE_PER_SPY     (1<<11)
#define SYS_CTRL_ENABLE_PER_TEST    (1<<12)
#define SYS_CTRL_ENABLE_CLK_PER(n)  (((n)&0x1FFF)<<0)
#define SYS_CTRL_ENABLE_CLK_PER_MASK (0x1FFF<<0)
#define SYS_CTRL_ENABLE_CLK_PER_SHIFT (0)

//Clk_Per_Disable
#define SYS_CTRL_DISABLE_PER_UART   (1<<0)
#define SYS_CTRL_DISABLE_PER_UART2  (1<<1)
#define SYS_CTRL_DISABLE_PER_I2C    (1<<2)
#define SYS_CTRL_DISABLE_PER_I2C2   (1<<3)
#define SYS_CTRL_DISABLE_PER_I2C3   (1<<4)
#define SYS_CTRL_DISABLE_PER_SDMMC  (1<<5)
#define SYS_CTRL_DISABLE_PER_CAMERA (1<<6)
#define SYS_CTRL_DISABLE_PERD_SPI1  (1<<7)
#define SYS_CTRL_DISABLE_PERD_SPI2  (1<<8)
#define SYS_CTRL_DISABLE_PERD_SPI3  (1<<9)
#define SYS_CTRL_DISABLE_PER_USBC   (1<<10)
#define SYS_CTRL_DISABLE_PER_SPY    (1<<11)
#define SYS_CTRL_DISABLE_PER_TEST   (1<<12)
#define SYS_CTRL_DISABLE_CLK_PER(n) (((n)&0x1FFF)<<0)
#define SYS_CTRL_DISABLE_CLK_PER_MASK (0x1FFF<<0)
#define SYS_CTRL_DISABLE_CLK_PER_SHIFT (0)

//Clk_BB_Mode
#define SYS_CTRL_MODE_BB_BCPU       (1<<0)
#define SYS_CTRL_MODE_BB_BCPU_INT_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_BB_BCPU_INT_MANUAL (1<<1)
#define SYS_CTRL_MODE_BB_AMBA_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_BB_AMBA_MANUAL (1<<2)
#define SYS_CTRL_MODE_BB_PCLK_CONF_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_BB_PCLK_CONF_MANUAL (1<<3)
#define SYS_CTRL_MODE_BB_PCLK_DATA_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_BB_PCLK_DATA_MANUAL (1<<4)
#define SYS_CTRL_MODE_BB_VOC_AHB_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_BB_VOC_AHB_MANUAL (1<<5)
#define SYS_CTRL_MODE_BB_IFC_CH0_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_BB_IFC_CH0_MANUAL (1<<6)
#define SYS_CTRL_MODE_BB_IFC_CH1_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_BB_IFC_CH1_MANUAL (1<<7)
#define SYS_CTRL_MODE_BB_IFC_CH2_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_BB_IFC_CH2_MANUAL (1<<8)
#define SYS_CTRL_MODE_BB_IFC_CH3_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_BB_IFC_CH3_MANUAL (1<<9)
#define SYS_CTRL_MODE_BB_SRAM_AUTOMATIC (0<<10)
#define SYS_CTRL_MODE_BB_SRAM_MANUAL (1<<10)
#define SYS_CTRL_MODE_BB_A2A_AUTOMATIC (0<<11)
#define SYS_CTRL_MODE_BB_A2A_MANUAL (1<<11)
#define SYS_CTRL_MODE_BB_ITLV_AUTOMATIC (0<<12)
#define SYS_CTRL_MODE_BB_ITLV_MANUAL (1<<12)
#define SYS_CTRL_MODE_BB_VITERBI_AUTOMATIC (0<<13)
#define SYS_CTRL_MODE_BB_VITERBI_MANUAL (1<<13)
#define SYS_CTRL_MODE_BB_CIPHER_AUTOMATIC (0<<14)
#define SYS_CTRL_MODE_BB_CIPHER_MANUAL (1<<14)
#define SYS_CTRL_MODE_BB_RF_IF_AUTOMATIC (0<<15)
#define SYS_CTRL_MODE_BB_RF_IF_MANUAL (1<<15)
#define SYS_CTRL_MODE_BB_COPRO_AUTOMATIC (0<<16)
#define SYS_CTRL_MODE_BB_COPRO_MANUAL (1<<16)
#define SYS_CTRL_MODE_BB_CP2_REG_AUTOMATIC (0<<17)
#define SYS_CTRL_MODE_BB_CP2_REG_MANUAL (1<<17)
#define SYS_CTRL_MODE_BB_XCOR_AUTOMATIC (0<<18)
#define SYS_CTRL_MODE_BB_XCOR_MANUAL (1<<18)
#define SYS_CTRL_MODE_BB_AIF_AUTOMATIC (0<<19)
#define SYS_CTRL_MODE_BB_AIF_MANUAL (1<<19)
#define SYS_CTRL_MODE_BBD_DAIS_208K_AUTOMATIC (0<<20)
#define SYS_CTRL_MODE_BBD_DAIS_208K_MANUAL (1<<20)
#define SYS_CTRL_MODE_CLK_BB(n)     (((n)&0xFFFFF)<<1)
#define SYS_CTRL_MODE_CLK_BB_MASK   (0xFFFFF<<1)
#define SYS_CTRL_MODE_CLK_BB_SHIFT  (1)

//Clk_BB_Enable
#define SYS_CTRL_ENABLE_BB_BCPU     (1<<0)
#define SYS_CTRL_ENABLE_BB_BCPU_INT (1<<1)
#define SYS_CTRL_ENABLE_BB_AMBA     (1<<2)
#define SYS_CTRL_ENABLE_BB_PCLK_CONF (1<<3)
#define SYS_CTRL_ENABLE_BB_PCLK_DATA (1<<4)
#define SYS_CTRL_ENABLE_BB_VOC_AHB  (1<<5)
#define SYS_CTRL_ENABLE_BB_IFC_CH0  (1<<6)
#define SYS_CTRL_ENABLE_BB_IFC_CH1  (1<<7)
#define SYS_CTRL_ENABLE_BB_IFC_CH2  (1<<8)
#define SYS_CTRL_ENABLE_BB_IFC_CH3  (1<<9)
#define SYS_CTRL_ENABLE_BB_SRAM     (1<<10)
#define SYS_CTRL_ENABLE_BB_A2A      (1<<11)
#define SYS_CTRL_ENABLE_BB_ITLV     (1<<12)
#define SYS_CTRL_ENABLE_BB_VITERBI  (1<<13)
#define SYS_CTRL_ENABLE_BB_CIPHER   (1<<14)
#define SYS_CTRL_ENABLE_BB_RF_IF    (1<<15)
#define SYS_CTRL_ENABLE_BB_COPRO    (1<<16)
#define SYS_CTRL_ENABLE_BB_CP2_REG  (1<<17)
#define SYS_CTRL_ENABLE_BB_XCOR     (1<<18)
#define SYS_CTRL_ENABLE_BB_AIF      (1<<19)
#define SYS_CTRL_ENABLE_BBD_DAIS_208K (1<<20)
#define SYS_CTRL_ENABLE_BB_IRQ      (1<<21)
#define SYS_CTRL_ENABLE_CLK_BB(n)   (((n)&0x3FFFFF)<<0)
#define SYS_CTRL_ENABLE_CLK_BB_MASK (0x3FFFFF<<0)
#define SYS_CTRL_ENABLE_CLK_BB_SHIFT (0)

//Clk_BB_Disable
#define SYS_CTRL_DISABLE_BB_BCPU    (1<<0)
#define SYS_CTRL_DISABLE_BB_BCPU_INT (1<<1)
#define SYS_CTRL_DISABLE_BB_AMBA    (1<<2)
#define SYS_CTRL_DISABLE_BB_PCLK_CONF (1<<3)
#define SYS_CTRL_DISABLE_BB_PCLK_DATA (1<<4)
#define SYS_CTRL_DISABLE_BB_VOC_AHB (1<<5)
#define SYS_CTRL_DISABLE_BB_IFC_CH0 (1<<6)
#define SYS_CTRL_DISABLE_BB_IFC_CH1 (1<<7)
#define SYS_CTRL_DISABLE_BB_IFC_CH2 (1<<8)
#define SYS_CTRL_DISABLE_BB_IFC_CH3 (1<<9)
#define SYS_CTRL_DISABLE_BB_SRAM    (1<<10)
#define SYS_CTRL_DISABLE_BB_A2A     (1<<11)
#define SYS_CTRL_DISABLE_BB_ITLV    (1<<12)
#define SYS_CTRL_DISABLE_BB_VITERBI (1<<13)
#define SYS_CTRL_DISABLE_BB_CIPHER  (1<<14)
#define SYS_CTRL_DISABLE_BB_RF_IF   (1<<15)
#define SYS_CTRL_DISABLE_BB_COPRO   (1<<16)
#define SYS_CTRL_DISABLE_BB_CP2_REG (1<<17)
#define SYS_CTRL_DISABLE_BB_XCOR    (1<<18)
#define SYS_CTRL_DISABLE_BB_AIF     (1<<19)
#define SYS_CTRL_DISABLE_BBD_DAIS_208K (1<<20)
#define SYS_CTRL_DISABLE_BB_IRQ     (1<<21)
#define SYS_CTRL_DISABLE_CLK_BB(n)  (((n)&0x3FFFFF)<<0)
#define SYS_CTRL_DISABLE_CLK_BB_MASK (0x3FFFFF<<0)
#define SYS_CTRL_DISABLE_CLK_BB_SHIFT (0)

//Clk_Other_Mode
#define SYS_CTRL_MODE_OC_DEBUG_UART_AUTOMATIC (0<<0)
#define SYS_CTRL_MODE_OC_DEBUG_UART_MANUAL (1<<0)
#define SYS_CTRL_MODE_OC_26M_GPADC_AUTOMATIC (0<<1)
#define SYS_CTRL_MODE_OC_26M_GPADC_MANUAL (1<<1)
#define SYS_CTRL_MODE_OC_BCK_AUTOMATIC (0<<2)
#define SYS_CTRL_MODE_OC_BCK_MANUAL (1<<2)
#define SYS_CTRL_MODE_OC_UART_AUTOMATIC (0<<3)
#define SYS_CTRL_MODE_OC_UART_MANUAL (1<<3)
#define SYS_CTRL_MODE_OC_UART2_AUTOMATIC (0<<4)
#define SYS_CTRL_MODE_OC_UART2_MANUAL (1<<4)
#define SYS_CTRL_MODE_OC_RF_RX_AUTOMATIC (0<<5)
#define SYS_CTRL_MODE_OC_RF_RX_MANUAL (1<<5)
#define SYS_CTRL_MODE_OC_RF_TX_AUTOMATIC (0<<6)
#define SYS_CTRL_MODE_OC_RF_TX_MANUAL (1<<6)
#define SYS_CTRL_MODE_OC_MEM_BRIDGE_AUTOMATIC (0<<7)
#define SYS_CTRL_MODE_OC_MEM_BRIDGE_MANUAL (1<<7)
#define SYS_CTRL_MODE_OC_VOC_CORE_AUTOMATIC (0<<8)
#define SYS_CTRL_MODE_OC_VOC_CORE_MANUAL (1<<8)
#define SYS_CTRL_MODE_OC_VOC_AHB_AUTOMATIC (0<<9)
#define SYS_CTRL_MODE_OC_VOC_AHB_MANUAL (1<<9)
#define SYS_CTRL_MODE_OC_VOC_DAI_AUTOMATIC (0<<10)
#define SYS_CTRL_MODE_OC_VOC_DAI_MANUAL (1<<10)
#define SYS_CTRL_MODE_CLK_OTHER(n)  (((n)&0x7FF)<<0)
#define SYS_CTRL_MODE_CLK_OTHER_MASK (0x7FF<<0)
#define SYS_CTRL_MODE_CLK_OTHER_SHIFT (0)

//Clk_Other_Enable
#define SYS_CTRL_ENABLE_OC_DEBUG_UART (1<<0)
#define SYS_CTRL_ENABLE_OC_26M_GPADC (1<<1)
#define SYS_CTRL_ENABLE_OC_BCK      (1<<2)
#define SYS_CTRL_ENABLE_OC_UART     (1<<3)
#define SYS_CTRL_ENABLE_OC_UART2    (1<<4)
#define SYS_CTRL_ENABLE_OC_RF_RX    (1<<5)
#define SYS_CTRL_ENABLE_OC_RF_TX    (1<<6)
#define SYS_CTRL_ENABLE_OC_MEM_BRIDGE (1<<7)
#define SYS_CTRL_ENABLE_OC_VOC_CORE (1<<8)
#define SYS_CTRL_ENABLE_OC_VOC_AHB  (1<<9)
#define SYS_CTRL_ENABLE_OC_VOC_DAI  (1<<10)
#define SYS_CTRL_ENABLE_OC_VOC      (1<<11)
#define SYS_CTRL_ENABLE_OC_LPS      (1<<12)
#define SYS_CTRL_ENABLE_OC_GPIO     (1<<13)
#define SYS_CTRL_ENABLE_OC_USBPHY   (1<<14)
#define SYS_CTRL_ENABLE_OC_PIX      (1<<15)
#define SYS_CTRL_ENABLE_OC_CLK_OUT  (1<<16)
#define SYS_CTRL_ENABLE_OC_MEM_CLK_OUT (1<<17)
#define SYS_CTRL_ENABLE_OC_DBG      (1<<18)
#define SYS_CTRL_ENABLE_CLK_OTHER(n) (((n)&0x7FFFF)<<0)
#define SYS_CTRL_ENABLE_CLK_OTHER_MASK (0x7FFFF<<0)
#define SYS_CTRL_ENABLE_CLK_OTHER_SHIFT (0)

//Clk_Other_Disable
#define SYS_CTRL_DISABLE_OC_DEBUG_UART (1<<0)
#define SYS_CTRL_DISABLE_OC_26M_GPADC (1<<1)
#define SYS_CTRL_DISABLE_OC_BCK     (1<<2)
#define SYS_CTRL_DISABLE_OC_UART    (1<<3)
#define SYS_CTRL_DISABLE_OC_UART2   (1<<4)
#define SYS_CTRL_DISABLE_OC_RF_RX   (1<<5)
#define SYS_CTRL_DISABLE_OC_RF_TX   (1<<6)
#define SYS_CTRL_DISABLE_OC_MEM_BRIDGE (1<<7)
#define SYS_CTRL_DISABLE_OC_VOC_CORE (1<<8)
#define SYS_CTRL_DISABLE_OC_VOC_AHB (1<<9)
#define SYS_CTRL_DISABLE_OC_VOC_DAI (1<<10)
#define SYS_CTRL_DISABLE_OC_VOC     (1<<11)
#define SYS_CTRL_DISABLE_OC_LPS     (1<<12)
#define SYS_CTRL_DISABLE_OC_GPIO    (1<<13)
#define SYS_CTRL_DISABLE_OC_USBPHY  (1<<14)
#define SYS_CTRL_DISABLE_OC_PIX     (1<<15)
#define SYS_CTRL_DISABLE_OC_CLK_OUT (1<<16)
#define SYS_CTRL_DISABLE_OC_MEM_CLK_OUT (1<<17)
#define SYS_CTRL_DISABLE_OC_DBG     (1<<18)
#define SYS_CTRL_DISABLE_CLK_OTHER(n) (((n)&0x7FFFF)<<0)
#define SYS_CTRL_DISABLE_CLK_OTHER_MASK (0x7FFFF<<0)
#define SYS_CTRL_DISABLE_CLK_OTHER_SHIFT (0)

//Pll_Ctrl
#define SYS_CTRL_PLL_ENABLE         (1<<0)
#define SYS_CTRL_PLL_ENABLE_MASK    (1<<0)
#define SYS_CTRL_PLL_ENABLE_SHIFT   (0)
#define SYS_CTRL_PLL_ENABLE_POWER_DOWN (0<<0)
#define SYS_CTRL_PLL_ENABLE_ENABLE  (1<<0)
#define SYS_CTRL_PLL_LOCK_RESET     (1<<4)
#define SYS_CTRL_PLL_LOCK_RESET_MASK (1<<4)
#define SYS_CTRL_PLL_LOCK_RESET_SHIFT (4)
#define SYS_CTRL_PLL_LOCK_RESET_RESET (0<<4)
#define SYS_CTRL_PLL_LOCK_RESET_NO_RESET (1<<4)
#define SYS_CTRL_PLL_BYPASS         (1<<8)
#define SYS_CTRL_PLL_BYPASS_MASK    (1<<8)
#define SYS_CTRL_PLL_BYPASS_SHIFT   (8)
#define SYS_CTRL_PLL_BYPASS_PASS    (0<<8)
#define SYS_CTRL_PLL_BYPASS_BYPASS  (1<<8)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE (1<<12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_MASK (1<<12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_SHIFT (12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE (1<<12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE (0<<12)

//Sel_Clock
#define SYS_CTRL_SLOW_SEL_RF_OSCILLATOR (1<<0)
#define SYS_CTRL_SLOW_SEL_RF_RF     (0<<0)
#define SYS_CTRL_SYS_SEL_FAST_SLOW  (1<<4)
#define SYS_CTRL_SYS_SEL_FAST_FAST  (0<<4)
#define SYS_CTRL_RF_DETECTED_OK     (1<<20)
#define SYS_CTRL_RF_DETECTED_NO     (0<<20)
#define SYS_CTRL_RF_DETECT_BYPASS   (1<<21)
#define SYS_CTRL_RF_DETECT_RESET    (1<<22)
#define SYS_CTRL_RF_SELECTED_L      (1<<23)
#define SYS_CTRL_PLL_LOCKED         (1<<24)
#define SYS_CTRL_PLL_LOCKED_MASK    (1<<24)
#define SYS_CTRL_PLL_LOCKED_SHIFT   (24)
#define SYS_CTRL_PLL_LOCKED_LOCKED  (1<<24)
#define SYS_CTRL_PLL_LOCKED_NOT_LOCKED (0<<24)
#define SYS_CTRL_PLL_BYPASS_LOCK    (1<<27)
#define SYS_CTRL_FAST_SELECTED_L    (1<<31)
#define SYS_CTRL_FAST_SELECTED_L_MASK (1<<31)
#define SYS_CTRL_FAST_SELECTED_L_SHIFT (31)

//Cfg_Clk_Sys
#define SYS_CTRL_FREQ(n)            (((n)&7)<<0)
#define SYS_CTRL_FREQ_MASK          (7<<0)
#define SYS_CTRL_FREQ_SHIFT         (0)
#define SYS_CTRL_FREQ_RESERVED      (0<<0)
#define SYS_CTRL_FREQ_26M           (1<<0)
#define SYS_CTRL_FREQ_39M           (2<<0)
#define SYS_CTRL_FREQ_52M           (3<<0)
#define SYS_CTRL_FREQ_78M           (4<<0)
#define SYS_CTRL_FREQ_104M          (5<<0)
#define SYS_CTRL_FREQ_156M          (6<<0)
#define SYS_CTRL_FORCE_DIV_UPDATE   (1<<4)
#define SYS_CTRL_REQ_DIV_UPDATE     (1<<8)

//Cfg_Clk_Mem_Bridge
#define SYS_CTRL_MEM_FREQ(n)        (((n)&7)<<0)
#define SYS_CTRL_MEM_FREQ_MASK      (7<<0)
#define SYS_CTRL_MEM_FREQ_SHIFT     (0)
#define SYS_CTRL_MEM_FREQ_13M       (0<<0)
#define SYS_CTRL_MEM_FREQ_26M       (1<<0)
#define SYS_CTRL_MEM_FREQ_39M       (2<<0)
#define SYS_CTRL_MEM_FREQ_52M       (3<<0)
#define SYS_CTRL_MEM_FREQ_78M       (4<<0)
#define SYS_CTRL_MEM_FREQ_89M       (5<<0)
#define SYS_CTRL_MEM_FREQ_104M      (6<<0)

//Cfg_Clk_Voc
#define SYS_CTRL_VOC_FREQ(n)        (((n)&7)<<0)
#define SYS_CTRL_VOC_FREQ_MASK      (7<<0)
#define SYS_CTRL_VOC_FREQ_SHIFT     (0)
#define SYS_CTRL_VOC_FREQ_13M       (0<<0)
#define SYS_CTRL_VOC_FREQ_26M       (1<<0)
#define SYS_CTRL_VOC_FREQ_39M       (2<<0)
#define SYS_CTRL_VOC_FREQ_52M       (3<<0)
#define SYS_CTRL_VOC_FREQ_78M       (4<<0)
#define SYS_CTRL_VOC_FREQ_89M       (5<<0)
#define SYS_CTRL_VOC_FREQ_104M      (6<<0)
#define SYS_CTRL_VOC_SEL_PLL_SLOW   (1<<8)
#define SYS_CTRL_VOC_SEL_PLL_PLL    (0<<8)

//Cfg_Clk_Out
#define SYS_CTRL_CLKOUT_DIVIDER(n)  (((n)&31)<<0)
#define SYS_CTRL_CLKOUT_SEL_OSC     (0<<8)
#define SYS_CTRL_CLKOUT_SEL_RF      (1<<8)
#define SYS_CTRL_CLKOUT_SEL_DIVIDER (2<<8)

//Cfg_Clk_GPADC
#define SYS_CTRL_GPADC_CLK_DIV(n)   (((n)&31)<<0)
#define SYS_CTRL_GPADC_CLK_DIV_MASK (31<<0)
#define SYS_CTRL_GPADC_CLK_DIV_SHIFT (0)
#define SYS_CTRL_GPADC_CLK_POL      (1<<8)
#define SYS_CTRL_GPADC_CLK_EN_MODE  (1<<12)

//Cfg_Clk_AudioBCK_Div
#define SYS_CTRL_AUDIOBCK_DIVIDER(n) (((n)&0x7FF)<<0)

//Cfg_Aif_Tx_Stb_Div
#ifdef GALLITE_IS_8806
#define SYS_CTRL_AIF_TX_STB_DIV(n)  (((n)&0x1FFF)<<0)
#else
#define SYS_CTRL_AIF_TX_STB_DIV(n)  (((n)&0xFFF)<<0)
#endif
#define SYS_CTRL_AIF_TX_STB_EN      (1<<31)

//Cfg_Clk_Uart
#define SYS_CTRL_UART_DIVIDER(n)    (((n)&0x3FF)<<0)
#define SYS_CTRL_UART_DIVIDER_MASK  (0x3FF<<0)
#define SYS_CTRL_UART_DIVIDER_SHIFT (0)
#define SYS_CTRL_UART_SEL_PLL_SLOW  (0<<12)
#define SYS_CTRL_UART_SEL_PLL_PLL   (1<<12)

//Cfg_Clk_PWM
#define SYS_CTRL_PWM_DIVIDER(n)     (((n)&0xFF)<<0)

//Cfg_Clk_Auxclk
#define SYS_CTRL_AUXCLK_EN_DISABLE  (0<<0)
#define SYS_CTRL_AUXCLK_EN_ENABLE   (1<<0)

//Cfg_Clk_Dbg_Div
#define SYS_CTRL_CLK_DBG_DIVIDER(n) (((n)&0x7FF)<<0)

//Cfg_Clk_Camera_Out
#define SYS_CTRL_CLK_CAMERA_OUT_EN_DISABLE      (0<<0)
#define SYS_CTRL_CLK_CAMERA_OUT_EN_ENABLE       (1<<0)
#define SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_156_MHZ (0<<4)
#define SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL__26_MHZ (1<<4)
#define SYS_CTRL_CLK_CAMERA_OUT_DIV(n)          (((n)&15)<<8)
#define SYS_CTRL_CLK_CAMERA_OUT_DIV_MASK        (15<<8)
#define SYS_CTRL_CLK_CAMERA_OUT_DIV_SHIFT       (8)
#define SYS_CTRL_CLK_CAMERA_OUT_SRC_SEL_FROM_DIVIDER (0<<12)
#define SYS_CTRL_CLK_CAMERA_OUT_SRC_SEL_FROM_32K (1<<12)
#define SYS_CTRL_CLK_CAMERA_OUT_SRC_SEL_FROM_26M (2<<12)

//Cfg_AHB
#define SYS_CTRL_SYS_NEW_ARBITRATION_ENABLE (1<<0)
#define SYS_CTRL_SYS_NEW_ARBITRATION_DISABLE (0<<0)
#define SYS_CTRL_ENABLE_SYS_MID_BIST_ENABLE (1<<1)
#define SYS_CTRL_ENABLE_SYS_MID_BIST_DISABLE (0<<1)
#define SYS_CTRL_ENABLE_SYS_MID_DMA_ENABLE (1<<2)
#define SYS_CTRL_ENABLE_SYS_MID_DMA_DISABLE (0<<2)
#define SYS_CTRL_ENABLE_SYS_MID_XCPU_ENABLE (1<<3)
#define SYS_CTRL_ENABLE_SYS_MID_XCPU_DISABLE (0<<3)
#define SYS_CTRL_ENABLE_SYS_MID_AHB2AHB_ENABLE (1<<4)
#define SYS_CTRL_ENABLE_SYS_MID_AHB2AHB_DISABLE (0<<4)
#define SYS_CTRL_ENABLE_SYS_MID_IFC_ENABLE (1<<5)
#define SYS_CTRL_ENABLE_SYS_MID_IFC_DISABLE (0<<5)
#define SYS_CTRL_ENABLE_SYS_MID_USB_ENABLE (1<<6)
#define SYS_CTRL_ENABLE_SYS_MID_USB_DISABLE (0<<6)
#define SYS_CTRL_ENABLE_SYS_MID_GOUDA_ENABLE (1<<7)
#define SYS_CTRL_ENABLE_SYS_MID_GOUDA_DISABLE (0<<7)
#define SYS_CTRL_BB_NEW_ARBITRATION_ENABLE (1<<16)
#define SYS_CTRL_BB_NEW_ARBITRATION_DISABLE (0<<16)
#define SYS_CTRL_ENABLE_BB_MID_BIST_ENABLE (1<<17)
#define SYS_CTRL_ENABLE_BB_MID_BIST_DISABLE (0<<17)
#define SYS_CTRL_ENABLE_BB_MID_VOC_ENABLE (1<<18)
#define SYS_CTRL_ENABLE_BB_MID_VOC_DISABLE (0<<18)
#define SYS_CTRL_ENABLE_BB_MID_BCPU_ENABLE (1<<19)
#define SYS_CTRL_ENABLE_BB_MID_BCPU_DISABLE (0<<19)
#define SYS_CTRL_ENABLE_BB_MID_AHB2AHB_ENABLE (1<<20)
#define SYS_CTRL_ENABLE_BB_MID_AHB2AHB_DISABLE (0<<20)
#define SYS_CTRL_ENABLE_BB_MID_IFC_ENABLE (1<<21)
#define SYS_CTRL_ENABLE_BB_MID_IFC_DISABLE (0<<21)
#define SYS_CTRL_USB_RAM_TEST_MODE_ENABLE (1<<31)
#define SYS_CTRL_USB_RAM_TEST_MODE_DISABLE (0<<31)
#define SYS_CTRL_SYS_ENABLE(n)      (((n)&0x7F)<<1)
#define SYS_CTRL_SYS_ENABLE_MASK    (0x7F<<1)
#define SYS_CTRL_SYS_ENABLE_SHIFT   (1)
#define SYS_CTRL_BB_ENABLE(n)       (((n)&31)<<17)
#define SYS_CTRL_BB_ENABLE_MASK     (31<<17)
#define SYS_CTRL_BB_ENABLE_SHIFT    (17)

//Ctrl_AHB
#define SYS_CTRL_SPLIT_SYS_MID_BIST_NORMAL (1<<0)
#define SYS_CTRL_SPLIT_SYS_MID_BIST_FORCE (0<<0)
#define SYS_CTRL_SPLIT_SYS_MID_DMA_NORMAL (1<<1)
#define SYS_CTRL_SPLIT_SYS_MID_DMA_FORCE (0<<1)
#define SYS_CTRL_SPLIT_SYS_MID_XCPU_NORMAL (1<<2)
#define SYS_CTRL_SPLIT_SYS_MID_XCPU_FORCE (0<<2)
#define SYS_CTRL_SPLIT_SYS_MID_AHB2AHB_NORMAL (1<<3)
#define SYS_CTRL_SPLIT_SYS_MID_AHB2AHB_FORCE (0<<3)
#define SYS_CTRL_SPLIT_SYS_MID_IFC_NORMAL (1<<4)
#define SYS_CTRL_SPLIT_SYS_MID_IFC_FORCE (0<<4)
#define SYS_CTRL_SPLIT_SYS_MID_USB_NORMAL (1<<5)
#define SYS_CTRL_SPLIT_SYS_MID_USB_FORCE (0<<5)
#define SYS_CTRL_SPLIT_SYS_MID_GOUDA_NORMAL (1<<6)
#define SYS_CTRL_SPLIT_SYS_MID_GOUDA_FORCE (0<<6)
#define SYS_CTRL_SPLIT_BB_MID_BIST_NORMAL (1<<16)
#define SYS_CTRL_SPLIT_BB_MID_BIST_FORCE (0<<16)
#define SYS_CTRL_SPLIT_BB_MID_VOC_NORMAL (1<<17)
#define SYS_CTRL_SPLIT_BB_MID_VOC_FORCE (0<<17)
#define SYS_CTRL_SPLIT_BB_MID_BCPU_NORMAL (1<<18)
#define SYS_CTRL_SPLIT_BB_MID_BCPU_FORCE (0<<18)
#define SYS_CTRL_SPLIT_BB_MID_AHB2AHB_NORMAL (1<<19)
#define SYS_CTRL_SPLIT_BB_MID_AHB2AHB_FORCE (0<<19)
#define SYS_CTRL_SPLIT_BB_MID_IFC_NORMAL (1<<20)
#define SYS_CTRL_SPLIT_BB_MID_IFC_FORCE (0<<20)
#define SYS_CTRL_SYS_FORCE_HSPLIT(n) (((n)&0x7F)<<0)
#define SYS_CTRL_SYS_FORCE_HSPLIT_MASK (0x7F<<0)
#define SYS_CTRL_SYS_FORCE_HSPLIT_SHIFT (0)
#define SYS_CTRL_BB_FORCE_HSPLIT(n) (((n)&31)<<16)
#define SYS_CTRL_BB_FORCE_HSPLIT_MASK (31<<16)
#define SYS_CTRL_BB_FORCE_HSPLIT_SHIFT (16)

//XCpu_Dbg_BKP
#define SYS_CTRL_BKPT_EN            (1<<0)
#define SYS_CTRL_BKPT_MODE(n)       (((n)&3)<<4)
#define SYS_CTRL_BKPT_MODE_I        (0<<4)
#define SYS_CTRL_BKPT_MODE_R        (1<<4)
#define SYS_CTRL_BKPT_MODE_W        (2<<4)
#define SYS_CTRL_BKPT_MODE_RW       (3<<4)
#define SYS_CTRL_STALLED            (1<<8)

//XCpu_Dbg_Addr
#define SYS_CTRL_BREAKPOINT_ADDRESS(n) (((n)&0x3FFFFFF)<<0)

//BCpu_Dbg_BKP
//#define SYS_CTRL_BKPT_EN          (1<<0)
//#define SYS_CTRL_BKPT_MODE(n)     (((n)&3)<<4)
//#define SYS_CTRL_BKPT_MODE_I      (0<<4)
//#define SYS_CTRL_BKPT_MODE_R      (1<<4)
//#define SYS_CTRL_BKPT_MODE_W      (2<<4)
//#define SYS_CTRL_BKPT_MODE_RW     (3<<4)
//#define SYS_CTRL_STALLED          (1<<8)

//BCpu_Dbg_Addr
//#define SYS_CTRL_BREAKPOINT_ADDRESS(n) (((n)&0x3FFFFFF)<<0)

//Cfg_Cpus_Cache_Ram_Disable
#define SYS_CTRL_XCPU_USE_MODE      (1<<0)
#define SYS_CTRL_XCPU_CLK_OFF_MODE  (1<<1)
#define SYS_CTRL_BCPU_USE_MODE      (1<<16)
#define SYS_CTRL_BCPU_CLK_OFF_MODE  (1<<17)
#define SYS_CTRL_XCPU_CACHE_RAM_DISABLE(n) (((n)&3)<<0)
#define SYS_CTRL_XCPU_CACHE_RAM_DISABLE_MASK (3<<0)
#define SYS_CTRL_XCPU_CACHE_RAM_DISABLE_SHIFT (0)
#define SYS_CTRL_BCPU_CACHE_RAM_DISABLE(n) (((n)&3)<<16)
#define SYS_CTRL_BCPU_CACHE_RAM_DISABLE_MASK (3<<16)
#define SYS_CTRL_BCPU_CACHE_RAM_DISABLE_SHIFT (16)

//Reset_Cause
#define SYS_CTRL_WATCHDOG_RESET_HAPPENED (1<<0)
#define SYS_CTRL_WATCHDOG_RESET_NO  (0<<0)
#define SYS_CTRL_GLOBALSOFT_RESET_HAPPENED (1<<4)
#define SYS_CTRL_GLOBALSOFT_RESET_NO (0<<4)
#define SYS_CTRL_HOSTDEBUG_RESET_HAPPENED (1<<8)
#define SYS_CTRL_HOSTDEBUG_RESET_NO (0<<8)
#define SYS_CTRL_ALARMCAUSE_HAPPENED (1<<12)
#define SYS_CTRL_ALARMCAUSE_NO      (0<<12)
#define SYS_CTRL_BOOT_MODE(n)       (((n)&0x3F)<<16)
#define SYS_CTRL_BOOT_MODE_MASK     (0x3F<<16)
#define SYS_CTRL_BOOT_MODE_SHIFT    (16)
#define SYS_CTRL_SW_BOOT_MODE(n)    (((n)&0x3F)<<22)
#define SYS_CTRL_SW_BOOT_MODE_MASK  (0x3F<<22)
#define SYS_CTRL_SW_BOOT_MODE_SHIFT (22)
#define SYS_CTRL_FONCTIONAL_TEST_MODE (1<<31)

//WakeUp
#define SYS_CTRL_FORCE_WAKEUP       (1<<0)

//Ignore_Charger
#define SYS_CTRL_IGNORE_CHARGER     (1<<0)


#endif
