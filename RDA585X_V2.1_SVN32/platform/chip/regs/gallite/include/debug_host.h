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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/debug_host.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _DEBUG_HOST_H_
#define _DEBUG_HOST_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'debug_host'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// DEBUG_HOST_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_DEBUG_HOST_BASE         0x01A3F000

typedef volatile struct
{
    REG32                          cmd;                          //0x00000000
    REG32                          data;                         //0x00000004
    REG32                          event;                        //0x00000008
    REG32                          mode;                         //0x0000000C
    REG32                          h2p_status;                   //0x00000010
    REG32                          p2h_status;                   //0x00000014
    REG32                          irq;                          //0x00000018
} HWP_DEBUG_HOST_T;

#define hwp_debugHost               ((HWP_DEBUG_HOST_T*) KSEG1(REG_DEBUG_HOST_BASE))


//cmd
#define DEBUG_HOST_ADDR(n)          (((n)&0x1FFFFFFF)<<0)
#define DEBUG_HOST_SIZE(n)          (((n)&3)<<29)
#define DEBUG_HOST_WRITE_H          (1<<31)

//data
#define DEBUG_HOST_DATA(n)          (((n)&0xFFFFFFFF)<<0)

//event
#define DEBUG_HOST_EVENT0_SEMA      (1<<0)
#define DEBUG_HOST_EVENT31_1(n)     (((n)&0x7FFFFFFF)<<1)

//mode
#define DEBUG_HOST_FORCE_ON         (1<<0)
#define DEBUG_HOST_CLK_HOST_ON      (1<<1)

//h2p_status
#define DEBUG_HOST_H2P_STATUS(n)    (((n)&0xFF)<<0)
#define DEBUG_HOST_H2P_STATUS_MASK  (0xFF<<0)
#define DEBUG_HOST_H2P_STATUS_SHIFT (0)
#define DEBUG_HOST_H2P_STATUS_RST   (1<<16)

//p2h_status
#define DEBUG_HOST_P2H_STATUS(n)    (((n)&0xFF)<<0)

//irq
#define DEBUG_HOST_XCPU_IRQ         (1<<0)
#define DEBUG_HOST_BCPU_IRQ         (1<<1)





#endif

