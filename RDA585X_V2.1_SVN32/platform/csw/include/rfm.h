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
/*      rfm.h                                                                 */
/*                                                                            */
/* DESCRIPTION                                                                */
/*      RFM interface definitions on CSW.                                     */
/*                                                                            */
/******************************************************************************/

#ifndef __RFM_H__
#define __RFM_H__

typedef HANDLE  HDATA;

#define RFM_FLASH_RES   0x00
#define RFM_FLASH_REG   0x01
#define RFM_FLASH_DB    0x02
#define RFM_FLASH_PBK   0x03
#define RFM_FLASH_FFS   0x04

#define RFM_LANG_EN     0x01
#define RFM_LANG_S_CN   0x02

#ifndef MF32_NO_RFM_RH
#ifndef _GLOBAL_RH_
#define _GLOBAL_RH_
#include <sys_default.rh>
#endif 

#ifndef _RFM_RH_ 
#define _RFM_RH_ 
#include <ctl.rh> 
#endif 

#endif // MF32_NO_RFM_RH

BOOL SRVAPI RFM_GetFlashRegion(
    RESID nSpaceId,
    UINT32* pAddrOffset,
    UINT32* pSize
);

BOOL SRVAPI RFM_GetRamRegion(
    RESID nSpaceId,
    UINT32* pAddrOffset,
    UINT32* pSize
);

HRES SRVAPI RFM_LoadData(
    RESID nDataFileId
);

HRES SRVAPI RFM_LoadSysResource(
    RESID nResId
);

#ifdef _WIN32
#pragma warning(disable:4214)
#endif 

typedef struct _RFM_FILE_INFO {
	UINT32  mode     :8;
	UINT32  size     :24;
	PVOID pFileData;
}RFM_FILE_INFO;

typedef struct _RFM_BASIC_CTL_HRD {
    UINT8  type;
    UINT8  size;
    PVOID  pData;
}RFM_BASIC_CTL_HRD;

BOOL SRVAPI RFM_GetControlHrd(
    UINT32 hControlRes, 
    RFM_BASIC_CTL_HRD* pCtlHrdInfo
);

BOOL SRVAPI RFM_GetFileInfo(
    HRES hResFile,
    UINT8 nFileIndex,
    RFM_FILE_INFO *pFileInfo
);

PCSTR SRVAPI RFM_GetGeneralString(
    HRES hResFile,
    UINT8 nStringIndex
);

#endif // _H_

