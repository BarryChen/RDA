/***********************************************************************
 *
 * MODULE NAME:    papi_impl.h
 * PROJECT CODE:   host side stack
 * DESCRIPTION:    PAPI eCos specific include
 * MAINTAINER:     John Sheehy
 * CREATION DATE:  1 May 2000
 *
 * SOURCE CONTROL: $Id: papi_impl.h,v 1.1 2008/12/18 01:53:15 tianwq Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2001 RDA Microelectronics Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    1.May.2000 - JS - first import
 *
 *
 * ISSUES:
 *       
 ***********************************************************************/
#ifndef PAPI_IMPL_H
#define PAPI_IMPL_H

#define pNULL NULL

#ifndef __CS_TYPE_H__
#ifndef __size_t
#define __size_t   1
typedef u_int32  size_t;
#endif
#endif
typedef u_int32  t_pMutex;
typedef u_int32  t_pTimer;
typedef u_int32  t_pEvent;


//#define pMemcpy memcpy
#define pMemset memset
#define pMemcmp memcmp
#define pMemchr memchr
#define pStrcmp strcmp
#define pStrncmp strncmp
#define pStrlen strlen
#define pStrcpy strcpy
#define pStrncpy strncpy

/* platform timer is no used in mtk */
#define pSetTimer(millisec)       (0)
#define pChangeTimer(id, millisec)       (0)
#define pCancelTimer(id)       (0)
#define pGetCurrentTicks()              (0)
#define pGetTickDifference(start, end)      (0)

#define pWakeUpScheduler()              rdabt_send_notify()

#if pDEBUG
#define pDebugPrintfEX(args)                 (RDA_Debug_PrintEX args)
#define pDebugPrintf(args)                   (RDA_Debug_Print args)
#define pDebugDumpEX(level,layer,buf,len)    (pDebugDumpFuncEX((level),(layer),(buf),(len)))
#define pDebugDump(level,buf,len)            (pDebugDumpFunc((level),(buf),(len)))
#define pDebugCheck()                        (pDebugCheckFunc())
#endif


#define  pMutexCreate(initState)        (0)
#define  pMutexFree(mutex)              (0)
#define  pMutexLock(mutex)              (0)
#define  pMutexUnlock(mutex)            (0)

#define pEnterCriticalSection()				RDABT_Enter_CriticalSection()
#define pExitCriticalSection(status)		RDABT_Exit_CriticalSection(status)

#endif /* PAPI_IMPL_H */


void rdabt_send_notify(void);
int RDA_Debug_PrintEX(int logLevel,int logLayer, char *formatString, ...) ;
int RDA_Debug_Print(int logLevel, char *formatString, ... );
int pDebugDumpFunc(int logLevel, u_int8 *buffer, u_int32 len);
int pDebugDumpFuncEX(int logLevel, int logLayer, u_int8 *buffer, u_int32 len) ;
int pDebugCheckFunc(void) ;

u_int32 RDABT_Enter_CriticalSection();
void RDABT_Exit_CriticalSection(u_int32 status);


