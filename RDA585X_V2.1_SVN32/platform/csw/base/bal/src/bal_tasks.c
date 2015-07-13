/******************************************************************************/
/*              Copyright (C) 2005, CII Technologies, Inc.                    */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    bal_init.c                                                    */
/*                                                                            */
/* Description:                                                               */
/*                   */
/******************************************************************************/
#include "csw.h"
#include "stdkey.h"
#include "event.h"
#include "base_prv.h"
#include "dm.h"
#include "sxr_sbx.h"
#include "chip_id.h"
#include "bal_fstrace.h"
#include "dm_m.h"
#include "dm_audio_ex.h"


BOOL BAL_ApsEvtParse (COS_EVENT* pEvent);
#if 0 
extern UINT32 _enalbe_flash_rw;
UINT32 _enalbe_flash_rw = 1;

BOOL BAL_GetFlashVersion()
{
    return _enalbe_flash_rw;
}
#endif 

//#define CSW_TRACE(x, fmt, ...)   {}

UINT16 g_PreKeyCode = 0;
UINT16 g_PreKeyPressTime = 0;
extern UINT8 DataNb;
extern UINT8 tone_slot;
extern UINT8 gpio_level;
extern VOID DM_CheckPowerOnCause();

#define DM_REGISTER_HANDLER_ARRAY_SIZE DM_DEVID_END_+1

PRIVATE PFN_DEVICE_HANDLER g_devHandlerArray[DM_REGISTER_HANDLER_ARRAY_SIZE];

extern UINT32 _Aps_ServiceEvProc(CFW_EVENT* pEvent, VOID* pUserData);
//extern void TCpIPInit(void);
extern BOOL BAL_ApsTaskMain (COS_EVENT* pEvent);

extern BOOL tm_AlramProc(COS_EVENT* pEv);
extern BOOL pm_PrvInfoInit();
extern BOOL pm_BatteryInit();
extern BOOL dm_PowerOnCheck(UINT16* pCause);
extern BOOL TM_PowerOn();
extern BOOL pm_BatteryMonitorProc(COS_EVENT * pEv);
//
// 
//
BOOL BAL_DevHandlerInit()
{
    UINT8 i = 0;
    
    for(i=0; i< DM_REGISTER_HANDLER_ARRAY_SIZE; i++)
        g_devHandlerArray[i] = NULL;

    g_PreKeyCode = 0;

    return TRUE;
}

//
// Install IRS Handler
//
BOOL DM_RegisterHandler(UINT8 nDeviceId, PFN_DEVICE_HANDLER pfnDevHandler)
{
    UINT32 ret = TRUE; //Modify from ERR_SUCCESS -> TRUE

    g_devHandlerArray[nDeviceId] = pfnDevHandler;    
    
    return ret;
}

BOOL dm_SendGpioMessage( COS_EVENT* pEv)
{
    return TRUE;
}
BOOL turn_key=TRUE;
BOOL dm_SendKeyMessage( COS_EVENT* pEv)
{
#if 0
  if(g_devHandlerArray[DM_DEVID_KEY])
        g_devHandlerArray[DM_DEVID_KEY](pEv);
    else
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return TRUE;
#endif
    
	sxr_SbxInfo_t  Mail;
	Mail.NbAvailMsg=0;
	if (COS_GetDefaultMmiTaskHandle())
	    sxr_GetSbxInfo( ((TASK_HANDLE*)COS_GetDefaultMmiTaskHandle())->nMailBoxId, &Mail);

	if ((Mail.NbAvailMsg >30)&&(turn_key==FALSE)) 
	{
      CSW_TRACE(BASE_BAL_TS_ID, TSTXT("\n !!keyboard mailbox full \n") );
      //turn_key=TRUE; // simon suggest that discard the all key message when the mail box nearly full
      return FALSE;
	}
  if(pEv->nEventId==EV_KEY_DOWN)
      turn_key=TRUE;
  if(pEv->nEventId==EV_KEY_UP)
      turn_key=FALSE;	

	
	if(g_devHandlerArray[DM_DEVID_KEY])
	    g_devHandlerArray[DM_DEVID_KEY](pEv);
	else
	{
	    COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
	return TRUE;
}

BOOL dm_SendDeviceStatusMessage( COS_EVENT* pEv)
{
    if(g_devHandlerArray[DM_DEVID_DM_STATUS]) 
        g_devHandlerArray[DM_DEVID_DM_STATUS](pEv);
    else
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return TRUE;
}

BOOL dm_SendPowerOnMessage( COS_EVENT* pEv)
{
    if(g_devHandlerArray[DM_DEVID_POWER_ON])
        g_devHandlerArray[DM_DEVID_POWER_ON](pEv);
    else
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return TRUE;
}

BOOL dm_SendArlarmMessage( COS_EVENT* pEv)
{
    if(g_devHandlerArray[DM_DEVID_ALARM])
        g_devHandlerArray[DM_DEVID_ALARM](pEv);
    else
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return TRUE;
}

BOOL dm_SendPMMessage( COS_EVENT* pEv)
{
    CSW_TRACE(BASE_BAL_TS_ID, " dm_SendPMMessage()\n");
    if(g_devHandlerArray[DM_DEVID_PM])
    {
        g_devHandlerArray[DM_DEVID_PM](pEv);
    }
    else
    {
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    }

    return TRUE;
}

BOOL dm_SendGpadcMessage( COS_EVENT* pEv)
{
    if(g_devHandlerArray[DM_DEVID_GPADC])
        g_devHandlerArray[DM_DEVID_GPADC](pEv);
    else
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return TRUE;
}
BOOL dm_SendDevTaskessage( COS_EVENT* pEv)
{
    BOOL ret = TRUE;
    
    ret = COS_SendEvent( BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return ret;
}

BOOL dm_StartDevTaskTimer(UINT8 nTimerId, UINT8 nMode, UINT32 nElapse)
{
    return COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) ,nTimerId, nMode,nElapse); 
}

BOOL dm_StopDevTaskTimer(UINT8 nTimerId)
{
   return COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY),nTimerId);
}

//
// Internal Tasks for BAL
//

VOID BAL_SysTask (void* pData)
{
    COS_EVENT ev;
    BOOL bRet;
    
    bRet = TRUE;
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );
    
    CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_SysTask Start. \n") );

    for(;;)
    {
        COS_WaitEvent(BAL_TH(BAL_SYS_TASK_PRIORITY), &ev, COS_WAIT_FOREVER);
    
        if(ev.nEventId == EV_TIMER)
        {
            ev.nEventId  = EV_KEY_PRESS;
            ev.nParam1  = dm_GetStdKey(g_PreKeyCode);
//            ev.nParam2  = 0;//for delete nParam2
//            ev.nParam3  = 0;//for delete nParam3
            COS_SendEvent(COS_GetDefaultMmiTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);            
        }
        else if(ev.nEventId == EV_CSW_PRV_GPIO)
        {
            UINT32 gpio = ev.nParam1;
            CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_SysTask: GPIO[%d] \n"), ev.nParam1);
           // gpio -= 8;//shawn
            if(gpio<=DM_DEVID_GPIO_15)
            {
                if(g_devHandlerArray[gpio])
                {
                    UINT32 data[4]={0,};
                    data[0] = ev.nEventId;
                    data[1] = ev.nParam1;
//                    data[2] = ev.nParam2;//for delete nParam2
                    g_devHandlerArray[gpio]((COS_EVENT*)data);
                }
                else 
                {
                    ev.nEventId = EV_DM_GPIO_IND;
                    ev.nParam1  = gpio;
//                    ev.nParam2  = ev.nParam2;//for delete nParam2
//                    ev.nParam3  = 0;//for delete nParam3
                    COS_SendEvent(COS_GetDefaultMmiTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);                
                }
            }
        }
        else if(ev.nEventId == EV_DM_EXTI_IND)
        {
             CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_SysTask: EXTI \n"), ev.nParam1);
             if(g_devHandlerArray[DM_DEVID_EXTI])
                g_devHandlerArray[DM_DEVID_EXTI](NULL);
             else
             { 
                ev.nEventId = EV_DM_EXTI_IND;
                ev.nParam1  = 0;
 //               ev.nParam2  = 0;//for delete nParam2
//                ev.nParam3  = 0;//for delete nParam3
                COS_SendEvent(COS_GetDefaultMmiTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
            }
        } 
        else if(ev.nEventId == EV_TIM_ALRAM_IND)
        {
            tm_AlramProc(&ev);
        }else if(ev.nEventId == EV_CSW_PRV_RECORD_END)
        {
             CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_SysTask: EXTI \n"), ev.nParam1);
             if(g_devHandlerArray[DM_DEVID_RECORD_END])
                g_devHandlerArray[DM_DEVID_RECORD_END](NULL);
             else
             { 
                ev.nEventId = EV_DM_RECORD_END_IND;
                ev.nParam1  = 0;
  //              ev.nParam2  = 0;//for delete nParam2
 //               ev.nParam3  = 0;//for delete nParam3
                COS_SendEvent(COS_GetDefaultMmiTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
            }

        }else if(ev.nEventId == EV_CSW_PRV_STREAM_END)
        {
             CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_SysTask: EXTI \n"), ev.nParam1);
             if(g_devHandlerArray[DM_DEVID_STREAM_END])
                g_devHandlerArray[DM_DEVID_STREAM_END](NULL);
             else
             { 
                ev.nEventId = EV_DM_STREAM_END_IND;
                ev.nParam1  = 0;
//                ev.nParam2  = 0;//for delete nParam2
//                ev.nParam3  = 0;//for delete nParam3
                COS_SendEvent(COS_GetDefaultMmiTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
            }

        }
        #if 1	
			 else if(ev.nEventId == EV_CSW_PRV_SPI) /*zj add, for spi */
			 {
				 	if(g_devHandlerArray[DM_DEVID_SPI])
				 	{
              UINT32 data[4]={0,};
              data[0] = ev.nEventId;
              data[1] = ev.nParam1;
  //            data[2] = DataNb;
 //             data[2] = ev.nParam2;//for delete nParam2
//              data[3] = ev.nParam3;//for delete nParam3
              g_devHandlerArray[DM_DEVID_SPI]((COS_EVENT*)data);
				 	}
					else
          {
              ev.nEventId = DM_DEVID_SPI;
              ev.nParam1  = 0;
//             ev.nParam2  = 0;//for delete nParam2
//              ev.nParam3  = 0;//for delete nParam3
              COS_SendEvent(COS_GetDefaultMmiTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);                
          }
				   	
			 }
			 #endif

    } // for(;;) loop
}

//
// Device Backgroud task. 
//
VOID BAL_DevMonitorTask (VOID* pData)
{
    COS_EVENT ev;
    UINT16 Cause ;
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );
    //
    // Init battery and charge routines.
    //
    pm_BatteryInit();
    dm_Init();
    dm_PowerOnCheck(&Cause);
    TM_PowerOn();
    CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_DevMonitorTask Start.\n"));

    for(;;)
    {
        COS_WaitEvent(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), &ev, COS_WAIT_FOREVER);
        if(ev.nEventId == EV_TIMER)
        {
            CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_DevMonitorTask: Timer id:%d \n"), ev.nParam1);
            //Change the Timer name
            //if(ev.nParam1==BAL_BATTERY_TIMER1 || ev.nParam1 == BAL_BATTERY_TIMER2 || ev.nParam1 == PRV_DM_LPCLKCALIB_TIMER_ID)
            if(ev.nParam1 == PRV_BAL_BATTERY_TIMER2 || ev.nParam1 == PRV_DM_LPCLKCALIB_TIMER_ID)
            {
                pm_BatteryMonitorProc(&ev);
            }
            else if(ev.nParam1==PRV_DM_AUDIO_TIMER_ID)
            {
                DM_ToneTimerHandlerEx(&ev);
            }
            else if(ev.nParam1==PRV_TM_POWERON_ALM_TIMER_ID)
            {
                tm_AlramProc(&ev);
            }
            #ifdef USE_BUZZER
            else if(ev.nParam1 == PRV_DM_BUZZER_TIMER_ID)
            {
                DM_BuzzerSongMsg(  &ev );   //Longman 09.14, edit 10.26
            }
            #endif

            else if(ev.nParam1 == PRV_CFW_TRACE_TO_FLASH_TIMER_ID)
            {
                bal_fsTraceProcess();
            }
            else if(ev.nParam1 == PRV_CFW_DISPLAY_WAITING_ANIMATION_TIMER_ID)
	     	{
                extern void MMIDisplayWaitingAnimation(void);
                MMIDisplayWaitingAnimation();
	     	}
	     
        }
        else if(ev.nEventId == EV_CSW_PRV_LPS_WAKEUP)
        {
            pm_BatteryMonitorProc(&ev);
        }
        else
        {
            #ifdef USE_BUZZER
            if(ev.nEventId == SYS_EV_MASK_BUZZER_FILE)
            {
                DM_BuzzerSongMsg(  &ev );	//Longman 09.14, edit 10.26
            }
            else
            #endif
            if(ev.nEventId == SYS_EV_MASK_AUDIO_TONE)
            {
                DM_ToneMsgHandlerEx(&ev);
            }
            else if (EV_DM_AUDIO_OPER_MSG_START <= ev.nEventId &&
                     EV_DM_AUDIO_OPER_MSG_END >= ev.nEventId)
            {
                UINT32 result = 0;

                if ( EV_DM_AUDIO_CONTROL_IND == ev.nEventId )
                {
                   if( 0x01 == ev.nParam1 )
                   {
                      result = DM_StartAudioEX();
                   }
                   else
                   {
                      result = DM_StopAudioEX();
                   }
                }
                else if( EV_DM_SET_AUDIO_VOLUME_IND == ev.nEventId )
                {
                  result = DM_SetAudioVolumeEX((DM_SpeakerGain)(ev.nParam1));
                }
                else if( EV_DM_SET_MIC_GAIN_IND == ev.nEventId )
                {
                  result = DM_SetMicGainEX((DM_MicGain)(ev.nParam1));
                }
                else if( EV_DM_SET_AUDIO_MODE_IND == ev.nEventId )
                {
                  result = DM_SetAudioModeEX((UINT8)ev.nParam1);
                }
                else if( EV_DM_AUD_SETUP_IND == ev.nEventId )
                {
                  result = DM_AudSetupEx((AUD_LEVEL_T*)ev.nParam1);
                }
                else if( EV_DM_AUD_STREAM_START_IND == ev.nEventId )
                {
                  result = DM_AudStreamStartEx((DM_AUD_STREAM_START_PARAM_T*)ev.nParam1);
                }
                else if( EV_DM_AUD_STREAM_STOP_IND == ev.nEventId )
                {
                  result = DM_AudStreamStopEx();
                }
                else if( EV_DM_AUD_STREAM_PAUSE_IND == ev.nEventId )
                {
                  result = DM_AudStreamPauseEx((BOOL)ev.nParam1);
                }
                else if( EV_DM_AUD_STREAM_RECORD_IND == ev.nEventId )
                {
                  result = DM_AudStreamRecordEx((DM_AUD_STREAM_RECORD_PARAM_T*)ev.nParam1);
                }
                else if( EV_DM_AUD_TEST_MODE_SETUP_IND == ev.nEventId )
                {
                  result = DM_AudTestModeSetupEx((DM_AUD_TEST_MODE_SETUP_PARAM_T*)ev.nParam1);
                }
                else if( EV_DM_AUD_FORCE_RECV_MIC_SEL_IND == ev.nEventId )
                {
                  result = DM_AudForceReceiverMicSelectionEx((BOOL)ev.nParam1);
                }
                else if( EV_DM_AUD_SET_CUR_OUTPUT_DEVICE_IND == ev.nEventId )
                {
                  result = DM_AudSetCurOutputDeviceEx((SND_ITF_T)ev.nParam1);
                }
                else if( EV_DM_AUD_MUTE_OUTPUT_DEVICE_IND == ev.nEventId )
                {
                  result = DM_AudMuteOutputDeviceEx((BOOL)ev.nParam1);
                }
                else if( EV_DM_AUD_LOUDSPK_WITH_EP_IND == ev.nEventId )
                {
                  result = DM_AudLoudspeakerWithEarpieceEx((BOOL)ev.nParam1);
                }
                else if( EV_DM_VOIS_RECORD_START_IND == ev.nEventId )
                {
                  result = DM_VoisRecordStartEx((DM_VOIS_RECORD_START_PARAM_T*)ev.nParam1);
                }
                else if( EV_DM_VOIS_RECORD_STOP_IND == ev.nEventId )
                {
                  result = DM_VoisRecordStopEx();
                }
                else if( EV_DM_ARS_SETUP_IND == ev.nEventId )
                {
                  result = DM_ArsSetupEx((ARS_AUDIO_CFG_T*)ev.nParam1);
                }
                else if( EV_DM_ARS_RECORD_IND == ev.nEventId )
                {
                  result = DM_ArsRecordEx((DM_ARS_RECORD_PARAM_T*)ev.nParam1);
                }
                else if( EV_DM_ARS_PAUSE_IND == ev.nEventId )
                {
                  result = DM_ArsPauseEx((BOOL)ev.nParam1);
                }
                else if( EV_DM_ARS_STOP_IND == ev.nEventId )
                {
                  result = DM_ArsStopEx();
                }

#if (CFW_AUDIO_CONTROL_BYPASS_SYNC)
                if (EV_DM_AUDIO_CONTROL_IND != ev.nEventId)
#endif
                {
                    // Send the result to the initiator
//                    DM_SendAudOperResult(ev.nParam3, result);//for delete nParam3
                }
            }
            CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_DevMonitorTask: %d \n"), ev.nEventId );
        }
    } // for(;;) loop 
}

PRIVATE HANDLE bal_GetTaskHandle(UINT8 nTaskId)
{
    HANDLE hTask = 0;
    if( nTaskId == CSW_TASK_ID_SYS)
    {
        hTask = BAL_TH(BAL_SYS_TASK_PRIORITY);
    }  
    else if( nTaskId == CSW_TASK_ID_APS)
    {
        hTask = BAL_TH(BAL_CFW_ADV_TASK_PRIORITY);
    }
    else if( nTaskId == CSW_TASK_ID_DEV_MONITOR)
    {
        hTask = BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY);
    } 
    else if( nTaskId == CSW_TASK_ID_MMI_DEFAULT)
    {
        hTask = COS_GetDefaultMmiTaskHandle();
    }
    else if( nTaskId == CSW_TASK_ID_BG)
    {
        hTask = BAL_TH(COS_BK_TASK_PRI);
    }
    
    return hTask;
}

HANDLE CFW_bal_GetTaskHandle(UINT8 nTaskId)
{
    return bal_GetTaskHandle(nTaskId);
}

BOOL BAL_SendTaskEvent(COS_EVENT* pEvent, UINT8 nTaskId)
{
    HANDLE hTask = bal_GetTaskHandle(nTaskId);
      
    return COS_SendEvent(hTask, pEvent,COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

//#define _SPI_CAMERA_ENABLED_
//
// Backgroud task. lower than MMI tasks.
//
//#ifdef _SPI_CAMERA_ENABLED_

#ifdef __PRJ_WITH_SPICAM__ 

#define MSG_MMC_BAL_CAPTURE_REQ 0x1001
extern INT32 sensorready;
extern VOID camerap_Capture_After_Preview(UINT16 Width, UINT16 Height);
extern VOID camerap_GetImageFrame(UINT16 * buff,UINT32 size);

VOID BAL_BackgroundTask ( VOID* pData)
{
    COS_EVENT ev;
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );

    UINT16* imgBuf = NULL;
    UINT32 counts_img=0;
 
    while (1)
    {
        COS_WaitEvent(BAL_TH(COS_BK_TASK_PRI), &ev, COS_WAIT_FOREVER);

        if (ev.nEventId == MSG_MMC_BAL_CAPTURE_REQ)
        {
            imgBuf = (UINT16*)(ev.nParam1);
            hal_HstSendEvent(SYS_EVENT,0x654305);
            camerap_Capture_After_Preview(240,320);

            camerap_GetImageFrame(imgBuf,240*320);
            
         //  pas6175_Capture_After_Preview(128,160);
         //  camerap_GetImageFrame(imgBuf,128*160);
            
          //  for(counts_img=0;counts_img<240*320;counts_img++)
            {
             //imgBuf[counts_img] = (imgBuf[counts_img] >> 8) |(imgBuf[counts_img] << 8);
            }
            sensorready = 1;            
        }
    }
    
}
#elif defined (_CAM_SPECIAL_GC6113_)
extern void mmc_camResize(UINT8* in, UINT32 inSize, UINT8* out);
extern UINT8* gTransBufPtr[2];
extern UINT16* sensorbuf;
extern UINT16 srcwidth;
extern UINT16 srcheight;
extern UINT32 gTransBufSize;
extern INT32 sensorready;

VOID BAL_BackgroundTask ( VOID* pData)
{
    COS_EVENT ev;
    static UINT32 s_count = 0;
    static UINT32 s_countdown = 2;
    const UINT32 s_max = 240*320*2;
    
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );

    UINT8* tmpBuf = NULL;
    while (1)
    {
        COS_WaitEvent(BAL_TH(COS_BK_TASK_PRI), &ev, COS_WAIT_FOREVER);

        switch (ev.nEventId)
        {
            case 1: // MSG_CAM_PART_DATA
                tmpBuf = gTransBufPtr[ev.nParam1];

                mmc_camResize(tmpBuf, gTransBufSize, ((UINT8*)sensorbuf+s_count));
                s_count += (gTransBufSize>>2);
                break;
                
            case 2: // MSG_CAM_FULL_DATA
                if (s_count != s_max)
                {
                    hal_HstSendEvent(SYS_EVENT,0xe1);
                    hal_HstSendEvent(SYS_EVENT,s_count);
                    hal_HstSendEvent(SYS_EVENT,s_max);
                    camera_CaptureImage();
                }
                else
                {
                    if (--s_countdown)
                        camera_CaptureImage();
                    else
                    {
                        s_countdown = 2;
                        sensorready = 1;
                    }
                }
                s_count = 0;
                break;
        }
    }
    
}
#else
VOID BAL_BackgroundTask ( VOID* pData)
{
    COS_EVENT ev;
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );

#if 0 //20060801 bridge b   
    ev.nEventId = EV_DEVICE_STATUS_IND;
    ev.nParam1  = DM_STATUS_FLASH_INIT;
    if(bRet == TRUE)
        ev.nParam2 = 0;
    else
        ev.nParam2 = 1;
    
    dm_SendDeviceStatusMessage(&ev);
#endif ////20060801 bridge e
    for(;;)
    {
        COS_WaitEvent(BAL_TH(COS_BK_TASK_PRI), &ev, COS_WAIT_FOREVER);
        if(ev.nEventId == EV_TIMER)
        {
            ;//ret = CFW_PbkPowerOn(0x02, APP_UTI_SHELL);
        }
        else
        {
          ;
        }
    } 
}
#endif

//
// CFW Advanced task to process advanced stack protocol.
//
#ifdef CFW_TCPIP_SUPPORT

void TCpIPInit(void);
VOID netif_init(VOID);
void APS_Init(void);
VOID tcpip_mem_init(VOID);
void wifi_init(void);
void wifi_open(void);
extern BOOL istcpip_inited;
VOID BAL_CFWApsTask ( VOID* pData)
{

    COS_EVENT ev;
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );
    CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_CFWApsTask Start.\n"));
#ifdef CFW_TCPIP_SUPPORT
    tcpip_mem_init();
//modify 2011-08-24
  			 if(!istcpip_inited)
		     	{
		     	      TCpIPInit();
				netif_init();
				APS_Init();
				istcpip_inited = TRUE;
		     	}

//modify end
#endif
//wifi_init();
//wifi_open();
    for(;;)
    {
        COS_WaitEvent(BAL_TH(BAL_CFW_ADV_TASK_PRIORITY), &ev, COS_WAIT_FOREVER);
#ifdef CFW_TCPIP_SUPPORT
        // BAL_ApsTaskMain(&ev);
	      BAL_ApsEvtParse(&ev);
#endif
        
        if(ev.nEventId == EV_TIMER)
        {
            CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_CFWApsTask,Timer id:%d \n"), ev.nParam1);
        }
        else
        {
            CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_CFWApsTask: %d \n"), ev.nEventId );
        }
    } 

}
#endif


