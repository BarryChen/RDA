////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audiotone.c
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#include "cswtype.h"
#include "mcip_debug.h"
#include "fs.h"

#include "mmc_audiotone.h"

#include "mmc.h"
#include "cpu_share.h"

#include "audio_api.h"

#include "aud_m.h"
#include "fs_asyn.h"

#include "cos.h"
#include "mci.h"
#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"

#include "string.h"
#include "hal_overlay.h"

#include "mmc_adpcm.h"
#include "resample.h"
#include "tgt_calib_m.h"

#define TONE_BUFF_SIZE          (3840)
#define TONE_SAMPLE_SIZE        (32)
#define MAX_SAMPLE_SIZE         (48)  // max sample rate 48KHz with loweset tone 256HZ, 1/4 sample length

TONE_PLAY TonePlayer;

static INT8 Mmc_Tone_Pcm_Half;
static INT8 Mmc_Tone_Current_Char;
static INT16 Mmc_Tone_Generate_Data_Size;
static INT8 ContinuePlayFlag;
static UINT32 merge_sampleRate;
static INT16 *merge_data;
static INT16 merge_data_length;
static INT8 merge_data_pos;
static INT8 merge_data_index; // 1, 2, -1, -2
static INT8 merge_data_channel;
static INT8 merge_data_flag;
static INT16 merge_data_freq;
static INT16 tone_sample_data[MAX_SAMPLE_SIZE];

extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;
extern UINT8 MCI_Play_Stream_Buffer;

static const INT16 tone_data[] = 
{
    0x0000,0x18f9,0x30fb,0x471c,0x5a82,0x6a6d,0x7641,0x7d89,
    0x7fff,0x7d89,0x7641,0x6a6d,0x5a82,0x471c,0x30fb,0x18f9,
    0x0000,0xe707,0xcf05,0xb8e4,0xa57e,0x9593,0x89bf,0x8277,
    0x8001,0x8277,0x89bf,0x9593,0xa57e,0xb8e4,0xcf05,0xe707,
    0x0000,0x18f9,0x30fb,0x471c,0x5a82,0x6a6d,0x7641,0x7d89,
};

static const UINT16 tone_freq[10] = {0, 256, 384, 512, 640, 758, 853, 1024, 1280, 1536};

/*
* NAME:	MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_AudioToneVocISR(void)
{
} 


/*
* NAME:	MMC_AudioToneHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/
void MMC_AudioToneHalfPcmISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_TONE]HALF!");
	Mmc_Tone_Pcm_Half = 0;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_TONE_PCM_INT);
}

/*
* NAME:	MMC_AudioToneEndPcmISR()
* DESCRIPTION: ISR for pcm end buffer exhausted.
*/
void MMC_AudioToneEndPcmISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_TONE]END!");
    Mmc_Tone_Pcm_Half = 1;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_TONE_PCM_INT);
}

void MMC_TonePcmOutStart(TONE_PLAY *pTonePlayer)
{
	TONE_INPUT *input = &pTonePlayer->ToneInput;
	TONE_OUTPUT *output = &pTonePlayer->ToneOutput;

    audioStream.startAddress = input->buff;
	audioStream.length        	= TONE_BUFF_SIZE;
	audioStream.channelNb     	= HAL_AIF_MONO;
	audioStream.voiceQuality   = !TRUE;    
	audioStream.halfHandler   	= MMC_AudioToneHalfPcmISR;
	audioStream.endHandler    = MMC_AudioToneEndPcmISR;

    // Initial cfg
   	{
   	  	// Initial cfg
   	  	  if(gpio_detect_earpiece())
    		audioItf = AUD_ITF_EAR_PIECE;
          else
            audioItf = AUD_ITF_LOUD_SPEAKER;
   	}
    audioStream.sampleRate = HAL_AIF_FREQ_16000HZ;
	aud_StreamStart(audioItf, &audioStream, &audio_cfg);
    output->start_time = 0;
    
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_TONE] END!");
}


PUBLIC MCI_ERR_T MMC_ToneGetFileInformation (
                        CONST INT32 FileHander,
                         AudDesInfoStruct  * CONST DecInfo  )
{
    return MCI_ERR_NO;
}


/*
* NAME:	MMC_AudioToneDecPcm()
* DESCRIPTION: Initialize tone decoder.
*/
MCI_ERR_T MMC_AudioToneDecPcm(TONE_PLAY *pTonePlayer)
{
	TONE_INPUT *input = &pTonePlayer->ToneInput;
	TONE_OUTPUT *output = &pTonePlayer->ToneOutput;
    int i,len;
    INT16 *p_PCM;

	if(ContinuePlayFlag)
	{
		//hal_HstSendEvent(SYS_EVENT,0x65900033);
		diag_printf("!!!!!play all left data in the pcm buffer\n");
		Audio_ToneStop( );
		MCI_AudioFinished(MCI_ERR_END_OF_FILE);
	
		ContinuePlayFlag = 0;
    	return MCI_ERR_NO;
	}

    if(Mmc_Tone_Pcm_Half == 0) // half, generate data
    {
        if(hal_TimGetUpTime()-output->start_time > 16384/10) // 0.1 second
        {
            uint8 c = input->data[input->index];

            //hal_HstSendEvent(SYS_EVENT,0x06120010);
            //hal_HstSendEvent(SYS_EVENT,input->times);
            //hal_HstSendEvent(SYS_EVENT,input->loop);
            //hal_HstSendEvent(SYS_EVENT,input->index);
            if(input->index == 0 && input->times && input->loop >= input->times)
                return MCI_ERR_END_OF_FILE;

            //hal_HstSendEvent(SYS_EVENT,0x06120030);
            //hal_HstSendEvent(SYS_EVENT,c);
            if(Mmc_Tone_Current_Char != c)
            {
                Mmc_Tone_Current_Char = c;
                
                p_PCM = input->buff;

                switch(c)
                {
                    case '0': // no audio
                        memset(input->buff, 0, TONE_BUFF_SIZE/2);
                        Mmc_Tone_Generate_Data_Size = TONE_BUFF_SIZE/2;
                        break;
                    case '1': // 256Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;i++)
                        {
                            *p_PCM++ = tone_data[i];
                            *p_PCM++ = (tone_data[i]+tone_data[i+1])/2;
                        }
                        break;
                    case '2': // 384Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;)
                        {
                            *p_PCM++ = tone_data[i];
                            *p_PCM++ = (tone_data[i]+tone_data[i+1])/2;
                            *p_PCM++ = (tone_data[i+1]+tone_data[i+2])/2;
                            *p_PCM++ = tone_data[i+3];
                            i+=3;
                            if(i>TONE_SAMPLE_SIZE)
                                i -= TONE_SAMPLE_SIZE;
                            else if(i==TONE_SAMPLE_SIZE)
                                break;
                        }
                        break;
                    case '3': // 512Hz
                        memcpy(input->buff, tone_data, TONE_SAMPLE_SIZE*2);
                        Mmc_Tone_Generate_Data_Size = TONE_SAMPLE_SIZE*2;
                        break;
                    case '4': // 640Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;)
                        {
                            *p_PCM++ = tone_data[i];
                            *p_PCM++ = (tone_data[i+1]+tone_data[i+2])/2;
                            *p_PCM++ = (tone_data[i+2]+tone_data[i+3])/2;
                            *p_PCM++ = tone_data[i+4];
                            i+=5;
                            if(i>TONE_SAMPLE_SIZE)
                                i -= TONE_SAMPLE_SIZE;
                            else if(i==TONE_SAMPLE_SIZE)
                                break;
                        }
                        break;
                    case '5': // 768Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;)
                        {
                            *p_PCM++ = tone_data[i];
                            *p_PCM++ = (tone_data[i+1]+tone_data[i+2])/2;
                            i+=3;
                            if(i>TONE_SAMPLE_SIZE)
                                i -= TONE_SAMPLE_SIZE;
                            else if(i==TONE_SAMPLE_SIZE)
                                break;
                        }
                        break;
                    case '6': // 853Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;)
                        {
                            *p_PCM++ = tone_data[i];
                            *p_PCM++ = tone_data[i+2];
                            *p_PCM++ = tone_data[i+4];
                            i+=5;
                            if(i>TONE_SAMPLE_SIZE)
                                i -= TONE_SAMPLE_SIZE;
                            else if(i==TONE_SAMPLE_SIZE)
                                break;
                        }
                        break;
                    case '7': // 1024Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;i+=2)
                        {
                            *p_PCM++ = tone_data[i];
                        }
                        break;
                    case '8': // 1280Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;)
                        {
                            *p_PCM++ = tone_data[i];
                            *p_PCM++ = tone_data[i+3];
                            i+=5;
                            if(i>TONE_SAMPLE_SIZE)
                                i -= TONE_SAMPLE_SIZE;
                            else if(i==TONE_SAMPLE_SIZE)
                                break;
                        }
                        break;
                    case '9': // 1536Hz
                        for(i=0;i<TONE_SAMPLE_SIZE;)
                        {
                            *p_PCM++ = tone_data[i];
                            i+=3;
                            if(i>TONE_SAMPLE_SIZE)
                                i -= TONE_SAMPLE_SIZE;
                            else if(i==TONE_SAMPLE_SIZE)
                                break;
                        }
                        break;
                }
                if(p_PCM != input->buff)
                    Mmc_Tone_Generate_Data_Size = ((uint8*)p_PCM)-input->buff;
                len = Mmc_Tone_Generate_Data_Size;
                while(len<TONE_BUFF_SIZE/2)
                {
                    memcpy(input->buff+len, input->buff, Mmc_Tone_Generate_Data_Size);
                    len += Mmc_Tone_Generate_Data_Size;
                };
            }
            
            input->index++;
            if(input->index >= input->length || input->data[input->index]==0)
            {
                input->index = 0;
                input->loop++;
            }
            output->start_time = hal_TimGetUpTime();
        }
    }
    else if(Mmc_Tone_Generate_Data_Size) // end, fill next half data data
    {
        len = TONE_BUFF_SIZE/2/Mmc_Tone_Generate_Data_Size*Mmc_Tone_Generate_Data_Size;
        //hal_HstSendEvent(SYS_EVENT,0x06120020);
        //hal_HstSendEvent(SYS_EVENT,len);
        while(len<TONE_BUFF_SIZE)
        {
            memcpy(input->buff+len, input->buff, Mmc_Tone_Generate_Data_Size);
            len += Mmc_Tone_Generate_Data_Size;
        };
        Mmc_Tone_Generate_Data_Size = 0;
    }
    
	return MCI_ERR_NO;
}

/*
* NAME:	MMC_AudioToneDecClose()
* DESCRIPTION: Close tone decoder.
*/
MCI_ERR_T MMC_AudioToneDecClose (TONE_PLAY *pTonePlayer)
{
    MCI_Play_Stream_Buffer = 0;
	aud_StreamStop(audioItf);
	mmc_MemFreeAll();
	
	return  MCI_ERR_NO;
}


extern UINT32 MCI_Play_Stream_Buffer_Length;
extern UINT8 *MCI_Play_Stream_Buffer_Address;

UINT32 MCI_SetSideTone(UINT32 sample_rate, UINT32 channel)
{
    merge_data_channel = channel;
    merge_sampleRate = sample_rate;
}

UINT32 MCI_PlaySideTone(UINT32 length, UINT8 *data, UINT32 times)
{
	TONE_INPUT *input = &TonePlayer.ToneInput;

    input->data = data;
    input->length = length;
    input->index = 0;
    input->loop = 0;
    input->times = times;
    merge_data_freq = 0;
    //merge_data_pos = 0;
    merge_data_index = 1;
    Mmc_Tone_Generate_Data_Size = 0;
    merge_data_flag = 1;

	return  MCI_ERR_NO;
}

UINT32 MCI_MergeSideTone(UINT32 length, INT16 *data)
{
	TONE_INPUT *input = &TonePlayer.ToneInput;
	TONE_OUTPUT *output = &TonePlayer.ToneOutput;
    int i,j,index,freq = 0;
    float rate;
    INT16 *p_PCM;
    INT32 data_tmp = 0;

    if(merge_data_flag == 0)
        return MCI_ERR_NO;

    for(i=0; i<length/2; i+=merge_data_channel)
    {
        if((merge_data_pos == 0 || freq == 0) && hal_TimGetUpTime()-output->start_time > 16384/10) // 0.1 second
        {
            uint8 c = input->data[input->index];

            if(input->index == 0 && input->times && input->loop >= input->times)
            {
                merge_data_flag = 0;
                return MCI_ERR_END_OF_FILE;
            }

            freq = tone_freq[c-'0']; 
            if(merge_data_freq != freq)
            {
                merge_data_freq = freq;
                if(freq == 0)
                {
                    memset(tone_sample_data, 0, MAX_SAMPLE_SIZE*2);
                    merge_data_length = MAX_SAMPLE_SIZE;
                }
                else
                {
                    CALIB_BUFFER_T* calibPtr = tgt_GetCalibConfig();
                    extern AUD_LEVEL_T audio_cfg;
                    extern AUD_ITF_T audioItf;
                    INT8 gain= calibPtr->aud.audioGains[calibPtr->aud.gainItfMap[audioItf]].sideTone[audio_cfg.spkLevel];
                    gain = gain/-3;
                    merge_data_length = merge_sampleRate/freq/4;
                    for(j=0;j<=merge_data_length;j++)
                    {
                        rate = j*8.0/merge_data_length;
                        index = (int)rate;
                        rate = rate-index;
                        tone_sample_data[j] = (float)tone_data[index]*(1.0-rate) + (float)tone_data[index+1]*rate;
                        tone_sample_data[j] = tone_sample_data[j]>>gain;
                    }
                }
            }
                
            input->index++;
            if(input->index >= input->length || input->data[input->index]==0)
            {
                input->index = 0;
                input->loop++;
            }
            if(freq == 0)
            {
                return MCI_ERR_NO;//do nothing for tone 0
            }
            output->start_time = hal_TimGetUpTime();
        }

        if(merge_data_index>0)
        {
            data_tmp = data[i] + tone_sample_data[merge_data_pos];
            data[i] += (tone_sample_data[merge_data_pos]);
            if(data_tmp > 0x7fff)
            {
                data[i] = 0x7fff;
            }
            else if(data_tmp < -0x8000)
            {
                data[i] = -0x8000;
            }
            
            if(merge_data_channel == HAL_AIF_STEREO)
            {
                data_tmp = data[i+1] + tone_sample_data[merge_data_pos];
                data[i+1] += (tone_sample_data[merge_data_pos]);
                if(data_tmp > 0x7fff)
                {
                    data[i+1] = 0x7fff;
                }
                else if(data_tmp < -0x8000)
                {
                    data[i+1] = -0x8000;
                }
            }
        }
        else
        {
            data_tmp = data[i] - tone_sample_data[merge_data_pos];
            data[i] -= (tone_sample_data[merge_data_pos]);
            if(data_tmp > 0x7fff)
            {
                data[i] = 0x7fff;
            }
            else if(data_tmp < -0x8000)
            {
                data[i] = -0x8000;
            }
            
            if(merge_data_channel == HAL_AIF_STEREO)
            {
                data_tmp = data[i+1] - tone_sample_data[merge_data_pos];
                data[i+1] -= (tone_sample_data[merge_data_pos]);
                if(data_tmp > 0x7fff)
                {
                    data[i+1] = 0x7fff;
                }
                else if(data_tmp < -0x8000)
                {
                    data[i+1] = -0x8000;
                }
            }
        }
        if(merge_data_index&1)
        {
            merge_data_pos ++;
            if(merge_data_pos>merge_data_length)
            {
                merge_data_pos = merge_data_length-1;
                merge_data_index *= 2;
            }
        }
        else
        {
            merge_data_pos --;
            if(merge_data_pos<0)
            {
                merge_data_pos = 1;
                merge_data_index = -merge_data_index/2;
            }
        }
    }
    return MCI_ERR_NO;
}

int32 Audio_TonePlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
	TONE_INPUT *input = &TonePlayer.ToneInput;

    input->data = MCI_Play_Stream_Buffer_Address;
    input->length = MCI_Play_Stream_Buffer_Length;
    input->index = 0;
    input->buff = (uint8*)pcmbuf_overlay;
    input->loop = 0;
    input->times = PlayProgress;
    memset(input->buff, 0, TONE_BUFF_SIZE);
    Mmc_Tone_Generate_Data_Size = 0;

	TonePlayer.ToneOutput.OutputPath = OutputPath;

	MMC_TonePcmOutStart(&TonePlayer);
    
	return  MCI_ERR_NO;
}

int32 Audio_ToneStop (void) 	
{
	MMC_AudioToneDecClose(&TonePlayer);	
	mmc_SetCurrMode(MMC_MODE_IDLE);
    mmc_ExitMode();
	return  MCI_ERR_NO;
}


int32 Audio_TonePause (void)	
{
    MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_TONE] Audio_TonePause!!!!");

	MMC_AudioToneDecClose(&TonePlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_ToneResume ( HANDLE fhd) 
{
	return  MCI_ERR_NO;
}


int32 Audio_ToneGetID3 (char * pFileName)  
{
	return 0;	
}

int32 Audio_ToneGetPlayInformation (MCI_PlayInf * MCI_PlayInfTONE)  
{
	MCI_PlayInfTONE->PlayProgress= 0;
	return MCI_ERR_NO;
}

int32 Audio_ToneUserMsg(int32 nMsg)
{
	switch (nMsg)
	{
		case MSG_MMC_TONE_PCM_INT:
			if(MMC_AudioToneDecPcm(&TonePlayer)==MCI_ERR_END_OF_FILE)
			{
				MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_TONE]END OF FILE!");				
                ContinuePlayFlag = 1;
			}
		    break;

		case MSG_MMC_TONE_PCM_END:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_TONE]MSG_MMC_TONE_PCM_END\n");
			MMC_AudioToneDecClose(&TonePlayer);
			MCI_AudioFinished(MCI_ERR_END_OF_FILE);
			mmc_SetCurrMode(MMC_MODE_IDLE);
			
			break;
			
		default:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_TONE_ERR]ERROR MESSAGE!");
			
			break;
	}
    return 0;
}


