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

#define COS_TIMER_UNUSED		0

#define COS_MAX_TIMERS              20

#if 1

static struct t_cosTimerInfo {	
    UINT8 flags;
    void (*callbackFunc)(void *);
    void *callbackParam;
} cos_timers[COS_MAX_TIMERS];

UINT8 COS_GetSysTaskMbx(void);

/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/
void COS_TimerDelivery (u32 *Id)
{
    //SXS_RAISE ((_SXR|TNB_ARG(1)|TDB, TSTR("Timer not handled %x\n", 0x070b0001), Id));
    hal_DbgAssert("Timer not handled %x\n", Id);
}


void COS_Timer_Initialise(void)
{
    sxr_InitTimer (COS_TimerDelivery, pal_TimerSetCnt, pal_TimerGetCnt, SXR_REGULAR_TIMER);

    memset(cos_timers, 0, sizeof(cos_timers));
}

void COS_Timer_Shutdown(void)
{
    //COS_DeleteSemaphore( timer_info.timer_sem);
}


UINT8 COS_SetTimer(UINT32 nMillisecondes, void(*callbackFunc)(void *), void *callbackArg, UINT8 flags)
{
    /* Timer handles must start at 1 - timer handle 0 is unused */
    struct t_cosTimerInfo *newEntry;
    UINT8 retVal=0;
	UINT8 i;
	UINT32 csStatus;
    UINT32 nElapse;

	if(flags == 0)
        return 0;

	for(i=1; i<COS_MAX_TIMERS; i++)
	{
		if(cos_timers[i].flags == COS_TIMER_UNUSED)
		{
		    //COS_WaitForSemaphore( timer_info.timer_sem, COS_WAIT_FOREVER);
		    csStatus = hal_SysEnterCriticalSection();
		    if(cos_timers[i].flags == COS_TIMER_UNUSED)
            {      
				newEntry = &(cos_timers[i]);
                newEntry->flags=flags;
				retVal = i;
            	hal_SysExitCriticalSection(csStatus);
				break;
            }
            //COS_ReleaseSemaphore( timer_info.timer_sem);
            hal_SysExitCriticalSection(csStatus);
		}
	}
    if (i==COS_MAX_TIMERS) {
        hal_HstSendEvent(SYS_EVENT, 0x12240100);
        return 0;
    }

    newEntry->callbackFunc=callbackFunc;
    newEntry->callbackParam=callbackArg;
    nElapse = nMillisecondes MS_WAITING;
    csStatus = hal_SysEnterCriticalSection();
    sxs_StartTimer ( nElapse, HVY_TIMER_IN+retVal, (VOID*)(((flags==COS_TIMER_MODE_SINGLE)?FALSE:TRUE)<<31|nElapse), FALSE, COS_GetSysTaskMbx());
    hal_SysExitCriticalSection(csStatus);
    return retVal;
}

BOOL COS_ChangeTimer(UINT8 timerHandle, UINT32 nMillisecondes)
{
    UINT32 nElapse;

    if(timerHandle !=0 && timerHandle < COS_MAX_TIMERS)
    {
        UINT8 timer_index = timerHandle;
        struct t_cosTimerInfo *p_timer = &cos_timers[timer_index];

        nElapse = nMillisecondes MS_WAITING;
        sxs_StartTimer ( nElapse, HVY_TIMER_IN+timer_index, (VOID*)(((p_timer->flags==COS_TIMER_MODE_SINGLE)?FALSE:TRUE)<<31|nElapse), TRUE, COS_GetSysTaskMbx());
        
        return TRUE;
    }
    else
	    return FALSE;
}

BOOL COS_ChangeTimerUseSecond(UINT8 timerHandle, UINT32 secondes)
{
    UINT32 nElapse;

    if(timerHandle !=0 && timerHandle < COS_MAX_TIMERS)
    {
        UINT8 timer_index = timerHandle;
        struct t_cosTimerInfo *p_timer = &cos_timers[timer_index];

        nElapse = secondes SECOND;
        sxs_StartTimer ( nElapse, HVY_TIMER_IN+timer_index, (VOID*)(((p_timer->flags==COS_TIMER_MODE_SINGLE)?FALSE:TRUE)<<31|nElapse), TRUE, COS_GetSysTaskMbx());
        
        return TRUE;
    }
    else
	    return FALSE;
}

//
// Stop and Realse Timer
//
BOOL COS_KillTimer(UINT8 timerHandle)
{
    if(timerHandle !=0 && timerHandle < COS_MAX_TIMERS && cos_timers[timerHandle].flags != COS_TIMER_UNUSED)
    {
        UINT32 csStatus = hal_SysEnterCriticalSection();
        UINT8 timer_index = timerHandle;
        if(cos_timers[timer_index].flags != COS_TIMER_UNUSED)
        {
        	cos_timers[timer_index].flags = COS_TIMER_UNUSED;
            sxs_StopTimer(HVY_TIMER_IN+timer_index, NULL, COS_GetSysTaskMbx());
        }
    	hal_SysExitCriticalSection(csStatus);

        return TRUE;
    }
    else
	    return FALSE;
}

void COS_HandleExpiredTimers(UINT8 timerHandle)
{
    int curr_index;
	struct t_cosTimerInfo *current;

    void (*callbackFunc)(void *);
    void *callbackParam;
    
    //COS_WaitForSemaphore( timer_info.timer_sem, COS_WAIT_FOREVER);
    UINT32 csStatus = hal_SysEnterCriticalSection();
	current = &(cos_timers[timerHandle]);

	if(current->flags == COS_TIMER_UNUSED)
	{
	    sxs_StopTimer(HVY_TIMER_IN+timerHandle, NULL, COS_GetSysTaskMbx());
	    hal_SysExitCriticalSection(csStatus);
		return;
	}	
    callbackFunc=current->callbackFunc;
    callbackParam = current->callbackParam;
    if (current->flags == COS_TIMER_MODE_SINGLE)
    {
        sxs_StopTimer(HVY_TIMER_IN+timerHandle, NULL, COS_GetSysTaskMbx());
    	current->flags = COS_TIMER_UNUSED; // disable the timer
    } 
    //COS_ReleaseSemaphore( timer_info.timer_sem);
	hal_SysExitCriticalSection(csStatus);

    // call callback function
	if(callbackFunc)
	    callbackFunc(callbackParam);
}

UINT32 COS_GetTickCount(VOID)
{
    return ((UINT64)hal_TimGetUpTime())*1000/HAL_TICK1S;
}


#else

static struct t_cosTimerInfo {	
    UINT8 flags;
    UINT8 next;
    UINT8  module_id;
    UINT32 timeoutTicks;
    UINT32 timeToExpiry;
    void (*callbackFunc)(void *);
    void *callbackParam;
} cos_timers[COS_MAX_TIMERS];

static struct st_timer_info {
    UINT32 nTickCount;
    UINT8 curr_process_timer;
    UINT8 event_pending;
    //HANDLE timer_sem;
} timer_info;

void cos_timer_enlist(UINT8 timer_index);
void cos_timer_delist(UINT8 timer_index);

void COS_Timer_Initialise(void)
{
    timer_info.nTickCount = 0;
    timer_info.curr_process_timer = 0;
    timer_info.event_pending = 0;
    //timer_info.timer_sem = COS_CreateSemaphore(1);

    memset(cos_timers, 0, sizeof(cos_timers));
}

void COS_Timer_Shutdown(void)
{
    //COS_DeleteSemaphore( timer_info.timer_sem);
}


UINT8 COS_SetTimer(UINT32 nMillisecondes, void(*callbackFunc)(void *), void *callbackArg, UINT8 flags)
{
    /* Timer handles must start at 1 - timer handle 0 is unused */
    struct t_cosTimerInfo *newEntry;
    UINT8 retVal;
	UINT8 i;
	UINT32 csStatus;

	if(flags == 0)
        return 0;

	for(i=1; i<COS_MAX_TIMERS; i++)
	{
		if(cos_timers[i].flags == COS_TIMER_UNUSED)
		{
		    //COS_WaitForSemaphore( timer_info.timer_sem, COS_WAIT_FOREVER);
		    csStatus = hal_SysEnterCriticalSection();
		    if(cos_timers[i].flags == COS_TIMER_UNUSED)
            {      
				newEntry = &(cos_timers[i]);
                newEntry->flags=flags;
				retVal = i;
            	hal_SysExitCriticalSection(csStatus);
				break;
            }
            //COS_ReleaseSemaphore( timer_info.timer_sem);
            hal_SysExitCriticalSection(csStatus);
		}
	}
    if (i==COS_MAX_TIMERS) {
        hal_HstSendEvent(SYS_EVENT, 0x12240100);
        return 0;
    }

    newEntry->callbackFunc=callbackFunc;
    if(nMillisecondes < COS_TIMER_TICK_INTERVAL)
        newEntry->timeoutTicks = 1;
    else
        newEntry->timeoutTicks=nMillisecondes/COS_TIMER_TICK_INTERVAL;
    newEntry->timeToExpiry=newEntry->timeoutTicks + timer_info.nTickCount;
    newEntry->callbackParam=callbackArg;
    newEntry->next = 0;
    cos_timer_enlist(retVal);
    return retVal;
}

BOOL COS_ChangeTimer(UINT8 timerHandle, UINT32 nMillisecondes)
{
    if(timerHandle !=0 && timerHandle < COS_MAX_TIMERS)
    {
        UINT8 timer_index = timerHandle;
        struct t_cosTimerInfo *p_timer = &cos_timers[timer_index];
        
        cos_timer_delist(timer_index);

        if(nMillisecondes < COS_TIMER_TICK_INTERVAL)
            p_timer->timeoutTicks = 1;
        else
            p_timer->timeoutTicks = nMillisecondes/COS_TIMER_TICK_INTERVAL;
        p_timer->timeToExpiry = p_timer->timeoutTicks + timer_info.nTickCount;
        cos_timer_enlist(timer_index);

        return TRUE;
    }
    else
	    return FALSE;
}


//
// Stop and Realse Timer
//
BOOL COS_KillTimer(UINT8 timerHandle)
{
    if(timerHandle !=0 && timerHandle < COS_MAX_TIMERS && cos_timers[timerHandle].flags != COS_TIMER_UNUSED)
    {
        UINT8 timer_index = timerHandle;
    	 cos_timers[timer_index].flags = COS_TIMER_UNUSED;
        cos_timer_delist(timer_index);
        return TRUE;
    }
    else
	    return FALSE;
}

BOOL COS_CheckTimers(UINT32 count)
{
	struct t_cosTimerInfo *current;

    timer_info.nTickCount += count;

    if(timer_info.event_pending)
        return FALSE;

    
	current = &(cos_timers[timer_info.curr_process_timer]);

	if(current->flags == COS_TIMER_UNUSED)
		return FALSE;

	/* Decrement the counter and check for expiry */
    if(timer_info.nTickCount >= current->timeToExpiry)
    {
        timer_info.event_pending = 1;
        return TRUE;
    }
    
    return FALSE;
}

void COS_HandleExpiredTimers(void)
{
    int curr_index;
	struct t_cosTimerInfo *current;

    //   timer_info.nTickCount ++;
    timer_info.event_pending = 0;
    
	current = &(cos_timers[timer_info.curr_process_timer]);

	if(current->flags == COS_TIMER_UNUSED)
		return;

	/* Decrement the counter and check for expiry */
       while(timer_info.nTickCount >= current->timeToExpiry)
       {
            void (*callbackFunc)(void *);
            void *callbackParam;

            
            //COS_WaitForSemaphore( timer_info.timer_sem, COS_WAIT_FOREVER);
            UINT32 csStatus = hal_SysEnterCriticalSection();
        	current = &(cos_timers[timer_info.curr_process_timer]);

        	if(current->flags == COS_TIMER_UNUSED)
        	{
        	    hal_SysExitCriticalSection(csStatus);
        		return;
        	}	
            callbackFunc=current->callbackFunc;
            callbackParam = current->callbackParam;
            curr_index = timer_info.curr_process_timer;
            timer_info.curr_process_timer = current->next;
            current->next = 0;
            if (current->flags == COS_TIMER_MODE_SINGLE)
            {
            	current->flags = COS_TIMER_UNUSED; // disable the timer
            } 
            //COS_ReleaseSemaphore( timer_info.timer_sem);
			hal_SysExitCriticalSection(csStatus);

			if (current->flags == COS_TIMER_MODE_PERIODIC) 
            {
                current->timeToExpiry = current->timeoutTicks + timer_info.nTickCount;
                cos_timer_enlist(curr_index);
            }
            
            // call callback function
		if(callbackFunc)
		    callbackFunc(callbackParam);
	}
}

UINT32 COS_GetTickCount( VOID)
{
    return timer_info.nTickCount;
}

/**************************************************************
 *
 * Function : cos_timer_delist
 *
 * Input Params : timer_index - Identifies timer to delist
 *                 
 * Description :
 * Local proc to remove timer from ordered list of used timers.
 *
 * Context: Both Task & ISR.
 *
 **************************************************************/
void cos_timer_delist(UINT8 timer_index)
{
    UINT8 iter;

    //COS_WaitForSemaphore( timer_info.timer_sem, COS_WAIT_FOREVER);
    UINT32 csStatus = hal_SysEnterCriticalSection();

    if(timer_index == timer_info.curr_process_timer)
    {
        timer_info.curr_process_timer = cos_timers[timer_index].next;
        cos_timers[timer_index].next = 0;
    }
    else if(timer_index)
    {
        for(iter=timer_info.curr_process_timer; iter; iter=cos_timers[iter].next)
        {
            if(cos_timers[iter].next == timer_index)
            {
                cos_timers[iter].next = cos_timers[timer_index].next;
                cos_timers[timer_index].next = 0;
                break;
            }
        }
    }

    //COS_ReleaseSemaphore( timer_info.timer_sem);
    hal_SysExitCriticalSection(csStatus);
}

/**************************************************************
 *
 * Function : cos_timer_enlist
 *
 * Input Params : timer_index - Identifies timer to enlist
 *                 
 * Description :
 * Local proc to insert timer into ordered list of used timers.
 *
 * Context: Both Task & ISR.
 *
 **************************************************************/
void cos_timer_enlist(UINT8 timer_index)
{    
    UINT8 iter;
    UINT8 b4timer = 0;

    //COS_WaitForSemaphore( timer_info.timer_sem, COS_WAIT_FOREVER);
    //if(timer_index == timer_info.curr_process_timer)
    //    hal_DbgAssert("the enlist index is current index");
    UINT32 csStatus = hal_SysEnterCriticalSection();

    iter = timer_info.curr_process_timer;
    while(iter && (cos_timers[iter].timeToExpiry<cos_timers[timer_index].timeToExpiry))
    {
        b4timer = iter;
        iter=cos_timers[iter].next;
    }

    if(iter == timer_info.curr_process_timer)
    {
        cos_timers[timer_index].next = timer_info.curr_process_timer;
        timer_info.curr_process_timer = timer_index;
    }
    else
    {
        cos_timers[timer_index].next = cos_timers[b4timer].next;
        cos_timers[b4timer].next = timer_index;
    }

    //COS_ReleaseSemaphore( timer_info.timer_sem);
    hal_SysExitCriticalSection(csStatus);
}

#endif

//
// Sleep current task in nMillisecondes
//
BOOL COS_Sleep (
  UINT32 nMillisecondes
)
{
    extern VOID sxr_Sleep (UINT32 Period);

     sxr_Sleep(nMillisecondes*16384/1000);
     return TRUE;
}

