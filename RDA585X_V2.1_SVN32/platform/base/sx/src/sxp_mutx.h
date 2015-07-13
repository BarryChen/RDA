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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/base/sx/src/sxp_mutx.h $
//	$Author: admin $
//	$Date: 2010-07-07 20:26:37 +0800 (星期三, 07 七月 2010) $
//	$Revision: 268 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file sxp_mutx.h
/// Systeme executif task mutual exclusion private header file.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __SXP_MUTX_H__
#define __SXP_MUTX_H__

#include "sxs_type.h"
#include "sxr_cnf.h"


#define SXR_NO_MUTEX        0xFF
#define SXR_MUTEX_ALLOCATED 0xFE

typedef struct
{
    u8 SemaId;
    u8 TaskId;
    u8 Count;
    /// chaining for allocation
    u8 Next;
    /// debug info
    u32 CallerAdd;
} Mutex_t;


#ifdef __SXR_MUTX_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif
DefExtern struct
{
    Mutex_t    Queue    [SXR_NB_MAX_MUTEX];
    u8         IdxFree;
    u8         Nb;
} sxr_Mutex;

#undef DefExtern

#ifdef SX_NO_ASSERT
#define SX_MUTX_ASSERT(BOOL, format, ...)
#else
#define SX_MUTX_ASSERT(BOOL, format, ...) \
    if (!(BOOL)) { \
        sxr_CheckMutex(); \
        hal_DbgAssert(format, ##__VA_ARGS__); \
    }
#endif

#endif

