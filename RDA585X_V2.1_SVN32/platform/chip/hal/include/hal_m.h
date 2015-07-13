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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_m.h $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2011-11-17 15:09:11 +0800 (周四, 17 十一月 2011) $                     //   
//    $Revision: 11922 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_m.h                                                             //
/// That file This file contains HAL API..                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_M_H_
#define _HAL_M_H_

#warning "Are you sure you wanted to include hal.h and not a hal_module.h ?"

/// @defgroup hal HAL Mainpage
/// @{
/// 
/// Hardware Abstraction Layer (HAL) API description.
/// @author Coolsand Technologies, Inc. 
/// @version 
/// @date   
/// 
/// @par Purpose
/// 
/// This document describes the characteristics of the chip's modules and how to
/// use them via its Hardware Abstraction Layer (HAL) API.
/// 
/// @par Module List
/// 
/// For a list of modules, please click on the Modules tab above.
/// .
/// The debug features rely the following module:
/// - @ref debugPort
/// - @ref debugExl 
/// - @ref debugPxts
/// - @ref signalSpy
/// - @ref error_handling
/// - @ref hwBkpt
/// - @ref pageProtection
/// - @ref palTrace
/// - @ref map
/// 

#include "hal_aif.h"
#include "hal_ana.h"
#include "hal_boot.h"
#include "hal_cipher.h"
#include "hal_comregs.h"
#include "hal_config.h"
#include "hal_debug.h"
#include "hal_debugpaltrc.h"
#include "hal_dma.h"
#include "hal_ebc.h"
#include "hal_error.h"
#include "hal_fint.h"
#include "hal_gpadc.h"
#include "hal_gpio.h"
#include "hal_host.h"
#include "hal_i2c.h"
#include "hal_key.h"
#include "hal_lps.h"
#include "hal_mem_map.h"
#include "hal_padac.h"
#include "hal_pwm.h"
#include "hal_rfspi.h"
#include "hal_sim.h"
#include "hal_speech.h"
#include "hal_spi.h"
#include "hal_sys.h"
#include "hal_tcu.h"
#include "hal_timers.h"
#include "hal_trace.h"
#include "hal_uart.h"
#include "hal_voc.h"
#include "hal_map_engine.h"
#include "hal_overlay.h"


/// @}

#endif //  _HAL_M_H_ 



