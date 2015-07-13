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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_perfmeter.c $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2010-09-02 16:02:28 +0800 (星期四, 02 九月 2010) $                     //   
//    $Revision: 1882 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///                                                                           //
///     @file hal_ebc.h                                                       //
///                                                                           //
///     This file contains the implementation of Garnet's              //
///     HAL EBC driver                                                        //
///                                                                           //
//////////////////////////////////////////////////////////////////////////////// 


#include "cs_types.h"

#include "chip_id.h"

#include "global_macros.h"
#include "sys_ahb_monitor.h"
#include "debug_port.h"
#include "mem_bridge.h"

#include "halp_debug.h"
#include "hal_perfmeter.h"

HAL_PERFMETER_TARGET_T g_hal_PerfMeterTarget=HAL_PERF_NOT_PROGRAMMED;
BOOL g_hal_PerfMeterSingleShot;


#if (CHIP_HAS_AHB_MONITOR == 1)
PUBLIC VOID hal_PerfMeterOpen(HAL_PERFMETER_CFG_T *cfg)
{
    g_hal_PerfMeterTarget = cfg->target;
    g_hal_PerfMeterSingleShot = cfg->singleShot;

    switch (g_hal_PerfMeterTarget)
    {
        case HAL_PERF_AHB_BUS_USAGE:
            hwp_sysAhbMonitor->Control=
                SYS_AHB_MONITOR_TIME_WINDOW_TW_16MC | SYS_AHB_MONITOR_ACCUMULATION_MODE_MAX |
                SYS_AHB_MONITOR_TARGET_SYS_BUS;
            break;
        case HAL_PERF_EBC_USAGE:
            hwp_sysAhbMonitor->Control=
                SYS_AHB_MONITOR_TIME_WINDOW_TW_16MC | 
                SYS_AHB_MONITOR_TARGET_SIGNAL_SELECT0;
            // Select nb cycles of EBC state machine not idle
            hwp_debugPort->Signal_Spy_Cfg=DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_EBC_BUSY | 
                DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_EBC_BUSY;
            hwp_memBridge->Monitor_Ctrl=MEM_BRIDGE_MONITOR_SEL_DATA_VS_EXTERNAL;
            break;
        case HAL_PERF_INT_RAMROM:
            hwp_sysAhbMonitor->Control=
                SYS_AHB_MONITOR_TIME_WINDOW_TW_16MC | SYS_AHB_MONITOR_ACCUMULATION_MODE_MAX |
                SYS_AHB_MONITOR_TARGET_SYS_MEM_INT;
            break;
        case HAL_PERF_ICACHE_MISS:
            hwp_debugPort->Signal_Spy_Cfg=DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_I_MISS;
            hwp_sysAhbMonitor->Control=
                SYS_AHB_MONITOR_TIME_WINDOW_TW_16MC | SYS_AHB_MONITOR_ACCUMULATION_MODE_MAX |
                SYS_AHB_MONITOR_TARGET_SIGNAL_SELECT0;
            break;
        case HAL_PERF_DCACHE_MISS:
            hwp_debugPort->Signal_Spy_Cfg=DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_D_MISS;
            hwp_sysAhbMonitor->Control=
                SYS_AHB_MONITOR_TIME_WINDOW_TW_16MC | SYS_AHB_MONITOR_ACCUMULATION_MODE_MAX |
                SYS_AHB_MONITOR_TARGET_SIGNAL_SELECT0;
            break;
        case HAL_PERF_I_FETCH:
            hwp_debugPort->Signal_Spy_Cfg=DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_I_FETCH;
            hwp_sysAhbMonitor->Control=
                SYS_AHB_MONITOR_TIME_WINDOW_TW_16MC | SYS_AHB_MONITOR_ACCUMULATION_MODE_MAX |
                SYS_AHB_MONITOR_TARGET_SIGNAL_SELECT0;
            break;
        case HAL_PERF_D_FETCH:
            hwp_debugPort->Signal_Spy_Cfg=DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_D_FETCH;
            hwp_sysAhbMonitor->Control=
                SYS_AHB_MONITOR_TIME_WINDOW_TW_16MC | SYS_AHB_MONITOR_ACCUMULATION_MODE_MAX |
                SYS_AHB_MONITOR_TARGET_SIGNAL_SELECT0;
            break;
        default:
            // Invalid choice
            HAL_TRACE(HAL_DBG_TRC,0,"Hal perfmeter: Invalid target %d",g_hal_PerfMeterTarget);
            hwp_sysAhbMonitor->Control=0;
            g_hal_PerfMeterTarget=HAL_PERF_NOT_PROGRAMMED;
            return;
    }
    if (cfg->singleShot)
    {
        hwp_sysAhbMonitor->Control|=SYS_AHB_MONITOR_ENABLE | SYS_AHB_MONITOR_RECORD_SINGLE_SHOT;
    }
    else
    {
        hwp_sysAhbMonitor->Control|=SYS_AHB_MONITOR_ENABLE | SYS_AHB_MONITOR_RECORD_MULTI_SHOT;
    }
}

PUBLIC VOID hal_PerfMeterClose(VOID)
{
    hwp_sysAhbMonitor->Control=0;
    g_hal_PerfMeterTarget=HAL_PERF_NOT_PROGRAMMED;
}

PUBLIC UINT32 hal_PerfMeterGetStatistics(VOID)
{
    UINT32 usage = hwp_sysAhbMonitor->Use;
    UINT32 HAL_POSSIBLY_UNUSED accessCount = hwp_sysAhbMonitor->Access_Count;
    UINT32 HAL_POSSIBLY_UNUSED latency = hwp_sysAhbMonitor->Latency;
    UINT32 stat;
    
    if (g_hal_PerfMeterTarget!=HAL_PERF_NOT_PROGRAMMED)
    {
        stat=usage*100/16777216;
        if ((hwp_sysAhbMonitor->Control & SYS_AHB_MONITOR_ENABLE) && (g_hal_PerfMeterSingleShot))
        {
            // Result not ready
            return PERFMETER_NOT_READY;
        }
        else
        {
            // Result is ready
            switch (g_hal_PerfMeterTarget)
            {
                case HAL_PERF_AHB_BUS_USAGE:
                    HAL_TRACE(HAL_DBG_TRC,0,"Bus is used %d%% of the time",stat);
                    break;
                case HAL_PERF_EBC_USAGE:
                    HAL_TRACE(HAL_DBG_TRC,0,"External Memory Bus is used %d%% of the time",stat);
                    break;
                case HAL_PERF_INT_RAMROM:
                    HAL_TRACE(HAL_DBG_TRC,0,"Internal Ram/Rom is used %d%% of the time",stat);
                    // TODO missing mask and shift in XML description
                    // Use it when available
                    HAL_TRACE(HAL_DBG_TRC,0,"Internal Ram/Rom max access time %d cycles over %d accesses",
                            latency & 0xffffff, accessCount);
                    break;
                case HAL_PERF_ICACHE_MISS:
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU is in Icache miss %d%% of the time",stat);
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU ICache miss max access time %d cycles over %d accesses",
                            latency & 0xffffff, accessCount);
                    break;
                case HAL_PERF_DCACHE_MISS:
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU is in Dcache miss %d%% of the time",stat);
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU DCache miss max access time %d cycles over %d accesses",
                            latency & 0xffffff, accessCount);
                    break;
                case HAL_PERF_I_FETCH:
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU is Fetching Instructions %d%% of the time",stat);
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU Instruction fetch max access time %d cycles over %d accesses",
                            latency & 0xffffff, accessCount);
                    break;
                case HAL_PERF_D_FETCH:
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU is Fetching Datas %d%% of the time",stat);
                    HAL_TRACE(HAL_DBG_TRC,0,"XCPU Data fetch max access time %d cycles over %d accesses",
                            latency & 0xffffff, accessCount);
                    break;
                default:
                    stat=PERFMETER_NOT_READY;
                    HAL_TRACE(HAL_DBG_TRC,0,"Unsupported perfmeter target");
                    break;
        
            }
            // Relaunch
            hwp_sysAhbMonitor->Control|=SYS_AHB_MONITOR_ENABLE;
        
            return stat;
        }
    }
    else
    {
        return PERFMETER_NOT_READY;
    }
}
#else
PUBLIC VOID hal_PerfMeterOpen(HAL_PERFMETER_CFG_T *cfg)
{
    HAL_TRACE(HAL_DBG_TRC,0,"hal_perfMeterOpen: Perfmeter not supported on this platform");
}
PUBLIC VOID hal_PerfMeterClose(VOID)
{
    HAL_TRACE(HAL_DBG_TRC,0,"hal_perfMeterClose: Perfmeter not supported on this platform");
}
PUBLIC UINT32 hal_PerfMeterGetStatistics(VOID)
{
    HAL_TRACE(HAL_DBG_TRC,0,"hal_perfGetStatistics: Perfmeter not supported on this platform");
    return PERFMETER_NOT_READY; 
}
#endif



