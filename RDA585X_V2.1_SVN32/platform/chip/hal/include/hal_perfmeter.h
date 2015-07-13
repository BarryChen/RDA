/////////////////////////////////////////////////////////////////////////////////
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
//                                                                            
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_perfmeter.h $
//    $Author: admin $                                                       
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                    
//    $Revision: 269 $                                                      
//                                                                            
////////////////////////////////////////////////////////////////////////////////
//                                                                            
/// @file hal_debug.h
/// This file contains  perfmeter functionalities API
//                                                                            
////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_PERF_H_
#define _HAL_PERF_H_

#include "cs_types.h"

/// @defgroup hal_ahb_monitor HAL AHB monitor driver
/// This chapter describes the performance monitoring capabilities of the AHB monitor
///
/// @par Description
/// The chip includes 2 bus monitors able to gather statistics about the bus usage but 
/// also some specific module usage (Cpu, Memory controler)
/// Using these 2 monitors, you can get human readable information through the trace
/// about the system usage.
///
/// @{

typedef enum
{
/// Default target when the perfmeter is not opened
/// In that mode, calls to hal_PerfMeterGetStatistics always return PERFMETER_NOT_READY
    HAL_PERF_NOT_PROGRAMMED,
/// Monitors the percentage of cycles where the internal AHB bus is not in Idle
    HAL_PERF_AHB_BUS_USAGE,
/// Monitors the percentage of cycles where the external memory bus is not in Idle
    HAL_PERF_EBC_USAGE,
/// Monitors the percentage of cycles where the internal memory (Ram/Rom) is used
    HAL_PERF_INT_RAMROM,
/// Monitors the percentage of the time where the Xcpu is in Icache miss
    HAL_PERF_ICACHE_MISS,
/// Monitors the percentage of the time where the Xcpu is in Dcache miss
    HAL_PERF_DCACHE_MISS,
/// Monitors the percentage of the time where the Xcpu is Fetching Instructions
    HAL_PERF_I_FETCH,
/// Monitors the percentage of the time where the Xcpu is Fetching Data
    HAL_PERF_D_FETCH
} HAL_PERFMETER_TARGET_T;

typedef struct
{
/// When TRUE, do one measure and stop, when FALSE, measure continuously
    BOOL singleShot;
/// Parameter to monitor
    HAL_PERFMETER_TARGET_T target;
} HAL_PERFMETER_CFG_T;

/// 
#define PERFMETER_NOT_READY 0xff

// ============================================================================
// hal_PerfMeterOpen
// ----------------------------------------------------------------------------
/// This function is called to configure and enable the monitoring
/// Monitoring is done over 16M system clock cyles
/// @param cfg pointer on the configuration structure
// ============================================================================
PUBLIC VOID hal_PerfMeterOpen(HAL_PERFMETER_CFG_T *cfg);

// ============================================================================
// hal_PerfMeterClose
// ----------------------------------------------------------------------------
/// This function is called to disable the monitoring
// ============================================================================
PUBLIC VOID hal_PerfMeterClose(VOID);

// ============================================================================
// hal_PerfMeterGetStatistics
// ----------------------------------------------------------------------------
/// This function is called to display the statistics through the trace 
/// (on level HAL_DBG_TRC). 
/// In Single shot mode, this will automatically relaunch a new measure
/// @return percentage measured for the selected parameter 
/// returns PERFMETER_NOT_READY in single shot mode when the measure is not finished
// ============================================================================
PUBLIC UINT32 hal_PerfMeterGetStatistics(VOID);

/// @}
#endif // _HAL_PERF_H_
