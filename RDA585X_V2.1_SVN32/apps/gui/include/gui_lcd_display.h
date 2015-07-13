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

#ifndef GUI_DISPLAY_H
#define GUI_DISPLAY_H

void gui_lcd_display_text(INT32 x, INT32 y, const UINT8 *text, region_t *region);
void gui_lcd_display_text_center(INT32 y, const UINT8 *text);
void gui_lcd_display_string(INT32 x, INT32 y, U16 str_id);
void gui_lcd_display_image(INT32 x, INT32 y, INT32 index, U16 img_id);
UINT32 gui_lcd_get_text_width(const INT8 *text);

void gui_lcd_update_screen(const region_t *region);
void gui_lcd_clear_screen(const region_t *clrregion, UINT16 value);
void gui_lcd_invert_Region(const region_t *invregion);

void gui_lcd_vertical_scroll(INT32 y);
void gui_lcd_horizontal_scroll(INT32 x);

void gui_lcd_draw_image(region_t *region, UINT8 *data, region_t *rect);

void gui_lcd_initialse(UINT8 width, UINT8 heigth);

#endif/*GUI_DISPLAY_H*/


