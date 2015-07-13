////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2008, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/boot/gallite/src/bootp_debug.h $
//  $Author: admin $
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $
//  $Revision: 269 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file bootp_debug.h
///
/// Debug features to be used only inside the module (protected).
/// Is this file is used for all the debug needs of the module, it will be
/// easier to disable those debug features for the whole module (by changing
/// the value of a define while compiling).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BOOTP_DEBUG_H_
#define _BOOTP_DEBUG_H_

#include "cs_types.h"

#include "debug_port.h"

#include "boot_profile_codes.h"

#include "hal_debug.h"


// =============================================================================
// MACROS                                                                       
// =============================================================================

#define CPEXITFLAG 0x8000

// =============================================================================
//  BOOT_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define BOOT_PROFILE_PULSE(pulseName)                                   \
do                                                                      \
{                                                                       \
    hwp_debugPort->Pxts_tag[HAL_DBG_PXTS_BOOT] = (CP_ ## pulseName);    \
}                                                                       \
while(0)

// =============================================================================
//  BOOT_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define BOOT_PROFILE_FUNCTION_ENTER(pulseName)                          \
do                                                                      \
{                                                                       \
    hwp_debugPort->Pxts_tag[HAL_DBG_PXTS_BOOT] = (CP_ ## pulseName);    \
}                                                                       \
while(0)

// =============================================================================
//  BOOT_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define BOOT_PROFILE_FUNCTION_EXIT(pulseName)                           \
do                                                                      \
{                                                                       \
    hwp_debugPort->Pxts_tag[HAL_DBG_PXTS_BOOT] = (CP_ ## pulseName | CPEXITFLAG);\
}                                                                       \
while(0)


// =============================================================================
// TYPES                                                                        
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================


#endif // _BOOTP_DEBUG_H_
