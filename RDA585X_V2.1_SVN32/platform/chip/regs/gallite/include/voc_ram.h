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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/voc_ram.h $                                                                   
//  $Author: huazeng $                                                                    
//  $Date: 2011-02-10 20:31:33 +0800 (Thu, 10 Feb 2011) $                                                                      
//  $Revision: 5810 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _VOC_RAM_H_
#define _VOC_RAM_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'voc_ram'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define VOC_RAM_X_SIZE                           (0X5000)
#define VOC_RAM_Y_SIZE                           (0X5000)
#define VOC_ROM_Z_SIZE                           (0X0000)
#define VOC_RAM_I_SIZE                           (0X5000)
#define VOC_RAM_X_SPACE                          (0X8000)
#define VOC_RAM_Y_SPACE                          (0X8000)
#define VOC_ROM_Z_SPACE                          (0X10000)
#define VOC_RAM_I_SPACE                          (0X20000)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// VOC_RAM_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_VOC_RAM_BASE            0x01940000

typedef volatile struct
{
    UINT8 voc_ram_x_base[20480];                //0x00000000
    UINT8 voc_ram_x_hole[12288];                //0x00005000
    UINT8 voc_ram_y_base[20480];                //0x00008000
    UINT8 voc_ram_y_hole[12288];                //0x0000D000
    UINT8 voc_rom_z_hole[65536];                //0x00010000
    UINT8 voc_ram_i_base[20480];                //0x00020000
    UINT8 voc_ram_i_hole[110592];               //0x00025000
} HWP_VOC_RAM_T;

#define hwp_vocRam                  ((HWP_VOC_RAM_T*) KSEG1(REG_VOC_RAM_BASE))






#endif

