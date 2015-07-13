/******************************************************************************/
/*              Copyright (C) 2005, CII Technologies, Inc.                    */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    base_prv.h                                                    */
/*                                                                            */
/* Description:                                                               */
/*   base private procedure declarations, constant definitions and macros.    */
/*                                                                            */
/******************************************************************************/

#if !defined(__BASE_PRV_H__)
#define __BASE_PRV_H__

#include "event.h"
#include "cos.h"
//#include "sxs_type.h"
//#include "sxr_sbx.h"
//#include "cfw_multi_sim.h"
#include "dm.h"
//
// Priority  Based on 211, the following priority should be increase one by one.
//
#define  COS_PRI_BASE             220
#define  COS_PRI_PRV_BASE         211

#define  COS_PRI_BK_BASE          250 // 251 -- 254

#define BAL_SYS_TASK_PRIORITY           (0 + COS_PRI_PRV_BASE)
#define BAL_CFW_ADV_TASK_PRIORITY       (1 + COS_PRI_PRV_BASE)
#define BAL_DEV_MONITOR_TASK_PRIORITY   (2 + COS_PRI_PRV_BASE)
#if (BAL_DEV_MONITOR_TASK_PRIORITY >= (COS_PRI_BASE))
#error 'Error: invalid BAL_XXX_TASK definition!'
#endif

//#define COS_BK_TASK_PRI          (COS_PRI_BK_BASE+1)
#define COS_BK_TASK_PRI          (4 + COS_PRI_PRV_BASE)


#define BAL_TASK_NUM(pri)        ((pri)-COS_PRI_PRV_BASE)

//
// Stack Size
//
#define BAL_SYS_TASK_STACK_SIZE            1024
#define BAL_CFW_TASK_STACK_SIZE            1024
#define BAL_CFW_ADV_TASK_STACK_SIZE        4*1024
#define BAL_DEV_MONITOR_TASK_STACK_SIZE    1024
#define BAL_BG_TASK_STACK_SIZE             1024

// Defined in sxr_sbx.h
//#define SXR_SEND_EVT    (1 << 0)
//#define SXR_SEND_MSG    ( 0 )
//#define SXR_QUEUE_FIRST (1 << 1)
//#define SXR_QUEUE_LAST  ( 0 )

VOID BAL_SysTask (
  VOID* pData
);

VOID BAL_DevMonitorTask (
  VOID* pData
);

VOID BAL_BackgroundTask (
  VOID* pData
);

VOID BAL_CfwApsTask (
  VOID* pData
);

#define CSW_TASK_ID_SYS            0
#define CSW_TASK_ID_APS            1
#define CSW_TASK_ID_DEV_MONITOR    2  
#define CSW_TASK_ID_MMI_DEFAULT    3   
#define CSW_TASK_ID_BG             4   

BOOL SRVAPI BAL_SendTaskEvent(COS_EVENT* pEvent, UINT8 nTaskId);

BOOL dm_SendToDefaultMMITask(COS_EVENT* pev);

BOOL BAL_DevHandlerInit();

HANDLE COS_GetDefaultMmiTaskHandle(VOID);
HANDLE BAL_TH(UINT8 pri);


//
//Add for define the range of timer.
//
//
//CSW BAL Timer 
//
#define PRV_BAL_BATTERY_TIMER1        ( COS_CSW_TIMER_ID_BASE + 1 )
#define PRV_BAL_BATTERY_TIMER2        ( COS_CSW_TIMER_ID_BASE + 2 )

//
//CSW DM Timer 
//
#define PRV_DM_AUDIO_TIMER_ID         ( COS_CSW_TIMER_ID_BASE + 3 )
#define PRV_DM_KEYPAD_TIMER_ID        ( COS_CSW_TIMER_ID_BASE + 4 )
#define PRV_DM_LPCLKCALIB_TIMER_ID    ( COS_CSW_TIMER_ID_BASE + 5 )
#define PRV_DM_BUZZER_TIMER_ID        ( COS_CSW_TIMER_ID_BASE + 6 )

//
//CSW CFW Timer 
//
#define PRV_CFW_IP_TIMER_ID           ( COS_CSW_TIMER_ID_BASE + 7 )
#define PRV_CFW_SIM_TIMER_ID          ( COS_CSW_TIMER_ID_BASE + 8 )
#define PRV_CFW_TCP_TIMER_ID          ( COS_CSW_TIMER_ID_BASE + 9 )
#define PRV_CFW_DNS_TIMER_ID          ( COS_CSW_TIMER_ID_BASE + 10 )
#ifdef CFW_MULTI_SIM
#define PRV_CFW_NW_SIM0_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 11 )
#define PRV_CFW_NW_SIM1_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 12 )
#define PRV_CFW_NW_SIM2_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 13 )
#define PRV_CFW_NW_SIM3_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 14 )
#define PRV_CFW_NW_CS_DELAY_SIM0_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 15)
#define PRV_CFW_NW_CS_DELAY_SIM1_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 16 )
#define PRV_CFW_NW_CS_DELAY_SIM2_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 17)
#define PRV_CFW_NW_CS_DELAY_SIM3_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 18 )
#ifdef CFW_GPRS_SUPPORT

#define CFW_START_GPRSDETACH_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 19)   // add by wuys 2010-05-17
#define CFW_START_GPRSDETACH_TIMER_ID_SIM1     	  ( COS_CSW_TIMER_ID_BASE + 20)   // add by wuys 2010-05-17
#define CFW_START_GPRSDETACH_TIMER_ID_SIM2     	  ( COS_CSW_TIMER_ID_BASE + 21)   // add by wuys 2010-05-17
#define CFW_START_GPRSDETACH_TIMER_ID_SIM3     	  ( COS_CSW_TIMER_ID_BASE + 22)   // add by wuys 2010-05-17

#endif
#else
#define PRV_CFW_NW_TIMER_ID           ( COS_CSW_TIMER_ID_BASE + 11 )
#define PRV_CFW_NW_CS_DELAY_TIMER_ID      ( COS_CSW_TIMER_ID_BASE + 12)
#ifdef CFW_GPRS_SUPPORT

#define CFW_START_GPRSDETACH_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 13)   // add by wuys 2010-05-17
#endif
#endif
#define PRV_CFW_DISPLAY_WAITING_ANIMATION_TIMER_ID   ( COS_CSW_TIMER_ID_BASE + 24 )
//
//CSW Trace 
//
#define PRV_CFW_TRACE_TO_FLASH_TIMER_ID    ( COS_CSW_TIMER_ID_BASE + 25)

//
//CSW TM Timer 
//
#define PRV_TM_POWERON_ALM_TIMER_ID   ( COS_CSW_TIMER_ID_BASE + 30 )


#define SDIO_DHCP_COARSE_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 31)   // add by wuys 2010-05-17
#define SDIO_DHCP_FINE_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 32)   // add by wuys 2010-05-17

#ifdef WIFI_SUPPORT

#define SDIO_POLL_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 33)   // add by wuys 2010-05-17
#define WIFI_CONNECT_AP_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 34)   // add by wuys 2010-05-17
#define WIFI_ASSOC_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 35)   // add by wuys 2010-05-17
#define WIFI_PREASSO_SLEEP_WAIT_TIMER_ID ( COS_CSW_TIMER_ID_BASE + 36) 

#define WIFI_TESTMODE_UART_RX_TIMER_ID ( COS_CSW_TIMER_ID_BASE + 37) 
#define WIFI_SCAN_WAIT_TIMER ( COS_CSW_TIMER_ID_BASE + 38) 
#define WIFI_GET_NETWORK_INFO_TIMER_ID     	  ( COS_CSW_TIMER_ID_BASE + 39)   // add by wuys 2010-05-17

#endif


//
//CSW Max timer
//
#define COS_CSW_TIMER_ID_XXX_END      ( COS_CSW_TIMER_ID_BASE + 50 )

#if (COS_CSW_TIMER_ID_XXX_END >= (COS_CSW_TIMER_ID_END_))
#error 'Error: invalid COS_CSW_TIMER_ID_XXX_END definition!'
#endif



#define MMI_LPCLKCALIB_TIMER (3600 * 16384)



//
// Internal EV ID: 0xF0000001 --  0xFFFFF000.
//
#define EV_CSW_PRV_KEY              (EV_CSW_PRV_BASE+1)
#define EV_CSW_PRV_ALARM            (EV_CSW_PRV_BASE+2)
#define EV_CSW_PRV_GPIO             (EV_CSW_PRV_BASE+3)
#define EV_CSW_PRV_TONE             (EV_CSW_PRV_BASE+4)
#define EV_INTER_APS_TCPIP_REQ      (EV_CSW_PRV_BASE+5)
#define EV_CSW_PRV_RECORD_END       (EV_CSW_PRV_BASE+6)
#define EV_CSW_PRV_STREAM_END       (EV_CSW_PRV_BASE+7)
/*zj add spi*/
#define EV_CSW_PRV_SPI              (EV_CSW_PRV_BASE+8 ) 
#define SYS_EV_MASK_BUZZER_FILE     (EV_CSW_PRV_BASE+9)   //checked 11.23 Longman
#define SYS_EV_MASK_AUDIO_TONE      (EV_CSW_PRV_BASE+10)  //checked 11.23
#define EV_INTER_APS_ACT_REQ        (EV_CSW_PRV_BASE+11)  //mabo added - 20071106
#define EV_INTER_APS_SMTP_REQ       (EV_CSW_PRV_BASE+12)  //add by wuys 2007-08-20
#define EV_INTER_APS_POP3_REQ       (EV_CSW_PRV_BASE+13) 
#define EV_CSW_PRV_LPS_WAKEUP       (EV_CSW_PRV_BASE+14)
#define EV_APS_PPP_DATA_REQ         (EV_CSW_PRV_BASE+15)  //add by wuys 2004-04-15
#define EV_CSW_PRV_XXX_END          (EV_CSW_PRV_BASE+16)

#if (EV_CSW_PRV_XXX_END >= (EV_CSW_PRV_END_))
#error 'Error: invalid EV_CSW_PRV_XXX_END definition!'
#endif

//
// Internal Malloc or Free by CSW only.
//
/*PVOID SRVAPI CSW_Malloc  (
  UINT32 nSize
);*/

PVOID CSW_StkMalloc  (
  UINT32 nSize
);

/*BOOL SRVAPI CSW_Free ( 
  PVOID pMemBlock
);*/


#ifdef CFW_MULTI_SIM

UINT32 CFW_BalSendMessage(
    UINT32 nMailBoxId, 
    UINT32 nEventId, 
    VOID* pData,
    UINT32 nSize,
    CFW_SIM_ID nSimID
);
UINT32 SRVAPI CFW_SendSclData(
    UINT8 nNsapi,
    UINT8 nQueueId,
    BOOL bMode, 
    UINT16 nDataSize, 
    UINT8 *pData,
    CFW_SIM_ID nSimID
);
#else
UINT32 CFW_BalSendMessage(
    UINT32 nMailBoxId, 
    UINT32 nEventId, 
    VOID* pData,
    UINT32 nSize
);
UINT32 SRVAPI CFW_SendSclData(
    UINT8 nNsapi,
    UINT8 nQueueId,
    BOOL bMode, 
    UINT16 nDataSize, 
    UINT8 *pData
);
#endif


#ifdef _DEBUG
PVOID SRVAPI  CSW_MallocDbg(UINT32 nModuleId, UINT32 nSize, CONST UINT8* pszFileName, UINT32 nLine);
BOOL SRVAPI CSW_FreeDbg(UINT32 nModuleId,VOID* pMemAddr, CONST UINT8* pszFileName, UINT32 nLine);
#define CSW_MALLOC(m,size)  CSW_MallocDbg(m, (UINT32)(size), __FILE__, __LINE__)
#define CSW_FREE(m,p)       CSW_FreeDbg(m, (PVOID)(p), __FILE__, __LINE__)
#else
#define CSW_MALLOC(m,size)  COS_Malloc((UINT32)(size))
#define CSW_FREE(m,p)       COS_Free((PVOID)(p))

#endif

#define DRV_GPIO(n)			    (1<<(n))
#define DRV_CHRG_CMD     		DRV_GPIO(5)
#define DRV_DC_ON_DETECT	    DRV_GPIO(13)
#define DRV_ON_OFF_KEY	        DRV_GPIO(9)


BOOL dm_SendPMMessage( COS_EVENT* pEv);
BOOL dm_SendGpadcMessage( COS_EVENT* pEv);
BOOL dm_SendArlarmMessage( COS_EVENT* pEv);
BOOL dm_SendPowerOnMessage( COS_EVENT* pEv);
BOOL dm_SendKeyMessage( COS_EVENT* pEv);

#endif // _H_ 

