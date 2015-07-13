/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"

#define SLIDER_TIMEOUT   (3000)


//��ֱ������λ��x
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

const region_t bar_region = {0, 16, LCD_WIDTH, 16};  //�ؼ���������


/*********************************************************************************
* Description : �����˿ؼ�
*
* Arguments   : slider, �����˽ṹ
        callback, �����˵Ļص�����,=NULL ��ʾû�лص�����
*
* Returns     : �����Ϣ, ����0
*
* Notes       : �ؼ�����û���ػ�����, ��Ϊ�ؼ������ػ�����
*
*********************************************************************************/
INT32 GUI_Slider(slider_t *slider, const void (*callback)(INT32 res_id))
{
	INT32 start, end;        //x����
	INT8 buf[10];    //ע����ܺ����� <<-0000>>
	//  INT8 *p;    //��ʽ��bufʱʹ��
	UINT32 key;
	UINT8 step;
	BOOL need_draw = TRUE;
	BOOL value_changed = TRUE;  //ֵ�Ƿ�ı�, ��Ҫcallback
#if APP_SUPPORT_RGBLCD==1
	region_t num_region = {50, 80, 91, 24};  //��������
#elif APP_SUPPORT_LCD==1
	region_t num_region = {35, 40, 91, 24};  //��������
#endif
	//INT32 first=1;
	
	//if(!(AP_Support_LED()))
	//    first = 0;
	
	//   UINT16 value;    //to keep slider->value
	//    UINT8 hold_state=0;    //��ס״̬,Ϊ�˼ӿ�ؼ��ı���ٶ�
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
			//�ֶ���ʾ����
			while(start + SLIDER_BG_WIDTH < end)
			{
				GUI_ResShowPic(GUI_IMG_SBKGRD, start, 48);
				start += SLIDER_BG_WIDTH;
			}
			// �ڱ����ľ�ͷ����ʾ�ϰ�
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
			//�ֶ���ʾ����
			while(start + SLIDER_BG_WIDTH < end)
			{
				GUI_ResShowPic(GUI_IMG_SBKGRD, start, 24);
				start += SLIDER_BG_WIDTH;
			}
			// �ڱ����ľ�ͷ����ʾ�ϰ�
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
				//ֱ���˳�����Ϊ�ؼ������ػ�����
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


