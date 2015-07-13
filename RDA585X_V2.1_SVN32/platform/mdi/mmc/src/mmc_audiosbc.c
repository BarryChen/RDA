////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audioSBC.c
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
#include "sbc.h"

#include "mmc_audiosbc.h"

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
#include "vpp_sbc_dec.h"
#include "event.h"

static INT8 Mmc_SBC_Pcm_Half;

static INT32 g_SBCHeaderLength=0;

SBC_PLAY *SBCPlayer;

extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

volatile INT32 g_SBCConsumedLength=0;

static INT32 g_SBCFILESIZE=-1;
static UINT8 ContinuePlayFlag=0;//this flag controls to continue play the left data in the pcm buffer
static UINT8 g_ReadFileFlag=0;
static UINT8 g_PCMPassFlag=0;
static UINT8 ErrorFrameCount = 0;
static UINT8 g_VocBusy=0;
static UINT8 g_SampleRateChangeFlag=0;

//static sbc_t g_sbc_dec;

#define SBC_INPUT_BUFSIZE		              (4*1024)
#define SBC_MAX_OUTPUT_BUFSIZE		(9*1024)

#define SBC_MIN_INPUT_SIZE		              (2*1024)

//#define SBCDEC_DEBUG

#ifdef BT_SUPPORT

extern UINT8 MCI_Play_BTStream;
extern bt_a2dp_audio_cap_struct *MCI_BTAudioCap;

#endif

MCI_ERR_T MMC_AudioSBCDecClose (SBC_PLAY *pSBCPlayer);

/*
* NAME:	MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_AudioSBCVocISR(void)
{
  	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SBC_VOC);
} 


/*
* NAME:	MMC_AudioSBCHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/

void MMC_AudioSBCHalfPcmISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_SBC]HALF!");
	Mmc_SBC_Pcm_Half = 0;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SBC_PCM);
}

/*
* NAME:	MMC_AudioSBCEndPcmISR()
* DESCRIPTION: ISR for pcm end buffer exhausted.
*/
void MMC_AudioSBCEndPcmISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_SBC]END!");
	Mmc_SBC_Pcm_Half = 1;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SBC_PCM);
}


static const uint16 PCMSamplesRate[]={48000,44100,32000,24000,22050,16000,12000,11025,8000};


MCI_ERR_T MMC_SBCHeaderParse(SBC_PLAY *pSBCPlayer)
{
	SBC_INPUT *input = &pSBCPlayer->SBCInput;
	SBC_OUTPUT *output = &pSBCPlayer->SBCOutput;

	return MCI_ERR_NO;

}


void MMC_SBCPcmOutStart(SBC_PLAY *pSBCPlayer)
{
    hal_HstSendEvent(SYS_EVENT,0x10250020);
	
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_SBC]MMC_SBCPcmOutStart BEGIN!");
// Stream out
	audioStream.startAddress  	= (UINT32 *)pSBCPlayer->SBCOutput.pcm.Buffer;
	audioStream.length        	= pSBCPlayer->SBCOutput.pcm.Size*4;//MP3DEC_PCM_OUTPUT_BUFFER_SIZE;//AIF_FIFO_LEN * 4;
	audioStream.channelNb     	= pSBCPlayer->SBCOutput.pcm.Channel;//HAL_AIF_STEREO;//HAL_AIF_MONO;
	
	audioStream.voiceQuality   = !TRUE;    
	audioStream.halfHandler   	= MMC_AudioSBCHalfPcmISR;
	audioStream.endHandler    = MMC_AudioSBCEndPcmISR;

    // Initial cfg
    if(gpio_detect_earpiece())
        audioItf = AUD_ITF_BT_EP;
    else
        audioItf = AUD_ITF_BT_SP;

	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_SBC]SAMPLERATE:%d",pSBCPlayer->SBCOutput.pcm.SampleRate);

	switch (pSBCPlayer->SBCOutput.pcm.SampleRate)
	{
	case SBC_SAMPLING_FREQ_48000:
  		audioStream.sampleRate    = HAL_AIF_FREQ_48000HZ;
		break;
	case SBC_SAMPLING_FREQ_44100:
		audioStream.sampleRate    = HAL_AIF_FREQ_44100HZ;
		break;
	case SBC_SAMPLING_FREQ_32000:
		audioStream.sampleRate    = HAL_AIF_FREQ_32000HZ;
		break;
	case SBC_SAMPLING_FREQ_16000:
		audioStream.sampleRate    = HAL_AIF_FREQ_16000HZ;
		break;
	default:
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MMC_SBC_ERROR]ERROR SAMPLERATE:%d",pSBCPlayer->SBCOutput.pcm.SampleRate);

		break;
	}
    hal_HstSendEvent(SYS_EVENT,0x10250021);
    hal_HstSendEvent(SYS_EVENT,pSBCPlayer->SBCOutput.pcm.SampleRate);
    hal_HstSendEvent(SYS_EVENT,audioStream.sampleRate);
    hal_HstSendEvent(SYS_EVENT,audioStream.channelNb);
        //COS_EnterCriticalSection();
	//while(1);
	aud_StreamStart(audioItf, &audioStream, &audio_cfg);
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_SBC] END!");

}


PUBLIC MCI_ERR_T MMC_SBCGetFileInformation (
                        CONST INT32 FileHander,
                         AudDesInfoStruct  * CONST DecInfo  )
{

  SBC_PLAY SBCPlay;
 SBC_INPUT *input = &SBCPlay.SBCInput;
  int32 result;

   //judge the file handler
	input->fileHandle = FileHander;
	if (input->fileHandle<0)
	{
		diag_printf("[MMC_SBC]MMC_SBCGetFileInformation FILE HANDLER ERROR!: %d \n", FileHander);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}
	else
	{
		diag_printf("[MMC_SBC] MMC_SBCGetFileInformation FILE HANDLER: %d\n", FileHander);
	}
	
       g_SBCHeaderLength=0;
 
  	//parse SBC file header
	if((result= MMC_SBCHeaderParse(&SBCPlay))!=MCI_ERR_NO)
	{
		return result;
	}

									// get the infromation.
	DecInfo->sampleRate             = SBCPlay.SBCOutput.pcm.SampleRate;
	DecInfo->stereo                     = SBCPlay.SBCOutput.pcm.Channel;

    diag_printf("[MMC_SBC]MMC_SBCGetFileInformation DecInfo->sampleRate : %d , DecInfo->stereo : %d\n", DecInfo->sampleRate,DecInfo->stereo );
					
      return MCI_ERR_NO;

}




/*
* NAME:	MMC_AudioSBCDecOpen()
* DESCRIPTION: Initialize SBC decoder.
*/
MCI_ERR_T MMC_AudioSBCDecOpen (HANDLE fhd, SBC_PLAY *pSBCPlayer)
{
    if(pSBCPlayer == NULL)
        pSBCPlayer = SBCPlayer = COS_Malloc(sizeof(SBC_PLAY));
	SBC_INPUT *input = &pSBCPlayer->SBCInput;
	SBC_OUTPUT *output = &pSBCPlayer->SBCOutput;
	vpp_SBC_DEC_IN_T *pVoc_Voc_AudioDecIN = &pSBCPlayer->Voc_AudioDecIN;

        HAL_ERR_T voc_ret = 0;
	
	int32 readlen = 0;
	int32 result;

        hal_HstSendEvent(SYS_EVENT,0x10250002);
//#ifdef TARGET_A2DPDISK_SUPPORT
        hal_SysSetVocClock(HAL_SYS_VOC_FREQ_156M);
//#endif
        ContinuePlayFlag=0;
        g_ReadFileFlag=0;
        g_PCMPassFlag=0;
        ErrorFrameCount = 0;

    output->buff_offset = 0;
	output->pcm.Buffer= pcmbuf_overlay;
	memset(output->pcm.Buffer, 0, SBC_MAX_OUTPUT_BUFSIZE);
	output->pcm.Buffer_head=output->pcm.Buffer;
       output->pcm.Channel=2;
	   g_SampleRateChangeFlag = 0;
        input->data = (uint8 *)inputbuf_overlay;
        memset(input->data, 0, SBC_INPUT_BUFSIZE);
        input->length = SBC_INPUT_BUFSIZE;
        input->inFlag=0;

        //hal_HstSendEvent(SYS_EVENT,0x10250011);
        //hal_HstSendEvent(SYS_EVENT,output->pcm.Buffer);
        //hal_HstSendEvent(SYS_EVENT,input->data);
        //sbc_init(&g_sbc_dec, 0);
        //g_sbc_dec.endian = SBC_LE;
        //hal_HstSendEvent(SYS_EVENT,0x10250012);
	
#ifdef BT_SUPPORT
        //hal_HstSendEvent(SYS_EVENT,MCI_Play_BTStream);
        //hal_HstSendEvent(SYS_EVENT,MCI_BTAudioCap);
       if(MCI_Play_BTStream == 1)
       {
              output->pcm.Bits = 16;
              output->pcm.SampleRate = MCI_BTAudioCap->codec_cap.sbc.sample_rate;
              output->pcm.Channel = MCI_BTAudioCap->codec_cap.sbc.channel_mode==SBC_CHANNEL_MODE_MONO?1:2;
              output->pcm.Size = (SBC_MAX_OUTPUT_BUFSIZE) >> 2;

            Avdtp_Clean_Data();

              input->length = 0;
        }
       else
#endif   
       if(0){// disable sbc file play
	//judge the file handler
	input->fileHandle = fhd;
	if (input->fileHandle<0)
	{
		diag_printf("[MMC_SBC]FILE HANDLER ERROR!: %d \n", fhd);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}
	else
	{
		diag_printf("[MMC_SBC] FILE HANDLER: %d\n", fhd);
	}

	g_SBCHeaderLength=0;
	
	//parse SBC file header
	if((result= MMC_SBCHeaderParse(pSBCPlayer))!=MCI_ERR_NO)
	{
		return result;
	}

	g_SBCConsumedLength=g_SBCConsumedLength-g_SBCHeaderLength;

	if(g_SBCConsumedLength<0)g_SBCConsumedLength=0;

	g_SBCConsumedLength=g_SBCConsumedLength&0xfffffffC;

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_SBC]g_SBCConsumedLength :%d",g_SBCConsumedLength);
	
	result=FS_Seek(input->fileHandle,g_SBCConsumedLength,FS_SEEK_CUR);

	/*prepare input stream*/
	readlen = FS_Read(input->fileHandle, input->data, input->length);

    hal_HstSendEvent(SYS_EVENT,0x12130080);
    hal_HstSendEvent(SYS_EVENT,g_SBCConsumedLength);
    hal_HstSendEvent(SYS_EVENT,input->data);
    hal_HstSendEvent(SYS_EVENT,input->length);
    hal_HstSendEvent(SYS_EVENT,readlen);
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_MP3AAC] First Read Len=%d ;Need Length=%d\n", readlen,input->length);
	
	if(readlen<0)
	{
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

	//input->fileOffset=g_SBCConsumedLength+g_SBCHeaderLength;

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_SBC]FILE SEEK POSITION:%d",result);
	
	if(result<0 )
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_SBC_ERR]FS_seek ERROR!");
		return MCI_ERR_BAD_FORMAT;
	}
	
	//MMC_SBCPcmOutStart(pSBCPlayer);
    }
	   
	voc_ret = vpp_SBCDecOpen((HAL_VOC_IRQ_HANDLER_T)MMC_AudioSBCVocISR);

	hal_HstSendEvent(SYS_EVENT,0x88100000+voc_ret);
	 
	switch (voc_ret)
	{
	case HAL_ERR_NO:
		break;
		
	case HAL_ERR_RESOURCE_RESET:
	case HAL_ERR_RESOURCE_BUSY:  
	case HAL_ERR_RESOURCE_TIMEOUT:
	case HAL_ERR_RESOURCE_NOT_ENABLED:	  
	case HAL_ERR_BAD_PARAMETER:
	case HAL_ERR_UART_RX_OVERFLOW:
	case HAL_ERR_UART_TX_OVERFLOW:
	case HAL_ERR_UART_PARITY:
	case HAL_ERR_UART_FRAMING:
	case HAL_ERR_UART_BREAK_INT:
	case HAL_ERR_TIM_RTC_NOT_VALID:
	case HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED:
	case HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED:
	case HAL_ERR_COMMUNICATION_FAILED:
	case HAL_ERR_QTY:
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_MP3AAC_ERROR]Fail to Call vpp_AudioJpegDecOpen()t: %d \n", voc_ret);
		return MCI_ERR_ERROR;
	default:
		break;
	}
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_AudioDecOpen end!");

        // setup first frame config
        
        
	pVoc_Voc_AudioDecIN->inStreamBufAddr=(INT32 *)input->data;
	pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)output->pcm.Buffer;
	pVoc_Voc_AudioDecIN->mode=MMC_SBC_DEC;
	pVoc_Voc_AudioDecIN->audioItf=audioItf;
    pVoc_Voc_AudioDecIN->reset = 1;

	return MCI_ERR_NO;
	
}

void Audio_SBCPrepareNextFrame(SBC_PLAY *pAudioPlayer)
{
	SBC_OUTPUT *output = &pAudioPlayer->SBCOutput;
	vpp_SBC_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;

        // prepare for nect frame decode
        if(Mmc_SBC_Pcm_Half==1) // end
        {
            output->pcm.Buffer_head=output->pcm.Buffer;
        }
        else if(Mmc_SBC_Pcm_Half==0) // half
        {
            output->pcm.Buffer_head=output->pcm.Buffer+(output->pcm.Size>>1);           
        }
        pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)output->pcm.Buffer_head;
#ifdef SBCDEC_DEBUG
		if((UINT32)output->pcm.Buffer_head < (UINT32)pcmbuf_overlay || (UINT32)output->pcm.Buffer_head > ((UINT32)pcmbuf_overlay + SBC_MAX_OUTPUT_BUFSIZE))
		{
			hal_HstSendEvent(SYS_EVENT,0x12010062);
			hal_DbgAssert("SBCDecPcmOutAddr is error!");  
		}
#endif
}

#ifdef BT_SUPPORT
MCI_ERR_T  MMC_AudioReceiveA2dpData(SBC_PLAY *pSBCPlayer)
{
	SBC_INPUT *input = &pSBCPlayer->SBCInput;
	SBC_OUTPUT *output = &pSBCPlayer->SBCOutput;
	vpp_SBC_DEC_IN_T *pVoc_Voc_AudioDecIN = &pSBCPlayer->Voc_AudioDecIN;
	int32 i,readlen;
       uint32 sbc_length, frame_len, frames, length;

#if 0
        hal_HstSendEvent(SYS_EVENT,0x10250030);
        hal_HstSendEvent(SYS_EVENT,input->data);
        hal_HstSendEvent(SYS_EVENT,input->length);
        hal_HstSendEvent(SYS_EVENT,input->inFlag);

        do{
            Avdtp_Get_Data(&sbc_length, input->data, SBC_INPUT_BUFSIZE);
            hal_HstSendEvent(SYS_EVENT,sbc_length);
        }while(sbc_length);
        return;
#endif

        if(g_ReadFileFlag)
            return 0;
       
        if(input->length < SBC_INPUT_BUFSIZE)
        {
            Avdtp_Get_Data(&sbc_length, input->data+input->length, SBC_INPUT_BUFSIZE-input->length);
            input->length += sbc_length;
        }

        if(input->inFlag!=2)
        {
            if(input->length < SBC_INPUT_BUFSIZE)
                return MCI_ERR_NO;
        }
        
        if(input->length < SBC_MIN_INPUT_SIZE)
            return MCI_ERR_NO;
        
        if(g_PCMPassFlag)
        {
            if(g_VocBusy==0)
            {
                vpp_SBCDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
                g_VocBusy = 1;
                g_ReadFileFlag = 0;
                g_PCMPassFlag = 0;

                // prepare for nect frame decode
                Audio_SBCPrepareNextFrame(pSBCPlayer);
            }
            else
                g_ReadFileFlag = 1;
        }
        else
            g_ReadFileFlag = 1;

	return MCI_ERR_NO;

}
#endif

MCI_ERR_T MMC_AudioSBCDecVoc(SBC_PLAY *pSBCPlayer)
{
	SBC_INPUT *input = &pSBCPlayer->SBCInput;
	SBC_OUTPUT *output = &pSBCPlayer->SBCOutput;
	vpp_SBC_DEC_OUT_T *pVoc_AudioDecStatus = &pSBCPlayer->Voc_AudioDecStatus;
	vpp_SBC_DEC_IN_T *pVoc_Voc_AudioDecIN = &pSBCPlayer->Voc_AudioDecIN;
	int32 i,readlen;
       uint32 length;
    extern UINT16 *g_last_buffer_addr;

    //hal_HstSendEvent(SYS_EVENT,0x12010010);
    //hal_HstSendEvent(SYS_EVENT,input->inFlag);
    g_VocBusy = 0;

#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 1)
    {
       uint32 sbc_length, frame_len, frames;//, length;
       int32 length;
        if(input->inFlag==0)
        {
            input->inFlag = 1;
            g_PCMPassFlag = 1;
			
			vpp_SBCDecCheckOutPcmAddr(output->pcm.Buffer,output->pcm.Buffer+(SBC_MAX_OUTPUT_BUFSIZE/4));
            return MCI_ERR_NO;
        }
    	vpp_SBCDecStatus(pVoc_AudioDecStatus);
        MCI_MergeSideTone(pVoc_AudioDecStatus->sample_length, g_last_buffer_addr);

//    hal_HstSendEvent(SYS_EVENT,0x12010030);
//    hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->consumedLen);
//    hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->sample_length);
//    hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->ErrorStatus);
        MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AudioSBCDecVoc] consumedLen=%d bytes;sample_length=%d bytes,ErrorStatus=%d bytes\n", pVoc_AudioDecStatus->consumedLen,pVoc_AudioDecStatus->sample_length,pVoc_AudioDecStatus->ErrorStatus);        
        if(input->inFlag==1)
        {
            if(pVoc_AudioDecStatus->ErrorStatus==0
				&& !(pVoc_AudioDecStatus->sample_length%4)
				&&pVoc_AudioDecStatus->consumedLen>0)//11251
	        {
	            // setup ouput buffer size as dec frame sise
	            if(output->pcm.SampleRate != pVoc_AudioDecStatus->freq)
	            {
	                hal_HstSendEvent(SYS_EVENT,0x11140030);
	                hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->freq);
	                g_SampleRateChangeFlag = 1;
	            }
	            output->pcm.Size = pVoc_AudioDecStatus->sample_length >> 1;
	            output->pcm.SampleRate = pVoc_AudioDecStatus->freq;
				output->pcm.Channel = pVoc_AudioDecStatus->channels;
	            hal_HstSendEvent(SYS_EVENT,output->pcm.Size);
	            hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->freq);
	            hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->blocks);
				hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->channels);
				hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->subbands);
				hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->bitpool);
				hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->blocks);
				
#ifdef A2DP_SAMPLE_44_1K_TO_48K
				if(pVoc_AudioDecStatus->freq==SBC_SAMPLING_FREQ_44100 && pVoc_AudioDecStatus->channels==2)
				{
					output->pcm.SampleRate=SBC_SAMPLING_FREQ_48000;
					hal_HstSendEvent(SYS_EVENT,0x19891001);
				}
#endif
	            
#ifndef TARGET_A2DPDISK_SUPPORT
	            	MMC_SBCPcmOutStart(pSBCPlayer); 
#endif
					input->inFlag = 2;
					ErrorFrameCount = 0;

	         }
			else
			{
				MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AudioSBCDecVoc] error!");
				ErrorFrameCount++;
				if(ErrorFrameCount>50)
				{
					MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MMC_SBCDEC_ERROR]ErrorFrameCount:%d ",ErrorFrameCount);
					return MCI_ERR_ERROR;				
				}

			}
	
        }

        if(pVoc_AudioDecStatus->ErrorStatus != 0 || pVoc_AudioDecStatus->consumedLen<=0 || pVoc_AudioDecStatus->sample_length%4)
        {
            hal_HstSendEvent(SYS_EVENT,0x12010060);
			hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->ErrorStatus);
			
#ifdef SBCDEC_DEBUG
			if(pVoc_AudioDecStatus->ErrorStatus == 9)
			{
            	hal_HstSendEvent(SYS_EVENT,0x12010061);
				hal_DbgAssert("SBCDecPcmOutAddr is error!");  
			}
#endif
            // find next packet
            length = (pVoc_AudioDecStatus->consumedLen >0)? pVoc_AudioDecStatus->consumedLen + 1 : 1;
            while(length < input->length)
            {
                if(input->data[length] == 0x9c)
                    break;
                length++;
            };
            if(length < input->length)
            {
                memcpy(input->data, input->data+length, input->length-length);
				input->length -= length;
            }
			else
			{
				input->length = 0;
			}
			
			hal_HstSendEvent(SYS_EVENT,length);
            hal_HstSendEvent(SYS_EVENT,input->length);
        }
        else
        {
            length = input->length-pVoc_AudioDecStatus->consumedLen;
            if(length > 0)
            {
                memcpy(input->data, input->data+pVoc_AudioDecStatus->consumedLen, length);
            }
            else
                length = 0;
            input->length = length;

        }

        if(input->length < SBC_INPUT_BUFSIZE)
        {
            Avdtp_Get_Data(&sbc_length, input->data+input->length, SBC_INPUT_BUFSIZE-input->length);
            input->length += sbc_length;
        }
        if(input->length >= SBC_MIN_INPUT_SIZE)
        {
            if(g_PCMPassFlag)
            {
                vpp_SBCDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
                g_VocBusy = 1;
                g_ReadFileFlag = 0;
                g_PCMPassFlag = 0;

                // prepare for nect frame decode
                Audio_SBCPrepareNextFrame(pSBCPlayer);
            }
            else
                g_ReadFileFlag = 1;        
        }
    }        
    else
#endif	
  if(0){ // disable sbc file play
    if(input->inFlag==0)
    {
        input->inFlag = 1;

        // decode first frame
        vpp_SBCDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
        return MCI_ERR_NO;
    }

	vpp_SBCDecStatus(pVoc_AudioDecStatus);

#if 0
    hal_HstSendEvent(SYS_EVENT,0x12010030);
    hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->consumedLen);
    hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->sample_length);
    hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->ErrorStatus);
#endif

    if(input->inFlag==1)
    {
       // return MCI_ERR_NO;

        if(pVoc_AudioDecStatus->ErrorStatus!=0)
            return MCI_ERR_ERROR;
        
        g_SBCConsumedLength += pVoc_AudioDecStatus->consumedLen;
        if(g_SBCConsumedLength>=g_SBCFILESIZE)
            return MCI_ERR_END_OF_FILE;

        // setup ouput buffer size as dec frame sise
        output->pcm.Size = pVoc_AudioDecStatus->sample_length >> 1;
        output->pcm.SampleRate = pVoc_AudioDecStatus->freq;
        output->pcm.Channel = pVoc_AudioDecStatus->channels==SBC_CHANNEL_MODE_MONO?1:2;
        hal_HstSendEvent(SYS_EVENT,output->pcm.Size);
        hal_HstSendEvent(SYS_EVENT,output->pcm.SampleRate);
        hal_HstSendEvent(SYS_EVENT,output->pcm.Channel);

        length = input->length-pVoc_AudioDecStatus->consumedLen;
        if(length > 0)
        {
            if(length <= SBC_INPUT_BUFSIZE)
            {
                 memcpy(input->data, input->data+pVoc_AudioDecStatus->consumedLen, length);
            }
            else
            {
                memcpy(input->data, input->data+pVoc_AudioDecStatus->consumedLen, SBC_INPUT_BUFSIZE);
                FS_Seek(input->fileHandle,length-SBC_INPUT_BUFSIZE,FS_SEEK_CUR);
                length = 0;
            }
        }
        else
        {
            FS_Seek(input->fileHandle,-length,FS_SEEK_CUR);
            length = 0;
        }
        length = FS_Read(input->fileHandle, input->data+length, input->length-length);
        //hal_HstSendEvent(SYS_EVENT,length);
        if(length<0)
        {
            return MCI_ERR_END_OF_FILE;
        }
        pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)(output->pcm.Buffer+(output->pcm.Size>>1));

        vpp_SBCDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
    
        pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)output->pcm.Buffer;
        
        MMC_SBCPcmOutStart(pSBCPlayer);    
        input->inFlag = 2;
        
        return MCI_ERR_NO;
    }
   // return MCI_ERR_NO;
    //    pcm_count++;
     //  if(pcm_count>1)
    //    return MCI_ERR_NO;    
    if(pVoc_AudioDecStatus->consumedLen > 0)
    {
        g_SBCConsumedLength += pVoc_AudioDecStatus->consumedLen;
        //if(g_SBCConsumedLength>=g_SBCFILESIZE)
        //    return MCI_ERR_END_OF_FILE;
        length = input->length-pVoc_AudioDecStatus->consumedLen;
        if(length > 0)
        {   
            uint8 temp;
            memcpy(input->data, input->data+pVoc_AudioDecStatus->consumedLen, length);
            //hal_HstSendEvent(SYS_EVENT,0x12010040);
            //hal_HstSendEvent(SYS_EVENT,length);
     
        }
        else if(length < 0)
        {
            FS_Seek(input->fileHandle,-length,FS_SEEK_CUR);
            length = 0;
        }
        
		length = FS_Read(input->fileHandle, input->data+length, input->length-length);
        //hal_HstSendEvent(SYS_EVENT,length);
		if(length<0)
		{
			return MCI_ERR_END_OF_FILE;
		}

    }  

        if(pVoc_AudioDecStatus->ErrorStatus==0)
		{

            ErrorFrameCount=0;
		}
		else
		{
			ErrorFrameCount++;
			if(ErrorFrameCount>100)
			{
				MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MMC_SBCDEC_ERROR]ErrorFrameCount:%d ",ErrorFrameCount);
				return MCI_ERR_ERROR;				
			}
		}

        if(g_PCMPassFlag)
        {
            vpp_SBCDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
            g_VocBusy = 1;
            g_ReadFileFlag = 0;
            g_PCMPassFlag = 0;

            // prepare for nect frame decode
            Audio_SBCPrepareNextFrame(pSBCPlayer);
        }
        else
            g_ReadFileFlag = 1;
    }
	return MCI_ERR_NO;
    
}


/*
* NAME:	MMC_AudioSBCDecPcm()
* DESCRIPTION: Initialize SBC decoder.
*/
MCI_ERR_T MMC_AudioSBCDecPcm(SBC_PLAY *pSBCPlayer)
{
	SBC_INPUT *input = &pSBCPlayer->SBCInput;
	SBC_OUTPUT *output = &pSBCPlayer->SBCOutput;
	vpp_SBC_DEC_IN_T *pVoc_Voc_AudioDecIN = &pSBCPlayer->Voc_AudioDecIN;
	int32 i,readlen;
       uint32 length;

	if(ContinuePlayFlag)
	{
		//hal_HstSendEvent(SYS_EVENT,0x65900033);
		diag_printf("!!!!!play all left data in the pcm buffer\n");
		MMC_AudioSBCDecClose(&pSBCPlayer);
		MCI_AudioFinished(MCI_ERR_END_OF_FILE);
	
		mmc_SetCurrMode(MMC_MODE_IDLE);
		ContinuePlayFlag = 0;
	}

#ifdef BT_SUPPORT
    if((!g_ReadFileFlag) && MCI_Play_BTStream == 1)
    {
        g_PCMPassFlag = 0;
        MMC_AudioReceiveA2dpData(pSBCPlayer);
    }
#endif

    if(g_ReadFileFlag)
    {
        // file data read, decode one frame
        if(g_VocBusy == 0)
        {
            vpp_SBCDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
            g_VocBusy = 1;
            g_PCMPassFlag = 0;
            g_ReadFileFlag = 0;
            // prepare for nect frame decode
            Audio_SBCPrepareNextFrame(pSBCPlayer);
        }
        else
            g_PCMPassFlag = 1;
    }
    else
    {
        // set flag
        g_PCMPassFlag = 1;
        hal_HstSendEvent(SYS_EVENT,0x88400001);
        memset(output->pcm.Buffer_head, 0, output->pcm.Size<<1);
        Audio_SBCPrepareNextFrame(pSBCPlayer);
    }
    return MCI_ERR_NO;

}

/*
* NAME:	MMC_AudioSBCDecClose()
* DESCRIPTION: Close SBC decoder.
*/
MCI_ERR_T MMC_AudioSBCDecClose (SBC_PLAY *pSBCPlayer)
{
    hal_HstSendEvent(SYS_EVENT,0x10250101);

    if(pSBCPlayer == NULL)
        return MCI_ERR_NO;
    pSBCPlayer->SBCInput.inFlag = 0;
    vpp_SBCDecClose();
//#ifdef TARGET_A2DPDISK_SUPPORT
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_78M);
//#endif
	aud_StreamStop(audioItf);
	hal_HstSendEvent(SYS_EVENT, 0x08220001);
       //sbc_finish(&g_sbc_dec);
    hal_HstSendEvent(SYS_EVENT, 0x08220000);
	mmc_MemFreeAll();
    hal_HstSendEvent(SYS_EVENT, 0x08220002);
#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 1)
        Avdtp_Stop_Data();
#endif    
    COS_Free(SBCPlayer);
    SBCPlayer = NULL;
    
	return  MCI_ERR_NO;
}


#ifdef TARGET_A2DPDISK_SUPPORT

MCI_ERR_T Audio_SBCReadData(INT16 *buff, UINT32 length)
{
    SBC_PLAY *pSBCPlayer = SBCPlayer;
	SBC_OUTPUT *output = &pSBCPlayer->SBCOutput;
	SBC_INPUT *input = &pSBCPlayer->SBCInput;
    INT32 next_frame = 0;

    //hal_HstSendEvent(SYS_EVENT,0x11090010);
    //hal_HstSendEvent(SYS_EVENT,length);    
    if(g_SampleRateChangeFlag)
    {
        g_SampleRateChangeFlag = 0;
        hal_HstSendEvent(SYS_EVENT,0x11140020);
        return MCI_ERR_END_OF_FILE;
    }
    
    if(input->inFlag != 2)
    {
        memset(buff, 0, length);
        return MCI_ERR_NO;
    }

    if(Mmc_SBC_Pcm_Half == 1 && output->buff_offset + length >= SBC_MAX_OUTPUT_BUFSIZE/2)
    {
        next_frame = 1;
        //Mmc_SBC_Pcm_Half = 0;
    }
    else if(Mmc_SBC_Pcm_Half == 0 && output->buff_offset + length >= SBC_MAX_OUTPUT_BUFSIZE)
    {
        //Mmc_SBC_Pcm_Half = 1;
        next_frame = 1;
    }

    if(next_frame)
    {
        if(g_VocBusy==1)
        {
            hal_HstSendEvent(SYS_EVENT,0x11120010);
        }
    }

    if(output->buff_offset + length >= SBC_MAX_OUTPUT_BUFSIZE)
    {
        memcpy(buff, output->pcm.Buffer + output->buff_offset/4, SBC_MAX_OUTPUT_BUFSIZE-output->buff_offset);
        output->buff_offset = length+output->buff_offset-SBC_MAX_OUTPUT_BUFSIZE;
        if(output->buff_offset > 0)
            memcpy(buff, output->pcm.Buffer, length - output->buff_offset);
    }
    else
    {
        memcpy(buff, output->pcm.Buffer + output->buff_offset/4, length);
        output->buff_offset += length;
    }
    //hal_HstSendEvent(SYS_EVENT,next_frame);    
    //hal_HstSendEvent(SYS_EVENT,Mmc_SBC_Pcm_Half);    
    //hal_HstSendEvent(SYS_EVENT,output->buff_offset);    
    

    if(next_frame)
    {
        Mmc_SBC_Pcm_Half = !Mmc_SBC_Pcm_Half;
        MMC_AudioSBCDecPcm(pSBCPlayer);
    }
    
	return  MCI_ERR_NO;
}

int32 Audio_SBCGetSampleRate(void)
{
	SBC_OUTPUT *output = &SBCPlayer->SBCOutput;
	SBC_INPUT *input = &SBCPlayer->SBCInput;
    int32 sample_rate;
	switch (output->pcm.SampleRate)
	{
	case SBC_SAMPLING_FREQ_48000:
  		sample_rate   = HAL_AIF_FREQ_48000HZ;
		break;
	case SBC_SAMPLING_FREQ_44100:
		sample_rate   = HAL_AIF_FREQ_44100HZ;
		break;
	case SBC_SAMPLING_FREQ_32000:
		sample_rate   = HAL_AIF_FREQ_32000HZ;
		break;
	case SBC_SAMPLING_FREQ_16000:
		sample_rate   = HAL_AIF_FREQ_16000HZ;
		break;
	default:
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MMC_SBC_ERROR]ERROR SAMPLERATE:%d",output->pcm.SampleRate);
		sample_rate   = HAL_AIF_FREQ_44100HZ;
		break;
	}
    return sample_rate;
}

#endif

int32 Audio_SBCPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
	int32 result;

    hal_HstSendEvent(SYS_EVENT,0x10250001);
    if(SBCPlayer == NULL)
        SBCPlayer = COS_Malloc(sizeof(SBC_PLAY));

	g_SBCConsumedLength=0;

#ifdef BT_SUPPORT
       if(0) // disable sbc file play MCI_Play_BTStream == 0)
#endif   
    {
	if (fhd< 0)
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_ERROR]ERROR FILE HANDLER: %d \n", fhd);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

	if ((g_SBCFILESIZE=FS_GetFileSize(fhd))< 0)
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_ERROR]CAN NOT GET FILE SIZE!:%d",g_SBCFILESIZE);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_AUDIO_SBC]FILE SIZE: %d",g_SBCFILESIZE);

	g_SBCConsumedLength=(INT32)(((INT64)PlayProgress*(INT64)g_SBCFILESIZE)/10000);

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_SBC]g_AMRConsumedLength: %d",g_SBCConsumedLength);

	if ((result=FS_Seek(fhd,g_SBCConsumedLength,FS_SEEK_SET))< 0)
	{
		diag_printf("[MMC_AUDIO_ERROR]can not seek file!:%d",result);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}
    }
	if((result= MMC_AudioSBCDecOpen(fhd, SBCPlayer))!=MCI_ERR_NO)
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_SBC_ERR]MMC_AudioDecOpen() RETURN FALSE!");
		MMC_AudioSBCDecClose(SBCPlayer);
		return result;
	}
	
	return  MCI_ERR_NO;
}

int32 Audio_SBCStop (void) 	
{
	MMC_AudioSBCDecClose(SBCPlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_SBCPause (void)	
{
MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_SBC] Audio_SBCPause!!!!");

	MMC_AudioSBCDecClose(SBCPlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_SBCResume ( HANDLE fhd) 
{
	int32 result;
	
	MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_SBC]Audio_SBCResume!");
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_SBC] Audio_SBCResume!!!!");
#if 0
	g_SBCConsumedLength-=(SBC_INPUT_BUFSIZE);

	if(g_SBCConsumedLength<0)g_SBCConsumedLength=0;

	result=FS_Seek(fhd,0,FS_SEEK_SET);

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_SBC]FILE SEEK POSITON:%d",result);
	
	if(result<0 )
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_SBC_ERR]FS_seek error!");
		return MCI_ERR_BAD_FORMAT;
	}


	if((result= MMC_AudioSBCDecOpen(fhd, SBCPlayer))!=MCI_ERR_NO)
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_SBC_ERR]MMC_AudioDecOpen RETURN FALSE!");
		MMC_AudioSBCDecClose(SBCPlayer);
		return result;
	}
#endif
	return  MCI_ERR_NO;
}


int32 Audio_SBCGetID3 (char * pFileName)  
{
	return 0;	
}

int32 Audio_SBCGetPlayInformation (MCI_PlayInf * MCI_PlayInfSBC)  
{
    if(g_SBCFILESIZE == 0)
        MCI_PlayInfSBC->PlayProgress = 0 ;
    else
    	MCI_PlayInfSBC->PlayProgress=(INT32)((((INT64)g_SBCConsumedLength+g_SBCHeaderLength)*10000)/g_SBCFILESIZE);
	
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_SBC]PlayProgress:%d",MCI_PlayInfSBC->PlayProgress);
	
	return MCI_ERR_NO;
}

int32 Audio_SBCUserMsg(int32 nMsg)
{
    //hal_HstSendEvent(SYS_EVENT,0x10250020);
    //hal_HstSendEvent(SYS_EVENT,nMsg);

	switch (nMsg)
	{
	    case MSG_MMC_SBC_VOC:
			switch (MMC_AudioSBCDecVoc(SBCPlayer))
			{
			case MCI_ERR_END_OF_FILE:
				diag_printf("[MMC_AUDIO]MMC_AACDecRun() return FINISH!\n");

				ContinuePlayFlag = 1;
				break;
			case MCI_ERR_ERROR:
				hal_HstSendEvent(SYS_EVENT,0x65900022);
				diag_printf("[MMC_AUDIO]##WARNING##MMC_AACDecRun() return FALSE!\n");
                
				MMC_AudioSBCDecClose(SBCPlayer);
				MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
				mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
				ContinuePlayFlag = 0;
				break;
			default:
				break;
			}
		break;
            
    
#ifndef TARGET_A2DPDISK_SUPPORT
		case MSG_MMC_SBC_PCM:
			if(MMC_AudioSBCDecPcm(SBCPlayer)==MCI_ERR_END_OF_FILE)
			{
				
				MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_SBC]END OF FILE!");				
			}
			break;
#endif

		case MSG_MMC_SBC_END:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_SBC]MSG_MMC_SBC_END\n");
			MMC_AudioSBCDecClose(SBCPlayer);
			MCI_AudioFinished(MCI_ERR_END_OF_FILE);
			mmc_SetCurrMode(MMC_MODE_IDLE);
			break;

#ifdef BT_SUPPORT
              case MSG_MMC_SBC_A2DP:
                    MMC_AudioReceiveA2dpData(SBCPlayer);
                    break;
#endif

		default:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_SBC_ERR]ERROR MESSAGE!");
			
			break;
	}
    return 0;
}


