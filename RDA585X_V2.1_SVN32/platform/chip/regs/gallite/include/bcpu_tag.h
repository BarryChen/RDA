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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/bcpu_tag.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _BCPU_TAG_H_
#define _BCPU_TAG_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bcpu_tag'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BCPU_TAG_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_BCPU_TAG_BASE           0x0190B000

typedef volatile struct
{
    /// The BCpu tags are accessible only when the corresponding cache is disabled
    /// or the cpu is stalled. 
    /// The bits "9:4" of the address select the line
    REG32                          Line[EXP2(6)];                //0x00000000
} HWP_BCPU_TAG_T;

#define hwp_bcpuTag                 ((HWP_BCPU_TAG_T*) KSEG1(REG_BCPU_TAG_BASE))


//Line
#define BCPU_TAG_TAG(n)             (((n)&0x7FFF)<<10)
#define BCPU_TAG_TAG_MASK           (0x7FFF<<10)
#define BCPU_TAG_TAG_SHIFT          (10)
#define BCPU_TAG_VALID              (1<<29)
#define BCPU_TAG_VALID_MASK         (1<<29)
#define BCPU_TAG_VALID_SHIFT        (29)





#endif

