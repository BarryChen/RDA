////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_utils_mips.S $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-10-11 14:24:51 +0800 (周一, 11 十月 2010) $                     //   
//    $Revision: 2715 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file    hal_utils_mips.S
/// 
/// This file contains utility functions not directly linked to an 
/// hardware module
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "regdef.h"
#include "cp0.h"

//  Code works both in mips16 and mips32 

// mips16 assembler is cold with debugging feature.
// We are consequently forced to use mips32 for 
// assembly files.
    .set nomips16
    .text
    .set    reorder

// ***********************************************************************
// ***********************************************************************
//                                                                        
//  CPU context utilities                                                 
//                                                                        
// ***********************************************************************
// ***********************************************************************

// =============================================================================
// hal_SysGetSp
// -----------------------------------------------------------------------------
/// This function returns the current value of the stack register
/// @return The current value of the stack pointer.
// =============================================================================
// ***********************************************************************
/// 
/// 
///     Get current Stack Pointer value      
///                                            
///     CALLED BY
///     
///         Application                                           
///                                                             
///     CALLS 
///     
///         None
///     
///     INPUTS
///     
///         None                                 
///                                                             
///     OUTPUTS
///         
///         Stack Pointer value                             
/// 
// ***********************************************************************
//UINT32 hal_SysGetSp(void)
//{
    .globl  hal_SysGetSp
    .globl  hal_SysGetSp
    .ent    hal_SysGetSp
hal_SysGetSp:
hal_SysGetSp:    
    .frame sp, 0, ra

    move    v0,sp

    j    ra

    .type   hal_SysGetSp,@function
    .size   hal_SysGetSp,.-hal_SysGetSp

    .end hal_SysGetSp
//}

