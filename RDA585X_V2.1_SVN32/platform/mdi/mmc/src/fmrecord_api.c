////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: record_api.c
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#include "cs_types.h"
#include "cpu_share.h"
#include "mcip_debug.h"

#include "mmc.h"

//#include "cpu_sleep.h"
#include "lily_statemachine.h"

#include "mmc_fmrecorder.h"
uint32 LILY_FmrRecordStart (int8 *file_name_p) 	//	MCI_MEDIA_PLAY_REQ,			
{  
	int32 result;
	//int32 recordtype;

	diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordStart\n");    
	if (STA_RES_FAIL == lily_changeToState(STM_MOD_FMREC))
  {
    diag_printf("Error state!!!!!!!!!!!in bmpshow\n");
    return -1;
  }

    mmc_SetCurrMode(MMC_MODE_FM_RECORD);
	result = FmrRecordStart(0, 0, file_name_p);
	mmc_SetModeStatus(AudioRecordStatus);
	
    if (result > 0)
    {
		mmc_SetCurrMode(MMC_MODE_IDLE);
    }
	return result;
}

uint32 LILY_FmrRecordPause(void)
{
	diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordPause\n"); 	  
	
	FmrRecordPause();
	mmc_SetModeStatus(AudioRecordPauseStatus);
	return 0;
	//mmc_SendResult(nEvent,1);
}

uint32 LILY_FmrRecordResume(void)
{
	diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordResume\n");	
	
	FmrRecordResume();
	mmc_SetModeStatus(AudioRecordStatus);
	return 0;
//	mmc_SendResult(nEvent,1);

}

uint32 LILY_FmrRecordStop(void)
{
	diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordStop\n");
	
	FmrRecordStop();
	mmc_SetModeStatus(AudioRecordStopStatus);
	mmc_SetCurrMode(MMC_MODE_IDLE);
  lily_exitModule(STM_MOD_FMREC);
  return 0;
	//mmc_SendResult(nEvent,1);
}


