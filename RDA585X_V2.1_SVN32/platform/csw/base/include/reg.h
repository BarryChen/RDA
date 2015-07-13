/******************************************************************************/
/*              Copyright (C) 2006~2007, CoolSand Technologies, Inc.          */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    reg.h                                                         */
/*                                                                            */
/* Description:                                                               */
/*    REG service declarations, constant definitions and macros               */
/******************************************************************************/

#ifndef _REG_H_
#define _REG_H_

//
// Default Reg Key handle.
//
#define REG_HKEY_SYSTEM_ROOT      1
#define REG_HKEY_LOCAL_MACHINE  2
#define REG_HKEY_CURRENT_COMM   3
#define REG_HKEY_CURRENT_USERS  4

// 
// Data type 
//
#define REG_TYPE_STRING  0xa1
#define REG_TYPE_INT8      0xb2
#define REG_TYPE_INT32    0xc3
#define REG_TYPE_BINARY  0xd4

// Value name max length.
#define REG_VALUENAME_LEN_MAX      20

// Sub Key name max length    
#define REG_SUBKEY_LEN_MAX             16

// Data max length.
#define REG_VALUE_LEN_MAX               64

INT32 REG_CloseKey(HANDLE hKey);

INT32  REG_OpenKey(HANDLE hKey, PCSTR pcSubKey, HANDLE* phKey);

INT32 REG_SetValue(HANDLE hKey, PCSTR pcValueName, UINT8 iType, PVOID pvData, UINT8 iDataSize);


INT32 REG_GetValue(HANDLE hKey, PCSTR pcValueName, UINT8 *pcType, PVOID pvData, UINT8 *pDataSize);

INT32 REG_QueryDefaultValue(HANDLE hRootKey, PCSTR pcSubKey, PCSTR pcValueName, 
									UINT8 *pcType, PVOID pvData, UINT32 *pDataSize);

INT32 REG_Init(VOID);
INT32 REG_SetDefault(VOID);

UINT32 REG_GetVersion(VOID);

UINT32 REG_GetRegSize(VOID);


#endif // for _REG_H_ 

