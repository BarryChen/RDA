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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_calendar.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_calendar.h
/// That file contains the calendar private header.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _HALP_CALENDAR_H_
#define _HALP_CALENDAR_H_

// =============================================================================
// hal_TimRtcIrqHandler
// -----------------------------------------------------------------------------
/// This function is the IRQ handler for the IRQ module called when a RTC/calendar
/// IRQ occurs. It clears the IRQ and calls the user handler if available.
// =============================================================================
PROTECTED VOID hal_TimRtcIrqHandler(UINT8 interruptId);



// =============================================================================
// hal_TimRtcAlarmReached
// -----------------------------------------------------------------------------
/// @return \c TRUE if the Alarm has been reached.
// =============================================================================
PROTECTED BOOL hal_TimRtcAlarmReached(VOID);


#endif // _HALP_CALENDAR_H_

