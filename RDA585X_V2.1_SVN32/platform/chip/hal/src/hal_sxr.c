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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_sxr.c $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_sxr.c
///     This file defines the low level functions for the OS
//                                                                            //
////////////////////////////////////////////////////////////////////////////////




#include "cs_types.h"


#include "hal_debug.h"
#include "halp_debug.h"
#include "hal_debugpaltrc.h"

// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================






// =============================================================================
// hal_SoftAbort
// -----------------------------------------------------------------------------
/// FIXME
// =============================================================================
PUBLIC VOID hal_SoftAbort(VOID)
{
    HAL_PROFILE_PULSE(XCPU_ERROR);
    hal_DbgFatalTrigger(0);
#ifndef PAL_NO_DBGTRC
    hal_DbgPalFlushTrace ();
#endif
}




