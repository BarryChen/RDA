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
#include "mmc_recorder.h"

extern Record_Selector gRecord;

uint32 LILY_RecordStart (int8 *file_name_p) 	//	MCI_MEDIA_PLAY_REQ,			
{  
	int32 result;

	diag_printf("[RECORD_API]: CALL - LILY_RecordStart\n");    
	#if 0
    recordtype = mmc_GetMediaTypeByFilename(file_name_p);
    diag_printf("now record play file : %s\n", file_name_p);
    mmc_EntryMode(MMC_MODE_RECORD, recordtype);
    ass(gRecord.Start == 0);
    result = gRecord.Start(0, 0, file_name_p);
	#endif
	if (STA_RES_FAIL == lily_changeToState(STM_MOD_AUDREC))
  {
    diag_printf("Error state!!!!!!!!!!!in bmpshow\n");
    return -1;
  }

    mmc_SetCurrMode(MMC_MODE_RECORD);
	result = RecordStart(0, 0, file_name_p);
	mmc_SetModeStatus(AudioRecordStatus);
	
    if (result > 0)
    {
		mmc_SetCurrMode(MMC_MODE_IDLE);
    }
	return result;
}

uint32 LILY_RecordPause(void)
{
	diag_printf("[RECORD_API]: CALL - LILY_RecordPause\n"); 	   
	#if 0
	ass(gRecord.Pause == 0); 
	gRecord.Pause();
	#endif
	RecordPause();
	
	mmc_SetModeStatus(AudioRecordPauseStatus);
	//mmc_SendResult(nEvent,1);

	return 0;
}

uint32 LILY_RecordResume(void)
{
	diag_printf("[RECORD_API]: CALL - LILY_RecordResume\n");	
	#if 0
	ass(gRecord.Resume == 0);  
	gRecord.Resume();
	#endif
	RecordResume();
	
	mmc_SetModeStatus(AudioRecordStatus);
//	mmc_SendResult(nEvent,1);
	return 0;
}

uint32 LILY_RecordStop(void)
{
	diag_printf("[RECORD_API]: CALL - LILY_RecordStop\n");		 
	#if 0
	ass(gRecord.Stop == 0);
	gRecord.Stop();
	#endif
	RecordStop();
	
	mmc_SetModeStatus(AudioRecordStopStatus);
	mmc_SetCurrMode(MMC_MODE_IDLE);
  lily_exitModule(STM_MOD_AUDREC);

	//mmc_SendResult(nEvent,1);

	return 0;
}

