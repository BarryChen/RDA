////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audiowav.h
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#include "mci.h"

#ifndef MMC_AUDIOTONE_H
#define MMC_AUDIOTONE_H

#define MSG_MMC_TONE_PCM_INT 1
#define MSG_MMC_TONE_PCM_END 2


typedef struct TONE_INPUT 
{
	uint8 *buff;
	uint8 *data; 
    uint8 loop;
    uint8 times;
	uint32 index;
	uint32 length;
} TONE_INPUT;

typedef struct TONE_OUTPUT 
{
	UINT32 start_time;
	int32 OutputPath;
} TONE_OUTPUT;

typedef struct TONE_PLAY 
{
	TONE_INPUT  ToneInput;
	TONE_OUTPUT  ToneOutput;
} TONE_PLAY;

int32 Audio_TonePlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);

int32 Audio_ToneStop (void);

int32 Audio_TonePause (void);	

int32 Audio_ToneResume (HANDLE fhd); 

int32 Audio_ToneGetID3 (char * pFileName);

int32 Audio_ToneUserMsg(int32 nMsg);

int32 Audio_ToneGetPlayInformation (MCI_PlayInf * MCI_PlayInfWAV);

PUBLIC MCI_ERR_T MMC_ToneGetFileInformation ( CONST INT32 FileHander,  AudDesInfoStruct  * CONST DecInfo  );

#endif


