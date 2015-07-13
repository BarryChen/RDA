/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"

#define SLIDER_TIMEOUT   (3000)


//垂直进度条位置x
#if APP_SUPPORT_RGBLCD==1
#define SLIDER_BAR_WIDTH      (130)
#elif APP_SUPPORT_LCD==1
#define SLIDER_BAR_WIDTH      (98)
#endif

#define SLIDER_BG_WIDTH       (4)
#define POSITION_SLIDERBAR    (15)

extern region_t res_region;
extern BOOL ui_auto_update;
extern BOOL ui_auto_select;
extern BOOL ui_timeout_exit;

const region_t bar_region = {0, 16, LCD_WIDTH, 16};  //控件所在区域


/*********************************************************************************
* Description : 滑动杆控件
*
* Arguments   : slider, 滑动杆结构
        callback, 滑动杆的回调函数,=NULL 表示没有回调函数
*
* Returns     : 结果消息, 或者0
*
* Notes       : 控件本身没有重画功能, 因为控件不能重画背景
*
*********************************************************************************/
INT32 GUI_Slider(slider_t *slider, const void (*callback)(INT32 res_id))
{
	INT32 start, end;        //x坐标
	INT8 buf[10];    //注意可能含符号 <<-0000>>
	//  INT8 *p;    //格式化buf时使用
	UINT32 key;
	UINT8 step;
	BOOL need_draw = TRUE;
	BOOL value_changed = TRUE;  //值是否改变, 需要callback
#if APP_SUPPORT_RGBLCD==1
	region_t num_region = {50, 80, 91, 24};  //数字区域
#elif APP_SUPPORT_LCD==1
	region_t num_region = {35, 40, 91, 24};  //数字区域
#endif
	//INT32 first=1;
	
	//if(!(AP_Support_LED()))
	//    first = 0;
	
	//   UINT16 value;    //to keep slider->value
	//    UINT8 hold_state=0;    //按住状态,为了加快控件改变的速度
	//0:normal; 1:holding up, 2:holding down
	
	MESSAGE_Initial(g_comval);
	
	APP_ASSERT(slider->value <= slider->max);
	APP_ASSERT(slider->value >= slider->min);
	APP_ASSERT(slider->step >= 0);
	
	LED_SetPattern(GUI_LED_SLIDER, LED_LOOP_INFINITE);
	
	while(1)
	{
		if(need_draw)
		{
#if APP_SUPPORT_RGBLCD==1
			GUI_ClearScreen(&bar_region);
			GUI_ResShowPic(GUI_IMG_SBAR, POSITION_SLIDERBAR, 48);
			start = POSITION_SLIDERBAR + 1;
			end   = (POSITION_SLIDERBAR - 1) + SLIDER_BAR_WIDTH * (slider->value - slider->min) / (slider->max - slider->min);
			APP_ASSERT(end < LCD_WIDTH);
			//分段显示背景
			while(start + SLIDER_BG_WIDTH < end)
			{
				GUI_ResShowPic(GUI_IMG_SBKGRD, start, 48);
				start += SLIDER_BG_WIDTH;
			}
			// 在背景的尽头处显示拖把
			GUI_ResShowPic(GUI_IMG_SSLIDER, start, 48);
			
			//format text <-xxxx>
			sprintf(buf, "<%d/%d>", slider->value, slider->max);
			
			//show text <-xxxx>
			GUI_ClearScreen(&num_region);
			GUI_DisplayText(num_region.x, num_region.y, buf);
			
			GUI_UpdateScreen(NULL);
#elif APP_SUPPORT_LCD==1
			GUI_ClearScreen(&bar_region);
			GUI_ResShowPic(GUI_IMG_SBAR, POSITION_SLIDERBAR, 24);
			start = POSITION_SLIDERBAR + 1;
			end   = (POSITION_SLIDERBAR - 1) + SLIDER_BAR_WIDTH * (slider->value - slider->min) / (slider->max - slider->min);
			APP_ASSERT(end < LCD_WIDTH);
			//分段显示背景
			while(start + SLIDER_BG_WIDTH < end)
			{
				GUI_ResShowPic(GUI_IMG_SBKGRD, start, 24);
				start += SLIDER_BG_WIDTH;
			}
			// 在背景的尽头处显示拖把
			GUI_ResShowPic(GUI_IMG_SSLIDER, start, 24);
			
			//format text <-xxxx>
			sprintf(buf, "<%d/%d>", slider->value, slider->max);
			
			//show text <-xxxx>
			GUI_ClearScreen(&num_region);
			GUI_DisplayText(num_region.x, num_region.y, buf);
			
			GUI_UpdateScreen(NULL);
#elif APP_SUPPORT_LED8S==1
			sprintf(buf, "%4d", slider->value);
			GUI_DisplayText(0, 0, buf);
			GUI_UpdateScreen(NULL);
#endif
			
			need_draw = FALSE;
			
		}//need draw
		
		if(slider->step == 0) // step 0 for progress display
		{
			return 0;
		}
		
#if 0// APP_SUPPORT_LED==1
		if(first)
		{
			if(first == 1)
			{
				SetLedStatus(LED_MP3, LED_OFF);
				SetLedStatus(LED_FM, LED_OFF);
				SetLedStatus(LED_MP3, LED_QUICK_FLASH);
			}
			
			first++;
			
			if(first == 5)
			{
				SetLedStatus(LED_FM, LED_QUICK_FLASH);
				first = 0;
			}
		}
#endif
		
		key = MESSAGE_Wait();
		
		step = slider->step;
		
		switch( key)
		{
		case AP_KEY_VOLADD|AP_KEY_PRESS:
		case AP_KEY_VOLADD | AP_KEY_HOLD:
			step = 1;
		case AP_KEY_NEXT|AP_KEY_PRESS:
		case AP_KEY_NEXT | AP_KEY_HOLD:
			if((slider->value) <= slider->max)
			{
				slider->value += step;
				if(slider->value > slider->max)
				{
					slider->value = slider->max;
				}
				value_changed = TRUE;
				need_draw = TRUE;
			}
			break;
			
			
		case AP_KEY_VOLSUB|AP_KEY_PRESS:
		case AP_KEY_VOLSUB | AP_KEY_HOLD:
			step = 1;
		case AP_KEY_PREV|AP_KEY_PRESS:
		case AP_KEY_PREV | AP_KEY_HOLD:
			if((slider->value) >= slider->min)
			{
				slider->value -= step;
				if(slider->value < slider->min)
				{
					slider->value = slider->min;
				}
				value_changed = TRUE;
				need_draw = TRUE;
			}
			break;
			
		case AP_KEY_MODE | AP_KEY_UP:
		case AP_KEY_PLAY | AP_KEY_UP:
			return 0;        //ok and return
			
			
		case AP_MSG_WAIT_TIMEOUT:
			if(!ui_timeout_exit)
			{
				break;
			}
			//if(ui_auto_select)
			return 0;
			break;
			
		default:
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW) //need_draw = TRUE;
			{
				//直接退出，因为控件不能重画背景
				return 0;
			}
			else if(key != 0)
			{
				return key;
			}
		}
		
#if 0
		//check if holding
		if(hold_state == 1)        //holding up
		{
			if((slider->value + slider->step) <= slider->max)
			{
				slider->value += slider->step;
				value_changed = TRUE;
				need_draw = TRUE;
				//while(key != NULL && (key & AP_KEY_UP) == NULL)key = ap_get_message();
				while(key != NULL)
				{
					if((key & AP_KEY_UP) != NULL)
					{
						hold_state = 0;
					}
					key = MESSAGE_Get();
				}
			}
			else
			{
				hold_state = 0;    //disable holding
			}
		}
		else if(hold_state == 2)    //holding down
		{
			if((slider->value - slider->step) >= slider->min)
			{
				slider->value -= slider->step;
				value_changed = TRUE;
				need_draw = TRUE;
				
				while(key != NULL)
				{
					if((key & AP_KEY_UP) != NULL)
					{
						hold_state = 0;
					}
					key = MESSAGE_Get();
				}
			}
			else
			{
				hold_state = 0;    //disable holding
			}
		}
#endif
		
		if(callback != NULL && value_changed)
		{
			callback(slider->value);
			value_changed = FALSE;
		}
	}//while(1)
}


