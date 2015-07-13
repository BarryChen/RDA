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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/regs/8809/include/bcpu_cache.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _BCPU_CACHE_H_
#define _BCPU_CACHE_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bcpu_cache'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BCPU_CACHE_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_BCPU_IDATA_BASE         0x0190C000
#define REG_BCPU_DDATA_BASE         0x0190D000

typedef volatile struct
{
    struct
    {
        /// The BCpu cache is accessible only when it is disabled or the cpu is stalled.
        /// 
        /// The bits "9:4" of the address select the line. 
        /// If the Tag at the corresponding line is valid, the bits "24:10" of the
        /// tag are the corresponding address bits. 
        /// The bits "3:0" of the address select the Data in the line.
        REG32                      Data[4];                      //0x00000000
    } Line[EXP2(6)];
} HWP_BCPU_CACHE_T;

#define hwp_bcpuIdata               ((HWP_BCPU_CACHE_T*) KSEG1(REG_BCPU_IDATA_BASE))
#define hwp_bcpuDdata               ((HWP_BCPU_CACHE_T*) KSEG1(REG_BCPU_DDATA_BASE))


//Data
#define BCPU_CACHE_DATA(n)          (((n)&0xFFFFFFFF)<<0)





#endif

