/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_music.h"
#include "ap_media.h"
#include "ap_gui.h"
#include "event.h"

char curr_file_name[128] = {0};
UINT8 g_play_disk = FS_DEV_TYPE_TFLASH;

#if APP_SUPPORT_MUSIC==1

//外部变量，在每次进入一个人模块的时候都将被重新初始化
extern slider_t              slider;
extern BOOL                  ui_run_realtime;
extern BOOL                  ui_auto_update;
extern UINT32                new_play_time;
#if XDL_APP_SUPPORT_TONE_TIP==1
extern UINT8 lowpower_flag_for_tone;
#endif
extern INT8 g_timer_setting_flag;

#define   MUSIC_2HZ_TIMER_LEN           500        //2hz定时器
#define   MUSIC_20MS_TIMER_LEN          20         //20ms定时器

#define MUSIC_FFPLAY_STEP           7  // second
//#define MUSIC_ERROR_SKIP_STEP          2// 0.5second
#define MUSIC_INTRO_TIME             10 // second

#define MUSIC_MAX_TAGINFO           120

#define MUSIC_START_REPEAT              AP_MSG_USER+1
#define MUSIC_SET_FOLLOW                AP_MSG_USER+2
#define MUSIC_SELECT_FILE               AP_MSG_USER+3
#define MUSIC_SET_EQ                    AP_MSG_USER+4
#define MUSIC_SET_LOOP                  AP_MSG_USER+5
#define MUSIC_SET_RPT_TIMES             AP_MSG_USER+6
#define MUSIC_SET_RPT_PEROID            AP_MSG_USER+7
#define MUSIC_OPEN_FMOUT                AP_MSG_USER+8
#define MUSIC_CLOSE_FMOUT               AP_MSG_USER+9
#define MUSIC_SETUP_FMOUT               AP_MSG_USER+10
#define MUSIC_OPEN_LYRIC                AP_MSG_USER+11
#define MUSIC_CLOSE_LYRIC               AP_MSG_USER+12
#define MUSIC_DELETE_FILE               AP_MSG_USER+13
music_vars_t *g_music_vars = NULL;//本AP的公共变量

UINT16   g_BitRate = 0;
UINT16   ERROR_EXIT = 0;
//INT8  subdir_namebackup[12];
UINT8    WaitForCaltimeCNT = 0; //在停止状态下先不计时,到无按键动作一定时间后再开始计时
//这样可以提高在停止状态时按next 跳歌的速度.
BOOL CaltimeFlag = 0;       //TRUE: 不用再算时间；false: 未算时间
static UINT16 ReplayStartPos = 0, ReplayEndPos = 0;
static UINT32 TotalTime = 0; //总时间
static INT8 FFPlay_CNT = 0;

//EQ音量调整表
static const INT8 EQVOLADJ_TAB[eqcount_def] = { 0, 0, 0, 0, 0, 0, 0, 0};

static UINT8 g_TagInfoBuf[MUSIC_MAX_TAGINFO] = {0};//用于放TAG信息的BUF
static BOOL g_FCexitflag = 0;

//这空间是用于打开时用来集中传送参数
static Open_param_t g_Openparam = {0};

BOOL need_draw = 0;

Replay_status_t PlayStatus = MusicStop; //普通播放模式

UINT16 UpDisplayFlag = 0; //相应的位表示要更新显示的内容。
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
BOOL   g_lyricflag;//有无歌词标志,true表示有
INT8 lyric_buff[AUDPLY_LYRIC_MAX_LINE_SIZE];
UINT32 lyric_next_time = 0;

#endif
Music_type_t g_currentMusicType = 0;//标志当前音乐的类型(mp3/wma/asf/wav)
static UINT16 g_musicamount = 0;//当前目录歌曲总数
static UINT16 g_musicsequence = 0;//当前歌曲序号

static UINT8 g_DiskStateFlag = 0;//b7 = 1:flash 盘已格式化
//b6 = 1:flash 盘无文件
//b3 = 1:卡已格式化
//b2 = 1: 卡无文件
//b5,b4,b1,b0 : 保留
//default = 0x00
//static INT8 g_PowerOffFlag; //默认为正常，g_PowerOffFlag=0x00，如果rtc掉电g_PowerOffFlag=0x55

//---如下两变量只用作临时变量,主要目的是节省指令及省代码空间,因为局部变量较费代码及指令时间
static INT32  g_result = 0;//表示16位变量
static BOOL isFileInfoGot = 0;// = FALSE;//TRUE

/***********************************************************************************************************************
    ==FLASH 与卡的 报错处理
设 A 表示有无格式化属性 (+A:已格；-A:未格)
   B 表示有无文件属性   (+B:有文件；-B:无文件)
   X 表示无意义(如在盘未格式化时谈有无文件就无意义)

   C 表示FLASH
   H 表示SD卡

下面用组合列出所有情况：
1) C[+A+B],H[+A+B] //两个盘都已格式化，都有文件                    //不报任何错，而直接恢复退出时的断点即可
2) C[+A+B],H[+A-B] //两个盘都已格式化，FLASH有文件，卡无文件       //直接跳到FLASH中即可
3) C[+A+B],H[-AX]  //FLASH已格式化，卡没格式化，FLASH有文件        //先报"插卡错误",再跳到FLASH中

4) C[+A-B],H[+A+B] //两个盘都已格式化，FLASH无文件，卡有文件       //直接跳到卡中
5) C[+A-B],H[+A-B] //两个盘都已格式化，两个盘都无文件              //先报"磁盘没文件",再报"插卡没文件",然后退出
6) C[+A-B],H[-AX]  //FLASH已格式化，卡没格式化，FLASH无文件        //先报"插卡错误",再报"磁盘没文件",然后退出

7) C[-AX],H[+A+B]  //FLASH没格式化，卡已格式化，卡中有文件         //先报"磁盘错误",然后跳到卡中
8) C[-AX],H[+A-B]  //FLASH没格式化，卡已格式化，卡中无文件         //先报"磁盘错误",再报"卡没文件",然后退出
9) C[-AX],H[-AX]   //两个盘都没格式化                              //先报"磁盘错误" ，再报“插卡错误”,然后退出
***********************************************************************************************************************/



/*********************************************************************************
* Description : --显示错误信息-
*
* Arguments   :
*
* Notes       :
*
*********************************************************************************/
void MUSIC_DisplayErrMsg(INT32 ErrMsg)
{
	GUI_DisplayMessage(0, ErrMsg, NULL, GUI_MSG_FLAG_WAIT);
	//    GUI_DisplayErr(ErrMsg);
	SetStopInterface();
	need_draw = TRUE;
}


/*********************************************************************************
* Description : 音量设置回调函数
*
* Arguments   :
*
* Notes       :
*
*********************************************************************************/
void MUSIC_VolumeCb( INT32 Vol )
{
	if(PlayStatus != MusicStop)
	{
		mediaSendCommand(MC_SETVOLUME, Vol);
	}
	g_music_vars->volume = Vol;
	g_comval->Volume = Vol;
}

INT32 Check_file_num(void)
{
	g_result = RESULT_UI_PLAY;
	
	do
	{
		if (UpdateFileSelector() == 0)//根据location设置文件选择器参数
		{
#if APP_SUPPORT_USB
			if(g_music_vars->location.disk == FS_DEV_TYPE_TFLASH)
			{
				g_music_vars->location.disk = FS_DEV_TYPE_USBDISK;
				if (MountDisk(g_music_vars->location.disk))
				{
					fselInit(FSEL_TYPE_MUSIC, FSEL_ALL_SEQUENCE, FSEL_TYPE_COMMONDIR, g_music_vars->location.disk);
					continue;
				}
			}
#endif
			MUSIC_DisplayErrMsg(GUI_STR_CFILEOUT);//显示卡无文件
			return 1;
		}
	}
	while(0);
	return 0;
}

extern TCHAR open_filename[100];
extern INT32  g_current_fd;
INT32 Delete_file(void)
{
	INT32 ret = 0;
	mediaSendCommand(MC_STOP, 0);
	//FS_GetFileName(g_current_fd, sizeof(curr_file_name), curr_file_name);
	if(mediaSendCommand(MC_CLOSE, 0))
	{
		hal_HstSendEvent(SYS_EVENT, 0x2012920c);
	}
	
	if(0 == (ret = FS_Delete(curr_file_name)))
	{
		hal_HstSendEvent(SYS_EVENT, 0x2012920a);
		return RESULT_RE_INIT;
	}
	else
	{
		//hal_DbgAssert("return value :%d",ret);
		//hal_DbgAssert("file name :%s",curr_file_name);
		
		hal_HstSendEvent(SYS_EVENT, 0x2012920b);
	}
	return 0;
}

INT32 music_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		if(value == GUI_MENU_MUSIC_EQ)
		{
			return g_music_vars->eq_type;
		}
		else if(value == GUI_MENU_MUSIC_LOOP)
		{
			return g_music_vars->repeat_mode;
		}
		return 0;
	}
	
#if APP_SUPPORT_LCD
	if(g_timer_setting_flag == 1)
	{
		if(value == MUSIC_SELECT_FILE)
		{
			return PlayStatus == MusicStop;
		}
		else
		{
			return FALSE;
		}
	}
#endif
	
	switch(value)
	{
	case MUSIC_START_REPEAT:
		return PlayStatus == NormalPlay;
	case MUSIC_SET_FOLLOW:
		return PlayStatus == LoopPlay;
	case MUSIC_SELECT_FILE:
		return TRUE;//PlayStatus == MusicStop;
#if APP_SUPPORT_FM_OUT
	case MUSIC_OPEN_FMOUT:
		return !g_music_vars->fm_out;
	case MUSIC_CLOSE_FMOUT:
		return g_music_vars->fm_out;
#endif
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
	case MUSIC_OPEN_LYRIC:
		return !g_music_vars->show_lyric;
	case MUSIC_CLOSE_LYRIC:
		return g_music_vars->show_lyric;
#endif
	default:
		break;
	}
	return 0;
}

/*********************************************************************************
* Description : 主函数
*
* Arguments   :进入MP3播放界面，播放时对歌词的处理等
        param:

*
* Returns     : 结果消息或0
*
* Notes       :
*
*********************************************************************************/
INT32 MUSIC_Entry(INT32 param)
{
	INT32  mainresult;
	ERROR_EXIT = 0;
init:
	MUSIC_GetPara();      //获取音乐播放参数
	
	ui_auto_update = TRUE;
	Set_TF_Flag(0);//warkey 2.1
	GUI_DisplayMessage(0, GUI_STR_WAITPASS, NULL, GUI_MSG_FLAG_DISPLAY);
	// GUI_ResShowMultiString(GUI_STR_WAITPASS, g_comval->langid, 16, 16);//显示请稍后
	hal_HstSendEvent(SYS_EVENT, 0x2013100a);
	if (!MountDisk(g_play_disk))
	{
#if APP_SUPPORT_USB
		if(g_play_disk == FS_DEV_TYPE_TFLASH)
		{
			g_play_disk = FS_DEV_TYPE_USBDISK;
		}
		else
		{
			g_play_disk = FS_DEV_TYPE_TFLASH;
		}
		if (!MountDisk(g_play_disk))
#endif
		{
			MUSIC_DisplayErrMsg(GUI_STR_MREADERR);//显示磁盘错
#if APP_SUPPORT_LCD
			if(g_timer_setting_flag == 1)
			{
				return RESULT_TIMER;
			}
#endif
			if(gpio_detect_linein() == 1)
			{
				return RESULT_LINE_IN;
			}
			else
			{
				return RESULT_RADIO;
			}
			return 0;
		}
	}
	hal_HstSendEvent(SYS_EVENT, 0x2013100b);
	g_DiskStateFlag |= SetDiskOKFlag_def;//标志 flash 已格式化
	
	//初始化变量
	g_DiskStateFlag    = 0;  //磁盘状态字，描述磁盘状态
	g_FCexitflag       = FALSE;
	UpDisplayFlag      = 0x00;
	g_currentMusicType = Music_NON;
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
	g_lyricflag        = FALSE; //有无歌词的标示
#endif
	g_musicsequence    = 0x00;
	g_musicamount      = 0x00;
	
	////////////////////////////////////////从上次意外退出的地方播放///////////////////////////////////////
	g_Openparam.file_entry  = g_music_vars->location.file_entry;//文件名地址
	//g_Openparam.BreakPTSave = &g_music_vars->BreakPTSave.breakPT;    //备份参数地址
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	
	TotalTime = 0;
	//g_music_vars->BreakTime = 0;
	
	isFileInfoGot = FALSE;
	CaltimeFlag             = FALSE;
	WaitForCaltimeCNT       = 0;
	
	//if ( !((g_music_vars->fselmod == FSEL_TYPE_COMMONDIR)||(g_music_vars->fselmod == FSEL_TYPE_PLAYLIST)))
	{
		//  g_music_vars->fselmod = FSEL_TYPE_COMMONDIR;
	}
	//////////////////////////////////////////初始化文件播放参数//////////////////////////////////////////
	//初始化文件选择器
	hal_HstSendEvent(SYS_EVENT, 0x2013100c);
	if (InitFileSelector() == 0)//初始化文件器失败，没有音频文件
	{
		return 0;
	}
#if XDL_APP_SUPPORT_TONE_TIP==1
	media_PlayInternalAudio(GUI_AUDIO_TFCARD_PLAY, 1, FALSE);
	while(GetToneStatus())
	{
		MESSAGE_Sleep_EXT(1);
	}
	hal_HstSendEvent(SYS_EVENT, 0x11114444);
	SetStopToPlayInterface();
#endif
	mainresult = MUSIC_Main();// =============> 进入界面
	
	if(mainresult == RESULT_RE_INIT)
	{
		goto init;
	}
#if APP_SUPPORT_FM_OUT
	FM_OutPut(0xfe, NULL, NULL);  // close fm
#endif
	
	//NVRAMWrite(& g_music_vars, VM_AP_MUSIC,sizeof(g_music_vars));
	NVRAMWriteData();
	
	//删除跟读时可能产生的临时文件,在正常退出时保证这个文件能被删除
	FS_Delete( FollowReadTempLongName );
	
	g_play_disk = FS_DEV_TYPE_TFLASH;
	
#if APP_SUPPORT_LCD
	if(g_timer_setting_flag == 1 && (mainresult == RESULT_MAIN || mainresult == RESULT_NULL))
	{
		mainresult = RESULT_TIMER;
	}
#endif
	
	return mainresult;
}

/*********************************************************************************
*             void MUSIC_ClearBreakTimePara(void)
*
* Description : 清除MP3播放断点保存信息
*
* Arguments   : void
*
* Returns     : void
*
* Notes       :
*
*********************************************************************************/
void MUSIC_ClearBreakTimePara(void)
{
	//g_music_vars->MusicOpenMod           = Nor_open;
	//g_music_vars->BreakPTSave.breakPT    = 0; //开始断点为0
	// g_music_vars->BreakTime = 0;
	//    g_music_vars->BreakTime.hour    = 0;
	//    g_music_vars->BreakTime.minute  = 0;
	//    g_music_vars->BreakTime.second  = 0;
	// g_music_vars->BreakPTSave.ApointSave = 0; //开始断点为0
	// g_music_vars->BreakPTSave.BpointSave = 0; //开始断点为0
	g_music_vars->BreakTime = 0;
	//g_music_vars->BreakTime = 0;
	StartIntro(TRUE)//因为时间清0了，所以浏览要从头开始
}

/*********************************************************************************
*             void MUSIC_GetPara(void)
*
* Description : 读系统变量和ap局部变量
*
* Arguments   : void
*
* Returns     : void
*
* Notes       :
*
*********************************************************************************/
void MUSIC_GetPara( void )
{
	//kval_t kval;
	
	//读出是否刚上电，如是，则要重调时间。
	//NVRAMRead(&kval, VM_KERNEL, sizeof(kval_t));
	
	//g_PowerOffFlag = kval.rtcresetflag;
	
	MESSAGE_Initial(g_comval);
	
	//NVRAMRead(& g_music_vars, VM_AP_MUSIC, sizeof(g_music_vars));
	g_music_vars = (music_vars_t*)NVRAMGetData(VM_AP_MUSIC, sizeof(music_vars_t));
	
	//check if valid
	if (g_music_vars->magic != MAGIC_MUSIC)
	{
		g_music_vars->magic       = MAGIC_MUSIC;
		g_music_vars->volume      = MUSIC_MAX_VOLUME / 2;
		g_music_vars->eq_type     = g_comval->music_cfg.eq_mode;
		g_music_vars->replaytimes = g_comval->music_cfg.repeat_count;
		g_music_vars->maxReplayTime = g_comval->music_cfg.repeat_time;
		//g_music_vars->replaygap   = 5;
		if(g_comval->music_cfg.loop_mode == 1)
		{
			g_music_vars->repeat_mode = FSEL_ALL_REPEATONE;
		}
		else
		{
			g_music_vars->repeat_mode = FSEL_ALL_REPEATALL;
		}
		//g_music_vars->play_rate   = 0;
		g_music_vars->location.disk = FS_DEV_TYPE_TFLASH;
		//g_music_vars->play_listmod  = mlistnor;
		//g_music_vars->fselmod       = FSEL_TYPE_COMMONDIR;
		/*g_music_vars->MusicOpenMod  = Nor_open;
		g_music_vars->Wow  =0;
		g_music_vars->Bass =0;
		g_music_vars->Srs  =0; */
		g_music_vars->Lyric_Size = 16;
		g_music_vars->show_lyric = 1;
#if APP_SUPPORT_FM_OUT
		g_music_vars->fm_out = 0;
		g_music_vars->freq  = g_comval->fm_value.fm_sendfreq * 100;
#endif
		g_music_vars->location.subdir_name[0] = 0;
		g_music_vars->location.subdir_entry = 0;
		g_music_vars->location.file_name[0] = 0;
		g_music_vars->location.file_entry = 0;
		
		MUSIC_ClearBreakTimePara();
	}
	mediaSendCommand(MC_SETEQ, g_music_vars->eq_type );
	
#if APP_SUPPORT_INDEPEND_VOLUMNE==0
	g_music_vars->volume = g_comval->Volume;
#endif
	
}

//============改变音量================
INT32 ChangeVolume( UINT32 key )
{
#if APP_SUPPORT_RGBLCD==1
	if(AP_Support_LCD() && PlayStatus <= NormalPlay) // dont use slider in follow mode
	{
		//show background
		GUI_ClearScreen(NULL);
		GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
		
		slider.min = 0;
		slider.max = MUSIC_MAX_VOLUME;
		slider.value = g_music_vars->volume;
		slider.step = 1;
		
		ui_auto_update = FALSE;
		g_result = GUI_Slider(& slider, MUSIC_VolumeCb);
		ui_auto_update = TRUE;
		
		StartIntro(FALSE);
		
		if (g_result != 0)
		{
			return g_result;
		}
		SetFullInterface();
		need_draw = TRUE;
	}
	else
	
#elif APP_SUPPORT_LCD==1
	if(AP_Support_LCD() && PlayStatus <= NormalPlay) // dont use slider in follow mode
	{
		//show background
		GUI_ClearScreen(NULL);
		GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
	
		slider.min = 0;
		slider.max = MUSIC_MAX_VOLUME;
		slider.value = g_music_vars->volume;
		slider.step = 1;
	
		ui_auto_update = FALSE;
		g_result = GUI_Slider(& slider, MUSIC_VolumeCb);
		ui_auto_update = TRUE;
	
		StartIntro(FALSE);
	
		if (g_result != 0)
		{
			return g_result;
		}
		SetFullInterface();
		need_draw = TRUE;
	}
	else
#endif
	{
		if(((key & 0xffff0000) == AP_KEY_VOLADD) || ((key & 0xffff0000) == AP_KEY_NEXT))
		{
			g_music_vars->volume++;
			if(g_music_vars->volume > MUSIC_MAX_VOLUME)
			{
				g_music_vars->volume = MUSIC_MAX_VOLUME;
			}
		}
		if(((key & 0xffff0000) == AP_KEY_VOLSUB) || ((key & 0xffff0000) == AP_KEY_PREV))
		{
			g_music_vars->volume--;
			if(g_music_vars->volume < 0)
			{
				g_music_vars->volume = 0;
			}
		}
		if(PlayStatus != MusicStop)
		{
			mediaSendCommand(MC_SETVOLUME, g_music_vars->volume);
		}
		g_comval->Volume = g_music_vars->volume;
#if XDL_APP_SUPPORT_TONE_TIP==1
		hal_HstSendEvent(SYS_EVENT, 0x11551133);
		hal_HstSendEvent(SYS_EVENT, g_music_vars->volume);
		if(g_music_vars->volume >= MUSIC_MAX_VOLUME || g_music_vars->volume <= 0)
		{
			lowpower_flag_for_tone = 0;
			MESSAGE_SetEvent(0xaaaa1111);
		}
#endif
	}
	return 0;
}

/*********************************************************************************
*             Music_type_t CheckMusicType(INT8 *filename)
*
* Description : 检测当前是哪种类型的MUSIC文件
*
* Arguments   : INT8 *filename  文件名
*
* Returns     : Music_type_t 文件类型
*
* Notes       :
*
*********************************************************************************/
Music_type_t MUSIC_CheckType( TCHAR * filename )
{
	INT32 i;
	
	const TCHAR *const ExtMusicstr [] =
	{
		"MP3", "WMA", "ASF", "WAV", "AAC", "SBC"
#if 0
		, "VM", "AU", "SND", "PCM", "AMR", "AWB", "AIF", "AIFF", "AIFC"
#endif
	};
	/*小写转换为大写*/
	for (i = 8; i < 11; i++)
	{
		if (filename[i] >= 97 /* 'a' */  && filename[i] <= 122 /* 'z' */ )
		{
			filename[i] -= 32 /* ('a'-'A') */ ;
		}
	}
	
	if (strcmp(filename + 8, ExtMusicstr[0]) == 0)
	{
		return Music_MP3;
	}
	
	else if (strcmp(filename + 8, ExtMusicstr[3]) == 0)
	{
		return Music_WAV;
	}
	else if (strcmp(filename + 8, ExtMusicstr[5]) == 0)
	{
		return Music_SBC;
	}
#ifndef TARGET_AAC_SUPPORT_OFF
	else if (strcmp(filename + 8, ExtMusicstr[4]) == 0)
	{
		return Music_AAC;
	}
#endif
#ifndef TARGET_WMA_SUPPORT_OFF
	else if (strcmp(filename + 8, ExtMusicstr[1]) == 0)
	{
		return Music_WMA;
	}
#endif
#if 0
	
	else if (strcmp(filename + 8, ExtMusicstr[4]) == 0)
	{
		return Music_OGG;
	}
	else if (strcmp(filename + 8, ExtMusicstr[2]) == 0)
	{
		return Music_ASF;
	}
	else if (strcmp(filename + 8, ExtMusicstr[5]) == 0)
	{
		return Music_VM;
	}
	else if ((strcmp(filename + 8, ExtMusicstr[6]) == 0) || (strcmp(filename + 8,    ExtMusicstr[7]) ==   0))
	{
		return Music_AU;
	}
	else if (strcmp(filename + 8, ExtMusicstr[8]) == 0)
	{
		return Music_PCM;
	}
	else if( (strcmp(filename + 8, ExtMusicstr[9]) == 0) || (strcmp(filename + 8, ExtMusicstr[10]) == 0))
	{
		return Music_AMR;
	}
	else if ((strcmp(filename + 8, ExtMusicstr[11]) == 0) || (strcmp(filename + 8, ExtMusicstr[12]) == 0) || (strcmp(filename + 8, ExtMusicstr[13]) == 0))
	{
		return Music_AIFF;
	}
#endif
	else
	{
		return Music_NON;
	}
}



/*********************************************************************************
*             INT32 MUSIC_GetInformation(void)
*
* Description : 读取歌曲信息,歌词
*
* Arguments   : void
*
* Returns     : INT32
*
* Notes       :
*
*********************************************************************************/
INT32 MUSIC_GetInformation( void )
{
	INT32          result;
	audInfoStruct  *pAudioinfo;
	ap_time_t      total_time;
	
	hal_HstSendEvent(SYS_EVENT, 0x09220010);
	g_currentMusicType = MUSIC_CheckType(g_music_vars->location.file_name);
	
	g_Openparam.type      = g_currentMusicType;
	g_Openparam.mode      = MEDIA_PLAY;
	g_Openparam.file_entry = g_music_vars->location.file_entry;
	
	result = mediaSendCommand(MC_OPEN, (INT32)&g_Openparam);
	if(!result)
	{
		return FALSE;
	}
	
	FFPlay_CNT = 0;
	if (!isFileInfoGot)
	{
#if APP_SUPPORT_RGBLCD==1
		//memset(&g_curaudioinfo, 0, sizeof(g_curaudioinfo));/* get id3 tags info */
		if(AP_Support_LCD())
		{
			pAudioinfo = COS_Malloc(sizeof(audInfoStruct));
			//g_TagInfoBuf[0] = '\0';
			memset(g_TagInfoBuf, 0, MUSIC_MAX_TAGINFO);
			//if(Audio_GetDetailsInfo(g_music_vars->location.file_entry,&g_curaudioinfo,g_currentMusicType,g_comval->langid))
			if(mediaSendCommand(MC_GETFILEINFO, pAudioinfo))
			{
				INT32 sec;
				if(pAudioinfo->title[0] != 0)
				{
					strncpy(g_TagInfoBuf, pAudioinfo->title, 30);
					//hal_DbgAssert("g_TagInfoBuf:%s",pAudioinfo->title);
					strcat(g_TagInfoBuf, " ");
					strncat(g_TagInfoBuf, pAudioinfo->artist, 30);
					strcat(g_TagInfoBuf, " ");
					strncat(g_TagInfoBuf, pAudioinfo->album, 30);
					strcat(g_TagInfoBuf, " ");
					strncat(g_TagInfoBuf, pAudioinfo->author, 30);
					strcat(g_TagInfoBuf, "  ");
				}
				//  	    memcpy(g_music_vars->location.artist,pAudioinfo->artist,MAX_FILE_LEN);
				
				g_BitRate = pAudioinfo->bitRate / 1000;
				TotalTime =  pAudioinfo->time;
			}
			
			COS_Free(pAudioinfo);
			//---处理没有TAG信息的情况
			if (g_TagInfoBuf[0] == 0)
			{
				FS_GetEntryName(g_music_vars->location.file_entry, MUSIC_MAX_TAGINFO, g_TagInfoBuf);
				gui_unicode2local(g_TagInfoBuf, MUSIC_MAX_TAGINFO - 2);
				app_trace(APP_MUSIC_TRC, "FileName:%s", g_TagInfoBuf);
			}
			
			//---在TAG信息后加个空格避免滚屏时串在一起
			strcat(g_TagInfoBuf, " ");
			hal_HstSendEvent(SYS_EVENT, 0x09220025);
			
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
			if(g_music_vars->show_lyric)
			{
				g_lyricflag = lyricLoad(g_music_vars->location.file_entry);
				if(g_lyricflag)
				{
					lyric_next_time = 0;
					memset(lyric_buff, 0, AUDPLY_LYRIC_MAX_LINE_SIZE);
				}
			}
#endif //_LRC_SUPPORT_
		}
		else
#elif APP_SUPPORT_LCD==1
		//memset(&g_curaudioinfo, 0, sizeof(g_curaudioinfo));/* get id3 tags info */
		if(AP_Support_LCD())
		{
			pAudioinfo = COS_Malloc(sizeof(audInfoStruct));
			//g_TagInfoBuf[0] = '\0';
			memset(g_TagInfoBuf, 0, MUSIC_MAX_TAGINFO);
			//if(Audio_GetDetailsInfo(g_music_vars->location.file_entry,&g_curaudioinfo,g_currentMusicType,g_comval->langid))
			if(mediaSendCommand(MC_GETFILEINFO, pAudioinfo))
			{
				INT32 sec;
				if(pAudioinfo->title[0] != 0)
				{
					strncpy(g_TagInfoBuf, pAudioinfo->title, 30);
					//hal_DbgAssert("g_TagInfoBuf:%s",pAudioinfo->title);
					strcat(g_TagInfoBuf, " ");
					strncat(g_TagInfoBuf, pAudioinfo->artist, 30);
					strcat(g_TagInfoBuf, " ");
					strncat(g_TagInfoBuf, pAudioinfo->album, 30);
					strcat(g_TagInfoBuf, " ");
					strncat(g_TagInfoBuf, pAudioinfo->author, 30);
					strcat(g_TagInfoBuf, "  ");
				}
				//  	    memcpy(g_music_vars->location.artist,pAudioinfo->artist,MAX_FILE_LEN);
		
				g_BitRate = pAudioinfo->bitRate / 1000;
				TotalTime =  pAudioinfo->time;
			}
		
			COS_Free(pAudioinfo);
			//---处理没有TAG信息的情况
			if (g_TagInfoBuf[0] == 0)
			{
				FS_GetEntryName(g_music_vars->location.file_entry, MUSIC_MAX_TAGINFO, g_TagInfoBuf);
				gui_unicode2local(g_TagInfoBuf, MUSIC_MAX_TAGINFO - 2);
				app_trace(APP_MUSIC_TRC, "FileName:%s", g_TagInfoBuf);
			}
		
			//---在TAG信息后加个空格避免滚屏时串在一起
			strcat(g_TagInfoBuf, " ");
			hal_HstSendEvent(SYS_EVENT, 0x09220025);
		
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
			if(g_music_vars->show_lyric)
			{
				g_lyricflag = lyricLoad(g_music_vars->location.file_entry);
				if(g_lyricflag)
				{
					lyric_next_time = 0;
					memset(lyric_buff, 0, AUDPLY_LYRIC_MAX_LINE_SIZE);
				}
			}
#endif //_LRC_SUPPORT_
		}
		else
#endif //  APP_SUPPORT_LCD==1
			mediaSendCommand(MC_GETTOTALTIME, &TotalTime);
		isFileInfoGot = TRUE;
	}
	//	AudioPlayTimeTransfer(g_curaudioinfo.time,&g_media_status.total_time);
	
	//NVRAMWrite(&g_music_vars, VM_AP_MUSIC,sizeof(g_music_vars));
	NVRAMWriteData();
	hal_HstSendEvent(SYS_EVENT, 0x09220030);
	
	return 1;
}


/******************************************************************************************
input: void

output:   g_result == NULL: 退出到STOP
               g_result == 其他，按热键处理

******************************************************************************************/
INT32 HandlePlugandPull(UINT32 key)
{
	UINT8 other_disk;
	
	hal_HstSendEvent(SYS_EVENT, 0x25000000 + key);
	
	if(key == AP_MSG_USB_PULLOUT && g_music_vars->location.disk != FS_DEV_TYPE_USBDISK)
	{
		return 0;
	}
	if(key == AP_MSG_SD_OUT && g_music_vars->location.disk != FS_DEV_TYPE_TFLASH)
	{
		return 0;
	}
	
	if( (key == AP_MSG_SD_OUT && g_music_vars->location.disk == FS_DEV_TYPE_TFLASH)
	    || (key == AP_MSG_USB_PULLOUT && g_music_vars->location.disk == FS_DEV_TYPE_USBDISK) )
	{
		mediaSendCommand(MC_SETVOLUME, 0);
		mediaSendCommand(MC_STOP, no_fade);
		mediaSendCommand(MC_CLOSE, 0);
		if(gpio_detect_linein() == 1)
		{
			return RESULT_LINE_IN;
		}
		else
		{
			return RESULT_RADIO;
		}
		return RESULT_MAIN;
	}
	
	
	if(key == AP_MSG_SD_IN || key == AP_MSG_USB_PULLOUT)
	{
		g_music_vars->location.disk = FS_DEV_TYPE_TFLASH;
		other_disk = FS_DEV_TYPE_USBDISK;
	}
	else
	{
		g_music_vars->location.disk = FS_DEV_TYPE_USBDISK;
		other_disk = FS_DEV_TYPE_TFLASH;
	}
	if(!MountDisk(g_music_vars->location.disk))
	{
		MUSIC_DisplayErrMsg(GUI_STR_MREADERR);//显示磁盘错
		
		g_music_vars->location.disk = other_disk;
		if(!MountDisk(g_music_vars->location.disk))
		{
#if APP_SUPPORT_FLASHDISK==1
			g_music_vars->location.disk = FS_DEV_TYPE_FLASH;
			if(!MountDisk(g_music_vars->location.disk))
#endif
				return RESULT_MAIN;
		}
	}
	
	fselInit(FSEL_TYPE_MUSIC, FSEL_ALL_SEQUENCE, FSEL_TYPE_COMMONDIR, g_music_vars->location.disk);
	
	return RESULT_UI_PLAY;
}


/*****************************************************************************************
功能：检测SYS时钟是否被回调
接口：BOOL DisplayTimeRollback(void)
输入：void
输出：true: 正常播放
      FALSE: 显示完时钟已被回调，要进行设置退出
*****************************************************************************************/
BOOL DisplayTimeRollback(void)
{
#if 0
	wma_inf_t licensinfbuf;
	
	//取lincens 信息,判断是否DRM歌曲
	if (!mediaSendCommand(MC_GETINF,  (INT32)&licensinfbuf))
	{
		return TRUE;//不用显示重设信息
	}
	
	if (licensinfbuf.drmmode == INF_NORMALWMA)
	{
		//普通WMA也不用显示重设信息
		return TRUE;
	}
	
	GUI_ClearScreen(NULL);//清屏
	ui_auto_update = TRUE;
	GUI_DisplayMessage(0, GUI_STR_REQSETDT, NULL, GUI_MSG_FLAG_WAIT);
	//GUI_ResShowMultiString(GUI_STR_REQSETDT, g_comval->langid, 12, 16);//显示磁盘无文件
	//while( MESSAGE_Get() != 0 );//清空消息池,确保DELAY时间准确
	//MESSAGE_Sleep(4);
#endif
	return FALSE;
}

//===========[放到文件尾时的处理]===============
INT32 DealMusicFileEnd( INT8 DisMod)
{
	need_draw = TRUE;
	
	g_result = ToNextMusic(Play_play);
	if(DisMod == 0)
	{
		SetFullInterface();
	}
	if(g_result == RESULT_UI_STOP)
	{
		return g_result;
	}
	return 0;
}


/********************************************************************************
					MUSIC_SelectAudio
* Description : 换曲时要做的选曲，初始化动作
* Arguments  :
* Returns     :真时表示可继续放，假时表示要停止放(播放状态)
* Notes       :
********************************************************************************/
BOOL MUSIC_SelectAudio( UINT8 Direct, Music_sel_mod_t mod )
{
	BOOL result;
	
	//选文件
	//PlayStatus = NormalPlay;
	MUSIC_ClearBreakTimePara();
	//    g_TagInfoBuf[0] = 0x0;
	
	if (mod == Play_key)
	{
		//play 按键的情况
		switch (g_music_vars->repeat_mode)
		{
		case FSEL_ALL_REPEATONE:
			//case FSEL_ALL_SEQUENCE:
			//case FSEL_DIR_RANDOM:
			fselSetMode( FSEL_ALL_REPEATALL );/*PLAY 时按键*/
			break;
			
		default:
			fselSetMode(g_music_vars->repeat_mode);
			break;
		}
	}
	else if (mod == Stop_key)
	{
		fselSetMode(FSEL_ALL_REPEATALL);//STOP时按键
	}
	else
	{
		fselSetMode(g_music_vars->repeat_mode);//正常播放的情况
	}
	
	switch (Direct)
	{
	case Music_Reset_Head:
	case Music_Reset_End:
		break;
		
	case Music_Cur:
		result = TRUE;
		break;
		
	case Music_Next:
		result = fselGetNextFile(&g_music_vars->location.file_entry);
		break;
		
	case Music_Prev:
		result = fselGetPrevFile(&g_music_vars->location.file_entry);
		break;
		
	default:
		return FALSE;
	}
	
	if (!result)
	{
		fselSetLocation(& g_music_vars->location);
		return result;
	}
	//取当前歌曲数状态
	g_musicamount   = fselGetTotal();
	g_musicsequence = fselGetNo();
	
	fselSetMode(g_music_vars->repeat_mode);
	//确保选择器与AP的目录对应
	fselGetLocation(& g_music_vars->location);
	//检测Music的类型
	g_currentMusicType = MUSIC_CheckType(g_music_vars->location.file_name);
	
#if APP_SUPPORT_LCD==1
	FS_GetEntryName(g_music_vars->location.file_entry, MUSIC_MAX_TAGINFO, g_TagInfoBuf);
	gui_unicode2local(g_TagInfoBuf, MUSIC_MAX_TAGINFO - 2);
	app_trace(APP_MUSIC_TRC, "FileName:%s", g_TagInfoBuf);
	
	strcat(g_TagInfoBuf, " ");
#endif
	isFileInfoGot = FALSE;
	
	return TRUE;
}


/******************************************************************************
** 名字:  文件选择器初始化及报错显示
** 接口:  INT32 InitFileSelector(void)
** 描述:  把文件选择器初始化到有效的盘上，如找不到，则报错退出
** 输入参数: void
** 输出参数: 0: 没有有效的盘或盘上无文件，初始化失败，要求退出AP
             1: 已完成初始化文件选择器动作
** 使用说明:
********************************************************************************/
INT32 InitFileSelector(void)
{
	FS_DEV_TYPE DevType;
	UINT8       FsType;
	
	FS_GetRootType(&DevType, &FsType);
	
	if((DevType != FS_DEV_TYPE_INVALID) && (FsType == FS_TYPE_FAT))
	{
		g_music_vars->location.disk = DevType;
		
		if (fselInit(FSEL_TYPE_MUSIC, FSEL_ALL_SEQUENCE, FSEL_TYPE_COMMONDIR, g_music_vars->location.disk))
		{
			return 1;
		}
		
		if (fselGetErrID() == 0)
		{
			return 0;//非法，
		}
		
		/*显示卡文件状况*/
		if(g_musicamount == 0)
		{
			MUSIC_DisplayErrMsg(GUI_STR_CFILEOUT);//显示卡无文件
			g_DiskStateFlag |= SetDiskNoFile_def;//标志磁盘无文件
		}
	}
	
	return 0;//要退出 ap
}

/******************************************************************************
** 名字:
** 接口: INT32 UpdateFileSelector(void)
** 描述: 更新文件选择器
** 输入参数: void
** 输出参数:
			0: 更新不成功，要退出AP
			1: 更新成功，已可用
** 使用说明:
********************************************************************************/
INT32 UpdateFileSelector(void)
{
	/*1.在非正常播放下退出的情况，从上次播放地点继续播放保存变量的位置信息找到播放文件*/
	if (fselSetLocation(& g_music_vars->location))//根据location设置文件选择器参数
	{
		if (g_music_vars->location.file_entry != 0)//已经有播放的文件了，并且这个文件就在当前目录
		{
			fselGetLocation(& g_music_vars->location);
			return 1;
		}
	}
	
	MUSIC_ClearBreakTimePara();//时间断点参数从0开始
	
	/*2.找不到上次播放的痕迹,找到目录的第一个文件*/
	hal_HstSendEvent(SYS_EVENT, 0x09130002);
	
	if (fselGetNextFile(&g_music_vars->location.file_entry))
	{
		fselGetLocation(& g_music_vars->location);
		return 1;
	}
	return 0;
}



/*********************************************************************************
*           void RefreshMainScreen( void )
*
* Description : 主显示的刷新函数,只有有需要时,才调用此函数
*
* Arguments   : void
*
* Returns     : void
*
* Notes       :
*
*********************************************************************************/
void RefreshMainScreen( void )
{
	UINT32 led_mode;
#if APP_SUPPORT_RGBLCD==1
	INT8 temp_buffer[6];
	const region_t  reg_Scroll = {0, 80, LCD_WIDTH, 16};
	if(AP_Support_LCD())
	{
		ui_auto_update = FALSE;
		if (UpDisplayFlag & (up_cardflag))
		{
			GUI_DisplayIconA(GUI_IMG_MMUSIC);
			GUI_ResShowImage(GUI_IMG_DEVICE, g_music_vars->location.disk, 121, 0);
			BT_DisplayIcon(POSITION_C, 0);
		}
#if APP_SUPPORT_FM_OUT
		if (UpDisplayFlag & (up_musicfmout))
		{
			/*if (g_music_vars->fm_out) // FM output
			    GUI_ResShowPic(GUI_IMG_RA_FM, POSITION_C, 0);
			else
			{
			    const region_t region={POSITION_C, 0, 12, 16};
			    GUI_ClearScreen(&region);
			}*/
		}
#endif
		if(PlayStatus == MusicStop)
		{
			GUI_ClearScreen(&reg_Scroll);
			GUI_DisplayText(0, 80, g_TagInfoBuf);
			UpDisplayFlag |= up_musicinformation;
		}
		else if (UpDisplayFlag & up_musicinformation)/*Display file infomation */
		{
			GUI_SetScrollRegin(&reg_Scroll, 2);
			GUI_Scroll(g_TagInfoBuf, TRUE);
		}
		
		if (UpDisplayFlag & up_musicbitrate)
		{
			//先显示音乐类型图标
			if(g_currentMusicType != Music_NON)
			{
				GUI_ResShowImage(GUI_IMG_MUSICTYPE, g_currentMusicType > Music_WMA ? 0 : g_currentMusicType - 1, POSITION_B, 0);
			}
			if(g_BitRate > 0)
			{
				GUI_DisplayNumbers(POSITION_B + 3, 0, 3, GUI_IMG_BITRATENUMS, g_BitRate);
			}
			
		}
		
		if (UpDisplayFlag & up_musicsequence) /*Display Music Number Of Total Music */
		{
			switch (PlayStatus)
			{
			case MusicStop:
			case NormalPlay:
				GUI_DisplayNumberD(g_musicsequence, 0);
				GUI_DisplayNumberD(g_musicamount, 1);
				break;
			default:
				break;
			} //switch(PlayStatus)
		}
		
		
		if (UpDisplayFlag & up_musictotaltime)
		{
			GUI_DisplayText(LCD_WIDTH / 2 - 4, 36, "/");
			GUI_DisplayTimeText(LCD_WIDTH / 2 + 4, 36, GUI_TIME_DISPLAY_AUTO, TotalTime);
		}
		
		if (UpDisplayFlag & up_musiccurrenttime)
		{
			GUI_DisplayTimeText(36, 36, GUI_TIME_DISPLAY_AUTO, g_music_vars->BreakTime);
		}
		
		if (UpDisplayFlag & up_musicloopmode)/*Display Loop Mode*/
		{
			/* if (PlayStatus == MusicStop)
			 {
			     if (g_music_vars->fselmod == FSEL_TYPE_COMMONDIR)
			     {
			         g_music_vars->play_listmod = mlistnor;//改为普通目录模式
			     }
			    GUI_ResShowImage(GUI_IMG_MENUMUSIC, 2, POSITION_C, 0);
			    // GUI_DisplayIconC(PALYLIST_PIC[g_music_vars->play_listmod]);
			 }
			 else*/
			{
				GUI_ResShowImage(GUI_IMG_MUSICLOOPS, g_music_vars->repeat_mode, POSITION_F, 0);
			}
		}
		
		
		if (UpDisplayFlag & up_musiceqmode)
		{
			GUI_ResShowImage(GUI_IMG_MUSICEQS, g_music_vars->eq_type, POSITION_E + 2, 0);
		}
		
		if (UpDisplayFlag & up_followmode) /*Display Music Number Of Total Music */
		{
			if(PlayStatus > NormalPlay)
			{
				GUI_ResShowImage(GUI_IMG_MUSICRPT, PlayStatus - LoopPlay, POSITION_D, 0);
			}
		}
		
		if (UpDisplayFlag & up_musicbattery)
		{
			GUI_DisplayBattaryLever();
		}
		
		GUI_UpdateScreen(NULL);
		
		UpDisplayFlag  = 0x00;
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LCD==1
	INT8 temp_buffer[6];
	const region_t  reg_Scroll = {0, 32, LCD_WIDTH, 16};
	
	if(AP_Support_LCD())
	{
		ui_auto_update = FALSE;
		if (UpDisplayFlag & (up_cardflag))
		{
			GUI_DisplayIconA(GUI_IMG_MMUSIC);
			GUI_ResShowImage(GUI_IMG_DEVICE, g_music_vars->location.disk, 97, 0);
			BT_DisplayIcon(POSITION_C, 0);
		}
#if APP_SUPPORT_FM_OUT
		if (UpDisplayFlag & (up_musicfmout))
		{
			/*if (g_music_vars->fm_out) // FM output
			    GUI_ResShowPic(GUI_IMG_RA_FM, POSITION_C, 0);
			else
			{
			    const region_t region={POSITION_C, 0, 12, 16};
			    GUI_ClearScreen(&region);
			}*/
		}
#endif
		if(PlayStatus == MusicStop)
		{
			GUI_ClearScreen(&reg_Scroll);
			GUI_DisplayText(0, 32, g_TagInfoBuf);
			UpDisplayFlag |= up_musicinformation;
		}
		else if (UpDisplayFlag & up_musicinformation)/*Display file infomation */
		{
			GUI_SetScrollRegin(&reg_Scroll, 2);
			GUI_Scroll(g_TagInfoBuf, TRUE);
		}
	
		if (UpDisplayFlag & up_musicbitrate)
		{
			//先显示音乐类型图标
			if(g_currentMusicType != Music_NON)
			{
				GUI_ResShowImage(GUI_IMG_MUSICTYPE, g_currentMusicType > Music_WMA ? 0 : g_currentMusicType - 1, POSITION_B, 0);
			}
			if(g_BitRate >= 1000)
			{
				GUI_DisplayNumbers(POSITION_B + 3, 0, 3, GUI_IMG_BITRATENUMS, 999);
			}
			else if(g_BitRate > 0)
			{
				GUI_DisplayNumbers(POSITION_B + 3, 0, 3, GUI_IMG_BITRATENUMS, g_BitRate);
			}
	
		}
	
		if (UpDisplayFlag & up_musicsequence) /*Display Music Number Of Total Music */
		{
			switch (PlayStatus)
			{
			case MusicStop:
			case NormalPlay:
				GUI_DisplayNumberD(g_musicsequence, 0);
				GUI_DisplayNumberD(g_musicamount, 1);
				break;
			default:
				break;
			} //switch(PlayStatus)
		}
	
	
		if (UpDisplayFlag & up_musictotaltime)
		{
			GUI_DisplayText(LCD_WIDTH / 2 - 4, 16, "/");
			GUI_DisplayTimeText(LCD_WIDTH / 2 + 4, 16, GUI_TIME_DISPLAY_AUTO, TotalTime);
		}
	
		if (UpDisplayFlag & up_musiccurrenttime)
		{
			GUI_DisplayTimeText(20, 16, GUI_TIME_DISPLAY_AUTO, g_music_vars->BreakTime);
		}
	
		if (UpDisplayFlag & up_musicloopmode)/*Display Loop Mode*/
		{
			/* if (PlayStatus == MusicStop)
			 {
			     if (g_music_vars->fselmod == FSEL_TYPE_COMMONDIR)
			     {
			         g_music_vars->play_listmod = mlistnor;//改为普通目录模式
			     }
			    GUI_ResShowImage(GUI_IMG_MENUMUSIC, 2, POSITION_C, 0);
			    // GUI_DisplayIconC(PALYLIST_PIC[g_music_vars->play_listmod]);
			 }
			 else*/
			{
				GUI_ResShowImage(GUI_IMG_MUSICLOOPS, g_music_vars->repeat_mode, POSITION_F, 0);
			}
		}
	
	
		if (UpDisplayFlag & up_musiceqmode)
		{
			GUI_ResShowImage(GUI_IMG_MUSICEQS, g_music_vars->eq_type, POSITION_E + 2, 0);
		}
	
		if (UpDisplayFlag & up_followmode) /*Display Music Number Of Total Music */
		{
			if(PlayStatus > NormalPlay)
			{
				GUI_ResShowImage(GUI_IMG_MUSICRPT, PlayStatus - LoopPlay, POSITION_D, 0);
			}
		}
	
		if (UpDisplayFlag & up_musicbattery)
		{
			GUI_DisplayBattaryLever();
		}
	
		GUI_UpdateScreen(NULL);
	
		UpDisplayFlag  = 0x00;
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LED8S==1
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(GUI_ICON_MUSIC | GUI_ICON_COLON, 0, 0);
	if(g_music_vars->location.disk == FS_DEV_TYPE_USBDISK)
	{
		GUI_ResShowPic(GUI_ICON_USB, 0, 0);
	}
	else
	{
		GUI_ResShowPic(GUI_ICON_SD, 0, 0);
	}
	if(PlayStatus == MusicStop)
	{
		GUI_ResShowPic(GUI_ICON_PAUSE, 0, 0);
	}
	else
	{
		GUI_ResShowPic(GUI_ICON_PLAY, 0, 0);
	}
	
	GUI_DisplayTimeLEDs(g_music_vars->BreakTime);
	GUI_UpdateScreen(NULL);
#endif
	/*            delete by gary
	    if(PlayStatus >= FollowRec)
	        LED_SetPattern(GUI_LED_MUSIC_FOLLOW, LED_LOOP_INFINITE);
	    else
	        LED_SetPattern(GUI_LED_MUSIC_STOP + PlayStatus, LED_LOOP_INFINITE);
	        */
	
	if(PlayStatus == NormalPlay)
	{
		LED_SetPattern(GUI_LED_MUSIC_PLAY, LED_LOOP_INFINITE);		  // added by gary	 led light return to system
	}
	else
	{
		LED_SetPattern(GUI_LED_MUSIC_STOP, LED_LOOP_INFINITE);		 // added by gary	led light return to system
	}
}

void RealDisplay( void )
{
	INT8 temp_buffer[6];
	region_t refresh_rect = {0, 80, LCD_WIDTH, 32};
	region_t time_rect = {36, 36, 40, 16};
	ui_auto_update = FALSE;
	
#if APP_SUPPORT_RGBLCD==1
	
	
	if(AP_Support_LCD())
	{
		if (UpDisplayFlag & up_followmode) /*Display Music Number Of Total Music */
		{
			if(PlayStatus > NormalPlay)
			{
				GUI_ResShowImage(GUI_IMG_MUSICRPT, PlayStatus - LoopPlay, POSITION_D, 0);
			}
		}
		
		
		if (UpDisplayFlag & up_musiccurrenttime)
		{
			GUI_ClearScreen(&time_rect);
			GUI_DisplayTimeText(36, 36, GUI_TIME_DISPLAY_AUTO, g_music_vars->BreakTime);
			GUI_UpdateScreen(&time_rect);
		}
		
		if (0)//UpDisplayFlag & up_musicscroll)
		{
			INT8 nums[22];
			UINT8 num[5];
			UINT16 randnum;
			int i;
			
			//滚屏时的处理
			GUI_Scroll(g_TagInfoBuf, FALSE);
			
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
			if(!g_lyricflag)
#endif
			{
				randnum = rand();
				num[0] = (randnum >> 12) % 1;
				num[1] = (randnum >> 9) % 3;
				num[2] = (randnum >> 6) % 3;
				num[3] = (randnum >> 3) % 5;
				num[4] = (randnum) % 5;
				
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i], 18 + i * 6, 100);
				}
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i] + 4, 48 + i * 6, 100);
				}
				
				num[0] = (randnum >> 12) % 5;
				num[1] = (randnum >> 9) % 5;
				num[2] = (randnum >> 6) % 3;
				num[3] = (randnum >> 3) % 3;
				num[4] = (randnum) % 1;
				
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i] + 4, 78 + i * 6, 100);
				}
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i], 108 + i * 6, 100);
				}
				
			}
		}
		
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
		if (UpDisplayFlag & up_musiclyric)
		{
			if(g_lyricflag)
			{
				const region_t region = {0, 48, LCD_WIDTH, 16};
				GUI_ClearScreen(&region);
				GUI_DisplayText(0, 48, lyric_buff);
			}
		}
#endif
		//GUI_UpdateScreen(&refresh_rect);
		
		UpDisplayFlag  = 0x00;
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LCD==1
	if(AP_Support_LCD())
	{
		if (UpDisplayFlag & up_followmode) /*Display Music Number Of Total Music */
		{
			if(PlayStatus > NormalPlay)
			{
				GUI_ResShowImage(GUI_IMG_MUSICRPT, PlayStatus - LoopPlay, POSITION_D, 0);
			}
		}
	
	
		if (UpDisplayFlag & up_musiccurrenttime)
		{
			GUI_DisplayTimeText(20, 16, GUI_TIME_DISPLAY_AUTO, g_music_vars->BreakTime);
		}
	
		if (UpDisplayFlag & up_musicscroll)
		{
			INT8 nums[22];
			UINT8 num[5];
			UINT16 randnum;
			int i;
	
			//滚屏时的处理
			GUI_Scroll(g_TagInfoBuf, FALSE);
	
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
			if(!g_lyricflag)
#endif
			{
				randnum = rand();
				num[0] = (randnum >> 12) % 1;
				num[1] = (randnum >> 9) % 3;
				num[2] = (randnum >> 6) % 3;
				num[3] = (randnum >> 3) % 5;
				num[4] = (randnum) % 5;
	
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i], 4 + i * 6, 48);
				}
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i] + 4, 34 + i * 6, 48);
				}
	
				num[0] = (randnum >> 12) % 5;
				num[1] = (randnum >> 9) % 5;
				num[2] = (randnum >> 6) % 3;
				num[3] = (randnum >> 3) % 3;
				num[4] = (randnum) % 1;
	
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i] + 4, 64 + i * 6, 48);
				}
				for(i = 0; i < 5; i++)
				{
					GUI_ResShowImage(GUI_IMG_BARS, num[i], 94 + i * 6, 48);
				}
	
			}
		}
	
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
		if (UpDisplayFlag & up_musiclyric)
		{
			if(g_lyricflag)
			{
				const region_t region = {0, 48, LCD_WIDTH, 16};
				GUI_ClearScreen(&region);
				GUI_DisplayText(0, 48, lyric_buff);
			}
		}
#endif
		GUI_UpdateScreen(NULL);
	
		UpDisplayFlag  = 0x00;
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LED8S==1
	GUI_DisplayTimeLEDs(g_music_vars->BreakTime);
	GUI_UpdateScreen(NULL);
#endif
	
}

int DealFilePlayEnd(void)
{
	int g_result_bak;
	media_status_t status_buf;
	//////////////////////////////
	g_result_bak = g_result;
	
	mediaSendCommand(MC_GETSTATUS, (int) (&status_buf));
	if (status_buf.status == PLAYING_REACH_END)
	{
		g_result = DealMusicFileEnd(0);
		if (g_result != NULL)
		{
			if (g_result_bak) //g_result_bak ！＝0，返回其消息
			{
				g_result = g_result_bak;
			}
			return g_result;
		}
	}
	g_result = g_result_bak;
	////////////////////////解决DRM在菜单中长按mode重启问题
	return 0;
}



/*********************************************************************************
* Description :  处理在播放状态下对按键的判断处理
*
* input : 键值 key
*
* output: 返回值 == 0: 正常返回
*                != 0: 不是正常返回（如热键）
* Notes       :
*********************************************************************************/
INT32 ToNextMusic( Music_sel_mod_t mod )
{
	media_status_t status_buf;
	
	mediaSendCommand(MC_GETSTATUS, (INT32)(&status_buf));
	
	if(status_buf.status != PLAYING_REACH_END && status_buf.status != PLAYING_ERROR)
	{
		ERROR_EXIT = 0;
	}
	mediaSendCommand(MC_STOP, fast_fade);
	mediaSendCommand(MC_CLOSE, 0);
	g_result = MUSIC_SelectAudio(Music_Next, mod);
	if(!g_result)
	{
		return RESULT_UI_STOP;//回到停止界面
	}
	g_result = MUSIC_GetInformation(); //取文件信息
	//mediaSendCommand( MC_OPEN,  (INT32)& g_Openparam );
	//g_result = mediaSendCommand(MC_LOAD, 0);
	
	if(g_result)
	{
		//当是WMA时由于要提高速度，在这种情况下就不计算总时间了，
		//if ((g_currentMusicType != Music_WMA)||(g_PowerOffFlag == 0x55) ) //在掉电时也要判断一下是否drm 所以要算时间才知道
		{
			//g_result = mediaSendCommand(MC_CALTIME, 0);
		}
		//else
		{
			//CaltimeFlag = TRUE;
			//isFileInfoGot = FALSE;
		}
		
		/*if (g_PowerOffFlag == 0x55)
		{
		    if (!DisplayTimeRollback())
		    {
		        mediaSendCommand(MC_STOP, 0);
		        mediaSendCommand(MC_CLOSE, 0);
		        SetFullInterface();
		        need_draw = TRUE;
		        return RESULT_UI_STOP;
		    }
		}   */
	}
	
	if(!g_result)
	{
		//mediaSendCommand(MC_GETSTATUS, (INT32)(& status_buf));
		//ErrReport(PlayStatus);
		return RESULT_UI_STOP;//回到停止界面
	}
	
	mediaSendCommand(MC_PLAY, 0);
	return 0;
}


INT32 PrevMusicInPlay(void)
{
	media_status_t status_buf;
	mediaSendCommand(MC_GETSTATUS, (INT32)(&status_buf));
	if(status_buf.status != PLAYING_REACH_END && status_buf.status != PLAYING_ERROR)
	{
		ERROR_EXIT = 0;
	}
	mediaSendCommand(MC_STOP, fast_fade);
	mediaSendCommand(MC_CLOSE, 0);
	g_result = MUSIC_SelectAudio(Music_Prev, Play_key);
	if(!g_result)
	{
		return RESULT_UI_STOP;//回到停止界面
	}
	g_result = MUSIC_GetInformation(); //取文件信息
	//g_music_vars->MusicOpenMod = Nor_open;
	//mediaSendCommand( MC_OPEN,  (INT32)& g_Openparam );
	//g_result = mediaSendCommand( MC_LOAD, 0 );
	//当是WMA时由于要提高速度，在这种情况下就不计算总时间了，
	if (g_result)
	{
		/*if ((g_currentMusicType != Music_WMA)||(g_PowerOffFlag == 0x55))
		{
		    g_result = mediaSendCommand(MC_CALTIME, 0);
		}
		else*/
		{
			//CaltimeFlag = TRUE;
			//isFileInfoGot = FALSE;
		}
		
		/*if (g_PowerOffFlag == 0x55)
		{
		    if (!DisplayTimeRollback())
		    {
		        mediaSendCommand(MC_STOP, 0);
		        mediaSendCommand(MC_CLOSE, 0);
		        SetFullInterface();
		        need_draw = TRUE;
		        return RESULT_UI_STOP;
		    }
		}                      */
	}
	if(!g_result)
	{
		//mediaSendCommand(MC_GETSTATUS, (INT32)(& status_buf));
		ErrReport(PlayStatus);
		return RESULT_UI_STOP;//回到停止界面
	}
	SetFullInterface();
	need_draw = TRUE;
	
	mediaSendCommand(MC_PLAY, 0);
	
	return 0;
}

//===================================================
INT32 DealPlayKey( void )
{
	media_status_t status_buf;
	if (g_FCexitflag)
	{
		//关闭音量(在有此情况下退出时不想发声)
		mediaSendCommand(MC_SETVOLUME, 0);
	}
	
	app_trace(APP_MUSIC_TRC, "[DealPlayKey]g_FCexitflag=%d.\r\n", g_FCexitflag);
	g_FCexitflag = FALSE;
	
	
	mediaSendCommand(MC_GETSTATUS, (INT32)(&status_buf));
	app_trace(APP_MAIN_TRC, "%s(%d),status_buf.status:%d", __func__, __LINE__, status_buf.status);
	if(status_buf.status != PLAYING_REACH_END && status_buf.status != PLAYING_ERROR)
	{
		ERROR_EXIT = 0;
	}
	mediaSendCommand(MC_GETTIME, (INT32)&g_music_vars->BreakTime);
	mediaSendCommand(MC_STOP, slow_fade);
	//mediaSendCommand(MC_BACKUP, (INT32)& g_music_vars->BreakPTSave);
	mediaSendCommand(MC_CLOSE, 0);
	SetStopToPlayInterface();
	//g_music_vars->MusicOpenMod = Nor_Break_open;
	//RefreshMainScreen();
	//isFileInfoGot = TRUE;
	hal_HstSendEvent(SYS_EVENT, 0x19901327);
	hal_HstSendEvent(SYS_EVENT, TotalTime);
	hal_HstSendEvent(SYS_EVENT, g_music_vars->BreakTime);
	return RESULT_UI_STOP;//退回到停止状态
}

/******************************************************************************
** 名字:
** 接口:
** 描述: 在 play 状态下碰到　错的报告
** 输入参数:
** 输出参数:
** 使用说明:
********************************************************************************/
BOOL bad_format = FALSE;
void ErrReport(UINT8 status)
{
#if 0
	wma_inf_t drminfbuff;
	
	if (mediaSendCommand(MC_GETINF, (INT32)&drminfbuff))
	{
		if (drminfbuff.drmmode == INF_DRMNOLICENSE)
		{
			ResShowMultiLongStr(GUI_STR_NOLICENS, g_comval->langid, 10);
			goto ErrReport_exit;
		}
	}
#endif
	if ( (g_music_vars->location.disk == FS_DEV_TYPE_USBDISK) && ( !Check_CardOrUsb()) )
	{
		MUSIC_DisplayErrMsg(GUI_STR_CARDPULL);//显示卡拔出
	}
	else
	{
		ERROR_EXIT++;
		MUSIC_DisplayErrMsg(GUI_STR_FORMATERR);//显示格式?
		MESSAGE_SetEvent( AP_KEY_NEXT | AP_KEY_PRESS);
		bad_format = TRUE;
		return;
	}
ErrReport_exit:
	mediaSendCommand(MC_STOP, no_fade);
	mediaSendCommand(MC_CLOSE, 0);
	
	SetFullInterface();
	
}


/*********************************************************************************
*             INT32 MUSIC_HandleInputNum( void )
*
* Description : input number
*
* Arguments   : void
*
* Returns     : INT32
*               RESULT_UI_STOP： 表示要进入停止状态，此时所有module已关闭
*               key: 热健消息
*               NULL: 表示正常退出
*
* Notes       : 在此状态下，按play到stop状态；按mode到play;按NEXT到对比状态；
*
*********************************************************************************/
INT32 MUSIC_HandleInputNum(UINT32 num)
{
#if APP_SUPPORT_REMOTECONTROL==1
	UINT32 LEntry = 0;
	g_musicsequence = num;
	mediaSendCommand(MC_CLOSE, 0);
	fselGetByNO(&LEntry, num);
	g_music_vars->location.file_entry = LEntry;
	FS_GetShortName(LEntry, g_music_vars->location.file_name);
	g_currentMusicType = MUSIC_CheckType(g_music_vars->location.file_name);
	MUSIC_ClearBreakTimePara();
	isFileInfoGot = FALSE;
	MUSIC_GetInformation(); //取文件信息
#endif
	return 0;
}

/*********************************************************************************
*             void OpenMusic( void )
*
* Description :
*
* Arguments   : void
*
* Returns     : void
*
* Notes       :
*
*********************************************************************************/
void OpenMusic( void )
{
	//g_music_vars->MusicOpenMod = ABPlay_open;
	mediaSendCommand(MC_OPEN, (INT32)& g_Openparam);
	//mediaSendCommand(MC_LOAD, 0);
	//mediaSendCommand(MC_CALTIME, 0);
	//mediaSendCommand(MC_RESTORE, (INT32)& g_music_vars->BreakPTSave);
	mediaSendCommand(MC_PLAY, 0);//g_music_vars->MusicOpenMod);
}


/*********************************************************************************
*             INT32 MUSIC_DisplayFollow( void )
*
* Description : 复读 子状态
*
* Arguments   : void
*
* Returns     : INT32
*
* Notes       : 在此状态下，按play/next/prev 到play状态；
*
*********************************************************************************/
INT32 MUSIC_DisplayFollow( void )
{
	UINT32 key;
	UINT8 play_end = 1;
	UINT8 loop_count = 0;
	UINT8 loop_time = 0; // in half second
	UINT8 loop_time_count = 0;
	UINT32 start_time;
	Open_param_t param;
	
	media_status_t status_buf;
	
	mediaSendCommand(MC_GETSTATUS, &status_buf);
	mediaSendCommand(MC_GETSTARTPOS, &start_time);
	ReplayStartPos = start_time;
	ReplayEndPos = status_buf.progress;
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
	lyric_next_time = 0;
#endif
	if(((ReplayEndPos - ReplayStartPos) * TotalTime / 10000) / 1000 > g_music_vars->maxReplayTime)
	{
		ReplayStartPos = ReplayEndPos - g_music_vars->maxReplayTime * 10000 / (TotalTime / 1000);
	}
	loop_time = ((ReplayEndPos - ReplayStartPos) * TotalTime / 10000 + 500) / 1000 * 2 ;
	start_time = ((ReplayStartPos) * TotalTime / 10000 + 500);
	g_music_vars->BreakTime = start_time;
	
	PlayStatus = LoopPlay;
	//RealDisplay();
	SetFullInterface();
	UpDisplayFlag |= up_followmode;
	need_draw = TRUE;
	
	hal_HstSendEvent(SYS_EVENT, 0x12200020);
	hal_HstSendEvent(SYS_EVENT, ReplayStartPos);
	hal_HstSendEvent(SYS_EVENT, ReplayEndPos);
	hal_HstSendEvent(SYS_EVENT, loop_time);
	hal_HstSendEvent(SYS_EVENT, start_time);
	
	param.file_name = FollowReadTempShortName;
	param.type = Music_WAV;
	
	while (1)
	{
		if(play_end)
		{
			mediaSendCommand(MC_STOP, no_fade);
			mediaSendCommand(MC_PLAY, ReplayStartPos);
			
			play_end = 0;
		}
		
		if (need_draw)
		{
			RefreshMainScreen();
			need_draw = FALSE;
		}
		else if(UpDisplayFlag)
		{
			RealDisplay();
		}
		
		key = MESSAGE_Wait();
		
		switch (key)
		{
		case AP_MSG_STANDBY:
			break;
			
		case AP_KEY_NULL:
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
			if(g_lyricflag)
			{
				if(g_music_vars->BreakTime >= lyric_next_time)
				{
					lyric_next_time = g_music_vars->BreakTime;
					if(lyricGetNext(lyric_buff, g_music_vars->Lyric_Size, &lyric_next_time))
					{
						UpDisplayFlag |= up_musiclyric;
					}
					else
					{
						lyricPreLoad();
					}
				}
				else
				{
					lyricPreLoad();
				}
			}
#endif
			//RealDisplay();
			//need_draw = TRUE;
			break;
		case AP_KEY_NEXT | AP_KEY_DOWN:
		case AP_KEY_PREV | AP_KEY_DOWN:
		case AP_KEY_MODE | AP_KEY_PRESS:
			PlayStatus = NormalPlay;
			g_FCexitflag = TRUE;
			if(PlayStatus == ComparePlay || PlayStatus == FollowRec)
			{
				mediaSendCommand(MC_STOP, no_fade);
				mediaSendCommand(MC_CLOSE, 0);
				mediaSendCommand(MC_OPEN, (INT32)&g_Openparam);
			}
			else
			{
				mediaSendCommand(MC_STOP, no_fade);
			}
			//OpenMusic();
			SetFullInterface();
			g_music_vars->BreakTime = ((ReplayEndPos) * TotalTime / 10000);
			need_draw = TRUE;
			return RESULT_UI_PLAY; //选关播放后回到停止界面
			
		case MUSIC_SET_FOLLOW:
		case AP_KEY_PLAY | AP_KEY_PRESS:
		case AP_KEY_REC | AP_KEY_PRESS: // start follow record
			if(PlayStatus == FollowRec)
			{
				break;
			}
			else
			{
				PlayStatus = FollowRec;
				loop_time_count  = 0;
				loop_count = 0;
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
				lyric_next_time = 0;
#endif
				mediaSendCommand(MC_STOP, no_fade);
				mediaSendCommand(MC_CLOSE, 0);
				param.mode = MEDIA_RECORD;
				//open codec
				mediaSendCommand(MC_OPEN, (INT32)&param);
				mediaSendCommand(MC_RECORD, 0);
				UpDisplayFlag = up_followmode;
			}
		//break;
		
		case AP_MSG_RTC:
			UpDisplayFlag = up_musicscroll;
			//UpDisplayFlag = UpDisplayFlag | up_musiccurrenttime | up_musicbattery;
			//need_draw = TRUE;
			if(PlayStatus == LoopPlay || PlayStatus == CompareAB)
			{
				mediaSendCommand(MC_GETSTATUS, &status_buf);
				mediaSendCommand(MC_GETTIME, (INT32) & (g_music_vars->BreakTime));
				if(status_buf.progress >= ReplayEndPos)
				{
					//g_music_vars->BreakTime = start_time;
					if(PlayStatus == CompareAB)
					{
						g_music_vars->BreakTime = start_time;
						PlayStatus = ComparePlay;
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
						lyric_next_time = 0;
#endif
						loop_time_count  = 0;
						
						mediaSendCommand(MC_STOP, no_fade);
						mediaSendCommand(MC_CLOSE, 0);
						param.mode = MEDIA_PLAYREC;
						//open codec
						mediaSendCommand(MC_OPEN, (INT32)&param);
						mediaSendCommand(MC_PLAY, 0);
					}
					else
					{
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
						lyric_next_time = 0;
#endif
						play_end = 1;
						loop_count ++;
						if(loop_count >= g_music_vars->replaytimes)
						{
							//此处复读完成后要发close命令。
							//否则会导致hal_voc.c hal_VocOpen函数中g_halVocState
							//状态不为HAL_VOC_CLOSE_DONE，
							//导致重新进入UI后会报格式错误
							mediaSendCommand(MC_STOP, no_fade);
							mediaSendCommand(MC_CLOSE, 0);
							//---------------end---------------
							return RESULT_UI_PLAY;
						}
						else
						{
							g_music_vars->BreakTime = start_time;
						}
					}
					UpDisplayFlag = up_followmode;
				}
			}
			else if(PlayStatus == ComparePlay || PlayStatus == FollowRec)
			{
				loop_time_count ++;
				g_music_vars->BreakTime = start_time + loop_time_count * 1000 / 2;
				// ShowNowTime(start_time + loop_time_count*1000/2);
				if(loop_time_count >= loop_time)
				{
					mediaSendCommand(MC_STOP, no_fade);
					mediaSendCommand(MC_CLOSE, 0);
					
					mediaSendCommand(MC_OPEN, (INT32)&g_Openparam);
					
					play_end = 1;
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
					lyric_next_time = 0;
#endif
					g_music_vars->BreakTime = start_time;
					if(PlayStatus == FollowRec)
					{
						PlayStatus = CompareAB;
					}
					else
					{
						PlayStatus = LoopPlay;
						loop_count = 0;
					}
					UpDisplayFlag = up_followmode;
				}
			}
			UpDisplayFlag |= up_musiccurrenttime;
			//RealDisplay();
			
			break;
			
		default:
			key = MUSIC_HandleKey(key);
			if (key == RESULT_REDRAW)
			{
				SetFullInterface();
				need_draw = TRUE;
			}
			else if (key != 0)
			{
				g_FCexitflag = TRUE;
				OpenMusic();
				SetFullInterface();
				need_draw = TRUE;
				return key;
			}
			break;
		} //switch(key)
	} //while(1)
	
	return 0;
}

//======================================================================================
//RESULT_CURR_UI: 还要处在当前界面
//RESULT_UI_STOP: 到UI_STOP界面（因现在就是UI_STOP界面，所以同上）
//RESULT_LAST_UI: 到上一级界面
//--------------------------------------------------------------------------------------
INT32 MUSIC_Main(void)
{
	MESSAGE_Initial(NULL);//开始算STANDBY 时间
	g_result = RESULT_UI_PLAY;
	hal_HstSendEvent(SYS_EVENT, 0x20131010);
	gpio_SetMute(TRUE);//warkey 2.0
	do
	{
		if (UpdateFileSelector() == 0)//根据location设置文件选择器参数
		{
#if APP_SUPPORT_USB
			if(g_music_vars->location.disk == FS_DEV_TYPE_TFLASH)
			{
				g_music_vars->location.disk = FS_DEV_TYPE_USBDISK;
				if (MountDisk(g_music_vars->location.disk))
				{
					fselInit(FSEL_TYPE_MUSIC, FSEL_ALL_SEQUENCE, FSEL_TYPE_COMMONDIR, g_music_vars->location.disk);
					continue;
				}
			}
#endif
			hal_HstSendEvent(SYS_EVENT, 0x20131011);
			MUSIC_DisplayErrMsg(GUI_STR_CFILEOUT);//显示卡无文件
			return 0;
		}
	}
	while(0);
	hal_HstSendEvent(SYS_EVENT, 0x20131012);
	g_musicamount   = fselGetTotal(); //当前歌曲目录总数
	g_musicsequence = fselGetNo();    //获得当前歌曲编号
	
	fselSetMode(g_music_vars->repeat_mode); //循环模式
	
	//MUSIC_GetInformation(); //取文件信息ID3信息no lcd
	
	SetFullInterface();
	need_draw = TRUE;
	
	do
	{
#if APP_SUPPORT_FOLLOW_COMPARE==1
		if(g_result == RESULT_UI_FOLLOW)
		{
			SetFullInterface();
			need_draw = TRUE;
			g_result = MUSIC_DisplayFollow();
			//mediaSendCommand(MC_STOP, no_fade);
			//mediaSendCommand(MC_CLOSE, 0);
			FS_Delete(FollowReadTempLongName);
			if(g_result != RESULT_UI_PLAY)
			{
				return g_result;
			}
		}
#endif
		hal_HstSendEvent(SYS_EVENT, 0x20131013);
		hal_HstSendEvent(SYS_EVENT, g_result);
		if(g_result == RESULT_UI_PLAY)
		{
			g_result = MUSIC_DisplayPlay();
			
			mediaSendCommand(MC_GETTIME, (INT32)&g_music_vars->BreakTime);
			//mediaSendCommand(MC_BACKUP, (INT32)& g_music_vars->BreakPTSave);
			
			MESSAGE_Initial(NULL);
			
			if (g_result != 0 && g_result != RESULT_UI_FOLLOW)
			{
				INT32 i = g_result;
				//if(PlayStatus != MusicStop)
				{
					mediaSendCommand(MC_STOP, slow_fade);
					//mediaSendCommand(MC_BACKUP, (INT32)& g_music_vars->BreakPTSave);
					//g_music_vars->MusicOpenMod = Nor_Break_open;
				}
				g_result = i;
			}
			g_FCexitflag = FALSE;
		}
		else
		{
			g_result = MUSIC_DisplayStop();//进入单首歌曲播放界面，直到切换播放状态或者其它原因退出。
		}
		
		
		/*        mediaSendCommand(MC_CALTIME, 0); //carl 06-11-27
		        CaltimeFlag = TRUE; //carl 06-11-27
		        g_result = MUSIC_DisplayStop();//进入单首歌曲播放界面，直到切换播放状态或者其它原因退出。
		
		        mediaSendCommand(MC_CLOSE, 0);      */
		// dont get location here, maybe the disk is not avaible!!!  fselGetLocation(& g_music_vars->location);//将当前播放时产生的文件选择器的全局变量信息保存到nvram区域。
	}
	while((g_result == RESULT_CURR_UI) || (g_result == RESULT_UI_STOP) || (g_result == RESULT_UI_STOP2) || (g_result == RESULT_UI_PLAY ) || (g_result == RESULT_UI_FOLLOW)); //还保持在ui_stop界面
	
	if (g_result == RESULT_LAST_UI)
	{
		return 0;//不带参数回到上一级
	}
	return g_result;//带参数回到上一级
}

//======================================================================================

INT32 MUSIC_DisplayStop(void)
{
	media_status_t status_buf;
	UINT32 key;
	gpio_SetMute(TRUE);//warkey 2.0
	PlayStatus = MusicStop;
	
	//UpDisplayFlag = up_musicloopmode | up_cardflag|up_musicinformation;
	need_draw = TRUE;
	
	while (1)
	{
		key = MESSAGE_Wait();
		
		if (need_draw)
		{
			RefreshMainScreen();
			need_draw = FALSE;
		}
		
#if APP_SUPPORT_REMOTECONTROL==1
		if(MESSAGE_IsNumberKey(key))
		{
			UINT32 num_input = GUI_KeyNumberHandle(key);
			if ( (num_input <= g_musicamount) && (num_input != 0) )
			{
				MUSIC_HandleInputNum(num_input);
				CaltimeFlag = FALSE;//等待计时间
				WaitForCaltimeCNT = 0;
			}
			SetFullInterface();
			need_draw = TRUE;
		}
#endif
		switch (key)
		{
		case AP_MSG_WAIT_TIMEOUT:
			APP_Sleep();
			break;
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
			if (CaltimeFlag == 0)
			{
				if (WaitForCaltimeCNT++ >= CaltimeCNT_def)
				{
					MUSIC_GetInformation(); //取文件信息
					UpDisplayFlag = UpDisplayFlag | up_musictotaltime | up_musicbitrate | up_musicinformation;
					CaltimeFlag = 1;
					need_draw = TRUE;
				}
			}
			break;
			
		case AP_KEY_PLAY | AP_KEY_PRESS:
			/*if (CaltimeFlag || ifSelBuffToltalTimeFlag)
			{
			    mediaSendCommand(MC_GETSTATUS, (&status_buf));
			    if (status_buf.status == PLAYING_ERROR)
			    {
			        g_music_vars->BreakTime = 0;
			    }
			}*/
			
			//mediaSendCommand(MC_RESTORE, (INT32)& g_music_vars->BreakPTSave);
			
			//if (g_music_vars->MusicOpenMod == Nor_open)
			{
				// StartIntro(TRUE);
			}
			//else
			{
				StartIntro(FALSE);
			}
			
			//if ((!CaltimeFlag) && (!ifSelBuffToltalTimeFlag))
			{
				//mediaSendCommand(MC_CALTIME, 0);
			}
			
			//判断是否刚断电完而没有重设时间
			/*if (g_PowerOffFlag == 0x55)
			{
			    if (!DisplayTimeRollback())
			    {
					SetFullInterface();
					need_draw = TRUE;
					break;
			    }
			}*/
			
			UpDisplayFlag |= (up_musictotaltime | up_musicbitrate);
			need_draw = TRUE;
			return RESULT_UI_PLAY;
			break;
			
		case AP_KEY_PREV | AP_KEY_PRESS:
		case AP_KEY_PREV | AP_KEY_HOLD:
			mediaSendCommand(MC_CLOSE, 0);
			g_result = MUSIC_SelectAudio(Music_Prev, Stop_key);
			CaltimeFlag = FALSE;//等待计时间
			WaitForCaltimeCNT = 0;
			SetFullInterface();
			need_draw = TRUE;
			
			//清除多出的HOLD消息,防止按键停了还跳几曲
			//while(MESSAGE_Get() != 0);
			break;
			
		case AP_KEY_NEXT | AP_KEY_UP:
			//while(MESSAGE_Get() != 0);
			break;
			
		case AP_KEY_NEXT | AP_KEY_PRESS:
		case AP_KEY_NEXT | AP_KEY_HOLD:
			mediaSendCommand(MC_CLOSE, 0);
			g_result = MUSIC_SelectAudio(Music_Next, Stop_key);
			CaltimeFlag = FALSE;//等待计时间
			WaitForCaltimeCNT = 0;
			
			SetFullInterface();
			need_draw = TRUE;
			
			//while(MESSAGE_Get() != 0);
			break;
			
		default:
			key = MUSIC_HandleKey(key);
			if (key == RESULT_REDRAW)
			{
				SetFullInterface();
				need_draw = TRUE;
			}
			else if (key != 0)
			{
				return key;
			}
			break;
		} //switch(key)
	} //while(1)
}


/*********************************************************************************
*
* Description : 显示时间，滚屏,AB闪
*
* Arguments   : void
*
* Returns     : void
*
* Notes       :
*
*********************************************************************************/
BOOL music_play_flag = FALSE;
UINT8 music_play_timer = 0;

UINT8 music_play_timeout(void *param)
{
	COS_EVENT ev = {0};
	hal_HstSendEvent(SYS_EVENT, 0x20133000);
	hal_HstSendEvent(SYS_EVENT, music_play_flag);
	if(music_play_flag)
	{
	
		ev.nEventId = 0;
		ev.nParam1 = 0;
		COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	}
	music_play_timer = 0;
}

INT32 MUSIC_DisplayPlay( void )
{
	UINT32 key, count = 0;
	media_status_t status_buf;
	PlayStatus = NormalPlay;/* 播放状态 */
	need_draw = TRUE;/* 需要画screen */
	if(music_play_timer == 0)
	{
		music_play_timer = COS_SetTimer(4000, music_play_timeout, NULL, COS_TIMER_MODE_SINGLE);
	}
	music_play_flag = TRUE;
	//key = media_PlayInternalAudio(GUI_AUDIO_TFCARD_PLAY, 1, TRUE);
	music_play_flag = FALSE;
	SetStopToPlayInterface();
	//if (!isFileInfoGot)
	{
		MUSIC_GetInformation(); //取文件信息
	}
	
	//mediaSendCommand(MC_RESTORE, (INT32)& g_music_vars->BreakPTSave);
	
	//if (g_music_vars->MusicOpenMod == Nor_open)
	{
		//  StartIntro(TRUE);
	}
	//else
	{
		StartIntro(FALSE);
	}
	
	//if ((!CaltimeFlag) && (!ifSelBuffToltalTimeFlag))
	{
		//mediaSendCommand(MC_CALTIME, 0);
	}
	
	//判断是否刚断电完而没有重设时间
	/*if (g_PowerOffFlag == 0x55)
	{
		if (!DisplayTimeRollback())
	    {
			SetFullInterface();
			need_draw = TRUE;
			return RESULT_UI_STOP;
	    }
	}*/
	hal_HstSendEvent(SYS_EVENT, 0x20131001);
	hal_HstSendEvent(SYS_EVENT, TotalTime);
	hal_HstSendEvent(SYS_EVENT, g_music_vars->BreakTime);
	
	
	if(TotalTime == 0)
	{
		ReplayStartPos = 0;
	}
	else
	{
		ReplayStartPos = (INT64)g_music_vars->BreakTime * 10000 / TotalTime;
	}
	
	mediaSendCommand(MC_SETVOLUME, g_music_vars->volume);
	if (!mediaSendCommand(MC_PLAY, ReplayStartPos))//g_music_vars->MusicOpenMod))
	{
		ErrReport(PlayStatus);// playStatus 并未改变
		if(!bad_format)
		{
			hal_HstSendEvent(SYS_EVENT, 0x20131002);
			return RESULT_UI_STOP;
		}
		bad_format = FALSE;
	}
	mediaSendCommand(MC_SETVOLUME, g_music_vars->volume);
	hal_HstSendEvent(SYS_EVENT, 0x20131003);
	while (1)
	{
		if (need_draw)
		{
			RefreshMainScreen();
			need_draw = FALSE;
		}
		else if(UpDisplayFlag)
		{
			RealDisplay();
		}
		
		if(key == 0)
		{
			key = MESSAGE_Wait();    //读键值
		}
		
#if APP_SUPPORT_REMOTECONTROL==1
		if(MESSAGE_IsNumberKey(key))
		{
			UINT32 num_input = GUI_KeyNumberHandle(key);
			if ( (num_input <= g_musicamount) && (num_input != 0) )
			{
				mediaSendCommand(MC_STOP, fast_fade);
				MUSIC_HandleInputNum(num_input);
				mediaSendCommand(MC_PLAY, 0);
			}
			SetFullInterface();
			need_draw = TRUE;
		}
#endif
		
		switch (key)
		{
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
			UpDisplayFlag = up_musicscroll;
			break;
		case AP_KEY_NULL://没按键
			{
				UINT32 time;
				
				if(FFPlay_CNT != 0)
				{
					break;
				}
				//hal_HstSendEvent(SYS_EVENT, 0x19901329);
				//hal_HstSendEvent(SYS_EVENT,  TotalTime);
				//hal_HstSendEvent(SYS_EVENT,  g_music_vars->BreakTime);
				mediaSendCommand(MC_GETTIME, (INT32)&time);
				//hal_HstSendEvent(SYS_EVENT,  time);
				if(g_music_vars->repeat_mode == FSEL_DIR_INTRO && time > MUSIC_INTRO_TIME * 1000)
				{
					g_result = ToNextMusic(Play_play);
					if(g_result != 0)
					{
						return g_result;
					}
					SetFullInterface();
					need_draw = TRUE;
					break;
				}
				if(time - g_music_vars->BreakTime > 500)
				{
					//hal_HstSendEvent(SYS_EVENT, 0x19901330);
					//g_music_vars->BreakTime = time;
					g_music_vars->BreakTime = time;
					UpDisplayFlag |= up_musiccurrenttime;
				}
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
				if(g_lyricflag)
				{
					if(time >= lyric_next_time)
					{
						lyric_next_time = time;
						if(lyricGetNext(lyric_buff, g_music_vars->Lyric_Size, &lyric_next_time))
						{
							UpDisplayFlag |= up_musiclyric;
						}
						else
						{
							lyricPreLoad();
						}
					}
					else
					{
						lyricPreLoad();
					}
				}
#endif
				//need_draw = TRUE;
				//RealDisplay();
			}
			break;
			
		case AP_MSG_STANDBY:
			break;
			
		case AP_KEY_NEXT | AP_KEY_LONG: //快进
			if(FFPlay_CNT == 0)
			{
				mediaSendCommand(MC_STOP, fast_fade);
				//media_PlayInternalAudio(GUI_AUDIO_MUSIC_FAST_FORWARD, 0, FALSE);
				//need_draw = TRUE;
			}
			FFPlay_CNT = 1;
			
			g_music_vars->BreakTime += MUSIC_FFPLAY_STEP * 1000;
			if(g_music_vars->BreakTime > TotalTime)
			{
				g_music_vars->BreakTime = TotalTime;
			}
			g_music_vars->BreakTime = g_music_vars->BreakTime;
			//ShowNowTime(g_music_vars->BreakTime);
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
			if(g_lyricflag && lyric_next_time < TotalTime)
			{
				lyric_next_time = g_music_vars->BreakTime;
				if(!lyricGetNext(lyric_buff, g_music_vars->Lyric_Size, &lyric_next_time))
				{
					while(lyricPreLoad()); // load lyric for fast play
					lyricGetNext(lyric_buff, g_music_vars->Lyric_Size, &lyric_next_time);
				}
				UpDisplayFlag |= up_musiclyric;
				//need_draw = TRUE;
				while(lyricPreLoad()); // load lyric for fast play
			}
#endif
			//RealDisplay();
			UpDisplayFlag |= up_musiccurrenttime;
			break;
			
		case AP_KEY_PREV | AP_KEY_LONG: //快退
			if(FFPlay_CNT == 0)
			{
				mediaSendCommand(MC_STOP, fast_fade);
				//media_PlayInternalAudio(GUI_AUDIO_MUSIC_FAST_FORWARD, 0, FALSE);
				//need_draw = TRUE;
			}
			FFPlay_CNT = -1;
			
			if(g_music_vars->BreakTime  < MUSIC_FFPLAY_STEP * 1000)
			{
				g_music_vars->BreakTime = 0;
			}
			else
			{
				g_music_vars->BreakTime -= MUSIC_FFPLAY_STEP * 1000;
			}
			g_music_vars->BreakTime = g_music_vars->BreakTime;
			//ShowNowTime(g_music_vars->BreakTime);
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
			if(g_lyricflag)
			{
				lyric_next_time = g_music_vars->BreakTime;
				if(!lyricGetNext(lyric_buff, g_music_vars->Lyric_Size, &lyric_next_time))
				{
					while(lyricPreLoad()); // load lyric for fast play
					lyricGetNext(lyric_buff, g_music_vars->Lyric_Size, &lyric_next_time);
				}
				UpDisplayFlag |= up_musiclyric;
				//need_draw = TRUE;
				while(lyricPreLoad()); // load lyric for fast play
			}
#endif
			UpDisplayFlag |= up_musiccurrenttime;
			//RealDisplay();
			break;
			
		case AP_KEY_NEXT | AP_KEY_UP: //快进
		case AP_KEY_PREV | AP_KEY_UP: //快退
			if(FFPlay_CNT != 0)
			{
				INT32 progress;
				if(TotalTime != 0)
				{
					progress = (INT64)g_music_vars->BreakTime * 10000 / TotalTime;
				}
				else
				{
					progress = 0;
				}
				media_StopInternalAudio();
				if(progress > 9990)
				{
					ERROR_EXIT = 0;
					DealMusicFileEnd(0);
				}
				else
				{
					mediaSendCommand(MC_PLAY, progress);
				}
				FFPlay_CNT = 0;
				//need_draw = TRUE;
			}
			break;
#if APP_SUPPORT_FM_OUT
		case MUSIC_OPEN_FMOUT:
		case AP_KEY_PLAY | AP_KEY_HOLD: // open fm out
			{
				if(g_music_vars->fm_out)
				{
					break;
				}
				else
				{
					g_result = FM_OutPut(0xff, &(g_music_vars->freq), &g_music_vars->fm_out);
				}
				
				if (g_result != 0)
				{
					return g_result;
				}
				mediaSendCommand(MC_SETVOLUME, g_music_vars->volume);
				SetFullInterface();
				need_draw = TRUE;
			}
			break;
#endif
		case AP_KEY_PREV | AP_KEY_PRESS:
			g_result = PrevMusicInPlay();
			if(g_result != 0)
			{
				return g_result;
			}
			SetFullInterface();
			need_draw = TRUE;
			if(g_comval->music_cfg.loop_mode == 0)
			{
				g_music_vars->repeat_mode = FSEL_ALL_REPEATALL;
			}
			break;
			
		case AP_KEY_NEXT | AP_KEY_PRESS:
			g_result = ToNextMusic(Play_key);
			if(g_result != 0)
			{
				return g_result;
			}
			SetFullInterface();
			need_draw = TRUE;
			if(g_comval->music_cfg.loop_mode == 0)
			{
				g_music_vars->repeat_mode = FSEL_ALL_REPEATALL;
			}
			break;
			
#if APP_SUPPORT_FOLLOW_COMPARE==1
		case MUSIC_START_REPEAT:
		case AP_KEY_REC   | AP_KEY_PRESS: //start follow
#if APP_SUPPORT_FM_OUT
			if(g_music_vars->fm_out)
			{
				FM_OutPut(0xfe, NULL, &g_music_vars->fm_out);  // close fm
			}
#endif
			return RESULT_UI_FOLLOW;
			break;
#endif
		case AP_KEY_PLAY | AP_KEY_PRESS:
			return DealPlayKey();
			
		default:
			key = MUSIC_HandleKey(key);
			if (key == RESULT_REDRAW)
			{
				SetFullInterface();
				need_draw = TRUE;
			}
			else if (key != 0)
			{
				if (key == RESULT_UI_STOP)
				{
					return 0;
				}
				else
				{
					return key;
				}
			}
			break;
		}
		
		key = 0;
		
		mediaSendCommand(MC_GETSTATUS, (INT32)(&status_buf));
		
		if (status_buf.status == PLAYING_ERROR)
		{
			//mediaSendCommand(MC_STOP, no_fade);
			//mediaSendCommand(MC_CLOSE, 0);
			if ( (g_music_vars->location.disk == FS_DEV_TYPE_USBDISK) && (!Check_CardOrUsb()) )
			{
				MUSIC_DisplayErrMsg(GUI_STR_CARDPULL);//显示卡拔出
				SetFullInterface();
				return 0;
			}
			else
			{
				//INT16 CLEAR_FLAG;
				ERROR_EXIT++;
				//app_trace(APP_MAIN_TRC,"%s(%d):g_musicamount:%d,ERROR_EXIT:%d",__func__,__LINE__,g_musicamount,ERROR_EXIT);
				//app_trace(APP_MAIN_TRC,"%d",g_music_vars->location.subdir_entry);
				//CLEAR_FLAG=strcmp();
				//strcpy(subdir_namebackup, g_music_vars->location.subdir_name)
				
				
				MUSIC_DisplayErrMsg(GUI_STR_FORMATERR);//显示格式错
				if(ERROR_EXIT >= (g_musicamount + 1))
				{
					ERROR_EXIT = 0;
					return  RESULT_MAIN;//now  exit music mode
				}
				g_result = ToNextMusic(Play_play);
				if(g_result != 0)
				{
					return g_result;
				}
				SetFullInterface();
				need_draw = TRUE;
				if(g_comval->music_cfg.loop_mode == 0)
				{
					g_music_vars->repeat_mode = FSEL_ALL_REPEATALL;
				}
				
				
				//MESSAGE_SetEvent( AP_KEY_NEXT | AP_KEY_PRESS);
				hal_HstSendEvent(SYS_EVENT, 0x21030109);
			}
		}
		
		if (status_buf.status == PLAYING_REACH_END)
		{
		
#if 1
		
			if((UINT32)gpio_detect_tcard() != 1)
			
			{
			
				return RESULT_MAIN;
				
			}
			
#endif
			
			
			g_result = DealMusicFileEnd(0);
			if ( g_result != 0 )
			{
				return g_result;
			}
#if 0
			INT32 progress;
			
			if(ERROR_COUNT < ERROR_COUNT_MAX)
			{
				if(TotalTime != 0)
				{
					progress = (INT64)g_music_vars->BreakTime * 10000 / TotalTime;
				}
				
				else
				{
					progress = 0;
				}
				hal_HstSendEvent(SYS_EVENT, 0x19900001);
				hal_HstSendEvent(SYS_EVENT, progress);
				if (progress < 9900)
				{
					ERROR_COUNT++;
					hal_HstSendEvent(SYS_EVENT, 0x19900002);
					hal_HstSendEvent(SYS_EVENT, progress);
					
					
					mediaSendCommand(MC_STOP, no_fade);
					//mediaSendCommand(MC_CLOSE, 0);
					
					g_music_vars->BreakTime += MUSIC_ERROR_SKIP_STEP * 1000;
					if(g_music_vars->BreakTime > TotalTime)
					{
						g_music_vars->BreakTime = TotalTime;
					}
					
					if(TotalTime != 0)
					{
						progress = (INT64)g_music_vars->BreakTime * 10000 / TotalTime;
					}
					else
					{
						progress = 0;
					}
					//  media_StopInternalAudio();
					mediaSendCommand(MC_OPEN, (INT32)&g_Openparam);
					if(progress > 9990)
					{
						g_result = DealMusicFileEnd(0);
						if ( g_result != 0 )
						{
							return g_result;
						}
					}
					else
					{
						mediaSendCommand(MC_PLAY, progress);
					}
					
					UpDisplayFlag |= up_musiccurrenttime;
					
				}
				else
				{
				
					ERROR_EXIT = 0;
					g_result = DealMusicFileEnd(0);
					if ( g_result != 0 )
					{
						return g_result;
					}
				}
			}
			else
			{
			
				app_trace(APP_MAIN_TRC, "%s(%d):ERROR_COUNT:%d,ERROR_EXIT:%d", __func__, __LINE__, ERROR_COUNT, ERROR_EXIT);
				
				ERROR_EXIT++;
				if(ERROR_EXIT >= g_musicamount)
				{
					ERROR_EXIT = 0;
					return  RESULT_BT;
				}
				g_result = DealMusicFileEnd(0);
				if ( g_result != 0 )
				{
					return g_result;
				}
			}
#endif
		}
		else if(TotalTime == 0) // if get file information fail in open file, reget here
		{
#if APP_SUPPORT_RGBLCD==1
			if(AP_Support_LCD())
			{
				audInfoStruct *pAudioinfo = COS_Malloc(sizeof(audInfoStruct));
				
				//if(Audio_GetDetailsInfo(g_music_vars->location.file_entry,&g_curaudioinfo,g_currentMusicType,g_comval->langid))
				if(mediaSendCommand(MC_GETFILEINFO, pAudioinfo))
				{
					g_BitRate = pAudioinfo->bitRate / 1000;
					TotalTime =  pAudioinfo->time;
					SetStopToPlayInterface();
					need_draw = TRUE;
				}
				
				COS_Free(pAudioinfo);
			}
			else
#elif APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				audInfoStruct *pAudioinfo = COS_Malloc(sizeof(audInfoStruct));
			
				//if(Audio_GetDetailsInfo(g_music_vars->location.file_entry,&g_curaudioinfo,g_currentMusicType,g_comval->langid))
				if(mediaSendCommand(MC_GETFILEINFO, pAudioinfo))
				{
					g_BitRate = pAudioinfo->bitRate / 1000;
					TotalTime =  pAudioinfo->time;
					SetStopToPlayInterface();
					need_draw = TRUE;
				}
			
				COS_Free(pAudioinfo);
			}
			else
#endif //  APP_SUPPORT_LCD==1
				mediaSendCommand(MC_GETTOTALTIME, &TotalTime);
		}
	}/* while(1) */
	
}


INT32 MUSIC_HandleKey(UINT32 key)
{
	UINT16 param;
	INT32 result = 0;
	INT8 *string;
	switch(key)
	{
#if APP_SUPPORT_MENU==1
	case MUSIC_DELETE_FILE:
		{
			return Delete_file();
		}
		break;
#endif
	case AP_KEY_MODE | AP_KEY_PRESS: //短按 MODE +
#if APP_SUPPORT_MENU==1
		if(AP_Support_MENU())
		{
			g_result = GUI_Display_Menu(GUI_MENU_MUSIC, music_menu_callback);
			{
				if(g_result == RESULT_IGNORE)
				{
					g_result = 0;
				}
				if (DealFilePlayEnd()) //处理播放到文件尾的情况
				{
					return g_result;
				}
				if (g_result != 0)
				{
					return g_result;
				}
			}
			SetFullInterface();
		}
		else
#endif
		{
#if APP_SUPPORT_USB
			if(g_music_vars->location.disk == FS_DEV_TYPE_TFLASH)
			{
				g_music_vars->location.disk = FS_DEV_TYPE_USBDISK;
				if (!MountDisk(g_music_vars->location.disk))
				{
					return RESULT_MAIN;
				}
				else
				{
					fselInit(FSEL_TYPE_MUSIC, FSEL_ALL_SEQUENCE, FSEL_TYPE_COMMONDIR, g_music_vars->location.disk);
					return RESULT_UI_PLAY;
				}
			}
			else
#endif
			{
				if(gpio_detect_linein() == 1)
				{
					return RESULT_LINE_IN;
				}
				else
				{
					return RESULT_RADIO;
				}
				return RESULT_MAIN;
			}
		}
		
		need_draw = TRUE;
		break;
		
#if APP_SUPPORT_MENU==1
	case MUSIC_SELECT_FILE: // select file
		if(PlayStatus == NormalPlay) // play
		{
			mediaSendCommand(MC_STOP, fast_fade);
			mediaSendCommand(MC_CLOSE, 0);
			CaltimeFlag = TRUE;
		}
		else
		{
			CaltimeFlag = FALSE;    //等待计时间
		}
		GUI_Directory(&g_music_vars->location, DIRECTORY_MUSIC, 0);
		UpdateFileSelector();
		MUSIC_SelectAudio(Music_Cur, Stop_key);
		WaitForCaltimeCNT = 0;
		SetFullInterface();
		need_draw = TRUE;
		return RESULT_UI_PLAY;
		break;
#endif
		
		//case AP_KEY_MODE | AP_KEY_HOLD:
		//if(AP_Support_MENU())
		//return RESULT_MAIN;
		// else close fm out
#if APP_SUPPORT_FM_OUT
	case MUSIC_CLOSE_FMOUT:
		if(g_music_vars->fm_out)
		{
			FM_OutPut(0xfe, NULL, &g_music_vars->fm_out);  // close fm
			mediaSendCommand(MC_SETVOLUME, g_music_vars->volume);
			UpDisplayFlag |= up_musicfmout;
			need_draw = TRUE;
		}
		break;
		
	case MUSIC_SETUP_FMOUT:
		FM_OutPut(0, &g_music_vars->freq, &g_music_vars->fm_out);
		SetFullInterface();
		need_draw = TRUE;
#endif
		break;
		
	case AP_KEY_EQ | AP_KEY_PRESS:
		if (PlayStatus == NormalPlay)
		{
			g_music_vars->eq_type = (g_music_vars->eq_type + 1) % max_eq;
			mediaSendCommand(MC_SETEQ, g_music_vars->eq_type );
			UpDisplayFlag |= up_musiceqmode;
			need_draw = TRUE;
		}
		break;
	case AP_KEY_LOOP | AP_KEY_PRESS:
		if (PlayStatus == NormalPlay)
		{
#if APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				g_music_vars->repeat_mode = (g_music_vars->repeat_mode + 1) % FSEL_DIR_END;
			}
			else
#endif
			{
				if(g_music_vars->repeat_mode == FSEL_ALL_REPEATONE)
				{
					g_music_vars->repeat_mode = FSEL_ALL_REPEATALL;
				}
				else
				{
					g_music_vars->repeat_mode = FSEL_ALL_REPEATONE;
				}
			}
			UpDisplayFlag |= up_musicloopmode;
			need_draw = TRUE;
		}
		break;
#if XDL_APP_SUPPORT_TONE_TIP==1
	case 0xaaaa1111:	// xundon
		if(PlayStatus == NormalPlay)
		{
			hal_HstSendEvent(SYS_EVENT, 0x11551111);
			mediaSendCommand(MC_GETTIME, (INT32)&g_music_vars->BreakTime);			
			mediaSendCommand(MC_STOP, slow_fade);
			mediaSendCommand(MC_CLOSE, 0);
			SetStopToPlayInterface();
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
			SetPAVolume(g_music_vars->volume);
			StartIntro(FALSE);
			UpDisplayFlag |= (up_musictotaltime | up_musicbitrate);
			need_draw = TRUE;
			return RESULT_UI_PLAY;
		}
		else
		{
			hal_HstSendEvent(SYS_EVENT, 0x11551122);
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
			SetPAVolume(g_music_vars->volume);
		}
		break;
#endif
		
	case AP_KEY_VOLSUB | AP_KEY_DOWN:
	case AP_KEY_VOLADD | AP_KEY_DOWN:
	case AP_KEY_VOLSUB | AP_KEY_HOLD:
	case AP_KEY_VOLADD | AP_KEY_HOLD:
		if(!GetToneStatus())
		{
			result = ChangeVolume(key);
		}
		break;
		
	case AP_MSG_USB_PLUGIN:
	case AP_MSG_USB_PULLOUT:
	case AP_MSG_SD_IN:
	case AP_MSG_SD_OUT:
		result = HandlePlugandPull(key);
		break;
#if APP_SUPPORT_MENU==1
	case MUSIC_SET_EQ:
		GUI_GetMenuResult(&param);
		g_music_vars->eq_type = param;
		mediaSendCommand(MC_SETEQ, g_music_vars->eq_type );
		UpDisplayFlag |= up_musiceqmode;
		need_draw = TRUE;
		break;
	case MUSIC_SET_LOOP:
		GUI_GetMenuResult(&param);
		g_music_vars->repeat_mode = param;
		UpDisplayFlag |= up_musicloopmode;
		need_draw = TRUE;
		break;
		
	case MUSIC_SET_RPT_TIMES:
		slider.max = 9;
		slider.min = 1;
		slider.step = 1;
		
		if (g_music_vars->replaytimes < 1) //检测复读次数是否超出范围
		{
			g_music_vars->replaytimes = 1;
		}
		if (g_music_vars->replaytimes > 9)
		{
			g_music_vars->replaytimes = 9;
		}
		slider.value = g_music_vars->replaytimes;
		string = GUI_GetString(GUI_STR_REPLAYTIMES);
		GUI_ClearScreen(NULL);
		GUI_DisplayTextCenter(0, string);
		
		g_result = GUI_Slider(& slider, NULL);
		ui_auto_update = TRUE;
		g_music_vars->replaytimes = slider.value;
		if (g_result != 0)
		{
			return g_result;
		}
		SetFullInterface();
		need_draw = TRUE;
		break;
		
	case MUSIC_SET_RPT_PEROID:
		slider.max = 99;
		slider.min = 5;
		slider.step = 10;
		
		if (g_music_vars->maxReplayTime < 5) //检测复读间隔是否超出范围
		{
			g_music_vars->maxReplayTime = 5;
		}
		if (g_music_vars->maxReplayTime > 99)
		{
			g_music_vars->maxReplayTime = 99;
		}
		slider.value = g_music_vars->maxReplayTime;
		
		string = GUI_GetString(GUI_STR_REPLAYGAP);
		GUI_ClearScreen(NULL);
		GUI_DisplayTextCenter(0, string);
		
		g_result = GUI_Slider(& slider, NULL);
		ui_auto_update = TRUE;
		g_music_vars->maxReplayTime = slider.value;
		if (g_result != 0)
		{
			return g_result;
		}
		SetFullInterface();
		need_draw = TRUE;
		break;
		
#if APP_SUPPORT_LYRIC==1//支持歌词显示功能
	case MUSIC_OPEN_LYRIC:
		g_music_vars->show_lyric = TRUE;
		g_lyricflag = lyricLoad(g_music_vars->location.file_entry);
		if(g_lyricflag)
		{
			lyric_next_time = 0;
			memset(lyric_buff, 0, AUDPLY_LYRIC_MAX_LINE_SIZE);
		}
		break;
	case MUSIC_CLOSE_LYRIC:
		g_music_vars->show_lyric = FALSE;
		g_lyricflag = FALSE;
		break;
#endif
#endif // APP_SUPPORT_MENU==1
	default:
		result = MESSAGE_HandleHotkey(key);
	}
	//hal_HstSendEvent(SYS_EVENT, 0x19887900);
	//hal_HstSendEvent(SYS_EVENT, key);
	//hal_HstSendEvent(SYS_EVENT, result);
	
	if(key == EV_BT_NOTIFY_IND)
	{
		mediaSendCommand(MC_GETTIME, (INT32)&g_music_vars->BreakTime);
		mediaSendCommand(MC_STOP, slow_fade);
		mediaSendCommand(MC_CLOSE, 0);
	}
	return result;
}
#endif
