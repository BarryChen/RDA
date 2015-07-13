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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/pmd/rda1203_gallite/include/rda1203.h $                                                                   
//	$Author: huazeng $                                                                    
//	$Date: 2010-12-31 15:46:42 +0800 (星期五, 31 十二月 2010) $                                                                      
//	$Revision: 5033 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _OPAL_H_
#define _OPAL_H_

// =============================================================================
//  MACROS
// =============================================================================

typedef enum {
    RDA_ADDR_REVISION_ID                      = 0x00000000,
    RDA_ADDR_IRQ_SETTINGS                     = 0x00000001,
    RDA_ADDR_LDO_SETTINGS                     = 0x00000002,
    RDA_ADDR_LDO_ACTIVE_SETTING1              = 0x00000003,
    RDA_ADDR_LDO_ACTIVE_SETTING2              = 0x00000004,
    RDA_ADDR_LDO_ACTIVE_SETTING3              = 0x00000005,
    RDA_ADDR_LDO_ACTIVE_SETTING4              = 0x00000006,
    RDA_ADDR_LDO_ACTIVE_SETTING5              = 0x00000007,
    RDA_ADDR_LDO_LP_SETTING1                  = 0x00000008,
    RDA_ADDR_LDO_LP_SETTING2                  = 0x00000009,
    RDA_ADDR_LDO_LP_SETTING3                  = 0x0000000A,
    RDA_ADDR_LDO_LP_SETTING4                  = 0x0000000B,
    RDA_ADDR_LDO_LP_SETTING5                  = 0x0000000C,
    RDA_ADDR_LDO_POWER_ON_SETTING1            = 0x0000000D,
    RDA_ADDR_LDO_POWER_ON_SETTING2            = 0x0000000E,
    RDA_ADDR_LDO_POWER_ON_SETTING3            = 0x0000000F,
    RDA_ADDR_LDO_POWER_ON_SETTING4            = 0x00000010,
    RDA_ADDR_LDO_POWER_ON_SETTING5            = 0x00000011,
    RDA_ADDR_CHARGER_SETTING1                 = 0x00000012,
    RDA_ADDR_CHARGER_SETTING2                 = 0x00000013,
    RDA_ADDR_CHARGER_STATUS                   = 0x00000014,
    RDA_ADDR_CHARGER_CONTROL                  = 0x00000015,
    RDA_ADDR_LDO_CALIBRATION_SETTING1         = 0x00000016,
    RDA_ADDR_LDO_CALIBRATION_SETTING2         = 0x00000017,
    RDA_ADDR_LDO_MISC_CONTROL                 = 0x00000018,
    RDA_ADDR_LED_SETTING1                     = 0x00000019,
    RDA_ADDR_LED_SETTING2                     = 0x0000001A,
    RDA_ADDR_LED_SETTING3                     = 0x0000001B,
    RDA_ADDR_AUDIO_CONTROL                    = 0x0000001C,
    RDA_ADDR_TOUCH_SCREEN_CONTROL             = 0x0000001D,
    RDA_ADDR_TOUCH_SCREEN_RESULTS1            = 0x0000001E,
    RDA_ADDR_TOUCH_SCREEN_RESULTS2            = 0x0000001F,
    RDA_ADDR_TOUCH_SCREEN_RESULTS3            = 0x00000020,
    RDA_ADDR_EFUSE_OPT_SETTING1               = 0x00000021,
    RDA_ADDR_EFUSE_OPT_SETTING2               = 0x00000022,
    RDA_ADDR_EFUSE_OPT_SETTING3               = 0x00000023,
    RDA_ADDR_EFUSE_OPT_SETTING4               = 0x00000024,
    RDA_ADDR_EFUSE_OPT_SETTING5               = 0x00000025,
    RDA_ADDR_SIM_INTERFACE                    = 0x00000026,
    RDA_ADDR_LDO_POWER_OFF_SETTING1           = 0x00000031,
    RDA_ADDR_LDO_POWER_OFF_SETTING2           = 0x00000032,
    RDA_ADDR_LDO_POWER_OFF_SETTING3           = 0x00000033,
    RDA_ADDR_LDO_POWER_OFF_SETTING4           = 0x00000034,
    RDA_ADDR_LDO_POWER_OFF_SETTING5           = 0x00000035,
    RDA_ADDR_THERMAL_CALIBRATION              = 0x00000036,
    RDA_ADDR_MISC                             = 0x00000037,
/// Use this number as paramter to the debug function instead of a register number
/// to poll all Opal Registers and update the memory structure.
    RDA_READ_ALL                              = 0x00000080,
/// Use this number as paramter to the debug function instead of a register number
/// to poll the GPADC Registers and update the memory structure.
    RDA_READ_ALL_GPADC                        = 0x00000081
} RDA_REG_MAP_T;

// IRQ settings 
#define RDA1203_EOMIRQ_MASK         (1<<9)
#define RDA1203_PENIRQ_MASK          (1<<8)
#define RDA1203_PD_MODE_SEL           (1<<0)
#define RDA1203_PENIRQ_CLEAR           (1<<10)
#define RDA1203_EOMIRQ_CLEAR           (1<<11)
#define RDA1203_INT_CHR_MASK           (1<<4)
#define RDA1203_INT_CHR_CLEAR           (1<<5)
#define RDA1203_INT_CHR_CAUSE           (1<<6)
#define RDA1203_INT_CHR_ON           (1<<7)
#define RDA1203_PENIRQ     (1<<14)
// end IRQ settings

//LDO settings 0x02
#define RDA1203_LDO_SETTINGS(n)          (((n)&0xfff)<<0)
#define RDA1203_LDO_MASK                 (12<<0)
#define RDA1203_LDO_SHIFT                (0)
#define RDA1203_LDO_GPADC                (1<<0)
#define RDA1203_LDO_TSC                  (1<<1)
#define RDA1203_LDO_LCD                  (1<<2)
#define RDA1203_LDO_CAMERA               (1<<3)
#define RDA1203_LDO_MIC                  (1<<4)
#define RDA1203_LDO_VIBRATE              (1<<5)
#define RDA1203_LDO_USB                  (1<<6)
#define RDA1203_LDO_SIM2                 (1<<7)
#define RDA1203_LDO_SIM1                 (1<<8)
#define RDA1203_LDO_RF                   (1<<9)
#define RDA1203_LDO_ABB                  (1<<10)
#define RDA1203_LDO_MMC                  (1<<11)

//LDO active setting 0x03
#define RDA1203_ACT_LP_MODE              (1<<0)
#define RDA1203_ACT_SIM2                 (1<<1)
#define RDA1203_ACT_SIM1                 (1<<2)
#define RDA1203_ACT_MIC                  (1<<3)
#define RDA1203_ACT_USB                  (1<<4)
#define RDA1203_ACT_VIBR                 (1<<5)
#define RDA1203_ACT_MMC                  (1<<6)
#define RDA1203_ACT_LCD                  (1<<7)
#define RDA1203_ACT_CAMERA               (1<<8)
#define RDA1203_ACT_RF                   (1<<9)
#define RDA1203_ACT_ABB                  (1<<10)
#define RDA1203_ACT_PAD                  (1<<11)
#define RDA1203_ACT_MEMO                 (1<<12)
#define RDA1203_ACT_CORE                 (1<<13)
#define RDA1203_ACT_LDO_CORE             (1<<14)
#define RDA1203_ACT_RTC_SEL1V6           (1<<15)


#define OPAL_LDO_EN(n)              (((n)&0x7FFF)<<0)
#define OPAL_LDO_EN_MASK            (0x7FFF<<0)
#define OPAL_LDO_EN_SHIFT           (0)


// LDO sel  0x04
#if 0
#define RDA1203_ACT_RTC_SEL(n)      (((n)&7)<<0)
#define RDA1203_ACT_RTC_SEL_MASK    (7<<0)
#define RDA1203_ACT_RTC_SEL_SHIFT   (0)
#define RDA1203_ACT_RTC_SEL1V5   (1<<0)
#define RDA1203_ACT_RTC_SEL1V3   (2<<0)
#endif
#define RDA1203_ACT_SIM2_SEL1V8      (1<<3)
#define RDA1203_ACT_SIM1_SEL1V8      (1<<4)
#define RDA1203_ACT_SIM2_SEL2V8      (0<<3)
#define RDA1203_ACT_SIM1_SEL2V8      (0<<4)
#define RDA1203_ACT_SIM2_SEL_MASK      (1<<3)
#define RDA1203_ACT_SIM1_SEL_MASK      (1<<4)

#define RDA1203_ACT_MIC_SEL(n)      (((n)&7)<<5)
#define RDA1203_ACT_MIC_SEL_MASK    (7<<5)
#define RDA1203_ACT_MIC_SEL_SHIFT   (5)
#define RDA1203_ACT_MIC_SEL2V8   (4<<5)
#define RDA1203_ACT_MIC_SEL1V8   (6<<5)
//#define RDA1203_ACT_MIC_SEL1V57   (7<<5)
#define RDA1203_ACT_MIC_SEL1V57   (0<<5)
#define RDA1203_ACT_MIC_SEL2V2   (4<<5)
#define RDA1203_ACT_MIC_SEL2V0   (2<<5)
 #define RDA1203_ACT_MIC_SEL1V5   (5<<5)

#define RDA1203_ACT_VIBRATE_SEL_MASK   (1<<8)
#define RDA1203_ACT_VIBRATE_SEL1V8   (1<<8)
#define RDA1203_ACT_VIBRATE_SEL2V8   (0<<8)

#define RDA1203_ACT_MMC_SEL_MASK   (1<<9)
#define RDA1203_ACT_MMC_SEL1V8   (1<<9)
#define RDA1203_ACT_MMC_SEL2V8   (0<<9)

#define RDA1203_ACT_LCD_SEL_MASK   (1<<10)
#define RDA1203_ACT_LCD_SEL1V8   (1<<10)
#define RDA1203_ACT_LCD_SEL2V8   (0<<10)

#define RDA1203_ACT_CAMERA_SEL_MASK   (1<<11)
#define RDA1203_ACT_CAMERA_SEL1V8   (1<<11)
#define RDA1203_ACT_CAMERA_SEL2V8   (0<<11)

#define RDA1203_ACT_RF_SEL_MASK   (1<<12)
#define RDA1203_ACT_RF_SEL1V8   (1<<12)
#define RDA1203_ACT_RF_SEL2V8   (0<<12)

#define RDA1203_ACT_PAD_SEL_MASK   (1<<13)
#define RDA1203_ACT_PAD_SEL1V8   (1<<13)
#define RDA1203_ACT_PAD_SEL2V8   (0<<13)
#if 0
#define RDA1203_ACT_CORE_SEL(n)      (((n)&3)<<14)
#define RDA1203_ACT_CORE_SEL_MASK    (3<<14)
#define RDA1203_ACT_CORE_SEL_SHIFT   (14)
#define RDA1203_ACT_CORE_SEL1V5   (1<<14)
#define RDA1203_ACT_CORE_SEL1V3   (2<<14)
#endif


// LDO ACT/LP setting3 0x5/0xa
#define RDA1203_LDO_ABB_CURRENT_SHIFT      (0)
#define RDA1203_LDO_ABB_CURRENT_MASK       (7<<0)
#define RDA1203_LDO_ABB_CURRENT(n)         (((n)&7)<<0)
#define RDA1203_LDO_IO_CURRENT_SHIFT       (3)
#define RDA1203_LDO_IO_CURRENT_MASK        (7<<3)
#define RDA1203_LDO_IO_CURRENT(n)          (((n)&7)<<3)
#define RDA1203_LDO_MEM_CURRENT_SHIFT      (6)
#define RDA1203_LDO_MEM_CURRENT_MASK       (7<<6)
#define RDA1203_LDO_MEM_CURRENT(n)         (((n)&7)<<6)
#define RDA1203_DCDC_VCORE_CURRENT_SHIFT   (9)
#define RDA1203_DCDC_VCORE_CURRENT_MASK    (7<<9)
#define RDA1203_DCDC_VCORE_CURRENT(n)      (((n)&7)<<9)
#define RDA1203_LDO_VCORE_CURRENT_SHIFT    (12)
#define RDA1203_LDO_VCORE_CURRENT_MASK     (7<<12)
#define RDA1203_LDO_VCORE_CURRENT(n)       (((n)&7)<<12)


// LDO LP  setting1  0x8
#define RDA1203_LP_LDO_MODE                (1<<0)
#define RDA1203_LP_LDO_VSIM2                (1<<1)
#define RDA1203_LP_LDO_VSIM1                (1<<2)
#define RDA1203_LP_LDO_VMIC                (1<<3)
#define RDA1203_LP_LDO_VUSB                (1<<4)
#define RDA1203_LP_LDO_VIBRATE                (1<<5)
#define RDA1203_LP_LDO_VMMC               (1<<6)
#define RDA1203_LP_LDO_VLCD                (1<<7)
#define RDA1203_LP_LDO_VCAM                (1<<8)
#define RDA1203_LP_LDO_VRF               (1<<9)
#define RDA1203_LP_LDO_VABB              (1<<10)
#define RDA1203_LP_LDO_VPAD               (1<<11)
#define RDA1203_LP_LDO_VMEM              (1<<12)
#define RDA1203_LP_LDO_DCDC_CORE              (1<<13)
#define RDA1203_LP_LDO_LDO_CORE             (1<<14)
// end LDO LP  setting1  0x8

// LDO LP SETTING2  0x09h
#define RDA1203_LP_RTC_SEL(n)      (((n)&7)<<0)
#define RDA1203_LP_RTC_SEL_MASK    (7<<0)
#define RDA1203_LP_RTC_SEL_SHIFT   (0)
#define RDA1203_LP_RTC_SEL1V5   (1<<0)
#define RDA1203_LP_RTC_SEL1V3   (2<<0)
#define RDA1203_LP_SIM2_SEL      (1<<3)
#define RDA1203_LP_SIM1_SEL      (1<<4)
#define RDA1203_LP_MIC_SEL(n)      (((n)&7)<<5)
#define RDA1203_LP_MIC_SEL_MASK    (7<<5)
#define RDA1203_LP_MIC_SEL_SHIFT   (5)
#define RDA1203_LP_MIC_SEL2V8   (1<<5)
#define RDA1203_LP_MIC_SEL1V8   (2<<5)
#define RDA1203_LP_VIBRATE_SEL   (1<<8)
#define RDA1203_LP_MEMO_SEL   (1<<9)
#define RDA1203_LP_LCD_SEL   (1<<10)
#define RDA1203_LP_CAMERA_SEL   (1<<11)
#define RDA1203_LP_RF_SEL   (1<<12)
#define RDA1203_LP_PAD_SEL   (1<<13)
#define RDA1203_LP_CORE_SEL(n)      (((n)&3)<<14)
#define RDA1203_LP_CORE_SEL_MASK    (3<<14)
#define RDA1203_LP_CORE_SEL_SHIFT   (14)
#define RDA1203_LP_CORE_SEL1V5   (1<<14)
#define RDA1203_LP_CORE_SEL1V3   (2<<14)
//end LDO LP setting

// Charger setting 1 / 0x12
#define RDA1203_UV_SEL(n)         (((n)&3)<<8)
#define RDA1203_UV_SEL_MASK       (3<<8)
#define RDA1203_PRECH_VSEL(n)     (((n)&7)<<10)
#define RDA1203_PRECH_VSEL_MASK   (7<<10)
#define RDA1203_CHG_CUR_SEL_MASK  (7<<13)
#define RDA1203_CHG_CUR_SEL_650MA (7<<13)
#define RDA1203_CHG_CUR_SEL_550MA (6<<13)
#define RDA1203_CHG_CUR_SEL_400MA (5<<13)
#define RDA1203_CHG_CUR_SEL_300MA (4<<13)   // 310 mA
#define RDA1203_CHG_CUR_SEL_200MA (3<<13)   // 227 mA
#define RDA1203_CHG_CUR_SEL_150MA (2<<13)   // 147 mA
#define RDA1203_CHG_CUR_SEL_100MA (1<<13)   // 93 mA
#define RDA1203_CHG_CUR_SEL_50MA  (0<<13)   // 53 mA
// End of charger setting 1

// Charger setting2 / 0x13
#define RDA1203_CHR_VFB_SEL(n)    (((n)&0xf)<<5)
#define RDA1203_CHR_VFB_SEL_MASK  (0xf<<5)
#define RDA1203_CHR_VFB_SEL_DR    (1<<9)
#define RDA1203_PRECH_VSEL_DR     (1<<10)

// charger status 0x14
#define RDA1203_PENIRQ_B   (1<<11)
#define RDA1203_CHR_AC_ON   (1<<6)
#define RDA1203_CHR_VREG   (1<<4)
#define RDA1203_LOW_BAT   (1<<0)
#define RDA1203_CHR_TAPER   (1<<3)
#define RDA1203_CHR_TERM   (1<<2)
// end charger status

// Charger calib setting 1
#define RDA1203_CHR_BGAP_CAL_RESETN_DR   (1<<1)
#define RDA1203_CHR_BGAP_CAL_RESETN_REG  (1<<0)
// End of charget calib setting 1

// LED setting1 0x19
#define RDA1203_DIM_KP_REG               (1<<0)
#define RDA1203_PWM_KP_ENABLE            (1<<1)
#define RDA1203_DIM_BL_REG               (1<<2)
#define RDA1203_DIM_BL_DR                (1<<3)
#define RDA1203_PWM_BL_MODE              (1<<4)
#define RDA1203_BOOST_BIT(n)             (((n)&3)<<5)
#define RDA1203_BOOST_BIT_MASK           (3<<5)
#define RDA1203_BOOST_BIT_SHIFT          (5)
#define RDA1203_BOOST_ENABLE             (1<<7)
#define RDA1203_PD_BL_LP                 (1<<8)
#define RDA1203_PD_BL_ACT                (1<<9)
#define RDA1203_PD_BL_PON                (1<<10)
#define RDA1203_PD_KP_LP                 (1<<11)
#define RDA1203_PD_KP_ACT                (1<<12)
#define RDA1203_PD_KP_PON                (1<<13)
// end LED setting1 0x19

// LED setting2 0x1a
#define RDA1203_I_BIT_ACT_BL(n)          (((n)&0xf)<<8)
#define RDA1203_I_BIT_ACT_BL_MASK        (0xf<<8)
#define RDA1203_I_BIT_ACT_BL_SHIFT       (8)
// end LED setting2 0x1a

// LED setting3 0x1b
#define RDA1203_PWM_KP_DUTY_CYCLE(n)     (((n)&0x1f)<<0)
#define RDA1203_PWM_KP_DUTY_CYCLE_MASK   (0x1f<<0)
#define RDA1203_PWM_KP_DUTY_CYCLE_SHIFT  (0)
// end LED setting3 0x1b

// audio control 0x1c
#define RDA1203_TIMER_PEN_IRQ_STABLE(n)      (((n)&3)<<2)
#define RDA1203_TIMER_PEN_IRQ_STABLE_MASK    (3<<2)
#define RDA1203_TIMER_PEN_IRQ_STABLE_SHIFT   (2)
#define RDA1203_TIMER_PEN_IRQ_STABLE_2US   (0<<2)
#define RDA1203_TIMER_PEN_IRQ_STABLE_4US   (1<<2)
#define RDA1203_TIMER_PEN_IRQ_STABLE_8US   (2<<2)
#define RDA1203_TIMER_PEN_IRQ_STABLE_16US   (3<<2)
#define RDA1203_TEST_CLASSAB                (1<<4)
#define RDA1203_MUTE_CLASSAB                 (1<<5)
#define RDA1203_PU_CLASSAB_L                 (1<<7)
#define RDA1203_PU_CLASSAB_R                 (1<<6)
#define RDA1203_AMP_GAIN(n)                (((n)&7)<<8)
#define RDA1203_AMP_GAIN_MASK                (7<<8)
#define RDA1203_AMP_GAIN_SHIFT               (8)
#define RDA1203_PWM_CLK_DIV2_ENABLE                (1<<11)
#define RDA1203_DELAY_BEFORE_SAMP(n)      (((n)&3)<<12)
#define RDA1203_DELAY_BEFORE_SAMP_MASK    (3<<12)
#define RDA1203_DELAY_BEFORE_SAMP_SHIFT   (12)
#define RDA1203_DELAY_BEFORE_SAMP_2US   (0<<12)
#define RDA1203_DELAY_BEFORE_SAMP_4US   (1<<12)
#define RDA1203_DELAY_BEFORE_SAMP_8US   (2<<12)
#define RDA1203_DELAY_BEFORE_SAMP_16US   (3<<12)
#define RDA1203_MEAS_WAIT_CLR_PENIRQ               (1<<14)
#define RDA1203_BYPASS_BB_READ               (1<<15)
// end audio control 0x1c

// touch screen control  0x1d
#define RDA1203_PD_SAR_REG                 (1<<0)
#define RDA1203_PD_SAR_DR                   (1<<1)
#define RDA1203_CONV_CLK_INV                (1<<2)
#define RDA1203_SAMP_CLK_INV                 (1<<3)
#define RDA1203_SAR_ADC_MODE                  (1<<4)
#define RDA1203_SAR_OUT_POLARITY               (1<<5)
#define RDA1203_WAIT_BB_READ_TIMESL(n)        (((n)&3)<<6)
#define RDA1203_WAIT_BB_READ_TIMESL_MASK       (3<<6)
#define RDA1203_WAIT_BB_READ_TIMESL_SHIFT      (6)
#define RDA1203_SAR_VERF_BIT(n)        (((n)&3)<<8)
#define RDA1203_SAR_VERF_BIT_MASK       (3<<8)
#define RDA1203_SAR_VERF_BIT_SHIFT      (8)
#define RDA1203_TSC_SAR_SEL(n)        (((n)&3)<<10)
#define RDA1203_TSC_SAR_SEL_MASK       (3<<10)
#define RDA1203_TSC_SAR_SEL_SHIFT      (10)
#define RDA1203_TSC_CH4                (1<<12)
#define RDA1203_TSC_CH3                (1<<13)
#define RDA1203_TSC_CH2               (1<<14)
#define RDA1203_TSC_CH1                (1<<15)

// end touch screen control 

// touch screen results 1 0x1e
#define RDA1203_TSC_X_VALUE_BIT(n)        (((n)&0x3ff)<<0)
#define RDA1203_TSC_X_VALUE_VALID          (1<<10)
// end touch screen results 1
// touch screen results 2 0x1F
#define RDA1203_TSC_Y_VALUE_BIT(n)        (((n)&0x3ff)<<0)
#define RDA1203_TSC_Y_VALUE_VALID          (1<<10)
// end touch screen results 2
// touch screen results 3 0x20
#define RDA1203_TSC_GPADC_VALUE_BIT(n)        (((n)&0x3ff)<<0)
#define RDA1203_TSC_GPADC_VALUE_VALID          (1<<10)
// end touch screen results 3

// Effuse and OTP setting 4 / 0x24
#define RDA1203_EFFUSE_CHR_VFB_SEL_MASK    (0xf<<0)
#define RDA1203_EFFUSE_CHR_VFB_SEL_SHIFT   (0)
#define RDA1203_EFFUSE_PRECH_VSEL_MASK     (7<<4)
#define RDA1203_EFFUSE_PRECH_VSEL_SHIFT    (4)
// end of effuse and OTP setting 4

// sim interface 0x26
#define RDA1203_SIM_SELECT                 (1<<15)
#define RDA1203_PULLUP_ENABLE_1            (1<<14)
#define RDA1203_UNSEL_CLK_VAL_1            (1<<13)
#define RDA1203_UNSEL_RST_VAL_1            (1<<12)
#define RDA1203_PULLUP_ENABLE_2            (1<<11)
#define RDA1203_UNSEL_CLK_VAL_2            (1<<10)
#define RDA1203_UNSEL_RST_VAL_2            (1<<9)
#define RDA1203_TIMER_BB_RESETN_SEL_MASK   (3<<4)
#define RDA1203_TIMER_BB_RESETN_SEL_10MS   (0<<4)
#define RDA1203_TIMER_BB_RESETN_SEL_40MS   (1<<4)
#define RDA1203_TIMER_BB_RESETN_SEL_60MS   (2<<4)
#define RDA1203_TIMER_BB_RESETN_SEL_100MS  (3<<4)
#define RDA1203_BYPASS_CLK_25K_GATE        (1<<3)
#define RDA1203_BYPASS_CLK_2M_GATE         (1<<2)
#define RDA1203_SELF_CAL_ENABLE_DR         (1<<1)
#define RDA1203_SELF_CAL_ENABLE_REG        (1<<0)
// end sim interface 

typedef enum {
    ANA                                         = 0x00000000,
    AUDIO                                       = 0x00000001,
    RF1                                         = 0x00000002,
    RF2                                         = 0x00000003,
    MMC                                         = 0x00000004,
    ACC                                         = 0x00000005,
    MEM                                         = 0x00000006,
    USB18                                       = 0x00000007,
    USB33                                       = 0x00000008,
    USIM                                        = 0x00000009,
    VIB                                         = 0x0000000A
} LDO_ID_T;


// =============================================================================
//  TYPES
// =============================================================================
// ============================================================================
// OPAL_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================


typedef struct
{
    REG16                          Charger_Ctrl;                 //0x00000000
    REG16                          Charger_State;                //0x00000002
    REG16                          Buck_Ctrl0;                   //0x00000004
    REG16                          Buck_Ctrl1;                   //0x00000006
    REG16                          Boost_Ctrl0;                  //0x00000008
    REG16                          Boost_Ctrl1;                  //0x0000000A
    REG16                          DC_ORCmp_Valid;               //0x0000000C
    REG16                          Sys_Ctrl;                     //0x0000000E
    REG16                          LDO_Global_Ctrl;              //0x00000010
    REG16                          LDO_Prot_Ctrl;                //0x00000012
    REG16                          LDO_Out_V_Ctrl;               //0x00000014
    REG16                          LDO_PSave_Ctrl;               //0x00000016
    REG16                          LED_Ctrl0;                    //0x00000018
    REG16                          LED_Ctrl1;                    //0x0000001A
    REG16                          LED_VDrop;                    //0x0000001C
    REG16                          BG_Ctrl;                      //0x0000001E
    REG16                          BG_Ref_Curr;                  //0x00000020
    REG16                          Speaker_Ctrl;                 //0x00000022
    REG16                          Clk_Ctrl;                     //0x00000024
    REG16                          Test_Mode;                    //0x00000026
    REG16 Reserved_00000028[11];                //0x00000028
    REG16                          Reg_Backup;                   //0x0000003E
    /// Profile register 1, define the profile of normal (active) state. 
    /// USB LDOs are enabled by charger detection and the control is inverted. (0:LDO
    /// on, 1:LDO power Down)
    REG16                          Profile_1;                    //0x00000040
    /// Profile register 2, define the profile of low power state state. 
    /// USB LDOs are enabled by charger detection and the control is inverted. (0:LDO
    /// on, 1:LDO power Down)
    REG16                          Profile_2;                    //0x00000042
    /// Profile register 3, define the profile of low power state state. 
    /// Unused.
    REG16                          Profile_3;                    //0x00000044
    /// Profile register 4, define the profile of low power state state. 
    /// Unused.
    REG16                          Profile_4;                    //0x00000046
    REG16                          Standby_On_Ctrl;              //0x00000048
    REG16 Reserved_0000004A[11];                //0x0000004A
    REG16                          GPADC_Ctrl;                   //0x00000060
    REG16                          Reg_TSC_X;                    //0x00000062
    REG16                          Reg_TSC_Y;                    //0x00000064
    REG16                          Reg_Stack1;                   //0x00000066
    REG16                          Reg_Stack2;                   //0x00000068
    REG16                          Reg_Stack3;                   //0x0000006A
    REG16                          Reg_Stack4;                   //0x0000006C
    REG16                          GPADC_Sig;                    //0x0000006E
    REG16                          TSC_Config;                   //0x00000070
    REG16 Reserved_00000072[7];                 //0x00000072
    REG16                          Special;                      //0x00000080
    REG16                          Rst_GPADC;                    //0x00000082
    REG16                          Rst_LED;                      //0x00000084
    REG16 Reserved_00000086;                    //0x00000086
    REG16                          Rst_REG;                      //0x00000088
} OPAL_T; //Size : 0x8A




// ============================================================================
// PMD_OPAL_DEBUG_FUNC_PARAM_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================


typedef struct
{
    UINT32                         Param;                        //0x00000000
} PMD_OPAL_DEBUG_FUNC_PARAM_T; //Size : 0x4

//Param
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_WRITE_FLAG (1<<30)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_WRITE_FLAG_MASK (1<<30)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_WRITE_FLAG_SHIFT (30)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG(n) (((n)&0xFF)<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_MASK (0xFF<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_SHIFT (16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_CHARGER_CTRL (0x0<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_CHARGER_STATE (0x1<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_BUCK_CTRL0 (0x2<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_BUCK_CTRL1 (0x3<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_BOOST_CTRL0 (0x4<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_BOOST_CTRL1 (0x5<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_DC_ORCMP_VALID (0x6<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_SYS_CTRL (0x7<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_LDO_GLOBAL_CTRL (0x8<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_LDO_PROT_CTRL (0x9<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_LDO_OUT_V_CTRL (0xA<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_LDO_PSAVE_CTRL (0xB<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_LED_CTRL0 (0xC<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_LED_CTRL1 (0xD<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_LED_VDROP (0xE<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_BG_CTRL (0xF<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_BG_REF_CURR (0x10<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_SPEAKER_CTRL (0x11<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_CLK_CTRL (0x12<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_TEST_MODE (0x13<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_REG_BACKUP (0x1F<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_PROFILE_1 (0x20<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_PROFILE_2 (0x21<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_PROFILE_3 (0x22<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_PROFILE_4 (0x23<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_STANDBY_ON_CTRL (0x24<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_GPADC_CTRL (0x30<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_REG_TSC_X (0x31<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_REG_TSC_Y (0x32<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_REG_STACK1 (0x33<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_REG_STACK2 (0x34<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_REG_STACK3 (0x35<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_REG_STACK4 (0x36<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_GPADC_SIG (0x37<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_TSC_CONFIG (0x38<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_SPECIAL (0x40<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_RST_GPADC (0x41<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_RST_LED (0x42<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_ADDR_RST_REG (0x44<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_READ_ALL (0x80<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_REG_OPAL_READ_ALL_GPADC (0x81<<16)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_VALUE(n) (((n)&0xFFFF)<<0)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_VALUE_MASK (0xFFFF<<0)
#define PMD_OPAL_DEBUG_FUNC_PARAM_T_VALUE_SHIFT (0)


typedef enum {
    CHR_STATUS_UNKNOWN,
    CHR_STATUS_DC_OFF,
    CHR_STATUS_FAST,
    CHR_STATUS_TAPER,
    CHR_STATUS_FULL,
} RDA1203_CHR_STATUS_T;


// =============================================================================
//  MACROS
// =============================================================================

/// to extract bitfield from register value
#define OPAL_GET_BITFIELD(dword, bitfield) (((dword) & (bitfield ## _MASK)) >> (bitfield ## _SHIFT))

/// to build commands for pmd_OpalSpiSendCommand
#define PMD_OPAL_SPI_BUILD_WRITE_COMMAND(cmd,regIdx,val) \
{ (*cmd) = (0<<25) | ((regIdx & 0x1ff) << 16) | (val & 0xffff); }

#define PMD_OPAL_SPI_BUILD_READ_COMMAND(cmd,regIdx) \
{ (*cmd) = (1<<25) | ((regIdx & 0x1ff) << 16) | (val & 0xffff); }

#define PMD_OPAL_SPI_EXTRACT_DATA(data) ((UINT16) (((data)[1] << 8) | (data)[2]))

// =============================================================================
//  FUNCTIONS
// =============================================================================

//=============================================================================
// pmd_opalDebugFunction
//-----------------------------------------------------------------------------
/// Read Opal register
/// @param param Write_Flag, Reg register to read/write, write Value
/// @return register value
/// This function exist and is placed in pmd_map only 
/// when PMD_OPAL_DEBUG_FUNC is defined to 1
/// in Coolwatcher:
///  * require "opal"
///  * help Opal
//=============================================================================
//PROTECTED UINT16 pmd_opalDebugFunction(PMD_OPAL_DEBUG_FUNC_PARAM_T);

//=============================================================================
// pmd_OpalWrite
//-----------------------------------------------------------------------------
/// Read Opal register
/// @param regIdx register to read
/// @param value register value
///
/// Note: this function completely handles the SPI control
//=============================================================================
PUBLIC VOID pmd_RDAWrite(RDA_REG_MAP_T regIdx, UINT16 data);

//=============================================================================
// pmd_OpalRead
//-----------------------------------------------------------------------------
/// Read Opal register
/// @param regIdx register to read
/// @return register value
///
/// Note: this function completely handles the SPI control
//=============================================================================
PUBLIC UINT16 pmd_RDARead(RDA_REG_MAP_T regIdx);


// =============================================================================
//  Special API with lower level SPI handling (for multiple register access)
// =============================================================================

//=============================================================================
// pmd_OpalSpiActivateCs
//-----------------------------------------------------------------------------
/// Activate Opal SPI. refer to hal_SpiActivateCs() for details.
/// @return \c TRUE if the cs has been successfully activated
/// \c FALSE when another cs is already active
// =============================================================================
PUBLIC BOOL pmd_OpalSpiActivateCs(VOID);

//=============================================================================
// pmd_OpalSpiDeactivateCs
//-----------------------------------------------------------------------------
/// Deactivate Opal SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PUBLIC VOID pmd_OpalSpiDeactivateCs(VOID);

//=============================================================================
// pmd_OpalSpiSendCommand
//-----------------------------------------------------------------------------
/// Send a burst of commands to Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results ensure the command has been effectively send).
/// @param cmd array of bytes containing the command, use #PMD_OPAL_SPI_BUILD_WRITE_COMMAND and #PMD_OPAL_SPI_BUILD_READ_COMMAND.
/// @param length number of SPI command to send (cmd array must by 3 time that)
//=============================================================================
PUBLIC VOID pmd_OpalSpiSendCommand(CONST UINT32* cmd, UINT32 length);

//=============================================================================
// pmd_OpalSpiGetData
//-----------------------------------------------------------------------------
/// Read a burst of data from Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results before sending too much new commands).
/// @param data array of bytes to receive the data, use #PMD_OPAL_SPI_EXTRACT_DATA
/// @param length number of SPI command recieve (data array must by 3 time that)
//=============================================================================
PUBLIC VOID pmd_OpalSpiGetData(UINT32* data, UINT32 length);


PUBLIC BOOL pmd_SpiCsNonblockingActivate(BOOL singleWrite);
PUBLIC VOID pmd_OpalCsDeactivate(BOOL singleWrite);
PUBLIC UINT16 pmd_OpalSpiRead(RDA_REG_MAP_T regIdx);
PUBLIC VOID pmd_OpalSpiWrite(RDA_REG_MAP_T regIdx, UINT16 value);
// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingSet(UINT32 flags);
// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingClr(UINT32 flags);


VOID pmd_InitRda1203(VOID);

BOOL pmd_SelectSimCard(UINT8 sim_card);

BOOL pmd_TSDGetSPI(VOID);
BOOL pmd_TSDReleaseSPI(VOID);
BOOL pmd_TsdReadCoordinatesInternal(UINT16* gpadc_x, UINT16* gpadc_y);
BOOL pmd_TsdClearTouchIrq(VOID);
BOOL pmd_ResetTouch(VOID);
VOID pmd_TsdClearTouchIrqInternal(VOID);
VOID pmd_TsdClearEomIrqInternal(VOID);
BOOL pmd_TsdEnableIrq(VOID);
BOOL pmd_TsdDisableIrq(VOID);

PROTECTED RDA1203_CHR_STATUS_T pmd_GetChargerHwStatus(VOID);

#endif
