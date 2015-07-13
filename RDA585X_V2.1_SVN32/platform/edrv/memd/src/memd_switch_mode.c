////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2009, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/memd/src/memd_switch_mode.c $
//	$Author: huazeng $
//	$Date: 2010-08-23 13:47:34 +0800 (星期一, 23 八月 2010) $
//	$Revision: 1532 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file memd_switch_mode.c                                                  //
/// That file implements the function to switch the flash into a RAM mode (where
/// it behaves like a RAM), for Flash model which are not the Romulator (Ie for
/// which this is not possible)
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"

#include "memd_m.h"
#include "memdp_debug.h"
#include "memdp.h"

#ifndef MEMD_USE_ROMULATOR 

// =============================================================================
// memd_FlashRomulatorSetRamMode
// -----------------------------------------------------------------------------
/// Simili 'Set the  in RAM mode'. Always return \c FALSE and does nothing.
/// This function is only effective when using a romulator device.
///
/// When the romualtor is in RAM mode, it can be accessed as a RAM, that is to 
/// say read and written without sending any command, but through direct 
/// addressing.
///
/// @param  enable Enable or disable the RAM mode.
/// @return FALSE.
// =============================================================================
PUBLIC BOOL memd_FlashRomulatorSetRamMode(BOOL enable)
{
    MEMD_TRACE(MEMD_WARN_TRC, 0, "MEMD: Ram Mode Feature not available on a combo.");
    return FALSE;
}

#endif // MEMD_USE_ROMULATOR not defined.

