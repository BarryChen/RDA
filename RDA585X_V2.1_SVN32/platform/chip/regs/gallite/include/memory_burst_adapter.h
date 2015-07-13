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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/memory_burst_adapter.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _MEMORY_BURST_ADAPTER_H_
#define _MEMORY_BURST_ADAPTER_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'memory_burst_adapter'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// MEMORY_BURST_ADAPTER_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_CS0_CONF_BASE           0x02000000
#define REG_CS1_CONF_BASE           0x00000000
#define REG_CS2_CONF_BASE           0x01800000
#define REG_CS3_CONF_BASE           0x01880000
#define REG_CS4_CONF_BASE           0x01000000

typedef volatile struct
{
    /// This register is accessible only when CRE is set to 1. 
    /// When read and write are not both in same mode, a cycle with CS high must
    /// be insterted between read and write access.
    REG16                          Burst_Config;                 //0x00000000
    REG16                          Scratch;                      //0x00000002
} HWP_MEMORY_BURST_ADAPTER_T;

#define hwp_cs0Conf                 ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS0_CONF_BASE))
#define hwp_cs1Conf                 ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS1_CONF_BASE))
#define hwp_cs2Conf                 ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS2_CONF_BASE))
#define hwp_cs3Conf                 ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS3_CONF_BASE))
#define hwp_cs4Conf                 ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS4_CONF_BASE))


//Burst_Config
#define MEMORY_BURST_ADAPTER_READ_MODE_ASYNCHRONOUS (0<<0)
#define MEMORY_BURST_ADAPTER_READ_MODE_SYNCHRONOUS (1<<0)
#define MEMORY_BURST_ADAPTER_WRITE_MODE_ASYNCHRONOUS (0<<1)
#define MEMORY_BURST_ADAPTER_WRITE_MODE_SYNCHRONOUS (1<<1)
#define MEMORY_BURST_ADAPTER_BURST_LAT(n) (((n)&15)<<4)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE(n) (((n)&7)<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_MASK (7<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_SHIFT (8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_4HW (0<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_8HW (1<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_16HW (2<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_32HW (3<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_64HW (4<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_128HW (5<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_256HW (6<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_FULL (7<<8)

//Scratch
#define MEMORY_BURST_ADAPTER_SCRATCH(n) (((n)&0xFFFF)<<0)





#endif

