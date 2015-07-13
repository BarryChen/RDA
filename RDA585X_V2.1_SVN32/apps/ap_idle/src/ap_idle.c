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

// ʵ������idle״̬����ϵͳû���κβ���������µ�״̬��ʡ��Ϊ��һ������������ʱ�䣬�Լ�ϵͳ���Ƶ�һЩ�����ַ�


/*********************************************************************************
* Description : �ȴ������¼��ķ��������뿨�����߶��������߰����������˳�standby
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
	//���ж�
	MyDI();
	
	Sys_Idle(g_comval->BatteryType);         //����DCģʽ������������32K�ĵ�Ƶ����
	//�ж�����STANDBY�������¼����ǰ�����USB���ϣ�
	
	for(i = 0; i < 80; i++)
	{
		//�ȴ����� ��PLAY KEY��
		if(input8(0x26) & 0x40)        //�а������жϼ�ֵ
		{
		
			if((input8(0xc0) | 0xfe) == 0xfe) //�ж��Ƿ���Play������
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
	
	MyEI();                                  //���ж�
#endif
	return event;
}

/*********************************************************************************
* Description : IDLE �������ڸ�ģʽ�����ʡ�磬����ϣ������ʾ����ʱ�ӵĹ���
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
					GUI_ClearScreen(NULL);//��ȫ��Ļ
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
					GUI_ClearScreen(NULL);//��ȫ��Ļ
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
	
	ClearScreen(NULL);                    //����
	UpdateScreen(NULL);
	
	
	StandbyScreen(0);                    //LCM��STANDBY
	//�ر��ж�
	ResetDsp();
	
	MyDI();
	
	Rtc43Reg      = input8(0x43);               //�ر�RTC
	Interrupt27Reg = input8(0x27);
	
	CloseBacklight();                //�ر���
	
	if(param == 0)
	{
		for(i = 0; i < Cycle; i++)  //���̧��
		{
			//�ȴ����� ��PLAY KEY��
			if(input8(0x26) & 0x40)
			{
				i = 0;
			}
		}
	}
	MyEI();                             //���ж�
	
	result = 0;
	
	while(!result)
	{
		if(!(input8(0x57) & 0x02))  //added by jmzhang for using 32k osc
		{
			PlugOn = 0;
		}
		switch(IDLE_StandBy())//�Ƿ��д������¼�
		{
		case  RESULT_POWERON:               //��standby����
			Sys_Restore();                 //�ָ���Ƶ
			
			if(HoldCheck())             //HOLDʱ�Ĵ���
			{
				OpenBacklight();    //������
				StandbyScreen(1);       //LCM �ָ�
				GUI_ResShowPic(GUI_IMG_LOCK, 0, 0);    //��ʾHOLDͼ��
				
				CloseBacklight();        //�ر���
				GUI_ClearScreen(NULL);            //����
				GUI_UpdateScreen(NULL);        //ˢ��
				StandbyScreen(0);            //LCM����STANDBY
				flag = 0;
				break;
			}
			if(((input8(0x57) & 0x02) == 0) && (input8(0x9c) < 0x40))
			{
				OpenBacklight();    //������
				StandbyScreen(1);        //LCM �ָ�
				CloseBacklight();        //�ر���
				ClearScreen(NULL);            //����
				UpdateScreen(NULL);        //ˢ��
				StandbyScreen(0);            //LCM����STANDBY
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
					if(powerkeytimes++ == 3)        //����PLAY��RESULT_POWERON��Ϣ
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
			if((input8(0x57) & 0x02) != 0)//Ҫ����PC
			{
				Sys_Restore();
				result = RESULT_UDISK;
			}
			else //���״̬
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







