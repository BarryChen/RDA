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
//  $HeadURL: http://10.10.100.14/svn/developing/modem2G/trunk/platform/edrv/memd/flsh_k5l2731caa_d770/src/memdp.h $ //
//	$Author: Jixj $                                                        // 
//	$Date: 2008-07-22 11:00:00 +0800 (Tue, 22 Jul 2007) $                     //   
//	$Revision: 7769 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file memdp.h                                                             //
/// Private interface in the flash driver
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _MEMDP_H_
#define _MEMDP_H_



// =============================================================================
// g_memdFlashLayout
// -----------------------------------------------------------------------------
/// This constant describes the flash layout structure 
/// using the previously defined types
// =============================================================================
EXPORT CONST MEMD_FLASH_LAYOUT_T g_memdFlashLayout;

EXPORT PUBLIC UINT32 get_flash_status(UINT32 flash_addr);
EXPORT PUBLIC BOOL spi_flash_ini(BOOL quard_flag, UINT8 clk_offset_val, UINT8 clkdiv_val, UINT8 dual_mode);
EXPORT PUBLIC void SPI_Flash_Open();
EXPORT PUBLIC BOOL spi_flash_block_erase(UINT32 flash_addr);
EXPORT PUBLIC BOOL spi_flash_chip_erase_dual();
EXPORT PUBLIC BOOL spi_flash_chip_erase(UINT32 flash_addr);
EXPORT PUBLIC BOOL spi_flash_write(UINT32 flash_addr, UINT8 data_array[], UINT32 data_size);
EXPORT PUBLIC void flash_read(UINT32 flash_addr, UINT8 data_array[], UINT32 data_size);

#endif // FLDP_H




