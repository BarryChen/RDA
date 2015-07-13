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


extern BOOL     ui_auto_update;
extern INT8 curr_active_alarm;

// 实际上是idle状态，对系统没有任何操作的情况下的状态，省电为第一，屏保可以是时间，以及系统定制的一些特殊字符


/*********************************************************************************
* Description : 等待触发事件的发生，插入卡，或者耳机，或者按键，都将退出standby
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
static INT8 IDLE_StandBy(void)
{
	INT8 event = 0;
#if 0
	INT32 i;
	//关中断
	MyDI();
	
	Sys_Idle(g_comval->BatteryType);         //进入DC模式，函数返回是32K的低频运行
	//判断引起STANDBY起来的事件（是按键或USB插上）
	
	for(i = 0; i < 80; i++)
	{
		//等待按键 （PLAY KEY）
		if(input8(0x26) & 0x40)        //有按键，判断键值
		{
		
			if((input8(0xc0) | 0xfe) == 0xfe) //判断是否有Play键按下
			{
				event = RESULT_POWERON;
				break;
			}
			else
			{
				break;
			}
		}
	}
	
	if((input8(0x57) & 0x02) && (PlugOn == 0))
	{
		event = RESULT_UDISK;    //for using 32k osc
	}
	
	MyEI();                                  //开中断
#endif
	return event;
}

/*********************************************************************************
* Description : IDLE 函数，在该模式下面会省电，但是希望能显示电子时钟的功能
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
INT32 IDLE_Entry(INT32 param)
{
	UINT32  key;
	UINT8 result = RESULT_MAIN; //POWERON;
	TM_SYSTEMTIME systemTime;
	INT32 time, count = 0, last_time = 0;
	BOOL key_down = FALSE;
	BOOL need_draw = TRUE;
	INT32 sleep = 0;
	ui_auto_update = FALSE;
	
	MESSAGE_Initial(g_comval);
	
	LED_SetPattern(GUI_LED_IDLE, LED_LOOP_INFINITE);
	
	while(1)
	{
		if(need_draw)
		{
#if APP_SUPPORT_RGBLCD==1
			if(AP_Support_LCD())
			{
				TM_GetSystemTime(&systemTime);
				if(sleep == 1)
				{
					time = systemTime.uHour * 60 + systemTime.uMinute;
				}
				else
				{
					time = systemTime.uHour * 3600 + systemTime.uMinute * 60 + systemTime.uSecond;
				}
				if(last_time != time)
				{
					GUI_ClearScreen(NULL);//清全屏幕
					GUI_DisplayBattaryLever();
					if(sleep == 1)
					{
						GUI_DisplayTime(24, 48, GUI_TIME_DISPLAY_HOUR_MINUTE, GUI_IMG_BIG_NUM, time * 60 * 1000);
					}
					else
					{
						GUI_DisplayTime(24, 48, GUI_TIME_DISPLAY_ALL, GUI_IMG_BIG_NUM, time * 1000);
					}
#if APP_SUPPORT_USBSTORAGE_BG==1
					if(GetUsbCableStatus())
					{
						GUI_ResShowImage(GUI_IMG_DEVICE, 2, 48, 0);
					}
#endif
					if(curr_active_alarm >= 0)
					{
						GUI_ResShowImage(GUI_IMG_MENU_SETUP, 3, 100, 0);
					}
					BT_DisplayIcon(88, 0);
					GUI_UpdateScreen(NULL);
					last_time = time;
				}
			}
			
#elif APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				TM_GetSystemTime(&systemTime);
				if(sleep == 1)
				{
					time = systemTime.uHour * 60 + systemTime.uMinute;
				}
				else
				{
					time = systemTime.uHour * 3600 + systemTime.uMinute * 60 + systemTime.uSecond;
				}
				if(last_time != time)
				{
					GUI_ClearScreen(NULL);//清全屏幕
					GUI_DisplayBattaryLever();
					if(sleep == 1)
					{
						GUI_DisplayTime(24, 16, GUI_TIME_DISPLAY_HOUR_MINUTE, GUI_IMG_BIG_NUM, time * 60 * 1000);
					}
					else
					{
						GUI_DisplayTime(4, 16, GUI_TIME_DISPLAY_ALL, GUI_IMG_BIG_NUM, time * 1000);
					}
#if APP_SUPPORT_USBSTORAGE_BG==1
					if(GetUsbCableStatus())
					{
						GUI_ResShowImage(GUI_IMG_DEVICE, 2, 70, 0);
					}
#endif
					if(curr_active_alarm >= 0)
					{
						GUI_ResShowImage(GUI_IMG_MENU_SETUP, 3, 100, 0);
					}
					BT_DisplayIcon(88, 0);
					GUI_UpdateScreen(NULL);
					last_time = time;
				}
			}
#elif APP_SUPPORT_LED8S==1
			{
				TM_GetSystemTime(&systemTime);
				time = systemTime.uHour * 3600 + systemTime.uMinute * 60 + systemTime.uSecond;
				GUI_ClearScreen(NULL);
				GUI_ResShowPic(GUI_ICON_COLON, 0, 0);
				GUI_DisplayTimeLEDs(time * 1000);
			
				GUI_UpdateScreen(NULL);
			}
#endif
			need_draw = FALSE;
		}
		key = MESSAGE_Wait();
		switch( key )
		{
		case AP_MSG_WAIT_TIMEOUT:
			if(-1 != GetBattery())
			{
				APP_Sleep();
				sleep = 1;
			}
			else
			{
				app_trace(APP_MAIN_TRC, "Can't sleep for charging");
				sleep = 2;
			}
			need_draw = TRUE;
			break;
		case AP_KEY_NULL:
		case AP_MSG_RTC:
			if(-1 == GetBattery())
			{
				if(sleep == 1)
				{
					app_trace(APP_MAIN_TRC, "Wakeup for charging");
					APP_Wakeup();
					sleep = 2;
				}
			}
			else if(sleep == 2)
			{
				app_trace(APP_MAIN_TRC, "Sleep for charging end");
				
				APP_Sleep();
				sleep = 1;
			}
			
			need_draw = TRUE;
			break;
			
		case AP_KEY_MODE|AP_KEY_HOLD:
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
			if(result == RESULT_REDRAW)
			{
				last_time = 0; // force display time
				need_draw = TRUE;
			}
			else if(result != 0)
			{
				APP_Wakeup();
				return result;
			}
			if((key & 0xffff) == AP_KEY_DOWN)
			{
				key_down = TRUE;
				APP_Wakeup();
			}
			else if(key_down && (key & 0xffff) == AP_KEY_UP)
			{
				if(!sleep || key == (AP_KEY_MODE | AP_KEY_UP))
				{
					return RESULT_MAIN;
				}
				sleep = 0;
				need_draw = TRUE;
			}
		}
	};
	
#if 0
	UINT8 powerkeytimes, flag;
	
	INT32     i, j;
	PlugOn = 0;      //added by jmzhang for using 32khz osc
	
	Cycle = 0x2000;
	
	MMI_DisplaySysLogo(TRUE);
	
	ClearScreen(NULL);                    //清屏
	UpdateScreen(NULL);
	
	
	StandbyScreen(0);                    //LCM进STANDBY
	//关闭中断
	ResetDsp();
	
	MyDI();
	
	Rtc43Reg      = input8(0x43);               //关闭RTC
	Interrupt27Reg = input8(0x27);
	
	CloseBacklight();                //关背光
	
	if(param == 0)
	{
		for(i = 0; i < Cycle; i++)  //检测抬键
		{
			//等待按键 （PLAY KEY）
			if(input8(0x26) & 0x40)
			{
				i = 0;
			}
		}
	}
	MyEI();                             //开中断
	
	result = 0;
	
	while(!result)
	{
		if(!(input8(0x57) & 0x02))  //added by jmzhang for using 32k osc
		{
			PlugOn = 0;
		}
		switch(IDLE_StandBy())//是否有触发的事件
		{
		case  RESULT_POWERON:               //从standby返回
			Sys_Restore();                 //恢复高频
			
			if(HoldCheck())             //HOLD时的处理
			{
				OpenBacklight();    //开背光
				StandbyScreen(1);       //LCM 恢复
				GUI_ResShowPic(GUI_IMG_LOCK, 0, 0);    //显示HOLD图形
				
				CloseBacklight();        //关背光
				GUI_ClearScreen(NULL);            //清屏
				GUI_UpdateScreen(NULL);        //刷新
				StandbyScreen(0);            //LCM进入STANDBY
				flag = 0;
				break;
			}
			if(((input8(0x57) & 0x02) == 0) && (input8(0x9c) < 0x40))
			{
				OpenBacklight();    //开背光
				StandbyScreen(1);        //LCM 恢复
				CloseBacklight();        //关背光
				ClearScreen(NULL);            //清屏
				UpdateScreen(NULL);        //刷新
				StandbyScreen(0);            //LCM进入STANDBY
				flag = 0;
				break;
			}
			powerkeytimes = 0;
			flag = 1;
			while(flag)
			{
				switch(GetSysMsg())
				{
				case Msg_KeyPlayPause:
					if(powerkeytimes++ == 3)        //长按PLAY发RESULT_POWERON消息
					{
						result = RESULT_POWERON;
						flag = 0;
					}
					break;
				case Msg_KeyShortUp:
				case Msg_KeyLongUp:
					flag = 0;
					break;
				default:
					break;
				}
			}
			break;
		case    RESULT_UDISK:
			if((input8(0x57) & 0x02) != 0)//要连接PC
			{
				Sys_Restore();
				result = RESULT_UDISK;
			}
			else //充电状态
			{
				while(1)
				{
				}
			}
		default:
			break;
		}
	}
	//rtc remove
	StandbyScreen(1);
#endif
	return result;
}







