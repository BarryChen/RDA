#include <csw.h>
#include <stdkey.h>
#include <event.h>
#include <base_prv.h>
#include <ts.h>
#include<sxr_tksd.h>
#include<sxr_tim.h>
#include "cos.h"
#include "csw_csp.h"

void  COS_CleanDCache() 
{
	asm volatile(".set noreorder");
	asm volatile(".align 4");
	asm volatile("cache 2,0");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile(".set reorder");
}
void  COS_CleanICache()
{
	asm volatile(".set noreorder");
	asm volatile(".align 4");
	asm volatile("cache 1,0");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile(".set reorder");

}
void  COS_CleanALLCach()
{
	asm volatile(".set noreorder");
	asm volatile(".align 4");
	asm volatile("cache 0,0");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile(".set reorder");
}

