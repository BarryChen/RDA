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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_gdb_stub_mips.S $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-10-11 14:24:51 +0800 (周一, 11 十月 2010) $                     //   
//    $Revision: 2715 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_gbd_stub_mips.S
///     GDB assembly functions for the GDB stub, 
///     about cache flushing
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#include "regdef.h"
.text

.globl hal_GdbFlushCache
.ent   hal_GdbFlushCache
.frame sp,0,ra
hal_GdbFlushCache:
    .set noreorder
    .align 4
    cache 0,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end hal_GdbFlushCache

.globl hal_GdbFlushCacheData
.ent   hal_GdbFlushCacheData
.frame sp,0,ra
hal_GdbFlushCacheData:
    .set noreorder
    .align 4
    cache 2,0
    nop
    nop
    nop
    jr ra
    nop
    .set reorder
.end hal_GdbFlushCacheData
