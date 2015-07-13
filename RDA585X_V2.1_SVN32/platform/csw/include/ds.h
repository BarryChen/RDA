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
/*      ds.h                                                                  */
/*                                                                            */
/* DESCRIPTION                                                                */
/*    Debug service declarations, constant definitions and macros             */
/*                                                                            */
/******************************************************************************/
#ifndef _DS_H_
#define _DS_H_
  
#ifdef __cplusplus
CPP_START
#endif

#include <ts.h>


UINT32 SRVAPI DS_FatalErr (
  PCSTR pszFileName,
  UINT32 nLineNum,
  PCSTR pszErrStr
); 

//
// Redefine the Debug API to trace in default level.
// 
#if defined(DEBUG)
#if defined(_WIN32)
#define DS_ASSERT(x) 
#define DS_FATAL_ERR(x) 
#define DS_CHECKHANDLE(handle) 
#else // _WIN32
#define DS_CHECKHANDLE(handle)  
#define DS_ASSERT(x) 
#endif
#else // DEBUG
#define DS_CHECKHANDLE(handle)      {	 }
#define DS_ASSERT(x)                {	 }
#endif // DEBUG

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
CPP_END
#endif

#endif // _H_
