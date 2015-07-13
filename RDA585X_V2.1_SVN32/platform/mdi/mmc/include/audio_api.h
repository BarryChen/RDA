////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: audio_api.h
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#ifndef AUDIO_API_H
#define AUDIO_API_H

#include "cs_types.h"
#include "mmc_audiompeg.h"
#include "mmc_audiowma.h"
#include "mmc_audiosbc.h"
#include "mci.h"


uint32 LILY_AudioPlay (int32 OutputPath, HANDLE fileHandle, int16 fielType,INT32 PlayProgress); 	//	MCI_MEDIA_PLAY_REQ,			

uint32 LILY_AudioPause(void);

uint32 LILY_AudioResume(HANDLE fileHandle);

uint32 LILY_AudioSeek(int32 time);

uint32 LILY_AudioStop(void);

//uint32 LILY_AudioGetID3(int8 *pfilename,ID3INFO *id3info) ;	//	MCI_MEDIA_PLAY_REQ,			

uint32 LILY_AudioSetEQ(AUDIO_EQ 	EQMode);

uint32 LILY_AudioGetPlayInformation(MCI_PlayInf* PlayInformation);


#endif

