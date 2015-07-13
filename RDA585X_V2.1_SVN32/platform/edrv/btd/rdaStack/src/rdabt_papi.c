/***********************************************************************
 *
 * MODULE NAME:    papi_uitron.c
 * PROJECT CODE:   host side stack
 * DESCRIPTION:    PAPI for UITRON port
 * MAINTAINER:     John Sheehy
 * CREATION DATE:  1 May 2000
 *
 * SOURCE CONTROL: $Id: papi_mtk.c,v 1.3 2008/12/30 09:58:42 tianwq Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2001 Parthus Technologies Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    1.May.2000 - JS - first import
 *   03.Jul.2000 - DA - Updated with timer/semaphores
 *
 *
 * ISSUES:
 *       
 ***********************************************************************/

#include "project_config.h"
#include "cos.h"
#include "bt.h"
#include "papi.h"
#include "rdabt_main.h"
#include "hal_dma.h"
#include "hal_sys.h"
#include "sxs_io.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if pDEBUG
#include "fs.h"

INT32 h_Log_File = -1;


#endif

void RDABT_Platform_Init(void)
{
}


#if pDEBUG
static const char* LOGLAYER[]=
{
    "pLOGSYSTEM",
    "pLOGHCI",
    "pLOGMANAGER",
    "pLOGL2CAP",
    "pLOGRFCOMM",
    "pLOGSDP",
    "pLOGOBEX",
    "pLOGTCS",
    "pLOGA2DP",
    "pALWAYS"
};

static const char* LOGLEVEL[]=
{
//    "pLOGDATA",	
    "pLOGFATAL",
    "pLOGCRITICAL",
    "pLOGERROR",	
    "pLOGWARNING",
    "pLOGNOTICE",
    "pLOGDEBUG"	
};

UINT8 LogLayer_Encode(UINT32 loglayer)
{
    UINT8 i=0;
    if(loglayer==0)
        return 16;//error
    if(pLOGALWAYS == loglayer)
        return 16;
    while(loglayer)
    {
         loglayer=loglayer>>1;
         i++;
    }        
    if(i>16)
        return 16;
    return i;
    
}

static char bt_trace_buff[512];

int RDA_Debug_PrintEX(int logLevel,int logLayer, char *formatString, ... ) 
{
    va_list args;

	//if(pLOGHCI==logLayer)
	//return;
	//if(pLOGSYSTEM==logLayer)
	//	return;
	//if(pLOGMANAGER==logLayer)
	//	return;
	//if(pLOGSDP==logLayer)
	//return;
	//if(pLOGRFCOMM==logLayer)
		//return;
	//if(pLOGL2CAP==logLayer)
		//return;
	//if(pLOGDEBUG==logLevel)
	//	return;

    va_start(args,formatString);
    sxs_vprintf(_BT|TLEVEL(LogLayer_Encode(logLayer)), formatString, args);
    va_end(args);

    return RDABT_NOERROR;
}

int RDA_Debug_Print(int logLevel, char *formatString, ... )
{
    va_list args;

    va_start(args,formatString);
    sxs_vprintf(_BT|TLEVEL(9), formatString, args);
    va_end(args);

    return RDABT_NOERROR;
}

int pDebugDumpFunc(int logLevel, u_int8 *buffer, u_int32 len)
{
  return pDebugDumpFuncEX(logLevel,pLOGALWAYS,buffer,len);
}

int pDebugDumpFuncEX(int logLevel, int logLayer, u_int8 *buffer, u_int32 len) 
{
  return RDABT_NOERROR;
}

int pDebugCheckFunc(void) 
{
  return RDABT_NOERROR;
}

#endif

void *pMalloc(size_t size)
{
    return COS_Malloc(size);
}

void pFree(void *buffer)
{
   COS_Free(buffer);
}

/*
 * PAPI TIMER SECTION
 * no used in MTK
 */
t_pTimer pTimerCreate(u_int32 timeoutTenths, void(*callbackFunc)(void *), void *callbackArg, u_int8 flags) 
{
    return 0;
}

int pTimerSet(unsigned int timeout, void(*callbackFunc)(void *), void *callbackParm, int *timerHandle)
{
   return 0;
}

int pTimerCancel(t_pTimer timerHandle)
{
    return 0;
}

void rdabt_send_notify(void)
{
    extern u_int8  rdabt_send_notify_pending;

    if(rdabt_send_notify_pending < 3)
    {
        COS_EVENT ev = {0};
        ev.nEventId = EV_BT_NOTIFY_IND;
        COS_SendEvent(MOD_BT,  &ev , COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        rdabt_send_notify_pending++;
    }
}

void *pMemcpy(void *dest, void *src, size_t numbytes)
{

    if(1)//numbytes < 128)
        return memcpy(dest, src, numbytes);
    else
    {
        HAL_DMA_CFG_T transferCfg;
        // Use a DMA transfer as it will be faster than the CPU.
        transferCfg.srcAddr         = (UINT8*)src;
        transferCfg.dstAddr         = (UINT8*)dest;
        transferCfg.alterDstAddr    = NULL;
        transferCfg.pattern         = 0;
        transferCfg.transferSize    = (UINT32)numbytes;
        transferCfg.mode            = HAL_DMA_MODE_NORMAL;
        transferCfg.userHandler     = NULL;
        // Start the DMA
        while (HAL_ERR_NO != hal_DmaStart(&transferCfg))
        {
            sxr_Sleep(10);
        }

        // Wait for the DMA to finish
        // (hal_OverlayLoad is a blocking function).
        while (!hal_DmaDone())
        {
            sxr_Sleep(10);
        }
    }        
}

int pSendEvent(u_int16 event_id, u_int8 *msg)
{
    return 0;
}

u_int32 RDABT_Enter_CriticalSection()
{
    return hal_SysEnterCriticalSection();
}

void RDABT_Exit_CriticalSection(u_int32 status)
{
    hal_SysExitCriticalSection(status);
}

