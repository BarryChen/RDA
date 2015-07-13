/******************************************************************************/
/*              Copyright (C) 2005, CII Technologies, Inc.                    */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    cos_task.c                                                    */
/*                                                                            */
/* Description:                                                               */
/*   COS implementation.                                                      */
/******************************************************************************/
#include <csw.h>
#include <stdkey.h>
#include <event.h>
#include <base_prv.h>
#include <ts.h>
#include<sxr_tksd.h>
#include<sxr_tim.h>
#include "cos.h"
#include "csw_csp.h"

#include "dbg.h"
#include "sxr_sbx.h"
extern int sprintf(char *buf, const char *fmt, ...);
#define COS_ASSERT(expr) \
				do { \
					char assertion_info[256] = {0}; \
					sprintf(assertion_info, "[COS_ASSERT]FILE:%s FUN:%s LN:%d", __FILE__, __FUNCTION__, __LINE__); \
					DBG_ASSERT(expr, assertion_info); \
				} while (0)


#define IS_VALID_MAILBOX_ID(x)  TRUE 

#define  MAX_TASK_NAME_LEN        16
static TASK_HANDLE g_tskTaskList[TOTAL_MODULES];

extern BOOL hasFreeEnv(UINT8 nMbxId);
extern VOID sxr_Sleep (UINT32 Period);

PRIVATE HANDLE g_hCosMmiTask = HNULL; // the default MMI task.

HANDLE COS_GetDefaultMmiTaskHandle(VOID)
{
    COS_ASSERT(g_hCosMmiTask != HNULL);
    return g_hCosMmiTask;
}

//
// Create task. Apply to upper layer MMI.
//
HANDLE COS_CreateTask(
  PTASK_ENTRY pTaskEntry,
  COS_MOD_ID mod,
  PVOID pParameter,
  PVOID pStackAddr,
  UINT16 nStackSize,
  UINT8 nPriority,
  UINT16 nCreationFlags,
  UINT16 nTimeSlice,
  PCSTR pTaskName
)
{
    TASK_HANDLE* pHTask = NULL;
    
//    if(nPriority < COS_PRI_BASE || nPriority > COS_PRI_BK_BASE)
//        return HNULL;

    pHTask = &(g_tskTaskList[mod]);

    pHTask->sTaskDesc.Name = pTaskName;
    if(pTaskName == 0)
    {
        pHTask->sTaskDesc.Name = "noname";

    }

    pHTask->sTaskDesc.nPriority   = nPriority;
    pHTask->sTaskDesc.nStackSize  = nStackSize;
    pHTask->sTaskDesc.TaskBody    = pTaskEntry;
    pHTask->sTaskDesc.TaskExit    = NULL;
    pHTask->nMailBoxId            = sxr_NewMailBox();
    pHTask->nTaskId               = sxr_NewTask ((sxr_TaskDesc_t*)&pHTask->sTaskDesc);
    
    if(nCreationFlags&COS_CREATE_SUSPENDED)
    {
        return (HANDLE)NULL; 
    }
    else if(nCreationFlags&COS_CREATE_PREEMPT)
    {
        return (HANDLE)NULL;  
    }
    else if(nCreationFlags&COS_CREATE_NO_PREEMPT)
    {
        return (HANDLE)NULL;     
    }
    else 
    {
        sxr_StartTask (pHTask->nTaskId, pParameter);
    }
    
   // if( (UINT32)pTaskEntry == (UINT32)BAL_MmiTask )
    {
       // g_hCosMmiTask = (HANDLE)pHTask;;
    }
    
    return (HANDLE)pHTask; 
}

//
// Start task. Start a suspended task(never start over).
//
VOID COS_StartTask(
  TASK_HANDLE *pHTask,
  PVOID pParameter
)
{
  sxr_StartTask (pHTask->nTaskId, pParameter);
}


//
// Stop task. 
//
VOID COS_StopTask(
  TASK_HANDLE *pHTask
)
{
  sxr_StopTask (pHTask->nTaskId);
}

//
// Create task, only apply to CSW internal.
//
HANDLE COS_CreateTask_Prv(
  PTASK_ENTRY pTaskEntry,
  PVOID pParameter,
  PVOID pStackAddr,
  UINT16 nStackSize,
  UINT8 nPriority,
  UINT16 nCreationFlags,
  UINT16 nTimeSlice,
  PCSTR pTaskName
)
{
    TASK_HANDLE* pHTask = NULL;

    //CSW_TRACE(200, TSTXT("COS_CreateTask_Prv nPriority: %d \n"), nPriority);

    pHTask = (TASK_HANDLE*)CSW_COS_MALLOC( SIZEOF(TASK_HANDLE) ); //&g_hTask[g_TaskCnt++]; 
    if(pHTask == NULL)
        return FALSE;

    if(pTaskName)
    {
        pHTask->sTaskDesc.Name = pTaskName;
    }
    else
    {
        pHTask->sTaskDesc.Name = "no name";
    }


    pHTask->sTaskDesc.nPriority   = nPriority;
    pHTask->sTaskDesc.nStackSize  = nStackSize;
    pHTask->sTaskDesc.TaskBody    = pTaskEntry;
    pHTask->sTaskDesc.TaskExit    = NULL;
    pHTask->nMailBoxId            = sxr_NewMailBox();
    pHTask->nTaskId               = sxr_NewTask ((sxr_TaskDesc_t*)&pHTask->sTaskDesc);
    
    if(nCreationFlags&COS_CREATE_SUSPENDED)
    {
        return (HANDLE)NULL; 
    }
    else if(nCreationFlags&COS_CREATE_PREEMPT)
    {
        return (HANDLE)NULL;
    }
    else if(nCreationFlags&COS_CREATE_NO_PREEMPT)
    {
        return (HANDLE)NULL;
    }
    else
    {
        sxr_StartTask (pHTask->nTaskId, pParameter);
    }
    
    return (HANDLE)pHTask;
}

//
// Delete Task. Release Mailbox, task, and the handle.
//
BOOL COS_DeleteTask(
  COS_MOD_ID mod
)
{
    TASK_HANDLE* pHTask  = &(g_tskTaskList[mod]);
    
    sxr_FreeTask(pHTask->nTaskId);
    sxr_FreeMailBox(pHTask->nMailBoxId);

    return FALSE;
}

//
// Suspsend Task.
//
UINT32 COS_SuspendTask(
  COS_MOD_ID mod
)
{
    TASK_HANDLE* pHTask  = &(g_tskTaskList[mod]);
    sxr_SuspendTask(pHTask->nMailBoxId);

    return TRUE;
}

//
// Resume task.
//
BOOL COS_ResumeTask (
  COS_MOD_ID mod
)
{
    TASK_HANDLE* pHTask  = &(g_tskTaskList[mod]);
    sxr_ResumeTask(pHTask->nMailBoxId);

    return TRUE;
}

BOOL COS_WaitEvent(
  COS_MOD_ID mod,
  COS_EVENT* pEvent,
  UINT32 nTimeOut
)
{
    TASK_HANDLE* pHTask  = &(g_tskTaskList[mod]);
    UINT32 Evt [4] = {0, 0, 0, 0};
    BOOL ret = FALSE;

    if(nTimeOut == COS_WAIT_FOREVER)
    {
        if( IS_VALID_MAILBOX_ID(pHTask->nMailBoxId) )
            sxr_Wait(Evt, pHTask->nMailBoxId);
        else
            return FALSE;
    }
    else
        return FALSE;
    
    if( Evt[0] ) 
    {
        if(Evt[0] >= HVY_TIMER_IN )
        {
            if(Evt[1]>>31)
                sxs_StartTimer ( Evt[1]&0x7FFFFFFF, Evt[0], (VOID*)Evt[1], FALSE, pHTask->nMailBoxId );
            Evt[1] = Evt[0]-HVY_TIMER_IN;
            Evt[0] = EV_TIMER;
        }
        ret = TRUE;
    }
    else
        ret = FALSE;
    
    if(Evt[0])
        ret = TRUE;
    
    pEvent->nEventId = Evt[0];
    pEvent->nParam1  = Evt[1];
//    pEvent->nParam2  = Evt[2];//for delete nParam2
//    pEvent->nParam3  = Evt[3];//for delete nParam3
//    CSW_TRACE(6, ("COS_WaitEvent event id = %d, param1 = %d. \n"), pEvent->nEventId, pEvent->nParam1);

    return ret;
}

//
// Send message to the specified task.
//
BOOL COS_SendEvent(
  COS_MOD_ID  mod, 
  COS_EVENT* pEvent,
  UINT32 nTimeOut,
  UINT16 nOption
)
{
    TASK_HANDLE* pHTask  = &(g_tskTaskList[mod]);
    
    if( pHTask == NULL || pEvent== NULL )
        return FALSE;
    if(nTimeOut == COS_WAIT_FOREVER)
    {
        UINT8 status = SXR_SEND_EVT;
        if( nOption&COS_EVENT_PRI_URGENT )
            status |= SXR_QUEUE_FIRST;
        
        if (!hasFreeEnv(pHTask->nMailBoxId))
        {
			CSW_TRACE(6, ("COS_SendEvent, Hs no Free Env, event id=%d. \n"), pEvent->nEventId);
            return FALSE;
        }
        sxr_Send(pEvent, pHTask->nMailBoxId, status);
//        CSW_TRACE(6, ("COS_SendEvent event id = %d, param1 = %d. \n"), pEvent->nEventId, pEvent->nParam1);
        
        return TRUE;
    }

    return FALSE;
}


//#define GET_SYSTASK_QUEUE_ID()    (((TASK_HANDLE*)BAL_TH(BAL_SYS_TASK_PRIORITY))->nMailBoxId)

//
// Send message to SYS_Task.
//
BOOL COS_SendSysEvent(
  COS_EVENT* pEvent,
  UINT16 nOption
)
{
    TASK_HANDLE* pHTask  = &(g_tskTaskList[MOD_SYS]);
    UINT8 status = SXR_SEND_EVT;
    
    if( nOption&COS_EVENT_PRI_URGENT )
        status |= SXR_QUEUE_FIRST;
    
    sxr_Send(pEvent,pHTask->nMailBoxId, status);

    return TRUE;
}

BOOL COS_ResetEventQueue(
  HANDLE hTask
)
{
    return FALSE;
}

BOOL COS_IsEventAvailable(
  COS_MOD_ID  mod
)
{
    TASK_HANDLE* pHTask  = &(g_tskTaskList[mod]);
    
    if(pHTask == NULL)
        return FALSE;
    
    if( sxr_SbxHot( pHTask->nMailBoxId ) )
         return TRUE;
    else
        return FALSE;
}

#if 0
//
// Start a timer with time-out value.
//
BOOL COS_SetTimer (
  HANDLE hTask,
  UINT8 nTimerId,
  UINT8 nMode,
  UINT32 nElapse
)
{
    TASK_HANDLE* pHTask = (TASK_HANDLE*)hTask;
    UINT8 nMailBoxId    = 0;

    if(nElapse > 0x7FFFFFFF)
        return FALSE;
    
    if((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }
    
    if(pHTask)
    {
        if( nMailBoxId )
        {
            BOOL bMode = (nMode==COS_TIMER_MODE_SINGLE)?FALSE:TRUE;            
            //sxs_StartTimer ( nElapse*15, HVY_TIMER_IN+nTimerId, (VOID*)(bMode<<31|nElapse), FALSE, nMailBoxId );
            sxs_StartTimer ( nElapse, HVY_TIMER_IN+nTimerId, (VOID*)(bMode<<31|nElapse), FALSE, nMailBoxId );

            return TRUE;
        }
    }

    return FALSE;
}

BOOL COS_SetTimerEX (
  HANDLE hTask,
  UINT16 nTimerId,//nTimerId should small than 0x1000,
  UINT8 nMode,
  UINT32 nElapse
)
{
    TASK_HANDLE* pHTask = (TASK_HANDLE*)hTask;
    UINT8 nMailBoxId    = 0;
     if(nTimerId>0x1000)
   {	
	return FALSE;
   }
	
    if(nElapse > 0x7FFFFFFF)
        return FALSE;
    
    if((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }
    
    if(pHTask)
    {
        if( nMailBoxId )
        {
            BOOL bMode = (nMode==COS_TIMER_MODE_SINGLE)?FALSE:TRUE;            
            //sxs_StartTimer ( nElapse*15, HVY_TIMER_IN+nTimerId, (VOID*)(bMode<<31|nElapse), FALSE, nMailBoxId );
            sxs_StartTimer ( nElapse, HVY_TIMER_IN+nTimerId, (VOID*)(bMode<<31|nElapse), FALSE, nMailBoxId );
            return TRUE;
        }
    }

    return FALSE;
}

//
// Stop and Realse Timer
//
BOOL COS_KillTimer (
  HANDLE hTask,
  UINT8 nTimerId 
)
{
    TASK_HANDLE* pHTask = (TASK_HANDLE*)hTask;
    UINT8 nMailBoxId = 0;
    
    if((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }
    
    if(pHTask)
    {
        if( nMailBoxId )
        {
            sxs_StopTimer(HVY_TIMER_IN+nTimerId, NULL, nMailBoxId);
            return TRUE;
        }
    }

    return FALSE;
}
BOOL COS_KillTimerEX (
  HANDLE hTask,
  UINT16 nTimerId 
)
{
    TASK_HANDLE* pHTask = (TASK_HANDLE*)hTask;
    UINT8 nMailBoxId = 0;
    
    if(nTimerId>0x1000)
   {	
	return FALSE;
   }
    if((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }
    
    if(pHTask)
    {
        if( nMailBoxId )
        {
            sxs_StopTimer(HVY_TIMER_IN+nTimerId, NULL, nMailBoxId);
            return TRUE;
        }
    }

    return FALSE;
}


//
// Sleep current task in nMillisecondes
//
BOOL COS_Sleep (
  UINT32 nMillisecondes
)
{
     sxr_Sleep(nMillisecondes*16384/1000);
     return TRUE;
}

UINT32 COS_GetTickCount( VOID)
{
    return 0; // TODO...
}
#endif

HANDLE COS_EnterCriticalSection (
  VOID
)
{
    return (HANDLE)hal_EnterCriticalSection();
}

BOOL COS_ExitCriticalSection (
  HANDLE hSection
)
{
    hal_ExitCriticalSection(hSection);
    
    return TRUE;
}

HANDLE SRVAPI COS_CreateSemaphore (
     UINT32  nInitCount  // Specify the initial count of the semaphore    sxr_NewSemaphore
)
{
  volatile UINT32  nCallerAdd = 0x00;
  COS_GET_RA(&nCallerAdd);
UINT8 id = sxr_NewSemaphore((UINT8)nInitCount);
    CSW_TRACE(9, "COS_CreateSemaphore sbx: 0x%x; caller:0x%x \n", id, nCallerAdd);

    return (HANDLE) id;
}

BOOL SRVAPI COS_DeleteSemaphore(
    HANDLE hSem
)
{
    if(hSem == 0)//NULL)modified by fengwei 20080917 for warning reduce
        return FALSE;
    
    sxr_FreeSemaphore(hSem);
    
    return TRUE;
}

UINT8 test_cont =10;
BOOL SRVAPI COS_WaitForSemaphore(
    HANDLE hSem,     // Specify the handle to a counting semaphore 
    UINT32 nTimeOut  // the time-out value
)
{
    if(hSem == 0)//NULL)modified by fengwei 20080917 for warning reduce
        return FALSE;
    
    sxr_TakeSemaphore(hSem);
    test_cont--;
//	  CSW_TRACE(9, "COS_WaitForSemaphore test_cont:0x%x \n",test_cont);
    return TRUE;
}

BOOL SRVAPI COS_ReleaseSemaphore(
    HANDLE hSem //Specify the counting semaphore
)
{
    if(hSem == 0)//NULL)modified by fengwei 20080917 for warning reduce
        return FALSE;
    volatile  UINT32  nCallerAdd = 0x00;
    COS_GET_RA(&nCallerAdd);
    sxr_ReleaseSemaphore(hSem); //sxr_SemaphoreHot
test_cont++;
if(test_cont>30)
{
      CSW_TRACE(9, "COS_ReleaseSemaphore sbx: 0x%x; caller:0x%x ,test_cont:0x%x\n", hSem, nCallerAdd,test_cont);

	COS_ASSERT(FALSE);
}
//      CSW_TRACE(9, "COS_ReleaseSemaphore test_cont:0x%x \n",test_cont);

    return TRUE;
}

PRIVATE UINT32 g_TaskLastError[20] = {0, };
PRIVATE UINT32 g_CurTaskId         = 0;     // Need to remember the task id,  TODO... 

UINT32 COS_GetLastError(VOID)
{
    return g_TaskLastError[g_CurTaskId];
}

VOID COS_SetLastError(
  UINT32 nErrCode   // per-thread error code
)
{
    g_TaskLastError[g_CurTaskId] = nErrCode;
}

UINT8 COS_GetSysTaskMbx(void)
{
    return g_tskTaskList[MOD_SYS].nMailBoxId;
}

