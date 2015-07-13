

#ifndef _FLASH_H_
#define _FLASH_H_

#include "cs_types.h"

typedef struct
{
	UINT32 start_addr;
	UINT32 size;
}t_flash_block;
typedef struct
{
	UINT32 version;		
	t_flash_block lcd_drv;
	t_flash_block flash_drv;
	t_flash_block patch;
	t_flash_block dsp;
	t_flash_block display;
	t_flash_block resource;
}t_flash_head;

#endif
