/******************************************************************************/
/*                                                                            */
/*              Copyright (C) 2005, Coolsand Technologies, Inc.               */
/*                            All Rights Reserved                             */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* This source code is property of Coolsand. The information contained in     */
/* this file is confidential. Distribution, reproduction, as well as          */
/* exploitation,or transmisison of any content of this file is not            */
/* allowed except if expressly permitted.                                     */
/* Infringements result in damage claims!                                     */
/*                                                                            */
/* FILE NAME                                                                  */
/*      cswtype.h                                                             */
/*                                                                            */
/* DESCRIPTION                                                                */
/*      all basic types or constant definitions on CSW.                       */
/*                                                                            */
/******************************************************************************/

//#define CFW_PBK_SYNC_VER  

#if !defined(__CSWTYPE_H__)
#define __CSWTYPE_H__

#define HAVE_TM_MODULE    1
#define HAVE_LIBC_MALLOC  0
#include "cs_types.h"

// CSW specific types
typedef UINT16              RESID;

typedef HANDLE              HRES;
typedef UINT32              EVID;

typedef UINT32              EVPARAM;
typedef UINT32              HAO;

/*function point type*/
typedef BOOL (*PFN_BOOL)(VOID);
typedef VOID (*PTASK_ENTRY)(PVOID pParameter);

#endif // _H_

