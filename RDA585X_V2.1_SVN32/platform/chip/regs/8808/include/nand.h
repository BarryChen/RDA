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
//  $HeadURL: http://10.10.100.14/svn/developing1/Sources/chip/branches/8808/regs/gallite/include/debug_host.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _NAND_H_
#define _NAND_H_

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
// NAND_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_NAND_BUF_BASE          0x01A26000

#define REG_NAND_CONF_BASE         0x01A28000

typedef volatile struct
{
    REG32                          data[512];                       //0x00000000
    REG32                          hole[512];                       //0x00000000
    REG32                          spare[256];                         //0x00000004
    REG32                          command[128];                        //0x00000008
    REG32                          sys[640];                         //0x0000000C
} HWP_NAND_BUF_T;

typedef volatile struct
{
    REG32                          dcmd;                 //0x00000010
    REG32                          op;                   //0x00000014
    REG32                          cp;                          //0x00000018
    REG32                          car;                          //0x00000018
    REG32                          conra;                          //0x00000018
    REG32                          conrb;                          //0x00000018
    REG32                          bufcon;                          //0x00000018
    REG32                          bsy;                          //0x00000018
    REG32                          imr;                          //0x00000018
    REG32                          isr;                          //0x00000018
    REG32                          idra;                          //0x00000018
    REG32                          idrb;                          //0x00000018
    REG32                          reqen;                          //0x00000018
    REG32                          defa;                          //0x00000018
    REG32                          defb;                          //0x00000018
} HWP_NAND_CONF_T;

#define hwp_nand_buf               ((HWP_NAND_BUF_T*) KSEG1(REG_NAND_BUF_BASE))

#define hwp_nand_conf              ((HWP_NAND_CONF_T*) KSEG1(REG_NAND_CONF_BASE))



#endif // 


