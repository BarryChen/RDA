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
//  $HeadURL: http://10.10.100.14/svn/developing1/Sources/chip/branches/8808/boot/src/boot_cipher.c $ //
//    $Author: admin $                                                       // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_nand.c                                                            //
/// BOOT NAND driver                                                           //
//                                                                            //
//////////////////////////////////////////////////////////////////////////////// 

#include "cs_types.h"

#include "global_macros.h"
#include "nand.h"
#include "boot.h"
#include "bootp_mode.h"
#include "boot_dma.h"
#include "boot_nand.h"

#define NAND_CONFIG_A_PAGE_2K   (0x47b)
#define NAND_CONFIG_A_PAGE_512  (0x469)
#define NAND_CONFIG_B           (0x3e)

#define NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_2K   (11)
#define NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_512  (9)

#define NAND_PAGE_SIZE_PAGE_2K  (1<<NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_2K)
#define NAND_PAGE_SIZE_PAGE_512 (1<<NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_512)

// =============================================================================
// boot_NandOpen
// -----------------------------------------------------------------------------
/// Initialize NAND flash controller
/// @param None
/// @return None
// =============================================================================

PUBLIC VOID boot_NandOpen(VOID)
{
    // Reset flash
    hwp_nand_conf->dcmd = 0xff000000;
    // Wait until finished
    while(hwp_nand_conf->bsy & 0x3);

    // Config NAND flash controller
    if (g_bootBootMode & BOOT_MODE_ADMUX_OR_NAND_PAGE_2K)
    {
        hwp_nand_conf->conra = NAND_CONFIG_A_PAGE_2K;
    }
    else
    {
        hwp_nand_conf->conra = NAND_CONFIG_A_PAGE_512;
    }

    hwp_nand_conf->conrb = NAND_CONFIG_B;

    // Write column address reg to 0
    hwp_nand_conf->car = 0;
}


// =============================================================================
// boot_NandGetPageSize
// -----------------------------------------------------------------------------
/// Get the page size of the NAND flash
/// @param None
/// @return The page size
// =============================================================================

PUBLIC UINT32 boot_NandGetPageSize(VOID)
{
    if (g_bootBootMode & BOOT_MODE_ADMUX_OR_NAND_PAGE_2K)
    {
        return NAND_PAGE_SIZE_PAGE_2K;
    }
    else
    {
        return NAND_PAGE_SIZE_PAGE_512;
    }
}


// =============================================================================
// boot_NandReadByPageNum
// -----------------------------------------------------------------------------
/// Read one page of data from NAND flash by page number
/// @param page    The page number in NAND flash
/// @param pData   Pointing to a buffer to hold the data (should be word-aligned)
/// @param len     The number of bytes to be read (should be mulitple of 4)
/// @return None
// =============================================================================

PUBLIC VOID boot_NandReadByPageNum(UINT32 page, UINT32 *pData, UINT32 len)
{
    // Config to indirect mode, and flush page data buffer
    hwp_nand_conf->bufcon = 7;
    // Write column address reg to 0
    hwp_nand_conf->car = 0;
    // Read page
    hwp_nand_conf->dcmd = page&0x00ffffff;
    // Wait until finished
    while(hwp_nand_conf->bsy & 0x3);
    // Page data is in buffer now
#if 0
    UINT32 nWords = len >> 2;
    REG32 *bufData = &(hwp_nand_buf->data[0]);
    UINT32 *outData = (UINT32 *)pData;

    for (UINT32 i=0; i<nWords; i++)
    {
        *outData++ = *bufData++;
    }
#else
    boot_DmaCopy(pData, (UINT32 *)&(hwp_nand_buf->data[0]), len);
#endif
}


// =============================================================================
// boot_NandReadPages
// -----------------------------------------------------------------------------
/// Read multiple page data from NAND flash
/// @param addr    The start address in NAND flash (should be page-aligned)
/// @param pData   Pointing to a buffer to hold the data (should be word-aligned)
/// @param len     The number of bytes to be read (should be mulitple of 4)
/// @return None
// =============================================================================

PUBLIC VOID boot_NandReadPages(UINT32 addr, UINT32 *pData, UINT32 len)
{
    UINT32 pageSize;
    UINT32 page;
    UINT32 pageEnd;

    if (g_bootBootMode & BOOT_MODE_ADMUX_OR_NAND_PAGE_2K)
    {
        pageSize = NAND_PAGE_SIZE_PAGE_2K;
        page = addr >> NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_2K;
        pageEnd = page + (len>>NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_2K);
    }
    else
    {
        pageSize = NAND_PAGE_SIZE_PAGE_512;
        page = addr >> NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_512;
        pageEnd = page + (len>>NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_512);
    }

    UINT32 *outData = pData;

    while (page < pageEnd)
    {
        boot_NandReadByPageNum(page++, outData, pageSize);
        outData += (pageSize>>2);
    }
}

