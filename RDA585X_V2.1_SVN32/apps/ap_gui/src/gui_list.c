/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"

extern BOOL ui_auto_update;
extern BOOL ui_auto_select;
extern BOOL ui_timeout_exit;

#define  POSITION_LISTDERBG    (LCD_WIDTH-7)
#define  POSITION_MENUL        (2)
#define  POSITION_MENUR        (LCD_WIDTH-13)

#if APP_SUPPORT_RGBLCD==1
#define LIST_ITEM_PAGE      6
#elif APP_SUPPORT_LCD==1
#define LIST_ITEM_PAGE      3
#else
#define LIST_ITEM_PAGE      1
#endif

UINT16 g_list_count = 0;
UINT16 g_list_active = 0;
UINT16 g_last_active = 0;
UINT16 g_list_image_id = 0;
list_callback g_list_callback = NULL;
UINT8 g_list_type = 0;
UINT8 need_draw_list = 0;
UINT8 scroll_flag = 0;

region_t list_rect = {0};
BOOL list_update = FALSE;
BOOL list_direct = TRUE;
int list_num = 0;

void GUI_ShowList(void)
{
	int i, n, cur_group;
	INT8 *string;
	INT32 select;
	region_t    active_region = {0};
#if APP_SUPPORT_LCD==1
	if(g_list_type & LIST_TYPE_HORIZONTAL)
	{
#if APP_SUPPORT_RGBLCD==1
		if(!list_update)
#endif
		{
			//what page?
			cur_group = g_list_active / LIST_ITEM_PAGE;
			n = cur_group * LIST_ITEM_PAGE;
			GUI_ClearScreen(NULL);
			GUI_ResShowImage(GUI_IMG_BG_MAIN, 0, 0, 0);
			//show icons & text
			for(i = 0; i < LIST_ITEM_PAGE; i++)
			{
				if(n + i >= g_list_count)
				{
					break;    //处理不足4个的情况
				}
				select = g_list_callback(LIST_CALLBACK_GET_ITEM, i + n, &string);
				if(i + n == g_list_active)
				{
#if APP_SUPPORT_RGBLCD==1
					GUI_DisplayTextCenter(10, string);
					if(i < 3)
					{
						GUI_ResShowImage(g_list_image_id + 1, select,   12 + 55 * i, 36);
					}
					else
					{
						GUI_ResShowImage(g_list_image_id + 1, select,   12 + (55 * (i - 3)), 80);
					}
#else
					GUI_DisplayTextCenter(0, string);
					GUI_ResShowImage(g_list_image_id + 1, select,   12 + 40 * i, 24);
#endif
				}
				else
#if APP_SUPPORT_RGBLCD==1
					if(i < 3)
					{
						GUI_ResShowImage(g_list_image_id, select,   12 + 55 * i, 36);
					}
					else
					{
						GUI_ResShowImage(g_list_image_id, select,   12 + (55 * (i - 3)), 80);
					}
#else
					GUI_ResShowImage(g_list_image_id, select,   12 + 40 * i, 24);
#endif
			}
			
			//show active, 2 pixels left
			//GUI_ResShowPic(menu->pic_active[menu->active], menu_pos[menu->active % 5], 24);
			//GUI_ResShowImage(g_pMenu->active_image, GUI_MapIndex(g_menu_active),   8+29*(g_menu_active&3), 24);
			
			//check if show left arrows
			if(cur_group > 0)
			{
				GUI_ResShowPic(GUI_IMG_CARROWL, POSITION_MENUL, 0);
			}
			else if(g_list_count > 6)
			{
				GUI_ResShowPic(GUI_IMG_CARROWN, POSITION_MENUL, 0);
			}
#if APP_SUPPORT_RGBLCD==1
			//check if show right arrows
			if(cur_group + 1 < g_list_count / 6)
#else
			if(cur_group + 1 < g_list_count / 3)
#endif
				GUI_ResShowPic(GUI_IMG_CARROWR, POSITION_MENUR, 0);
			GUI_UpdateScreen(NULL);
		}
		// else
		//   GUI_ResShowPic(GUI_IMG_CARROWN, POSITION_MENUR, 0);
		//string = GUI_GetString(g_pMenuItems[ GUI_MapIndex(g_menu_active)].string_id);
	}
	else
#endif //APP_SUPPORT_LCD==1    
	{
#if APP_SUPPORT_RGBLCD==1
		if(!list_update)
#endif
		{
			//what page?
			cur_group = g_list_active / (LCD_HEIGHT >> 4);
			n = cur_group * (LCD_HEIGHT >> 4);
			
			active_region.x =  16;
			if(16 * (g_list_active - n - 1) < 0)
			{
				active_region.y = 0;
			}
			else
			{
				active_region.y =  16 * (g_list_active - n);
			}
			active_region.width  = (LCD_WIDTH - 23);
			active_region.height = 16;
			
			if(need_draw_list == 2)
			{
				// scroll active text
				g_list_callback(LIST_CALLBACK_GET_ITEM, g_list_active, &string);
				GUI_Scroll(string, FALSE);
				/*Invert Active Item*/
				GUI_InvertRegion(&active_region);
				GUI_UpdateScreen(NULL);
				return ;
			}
			
			GUI_ClearScreen(NULL);
			//show icons & text
			for(i = 0; i < (LCD_HEIGHT >> 4); i++)
			{
				if(n + i >= g_list_count)
				{
					break;    //处理不足一页的情况
				}
				select = g_list_callback(LIST_CALLBACK_GET_ITEM, i + n, &string);
				if(g_list_active == i + n)
				{
					if(GUI_GetTextWidth(string) > LCD_WIDTH - 16 - 8)
					{
						GUI_SetScrollRegin(&active_region, 5);
						GUI_Scroll(string, TRUE);
						scroll_flag = 1;
					}
					else
					{
						scroll_flag = 0;
					}
				}
				if(g_list_type <= LIST_TYPE_SINGLE_SELECT)
				{
					GUI_ResShowImage(GUI_IMG_SELECTICONS, g_list_type * 2 + select,  2,  16 * i);
				}
				else if(i + n == g_list_active)
				{
					GUI_ResShowImage(g_list_image_id + 1, select,  0,  16 * i);
				}
				else
				{
					GUI_ResShowImage(g_list_image_id, select,  0,  16 * i);
				}
				GUI_DisplayText(16, 16 * i, string);
				//   GUI_ResShowMultiString(g_pMenuItems[ g_menu_map[n+i]].string_id, 0, 16, 16*i);
			}
			
			
			/*Dispaly Active Items*/
			//GUI_ResShowImage(g_pMenu->active_image, g_menu_map[g_menu_active],  0, active_region.y);
			
			/*Invert Active Item*/
			GUI_InvertRegion(&active_region);
			
			/*Display Indication Pic In The Bg Pic*/
			GUI_ResShowPic(GUI_IMG_LISTDERBG, POSITION_LISTDERBG, 0);
			
#if APP_SUPPORT_RGBLCD==1
			GUI_ResShowPic(GUI_IMG_LISTDER, POSITION_LISTDERBG + 1, ((g_list_active * (LCD_HEIGHT - 18)) / (g_list_count - 1) + 5));
#else
			GUI_ResShowPic(GUI_IMG_LISTDER, POSITION_LISTDERBG + 1, ((g_list_active * (LCD_HEIGHT / 8 - 2)) / g_list_count + 1) * 8);
#endif
			GUI_UpdateScreen(NULL);
		}
#if APP_SUPPORT_RGBLCD==1
		else
		{
			//what page?
			cur_group = g_list_active / (LCD_HEIGHT >> 4);
			n = cur_group * (LCD_HEIGHT >> 4);
			active_region.x =  16;
			if(16 * (g_list_active - n - 1) < 0)
			{
				active_region.y = 0;
			}
			else
			{
				active_region.y =  16 * (g_list_active - n);
			}
			active_region.width  = (LCD_WIDTH - 23);
			active_region.height = 16;
			
			if(need_draw_list == 2)
			{
				// scroll active text
				g_list_callback(LIST_CALLBACK_GET_ITEM, g_list_active, &string);
				GUI_Scroll(string, FALSE);
				/*Invert Active Item*/
				GUI_InvertRegion(&active_region);
				GUI_UpdateScreen(NULL);
				return ;
			}
			GUI_ClearScreen(&list_rect);
			//show icons & text
			if(list_direct == TRUE)
			{
				for(i = g_list_active - list_num; i <= g_list_active; i++)
				{
					select = g_list_callback(LIST_CALLBACK_GET_ITEM, i, &string);
					if(g_list_active == i)
					{
						if(GUI_GetTextWidth(string) > LCD_WIDTH - 16 - 8)
						{
							GUI_SetScrollRegin(&active_region, 5);
							GUI_Scroll(string, TRUE);
							scroll_flag = 1;
						}
						else
						{
							scroll_flag = 0;
						}
					}
					if(g_list_type <= LIST_TYPE_SINGLE_SELECT)
					{
						GUI_ResShowImage(GUI_IMG_SELECTICONS, g_list_type * 2 + select,  2,  16 * (i % 8));
					}
					else if(i == g_list_active)
					{
						GUI_ResShowImage(g_list_image_id + 1, select,  0,  16 * (i % 8));
					}
					else
					{
						GUI_ResShowImage(g_list_image_id, select,  0,  16 * (i % 8));
					}
					GUI_DisplayText(16, 16 * (i % 8), string);
					//   GUI_ResShowMultiString(g_pMenuItems[ g_menu_map[n+i]].string_id, 0, 16, 16*i);
				}
			}
			else
			{
				for(i = g_list_active + list_num; i >= g_list_active; i--)
				{
					select = g_list_callback(LIST_CALLBACK_GET_ITEM, i, &string);
					if(g_list_active == i)
					{
						if(GUI_GetTextWidth(string) > LCD_WIDTH - 16 - 8)
						{
							GUI_SetScrollRegin(&active_region, 5);
							GUI_Scroll(string, TRUE);
							scroll_flag = 1;
						}
						else
						{
							scroll_flag = 0;
						}
					}
					if(g_list_type <= LIST_TYPE_SINGLE_SELECT)
					{
						GUI_ResShowImage(GUI_IMG_SELECTICONS, g_list_type * 2 + select,  2,  16 * (i % 8));
					}
					else if(i == g_list_active)
					{
						GUI_ResShowImage(g_list_image_id + 1, select,  0,  16 * (i % 8));
					}
					else
					{
						GUI_ResShowImage(g_list_image_id, select,  0,  16 * (i % 8));
					}
					GUI_DisplayText(16, 16 * (i % 8), string);
					//   GUI_ResShowMultiString(g_pMenuItems[ g_menu_map[n+i]].string_id, 0, 16, 16*i);
				}
			}
			
			
			/*Dispaly Active Items*/
			//GUI_ResShowImage(g_pMenu->active_image, g_menu_map[g_menu_active],  0, active_region.y);
			
			/*Invert Active Item*/
			GUI_InvertRegion(&active_region);
			
			/*Display Indication Pic In The Bg Pic*/
			GUI_ResShowPic(GUI_IMG_LISTDERBG, POSITION_LISTDERBG, 0);
			
			GUI_ResShowPic(GUI_IMG_LISTDER, POSITION_LISTDERBG + 1, ((g_list_active * (LCD_HEIGHT - 18)) / (g_list_count - 1) + 5));
			list_update = FALSE;
			GUI_UpdateScreen(&list_rect);
			
			active_region.x = POSITION_LISTDERBG;
			active_region.y = 0;
			active_region.width = 6;
			active_region.height = LCD_HEIGHT - 1;
			GUI_UpdateScreen(&active_region);
		}
#endif
	}
}

void GUI_Add_item()
{
	g_list_count++;
	need_draw_list = TRUE;
}

INT32 GUI_Display_List(UINT8 style, UINT16 image_id, UINT16 count,
                       UINT16 active, list_callback callback, BOOL is_dynamic)
{
	INT32 i, j;
	UINT32 key;
	BOOL value_changed = TRUE;
	INT32 value = -1;  //active item param value
	
	need_draw_list  = TRUE;
	g_list_count    = count;
	g_list_type     = style;
	g_list_image_id = image_id;
	g_list_callback = callback;
	g_list_active   = active;
	
	if(count == 0)
	{
		return 0;
	}
	
	ui_auto_update = FALSE;
	LED_SetPattern(GUI_LED_LIST, LED_LOOP_INFINITE);
	
	while(1)
	{
		if(!is_dynamic)
		{
			g_list_callback = callback;
			g_list_count    = count;
		}
		if(need_draw_list)
		{
			GUI_ShowList();
			need_draw_list = FALSE;
		}//need draw
		
		key = MESSAGE_Wait();
		switch( key)
		{
		case AP_KEY_NULL:
			if(scroll_flag)
			{
				scroll_flag++;
			}
			if(scroll_flag > 5 && (scroll_flag & 1))
			{
				need_draw_list |= 2;
			}
			break;
		case AP_KEY_VOLSUB| AP_KEY_PRESS:
			if(g_list_type & LIST_TYPE_HORIZONTAL)
			{
				if(g_list_active < g_list_count - 3)
				{
					g_list_active += 3;
					value_changed = TRUE;
					need_draw_list = TRUE;
					break;
				}
			}
		case AP_KEY_NEXT | AP_KEY_PRESS:
			if(g_list_active < g_list_count - 1)
			{
				list_direct = TRUE;
				list_num = 1;
				if(!(g_list_type & LIST_TYPE_HORIZONTAL))
				{
					list_rect.width = LCD_WIDTH - 7;
					list_rect.height = 32;
					list_rect.x = 0;
					list_rect.y = (g_list_active % 8) * 16;
					list_update = TRUE;
				}
				g_list_active++;
				if(!(g_list_active % 8) && g_list_active != 0
				   && !(g_list_type & LIST_TYPE_HORIZONTAL))
				{
					list_update = FALSE;
				}
			}
			else
			{
				//	list_update = FALSE;
				g_list_active = 0;
			}
			value_changed = TRUE;
			need_draw_list = TRUE;
			break;
			
		case AP_KEY_VOLSUB | AP_KEY_HOLD:
		case AP_KEY_NEXT | AP_KEY_HOLD:
			if(g_list_active < g_list_count - 4)
			{
				list_direct = TRUE;
				list_num = 4;
				list_rect.width = LCD_WIDTH;
				list_rect.height = 64;
				list_rect.x = 0;
				list_rect.y = (g_list_active % 8) * 16;
				if(!(g_list_type & LIST_TYPE_HORIZONTAL))
				{
					list_update = TRUE;
				}
				g_list_active += 4;
				if(!(g_list_active % 8) && g_list_active != 0)
				{
					list_update = FALSE;
				}
			}
			else
			{
				//	list_update = TRUE;
				g_list_active = g_list_count - 1;
			}
			value_changed = TRUE;
			need_draw_list = TRUE;
			break;
			
		case AP_KEY_VOLADD | AP_KEY_PRESS:
			if(g_list_type & LIST_TYPE_HORIZONTAL)
			{
				if(g_list_active >= 3)
				{
					g_list_active -= 3;
					value_changed = TRUE;
					need_draw_list = TRUE;
					break;
				}
			}
		case AP_KEY_PREV | AP_KEY_PRESS:
			if(g_list_active > 0)
			{
				g_list_active--;
				list_direct = FALSE;
				list_num = 1;
				list_rect.width = LCD_WIDTH;
				list_rect.height = 32;
				list_rect.x = 0;
				list_rect.y = (g_list_active % 8) * 16;
				if(!(g_list_type & LIST_TYPE_HORIZONTAL))
				{
					list_update = TRUE;
				}
				
				if(!(g_list_active % 7) && g_list_active != 0)
				{
					list_update = FALSE;
				}
			}
			else
			{
				//	list_update = FALSE;
				g_list_active = g_list_count - 1;
			}
			value_changed = TRUE;
			need_draw_list = TRUE;
			break;
			
		case AP_KEY_VOLADD | AP_KEY_HOLD:
		case AP_KEY_PREV | AP_KEY_HOLD:
			if(g_list_active > 4)
			{
				g_list_active -= 4;
				list_direct = FALSE;
				list_num = 4;
				list_rect.width = LCD_WIDTH;
				list_rect.height = 64;
				list_rect.x = 0;
				list_rect.y = (g_list_active % 8) * 16;
				if(!(g_list_type & LIST_TYPE_HORIZONTAL))
				{
					list_update = TRUE;
				}
				
				if(!(g_list_active % 7) && g_list_active != 0)
				{
					list_update = FALSE;
				}
			}
			else
			{
				//	list_update = FALSE;
				g_list_active = 0;
			}
			value_changed = TRUE;
			need_draw_list = TRUE;
			break;
		case AP_KEY_MODE | AP_KEY_PRESS:   //取消退出
			callback(LIST_CALLBACK_CANCEL, g_list_active, NULL);
			ui_auto_update = TRUE;
			return RESULT_IGNORE;
			break;
			
		case AP_MSG_WAIT_TIMEOUT:
			if(!ui_timeout_exit)
			{
				break;
			}
			if(!ui_auto_select)
			{
				callback(LIST_CALLBACK_CANCEL, g_list_active, NULL);
				ui_auto_update = TRUE;
				return RESULT_IGNORE;
			}
			
		case AP_KEY_PLAY | AP_KEY_PRESS:   //确认选择
			g_last_active = g_list_active;
			key = callback(LIST_CALLBACK_SELECT_ITEM, g_list_active, NULL);
			g_list_active = g_last_active;
			if(key == 0)
			{
				ui_auto_update = TRUE;
				return 0;
			}
			need_draw_list = TRUE;
			break;
			
		default:
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW)
			{
				need_draw_list = TRUE;
			}
			else if(key != 0)
			{
				return key;
			}
		}
		
	}//while (1)
}




