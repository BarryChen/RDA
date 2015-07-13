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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/src/boot_loader.c $ //
//  $Author: admin $                                                         //
//  $Date: 2012-09-28 13:32:55 +0800 (周五, 28 九月 2012) $                     //
//  $Revision: 17191 $                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file xcpu_init.c                                                         //
/// That file contains the XCPU init function                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"
#include "chip_id.h"

#include "global_macros.h"
#include "sys_ctrl.h"
#include "bb_sram.h"

#include "boot.h"
#include "boot_host.h"
#include "bootp_debug.h"
#include "bootp_loader.h"

#include "tgt_hal_cfg.h"

#include "hal_host.h"
#include "hal_mem_map.h"
#include "hal_open.h"
#include "hal_timers.h"
#include "hal_sys.h"

#include "hal_debug.h"

#include "tgt_calib_m.h"
#include "boot_map.h"

// For the opening of the flash
#include "memd_m.h"
#include "tgt_memd_cfg.h"

#include "sys_irq.h"
#include "gpio.h"
// Boot loader entry point.
extern VOID boot_LoaderEnter(UINT32 param);

// ROM version number (defined in ROM)
extern CONST UINT32 g_bootRomVersionNumber;

// Standard main entry, user function.
extern int main(int, char*[]);

// Calibration entry point.
extern int calib_StubMain(int, char*[]);

// initialise cos memory
VOID COS_MemIint();

// =============================================================================
// g_halFixedPointer
// -----------------------------------------------------------------------------
/// Address of the fixed pointer. At this address, the host tools
/// will find a pointer to the configuare structure.
// =============================================================================
PROTECTED HAL_HOST_CONFIG_t* g_halFixedPointer __attribute__((section (".fixptr")));
EXPORT HAL_HOST_CONFIG_t g_halHostConfig;
EXPORT UINT16 g_TraceBitMap [];

#define BOOT_LOADER_SECTION_START __attribute__((section(".text.boot_loader_start")))

// initialise user data area in flash
void DSM_UserDataInit(UINT32 *start_addr);
UINT32 pmd_GetEncryptUid();

// =============================================================================
//  boot_LoaderEnter
// -----------------------------------------------------------------------------
/// Real implementation of the boot loader, loading code after the stack has 
/// been reseted in #boot_LoaderEnter();
///
/// @param param This parameter as a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID boot_Loader(UINT32 param);

// =============================================================================
//  boot_LoaderEnter
// -----------------------------------------------------------------------------
/// Entry point for the code out of the boot sector.
///
/// To compensate for the stack usage of the boot romed code, this function
/// resets the stack pointer at the usable top of the internal SRAM, and call
/// the 'real' boot loader without any local variable on the stack that could
/// thus get corrupted.
/// @param param This parameter as a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID BOOT_LOADER_SECTION_START boot_LoaderEnter(UINT32 param)
{
    // Initializes IRQ stack top to the top of the available sram
    // and set here stack pointer to the desired location,
    // at the top of the int sram that is not preallocated for, eg,
    // the mailbox. This location is provided through the symbol
    // properly defined in the linker script _sys_sram_top.
    sxr_IrqStack_pointer = (UINT32) &_sys_sram_top;
    // $29 is the SP register.
    asm volatile("move $29, %0" : /* no output */ : "r"(sxr_IrqStack_pointer));

    boot_Loader(param);

    // Sep. 05, 2009 - Hua Zeng - Workaround for stack corruption issue after resetting SP.
    // If we are not using -mips16 (ISA is mips1 by default), with -O2 or more optimazation specified,
    // in case that the final statement is a function call, the compiler will pop the using stack first
    // and then branch to the final function call. Unfortunately, here we just reset the SP to
    // the top of the stack space, so the stack of the final function falls beyond the stack top.
    // The workaournd is to add a dummy statement as the final statement so as to avoid the optimization.
    asm volatile("nop");
}

// =============================================================================
//  boot_BreakBootMain
// -----------------------------------------------------------------------------
/// Called when launching the code in elfmaster debug mode. The purpose is to 
/// break when the code starts, to begin a debugging session.
///
/// Hi! This is Elf Master. You got here because you started me
/// with the "Run Program" option. Your code has been restarted,
/// and is currently stalled at the beginning. You can hit
/// "continue" to unstall it, or you can set some additional 
/// breakpoints where you need to.
// =============================================================================
PROTECTED VOID boot_BreakBootMain( VOID )
{
    // WELCOME TO ELFMASTER!
    BREAKPOINT;
    main(0, 0);
}

// =============================================================================
//  boot_Loader
// -----------------------------------------------------------------------------
/// Real implementation of the boot loader, loading code after the stack has 
/// been reseted in #boot_LoaderEnter();
///
/// @param param This parameter as a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID boot_Loader(UINT32 param)
{
    UINT32* src;
    UINT32* dst;
#ifndef _FLASH_PROGRAMMER
    UINT32 system_flag;
#endif

    // NO FUNCTION CALL ALLOWED AS LONG AS THE CODE HAS NOT BEEN
    // COPIED FROM FLASH TO RAM...

#if 0 // tianwq
    // If a WD occured, treat it before initializing the bb SRAM.
    if (hwp_sysCtrl->Reset_Cause & SYS_CTRL_WATCHDOG_RESET_HAPPENED)
    {
        if  ((g_halWatchdogCtx.validityMarker == 0xC001C001) ||
            (g_halWatchdogCtx.validityMarker == 0xDEADBEEF) )
        {
            // This is a real watchdog and the structure is valid.
            UINT32* saved_context_ptr = (UINT32*) KSEG1(REG_BB_SRAM_BASE);
            UINT32 i;

            // Copy from BB_SRAM into External sram structure.
            for (i = 0; i < sizeof(HAL_DBG_ERROR_CTX_T)/4; i++)
            {
                *(&(g_halWatchdogCtx.errorContext.zero) + i) =
                    *(saved_context_ptr + i);
            }
            // reset the marker to normal
            g_halWatchdogCtx.validityMarker = 0xC001C001;
            hal_TimeWdRecover();
        }
        else
        {
            // The structure is not valid behave as in case of soft reset.
            g_halWatchdogCtx.validityMarker = 0xDEADBEEF;
            //hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_WATCHDOG_STATUS;
        }
    }
    else
    {
        g_halWatchdogCtx.validityMarker = 0xC001C001;
    }
#endif

#ifndef _FLASH_PROGRAMMER
    system_flag = *(((U32 *)(&_flash_start))+1);
#endif

    // Set the global variables from ROM not set by the ROM code.
    xcpu_error_code   = 0;
    xcpu_error_status = 0;
    xcpu_sp_context   = 0;

#ifndef _FLASH_PROGRAMMER
    // Copy code & data from ROM space to Dualport SRAM.
    for (dst = (UINT32*) &_bcpu_sys_sram_bss_start;
         dst < (UINT32*) &_bcpu_sys_sram_bss_end;
         dst++)
    {
        *dst = 0;
    }

    // Clear BSS in Dualport SRAM.
    for (dst = (UINT32*) &_dualport_bss_start;
         dst < (UINT32*) &_dualport_bss_end;
         dst++)
    {
        *dst = 0;
    }

    // Clear BSS in Dualport SRAM.
    for (dst = (UINT32*) &_ram_bss_start;
         dst < (UINT32*) &_ram_bss_end;
         dst++)
    {
        *dst = 0;
    }
#endif

    // Copy irq code from ROM space to internal SRAM.
    for (src = (UINT32*) &_irq_sys_sram_flash_start_location,
             dst = (UINT32*) &_irq_sys_sram_start;
         src < (UINT32*) &_irq_sys_sram_flash_end_location;
         src++, dst++)
    {
        *dst = *src;
    }

    // Copy code & data from ROM space to internal SRAM.
    for (src = (UINT32*) &_sys_sram_flash_start_location,
             dst = (UINT32*) &_sys_sram_start;
         src < (UINT32*) &_sys_sram_flash_end_location;
         src++, dst++)
    {
        *dst = *src;
    }

    // Clear BSS in Internal SRAM.
    for (dst = (UINT32*) &_sys_sram_bss_start;
         dst < (UINT32*) &_sys_sram_bss_end;
         dst++)
    {
        *dst = 0;
    }

#ifndef _FLASH_PROGRAMMER // flash don't use host config and memory manager

    g_halFixedPointer = &g_halHostConfig;
    g_halHostConfig.trace_mask = g_TraceBitMap;
    g_halHostConfig.audio_calib = tgt_GetCalibConfig();
    g_halHostConfig.chip_id = CHIP_ASIC_ID;
    g_halHostConfig.max_gain_level = AUD_MAX_LEVEL;

    // setup heap
    COS_MemIint();

    // Initialise Resource and User Data Aera
#ifdef RESOURCE_SUPPORT
    {
        extern  U8 *gpResourceBase;
        extern const UINT32 g_flash_size;
        
        UINT32 *ptr;
        gpResourceBase = (U8*)g_flash_size;
        ptr = (UINT32*)gpResourceBase;
        while(*ptr)
        {
            ptr += ptr[1]/4;
        };
        DSM_UserDataInit(ptr);
    }
#else
    {
        UINT32 *ptr = (UINT32*)&_flash_end;
        DSM_UserDataInit(ptr);
    }
#endif
#endif

#ifdef REWRITE_ROM_AT_BOOT
#ifdef FPGA
    // Copy data from flash space to internal ROM.

    // Unlock.
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;

    for (src = (UINT32*) &_int_rom_flash_start_location,
             dst = (UINT32*) &_int_rom_start;
         src < (UINT32*) &_int_rom_flash_end_location;
         src++, dst++)
    {
        *dst = *src;
    }

    // Lock.
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif // FPGA
#else // !REWRITE_ROM_AT_BOOT
    // Check that the ROM version in the chip is the same as the ROM version
    // of the ROM Library against which the code has been compiled.
    //if(ROM_LINKED_AGAINST_VERSION != *(UINT32*)ROM_VERSION_LOCATION)
    //if(ROM_LINKED_AGAINST_VERSION != *(UINT32*)&_int_rom_version_number) // for 8808 or later
    if(ROM_LINKED_AGAINST_VERSION != g_bootRomVersionNumber)
    {
        // Mismatch!
        hal_HstSendEvent(BOOT_EVENT,BOOT_LOADER_EVENT_ROM_ERROR);
        while(1);
    }
#endif // !REWRITE_ROM_AT_BOOT

#ifndef NO_BCPU_IMAGE
#ifndef CHIP_HAS_BCPU_ROM
    // Same thing for the bcpu. These are needed by GDB.
    bcpu_error_code   = HAL_DBG_ERROR_NO;
    bcpu_error_status = 0xc001c001;
    bcpu_sp_context   = (UINT32*)0x0db90db9;
#endif
#endif

    // FUNCTIONS CAN BE CALL NOW!
    // Enable exception (gdb breaks), Host, Debug and Page Spy external it,
    // but not regular external it.
    asm volatile("li    $27,    (0x0001 | 0xe000)\n\t"
                 "mtc0  $0, $13\n\t"
                 "nop\n\t"
                 "mtc0  $27, $12\n\t"
                 // k1 is status for os irq scheme.
                 "li    $27, 1");

    hal_HstSendEvent(BOOT_EVENT,BOOT_LOADER_EVENT_START_CODE);

    // get CdacDelta
#ifndef _FLASH_PROGRAMMER
    {
        extern INT16 g_CdacDelta;
        if((system_flag&4)==0)
            g_CdacDelta = (INT16)*(((INT32 *)(&_flash_start))+2);
    }
#endif

    hal_Open(tgt_GetHalConfig());

#ifndef _FLASH_PROGRAMMER // flash don't need decrpyt and testmode
    if(system_flag&2) // decrypt flash
    {
        UINT8 *data_buf = (UINT8*)&_flash_end;
        UINT32 encrypt_key = 0;
        int count;
        count = ((data_buf[2])+(data_buf[3]<<8));
        while(count>=0)
        {
            if((data_buf[(2+count)*4])==0x03)
            {
                count = (2+count)*4+((data_buf[(2+count)*4+2])+((data_buf[(2+count)*4+3])<<8))+38;
                encrypt_key = 0x5851;//data_buf[count] + (data_buf[count+1]<<8) + (data_buf[count+2]<<16) + (data_buf[count+3]<<24);
                encrypt_key = encrypt_key ^ (pmd_GetEncryptUid()<<2);
                break;
            }
            count --;
        }

        count = 0;
        for (dst = (UINT32*) &_sys_sram_start;
             count < ((UINT32)&_sys_sram_txt_size)/4-4;
             dst++, count++)
        {
            *dst = (*dst) ^ encrypt_key;
       }

    }

    // check test mode
    {
        extern BOOL g_test_mode;
        UINT32 s;
        if(system_flag&1)
        {
            g_test_mode = TRUE;
            system_flag = system_flag&~1;
            memd_FlashWrite((UINT8*)0x04,sizeof(UINT32), &s,(CHAR*)(&system_flag));
        }
    }
#endif

    BOOT_PROFILE_PULSE(XCPU_MAIN);

    // Selects where we will boot: normal main or calib stub main.
    switch(param)
    {
//        case CALIB_MAGIC_TAG:
    //        calib_StubMain(0, 0);
            break;
        case BOOT_MAIN_TAG_BREAKBOOT:
            // Gdb needs the flash to be open to be able to step in the romulator!!!
            boot_BreakBootMain();
            break;
        default:
            main(0, 0);
            break;
    }
}

