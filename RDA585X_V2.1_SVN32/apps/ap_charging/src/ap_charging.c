/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"
#include "ap_charging.h"

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
INT32 Charging_Entry(void)
{
	BOOL need_draw = TRUE;
	UINT32  key;
	BOOL is_full = FALSE;
	while(1)
	{
		if(GetUsbCableStatus())
		{
			return ;// usb irq came
		}
		
		if(need_draw)
		{
#if APP_SUPPORT_RGBLCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);//清全屏幕
				if(!is_full)
				{
					GUI_DisplayMessage(0, GUI_STR_CHARGING, 0, GUI_MSG_FLAG_DISPLAY);
				}
				else
				{
					GUI_DisplayMessage(0, GUI_STR_FULL_CHARGING, 0, GUI_MSG_FLAG_DISPLAY);
				}
				GUI_DisplayBattaryLever();
				GUI_UpdateScreen(NULL);
			}
			
			
#elif APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);//清全屏幕
				if(!is_full)
				{
					GUI_DisplayMessage(0, GUI_STR_CHARGING, 0, GUI_MSG_FLAG_DISPLAY);
				}
				else
				{
					GUI_DisplayMessage(0, GUI_STR_FULL_CHARGING, 0, GUI_MSG_FLAG_DISPLAY);
				}
				GUI_DisplayBattaryLever();
				GUI_UpdateScreen(NULL);
			}
#elif APP_SUPPORT_LED8S==1
			{
			}
#endif
			need_draw = FALSE;
		}
		
		
		key = MESSAGE_Wait();
		switch( key )
		{
		case AP_MSG_WAIT_TIMEOUT:
			break;
		case AP_KEY_NULL:
			break;
		case AP_MSG_RTC:
			need_draw = TRUE;
			if(is_full && GetBattery() != 100)
			{
				DM_DeviceSwithOff();
			}
			if(-1 != GetBattery())
			{
				is_full = TRUE;
			}
			break;
		case AP_KEY_POWER | AP_KEY_PRESS:
			return RESULT_POWERON;
			
		}
	}
}

