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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/bist.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _BIST_H_
#define _BIST_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bist'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BIST_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_SYS_BIST_BASE           0x01A1F000
#define REG_BB_BIST_BASE            0x01909000

typedef volatile struct
{
    REG32                          control;                      //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          mem_addr;                     //0x00000008
    REG32                          mem_size;                     //0x0000000C
    REG32                          data_mask;                    //0x00000010
} HWP_BIST_T;

#define hwp_sysBist                 ((HWP_BIST_T*) KSEG1(REG_SYS_BIST_BASE))
#define hwp_bbBist                  ((HWP_BIST_T*) KSEG1(REG_BB_BIST_BASE))


//control
#define BIST_START                  (1<<0)

//status
#define BIST_ENABLE                 (1<<0)
#define BIST_FAIL                   (1<<4)

//mem_addr
#define BIST_MEM_ADDR(n)            (((n)&0x3FFFFFF)<<0)

//mem_size
#define BIST_MEM_SIZE(n)            (((n)&0xFFFF)<<0)
#define BIST_MEM_WIDTH(n)           (((n)&3)<<24)
#define BIST_MEM_WIDTH_MASK         (3<<24)
#define BIST_MEM_WIDTH_SHIFT        (24)
#define BIST_MEM_WIDTH_MEM_WIDTH_DEFAULT (0<<24)
#define BIST_MEM_WIDTH_MEM_WIDTH_8  (1<<24)
#define BIST_MEM_WIDTH_MEM_WIDTH_16 (2<<24)
#define BIST_MEM_WIDTH_MEM_WIDTH_32 (3<<24)

//data_mask
#define BIST_DATA_MASK(n)           (((n)&0xFFFFFFFF)<<0)





#endif

