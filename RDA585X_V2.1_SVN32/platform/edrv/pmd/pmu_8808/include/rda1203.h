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
    RDA_ADDR_REVISION_ID                      = 0x00,
    RDA_ADDR_IRQ_SETTINGS                     = 0x01,
    RDA_ADDR_LDO_SETTINGS                     = 0x02,
    RDA_ADDR_LDO_ACTIVE_SETTING1              = 0x03,
    RDA_ADDR_LDO_ACTIVE_SETTING2              = 0x04,
    RDA_ADDR_LDO_ACTIVE_SETTING3              = 0x05,
    RDA_ADDR_LDO_ACTIVE_SETTING4              = 0x06,
    RDA_ADDR_LDO_ACTIVE_SETTING5              = 0x07,
    RDA_ADDR_LDO_LP_SETTING1                  = 0x08,
    RDA_ADDR_LDO_LP_SETTING2                  = 0x09,
    RDA_ADDR_LDO_LP_SETTING3                  = 0x0A,
    RDA_ADDR_LDO_LP_SETTING4                  = 0x0B,
    RDA_ADDR_LDO_LP_SETTING5                  = 0x0C,
    RDA_ADDR_LDO_POWER_ON_SETTING1            = 0x0D,
    RDA_ADDR_LDO_POWER_ON_SETTING2            = 0x0E,
    RDA_ADDR_LDO_POWER_ON_SETTING3            = 0x0F,
    RDA_ADDR_LDO_POWER_ON_SETTING4            = 0x10,
    RDA_ADDR_LDO_POWER_ON_SETTING5            = 0x11,
    RDA_ADDR_CHARGER_SETTING1                 = 0x12,
    RDA_ADDR_CHARGER_SETTING2                 = 0x13,
    RDA_ADDR_CHARGER_STATUS                   = 0x14,
    RDA_ADDR_CHARGER_CONTROL                  = 0x15,
    RDA_ADDR_CALIBRATION_SETTING1             = 0x16,
    RDA_ADDR_CALIBRATION_SETTING2             = 0x17,
    RDA_ADDR_MISC_CONTROL                     = 0x18,
    RDA_ADDR_LED_SETTING1                     = 0x19,
    RDA_ADDR_LED_SETTING2                     = 0x1A,
    RDA_ADDR_LED_SETTING3                     = 0x1B,
    RDA_ADDR_AUDIO_CONTROL                    = 0x1C,
    RDA_ADDR_TOUCH_SCREEN_CONTROL             = 0x1D,
    RDA_ADDR_TOUCH_SCREEN_RESULTS1            = 0x1E,
    RDA_ADDR_TOUCH_SCREEN_RESULTS2            = 0x1F,
    RDA_ADDR_TOUCH_SCREEN_RESULTS3            = 0x20,
    RDA_ADDR_EFUSE_OPT_SETTING1               = 0x21,
    RDA_ADDR_EFUSE_OPT_SETTING2               = 0x22,
    RDA_ADDR_EFUSE_OPT_SETTING3               = 0x23,
    RDA_ADDR_EFUSE_OPT_SETTING4               = 0x24,
    RDA_ADDR_EFUSE_OPT_SETTING5               = 0x25,
    RDA_ADDR_DEBUG1                           = 0x26,
    RDA_ADDR_SIM_INTERFACE                    = 0x27,
    RDA_ADDR_SIM_ACTIVE_SETTING               = 0x28,
    RDA_ADDR_SIM_LP_SETTING                   = 0x29,
    RDA_ADDR_SIM_POWER_ON_SETTING             = 0x2a,
    RDA_ADDR_SIM_POWER_OFF_SETTING            = 0x2b,
    RDA_ADDR_DEBUG2                           = 0x2c,
    RDA_ADDR_DCDC_BUCK1                       = 0x2d,
    RDA_ADDR_DCDC_BUCK2                       = 0x2e,
    RDA_ADDR_EFUSE_OUT                        = 0x2f,
    RDA_ADDR_PMU_RESET                        = 0x30,
    RDA_ADDR_LDO_POWER_OFF_SETTING1           = 0x31,
    RDA_ADDR_LDO_POWER_OFF_SETTING2           = 0x32,
    RDA_ADDR_LDO_POWER_OFF_SETTING3           = 0x33,
    RDA_ADDR_LDO_POWER_OFF_SETTING4           = 0x34,
    RDA_ADDR_LDO_POWER_OFF_SETTING5           = 0x35,
    RDA_ADDR_THERMAL_CALIBRATION              = 0x36,
    RDA_ADDR_DEBUG3                           = 0x37,
    RDA_ADDR_LED_SETTING4                     = 0x38,
    RDA_ADDR_DEBUG4                           = 0x39,
    RDA_ADDR_GPADC1                           = 0x3a,
    RDA_ADDR_GPADC2                           = 0x3b,
    RDA_ADDR_POWER_TIMING1                    = 0x3c,
    RDA_ADDR_POWER_TIMING2                    = 0x3d,
    RDA_ADDR_LED_SETTING5                     = 0x3e,
} RDA_REG_MAP_T;


// IRQ settings 0x1
#define RDA_PMU_PD_MODE_SEL             (1<<0)
//#define RDA_PMU_CHR_DISABLE             (1<<1)
#define RDA_PMU_INT_CHR_MASK            (1<<4)
#define RDA_PMU_INT_CHR_CLEAR           (1<<5)
#define RDA_PMU_INT_CHR_CAUSE           (1<<6)
#define RDA_PMU_INT_CHR_ON              (1<<7)
#define RDA_PMU_PENIRQ_MASK             (1<<8)
#define RDA_PMU_EOMIRQ_MASK             (1<<9)
#define RDA_PMU_PENIRQ_CLEAR            (1<<10)
#define RDA_PMU_EOMIRQ_CLEAR            (1<<11)
#define RDA_PMU_PENIRQ_CAUSE            (1<<12)
#define RDA_PMU_EOMIRQ_CAUSE            (1<<13)
#define RDA_PMU_PENIRQ                  (1<<14)
#define RDA_PMU_EOMIRQ                  (1<<15)
// end IRQ settings

//LDO settings 0x02
#define RDA_PMU_TSC_ENABLE                   (1<<1)
#define RDA_PMU_VLCD_ENABLE                  (1<<2)
#define RDA_PMU_VCAM_ENABLE                  (1<<3)
#define RDA_PMU_VMIC_ENABLE                  (1<<4)
#define RDA_PMU_VIBR_ENABLE                  (1<<5)
#define RDA_PMU_VRF_ENABLE                   (1<<9)
#define RDA_PMU_VABB_ENABLE                  (1<<10)
#define RDA_PMU_VMMC_ENABLE                  (1<<11)
#define RDA_PMU_VSIM4_ENABLE                 (1<<12)
#define RDA_PMU_VSIM3_ENABLE                 (1<<13)
#define RDA_PMU_VSIM2_ENABLE                 (1<<14)
#define RDA_PMU_VSIM1_ENABLE                 (1<<15)
#define RDA_PMU_VSIM_ENABLE_MASK              (0xf<<12)
// end LDO settings

//LDO active/lp/pon setting1 0x03/0x08/0x0d
#define RDA_PMU_NORMAL_MODE               (1<<0)
#define RDA_PMU_VSPIMEM_OFF               (1<<1)
#define RDA_PMU_VBLLED_OFF                (1<<2)
#define RDA_PMU_VMIC_OFF                  (1<<3)
#define RDA_PMU_VUSB_OFF                  (1<<4)
#define RDA_PMU_VIBR_OFF                  (1<<5)
#define RDA_PMU_VMMC_OFF                  (1<<6)
#define RDA_PMU_VLCD_OFF                  (1<<7)
#define RDA_PMU_VCAM_OFF                  (1<<8)
#define RDA_PMU_VRF_OFF                   (1<<9)
#define RDA_PMU_VABB_OFF                  (1<<10)
#define RDA_PMU_VPAD_OFF                  (1<<11)
#define RDA_PMU_VMEM_OFF                  (1<<12)
#define RDA_PMU_BUCK1_LDO_OFF             (1<<13)
#define RDA_PMU_BUCK1_PFM_ON              (1<<14)
#define RDA_PMU_BUCK1_ON                  (1<<15)

#define RDA_PMU_LDO_EN(n)              (((n)&0xFFFF)<<0)
#define RDA_PMU_LDO_EN_MASK            (0xFFFF<<0)
#define RDA_PMU_LDO_EN_SHIFT           (0)
// end LDO active/lp/pon setting1

// LDO active/lp/pon setting2  0x04/0x09/0x0e
#define RDA_PMU_VRTC_VBIT_SHIFT   (0)
#define RDA_PMU_VRTC_VBIT_MASK    (3<<0)
#define RDA_PMU_VRTC_VBIT(n)      (((n)&3)<<0)

#define RDA_PMU_BUCK2_LDO_OFF (1<<2)
#define RDA_PMU_BUCK2_PFM_ON (1<<3)
#define RDA_PMU_BUCK2_ON  (1<<4)
#define RDA_PMU_VBOOST_ON  (1<<5)
#define RDA_PMU_VINTRF_OFF (1<<6)

#define RDA_PMU_VIBR_SEL_MASK (1<<7)
#define RDA_PMU_VIBR_VSEL_1_8   (1<<7)
#define RDA_PMU_VIBR_VSEL_2_8   (0<<7)

#define RDA_PMU_VMMC_SEL_MASK   (1<<8)
#define RDA_PMU_VMMC_VSEL_1_8     (1<<8)
#define RDA_PMU_VMMC_VSEL_2_8     (0<<8)

#define RDA_PMU_VLCD_SEL_MASK   (1<<9)
#define RDA_PMU_VLCD_VSEL_1_8     (1<<9)
#define RDA_PMU_VLCD_VSEL_2_8     (0<<9)

#define RDA_PMU_VCAM_SEL_MASK   (1<<10)
#define RDA_PMU_VCAM_VSEL_1_8     (1<<10)
#define RDA_PMU_VCAM_VSEL_2_8     (0<<10)

#define RDA_PMU_VRF_SEL_MASK   (1<<11)
#define RDA_PMU_VRF_VSEL_1_8     (1<<11)
#define RDA_PMU_VRF_VSEL_2_8     (0<<11)

#define RDA_PMU_VPAD_SEL_MASK   (1<<12)
#define RDA_PMU_VPAD_VSEL_1_8     (1<<12)
#define RDA_PMU_VPAD_VSEL_2_8     (0<<12)

//#define RDA_PMU_VCORE_IBIT_SHIFT    (13)
//#define RDA_PMU_VCORE_IBIT_MASK     (7<<13)
//#define RDA_PMU_VCORE_IBIT(n)       (((n)&7)<<13)
//#define RDA_PMU_VCORE_SEL1V5   (1<<13)
//#define RDA_PMU_VCORE_SEL1V3   (2<<13)

// end LDO active/lp/pon setting2

// LDO ACT/LP/PON setting3 0x5/0xa/0xf
#define RDA_PMU_VABB_IBIT_SHIFT      (0)
#define RDA_PMU_VABB_IBIT_MASK       (7<<0)
#define RDA_PMU_VABB_IBIT(n)         (((n)&7)<<0)

#define RDA_PMU_VPAD_IBIT_SHIFT       (3)
#define RDA_PMU_VPAD_IBIT_MASK        (7<<3)
#define RDA_PMU_VPAD_IBIT(n)          (((n)&7)<<3)

#define RDA_PMU_VMEM_IBIT_SHIFT      (6)
#define RDA_PMU_VMEM_IBIT_MASK       (7<<6)
#define RDA_PMU_VMEM_IBIT(n)         (((n)&7)<<6)

#define RDA_PMU_VSPIMEM_IBIT_SHIFT   (9)
#define RDA_PMU_VSPIMEM_IBIT_MASK    (7<<9)
#define RDA_PMU_VSPIMEM_IBIT(n)      (((n)&7)<<9)

#define RDA_PMU_VBACKUP_VBIT_SHIFT    (12)
#define RDA_PMU_VBACKUP_VBIT_MASK     (7<<12)
#define RDA_PMU_VBACKUP_VBIT(n)       (((n)&7)<<12)
// end LDO ACT/LP/PON setting3

// LDO ACT setting4 0x6
// NOTE:
//   These settings are valid for all profiles
#define RDA_PMU_VIBR_IBIT_SHIFT      (0)
#define RDA_PMU_VIBR_IBIT_MASK       (7<<0)
#define RDA_PMU_VIBR_IBIT(n)         (((n)&7)<<0)

#define RDA_PMU_VMMC_IBIT_SHIFT       (3)
#define RDA_PMU_VMMC_IBIT_MASK        (7<<3)
#define RDA_PMU_VMMC_IBIT(n)          (((n)&7)<<3)

#define RDA_PMU_VLCD_IBIT_SHIFT      (6)
#define RDA_PMU_VLCD_IBIT_MASK       (7<<6)
#define RDA_PMU_VLCD_IBIT(n)         (((n)&7)<<6)

#define RDA_PMU_VCAM_IBIT_SHIFT   (9)
#define RDA_PMU_VCAM_IBIT_MASK    (7<<9)
#define RDA_PMU_VCAM_IBIT(n)      (((n)&7)<<9)

#define RDA_PMU_VRF_IBIT_SHIFT    (12)
#define RDA_PMU_VRF_IBIT_MASK     (7<<12)
#define RDA_PMU_VRF_IBIT(n)       (((n)&7)<<12)
// end LDO ACT setting4

// LDO ACT/LP/PON setting5 0x7/0xc/0x11
// NOTE:
//   VBLLED bits are located in ACT setting5 only
//   and are valid for all profiles
#define RDA_PMU_VBLLED_IBIT_SHIFT   (3)
#define RDA_PMU_VBLLED_IBIT_MASK    (7<<3)
#define RDA_PMU_VBLLED_IBIT(n)      (((n)&7)<<3)

#define RDA_PMU_VBLLED_VSEL_SHIFT   (6)
#define RDA_PMU_VBLLED_VSEL_MASK    (7<<6)
#define RDA_PMU_VBLLED_VSEL(n)      (((n)&7)<<6)
// End of NOTE

#define RDA_PMU_VUSB_IBIT_SHIFT   (9)
#define RDA_PMU_VUSB_IBIT_MASK    (7<<9)
#define RDA_PMU_VUSB_IBIT(n)      (((n)&7)<<9)

#define RDA_PMU_VSIM4_VSEL_1_8      (1<<12)
#define RDA_PMU_VSIM4_VSEL_2_8      (0<<12)
#define RDA_PMU_VSIM4_SEL_MASK    (1<<12)
#define RDA_PMU_VSIM3_VSEL_1_8      (1<<13)
#define RDA_PMU_VSIM3_VSEL_2_8      (0<<13)
#define RDA_PMU_VSIM3_SEL_MASK    (1<<13)
#define RDA_PMU_VSIM2_VSEL_1_8      (1<<14)
#define RDA_PMU_VSIM2_VSEL_2_8      (0<<14)
#define RDA_PMU_VSIM2_SEL_MASK    (1<<14)
#define RDA_PMU_VSIM1_VSEL_1_8      (1<<15)
#define RDA_PMU_VSIM1_VSEL_2_8      (0<<15)
#define RDA_PMU_VSIM1_SEL_MASK    (1<<15)
// end LDO ACT/LP/PON setting5

// LDO LP setting4 0xb
// NOTE:
//   These settings are valid for all profiles
#define RDA_PMU_VINTRF_IBIT_SHIFT    (0)
#define RDA_PMU_VINTRF_IBIT_MASK     (7<<0)
#define RDA_PMU_VINTRF_IBIT(n)       (((n)&7)<<0)

#define RDA_PMU_VINTRF_SEL_MASK      (1<<12)
#define RDA_PMU_VINTRF_VSEL_1_8      (1<<12)
#define RDA_PMU_VINTRF_VSEL_2_8      (0<<12)
// end LDO LP setting4

// Charger setting 1 / 0x12
#define RDA_PMU_BG_CAL_TC_BIT1(n)     (((n)&0x1f)<<0)
#define RDA_PMU_BG_CAL_TC_BIT1_MASK   (0x1f<<0)
#define RDA_PMU_BG_CAL_TC_BIT1_SHIFT  (0)

#define RDA_PMU_IREF_CTL_CHARGER(n)     (((n)&0x7)<<5)
#define RDA_PMU_IREF_CTL_CHARGER_MASK   (0x7<<5)
#define RDA_PMU_IREF_CTL_CHARGER_SHIFT  (5)

#define RDA_PMU_UV_SEL(n)     (((n)&0x3)<<8)
#define RDA_PMU_UV_SEL_MASK   (0x3<<8)
#define RDA_PMU_UV_SEL_SHIFT  (8)

#define RDA_PMU_CHR_AUXADC_SEL(n)     (((n)&0x7)<<11)
#define RDA_PMU_CHR_AUXADC_SEL_MASK   (0x7<<11)
#define RDA_PMU_CHR_AUXADC_SEL_SHIFT  (11)

#define RDA_PMU_CHR_CV_LOWGAIN  (1<<14)
#define RDA_PMU_CHR_ALWAYS_CC   (1<<15)
// End of charger setting 1

// Charger setting2 / 0x13
#define RDA_PMU_CHR_VFB_SEL_REG(n)     (((n)&0xf)<<0)
#define RDA_PMU_CHR_VFB_SEL_REG_MASK   (0xf<<0)
#define RDA_PMU_CHR_VFB_SEL_REG_SHIFT  (0)

#define RDA_PMU_CHR_PRE_IBIT_REG(n)    (((n)&0x7)<<4)
#define RDA_PMU_CHR_PRE_IBIT_REG_MASK  (0x7<<4)
#define RDA_PMU_CHR_PRE_IBIT_REG_SHIFT (4)

#define RDA_PMU_CHR_CC_IBIT_REG(n)     (((n)&0x7)<<7)
#define RDA_PMU_CHR_CC_IBIT_REG_MASK   (0x7<<7)
#define RDA_PMU_CHR_CC_IBIT_REG_SHIFT  (7)
#define RDA_PMU_CHR_CC_IBIT_30MA       RDA_PMU_CHR_CC_IBIT_REG(0)
#define RDA_PMU_CHR_CC_IBIT_50MA       RDA_PMU_CHR_CC_IBIT_REG(1)
#define RDA_PMU_CHR_CC_IBIT_100MA      RDA_PMU_CHR_CC_IBIT_REG(2)
#define RDA_PMU_CHR_CC_IBIT_150MA      RDA_PMU_CHR_CC_IBIT_REG(3)
#define RDA_PMU_CHR_CC_IBIT_250MA      RDA_PMU_CHR_CC_IBIT_REG(4)
#define RDA_PMU_CHR_CC_IBIT_350MA      RDA_PMU_CHR_CC_IBIT_REG(5)
#define RDA_PMU_CHR_CC_IBIT_500MA      RDA_PMU_CHR_CC_IBIT_REG(6)
#define RDA_PMU_CHR_CC_IBIT_650MA      RDA_PMU_CHR_CC_IBIT_REG(7)

#define RDA_PMU_CHR_VFB_SEL_DR         (1<<10)
#define RDA_PMU_CHR_PRE_IBIT_DR        (1<<11)
#define RDA_PMU_CHR_CC_IBIT_DR         (1<<12)
// End of charger setting 2

// Charger status 0x14
#define RDA_PMU_CHR_VREG   (1<<2)
#define RDA_PMU_CHR_AC_ON   (1<<7)
#define RDA_PMU_PENIRQ_B   (1<<11)
 // end charger status

// Charger control 0x15
#define RDA_PMU_BAT_MON_TIME_DELAY(n)    (((n)&0x3)<<0)
#define RDA_PMU_BAT_MON_TIME_DELAY_MASK  (0x3<<0)
#define RDA_PMU_BAT_MON_TIME_DELAY_SHIFT (0)

#define RDA_PMU_BAT_MON_ENABLE           (1<<2)

#define RDA_PMU_CHR_EN_DUTY_CYCLE(n)     (((n)&0x3)<<3)
#define RDA_PMU_CHR_EN_DUTY_CYCLE_MASK   (0x3<<3)
#define RDA_PMU_CHR_EN_DUTY_CYCLE_SHIFT  (3)

#define RDA_PMU_CHR_EN_FREQ(n)           (((n)&0x3)<<5)
#define RDA_PMU_CHR_EN_FREQ_MASK         (0x3<<5)
#define RDA_PMU_CHR_EN_FREQ_SHIFT        (5)

#define RDA_PMU_CHR_CC_MODE_REG          (1<<12)
#define RDA_PMU_CHR_CC_MODE_DR           (1<<13)
#define RDA_PMU_CHR_ENABLE_REG           (1<<14)
#define RDA_PMU_CHR_ENABLE_DR            (1<<15)
// end charger control

// Charger calib setting1 0x16
#define RDA_PMU_BGAP_CAL_BIT_REG(n)      (((n)&0x3f)<<0)
#define RDA_PMU_BGAP_CAL_BIT_REG_MASK    (0x3f<<0)
#define RDA_PMU_BGAP_CAL_BIT_REG_SHIFT   (0)

#define RDA_PMU_BGAP_CAL_BIT_DR          (1<<6)
#define RDA_PMU_RESETN_BGAP_CAL_REG      (1<<7)
#define RDA_PMU_RESETN_BGAP_CAL_DR       (1<<8)
#define RDA_PMU_BGAP_CAL_POLARITY        (1<<12)
#define RDA_PMU_BGAP_CAL_CLOCK_INV       (1<<13)
#define RDA_PMU_VBAT_OVER_3P2_BYPASS     (1<<14)
#define RDA_PMU_BGAP_CAL_BYPASS          (1<<15)
// End of charget calib setting1

// Charger calib setting2 0x17
#define RDA_PMU_PD_CHARGE_LDO            (1<<0)

#define RDA_PMU_TS_I_CTRL_BATTERY(n)     (((n)&0xf)<<11)
#define RDA_PMU_TS_I_CTRL_BATTERY_MASK   (0xf<<11)
#define RDA_PMU_TS_I_CTRL_BATTERY_SHIFT  (11)

#define RDA_PMU_PU_TS_BATTERY            (1<<15)
// End of charget calib setting2

// MISC Control 0x18
#define RDA_PMU_LDO_AVDD3_BIT(n)         (((n)&7)<<0)
#define RDA_PMU_LDO_AVDD3_BIT_MASK       (7<<0)
#define RDA_PMU_LDO_AVDD3_BIT_SHIFT      (0)
#define RDA_PMU_CLK2M_FTUN_BIT(n)        (((n)&7)<<3)
#define RDA_PMU_CLK2M_FTUN_BIT_MASK      (7<<3)
#define RDA_PMU_CLK2M_FTUN_BIT_SHIFT     (3)
#define RDA_PMU_PD_LDO_AVDD3_REG         (1<<6)
#define RDA_PMU_PD_LDO_AVDD3_DR          (1<<7)
#define RDA_PMU_PU_CLK_4M_REG            (1<<8)
#define RDA_PMU_PU_CLK_4M_DR             (1<<9)
#define RDA_PMU_PU_CLK_32K_REG           (1<<10)
#define RDA_PMU_PU_CLK_32K_DR            (1<<11)
// End of MISC Control

// LED setting1 0x19
#define RDA_PMU_DIM_BL_REG               (1<<0)
#define RDA_PMU_DIM_BL_DR                (1<<1)
#define RDA_PMU_DIM_LED_B_REG            (1<<2)
#define RDA_PMU_DIM_LED_B_DR             (1<<3)
#define RDA_PMU_DIM_LED_G_REG            (1<<4)
#define RDA_PMU_DIM_LED_G_DR             (1<<5)
#define RDA_PMU_DIM_LED_R_REG            (1<<6)
#define RDA_PMU_DIM_LED_R_DR             (1<<7)
#define RDA_PMU_PWM_RGB_PMU_MODE         (1<<8)
#define RDA_PMU_PWM_BL_ENABLE            (1<<9)

#define RDA_PMU_PWM_RGB_FREQ(n)      (((n)&0xf)<<12)
#define RDA_PMU_PWM_RGB_FREQ_MASK    (0xf<<12)
#define RDA_PMU_PWM_RGB_FREQ_SHIFT   (12)
// end LED setting1 0x19

// LED setting2 0x1a
#define RDA_PMU_BL_OFF_LP                  (1<<1)
#define RDA_PMU_BL_OFF_ACT                 (1<<2)
#define RDA_PMU_BL_OFF_PON                 (1<<3)
#define RDA_PMU_BL_IBIT_LP(n)              (((n)&0xf)<<4)
#define RDA_PMU_BL_IBIT_LP_MASK            (0xf<<4)
#define RDA_PMU_BL_IBIT_LP_SHIFT           (4)
#define RDA_PMU_BL_IBIT_ACT(n)             (((n)&0xf)<<8)
#define RDA_PMU_BL_IBIT_ACT_MASK           (0xf<<8)
#define RDA_PMU_BL_IBIT_ACT_SHIFT          (8)
#define RDA_PMU_BL_IBIT_PON(n)             (((n)&0xf)<<12)
#define RDA_PMU_BL_IBIT_PON_MASK           (0xf<<12)
#define RDA_PMU_BL_IBIT_PON_SHIFT          (12)
// end LED setting2 0x1a

// LED setting3 0x1b
#define RDA_PMU_PWM_BL_FREQ_SEL          (1<<5)

#define RDA_PMU_PWM_BL_FREQ(n)           (((n)&0xf)<<6)
#define RDA_PMU_PWM_BL_FREQ_MASK         (0xf<<6)
#define RDA_PMU_PWM_BL_FREQ_SHIFT        (6)

#define RDA_PMU_PWM_BL_DUTY_CYCLE(n)     (((n)&0x1f)<<10)
#define RDA_PMU_PWM_BL_DUTY_CYCLE_MASK   (0x1f<<10)
#define RDA_PMU_PWM_BL_DUTY_CYCLE_SHIFT  (10)
// end LED setting3 0x1b

// audio control 0x1c
#define RDA_PMU_TIMER_SAMP_NEG_TSC      (1<<3)
#define RDA_PMU_TIMER_SAMP_POS_TSC      (1<<4)

#define RDA_PMU_TIMER_X_TO_Y_SEL(n)      (((n)&3)<<5)
#define RDA_PMU_TIMER_X_TO_Y_SEL_MASK    (3<<5)
#define RDA_PMU_TIMER_X_TO_Y_SEL_SHIFT   (5)
#define RDA_PMU_TIMER_X_TO_Y_SEL_1   (0<<5)
#define RDA_PMU_TIMER_X_TO_Y_SEL_2   (1<<5)
#define RDA_PMU_TIMER_X_TO_Y_SEL_3   (2<<5)
#define RDA_PMU_TIMER_X_TO_Y_SEL_4   (3<<5)

#define RDA_PMU_TIMER_INT_TO_X_SEL(n)      (((n)&3)<<7)
#define RDA_PMU_TIMER_INT_TO_X_SEL_MASK    (3<<7)
#define RDA_PMU_TIMER_INT_TO_X_SEL_SHIFT   (7)
#define RDA_PMU_TIMER_INT_TO_X_SEL_1   (0<<7)
#define RDA_PMU_TIMER_INT_TO_X_SEL_2   (1<<7)
#define RDA_PMU_TIMER_INT_TO_X_SEL_3   (2<<7)
#define RDA_PMU_TIMER_INT_TO_X_SEL_4   (3<<7)

#define RDA_PMU_TIMER_PEN_IRQ_STABLE(n)      (((n)&3)<<9)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_MASK    (3<<9)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_SHIFT   (9)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_2US   (0<<9)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_4US   (1<<9)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_8US   (2<<9)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_16US   (3<<9)

#define RDA_PMU_PWM_CLK_DIV2_ENABLE          (1<<11)

#define RDA_PMU_DELAY_BEFORE_SAMP(n)      (((n)&3)<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_MASK    (3<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_SHIFT   (12)
#define RDA_PMU_DELAY_BEFORE_SAMP_2US   (0<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_4US   (1<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_8US   (2<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_16US   (3<<12)

#define RDA_PMU_BYPASS_BB_READ               (1<<14)
#define RDA_PMU_MEAS_WAIT_CLR_PENIRQ               (1<<15)
// end audio control 0x1c

// touch screen control  0x1d
#define RDA_PMU_PD_SAR_REG                 (1<<0)
#define RDA_PMU_PD_SAR_DR                   (1<<1)
#define RDA_PMU_CONV_CLK_INV                (1<<2)
#define RDA_PMU_SAMP_CLK_INV                 (1<<3)
#define RDA_PMU_SAR_ADC_MODE                  (1<<4)
#define RDA_PMU_SAR_OUT_POLARITY               (1<<5)
#define RDA_PMU_WAIT_BB_READ_TIMESL(n)        (((n)&3)<<6)
#define RDA_PMU_WAIT_BB_READ_TIMESL_MASK       (3<<6)
#define RDA_PMU_WAIT_BB_READ_TIMESL_SHIFT      (6)
#define RDA_PMU_SAR_VERF_BIT(n)        (((n)&3)<<8)
#define RDA_PMU_SAR_VERF_BIT_MASK       (3<<8)
#define RDA_PMU_SAR_VERF_BIT_SHIFT      (8)
#define RDA_PMU_TSC_SAR_SEL(n)        (((n)&3)<<10)
#define RDA_PMU_TSC_SAR_SEL_MASK       (3<<10)
#define RDA_PMU_TSC_SAR_SEL_SHIFT      (10)
#define RDA_PMU_TSC_CLK_DIV_EN               (1<<13)
#define RDA_PMU_TSC_XP_XN_INV               (1<<14)
#define RDA_PMU_TSC_YP_YN_INV                (1<<15)
// end touch screen control 

// touch screen results1 0x1e
#define RDA_PMU_TSC_X_VALUE_BIT_MASK       (0x3ff<<0)
#define RDA_PMU_TSC_X_VALUE_BIT_SHIFT      (0)
#define RDA_PMU_TSC_X_VALUE_VALID          (1<<10)
// end touch screen results1

// touch screen results2 0x1f
#define RDA_PMU_TSC_Y_VALUE_BIT_MASK       (0x3ff<<0)
#define RDA_PMU_TSC_Y_VALUE_BIT_SHIFT      (0)
#define RDA_PMU_TSC_Y_VALUE_VALID          (1<<10)
// end touch screen results2

// touch screen results3 0x20
#define RDA_PMU_GPADC_VALUE_BIT_MASK       (0x3ff<<0)
#define RDA_PMU_GPADC_VALUE_BIT_SHIFT      (0)
#define RDA_PMU_GPADC_VALUE_VALID          (1<<10)
// end touch screen results3

// Effuse and OTP setting1  0x21
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_2(n)     (((n)&0x3)<<8)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_2_MASK   (0x3<<8)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_2_SHIFT  (8)

#define RDA_PMU_EFUSE_SEL_WORD_AUTO_1(n)     (((n)&0x3)<<10)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_1_MASK   (0x3<<10)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_1_SHIFT  (10)

#define RDA_PMU_EFUSE_SEL_WORD_REG(n)        (((n)&0x3)<<12)
#define RDA_PMU_EFUSE_SEL_WORD_REG_MASK      (0x3<<12)
#define RDA_PMU_EFUSE_SEL_WORD_REG_SHIFT     (12)

#define RDA_PMU_EFUSE_SENSE_REG              (1<<14)
#define RDA_PMU_EFUSE_SENSE_DR               (1<<15)
// end of effuse and OTP setting1

// Effuse and OTP setting2  0x22
#define RDA_PMU_PD_OTP_REG              (1<<0)
#define RDA_PMU_LP_MODE_REG             (1<<1)
#define RDA_PMU_LP_MODE_DR              (1<<2)

#define RDA_PMU_OSC_FREQ_BOOST_SHIFT    (9)
#define RDA_PMU_OSC_FREQ_BOOST_MASK     (0x3<<9)
#define RDA_PMU_OSC_FREQ_BOOST(n)       (((n)&0x3)<<9)

#define RDA_PMU_BYPASS_BOOST            (1<<12)
#define RDA_PMU_SLOPE_EN_DCDC           (1<<12)

#define RDA_PMU_VBOOST_BIT_SHIFT        (13)
#define RDA_PMU_VBOOST_BIT_MASK         (0x7<<13)
#define RDA_PMU_VBOOST_BIT(n)           (((n)&0x7)<<13)
// end of effuse and OTP setting2

// Effuse and OTP setting4  0x24
#define RDA_PMU_EFFUSE_OUT_READ_1_MASK    (0xffff<<0)
#define RDA_PMU_EFFUSE_OUT_READ_1_SHIFT   (0)
// end of effuse and OTP setting4

// Effuse and OTP setting4  0x25
#define RDA_PMU_EFFUSE_OUT_READ_2_MASK    (0xffff<<0)
#define RDA_PMU_EFFUSE_OUT_READ_2_SHIFT   (0)
// end of effuse and OTP setting4

// sim interface 0x27
#define RDA_PMU_SIM_SELECT(n)              (((n)&0x3)<<14)
#define RDA_PMU_SIM_SELECT_MASK            (3<<14)
#define RDA_PMU_SIM_SELECT_SHIFT           (14)
#define RDA_PMU_SIM_SELECT_SIM1            (0<<14)
#define RDA_PMU_SIM_SELECT_SIM2            (1<<14)
#define RDA_PMU_SIM_SELECT_SIM3            (2<<14)
#define RDA_PMU_SIM_SELECT_SIM4            (3<<14)
#define RDA_PMU_PULLUP_ENABLE_1            (1<<11)
#define RDA_PMU_UNSEL_CLK_VAL_1            (1<<10)
#define RDA_PMU_UNSEL_RST_VAL_1            (1<<9)
#define RDA_PMU_PULLUP_ENABLE_2            (1<<8)
#define RDA_PMU_UNSEL_CLK_VAL_2            (1<<7)
#define RDA_PMU_UNSEL_RST_VAL_2            (1<<6)
#define RDA_PMU_PULLUP_ENABLE_3            (1<<5)
#define RDA_PMU_UNSEL_CLK_VAL_3            (1<<4)
#define RDA_PMU_UNSEL_RST_VAL_3            (1<<3)
#define RDA_PMU_PULLUP_ENABLE_4            (1<<2)
#define RDA_PMU_UNSEL_CLK_VAL_4            (1<<1)
#define RDA_PMU_UNSEL_RST_VAL_4            (1<<0)
// end sim interface 

// sim ACT/LP/PON/POFF setting 0x28/0x29/0x2a/0x2b
#define RDA_PMU_VSIM4_IBIT_SHIFT      (0)
#define RDA_PMU_VSIM4_IBIT_MASK       (7<<0)
#define RDA_PMU_VSIM4_IBIT(n)         (((n)&7)<<0)

#define RDA_PMU_VSIM3_IBIT_SHIFT      (3)
#define RDA_PMU_VSIM3_IBIT_MASK       (7<<3)
#define RDA_PMU_VSIM3_IBIT(n)         (((n)&7)<<3)

#define RDA_PMU_VSIM2_IBIT_SHIFT      (6)
#define RDA_PMU_VSIM2_IBIT_MASK       (7<<6)
#define RDA_PMU_VSIM2_IBIT(n)         (((n)&7)<<6)

#define RDA_PMU_VSIM1_IBIT_SHIFT      (9)
#define RDA_PMU_VSIM1_IBIT_MASK       (7<<9)
#define RDA_PMU_VSIM1_IBIT(n)         (((n)&7)<<9)

#define RDA_PMU_VSIM4_OFF                (1<<12)
#define RDA_PMU_VSIM3_OFF                (1<<13)
#define RDA_PMU_VSIM2_OFF                (1<<14)
#define RDA_PMU_VSIM1_OFF                (1<<15)
// end sim interface2

// DCDC buck1/buck2 0x2d/0x2e
#define RDA_PMU_IR_DISABLE_EN            (1<<0)
#define RDA_PMU_HEAVY_LOAD               (1<<1)
#define RDA_PMU_DISCHARGE_EN             (1<<2)
#define RDA_PMU_LOW_SENSE                (1<<3)

#define RDA_PMU_OSC_FREQ(n)              (((n)&3)<<4)
#define RDA_PMU_OSC_FREQ_MASK            (3<<4)
#define RDA_PMU_OSC_FREQ_SHIFT           (4)

#define RDA_PMU_PFM_THRESHOLD(n)         (((n)&3)<<6)
#define RDA_PMU_PFM_THRESHOLD_MASK       (3<<6)
#define RDA_PMU_PFM_THRESHOLD_SHIFT      (6)

#define RDA_PMU_COUNTER_DISABLE          (1<<8)

#define RDA_PMU_VBUCK_BIT(n)             (((n)&0xf)<<9)
#define RDA_PMU_VBUCK_BIT_MASK           (0xf<<9)
#define RDA_PMU_VBUCK_BIT_SHIFT          (9)

#define RDA_PMU_VBUCK_LDO_IBIT(n)        (((n)&7)<<13)
#define RDA_PMU_VBUCK_LDO_IBIT_MASK      (7<<13)
#define RDA_PMU_VBUCK_LDO_IBIT_SHIFT     (13)
// end DCDC buck1/buck2

// PMU reset 0x30
#define RDA_PMU_SOFT_RESETN              (1<<0)
#define RDA_PMU_REGISTER_RESETN          (1<<15)
// end of PMU reset

//DCDC  LP mode 0x35
#define RDA_PMU_VBUCK_LP_BIT(n)          (((n)&0xf)<< 4)
#define RDA_PMU_VBUCK_LP_BIT_MASK        (0xf<<4)
#define RDA_PMU_VBUCK_LP_BIT_SHIFT       (4)

// Thermal Calibration 0x36
#define RDA_PMU_LP_MODE_BG_ACT           (1<<6)
#define RDA_PMU_LP_MODE_BG_LP            (1<<7)
#define RDA_PMU_BUCK_HEAVY_LOAD          (1<<8)
#define RDA_PMU_CHR_CALMODE_EN_BG_REG    (1<<9)
#define RDA_PMU_CHR_CALMODE_EN_NONCAL    (1<<10)
#define RDA_PMU_CALMODE_EN_BG_CAL        (1<<11)

#define RDA_PMU_THERMO_SEL(n)            (((n)&0x3)<<12)
#define RDA_PMU_THERMO_SEL_MASK          (0x3<<12)
#define RDA_PMU_THERMO_SEL_SHIFT         (12)

#define RDA_PMU_PU_THERMO_PROTECT        (1<<14)
#define RDA_PMU_PD_VD_LDO                (1<<15)
// end Thermal Calibration

// Debug3 0x37
#define RDA_PMU_OVER_TEMP_BYPASS         (1<<9)
#define RDA_PMU_PU_BGAP_REG              (1<<10)
#define RDA_PMU_PU_BGAP_DR               (1<<11)
#define RDA_PMU_RESETN_BB_REG            (1<<12)
#define RDA_PMU_RESETN_BB_DR             (1<<13)
#define RDA_PMU_RESETN_TSC_REG           (1<<14)
#define RDA_PMU_RESETN_TSC_DR            (1<<15)
// end Debug3

// LED Setting4 0x38
#define RDA_PMU_PWM_B_DUTY_CYCLE(n)         (((n)&0x1f)<<1)
#define RDA_PMU_PWM_B_DUTY_CYCLE_MASK       (0x1f<<1)
#define RDA_PMU_PWM_B_DUTY_CYCLE_SHIFT      (1)

#define RDA_PMU_PWM_G_DUTY_CYCLE(n)         (((n)&0x1f)<<6)
#define RDA_PMU_PWM_G_DUTY_CYCLE_MASK       (0x1f<<6)
#define RDA_PMU_PWM_G_DUTY_CYCLE_SHIFT      (6)

#define RDA_PMU_PWM_R_DUTY_CYCLE(n)         (((n)&0x1f)<<11)
#define RDA_PMU_PWM_R_DUTY_CYCLE_MASK       (0x1f<<11)
#define RDA_PMU_PWM_R_DUTY_CYCLE_SHIFT      (11)

// GPADC Setting2 0x3b
#define RDA_PMU_GPADC_ENABLE                (1<<0)
#define RDA_PMU_GPADC_ENABLE_CHANNEL(n)     ((1<<((n)&0x7))<<1)
#define RDA_PMU_GPADC_ENABLE_CHANNEL_MASK   (0xff<<1)
#define RDA_PMU_GPADC_ENABLE_CHANNEL_SHIFT  (1)

#define RDA_PMU_GPADC_VREF_BIT(n)           (((n)&0x3)<<9)
#define RDA_PMU_GPADC_VREF_BIT_MASK         (0x3<<9)
#define RDA_PMU_GPADC_VREF_BIT_SHIFT        (9)

#define RDA_PMU_GPADC_START_TIME(n)         (((n)&0x7)<<13)
#define RDA_PMU_GPADC_START_TIME_MASK       (0x7<<13)
#define RDA_PMU_GPADC_START_TIME_SHIFT      (13)

// LED Setting5 0x3e
#define RDA_PMU_LED_B_IBIT(n)         (((n)&0x7)<<0)
#define RDA_PMU_LED_B_IBIT_MASK       (0x7<<0)
#define RDA_PMU_LED_B_IBIT_SHIFT      (0)

#define RDA_PMU_LED_G_IBIT(n)         (((n)&0x7)<<3)
#define RDA_PMU_LED_G_IBIT_MASK       (0x7<<3)
#define RDA_PMU_LED_G_IBIT_SHIFT      (3)

#define RDA_PMU_LED_R_IBIT(n)         (((n)&0x7)<<6)
#define RDA_PMU_LED_R_IBIT_MASK       (0x7<<6)
#define RDA_PMU_LED_R_IBIT_SHIFT      (6)

#define RDA_PMU_LED_B_OFF_LP          (1<<9)
#define RDA_PMU_LED_G_OFF_LP          (1<<10)
#define RDA_PMU_LED_R_OFF_LP          (1<<11)
#define RDA_PMU_LED_B_OFF_ACT         (1<<12)
#define RDA_PMU_LED_G_OFF_ACT         (1<<13)
#define RDA_PMU_LED_R_OFF_ACT         (1<<14)
// end LED Setting5

// =============================================================================
//  TYPES
// =============================================================================
// ============================================================================
// RDA_PMU_CHR_STATUS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================

typedef enum {
    CHR_STATUS_UNKNOWN,
    CHR_STATUS_AC_OFF,
    CHR_STATUS_AC_ON,
} RDA_PMU_CHR_STATUS_T;


// =============================================================================
//  MACROS
// =============================================================================

/// to extract bitfield from register value
#define PMU_GET_BITFIELD(dword, bitfield) (((dword) & (bitfield ## _MASK)) >> (bitfield ## _SHIFT))
#define PMU_SET_BITFIELD(dword, bitfield, value) (((dword) & ~(bitfield ## _MASK)) | (bitfield(value)))

/// to build commands for pmd_OpalSpiSendCommand
#define PMU_SPI_BUILD_WRITE_COMMAND(cmd,regIdx,val) \
{ (*cmd) = (0<<25) | ((regIdx & 0x1ff) << 16) | (val & 0xffff); }

#define PMU_SPI_BUILD_READ_COMMAND(cmd,regIdx) \
{ (*cmd) = (1<<25) | ((regIdx & 0x1ff) << 16) | (val & 0xffff); }

#define PMU_SPI_EXTRACT_DATA(data) ((UINT16) (((data)[1] << 8) | (data)[2]))

// =============================================================================
//  FUNCTIONS
// =============================================================================

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


VOID pmd_InitRdaPmu(VOID);

BOOL pmd_SelectSimCard(UINT8 sim_card);

BOOL pmd_TSDGetSPI(VOID);
BOOL pmd_TSDReleaseSPI(VOID);
BOOL pmd_TsdReadCoordinatesInternal(UINT16* gpadc_x, UINT16* gpadc_y);
VOID pmd_TsdClearTouchIrq(VOID);
BOOL pmd_ResetTouch(VOID);
VOID pmd_TsdClearEomIrqInternal(VOID);
BOOL pmd_TsdEnableIrq(VOID);
BOOL pmd_TsdDisableIrq(VOID);

PROTECTED RDA_PMU_CHR_STATUS_T pmd_GetChargerHwStatus(VOID);

#endif
