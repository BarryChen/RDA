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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/boot/gallite/src/boot_bist.c $ //
//  $Author: admin $                                                         //
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                     //
//  $Revision: 269 $                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file boot_bist.c                                                         //
///     Internal RAMs BIST Production test
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"
#include "boot_ispi.h"

#include "global_macros.h"
#include "mem_bridge.h"
#include "bist.h"
#include "sys_ctrl.h"
#include "xcpu.h"
#include "bcpu_cache.h"
#include "bcpu_tag.h"
#include "bb_sram.h"
#include "xcpu_cache.h"
#include "xcpu_tag.h"
#include "gpio.h"
#include "dma.h"
#include "camera.h"
#include "gouda.h"
#include "voc_cfg.h"
#include "usbc.h"
#include "cfg_regs.h"

extern UINT32 bcpu_main_entry;
extern UINT32 bcpu_stack_base;
extern void boot_FlushCache(void);

void boot_BistBCpu(void);
int boot_MarchCBBCp2 (void);
int boot_MarchCTcu (void);
int boot_MarchCGouda (void);


typedef struct {
    INT32 errorCount;
    INT32 bistBBIdx;
    INT32 bistSysIdx;
    INT32 bistMemIdx;
} BOOT_BIST_STATE_T;

typedef struct {
    UINT32 addr;
    UINT32 size;
    UINT32 width;
    UINT32 mask;
    UINT32 other;
} BOOT_BIST_CONF_T;

register BOOT_BIST_STATE_T* g_bootBistStatePtr asm("$17"); // $17 is s1

// fake global absolute uncached variable
#define b_bootBistBcpuEnd *((UINT32*)(0xa0000000+ REG_BB_SRAM_BASE))


#if 1 // Real chip (Two gpo are used to check the result of the test)

#define ERROR() {g_bootBistStatePtr->errorCount++; hwp_gpio->gpo_set=(1<<0);}
#define SUCCESS() {hwp_gpio->gpo_set=(1<<2);}
#define DEBUG_PRINT(a)

#else // For simulation only (using the tester chip and the two gpo)

#include "tester_chip.h"
#define USE_TESTER_CHIP
#define VERBOSE_DEBUG // Uncomment to have more debug messages
#define ERROR() {g_bootBistStatePtr->errorCount++; hwp_gpio->gpo_set=(1<<0); hwp_testerChip->Stopper=TESTER_CHIP_STOP_CAUSE_ERROR;}
#define SUCCESS() {hwp_gpio->gpo_set=(1<<2); hwp_testerChip->Stopper=TESTER_CHIP_STOP_CAUSE_SUCCESS;}
#define DEBUG_PRINT CT_simu_print
#define CS3_MODE_SETTING 0x00000011 // tester_chip on CS3
#define CS3_TIME_SETTING 0x02080206 // tester_chip on CS3

void CT_simu_print(char*s)
{
    while (hwp_testerChip->STR_Print != 0); // wait that nobody use the print...
    while (*s)
    {
         hwp_testerChip->STR_Print = (*s++);
    }    
    // send the string
    hwp_testerChip->STR_Print = 0;
    
    {u32 flush __attribute__ ((unused)) = hwp_memFifo->Flush;}
}

#endif



#define BOOT_BIST_BB_NB_CONF 5
// assuming less of 4kByte caches of bcpu (no page change)
const BOOT_BIST_CONF_T g_bootBistBBConf[BOOT_BIST_BB_NB_CONF] = 
    { 
        {REG_BCPU_DDATA_BASE, sizeof(HWP_BCPU_CACHE_T), BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, 0}, // Data cache ram
        {REG_BCPU_IDATA_BASE, sizeof(HWP_BCPU_CACHE_T), BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, 0}, // Instruction cache ram
        {REG_BCPU_TAG_BASE, sizeof(HWP_BCPU_TAG_T), BIST_MEM_WIDTH_MEM_WIDTH_32, BCPU_TAG_VALID | BCPU_TAG_TAG_MASK, XCPU_DEBUG_PAGE_ADDRESS_DTAG}, // Data tag ram
        {REG_BCPU_TAG_BASE, sizeof(HWP_BCPU_TAG_T), BIST_MEM_WIDTH_MEM_WIDTH_32, BCPU_TAG_VALID | BCPU_TAG_TAG_MASK, XCPU_DEBUG_PAGE_ADDRESS_ITAG}, // Instruction tag ram
        {REG_BB_SRAM_BASE, sizeof(HWP_BB_SRAM_T), BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, 0} // BB_SRAM
    }; 


int boot_BistBBScheduler(void)
{
    register UINT32 t;
    if (g_bootBistStatePtr->bistBBIdx < BOOT_BIST_BB_NB_CONF) {
        if (g_bootBistStatePtr->bistBBIdx == -1) {
            t = 0;
        } else {
            t=hwp_bbBist->status;
            if (t & BIST_FAIL) {
                DEBUG_PRINT("bb bist scheduler error");
                ERROR();
            }
#ifdef VERBOSE_DEBUG
            else if (t == 0)
            {
                DEBUG_PRINT("bb bist success");
            }
#endif
        }
        if ((t & BIST_ENABLE) == 0) {
            g_bootBistStatePtr->bistBBIdx++;
            if (g_bootBistStatePtr->bistBBIdx < BOOT_BIST_BB_NB_CONF) {
                hwp_bcpu->Debug_Page_Address = g_bootBistBBConf[g_bootBistStatePtr->bistBBIdx].other;
                hwp_bbBist->mem_addr = g_bootBistBBConf[g_bootBistStatePtr->bistBBIdx].addr;
                hwp_bbBist->mem_size = g_bootBistBBConf[g_bootBistStatePtr->bistBBIdx].width | g_bootBistBBConf[g_bootBistStatePtr->bistBBIdx].size;
                hwp_bbBist->data_mask = g_bootBistBBConf[g_bootBistStatePtr->bistBBIdx].mask;
                hwp_bbBist->control = BIST_START;
#ifdef VERBOSE_DEBUG
                DEBUG_PRINT("bb bist started");
#endif
            } else if ( BOOT_BIST_BB_NB_CONF == g_bootBistStatePtr->bistBBIdx) {
                // start bcpu
                bcpu_stack_base = 0xa0000000+ REG_BB_SRAM_BASE + BB_SRAM_SIZE;
                bcpu_main_entry = (UINT32)&boot_BistBCpu;
                b_bootBistBcpuEnd = 0;
                DEBUG_PRINT("bb bist scheduler start bcpu");
                hwp_sysCtrl->BCpu_Dbg_BKP = 0;
            }
        }
        return 1;
    } else if ( BOOT_BIST_BB_NB_CONF == g_bootBistStatePtr->bistBBIdx) {
        // bcpu tests
        if (b_bootBistBcpuEnd > 1) {
            if (b_bootBistBcpuEnd != 2) {
                DEBUG_PRINT("bb bist scheduler error: bcpu failed");
                ERROR();
            }
            g_bootBistStatePtr->bistBBIdx++;
        }
        return 1;
    }

    return 0;
}



// BOOT_BIST_SYS_NB_CONF defines the number of different configuration of the BIST 
// module to check USB and XCPU Rams.
// Note that caches over MAX_PAGE_SIZE can only be accessed by pages of MAX_PAGE_SIZE
// ex:  XCPU with 6k of data cache and 4k of instruction cache
//      1 for DTAG          (size<=MAX_PAGE_SIZE)
//      + 2 for DDATA       (MAX_PAGE_SIZE<size<=2*MAX_PAGE_SIZE)
//      + 1 for ITAG        (size<=MAX_PAGE_SIZE)
//      + 1 for IDATA = 5   (size<=MAX_PAGE_SIZE)
//      Total 5 configurations
#define BOOT_BIST_SYS_NB_CONF 8
#define BOOT_BIST_SYS_NB_XCPU_CACHE_CONF 4 // this is the number of tests that are related to the XCPU, unce those tests are run, we reable the caches.
#define MAX_PAGE_SIZE 4096 // this is defined by the HW implementation of APB bus

// Configuration for Gallite TODO
// 16k data cache => 4 pages
// 16k instruction cache => 4 pages
const BOOT_BIST_CONF_T g_bootBistSysConf[BOOT_BIST_SYS_NB_CONF] = 
    { 
        {REG_XCPU_DDATA_BASE, MAX_PAGE_SIZE, BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, XCPU_DEBUG_PAGE_ADDRESS(0)}, // Data cache ram
        {REG_XCPU_IDATA_BASE, MAX_PAGE_SIZE, BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, XCPU_DEBUG_PAGE_ADDRESS(0)}, // Instruction cache ram
        {REG_XCPU_TAG_BASE, sizeof(HWP_XCPU_TAG_T), BIST_MEM_WIDTH_MEM_WIDTH_32, XCPU_TAG_VALID | XCPU_TAG_TAG_MASK, XCPU_DEBUG_PAGE_ADDRESS_DTAG}, // Data tag ram
        {REG_XCPU_TAG_BASE, sizeof(HWP_XCPU_TAG_T), BIST_MEM_WIDTH_MEM_WIDTH_32, XCPU_TAG_VALID | XCPU_TAG_TAG_MASK, XCPU_DEBUG_PAGE_ADDRESS_ITAG},  // Instruction tag ram
        {(UINT32)hwp_usbc->SPFIFORAM, SPFIFORAM_SIZE*4, BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, 0},
        {(UINT32)hwp_usbc->SPFIFORAM, SPFIFORAM_SIZE*4, BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, SYS_CTRL_USB_RAM_TEST_MODE_ENABLE},
        {(u32)hwp_camera->FIFORAM, FIFORAM_SIZE*4, BIST_MEM_WIDTH_MEM_WIDTH_32, ~0, 0},
        {REG_GOUDA_SRAM_BASE, GD_SRAM_SIZE, BIST_MEM_WIDTH_MEM_WIDTH_16, ~0, 0}, 
    };


int boot_BistSysScheduler(void)
{
    register UINT32 t;
    if (g_bootBistStatePtr->bistSysIdx < BOOT_BIST_SYS_NB_CONF) {
        if (g_bootBistStatePtr->bistSysIdx == -1) {
            t = 0;
        } else {
            t=hwp_sysBist->status;
            if (t & BIST_FAIL) {
                DEBUG_PRINT("sys bist scheduler error");
                ERROR();
            }
#ifdef VERBOSE_DEBUG
            else if (t == 0)
            {
                DEBUG_PRINT("sys bist success");
            }
#endif
        }
        if ((t & BIST_ENABLE) == 0) {
            g_bootBistStatePtr->bistSysIdx++;
            if (g_bootBistStatePtr->bistSysIdx < BOOT_BIST_SYS_NB_CONF) 
            {

                if ((g_bootBistSysConf[g_bootBistStatePtr->bistSysIdx].addr == ((UINT32)hwp_usbc->SPFIFORAM))) 
                {
                    DEBUG_PRINT("sys bist scheduler in usb 2 times");
                    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

                    hwp_sysCtrl->Cfg_AHB = SYS_CTRL_SYS_ENABLE_MASK | SYS_CTRL_BB_ENABLE_MASK | g_bootBistSysConf[g_bootBistStatePtr->bistSysIdx].other;
                    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
                }
                else
                {    
                    hwp_xcpu->Debug_Page_Address = g_bootBistSysConf[g_bootBistStatePtr->bistSysIdx].other;
                }
                
                hwp_sysBist->mem_addr = g_bootBistSysConf[g_bootBistStatePtr->bistSysIdx].addr;
                hwp_sysBist->mem_size = g_bootBistSysConf[g_bootBistStatePtr->bistSysIdx].width | g_bootBistSysConf[g_bootBistStatePtr->bistSysIdx].size;
                hwp_sysBist->data_mask = g_bootBistSysConf[g_bootBistStatePtr->bistSysIdx].mask;
                hwp_sysBist->control = BIST_START;
#ifdef VERBOSE_DEBUG
                DEBUG_PRINT("sys bist started");
#endif
            } 
            if ( BOOT_BIST_SYS_NB_XCPU_CACHE_CONF == g_bootBistStatePtr->bistSysIdx) {
                // flush the caches
                boot_FlushCache();
                // reenable all XCPU caches
                hwp_xcpu->Cache_Control = 0;
                {UINT32 unused __attribute__ ((unused)) = hwp_xcpu->Cache_Control;}
            }
        }
        return 1;
    }
    return 0;
}

#define BOOT_BIST_MEM_NB_CONF MEMBRIDGE_RAM_NB_BLOCK
int boot_BistMemScheduler(void)
{
    register UINT32 t;
    if (g_bootBistStatePtr->bistMemIdx < BOOT_BIST_MEM_NB_CONF) {
        if (g_bootBistStatePtr->bistMemIdx == -1) {
            t = 0;
            // start membridge bist : rom
            hwp_memBridge->Rom_Bist = MEM_BRIDGE_CTRL_RUNNING;
        } else {
            t=hwp_memBridge->SRam_Bist;
            if ((t & MEM_BRIDGE_CTRL_RUNNING) == 0) {
                if (t != (MEM_BRIDGE_STATUS_OK | MEM_BRIDGE_BLOCK_SEL(g_bootBistStatePtr->bistMemIdx))) {
                    DEBUG_PRINT("mem_bridge RAM Bist error");
                    ERROR();
                }
            }
        }
        if ((t & MEM_BRIDGE_CTRL_RUNNING) == 0) {
            g_bootBistStatePtr->bistMemIdx++;
            if (g_bootBistStatePtr->bistMemIdx < BOOT_BIST_MEM_NB_CONF) {
                // start membridge bist : ram
                hwp_memBridge->SRam_Bist = MEM_BRIDGE_CTRL_RUNNING | MEM_BRIDGE_BLOCK_SEL(g_bootBistStatePtr->bistMemIdx);

            } else if ( BOOT_BIST_MEM_NB_CONF == g_bootBistStatePtr->bistMemIdx) {
                if( (MEM_BRIDGE_CTRL_DONE | MEM_BRIDGE_CRC(0xc001)) != hwp_memBridge->Rom_Bist) {
                    DEBUG_PRINT("mem_bridge ROM Bist error");
                    ERROR();
                }
            }
        }
        return 1;
    }
    return 0;
}

// check the membridge bist status of first block of ram for step 1
int boot_BistMemStep1(void)
{
    register UINT32 t;
    if (g_bootBistStatePtr->bistMemIdx < 1) {
        if (g_bootBistStatePtr->bistMemIdx == -1) {
            t = 0;
            // start membridge bist : rom
            hwp_memBridge->Rom_Bist = MEM_BRIDGE_CTRL_RUNNING;
        } else {
            t=hwp_memBridge->SRam_Bist;
            if ((t & MEM_BRIDGE_CTRL_RUNNING) == 0) {
                if (t != (MEM_BRIDGE_STATUS_OK | MEM_BRIDGE_BLOCK_SEL(g_bootBistStatePtr->bistMemIdx))) {
                    DEBUG_PRINT("mem_bridge RAM Bist error");
                    ERROR();
                }
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

void boot_BistStep1(void)
{
    int status;

#ifdef USE_TESTER_CHIP
    // Enable CS3 (tester chip)
    hwp_memBridge -> CS_Time_Write = CS3_TIME_SETTING;
    hwp_memBridge -> CS_Config[3].CS_Mode = CS3_MODE_SETTING;
    {u32 flush __attribute__ ((unused)) = hwp_memFifo->Flush;}
    // Configure iomux
    hwp_configRegs->GPIO_Mode &= ~(CFG_REGS_MODE_PIN_M_CS_3);
#endif

    g_bootBistStatePtr = (BOOT_BIST_STATE_T*) 0x81A0801C; // dma pattern and nexts 
    g_bootBistStatePtr->errorCount= 0;
    g_bootBistStatePtr->bistBBIdx =-1;
    g_bootBistStatePtr->bistSysIdx=-1;
    g_bootBistStatePtr->bistMemIdx=-1;

    // start membridge bist : 1st block + rom
    boot_BistMemScheduler();

    // start voc Bist
    hwp_vocCfg->BIST_Ctrl = VOC_CFG_BIST_RUN;

    // LPG cleared and as GPO mode: GPO 1 and 2
    //asm volatile("nop;\nboot_Bist_zob: nop;");
    hwp_gpio->gpo_clr = (1<<2) | (1<<0);

    // stall bcpu, out of reset and start bist of it's caches
    hwp_sysCtrl->BCpu_Dbg_BKP = SYS_CTRL_STALLED;
    hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_BCPU;

    // disable all XCPU caches : done in assembly before entering this function
    //hwp_xcpu->Cache_Control = XCPU_DCACHE_INHIBIT | XCPU_ICACHE_INHIBIT;
    //{u32 unused __attribute__ ((unused)) = hwp_xcpu->Cache_Control;}

    // Disable the reg protection
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    // Bypass the Pll to give a phy clock to usb (to go out of reset)
    hwp_sysCtrl->Pll_Ctrl =       SYS_CTRL_PLL_ENABLE_POWER_DOWN 
                                | SYS_CTRL_PLL_BYPASS_BYPASS 
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE 
                                | SYS_CTRL_PLL_LOCK_RESET_RESET;

    // Enable the reg protection
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    // Put camera module in BIST mode 
    hwp_camera->CTRL = CAMERA_BIST_MODE_BIST;    

    // Scheduler :
    // Each module bist test are launched in parallel
    // It checks the status of each bist in progress and launches the next one each time one is done 
    status=1;
    while (boot_BistMemStep1())
    {
        status = 0;
        // bcpu caches and tags + bbsram + tcu(soft)
        status += boot_BistBBScheduler();

        // xcpu caches and tags + usb + camera + gouda
        status += boot_BistSysScheduler();
    }
    
}

void boot_BistStep2(void)
{
    int status;
    g_bootBistStatePtr = (BOOT_BIST_STATE_T*) 0x81A0801C; // dma pattern and nexts 

    // Scheduler :
    // Each module bist test are launched in parallel
    // It checks the status of each bist in progress and launches the next one each time one is done 
    status=1;
    while (status)
    {
        status = 0;
        // bcpu caches and tags + bbsram + bbcp2 + tcu(soft)
        status += boot_BistBBScheduler();

        // xcpu caches and tags + usb + camera + gouda
        status += boot_BistSysScheduler();

        // mem bridge ram & rom
        status += boot_BistMemScheduler();
    }
    
    // wait finish
    while (hwp_vocCfg->BIST_Ctrl & 0x1);      
    // check voc bist status and crc value
    if( VOC_CFG_BIST_CRC(0xc001) != hwp_vocCfg->BIST_Ctrl) {
        DEBUG_PRINT("VoC Bist error");
        ERROR();
    }

    if (g_bootBistStatePtr->errorCount==0)
    {   
        SUCCESS();
    }

    while (1);

}
/******************************************************************************/


void boot_BistBCpu(void)
{
    b_bootBistBcpuEnd = 1;
    // test BBCP2
    if(boot_MarchCBBCp2()) {
        DEBUG_PRINT("bbcp2 march error");
        b_bootBistBcpuEnd = 10;
        ERROR();
    }
    // test TCU
    if(boot_MarchCTcu()) {
        DEBUG_PRINT("tcu march error");
        b_bootBistBcpuEnd = 10;
        ERROR();
    }
    b_bootBistBcpuEnd = 2;
    while (1);
}

/******************************************************************************/

#define MEM_READ_UNCACHE(addr)          ( *(volatile unsigned long *)((addr) | (0xa0000000)) )
#define MEM_WRITE_UNCACHE(addr, data)   ( *(volatile unsigned long *)((addr) | (0xa0000000)) ) = data

#define ZEROS       0xaaaaaaaa
#define ONES        0x55555555

#define PAT_A       0xaaaa
#define PAT_B       0x5555

#if 0 //debug

#define Stop(a,b,c) {\
        ct_setW0(a);\
        ct_setW0(b);\
        ct_setW1(c);\
        return 1;\
    }

#else
#define Stop(a,b,c) return 1
#endif


/******************************************************************************/
#include "tcu.h"
#define FORCE_ALL TCU_FORCELATCH_MASK

int boot_MarchCTcu(void)
{

    int i,k,Incr;
    unsigned int ReadVal_A, ReadVal_P, ReadExp, WriteVal, RdTime, WrTime;
    unsigned int ptr, EvId, Start;

    
    // setup gsm clocks:
    hwp_tcu->Cfg_Clk_Div= TCU_ENABLE_QBIT_ENABLED;

    // set both latch rams to 0
    hwp_tcu->Latch = TCU_CLEARPROGAREA;
    hwp_tcu->Latch = FORCE_ALL;
    hwp_tcu->Ctrl = TCU_LOAD_VAL(0x2aaa) | TCU_LOAD | TCU_ENABLE_ENABLED;
    
    for (k = 0 ; k<2; k++) {
        if ((k&0x1)==0) {
            /* ----------------------------------------------------- */
            // -- Going Down
            /* ----------------------------------------------------- */
            Incr = 4;
            Start = (unsigned int) (hwp_tcu->Event);
        } else {
            Incr = -4;
            Start = (unsigned int) (hwp_tcu->Event+NB_TCU_PROG_EVENTS-1);
        }
        ptr = Start;
        for (i=0; i<60; i++) {
            MEM_WRITE_UNCACHE(ptr,0x151555);
            ptr+=Incr;
        }
        hwp_tcu->Latch = FORCE_ALL;
    //######### one loop
        ptr = Start;
        ReadExp = 0x151555;
        WrTime = 0x2aaa;
        for (i=0; i<15; i++) {
            EvId = (2*i+1);
            WriteVal = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
        WrTime = 0x2aab;
        for (i=15; i<30; i++) {
            EvId = 2*(i-15);
            WriteVal = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
        WrTime = 0x2aac;
        for (i=30; i<45; i++) {
            EvId = 2*(i-30)+1;
            WriteVal = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
        WrTime = 0x2aad;
        for (i=45; i<60; i++) {
            EvId      = 2*(i-45);
            WriteVal  = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
    //#########
    //######### one loop
        ptr = Start;
        RdTime = 0x2aaa;
        WrTime = 0x1555;
        for (i=0; i<15; i++) {
            EvId      = 2*i+1;
            ReadExp   = (TCU_EVENT_TIME(RdTime) | TCU_EVENT_ID(EvId));
            WriteVal  = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
        RdTime = 0x2aab;
        WrTime = 0x1556;
        for (i=15; i<30; i++) {
            EvId      = 2*(i-15);
            ReadExp   = (TCU_EVENT_TIME(RdTime) | TCU_EVENT_ID(EvId));
            WriteVal  = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
        RdTime = 0x2aac;
        WrTime = 0x1557;
        for (i=30; i<45; i++) {
            EvId      = 2*(i-30)+1;
            ReadExp   = (TCU_EVENT_TIME(RdTime) | TCU_EVENT_ID(EvId));
            WriteVal  = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
        RdTime = 0x2aad;
        WrTime = 0x1558;
        for (i=45; i<60; i++) {
            EvId = 2*(i-45);
            ReadExp   = (TCU_EVENT_TIME(RdTime) | TCU_EVENT_ID(EvId));
            WriteVal  = (TCU_EVENT_TIME(WrTime) | TCU_EVENT_ID(EvId));
            ReadVal_P = MEM_READ_UNCACHE(ptr);
            MEM_WRITE_UNCACHE(ptr,WriteVal);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_DEBUG;
            ReadVal_A = MEM_READ_UNCACHE(ptr);
            hwp_tcu->Setup=TCU_DEBUG_ACTIVE_NORMAL;
            hwp_tcu->Latch = FORCE_ALL;
            if (ReadVal_A != ReadExp) Stop(ptr, ReadVal_A, ReadExp);
            if (ReadVal_P != ReadExp) Stop(ptr, ReadVal_P, ReadExp);
            ptr+=Incr;
        }
        hwp_tcu->Ctrl = TCU_LOAD_VAL(WrTime-1) | TCU_LOAD | TCU_ENABLE_ENABLED;
        while (hwp_tcu->Cur_Val < WrTime + 1);
        //#########
    }

    return 0;
}


/******************************************************************************/

