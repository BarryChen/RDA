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

#ifndef GUI_LED8S_DISPLAY_H
#define GUI_LED8S_DISPLAY_H

#define GUI_ICON_MUSIC      0x0001
#define GUI_ICON_FM         0x0002
#define GUI_ICON_RECORD     0x0004
#define GUI_ICON_LINEIN     0x0008
#define GUI_ICON_BLUETOOTH  0x0010


#define GUI_ICON_PLAY       0x0100
#define GUI_ICON_PAUSE      0x0200
#define GUI_ICON_SD         0x0400
#define GUI_ICON_USB        0x0800

#define GUI_ICON_COLON      0x1000
#define GUI_ICON_DECPOINT   0x2000


void gui_led8s_display_icon(UINT16 icon_id);
void gui_led8s_display_text(UINT8 pos, UINT8 *text);

void gui_led8s_clear_screen(void);
void gui_led8s_update_screen(void);

void gui_led8s_initialse(void);

#endif/*GUI_LED8S_DISPLAY_H*/


