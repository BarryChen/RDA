/***********************************************************************
 *
 * MODULE NAME:    gui_display.h
 * DESCRIPTION:    gui display Interface
 * AUTHOR:         Tianwq
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2011-2011 RDA Microelectronics.
 *     All rights reserved.
 *
 * REMARKS:
 *
 ***********************************************************************/

#ifndef _GUI__H_
#define _GUI__H_


typedef struct
{
	INT16 x;         //列位置
	INT16 y;         //行位置
	INT16 width;     //宽度
	INT16 height;    //高度
} region_t;

#include "project_config.h"
#include "gui_const.h"
#include "gui_resource.h"

#if APP_SUPPORT_RGBLCD==1
#include "gui_rgblcd_display.h"

#define GUI_Initialise(width,height)           gui_rgblcd_initialse(width,height)
#define GUI_UpdateScreen(region)               gui_rgblcd_update_screen(region)
#define GUI_ClearScreen(region)                gui_rgblcd_clear_screen(region)
#define GUI_InvertRegion(region)               gui_rgblcd_invert_Region(region)
#define GUI_VerticalScroll(y)                  gui_rgblcd_vertical_scroll(y)
#define GUI_HorizontalScroll(x)                gui_rgblcd_horizontal_scroll(x)
#define GUI_SetBackColor(color)                gui_rgblcd_set_back_color(color)
#define GUI_SetTextColor(color)                gui_rgblcd_set_text_color(color)
#define GUI_ResShowPic(id, x, y)               gui_rgblcd_display_image(x,y,0,id)
#define GUI_ResShowImage(id,index,x,y)         gui_rgblcd_display_image(x,y,index,id)
#define GUI_ResShowString(id,x,y)              gui_rgblcd_display_string(x, y, id)
#define GUI_DisplayText(x,y,str)               gui_rgblcd_display_text(x, y, str, NULL)
#define GUI_DisplayTextCenter(y,str)           gui_rgblcd_display_text_center(y, str)
#define GUI_DisplayTextRect(region,x,y,str)    gui_rgblcd_display_text(x, y, str, region)
#define GUI_GetTextWidth(str)                  gui_rgblcd_get_text_width(str)
#define GUI_GetString(str_id)                  gui_get_string(str_id)

#elif APP_SUPPORT_LCD==1
#include "gui_lcd_display.h"

#define GUI_Initialise(width,height)           gui_lcd_initialse(width,height)
#define GUI_UpdateScreen(region)               gui_lcd_update_screen(region)
#define GUI_ClearScreen(region)                gui_lcd_clear_screen(region, 0)
#define GUI_InvertRegion(region)               gui_lcd_invert_Region(region)
#define GUI_VerticalScroll(y)                  gui_lcd_vertical_scroll(y)
#define GUI_HorizontalScroll(x)                gui_lcd_horizontal_scroll(x)
#define GUI_ResShowPic(id, x, y)               gui_lcd_display_image(x,y,0,id)
#define GUI_ResShowImage(id,index,x,y)         gui_lcd_display_image(x,y,index,id)
#define GUI_ResShowString(id,x,y)              gui_lcd_display_string(x, y, id)
#define GUI_DisplayText(x,y,str)               gui_lcd_display_text(x, y, str, NULL)
#define GUI_DisplayTextCenter(y,str)           gui_lcd_display_text_center(y, str)
#define GUI_DisplayTextRect(region,x,y,str)    gui_lcd_display_text(x, y, str, region)
#define GUI_GetTextWidth(str)                  gui_lcd_get_text_width(str)
#define GUI_GetString(str_id)                  gui_get_string(str_id)
#define GUI_SetTextColor(color)                (0)
#define GUI_SetBackColor(color)                (0)


#elif APP_SUPPORT_LED8S==1   // 8 段数码管显示
#include "gui_led8s_display.h"

#define GUI_Initialise(width,height)        gui_led8s_clear_screen()
#define GUI_UpdateScreen(region)            gui_led8s_update_screen()
#define GUI_ClearScreen(region)             gui_led8s_clear_screen()
#define GUI_InvertRegion(region)            (0)
#define GUI_VerticalScroll(y)               (0)
#define GUI_HorizontalScroll(x)             (0)
#define GUI_ResShowPic(id, x, y)            gui_led8s_display_icon(id)
#define GUI_ResShowImage(id,index,x,y)      (0)
#define GUI_ResShowString(id,x,y)           (0)
#define GUI_DisplayText(x,y,str)            gui_led8s_display_text(x,str)
#define GUI_DisplayTextCenter(y,str)        (0)
#define GUI_DisplayTextRect(region,x,y,str) (0)
#define GUI_GetTextWidth(str)               (0)
#define GUI_GetString(str_id)               (0)
#define GUI_SetTextColor(color)             (0)
#define GUI_SetBackColor(color)             (0)

#else // no gui

#define GUI_Initialise(width,height)        (0)
#define GUI_UpdateScreen(region)            (0)
#define GUI_ClearScreen(region)             (0)
#define GUI_InvertRegion(region)            (0)
#define GUI_VerticalScroll(y)               (0)
#define GUI_HorizontalScroll(x)             (0)
#define GUI_ResShowPic(id, x, y)            (0)
#define GUI_ResShowImage(id,index,x,y)      (0)
#define GUI_ResShowString(id,x,y)           (0)
#define GUI_DisplayText(x,y,str)            (0)
#define GUI_DisplayTextCenter(y,str)        (0)
#define GUI_DisplayTextRect(region,x,y,str) (0)
#define GUI_GetTextWidth(str)               (0)
#define GUI_GetString(str_id)               (0)
#define GUI_SetTextColor(color)             (0)
#define GUI_SetBackColor(color)             (0)


#endif


#endif/*_GUI__H_*/

