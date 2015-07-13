////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_timer.h
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   shenh           20090721        create
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MMC_TIMER_H
#define MMC_TIMER_H

#include "cos.h"

typedef enum
{
	/*
	** Start for for mmc timer.
	** Total 256 IDS.
	*/
	MMC_TIMER_ID_NONE=COS_MDI_TIMER_ID_BASE, /* Imp: Please do not modify this */ 
	
  	MMC_VID_PLY_TIMER_ID,
  	MMC_VID_REC_TIMER_ID,
	MMC_CAMERA_TIMER_ID,
	MMC_AUD_PLY_TIMER_ID,
	MMC_AUD_REC_TIMER_ID,
	    
	MMC_MAX_TIMERS=COS_MDI_TIMER_ID_END_ /* Imp: Please do not modify this */ 	 
}MMC_TIMER_IDS;

#endif


