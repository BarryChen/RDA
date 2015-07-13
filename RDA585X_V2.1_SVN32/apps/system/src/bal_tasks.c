/******************************************************************************/
/*              Copyright (C) 2005, CII Technologies, Inc.                    */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    bal_init.c                                                    */
/*                                                                            */
/* Description:                                                               */
/*                   */
/******************************************************************************/
#include "platform.h"
#include "event.h"
#include "base_prv.h"
#include "ap_common.h"
#include "ap_message.h"
#include "GlobalConstants.h"
#include "bal_config.h"
#include "hal_host.h"
#include "ts.h"
#include "MainTask.h"

UINT16 g_PreKeyCode = 0;
UINT16 g_PreKeyPressTime = 0;
extern VOID DM_CheckPowerOnCause();
extern BOOL g_test_mode;

#define DM_REGISTER_HANDLER_ARRAY_SIZE DM_DEVID_END_+1

PRIVATE PFN_DEVICE_HANDLER g_devHandlerArray[DM_REGISTER_HANDLER_ARRAY_SIZE];

extern UINT8 dm_GetStdKey(UINT8 key);
extern BOOL tm_AlramProc(COS_EVENT* pEv);
extern BOOL pm_PrvInfoInit();
extern BOOL pm_BatteryInit();
extern BOOL dm_PowerOnCheck(UINT16* pCause);
extern BOOL TM_PowerOn();
extern BOOL pm_BatteryMonitorProc(COS_EVENT * pEv);
extern VOID audio_ParseMsg( COS_EVENT* pEvent);
extern UINT8 DM_BuzzerSongMsg( COS_EVENT* pEvent);
extern void COS_HandleExpiredTimers(UINT8 timerHandle);
extern VOID BTSco_WriteData(INT16 *pMicData, INT16 *pSpeakerData);

static UINT8 g_current_key;
static UINT8 g_key_timer;
static UINT8 g_key_hold_flag;
//add by gary start
static UINT8 g_double_key;
UINT8 g_double_key_flag = 0;

UINT8 g_doublekey_timer = 0xff;
//add by gary end

static UINT8 g_irc_current_key;
static UINT8 g_irc_repeat_count;
static UINT8 g_irc_timer;
static UINT8 g_irc_hold_flag;

static UINT8 g_sys_timer_id;
UINT8 g_sys_sleep_flag;
static UINT8 g_sys_timer_id_usb;

static UINT8 UsbStatus = 0;
UINT8 g_usb_active_after_bt;
//extern UINT8 usb_first_frame;

extern UINT8 mcd_usbHostInitOpCompleteCb_status;//added for delete nParam2
extern UINT32 irc_press_mode;//added for delete nParam2
extern UINT32 g_uAudIsPlaying;
extern INT8* Get_BtvoisEncOutput(void);
extern INT8* Get_BtAgcPcmBufferReceiver(void);
EXPORT HAL_HOST_CONFIG_t g_halHostConfig;

PUBLIC VOID bal_SetSleepState(BOOL sleep)
{
	if(sleep != g_sys_sleep_flag)
	{
		g_sys_sleep_flag = sleep;
		if(sleep)
		{
#if APP_SUPPORT_LCD==0
			if(g_halHostConfig.force_nosleep == 0)
			{
				COS_ChangeTimer(g_sys_timer_id, 20000);    // 20s
			}
			else
#endif
				COS_ChangeTimer(g_sys_timer_id, 1000); // 1s
		}
		else
		{
			COS_ChangeTimer(g_sys_timer_id, 100);
		}
	}
}

void BAL_SysTimerTimeout(void *param)
{
}

#if 1//warkey 2.1
UINT8  TF_INSERT_FLAG=0;
UINT8  AUX_INSERT_FLAG=0;
extern INT32 g_current_module;

UINT8 Get_TF_Flag()
{
	return TF_INSERT_FLAG;
}

UINT8 Get_AUX_Flag()
{
	return AUX_INSERT_FLAG;
}

VOID Set_TF_Flag(UINT8 flag)
{
	TF_INSERT_FLAG=flag;
}

VOID Set_AUX_Flag(UINT8 flag)
{
	AUX_INSERT_FLAG=flag;
}

void Delay_Detect_Interrupt()
{
	COS_EVENT ev;
	if(Get_TF_Flag()&&gpio_detect_tcard()&&g_current_module!=FUN_MUSIC)
	{
		app_trace(APP_SETTING_TRC, "Now try to send tf message again!");
		ev.nEventId = EV_FS_TFLASH_DETECT;
		ev.nParam1  = (UINT32)gpio_detect_tcard();
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		COS_SetTimer(2000, Delay_Detect_Interrupt, NULL, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		Set_TF_Flag(0);
	}

	if(Get_AUX_Flag()&&gpio_detect_linein()&&g_current_module!=FUN_LINEIN)
	{
		app_trace(APP_SETTING_TRC, "Now try to send aux message again!");
		ev.nEventId = EV_LINE_IN;
		ev.nParam1  = gpio_detect_linein();
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		COS_SetTimer(2000, Delay_Detect_Interrupt, NULL, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		Set_AUX_Flag(0);
	}
}
#endif

UINT8 g_tflash_detect_flag;

void SendMessageForTFlashDetect(void *param)
{
	COS_EVENT ev;
	g_tflash_detect_flag = 0;
	//if(param)
	{
		ev.nEventId = EV_FS_TFLASH_DETECT;
		ev.nParam1  = (UINT32)gpio_detect_tcard();
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		if(ev.nParam1)
		{
			Set_TF_Flag(1);
			COS_SetTimer(2000, Delay_Detect_Interrupt, NULL, COS_TIMER_MODE_SINGLE);
		}
		else
		{
			Set_TF_Flag(0);
		}
		
		CSW_TRACE(BASE_BAL_TS_ID, "SendMessageForTFlashDetect, param1=%d.", ev.nParam1);
	}
}

UINT8 g_linein_detect_flag;

void SendMessageForLineinDetect(void *param)
{
	COS_EVENT ev;
	
	g_linein_detect_flag = 0;
	//if(param)
	{
		ev.nEventId = EV_LINE_IN;
		ev.nParam1  = gpio_detect_linein();
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		if(ev.nParam1)
		{
			Set_AUX_Flag(1);
			COS_SetTimer(2000, Delay_Detect_Interrupt, NULL, COS_TIMER_MODE_SINGLE);
		}
		else
		{
			Set_AUX_Flag(0);
		}
		
		CSW_TRACE(BASE_BAL_TS_ID, "SendMessageForLINEINDetect, param1=%d.", ev.nParam1);
	}
}

BOOL turn_key = TRUE;
BOOL dm_SendKeyMessage( COS_EVENT* pEv)
{
#if 0
	if(g_devHandlerArray[DM_DEVID_KEY])
	{
		g_devHandlerArray[DM_DEVID_KEY](pEv);
	}
	else
	{
		COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
	return TRUE;
	
	sxr_SbxInfo_t  Mail;
	Mail.NbAvailMsg = 0;
	if (COS_GetDefaultMmiTaskHandle())
	{
		sxr_GetSbxInfo( ((TASK_HANDLE*)COS_GetDefaultMmiTaskHandle())->nMailBoxId, &Mail);
	}
	
	if ((Mail.NbAvailMsg > 30) && (turn_key == FALSE))
	{
		CSW_TRACE(BASE_BAL_TS_ID, TSTXT("\n !!keyboard mailbox full \n") );
		//turn_key=TRUE; // simon suggest that discard the all key message when the mail box nearly full
		return FALSE;
	}
	if(pEv->nEventId == EV_KEY_DOWN)
	{
		turn_key = TRUE;
	}
	if(pEv->nEventId == EV_KEY_UP)
	{
		turn_key = FALSE;
	}
	
	
	if(g_devHandlerArray[DM_DEVID_KEY])
	{
		g_devHandlerArray[DM_DEVID_KEY](pEv);
	}
	else
	{
		COS_SendEvent(COS_GetDefaultMmiTaskHandle(), pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
#endif
	return TRUE;
}

BOOL dm_SendPowerOnMessage( COS_EVENT* pEv)
{
#if 0
	if(g_devHandlerArray[DM_DEVID_POWER_ON])
	{
		g_devHandlerArray[DM_DEVID_POWER_ON](pEv);
	}
	else
#endif
		COS_SendEvent(MOD_APP, pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		
	return TRUE;
}


BOOL dm_SendArlarmMessage( COS_EVENT* pEv)
{
#if 0
	if(g_devHandlerArray[DM_DEVID_ALARM])
	{
		g_devHandlerArray[DM_DEVID_ALARM](pEv);
	}
	else
#endif
		COS_SendEvent(MOD_APP, pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		
	return TRUE;
}

BOOL dm_SendPMMessage( COS_EVENT* pEv)
{
#if 0
	if(g_devHandlerArray[DM_DEVID_PM])
	{
		g_devHandlerArray[DM_DEVID_PM](pEv);
	}
	else
#endif
	{
		COS_SendEvent(MOD_APP, pEv, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
	
	return TRUE;
}

void handle_key_timeout(void *param)
{
	COS_EVENT ev;
	CONST BAL_CFG_KEY_T *p_KeyCfg = tgt_GetKeyCfg();
	
	g_key_hold_flag = 1;
	
	ev.nEventId = EV_KEY_LONG_PRESS;
	ev.nParam1 = g_current_key;
	COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	
	COS_ChangeTimer(g_key_timer, p_KeyCfg->repeat_interval);
}

#ifdef SUPPORT_KEY_FILTER
volatile UINT8 g_key_filter = 0;
static UINT8 g_key_timer_id = 0;
void BAL_ClearKeyFilter()
{
	app_trace(APP_SETTING_TRC, "Clear key filter mask %d", g_key_filter);
	UINT32 csStatus;
	csStatus = hal_SysEnterCriticalSection();
	g_key_filter = 0;
	g_key_timer_id = 0;
	hal_SysExitCriticalSection(csStatus);
}
#endif

//add by gary start
handle_doublekey_timeout()
{
	COS_EVENT ev;
	g_doublekey_timer = 0xff;
	if (g_double_key_flag == 1)
	{
	
		ev.nParam1 = g_double_key;
		ev.nEventId = EV_KEY_PRESS;
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		ev.nEventId = EV_KEY_UP;
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
	g_double_key = 0;
	g_double_key_flag = 0;
}
//add by gary end

void handle_Key_Event(UINT32 event, UINT32 key)
{
	UINT8 k;
	int i;
	COS_EVENT ev;
	UINT32 csStatus;
	CONST BAL_CFG_KEY_T *p_KeyCfg = tgt_GetKeyCfg();
	
	ev.nParam1 = k = p_KeyCfg->keymap[key];
	
	app_trace(APP_SETTING_TRC, "Receive Key message: event=%d, key=%d, current_key=%d", event, key, g_current_key);
#ifdef SUPPORT_KEY_FILTER
	if(g_key_timer_id == 0)
	{
		g_key_timer_id = COS_SetTimer(1000, BAL_ClearKeyFilter, NULL, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		COS_ChangeTimer(g_key_timer_id, 1000);
	}
#endif
	
#ifdef SUPPORT_KEY_FILTER
	if(g_key_timer_id == 0)
	{
		g_key_timer_id = COS_SetTimer(1000, BAL_ClearKeyFilter, NULL, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		COS_ChangeTimer(g_key_timer_id, 1000);
	}
#endif
	
	if(key == 0 && k == KEY_INVALID) // power key is not used
	{
		event = EV_KEY_UP;
	}
	
	if(event == EV_KEY_UP || g_current_key != 0)
	{
		ev.nParam1 = g_current_key;
		g_current_key = 0;
		COS_KillTimer(g_key_timer);
		g_key_timer = 0;
		app_trace(3, "handle_Key_Event111: g_doublekey_timer=%d, k=%d", g_doublekey_timer, k);
		if (g_doublekey_timer != 0xff)
		{
		
			g_double_key_flag ++;
			app_trace(3, "handle_Key_Event222: g_doublekey_timer=%d, k=%d,g_double_key_flag=%d", g_doublekey_timer, k, g_double_key_flag);
			if (g_double_key_flag == 2)
			{
			
				COS_KillTimer(g_doublekey_timer);
				g_doublekey_timer = 0xff;
				g_double_key_flag = 0;
				ev.nParam1 = g_double_key;
				g_double_key = 0;
				ev.nEventId = EV_KEY_DOUBLE_PRESS;
				
				COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
#ifdef SUPPORT_KEY_FILTER
				if(g_key_filter & 0x4)
				{
					app_trace(APP_SETTING_TRC, "Key UP message %d is ignored for %d key is unhandled", key, g_current_key);
					return ;
				}
				csStatus = hal_SysEnterCriticalSection();
				g_key_filter |= 0x4;
				hal_SysExitCriticalSection(csStatus);
#endif
				ev.nEventId = EV_KEY_UP;
				
				COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			}
		}
		else
		{
			if(g_key_hold_flag == 0)
			{
#ifdef SUPPORT_KEY_FILTER
				if(g_key_filter & 0x2)
				{
					app_trace(APP_SETTING_TRC, "Key PRESS message %d is ignored for %d key is unhandled", key, g_current_key);
					return ;
				}
				csStatus = hal_SysEnterCriticalSection();
				g_key_filter |= 0x2;
				hal_SysExitCriticalSection(csStatus);
#endif
				
				ev.nEventId = EV_KEY_PRESS;
				COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			}
			
#ifdef SUPPORT_KEY_FILTER
			if(g_key_filter & 0x4)
			{
				app_trace(APP_SETTING_TRC, "Key UP message %d is ignored for %d key is unhandled", key, g_current_key);
				return ;
			}
			csStatus = hal_SysEnterCriticalSection();
			g_key_filter |= 0x4;
			hal_SysExitCriticalSection(csStatus);
#endif
			ev.nEventId = EV_KEY_UP;
			COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		}
	}
	if(event == EV_KEY_DOWN)
	{
		if(g_current_key == 0) //ignore the current key press,because of more than one key are pressing,xzc
		{
			g_key_hold_flag = 0;
			g_current_key = k;
			
#ifdef SUPPORT_KEY_FILTER
			if(g_key_filter & 0x1)
			{
				app_trace(APP_SETTING_TRC, "Key DOWN message %d is ignored for %d key is unhandled", key, g_current_key);
				return ;
			}
			csStatus = hal_SysEnterCriticalSection();
			g_key_filter |= 0x1;
			hal_SysExitCriticalSection(csStatus);
#endif
			
			ev.nEventId = EV_KEY_DOWN;
			COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			app_trace(3, "handle_Key_Event: g_doublekey_timer=%d, k=%d", g_doublekey_timer, k);
			if(g_doublekey_timer == 0xff)
			{
				g_double_key = k;
				
				g_doublekey_timer = COS_SetTimer(500, handle_doublekey_timeout, NULL, COS_TIMER_MODE_SINGLE);
			}
			g_key_timer = COS_SetTimer(p_KeyCfg->repeat_timeout, handle_key_timeout, NULL, COS_TIMER_MODE_PERIODIC);
		}
	}
}


void handle_irc_timeout(void *param)
{
	COS_EVENT ev;
	
	ev.nParam1 = g_irc_current_key;
	
	if(g_irc_hold_flag == 0)
	{
		ev.nEventId = EV_KEY_PRESS;
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
	ev.nEventId = EV_KEY_UP;
	COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	
	g_irc_timer = 0;
}

void handle_irc_event(UINT32 code, UINT32 mode)
{
	int i, j = 0;
	int key, c;
	COS_EVENT ev;
	BAL_CFG_IRC_T *pIRCCfg = tgt_GetIRCCfg();
	UINT8 *irc_map;
	
	//hal_HstSendEvent(SYS_EVENT,code);
	
	c = (code & 0xffff);
	for(j = 0; j < pIRCCfg->irc_styles; j++)
	{
		if(pIRCCfg->style_list[j] == c)
		{
			break;
		}
	}
	if(j == pIRCCfg->irc_styles)
	{
		return;
	}
	
	key = KEY_INVALID;
	c = (code >> 16) & 0xff;
	irc_map = pIRCCfg->code_map + j;
	for(i = 0; i < pIRCCfg->map_size; i++)
	{
		if(irc_map[i * pIRCCfg->irc_styles] == c)
		{
			key = i + 1;
			break;
		}
	}
	//hal_HstSendEvent(SYS_EVENT,key);
	
	if(g_irc_timer == 0)
	{
		mode = 1;
	}
	ev.nParam1 = key;
	if(mode == 1)
	{
		g_irc_repeat_count = 0;
		g_irc_current_key = key;
		g_irc_hold_flag = 0;
		COS_KillTimer(g_irc_timer);
		g_irc_timer = 0;
		
		if(key != KEY_INVALID)
		{
			g_irc_timer = COS_SetTimer(pIRCCfg->timeout, handle_irc_timeout, NULL, COS_TIMER_MODE_SINGLE);
			ev.nEventId = EV_KEY_DOWN;
			COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		}
	}
	else if(mode == 8)
	{
		if(g_irc_current_key == key && key != KEY_INVALID)
		{
			COS_ChangeTimer(g_irc_timer, pIRCCfg->timeout);
			g_irc_repeat_count ++;
			if(g_irc_repeat_count > 10)
			{
				g_irc_hold_flag = 1;
				g_irc_repeat_count = 0;
				ev.nEventId = EV_KEY_LONG_PRESS;
				COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			}
		}
	}
}


//
// Internal Tasks for BAL
//
VOID umss_TransportEventProcess(UINT32 nParam1);

void hal_UsbDisconnCb(BOOL isHost)
{
	extern BYTE g_usb_connected ;
	
	//hal_HstSendEvent(SYS_EVENT,0x22223333);
	g_usb_connected = 0;
#ifdef MCD_USB_HOST_SUPPORT
	if(isHost)
	{
		COS_EVENT ev;
		Mcd_UsbHostDeInit(0);
		ev.nEventId = EV_FS_USB_PLUG_OUT;
		ev.nParam1  = 0;
		COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
#endif
}

extern UINT32 TEST_M;

extern BYTE g_usb_connected;

int count = 0;
int uart_tx_test = 0;
UINT32 system_usb_ticks = 0;
VOID BAL_SysTimerTimeout_USB()
{
	COS_EVENT ev;
	
	if(gpio_detect_usb())//
	{
		if( g_usb_connected == 1 && UsbStatus == 1)
		{
			//hal_HstSendEvent(SYS_EVENT, 0xa1b2c3d4);
			//hal_HstSendEvent(SYS_EVENT, hal_TimGetUpTime() - system_usb_ticks);
			hal_UsbClose();
			COS_KillTimer(g_sys_timer_id_usb);
			
			//            ev.nEventId = EV_DM_USB_PLUG_IN;
			//            COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		}
	}
	
	
	//        if(g_usb_connected==1 && UsbStatus == 2)
	//        {
	//            COS_KillTimer(g_sys_timer_id_usb);
	//        }
	
	
}

VOID BAL_SysTask (void* pData)
{
	COS_EVENT ev;
	BOOL bRet;
	UINT16 Cause ;
	static UINT8 notify_count = 0;
	//	static int UsbStatus = 0,BatteryStatue=0;
	static int BatteryStatue = 0;
	//    static INT8 usb_rtc_counter = -1;
	BAL_CFG_IRC_T *pIRCCfg = tgt_GetIRCCfg();
	
	g_key_timer = 0;
	g_current_key = 0;
	g_key_hold_flag = 0;
	g_tflash_detect_flag = 0;
	
	bRet = TRUE;
	SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );
	DSM_ReadUserData();
	
#ifdef UART2_SUPPORT
	uart2_start();
#endif
	
	pm_BatteryInit();
	dm_Init();
	dm_PowerOnCheck(&Cause);
	TM_PowerOn();
	
	if(gpio_detect_earpiece())
	{
		DM_SetAudioMode(1);    // default set audio path earpiece
	}
	else
	{
		DM_SetAudioMode(2);    // default set audio path loud speaker
	}
	
	g_sys_timer_id = COS_SetTimer(100, BAL_SysTimerTimeout, NULL, COS_TIMER_MODE_PERIODIC);
	g_sys_sleep_flag = FALSE;
	hal_SysRequestFreq(0, HAL_SYS_FREQ_26M, NULL);
	CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_SysTask Start. \n") );
	
	//    hal_UsbOpen(NULL);
	
	for(;;)
	{
		COS_WaitEvent(MOD_SYS, &ev, COS_WAIT_FOREVER);
		
		switch(ev.nEventId)
		{
#ifdef UART2_SUPPORT
		case EV_UART2_DATA:
			hal_HstSendEvent(SYS_EVENT, 0x00275854);
			uart2_deal_data(ev.nParam1);
			break;
#endif
		case EV_KEY_UP:
		case EV_KEY_DOWN:
		case EV_KEY_PRESS:
		case EV_KEY_LONG_PRESS:
			handle_Key_Event(ev.nEventId, ev.nParam1);
			break;
		case EV_KEY_CALIB:
			ev.nEventId = AP_MSG_KEY_CALIB;
			ev.nParam1 = 0;
			COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			break;
		case EV_IRC_PRESS:
			handle_irc_event(ev.nParam1, irc_press_mode);
			break;
		case EV_TIMER:
			COS_HandleExpiredTimers(ev.nParam1);
			if(ev.nParam1 != g_sys_timer_id) //sys timer is rtc
			{
				break;
			}
			ev.nParam1 = 0;
			if(!g_sys_sleep_flag)
			{
				notify_count++;
				if(notify_count < 5)
				{
					ev.nEventId = 0;
					COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
					break;
				}
				notify_count = 0;
			} // continue for rtc message
#if 0
			if(g_uAudIsPlaying == 1)
			{
				break;
			}
#endif
		case EV_RTC:
			{
				//			static int UsbStatus = 0,BatteryStatue=0;
#ifdef UART2_SUPPORT
				if(uart_tx_test)
				{
					static char buf[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
					uart_tx_test = uart2_send_data(buf, 6);
					hal_HstSendEvent(SYS_EVENT, 0x2013ffff);
					hal_HstSendEvent(SYS_EVENT, uart_tx_test);
					uart_tx_test = 0;
				}
#endif
				rdabt_send_notify();
				ev.nEventId = AP_MSG_RTC;
				COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
				
				if(++BatteryStatue > 4)
				{
					BatteryStatue = 0;
					pm_BatteryMonitorProc(&ev);
				}
				if(g_test_mode)
				{
					hal_HstSendEvent(BOOT_EVENT, 0x7e570010);
				}
				
#if 0
				//     if(gpio_detect_usb())
				if(g_usb_active_after_bt )
				{
					if(UsbStatus == 2)
					{
						hal_HstSendEvent(SYS_EVENT, 0x2013ffdd);
						ev.nEventId = EV_DM_USB_PLUG_IN;
						COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
					}
				}
#endif
#if 1
				if(gpio_detect_usb())
				{
				
					if(UsbStatus == 0)
					{
						hal_HstSendEvent(SYS_EVENT, 0x2013ffee);
						system_usb_ticks = hal_TimGetUpTime();
						hal_UsbOpen(NULL);
						UsbStatus = 1;
						g_sys_timer_id_usb = COS_SetTimer(1, BAL_SysTimerTimeout_USB, NULL, COS_TIMER_MODE_PERIODIC);
					}
					else if( (g_usb_connected == 1 && UsbStatus == 1) || (g_usb_active_after_bt == 1))
					{
						//                hal_HstSendEvent(SYS_EVENT,UsbStatus);
						//                hal_HstSendEvent(SYS_EVENT,g_usb_active_after_bt);
						//     	 hal_UsbClose();//
						UsbStatus = 2;
						hal_HstSendEvent(SYS_EVENT, 0x04170005);
						//	hal_UsbClose();
						//g_usb_connected = 1;
						ev.nEventId = EV_FS_USB_PLUG_DETECT;
						ev.nParam1  = 1;
						//		ev.nParam2  = 0;//for delete nParam2
						COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
						
						if(g_usb_active_after_bt == 1)
						{
							//                                                   g_usb_active_after_bt = 2;
						}
						
					}
				}
				else if(UsbStatus != 0)
				{
					UsbStatus = 0;
					
					ev.nEventId = EV_FS_USB_PLUG_DETECT;
					ev.nParam1  = 0;
					//		ev.nParam2  = 0;//for delete nParam2
					COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
					hal_UsbClose();
				}
#endif
#if 0
				if(usb_rtc_counter >= 0)
				{
					//                     hal_HstSendEvent(SYS_EVENT,0x04174444);
					//                     hal_HstSendEvent(SYS_EVENT,usb_rtc_counter);
					//                     hal_HstSendEvent(SYS_EVENT,gpio_detect_usb());
					if(usb_rtc_counter < 2)
					{
						usb_rtc_counter++;
					}
					else
					{
						if(!gpio_detect_usb() && UsbStatus != 0)
						{
							UsbStatus = 0;
							hal_HstSendEvent(SYS_EVENT, 0x04170006);
							ev.nEventId = EV_FS_USB_PLUG_DETECT;
							ev.nParam1  = 0;
							ev.nParam2  = 0;
							COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
							
							usb_rtc_counter = -1;
							usb_first_frame = 0;//Reinitialize the flag to detect next usb plug-in
						}
					}
				}
#endif
			}
			break;
		case EV_TIM_ALRAM_IND:
		case EV_ALARM:
			ev.nEventId = EV_ALARM;
			COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			break;
		case EV_FS_TFLASH_DETECT:
			{
				CSW_TRACE(BASE_BAL_TS_ID, "EV_FS_TFLASH_DETECT received, param1=%d, flag=%d.", ev.nParam1, g_tflash_detect_flag);
				if(g_tflash_detect_flag == 0)
				{
					if(ev.nParam1 == 0)
					{
						SendMessageForTFlashDetect(ev.nParam1);
					}
					else
					{
						COS_SetTimer(500, SendMessageForTFlashDetect, (void*)ev.nParam1, COS_TIMER_MODE_SINGLE );
						g_tflash_detect_flag = 1;
					}
				}
			}
			break;
#if 0
		case EV_DM_USB_PLUG_IN:
		
			if(UsbStatus == 0)
			{
				//                   mediaSendCommand(MC_STOP, 0);
				hal_HstSendEvent(SYS_EVENT, 0x2013ffee);
				system_usb_ticks = hal_TimGetUpTime();
				//      hal_UsbOpen(NULL);
				UsbStatus = 1;
				g_sys_timer_id_usb = COS_SetTimer(1, BAL_SysTimerTimeout_USB, NULL, COS_TIMER_MODE_PERIODIC);
				
			}
			else if( (g_usb_connected == 1 && UsbStatus == 1) || (g_usb_active_after_bt == 1))
			{
				//                hal_HstSendEvent(SYS_EVENT,UsbStatus);
				//                hal_HstSendEvent(SYS_EVENT,g_usb_active_after_bt);
				//     	 hal_UsbClose();//
				UsbStatus = 2;
				hal_HstSendEvent(SYS_EVENT, 0x04170055);
				//       hal_UsbClose();
				//g_usb_connected = 1;
				ev.nEventId = EV_FS_USB_PLUG_DETECT;
				ev.nParam1  = 1;
				ev.nParam2  = 0;
				COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
				
			}
			
			break;
#endif
		case EV_FS_USB_PLUG_DETECT:
			{
				sxr_Sleep(1000);	/*wait a moment, or else we cann't get the right info*/
				
				hal_UsbSetDisconnCb(hal_UsbDisconnCb);
				if(hal_UsbIsWorkAsHost() == FALSE) // device
				{
					g_usb_connected = ev.nParam1;
					
					hal_HstSendEvent(SYS_EVENT, 0x09061001);
					if(g_usb_connected)
					{
						ev.nEventId = EV_FS_UMSS_DETECT;
						ev.nParam1  = 0;
						//				    ev.nParam2  = 0;//for delete nParam2
						//				    ev.nParam3  = 0;//for delete nParam3
						COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
						//                    usb_first_frame = 1;
						//                    usb_rtc_counter = 0;
					}
					else
					{
						ev.nEventId = EV_FS_UMSS_PLUG_OUT;
						ev.nParam1  = 0;
						//                 ev.nParam2  = 0;//for delete nParam2
						//                 ev.nParam3  = 0;//for delete nParam3
						
						COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
					}
				}
#ifdef MCD_USB_HOST_SUPPORT
				else // host
				{
					hal_HstSendEvent(SYS_EVENT, 0x08150003);
					hal_HstSendEvent(SYS_EVENT, ev.nParam1);
					/*usb device plugged in*/
					if(ev.nParam1)
					{
						Mcd_USBHostInit();
					}
				}
#endif
			}
			break;
		case EV_FS_USB_PLUG_OUT:
			{
				uint32 handle;
				hal_HstSendEvent(SYS_EVENT, 0x09061002);
				ev.nEventId = EV_FS_UDISK_REMOVE;
				ev.nParam1  = 0;
				//  		ev.nParam2  = 0;//for delete nParam2
				//   		ev.nParam3  = 0;//for delete nParam3
				//Mcd_UsbHostDeInit(0);
				//hal_IrqMaskCon(3);
				COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
			}
			break;
		case EV_LINE_IN:
			//hal_HstSendEvent(SYS_EVENT,0x08075000);
			//hal_HstSendEvent(SYS_EVENT,ev.nParam1);
			if(g_linein_detect_flag == 0)
			{
				COS_SetTimer(500, SendMessageForLineinDetect, NULL, COS_TIMER_MODE_SINGLE );
				g_linein_detect_flag = 1;
			}
			break;
		case EV_DM_EAR_PEACE_DETECT_IND:
			//hal_HstSendEvent(SYS_EVENT,0x06140100);
			//hal_HstSendEvent(SYS_EVENT,ev.nParam1);
			if(ev.nParam1)
			{
				DM_SetAudioMode(1); // ear peace
			}
			else
			{
				DM_SetAudioMode(2); // loud speaker
			}
			break;
		case SYS_EV_MASK_AUDIO_TONE:
			{
				//DM_ToneMsgHandlerEx(&ev);
			}
			break;
		case EV_DM_AUDIO_CONTROL_IND:
			{
				if( 0x01 == ev.nParam1 )
				{
					DM_StartAudioEX();
				}
				else
				{
					DM_StopAudioEX();
				}
			}
			break;
		case EV_DM_SET_AUDIO_VOLUME_IND:
			{
				DM_SetAudioVolumeEX(ev.nParam1);
				hal_HstSendEvent(SYS_EVENT, 0x13077777);
			}
			break;
		case EV_DM_SET_MIC_GAIN_IND:
			{
				DM_SetMicGainEX(ev.nParam1);
			}
			break;
		case EV_DM_SET_AUDIO_MODE_IND:
			{
				DM_SetAudioModeEX((UINT8)ev.nParam1);
			}
			break;
			
#if 0
		case EV_FS_ASYN_READ_REQ:
			FS_AsynReadPrc(ev.nParam1);
			break;
		case EV_FS_ASYN_WRITE_REQ:
			FS_AsynWritePrc(ev.nParam1);
			break;
#endif
			/*
			case EV_FS_USB_PLUG_IN:
				if( RDAUsb_ServiceAsHost())
				{
					Mcd_USBHostInit();
				}
			                else
			                {
			                	uctls_Open(0, 0x0000, 0x0000, 0x0000, "USB Controller 1.0");
			                }
				        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_SysTask: %d \n"), ev.nEventId );
				break;
			}
			*/
#ifdef MCD_USB_HOST_SUPPORT
		case EV_FS_USB_INIT:
			Mcd_USBHostSchOperation(ev.nParam1, mcd_usbHostInitOpCompleteCb_status);
			sxr_Sleep(1000);
			//hal_HstSendEvent(SYS_EVENT,0x11070001);
			//hal_HstSendEvent(SYS_EVENT,0x11070002);
			break;
#endif
		case EV_FS_USB_TRANSFERRED:
			{
				//COS_ReleaseSemaphore((HANDLE)ev.nParam1);
			}
			break;
		case EV_USB_MSS_EVENT_ID:
			umss_TransportEventProcess(ev.nParam1);
			break;
#ifdef USB_AUDIO_SUPPORT
		case EV_USB_AUD_EVENT_ID:
			uaud_TransportEventProcess(ev.nParam1);
			break;
#endif
#if APP_SUPPORT_BT_RECORD == 1
		case EV_BT_SCO_REC:
			hal_HstSendEvent(SYS_EVENT, 0x13103101);
			hal_HstSendEvent(SYS_EVENT, ev.nParam1);
			if(ev.nParam1 == 0)
			{
				BTSco_WriteData(Get_BtvoisEncOutput(), Get_BtAgcPcmBufferReceiver());
			}
			else
			{
				BTSco_WriteData(Get_BtvoisEncOutput() + 320, Get_BtAgcPcmBufferReceiver() + 320);
			}
#endif
		default:
			break;
			
		}
		
	} // for(;;) loop
	
}


