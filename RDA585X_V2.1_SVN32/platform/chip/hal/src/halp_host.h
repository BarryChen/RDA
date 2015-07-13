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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_host.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_host.h                                                         //
/// That file provides the private API for the host monitor and host client.  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _HALP_HOST_H_
#define _HALP_HOST_H_

#include "cs_types.h"



// =============================================================================
// hal_HstMonitor
// -----------------------------------------------------------------------------
/// Main host monitor function. Read the command passed to the platform through
/// the Host port and call the host command handler if appropriate.
/// It read the H2P register to execute commands
/// until the Exit command is received (BOOT_HST_MONITOR_END_CMD).
// =============================================================================
PROTECTED VOID hal_HstMonitor(VOID);



// =============================================================================
// hal_HstMonitorX
// -----------------------------------------------------------------------------
/// That function execute the function pointed by the execution context
/// #hal_HstMonitorXCtx.
// =============================================================================
PROTECTED  VOID hal_HstMonitorX(VOID); 




// =============================================================================
// hal_HstRamClientIdle
// -----------------------------------------------------------------------------
/// Read the H2P exchange register to retrieve host commands. If any supported
/// one found (type #BOOT_HST_CMD_T), it is executed during the idle task.
// =============================================================================
PROTECTED VOID hal_HstRamClientIdle(VOID);

// =============================================================================
// hal_HstRamClientIrq
// -----------------------------------------------------------------------------
/// Executed on IRQ from the Host. It reads the H2P exchange register to retrieve
/// host commands. If any supported one is found (type #BOOT_HST_CMD_T), it is
/// executed immediatly. Takes care of Critical Section breaking and coherence
/// etc ...
// =============================================================================
PROTECTED VOID hal_HstRamClientIrq(VOID);

// =============================================================================
// hal_HstSleep
// -----------------------------------------------------------------------------
/// Release the Host Clock Resource so system can go to low power
// =============================================================================
PROTECTED VOID hal_HstSleep(VOID);

// =============================================================================
// hal_HstWakeup
// -----------------------------------------------------------------------------
/// Set the Host Clock Resource as active.  Request 26M clock.
// =============================================================================
PROTECTED VOID hal_HstWakeup(VOID);


#endif // _HALP_HOST_H_
