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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_pwm.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_pwm.h                                                          //
/// That file provides the private API for the PWM driver.                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _HALP_PWM_H_
#define _HALP_PWM_H_

#include "cs_types.h"
#include "hal_sys.h"




// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================
// =============================================================================
// HAL_PWM_BASE_DIVIDER
// -----------------------------------------------------------------------------
/// This is the base divider used to calculate the PWM frequency from a 13MHZ
/// system clock. This value allow us to keep a constant PWM clock (309523 Hz)
/// whatever the system frequency is --> the done is kept at the same note
/// through system clock changes.
// =============================================================================
#define HAL_PWM_BASE_DIVIDER    32



// =============================================================================
// HAL_PWM_FREQ
// -----------------------------------------------------------------------------
/// This is the value of the PWM clock rate, using the provided base divider.
// =============================================================================
#define HAL_PWM_FREQ   (13000000/HAL_PWM_BASE_DIVIDER)



// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//  

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_PwmResourceMgmt
// -----------------------------------------------------------------------------
/// Checks if any components in the PWM module are active and requests or
/// releases the resource.  The return value is TRUE if the resource is
/// active and FALSE otherwise.
// =============================================================================
PROTECTED BOOL hal_PwmResourceMgmt(VOID);

// =============================================================================
// hal_PwlSetGlowing
// -----------------------------------------------------------------------------
/// Set which pin is the glowing pwl
// =============================================================================
PROTECTED VOID hal_PwlSetGlowing(HAL_PWL_ID_T id);



#endif // _HALP_PWM_H_



