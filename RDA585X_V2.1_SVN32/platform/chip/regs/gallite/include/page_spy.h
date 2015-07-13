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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/page_spy.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _PAGE_SPY_H_
#define _PAGE_SPY_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'page_spy'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================
#define PAGE_SPY_NB_PAGE                         (6)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PAGE_SPY_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_PAGE_SPY_BASE           0x01A0C000

typedef volatile struct
{
    REG32                          enable;                       //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          disable;                      //0x00000008
    REG32 Reserved_0000000C;                    //0x0000000C
    struct
    {
        REG32                      start;                        //0x00000010
        REG32                      end;                          //0x00000014
        REG32                      master;                       //0x00000018
        REG32                      addr;                         //0x0000001C
    } page[PAGE_SPY_NB_PAGE];
} HWP_PAGE_SPY_T;

#define hwp_pageSpy                 ((HWP_PAGE_SPY_T*) KSEG1(REG_PAGE_SPY_BASE))


//enable
#define PAGE_SPY_ENABLE_PAGE_0      (1<<0)
#define PAGE_SPY_ENABLE_PAGE_1      (1<<1)
#define PAGE_SPY_ENABLE_PAGE_2      (1<<2)
#define PAGE_SPY_ENABLE_PAGE_3      (1<<3)
#define PAGE_SPY_ENABLE_PAGE_4      (1<<4)
#define PAGE_SPY_ENABLE_PAGE_5      (1<<5)
#define PAGE_SPY_ENABLE_PAGE(n)     (((n)&0x3F)<<0)
#define PAGE_SPY_ENABLE_PAGE_MASK   (0x3F<<0)
#define PAGE_SPY_ENABLE_PAGE_SHIFT  (0)

//status
#define PAGE_SPY_STATUS_PAGE_0      (1<<0)
#define PAGE_SPY_STATUS_PAGE_0_NOT_HIT (0<<0)
#define PAGE_SPY_STATUS_PAGE_0_HIT  (1<<0)
#define PAGE_SPY_STATUS_PAGE_1      (1<<1)
#define PAGE_SPY_STATUS_PAGE_1_NOT_HIT (0<<1)
#define PAGE_SPY_STATUS_PAGE_1_HIT  (1<<1)
#define PAGE_SPY_STATUS_PAGE_2      (1<<2)
#define PAGE_SPY_STATUS_PAGE_2_NOT_HIT (0<<2)
#define PAGE_SPY_STATUS_PAGE_2_HIT  (1<<2)
#define PAGE_SPY_STATUS_PAGE_3      (1<<3)
#define PAGE_SPY_STATUS_PAGE_3_NOT_HIT (0<<3)
#define PAGE_SPY_STATUS_PAGE_3_HIT  (1<<3)
#define PAGE_SPY_STATUS_PAGE_4      (1<<4)
#define PAGE_SPY_STATUS_PAGE_4_NOT_HIT (0<<4)
#define PAGE_SPY_STATUS_PAGE_4_HIT  (1<<4)
#define PAGE_SPY_STATUS_PAGE_5      (1<<5)
#define PAGE_SPY_STATUS_PAGE_5_NOT_HIT (0<<5)
#define PAGE_SPY_STATUS_PAGE_5_HIT  (1<<5)
#define PAGE_SPY_STATUS_PAGE(n)     (((n)&0x3F)<<0)
#define PAGE_SPY_STATUS_PAGE_MASK   (0x3F<<0)
#define PAGE_SPY_STATUS_PAGE_SHIFT  (0)

//disable
#define PAGE_SPY_DISABLE_PAGE_0     (1<<0)
#define PAGE_SPY_DISABLE_PAGE_1     (1<<1)
#define PAGE_SPY_DISABLE_PAGE_2     (1<<2)
#define PAGE_SPY_DISABLE_PAGE_3     (1<<3)
#define PAGE_SPY_DISABLE_PAGE_4     (1<<4)
#define PAGE_SPY_DISABLE_PAGE_5     (1<<5)
#define PAGE_SPY_DISABLE_PAGE(n)    (((n)&0x3F)<<0)
#define PAGE_SPY_DISABLE_PAGE_MASK  (0x3F<<0)
#define PAGE_SPY_DISABLE_PAGE_SHIFT (0)

//start
#define PAGE_SPY_START_ADDRESS(n)   (((n)&0x3FFFFFF)<<0)
#define PAGE_SPY_DETECT_READ        (1<<30)
#define PAGE_SPY_DETECT_WRITE       (1<<31)
#define PAGE_SPY_MODE(n)            (((n)&3)<<30)
#define PAGE_SPY_MODE_MASK          (3<<30)
#define PAGE_SPY_MODE_SHIFT         (30)

//end
#define PAGE_SPY_END_ADDRESS(n)     (((n)&0x3FFFFFF)<<0)

//master
#define PAGE_SPY_MASTER(n)          (((n)&7)<<0)
#define PAGE_SPY_MASTER_NO_ACCESS   (0<<0)
#define PAGE_SPY_MASTER_BIST        (1<<0)
#define PAGE_SPY_MASTER_DMA         (2<<0)
#define PAGE_SPY_MASTER_SYS_XCPU    (3<<0)
#define PAGE_SPY_MASTER_AHB2AHB     (4<<0)
#define PAGE_SPY_MASTER_IFC         (5<<0)
#define PAGE_SPY_MASTER_USB         (6<<0)

//addr
#define PAGE_SPY_HIT_ADDR(n)        (((n)&0x1FFFFFF)<<0)
#define PAGE_SPY_HIT_READ           (1<<30)
#define PAGE_SPY_HIT_WRITE          (1<<31)





#endif

