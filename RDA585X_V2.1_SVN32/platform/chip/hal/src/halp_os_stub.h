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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_os_stub.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// 
///     @file hal_os_stub.h
/// 
///     That file is the header permetting access to 
///     the OS function in case there is a OS, and smoothing 
///     things in case there's not.
///     That file is deprecated and fated to disappear.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef  _HALP_OS_STUB_H_
#define  _HALP_OS_STUB_H_

// Then there's an OS somewhere and those
// functions exist
extern void sxr_EnterScSchedule();
extern void sxr_ExitScSchedule();

#endif // _HALP_OS_STUB_H_
