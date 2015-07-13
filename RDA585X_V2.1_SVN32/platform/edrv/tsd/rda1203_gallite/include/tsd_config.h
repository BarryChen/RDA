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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/tsd/rda1203_gallite/include/tsd_config.h $ //
//	$Author: huazeng $                                                        // 
//	$Date: 2010-07-07 20:01:15 +0800 (星期三, 07 七月 2010) $                     //   
//	$Revision: 263 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file lcdd_config.h                                                       //
/// That file describes the configuration of the structure used to set up     //
/// the AU driver stored in TGT.                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TSD_CONFIG_H_
#define _TSD_CONFIG_H_

#include "cs_types.h"
#include "hal_gpio.h"

// =============================================================================
// TSD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type TSD_CONFIG_T is defined in tsd_m.h using the structure
/// #TSD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include tsd_config.h before tsd_m.h !
// =============================================================================
#define TSD_CONFIG_STRUCT_T TSD_CONFIG_OPAL_STRUCT_T

#include "tsd_m.h"



// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// TSD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the TSD driver CS
// =============================================================================
struct TSD_CONFIG_OPAL_STRUCT_T
{
    /// GPIO Pin used for PEN IRQ Detection
    HAL_GPIO_GPIO_ID_T      penGpio;
    /// Timier value for debouncing check (in ticks) after pen Irq
    /// This is the smaller time unit used in the driver.
    /// #downPeriod, #upPeriod and TSD_REPORTING_MODE_T.repetitionPeriod
    /// are multiple of this value.
    UINT32 debounceTime;
    /// Period of debouncing time ( in # checks ) after what we consider the pen down.
    UINT16 downPeriod;
    /// Period of debouncing time ( in # checks ) after what we consider the pen up.
    UINT16 upPeriod;
    /// Maximum difference between two samples of the same point.
    UINT32 maxError;
    /// Key count
    UINT8 keyCount;
    /// volt when first key press
    UINT16 minVolt;
    /// volt when first key press
    UINT16 maxVolt;
};



#endif // _TSD_CONFIG_H_


