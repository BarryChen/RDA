/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_gui.h"
#include "ap_setting.h"
#include "ap_calendar.h"

#define  MAX_SYSMENUNUM          7
#define  MAX_LIST_NODE           15

typedef INT32 (*setting_func)(void);

extern slider_t         slider;
//extern progress_t       progress;
extern BOOL             ui_auto_select;
extern BOOL             ui_auto_update;
extern INT32            g_last_reslt;


#define CONFIG_SET_SHUTTIME             AP_MSG_USER+1
#define CONFIG_SET_BACKTIME             AP_MSG_USER+2
#define CONFIG_SET_BACKCOLOR            AP_MSG_USER+3
#define CONFIG_SET_BACKLIGHT            AP_MSG_USER+4
#define CONFIG_SET_LANGUAGE             AP_MSG_USER+5
#define CONFIG_SET_MEMORY_INFO          AP_MSG_USER+6
#define CONFIG_SET_HW_VERSION           AP_MSG_USER+7
#define CONFIG_SET_HW_UPGRADE           AP_MSG_USER+8
#define CONFIG_SYSTEM_CLOCK             AP_MSG_USER+9
#define CONFIG_SET_DATE_FORMAT          AP_MSG_USER+10
#define CONFIG_SET_HW_SYSTEMRESET       AP_MSG_USER+11


#if APP_SUPPORT_LCD==1

static INT32 menu_active;    //main menu's active number,

//days of different month
const INT8 month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

extern clndr_table_struct *g_pCalendarInfo;

static const INT8 digitalchar[] = "0123456789";

/*********************************************************************************
* Description :
*
* Arguments   :
*
* Returns     :
*
*
*********************************************************************************/
INT32 config_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		if(value == GUI_MENU_CONFIG)
		{
			return menu_active;
		}
		else if(value == GUI_MENU_CFG_LANG)
		{
			return g_comval->langid;
		}
		else
		{
			return 0;
		}
	}
	
	return 0;
}

/*********************************************************************************
* Description : 主函数
*
* Arguments   :
        param:
            0, 开始时关标停在第0项
            1, 开始时关标停在ADFU
*
* Returns     : 结果消息或0
*
* Notes       :
*
*********************************************************************************/
INT32 SET_Entry(INT32 argument)
{
	INT32 result;
	UINT32 key;
	UINT16 param;
	BOOL need_draw = TRUE;
	
	menu_active = 0;
	if(g_last_reslt == RESULT_TIMER)
	{
		menu_active = RESULT_TIMER;
	}
	
	MESSAGE_Initial(g_comval);
	
	while(1)
	{
		if(need_draw)
		{
			result = GUI_Display_Menu(GUI_MENU_CONFIG, config_menu_callback);
			if(result == RESULT_IGNORE)
			{
				return RESULT_MAIN;
			}
			if(result != 0)
			{
				return result;
			}
		}
		key = MESSAGE_Wait();
		need_draw = TRUE;
		switch( key )
		{
		case CONFIG_SYSTEM_CLOCK:
			SET_ClockSetting();
			menu_active = 1;
			break;
			
		case CONFIG_SET_DATE_FORMAT:
			SET_DataFormatSetting();
			menu_active = 7;
			break;
			
		case CONFIG_SET_SHUTTIME:
			SET_PowerDownSetting();
			menu_active = 4;
			break;
		case CONFIG_SET_BACKTIME:
			SET_BackLightTimeSetting();
			menu_active = 5;
			break;
		case CONFIG_SET_BACKCOLOR:
			//SET_BackLightColorSetting();
			menu_active = 5;
			break;
		case CONFIG_SET_BACKLIGHT:
			SET_BackLightSetting();
			menu_active = 5;
			break;
		case CONFIG_SET_LANGUAGE:
			GUI_GetMenuResult(&param);
			g_comval->langid = param;
			gui_load_resource(param);
			menu_active = 2;
			break;
		case CONFIG_SET_MEMORY_INFO:
			SET_StorageSetting();
			menu_active = 6;
			break;
		case CONFIG_SET_HW_VERSION:
			SET_FirmwareSetting();
			menu_active = 6;
			break;
		case CONFIG_SET_HW_UPGRADE:
			SET_UpdateSetting();
			menu_active = 6;
			break;
		case CONFIG_SET_HW_SYSTEMRESET:
			SET_SystemReset();
			return RESULT_MAIN;
			menu_active = 6;
			break;
		case AP_KEY_MODE|AP_KEY_UP:
		case AP_KEY_PLAY|AP_KEY_UP:
			need_draw = FALSE;
			break;
		default:
			break;
		}
		//NVRAMWrite(g_comval, VM_SYSTEM,sizeof(g_comval));//做完设置后，将相应的修改进行保存
		NVRAMWriteData();
		
	}//while(1)
	
	return result;
}



/*********************************************************************************
* Description : 背光时间设置回调函数
*
* Arguments   : value, 背光时间,单位秒
*
* Returns     :
*
* Notes       : =0时关闭背光
*
*********************************************************************************/
void SET_BackLightTimeCb(INT32 value)
{
	g_comval->LightTime = (UINT8)value;
	
	//及时关闭背光
	if(g_comval->LightTime == 0)
	{
		CloseBacklight();
	}
	else
	{
		OpenBacklight();
	}
	
	MESSAGE_Initial(g_comval);
}

/*********************************************************************************
* Description : 背光时间设定界面
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 SET_BackLightTimeSetting(void)
{
	INT32  result = 0;
	
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(GUI_IMG_BKLIGHT, 0, 0);
	GUI_DisplayBattaryLever();
	GUI_DisplayTextCenter(0, GUI_GetString(GUI_STR_BACKLIGHTTIME));
	
	slider.max  = 30;
	slider.min  = 0;
	slider.step = 3;
	slider.value = g_comval->LightTime;
	
	result = GUI_Slider(&slider, SET_BackLightTimeCb);
	
	return result;
}

/*********************************************************************************
* Description : 关机设定界面
*
* Arguments   :
*
* Returns     :
*
* Notes       : 标准功能菜单控件
*
*********************************************************************************/
INT32 SET_PowerDownSetting(void)
{
	INT32 result = 0;
	
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(GUI_IMG_POWER, 0, 0);
	GUI_DisplayTextCenter(0, GUI_GetString(GUI_STR_POWEOFFTIME));
	GUI_DisplayBattaryLever();
	
	slider.min   = 0;
	slider.max   = 60;
	slider.step  = 10;
	slider.value = g_comval->StandbyTime;
	result = GUI_Slider(&slider, NULL);
	if(result != 0)
	{
		return result;
	}
	g_comval->StandbyTime = (UINT8)slider.value;
	
	//重新设置关机时间
	MESSAGE_Initial(g_comval);
	
	return result;
	
}

/*********************************************************************************
* Description : 对比度设定回调函数
*
* Arguments   : value, 1~16, 对应硬件 6~21
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
void SET_ContrastCb(INT32 value)
{
	g_comval->DisplayContrast = (UINT8)value;
	//设置到硬件
	SetContrast(g_comval->DisplayContrast);
}


/*********************************************************************************
* Description : 对比度设定界面
*
* Arguments   :
*
* Returns     :
*
* Notes       : 注意背景先画
*
*********************************************************************************/
INT32 SET_ContrastSetting(void)
{
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(GUI_IMG_CONTRSTW, 10, 0);
	GUI_ResShowPic(GUI_IMG_CONTRSTB, 98, 0);
	
	GUI_DisplayBattaryLever();
	
	slider.min  = 1;
	slider.max  = 7;
	slider.step = 1;
	slider.value = g_comval->DisplayContrast;
	
	return GUI_Slider(&slider, SET_ContrastCb);
}

/*********************************************************************************
* Description : 亮度设定界面
*
* Arguments   :
*
* Returns     :
*
* Notes       : 注意背景先画
*
*********************************************************************************/
void SET_BackLightCb(INT32 value)
{
	g_comval->BackLight = (UINT8)value;
	//设置到硬件
	SetBackLight(g_comval->BackLight);
}
INT32 SET_BackLightSetting(void)
{
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(GUI_IMG_BKLIGHT, 10, 0);
	GUI_ResShowPic(GUI_IMG_BKLIGHTA, 98, 0);
	
	GUI_DisplayBattaryLever();
	
	slider.min  = 0;
	slider.max  = 7;
	slider.step = 1;
	slider.value = g_comval->BackLight;
	
	return GUI_Slider(&slider, SET_BackLightCb);
}

/*********************************************************************************
* Description : 格式化储存容量的显示字符串
*
* Arguments   : buf, 输出字符串buffer
        mega，总容量
        percent，使用百分比
*
* Returns     :
*
* Notes       : 1024M<100%>
*
*********************************************************************************/
void SET_FmtStorage(INT8 *buf, UINT16 mega, UINT8 percent)
{
	sprintf(buf, "%dM<%d%>", mega, percent);
}


/*********************************************************************************
* Description : 储存状况界面
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 SET_StorageSetting(void)
{
	BOOL mem_ok = FALSE;   //主内存是否可用
	BOOL card_ok = FALSE;      //卡是否可用
	UINT64 total1 = 0L, free1 = 0L;    //内存
	UINT8  percent1 = 0;
	UINT64 total2 = 0L, free2 = 0L;    //插卡
	UINT8  percent2 = 0L;
	INT8 toshow = 0;      //要显示那个, 0:main, 1:card
	UINT32 key;
	BOOL need_draw = TRUE;
	INT8 buf[20];    // 1024M<100%>
	
	card_ok = MountDisk(FS_DEV_TYPE_TFLASH);
	
	if(card_ok)
	{
		total2 = FS_GetSpace(FS_DEV_TYPE_TFLASH, FS_GET_SPACE_TOTAL);
		free2  = FS_GetSpace(FS_DEV_TYPE_TFLASH, FS_GET_SPACE_FREE);
		
		//处理<1%
		if(total2 == free2)
		{
			percent2 = 0;
		}
		else
		{
			percent2 = (total2 - free2) * 100 / total2;
			if(percent2 == 0)
			{
				percent2 = 1;
			}
		}
		toshow = 1;
	}
	
#if APP_SUPPORT_USB
	mem_ok = MountDisk(FS_DEV_TYPE_USBDISK);
	
	if(mem_ok)
	{
		total1 = FS_GetSpace(FS_DEV_TYPE_USBDISK, FS_GET_SPACE_TOTAL);
		free1  = FS_GetSpace(FS_DEV_TYPE_USBDISK, FS_GET_SPACE_FREE);
		
		//处理<1%
		if(total1 == free1)
		{
			percent1 = 0;
		}
		else
		{
			percent1 = (total1 - free1) * 100 / total1;
			if(percent1 == 0)
			{
				percent1 = 1;
			}
		}
		toshow = 0;
	}
#endif
	//control
	slider.min = 0;
	slider.max = 100;
	
	while(1)
	{
		if(need_draw)
		{
			GUI_ClearScreen(NULL);
			
			//show progress bar & text//percentage
#if APP_SUPPORT_USB
			if(toshow == 0)
			{
				slider.value = percent1;
				
				//显示主内存图标
				GUI_ResShowImage(GUI_IMG_DEVICE, FS_DEV_TYPE_USBDISK, 0, 0);
				
				//显示箭头
				if(card_ok)
				{
					GUI_ResShowPic(GUI_IMG_CARROWR, 115, 0);
				}
				
				//显示文字
				SET_FmtStorage(buf, total1 / 1024 / 1024, percent1);
				GUI_DisplayTextCenter(0, buf);
				//GUI_UpdateScreen(NULL);
				//GUI_Display_Progress(&progress);
				GUI_Slider(&slider, NULL);
			}
			else if(toshow == 1)
#endif
			{
				slider.value = percent2;//percentage
				
				//显示卡图标
				GUI_ResShowImage(GUI_IMG_DEVICE, FS_DEV_TYPE_TFLASH, 0, 0);
				
				//显示箭头
				if(mem_ok)
				{
					GUI_ResShowPic(GUI_IMG_CARROWL, 115, 0);
				}
				
				//显示文字
				SET_FmtStorage(buf, total2 / 1024 / 1024, percent2);
				GUI_DisplayTextCenter(0, buf);
				//GUI_UpdateScreen(NULL);
				//GUI_Display_Progress(&progress);//progress bar
				GUI_Slider(&slider, NULL);
			}
			
			need_draw = FALSE;
		}//need_draw
		
		
		key = MESSAGE_Wait();
		switch( key )
		{
		case AP_MSG_WAIT_TIMEOUT:           //子菜单界面在8秒后返回系统功能菜单
		case AP_KEY_MODE | AP_KEY_UP:
		case AP_KEY_PLAY | AP_KEY_UP:
			return 0;                       //退出
			
		case AP_KEY_NEXT | AP_KEY_PRESS:
		case AP_KEY_PREV | AP_KEY_PRESS:
			if(toshow == 0 && card_ok)
			{
				toshow = 1;
			}
			else if(toshow == 1 && mem_ok)
			{
				toshow = 0;
			}
			need_draw = TRUE;
			break;
			
		default:
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				return key;
			}
		}//switch(key)
		
	}//while(1)
	
}




/*********************************************************************************
* Description : 固件版本界面
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 SET_FirmwareSetting(void)
{
	UINT32 key;
	INT8 buf[20];
	BOOL need_draw = TRUE;
	
	while(1)
	{
		if(need_draw)
		{
			//			INT8 *p;
			//		    UINT8 ch1,ch2,ch3,ch4;
			
			GUI_ClearScreen(NULL);
			
			//show barcode
			GUI_ResShowPic(GUI_IMG_FWBCODE, 10, 0);
			sprintf(buf, "APP:%d.%d", AP_GetVersion() >> 8, AP_GetVersion() & 0xff);
			GUI_DisplayText(0, 16, buf);
			sprintf(buf, "Date: %08d", AP_GetBuildDate());
			GUI_DisplayText(0, 32, buf);
			sprintf(buf, "HW:%d.%d r%d", GetPlatformVersion() >> 8, GetPlatformVersion() & 0xff, GetPlatformRevision());
			GUI_DisplayText(0, 48, buf);
			
			GUI_UpdateScreen(NULL);
			
			need_draw = FALSE;
		}//need_draw
		key  = MESSAGE_Wait();
		switch( key )
		{
		case AP_MSG_WAIT_TIMEOUT:           //子菜单界面在8秒后返回系统功能菜单
		case AP_KEY_MODE | AP_KEY_UP:
		case AP_KEY_PLAY | AP_KEY_UP:
			return 0;
			
		default:
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				return key;
			}
		}
	}//while(1)
}


/*********************************************************************************
* Description : 固件升级
*
* Arguments   :
*
* Returns     : 结果消息
*
* Notes       :
*
*********************************************************************************/
INT32 SET_UpdateSetting(void)
{
	file_location_t location;
	INT32 file;
	INT32 result;
	INT8 *string;
	INT i;
	
	slider.min = 0;
	slider.max = 100;
	slider.step = 0;
	result = GUI_Directory(&location, FSEL_TYPE_ALLFILE, 0);
	if(result != 0)
	{
		return result;
	}
	
	file = FS_OpenDirect(location.file_entry, /*FS_O_RDONLY*/FS_O_RDWR, 0);
	if(file < 0)
	{
		GUI_DisplayMessage(0, GUI_STR_MREADERR, NULL, GUI_MSG_FLAG_WAIT);
		return;
	}
	
	GUI_ClearScreen(NULL);
	
	string = GUI_GetString(GUI_STR_UPGRADING);
	GUI_DisplayTextCenter(16, string);
	GUI_UpdateScreen(NULL);
	/*for(i=0;i<100;i++)
	{
	    slider.value = i;
	    GUI_Slider(&slider, NULL);
	    MESSAGE_Sleep(1);
	}*/
	Upgrade_Flash(file);
	
	GUI_DisplayMessage(0, GUI_STR_UPGRADE_COMPLETE, NULL, GUI_MSG_FLAG_WAIT);
	RestartSystem();
	return RESULT_MAIN;
}

#if 0
/*********************************************************************************
* Description : 背光颜色选择界面,
*
* Arguments   : 0:White,1:Red,2:Green,3Yellow,4:Blue,5:Purple,6:Cyan
*
* Returns     :
*
*
*********************************************************************************/
void SET_BackLightColorCb(INT32 value)
{
	CloseBacklight();
	SetBackLightColor((INT8)value);   //将颜色值传递给键盘驱动的BLightColor全局变量.
	OpenBacklight();               //根据上面设置的BLightColor，点亮对应颜色的背光.
	g_comval->BLightColor = (INT8)value;//保存当前背光颜色值.
}

INT32 bkcolor_list_callback(INT32 type, INT32 value, INT8 **string)
{
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = GUI_GetString(GUI_STR_REDLIGHT + value);
		return value + 1 == g_comval->BLightColor;
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		CloseBacklight();
		SetBackLightColor((INT8)value + 1); //将颜色值传递给键盘驱动的BLightColor全局变量.
		OpenBacklight();               //根据上面设置的BLightColor，点亮对应颜色的背光.
		g_comval->BLightColor = (INT8)value + 1; //保存当前背光颜色值.
		return 1;
	}
}

/*********************************************************************************
* Description : 设置背光灯颜色
*
* Arguments   :
*
* Returns     :
*
* Notes       : 多色彩支持
*
*********************************************************************************/
INT32 SET_BackLightColorSetting(void)
{
	return  GUI_Display_List(LIST_TYPE_SINGLE_SELECT, 0, 7, g_comval->BLightColor - 1, bkcolor_list_callback);
}
#endif

#define CLOCK_SET_YEAR       1
#define CLOCK_SET_MONTH      2
#define CLOCK_SET_DAY        3
#define CLOCK_SET_HOUR       4
#define CLOCK_SET_MINUTE     5

UINT8 SET_DateAndTime(INT8 set_what, INT8 *days, date_t *date, ap_time_t *time, BOOL flag)
{

	// check date format
	if(date && set_what <= CLOCK_SET_DAY)
	{
		const char *format = date_formats[g_comval->DateFormat];
		switch(format[(set_what - 1) * 3])
		{
		case 'Y':
			set_what = CLOCK_SET_YEAR;
			break;
		case 'M':
			set_what = CLOCK_SET_MONTH;
			break;
		case 'D':
			set_what = CLOCK_SET_DAY;
			break;
		}
	}
	{
		switch(set_what)
		{
		case CLOCK_SET_YEAR:    //year
			if(date)
			{
				if(flag)
				{
					if(date->year < g_pCalendarInfo->last_year)
					{
						date->year++;
					}
					else
					{
						date->year = g_pCalendarInfo->first_year;
					}
				}
				else
				{
					if(date->year > g_pCalendarInfo->first_year)
					{
						date->year--;
					}
					else
					{
						date->year = g_pCalendarInfo->last_year;
					}
				}
			}
			break;
		case CLOCK_SET_MONTH:    //month
			if(date)
			{
				if(flag)
				{
					if(date->month < 12)
					{
						date->month++;
					}
					else
					{
						date->month = 1;
					}
				}
				else
				{
					if(date->month > 1)
					{
						date->month--;
					}
					else
					{
						date->month = 12;
					}
				}
			}
			break;
		case CLOCK_SET_DAY:    //day
			if(date)
			{
				*days = month_days[date->month - 1];	 //本月天数
				if(IsLeapYear(date->year) && (date->month == 2))
				{
					*days = *days + 1;    //闰年+1
				}
				
				if(flag)
				{
					if(date->day < *days )
					{
						date->day++;
					}
					else
					{
						date->day = 1;
					}
				}
				else
				{
					if(date->day > 1 )
					{
						date->day--;
					}
					else
					{
						date->day = *days;
					}
				}
			}
			break;
		case CLOCK_SET_HOUR:    //hour
			if(time)
			{
				if(flag)
				{
					if(time->hour < 23)
					{
						time->hour++;
					}
					else
					{
						time->hour = 0;
					}
				}
				else
				{
					if(time->hour > 0)
					{
						time->hour--;
					}
					else
					{
						time->hour = 23;
					}
				}
			}
			break;
		case CLOCK_SET_MINUTE:    //min
			if(time)
			{
				if(flag)
				{
					if(time->minute < 59)
					{
						time->minute++;
					}
					else
					{
						time->minute = 0;
					}
				}
				else
				{
					if(time->minute > 0)
					{
						time->minute--;
					}
					else
					{
						time->minute = 59;
					}
				}
			}
			break;
		default:	//no in setting
			return 0;
		}	 //set_what
	}
	
	return 0xff;
}



INT32 GUI_ClockSetting(date_t *date, ap_time_t *time, UINT16 img_id, UINT16 str_id)
{
	UINT8  bSettime = 0;
	INT8   buf[11];    //date &  time
	UINT32  key;
	INT8   set_what = 0;     //0, 非设定状态, 1,year, 2,month...
	BOOL   show_what = TRUE;  //TRUE, 显示, FALSE, 不显示
	INT8   days;        //本月的天数
	INT8   item_start, item_end;
	BOOL     need_draw = TRUE;
	BOOL     need_draw_backgroud = TRUE;
	BOOL     set_ok = FALSE;  //设置完成
	BOOL     adjusting = FALSE;      //正在设置,不要闪烁
	region_t clock_region = {32, 40, 64, 16};
	region_t setting_region = {0, 0, 0, 0};
	const char *format = date_formats[g_comval->DateFormat];
	
	g_pCalendarInfo = (clndr_table_struct*)GetCalendarData();
	
	if(date == NULL && time == NULL)
	{
		return 0;
	}
	
	if(date)
	{
		item_start = CLOCK_SET_YEAR;
	}
	else
	{
		item_start = CLOCK_SET_HOUR;
	}
	if(time)
	{
		time->second = 0;
		item_end = CLOCK_SET_MINUTE;
	}
	else
	{
		item_end = CLOCK_SET_DAY;
	}
	
	
	if(date)
	{
		if(date->year > g_pCalendarInfo->last_year)
		{
			date->year = g_pCalendarInfo->last_year;
		}
		else if(date->year < g_pCalendarInfo->first_year)
		{
			date->year = g_pCalendarInfo->first_year;
		}
	}
	
	while(1)
	{
		//显示背景
		if(need_draw_backgroud)
		{
			GUI_ClearScreen(NULL);
			ui_auto_update = FALSE;
			if(img_id)
			{
				GUI_ResShowPic(img_id, 0, 0);    //show icon
			}
			GUI_DisplayTextCenter(0, GUI_GetString(str_id));
			GUI_DisplayBattaryLever();
			ui_auto_update = TRUE;
			need_draw_backgroud = FALSE;
		}
		
		//显示时钟
		if(need_draw)
		{
			setting_region.width = 16;
			setting_region.height = 16;
			
			if(date)
			{
				GUI_DisplayDateText(24, 24, TRUE, ((date->year) << 9) + (date->month << 5) + date->day);
				if(set_what < CLOCK_SET_HOUR)
				{
					setting_region.x = 24 + (set_what - 1) * 24;
					setting_region.y = 24;
					if(format[(set_what - 1) * 3] == 'Y')
					{
						setting_region.width = 32;
					}
					else
					{
						if((set_what == 2 && format[0] == 'Y') || (set_what == 3 && (format[0] == 'Y' || format[3] == 'Y')))
						{
							setting_region.x += 16;
						}
					}
				}
				
			}
			
			if(time)
			{
				UINT32 time_value = ((time->hour * 60 + time->minute) * 60 + time->second) * 1000;
				
				if(set_what >= CLOCK_SET_HOUR)
				{
					setting_region.x = 44 + (set_what - CLOCK_SET_HOUR) * 24;
				}
				
				if(date)
				{
					GUI_DisplayTimeText(44, 40, GUI_TIME_DISPLAY_HOUR_MINUTE, time_value);
					if(set_what >= CLOCK_SET_HOUR)
					{
						setting_region.y = 40;
					}
				}
				else
				{
					GUI_DisplayTimeText(44, 24, GUI_TIME_DISPLAY_HOUR_MINUTE, time_value);
					if(set_what >= CLOCK_SET_HOUR)
					{
						setting_region.y = 24;
					}
				}
			}
			if(set_what != 0)
			{
				if(show_what == 0)
				{
					GUI_ClearScreen(&setting_region);
				}
				//GUI_InvertRegion(&setting_region);
			}
			
			//to reduce scr flash
			GUI_UpdateScreen(NULL);
			
			need_draw = FALSE;
		}
		
		key = MESSAGE_Wait();
		
		switch( key )
		{
		case AP_MSG_RTC:
			if(adjusting)
			{
				show_what = TRUE;
			}
			else
			{
				show_what = !show_what;
			}
			need_draw = TRUE;
			break;
		case AP_KEY_NEXT | AP_KEY_PRESS:
			if(set_what >= item_end || set_what < item_start)
			{
				set_what = item_start;
			}
			else
			{
				set_what++;
			}
			break;
		case AP_KEY_PREV | AP_KEY_PRESS:
			if(set_what <= item_start)
			{
				set_what = item_end;
			}
			else
			{
				set_what--;
			}
			break;
		case AP_KEY_VOLADD | AP_KEY_HOLD:
			adjusting = TRUE;
		case AP_KEY_VOLADD | AP_KEY_PRESS:    //in setting clock
			bSettime  = SET_DateAndTime(set_what, &days, date, time, 1);
			need_draw = TRUE;
			break;
			
		case AP_KEY_VOLSUB | AP_KEY_HOLD:
			adjusting = TRUE;
		case AP_KEY_VOLSUB | AP_KEY_PRESS:    //in setting clock
			bSettime  = SET_DateAndTime(set_what, &days, date, time, 0);
			need_draw = TRUE;
			break;
			
		case AP_KEY_VOLADD| AP_KEY_UP:
		case AP_KEY_VOLSUB| AP_KEY_UP:
			adjusting = FALSE;
			break;
			
		case AP_KEY_MODE | AP_KEY_PRESS:
		case AP_MSG_WAIT_TIMEOUT:        //子菜单界面在8秒后返回系统功能菜单
			bSettime = 0;
		case AP_KEY_PLAY | AP_KEY_PRESS:
			key    = 0;
			set_ok = TRUE;
			break;
		default:
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW)
			{
				need_draw_backgroud = TRUE;
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				set_ok = TRUE;
			}
		} //switch(key)
		
		if(set_ok)
		{
			break;
		}
		
	}//while(1)
	
	if(bSettime == 0)
	{
		return key;
	}
	else if ( key != 0 )    //不是按mode退出的都不保存设置，如USB线插上等
	{
		return key;
	}
	
	if(date)
	{
		//re-check the date/time format
		days =  month_days[date->month - 1];
		
		if((date->year % 4 == 0) && (date->month == 2))
		{
			days++;    //润年
		}
		if(date->day > days)
		{
			date->day =  days;
		}
	}
	if(time)
	{
		time->second = 0;
	}
}



/*********************************************************************************
* Description : 时间设定界面
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 SET_ClockSetting(void)
{
	INT32 result;
	
	date_t    date = {2012, 1, 15};
	ap_time_t time = {12, 30, 00};
	
	TM_GetDateAndTime(&date, &time);
	
	result = GUI_ClockSetting(&date, &time, GUI_IMG_CLOCKICON, GUI_STR_SYSTEMCLOCK);
	
	//save date & time
	TM_SetDateAndTime(&date, &time);
	
	return result;
}


INT32 format_list_callback(INT32 type, INT32 value, INT8 **string)
{
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = date_formats[value];
		return value == g_comval->DateFormat;
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		g_comval->DateFormat = value;
		return 0;
	}
}



/*********************************************************************************
* Description : 设置日期格式
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 SET_DataFormatSetting(void)
{
	INT32   result;
	result = GUI_Display_List(LIST_TYPE_SINGLE_SELECT, 0, AP_MAX_FROMAT, g_comval->DateFormat, format_list_callback, FALSE);
	
	return result;
}



/*********************************************************************************
* Description : 系统复位
*
* Arguments   :
*
* Returns     : 结果消息
*
* Notes       :
*
*********************************************************************************/
INT32 SET_SystemReset(void)
{

	DSM_UserDataClear();
	g_comval = (comval_t*)NVRAMGetData(VM_SYSTEM, sizeof(comval_t));
	
	LoadFactorySetting(g_comval, sizeof(g_comval));
	//    NVRAMWrite(g_comval, VM_SYSTEM,sizeof(g_comval));
	NVRAMWriteData();
	GUI_DisplayMessage(0, GUI_STR_RESET_COMPLETE, NULL, GUI_MSG_FLAG_WAIT);
	RestartSystem();
	return RESULT_MAIN;
}



#endif // APP_SUPPORT_LCD==1    


