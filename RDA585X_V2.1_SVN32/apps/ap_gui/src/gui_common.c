/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_gui.h"
#include "string.h"
#include "gui_led8s_display.h"
extern displayconfig_t  g_displayconfig;

//请不要把这个变量定义在非IDATA0 段,否则不能初始化
BOOL ui_auto_update = TRUE;	//是否自动刷新屏幕
BOOL ui_auto_select = TRUE;	//是否自动确认
BOOL ui_run_realtime = FALSE;	//控件跑实时模式
BOOL ui_timeout_exit = TRUE; //超时是否退出

const char *date_formats[AP_MAX_FROMAT] =
{
	"YY-MM-DD", "MM-DD-YY", "DD-MM-YY",
	"YY/MM/DD", "MM/DD/YY", "DD/MM/YY",
	"YY.MM.DD", "MM.DD.YY", "DD.MM.YY"
};

/**************************************************************************************
 * One level is 5.86mv//0~15, -1 表示正在充电
****************************************************************************************/
void GUI_DisplayBattaryLever(void)
{
#if APP_SUPPORT_LCD==1
	static INT32 cur_value = 0;  //当前电量值
	INT32 value = GetBattery();
	INT8 batt_type = 0;
	
	if(AP_Support_LCD())
	{
		if(value == -1)
		{
			cur_value++;
#if APP_SUPPORT_RGBLCD==1
			if(cur_value > 5)
#else
			if(cur_value > 9)
#endif
				cur_value = 1;
		}
		else
		{
#if APP_SUPPORT_RGBLCD==1
			value = (value + 3) / 6;
			if(value > 5)
			{
				value = 5;
			}
#else
			value = (value + 5) / 10;
			if(value > 9)
			{
				value = 9;
			}
#endif
			if(value == cur_value && ui_auto_update)
			{
				return;
			}
			cur_value = value;
		}
		
		//GUI_ResShowPic(batt_id[cur_value], POSITION_BATT, 0);
		GUI_ResShowImage(GUI_IMG_BATTLVLS, cur_value, g_displayconfig.position_batt, 0);
		
		if(ui_auto_update)
		{
			GUI_UpdateScreen(NULL);    //刷新
		}
	}
#endif
}


/*********************************************************************************
* Description : 显示错误字符串
*
*
*
* Notes       :
*
*********************************************************************************/
UINT32 GUI_DisplayMessage(INT16 title, INT16 message, INT8 *string, UINT8 flag)
{
	UINT32 key;
	INT8 *str;
	INT32 time = 0;
	
#if APP_SUPPORT_LCD==1
	if(!AP_Support_LCD())
	{
		return 0;
	}
	
	ui_auto_update = FALSE;
	GUI_ClearScreen(NULL);
	
	GUI_DisplayBattaryLever();
	
	str = gui_get_string(message);
	if(str)
	{
		GUI_DisplayTextCenter(g_displayconfig.message_y, str);
	}
	else if(string)
	{
		GUI_DisplayTextCenter(g_displayconfig.message_y, string);
	}
	
	
	GUI_UpdateScreen(NULL);                             //刷新
	ui_auto_update = TRUE;
	
	if(flag == GUI_MSG_FLAG_DISPLAY)
	{
		return 0;
	}
	
	while(1)
	{
		key = MESSAGE_Wait();
		switch(key)
		{
		case AP_MSG_STANDBY:
			//do nothing
			break;
			
		case AP_MSG_RTC:
			time ++;
			if(time > 4 && flag == GUI_MSG_FLAG_WAIT)
			{
				return 0;
			}
			break;
		case AP_MSG_WAIT_TIMEOUT:
		case AP_KEY_MODE|AP_KEY_PRESS:
			if(flag == GUI_MSG_FLAG_CONFIRM)
			{
				return 1;
			}
			else
			{
				return 0;
			}
			
		default:                                        //热键处理
			if((key & 0xffff) == AP_KEY_PRESS)
			{
				return 0;
			}
			key = MESSAGE_HandleHotkey(key);
			if(key != 0)
			{
				return key;
			}
		}
	};
#endif
	return 0;
}

void GUI_DisplayNumbers(INT32 x, INT32 y, UINT8 size, UINT16 img_id, UINT32 number)
{
	INT8 i;
	INT8 width;
	UINT16 len;
	
	gui_get_image(&width, &i, &len, img_id);
	for(i = size - 1; i >= 0; i--)
	{
		GUI_ResShowImage(img_id, number % 10, x + i * width, y);
		number /= 10;
	}
}

void GUI_DisplayTime(INT32 x, INT32 y, UINT8 type, UINT16 img_id, UINT32 time)
{
	UINT32 sec = time / 1000;
	UINT32 part1, part2, part3;
	INT width;
	
	if(img_id == GUI_IMG_BIG_NUM)
	{
		width = g_displayconfig.big_num_width;
	}
	else if(img_id == GUI_IMG_SMALL_NUM)
	{
		width = g_displayconfig.small_num_width;
	}
	
	part1 = sec / 3600;
	part2 = (sec / 60) % 60;
	part3 = sec % 60;
	
	if((type == GUI_TIME_DISPLAY_ALL) || (type == GUI_TIME_DISPLAY_HOUR_MINUTE) || (type == GUI_TIME_DISPLAY_AUTO && sec >= 3600))
	{
		GUI_ResShowImage(img_id, part1 / 10, x, y);
		x += width;
		GUI_ResShowImage(img_id, part1 % 10, x, y);
		x += width;
		GUI_ResShowImage(img_id, 10, x, y);
		if(img_id == GUI_IMG_BIG_NUM)
		{
			x += g_displayconfig.big_colon_width;
		}
		else if(img_id == GUI_IMG_SMALL_NUM)
		{
			x += g_displayconfig.small_colon_width;
		}
	}
	GUI_ResShowImage(img_id, part2 / 10, x, y);
	x += width;
	GUI_ResShowImage(img_id, part2 % 10, x, y);
	x += width;
	if((type == GUI_TIME_DISPLAY_ALL) || (type == GUI_TIME_DISPLAY_MINUTE_SECOND) || (type == GUI_TIME_DISPLAY_AUTO && sec < 3600))
	{
		GUI_ResShowImage(img_id, 10, x, y);
		if(img_id == GUI_IMG_BIG_NUM)
		{
			x += g_displayconfig.big_colon_width;
		}
		else if(img_id == GUI_IMG_SMALL_NUM)
		{
			x += g_displayconfig.small_colon_width;
		}
		GUI_ResShowImage(img_id, part3 / 10, x, y);
		x += width;
		GUI_ResShowImage(img_id, part3 % 10, x, y);
	}
}


void GUI_DisplayTimeText(INT32 x, INT32 y, UINT8 type, UINT32 time)
{
	UINT32 sec = time / 1000;
	UINT32 part1, part2, part3;
	INT8 buff[10];
	
	part1 = sec / 3600;
	part2 = (sec / 60) % 60;
	part3 = sec % 60;
	
	if(type == GUI_TIME_DISPLAY_ALL)
	{
		sprintf(buff, "%02d:%02d:%02d", part1, part2, part3);
	}
	else if((type == GUI_TIME_DISPLAY_HOUR_MINUTE) || (type == GUI_TIME_DISPLAY_AUTO && sec >= 3600))
	{
		sprintf(buff, "%02d:%02d", part1, part2);
	}
	else
	{
		sprintf(buff, "%02d:%02d", part2, part3);
	}
	
	GUI_DisplayText(x, y, buff);
}

void GUI_DisplayDateText(INT32 x, INT32 y, UINT8 islong, UINT32 date)
{
	INT8 buff[12], format[15], i;
	INT param[3];
	extern const char *date_formats[];
	const char *format_str;
	
	format_str = date_formats[g_comval->DateFormat];
	for(i = 0; i < 3; i++)
	{
		format[i * 5] = '%';
		format[i * 5 + 1] = '0';
		format[i * 5 + 2] = '2';
		format[i * 5 + 3] = 'd';
		format[i * 5 + 4] = format_str[i * 3 + 2];
		switch(format_str[i * 3])
		{
		case 'Y':
			param[i] = ((date >> 9));
			if(islong)
			{
				format[i * 5 + 2] = '4';
			}
			else
			{
				param[i] = param[i] % 100;
			}
			break;
		case 'M':
			param[i] = (date >> 5) & 0xf;
			break;
		case 'D':
			param[i] = date & 0x1f;
			break;
		}
	}
	
	sprintf(buff, format, param[0], param[1], param[2]);
	GUI_DisplayText(x, y, buff);
}

void GUI_DisplayTimeLEDs(UINT32 time)
{
	UINT32 sec = time / 1000;
	UINT32 part1, part2, part3;
	INT8 buff[10];
	
	GUI_ResShowPic(GUI_ICON_MUSIC | GUI_ICON_COLON, 0, 0);
	
	part1 = sec / 3600;
	part2 = (sec / 60) % 60;
	part3 = sec % 60;
	
	if(sec >= 3600)
	{
		sprintf(buff, "%02d%02d", part1, part2);
	}
	else
	{
		sprintf(buff, "%02d%02d", part2, part3);
	}
	
	GUI_DisplayText(0, 0, buff);
}

void GUI_DisplayMultiText(UINT8 *str)
{
	int i = 0;
	BOOL need_draw = FALSE;
	BOOL is_line = FALSE;
	int cpy_len = 0;
	int index = 0;
	int curr_index = 0;
	UINT32 key = 0;
	int line_num = 0;
	int result = 0;
	char *str_arr[60] = {0};
	int page = 0;
	int page_num = 0;
	int j = 0;
	int str_len = strlen(str);
	int deal_len = 0;
	
	line_num = (str_len >> 4);
	if(str_len % 16)
	{
		line_num++;
		is_line = TRUE;
	}
	
	page_num = (line_num >> 2);
	if(line_num % 4)
	{
		page_num++;
	}
	
	if(str_len >= 16)
	{
		deal_len = 16;
	}
	else
	{
		deal_len = str_len;
	}
	
	while(i < line_num)
	{
		while(cpy_len < deal_len)
		{
			if(str[curr_index] & 0x80)
			{
				cpy_len += 2;
				curr_index += 2;
			}
			else
			{
				cpy_len += 1;
				curr_index += 1;
			}
		}
		index += cpy_len;
		str_arr[i] = (char *)pMalloc(cpy_len + 2);
		memset(str_arr[i], 0, cpy_len + 2);
		if((i == line_num - 1) && is_line)
		{
			if(str_len >= 16)
			{
				strncpy(str_arr[i], &str[index - cpy_len], str_len & 0x10);
			}
			else
			{
				strncpy(str_arr[i], &str[index - cpy_len], str_len);
			}
		}
		else
		{
			strncpy(str_arr[i], &str[index - cpy_len], cpy_len);
		}
		cpy_len = 0;
		i++;
	}
	
	GUI_ClearScreen(NULL);
	need_draw = TRUE;
	if(line_num <= 4)
	{
		for(i = 0; i < line_num; i++)
		{
			GUI_DisplayText(0, i * 16, str_arr[i]);
		}
	}
	else
	{
		for(i = 0; i < 4; i++)
		{
			GUI_DisplayText(0, i * 16, str_arr[i]);
		}
	}
	
	GUI_UpdateScreen(NULL);
	while(1)
	{
		if(line_num <= 4)
		{
			key = MESSAGE_Wait();
			if((key & 0xffff) == AP_KEY_PRESS || (key & 0xffff) == AP_KEY_HOLD)
			{
				break;
			}
		}
		else
		{
			if(need_draw)
			{
				GUI_ClearScreen(NULL);
				for(i = (page << 2), j = 0; i < ((page + 1) << 2) - 1, j < 4; i++, j++)
				{
					GUI_DisplayText(0, j * 16, str_arr[i]);
				}
				need_draw = FALSE;
				GUI_UpdateScreen(NULL);
			}
			
			key = MESSAGE_Wait();
			switch(key)
			{
			case AP_KEY_MODE|AP_KEY_PRESS:
				goto end;
				break;
			case AP_KEY_NEXT|AP_KEY_DOWN:
			case AP_KEY_VOLSUB|AP_KEY_DOWN:
				if(page < page_num - 1)
				{
					need_draw = TRUE;
					page++;
				}
				break;
			case AP_KEY_PREV|AP_KEY_DOWN:
			case AP_KEY_VOLADD|AP_KEY_DOWN:
				if(page > 0)
				{
					need_draw = TRUE;
					page--;
				}
				break;
			default:
				result = MESSAGE_HandleHotkey(key);
				if(result != 0)
				{
					return result;
				}
			}
		}
	}
end:
	i = 0;
	while(i < line_num)
	{
		if(str_arr[i])
		{
			pFree(str_arr[i]);
			str_arr[i] = NULL;
		}
		i++;
	}
}

#if APP_SUPPORT_REMOTECONTROL==1

#define INPUT_NUMBER_TIMEOUT            1500

void Number_Input_Timeout(void *param)
{
	MESSAGE_SetEvent(AP_MSG_WAIT_TIMEOUT);
}

/*********************************************************************************
* Description : 处理输入的数字按键消息
*
*
*
* Notes       :
*
*********************************************************************************/
UINT32 GUI_KeyNumberHandle(UINT32 input_num)
{
	UINT32 key;
	UINT32 NumCounter = 0;
	BOOL need_draw = FALSE;
	INT32 input_value = 0;
	UINT8 number_timer;
	
	MESSAGE_Initial(g_comval);
	
	number_timer = COS_SetTimer(INPUT_NUMBER_TIMEOUT, Number_Input_Timeout, NULL, COS_TIMER_MODE_SINGLE);
	
	while(1)
	{
		if (need_draw)
		{
			ui_auto_update = FALSE;
#if APP_SUPPORT_RGBLCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);
				GUI_DisplayNumbers(32, 16, NumCounter, GUI_IMG_BIG_NUM, input_value);
				GUI_UpdateScreen(NULL);
			}
#elif APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);
				GUI_DisplayNumbers(32, 16, NumCounter, GUI_IMG_BIG_NUM, input_value);
				GUI_UpdateScreen(NULL);
			}
#elif APP_SUPPORT_LED8S==1
			{
				INT8 buff[10];
			
				sprintf(buff, "%4d", input_value);
				GUI_DisplayText(0, 0, buff);
				GUI_UpdateScreen(NULL);
			}
#endif
			ui_auto_update = TRUE;
			need_draw = FALSE;
		}
		
		if (NumCounter == 4)
		{
			return input_value;
		}
		
		key =  MESSAGE_Wait();
		
		if((key & 0xffff) == AP_KEY_PRESS || (key & 0xffff) == AP_KEY_HOLD)
		{
			if((key >> 16) <= 10)
			{
				NumCounter++;
				need_draw = TRUE;
				input_value *= 10;
				if((key >> 16) < 10)
				{
					input_value += (key >> 16);
				}
			}
			else if(key == (AP_KEY_MODE | AP_KEY_PRESS) || key == (AP_KEY_PLAY | AP_KEY_PRESS))
			{
				COS_KillTimer(number_timer);
				return input_value;
			}
			COS_ChangeTimer(number_timer, INPUT_NUMBER_TIMEOUT);
		}
		else if(key == AP_MSG_WAIT_TIMEOUT)
		{
			return input_value;
		}
	}
	
	return 0;
}
#endif




