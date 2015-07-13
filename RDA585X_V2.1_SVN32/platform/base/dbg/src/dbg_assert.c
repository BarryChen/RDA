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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/base/dbg/src/dbg_assert.c $
//  $Author: admin $
//  $Date: 2010-07-07 20:26:37 +0800 (星期三, 07 七月 2010) $
//  $Revision: 268 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file dbg_assert.c
/// Implement the assert.
//
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"
#include "dbg.h"
#include "stdarg.h"
#include "hal_debug.h"

// =============================================================================
// dbg_AssertUtil
// -----------------------------------------------------------------------------
/// Utilitary function to define the assert.
/// This function is used in the public macro that implements the minimal side-
/// effect assert.
/// @param format String format a-la printf, displayed in the trace.
// =============================================================================
PUBLIC VOID dbg_AssertUtil(CONST CHAR* format, ...)
{
    va_list args;
    va_start(args, format);
    // Just a HAL wrapper.
    hal_DbgAssert(format, args);
    va_end(args);
}
