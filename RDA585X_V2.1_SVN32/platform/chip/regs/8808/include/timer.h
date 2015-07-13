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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/regs/8808/include/timer.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2012-09-18 19:24:03 +0800 (周二, 18 九月 2012) $                                                                      
//  $Revision: 17062 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'timer'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define NB_INTERVAL                              (1)
#define INT_TIMER_NB_BITS                        (24)
#define WD_TIMER_NB_BITS                         (24)
#define HW_TIMER_NB_BITS                         (32)
#define TIM_MAXVAL                               (0XFFFFFF)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// TIMER_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_TIMER_BASE              0x01A02000

typedef volatile struct
{
    REG32                          OSTimer_Ctrl;                 //0x00000000
    REG32                          OSTimer_CurVal;               //0x00000004
    REG32                          WDTimer_Ctrl;                 //0x00000008
    REG32                          WDTimer_LoadVal;              //0x0000000C
    REG32                          HWTimer_Ctrl;                 //0x00000010
    REG32                          HWTimer_CurVal;               //0x00000014
    REG32                          Timer_Irq_Mask_Set;           //0x00000018
    REG32                          Timer_Irq_Mask_Clr;           //0x0000001C
    REG32                          Timer_Irq_Clr;                //0x00000020
    REG32                          Timer_Irq_Cause;              //0x00000024
} HWP_TIMER_T;

#define hwp_timer                   ((HWP_TIMER_T*) KSEG1(REG_TIMER_BASE))


//OSTimer_Ctrl
#define TIMER_LOADVAL(n)            (((n)&0xFFFFFF)<<0)
#define TIMER_LOADVAL_MASK          (0xFFFFFF<<0)
#define TIMER_LOADVAL_SHIFT         (0)
#define TIMER_ENABLE                (1<<24)
#define TIMER_ENABLED               (1<<25)
#define TIMER_CLEARED               (1<<26)
#define TIMER_REPEAT                (1<<28)
#define TIMER_WRAP                  (1<<29)
#define TIMER_LOAD                  (1<<30)

//OSTimer_CurVal
#define TIMER_CURVAL(n)             (((n)&0xFFFFFFFF)<<0)

//WDTimer_Ctrl
#define TIMER_START                 (1<<0)
#define TIMER_WDENABLED             (1<<3)
#define TIMER_STOP                  (1<<4)
#define TIMER_RELOAD                (1<<16)

//WDTimer_LoadVal
//#define TIMER_LOADVAL(n)          (((n)&0xFFFFFF)<<0)
//#define TIMER_LOADVAL_MASK        (0xFFFFFF<<0)
//#define TIMER_LOADVAL_SHIFT       (0)

//HWTimer_Ctrl
#define TIMER_INTERVAL_EN           (1<<8)
#define TIMER_INTERVAL(n)           (((n)&3)<<0)

//HWTimer_CurVal
//#define TIMER_CURVAL(n)           (((n)&0xFFFFFFFF)<<0)

//Timer_Irq_Mask_Set
#define TIMER_OSTIMER_MASK          (1<<0)
#define TIMER_HWTIMER_WRAP_MASK     (1<<1)
#define TIMER_HWTIMER_ITV_MASK      (1<<2)

//Timer_Irq_Mask_Clr
//#define TIMER_OSTIMER_MASK        (1<<0)
//#define TIMER_HWTIMER_WRAP_MASK   (1<<1)
//#define TIMER_HWTIMER_ITV_MASK    (1<<2)

//Timer_Irq_Clr
#define TIMER_OSTIMER_CLR           (1<<0)
#define TIMER_HWTIMER_WRAP_CLR      (1<<1)
#define TIMER_HWTIMER_ITV_CLR       (1<<2)

//Timer_Irq_Cause
#define TIMER_OSTIMER_CAUSE         (1<<0)
#define TIMER_HWTIMER_WRAP_CAUSE    (1<<1)
#define TIMER_HWTIMER_ITV_CAUSE     (1<<2)
#define TIMER_OSTIMER_STATUS        (1<<16)
#define TIMER_HWTIMER_WRAP_STATUS   (1<<17)
#define TIMER_HWTIMER_ITV_STATUS    (1<<18)
#define TIMER_OTHER_TIMS_IRQ(n)     (((n)&3)<<1)
#define TIMER_OTHER_TIMS_IRQ_MASK   (3<<1)
#define TIMER_OTHER_TIMS_IRQ_SHIFT  (1)





#endif

