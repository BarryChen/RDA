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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/bb_sram.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _BB_SRAM_H_
#define _BB_SRAM_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'bb_sram'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BB_SRAM_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_BB_SRAM_BASE            0x01980000

typedef volatile struct
{
    UINT8 ram_array[13312];                     //0x00000000
} HWP_BB_SRAM_T;

#define hwp_bbSram                  ((HWP_BB_SRAM_T*) KSEG1(REG_BB_SRAM_BASE))






#endif

