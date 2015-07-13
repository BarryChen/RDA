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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/8808/boot/src/boot_cipher.c $ //
//    $Author: admin $                                                       // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_dma.c                                                            //
/// BOOT DMA driver                                                           //
//                                                                            //
//////////////////////////////////////////////////////////////////////////////// 

#include "cs_types.h"

#include "global_macros.h"
#include "dma.h"
#include "boot_dma.h"

// =============================================================================
// boot_DmaCopy
// -----------------------------------------------------------------------------
/// Copy data via DMA
/// @param dst  The destination address
/// @param src  The source address
/// @param len  The data length in bytes
/// @return None
// =============================================================================

PUBLIC VOID boot_DmaCopy(VOID *dst, CONST VOID *src, UINT32 len)
{
    if (len == 0) return;

    // Check if DMA channel is available
    while (!((hwp_dma->get_channel) & DMA_GET_CHANNEL));

    hwp_dma->src_addr = (UINT32)src;
    hwp_dma->dst_addr = (UINT32)dst;
    hwp_dma->sd_dst_addr = 0;
    hwp_dma->pattern  = 0;
    hwp_dma->xfer_size = len;

    hwp_dma->control = DMA_ENABLE|DMA_INT_DONE_CLEAR;
    // Flush the CPU write buffer.
    UINT32 unused __attribute__((unused)) = hwp_dma->control;
    // Wait until finished
    while(!(hwp_dma->status & DMA_INT_DONE_STATUS));
}


// =============================================================================
// boot_DmaSet
// -----------------------------------------------------------------------------
/// Set the memory to a pattern value via DMA
/// @param dst      The destination address (should be word-aligned)
/// @param pattern  The pattern to be written
/// @param len      The data length in bytes (should be mulitple of 4)
/// @return None
// =============================================================================

PUBLIC VOID boot_DmaSet(UINT32 *dst, UINT32 pattern, UINT32 len)
{
    len &= ~0x3;
    if (len == 0) return;
    dst = (UINT32 *)((UINT32)dst & ~0x3);

    // Check if DMA channel is available
    while (!((hwp_dma->get_channel) & DMA_GET_CHANNEL));

    hwp_dma->src_addr = 0;
    hwp_dma->dst_addr = (UINT32)dst;
    hwp_dma->sd_dst_addr = 0;
    hwp_dma->pattern  = pattern;
    hwp_dma->xfer_size = len;

    hwp_dma->control = DMA_ENABLE|DMA_INT_DONE_CLEAR|DMA_USE_PATTERN;
    // Flush the CPU write buffer.
    UINT32 unused __attribute__((unused)) = hwp_dma->control;
    // Wait until finished
    while(!(hwp_dma->status & DMA_INT_DONE_STATUS));
}

