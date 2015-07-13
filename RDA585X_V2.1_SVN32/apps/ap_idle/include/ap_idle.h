/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#ifndef  _AP_IDLE_H_
#define  _AP_IDLE_H_

#include "ap_common.h"

INT32  IDLE_Entry(INT32 param);
INT32  TIMER_Entry(INT32 param);
void TIMER_SetAlarm(BOOL maintask);

#endif/*_AP_IDLE_H_*/




