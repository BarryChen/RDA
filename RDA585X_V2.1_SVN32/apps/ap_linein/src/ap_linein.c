/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"
#include "ap_linein.h"

#define LINEIN_MAX_VOLUME        AUD_MAX_LEVEL

extern slider_t              slider;
extern BOOL     ui_auto_update;
#if XDL_APP_SUPPORT_TONE_TIP==1
extern UINT8 lowpower_flag_for_tone;
#endif

INT8        g_linein_volumne = 8;
static BOOL need_draw;
static BOOL line_mute_state;
UINT8 g_linein_out_event = 0;

void LineIn_VolumeCb( INT32 Vol )
{
	g_linein_volumne = Vol;
	g_comval->Volume = Vol;
	SetPAVolume(Vol);
}


INT32 LineIn_ChangeVolume( UINT32 key )
{
	INT32 result;
	if(AP_Support_LCD())
	{
		//show background
		GUI_ClearScreen(NULL);
		GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
		
		slider.min = 0;
		slider.max = LINEIN_MAX_VOLUME;
		slider.value = g_linein_volumne;
		slider.step = 1;
		
		ui_auto_update = FALSE;
		result = GUI_Slider(& slider, LineIn_VolumeCb);
		ui_auto_update = TRUE;
		
		if (result != 0)
		{
			return result;
		}
		need_draw = TRUE;
	}
	else
	{
		key = key & 0xffff0000;
		if(key == AP_KEY_VOLADD || key == AP_KEY_NEXT)
		{
			g_linein_volumne++;
			if(g_linein_volumne > LINEIN_MAX_VOLUME)
			{
				g_linein_volumne = LINEIN_MAX_VOLUME;
			}
		}
		if(key == AP_KEY_VOLSUB || key == AP_KEY_PREV)
		{
			g_linein_volumne--;
			if(g_linein_volumne < 0)
			{
				g_linein_volumne = 0;
			}
		}
		SetPAVolume(g_linein_volumne);
		g_comval->Volume = g_linein_volumne;
#if 0//XDL_APP_SUPPORT_TONE_TIP==1
		if(g_linein_volumne >= LINEIN_MAX_VOLUME || g_linein_volumne <= 0)
		{
			lowpower_flag_for_tone = 0;
			MESSAGE_SetEvent(0xaaaa1111);
		}
#endif
	}
	return 0;
}

extern UINT8 media_GetInternalStatus();

/*********************************************************************************
* Description : LINE IN 函数，在该模式下
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 LINEIN_Entry(INT32 param)
{
	UINT32  key;
	UINT8 result;
	INT32 wait_audio = 3; // wait 1 second to open pa
	gpio_SetMute(TRUE);//warkey 2.1
	need_draw = TRUE;
	ui_auto_update = FALSE;
	line_mute_state = FALSE; //2012-9-27
	
	hal_HstSendEvent(SYS_EVENT, 0x08078000);
	
	if(media_GetInternalStatus() != 0)
	{
		//MCI_StopInternalAudio();
		media_StopInternalAudio();
		hal_HstSendEvent(SYS_EVENT, 0x08078888);
	}
	
	MESSAGE_Initial(g_comval);
#if APP_SUPPORT_INDEPEND_VOLUMNE==0
	g_linein_volumne = g_comval->Volume;
#endif
	
#if XDL_APP_SUPPORT_TONE_TIP==1
	media_PlayInternalAudio(GUI_AUDIO_LINEIN_PLAY, 1, FALSE);
	while(GetToneStatus())
	{
		MESSAGE_Sleep_EXT(1);
	}
	//COS_Sleep(100);
	hal_HstSendEvent(SYS_EVENT, 0x11117711);
#endif
	
	MCI_LineinPlay(0);
#if APP_SUPPORT_DELAY_VOLUME==1
	SetPAVolume(0);
#else
	SetPAVolume(g_linein_volumne);
#endif
	if(!gpio_detect_linein())//howe 2.1
	{
		MESSAGE_SetEvent(AP_MSG_REMOVE_LINE_IN);
	}
	while(1)
	{
		if(need_draw)
		{
#if APP_SUPPORT_RGBLCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);//清全屏幕
				GUI_ResShowPic(GUI_IMG_LINEIN, 0, 0);
				GUI_DisplayBattaryLever();
				GUI_UpdateScreen(NULL);
			}
#elif APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);//清全屏幕
				GUI_ResShowPic(GUI_IMG_LINEIN, 0, 0);
				GUI_DisplayBattaryLever();
				GUI_UpdateScreen(NULL);
			}
#elif APP_SUPPORT_LED8S==1
			{
				GUI_ClearScreen(NULL);
				GUI_ResShowPic(GUI_ICON_PLAY | GUI_ICON_LINEIN, 0, 0);
				GUI_DisplayText(0, 0, "LINE");
			
				GUI_UpdateScreen(NULL);
			}
#endif
			LED_SetPattern(GUI_LED_LINEIN_PLAY - line_mute_state, LED_LOOP_INFINITE);
			
			need_draw = FALSE;
		}
		if(key == 0)
		{
			key = MESSAGE_Wait();
		}
		switch( key )
		{
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
			//#if APP_SUPPORT_DELAY_VOLUME==1
			if(wait_audio > 0)
			{
				wait_audio --;
				if(wait_audio == 0)
				{
					if(g_linein_volumne && (line_mute_state == FALSE))
					{
						gpio_SetMute(FALSE);
					}
					else
					{
						gpio_SetMute(TRUE);
					}
				}
				else if(wait_audio == 2)
				{
					//SetPAVolume(g_linein_volumne);
					SetInternalPAVolume(g_linein_volumne);//howe 2.1
					hal_HstSendEvent(SYS_EVENT, 0x08078012);
				}
				
			}
			//#endif
			break;
		case AP_MSG_STANDBY:
		// case AP_MSG_SD_IN:  //此处注释就等于退出2012-9-27
		case AP_MSG_USB_PLUGIN:
			// case AP_KEY_MODE|AP_KEY_PRESS:
			break;
#if XDL_APP_SUPPORT_TONE_TIP==1
		case 0xaaaa1111:
			{
				MCI_LineStop();
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
				MCI_LineinPlay(0);
				if(line_mute_state == FALSE)
				{
					SetInternalPAVolume(g_linein_volumne);
				}
				else
				{
					SetInternalPAVolume(0);
				}
				wait_audio = 1;
			}
			break;
#endif
			
		case AP_KEY_VOLSUB | AP_KEY_DOWN:
		case AP_KEY_VOLADD | AP_KEY_DOWN:
		case AP_KEY_VOLSUB | AP_KEY_HOLD:
		case AP_KEY_VOLADD | AP_KEY_HOLD:
		case AP_KEY_NEXT   | AP_KEY_HOLD:
		case AP_KEY_PREV   | AP_KEY_HOLD:
			if(GetToneStatus())
			{
				break;
			}
			result = LineIn_ChangeVolume(key);
			line_mute_state = FALSE;
			need_draw = TRUE;
			if(result != 0)
			{
				MCI_LineStop();
				return result;
			}
			break;
			
		case AP_MSG_REMOVE_LINE_IN:
			MCI_LineStop();
			g_linein_out_event = 1;//line-in removed
			return RESULT_MAIN;
			break;
			
		case AP_KEY_PLAY| AP_KEY_PRESS:
			if(line_mute_state)
			{
				line_mute_state = FALSE;
				need_draw = TRUE;
				SetPAVolume(g_linein_volumne);
				//先设置音量再开功放，防止PA声，音量为0，不开功放
				if(g_linein_volumne)
				{
					gpio_SetMute(FALSE);
				}
				else
				{
					gpio_SetMute(TRUE);
				}
			}
			else
			{
				line_mute_state = TRUE;
				need_draw = TRUE;
				gpio_SetMute(TRUE);//warkey 2.0
				SetPAVolume(0);
			}
			break;
		case AP_KEY_MODE | AP_KEY_PRESS:
			MCI_LineStop();
			return RESULT_BT;
			break;
		case AP_KEY_PLAY| AP_KEY_HOLD:
			if(MESSAGE_IsHold())
			{
			MCI_LineStop();
			return RESULT_BT;
			}
			break;
		default:
			result = MESSAGE_HandleHotkey(key);
			//            hal_HstSendEvent(SYS_EVENT,0x08078011);
			//            hal_HstSendEvent(SYS_EVENT, key);
			//            hal_HstSendEvent(SYS_EVENT, result);
			if(result == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(result == RESULT_LINE_IN)
			{
				break;//plug line-in out and in frequently
			}
			else if(result != 0)
			{
				MCI_LineStop();
				return result;
			}
		}
		key = 0;
	};
	
	MCI_LineStop();
	return result;
}







