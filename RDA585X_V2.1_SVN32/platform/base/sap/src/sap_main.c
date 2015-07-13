/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : sap_main.c
--------------------------------------------------------------------------------

  Scope      : Main.

  History    :
--------------------------------------------------------------------------------
================================================================================
*/

#include "chip_id.h"
#include "hal_sys.h"

#if !defined(__TARGET__) || defined (__SIMU__)
#include <malloc.h>
#ifdef __STKTRACE__
#include "sxs_udp.h"
#endif
#endif

#include "sxs_srl.h"
#include "sxs_type.h"
#include "sxs_lib.h"
#include "sxr_ops.h"
#include "sxr_csp.h"
#include "sxr_mutx.h"
#include "sxs_rmt.h"


#include "sxs_io.h"
#include "sxs_spy.h"

#define PAL_DBG_PORT_IDX 0


#include "cos.h"
#include "event.h"
#include "hal_host.h"
#include "hal_lps.h"

#define IDLE_TASK_STACK_SIZE 128 // 256byte
#define IDLE_TASK_PRIORITY   255

void sxs_TaskIdle (void);
BOOL BAL_TaskInit(VOID);

const sxr_TaskDesc_t sxs_TaskIdleDesc =
{
 (void(*)(void *))sxs_TaskIdle,
 0,
 "Task Idle.",
 IDLE_TASK_STACK_SIZE,
 IDLE_TASK_PRIORITY
};


//------------------------------------------------------------------
// Idle hook variables

void (*g_sxsFsIdleHookFuncPtr)(void) = NIL;

#define SXS_DEBUG_IDLE_HOOK_NUM 4
void (*g_sxsDebugIdleHookFuncPtr[SXS_DEBUG_IDLE_HOOK_NUM])(void) =
    { NIL, };

// To avoid linkage issue while compiling platform_test programs,
// we put the definition in sx_map.c instead of here
//extern volatile u32 g_sxsDebugIdleHookEnable;
EXPORT HAL_HOST_CONFIG_t g_halHostConfig;

//------------------------------------------------------------------
// FS idle hook functions

void sxs_RegisterFsIdleHookFunc(void (*funcPtr)(void))
{
    g_sxsFsIdleHookFuncPtr = funcPtr;
}

void sxs_FsIdleHook (void)
{
    if (g_sxsFsIdleHookFuncPtr != NIL)
    {
        (*g_sxsFsIdleHookFuncPtr)();
    }
}

//------------------------------------------------------------------
// Debug idle hook functions

bool sxs_RegisterDebugIdleHookFunc(void (*funcPtr)(void))
{
    int i;
    bool result = FALSE;

    u32 status = sxr_EnterSc ();
    for (i=0; i<SXS_DEBUG_IDLE_HOOK_NUM; i++)
    {
        if (g_sxsDebugIdleHookFuncPtr[i] == NIL)
            break;
    }

    if (i < SXS_DEBUG_IDLE_HOOK_NUM)
    {
        g_sxsDebugIdleHookFuncPtr[i] = funcPtr;
        result = TRUE;
    }
    sxr_ExitSc (status);

    return result;
}

void sxs_DebugIdleHook (void)
{
    PUBLIC VOID hal_AllSpiRegCheck(VOID);

    hal_AllSpiRegCheck(); // no check flag 

    if (g_halHostConfig.idle_hook_enable == 0)
    {
        return; // skip this debug hook
    }

    int i;
    for (i=0; i<SXS_DEBUG_IDLE_HOOK_NUM; i++)
    {
        if (g_sxsDebugIdleHookFuncPtr[i] != NIL)
        {
            (*g_sxsDebugIdleHookFuncPtr[i])();
        }
    }
}

UINT32 TEST_M=0;

/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/
void sxs_StartAll (void)
{
 u32 i;
 
 sxr_TkSdInit ();
// sxr_JbSdInit ();

 sxr_SbxInit ();
 sxr_MutexInit ();

  COS_Timer_Initialise();

 sxs_IoInit ();

#ifndef NO_REMOTE_COMPUTER
 u16 SetUart = SXS_SET_NB_DATA (SXS_8_DATA)
   | SXS_SET_NB_STOP (SXS_1_STOP)
   | SXS_SET_PARITY (SXS_PARITY_DISABLE)
   | SXS_SET_RATE (SXS_R460800);

 sxs_InitSerial (SetUart, PAL_DBG_PORT_IDX, SXS_RMT_DBG_CPU_IDX);


#ifdef __PROTO_REMOTE__
 for (i=CMN_RR; i< CMN_LAST; i++)
  sxr_SetRmtMbx (i, TRUE);
#endif
#endif// #ifndef NO_REMOTE_COMPUTER

  sxr_StartTask (sxr_NewTask (&sxs_TaskIdleDesc), NIL);
}


/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/

void sxs_TaskIdle (void)
{
 u32 Cnt = 0;
 extern UINT8 g_sys_sleep_flag;
 u8 SerialDataAvail = FALSE;
 TEST_M=7;
 
 while (TRUE)
 {
  TEST_M++;
  hal_TimWatchDogKeepAlive();
  
  sxs_FsIdleHook();

#ifndef NO_REMOTE_COMPUTER
  SerialDataAvail = sxs_SerialFlush ();
#endif
//  hal_SysProcessIdle();
  pal_WDogCntRst ();
//  sxs_SpyData ();
  if(g_sys_sleep_flag)
  {
   hal_LpsSleep();
  }
#ifndef NO_REMOTE_COMPUTER
  sxs_RmtIdleHook ();
#endif
  sxs_DebugIdleHook();

  Cnt++;
 }
}

/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/
int main (int argc, char *argv[])
{
TEST_M=1;

#ifndef NO_REMOTE_COMPUTER
 sxs_SetFnStamp( (u32*)(((u32)_sxs_SetFnStamp) & ~3) );
#endif

TEST_M=2;

 sxs_StartAll ();
TEST_M=3;

 // User specific initialisation
 BAL_TaskInit();


 sxr_CheckTask();
 TEST_M=6;

 //hal_TimWatchDogOpen(10 SECOND);

 sxr_StartScheduling ();

 return 0;
}



