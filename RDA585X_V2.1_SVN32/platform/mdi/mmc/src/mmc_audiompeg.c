
////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audiompeg.c
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   xuml
//
////////////////////////////////////////////////////////////////////////////////

#include "cswtype.h"
#include "mcip_debug.h"
#include "fs.h"
#include "mmc_audiompeg.h"
#include "hal_error.h"

#include "cpu_share.h"
#include "assert.h"
#include "hal_voc.h"

#include "global.h"
#include "audio_api.h"

#include "aud_m.h"

#include "fs_asyn.h"

#include "mci.h"
#include "cos.h"

#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"

#include "string.h"
#include "hal_overlay.h"

#include "sxr_tls.h"

#include "resample.h"
#include "event.h"


#define AudioJpeg_INPUT_BUFSIZE		(7*1024)

#define  MIN_INPUT_REMAIN_SIZE   (2*1024)

#define AUDIO_MAX_OUTPUT_BUFSIZE (1152*4)
#define AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE (1152*4*2) 


#define AACDEC_OUTPUT_BUFSIZE (1024*4)
#define AACDEC_PCM_OUTPUT_BUFFER_SIZE (1024*4*2) 

#define MP3DEC_OUTPUT_BUFSIZE (1152*4)
#define MP3DEC_PCM_OUTPUT_BUFFER_SIZE (1152*4*2) 
#define MP3_MAX_ERROR_CNT    10
extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

MPEG_PLAY *MpegPlayer;

static INT8 Mmc_Audio_Pcm_Half;
static UINT8 ContinuePlayFlag=0;//this flag controls to continue play the left data in the pcm buffer

static UINT8 g_ReadFileFlag=0;
static UINT8 g_PCMPassFlag=0;
static UINT8 g_GetInfoFlag=0;
static UINT8 g_DecOpenFlag=0;
static UINT8 FrameCount=0;
static UINT8 ErrorFrameCount = 0;
static UINT8 Frame_exit= 0;
static INT32 g_ConsumedLength=0;
static UINT8 g_AudioMode=0;
static UINT8 g_AudioSeekFlag=0;

static INT32 g_MPEGFILESIZE=0;
//static INT32 g_MPEGConsumeSIZE=0;
//static INT32 g_MPEGOutputSIZE=0;

extern UINT8 MCI_Play_Stream_Buffer;
extern UINT32 MCI_Play_Stream_Buffer_Length;
extern UINT8 *MCI_Play_Stream_Buffer_Address;
static UINT32 g_Audio_Buffer_Offset;
static UINT8 g_Audio_Buffer_Loop;
static UINT8 g_Audio_Buffer_Times;
UINT16 *g_last_buffer_addr;
UINT32  g_PCM_SMAPLE_RATE_ERROR;
INT32 AACFRAME;
INT32 AAClearBuffer;
#ifdef BT_SUPPORT

extern UINT8 MCI_Play_BTStream;
extern bt_a2dp_audio_cap_struct *MCI_BTAudioCap;

#endif

MCI_ERR_T  MMC_MpegReceiveA2dpData(MPEG_PLAY *pAudioPlayer);

/*
* NAME:	MMC_AUDIODecClose()
* DESCRIPTION: Close aac decoder.
*/
//extern uint32 *DRV_PCMOutStop(void);
MCI_ERR_T MMC_AUDIODecClose (MPEG_PLAY *pAudioPlayer)
{ 
	hal_HstSendEvent(SYS_EVENT,0xabc20000);

	MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MCI_MP3AAC]MMC_AUDIODecClose!");
	vpp_AudioJpegDecClose();
	aud_StreamStop(audioItf);

	mmc_MemFreeAll();

	g_DecOpenFlag = 0;
    MCI_Play_Stream_Buffer = 0;

#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 1)
        Avdtp_Stop_Data();
#endif    
    COS_Free(MpegPlayer);
    MpegPlayer = NULL;
    
	return  MCI_ERR_NO;
	
}


	
/*
* NAME:	MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_AudioVocISR(void)
{
  	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_VOC);
} 


/*
* NAME:	MMC_AudioPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/

UINT8 flag_PCM = 0;

void AudioHalfPcmISR(void)
{
	Mmc_Audio_Pcm_Half = 0;
  // hal_HstSendEvent(SYS_EVENT,0x96600001);

  if(flag_PCM < 6)
  {
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_PCM);
        flag_PCM++;
  }
    

#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 0)
#endif
    if(!g_ReadFileFlag)
    {
        memset(pcmbuf_overlay, 0, AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
    }

}

/*
* NAME:	MMC_AudioPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/
void AudioEndPcmISR(void)
{
	Mmc_Audio_Pcm_Half = 1;
	//hal_HstSendEvent(SYS_EVENT,0x96600002);
	if(flag_PCM < 6)
          {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_PCM);
                flag_PCM++;
          }

#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 0)
#endif
    if(!g_ReadFileFlag)
    {
        memset(pcmbuf_overlay, 0, AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
    }
}
/// Audio interface used in the test

void MMC_MpegPcmOutStart(MPEG_PLAY *pAudioPlayer)
{

	hal_HstSendEvent(SYS_EVENT,0x88503000);    	
	hal_HstSendEvent(SYS_EVENT,pAudioPlayer->MpegOutput.pcm.Buffer);    	

    // Stream out
	audioStream.startAddress  = (UINT32*) pAudioPlayer->MpegOutput.pcm.Buffer;
	audioStream.length        = pAudioPlayer->MpegOutput.pcm.Size*4;
	audioStream.channelNb     = pAudioPlayer->Voc_AudioDecStatus.nbChannel;
   
    audioStream.voiceQuality         = !TRUE;
    audioStream.halfHandler   = AudioHalfPcmISR;
    audioStream.endHandler    = AudioEndPcmISR;

#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 1)
    {
        if(gpio_detect_earpiece())
            audioItf = AUD_ITF_BT_EP;
        else
            audioItf = AUD_ITF_BT_SP;
    }
    else
#endif
    if(gpio_detect_earpiece())
        audioItf = AUD_ITF_EAR_PIECE;
    else
        audioItf = AUD_ITF_LOUD_SPEAKER;

   	
	hal_HstSendEvent(SYS_EVENT,0x88900000 + pAudioPlayer->Voc_AudioDecStatus.SampleRate);    	

    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]pAudioPlayer->Voc_AudioDecStatus.SampleRate:%d",pAudioPlayer->Voc_AudioDecStatus.SampleRate);
        g_PCM_SMAPLE_RATE_ERROR=pAudioPlayer->Voc_AudioDecStatus.SampleRate;
	g_PCM_SMAPLE_RATE_ERROR|=0x80000000;
	switch (pAudioPlayer->Voc_AudioDecStatus.SampleRate)
	{
	case 48000:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_48K\n");
  		audioStream.sampleRate    = HAL_AIF_FREQ_48000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 44100:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_44_1K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 32000:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_32K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_32000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 24000:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_24K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_24000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 22050:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_22_05K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_22050HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 16000:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_16K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_16000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 12000:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_12K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_12000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 11025:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_11_025K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_11025HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;
	case 8000:
		diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_8K\n");
		audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		break;			
	default:
		diag_printf("[MMC_AUDIO]##WARNING## Sample rate error:%d\n",pAudioPlayer->Voc_AudioDecStatus.SampleRate);
		break;
	}
	UINT32 errStatus = 	aud_StreamStart(audioItf, &audioStream, &audio_cfg);
	
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_MpegPcmOutStart end; errStatus = %d", errStatus);
}

INT32 MMC_ReadFromBuff(UINT8 *pBuf, UINT32 iLen)
{
    INT32 len = 0;
    if(g_Audio_Buffer_Times!=0)
    {
        if(g_Audio_Buffer_Loop >= g_Audio_Buffer_Times)
        {
            // ÖÆÔì´íÎóÊý¾ÝÀ´Í£Ö¹²¥·ÅÌáÊ¾Òô
            memset(pBuf, 0, iLen);
            return iLen;
        }
    }
    //hal_HstSendEvent(SYS_EVENT,0x07280100);
    //hal_HstSendEvent(SYS_EVENT,pBuf);
    //hal_HstSendEvent(SYS_EVENT,iLen);

    while(len < iLen)
    {
        if(g_Audio_Buffer_Offset + iLen - len < MCI_Play_Stream_Buffer_Length)
        {
            if(g_Audio_Buffer_Offset == 0 && g_Audio_Buffer_Loop >= g_Audio_Buffer_Times)
            {
                // make data error to stop it
                memset(pBuf, 0, iLen-len);
            }
            else
            {
                memcpy(pBuf, MCI_Play_Stream_Buffer_Address+g_Audio_Buffer_Offset, iLen-len);
            }
            g_Audio_Buffer_Offset += iLen - len;
            len = iLen;
        }
        else
        {
            memcpy(pBuf, MCI_Play_Stream_Buffer_Address+g_Audio_Buffer_Offset, MCI_Play_Stream_Buffer_Length-g_Audio_Buffer_Offset);
            pBuf += MCI_Play_Stream_Buffer_Length-g_Audio_Buffer_Offset;
            len += MCI_Play_Stream_Buffer_Length-g_Audio_Buffer_Offset;
            g_Audio_Buffer_Offset = 0;
            if(g_Audio_Buffer_Times!=0)
            {
                g_Audio_Buffer_Loop++;
                if(g_Audio_Buffer_Loop >= g_Audio_Buffer_Times)
                {
                    return len;
                }
            }
        }
    };
    g_ReadFileFlag = 1;
    g_ConsumedLength = 0;
    //hal_HstSendEvent(SYS_EVENT,len);
    return len;
}

/*
* NAME:	MMC_AudioDecOpen()
* DESCRIPTION: Initialize AUDIO decoder.
*/
MCI_ERR_T MMC_AudioDecOpen (HANDLE fhd, MPEG_PLAY *pAudioPlayer)
{
    if(MpegPlayer == NULL)
        pAudioPlayer = MpegPlayer = COS_Malloc(sizeof(MPEG_PLAY));
	MPEG_INPUT *input = &pAudioPlayer->MpegInput;
	MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput;
	vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;
	HAL_ERR_T voc_ret = 0;
	int32 readlen = 0;
	uint32 tagsize=0;
	   FrameCount=0;
	   AAClearBuffer=0;
	   AACFRAME=0;
	hal_HstSendEvent(SYS_EVENT,0x88100000);
    hal_HstSendEvent(SYS_EVENT,input);
    
	/*open AAC file*/
	input->fileHandle =fhd;

	/*initialize I/O data struct*/
	input->bof = (uint8 *)inputbuf_overlay;
	
	memset(input->bof, 0, AudioJpeg_INPUT_BUFSIZE);
	input->length = AudioJpeg_INPUT_BUFSIZE;
	input->inFlag = 0;
	
	//output->length = (AUDIO_MAX_OUTPUT_BUFSIZE) >> 2;//80 int

	output->pcm.Buffer = (uint32 *)pcmbuf_overlay;
	//output->data =  (int16 *)pcmbuf_overlay;
	memset(output->pcm.Buffer,0,AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
	output->pcm.Buffer_head=output->pcm.Buffer;

	hal_HstSendEvent(SYS_EVENT,0x88100020);
    hal_HstSendEvent(SYS_EVENT,input->bof);

#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 1)
    {
            Avdtp_Clean_Data();
            input->length = 0;
    }
#endif    

	switch(pAudioPlayer->mode)
	{
		case MMC_AAC_DECODE:
			pAudioPlayer->Voc_AudioDecStatus.consumedLen = 0;
			pAudioPlayer->Voc_AudioDecStatus.nbChannel = 0;
			pAudioPlayer->Voc_AudioDecStatus.output_len = 0;
			pAudioPlayer->Voc_AudioDecStatus.streamStatus= 0;
			
			pAudioPlayer->Voc_AudioDecIN.EQ_Type=-1;
			pAudioPlayer->Voc_AudioDecIN.reset=1;
			break;
			
		case MMC_MP3_DECODE:
			pAudioPlayer->Voc_AudioDecStatus.consumedLen = 0;
			pAudioPlayer->Voc_AudioDecStatus.nbChannel = 0;
			pAudioPlayer->Voc_AudioDecStatus.output_len = 0;
			pAudioPlayer->Voc_AudioDecStatus.streamStatus= 0;
			
			pAudioPlayer->Voc_AudioDecIN.EQ_Type=-1;
			pAudioPlayer->Voc_AudioDecIN.reset=1;			
			break;	
			
		default:
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_MP3AAC_ERROR]Error Mode!");
			break;
	}
	Mmc_Audio_Pcm_Half=1;
	ErrorFrameCount=0;
	ContinuePlayFlag = 0;
	g_ReadFileFlag=0;
    g_PCMPassFlag = 0;
    //g_MPEGConsumeSIZE = 0;
    //g_MPEGOutputSIZE = 0;

#ifdef BT_SUPPORT
  if(MCI_Play_BTStream == 0)
#endif    
  {
	/*prepare input stream*/
    if(MCI_Play_Stream_Buffer)
    {
        readlen = MMC_ReadFromBuff(input->bof, input->length);
    }
#ifdef MCD_TFCARD_SUPPORT
    else
    	readlen = FS_Read(input->fileHandle, input->bof, input->length);
#endif
	
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_MP3AAC] First Read Len=%d ;Need Length=%d\n", readlen,input->length);
	
	if(readlen<0)
	{
		return MCI_ERR_CANNOT_OPEN_FILE;
	}
	
	if(g_ConsumedLength>0)
	{
		g_AudioSeekFlag=1;
	}
	else
	{
		g_AudioSeekFlag=0;
	}

	if ((input->bof[0]=='I')&&(input->bof[1]=='D')&&(input->bof[2]=='3'))
	{
		//hal_HstSendEvent(SYS_EVENT,0x88100030);
        /* high bit is not used */
		tagsize = (input->bof[6] << 21) | (input->bof[7] << 14) |
			(input->bof[8] <<  7) | (input->bof[9] <<  0);
		
		tagsize += 10;
		
		diag_printf("[MMC_AUDIO] tagsize: %d\n", tagsize);
		
        if(MCI_Play_Stream_Buffer)
        {
            MCI_Play_Stream_Buffer_Length -= tagsize;
            MCI_Play_Stream_Buffer_Address += tagsize;
            g_Audio_Buffer_Offset = 0;
            g_Audio_Buffer_Loop = 0;
            readlen = MMC_ReadFromBuff(input->bof, input->length);
            
        }
#ifdef MCD_TFCARD_SUPPORT
        else
        {
    		if(FS_Seek(input->fileHandle, tagsize, FS_SEEK_SET)<0)
            {      
                return MCI_ERR_CANNOT_OPEN_FILE;;
            }
    		readlen = FS_Read(input->fileHandle, input->bof, input->length);
		    diag_printf("[MMC_AUDIO] after ID3first read len=%d ;need length=%d\n", readlen,input->length);
        }
#endif
		if(readlen<0)
		{
			return MCI_ERR_CANNOT_OPEN_FILE;
		}
		if(readlen<input->length)
		{
		}
	}
   }
	
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_AudioDecOpen vpp_AudioJpegDecOpen!");
	/*open voc decoder*/

	voc_ret = vpp_AudioJpegDecOpen((HAL_VOC_IRQ_HANDLER_T)MMC_AudioVocISR);

	hal_HstSendEvent(SYS_EVENT,0x88110000+voc_ret);
	 
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
	pVoc_Voc_AudioDecIN->inStreamBufAddr=(INT32 *)input->bof;
	pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)output->pcm.Buffer;

	pVoc_Voc_AudioDecIN->mode=pAudioPlayer->mode;
	pVoc_Voc_AudioDecIN->BsbcEnable = 0;
    if(AUD_ITF_BT_SP==audioItf)
    	pVoc_Voc_AudioDecIN->audioItf=AUD_ITF_LOUD_SPEAKER;
    else
    	pVoc_Voc_AudioDecIN->audioItf=audioItf;

    g_DecOpenFlag = 1;

    return MCI_ERR_NO;
}


void Audio_MpegPrepareNextFrame(MPEG_PLAY *pAudioPlayer)
{
	MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput;
	vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;

    MCI_TRACE (MCI_AUDIO_TRC,0, "[Audio_MpegPrepareNextFrame]Mmc_Audio_Pcm_Half:%d",Mmc_Audio_Pcm_Half);

        // prepare for nect frame decode
        if(Mmc_Audio_Pcm_Half==1) // end
        {
            output->pcm.Buffer_head=output->pcm.Buffer;
        }
        else if(Mmc_Audio_Pcm_Half==0) // half
        {
            output->pcm.Buffer_head=output->pcm.Buffer+(output->pcm.Size>>1);           
        }
        pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)output->pcm.Buffer_head;
}


/*
* NAME:	MMC_AUDIODecVoc()
* DESCRIPTION: Decode AAC stream ......
*/
MCI_ERR_T MMC_AUDIODecVoc (MPEG_PLAY *pAudioPlayer)
{
	MPEG_INPUT *input = &pAudioPlayer->MpegInput;
	MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput;
	vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus = &pAudioPlayer->Voc_AudioDecStatus;
	vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;

     INT32 length;
      vpp_AudioJpegDecStatus(pVoc_AudioDecStatus);
      //hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->SampleRate+0x18500000);   
      
	//hal_HstSendEvent(SYS_EVENT,0x88500000);  
	//hal_HstSendEvent(SYS_EVENT,input->inFlag);  
#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 1)
    {
        uint32 mpeg_length, frames;
        if(input->inFlag==0)
        {
            input->inFlag = 1;
            g_PCMPassFlag = 1;
            return MCI_ERR_NO;
        }
    	vpp_AudioJpegDecStatus(pVoc_AudioDecStatus);
        diag_printf("MMC_AUDIODecVoc: inFlag = %d, consumedLen=%d, output_len=%d, ErrorStatus=%d", input->inFlag,pVoc_AudioDecStatus->consumedLen, pVoc_AudioDecStatus->output_len, pVoc_AudioDecStatus->ErrorStatus);
        hal_HstSendEvent(SYS_EVENT,AAClearBuffer);
        hal_HstSendEvent(SYS_EVENT,AACFRAME);
        //hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->output_len);
        //hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->ErrorStatus);

	if(AAClearBuffer>=1)
	{
		AAClearBuffer++;
		if(AAClearBuffer<5)
		memset(pcmbuf_overlay,0,AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
		else
		AAClearBuffer=0;
	}


		
        if(pVoc_AudioDecStatus->ErrorStatus!=0 && (pVoc_AudioDecStatus->consumedLen==0||pVoc_AudioDecStatus->output_len>AUDIO_MAX_OUTPUT_BUFSIZE))
        {
            hal_HstSendEvent(SYS_EVENT,0x88540000);
            return MCI_ERR_ERROR;
        }
        MCI_MergeSideTone(pVoc_AudioDecStatus->output_len, g_last_buffer_addr);

        if(input->inFlag==1)
        {
            // setup ouput buffer size as dec frame sise
            output->pcm.Size = pVoc_AudioDecStatus->output_len >> 1;
            hal_HstSendEvent(SYS_EVENT,output->pcm.Size);
            if(pVoc_AudioDecStatus->output_len&0x1f) // the length should be 32byte aligned
                return MCI_ERR_ERROR;

            MMC_MpegPcmOutStart(pAudioPlayer);    
            input->inFlag = 2;
        }

        length = input->length-pVoc_AudioDecStatus->consumedLen;
        if(length > 0)
        {
            //while(1);
            memcpy(input->bof, input->bof+pVoc_AudioDecStatus->consumedLen, length);
        }
        else
        {
            length = 0;
        }
        input->length = length;
        if(length < AudioJpeg_INPUT_BUFSIZE)
       {
            Avdtp_Get_Data(&mpeg_length, input->bof+input->length, AudioJpeg_INPUT_BUFSIZE-input->length);
            input->length += mpeg_length;
            diag_printf("MMC_AUDIODecVoc: Avdtp_Get_Data(), mpeg_length=%d, input->length=%d", mpeg_length, input->length);
        }        
        
        if(input->length >= MIN_INPUT_REMAIN_SIZE)
        {
        if(g_PCMPassFlag)
        {
            vpp_AudioJpegDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
            g_ReadFileFlag = 0;
            g_PCMPassFlag = 0;

            // prepare for nect frame decode
            Audio_MpegPrepareNextFrame(pAudioPlayer);
        }
        else
            g_ReadFileFlag = 1;
        }
    }        
    else
#endif	
  {
    if(input->inFlag==0)
    {
        input->inFlag = 1;

        // decode first frame
        vpp_AudioJpegDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
        return MCI_ERR_NO;
    }

	vpp_AudioJpegDecStatus(pVoc_AudioDecStatus);

    diag_printf("MMC_AUDIODecVoc: inFlag = %d, consumedLen=%d, output_len=%d, ErrorStatus=%d", input->inFlag,pVoc_AudioDecStatus->consumedLen, pVoc_AudioDecStatus->output_len, pVoc_AudioDecStatus->ErrorStatus);
    //hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->consumedLen);
    //hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->output_len);
    //hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->ErrorStatus);
    //g_MPEGConsumeSIZE += pVoc_AudioDecStatus->consumedLen;
    //g_MPEGOutputSIZE += pVoc_AudioDecStatus->output_len;

    if(input->inFlag==1)
    {
        // if the error data can skip, don't return
        if(pVoc_AudioDecStatus->ErrorStatus!=0 && pVoc_AudioDecStatus->consumedLen<=0)
	{
		Frame_exit=1;
		ErrorFrameCount++;
		return MCI_ERR_ERROR;
	}
        // the data length cannot play one frame.
        g_ConsumedLength += pVoc_AudioDecStatus->consumedLen;
        if(g_ConsumedLength>=g_MPEGFILESIZE)
	{    
		Frame_exit=1;
		ErrorFrameCount++;
		return MCI_ERR_ERROR;
	}
        // setup ouput buffer size as dec frame sise
        if(pVoc_AudioDecStatus->output_len>0&&pVoc_AudioDecStatus->output_len<=AUDIO_MAX_OUTPUT_BUFSIZE)
        {
        output->pcm.Size = pVoc_AudioDecStatus->output_len >> 1;
        hal_HstSendEvent(SYS_EVENT,output->pcm.Size);
        }
        else
        output->pcm.Size=0;

        // move data from pcmbuf_overlay to inputbuf_overlay;
        length = input->length-pVoc_AudioDecStatus->consumedLen;
        if(length > 0)
        {
            if(length < AudioJpeg_INPUT_BUFSIZE)
            {
                memcpy(inputbuf_overlay, input->bof+pVoc_AudioDecStatus->consumedLen, length);
            }
            /*else
            {
                memcpy(inputbuf_overlay, input->bof+pVoc_AudioDecStatus->consumedLen, AudioJpeg_INPUT_BUFSIZE);
                FS_Seek(input->fileHandle,length-AudioJpeg_INPUT_BUFSIZE,FS_SEEK_CUR);
                length = 0;
            }*/
        }

        if(MCI_Play_Stream_Buffer)
        {
            if(length > 0)
                length = MMC_ReadFromBuff(input->bof+length, input->length-length);
        }
#ifdef MCD_TFCARD_SUPPORT
        else
        {
            if(length < 0)
            {
                FS_Seek(input->fileHandle,-length,FS_SEEK_CUR);
                length = 0;
            }
            length = FS_Read(input->fileHandle, input->bof+length, input->length-length);
        }
#endif
        //hal_HstSendEvent(SYS_EVENT,length);
        diag_printf("MMC_AUDIODecVoc: Read data length=%d ", length);
        if(length<0)
        { 	
        	Frame_exit=1;
        	ErrorFrameCount++;
            return MCI_ERR_ERROR;
        }
        //output->data=(int16 *)(output->pcm.Buffer+(output->pcm.Size>>1));
      
            pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)(output->pcm.Buffer+(output->pcm.Size>>1));

            vpp_AudioJpegDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
        
            //output->data=(int16 *)output->pcm.Buffer;
            pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)output->pcm.Buffer;
         FrameCount++;
        if( g_GetInfoFlag == 0)
        {   
              hal_HstSendEvent(SYS_EVENT,pVoc_AudioDecStatus->output_len);      
         if(pVoc_AudioDecStatus->ErrorStatus==0&&pVoc_AudioDecStatus->output_len>0&&
              (pVoc_AudioDecStatus->output_len&0x1f)==0&&pVoc_AudioDecStatus->output_len<=AUDIO_MAX_OUTPUT_BUFSIZE&&
                 g_GetInfoFlag== 0&&FrameCount>2)
            {
                MMC_MpegPcmOutStart(pAudioPlayer);    
                input->inFlag = 2;
            }
            else
            {
             input->inFlag =1;
             
              if(pVoc_AudioDecStatus->ErrorStatus!=0)
			    {
				 ErrorFrameCount++;
                 FrameCount=0;
                }
             }
        }
        else
        {
          if(pVoc_AudioDecStatus->ErrorStatus!=0||pVoc_AudioDecStatus->output_len<0||
             (pVoc_AudioDecStatus->output_len&0x1f)!=0||pVoc_AudioDecStatus->output_len>AUDIO_MAX_OUTPUT_BUFSIZE)
		{
			ErrorFrameCount++;
			Frame_exit=0;
		}
	 else
		{
			Frame_exit=2;
		}

	}
        if(ErrorFrameCount>=5)         
      	{
      		MCI_TRACE(MCI_AUDIO_TRC, 0,"%s(%d):ErrorFrameCount:%d,FrameCount:%d",__func__,__LINE__,ErrorFrameCount,FrameCount);
		return MCI_ERR_ERROR;
#if 0
		MMC_AUDIODecClose(MpegPlayer);
		MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
		mmc_SetCurrMode(MMC_MODE_IDLE);
		ContinuePlayFlag = 0;
#endif
	}
	  return MCI_ERR_NO;

	}

    MCI_MergeSideTone(pVoc_AudioDecStatus->output_len, g_last_buffer_addr);

    // fulfill inputbuf_overlay
    if(pVoc_AudioDecStatus->consumedLen > 0)
    {
        g_ConsumedLength += pVoc_AudioDecStatus->consumedLen;
        if(g_ConsumedLength>=g_MPEGFILESIZE)
            return MCI_ERR_END_OF_FILE;
        length = input->length-pVoc_AudioDecStatus->consumedLen;
        if(length > 0)
            memcpy(input->bof, input->bof+pVoc_AudioDecStatus->consumedLen, length);
        
        if(MCI_Play_Stream_Buffer)
        {
            if(length > 0)
                length = MMC_ReadFromBuff(input->bof+length, input->length-length);
        }
#ifdef MCD_TFCARD_SUPPORT
        else
        {
        	if(length < 0)
            {
                FS_Seek(input->fileHandle,-length,FS_SEEK_CUR);
                length = 0;
            }	
            length = FS_Read(input->fileHandle, input->bof+length, input->length-length);
        }
#endif
        //hal_HstSendEvent(SYS_EVENT,length);
		if(length<0)
		{
			return MCI_ERR_END_OF_FILE;
		}

    }    

#if 1
	 //hal_HstSendEvent(SYS_EVENT,0x88510000);    	
		if(pVoc_AudioDecStatus->ErrorStatus==0)
		{
			if(g_PCM_SMAPLE_RATE_ERROR&0x80000000)
			{
				if((g_PCM_SMAPLE_RATE_ERROR&0x7fffffff)!=pVoc_AudioDecStatus->SampleRate)
				{
					memset(pAudioPlayer->MpegOutput.pcm.Buffer, 0, 0x2400);
					ErrorFrameCount+=4;     
				}
				else 
				{
				    ErrorFrameCount=0;
				}
				//hal_HstSendEvent(SYS_EVENT,0x19910000+pVoc_AudioDecStatus->SampleRate);
				//hal_HstSendEvent(SYS_EVENT,0x19920000+g_PCM_SMAPLE_RATE_ERROR);
				//hal_HstSendEvent(SYS_EVENT,0x19900000+ErrorFrameCount);

                          
				if(ErrorFrameCount>MP3_MAX_ERROR_CNT)
				{
					MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_MP3AAC_ERROR]ErrorFrameCount:%d",ErrorFrameCount);
					g_PCM_SMAPLE_RATE_ERROR=0;
					return MCI_ERR_ERROR;				
				}

			}
			else
			{
				ErrorFrameCount=0;
			} 
		}
		else
		{
		    if(MCI_Play_Stream_Buffer) // ³ö´íÊ±Í£Ö¹²¥·ÅÌáÊ¾Òô
                return MCI_ERR_ERROR;
			ErrorFrameCount++;
			//hal_HstSendEvent(SYS_EVENT,ErrorFrameCount); 
			memset(pAudioPlayer->MpegOutput.pcm.Buffer, 0, 0x2400);
			if(ErrorFrameCount>MP3_MAX_ERROR_CNT)
			{
				MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_MP3AAC_ERROR]ErrorFrameCount:%d",ErrorFrameCount);
				g_PCM_SMAPLE_RATE_ERROR=0;
				return MCI_ERR_ERROR;				
			}
		}

#endif    
	if(pVoc_AudioDecStatus->ErrorStatus!=0)
	{
		if(MCI_Play_Stream_Buffer) 
       			 return MCI_ERR_ERROR;
		ErrorFrameCount++;
		
		memset(pAudioPlayer->MpegOutput.pcm.Buffer, 0, 0x2400);
		if(ErrorFrameCount>=MP3_MAX_ERROR_CNT)
		{
			MCI_TRACE (MCI_AUDIO_TRC,0, "%s(%d)ErrorFrameCount:%d",__func__,__LINE__,ErrorFrameCount);
		//	g_PCM_SMAPLE_RATE_ERROR=0;
			return MCI_ERR_ERROR;				
		}
	}



        if(g_PCMPassFlag)
        {
            vpp_AudioJpegDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
            g_ReadFileFlag = 0;
            g_PCMPassFlag = 0;

            // prepare for nect frame decode
            Audio_MpegPrepareNextFrame(pAudioPlayer);
        }
        else
            g_ReadFileFlag = 1;
    }
	return MCI_ERR_NO;
}

MCI_ERR_T MMC_AUDIODecPcm(MPEG_PLAY *pAudioPlayer)
{
	MPEG_INPUT *input = &pAudioPlayer->MpegInput;
	MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput;	
	vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;

	if(ContinuePlayFlag)
	{
		//hal_HstSendEvent(SYS_EVENT,0x65900033);
		diag_printf("!!!!!play all left data in the pcm buffer\n");
		MMC_AUDIODecClose(MpegPlayer);
		MCI_AudioFinished(MCI_ERR_END_OF_FILE);
	
		mmc_SetCurrMode(MMC_MODE_IDLE);
		ContinuePlayFlag = 0;
    	return MCI_ERR_NO;
	}
#ifdef BT_SUPPORT
    if((!g_ReadFileFlag) && MCI_Play_BTStream == 1)
    {
        g_PCMPassFlag = 0;
        MMC_MpegReceiveA2dpData(pAudioPlayer);
    }
#endif

    MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_AUDIODecPcm]g_ReadFileFlag:%d",g_ReadFileFlag);

    if(g_ReadFileFlag)
    {
        // file data read, decode one frame
        vpp_AudioJpegDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
        g_PCMPassFlag = 0;
        g_ReadFileFlag = 0;
        // prepare for nect frame decode
        Audio_MpegPrepareNextFrame(pAudioPlayer);
		AACFRAME=0;
	}
    else
    {
        // set flag
        g_PCMPassFlag = 1;
        hal_HstSendEvent(SYS_EVENT,0x88400001);
        memset(output->pcm.Buffer_head, 0, output->pcm.Size<<1);
	  Audio_MpegPrepareNextFrame(pAudioPlayer);

	AACFRAME++;
	if(AACFRAME>4)
	{
	AACFRAME=0;
	AAClearBuffer=1;

 	}

     }

	return MCI_ERR_NO;
}



#ifdef BT_SUPPORT

MCI_ERR_T  MMC_MpegReceiveA2dpData(MPEG_PLAY *pAudioPlayer)
{
	MPEG_INPUT *input = &pAudioPlayer->MpegInput;
	MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput;
	int32 i,readlen;
	vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;
       uint32 mpeg_length, frame_len, frames, length;

        //hal_HstSendEvent(SYS_EVENT,0x10260032);
        if(g_ReadFileFlag)
            return 0;

        if(input->length < AudioJpeg_INPUT_BUFSIZE)
        {
            Avdtp_Get_Data(&mpeg_length, input->bof+input->length, AudioJpeg_INPUT_BUFSIZE-input->length);
            input->length += mpeg_length;
        }
        diag_printf("MMC_MpegReceiveA2dpData: inFlag = %d, mpeg_length=%d, input->length=%d", input->inFlag,mpeg_length, input->length);
        if(input->inFlag!=2)
        {
            if(input->length < MIN_INPUT_REMAIN_SIZE*2)
                return MCI_ERR_NO;
        }

        if(input->length < MIN_INPUT_REMAIN_SIZE)
            return MCI_ERR_NO;
        
        if(g_PCMPassFlag)
        {
            vpp_AudioJpegDecScheduleOneFrame(pVoc_Voc_AudioDecIN);
            g_ReadFileFlag = 0;
            g_PCMPassFlag = 0;

            // prepare for nect frame decode
            Audio_MpegPrepareNextFrame(pAudioPlayer);
#if 0
            hal_HstSendEvent(SYS_EVENT,0x10260035);
            hal_HstSendEvent(SYS_EVENT,Mmc_Audio_Pcm_Half);
            hal_HstSendEvent(SYS_EVENT,pVoc_Voc_AudioDecIN->outStreamBufAddr);
            hal_HstSendEvent(SYS_EVENT,output->pcm.Size);
#endif
        }
        else
            g_ReadFileFlag = 1;

	return MCI_ERR_NO;

}
#endif

int32 Audio_MpegPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
	int32 result;
	MCI_TRACE (MCI_AUDIO_TRC,0,"Audio_MpegPlay,fhd is %d",fhd);

	hal_HstSendEvent(SYS_EVENT,0x88000000);
    if(MpegPlayer == NULL)
        MpegPlayer = COS_Malloc(sizeof(MPEG_PLAY));

#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 0)
#endif    
    {
        if(MCI_Play_Stream_Buffer)
        {
            g_Audio_Buffer_Offset = 0;
            g_MPEGFILESIZE = 0x7fffffff;
            g_ConsumedLength = 0;
            g_Audio_Buffer_Loop = 0;
            g_Audio_Buffer_Times = PlayProgress;
        }
#ifdef MCD_TFCARD_SUPPORT
        else
        {
        	//g_FileHandle=fhd;
        	if (fhd< 0)
        	{
        		diag_printf("[MMC_AUDIO_ERROR]ERROR file hander: %d \n", fhd);
        		return MCI_ERR_CANNOT_OPEN_FILE;
        	}

        	if ((g_MPEGFILESIZE=FS_GetFileSize(fhd))<= 0)
        	{
        		diag_printf("[MMC_AUDIO_ERROR]can not get file size!:%d",g_MPEGFILESIZE);
        		return MCI_ERR_CANNOT_OPEN_FILE;
        	}

        	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AMR]File Size: %d",g_MPEGFILESIZE);

        	g_ConsumedLength=(INT32)(((INT64)PlayProgress*(INT64)g_MPEGFILESIZE)/10000);

        	hal_HstSendEvent(SYS_EVENT,0x88000010);
          hal_HstSendEvent(SYS_EVENT,PlayProgress);
        	if ((result=FS_Seek(fhd,g_ConsumedLength,FS_SEEK_SET))< 0)
        	{
        		diag_printf("[MMC_AUDIO_ERROR]can not seek file!:%d",result);
        		return MCI_ERR_CANNOT_OPEN_FILE;
        	}
        }
#endif
    }
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AMR]g_AMRConsumedLength: %d",g_ConsumedLength);
	switch(filetype)
	{
		case MCI_TYPE_DAF:
			MpegPlayer->mode = MMC_MP3_DECODE;
			break;
			
		case MCI_TYPE_AAC:
			MpegPlayer->mode = MMC_AAC_DECODE;
			break;
			
		default:
            hal_HstSendEvent(SYS_EVENT,0x8800852);
			diag_printf("Audio_MpegPlay()->unsupport record media type");
            while(1);
			break;
	}
	
	if((result= MMC_AudioDecOpen(fhd, MpegPlayer))!=MCI_ERR_NO)
	{
		diag_printf("[MMC_AUDIO]##WARNING##MMC_AudioDecOpen() return false!\n");
		MMC_AUDIODecClose(MpegPlayer);
		return result;
	}
	
	return  MCI_ERR_NO;
}

int32 Audio_MpegStop (void) 	
{
	//clear the flag used to play left data in the input buffer and pcm buffer 
	ContinuePlayFlag=0;

	MMC_AUDIODecClose(MpegPlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_MpegPause (void)	
{
	//MPEG_INPUT *input = &MpegPlayer->MpegInput;
	//Drv_PcmOutStop();
	g_AudioMode=MpegPlayer->mode;
	MCI_TRACE (MCI_AUDIO_TRC,0, "******************* Audio_MpegPause**********************");
	//aud_StreamPause(audioItf,TRUE);
	//clear the flag used to play left data in the input buffer and pcm buffer 
	ContinuePlayFlag=0;

	MMC_AUDIODecClose(MpegPlayer);	
	
	//memset(MpegPlayer->MpegOutput.pcm.Buffer,0,AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
	return  MCI_ERR_NO;
}


int32 Audio_MpegResume ( HANDLE fhd) 
{
  int32 result;
#ifdef MCD_TFCARD_SUPPORT
  if(MCI_Play_Stream_Buffer == 0)
  {
//	Drv_PcmOutStart();
       MpegPlayer->mode=g_AudioMode;
//	aud_StreamPause(audioItf,FALSE);

	g_ConsumedLength-=2048;//VoC buffer length

	if(g_ConsumedLength<0)g_ConsumedLength=0;
	
	result=FS_Seek(fhd,g_ConsumedLength,FS_SEEK_SET);

	MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek pos:%d",result);
	
	if(result<0 )
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek error!");
		return -1;
	}
	
	if((result= MMC_AudioDecOpen(fhd, MpegPlayer))!=MCI_ERR_NO)
	{
		diag_printf("[MMC_AUDIO]##WARNING##MMC_AudioDecOpen() return false!\n");
		MMC_AUDIODecClose(MpegPlayer);
		return result;
	}
  }
#endif
	return  MCI_ERR_NO;
}


int32 Audio_MpegGetID3 (char * pFileName)  
{
	return MCI_ERR_NO;
}

int32 Audio_MpegGetPlayInformation (MCI_PlayInf * MCI_PlayInfMPEG)  
{
    if(g_MPEGFILESIZE==0)
        MCI_PlayInfMPEG->PlayProgress = 0;
    else
    	MCI_PlayInfMPEG->PlayProgress=(INT32)((((INT64)g_ConsumedLength)*10000)/g_MPEGFILESIZE);
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_AUDIO_MPEG]PlayProgress:%d,g_MPEGFILESIZE=%d,g_DecOpenFlag=%d",MCI_PlayInfMPEG->PlayProgress,g_MPEGFILESIZE,g_DecOpenFlag);
	

	return MCI_ERR_NO;
}

int32 Audio_MpegGetFileInformation(HANDLE fhd, AudDesInfoStruct  * CONST DecInfo, int16 filetype)
{
	int32 result, wait;
    COS_EVENT ev;
    
	MCI_TRACE (MCI_AUDIO_TRC,0,"Audio_MpegPlay,fhd is %d",fhd);
    if(MpegPlayer == NULL)
        MpegPlayer = COS_Malloc(sizeof(MPEG_PLAY));

	vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus = &MpegPlayer->Voc_AudioDecStatus;

	//hal_HstSendEvent(SYS_EVENT,0x88000000);
    g_GetInfoFlag = 0;
    if(g_DecOpenFlag)
    {
       // hal_HstSendEvent(SYS_EVENT,0x12150010);
        wait = 0;
        while( ErrorFrameCount==0&&MpegPlayer->MpegInput.inFlag <2)
        {   hal_HstSendEvent(SYS_EVENT,0x88000024);             
            COS_Sleep(10);
            hal_HstSendEvent(SYS_EVENT,0x88000018);
            hal_HstSendEvent(SYS_EVENT,wait);
            wait ++;
            if(wait>10)
                break;
        }
        DecInfo->sampleRate = pVoc_AudioDecStatus->SampleRate;
        DecInfo->stereo = pVoc_AudioDecStatus->nbChannel==2;
        if(pVoc_AudioDecStatus->BitRate != 0)
        {
            DecInfo->bitRate = pVoc_AudioDecStatus->BitRate*1000;
            DecInfo->time = g_MPEGFILESIZE*8/pVoc_AudioDecStatus->BitRate+100;
        }
        else
        {
            DecInfo->time = DecInfo->bitRate = 0;
        }

    	//hal_HstSendEvent(SYS_EVENT,0x89200000);
    	//hal_HstSendEvent(SYS_EVENT,DecInfo->sampleRate);
    	//hal_HstSendEvent(SYS_EVENT,DecInfo->bitRate);
    	//hal_HstSendEvent(SYS_EVENT,DecInfo->time);
        
        return MCI_ERR_NO;
    }


#ifdef BT_SUPPORT
    if(MCI_Play_BTStream == 0)
#endif    
#ifdef MCD_TFCARD_SUPPORT
  if(MCI_Play_Stream_Buffer == 0)
  {
	//g_FileHandle=fhd;
	if (fhd< 0)
	{
		diag_printf("[MMC_AUDIO_ERROR]ERROR file hander: %d \n", fhd);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

	if ((g_MPEGFILESIZE=FS_GetFileSize(fhd))< 0)
	{
		diag_printf("[MMC_AUDIO_ERROR]can not get file size!:%d",g_MPEGFILESIZE);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AMR]File Size: %d",g_MPEGFILESIZE);

	hal_HstSendEvent(SYS_EVENT,0x88000010);
	if ((result=FS_Seek(fhd,g_MPEGFILESIZE/2,FS_SEEK_SET))< 0) // from middle of file to get information
	{
		diag_printf("[MMC_AUDIO_ERROR]can not seek file!:%d",result);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

    g_ConsumedLength = 0;

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AMR]g_AMRConsumedLength: %d",g_ConsumedLength);
  }
#endif
	switch(filetype)
	{
		case MCI_TYPE_DAF:
			MpegPlayer->mode = MMC_MP3_DECODE;
			break;
			
		case MCI_TYPE_AAC:
			MpegPlayer->mode = MMC_AAC_DECODE;
			break;
			
		default:
            hal_HstSendEvent(SYS_EVENT,0x88000963);
			diag_printf("Audio_MpegPlay()->unsupport record media type");
            while(1);
			break;
	}

	ErrorFrameCount = 0;
	g_GetInfoFlag = 1;
	Frame_exit=0;
	if((result= MMC_AudioDecOpen(fhd, MpegPlayer))!=MCI_ERR_NO)
	{
		diag_printf("[MMC_AUDIO]##WARNING##MMC_AudioDecOpen() return false!\n");
		MMC_AUDIODecClose(MpegPlayer);
              g_GetInfoFlag = 0;
		return result;
	}

	wait = 0;
    while(Frame_exit==0&& ErrorFrameCount<5)
    {               
        COS_Sleep(10);
        hal_HstSendEvent(SYS_EVENT,0x88000018);
        hal_HstSendEvent(SYS_EVENT,wait);
        wait ++;
        if(wait>10)
            break;
    }

    if( pVoc_AudioDecStatus->BitRate != 0&& ErrorFrameCount<5)
    {
        DecInfo->sampleRate = pVoc_AudioDecStatus->SampleRate;
        DecInfo->stereo = pVoc_AudioDecStatus->nbChannel==2;
        DecInfo->bitRate = pVoc_AudioDecStatus->BitRate*1000;
        DecInfo->time = g_MPEGFILESIZE*8/pVoc_AudioDecStatus->BitRate+100;
        //DecInfo->time = ((INT64)g_MPEGFILESIZE)*g_MPEGOutputSIZE/g_MPEGConsumeSIZE/2/pVoc_AudioDecStatus->nbChannel/(pVoc_AudioDecStatus->SampleRate/1000);

    	hal_HstSendEvent(SYS_EVENT,0x89100000);
    	hal_HstSendEvent(SYS_EVENT,DecInfo->sampleRate);
    	hal_HstSendEvent(SYS_EVENT,DecInfo->bitRate);
    	hal_HstSendEvent(SYS_EVENT,DecInfo->time);
    }
    else
        result = MCI_ERR_ERROR;

	MMC_AUDIODecClose(MpegPlayer);
    mmc_ExitMode();

	g_GetInfoFlag = 0;
    
    return  result;
    
}


int32 Audio_MpegUserMsg(int32 nMsg)
{
    switch (nMsg)
    {
	case MSG_MMC_AUDIODEC_VOC:

			switch (MMC_AUDIODecVoc(MpegPlayer))
			{
			case MCI_ERR_END_OF_FILE:
				diag_printf("[MMC_AUDIO]MMC_AACDecRun() return FINISH!\n");

				ContinuePlayFlag = 1;
                ErrorFrameCount ++;
				break;
			case MCI_ERR_ERROR:
#if 0
				hal_HstSendEvent(SYS_EVENT,0x65900022);
				diag_printf("[MMC_AUDIO]##WARNING##MMC_AACDecRun() return FALSE!\n");
                ErrorFrameCount ++;
#ifdef BT_SUPPORT
                            if(MCI_Play_BTStream)
                            {  
							ErrorFrameCount ++;
                                Avdtp_Clean_Data();
                                break;
                            }
#endif
                if(g_GetInfoFlag == 0)
                {
    				MMC_AUDIODecClose(MpegPlayer);
    				if(g_AudioSeekFlag==1)
    				{
    					MCI_AudioFinished(MCI_ERR_END_OF_FILE);
    				}
    				else
    				{
    					MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
    				}
                }		

#endif
#ifdef BT_SUPPORT
                            if(MCI_Play_BTStream)
                            {  
							ErrorFrameCount ++;
                                Avdtp_Clean_Data();
                                break;
                            }
#endif

     			MMC_AUDIODecClose(MpegPlayer);
			        MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);	
				mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
				ContinuePlayFlag = 0;
				break;
			default:
				break;
			}
		break;
#ifdef BT_SUPPORT
    case MSG_MMC_AUDIODEC_A2DP:
        MMC_MpegReceiveA2dpData(MpegPlayer);
        break;
#endif

	case MSG_MMC_AUDIODEC_PCM:
            flag_PCM--;
			MMC_AUDIODecPcm(MpegPlayer);
    
			break;
	default:
	break;
    }
	
    return 0;
}
 

