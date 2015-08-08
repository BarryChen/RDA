/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_common.h"
#include "ap_calendar.h"
#include "ap_idle.h"
#include "ap_linein.h"
#include "ap_record.h"
#include "ap_setting.h"
#include "ap_fm.h"
#include "ap_music.h"
#include "ap_bluetooth.h"
#include "ap_usbdisk.h"
#include "ap_charging.h"
#include "tm.h"
#include "MainTask.h"
#include "event.h"
#include "gpio_edrv.h"

#define READ_BUF_MAX          128
#define MAIN_ITEMCOUNT        5  /*Main Menu Nums*/
#define MAX_BOOT_LOGO_PICS    6  /*BootLogo Nums Used by User*/

#define UPDATE_TFLASH_BIN_NAME  L"tflash_update.bin"


static const UINT16 boot_logo_pic[MAX_BOOT_LOGO_PICS] = {GUI_IMG_LOGO, GUI_IMG_LOGO, GUI_IMG_LOGO, GUI_IMG_LOGO, GUI_IMG_LOGO, GUI_IMG_LOGO};

///////////////////////////////////////////////////////////////////////////////////
comval_t         *g_comval; //系统总的环境变量
slider_t         slider = {0};
//function_menu_t  menu;
//progress_t       progress;
sysconfig_t      g_sysconfig = {0};
displayconfig_t  g_displayconfig = {0};
#if APP_SUPPORT_BLUETOOTH==1
btconfig_t       g_btconfig = {0};
#endif
INT32           g_last_reslt = -1;
INT32           g_current_module = 0;

//////////////////////////////////////////////////////////////////////////////////

//add by wuxiang
extern UINT16 call_status;
extern UINT32 g_bt_call_time;

//add end
extern BOOL ui_auto_update;
extern BOOL ui_auto_select;
extern BOOL g_test_mode;
extern UINT8 g_usb_active_after_bt;
UINT32 event_detected_displaying_log;
extern UINT8 g_linein_out_event;
extern UINT8 g_usb_audio_linein;
extern UINT8 g_usb_audio_tcard;
#if APP_SUPPORT_FADE_INOUT==1
extern INT8 g_fade_volume_backup;
#endif

extern UINT32 musicplay_timer;

void APP_Test_Mode(void);


typedef INT32 (*MOD_FunCbPtr )(INT32 param);
/* Mod Functions To Entry Every Mod By Special Satus  */
static MOD_FunCbPtr currModFuncPtrs[MAX_FUNCTION_MOD];

//Main Menu Picture Ids
//static const UINT16 inactive_id[MAIN_ITEMCOUNT] = {GUI_IMG_MUSIC,  GUI_IMG_RECORD,  GUI_IMG_BT,  GUI_IMG_FM,  GUI_IMG_SYSTEM};
//static const UINT16 active_id[MAIN_ITEMCOUNT]   = {GUI_IMG_MUSICA, GUI_IMG_RECORDA, GUI_IMG_BTA, GUI_IMG_FMA, GUI_IMG_SYSTEMA};
//static const UINT16 item_str_id[MAIN_ITEMCOUNT] = {GUI_STR_MUSIC,  GUI_STR_RECORD,  GUI_STR_BT,  GUI_STR_FM,  GUI_STR_SETTING};

//应用模块ID
//static const UINT16 ap_id[MAIN_ITEMCOUNT]={RESULT_MUSIC, RESULT_RECORD_NOSTART,RESULT_BT, RESULT_RADIO, RESULT_SYSTEM};


/*********************************************************************************
* Description : 注册功能模块函数，提供外部调用
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32  APP_RegisterFunPtr(UINT8 mod, MOD_FunCbPtr cb)
{
	if(cb == NULL || mod > MAX_FUNCTION_MOD)
	{
		return -1;
	}
	else
	{
		currModFuncPtrs[mod] = cb;
	}
	return 1;
}


/*********************************************************************************
* Description : 初始化注册函数
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 APP_InitialFunPtr(void)
{
	INT32 i;
	
	for(i = 0; i <= MAX_FUNCTION_MOD; i++)
	{
		currModFuncPtrs[i] = NULL;
	}
	
	//注册各个模块的入口函数
#if APP_SUPPORT_LCD==1
	APP_RegisterFunPtr(FUN_CALENDAR,      CALENDAR_Entry);
	APP_RegisterFunPtr(FUN_SYSTEM_SETTING, SET_Entry);
	APP_RegisterFunPtr(FUN_TIMER, TIMER_Entry);
#endif
#if APP_SUPPORT_RECORD==1
	APP_RegisterFunPtr(FUN_RECORD,        RECORD_Entry);
#endif
#if APP_SUPPORT_FM==1
	APP_RegisterFunPtr(FUN_FM,            FM_Entry);
#endif
#if APP_SUPPORT_LINEIN==1
	APP_RegisterFunPtr(FUN_LINEIN,        LINEIN_Entry);
#endif
	APP_RegisterFunPtr(FUN_IDLE,          IDLE_Entry);
#if APP_SUPPORT_MUSIC==1
	APP_RegisterFunPtr(FUN_MUSIC,         MUSIC_Entry);
#endif
#if APP_SUPPORT_USBDEVICE==1
	APP_RegisterFunPtr(FUN_USB,           USB_Entry);
#endif
#if APP_SUPPORT_BLUETOOTH==1
	APP_RegisterFunPtr(FUN_BT,            BT_Entry);
#endif
	APP_RegisterFunPtr(FUN_CHARGING,      Charging_Entry);
	return 1;
}

/*********************************************************************************
* Description : 注册功能模块函数，提供外部调用
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 APP_ModCall(UINT8 mod, INT32 Param)
{
	hal_HstSendEvent(SYS_EVENT, 0x10140000 + mod);
	g_current_module = mod;
	if((mod < MAX_FUNCTION_MOD) && (currModFuncPtrs[mod] != NULL))
	{
		return currModFuncPtrs[mod](Param);
	}
	return -1;
}


/*********************************************************************************
* Description : 显示系统开机动画
*
* Arguments   : wait_key_up, 是否要等待按键
*
* Returns     : TRUE, 正常
        FALSE, enter testmode
*
* Notes       :
*
*********************************************************************************/
BOOL APP_DisplaySysLogo(BOOL wait_key_up)
{
	INT32 i, j;
	BOOL keydown = FALSE;
	UINT32 key;
	UINT32 prekey = 0;
	hal_HstSendEvent(SYS_EVENT, 0x19884501);
	hal_HstSendEvent(SYS_EVENT, g_displayconfig.log_image_time);
	hal_HstSendEvent(SYS_EVENT, g_displayconfig.log_image_count);
	if(g_displayconfig.log_image_count == 0 && g_displayconfig.log_image_time == 0)
	{
		for(i = 0; i < 1; i++)
			for(j = 0; j < 1;)
			{
				key = MESSAGE_Wait();
				//app_trace(APP_MAIN_TRC, "Display Log receive event 0x%x,prekey=0x%x, keydown=%d", key, prekey, keydown);
				switch(key)
				{
				case AP_MSG_RTC:
				case AP_MSG_CHARGING:
					j++;
					break;
				default:
					if((key & 0xffff) == AP_KEY_DOWN)
					{
						if(prekey != 0 || key != (AP_KEY_DOWN | AP_KEY_POWER))
						{
							keydown = TRUE;
						}
						prekey = key;
					}
					if(keydown && (key & 0xffff) == AP_KEY_UP)
					{
						return TRUE;
					}
					
					key = MESSAGE_HandleHotkey(key);//handle unusual usb/t-card/line-in pluging event
					if(key != 0)
					{
						event_detected_displaying_log = key;
						hal_HstSendEvent(SYS_EVENT, 0x13063002);
						hal_HstSendEvent(SYS_EVENT, key);
					}
					
					break;
				}
			}
	}
	
	for(i = 0; i < g_displayconfig.log_image_count; i++)
	{
#if APP_SUPPORT_RGBLCD==1
		if(AP_Support_LCD())
		{
			GUI_ClearScreen(NULL);//清全屏幕
			GUI_ResShowImage(GUI_IMG_LOGO, i, 0, 40); //g_displayconfig.log_x, g_displayconfig.log_y);/*BootLogo*/
			GUI_UpdateScreen(NULL);
		}
#elif APP_SUPPORT_LCD==1
		if(AP_Support_LCD())
		{
			GUI_ClearScreen(NULL);//清全屏幕
			GUI_ResShowImage(GUI_IMG_LOGO, i, 0, 8); //g_displayconfig.log_x, g_displayconfig.log_y);/*BootLogo*/
			GUI_UpdateScreen(NULL);
		}
#elif APP_SUPPORT_LED8S==1
		GUI_ClearScreen(NULL);
		GUI_ResShowPic(0xffff, 0, 0);
		GUI_DisplayText(0, 0, "8888");
		GUI_UpdateScreen(NULL);
#endif
		hal_HstSendEvent(SYS_EVENT, 0x19884500);
		hal_HstSendEvent(SYS_EVENT, g_displayconfig.log_image_time);
		for(j = 0; j < g_displayconfig.log_image_time; )
		{
			key = MESSAGE_Wait();
			//app_trace(APP_MAIN_TRC, "Display Log receive event 0x%x,prekey=0x%x, keydown=%d", key, prekey, keydown);
			switch(key)
			{
			case AP_MSG_RTC:
			case AP_MSG_CHARGING:
				j++;
				break;
			default:
				if((key & 0xffff) == AP_KEY_DOWN)
				{
					if(prekey != 0 || key != (AP_KEY_DOWN | AP_KEY_POWER))
					{
						keydown = TRUE;
					}
					prekey = key;
				}
				if(keydown && (key & 0xffff) == AP_KEY_UP)
				{
					return TRUE;
				}
				break;
			}
		}
	}
	
	return TRUE;
}


/*********************************************************************************
* Description : 初始化系统设置变量
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
void APP_ReadComPara(void)
{
	//DSM_ReadUserData();
	
	//NVRAMRead(g_comval, VM_SYSTEM, sizeof(g_comval));
	g_comval = (comval_t*)NVRAMGetData(VM_SYSTEM, sizeof(comval_t));
	hal_HstSendEvent(SYS_EVENT, 0x19880900);
	if(g_comval->magic != MAGIC_COMVAL)
	{
		hal_HstSendEvent(SYS_EVENT, 0x19880901);
		if(!LoadFactorySetting(g_comval, sizeof(g_comval)))
		{
			g_comval->DisplayContrast = 5;
			g_comval->langid    = GUI_LANG_SM_CHINESE;
			g_comval->LightTime = 3;
			//g_comval->ReplayMode = 0;
			//g_comval->SleepTime = 0;
			g_comval->StandbyTime = 30;
			//g_comval->BatteryType = BATT_TYPE_ALKALINE;
			//g_comval->FMBuildInFlag = 0;
			//g_comval->RecordType = 0;
			//g_comval->BLightColor = 3;
			g_comval->BackLight = 5;
			g_comval->Volume = 8; // default volume
			
			g_comval->music_cfg.loop_mode = 0;
			g_comval->music_cfg.eq_mode = 0;
			g_comval->music_cfg.repeat_count = 3;
			g_comval->music_cfg.repeat_time = 30;
			
#if APP_SUPPORT_FM==1           /*Surport fm*/
			g_comval->fm_value.fm_band = 0;
			g_comval->fm_value.fm_sendfreq = 1000;
#endif
		}
		hal_HstSendEvent(SYS_EVENT, 0x19880902);
		g_comval->magic     = MAGIC_COMVAL;
		// no need save data
		//NVRAMWrite(g_comval, VM_SYSTEM,sizeof(g_comval));
		
		//g_test_mode = 1;
	}
	
	if(!ReadConfigData(&g_sysconfig, CFG_SYSTEM, sizeof(g_sysconfig)))
	{
		g_sysconfig.AppSupportFlag = AP_SUPPORT_RECORD | (AP_SUPPORT_FM * APP_SUPPORT_FM) | (AP_SUPPORT_FMREC * APP_SUPPORT_FM_RECORD) | (AP_SUPPORT_LCD * APP_SUPPORT_LCD) | (AP_SUPPORT_MENU * APP_SUPPORT_MENU) | (AP_SUPPORT_LED * APP_SUPPORT_LED);
	}
#if APP_SUPPORT_RGBLCD==1
	if(AP_Support_LCD() && !ReadConfigData(&g_displayconfig, CFG_DISPLAY, sizeof(g_displayconfig)))
	{
		g_displayconfig.lcd_heigth = 128;
		g_displayconfig.lcd_width = 160;
		//g_displayconfig.log_x = 0;
		//g_displayconfig.log_y = 8;
		//g_displayconfig.usb_x = 0;
		//g_displayconfig.usb_y = 16;
		
		g_displayconfig.big_num_width = 16;
		g_displayconfig.big_colon_width = 9;
		g_displayconfig.small_num_width = 6;
		g_displayconfig.small_colon_width = 4;
		
		//g_displayconfig.num_key_img = GUI_IMG_BIG_NUM;
		//g_displayconfig.num_key_x = 32;
		//g_displayconfig.num_key_y = 16;
		
		g_displayconfig.position_batt = POSITION_BATT;
		g_displayconfig.message_y = 48;
	}
#elif APP_SUPPORT_LCD==1
	if(AP_Support_LCD() && !ReadConfigData(&g_displayconfig, CFG_DISPLAY, sizeof(g_displayconfig)))
	{
		g_displayconfig.lcd_heigth = 64;
		g_displayconfig.lcd_width = 128;
		//g_displayconfig.log_x = 0;
		//g_displayconfig.log_y = 8;
		//g_displayconfig.usb_x = 0;
		//g_displayconfig.usb_y = 16;
	
		g_displayconfig.big_num_width = 16;
		g_displayconfig.big_colon_width = 9;
		g_displayconfig.small_num_width = 6;
		g_displayconfig.small_colon_width = 4;
	
		//g_displayconfig.num_key_img = GUI_IMG_BIG_NUM;
		//g_displayconfig.num_key_x = 32;
		//g_displayconfig.num_key_y = 16;
	
		g_displayconfig.position_batt = POSITION_BATT;
		g_displayconfig.message_y = 24;
	}
#endif
#if APP_SUPPORT_BLUETOOTH==1     /*Surport bt*/
	if(!ReadConfigData(&g_btconfig, CFG_BLUETOOTH, sizeof(g_btconfig)))
	{
		strcpy(g_btconfig.local_name, "RDA MP3 BT"); // bluetooth name
		strcpy(g_btconfig.password, "0000");         // bluetooth password
	}
#endif
	//SetBackLightColor(g_comval->BLightColor);        //设置背光颜色。
	
	SetBackLight(g_comval->BackLight);
	
	SetContrast(g_comval->DisplayContrast);           //设置屏幕的对比度
	
	MESSAGE_Initial(g_comval);
	
}


INT32 APP_StandBy(void)
{
	UINT32 key;
	INT wakeup = 0;
	
	GUI_DisplayMessage(0, GUI_STR_STANDBY, NULL, GUI_MSG_FLAG_WAIT);
	
	if(AP_Support_LCD())
	{
		GUI_ClearScreen(NULL);//清全屏幕
		GUI_UpdateScreen(NULL);
		CloseBacklight();
	}
	ui_auto_update = FALSE;


	
	//APP_Sleep();
#ifdef XDL_35uA_POWEROFF//warkey 2.1  //进入超低电流关机
	rfd_XcvRegForceWrite32k();
#endif
	while(1)
	{
		key = MESSAGE_Wait();
		if(key == AP_MSG_RTC && GetBattery() >= 0)
		{
			if(tgt_GetKeyCode(0) == KEY_POWER)
			{
				app_trace(APP_MAIN_TRC, "APP_StandBy call DM_DeviceSwithOff");
				DM_DeviceSwithOff();
			}
			else // 如果power键长高，无法关机，进入睡眠
			{
				app_trace(APP_MAIN_TRC, "APP_StandBy call APP_Sleep");
				APP_Sleep();
			}
			continue;
		}
		if(key == (AP_KEY_POWER | AP_KEY_PRESS))
		{
			wakeup = 1;
		}
		if(key == (AP_KEY_PLAY | AP_KEY_HOLD))
			if(MESSAGE_IsHold())
			{
				wakeup = 1;
			}
		if((key & 0xffff) == AP_KEY_UP && wakeup)
		{
			key = RESULT_POWERON;
			break;
		}
		if(key == AP_MSG_ALARM)
		{
			key = RESULT_ALARM;
			break;
		}
	}
	APP_Wakeup();
	return key;
}

#if APP_SUPPORT_CALIB_KEY==1
INT32 APP_CalibKey(void)
{
	UINT32 key;
	
#ifdef _USED_TSC_KEY_
	GUI_DisplayMessage(0, GUI_STR_CALIBKEY, NULL, GUI_MSG_FLAG_DISPLAY);
	while(1)
	{
		tsd_Key_Calib(0); // start
		key = MESSAGE_Wait();
		if((key & 0xffff) == AP_KEY_UP)
		{
			break;
		}
	}
	
	LED_SetPattern(GUI_LED_CALIB_KEY1, LED_LOOP_INFINITE);
	GUI_DisplayMessage(0, GUI_STR_CALIBKEY_1, NULL, GUI_MSG_FLAG_DISPLAY);
	while(1)
	{
		key = MESSAGE_Wait();
		if((key & 0xffff) == AP_KEY_UP)
		{
			break;
		}
	}
	tsd_Key_Calib(1); // first key press
	LED_SetPattern(GUI_LED_CALIB_KEYN, LED_LOOP_INFINITE);
	GUI_DisplayMessage(0, GUI_STR_CALIBKEY_N, NULL, GUI_MSG_FLAG_DISPLAY);
	while(1)
	{
		key = MESSAGE_Wait();
		if((key & 0xffff) == AP_KEY_UP)
		{
			break;
		}
	}
	key = tsd_Key_Calib(2); // last key press
	if(key == 0)
	{
		LED_SetPattern(GUI_LED_NONE, 0);
		GUI_DisplayMessage(0, GUI_STR_CALIBKEY_SUCC, NULL, GUI_MSG_FLAG_WAIT);
	}
	else
	{
		GUI_DisplayMessage(0, GUI_STR_CALIBKEY_FAIL, NULL, GUI_MSG_FLAG_WAIT);
	}
#endif
	return 0;
}
#endif

BOOL APP_Read_Sys_Config(void)
{
#if 0
	//////////////////////Set Para Through File//////////////////////////
	INT32 config_file_handle = 0;
	UINT8 buf[READ_BUF_MAX];
	if(MountDisk(FS_DEV_TYPE_TFLASH))
	{
		config_file_handle = FS_Open(L"Config.txt", FS_O_RDONLY, 0);
	}
#if APP_SUPPORT_USB
	else if(MountDisk(FS_DEV_TYPE_USBDISK))
	{
		config_file_handle = FS_Open("Config.txt", FS_O_RDONLY, 0);
	}
#endif
	else
	{
		return FALSE;
	}
	
	if(config_file_handle < 0)
	{
		return FALSE;
	}
	
	///////Scan Param For  Setting/////
	memset(buf, 0, sizeof(buf));
	
	buf[READ_BUF_MAX - 1] = '\n';
	
	if(FS_Read(config_file_handle, buf, (sizeof(buf) - 1)))
	{
		//kval_t kval;
		UINT16 uYear, uMonth, uDay, uHour, uMinute, uSecond, position;
		
		/*First1:parse DATA*/
		{
			INT8 *str = "DATE:" ;
			position =  strstr(buf, str) - buf;
			position += strlen(str);
			uYear	= (buf[position] - '0') * 1000 + (buf[position + 1] - '0') * 100 + (buf[position + 2] - '0') * 10 + (buf[position + 3] - '0'); /*buf[0-3],Year... etc 2011 */
			uMonth	= (buf[position + 4] - '0') * 10 + (buf[position + 5] - '0'); /*buf[4-5],Month... etc 11 */
			uDay	    = (buf[position + 6] - '0') * 10 + (buf[position + 7] - '0'); /*buf[6-7],Day...	etc 11 */
			str = "TIME:" ;
			position =  strstr(buf, str) - buf;
			position += strlen(str);
			uHour	= (buf[position] - '0') * 10  + (buf[position + 1] - '0'); /*buf[8-9],uHour... etc 11 */
			uMinute = (buf[position + 2] - '0') * 10 + (buf[position + 3] - '0'); /*buf[10-11],uMinute... etc 11 */
			uSecond = (buf[position + 4] - '0') * 10 + (buf[position + 5] - '0'); /*buf[12-13],uSecond... etc 21 */
		}
		
		app_trace(APP_MAIN_TRC, "[APP_Read_Sys_Config]:%d,%d,%d,%d,%d,%d\r\n", uYear, uMonth, uDay, uHour, uMinute, uSecond);
		
		
		/*Get Band,FMFREQ*/
#if APP_SUPPORT_FM==1           /*Surport fm*/
		{
			INT8 *str = "BAND:" ;
			position =  strstr(buf, str) - buf;
			position += strlen(str);
			g_comval->fm_value.fm_band = (buf[position] - '0');
			str = "FMSEND:" ;
			position =  strstr(buf, str) - buf;
			position += strlen(str);
			g_comval->fm_value.fm_sendfreq = (buf[position] - '0') * 1000 + (buf[position + 1] - '0') * 100 + (buf[position + 2] - '0') * 10 + (buf[position + 3] - '0');
		}
#endif
		/*Get Loop,Eq*/
		{
			INT8 *str = "LOOP:" ;
			position =	strstr(buf, str) - buf;
			position += strlen(str);
			g_comval->music_cfg.loop_mode = (buf[position] - '0');
			str = "EQ:" ;
			position =	strstr(buf, str) - buf;
			position += strlen(str);
			g_comval->music_cfg.eq_mode   = (buf[position] - '0');
		}
		/*Get repeat ,times*/
		{
			INT8 *str = "REPEAT:" ;
			position =	strstr(buf, str) - buf;
			position += strlen(str);
			g_comval->music_cfg.repeat_count = (buf[position] - '0');
			str = "REPETIME:" ;
			position =	strstr(buf, str) - buf;
			position += strlen(str);
			g_comval->music_cfg.repeat_time	 = (buf[position] - '0') * 10 + (buf[position + 1] - '0');
		}
		
		//将systemtime 和 gwtime写回到VRAM中
		//NVRAMRead(&kval, VM_KERNEL, sizeof(kval_t));
		//kval.systemtime = ((uYear-2000)*365+uMonth*30+uDay)*24*60+uHour*60+uMinute;	 //全局变量
		//NVRAMWrite(&kval, VM_KERNEL, sizeof(kval_t));
		
	}
	
	FS_Close(config_file_handle);
#endif
	return TRUE;
}

INT32 main_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		return g_last_reslt;
	}
	return FALSE;
}


TASK_ENTRY BAL_MainTaskEntry(void *pData)
{
	INT8   IsUDisk = 0;
	BOOL   need_draw = TRUE;
	INT32  active = 0;   //当前活动的项目
	INT32  result = APP_DEFAULT_RESULT;
	INT32  ret    = RESULT_NULL;
	INT32  bt_active_ret = RESULT_NULL;
	TM_SYSTEMTIME systime;
	extern BOOL g_usb_connected;
	
	// dump version and date
	//hal_HstSendEvent(SYS_EVENT,0x11223344);
	//hal_HstSendEvent(SYS_EVENT,GetPlatformVersion());
	//hal_HstSendEvent(SYS_EVENT,GetPlatformRevision());
	//hal_HstSendEvent(SYS_EVENT,GetPlatformBuildDate());
	//hal_HstSendEvent(SYS_EVENT,AP_GetVersion());
	//hal_HstSendEvent(SYS_EVENT,AP_GetBuildDate());
	
	APP_ReadComPara();	//读取系统的全局变量。可以放在os初始化的过程中做
#if XDL_APP_SUPPORT_LOWBAT_DETECT==1//warkey 2.1
	StartBatTimer();
#endif
	gui_load_resource(g_comval->langid);
	
	LED_SetPattern(GUI_LED_POWERON, 1);
	
#if APP_SUPPORT_LCD==1
	if(AP_Support_LCD())
	{
		lcdd_Open();
		GUI_Initialise(g_displayconfig.lcd_width, g_displayconfig.lcd_heigth);
		GUI_SetTextColor(g_displayconfig.font_color);
		GUI_SetBackColor(g_displayconfig.back_color);
	}
#endif
	
	if(g_test_mode)
	{
		NVRAMWriteData();
		APP_Test_Mode();
	}
	
	APP_DisplaySysLogo(FALSE);
	AP_GetBuildDate();//force link date time functions.
	
	// check systemtime
	TM_GetSystemTime(&systime);
	if(systime.uHour >= 24 || systime.uDay == 0 || systime.uYear > 2050) // invalid date time
	{
		systime.uYear = 2012;
		systime.uMonth = 8;
		systime.uDay = 15;
		systime.uHour = 12;
		systime.uMinute = 30;
		TM_SetSystemTime(&systime);
	}
#if APP_SUPPORT_LCD==1
	TIMER_SetAlarm(1);
#endif
	
	hal_HstSendEvent(SYS_EVENT, 0x09250001);
	
	APP_InitialFunPtr(); //初始化各个模块的回调函数，最后通过宏定义来实现开关
	
	APP_Read_Sys_Config();
	LED_SetPattern(GUI_LED_NONE, 1);
	
	hal_HstSendEvent(BOOT_EVENT, 0x09558000);
	
#if APP_SUPPORT_LCD==0  //added for T_card update without LCD. Search TF card, if there is a file with the name "tflash_update.bin", then do the update.
#ifdef MCD_TFCARD_SUPPORT
	{
		INT32 file = -1, card_ok = -1;
		UINT32 cur_data = AP_GetBuildDate();
		UINT32 cur_time = AP_GetBuildTime();
		boolean needupdate = FALSE;
		
		hal_HstSendEvent(BOOT_EVENT, 0x98880001);
		hal_HstSendEvent(BOOT_EVENT, cur_data);
		hal_HstSendEvent(BOOT_EVENT, cur_time);
		hal_HstSendEvent(BOOT_EVENT, 0x98880001);
		
		
		card_ok = FS_MountDevice(FS_DEV_TYPE_TFLASH);
		hal_HstSendEvent(BOOT_EVENT, card_ok);
		if(card_ok == ERR_SUCCESS)
		{
			FS_FILE_ATTR file_attr;
			TM_FILETIME sFileTime;
			TM_SYSTEMTIME sSysTm;
			UINT32 bin_createDate = 0;
			UINT32 bin_createTime = 0;
			UINT32 offset = 0;
			
			file = FS_Open(UPDATE_TFLASH_BIN_NAME, FS_O_RDWR, 0);
			
			hal_HstSendEvent(BOOT_EVENT, 0x09550000);
			hal_HstSendEvent(BOOT_EVENT, file);
			hal_HstSendEvent(BOOT_EVENT, 0x09550000);
			
#if 1
			FS_Seek(file, 0xc, FS_SEEK_SET);
			FS_Read(file, &offset, 4);
			FS_Seek(file, ((offset & 0x00ffffff)) - 0x10, FS_SEEK_SET);
			FS_Read(file, &bin_createDate, 4);
			FS_Read(file, &bin_createTime, 4);
			
			hal_HstSendEvent(BOOT_EVENT, 0x98880002);
			hal_HstSendEvent(BOOT_EVENT, bin_createDate);
			hal_HstSendEvent(BOOT_EVENT, bin_createTime);
			hal_HstSendEvent(BOOT_EVENT, 0x98880002);
#else
			if(FS_GetFileAttrByHandle(file, &file_attr) == 0)
			{
				sFileTime.DateTime = file_attr.i_mtime;
			
				TM_FileTimeToSystemTime(sFileTime, &sSysTm);
				bin_createDate = ((sSysTm.uYear) * 10000 + ((sSysTm.uMonth) * 100) + sSysTm.uDay);
				bin_createTime = sSysTm.uHour * 10000 + sSysTm.uMinute * 100 + sSysTm.uSecond;
			
				hal_HstSendEvent(BOOT_EVENT, 0x98880002);
				hal_HstSendEvent(BOOT_EVENT, bin_createDate);
				hal_HstSendEvent(BOOT_EVENT, bin_createTime);
				hal_HstSendEvent(BOOT_EVENT, 0x98880002);
			}
#endif
			
			//if((cur_data<bin_createDate) ||
			//(cur_data==bin_createDate && cur_time<bin_createTime))
			if((cur_data != bin_createDate) || (cur_time != bin_createTime)) //只要不是同一软件就允许升级
			{
				needupdate = TRUE;
			}
			
			hal_HstSendEvent(BOOT_EVENT, needupdate);
			hal_HstSendEvent(BOOT_EVENT, 0x98880008);
			
			if(file >= 0 && needupdate)
			{
				Upgrade_Flash(file);
				
				RestartSystem();
			}
			else
			{
				FS_Close(file);
			}
		}
	}
#endif
#endif
	
	
	g_last_reslt = result;
	if(GetUsbCableStatus())
	{
		hal_HstSendEvent(APP_EVENT, 0x13113001);
		result = RESULT_UDISK;
	}
	if(gpio_detect_linein() == GPIO_DETECTED)
	{
		hal_HstSendEvent(APP_EVENT, 0x13113002);
		result = RESULT_LINE_IN;
	}
	//    IsUDisk = g_comval->Onlinedev;
	
#ifdef SUPPORT_POWERON_ENTER_CHARGE_AP//warkey 2.1 
	app_trace(APP_MAIN_TRC, "GetBattery:%d", GetBattery());
	if(-1 == GetBattery() && g_usb_connected == 0)
	{
		//                  hal_HstSendEvent(SYS_EVENT, 0x13062707);
		hal_HstSendEvent(APP_EVENT, 0x13113003);
		result = Charging_Entry();
	}
#endif
	while(1)
	{
		if(result==RESULT_STANDBY)
		{
			LED_SetPattern(GUI_LED_NONE, 0xde);
		}
		else
		{
			LED_SetPattern(GUI_LED_NONE, 1);
		}
		
		hal_HstSendEvent(SYS_EVENT, 0x09250002);
		hal_HstSendEvent(SYS_EVENT, result);
		
		if(result == RESULT_NULL || result == RESULT_MAIN)
		{
#if APP_SUPPORT_MENU==1
			if(AP_Support_MENU())
			{
				ui_auto_select = FALSE;
				result = GUI_Display_Menu(GUI_MENU_MAIN, main_menu_callback);
				
				g_last_reslt        = RESULT_NULL;
				if(result == RESULT_TIMEOUT || result == RESULT_IGNORE || result == RESULT_NULL)
				{
					result = RESULT_IDLE; // RESULT_CALENDAR;
				}
			}
			else
#endif
			{
				if(RESULT_STANDBY != result)
				{
					result = g_last_reslt;
				}
				switch(result)
				{
				case RESULT_STANDBY:
					break;
				case RESULT_BT:
#if APP_SUPPORT_MUSIC==1
					result = RESULT_MUSIC;
					break;
				case RESULT_MUSIC:
#endif
#if APP_SUPPORT_FM || APP_SUPPORT_LINEIN
					//没有Line in时才进入FM 2012-9-27
					if(gpio_detect_linein() == GPIO_DETECTED)
					{
				#if APP_SUPPORT_LINEIN ==1	
						result = RESULT_LINE_IN;
						break;
				#else
						result = RESULT_RADIO;
						break;						
				#endif
					}
					else
					{
				#if APP_SUPPORT_FM==1
						result = RESULT_RADIO;
						break;
				#endif
					}
				case RESULT_LINE_IN:
				case RESULT_RADIO:
#endif
				
#if APP_SUPPORT_RECORD
					if(AP_Support_RECORD())
					{
						result = RESULT_RECORD_NOSTART;
						break;
					}
				case RESULT_RECORD_NOSTART:
#endif
#if APP_SUPPORT_BLUETOOTH
					result = RESULT_BT;
					break;
#endif
				default:
					result = APP_DEFAULT_RESULT;
					break;
				}
			}
		}
		else if(result == RESULT_POWERON)
		{
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);
				GUI_UpdateScreen(NULL);
			}
			LED_SetPattern(GUI_LED_POWERON, 1);
			
			APP_DisplaySysLogo(TRUE);
			LED_SetPattern(GUI_LED_NONE, 1);
			
			if (g_comval->LightTime == 0)            //每次开机判断一次！解决Bug Report:27
			{
				CloseBacklight();
			}
			else
			{
				OpenBacklight();
			}
			
			//APP_DUMP("g_last_reslt:", g_last_reslt);
			
			result = g_last_reslt;
			if(GetUsbCableStatus())
			{
				hal_HstSendEvent(APP_EVENT, 0x07021549);
				result = RESULT_UDISK;
			}
			if(gpio_detect_linein() == GPIO_DETECTED)
			{
				hal_HstSendEvent(APP_EVENT, 0x07021550);
				result = RESULT_LINE_IN;
			}
			need_draw = TRUE;
		}
		else        //boot with param or result != 0
		{
			switch( result)
			{
#if APP_SUPPORT_MUSIC==1
			case RESULT_MUSIC:
				hal_HstSendEvent(SYS_EVENT, 0x20120000);
				ret = APP_ModCall(FUN_MUSIC, 0);
				g_last_reslt = result;
				result = ret;
#if APP_SUPPORT_FADE_INOUT==1
				if(g_fade_volume_backup != -1)
				{
					//                    hal_HstSendEvent(SYS_EVENT, 0x13101701);
					//                    hal_HstSendEvent(SYS_EVENT, g_fade_volume_backup);
					SetPAVolume(g_fade_volume_backup);
					g_fade_volume_backup = -1;
				}
#endif
				if(g_usb_audio_tcard&&GetUsbCableStatus())//plug in tcard when usb audio is running
				{
					result = RESULT_UDISK;
				}
				hal_HstSendEvent(SYS_EVENT, 0x20124444);
				hal_HstSendEvent(SYS_EVENT, g_last_reslt);
				hal_HstSendEvent(SYS_EVENT, result);
				break;
#endif
			case RESULT_RADIO:
				hal_HstSendEvent(SYS_EVENT, 0x20120033);
				ret = APP_ModCall(FUN_FM, 0);
				hal_HstSendEvent(SYS_EVENT, 0x20120044);
				g_last_reslt = result;
				result = ret;
				hal_HstSendEvent(SYS_EVENT, 0x20123001);
				break;
				
			case RESULT_RECORD_START:    //进入录音
			case RESULT_RECORD_NOSTART:
			case RESULT_FMREC_START:
			case RESULT_FMREC_NOSTART:
				ret = APP_ModCall(FUN_RECORD, result); //传入result,以判断是否直接开始FM录音?
				g_last_reslt = result;
				result = ret;
				break;
				
			case RESULT_RECORD_SWITCH:
				if(g_last_reslt == RESULT_RECORD_START || g_last_reslt == RESULT_RECORD_NOSTART)
				{
					result = RESULT_FMREC_NOSTART;
				}
				else if(g_last_reslt == RESULT_FMREC_START || g_last_reslt == RESULT_FMREC_NOSTART)
				{
					result = RESULT_RECORD_NOSTART;
				}
				ret = APP_ModCall(FUN_RECORD, result); //传入result,以判断是否直接开始FM录音?
				g_last_reslt = result;
				result = ret;
				break;
			case RESULT_CHARGING:
				result = APP_ModCall(FUN_CHARGING, 0);
				break;
			case RESULT_IDLE:
				//APP_DUMP("g_last_reslt:", g_last_reslt);
				result = APP_ModCall(FUN_IDLE, 0);
				break;
			case RESULT_LINE_IN:
				hal_HstSendEvent(SYS_EVENT, 0x20128888);
				
				if(event_detected_displaying_log == RESULT_LINE_IN)
				{
					event_detected_displaying_log = 0;
				}
				
				result = APP_ModCall(FUN_LINEIN, 0);
				hal_HstSendEvent(SYS_EVENT, 0x20129999);
				hal_HstSendEvent(SYS_EVENT, g_last_reslt);
				hal_HstSendEvent(SYS_EVENT, result);
				//                if(result == RESULT_NULL || result == RESULT_MAIN)
				if(g_linein_out_event == 1)//line-in was removed
				{
					result = g_last_reslt;
					if(result == RESULT_LINE_IN)
					{
						result = APP_DEFAULT_RESULT;
					}
					g_linein_out_event = 0;
					
					if(g_usb_audio_linein&&GetUsbCableStatus())//plug in line-in when usb audio is running, so return usb audio again
					{
						result = RESULT_UDISK;
						hal_HstSendEvent(SYS_EVENT, 0x13071801);
					}
				}
				else
				{
					g_last_reslt = RESULT_LINE_IN;
				}
				break;
			case RESULT_STANDBY:
				{
#if XDL_APP_SUPPORT_TONE_TIP==1
					media_PlayInternalAudio(GUI_AUDIO_FM_SEARCH, 1, TRUE);
					while(GetToneStatus())
					{
						MESSAGE_Sleep(1);
					}
					hal_HstSendEvent(SYS_EVENT, 0x11117711);
#endif

					gpio_SetMute(TRUE);

					LED_SetPattern(GUI_LED_POWERON, 3);
					COS_Sleep(800);
					app_trace(APP_MAIN_TRC, "APP_StandBy call DM_DeviceSwithOff");					
#ifdef XDL_35uA_POWEROFF//warkey 2.1  //进入超低电流关机
					rfd_XcvRegForceWrite32k();
#endif

					DM_DeviceSwithOff();
					hal_HstSendEvent(SYS_EVENT, 0xdead8900);
					

					extern UINT8 g_light_time;
			#ifdef SUPPORT_POWERON_ENTER_CHARGE_AP//warkey 2.1
					if(-1 == GetBattery())
					{
						result = Charging_Entry();
					}
					else
			#endif
					{
#if 0//XDL_APP_SUPPORT_TONE_TIP==1
						hal_HstSendEvent(SYS_EVENT, 0x11117723);

						media_PlayInternalAudio(GUI_AUDIO_FM_SEARCH, 1, FALSE);
						while(GetToneStatus())
						{
							MESSAGE_Sleep_EXT(1);
						}
						COS_Sleep(1000);
#endif

						g_light_time = 0;
						result = APP_StandBy();
						
						if(g_comval->LightTime != 0)
						{
							g_light_time   = g_comval->LightTime * 2;
							OpenBacklight();
						}
					}
				}
				break;
#if APP_SUPPORT_CALIB_KEY==1
			case RESULT_KEYCALIB:
				result = APP_CalibKey();
				break;
#endif
			case RESULT_SYSTEM:
				ret = APP_ModCall(FUN_SYSTEM_SETTING, 0);
				g_last_reslt = result;
				result = ret;
				break;
			case RESULT_ALARM:
				ret = APP_ModCall(FUN_TIMER, 1);
				g_last_reslt = result;
				result = ret;
				break;
			case RESULT_TIMER:
				ret = APP_ModCall(FUN_TIMER, 0);
				g_last_reslt = result;
				result = ret;
				break;
			case RESULT_UDISK:
				//IsUDisk=1;
				//if(IsUDisk)
				{
					if(g_usb_active_after_bt != 0)
					{
						g_usb_active_after_bt = 0;
					}
					
					if(event_detected_displaying_log == RESULT_UDISK)
					{
						event_detected_displaying_log = 0;
					}
					hal_HstSendEvent(SYS_EVENT, 0x09250003);
					result = APP_ModCall(FUN_USB, 0);
					hal_HstSendEvent(SYS_EVENT, 0x09250004);
				}
				if(result == RESULT_NULL)
				{
					result = g_last_reslt;
					if(result == RESULT_UDISK)
					{
						result = APP_DEFAULT_RESULT;
					}
				}
				if(result == RESULT_BT_ACTIVE)
				{
					bt_active_ret = RESULT_UDISK;
				}
				//fix udisk return, drop key up
				//result = RESULT_MUSIC;
				hal_HstSendEvent(SYS_EVENT, result);
				break;
#if APP_SUPPORT_BLUETOOTH==1
			case RESULT_BT:
				hal_HstSendEvent(SYS_EVENT, 0x20120001);
				ret = APP_ModCall(FUN_BT, -1);
				g_last_reslt = result;
				result = ret;
				if(musicplay_timer)
				{
					COS_KillTimer(musicplay_timer);
					musicplay_timer = 0;
				}
				hal_HstSendEvent(SYS_EVENT, 0x20127777);
				hal_HstSendEvent(SYS_EVENT, g_last_reslt);
				hal_HstSendEvent(SYS_EVENT, result);
				break;
			case RESULT_BT_ACTIVE:
				hal_HstSendEvent(SYS_EVENT, 0x20120002);
				if(bt_active_ret != RESULT_NULL)
				{
					result = APP_ModCall(FUN_BT, bt_active_ret);
				}
				else
				{
					result = APP_ModCall(FUN_BT, g_last_reslt);
				}
				//add by wuxiang
				call_status = 0;
				g_bt_call_time = 0;
				bt_active_ret = RESULT_NULL;
				if(musicplay_timer)
				{
					COS_KillTimer(musicplay_timer);
					musicplay_timer = 0;
				}
				//add end
				hal_HstSendEvent(SYS_EVENT, 0x20126666);
				if(g_last_reslt == RESULT_ALARM)
				{
					result = RESULT_IDLE;
				}
				break;
#endif
			case RESULT_CALENDAR:
				ret = APP_ModCall(FUN_CALENDAR, 0);
				g_last_reslt = result;
				result = ret;
				break;
				
			default:
				//APP_DUMP("never run here: ", result);
				hal_HstSendEvent(SYS_EVENT, 0x20125555);
				hal_HstSendEvent(SYS_EVENT, result);
				result = RESULT_MAIN;
				break;
			}//switch(reault)
			
			//APP_ReadComPara();
			//IsUDisk = 1;      //更新Onlinedev
			need_draw = TRUE;
		}
	}//while never exit
	
}







