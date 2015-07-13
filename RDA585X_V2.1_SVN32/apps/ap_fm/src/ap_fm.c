/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_fm.h"
#include "ap_gui.h"
#include "tm.h"

#if APP_SUPPORT_FM

#define FM_TX_TIMEOUT               (3000)

#define FM_AUTO_SEARCH              AP_MSG_USER+1
#define FM_SAVE_FREQ                AP_MSG_USER+2
#define FM_DELETE_FREQ              AP_MSG_USER+3
#define FM_DELALL_FREQ              AP_MSG_USER+4
#define FM_SET_BAND                 AP_MSG_USER+5
#define FM_FREQ_LIST                AP_MSG_USER+6
#define FM_ADJUST_CLOCK             AP_MSG_USER+7

extern slider_t            slider;

extern BOOL ui_auto_update;
extern INT8 g_timer_setting_flag;
#if XDL_APP_SUPPORT_TONE_TIP==1
extern UINT8 lowpower_flag_for_tone;
#endif

FM_play_status_t    *FMStatus;                //当前电台信息
BOOL edit_station = FALSE;
BOOL output_flag = FALSE;

INT8 fm_item_buf[12];
static UINT32 current_time;

//当前状态
enum
{
	fm_play,
	fm_search,
	fm_output
}	g_fm_status = fm_play;

//标志：need_draw，重画 BandSwitch，频段切换 FirstWait，刚进AP
static BOOL need_draw = TRUE, BandSwitch = TRUE, FirstFlag = TRUE;

//电台频率对照表(注:频率范围87M-108M（US）或76M-90M（Japan）,以K为精度要三位数).
static const UINT32 freq_scope [FMD_BAND_QTY][2] = {{87000, 108000}, {76000, 90000}, {76000, 108000}, {65000, 76000}};
BOOL fm_mute = FALSE;
/*********************************************************************************
* Description : 音量设置回调函数
*
* Arguments   :
        param:
            0, 开始时关标停在第0项
*
* Returns     : 结果消息或0
*
* Notes       :
*
*********************************************************************************/
void  FM_VolumeCb(INT32 Vol)
{
	//hal_HstSendEvent(SYS_EVENT,0xFF100000 + Vol);
	FM_SendCommand(MC_SETVOLUME, Vol);
}


INT32 fm_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		return 0;
	}
	
	switch(value)
	{
#if APP_SUPPORT_LCD
	case RESULT_FMREC_START:
		return g_timer_setting_flag == 0;
		break;
#endif
	case FM_SAVE_FREQ:
		return FMStatus->station == 0;
		break;
	case FM_DELETE_FREQ:
		return FMStatus->station != 0;
		break;
	case FM_SET_BAND:
		return FMStatus->mode != param;
		break;
	case FM_FREQ_LIST:
		return FMStatus->tab_num > 0;
		break;
	}
	return 0;
}

INT32 fm_list_callback(INT32 type, INT32 value, INT8 **string)
{
	INT freq;
	
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = fm_item_buf;
		freq = (FMStatus->tabs[value] + 0x10000 + 49) / 100;
		
		sprintf(fm_item_buf, "FM%d.%dMHz", freq / 10, freq % 10);
		return value + 1 == FMStatus->station;
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		FM_SendCommand(MC_SETSTATION, value + 1);
		return 0;
	}
}

/*********************************************************************************
* Description : 主函数
*
* Arguments   :
        param:
            0,  rx mode
            1,  tx mode
*
* Returns     : 结果消息或0
*
* Notes       :
*
*********************************************************************************/
INT32 FM_Entry(INT32 param)
{
	INT32 result;
	
	if(param == 0xfe)
	{
		FM_SendCommand(MC_MUTE, SetMUTE);
		FM_SendCommand(MC_CLOSE, 0);
		BandSwitch = TRUE;
		return 0;
	}
	
	//clear message queue for make sure delay 0.5s
	//while(MESSAGE_Get() != 0) {;}
	
	// MESSAGE_Sleep(1);
	
	MESSAGE_Initial(g_comval);                                //系统消息（如RTC COUNTER）初始化
	gpio_SetMute(FALSE);//warkey 2.0
	//NVRAMRead(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
	FMStatus = (FM_play_status_t*)NVRAMGetData(VM_AP_RADIO, sizeof(FM_play_status_t));
	
#if APP_SUPPORT_INDEPEND_VOLUMNE==0
	FMStatus->volume = g_comval->Volume;
#endif
	
	if(param == 0xff)// open fm and return
	{
		if(!BandSwitch)
		{
			return 0;
		}
		if(FM_SendCommand(MC_OPEN, 0) != 0)           //硬件错误提示
		{
			return RESULT_MAIN;//退出进入主界面，主界面可以设置时间
		}
		//FM_SendCommand(MC_MUTE,SetMUTE);
		FM_SendCommand(MC_PLAY, 0);
		FM_SendCommand(MC_SETVOLUME, FMStatus->volume);
		BandSwitch = FALSE;
		return 0;
	}
	
	result = FM_MainFunc(param);
	
	//保存变量
	//FM_SendCommand(MC_BACKUP,(INT32)&radio_param);
	
	if (result == RESULT_RECORD_START)                            //截获FM中所有Rec热键录音。
	{
		result = RESULT_FMREC_START;
	}
	
	if ((result != RESULT_FMREC_NOSTART) && (result != RESULT_FMREC_START)) //非电台录音退出！for FM record
	{
		FM_SendCommand(MC_CLOSE, 0);
		BandSwitch = TRUE;
	}
	
#if APP_SUPPORT_LCD
	if(g_timer_setting_flag == 1 && (result == RESULT_MAIN || result == RESULT_NULL))
	{
		result = RESULT_TIMER;
	}
#endif
	
	//NVRAMWrite(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
	NVRAMWriteData();
	
	return result;
}


void FM_CheckParam(void)
{
	INT32 k;
	
	if(FirstFlag)
	{
		FirstFlag = FALSE;        //FM BUFFER标志赋初值
		
		if(FMStatus->magic != MAGIC_FMRADIO)
		{
			for(k = 0; k < 20; k++)
			{
				FMStatus->tabs[k] = 0;
			}
			
			FMStatus->magic = MAGIC_FMRADIO;
			FMStatus->mode  = g_comval->fm_value.fm_band;
			FMStatus->freq  = freq_scope[FMStatus->mode][0];
			FMStatus->volume = g_comval->Volume;
			FMStatus->station = 0;
			FMStatus->stereo_status = 0;
			FMStatus->tab_num = 0;
		}
	}
}


/*********************************************************************************
* Description : FM_MainFunc
*
* Arguments   :
        param:
            0, 开始时关标停在第0项
*
* Returns     : 结果消息或0
*
* Notes       :
*
*********************************************************************************/
INT32 FM_MainFunc(INT32 test)
{
	INT32 result = 0;
	UINT32 key;
	UINT16 param;
	TM_SYSTEMTIME systemTime;
	gpio_SetMute(TRUE);//warkey 2.1
#if XDL_APP_SUPPORT_TONE_TIP==1
	key = media_PlayInternalAudio(GUI_AUDIO_FM_PLAY, 1, FALSE);
	while(GetToneStatus())
	{
		MESSAGE_Sleep_EXT(1);
	}
	hal_HstSendEvent(SYS_EVENT, 0x11441133);
#endif
	output_flag = FALSE;
	need_draw = TRUE;
	
	FM_CheckParam();
	
	TM_GetSystemTime(&systemTime);
	current_time = systemTime.uHour * 3600 + systemTime.uMinute * 60;
	
	//clear the message pool  清除“初始化...”过程中按下的键!
	//while(MESSAGE_Get() != AP_KEY_NULL){;}
	
	while(1)
	{
		if(BandSwitch)        //初始化
		{
			//FM_SendCommand(MC_MUTE,SetMUTE);
			if(FM_SendCommand(MC_OPEN, 0) != 0)           //硬件错误提示
			{
				GUI_DisplayMessage(0, GUI_STR_FMERROR, NULL, GUI_MSG_FLAG_WAIT);
				return RESULT_MAIN;//退出进入主界面，主界面可以设置时间
			}
			
			if(FMStatus->tab_num == 0)
			{
				result = FM_AutoSearchHandle(UP);
				gpio_SetMute(FALSE);
				if(result == RESULT_IGNORE)
				{
					result = 0;
				}
				if(result != 0)
				{
					return result;
				}
			}
			else
			{
				FM_SendCommand(MC_PLAY, 0);
				COS_Sleep(1000);
				FM_SendCommand(MC_SETVOLUME, FMStatus->volume);
				FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
				gpio_SetMute(FALSE);
			}
			BandSwitch = FALSE;
		}
		
		//hal_HstSendEvent(SYS_EVENT,0xff200000 + need_draw);
		if(need_draw)            //屏幕重画
		{
			FM_ShowFreqIn();
			need_draw = FALSE;
		}
		
		if(key == 0)
		{
			key = MESSAGE_Wait();
		}
		
#if APP_SUPPORT_REMOTECONTROL==1
		if(MESSAGE_IsNumberKey(key))
		{
			UINT32 num_input = GUI_KeyNumberHandle(key);
			
			if(num_input > 0 && num_input <= FMStatus->tab_num)
			{
				FM_SendCommand(MC_SETSTATION, num_input);
				FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			}
			else if(num_input * 100 >= freq_scope[FMStatus->mode][0] && num_input * 100 <= freq_scope[FMStatus->mode][1])
			{
				FMStatus->freq = num_input * 100;
				FM_SendCommand(MC_PLAY, 0);
				FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			}
			need_draw = TRUE;
		}
#endif
		switch(key)
		{
		case AP_MSG_STANDBY:
			//do nothing
			break;
		case AP_MSG_WAIT_TIMEOUT:
			//APP_Sleep();
			break;
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
#if APP_SUPPORT_LCD==1
			TM_GetSystemTime(&systemTime);
			result = systemTime.uHour * 3600 + systemTime.uMinute * 60;
			if(current_time != result)
			{
				current_time = result;
				need_draw = TRUE;
			}
			result = 0;
#endif
			break;
		case AP_KEY_NEXT|AP_KEY_HOLD:
			if(MESSAGE_IsHold())
			{
				FMStatus->freq += 100;
				result = FM_SearchHandle(UP);
			}
			break;
		case AP_KEY_PREV|AP_KEY_HOLD:
			if(MESSAGE_IsHold())
			{
				FMStatus->freq -= 100;
				result = FM_SearchHandle(DOWN);
			}
			break;
		case AP_KEY_NEXT|AP_KEY_PRESS:
			if(FMStatus->tab_num == 0)
			{
				FM_SendCommand(MC_STEPGRID, UP);
			}
			else
			{
				FM_SendCommand(MC_STEP, UP);
			}
			FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			need_draw = TRUE;
			break;
		case AP_KEY_PREV|AP_KEY_PRESS:
			if(FMStatus->tab_num == 0)
			{
				FM_SendCommand(MC_STEPGRID, DOWN);
			}
			else
			{
				FM_SendCommand(MC_STEP, DOWN);
			}
			FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			need_draw = TRUE;
			break;
			
		case AP_KEY_REC | AP_KEY_PRESS:
			//    return RESULT_FMREC_START;
			break;
			
		case AP_KEY_MODE|AP_KEY_PRESS:
#if APP_SUPPORT_MENU==1
			if(AP_Support_MENU())
			{
				result = GUI_Display_Menu(GUI_MENU_FM, fm_menu_callback);
				if(result == RESULT_IGNORE)
				{
					result = 0;
				}
				if(result != 0)
				{
					return result;
				}
				need_draw = TRUE;
			}
			else
#endif
				return 0;
			break;
			
			
#if APP_SUPPORT_MENU==1
		case AP_KEY_MODE|AP_KEY_LONG:
			if(AP_Support_MENU())
			{
				return 0;
			}
#endif
		case AP_KEY_PLAY|AP_KEY_HOLD:
			if(!MESSAGE_IsHold())
			{
				break;
			}
		case FM_AUTO_SEARCH:
			{
				//全自动搜台.
				result = FM_AutoSearchHandle(UP);
				if(result == RESULT_IGNORE)
				{
					result = 0;
				}
				if(result != 0)
				{
					return result;
				}
				//FM_SendCommand(MC_MUTE,releaseMUTE);
				//FM_SendCommand(MC_SETVOLUME,FMStatus->volume);
			}
			need_draw = TRUE;
			break;
		case AP_KEY_LOOP|AP_KEY_PRESS:    //跳到电台表的下一个
			FM_SendCommand(MC_STEP, DOWN);
			FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			need_draw = TRUE;
			break;
			
		case AP_KEY_PLAY|AP_KEY_PRESS:                //跳到电台表的下一个
#if 1
			if(!fm_mute)
			{
				gpio_SetMute(TRUE);//warkey 2.0
				FM_SendCommand(MC_MUTE, SetMUTE);
				fm_mute = TRUE;
			}
			else
			{
				FM_SendCommand(MC_MUTE, releaseMUTE);
				if(FMStatus->volume)
				{
					gpio_SetMute(FALSE);
				}
				else
				{
					gpio_SetMute(TRUE);
				}
				fm_mute = FALSE;
			}
#else
			FM_SendCommand(MC_STEP, UP);
			FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			need_draw = TRUE;
#endif
			break;
			
		case FM_SAVE_FREQ:
		case FM_DELETE_FREQ:
			//case AP_KEY_PLAY|AP_KEY_HOLD:				  //insert or delete a station
			if(edit_station)
			{
				break;
			}
			edit_station = TRUE;
			if(FMStatus->station == 0) // add a station
			{
				FM_SendCommand(MC_SAVESTATION, 0);
			}
			else // remove a station
			{
				FM_SendCommand(MC_DELONE, (INT32)&FMStatus);
				FM_SendCommand(MC_STEP, UP);
			}
			FM_SendCommand(MC_BACKUP, 0);
			FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			need_draw = TRUE;
			break;
		case FM_DELALL_FREQ:
			FM_SendCommand(MC_DELALL, 0);
			FM_SendCommand(MC_BACKUP, 0);
			break;
		case AP_KEY_PLAY|AP_KEY_UP:				  //insert or delete a station
			edit_station = FALSE;
			break;
#if APP_SUPPORT_MENU==1
		case FM_SET_BAND:
			GUI_GetMenuResult(&param);
			FMStatus->mode = param;
			FMStatus->tab_num = 0;
			FM_SendCommand(MC_CLOSE, 0);
			BandSwitch = TRUE;
			need_draw = TRUE;
			break;
		case FM_FREQ_LIST:
			result = GUI_Display_List(LIST_TYPE_SINGLE_SELECT, 0, FMStatus->tab_num, FMStatus->station - 1, fm_list_callback, FALSE);
			if(result == RESULT_IGNORE)
			{
				result = 0;
			}
			if(result != 0)
			{
				return result;
			}
			
			FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			need_draw = TRUE;
			break;
#endif
#if XDL_APP_SUPPORT_TONE_TIP==1
		case 0xaaaa1111:
			if(fm_mute == FALSE)
			{
				FM_SendCommand(MC_MUTE, SetMUTE);
			}
			if(lowpower_flag_for_tone == 1)
			{
				media_PlayInternalAudio(GUI_AUDIO_LOWPOWER, 1, FALSE);
			}
			else if(lowpower_flag_for_tone == 2)
			{
				media_PlayInternalAudio(GUI_AUDIO_CHARGE_COMPLETE, 1, FALSE);
			}
			else if(lowpower_flag_for_tone == 4)
			{
				media_PlayInternalAudio(GUI_AUDIO_BT_POWEROFF, 1, FALSE);
			}
			else
			{
				media_PlayInternalAudio(GUI_AUDIO_ALARM1, 1, FALSE);
			}
			while(GetToneStatus())
			{
				MESSAGE_Sleep_EXT(1);
			}
			if(lowpower_flag_for_tone == 4)
			{
				MESSAGE_SetEvent(0xad11dead);
			}
			if(fm_mute == FALSE)
			{
				FM_SendCommand(MC_MUTE, releaseMUTE);
			}
			SetPAVolume(FMStatus->volume);
			hal_HstSendEvent(SYS_EVENT, 0x66552222);
			break;
#endif
		case AP_KEY_VOLADD|AP_KEY_DOWN:
		case AP_KEY_VOLSUB|AP_KEY_DOWN:
#if APP_SUPPORT_RGBLCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);
				GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
				slider.max = AUD_MAX_LEVEL;
				slider.min = 0;
				slider.step = 1;
				slider.value = FMStatus->volume;
				result = GUI_Slider(&slider, FM_VolumeCb);     //音量滚动条显示
				need_draw = TRUE;
				FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
				if(result == RESULT_IGNORE)
				{
					result = 0;
				}
				if(result != 0)
				{
					return result;
				}
				break;
			}
#elif APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);
				GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
				slider.max = AUD_MAX_LEVEL;
				slider.min = 0;
				slider.step = 1;
				slider.value = FMStatus->volume;
				result = GUI_Slider(&slider, FM_VolumeCb);     //音量滚动条显示
				need_draw = TRUE;
				FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
				if(result == RESULT_IGNORE)
				{
					result = 0;
				}
				if(result != 0)
				{
					return result;
				}
				break;
			}
#endif
		case AP_KEY_VOLSUB | AP_KEY_HOLD:
		case AP_KEY_VOLADD | AP_KEY_HOLD:
			{
				if(GetToneStatus())
				{
					break;
				}
				if((key & 0xffff00) == AP_KEY_VOLADD)
				{
					FMStatus->volume++;
					if(FMStatus->volume > AUD_MAX_LEVEL)
					{
						FMStatus->volume = AUD_MAX_LEVEL;
					}
				}
				if((key & 0xffff00) == AP_KEY_VOLSUB)
				{
					if(FMStatus->volume > 0)
					{
						FMStatus->volume--;
					}
				}
				if(fm_mute == TRUE)
				{
					SetInternalPAVolume(FMStatus->volume);
				}
				FM_SendCommand(MC_SETVOLUME, FMStatus->volume);
#if XDL_APP_SUPPORT_TONE_TIP==1
				hal_HstSendEvent(SYS_EVENT, 0x66552200);
				hal_HstSendEvent(SYS_EVENT, FMStatus->volume);
				if(FMStatus->volume >= AUD_MAX_LEVEL || FMStatus->volume <= 0)
				{
					lowpower_flag_for_tone = 0;
					MESSAGE_SetEvent(0xaaaa1111);
				}
#endif
			}
			break;
#if APP_SUPPORT_USBSTORAGE_BG==1
		case AP_MSG_USB_DISCONNECT:
			uctls_Close();
			need_draw = TRUE;
			break;
#endif
#if APP_SUPPORT_MENU==1
		case FM_ADJUST_CLOCK:
			{
				systemTime.uMinute = systemTime.uSecond = systemTime.uMilliseconds = 0;
				if(systemTime.uMinute < 10)
				{
					systemTime.uMinute = 0;
				}
				else if(systemTime.uMinute >= 50)
				{
					systemTime.uHour ++;
					if(systemTime.uHour == 24)
					{
						systemTime.uHour = 23;
						systemTime.uMinute = 59;
						systemTime.uSecond = 59;
					}
				}
				else
				{
					break;
				}
				
				TM_SetSystemTime(&systemTime);
			}
			break;
#endif
		default:                                        //热键处理
			
			key = MESSAGE_HandleHotkey(key);
			
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key == RESULT_RECORD_START)
				;//return RESULT_FMREC_START;  //for FM record (press REC)
			else if((key != 0) || (result != 0))
			{
				if(key != 0)
				{
					return key;
				}
				else
				{
					return result;
				}
			}
		}
		key = 0;
	}
}


#if APP_SUPPORT_FM_OUT
INT32  FM_OutPut(INT32 param, UINT32  *pFreq, BOOL *fm_ouput_on)/*进入FM Output 设置界面*/
{
	UINT32 key;
	INT32 result = 0;
	
	if(param == 0xfe)
	{
		//hal_AifSelectI2SPath(FALSE);
		FM_SendCommand(MC_MUTE, SetMUTE);
		FM_SendCommand(MC_CLOSE, 0);
		if(fm_ouput_on)
		{
			*fm_ouput_on = FALSE;
		}
		BandSwitch = TRUE;
		return 0;
	}
	
	output_flag = TRUE;
	need_draw = TRUE;
	
	if(param == 0xff)// open fm output, no display
	{
		need_draw = FALSE;
		BandSwitch = TRUE;
		MESSAGE_SetEvent(AP_MSG_WAIT_TIMEOUT);
	}
	
	//NVRAMRead(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
	FMStatus = (FM_play_status_t*)NVRAMGetData(VM_AP_RADIO, sizeof(FM_play_status_t));
	FM_CheckParam();
	
	FMStatus->freq = *pFreq;
	if(FMStatus->freq == 0)
	{
		FMStatus->freq = freq_scope[FMStatus->mode][0];
	}
	
	while(1)
	{
	
		if(BandSwitch)        //初始化
		{
			//FM_SendCommand(MC_MUTE,SetMUTE);
			if(FM_SendCommand(MC_OPEN, 1) != 0)           //硬件错误提示
			{
				GUI_DisplayMessage(0, GUI_STR_FMERROR, NULL, GUI_MSG_FLAG_WAIT);
				return 0;
			}
			
			COS_Sleep(200);
			FM_SendCommand(MC_PLAY, 0);
			//FM_SendCommand(MC_SETVOLUME, 12);
			
			//hal_AifSelectI2SPath(TRUE);
			
			BandSwitch = FALSE;
			
		}
		
		if(need_draw)            //屏幕重画
		{
			FM_ShowFreqIn();
			need_draw = FALSE;
		}
		
		key = MESSAGE_Wait();
		
#if APP_SUPPORT_REMOTECONTROL==1
		if(MESSAGE_IsNumberKey(key))
		{
			UINT32 num_input = GUI_KeyNumberHandle(key);
			
			if(num_input * 100 >= freq_scope[FMStatus->mode][0] && num_input * 100 <= freq_scope[FMStatus->mode][1])
			{
				FMStatus->freq = num_input * 100;
				FM_SendCommand(MC_PLAY, 0);
				FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
				FM_SendCommand(MC_BACKUP, 0);
			}
			need_draw = TRUE;
		}
#endif
		
		switch(key)
		{
		case AP_MSG_STANDBY:
			//do nothing
			break;
			
		case AP_KEY_MODE | AP_KEY_UP:
		case AP_KEY_PLAY | AP_KEY_UP:
			goto end;      //ok and return
			
		case AP_KEY_NEXT|AP_KEY_PRESS:
			FMStatus->freq += 900;
		case AP_KEY_VOLADD|AP_KEY_PRESS:
			FM_SendCommand(MC_STEPGRID, UP);
			//FM_SendCommand(MC_GETSTATUS,(int)&FMStatus);
			need_draw = TRUE;
			break;
		case AP_KEY_PREV|AP_KEY_PRESS:
			FMStatus->freq -= 900;
		case AP_KEY_VOLSUB |AP_KEY_PRESS:
			FM_SendCommand(MC_STEPGRID, DOWN);
			//FM_SendCommand(MC_GETSTATUS,(int)&FMStatus);
			need_draw = TRUE;
			break;
		case AP_MSG_WAIT_TIMEOUT:
			goto end;
			break;
			
		default:                                        //热键处理
			key = MESSAGE_HandleHotkey(key);
			
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if((key != 0))
			{
				result = key;
				goto end;
			}
		}
		
	}
	
end:
	*pFreq = FMStatus->freq;
	*fm_ouput_on = TRUE;
	return result;
}
#endif

/*********************************************************************************
* Description : FM_SendCommand
*
* Arguments   :
*
* Returns     :
*
* Notes       :发送消息给其他task
*
*********************************************************************************/
BOOL FM_SendCommand(UINT8 cmd, INT32 param)
{
	INT32 i;
	UINT8 k;
	FMD_ERR_T ret;
	
	app_trace(APP_FM_TRC, "Send FM Command cmd=0x%x, param=0x%x", cmd, param);
	//hal_HstSendEvent(SYS_EVENT,0xFF000300 + cmd);
	
	switch(cmd)
	{
	case     MC_OPEN:
		{
			FMD_INIT_T init;
			init.bassBoost = FALSE;
			init.volume = 0;
			init.freqKHz = FMStatus->freq;
			init.forceMono = FALSE;
			init.Tx1Rx0 = param;
			ret = fmd_Open(FMStatus->mode, NULL, NULL);//&init);
			if(ret == FMD_ERR_NO)
			{
				//ret = fmd_I2sOpen(cfg);
				MCI_FmPlay(0);
			}
			return ret;
		}
	case     MC_PLAY:
		{
			app_trace(APP_FM_TRC, "FM tune, freq=%d", FMStatus->freq);
			fmd_Tune(FMStatus->freq);
			//fmd_I2sOpen(cfg);
		}
		break;
	case     MC_STEP:
		if(FMStatus->tab_num == 0)
		{
			return FALSE;
		}
		switch(param)
		{
		case UP:                   //找到下一电台频率
			if(FMStatus->station == 0)
			{
				for(k = 0; k < FMStatus->tab_num; k++)
				{
					if((UINT32)FMStatus->tabs[k] + 0x10000 > FMStatus->freq)
					{
						break;
					}
				}
				FMStatus->station = k;
			}
			if(FMStatus->station >= FMStatus->tab_num)
			{
				FMStatus->station = 0;
			}
			
			FMStatus->station++;
			
			break;
		case DOWN:                    //找到上一电台频率
			if(FMStatus->station == 0)
			{
				for(k = FMStatus->tab_num - 1; k >= 0 ; k--)
				{
					if((UINT32)FMStatus->tabs[k] + 0x10000 < FMStatus->freq)
					{
						break;
					}
				}
				FMStatus->station = k + 2;
			}
			if(FMStatus->station <= 1)
			{
				FMStatus->station = FMStatus->tab_num + 1;
			}
			FMStatus->station--;
			break;
		default:
			break;
		}
	case MC_SETSTATION:
		if(cmd == MC_SETSTATION)
		{
			FMStatus->station = param;
		}
		
		FMStatus->freq = (FMStatus->tabs[FMStatus->station - 1]) + 0x10000;
		fmd_Tune(FMStatus->freq);
		FM_SendCommand(MC_BACKUP, 0);
		return TRUE;
		
	case   MC_STEPGRID:
		switch(param)
		{
		case UP:
			if(((FMStatus->freq + 100) > freq_scope[FMStatus->mode][1] + 20))
			{
				FMStatus->freq = freq_scope[FMStatus->mode][0];
			}
			else
			{
				FMStatus->freq += 100;
			}
			break;
		case DOWN:
			if(((FMStatus->freq - 100) < freq_scope[FMStatus->mode][0] - 20))
			{
				FMStatus->freq = freq_scope[FMStatus->mode][1];
			}
			else
			{
				FMStatus->freq -= 100;
			}
			break;
		default:
			break;
		}
		fmd_Tune(FMStatus->freq);
		FM_SendCommand(MC_BACKUP, 0);
		return TRUE;
		
		
	case    MC_SEARCH:
		//fmd_SetVolume(FMD_ANA_MUTE, FALSE, TRUE);
		return fmd_ValidStop(FMStatus->freq);
		
	case    MC_SAVESTATION:
		if(FMStatus->station != 0)          //当前电台在电台表中有保存的情况
		{
			k = 0;
			for(i = 0; i < FM_TAB_NUMS; i++)
			{
				if(FMStatus->tabs[i] != 0)
				{
					k++;
				}
				
				if(k == FMStatus->station)
				{
					FMStatus->tabs[i] = FMStatus->freq;
					break;
				}
			}
		}
		else if(FMStatus->tab_num > 19)
		{
			return    FALSE;
		}
		else                                    //新的电台频率保存到电台表
		{
			for(i = 0; i < FMStatus->tab_num; i++)
				if(FMStatus->tabs[i] >= FMStatus->freq - 0x10000)
				{
					break;
				}
				
			FMStatus->station = i;
			for(i = FMStatus->tab_num; i > FMStatus->station; i--)
			{
				FMStatus->tabs[i] = FMStatus->tabs[i - 1];
			}
			FMStatus->tabs[i] = FMStatus->freq - 0x10000;
			FMStatus->station++;
			FMStatus->tab_num++;
		}
		return TRUE;
		
	case    MC_SETMODE:
		FMStatus->mode = (FM_Station_bank_t)param;
		break;
	case    MC_SETVOLUME:
		if(param > 0 && param <= ((AUD_MAX_LEVEL + 1) / FMD_ANA_VOL_QTY)) //warkey 2.1
		{
			fmd_SetVolume(1, FALSE, FALSE);
		}
		else
		{
			fmd_SetVolume(param * FMD_ANA_VOL_QTY / (AUD_MAX_LEVEL + 1), FALSE, FALSE);
		}
		SetPAVolume(param);
		FMStatus->volume = (BYTE)param;
		g_comval->Volume = param;
		break;
		
	case    MC_BACKUP:
		//NVRAMWrite(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
		NVRAMWriteData();
		break;
		
	case    MC_RESTORE:
		//NVRAMRead(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
		break;
		
	case    MC_GETSTATUS:
		fmd_GetFreq(&FMStatus->freq);
		fmd_GetStereoStatus(&FMStatus->stereo_status);
		fmd_GetRssi(&FMStatus->rssi);
		k = 0;
		//当前电台频率值和电台表中某一频率差距小于150KHZ，此电台和此电台表对应
		FMStatus->station = 0;
		for(i = 0; i < FM_TAB_NUMS; i++)
		{
			if(FMStatus->tabs[i] != 0)
			{
				if(abs(FMStatus->freq - 0x10000 - FMStatus->tabs[i]) < 100)
				{
					FMStatus->station = i + 1;
					break;
				}
			}
		}
		
		// no save data here NVRAMWrite(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
		break;
		
	case    MC_GETTABLE:
		memcpy((UINT16 *)param, &FMStatus->tabs[0], FM_TAB_NUMS * sizeof(UINT16));
		break;
		
	case MC_DELONE:
		if(FMStatus->station == 0)
		{
			return    FALSE;
		}
		
		for(k = FMStatus->station; k < FMStatus->tab_num + 1; k++)
		{
			FMStatus->tabs[k - 1] = FMStatus->tabs[k];
		}
		
		FMStatus->tabs[FMStatus->tab_num - 1] = 0;
		FMStatus->tab_num--;
		FMStatus->station = 0;
		return     TRUE;
		
	case MC_DELALL:
		for(i = 0; i < FM_TAB_NUMS; i++)
		{
			FMStatus->tabs[i] = 0;
		}
		FMStatus->station = 0;
		FMStatus->tab_num = 0;
		break;
		
	case MC_MUTE:            //静音
		if(param == SetMUTE)
		{
			SetPAVolume(0);
			MCI_FmStop();
			fmd_SetVolume(FMD_ANA_MUTE, FALSE, TRUE);
		}
		else
		{
			if(FMStatus->volume > 0 && FMStatus->volume <= ((AUD_MAX_LEVEL + 1) / FMD_ANA_VOL_QTY)) //warkey 2.1
			{
				fmd_SetVolume(1, FALSE, FALSE);
			}
			else
			{
				fmd_SetVolume(FMStatus->volume * FMD_ANA_VOL_QTY / (AUD_MAX_LEVEL + 1), FALSE, FALSE);
			}
			//fmd_SetVolume(FMStatus->volume*FMD_ANA_VOL_QTY/(AUD_MAX_LEVEL+1), FALSE, FALSE);
			MCI_FmPlay(0);
			//COS_Sleep(1000);
			SetPAVolume(FMStatus->volume);
		}
		break;
		
	case MC_CLOSE:
		if(!param)
		{
			MCI_FmStop();
		}
		fmd_Close();
		break;
		
	case MC_GETFREQ:
		*(UINT32 *)param = FMStatus->freq;
		break;
		
	case MC_SETFREQ:
		FMStatus->freq = param;
		break;
	default:
		break;
	}
	
	return    TRUE;
}

void FM_SearchTimeout(void *param)
{
	MESSAGE_SetEvent(0);
}

/*********************************************************************************
* Description : FM_SearchHandle
*
* Arguments   :
*
* Returns     :
*
* Notes       : 搜索电台
*
*********************************************************************************/
#define SEARCH_TIME 200
INT32  FM_SearchHandle(UINT8 Direct)
{
	UINT32  key;
	UINT8 Searchflag = 1;
	UINT8 fm_search_timer;
	
	g_fm_status = fm_search;
	need_draw = TRUE;
	
	fm_search_timer = COS_SetTimer(SEARCH_TIME, FM_SearchTimeout, NULL, COS_TIMER_MODE_PERIODIC);
	
	//SetPAVolume(FMStatus->volume);
	//media_PlayInternalAudio(GUI_AUDIO_FM_SEARCH, 0, FALSE);
	while(1)
	{
		if(Direct != 0)
		{
			if(Direct == UP)          //NEXT搜索处理
			{
				if(((FMStatus->freq + 100) > freq_scope[FMStatus->mode][1]))
				{
					FMStatus->freq = freq_scope[FMStatus->mode][0];
				}
				else
				{
					FMStatus->freq += 100;
				}
				if(FM_SearchDisplay(UP))
				{
					key = 0;
					app_trace(APP_FM_TRC, "FM manual tune, freq=%ld is success!", FMStatus->freq);
					goto search_end;
				}
			}
			else if(Direct == DOWN)          //LAST搜索处理
			{
				if(((FMStatus->freq - 100) < freq_scope[FMStatus->mode][0]))
				{
					FMStatus->freq = freq_scope[FMStatus->mode][1];
				}
				else
				{
					FMStatus->freq -= 100;
				}
				if(FM_SearchDisplay(DOWN))
				{
					key = 0;
					app_trace(APP_FM_TRC, "FM manual tune, freq=%ld is success!", FMStatus->freq);
					goto search_end;
				}
			}
			else if(Direct == STOP)
			{
				Direct = 0;
				key = 0;
				goto search_end;
			}
		}
		
		if(need_draw)                //显示
		{
			FM_ShowFreqIn();
			need_draw = FALSE;
		}
		
		key = MESSAGE_Wait();
		
		switch(key)
		{
		case AP_KEY_NULL:
		case AP_MSG_STANDBY:
			//do nothing
			break;
		case AP_KEY_PLAY | AP_KEY_PRESS:
		case AP_KEY_MODE | AP_KEY_PRESS:		// 保存电台, 返回播放界面.
			key = 0;
			goto search_end;
			break;
		case AP_KEY_NEXT|AP_KEY_DOWN:        //NEXT搜索时按NEXT键，停止搜索
			if(Searchflag == 1)
			{
				Searchflag = 0;
			}
			else if(Direct == UP)
			{
				Direct = STOP;
				//FM_SendCommand(MC_MUTE,releaseMUTE);
			}
			break;
		case AP_KEY_PREV|AP_KEY_DOWN:        //LAST搜索时按LAST键，停止搜索
			if(Searchflag == 1)
			{
				Searchflag = 0;
			}
			else if(Direct == DOWN)
			{
				Direct = STOP;
				//FM_SendCommand(MC_MUTE,releaseMUTE);
			}
			break;
		default:                    //热键处理
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				goto search_end;
			}
		}
	}
	
search_end:
	//media_StopInternalAudio();
	FM_SendCommand(MC_SETVOLUME, FMStatus->volume);
	COS_KillTimer(fm_search_timer);
	g_fm_status = fm_play;
	need_draw = TRUE;
	return key;
}


/*********************************************************************************
* Description : FM_SearchDisplay
*
* Arguments   :
*
* Returns     :
*
* Notes       :发送消息给其他task
*
*********************************************************************************/
BOOL FM_SearchDisplay(FM_direct_t Direct)
{
	BOOL Search_Flag;
	if(!fm_mute)
	{
		FM_SendCommand(MC_MUTE, SetMUTE);
	}
	else
	{
		fm_mute=FALSE;
	}
	Search_Flag = FM_SendCommand(MC_SEARCH, Direct);
	FM_ShowFreqIn();
	if(Search_Flag)                //搜台成功后，MUTE释放
	{
		FM_SendCommand(MC_MUTE, releaseMUTE);
		FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
	}
	
	return Search_Flag;
}

void FM_ShowFreqIn(void)
{
#if APP_SUPPORT_RGBLCD==1
	INT8 buf[20] = {'\0'};
	INT32 i, k;
	
	if(AP_Support_LCD())
	{
		ui_auto_update = FALSE;
		GUI_ClearScreen(NULL);
		
		GUI_ResShowPic(GUI_IMG_RA_FM, 0, 0);       //fm 标志显示
		BT_DisplayIcon(31, 0);
#if APP_SUPPORT_USBSTORAGE_BG==1
		if(GetUsbCableStatus())
		{
			GUI_ResShowImage(GUI_IMG_DEVICE, 2, 48, 0);
		}
#endif
		if(output_flag)
		{
			GUI_ResShowPic(GUI_IMG_MMUSIC, 15, 0);       //music 标志显示
		}
		else
		{
			if((FMStatus->tab_num > 0) && (FMStatus->station > 0))  //显示电台号
			{
				sprintf(buf, "[CH %d]", FMStatus->station);
				GUI_DisplayTextCenter(20, buf);
				//if(FMStatus->station >= 10)
				//    GUI_DisplayText(28, 0, buf);
				//else
				//    GUI_DisplayText(32, 0, buf);
			}
			
			GUI_DisplayTimeText(64, 0, GUI_TIME_DISPLAY_HOUR_MINUTE, current_time * 1000);
			
			
			if(FMStatus->stereo_status)
			{
				GUI_ResShowPic(GUI_IMG_RA_STERE, 15, 0);      //显示立体声
			}
		}
		
		GUI_DisplayBattaryLever();
		
		//频率换算及转换成ASCII码
		i = (FMStatus->freq + 49) / 100;
		
		sprintf(buf, "<FM %d.%d MHz>", i / 10, i % 10);
		
		GUI_DisplayTextCenter(48, buf);                        //显示电台频率
		
		k = (FMStatus->freq - freq_scope[FMStatus->mode][0]) / 200;
		if(FMStatus->mode == 0)
		{
			GUI_ResShowPic(GUI_IMG_DWNARROW, 24 + k, 80);        //显示箭头
			GUI_ResShowPic(GUI_IMG_FQBAR, 0, 92);                  //显示US频率条
		}
		else
		{
			GUI_ResShowPic(GUI_IMG_DWNARROW, 40 + k, 80);        //显示箭头
			GUI_ResShowPic(GUI_IMG_FQBAR_JP, 0, 92);                  //显示US频率条
		}
		
		GUI_UpdateScreen(NULL);                             //刷新
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LCD==1
	INT8 buf[20] = {'\0'};
	INT32 i, k;
	
	if(AP_Support_LCD())
	{
		ui_auto_update = FALSE;
		GUI_ClearScreen(NULL);
	
		GUI_ResShowPic(GUI_IMG_RA_FM, 0, 0);       //fm 标志显示
		BT_DisplayIcon(31, 0);
#if APP_SUPPORT_USBSTORAGE_BG==1
		if(GetUsbCableStatus())
		{
			GUI_ResShowImage(GUI_IMG_DEVICE, 2, 48, 0);
		}
#endif
		if(output_flag)
		{
			GUI_ResShowPic(GUI_IMG_MMUSIC, 15, 0);       //music 标志显示
		}
		else
		{
			if((FMStatus->tab_num > 0) && (FMStatus->station > 0))  //显示电台号
			{
				sprintf(buf, "[CH %d]", FMStatus->station);
				GUI_DisplayTextCenter(16, buf);
				//if(FMStatus->station >= 10)
				//    GUI_DisplayText(28, 0, buf);
				//else
				//    GUI_DisplayText(32, 0, buf);
			}
	
			GUI_DisplayTimeText(70, 0, GUI_TIME_DISPLAY_HOUR_MINUTE, current_time * 1000);
	
	
			if(FMStatus->stereo_status)
			{
				GUI_ResShowPic(GUI_IMG_RA_STERE, 15, 0);      //显示立体声
			}
		}
	
		GUI_DisplayBattaryLever();
	
		//频率换算及转换成ASCII码
		i = (FMStatus->freq + 49) / 100;
	
		sprintf(buf, "<FM %d.%d MHz>", i / 10, i % 10);
	
		GUI_DisplayTextCenter(32, buf);                        //显示电台频率
	
		k = (FMStatus->freq - freq_scope[FMStatus->mode][0]) / 200;
		if(FMStatus->mode == 0)
		{
			GUI_ResShowPic(GUI_IMG_DWNARROW, 6 + k, 48);        //显示箭头
			GUI_ResShowPic(GUI_IMG_FQBAR, 0, 56);                  //显示US频率条
		}
		else
		{
			GUI_ResShowPic(GUI_IMG_DWNARROW, 24 + k, 48);        //显示箭头
			GUI_ResShowPic(GUI_IMG_FQBAR_JP, 0, 56);                  //显示US频率条
		}
	
		GUI_UpdateScreen(NULL);                             //刷新
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LED8S==1
	INT8 buf[10] = {'\0'};
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(GUI_ICON_FM | GUI_ICON_DECPOINT, 0, 0);
	sprintf(buf, "%4d", (FMStatus->freq + 49) / 100);
	GUI_DisplayText(0, 0, buf);
	GUI_UpdateScreen(NULL);
#endif
	
	if(g_fm_status == fm_search)
	{
		LED_SetPattern(GUI_LED_FM_SEEK, LED_LOOP_INFINITE);
	}
	else if(g_fm_status == fm_play)
	{
		LED_SetPattern(GUI_LED_FM_PLAY, LED_LOOP_INFINITE);
	}
	else
	{
		LED_SetPattern(GUI_LED_FM_NOSTATION, LED_LOOP_INFINITE);
	}
	
}

/*********************************************************************************
* Description : 全自动搜索
*
* Arguments   : Direct,Next或Last搜索.
*
* Returns     : 如果有可识别的热键消息,  返回结果消息
        如果没有可识别的热键消息,  返回0
*
* Notes       :
*
*********************************************************************************/
INT32    FM_AutoSearchHandle(UINT8 Direct)
{
	UINT32 key;
	UINT32 max_freq = freq_scope[FMStatus->mode][1];
	UINT32 min_freq = freq_scope[FMStatus->mode][0];
	UINT8 rssi[FM_TAB_NUMS];
	UINT8 fm_search_timer;
	
	BOOL    loop = TRUE;
	UINT16    temp, min_rssi;
	INT32 i, j;
	
	FMStatus->tab_num = 0;                 // 全自动搜索,从极值开始.
	FMStatus->freq = min_freq;
	
	g_fm_status = fm_search;
	
	temp =  0;
	if(!fm_mute)
	{
		FM_SendCommand(MC_MUTE, SetMUTE);
	}
	else
	{
		fm_mute=FALSE;
	}
	
	fm_search_timer = COS_SetTimer(SEARCH_TIME, FM_SearchTimeout, NULL, COS_TIMER_MODE_PERIODIC);
	//SetPAVolume(FMStatus->volume);
	//media_PlayInternalAudio(GUI_AUDIO_FM_SEARCH, 0, FALSE); // can't play audio for the search need high cpu time
	
	while(loop)
	{
		if((FMStatus->freq + 100) > max_freq)
		{
			loop = FALSE;
			break;
		}
		else
		{
			FMStatus->freq += 100;
		}
		
		if(FM_SendCommand(MC_SEARCH, UP))
		{
			FM_SendCommand(MC_GETSTATUS, (INT32)&FMStatus);
			if(FMStatus->tab_num < FM_TAB_NUMS)
			{
				FMStatus->tabs[FMStatus->tab_num] = FMStatus->freq - 0x10000;
				rssi[FMStatus->tab_num] = FMStatus->rssi;
			}
			else
			{
				temp = 0;
				min_rssi = 0xff;
				for(i = 0; i < FM_TAB_NUMS; i++)
				{
					if(rssi[i] < min_rssi)
					{
						min_rssi = rssi[i];
						temp = i;
					}
				}
				if(FMStatus->rssi > min_rssi)
				{
					FMStatus->tabs[temp] = FMStatus->freq - 0x10000;
					rssi[temp] = FMStatus->rssi;
				}
			}
			FMStatus->tab_num ++;
#if 1//warkey 2.0
			app_trace(APP_FM_TRC, "FM auto tune, freq=%ld is success!", FMStatus->freq);
			hal_HstSendEvent(APP_EVENT, 0x5555ffff);
			hal_HstSendEvent(APP_EVENT, FMStatus->tab_num);
			FM_SendCommand(MC_MUTE, releaseMUTE);
			FM_SendCommand(MC_PLAY, 0);
			//COS_Sleep(500);
			MESSAGE_Sleep_EXT(3);
			FM_SendCommand(MC_MUTE, SetMUTE);
#endif
			FMStatus->station = FMStatus->tab_num;
		}
		
		FM_ShowFreqIn();
		
		key = MESSAGE_Wait();
		//hal_HstSendEvent(SYS_EVENT,key);
		switch(key)
		{
			BOOL Search_Flag = FALSE;
		case AP_MSG_STANDBY:
			//do nothing
			break;
		case AP_KEY_PLAY | AP_KEY_PRESS:
		case AP_KEY_MODE | AP_KEY_PRESS:        // 保存电台, 返回播放界面.
			loop = FALSE;
			key = RESULT_IGNORE;
			break;
		default:                    //热键处理
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				loop = FALSE;
			}
		}        // end switch(key)
	}            // end while(loop)
	
	COS_KillTimer(fm_search_timer);
	
	if(FMStatus->tab_num > 0)
	{
		if(FMStatus->tab_num > FM_TAB_NUMS)
		{
			FMStatus->tab_num = FM_TAB_NUMS;
			
			// sort tabs
			for(i = 0; i < FM_TAB_NUMS - 1; i++)
				for(j = i + 1; j < FM_TAB_NUMS; j++)
				{
					if(FMStatus->tabs[i] > FMStatus->tabs[j])
					{
						temp = FMStatus->tabs[i];
						FMStatus->tabs[i] = FMStatus->tabs[j];
						FMStatus->tabs[j] = temp;
					}
				}
		}
		
		//        FMStatus->station = 1;
		FMStatus->freq = (FMStatus->tabs[0]) + 0x10000;
		
	}
	app_trace(APP_FM_TRC, "FM search complete, tab_num=%d, set freq=%d", FMStatus->tab_num, FMStatus->freq);
	
	FM_SendCommand(MC_PLAY, 0);
	COS_Sleep(100);
	FM_SendCommand(MC_MUTE, releaseMUTE);
	//FM_SendCommand(MC_GETSTATUS,(INT32)&FMStatus);
	FM_ShowFreqIn();
	//FM_SendCommand(MC_MUTE,releaseMUTE);
	//FM_SendCommand(MC_SETVOLUME,FMStatus->volume);
	FM_SendCommand(MC_BACKUP, 0);
	//need_draw=TRUE;
	
	g_fm_status = fm_play;
	//media_StopInternalAudio();
	
	return key;
}

#endif // #if APP_SUPPORT_FM
