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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/gpadc.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _GPADC_H_
#define _GPADC_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'gpadc'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// GPADC_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_GPADC_BASE              0x01A20000

typedef volatile struct
{
    REG32                          ctrl;                         //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          data;                         //0x00000008
    REG32                          data_ch0;                     //0x0000000C
    REG32                          data_ch1;                     //0x00000010
    REG32                          data_ch2;                     //0x00000014
    REG32                          data_ch3;                     //0x00000018
    REG32                          irq_mask;                     //0x0000001C
    REG32                          irq_cause;                    //0x00000020
    REG32                          irq_clr;                      //0x00000024
    REG32                          thrshd;                       //0x00000028
    REG32                          cmd_set;                      //0x0000002C
    REG32                          cmd_clr;                      //0x00000030
} HWP_GPADC_T;

#define hwp_gpadc                   ((HWP_GPADC_T*) KSEG1(REG_GPADC_BASE))


//ctrl
#define GPADC_CH0_EN                (1<<0)
#define GPADC_CH1_EN                (1<<1)
#define GPADC_CH2_EN                (1<<2)
#define GPADC_CH3_EN                (1<<3)
#define GPADC_ATP(n)                (((n)&7)<<8)
#define GPADC_ATP_122US             (0<<8)
#define GPADC_ATP_100MS             (1<<8)
#define GPADC_ATP_10MS              (2<<8)
#define GPADC_ATP_1MS               (3<<8)
#define GPADC_ATP_250MS             (4<<8)
#define GPADC_ATP_500MS             (5<<8)
#define GPADC_ATP_1S                (6<<8)
#define GPADC_ATP_2S                (7<<8)
#define GPADC_PD_MODE               (1<<16)
#define GPADC_PD_MODE_SW            (0<<16)
#define GPADC_PD_MODE_HW            (1<<16)
#define GPADC_MANUAL_MODE           (1<<20)
#define GPADC_CH_EN(n)              (((n)&15)<<0)
#define GPADC_CH_EN_MASK            (15<<0)
#define GPADC_CH_EN_SHIFT           (0)

//status
#define GPADC_THRSHD0_IRQ           (1<<0)
#define GPADC_THRSHD1_IRQ           (1<<4)
#define GPADC_EOC                   (1<<8)
#define GPADC_BUSY                  (1<<12)

//data
#define GPADC_DATA(n)               (((n)&0x3FF)<<0)

//data_ch0
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//data_ch1
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//data_ch2
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//data_ch3
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//irq_mask
//#define GPADC_THRSHD0_IRQ         (1<<0)
//#define GPADC_THRSHD1_IRQ         (1<<4)
//#define GPADC_EOC                 (1<<8)

//irq_cause
//#define GPADC_THRSHD0_IRQ         (1<<0)
//#define GPADC_THRSHD1_IRQ         (1<<4)
//#define GPADC_EOC                 (1<<8)

//irq_clr
//#define GPADC_THRSHD0_IRQ         (1<<0)
//#define GPADC_THRSHD1_IRQ         (1<<4)
//#define GPADC_EOC                 (1<<8)

//thrshd
#define GPADC_THRSHD0(n)            (((n)&0x3FF)<<0)
#define GPADC_THRSHD1(n)            (((n)&0x3FF)<<16)

//cmd_set
#define GPADC_GPADC_PD              (1<<0)
#define GPADC_GPADC_RESET           (1<<4)
#define GPADC_START_MEASURE         (1<<8)

//cmd_clr
//#define GPADC_GPADC_PD            (1<<0)
//#define GPADC_GPADC_RESET         (1<<4)





#endif

