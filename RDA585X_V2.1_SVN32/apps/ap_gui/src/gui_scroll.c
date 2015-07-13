
#include "ap_common.h"
#include "ap_gui.h"

#define FirstSectionCode         0xa1
#define SCROLL_SPACE    20

//设置滚屏的区域
//modify by wuxiang
//static region_t scrollregion={0,32,LCD_WIDTH,16};
region_t scrollregion = {0, 32, LCD_WIDTH, 16};
//modify end
static INT32 scroll_pos;
static INT8 scroll_step;

/* (字符串首址，是否初始化true为要初始化) */
void GUI_Scroll( const INT8 * scrollstr, BOOL mode)
{
	INT32 length         = GUI_GetTextWidth(scrollstr);
	
	if(mode)//第一次进入滚屏处理
	{
		scroll_pos = scrollregion.x;
	}
	else
	{
		scroll_pos -= scroll_step;
		if(length + scroll_pos < scrollregion.x)
		{
			scroll_pos += length + SCROLL_SPACE;
		}
	}
	
	GUI_ClearScreen(&scrollregion);
	GUI_DisplayTextRect(&scrollregion, scroll_pos, scrollregion.y, scrollstr);
	if(length + scroll_pos + SCROLL_SPACE < scrollregion.width)
	{
		GUI_DisplayTextRect(&scrollregion, scroll_pos + length + SCROLL_SPACE, scrollregion.y, scrollstr);
	}
}


void GUI_SetScrollRegin( const region_t *region, INT8 step)
{
	scroll_step = step;
	if(region)
	{
		scrollregion.x     = region->x;
		scrollregion.y     = region->y;
		scrollregion.width = region->width;
		scrollregion.height = region->height;
	}
}

