////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: vidrec_api.c
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   baoyh           20071130        create
//
////////////////////////////////////////////////////////////////////////////////
#ifndef VIDREC_API_H
#define VIDREC_API_H

#include "cpu_share.h"


int32 LILY_VidrecPreviewStartReq(MMC_VDOREC_SETTING_STRUCT *previewParam , void(*vid_rec_finish_ind)(uint16 msg_result) );


int32 LILY_VidrecAdjustSettingReq (int32 adjustItem, int32 value);

int32 LILY_VidrecPreviewStopReq(void);


int32 LILY_VidrecRecordStartReq (HANDLE filehandle) ;	//	MCI_MEDIA_PLAY_REQ,			


int32 LILY_VidrecRecordPauseReq(void);


int32 LILY_VidrecRecordResumeReq(void);

int32 LILY_VidrecRecordStopReq(void);

int32 LILY_VidrecAudioSampleReadyReq(uint32 nEvent);

int32 LILY_VidrecApplyAudioSampleStop(void);

#endif


