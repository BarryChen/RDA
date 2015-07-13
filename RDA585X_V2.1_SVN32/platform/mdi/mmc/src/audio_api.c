////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: audio_api.c
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   
//
////////////////////////////////////////////////////////////////////////////////
#include "cs_types.h"

#include "mcip_debug.h"

#include "audio_api.h"
#include "mmc.h"
#include "lily_statemachine.h"


//#include "med_main.h" // to be removed 

extern SBC_PLAY *SBCPlayer;
extern Audio_Selector gAudio;
extern MPEG_PLAY *MpegPlayer;
extern WMA_PLAY *WMA_PLAYER;

static int32 gAudioType;


uint32 LILY_AudioPlay (int32 OutputPath, HANDLE fileHandle, int16 fielType,INT32 PlayProgress) 	//	MCI_MEDIA_PLAY_REQ,			
{
	//int32 result,res;
	int32 result;
	int32 audiotype;

    	audiotype = fielType;
		
	if(audiotype == MCI_TYPE_NONE)
	{
		return MCI_ERR_BAD_FORMAT;
	}
	
	gAudioType = audiotype;
	
    	mmc_EntryMode(MMC_MODE_AUDIO, audiotype);
    	lily_changeToState(STM_MOD_AUD_PLAY);

    	ass(gAudio.Play == 0);
    	result = gAudio.Play(OutputPath, fileHandle,fielType, PlayProgress);
	
    	if (result != MCI_ERR_NO)
    	{
      		mmc_ExitMode();
      		lily_exitModule(STM_MOD_AUD_PLAY);
    	}


	
	return result;
}

uint32 LILY_AudioPause(void)
{
	int32 result;

	ass(gAudio.Pause == 0);  
  	lily_exitModule(STM_MOD_AUD_PLAY);
  	lily_changeToState(STM_MOD_AUD_PAUSE);
	result = gAudio.Pause();
	
	return result;
}

uint32 LILY_AudioResume(HANDLE fileHandle)
{
	int32 result;

	ass(gAudio.Resume == 0); 
  	lily_exitModule(STM_MOD_AUD_PAUSE);
 	lily_changeToState(STM_MOD_AUD_PLAY);
	result = gAudio.Resume(fileHandle);


	
	return result;
}

uint32 LILY_AudioSeek(int32 time)
{
   return 0;
}

uint32 LILY_AudioStop(void)
{
	int32 result = MCI_ERR_NO;

//	ass(gAudio.Stop == 0);  
if(gAudio.Stop == 0)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[LILY_AudioStop] gAudio.Stop = 0");
	return result;
}
	result = gAudio.Stop();
//	lily_changeToState(STA_MAC_MEDIA_AUD_STOP);
  	lily_exitModule(STM_MOD_AUD_PLAY);
	mmc_ExitMode();
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_AUDIO]media stop\n");
	
	return result;
}

/*uint32 LILY_AudioGetID3(int8 *pfilename,ID3INFO *id3info) 			
{
	int32 result;
	int32 audiotype;

	audiotype = med_get_media_type((kal_wchar*)pfilename);
	mmc_EntryMode(MMC_MODE_AUDIO, audiotype);
	ass(gAudio.GetID3 == 0);        
	result = gAudio.GetID3(pfilename);
	mmc_ExitMode();

	return result;
}*/

uint32 LILY_AudioGetPlayInformation(MCI_PlayInf* PlayInformation) 			
{
	uint32 result;
	ass(PlayInformation == NULL);
	ass(gAudio.GetPlayInformation == 0);        
	result = gAudio.GetPlayInformation(PlayInformation);
	
	return result;
}
	

uint32 LILY_AudioSetEQ(AUDIO_EQ EQMode)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_AUDIO]LILY_AudioSetEQ()->EQMode:%d\n",EQMode);
	
	switch(gAudioType)
	{
		case MCI_TYPE_DAF:
		case MCI_TYPE_AAC:
            if(MpegPlayer != NULL)
    			MpegPlayer->Voc_AudioDecIN.EQ_Type = EQMode-1;
			break;
		case MCI_TYPE_WMA:
            if(WMA_PLAYER != NULL)
	    		WMA_PLAYER->EQMode = EQMode;
		    break;
		case MCI_TYPE_SBC:
            if(SBCPlayer != NULL)
                SBCPlayer->Voc_AudioDecIN.EQ_Type = EQMode;
            break;
		default:
			MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_AUDIO]audio type unsupport set eq\n");
			break;
	}
	return 1;
}



