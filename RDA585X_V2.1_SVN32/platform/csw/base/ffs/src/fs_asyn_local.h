////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: fs_ansy_local.h
//
// DESCRIPTION:
//   This file to define asynchronous access file local structures,local functions etc..
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//  Caoxh 	         2009.09.28      Create
////////////////////////////////////////////////////////////////////////////////

#ifndef _FS_ASYN_LOCAL_H_
#define _FS_ASYN_LOCAL_H_

typedef struct  _FS_ASYN_PARAM
{
	UINT32 Fd;
	UINT32 pBuff;
	UINT32 uSize;
	UINT32 pCallback;
}FS_ASYN_PARAM;

typedef struct _FS_ASYN_EVENT 
{
  UINT32 nEventId;
  UINT32 nParam1;
 // UINT32 nParam2;//for delete nParam2
//  UINT32 nParam3;//for delete nParam3
} FS_ASYN_EVENT;

VOID FS_AsynReadPrc(UINT32 pParam1);
VOID FS_AsynWritePrc(UINT32 pParam1);


#endif






