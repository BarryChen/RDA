/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_record.h"
#include "ap_media.h"
#include "ap_music.h"
#include "event.h"

#if APP_SUPPORT_RECORD==1
#define MUSIC_FFPLAY_STEP           7  // second

#define RECORD_SET_SOURCE               AP_MSG_USER+1
#define RECORD_SET_SAMPRATE             AP_MSG_USER+2
#define RECORD_SELECT_FOLDER            AP_MSG_USER+3

//Common variables
extern INT8            m_adagencDataBuffer[];
extern UINT8            fselDirLayer;  //dir  layer
extern UINT8            fselDirNo[];    //Number of current valid directory
extern BOOL            ui_run_realtime;
extern BOOL            ui_auto_update;

//显示区域
extern region_t        region_msg;


UINT32                  g_file_size = 0;             //当前录音文件的page数
UINT32                  g_rec_time = 0;              //实时显示录音的时间
UINT32                  g_rec_bitrate = 0;
UINT32                  g_free_space = 0;            //剩余磁盘空间

//本模块的全局变量，只有在进入本模块的时候调用
record_vars_t *g_record_vars = NULL;
//录音总文件数目
UINT8   g_file_num = 0;              //录音文件总个数
UINT8 g_max_file_num = 0; // max file number
INT8   g_play_direct = 0;   // search play file dir

UINT32 g_totalTime = 0;
UINT32 g_file_date = 0;
UINT32 g_file_time = 0;

static UINT8 need_draw = 0;    //0, no need, 1, draw all, 2, only current time
static BOOL need_parse = TRUE;  //需要分析文件个数,总时间

//当前文件的全局变量
static UINT8    g_rec_from = 0;    //当前的录音源
//当前状态
enum
{
	stop,
	prepareing,
	paused,
	recording,
	playing,
	play_paused,
	wait_to_save,
	saving
}	g_rec_status = stop;

extern const TCHAR ExtVoice[];

static void RECORD_Display(void);
static BOOL RECORD_CheckLowBatt(void);
static INT32 RECORD_Stop(BOOL auto_start);
static void RECORD_FileName(void);

/*********************************************************************************
* Description : 设置录音缺省目录\Record\Music
*
* Arguments   :
*
* Returns     :
*
* Notes       : 把\Record\Voice做为当前目录
*
*********************************************************************************/
BOOL RECORD_ResetDir(file_location_t *location, WCHAR *DirName)
{
	// INT32  status;
	//UINT32 inode;
	
	//    tcscpy(location->path, DirName);
	
	//    location->subdir_entry[11]    = 0x00;
	//    location->disk        = FS_DEV_TYPE_TFLASH;
	location->file_name[0] = 0;
	
	FS_ChangeDir(FS_CD_ROOT);
	
	if(ERR_SUCCESS != FS_ChangeDir(RECORD_DIR))
	{
		if(ERR_SUCCESS != FS_MakeDir(RECORD_DIR, 0))
		{
			return FALSE;    // 创建目录失败
		}
		FS_ChangeDir(RECORD_DIR);
	}
	
	if(ERR_SUCCESS != FS_ChangeDir(DirName))
	{
		if(ERR_SUCCESS != FS_MakeDir(DirName, 0))
		{
			return FALSE;    // 创建目录失败
		}
		FS_ChangeDir(DirName);
	}
	
	FS_GetSubDirEntry(&(location->subdir_entry));
	
	//    location->dirLayer  = 1;
	//    FS_ChangeDir(DirName);
	//    FS_GetCurDir(sizeof(location->path),location->path);
	
	return TRUE;
}


/*********************************************************************************
* Description : 主函数
*
* Arguments   :
        param:
            0, 不自动开始录音
            1, 自动开始录音
*
* Returns     : 结果消息或0
*
* Notes       :
*
*********************************************************************************/
INT32 RECORD_Entry(INT32 param)
{
	INT32 result = 0;
	BOOL disk_ok = FALSE;
	
	BOOL status;
	WCHAR pathname[20];
	
	GUI_ClearScreen(NULL);
	
	MESSAGE_Initial(g_comval);
	
	//NVRAMRead(&g_record_vars, VM_AP_RECORD, sizeof(g_record_vars));
	g_record_vars = (record_vars_t*)NVRAMGetData(VM_AP_RECORD, sizeof(record_vars_t));
	
	if(g_record_vars->maigc != MAGIC_RECORD)
	{
		g_record_vars->maigc = MAGIC_RECORD;
		g_record_vars->rec_sample_rate = 16000; // 16K
		g_record_vars->wav_num = 1;
		g_record_vars->volume = 8;
		g_record_vars->location.disk = FS_DEV_TYPE_TFLASH;
	}
#if APP_SUPPORT_INDEPEND_VOLUMNE==0
	g_record_vars->volume = g_comval->Volume;
#endif
	
	
	//初始化文件系统
	if(MountDisk(g_record_vars->location.disk))
	{
		disk_ok = TRUE;
	}
	else if(MountDisk(FS_DEV_TYPE_TFLASH))
	{
		disk_ok = TRUE;
		g_record_vars->location.disk = FS_DEV_TYPE_TFLASH;
	}
#if APP_SUPPORT_USB
	else if(MountDisk(FS_DEV_TYPE_USBDISK))
	{
		disk_ok = TRUE;
		g_record_vars->location.disk = FS_DEV_TYPE_USBDISK;
	}
#endif
	
	if(disk_ok)
	{
		GUI_ClearScreen(NULL);
		GUI_DisplayMessage(0, GUI_STR_RINIT, NULL, GUI_MSG_FLAG_DISPLAY);
		//GUI_ResShowMultiString(GUI_STR_RINIT, g_comval->langid, 16, 16);
		status = FS_GetEntryName(g_record_vars->location.subdir_entry, 20, pathname);
		
		if(status == ERR_SUCCESS && ERR_SUCCESS == FS_ChangeSubDir(g_record_vars->location.subdir_entry))
		{
			status = TRUE;
		}
		else
		{
			status = RECORD_ResetDir(&g_record_vars->location, VOICE_DIR);
		}
		
		if(status)
		{
			switch(param)
			{
			case MICREC_NOSTART:       //主菜单下按mode键,非FM录音.录音源取决于当前.rec_source的设置.
				g_rec_from = R_MIC;
				result = RECORD_Stop(FALSE);    //no start rec
				break;
				
			case MICREC_START:    //非FM进程下,按热键REC,进入MIC录音.
				g_rec_from = R_MIC;
				result = RECORD_Stop(TRUE);    //start rec
				break;
				
#if APP_SUPPORT_FM_RECORD
			case FMREC_START:     //在电台收听界面,按热键REC.进入FM录音.
			case FMREC_NOSTART:   //在电台收听界面,选择"电台录音"菜单,进入FM录音停止界面.
				g_rec_from = R_FM;
				result = FM_Entry(0xff); // start fm
				if(result != 0)
				{
					return result;
				}
				if (param == FMREC_START)
				{
					result = RECORD_Stop(TRUE);    //start rec
				}
				else
				{
					result = RECORD_Stop(FALSE);    //no start rec
				}
				break;
#endif
			default:
				break;
			}
		}
		else
		{
			g_free_space = FS_GetSpace(g_record_vars->location.disk , FS_GET_SPACE_FREE);
			if(g_free_space >= 2)
			{
				result = GUI_DisplayMessage(0, GUI_STR_MREADERR, NULL, GUI_MSG_FLAG_WAIT);
			}
			//result = GUI_DisplayErr();            // 显示创建目录失败
			else
			{
				result = GUI_DisplayMessage(0, GUI_STR_MREADERR, NULL, GUI_MSG_FLAG_WAIT);
			}
			//result = GUI_DisplayErr(GUI_STR_MWRIERR);
		}
	}
	else
	{
		result = GUI_DisplayMessage(0, GUI_STR_MREADERR, NULL, GUI_MSG_FLAG_WAIT);
		//result = GUI_DisplayErr(GUI_STR_MREADERR);
	}
	
	//NVRAMWrite(&g_record_vars, VM_AP_RECORD,sizeof(g_record_vars));
	NVRAMWriteData();
	
	//if(result == RESULT_MAIN || result == RESULT_NULL)
	{
#if APP_SUPPORT_FM_RECORD
		if(g_rec_from == R_FM)
		{
			FM_Entry(0xfe);
		}
		
		if(param == FMREC_START)
		{
			result = RESULT_RADIO;
		}
#endif
		SetPAVolume(0);
	}
	
	return  result;
	
}

void RECORD_VolumeCb( INT32 Vol )
{
	mediaSendCommand(MC_SETVOLUME, Vol);
	g_record_vars->volume = Vol;
	g_comval->Volume = Vol;
}


INT32 ChangePlayVolume( UINT32 key )
{
	extern slider_t         slider;
	INT32 result;
	
	if(AP_Support_LCD())
	{
		//show background
		GUI_ClearScreen(NULL);
		GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
		
		slider.min = 0;
		slider.max = MUSIC_MAX_VOLUME;
		slider.value = g_record_vars->volume;
		slider.step = 1;
		
		ui_auto_update = FALSE;
		result = GUI_Slider(& slider, RECORD_VolumeCb);
		ui_auto_update = TRUE;
		
		if (result != 0)
		{
			return result;
		}
		need_draw = TRUE;
	}
	else
	{
		if((key & 0xffff0000) == AP_KEY_VOLADD)
		{
			g_record_vars->volume++;
			if(g_record_vars->volume > MUSIC_MAX_VOLUME)
			{
				g_record_vars->volume = MUSIC_MAX_VOLUME;
			}
		}
		if((key & 0xffff0000) == AP_KEY_VOLSUB)
		{
			g_record_vars->volume--;
			if(g_record_vars->volume < 0)
			{
				g_record_vars->volume = 0;
			}
		}
		mediaSendCommand(MC_SETVOLUME, g_record_vars->volume);
		g_comval->Volume = g_record_vars->volume;
	}
	
	
	return 0;
}

INT32 rec_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		return 0;
	}
	
	switch(value)
	{
	case RECORD_SET_SOURCE:
		return g_rec_from != param;
		break;
	case RECORD_SET_SAMPRATE:
		return g_rec_from == R_FM && g_record_vars->rec_sample_rate != param;
		break;
	}
	return 0;
}

INT32 RECORD_HandleKey(UINT32 key)
{
	INT32 result = 0;
	UINT16 param;
	switch(key)
	{
	case AP_MSG_WAIT_TIMEOUT:
		if(g_rec_status == stop || g_rec_status == paused || g_rec_status == play_paused)
		{
			APP_Sleep();
		}
		break;
	case AP_KEY_MODE | AP_KEY_PRESS:
#if APP_SUPPORT_MENU==1
		if(AP_Support_MENU())
		{
			result = GUI_Display_Menu(GUI_MENU_RECORD, rec_menu_callback);
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
			return RESULT_MAIN;
		break;
		
	case RECORD_SET_SOURCE:
		return RESULT_RECORD_SWITCH;
		break;
	case RECORD_SET_SAMPRATE:
#if APP_SUPPORT_MENU==1
		if(g_rec_from == R_FM)
		{
			GUI_GetMenuResult(&param);
			g_record_vars->rec_sample_rate = param;
			g_rec_bitrate = g_record_vars->rec_sample_rate * 8 * 2 / 1000;
			need_parse = TRUE;
			need_draw = TRUE;
		}
#endif
		break;
	case RECORD_SELECT_FOLDER:
#if APP_SUPPORT_LCD==1
		result =  GUI_Directory(&g_record_vars->location, DIRECTORY_ALL, g_comval->langid);
		need_parse = TRUE;
		need_draw = TRUE;
		if(result != 0)
		{
			return result;
		}
		else
		{
			return RESULT_USER_STOP;
		}
#endif
		break;
		
	case AP_KEY_VOLSUB | AP_KEY_DOWN:
	case AP_KEY_VOLADD | AP_KEY_DOWN:
	case AP_KEY_VOLSUB | AP_KEY_HOLD:
	case AP_KEY_VOLADD | AP_KEY_HOLD:
		result = ChangePlayVolume(key);
		break;
		
#if APP_SUPPORT_USB
	case AP_MSG_USB_PULLOUT:
		if(g_record_vars->location.disk == FS_DEV_TYPE_USBDISK)
		{
			return RESULT_MAIN;
		}
		break;
#endif
		
	case AP_MSG_SD_OUT:
		if(g_record_vars->location.disk == FS_DEV_TYPE_TFLASH)
		{
			return RESULT_MAIN;
		}
		break;
		
		
	default:
		result = MESSAGE_HandleHotkey(key);
	}
	
	return result;
}

/*********************************************************************************
* Description : play  界面
*
* Arguments   :
*
* Returns     : 结果消息或0
		RESULT_USER_STOP	//用户按键结束
		RESULT_LOW_POWER_STOP	//低电压结束
*
* Notes       :
*
*********************************************************************************/
static INT32 RECORD_Play(void)
{
	INT32 result;
	UINT32 key;
	BOOL flash_state = TRUE; //闪烁的状态
	UINT8 low_power_count = 0;      //获得低电压的次数
	UINT8 select_file = TRUE;
	UINT8 ffflag = 0;
	Open_param_t param;
	
	app_trace(APP_RECORD_TRC, "play record file");
	//APP_ASSERT(ui_run_realtime == TRUE);
	
	if(g_file_num == 0)
	{
		return RESULT_NEW_FILE;
	}
	
	//这个界面打开hold显示
	ui_run_realtime = FALSE;
	need_draw = TRUE;
	
#if APP_SUPPORT_FM_RECORD
	if(g_rec_from == R_FM)
	{
		FM_Entry(0xfe);
	}
#endif
	
	RECORD_VolumeCb(g_record_vars->volume);
	while(1)
	{
		if(select_file)
		{
			extern Music_type_t g_currentMusicType;
			
			RECORD_FileName();
			
			g_currentMusicType = Music_WAV;
			
			param.file_name = g_record_vars->location.file_name;
			param.mode = MEDIA_PLAYREC;
			param.type = Music_WAV;
			
			mediaSendCommand(MC_STOP, 0);
			mediaSendCommand(MC_CLOSE, 0);
			//open codec
			result = mediaSendCommand(MC_OPEN, &param);
			
			if(!result) // open file fail, select next file
			{
				if(g_play_direct == 1 && g_record_vars->wav_num > g_max_file_num)
				{
					g_record_vars->wav_num = 1;
				}
				else if(g_play_direct == -1 && g_record_vars->wav_num < 1)
				{
					g_record_vars->wav_num = g_max_file_num;
				}
				else
				{
					g_record_vars->wav_num += g_play_direct;
				}
				continue;
			}
			
			if(AP_Support_LCD())
			{
				// get file infomation
				audInfoStruct *pAudioinfo = COS_Malloc(sizeof(audInfoStruct));
				if(mediaSendCommand(MC_GETFILEINFO, pAudioinfo))
				{
					g_rec_bitrate = pAudioinfo->bitRate / 1000;
					g_totalTime = pAudioinfo->time;
					
					g_record_vars->location.file_name[7] = 0;
					g_file_date = pAudioinfo->createDate;
					g_file_time = pAudioinfo->createTime;
				}
				COS_Free(pAudioinfo);
			}
			else
			{
				mediaSendCommand(MC_GETTOTALTIME, &g_totalTime);
			}
			
			mediaSendCommand(MC_PLAY, 0);
			g_rec_status = playing;
			
			select_file = FALSE;
			need_draw = TRUE;
		}
		if(need_draw)
		{
			RECORD_Display();
			need_draw = FALSE;
		}
		key = MESSAGE_Wait();
		switch( key )
		{
		case AP_MSG_STANDBY:
			//do nothing
			break;
			
		case AP_KEY_PLAY | AP_KEY_PRESS:
			mediaSendCommand(MC_STOP, 0);
			mediaSendCommand(MC_CLOSE, 0);
			return RESULT_USER_STOP;
			break;
			
		case AP_KEY_PREV| AP_KEY_PRESS:
			g_play_direct = -1;
			g_record_vars->wav_num --;
			select_file = TRUE;
			break;
		case AP_KEY_NEXT | AP_KEY_PRESS:
			g_play_direct = 1;
			g_record_vars->wav_num ++;
			select_file = TRUE;
			break;
			
			
		case AP_KEY_PLAY | AP_KEY_HOLD:
		case AP_KEY_REC | AP_KEY_PRESS:
			mediaSendCommand(MC_STOP, 0);
			mediaSendCommand(MC_CLOSE, 0);
			return RESULT_NEW_FILE;
			break;
			
		case AP_KEY_NEXT | AP_KEY_LONG: //快进
			if(ffflag == 0)
			{
				mediaSendCommand(MC_STOP, 0);
				g_rec_status = play_paused;
			}
			ffflag = 1;
			
			g_rec_time += MUSIC_FFPLAY_STEP * 1000;
			if(g_rec_time > g_totalTime)
			{
				g_rec_time = g_totalTime;
			}
			need_draw = 2;
			
			break;
		case AP_KEY_PREV | AP_KEY_LONG: //快退
			if(ffflag == 0)
			{
				mediaSendCommand(MC_STOP, 0);
				g_rec_status = play_paused;
			}
			ffflag = -1;
			
			if(g_rec_time  < MUSIC_FFPLAY_STEP * 1000)
			{
				g_rec_time = 0;
			}
			else
			{
				g_rec_time -= MUSIC_FFPLAY_STEP * 1000;
			}
			need_draw = 2;
			
			break;
			
		case AP_KEY_NEXT | AP_KEY_UP: //快进
		case AP_KEY_PREV | AP_KEY_UP: //快退
			if(ffflag != 0)
			{
				INT32 progress = g_rec_time * 10000 / g_totalTime;
				if(progress > 9990)
				{
					g_play_direct = 1;
					g_record_vars->wav_num ++;
					select_file = TRUE;
				}
				else
				{
					g_rec_status = playing;
					mediaSendCommand(MC_PLAY, progress);
				}
				ffflag = 0;
			}
			break;
		case AP_MSG_RTC:
			if(g_rec_status == playing)
			{
				media_status_t status_buf;
				mediaSendCommand(MC_GETTIME, &g_rec_time);
				need_draw = 2;
				
				mediaSendCommand(MC_GETSTATUS, (int) (&status_buf));
				if (status_buf.status == PLAYING_REACH_END)
				{
					g_play_direct = 1;
					g_record_vars->wav_num ++;
					select_file = TRUE;
				}
			}
			
			break;
			
		default:
			key = RECORD_HandleKey( key);
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				mediaSendCommand(MC_STOP, 0);
				mediaSendCommand(MC_CLOSE, 0);
				return key;
			}
		}//switch()
	}//while(1)
}


/*********************************************************************************
* Description : draw A, B, CD, EF, BATT, G,  并且刷新界面
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
static void RECORD_Display(void)
{
#if APP_SUPPORT_RGBLCD==1
	INT8 buf[8];
	
	if(AP_Support_LCD())
	{
		ui_auto_update = FALSE;
		
		if(need_draw == 1)
		{
			GUI_ClearScreen(NULL);
			
			GUI_DisplayIconA(GUI_IMG_MREC);
			GUI_ResShowImage(GUI_IMG_MUSICTYPE, 1, POSITION_B, 0);
			if(g_rec_status == playing || g_rec_status == play_paused)
			{
				GUI_DisplayIconC(GUI_IMG_PLAY);
			}
			else if(g_rec_from == R_FM)
			{
				GUI_DisplayIconC(GUI_IMG_RA_FM);
			}
			GUI_DisplayNumbers(POSITION_B + 3, 0, 3, GUI_IMG_BITRATENUMS, g_rec_bitrate);
			
			GUI_ResShowImage(GUI_IMG_DEVICE, g_record_vars->location.disk, 121, 0);
			BT_DisplayIcon(101, 0);
			
			//sprintf(buf, "%03d", g_record_vars->wav_num);
			//GUI_DisplayText(POSITION_D, 0, buf);
			sprintf(buf, "%03d", g_file_num);
			GUI_DisplayText(POSITION_D, 0, buf);
			
			GUI_DisplayBattaryLever();
			
			GUI_DisplayTimeText(36, 36, GUI_TIME_DISPLAY_AUTO, g_rec_time);
			GUI_DisplayText(LCD_WIDTH / 2 - 4, 36, "/");
			GUI_DisplayTimeText(LCD_WIDTH / 2 + 4, 36, GUI_TIME_DISPLAY_AUTO, g_totalTime);
			
			//file name & time
			GUI_DisplayText(24, 70, g_record_vars->location.file_name);
			GUI_DisplayText(72, 70, ".WAV");
			GUI_DisplayDateText(18, 100, TRUE, g_file_date);
			GUI_DisplayTimeText(106, 100, GUI_TIME_DISPLAY_AUTO, g_file_time * 1000);
		}
		
		// current time
		if(need_draw == 2)
		{
			if(g_rec_status == paused)
			{
				static BOOL time_count = FALSE;
				if(time_count)
					//	 GUI_DisplayTimeText(32, 16, 1, g_rec_time);
				{
					GUI_DisplayTimeText(20, 16, GUI_TIME_DISPLAY_AUTO, g_rec_time);
				}
				else
				{
					GUI_DisplayText(20, 16, "     ");
				}
				time_count = !time_count;
			}
			else
			{
				GUI_DisplayTimeText(20, 16, GUI_TIME_DISPLAY_AUTO, g_rec_time);
			}
		}
		
		
		GUI_UpdateScreen(NULL);
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LCD==1
	INT8 buf[8];
	
	if(AP_Support_LCD())
	{
		ui_auto_update = FALSE;
	
		if(need_draw == 1)
		{
			GUI_ClearScreen(NULL);
	
			GUI_DisplayIconA(GUI_IMG_MREC);
			GUI_ResShowImage(GUI_IMG_MUSICTYPE, 1, POSITION_B, 0);
			if(g_rec_status == playing || g_rec_status == play_paused)
			{
				GUI_DisplayIconC(GUI_IMG_PLAY);
			}
			else if(g_rec_from == R_FM)
			{
				GUI_DisplayIconC(GUI_IMG_RA_FM);
			}
			GUI_DisplayNumbers(POSITION_B + 3, 0, 3, GUI_IMG_BITRATENUMS, g_rec_bitrate);
	
			GUI_ResShowImage(GUI_IMG_DEVICE, g_record_vars->location.disk, POSITION_F, 0);
			BT_DisplayIcon(101, 0);
	
			//sprintf(buf, "%03d", g_record_vars->wav_num);
			//GUI_DisplayText(POSITION_D, 0, buf);
			sprintf(buf, "%03d", g_file_num);
			GUI_DisplayText(POSITION_D, 0, buf);
	
			GUI_DisplayBattaryLever();
	
			GUI_DisplayTimeText(20, 16, GUI_TIME_DISPLAY_AUTO, g_rec_time);
			GUI_DisplayText(LCD_WIDTH / 2 - 4, 16, "/");
			GUI_DisplayTimeText(LCD_WIDTH / 2 + 4, 16, GUI_TIME_DISPLAY_AUTO, g_totalTime);
	
			//file name & time
			GUI_DisplayText(24, 32, g_record_vars->location.file_name);
			GUI_DisplayText(72, 32, ".WAV");
			GUI_DisplayDateText(0, 48, TRUE, g_file_date);
			GUI_DisplayTimeText(88, 48, GUI_TIME_DISPLAY_AUTO, g_file_time * 1000);
		}
	
		// current time
		if(need_draw == 2)
		{
			if(g_rec_status == paused)
			{
				static BOOL time_count = FALSE;
				if(time_count)
					//	 GUI_DisplayTimeText(32, 16, 1, g_rec_time);
				{
					GUI_DisplayTimeText(20, 16, GUI_TIME_DISPLAY_AUTO, g_rec_time);
				}
				else
				{
					GUI_DisplayText(20, 16, "     ");
				}
				time_count = !time_count;
			}
			else
			{
				GUI_DisplayTimeText(20, 16, GUI_TIME_DISPLAY_AUTO, g_rec_time);
			}
		}
	
	
		GUI_UpdateScreen(NULL);
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LED8S==1
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(GUI_ICON_RECORD | GUI_ICON_COLON, 0, 0);
	if(g_record_vars->location.disk == FS_DEV_TYPE_USBDISK)
	{
		GUI_ResShowPic(GUI_ICON_USB, 0, 0);
	}
	else
	{
		GUI_ResShowPic(GUI_ICON_SD, 0, 0);
	}
	if(g_rec_status == recording || g_rec_status == playing)
	{
		GUI_ResShowPic(GUI_ICON_PLAY, 0, 0);
	}
	else
	{
		GUI_ResShowPic(GUI_ICON_PAUSE, 0, 0);
	}
	
	GUI_DisplayTimeLEDs(g_rec_time);
	
	GUI_UpdateScreen(NULL);
#endif
	
	if(g_rec_status == playing)
	{
		LED_SetPattern(GUI_LED_REC_PLAY, LED_LOOP_INFINITE);
	}
	else if(g_rec_status == recording)
	{
		if(g_rec_from == R_FM)
		{
			LED_SetPattern(GUI_LED_REC_FM, LED_LOOP_INFINITE);
		}
		else if(g_rec_from == R_MIC)
		{
			LED_SetPattern(GUI_LED_REC_MIC, LED_LOOP_INFINITE);
		}
	}
	else if(g_rec_status == stop)
	{
		LED_SetPattern(GUI_LED_REC_STOP, LED_LOOP_INFINITE);
	}
	else if(g_rec_status == paused)
	{
		LED_SetPattern(GUI_LED_REC_PAUSE, LED_LOOP_INFINITE);
	}
	
}


/*********************************************************************************
* Description : 根据当前录音文件的文件大小, 查表判断电压是否低
*
* Arguments   : batt, 电池电压
        全局变量
        g_file_size, 当前录音文件的文件大小,单位page
*
* Returns     : TRUE, 电压低
        FALSE, 电压正常
*
* Notes       :
*
*********************************************************************************/
static BOOL RECORD_CheckLowBatt(void)
{
	INT32 batt_level = 12;
	INT32 batt = GetBattery();
	
	if(batt < 0)
	{
		return FALSE;
	}
	
	// file < 32M
	if(g_file_size < 32L * 1024 * 2)
	{
		// if(g_comval->BatteryType == BATT_TYPE_ALKALINE ) batt_level = 4;
		// else if(g_comval->BatteryType == BATT_TYPE_NIH ) batt_level = 5;
		// else if(g_comval->BatteryType == BATT_TYPE_LITHIUM) batt_level = 11;
		// else APP_ASSERT(FALSE);
		batt_level --;
	}
	
	if(batt <= batt_level)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*********************************************************************************
* Description : 录音界面
*
* Arguments   :
*
* Returns     : 结果消息或0
        RESULT_USER_STOP    //用户按键结束
*
* Notes       :
*
*********************************************************************************/
static INT32 RECORD_DisplayRec(void)
{
	INT32 result = 0;  //result for return
	INT32 mc_result = 0;  //command result
	BOOL key_stop = FALSE;  //用户按键结束
	UINT32 key;
	media_status_t rec_stat;
	INT32 i = 0;
	INT32 page_count = 0;
	UINT8 low_power_count = 0;      //获得低电压的次数
	BOOL vor_waiting = FALSE;      //vor 没有写盘
	UINT8 vor_counter = 0;      //vor没有写盘的次数
	BOOL flash_state = FALSE;      //时间闪动
	
	APP_ASSERT(ui_run_realtime == TRUE);
	
	//init the counter
	g_rec_time = 0;
	
	mc_result = mediaSendCommand(MC_RECORD, 0);
	if(!mc_result)
	{
		//APP_DUMP("record error!", 0);
		GUI_DisplayMessage(0, GUI_STR_RECORDERROR, NULL, GUI_MSG_FLAG_WAIT);
		return 0;
	}
	
	g_rec_status = recording;
	
	while(1)
	{
		if(need_draw)
		{
			RECORD_Display();
			//RECORD_TimeDisplay();
			need_draw = FALSE;
		}
		
		key = MESSAGE_Wait();
		switch( key )
		{
		//overwrite auto standby
		//case AP_KEY_MODE | AP_KEY_LONG:
		case AP_MSG_STANDBY:
			//do nothing
			
			break;
			
		case AP_KEY_MODE|AP_KEY_LONG: // switch record source
			if(MESSAGE_IsHold())
			{
				return RESULT_RECORD_SWITCH;
			}
			break;
			
		case AP_KEY_PLAY | AP_KEY_PRESS:
		case AP_KEY_REC | AP_KEY_PRESS:
			if(g_rec_status == recording)
			{
				mediaSendCommand(MC_PAUSE, 0);
				g_rec_status = paused;
			}
			else if(g_rec_status == paused)
			{
				mediaSendCommand(MC_RECORD, 0);
				g_rec_status = recording;
			}
			need_draw = TRUE;
			break;
			
		case AP_KEY_PLAY | AP_KEY_HOLD:
			//record end and start new file
			if(MESSAGE_IsHold())
			{
				g_rec_status = wait_to_save;
				//key_stop = TRUE;
				result = RESULT_NEW_FILE;
			}
			break;
			
		case AP_KEY_NEXT | AP_KEY_DOWN:
		case AP_KEY_PREV | AP_KEY_DOWN:
			// stop record and play the last file
			g_rec_status = wait_to_save;
			result = RESULT_USER_STOP;
			break;
			
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
			if(g_rec_status == recording)
			{
				mediaSendCommand(MC_GETTIME, &g_rec_time);
				//RECORD_TimeDisplay();
			}
			need_draw = 2;
			
			if(RECORD_CheckLowBatt())
			{
				low_power_count++;
			}
			else
			{
				low_power_count = 0;
			}
			
			//检测低电压的次数
			if(low_power_count > LOW_POWER_COUNT)
			{
				GUI_DisplayMessage(0, GUI_STR_MPWROUT, NULL, GUI_MSG_FLAG_DISPLAY);
				//GUI_ClearScreen(&region_msg);
				//GUI_ResShowMultiString(GUI_STR_MPWROUT, g_comval->langid, 16, 16);
				g_rec_status = wait_to_save;
			}
			break;
			
		default:
			key = RECORD_HandleKey( key);
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				g_rec_status = wait_to_save;
				result = key;
			}
		}
		
		//check for user or hotkey stop recording
		if(g_rec_status == wait_to_save)
		{
			GUI_DisplayMessage(0, GUI_STR_RSAVING, NULL, GUI_MSG_FLAG_DISPLAY);
			//GUI_ClearScreen(&region_msg);
			//GUI_ResShowMultiString(GUI_STR_RSAVING,  g_comval->langid, 16, 16);
			mediaSendCommand(MC_STOP, 0);
			g_rec_status = saving;
		}
		
		mediaSendCommand(MC_GETSTATUS, (int)&rec_stat);
		//check status for codec stop
		if(g_rec_status == saving)
		{
			if(rec_stat.status == RECORDING_STOP)
			{
				GUI_DisplayMessage(0, GUI_STR_RECORDOK, NULL, GUI_MSG_FLAG_WAIT);
				//APP_DUMP("record OK!", 0);
				if(result != 0)
				{
					return result;    //热键退出
				}
				if(key_stop)
				{
					return RESULT_USER_STOP;
				}
				else
				{
					return 0;
				}
			}
			
			//avoid dead loop
			if(i++ >= 500)
			{
				//APP_DUMP("no stop flag!", 0);
				GUI_DisplayMessage(0, GUI_STR_RECORDERROR, NULL, GUI_MSG_FLAG_WAIT);
				return 0;
			}
		}
		
		//check disk full
		if(rec_stat.status == RECORDING_OUT_SPACE)
		{
			vor_waiting = FALSE;
			vor_counter = 0;
			g_file_size++;
			
			if(--g_free_space == 0)
			{
				g_rec_status = wait_to_save;
			}
		}
		else if(rec_stat.status == RECORDING_DISK_ERROR)    //致命错误
		{
			GUI_DisplayMessage(0, GUI_STR_MREADERR, NULL, GUI_MSG_FLAG_WAIT);
			//GUI_DisplayErr(GUI_STR_MREADERR);
			//APP_ASSERT(FALSE);
			//while(1) {;}    //dead loop
			//APP_DUMP("disk write error!", 0);
			return 0;
		}
		else
		{
			vor_waiting = TRUE;
		}
		
	}//while(1)
}



/*********************************************************************************
* Description : 分析剩余空间,计算可录音时间
*
* Arguments   :
*
* Returns     : 全局变量
        g_free_space, 剩余空间,单位page
*
* Notes       :
*
*********************************************************************************/
static void RECORD_RestTime(void)
{
	UINT32 total_sec = 0L;
	ap_time_t time;
	
	g_free_space = FS_GetSpace(g_record_vars->location.disk, FS_GET_SPACE_FREE);
	
	if(g_free_space > SPACE_LOW)
	{
		g_free_space -= SPACE_LOW;
	}
	else
	{
		g_free_space = 0;
		return;
	}
	hal_HstSendEvent(SYS_EVENT, g_free_space);
	
	if(g_rec_from == R_FM)
	{
		g_rec_bitrate = g_record_vars->rec_sample_rate * 8 * 2 / 1000;
	}
	else
	{
		g_rec_bitrate = 64;
	}
	
	g_totalTime = g_free_space / (g_rec_bitrate / 8 );
}


/*********************************************************************************
* Description : 计算当前目录下的总录音文件个数
*
* Arguments   :
*
* Returns     : 全局变量
        g_file_num, 总录音文件个数
*
* Notes       :
*
*********************************************************************************/
void RECORD_TotalNum(void)
{
	UINT32 inode;
	UINT32 file_number;
	UINT8 filename[30];
	
	g_file_num = 0;
	
	if(ERR_SUCCESS == FS_GetNextEntry(&inode, ExtVoice, FS_DIR_FILE))
	{
	
		do
		{
			file_number = 0;
			FS_GetShortName(inode, filename);
			sscanf(filename + 3, "%d", &file_number);
			if(file_number > 0)
			{
				g_file_num++;
				if(file_number > g_max_file_num)
				{
					g_max_file_num = file_number;
				}
			}
		}
		while( ERR_SUCCESS == FS_GetNextEntry(&inode, ExtVoice, FS_DIR_FILE_CONT));
	}
	
}


/*********************************************************************************
* Description : 格式化文件名
*
* Arguments   : 全局变量
        g_record_vars->rec_type
*
* Returns     : 全局变量
        g_record_vars->location.filename
*
* Notes       : RECxx.WAV or RECxx.ACT
*
*********************************************************************************/
static void RECORD_FileName(void)
{
	sprintf(g_record_vars->location.file_name, "REC%03d  WAV", g_record_vars->wav_num);
}


/*********************************************************************************
* Description : 生成新的录音文件名
*
* Arguments   : 全局变量
        g_record_vars->rec_type
*
* Returns     : TRUE, 成功
        FALSE, 编号01~99 文件都存在
        全局变量
        g_record_vars->location
        g_record_vars->wav_num
        g_file_name_time[], 文件名,文件时间字符串
*
* Notes       :
*
*********************************************************************************/
static BOOL RECORD_NewFileName(void)
{
	date_t date;
	ap_time_t time;
	
	INT32 num;
	
	g_record_vars->wav_num = 1;
	RECORD_FileName();
	
	num = GenNewName(&g_record_vars->location);
	
	if(num == 0)
	{
		return FALSE;
	}
	
	if(num > g_max_file_num)
	{
		g_max_file_num = num;
	}
	
	g_record_vars->wav_num = num;
	
	RECORD_FileName();
	
	//build g_file_name_time for display
	TM_GetDateAndTime(&date, &time);
	
	g_record_vars->location.file_name[6] = 0;
	g_file_date = ((date.year) << 9) + date.month * 32 + date.day;
	g_file_time = time.hour * 3600 + time.minute * 60 + time.second;
	return TRUE;
}


/*********************************************************************************
* Description : 进入录音
*
* Arguments   :
*
* Returns     : 结果消息或0
*
* Notes       :
*
*********************************************************************************/
static INT32 RECORD_Start(void)
{
	INT32 result = 0;
	Open_param_t param;
	
	g_file_size = 0L;
	
	//检查电压
	if(RECORD_CheckLowBatt())
	{
		return GUI_DisplayMessage(0, GUI_STR_MPWROUT, NULL, GUI_MSG_FLAG_WAIT);
		//  return GUI_DisplayErr(GUI_STR_MPWROUT);
	}
	app_trace(APP_RECORD_TRC, "start record ... ");
	
	//防止在录音过程中切换bank
	ui_run_realtime = TRUE;
	g_rec_status = prepareing;
	
	param.file_name = g_record_vars->location.file_name;
	param.mode = MEDIA_RECORD;
	param.type = Music_WAV;
	//open codec
	result = mediaSendCommand(MC_OPEN, &param);
	
	if(!result)
	{
		// MESSAGE_SetEvent(AP_EVENT_CODEC_ERR);
		ui_run_realtime = FALSE;
		return 0;
	}
	
	result = RECORD_DisplayRec();
	
	mediaSendCommand(MC_CLOSE, 0);
	
	// 文件个数加1
	g_record_vars->wav_num++;
	
	ui_run_realtime = FALSE;
	g_rec_status = stop;
	
	return result;
}


/*********************************************************************************
* Description : 停止界面
*
* Arguments   : auto_start, 是否自动开始录音
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
static INT32 RECORD_Stop(BOOL auto_start)
{
	INT32 result;
	UINT32 key;
	
	UINT8 rec_type;        //改变前保存旧的
	BOOL waiting_keyup = FALSE;  //录音后在等待用户按键抬起
	
	need_draw = TRUE;
	need_parse = TRUE;
	
	while(1)
	{
		g_rec_status = stop;
		
		//check if need count num & time
		if(need_parse)
		{
#if APP_SUPPORT_FM_RECORD
			if(g_rec_from == R_FM)
			{
				FM_Entry(0xff);
			}
#endif
			
			RECORD_TotalNum();
			
#if APP_SUPPORT_MENU==1
			if(g_file_num >= 200 && AP_Support_MENU())
			{
				GUI_DisplayMessage(0, GUI_STR_MDIRFULL, NULL, GUI_MSG_FLAG_WAIT);
				//GUI_DisplayErr(GUI_STR_MDIRFULL);
				
				result = GUI_Display_Menu(GUI_MENU_RECORD, rec_menu_callback);
				if(result == RESULT_IGNORE)
				{
					result = 0;
				}
				if(result != 0)
				{
					return result;
				}
				need_draw = TRUE;
				
				//重新分析文件个数
				need_parse = TRUE;
				continue;
			}
#endif
			
			RECORD_RestTime();
			
			//check disk out
			if(g_free_space == 0)
			{
				return GUI_DisplayMessage(0, GUI_STR_MWRIERR, NULL, GUI_MSG_FLAG_WAIT);
			}
			
			//check codec error, report dir out
			/* if(MESSAGE_GetEvent(AP_EVENT_CODEC_ERR, AP_EVENT_ANY) != 0)
			 {
			     MESSAGE_ClearEvent(AP_EVENT_CODEC_ERR);
			     return GUI_DisplayErr(GUI_STR_MDIRFULL);
			 }*/
			
			if(!RECORD_NewFileName())
			{
				return GUI_DisplayMessage(0, GUI_STR_MFILEOUT, NULL, GUI_MSG_FLAG_WAIT);
			}
			
			need_parse = FALSE;
			need_draw = TRUE;
		}
		
		if(need_draw)
		{
			RECORD_Display();
			need_draw = FALSE;
		}
		
		
		if(auto_start)
		{
			key = AP_KEY_REC | AP_KEY_PRESS;
			auto_start = FALSE;
		}
		else
		{
			key = MESSAGE_Wait();
		}
		
		switch( key )
		{
		case AP_KEY_MODE|AP_KEY_LONG: // switch record source
			if(MESSAGE_IsHold())
			{
				return RESULT_RECORD_SWITCH;
			}
			break;
		case AP_MSG_RTC:
			{
				date_t date;
				ap_time_t time;
				
				TM_GetDateAndTime(&date, &time);
				g_file_date = ((date.year) << 9) + date.month * 32 + date.day;
				g_file_time = time.hour * 3600 + time.minute * 60 + time.second;
				need_draw = TRUE;
			}
			break;
			
		case AP_KEY_PLAY | AP_KEY_PRESS:
		case AP_KEY_REC |AP_KEY_PRESS:
			result = RECORD_Start();
			
			if(result == RESULT_PLAY_RECORD)
			{
				result = RECORD_Play();
				need_parse = TRUE;
			}
			need_draw = TRUE;
			
			MESSAGE_Initial(NULL); //打开自动关机检测
			
			//check if user hold rec to stop it
			if(result == RESULT_USER_STOP)
			{
				waiting_keyup = TRUE;
				result = 0;
			}
			if(result == RESULT_NEW_FILE)
			{
				auto_start = TRUE;
				result = 0;
			}
			
			//check for hotkey
			if(result != 0)
			{
				return result;
			}
			
			//gen a new file name for nex time recording
			need_parse = TRUE;
			break;
			
		case AP_KEY_NEXT | AP_KEY_PRESS:
			g_play_direct = 1;
			g_record_vars->wav_num = 1;
		case AP_KEY_PREV | AP_KEY_PRESS:
			// play the last file
			if(key == (AP_KEY_PREV | AP_KEY_PRESS))
			{
				g_play_direct = -1;
			}
			result = RECORD_Play();
			
			MESSAGE_Initial(NULL); //打开自动关机检测
			need_draw = TRUE;
			
			//check if user hold rec to stop it
			if(result == RESULT_USER_STOP)
			{
				waiting_keyup = TRUE;
				result = 0;
			}
			if(result == RESULT_NEW_FILE)
			{
				auto_start = TRUE;
				result = 0;
			}
			
			//check for hotkey
			if(result != 0)
			{
				return result;
			}
			
			//gen a new file name for nex time recording
			need_parse = TRUE;
			
			break;
			
		default:
			key = RECORD_HandleKey( key);
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key == RESULT_USER_STOP)
			{
				result = 0;
			}
			else if(key != 0)
			{
				return key;
			}
		}//switch(key);
		
	}//while(1)
}



BOOL FMR_IsActive(void)
{
	return g_rec_status != stop && g_rec_from == R_FM;
}

UINT16 FMR_Get_SampleRate(void)
{
	return g_record_vars->rec_sample_rate;
}

#endif
