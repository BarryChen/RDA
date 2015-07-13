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
//
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_debug.h $
//    $Author: yanshengzhang $
//    $Date: 2011-06-30 14:50:40 +0800 (星期四, 30 六月 2011) $
//    $Revision: 8645 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file halp_debug.h
/// Debug features to be used only inside the module (protected).
/// If this file is used for all the debug needs of the module, it will be
/// easier to disable those debug features for the whole module (by changing
/// the value of a define while compiling).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef  _HALP_DEBUG_H_
#define  _HALP_DEBUG_H_

#include "hal_debug.h"
#include "hal_profile_codes.h"
#include "sxs_io.h"

#define HAL_POSSIBLY_UNUSED __attribute__((unused))

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// HAL is composed of several modules implementing several functionalities.
/// Each functionality that output traces will use one of the predefined 
/// trace levels below.
// =============================================================================
/// Important trace that can be enabled all the time (low rate)
#define HAL_WARN_TRC        (_HAL | TLEVEL(1))
/// TIMER specific trace
#define HAL_TIMER_TRC       (_HAL | TLEVEL(2))
/// IO (GPIO, KEYPAD, I2C, PWM, EBC) specific trace
#define HAL_IO_TRC          (_HAL | TLEVEL(3))
/// RF and TCU specific trace
#define HAL_RF_TRC          (_HAL | TLEVEL(4))
/// AUDIO (and speech) specific trace
#define HAL_AUDIO_TRC       (_HAL | TLEVEL(5))
/// LCD specific trace
#define HAL_LCD_TRC         (_HAL | TLEVEL(6))
/// SDMMC specific trace
#define HAL_SDMMC_TRC       (_HAL | TLEVEL(7))
/// CAMERA specific trace
#define HAL_CAMERA_TRC      (_HAL | TLEVEL(8))
/// SPI specific trace
#define HAL_SPI_TRC         (_HAL | TLEVEL(9))
/// UART (and Host) specific trace
#define HAL_UART_TRC        (_HAL | TLEVEL(10))
/// USB specific trace
#define HAL_USB_TRC         (_HAL | TLEVEL(11))
/// VOC specific trace
#define HAL_VOC_TRC         (_HAL | TLEVEL(12))
/// DMA and IFC specific trace
#define HAL_DMA_TRC         (_HAL | TLEVEL(13))
/// SIM specific trace
#define HAL_SIM_TRC         (_HAL | TLEVEL(14))
/// LPS and system setup clock specific trace
#define HAL_LPS_TRC         (_HAL | TLEVEL(15))
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define HAL_DBG_TRC         (_HAL | TLEVEL(16))


// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
#ifdef  HAL_NO_PRINTF
#define HAL_TRACE(level, tsmap, fmt, ...)
#else
#define HAL_TRACE(level, tstmap, format, ...) \
     hal_DbgTrace(_HAL | level, tstmap, format, ##__VA_ARGS__)
#endif



#ifdef HAL_NO_ASSERT
#define HAL_ASSERT(condition, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define HAL_ASSERT(condition, format, ...)      \
    if (UNLIKELY(!(condition))) {               \
        hal_DbgAssert(format, ##__VA_ARGS__);   \
    }
#endif



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with hal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef HAL_PROFILING


// =============================================================================
//  HAL_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define HAL_PROFILE_PULSE(pulseName)  \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_HAL, (CP_ ## pulseName))


// =============================================================================
//  HAL_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_WINDOW_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_WINDOW_ENTER(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_SPI_ACTIVATION
// -----------------------------------------------------------------------------
/// Use this macro to profile the activation of SPI Chip Select.
/// (There is up to 4 CS per SPI.)
// =============================================================================
#define HAL_PROFILE_SPI_ACTIVATION(id, csNum) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_HAL, (CP_ACTIVATE_SPI1_CS0 + id*4+ csNum))

// =============================================================================
//  HAL_PROFILE_SPI_DEACTIVATION
// -----------------------------------------------------------------------------
/// Use this macro to profile the deactivation of SPI Chip Select.
/// (There is up to 4 CS per SPI.)
// =============================================================================
#define HAL_PROFILE_SPI_DEACTIVATION(id, csNum) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_HAL, (CP_ACTIVATE_SPI1_CS0 + id*4+ csNum))

#else // HAL_PROFILING

#define HAL_PROFILE_FUNCTION_ENTER(eventName)
#define HAL_PROFILE_FUNCTION_EXIT(eventName)
#define HAL_PROFILE_WINDOW_ENTER(eventName)
#define HAL_PROFILE_WINDOW_EXIT(eventName)
#define HAL_PROFILE_PULSE(pulseName)
#define HAL_PROFILE_SPI_ACTIVATION(id, csNum)
#define HAL_PROFILE_SPI_DEACTIVATION(id, csNum)

#endif // HAL_PROFILING



#endif //_HALP_DEBUG_H_


