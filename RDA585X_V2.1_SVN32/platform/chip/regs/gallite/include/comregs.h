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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/regs/gallite/include/comregs.h $                                                                   
//  $Author: huazeng $                                                                    
//  $Date: 2011-12-02 15:56:10 +0800 (周五, 02 十二月 2011) $                                                                      
//  $Revision: 12275 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _COMREGS_H_
#define _COMREGS_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'comregs'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// COMREGS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_SYS_COMREGS_BASE        0x01A0B000
#define REG_BB_COMREGS_BASE         0x0190E000

typedef volatile struct
{
    REG32                          Snapshot;                     //0x00000000
    REG32                          Snapshot_Cfg;                 //0x00000004
    REG32                          Cause;                        //0x00000008
    REG32                          Mask_Set;                     //0x0000000C
    REG32                          Mask_Clr;                     //0x00000010
    /// If accesses to ItReg_Set and ItReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ItReg_Set;                    //0x00000014
    /// If accesses to ItReg_Set and ItReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ItReg_Clr;                    //0x00000018
    /// If accesses to ReqReg_Set and ReqReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ReqReg_Set;                   //0x0000001C
    /// If accesses to ReqReg_Set and ReqReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ReqReg_Clr;                   //0x00000020
    /// If accesses to AckReg_Set and AckReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          AckReg_Set;                   //0x00000024
    /// If accesses to AckReg_Set and AckReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          AckReg_Clr;                   //0x00000028
} HWP_COMREGS_T;

#define hwp_sysComregs              ((HWP_COMREGS_T*) KSEG1(REG_SYS_COMREGS_BASE))
#define hwp_bbComregs               ((HWP_COMREGS_T*) KSEG1(REG_BB_COMREGS_BASE))


//Snapshot
#define COMREGS_SNAPSHOT(n)         (((n)&3)<<0)

//Snapshot_Cfg
#define COMREGS_SNAPSHOT_CFG_WRAP_2 (2<<0)
#define COMREGS_SNAPSHOT_CFG_WRAP_3 (3<<0)

//Cause
#define COMREGS_X_IRQ0_CAUSE(n)     (((n)&15)<<0)
#define COMREGS_X_IRQ0_CAUSE_MASK   (15<<0)
#define COMREGS_X_IRQ0_CAUSE_SHIFT  (0)
#define COMREGS_X_IRQ1_CAUSE(n)     (((n)&15)<<4)
#define COMREGS_X_IRQ1_CAUSE_MASK   (15<<4)
#define COMREGS_X_IRQ1_CAUSE_SHIFT  (4)
#define COMREGS_B_IRQ0_CAUSE(n)     (((n)&15)<<8)
#define COMREGS_B_IRQ0_CAUSE_MASK   (15<<8)
#define COMREGS_B_IRQ0_CAUSE_SHIFT  (8)
#define COMREGS_B_IRQ1_CAUSE(n)     (((n)&15)<<12)
#define COMREGS_B_IRQ1_CAUSE_MASK   (15<<12)
#define COMREGS_B_IRQ1_CAUSE_SHIFT  (12)

//Mask_Set
#define COMREGS_X_IRQ0_MASK_SET(n)  (((n)&15)<<0)
#define COMREGS_X_IRQ0_MASK_SET_MASK (15<<0)
#define COMREGS_X_IRQ0_MASK_SET_SHIFT (0)
#define COMREGS_X_IRQ1_MASK_SET(n)  (((n)&15)<<4)
#define COMREGS_X_IRQ1_MASK_SET_MASK (15<<4)
#define COMREGS_X_IRQ1_MASK_SET_SHIFT (4)
#define COMREGS_B_IRQ0_MASK_SET(n)  (((n)&15)<<8)
#define COMREGS_B_IRQ1_MASK_SET(n)  (((n)&15)<<12)

//Mask_Clr
#define COMREGS_X_IRQ0_MASK_CLR(n)  (((n)&15)<<0)
#define COMREGS_X_IRQ0_MASK_CLR_MASK (15<<0)
#define COMREGS_X_IRQ0_MASK_CLR_SHIFT (0)
#define COMREGS_X_IRQ1_MASK_CLR(n)  (((n)&15)<<4)
#define COMREGS_X_IRQ1_MASK_CLR_MASK (15<<4)
#define COMREGS_X_IRQ1_MASK_CLR_SHIFT (4)
#define COMREGS_B_IRQ0_MASK_CLR(n)  (((n)&15)<<8)
#define COMREGS_B_IRQ1_MASK_CLR(n)  (((n)&15)<<12)

//ItReg_Set
#define COMREGS_X_IRQ0_SET(n)       (((n)&15)<<0)
#define COMREGS_X_IRQ0_SET_MASK     (15<<0)
#define COMREGS_X_IRQ0_SET_SHIFT    (0)
#define COMREGS_X_IRQ1_SET(n)       (((n)&15)<<4)
#define COMREGS_X_IRQ1_SET_MASK     (15<<4)
#define COMREGS_X_IRQ1_SET_SHIFT    (4)
#define COMREGS_B_IRQ0_SET(n)       (((n)&15)<<8)
#define COMREGS_B_IRQ0_SET_MASK     (15<<8)
#define COMREGS_B_IRQ0_SET_SHIFT    (8)
#define COMREGS_B_IRQ1_SET(n)       (((n)&15)<<12)
#define COMREGS_B_IRQ1_SET_MASK     (15<<12)
#define COMREGS_B_IRQ1_SET_SHIFT    (12)
#define COMREGS_X_IRQ(n)            (((n)&0xFF)<<0)
#define COMREGS_X_IRQ_MASK          (0xFF<<0)
#define COMREGS_X_IRQ_SHIFT         (0)
#define COMREGS_B_IRQ(n)            (((n)&0xFF)<<8)
#define COMREGS_B_IRQ_MASK          (0xFF<<8)
#define COMREGS_B_IRQ_SHIFT         (8)

//ItReg_Clr
#define COMREGS_X_IRQ0_CLR(n)       (((n)&15)<<0)
#define COMREGS_X_IRQ1_CLR(n)       (((n)&15)<<4)
#define COMREGS_B_IRQ0_CLR(n)       (((n)&15)<<8)
#define COMREGS_B_IRQ1_CLR(n)       (((n)&15)<<12)

//ReqReg_Set
#define COMREGS_REQUEST_SET(n)      (((n)&0xFFFFFFFF)<<0)

//ReqReg_Clr
#define COMREGS_REQUEST_CLR(n)      (((n)&0xFFFFFFFF)<<0)

//AckReg_Set
#define COMREGS_ACKNOWLEDGE_SET(n)  (((n)&0xFFFFFFFF)<<0)

//AckReg_Clr
#define COMREGS_ACKNOWLEDGE_CLR(n)  (((n)&0xFFFFFFFF)<<0)




#endif

