/********************************************************************************
*                File selector
*            enhanced library
*
*        (c) Copyright, Rda Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_common.h"
#include "ap_message.h"
#include "ap_music.h"
#include "ap_media.h"
#include "bt.h"
#include "a2dp.h"
#include "event.h"
#include "mci.h"
#include "MainTask.h"

#define AUD_FULL_PROGRESS_CNT (9999)

INT32  g_current_fd = -1;
UINT32 new_play_time = 0;//record current
UINT8  g_media_mode = 0;
mci_type_enum g_currentType = 0;
BOOL tone_status = 0; //warkey 2.0
#if APP_SUPPORT_MUSIC==1
extern UINT16   ERROR_EXIT;
#endif
#if APP_SUPPORT_FADE_INOUT==1
#define MEDIA_FADE_SLOW_STEPS        5
#define MEDIA_FADE_FAST_STEPS        3

static UINT8 g_fade_timer = 0;
static INT8 g_fade_volume = 0;
INT8 g_fade_volume_backup = -1;
static INT8 g_fade_step = 0;
#else
INT8 g_fade_volume_backup = 0;
#endif
extern INT32  g_current_module;//warkey 2.1
//static UINT8  AUDPLY_COUNT_PLAY_TIMER;//定时器

#define AUDIO_PLAYER_PLAY_TIMER      800      //更新播放时间的定时器周期


media_status_t g_media_status = {0};
UINT32 g_play_start_pos = 0;

static void file_play_finished( UINT16 result )
{
    app_trace(APP_MAIN_TRC,"file_play_finished:g_current_fd=%d,g_media_status.status=%d,result;%d\r\n",g_current_fd,g_media_status.status,result);
    hal_HstSendEvent(SYS_EVENT,0x11111111);    
    if(g_current_fd >= 0)
    {
        app_trace(APP_MAIN_TRC,"file_play_finished!\r\n");
        FS_Close(g_current_fd);     
        g_current_fd = -1;
	if(MCI_ERR_INVALID_FORMAT==result)
	{
		g_media_status.status= PLAYING_ERROR;

	}
	 else
	{
		g_media_status.status = PLAYING_REACH_END;
#if APP_SUPPORT_MUSIC==1
		 ERROR_EXIT=0;
#endif
	 }
        // wake up app task     
        MESSAGE_SetEvent(AP_KEY_NULL);
    }
}

/*****************************************************************************
* FUNCTION
*     MUSIC_AudplyTimer
* DESCRIPTION
*
* IMPACT
*
* PARAMETERS
*     void
* RETURNS
*     void
* GLOBALS AFFECTED
*
*****************************************************************************/
void MUSIC_AudplyTimer(void)
{
	app_trace(APP_MAIN_TRC, "[MUSIC_AudplyTimer]\r\n");
	if( g_media_status.status == PLAYING_PLAYING )
	{
		//      new_play_time++;
		//        COS_ChangeTimer( AUDPLY_COUNT_PLAY_TIMER, AUDIO_PLAYER_PLAY_TIMER );
	}
}

static void record_callback( void )
{
	g_media_status.status = RECORDING_DISK_ERROR;
	hal_HstSendEvent(SYS_EVENT, 0x22222222);
}

#if APP_SUPPORT_FADE_INOUT==1
void mediaFadeTimeout(void *param)
{
	COS_ChangeTimer(g_fade_timer, 30);
	if(g_fade_step > 0)
	{
		g_fade_volume += g_fade_step;
		if(g_fade_volume > g_media_status.volume)
		{
			g_fade_volume = g_media_status.volume;
		}
		SetPAVolume(g_fade_volume);
		if(g_fade_volume == g_media_status.volume)
		{
			COS_KillTimer(g_fade_timer);
			g_fade_timer = 0;
		}
	}
	else
	{
		if(g_fade_volume_backup == -1)
		{
			g_fade_volume_backup = g_fade_volume;
		}
		
		g_fade_volume += g_fade_step;
		if(g_fade_volume < 0)
		{
			g_fade_volume = 0;
		}
		
		SetPAVolume(g_fade_volume);
		if(g_fade_volume == 0)
		{
			COS_KillTimer(g_fade_timer);
			g_fade_timer = 0;
			//            SetPAVolume(g_fade_volume_backup);
			//            g_fade_volume_backup = -1;
			MESSAGE_SetEvent(0);
		}
	}
	
}
#endif
static void delay_open_external_pa()//warkey 2.0
{
	gpio_SetMute(FALSE);
}

extern char curr_file_name[128];
BOOL mediaSendCommand(UINT8 cmd, INT32 param)
{
	INT32 res;
	if(cmd != 0x80 && cmd != 0x83)
	{
		hal_HstSendEvent(SYS_EVENT, 0x82CED000 + cmd);
	}
	
	switch ( cmd )
	{
	case MC_OPEN:
		{
			Open_param_t *pParam = (Open_param_t*)param;
			if(g_current_fd >= 0)
			{
				FS_Close(g_current_fd);
			}
			if(pParam->mode == MEDIA_PLAY)
			{
				g_current_fd = FS_OpenDirect(pParam->file_entry, FS_O_RDONLY, 0);
#if APP_SUPPORT_LCD==1
				FS_GetFileName(g_current_fd, sizeof(curr_file_name), curr_file_name);
#endif
			}
			else
			{
#if APP_SUPPORT_RECORD==1
				WCHAR filename[12];
				filename[0] = pParam->file_name[0];
				filename[1] = pParam->file_name[1];
				filename[2] = pParam->file_name[2];
				filename[3] = pParam->file_name[3] ;
				filename[4] = pParam->file_name[4] ;
				filename[5] = pParam->file_name[5] ;
				filename[6] = '.';
				filename[7] = pParam->file_name[8];
				filename[8] = pParam->file_name[9];
				filename[9] = pParam->file_name[10];
				filename[10] = 0;
				if(pParam->mode == MEDIA_RECORD)
				{
					hal_HstSendEvent(SYS_EVENT, 0x19880203);
					g_current_fd = FS_Open(filename, FS_O_RDWR | FS_O_TRUNC | FS_O_CREAT, 0);
				}
				else if(pParam->mode == MEDIA_PLAYREC)
				{
					hal_HstSendEvent(SYS_EVENT, 0x19880204);
					g_current_fd = FS_Open(filename, FS_O_RDONLY, 0);
				}
				else
#endif
				{
					hal_HstSendEvent(SYS_EVENT, 0x19880205);
					return FALSE;
				}
			}
			g_media_mode = pParam->mode;
			
			hal_HstSendEvent(SYS_EVENT, 0x19880210 + pParam->type);
			switch(pParam->type)
			{
			case Music_MP3:
				g_currentType = MCI_TYPE_DAF;
				break;
			case Music_AAC:
				g_currentType = MCI_TYPE_AAC;
				break;
			case Music_WAV:
				g_currentType = MCI_TYPE_WAV;
				break;
			case Music_SBC:
				g_currentType = MCI_TYPE_SBC;
				break;
			case Music_WMA:
				g_currentType = MCI_TYPE_WMA;
				break;
			default:
				g_currentType = 0;
				return FALSE; // unsupport file type
			}
			if(g_media_status.status == PLAYING_INTERNAL)
			{
				media_StopInternalAudio();
			}
			
			if(g_media_mode == MEDIA_PLAY)
			{
				g_media_status.status = PLAYING_STOP;
			}
			else
			{
				g_media_status.status = RECORDING_STOP;
			}
			if(g_current_fd < 0)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19880220);
				return FALSE;
			}
			break;
		}
		
	case MC_GETFILEINFO:
		if(g_current_fd < 0)
		{
			return FALSE;
		}
		{
			AudDesInfoStruct *pInfo = (AudDesInfoStruct*)param;
			memset(pInfo, 0, sizeof(AudDesInfoStruct));
			MCI_AudioGetFileInformation(g_current_fd, pInfo, g_currentType);
			//Audio_GetDetailsInfo(g_current_fd, pInfo,g_currentType);
			g_media_status.total_time =  pInfo->time;
		}
		break;
		
	case MC_GETTOTALTIME:
		if(g_current_fd < 0)
		{
			return FALSE;
		}
		{
			MCI_ProgressInf time;
			INT32 sec;
			UINT32 *pTime = (UINT32*)param;
			MCI_AudioGetDurationTime(g_current_fd, g_currentType, 0, 10000, &time);
			*pTime = g_media_status.total_time =  time.DurationTime;
		}
		break;
		
	case MC_PLAY:
		{
			UINT32 totaltime = 0, progress = 0;
			if(g_current_module == FUN_MUSIC) //warkey 2.1
			{
				if(g_comval->Volume > 0)
				{
					COS_SetTimer(400, delay_open_external_pa, NULL, COS_TIMER_MODE_SINGLE); //warkey 2.0
				}
			}
			hal_HstSendEvent(SYS_EVENT, g_current_fd);
			hal_HstSendEvent(SYS_EVENT, param);
			
			if(g_current_fd < 0)
			{
				return FALSE;
			}
			hal_HstSendEvent(SYS_EVENT, g_media_status.status);
			
			app_trace(APP_MAIN_TRC, "[MC_PLAY]status=%d,param=%d\r\n", g_media_status.status, param);
			media_StopInternalAudio();
			
			if (g_media_status.status == PLAYING_ERROR)
			{
				return FALSE;
			}
			
			if(g_media_status.status == PLAYING_PAUSE)
			{
				new_play_time = ((g_media_status.progress * totaltime) / (AUD_FULL_PROGRESS_CNT + 1) + new_play_time) / 2;
				MCI_AudioResume(g_current_fd);
				g_media_status.status = PLAYING_PLAYING;
				return TRUE;
			}
#if 0
			if(g_currentType)
			{
				g_play_start_pos = param;
				res = MCI_AudioPlay(0, g_current_fd, g_currentType, file_play_finished, param);
			}
			else
			{
				res = MCI_ERR_BAD_FORMAT;
			}
			
			hal_HstSendEvent(SYS_EVENT, res);
			hal_HstSendEvent(SYS_EVENT, 0x19880300);
			if(res != MCI_ERR_NO)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19880400 + res);
				g_media_status.status = PLAYING_ERROR;
				return FALSE;
			}
#endif
#if APP_SUPPORT_FADE_INOUT==1
			COS_KillTimer(g_fade_timer);
			g_fade_step = (g_media_status.volume + MEDIA_FADE_SLOW_STEPS - 1) / MEDIA_FADE_SLOW_STEPS;
			if(g_fade_step == 0)
			{
				g_fade_step = 1;
			}
#if APP_SUPPORT_DELAY_VOLUME==1
			g_fade_volume = 0;
#else
			g_fade_volume = 1;
#endif
			SetPAVolume(g_fade_volume);
			g_fade_timer = COS_SetTimer(1000, mediaFadeTimeout, NULL, COS_TIMER_MODE_PERIODIC);
#endif
			
#if 1
			if(g_currentType)
			{
				g_play_start_pos = param;
				res = MCI_AudioPlay(0, g_current_fd, g_currentType, file_play_finished, param);
			}
			else
			{
				res = MCI_ERR_BAD_FORMAT;
			}
			
			hal_HstSendEvent(SYS_EVENT, res);
			hal_HstSendEvent(SYS_EVENT, 0x19880300);
			if(res != MCI_ERR_NO)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19880400 + res);
				g_media_status.status = PLAYING_ERROR;
				return FALSE;
			}
#endif
			
			MCI_AudioSetEQ(g_media_status.eq);
			g_media_status.status = PLAYING_PLAYING;
			new_play_time = (progress * totaltime) / (AUD_FULL_PROGRESS_CNT + 1);
			
			break;
		}
		
		
#if APP_SUPPORT_RECORD==1
	case MC_RECORD:
		{
			if(g_current_fd < 0)
			{
				return FALSE;
			}
			media_StopInternalAudio();
			
			if(g_media_status.status == RECORDING_PAUSE)
			{
				res = MCI_AudioRecordResume();
			}
			else
			{
				res = MCI_AudioRecordStart(g_current_fd, MCI_TYPE_WAV_DVI_ADPCM, 7, record_callback, NULL);//g_file_type
			}
			if(res == MCI_ERR_NO)
			{
				g_media_status.status = RECORDING_RECORDING;
				return TRUE;
			}
			else
			{
				g_media_status.status = RECORDING_EXCEPTION;
				return FALSE;
			}
		}
#endif
		
#if APP_SUPPORT_BLUETOOTH==1
	case MC_PLAY_SCO:
		media_StopInternalAudio();
		res = MCI_StartBTSCO((UINT16)param);
		if(res == MCI_ERR_NO)
		{
			g_media_status.status = PLAYING_PLAYING;
			return TRUE;
		}
		else
		{
			g_media_status.status = PLAYING_ERROR;
			return FALSE;
		}
		break;
	case MC_PLAY_A2DP:
		media_StopInternalAudio();
		res = MCI_PlayBTStream((bt_a2dp_audio_cap_struct*)param);
		if(res == MCI_ERR_NO)
		{
			g_media_status.status = PLAYING_PLAYING;
			return TRUE;
		}
		else
		{
			g_media_status.status = PLAYING_ERROR;
			return FALSE;
		}
		break;
#endif
		
#if APP_SUPPORT_USBAUDIO==1
	case MC_PLAY_USB:
		res = MCI_AudioPlay(0, 0, MCI_TYPE_USB, NULL, 0);
		if(res == MCI_ERR_NO)
		{
			g_media_status.status = PLAYING_PLAYING;
			return TRUE;
		}
		else
		{
			g_media_status.status = PLAYING_ERROR;
			return FALSE;
		}
		break;
#endif
	case MC_SETEQ:
		{
			//if(g_media_status.status == PLAYING_PLAYING)
			g_media_status.eq = param;
			MCI_AudioSetEQ(param);
			break;
		}
		
	case MC_PAUSE:
		{
			if(g_media_status.status == PLAYING_PLAYING)
			{
				MCI_PlayInf audiofile_lastPlayInfo ;
				MCI_AudioPause();
				MCI_AudioGetPlayInformation(&audiofile_lastPlayInfo);
				g_media_status.progress = audiofile_lastPlayInfo.PlayProgress;
				g_media_status.status = PLAYING_PAUSE;
			}
#if APP_SUPPORT_RECORD==1
			else  if(g_media_status.status == RECORDING_RECORDING)
			{
				MCI_AudioRecordPause();
				g_media_status.status = RECORDING_PAUSE;
			}
#endif
			break;
		}
		
	case MC_STOP:
		if(g_current_module == FUN_MUSIC) //warkey 2.1
		{
			gpio_SetMute(TRUE);//warkey 2.0
		}
		hal_HstSendEvent(SYS_EVENT, g_media_status.status);
		if(g_media_status.status == PLAYING_PLAYING || g_media_status.status == PLAYING_REACH_END)
		{
#if APP_SUPPORT_FADE_INOUT==1
			extern UINT8 g_cur_volume;
			UINT32 key;
			COS_KillTimer(g_fade_timer);
			g_fade_timer = 0;
			if(param != no_fade && g_cur_volume > 2)
			{
				if(param == fast_fade)
				{
					g_fade_step = (-g_cur_volume - MEDIA_FADE_FAST_STEPS + 1) / MEDIA_FADE_FAST_STEPS;
				}
				else
				{
					g_fade_step = (-g_cur_volume - MEDIA_FADE_SLOW_STEPS + 1) / MEDIA_FADE_SLOW_STEPS;
				}
				if(g_fade_step == 0)
				{
					g_fade_step = -1;
				}
				g_fade_volume = g_cur_volume;
				//SetPAVolume(g_fade_volume);
				g_fade_timer = COS_SetTimer(100, mediaFadeTimeout, NULL, COS_TIMER_MODE_PERIODIC);
				while(g_fade_timer)
				{
					key = MESSAGE_Wait();
					if((key & 0xffff) != AP_KEY_UP && key != AP_KEY_NULL && key != AP_MSG_RTC && key != AP_MSG_CHARGING)
					{
						COS_KillTimer(g_fade_timer);
						g_fade_timer = 0;
						MESSAGE_SetEvent(key);
						break;
					}
				};
			}
#endif
			hal_HstSendEvent(SYS_EVENT, 0x19880033);
			MCI_AudioStop();
			g_media_status.status = PLAYING_STOP;
		}
		else if(g_media_status.status == RECORDING_RECORDING || g_media_status.status == RECORDING_PAUSE)
		{
			MCI_AudioRecordStop();
			g_media_status.status = RECORDING_STOP;
		}
		break;
	case MC_CLOSE:
		{
			if(g_current_fd >= 0)
			{
				FS_Close(g_current_fd);
				g_current_fd = -1;
			}
			break;
		}
		
	case MC_SETVOLUME:
		{
			g_media_status.volume = param;
			if(PLAYING_PLAYING == g_media_status.status)
			{
				SetPAVolume(param);
			}
			break;
		}
		
	case MC_GETTIME:
		{
			UINT32 *pTime = (UINT32*)param;
			if(pTime == NULL)
			{
				return FALSE;
			}
			if((PLAYING_PLAYING == g_media_status.status) || (PLAYING_AB == g_media_status.status))
			{
				MCI_PlayInf playInfo;
				
				MCI_AudioGetPlayInformation(&playInfo);
				g_media_status.progress = playInfo.PlayProgress;
				*pTime = ((g_media_status.total_time / 100 *  g_media_status.progress) / 100) + 100;
#if APP_SUPPORT_FADE_INOUT==1
				if(g_media_status.total_time - *pTime < 1000) // one second to end
				{
					extern UINT8 g_cur_volume;
					COS_KillTimer(g_fade_timer);
					g_fade_timer = 0;
					if(g_cur_volume > 2)
					{
						g_fade_step = (-g_cur_volume - MEDIA_FADE_SLOW_STEPS + 1) / MEDIA_FADE_SLOW_STEPS;
						if(g_fade_step == 0)
						{
							g_fade_step = -1;
						}
						g_fade_volume = g_cur_volume;
						g_fade_timer = COS_SetTimer(100, mediaFadeTimeout, NULL, COS_TIMER_MODE_PERIODIC);
					}
				}
#endif
			}
#if APP_SUPPORT_RECORD==1
			else if(g_media_status.status == RECORDING_RECORDING || g_media_status.status == RECORDING_PAUSE)
			{
				*pTime = MCI_AudioRecordGetTime() + 100;
			}
#endif
			break;
		}
		
	case MC_GETSTATUS:
		{
			if(param)
			{
				MCI_PlayInf playInfo;
				media_status_t *pStatus = (media_status_t*) param;
				
				if(g_media_mode == MEDIA_PLAY)
				{
					MCI_AudioGetPlayInformation(&playInfo);
					g_media_status.progress = playInfo.PlayProgress;
				}
				*pStatus = g_media_status;
			}
			break;
		}
		
	case MC_GETSTARTPOS:
		{
			UINT32 *pPos = (UINT32*)param;
			*pPos = g_play_start_pos;
		}
		break;
		
	default:
		{
			app_trace(APP_MAIN_TRC, "unknow command:%d,%d.\r\n", cmd, param);
			break;
		}
	}
	return TRUE;
}


UINT32 MUSIC_GetPlaySeconds(void)
{
	return new_play_time;
}

BOOL  AudioPlayTimeTransfer(UINT32 duration, ap_time_t *time)
{
	UINT32 duration_h = 0, duration_m = 0, duration_s = 0;
	
	if(NULL == time)
	{
		return FALSE;
	}
	/* time */
	if (duration > 0)
	{
		duration /= 1000;
		duration_m  = duration / 60 ;
		duration_s  = duration % 60 ;
		if(duration_m > 59)
		{
			duration_h = duration_m / 60 ;
			duration_m = duration_m % 60 ;
			
			time->hour   = duration_h;
			time->minute = duration_m;
			time->second = duration_s;
			
			app_trace(APP_MAIN_TRC, "AudioPlayTimeTransfer:%02d:%02d:%02d", (INT32)duration_h, (INT32)duration_m, (INT32)duration_s);
		}
		else
		{
			time->hour    = 0;
			time->minute = duration_m;
			time->second = duration_s;
			app_trace(APP_MAIN_TRC, "AudioPlayTimeTransfer:%02d:%02d", (INT32)duration_m, (INT32)duration_s);
		}
	}
	
	return TRUE;
}


void  AudioGetDuration(ap_time_t *t, UINT32 *duration)
{
	UINT32 totaltime = 0;
	
	if(t->hour)
	{
		totaltime += t->hour * 3600;
	}
	
	if(t->minute)
	{
		totaltime += t->minute * 60;
	}
	
	if(t->second)
	{
		totaltime += t->second;
	}
	
	*duration = totaltime;
}

#if APP_SUPPORT_NUMBER_VOICE==1//warkey 2.1
static bool is_number_voice = FALSE;
#endif

static void audio_play_finished( UINT16 result)
{ 
	if(g_media_status.status == PLAYING_INTERNAL)
	{
		hal_HstSendEvent(SYS_EVENT, 0x11112222);
		//        g_media_status.status = PLAYING_STOP;
	}
	media_StopInternalAudio();
	hal_HstSendEvent(SYS_EVENT, 0x22221111);
	hal_HstSendEvent(SYS_EVENT, g_media_status.status);

	if(result==MCI_ERR_INVALID_FORMAT)
	{
		g_media_status.status=PLAYING_ERROR;
		if(g_current_fd >= 0)
		{
			app_trace(APP_MAIN_TRC,"callback file_play_finished!\r\n");
			FS_Close(g_current_fd);     
			g_current_fd = -1;
		}
	}	
}

UINT8 media_GetInternalStatus()
{
	return g_media_status.status;
}

UINT32 media_PlayInternalAudio(UINT8 audio_id, UINT8 times, BOOL waitKey)
{
	UINT8 *data = NULL;
	UINT8 type;
	UINT32 length;
	
	data = gui_get_audio(audio_id, &type, &length);
	
	app_trace(APP_MAIN_TRC, "Enter media_PlayInternalAudio, g_media_status.status = %d, type = %d, data = %x", g_media_status.status, type, data);
	
	if(g_media_status.status == PLAYING_INTERNAL)
	{
		MCI_StopInternalAudio();
		g_media_status.status = PLAYING_STOP;
	}
	
	if(data)
	{
#if APP_SUPPORT_NUMBER_VOICE==1//warkey 2.1
		if(audio_id >= GUI_AUDIO_NUMBER_0 && audio_id <= GUI_AUDIO_NUMBER_9)
		{
			is_number_voice = TRUE;
		}
#endif
#if APP_SUPPORT_NUMBER_VOICE==1//播提示音前先MUTE功放，防止提示音前PA声，报号不需要MUTE功放
		if(!is_number_voice)
#endif
			gpio_SetMute(TRUE);
		
		SetTONEVolume(15);
		COS_SetTimer(20, SetPA_For_TonePlay, NULL, COS_TIMER_MODE_SINGLE);
		
		hal_HstSendEvent(SYS_EVENT, 0x19885001);
		hal_HstSendEvent(SYS_EVENT, g_media_status.status);
		
		if(g_media_status.status == PLAYING_STOP || g_media_status.status == PLAYING_ERROR || g_media_status.status == RECORDING_STOP)
		{
			tone_status = 1;
		#ifdef EXTERNAL_PA_SUPPORT
            		gpio_SetMute_Pop(FALSE);
			COS_Sleep(150);
		#endif			
			MCI_PlayInternalAudio(type, length, data, audio_play_finished, times);
			
			g_media_status.status = PLAYING_INTERNAL;
		}
		else if(g_media_status.status == PLAYING_PLAYING && type == 1)
		{
			hal_HstSendEvent(SYS_EVENT, 0x19885002);
			hal_HstSendEvent(SYS_EVENT, length);
			hal_HstSendEvent(SYS_EVENT, data);
			hal_HstSendEvent(SYS_EVENT, times);
			tone_status = 0;
			MCI_PlaySideTone(length, data, times);
		}
		while(waitKey)
		{
			UINT32 key = MESSAGE_Wait();
			if ( (key != AP_MSG_RTC) && (key != 0) && ((key & 0xffff) != AP_KEY_UP) && ((key & 0xffff) != AP_KEY_HOLD) && ((key & 0xffff) != AP_MSG_WAIT_TIMEOUT))
			{
				media_StopInternalAudio();
				hal_HstSendEvent(SYS_EVENT, 0x19886000);
				hal_HstSendEvent(SYS_EVENT, key);
				return key;
			}
			if(g_media_status.status == PLAYING_STOP)
			{
				return 0;
			}
		}
	}
	return 0;
}

void media_StopInternalAudio(void)
{
	if(g_media_status.status == PLAYING_INTERNAL)
	{
#if APP_SUPPORT_NUMBER_VOICE==1//warkey 2.1
		if(is_number_voice == TRUE)
		{
			is_number_voice = FALSE;
			COS_EVENT ev = {0};
			ev.nEventId = EV_BT_HUMAN_VOICE_PLAY;
			COS_SendEvent(MOD_APP,  &ev , COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		}
#endif
		SetPA_For_ToneStop();
		SetTONEVolume(0);
		MCI_StopInternalAudio();
		tone_status = 0;
		g_media_status.status = PLAYING_STOP;
	}
}

