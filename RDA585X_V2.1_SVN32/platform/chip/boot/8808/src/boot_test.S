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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/8808/src/boot_test.S $ //
//    $Author: huazeng $                                                         // 
//    $Date: 2010-10-11 14:24:51 +0800 (周一, 11 十月 2010) $                     //   
//    $Revision: 2715 $                                                        //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_test.S                                                         //
///     Chip Production test                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "regdef.h"
#include "cp0.h"
#include "global_macros.h"
#include "xcpu_asm.h"
#include "mem_bridge_asm.h"

    .text
    .set at
    .globl  boot_FunctionalTest
    .ent    boot_FunctionalTest
//    .frame    sp, 0, ra
boot_FunctionalTest:
    // the function is compiled as the boot entry point (__xcpu_start) as cached,
    // but is is actually executed uncached, init the cache before 
    // should test BOOT MODEs (still in a1)

$L_bist_test:
    // bist test
    // step 1 : set stack at end of second mem_bridge 32k ram block, and test the first block of ram as well as other stuff
    // step 2 : set stack at end of first mem_bridge 32k ram block, and test the rest

    // bist test step 1
    // disable xcpu cache as bist will test the memories
    li a0, REG_XCPU_BASE_HI
    li t0, XCPU_DCACHE_INHIBIT | XCPU_ICACHE_INHIBIT
    sw t0, REG_XCPU_CACHE_CONTROL(a0)
    lw zero, REG_XCPU_CACHE_CONTROL(a0) // flush

    // build stack (-16 : virtual parameter a0 to a3 of gcc function call)
    li sp, (0x80000000 + REG_INT_SRAM_BASE + 32768*2 - 16)
    jal boot_BistStep1;


    // bist test step 2
    // build stack (-16 : virtual parameter a0 to a3 of gcc function call)
    li sp, (0x80000000 + REG_INT_SRAM_BASE + 32768 - 16)
    jal boot_BistStep2;


$L_bist_test_end:
    b $L_bist_test_end

    .type    boot_FunctionalTest,@function
    .size    boot_FunctionalTest,.-boot_FunctionalTest
    .end    boot_FunctionalTest

