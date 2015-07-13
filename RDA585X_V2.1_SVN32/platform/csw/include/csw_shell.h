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
/*      csw_shell.h                                                           */
/*                                                                            */
/* DESCRIPTION                                                                */
/*   Communication Framework constant definitions and macros.                 */
/*   See CSW developement Manual for more.                                    */
/*                                                                            */
/******************************************************************************/

#ifndef _CSW_SHELL_H_
#define _CSW_SHELL_H_

#include <cfw.h>

//
// Shell 
//

UINT32 SRVAPI SHL_EraseFlash(
    UINT8 nMode  // 0: FS flash area, 1: all flash area exclude code.
); 

#ifdef CFW_PBK_SYNC_VER
UINT32 SRVAPI SHL_SimAddPbkEntry (
  UINT8 nStorage,
  CFW_SIM_PBK_ENTRY_INFO* pEntryInfo,
  CFW_PBK_OUT_PARAM* pOutParam
);

UINT32 SRVAPI SHL_SimDeletePbkEntry (
  UINT8 nStorage,
  UINT8 nIndex,
  CFW_PBK_OUT_PARAM* pOutParam
);
#endif 

#endif // _H

