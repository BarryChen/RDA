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
//  $HeadURL: http://10.10.100.14/svn/developing/modem2G/trunk/application/coolmmi/media/image/src/imagep_debug.h $
//	$Author: zhangl $
//	$Date: 2009-04-07 10:20:53  $
//	$Revision: $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file imagep_debug.h
/// Debug features to be used only inside the module (protected).
/// If this file is used for all the debug needs of the module, it will be
/// easier to disable those debug features for the whole module (by changing
/// the value of a define while compiling).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef  _IMAGEP_DEBUG_H_
#define  _IMAGEP_DEBUG_H_

#ifdef MMI_ON_HARDWARE_P
#include "hal_debug.h"
#endif

// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
// =============================================================================
//#define IMAGE_PROFILING
#ifdef IMAGE_PROFILING

#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000
// =============================================================================
//  HAL_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define IMGDP_ENTRY(id)         hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, (id) & CPMASK)

// =============================================================================
//  HAL_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define IMGDP_EXIT(id)          hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, (id) & CPMASK));

#else // IMAGE_PROFILING

#define IMGDP_ENTRY(id)
#define IMGDP_EXIT(id)

#endif // IMAGE_PROFILING



#endif //_IMAGEP_DEBUG_H_


