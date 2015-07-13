/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"
#include "ap_idle.h"
#include "tm.h"
#include "ap_fm.h"
#include "ap_music.h"
#include "ap_media.h"

#define AP_MAX_TIMER                5

#define AP_TIMER_ACTIVE             1

#define AP_TIMER_MAX_NAME           64

#define TIMER_ALARM_OPEN            AP_MSG_USER+1
#define TIMER_ALARM_CLOSE           AP_MSG_USER+2
#define TIMER_ALARM_TIME            AP_MSG_USER+3
#define TIMER_ALARM_TYPE            AP_MSG_USER+4
#define TIMER_ALARM_DAYS            AP_MSG_USER+5
#define TIMER_ALARM_SELECT          AP_MSG_USER+6

enum alarm_type
{
	MUSIC_ALARM,
	FM_TIMER,
	FM_REC_TIMER,
	AUDIO_ALARM,  // internal audio
};

typedef struct
{
	UINT8 type;
	UINT8 flag; // bit 7~4 disk
	UINT8 days; // bit 0 ~ 6, sunday to saturday
	UINT8 volume;
	UINT16 time; // time in 24 hour
	UINT32 file_or_freq; // music file for alarm or freq for fm timer
} alarm_t;

typedef struct
{

	alarm_t timers[AP_MAX_TIMER];
	UINT16 magic;
} timer_vars_t;

extern BOOL     ui_auto_update;
extern INT32           g_last_reslt;

extern FM_play_status_t    *FMStatus;                //当前电台信息
extern music_vars_t *g_music_vars;//本AP的公共变量


#if APP_SUPPORT_LCD

timer_vars_t *ap_timers;
INT8 g_timer_setting_flag;
INT8 curr_active_alarm;

static INT8 alarm_item_buf[AP_TIMER_MAX_NAME];
static INT8 curr_setting_alarm;
static INT32 menu_active;

static const UINT16 alarm_days_text[7]     = {GUI_STR_MONDAY,   GUI_STR_TUESDAY,  GUI_STR_WEDNESDAY,  GUI_STR_THURSDAY, GUI_STR_FRIDAY,  GUI_STR_SATURDAY, GUI_STR_SUNDAY};
extern const INT8 month_days[];

INT32 TIMER_Alarm()
{
	UINT32  key, time;
	INT32 result = RESULT_MAIN; //POWERON;
	BOOL need_draw = TRUE;
	UINT8 alarm_index = curr_active_alarm;
	UINT8 disk, volume;
	UINT32 count = 0;
	Music_type_t music_type;
	Open_param_t param;
	
	if(alarm_index >= 0)
	{
		if(ap_timers->timers[alarm_index].days == 0)
		{
			ap_timers->timers[curr_setting_alarm].flag &= ~AP_TIMER_ACTIVE;
			curr_active_alarm--;
		}
	}
	
	TIMER_SetAlarm(0);
	if(alarm_index < 0)
	{
		return RESULT_IDLE;
	}
	
#if APP_SUPPORT_FM
	if(ap_timers->timers[alarm_index].type == FM_TIMER || ap_timers->timers[alarm_index].type == FM_REC_TIMER)
	{
		FMStatus->freq = ap_timers->timers[alarm_index].file_or_freq;
		// no need        NVRAMWrite(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
		if(ap_timers->timers[alarm_index].type == FM_TIMER)
		{
			return RESULT_RADIO;
		}
		else
		{
			return RESULT_FMREC_START;
		}
	}
#endif
	
	if(ap_timers->timers[alarm_index].type == AUDIO_ALARM)
	{
		media_PlayInternalAudio(GUI_AUDIO_ALARM1 + ap_timers->timers[alarm_index].file_or_freq - 1, 0, FALSE);
	}
	else
	{
		disk = ap_timers->timers[alarm_index].flag >> 4;
		if (!MountDisk(disk))
		{
			GUI_DisplayMessage(0, 0, "Alarm", GUI_MSG_FLAG_KEY);
			return RESULT_RADIO;
		}
		
		result = FS_GetShortName(ap_timers->timers[alarm_index].file_or_freq, alarm_item_buf);
		music_type = MUSIC_CheckType(alarm_item_buf);
		if(result != ERR_SUCCESS || music_type == Music_NON)
		{
			GUI_DisplayMessage(0, 0, "Alarm", GUI_MSG_FLAG_KEY);
			return RESULT_RADIO;
		}
		
		param.type      = music_type;
		param.mode      = MEDIA_PLAY;
		param.file_entry = ap_timers->timers[alarm_index].file_or_freq;
		
		result = mediaSendCommand(MC_OPEN, (INT32)&param);
		if(!result)
		{
			GUI_DisplayMessage(0, 0, "Alarm", GUI_MSG_FLAG_KEY);
			return RESULT_RADIO;
		}
		
		volume = ap_timers->timers[alarm_index].volume;
		mediaSendCommand(MC_PLAY, 0);
		mediaSendCommand(MC_SETVOLUME, volume);
	}
	
	while(1)
	{
		if(need_draw)
		{
			TM_SYSTEMTIME systemTime;
			TM_GetSystemTime(&systemTime);
			time = systemTime.uHour * 3600 + systemTime.uMinute * 60 + systemTime.uSecond;
			
			GUI_ClearScreen(NULL);//清全屏幕
			GUI_DisplayBattaryLever();
			GUI_ResShowImage(GUI_IMG_ALARM_PICS, count % 3, 48, 0);
			GUI_DisplayTime(4, 32, GUI_TIME_DISPLAY_ALL, GUI_IMG_BIG_NUM, time * 1000);
			GUI_UpdateScreen(NULL);
			need_draw = FALSE;
		}
		key = MESSAGE_Wait();
		switch( key )
		{
		case AP_MSG_RTC:
			if(ap_timers->timers[alarm_index].type == MUSIC_ALARM)
			{
				media_status_t status_buf;
				mediaSendCommand(MC_GETSTATUS, (int) (&status_buf));
				if (status_buf.status == PLAYING_REACH_END)
				{
					mediaSendCommand(MC_STOP, 0);
					mediaSendCommand(MC_PLAY, 0);
				}
			}
			if((count % 20) == 19)
			{
				volume ++;
				mediaSendCommand(MC_SETVOLUME, volume);
			}
			
			if(count == 120) // one minute
			{
				if(ap_timers->timers[alarm_index].type == MUSIC_ALARM)
				{
					mediaSendCommand(MC_STOP, 0);
					mediaSendCommand(MC_CLOSE, 0);
				}
				else
				{
					media_StopInternalAudio();
				}
			}
			else
			{
				count++;
			}
			
			need_draw = TRUE;
			break;
		case AP_MSG_WAIT_TIMEOUT:
		case AP_KEY_NULL:
		case AP_MSG_LOW_POWER:
			break;
		case AP_MSG_STANDBY:
			break;
		case AP_KEY_PLAY|AP_KEY_HOLD:
			if(MESSAGE_IsHold())
			{
				return RESULT_STANDBY;
			}
			break;
		case AP_KEY_POWER|AP_KEY_PRESS:
			return RESULT_STANDBY;
			break;
			
		default:
			result = MESSAGE_HandleHotkey(key);
			if((key & 0xffff) == AP_KEY_DOWN || result != 0)
			{
				if(ap_timers->timers[alarm_index].type == MUSIC_ALARM)
				{
					mediaSendCommand(MC_STOP, 0);
					mediaSendCommand(MC_CLOSE, 0);
				}
				else
				{
					media_StopInternalAudio();
				}
				if(result != 0)
				{
					return result;
				}
				
			}
			if((key & 0xffff) == AP_KEY_UP)
			{
				return RESULT_IDLE;
			}
			
			break;
		}
	};
}

/*********************************************************************************
* Description : 闹钟设置:设置闹钟时间
*
* Arguments   :
*
* Returns     :
*
*
*********************************************************************************/
INT32 SET_ALARMClockSetting(void)
{
	ap_time_t time;
	
	time.hour   = ap_timers->timers[curr_setting_alarm].time / 60;
	time.minute = ap_timers->timers[curr_setting_alarm].time % 60;
	
	GUI_ClockSetting(NULL, &time, GUI_IMG_CLOCKICON, GUI_STR_ALARMCLOCK);
	
	/*Save Clock*/
	ap_timers->timers[curr_setting_alarm].time = time.hour * 60 + time.minute;
	
	return 0;
}

INT32 days_list_callback(INT32 type, INT32 value, INT8 **string)
{
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = GUI_GetString(alarm_days_text[value]);
		return (ap_timers->timers[curr_setting_alarm].days & (1 << value)) != 0;
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		if(ap_timers->timers[curr_setting_alarm].days & (1 << value))
		{
			ap_timers->timers[curr_setting_alarm].days &= ~(1 << value);
		}
		else
		{
			ap_timers->timers[curr_setting_alarm].days |= (1 << value);
		}
		return 1;
	}
}


/*********************************************************************************
* Description : 闹钟设置:设置闹钟周期
*
* Arguments   :
*
* Returns     :
*
*
*********************************************************************************/
INT32 SET_ALARMTimesSetting(void)
{
	UINT8 result;
	
	result = GUI_Display_List(LIST_TYPE_MULTI_SELECT, 0, 7, 0, days_list_callback, FALSE);
	
	return 0;
}

INT32 timer_list_callback(INT32 type, INT32 value, INT8 **string)
{
	INT time, i;
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = alarm_item_buf;
		time = ap_timers->timers[value].time;
		if(ap_timers->timers[value].type == AUDIO_ALARM)
		{
			sprintf(alarm_item_buf, "%02d:%02d%s", time / 60, time % 60, GUI_GetString(GUI_STR_ALARM_AUDIO1 + ap_timers->timers[value].file_or_freq - 1));
		}
		else
		{
			sprintf(alarm_item_buf, "%02d:%02d%s", time / 60, time % 60, GUI_GetString(GUI_STR_ALARMMUSIC + ap_timers->timers[value].type));
		}
		return ap_timers->timers[value].flag & AP_TIMER_ACTIVE;
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		curr_setting_alarm = value;
		return 0;
	}
}

INT32 timer_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	INT time, i;
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		if(value == GUI_MENU_TIMERTYPE)
		{
			if(AUDIO_ALARM == ap_timers->timers[curr_setting_alarm].type)
			{
				return MUSIC_ALARM;
			}
			else
			{
				return ap_timers->timers[curr_setting_alarm].type;
			}
		}
		else if(value == GUI_MENU_TIMERAUDIO)
		{
			if(AUDIO_ALARM == ap_timers->timers[curr_setting_alarm].type)
			{
				return ap_timers->timers[curr_setting_alarm].file_or_freq;
			}
			else
			{
				return RESULT_MUSIC;
			}
		}
		else if(value == GUI_MENU_TIMER)
		{
			return menu_active;
		}
		else
		{
			return 0;
		}
	}
	
	if(MENU_CALLBACK_QUERY_TEXT == type)
	{
		*string = alarm_item_buf;
		if(value == TIMER_ALARM_TIME)
		{
			time = ap_timers->timers[curr_setting_alarm].time;
			sprintf(alarm_item_buf, "%02d:%02d", time / 60, time % 60);
		}
		else if(value == TIMER_ALARM_DAYS)
		{
			if(ap_timers->timers[curr_setting_alarm].days == 0)
			{
				*string = GUI_GetString(GUI_STR_ALARMNOREPEAT);
			}
			else if(ap_timers->timers[curr_setting_alarm].days == 0x7f)
			{
				*string = GUI_GetString(GUI_STR_ALARMEVERYDAY);
			}
			else if(ap_timers->timers[curr_setting_alarm].days == 0x1f)
			{
				*string = GUI_GetString(GUI_STR_ALARMWORKDAY);
			}
			else
			{
				time = -1;
				for(i = 0; i < 7; i++)
				{
					if(ap_timers->timers[curr_setting_alarm].days & (1 << i))
					{
						time = (time + 1) * 8 + i;
					}
				}
				if(time < 7)
				{
					*string = GUI_GetString(alarm_days_text[time]);
				}
				else
				{
					*string = GUI_GetString(GUI_STR_ALARMDAYS);
				}
			}
		}
		else if(value == GUI_MENU_TIMERAUDIO)
		{
			UINT32 len = 0;
			*string = GUI_GetString(GUI_STR_ALARMFILE);
			if(AUDIO_ALARM == ap_timers->timers[curr_setting_alarm].type)
			{
				*string = GUI_GetString(GUI_STR_ALARM_AUDIO1 + ap_timers->timers[curr_setting_alarm].file_or_freq - 1);
			}
			else if(MUSIC_ALARM == ap_timers->timers[curr_setting_alarm].type)
			{
				UINT8 disk = ap_timers->timers[curr_setting_alarm].flag >> 4;
				if (MountDisk(disk) && (FS_GetEntryName(ap_timers->timers[curr_setting_alarm].file_or_freq, AP_TIMER_MAX_NAME, alarm_item_buf) == ERR_SUCCESS) )
				{
					gui_unicode2local(alarm_item_buf, AP_TIMER_MAX_NAME - 2);
					*string = alarm_item_buf;
				}
			}
		}
		else if(value == RESULT_RADIO)
		{
			time = ap_timers->timers[curr_setting_alarm].file_or_freq / 100;
			
			sprintf(alarm_item_buf, "FM%d.%dMHz", time / 10, time % 10);
		}
		return 0;
	}
	
	switch(value)
	{
	case TIMER_ALARM_CLOSE:
		return ap_timers->timers[curr_setting_alarm].flag & AP_TIMER_ACTIVE;
		break;
	case TIMER_ALARM_OPEN:
		return (ap_timers->timers[curr_setting_alarm].flag & AP_TIMER_ACTIVE) == 0;
		break;
	case GUI_MENU_TIMERTYPE:
		if(ap_timers->timers[curr_setting_alarm].type == AUDIO_ALARM)
		{
			return param == MUSIC_ALARM;
		}
		else
		{
			return ap_timers->timers[curr_setting_alarm].type == param;
		}
		break;
	case RESULT_MUSIC:
		return ap_timers->timers[curr_setting_alarm].type == MUSIC_ALARM || ap_timers->timers[curr_setting_alarm].type == AUDIO_ALARM;
		break;
#if APP_SUPPORT_FM
	case RESULT_RADIO:
		return ap_timers->timers[curr_setting_alarm].type == FM_TIMER || ap_timers->timers[curr_setting_alarm].type == FM_REC_TIMER;
		break;
#endif
	default:
		break;
	}
	return 1;
}


INT32 TIMER_Setting()
{
	UINT32  key;
	UINT16 param;
	UINT8 result;
	INT8 curr_setting_type;
	BOOL need_draw = TRUE;
	
	if(g_last_reslt == RESULT_MUSIC)
	{
		hal_HstSendEvent(SYS_EVENT, 0x09090909);
		curr_setting_type = MUSIC_ALARM;
	}
#if APP_SUPPORT_FM
	else if(g_last_reslt == RESULT_RADIO)
	{
		curr_setting_type = FM_TIMER;
	}
#endif
	
	while(1)
	{
		if(g_timer_setting_flag == 0)
		{
			result = GUI_Display_List(LIST_TYPE_MULTI_SELECT, 0, AP_MAX_TIMER, curr_setting_alarm, timer_list_callback, FALSE);
			if(result == RESULT_IGNORE)
			{
				return RESULT_SYSTEM;
			}
			if(result != 0)
			{
				return result;
			}
		}
		
		if(curr_setting_type == MUSIC_ALARM)
		{
			hal_HstSendEvent(SYS_EVENT, 0x09090909);
			if(ap_timers->timers[curr_setting_alarm].type != MUSIC_ALARM)
			{
				ap_timers->timers[curr_setting_alarm].type = MUSIC_ALARM;
			}
			ap_timers->timers[curr_setting_alarm].flag |= AP_TIMER_ACTIVE;
			if(curr_active_alarm == -1)
			{
				curr_active_alarm = 0;
			}
			if(g_music_vars->location.file_entry > 0)
			{
				ap_timers->timers[curr_setting_alarm].file_or_freq = g_music_vars->location.file_entry;
			}
			ap_timers->timers[curr_setting_alarm].flag |= (g_music_vars->location.disk << 4);
			ap_timers->timers[curr_setting_alarm].volume = g_music_vars->volume;
		}
#if APP_SUPPORT_FM
		else if(curr_setting_type == FM_TIMER)
		{
			if(ap_timers->timers[curr_setting_alarm].type == MUSIC_ALARM)
			{
				ap_timers->timers[curr_setting_alarm].type = FM_TIMER;
			}
			ap_timers->timers[curr_setting_alarm].flag |= AP_TIMER_ACTIVE;
			if(curr_active_alarm == -1)
			{
				curr_active_alarm = 0;
			}
			if(FMStatus->freq > 0)
			{
				ap_timers->timers[curr_setting_alarm].file_or_freq = FMStatus->freq;
			}
			ap_timers->timers[curr_setting_alarm].volume = FMStatus->volume;
		}
#endif
		
		curr_setting_type = -1;
		
		while(1)
		{
			if(need_draw)
			{
				result = GUI_Display_Menu(GUI_MENU_TIMER, timer_menu_callback);
				if(result == RESULT_IGNORE)
				{
					g_timer_setting_flag = 0;
					break;
				}
				if(result != 0)
				{
					g_timer_setting_flag = 1;
					return result;
				}
			}
			key = MESSAGE_Wait();
			need_draw = TRUE;
			switch( key )
			{
			case TIMER_ALARM_OPEN:
				ap_timers->timers[curr_setting_alarm].flag |= AP_TIMER_ACTIVE;
				TIMER_SetAlarm(0);
				menu_active = 3;
				break;
			case TIMER_ALARM_CLOSE:
				ap_timers->timers[curr_setting_alarm].flag &= ~AP_TIMER_ACTIVE;
				TIMER_SetAlarm(0);
				menu_active = 4;
				break;
			case TIMER_ALARM_TIME:
				SET_ALARMClockSetting();
				TIMER_SetAlarm(0);
				menu_active = 8;
				break;
			case TIMER_ALARM_TYPE:
				GUI_GetMenuResult(&param);
				ap_timers->timers[curr_setting_alarm].type = param;
				menu_active = param;
				break;
			case TIMER_ALARM_DAYS:
				SET_ALARMTimesSetting();
				menu_active = 9;
				break;
			case TIMER_ALARM_SELECT:
				//menu_active = 9;
				ap_timers->timers[curr_setting_alarm].type = AUDIO_ALARM;
				GUI_GetMenuResult(&param);
				ap_timers->timers[curr_setting_alarm].file_or_freq = param;
				break;
			case AP_KEY_MODE|AP_KEY_UP:
			case AP_KEY_PLAY|AP_KEY_UP:
				need_draw = FALSE;
				break;
			default:
				break;
			}
		}
	};
}


void TIMER_SetAlarm(BOOL maintask)
{
	INT i, first = -1, days;
	UINT32 time, cur_time;
	TM_SYSTEMTIME systime;
	//if(maintask)
	//    NVRAMRead(&ap_timers,VM_AP_TIMER,sizeof(ap_timers));
	ap_timers = (timer_vars_t*)NVRAMGetData(VM_AP_TIMER, sizeof(timer_vars_t));
	
	TM_GetSystemTime(&systime);
	cur_time = systime.uHour * 60 + systime.uMinute;
	time = cur_time;
	
	for(i = 0; i < AP_MAX_TIMER; i++)
	{
		if(ap_timers->timers[i].flag & AP_TIMER_ACTIVE)
		{
			if(ap_timers->timers[i].days != 0)
			{
				if(ap_timers->timers[i].time > cur_time)
				{
					if((ap_timers->timers[i].days & (1 << systime.uDayOfWeek)) == 0)
					{
						continue;
					}
				}
				else
				{
					if((ap_timers->timers[i].days & (1 << (systime.uDayOfWeek + 1) % 7)) == 0)
					{
						continue;
					}
				}
			}
			if(ap_timers->timers[i].time > cur_time)
			{
				if(time <= cur_time || ap_timers->timers[i].time < time)
				{
					time = ap_timers->timers[i].time;
					first = i;
				}
			}
			else
			{
				if(time <= cur_time && ap_timers->timers[i].time < time)
				{
					time = ap_timers->timers[i].time;
					first = i;
				}
			}
		}
	}
	if(first < 0)
	{
		TM_DisableAlarm();
		curr_active_alarm = -1;
		return;
	}
	
	if(time < cur_time)
	{
		days = month_days[systime.uMonth - 1];
		if(IsLeapYear(systime.uYear) && (systime.uMonth == 2))
		{
			days++;
		}
		if(systime.uDay == days)
		{
			systime.uDay = 1;
			if(systime.uMonth == 12)
			{
				systime.uMonth = 1;
				systime.uYear ++;
			}
			else
			{
				systime.uMonth ++;
			}
		}
		else
		{
			systime.uDay ++;
		}
	}
	systime.uHour = time / 60;
	systime.uMinute = time % 60;
	systime.uSecond = 0;
	
	curr_active_alarm = first;
	TM_SetAlarm(&systime);
}


/*********************************************************************************
* Description :
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 TIMER_Entry(INT32 param)
{
	UINT8 result = RESULT_MAIN; //POWERON;
	INT i;
	
	MESSAGE_Initial(g_comval);
	
	//NVRAMRead(&FMStatus,VM_AP_RADIO,sizeof(FMStatus));
	//NVRAMRead(& g_music_vars, VM_AP_MUSIC, sizeof(g_music_vars));
	
	//NVRAMRead(&ap_timers,VM_AP_TIMER,sizeof(ap_timers));
	
#if APP_SUPPORT_FM
	FMStatus = (FM_play_status_t*)NVRAMGetData(VM_AP_RADIO, sizeof(FM_play_status_t));
#endif
	g_music_vars = (music_vars_t*)NVRAMGetData(VM_AP_MUSIC, sizeof(music_vars_t));
	
	ap_timers = (timer_vars_t*)NVRAMGetData(VM_AP_TIMER, sizeof(timer_vars_t));
	
	if(ap_timers->magic != MAGIC_TIMER)
	{
		memset(ap_timers, 0, sizeof(ap_timers));
		for(i = 0; i < AP_MAX_TIMER; i++)
		{
			ap_timers->timers[i].time = 360; // set default clock to 6:00
			ap_timers->timers[i].file_or_freq = 1;
		}
		ap_timers->magic = MAGIC_TIMER;
	}
	
	if(param == 0)
	{
		result = TIMER_Setting();
	}
	else
	{
		result = TIMER_Alarm();
	}
	
	//NVRAMWrite(&ap_timers,VM_AP_TIMER,sizeof(ap_timers));
	NVRAMWriteData();
	
	return result;
}


#endif // APP_SUPPORT_LCD

