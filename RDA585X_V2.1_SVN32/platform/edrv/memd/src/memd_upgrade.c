////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2009, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/memd/src/memd_switch_mode.c $
//	$Author: huazeng $
//	$Date: 2010-08-23 13:47:34 +0800 (星期一, 23 八月 2010) $
//	$Revision: 1532 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file memd_upgrade.c                                                  //
/// That file implements the function to switch the flash into a RAM mode (where
/// it behaves like a RAM), for Flash model which are not the Romulator (Ie for
/// which this is not possible)
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"

#include "memd_m.h"
#include "memdp_debug.h"
#include "memdp.h"

#include "ram_run.tab"
#include "hal_boot_sector_reload.h"

// =============================================================================
// memd_FlashUpgrade
// -----------------------------------------------------------------------------
/// Earse the section and write new data.
///
///
/// @param  flashAddress The byte offset within the flash chip. (Take care not 
/// @param  buffer Buffer where to get the data to write in the flash
/// @param  byteSize Number of bytes to write in flash
/// @param  reboot Reboot system or not.
/// @return #MEMD_ERR_NO, #MEMD_ERR_WRITE or #MEMD_ERR_PROTECT
// =============================================================================
#if 0
PUBLIC MEMD_ERR_T memd_FlashUpgrade(UINT8 *flashAddress, UINT8 *buffer, UINT32 byteSize, BOOL reboot)
{
    UINT32 ByteSize;
    MEMD_ERR_T result;
    
    result = memd_FlashErase(flashAddress, flashAddress+byteSize);
    if(result != MEMD_ERR_NO)
        return result;
    result = memd_FlashWrite(flashAddress, byteSize, &ByteSize, buffer);
    if(result != MEMD_ERR_NO)
    {
        return result;
    }
    if(reboot)
    {
        hal_SysSoftReset();
    }

    return result;
}
#endif


PUBLIC void memd_updateFlash(void)
{
    UINT32 fillIdx=0;
    BOOT_SECTOR_RELOAD_CTX_T ctx;
    
    fillIdx++;
    ctx.mapAddress = 0x01980000;
    ctx.storeAddress =ram_run1;
    ctx.size =sizeof(ram_run1)/sizeof(UINT32);
    hal_BootSectorFillReloadCtx(&ctx,fillIdx-1);

    fillIdx++;
    ctx.mapAddress = 0x01c000a0;
    ctx.storeAddress =ram_run2;
    ctx.size =sizeof(ram_run2)/sizeof(UINT32);
    hal_BootSectorFillReloadCtx(&ctx,fillIdx-1);

    fillIdx++;
    ctx.mapAddress = 0x01c0027c;
    ctx.storeAddress =ram_run3;
    ctx.size =sizeof(ram_run3)/sizeof(UINT32);
    hal_BootSectorFillReloadCtx(&ctx,fillIdx-1);  

    hal_BootSectorReloadEnable();    
}


