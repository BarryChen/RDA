
/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_gui.h"
#include "event.h"
#include "ap_bluetooth.h"
#include "MainTask.h"//20130515
#ifdef SUPPORT_KEY_FILTER
#include "hal_sys.h"
#endif

typedef UINT32 (*Key_func)(UINT32);

UINT8 g_shutdown_tm_handle = 0;

UINT8 g_light_time = 0;   //�ر���ʱ��,0.5��Ϊ��λ
UINT16 g_standby_time = 0; //�Զ��ػ�ʱ��,0.5��Ϊ��λ
UINT16 g_rtc_counter = 0;  //rtc ��Ϣ����
UINT16 g_ap_event = 0;     //�¼���־
UINT16 g_bat_time = 0;
UINT8 time_id_2hz = 0;
UINT8 g_key_hold_flag = 0;
UINT8 g_mute_flag = 0;
Key_func g_custom_message = NULL;

extern BOOL ui_run_realtime;
extern UINT8 g_play_disk;
#if APP_SUPPORT_BLUETOOTH==1
extern bt_state_t g_bt_state;
#endif
extern INT32           g_current_module;
#if XDL_APP_SUPPORT_TONE_TIP==1
extern UINT8 lowpower_flag_for_tone;
#endif

#if  XDL_APP_BT_MSG_PRINTF==1//howe 2.1
INT8 A2DP_Printf[][35] = {"A2DP_SIGNAL_CONNECT_CNF",
                          "A2DP_SIGNAL_DISCONN_CNF",
                          "A2DP_MEDIA_CONNECT_CNF",
                          "A2DP_MEDIA_DISCONN_CNF",
                          "A2DP_SEND_SEP_DISCOVER_CNF",
                          "A2DP_SEND_SEP_GET_CAPABILITIES_CNF",
                          "A2DP_SEND_SEP_SET_CONFIG_CNF",
                          "A2DP_SEND_SEP_RECONFIG_CNF",
                          "A2DP_SEND_SEP_OPEN_CNF",
                          "A2DP_SEND_SEP_START_CNF",
                          "A2DP_SEND_SEP_PAUSE_CNF",
                          "A2DP_SEND_SEP_CLOSE_CNF",
                          "A2DP_SEND_SEP_ABORT_CNF",
                          "A2DP_ACTIVATE_CNF",
                          "A2DP_DEACTIVATE_CNF",
                          "A2DP_SIGNAL_CONNECT_IND",
                          "A2DP_SIGNAL_DISCONNECT_IND",
                          "A2DP_MEDIA_CONNECT_IND",
                          "A2DP_MEDIA_DISCONNECT_IND",
                          "A2DP_MEDIA_DATA_IND",
                          "A2DP_SEND_SEP_DISCOVER_IND",
                          "A2DP_SEND_SEP_GET_CAPABILITIES_IND",
                          "A2DP_SEND_SEP_SET_CONFIG_IND",
                          "A2DP_SEND_SEP_RECONFIG_IND",
                          "A2DP_SEND_SEP_OPEN_IND",
                          "A2DP_SEND_SEP_START_IND",
                          "A2DP_SEND_SEP_PAUSE_IND",
                          "A2DP_SEND_SEP_CLOSE_IND",
                          "A2DP_SEND_SEP_ABORT_IND"
                         };

INT8 HFP_Printf[][28] = {"HFG_ACTIVATE_CNF",
                         "HFG_DEACTIVATE_CNF",
                         "HFG_ACL_CONNECT_REQ",
                         "HFG_ACL_CONNECT_IND",
                         "HFG_ACL_DISCONNECT_IND",
                         "HFG_CHANNEL_CONNECT_IND",
                         "HFG_SERVER_CHANNEL_IND",
                         "HFG_CKPD_FROM_HS_IND",
                         "HFG_DATA_IND",
                         "HFG_RING_IND",			// send to hf to indicate incoming call
                         "HFG_CALL_RESPONSE_IND",	// send to ag to indicate hf accept, reject or hold the incoming call
                         "HFG_CALL_REQ_IND",		// send to ag to indicate the hf initialise call
                         "HFG_CALL_STATUS_IND",	// send to hf to indicate the call status
                         "HFG_CALL_CLCC_CNF",
                         "HFG_NO_CARRIER_IND",
                         "HFG_CALL_WAIT_3WAY_IND", // send to hf and ag to indicate call wait and three way call status
                         "HFG_VOICE_RECOGNITION_IND", // send to hf and ag to indicate voice recognition status
                         "HFG_PHONE_STATUS_IND",	 // send to hf to indicate the phone status
                         "HFG_NETWORK_OPERATOR_IND",	 // send to hf the Network Operator string
                         "HFG_SPEAKER_GAIN_IND",	   // send to hf and ag to indicate the speaker gain
                         "HFG_MIC_GAIN_IND",		   // send to hf and ag to indicate the mic gain
                         "HFG_PHONE_BOOK_IND",
                         "HFG_INCOMING_HISTORY_IND",
                         "HFG_OUTCOMING_HISTORY_IND",
                         "HFG_MISSED_CALL_HISTORY_IND",
                         "HFG_BATTERY_OK_IND"
                        };

INT8 AVRCP_Printf[][24] =
{
	"AVRCP_ACTIVATE_CNF",
	"AVRCP_DEACTIVATE_CNF",
	"AVRCP_CONNECT_CNF",
	"AVRCP_CONNECT_IND",
	"AVRCP_DISCONNECT_CNF",
	"AVRCP_DISCONNECT_IND",
	"AVRCP_CMD_FRAME_CNF",
	"AVRCP_CMD_FRAME_IND",
	"AVRCP_PLAYER_STATUS_IND",
	"AVRCP_VENDOR_DATA_IND"
};
#endif /* #if XDL_APP_MSG_PRINFT */
void shutdown_timeout(void *param);

void TimeOut_2HZ(void *param)
{
	//MESSAGE_SetEvent(AP_MSG_RTC);
}


/*********************************************************************************
* Description : ��ʼ����Ϣ����
*
* Arguments   : comval, ϵͳ�趨ֵ, =null ��ʾֻ��ʼ���ڲ�������
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
void MESSAGE_Initial(const comval_t *comval)
{
	APP_Wakeup();
	if(comval != NULL)
	{
		g_light_time   = comval->LightTime * 2;
		g_standby_time = comval->StandbyTime * 60;//comval->StandbyTime * 60 * 2;
	}
	//if(time_id_2hz == 0)
	//    time_id_2hz = COS_SetTimer(400, TimeOut_2HZ, NULL, COS_TIMER_MODE_PERIODIC);
	
	if(g_light_time != 0)
	{
		OpenBacklight();
	}
	
	g_rtc_counter = 0;
	g_bat_time = 0;//10; // 5 second update battery
	
	if(!g_shutdown_tm_handle && !g_standby_time)
	{
		g_shutdown_tm_handle = COS_SetTimer(g_standby_time * 1000, shutdown_timeout, NULL, COS_TIMER_MODE_SINGLE);
	}
	COS_ChangeTimerUseSecond(g_shutdown_tm_handle, g_standby_time);
}


/*********************************************************************************
* Description : ����ϵͳ��Ϣ, ���ذ�����Ϣ, ͬʱ����/�ر���
*
* Arguments   : key, ϵͳ��Ϣ
*
* Returns     : ����ap�ܹ�ʶ�����Ϣ, ���û��ϵͳ��Ϣ, ���� AP_KEY_NULL
*
* Notes       :

* ����ʱ��:
 *  0.....1.2....1.5........... (s)
 *  key    long   hold    up    (>1.2s)
 *  key  up            (<1.2s)
 *
 * key rate: 0.3s
*
*********************************************************************************/
BOOL is_first_batt = TRUE;
UINT8 batt_timer = 0;

void Batt_Timeout(void *param)
{
	//    media_PlayInternalAudio(GUI_AUDIO_LOWPOWER, 1, FALSE);
	LED_SetPattern(GUI_LED_LOWPOWER, 1);
}

void shutdown_timeout(void *param)
{
	g_shutdown_tm_handle = 0;
	MESSAGE_SetEvent(AP_MSG_STANDBY);
}

UINT32 MESSAGE_Key(UINT32 key)
{
	INT32 batt;
	switch( key )
	{
	case AP_KEY_UP:
		g_key_hold_flag = 0;
	case AP_KEY_DOWN:
	case AP_KEY_HOLD:
	case AP_MSG_USB_PLUGIN:        //usb plugin, ��ͬ����
	case AP_MSG_USB_PULLOUT:
	case AP_MSG_SD_IN:
	case AP_MSG_SD_OUT:
		//������
		APP_Wakeup();
		if(g_light_time != 0)
		{
			OpenBacklight();
		}
		g_rtc_counter = 0;
		
		if(g_shutdown_tm_handle)
		{
			COS_KillTimer(g_shutdown_tm_handle);
			g_shutdown_tm_handle = 0;
		}
		break;
		
	case AP_MSG_RTC:           //rtc
		g_rtc_counter++;
#if APP_SUPPORT_BLUETOOTH==1
		BT_UpdateLed();
#endif
#if 0//warkey 2.1
		batt = GetBattery();
		if(batt == -1)
		{
			GUI_DisplayBattaryLever();
		}
		else if(g_bat_time != 0 && (g_rtc_counter % g_bat_time) == 0)
		{
#if APP_SUPPORT_BTBATTERY==1
			BT_HFPSendBattery();
#endif
			if(batt == 0)
			{
				key = AP_MSG_LOW_POWER;
				//                media_PlayInternalAudio(GUI_AUDIO_LOWPOWER, 1, FALSE);
			}
			else if(batt < 15 && is_first_batt)
			{
				is_first_batt = FALSE;
				Batt_Timeout(NULL);
				batt_timer = COS_SetTimer(10000, Batt_Timeout, NULL, COS_TIMER_MODE_PERIODIC);
			}
			else if(batt >= 15)
			{
				if(batt_timer)
				{
					COS_KillTimer(batt_timer);
					batt_timer = 0;
				}
				is_first_batt = TRUE;
			}
			GUI_DisplayBattaryLever();
		}
#else
		g_bat_time++;
		if(g_bat_time%120==0)
		{
			BT_HFPSendBattery();
			g_bat_time=0;
		}
		if(g_light_time==0&&g_rtc_counter>30000)
		{
			g_rtc_counter=13;
		}
#endif
		if(g_light_time != 0 && g_rtc_counter == g_light_time)    //�Զ��ر���
		{
			//�ر���
			CloseBacklight();
		}
		/*
		if(g_standby_time != 0 && g_rtc_counter == g_standby_time) //�Զ��ػ�
		{
		    return AP_MSG_STANDBY;
		}
		*/
		if(g_rtc_counter == 12)
		{
			if(!g_shutdown_tm_handle && !g_standby_time)
			{
				g_shutdown_tm_handle = COS_SetTimer(g_standby_time * 1000, shutdown_timeout, NULL, COS_TIMER_MODE_SINGLE);
			}
			COS_ChangeTimerUseSecond(g_shutdown_tm_handle, g_standby_time);
			return AP_MSG_WAIT_TIMEOUT;    // 6 sec timeout
		}
		
		break;
		
	case MSG_LOW_POWER:    //low power, nmi 0.98v
		//APP_DUMP("low power", 0);
		break;
		
	} //switch()
	return key;
}


/*********************************************************************************
* Description : �����¼�������־
*
* Arguments   : event, �¼�����
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
void MESSAGE_SetEvent(UINT32 event)
{
	COS_EVENT ev = {0};
	
	ev.nEventId = event & 0xffff;
	ev.nParam1 = (event >> 16);
	COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

#ifdef SUPPORT_KEY_FILTER
extern volatile UINT8 g_key_filter;
#endif

/*********************************************************************************
* Description : ˯��ָ����ʱ��
*
* Arguments   : half_sec, ʱ��, 0.5��Ϊ��λ
*
* Returns     : ���ָ����ʱ����û��������Ϣ����, ����0
        ���ָ����ʱ������������Ϣ����, ���ظ���Ϣ
* Notes       : ���ָ����ʱ������������Ϣ����, ˯�ߵ�ʱ�佫< ָ����ʱ��
*
*********************************************************************************/
UINT32 MESSAGE_Wait(void)
{
	//  INT32 n=0;    //rtc counter
	UINT32 key;
	COS_EVENT ev;
	UINT32 csStatus;
	
	COS_WaitEvent(MOD_APP, &ev, COS_WAIT_FOREVER);
	
	//if(ev.nEventId != 0 && ev.nEventId!=AP_MSG_RTC)
	//    app_trace(APP_MAIN_TRC, "Receive Event 0x%x, param1=%x, param2=%x, param3=%x",ev.nEventId, ev.nParam1, ev.nParam2, ev.nParam3);
	
	//    key = (ev.nParam1<<16) | ((ev.nEventId)&0xffff);
#ifdef SUPPORT_KEY_FILTER
	csStatus = hal_SysEnterCriticalSection();
	switch(ev.nEventId)
	{
	case EV_KEY_DOWN:
		g_key_filter &= ~(0x1);
		break;
	case EV_KEY_PRESS:
		g_key_filter &= ~(0x2);
		break;
	case EV_KEY_UP:
		g_key_filter &= ~(0x4);
		break;
	default:
		break;
	}
	hal_SysExitCriticalSection(csStatus);
#endif
	
	
	key = (ev.nParam1 << 16) | (MESSAGE_Key(ev.nEventId) & 0xffff);
	
#ifdef SUPPORT_PREV_NEXT_SWAP//warkey 2.1 //��PREV/NEXT��������ΪVSUB/VADD�̰�
	if(key == (AP_KEY_PREV | AP_KEY_HOLD))
	{
		key = (AP_KEY_VOLSUB | AP_KEY_DOWN);
	}
	else if(key == (AP_KEY_NEXT | AP_KEY_HOLD))
	{
		key = (AP_KEY_VOLADD | AP_KEY_DOWN);
	}
#endif
	
	if(g_custom_message != NULL)
	{
		key = g_custom_message(key);
	}
	
	return key;
	
	/*    while(1)
	    {
	        key = MESSAGE_Get();
	        if(key == AP_MSG_RTC || key == AP_MSG_CHARGING || key == AP_MSG_WAIT_TIMEOUT)    //sleep ʱ����timeout
	        {
	            n++;
	            if(n >= half_sec)
	            	return 0;
	        }
	        else if(key == AP_KEY_NULL || key == AP_MSG_LOCK || key == (AP_MSG_LOCK | AP_KEY_UP))    //lock ֻ��up, û��long/hold
	        {
	            continue;
	        }
	        else
	        {
	            //here maybe hotkey
	            return MESSAGE_HandleHotkey(key);
	        }
	    }*/
	
}




/*********************************************************************************
* Description : �����ȼ���Ϣ, ���ؽ����Ϣ
*
* Arguments   : key, ������Ϣ
*
* Returns     : ����п�ʶ����ȼ���Ϣ,  ���ؽ����Ϣ
        ���û�п�ʶ����ȼ���Ϣ,  ����0
*
* Notes       :
*
*********************************************************************************/
UINT32 MESSAGE_Hotkey(UINT32 key)
{
	switch( key )
	{
	case AP_KEY_POWER  | AP_KEY_HOLD:
		if(MESSAGE_IsHold())
		{
		hal_HstSendEvent(APP_EVENT, 0x201500FF);
		return RESULT_STANDBY;
		}
		break;
#ifndef MMI_ON_WIN32
		{
			// enter usb download mode
			hal_SysSetBootMode((1 << 1)); // BOOT_MODE_FORCE_MONITOR
			hal_SysRestart();
		}
#endif
		break;
	//    case AP_KEY_PLAY | AP_KEY_LONG:    //����play��standby
	case AP_MSG_STANDBY:      //�Զ��ػ�
#if APP_SUPPORT_BLUETOOTH==1
		if(g_bt_state >= BT_CONNECTED)
		{
			return 0;
		}
#endif
	/*case AP_KEY_POWER  | AP_KEY_PRESS:
#if XDL_APP_SUPPORT_TONE_TIP==1
		lowpower_flag_for_tone = 4;
		MESSAGE_SetEvent(0xaaaa1111);
		return 0;
#endif*/

	case 0xad11dead:
		hal_HstSendEvent(APP_EVENT, 0x2013dead);
	//case AP_MSG_SLEEP:        //˯��
	//case AP_MSG_LOW_POWER:    //�͵��Զ��ػ�
	case AP_MSG_FORCE_STANDBY:    //ʹ���������ᱨlabel duplicate
		//MESSAGE_SetEvent(AP_EVENT_STANDBY);
		return RESULT_STANDBY;
	case AP_KEY_MODE|AP_KEY_PRESS:// press mode return to main | AP_KEY_LONG:    //����menu����main
		//MESSAGE_SetEvent(AP_EVENT_MAIN);
		if(!AP_Support_MENU())
		{
			return RESULT_MAIN;
		}
		else
		{
			return 0;
		}
	case AP_MSG_KEY_CALIB:
		hal_HstSendEvent(SYS_EVENT, 0x05130100);
		return RESULT_KEYCALIB;
		break;
	case AP_KEY_MUTE|AP_KEY_PRESS:
		g_mute_flag = !g_mute_flag;
		SetPAMute(g_mute_flag);
		return 0;
	case AP_MSG_LINE_IN:
		return RESULT_LINE_IN;
	case AP_MSG_ALARM:
		return RESULT_ALARM;
		//    case AP_KEY_REC | AP_KEY_PRESS:    //����¼��
		//MESSAGE_SetEvent(AP_EVENT_RECORD);
		//        return RESULT_RECORD_START;
#if APP_SUPPORT_USBDEVICE
	case AP_MSG_USB_CONNECT:        //����udisk ģʽ
		//        MESSAGE_SetEvent(AP_EVENT_UDISK);
		if(g_current_module != FUN_USB)
		{
			hal_HstSendEvent(SYS_EVENT, 0x09070002);
			return RESULT_UDISK;
		}
		else
		{
			hal_HstSendEvent(SYS_EVENT, 0x09070003);
			return 0;
		}
	// return and enter music
	case AP_MSG_USB_DISCONNECT:        //�˳�udiskģʽ
		uctls_Close();
		if(g_current_module == FUN_USB)//20130515
		{
			return RESULT_EXIT_UDISK;
		}
		else
		{
			return 0;
		}
#endif
#if APP_SUPPORT_USB
	case AP_MSG_USB_PLUGIN:
		g_play_disk = FS_DEV_TYPE_USBDISK;
		return RESULT_MUSIC;
#endif
	case AP_MSG_SD_IN:
		g_play_disk = FS_DEV_TYPE_TFLASH;
		return RESULT_MUSIC;
	case AP_MSG_BT_ACTIVE:
		return RESULT_BT_ACTIVE;
	case AP_MSG_LOCK:        //locked
		if(ui_run_realtime)    //ʵʱģʽ����ʾlock
		{
			//TODO: send lock message to message queue
			return 0;
		}
		else
		{
			GUI_DisplayLock(1);    //���ﲻ�����ȼ�,��ֹǶ��
			//MESSAGE_SetEvent(AP_EVENT_REDRAW);
			return RESULT_REDRAW;
		}
	//  case AP_KEY_UNLOCK:
	//      return GUI_DisplayLock(0);
	default:
		//here just drop the key
		return 0;
	}
	return 0;
}

/******************************************************* **************************
* Description : ����ϵͳ��Ϣ, ���ذ�����Ϣ, ͬʱ����/�ر���
		ap_get_message_core() �����
*
* Arguments   :
*
* Returns     : ����ap�ܹ�ʶ�����Ϣ, ���û��ϵͳ��Ϣ, ���� AP_KEY_NULL
*
* Notes       :
*********************************************************************************/
UINT32 MESSAGE_Sleep(INT32 half_sec)
{
	INT i = 0;
	UINT32 key;
#if 0
	COS_Sleep(500 * half_sec);
	
	return 0;
#else
	for(i = 0; i < half_sec; )
	{
		key = MESSAGE_Wait();
		switch(key)
		{
		case AP_KEY_NULL:
		case AP_MSG_WAIT_TIMEOUT:
			break;
		case AP_MSG_RTC:
		case AP_MSG_CHARGING:
			i++;
			break;
		default:
			return key;
			break;
		}
	}
	return key;
#endif
}
UINT32 MESSAGE_Sleep_EXT(INT32 half_sec)
{
	INT i = 0;
	UINT32 key;
#if 0
	COS_Sleep(500 * half_sec);
	
	return 0;
#else
	for(i = 0; i < half_sec; )
	{
		key = MESSAGE_Wait();
		switch(key)
		{
		case AP_MSG_RTC:
		case AP_MSG_CHARGING:
			i++;
			break;
	
		default:
			break;
		}
	}
	return key;
#endif
}

/*********************************************************************************
* Description : �����ȼ���Ϣ, ���ؽ����Ϣ
		ap_handle_hotkey_core() �����
*
* Arguments   : key, ������Ϣ
*
* Returns     : ����п�ʶ����ȼ���Ϣ,  ���ؽ����Ϣ
		���û�п�ʶ����ȼ���Ϣ,  ����0
*
* Notes       :
*
*********************************************************************************/
UINT32 MESSAGE_HandleHotkey(UINT32 key)
{
	if(key == AP_KEY_NULL)
	{
		return 0;
	}
	else
	{
		return MESSAGE_Hotkey(key);
	}
}


UINT32 MESSAGE_IsNumberKey(UINT32 key)
{
	if( ((key & 0xffff) == AP_KEY_DOWN) && ((key & 0xffff0000) >= AP_KEY_NUM1) && (key & 0xffff0000) <= (AP_KEY_DOWN | AP_KEY_NUM0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


UINT32 MESSAGE_IsHold(void)
{
	if(g_key_hold_flag == 0)
	{
		g_key_hold_flag = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}
#if  XDL_APP_BT_MSG_PRINTF==1//howe 2.1
UINT32 A2DP_DateIn = 0;

void msg_Printf(unsigned short msg_id )
{
	unsigned short i, k;
	i = msg_id & 0xFF00;
	k = msg_id & 0xFF;
	switch(i)
	{
	case 0xF00://HFP
		app_trace(16, "msg_id==>%s\r\n", HFP_Printf[k]);
		break;
		
	case 0xD00://A2DP
		if(k != 0x13)
		{
			if(A2DP_DateIn != 0)
			{
				app_trace(16, "msg_id==>End Coming:A2DP_MEDIA_DATA_IND %d", A2DP_DateIn);
				A2DP_DateIn = 0;
			}
			app_trace(16, "msg_id==>%s\r\n", A2DP_Printf[k]);
		}
		else if(A2DP_DateIn == 0 && k == 0x13)
		{
			A2DP_DateIn = 1;
			app_trace(16, "msg_id==>Begin Coming:A2DP_MEDIA_DATA_IND !!");
		}
		else if(A2DP_DateIn > 0 && k == 0x13)
		{
			A2DP_DateIn ++;
		}
		break;
		
	case 0xE00://AVRCP
		app_trace(16, "msg_id==>%s\r\n", AVRCP_Printf[k]);
		break;
		
	default:
		break;
	}
}
#endif /* #if XDL_APP_MSG_PRINFT */

