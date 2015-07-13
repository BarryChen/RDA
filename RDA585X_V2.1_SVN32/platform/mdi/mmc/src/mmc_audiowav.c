////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audiowav.c
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

#include "mmc_audiowav.h"

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
#include "event.h"


#define ADPCM_SUPPORT   1

static INT8 Mmc_Wav_Pcm_Half;
static INT32 g_copyed_frame;
static INT32 g_readed_frame;
static INT8 g_stop_read;
//patch for short file
static INT8 g_read_end;
static INT8 g_continue_play_flag;
static INT32 g_sbc_frame;

static INT32 g_WAVHeaderLength=0;

WAV_PLAY *WavPlayer;

extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

//extern UINT32 pcmbuf_overlay[4608];

volatile INT32 g_WAVConsumedLength=0;

volatile INT32 g_WAVFILESIZE=-1;

static int32 data_size =0;

static UINT16 g_WavNout;
static UINT32 g_Wavcounterframe;

static INT16 g_WavNumOfOutSample=0;

static INT16 g_WavTotalFrame=0;


static UINT8 g_WavResampleInitFlag=0;


static BOOL g_WavBT8KEnbleFlag=0;

static INT16 *g_WavBT8kPcmBuffer=NULL;


static  INT16  *g_WavBTDataAddress=0;



static INT32 g_Wav16BitInputBufSize =0;

static INT32 g_WavOutputPcmBufSize =0;

static INT32 g_nFrame               =0;

static INT32 g_WavInputBufSize	  =0;

static INT8 g_WavPlayFlag = 0;

static INT8 g_wav_read_flag = 0;

static UINT8 wav_pcm_irq_num = 0;


/*
* NAME:	MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_AudioWavVocISR(void)
{
//	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_WAVSBC]VoC INT!");
  	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_WAV_SBC_VOC);
} 


 

/*
* NAME:	MMC_AudioWavHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/

void MMC_AudioWavHalfPcmISR(void)
{
	if(wav_pcm_irq_num > 20)
	{
		return;
	}
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_WAV]HALF!");
	Mmc_Wav_Pcm_Half = 0;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_WAV_PCM_INT);
    if(g_wav_read_flag)
    {
    	wav_pcm_irq_num++;
        hal_HstSendEvent(SYS_EVENT,0x08440001);
    	memset(pcmbuf_overlay, 0, g_WavOutputPcmBufSize);
    }
	else
	{
		wav_pcm_irq_num = 0;
	}
}

/*
* NAME:	MMC_AudioWavEndPcmISR()
* DESCRIPTION: ISR for pcm end buffer exhausted.
*/
void MMC_AudioWavEndPcmISR(void)
{
	if(wav_pcm_irq_num > 20)
	{
		return;
	}
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_WAV]END!");
	Mmc_Wav_Pcm_Half = 1;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_WAV_PCM_INT);
    if(g_wav_read_flag)
    {
	    wav_pcm_irq_num++;
        hal_HstSendEvent(SYS_EVENT,0x08440002);
    	memset(pcmbuf_overlay, 0, g_WavOutputPcmBufSize);
    }
	else
	{
		wav_pcm_irq_num = 0;
	}
    
}


static const uint16 PCMSamplesRate[]={48000,44100,32000,24000,22050,16000,12000,11025,8000};

MCI_ERR_T MMC_WavHeaderParse(WAV_PLAY *pWavPlayer)
{
	WAV_INPUT *input = &pWavPlayer->WavInput;
	WAV_OUTPUT *output = &pWavPlayer->WavOutput;
    WAVE_DATA_HEADER WaveDataHeader;

	INT32 readlen=0,i;
	WAVE_HEADER *pWaveHdr=&pWavPlayer->WaveHeader;
	WAVE_DATA_HEADER *pWaveDataHdr=&WaveDataHeader;

	UINT8 ChunkInfoID[4];
	UINT32 ChunkInfoSize;
	UINT32 ChunkInfoData;

        FS_Seek(input->fileHandle, 0, FS_SEEK_SET);
	readlen=FS_Read(input->fileHandle,(UINT8 *)pWaveHdr,sizeof(WAVE_HEADER));
	if(readlen!=sizeof(WAVE_HEADER)){
		return MCI_ERR_ERROR;
	}

	input->fileOffset+=36;
	g_WAVHeaderLength+=36;


	if(pWaveHdr->wFormatTag==1){  //PCM format!!
	if((memcmp(pWaveHdr->szRiff,"RIFF",4)!=0)
		||(memcmp(pWaveHdr->szWaveFmt,"WAVEfmt ",8)!=0)
		||((pWaveHdr->nChannels!=1)&&(pWaveHdr->nChannels!=2))
		||((pWaveHdr->wBitsPerSample!=8)&&(pWaveHdr->wBitsPerSample!=16))
		||(pWaveHdr->nAvgBytesPerSec!=pWaveHdr->nChannels*pWaveHdr->wBitsPerSample*pWaveHdr->nSamplesPerSec/8)
		||(pWaveHdr->nBlockAlign!=pWaveHdr->nChannels*pWaveHdr->wBitsPerSample/8))
	{
	        hal_HstSendEvent(SYS_EVENT,0x11140020);
	    	return MCI_ERR_BAD_FORMAT;
	}
	}
      else{
#if ADPCM_SUPPORT        
          if((memcmp(pWaveHdr->szRiff,"RIFF",4)!=0)
		||(memcmp(pWaveHdr->szWaveFmt,"WAVEfmt ",8)!=0)
		||((pWaveHdr->nChannels!=1)&&(pWaveHdr->nChannels!=2)))
#endif
		{
		    hal_HstSendEvent(SYS_EVENT,0x11140030);
		   MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_WAV]parse ADPCM header error\n");
		 	return MCI_ERR_BAD_FORMAT;	
		}

       }
	
	if(pWaveHdr->dwFmtSize>=18)     /* We're obviously dealing with WAVEFORMATEX */
	{
             UINT32 cbSize,size;

		size = pWaveHdr->dwFmtSize -18;
 
		readlen=FS_Read(input->fileHandle,(UINT8*)&cbSize,2);
	
		if(readlen!=2)
		{
			diag_printf("[MMC_AUDIO_WAV]Optional infomation read error!:%d\n",readlen);
			return MCI_ERR_ERROR;
		}		

              cbSize = MIN(size, cbSize);

              hal_HstSendEvent(SYS_EVENT,0x11140050);
              hal_HstSendEvent(SYS_EVENT,cbSize);
       
	       if(cbSize > 0) {
		if( (cbSize >=22)&&( pWaveHdr->wFormatTag==0xfffe )){ /* WAVEFORMATEXTENSIBLE */
                    diag_printf("[MMC_AUDIO_WAV] ERROR!!!:  Don't support  WAVEFORMATEXTENSIBLE mode!!\n");
			return MCI_ERR_BAD_FORMAT;
			}

              input->extradata_size = cbSize; 			  
		input->extradata = (uint8 *)mmc_MemMalloc((input->extradata_size + WAV_INPUT_BUFFER_PADDING_SIZE));

		if(input->extradata==NULL)
		{
//tianwq				hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
			mmc_MemFreeAll();
			return MCI_ERR_OUT_OF_MEMORY;
			
		}	 
		
              readlen=FS_Read(input->fileHandle,input->extradata,input->extradata_size);
			  
		if(readlen!=input->extradata_size)
		{
			diag_printf("[MMC_AUDIO_WAV]Optional infomation read error!:%d\n",readlen);
			return MCI_ERR_ERROR;
		}		
	       }   
       

             size -= cbSize;
			 
            /* It is possible for the chunk to contain garbage at the end */
             if (size > 0){
			 	
		    INT32 result;
                  result=FS_Seek(input->fileHandle,size,FS_SEEK_CUR);

		if(result<0 ){
			MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV_ERR]FS_seek ERROR!");
			return MCI_ERR_BAD_FORMAT;
		 }
            }
			 
             input->fileOffset = input->fileOffset + 2 + cbSize + size;
             g_WAVHeaderLength = g_WAVHeaderLength + 2 + cbSize + size;
	
			
	}
	

	//check samplerate
	for(i=0;i<sizeof(PCMSamplesRate)/sizeof(PCMSamplesRate[0]);i++)
	{
		if(pWaveHdr->nSamplesPerSec==PCMSamplesRate[i])
		{
			break;
		}
	}
	
	if(i>=sizeof(PCMSamplesRate)/sizeof(PCMSamplesRate[0]))
	{
	    hal_HstSendEvent(SYS_EVENT,0x11140060);
		return MCI_ERR_INVALID_FORMAT;
	}

	readlen=FS_Read(input->fileHandle,ChunkInfoID,4);
	
	if(readlen!=4)
	{
		diag_printf("[MMC_AUDIO_WAV]data header read len:%d\n",readlen);
		return MCI_ERR_ERROR;
	}

	input->fileOffset+=4;
	g_WAVHeaderLength+=4;

	if(memcmp(ChunkInfoID,"fact",4)==0)
	{
		readlen=FS_Read(input->fileHandle,(UINT8 *)&ChunkInfoSize,4);
		
		if(readlen!=4||ChunkInfoSize!=4)
		{
			diag_printf("[MMC_WAV]Fact Chunk Size Read Error!:%d\n",readlen);
			return MCI_ERR_ERROR;
		}

		readlen=FS_Read(input->fileHandle,(UINT8 *)&ChunkInfoData,4);
		
		if(readlen!=4)
		{
			diag_printf("[MMC_WAV]Fact Chunk Data Read Error!:%d\n",readlen);
			return MCI_ERR_ERROR;
		}


		readlen=FS_Read(input->fileHandle,(UINT8 *)pWaveDataHdr,sizeof(WAVE_DATA_HEADER));
		
		if(readlen!=sizeof(WAVE_DATA_HEADER))
		{
			diag_printf("[MMC_WAV]Data Chunk Read Error!:%d\n",readlen);
			return MCI_ERR_ERROR;
		}
		input->fileOffset+=16;
		g_WAVHeaderLength+=16;
				
	}	
	else if(memcmp(ChunkInfoID,"data",4)==0)
	{
		pWaveDataHdr->szData[0]=ChunkInfoID[0];
		pWaveDataHdr->szData[1]=ChunkInfoID[1];
		pWaveDataHdr->szData[2]=ChunkInfoID[2];
		pWaveDataHdr->szData[3]=ChunkInfoID[3];

		readlen=FS_Read(input->fileHandle,(UINT8 *)&pWaveDataHdr->dwDataSize,4);
		
		if(readlen!=4)
		{
			diag_printf("[MMC_WAV]Data Chunk DataSize Read Error!:%d\n",readlen);
			return MCI_ERR_ERROR;
		}
		
		input->fileOffset+=4;
		g_WAVHeaderLength+=4;		

	}
	else
	{
    	hal_HstSendEvent(SYS_EVENT,0x11140080);
		diag_printf("[MMC_WAV]data header error [%d][%d][%d][%d]\n",pWaveDataHdr->szData[0],pWaveDataHdr->szData[1],pWaveDataHdr->szData[2],pWaveDataHdr->szData[3]);
		return MCI_ERR_BAD_FORMAT;	
	}

	if(pWaveDataHdr->dwDataSize == 0)
	{
		return MCI_ERR_END_OF_FILE;
	}
       input->codec        =   pWaveHdr->wFormatTag;
	input->codec_id    =  wav_codec_get_id(pWaveHdr->wFormatTag, pWaveHdr->wBitsPerSample);
	input->block_align = pWaveHdr->nBlockAlign;
	input->channels = (uint8)pWaveHdr->nChannels;	
	output->SampleRate=(uint16)pWaveHdr->nSamplesPerSec;
	output->Bits=(uint8)pWaveHdr->wBitsPerSample;
	
	return MCI_ERR_NO;

}

/*
* NAME:	MMC_AudioWavFileReadFinish()
*/
void MMC_AudioWavFileReadFinish(void *pParam)
{
	FS_ASYN_READ_RESULT *pRsp = ( FS_ASYN_READ_RESULT*)pParam;
	
	if(pRsp->iResult!=0)
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MMC_WAV_ERR] READING FILE ERROR!");  
		mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_WAV_PCM_END);
		return;
	}


	if(pRsp->uSize < (g_WavOutputPcmBufSize>>1))
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]READ LENGTHE:%d",pRsp->uSize);
		g_read_end = 1;
		return;
	}
//	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]MMC_AudioWavFileReadFinish g_copyed_frame:%d,g_readed_frame:%d,g_stop_read:%d",g_copyed_frame,g_readed_frame,g_stop_read);

	WavPlayer->WavInput.fileOffset = WavPlayer->WavInput.fileOffset + pRsp->uSize;

	g_readed_frame++;
	
	if(g_readed_frame == g_nFrame)
	{
		g_readed_frame = 0;
	}
	
	if(g_readed_frame != g_copyed_frame)
	{
		FS_AsynReadReq(WavPlayer->WavInput.fileHandle, WavPlayer->WavInput.bof + g_readed_frame * (g_WavOutputPcmBufSize>>1), g_WavOutputPcmBufSize>>1, MMC_AudioWavFileReadFinish);
	}
	else if(g_readed_frame == g_copyed_frame)
	{
		g_stop_read = 1;
	}
    return;
} 

void MMC_WavPcmOutStart(WAV_PLAY *pWavPlayer)
{

	hal_HstSendEvent(SYS_EVENT,0x88883000);    	
	hal_HstSendEvent(SYS_EVENT,pWavPlayer->WavOutput.Buffer);    	
	hal_HstSendEvent(SYS_EVENT,pWavPlayer->WavOutput.Size);    	
	hal_HstSendEvent(SYS_EVENT,pWavPlayer->WavOutput.Channel);    	
	hal_HstSendEvent(SYS_EVENT,pWavPlayer->WavOutput.Bits);    	

	
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_WAV]MMC_WavPcmOutStart BEGIN!");
#ifdef bt_support
	if(g_WavBT8KEnbleFlag==TRUE)
	{
		audioStream.startAddress  = (UINT32*) g_WavBT8kPcmBuffer;//pAudioPlayer->MpegOutput.pcm.Buffer;
		audioStream.length        = g_WavTotalFrame*g_WavNumOfOutSample*2;

		//   streamOut.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		audioStream.channelNb     =HAL_AIF_MONO; //pAudioPlayer->Voc_AudioDecStatus.nbChannel;//HAL_AIF_STEREO;//HAL_AIF_MONO;

		pWavPlayer->Voc_AudioDecStatus.SampleRate=HAL_AIF_FREQ_8000HZ;

		audioItf = AUD_ITF_BLUETOOTH;

	}
	else
#endif
	{
	// Stream out
		audioStream.startAddress  	= (UINT32 *)pWavPlayer->WavOutput.Buffer;
		audioStream.length        	= pWavPlayer->WavOutput.Size*4;//MP3DEC_PCM_OUTPUT_BUFFER_SIZE;//AIF_FIFO_LEN * 4;
		audioStream.channelNb     	= pWavPlayer->WavOutput.Channel;//HAL_AIF_STEREO;//HAL_AIF_MONO;

	}
	audioStream.voiceQuality   = !TRUE;    
	audioStream.halfHandler   	= MMC_AudioWavHalfPcmISR;
	audioStream.endHandler    = MMC_AudioWavEndPcmISR;

    // Initial cfg
   	{
   	  	  // Initial cfg
   	  	  if(gpio_detect_earpiece())
    		audioItf = AUD_ITF_EAR_PIECE;
          else
            audioItf = AUD_ITF_LOUD_SPEAKER;
   	}

	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_WAV]SAMPLERATE:%d",pWavPlayer->WavOutput.SampleRate);

	switch (pWavPlayer->WavOutput.SampleRate)
	{
	case 48000:
  		audioStream.sampleRate    = HAL_AIF_FREQ_48000HZ;
		break;
	case 44100:
		audioStream.sampleRate    = HAL_AIF_FREQ_44100HZ;
		break;
	case 32000:
		audioStream.sampleRate    = HAL_AIF_FREQ_32000HZ;
		break;
	case 24000:
		audioStream.sampleRate    = HAL_AIF_FREQ_24000HZ;
		break;
	case 22050:
		audioStream.sampleRate    = HAL_AIF_FREQ_22050HZ;
		break;
	case 16000:
		audioStream.sampleRate    = HAL_AIF_FREQ_16000HZ;
		break;
	case 12000:
		audioStream.sampleRate    = HAL_AIF_FREQ_12000HZ;
		break;
	case 11025:
		audioStream.sampleRate    = HAL_AIF_FREQ_11025HZ;
		break;
	case 8000:
		audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;
		break;			
	default:
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MMC_WAV_ERROR]ERROR SAMPLERATE:%d",pWavPlayer->WavOutput.SampleRate);

		break;
	}
	
	aud_StreamStart(audioItf, &audioStream, &audio_cfg);
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_WAV] END!");

}


PUBLIC MCI_ERR_T MMC_WavGetFileInformation (
                        CONST INT32 FileHander,
                         AudDesInfoStruct  * CONST DecInfo  )
{

  WAV_PLAY WavPlay;
 WAV_INPUT *input = &WavPlay.WavInput;
  int32 result;

   hal_HstSendEvent(SYS_EVENT,0x11140010);
    hal_HstSendEvent(SYS_EVENT,g_WavPlayFlag);
    hal_HstSendEvent(SYS_EVENT,FileHander);
   if(g_WavPlayFlag == 0)
   {
        input->fileOffset=0;
         //judge the file handler
    	input->fileHandle = FileHander;
    	if (input->fileHandle<0)
    	{
    		diag_printf("[MMC_WAV]MMC_WavGetFileInformation FILE HANDLER ERROR!: %d \n", FileHander);
    		return MCI_ERR_CANNOT_OPEN_FILE;
    	}
    	else
    	{
    		diag_printf("[MMC_WAV] MMC_WavGetFileInformation FILE HANDLER: %d\n", FileHander);
    	}

       if ((g_WAVFILESIZE=FS_GetFileSize(FileHander))< 0)
       {
           MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_ERROR]CAN NOT GET FILE SIZE!:%d",g_WAVFILESIZE);
           return MCI_ERR_CANNOT_OPEN_FILE;
       }
	
       g_WAVHeaderLength=0;
 
      	//parse wav file header
    	if((result= MMC_WavHeaderParse(&WavPlay))!=MCI_ERR_NO)
    	{
    		return result;
    	}
    }
									// get the infromation.
	DecInfo->sampleRate             = WavPlay.WavOutput.SampleRate;
	DecInfo->stereo                 = WavPlay.WavOutput.Channel!=1;
    DecInfo->bitRate = WavPlay.WaveHeader.nAvgBytesPerSec*8;
    if(DecInfo->bitRate == 0)
        DecInfo->time = 0;
    else
        DecInfo->time = g_WAVFILESIZE*8/DecInfo->bitRate*1000+500;

    hal_HstSendEvent(SYS_EVENT,0x89200000);
    hal_HstSendEvent(SYS_EVENT,DecInfo->sampleRate);
    hal_HstSendEvent(SYS_EVENT,DecInfo->bitRate);
    hal_HstSendEvent(SYS_EVENT,DecInfo->time);
    
    diag_printf("[MMC_WAV]MMC_WavGetFileInformation DecInfo->sampleRate : %d , DecInfo->stereo : %d\n", DecInfo->sampleRate,DecInfo->stereo );
					
      return MCI_ERR_NO;

}




/*
* NAME:	MMC_AudioWavDecOpen()
* DESCRIPTION: Initialize wav decoder.
*/
extern MPEG_PLAY MpegPlayer;
INT32 *G_WavSBCConstY=NULL;
extern const INT32 G_VppSBCConstX[];

MCI_ERR_T MMC_AudioWavDecOpen (HANDLE fhd, WAV_PLAY *pWavPlayer)
{
    if(pWavPlayer == NULL)
        pWavPlayer = WavPlayer = COS_Malloc(sizeof(WAV_PLAY));
	WAV_INPUT *input = &pWavPlayer->WavInput;
	WAV_OUTPUT *output = &pWavPlayer->WavOutput;

    HAL_ERR_T voc_ret = 0;
	
	int32 readlen = 0;
	int32 result;

	g_WavBT8KEnbleFlag=FALSE;//TRUE;


#ifdef bt_support
	 if(g_WavBT8KEnbleFlag==TRUE)
	{

	 	g_Wav16BitInputBufSize =(8*1024*4);

 		g_WavOutputPcmBufSize =(8*1024);

	 	g_nFrame               = (g_Wav16BitInputBufSize/(g_WavOutputPcmBufSize>>1));

	 	g_WavInputBufSize	  =(g_WavOutputPcmBufSize/2);
	 }
	 else
#endif
	 {
//	 	g_Wav16BitInputBufSize =(8*1024);//*8

 		g_WavOutputPcmBufSize =(6*1024); // in bytes

	 	g_nFrame               = (g_Wav16BitInputBufSize/(g_WavOutputPcmBufSize>>1));

	 	g_WavInputBufSize	  =(g_WavOutputPcmBufSize);
	 }

	 data_size =(g_WavOutputPcmBufSize>>1);
	
	g_Wavcounterframe=0;
	g_WavResampleInitFlag=1;


#ifdef bt_support
   	if(g_WavBT8KEnbleFlag==TRUE)
   	{
   		g_WavBT8kPcmBuffer=(INT16 *)mmc_MemMalloc(g_WavOutputPcmBufSize);   
		memset(g_WavBT8kPcmBuffer, 0, g_WavOutputPcmBufSize);
   	}
#endif

	input->fileOffset=0;
       input->is_first_frame=1;
	 g_sbc_frame =2;  
	//judge the file handler
	input->fileHandle = fhd;
	if (input->fileHandle<0)
	{
		diag_printf("[MMC_WAV]FILE HANDLER ERROR!: %d \n", fhd);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}
	else
	{
		diag_printf("[MMC_WAV] FILE HANDLER: %d\n", fhd);
	}
	

	input->fileTotalLen=FS_GetFileSize(fhd);
	
	diag_printf("[MMC_WAV]FILE TOTAL LENGTH:%d\n",input->fileTotalLen);

	g_WAVHeaderLength=0;
	
	//parse wav file header
	if((result= MMC_WavHeaderParse(pWavPlayer))!=MCI_ERR_NO)
	{
		return result;
	}

	g_WAVConsumedLength=g_WAVConsumedLength-g_WAVHeaderLength;

	if(g_WAVConsumedLength<0)g_WAVConsumedLength=0;

	g_WAVConsumedLength=g_WAVConsumedLength&0xfffffffC;

 	if((g_WAVConsumedLength>0)&& ( input->codec != 1 ) ){ //ADPCM
 	
        int times;
     MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]g_WAVConsumedLength :%d",g_WAVConsumedLength);

	   times = g_WAVConsumedLength / input->block_align;
	   
          g_WAVConsumedLength = (times+1)*input->block_align;
	  MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]g_WAVConsumedLength :%d ,times: %d",g_WAVConsumedLength,times);	  

 	}

//tianwq		hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);
	output->Buffer= (uint32 *)pcmbuf_overlay;
	memset(output->Buffer, 0, g_WavOutputPcmBufSize);
	//output->Size= (g_WavOutputPcmBufSize) >> 2;//int 
	output->Buffer_head=output->Buffer;
    output->Channel=2;
	//output->OnPCM = MMC_AudioWavPcmISR;
	
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]g_WAVConsumedLength :%d",g_WAVConsumedLength);
	
	result=FS_Seek(input->fileHandle,g_WAVConsumedLength,FS_SEEK_CUR);

	input->fileOffset=g_WAVConsumedLength+g_WAVHeaderLength;

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]FILE SEEK POSITION:%d",result);
	
	if(result<0 )
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV_ERR]FS_seek ERROR!");
		return MCI_ERR_BAD_FORMAT;
	}
	
	//fill the pcm out buffer first,only consider that the pcm out module is 16bit situation
	if(WAV_DATA_DEPTH_16BIT != output->Bits)
	{
	    //g_WavInputBufSize = g_WavInputBufSize*output->Bits/WAV_DATA_DEPTH_16BIT;
    }

    if(1 == input->channels)
    {
        g_WavInputBufSize /= 2;
    }
    if(WAV_DATA_DEPTH_8BIT == output->Bits)
    {
        g_WavInputBufSize /= 2;
    }
    
	output->Size = (g_WavOutputPcmBufSize) >> 2;

#if ADPCM_SUPPORT==1
    if(input->codec != 1  ) // adpcm
    {
        if(pWavPlayer->WavInput.codec_id == AdpcmCODEC_ID_NONE)
            return MCI_ERR_BAD_FORMAT;
        //g_WavInputBufSize /= 8;
        //input->block_align = g_WavInputBufSize;
        g_WavInputBufSize = input->block_align;
        
        input->bof = ((uint8 *)pcmbuf_overlay)+g_WavOutputPcmBufSize  ;//mmc_MemMalloc(g_WavInputBufSize);
        memset(input->bof, 0, g_WavInputBufSize);
        input->length = g_WavInputBufSize;
        input->data = input->bof;
        input->eof = input->bof + input->length;

        input->priv_data   = (uint8 *)mmc_MemMalloc(sizeof(ADPCMContext));
	    output->tmpBuffer = ((uint8 *)pcmbuf_overlay)+g_WavOutputPcmBufSize+g_WavInputBufSize ;// (uint8 *) mmc_MemMalloc(WAV_ADPCM_TMP_BUFSIZE);
	    output->tmpBuf_read = NULL;
	    output->Remain_Size = 0;
	  
        if((input->priv_data == NULL)|(output->tmpBuffer == NULL))
	    {
    //tianwq			hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
    		mmc_MemFreeAll();
    		return MCI_ERR_OUT_OF_MEMORY;
			
	    }

          adpcm_decode_init(input );

         output->Bits = WAV_DATA_DEPTH_16BIT;

	    diag_printf("[MMC_ADPCM]  Finished adpcm_decode_init:  codec_id is  0x%x    \n", input->codec_id);
     }

#endif

#if 0
          hal_HstSendEvent(SYS_EVENT,0x09190001);
          hal_HstSendEvent(SYS_EVENT,output->Bits);
          hal_HstSendEvent(SYS_EVENT,output->SampleRate);
          hal_HstSendEvent(SYS_EVENT,output->Channel);
          hal_HstSendEvent(SYS_EVENT,input->codec_id);
          hal_HstSendEvent(SYS_EVENT,input->channels);
          hal_HstSendEvent(SYS_EVENT,input->block_align);
#endif
		
/*	else if(WAV_DATA_DEPTH_16BIT == output->Bits)
	{
		input->bof = ((uint8 *)pcmbuf_overlay)+g_WavOutputPcmBufSize;//mmc_MemMalloc(g_Wav16BitInputBufSize);


		if(input->bof==NULL)
		{
//tianwq				hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
			mmc_MemFreeAll();
			return MCI_ERR_OUT_OF_MEMORY;
			
		}
		
		memset(input->bof, 0, g_Wav16BitInputBufSize);
		input->length = g_Wav16BitInputBufSize;
		input->data = input->bof;
	
		input->eof = input->bof + input->length;
		
		output->Size = (g_WavOutputPcmBufSize) >> 2;

         if(input->codec == 1  ){ //PCM
		 	
		readlen = FS_Read(input->fileHandle,input->bof,g_Wav16BitInputBufSize);
		
		if(readlen < g_Wav16BitInputBufSize)
		{
			MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]BUFFER IS NOT FULL!");
			
			g_input_buf_not_full = 1;
			g_buffered_frame = readlen/(g_WavOutputPcmBufSize>>1)+1;
			g_copyed_frame = 0;
			g_stop_read = 0;
		}
		else
		{
			input->fileOffset+=readlen;
			g_copyed_frame = 0;
			g_readed_frame = 0;
			g_stop_read = 1;
			g_input_buf_not_full = 0;
		}

         	}

		
		g_read_end = 0;
	}
    */
	g_continue_play_flag = 0;

#ifdef bt_support
	 if( is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1))      //BT_A2DP_sbc
	 {


	   pWavPlayer->Voc_AudioDecStatus.consumedLen = 0;
	   pWavPlayer->Voc_AudioDecStatus.nbChannel = 0;
	   pWavPlayer->Voc_AudioDecStatus.output_len = 0;
	   pWavPlayer->Voc_AudioDecStatus.streamStatus= 0;
			
	   pWavPlayer->Voc_AudioDecIN.EQ_Type=-1;
	   pWavPlayer->Voc_AudioDecIN.reset=1;

	G_WavSBCConstY = (INT32*)mmc_MemMalloc(468*4);
	if( G_WavSBCConstY==NULL)
	{
		mmc_MemFreeAll();
		return MCI_ERR_OUT_OF_MEMORY;		
	}

	/*open voc decoder*/
	voc_ret = vpp_AudioJpegDecOpen((HAL_VOC_IRQ_HANDLER_T)MMC_AudioWavVocISR);
	 
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
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_WAVSBC_ERROR]Fail to Call vpp_AudioJpegDecOpen()t: %d \n", voc_ret);
		return MCI_ERR_ERROR;
	default:
		break;
	}
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_WAVSBC]MMC_AudioDecOpen end!");
	}
   	if(g_WavBT8KEnbleFlag==TRUE)
   	{

		g_WavTotalFrame=2;

		g_WavNumOfOutSample=ResampleInit(pWavPlayer->WavOutput.SampleRate, 8000, g_WavOutputPcmBufSize>>1, pWavPlayer->WavOutput.Channel);
					
		g_WavResampleInitFlag=0; 

   	}
#endif

	MMC_WavPcmOutStart(pWavPlayer);
    g_WavPlayFlag= 1;
	
	return MCI_ERR_NO;
	
}



MCI_ERR_T MMC_AudioWavDecVoC(WAV_PLAY *pWavPlayer)
{

	WAV_INPUT *input = &pWavPlayer->WavInput;
	WAV_OUTPUT *output = &pWavPlayer->WavOutput;
	
	vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus = &pWavPlayer->Voc_AudioDecStatus;
	vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pWavPlayer->Voc_AudioDecIN;

   return MCI_ERR_NO;

}

/*
* NAME:	MMC_AudioWavDecPcm()
* DESCRIPTION: Initialize wav decoder.
*/
MCI_ERR_T MMC_AudioWavDecPcm(WAV_PLAY *pWavPlayer)
{
	WAV_INPUT *input = &pWavPlayer->WavInput;
	WAV_OUTPUT *output = &pWavPlayer->WavOutput;
	int32 i,readlen;
   int32 length;
   CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
  
    g_wav_read_flag=1;
    g_sbc_frame=0;

	if(g_continue_play_flag==1)
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]END PLAYING !");
		mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_WAV_PCM_END);
	}
	else if(g_continue_play_flag==2)
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]CONTINUE PLAY!");
		g_continue_play_flag--;

		if(WAV_DATA_DEPTH_8BIT == pWavPlayer->WavOutput.Bits)
		{
			MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]8 BIT WAV END PLAYING !");
			mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_WAV_PCM_END);
		}
		
	}
	else
	{
		//check the remain data 
//		if(WAV_DATA_DEPTH_8BIT == pWavPlayer->WavOutput.Bits)
		{
			if((input->fileTotalLen-input->fileOffset) < (g_WavInputBufSize))
			{
				return MCI_ERR_END_OF_FILE;
			}
		}
	/*	else if(WAV_DATA_DEPTH_16BIT == pWavPlayer->WavOutput.Bits)
		{
			if((input->fileTotalLen-input->fileOffset) < g_WavInputBufSize)
			{
				//return MCI_ERR_END_OF_FILE;
				g_read_end = 1;
			}
		}*/

	
		if(Mmc_Wav_Pcm_Half==0)
		{
			output->Buffer_head=output->Buffer; 		
		}
		else if(Mmc_Wav_Pcm_Half==1)
		{
			output->Buffer_head=output->Buffer+(output->Size>>1);			
		}
		
		g_WavBTDataAddress=(INT16 *)output->Buffer_head;
        if(input->codec == 1  ) //PCM
		{
			readlen = FS_Read(input->fileHandle, output->Buffer_head, g_WavInputBufSize >> 1);
			
			if(readlen<0)
			{
				return MCI_ERR_ERROR;
			}
			input->fileOffset+=readlen;

			
			g_WAVConsumedLength+=readlen;//(g_WavOutputPcmBufSize>>2);

            //if(WAV_DATA_DEPTH_16BIT == pWavPlayer->WavOutput.Bits)
            {
             //   memcpy(output->Buffer_head, input->data, readlen);
            }
            if(WAV_DATA_DEPTH_8BIT == output->Bits)
            {
		INT16 *PCM_p=(INT16 *)output->Buffer_head;
                INT8 *data = (INT8 *)output->Buffer_head;
            
		for(i=readlen-1;i>=0;i--)
		{
                    PCM_p[i]=((INT16)((INT8)(data[i])-0x80))<<8;
		}
            }

#ifdef bt_support
			 if(g_WavBT8KEnbleFlag==TRUE)
			{

				if(output->Channel==2)
				{
					INT16 *PCM_p=(INT16 *)g_WavBTDataAddress;
					
					for(i=0;i<(g_WavOutputPcmBufSize>>2);i=i+2)
					{
						PCM_p[i/2]=(PCM_p[i]>>1)+(PCM_p[i+1]>>1);					
					}
				}

				/* Resample stuff in input buffer */        

				g_WavNout=ResampleOneFrame(g_WavBTDataAddress,g_WavBT8kPcmBuffer+g_Wavcounterframe*g_WavNumOfOutSample);

				g_Wavcounterframe++;

				if(g_Wavcounterframe>=g_WavTotalFrame)
				{
					g_Wavcounterframe=0;
				}

	          		MCI_TRACE (TSTDOUT,0,"g_WavNout = %d",g_WavNout);  

			}
#endif
		}
#if ADPCM_SUPPORT==1
        else
        {
        
                 
		   uint32 remain_pcm = 0 ;
		   
		   uint8 *Buffer_end;
           if((2==output->Channel)&&(1==input->channels))
                Buffer_end = (uint8 *)  ( output->Buffer_head +(output->Size>>2)) ;
           else
               Buffer_end = (uint8 *)  ( output->Buffer_head +(output->Size>>1)) ;

            output->write =(uint8 *) output->Buffer_head;
            
#if 0
                 hal_HstSendEvent(SYS_EVENT,0x09170000);
                 hal_HstSendEvent(SYS_EVENT,Buffer_end);
                 hal_HstSendEvent(SYS_EVENT,output->write);
                 hal_HstSendEvent(SYS_EVENT,output->Remain_Size);
                 hal_HstSendEvent(SYS_EVENT,output->tmpBuf_read);
#endif		  
                remain_pcm =  Buffer_end -output->write;

                if(output->Remain_Size >= remain_pcm)
                {
	                memcpy(output->write, output->tmpBuf_read, remain_pcm);
	                output->tmpBuf_read += remain_pcm;
	                output->Remain_Size -= remain_pcm;
                }
                else
                {
                   if(output->Remain_Size > 0)
                   {
                     memcpy(output->write,   output->tmpBuf_read ,output->Remain_Size);
                     output->write += output->Remain_Size;
                     output->Remain_Size = 0;
                   }

                   remain_pcm =  Buffer_end -output->write;
                   do{
                       readlen = FS_Read(input->fileHandle,input->bof,input->block_align);

           	           if(readlen  < input->block_align){
                             g_read_end = 1;
                            return MCI_ERR_END_OF_FILE;
                       }
                         
             	       data_size =  WAV_ADPCM_TMP_BUFSIZE;
        	           length =  adpcm_decode_frame(input,output->tmpBuffer,&data_size, input->bof,readlen);

                     if(length<=0) // decode error
                        {
                            return MCI_ERR_UNKNOWN_FORMAT;
                        }
                     
        	           g_WAVConsumedLength+=readlen;// length;

#if 0
                       hal_HstSendEvent(SYS_EVENT,0x09170003);
                       hal_HstSendEvent(SYS_EVENT,readlen);
                       hal_HstSendEvent(SYS_EVENT,length);
                       hal_HstSendEvent(SYS_EVENT,data_size);
                       hal_HstSendEvent(SYS_EVENT,remain_pcm);
#endif
                       if(remain_pcm > data_size)
                       {
                            memcpy(output->write, output->tmpBuffer, data_size );
                            remain_pcm -= data_size;
                            output->write += data_size;
                        }
                       else
                        {
                           memcpy(output->write, output->tmpBuffer, remain_pcm );
            				
                           output->tmpBuf_read =  output->tmpBuffer + remain_pcm ;
                           output->Remain_Size =  data_size - remain_pcm;
                           remain_pcm = 0;
                        }
                   } while(remain_pcm>0);
                 }
           }
#endif
            if((2==output->Channel)&&(1==input->channels))
            {
                INT16 *PCM_p=(INT16 *)output->Buffer_head;
                for(i=(output->Size)-2 ;i>=0;i=i-2)
                {
                    PCM_p[i]=PCM_p[i+1]=PCM_p[i>>1];
                }
                
            }
		    else  if((2==output->Channel)&&audioCfg[audioItf].mixAudio)//(AUD_ITF_LOUD_SPEAKER==audioItf||AUD_ITF_BT_SP==audioItf))
			{

				INT16 *PCM_p=(INT16 *)output->Buffer_head;
				
				for(i=0;i<(g_WavOutputPcmBufSize>>2);i=i+2)
				{
					PCM_p[i]=PCM_p[i+1]=(PCM_p[i]>>1)+(PCM_p[i+1]>>1);					
				}
				
			}			


            MCI_MergeSideTone(g_WavOutputPcmBufSize/2, output->Buffer_head);
        
#if 0		

			 if(g_WavBT8KEnbleFlag==TRUE)
			{

				if(output->Channel==2)
				{
					INT16 *PCM_p=(INT16 *)g_WavBTDataAddress;
					
					for(i=0;i<(g_WavOutputPcmBufSize>>2);i=i+2)
					{
						PCM_p[i/2]=(PCM_p[i]>>1)+(PCM_p[i+1]>>1);					
					}
				}

				/* Resample stuff in input buffer */        

				g_WavNout=ResampleOneFrame(g_WavBTDataAddress,g_WavBT8kPcmBuffer+g_Wavcounterframe*g_WavNumOfOutSample);

				g_Wavcounterframe++;

				if(g_Wavcounterframe>=g_WavTotalFrame)
				{
					g_Wavcounterframe=0;
				}

	          		MCI_TRACE (TSTDOUT,0,"g_WavNout = %d",g_WavNout);  

			}
		    if(input->codec == 1  ) //PCM
		    	{
		    	
			g_copyed_frame++;
			
			if(g_copyed_frame == g_nFrame)
			{
				MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]COPYED FRAME IS RESET!");
				g_copyed_frame = 0;
			}
			
			if(g_read_end)
			{
				if(g_copyed_frame == g_readed_frame)
				{
					MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]END OF FILE!");
					return MCI_ERR_END_OF_FILE;
				}
			}
			
		    	}
			//patch for short file
			if(g_input_buf_not_full)
			{
				if(0 == g_buffered_frame)
				{
					return MCI_ERR_END_OF_FILE;
				}		

				if(g_copyed_frame == g_buffered_frame)
				{
					return MCI_ERR_END_OF_FILE;
				}
			}
			

	            if(input->codec == 1  ) //PCM
		    	{
		    	
			if(g_stop_read)
			{
				FS_AsynReadReq(input->fileHandle, input->bof + g_readed_frame * (g_WavOutputPcmBufSize>>1), g_WavOutputPcmBufSize>>1, MMC_AudioWavFileReadFinish);
				g_stop_read = 0;
			}
	            	}
				
		}
#endif
	}
g_wav_read_flag=0;
	return MCI_ERR_NO;
}

/*
* NAME:	MMC_AudioWavDecClose()
* DESCRIPTION: Close wav decoder.
*/
MCI_ERR_T MMC_AudioWavDecClose (WAV_PLAY *pWavPlayer)
{

	aud_StreamStop(audioItf);
	vpp_AudioJpegDecClose();
	mmc_MemFreeAll();
	g_WavPlayFlag = 0;
//tianwq		hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
    COS_Free(WavPlayer);
    WavPlayer = NULL;
	wav_pcm_irq_num = 0;
	
	return  MCI_ERR_NO;
	
}


int32 Audio_WavPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
	int32 result;

    if(WavPlayer == NULL)
        WavPlayer = COS_Malloc(sizeof(WAV_PLAY));

	WavPlayer->WavOutput.OutputPath = OutputPath;
	g_WAVConsumedLength=0;


	if (fhd< 0)
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_ERROR]ERROR FILE HANDLER: %d \n", fhd);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

	if ((g_WAVFILESIZE=FS_GetFileSize(fhd))< 0)
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_ERROR]CAN NOT GET FILE SIZE!:%d",g_WAVFILESIZE);
		return MCI_ERR_CANNOT_OPEN_FILE;
	}

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_AUDIO_WAV]FILE SIZE: %d",g_WAVFILESIZE);

	g_WAVConsumedLength=(INT32)(((INT64)PlayProgress*(INT64)g_WAVFILESIZE)/10000);

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_WAV]g_AMRConsumedLength: %d",g_WAVConsumedLength);

    result=FS_Seek(fhd,0,FS_SEEK_SET);
	if((result= MMC_AudioWavDecOpen(fhd, WavPlayer))!=MCI_ERR_NO)
	{
		MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV_ERR]MMC_AudioDecOpen() RETURN FALSE!");
		MMC_AudioWavDecClose(WavPlayer);
		return result;
	}
    
	return  MCI_ERR_NO;
}

int32 Audio_WavStop (void) 	
{
	MMC_AudioWavDecClose(WavPlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_WavPause (void)	
{
MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV] Audio_WavPause!!!!");

	MMC_AudioWavDecClose(WavPlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_WavResume ( HANDLE fhd) 
{
	int32 result;
	
	MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_WAV]Audio_WavResume!");
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV] Audio_WavResume!!!!");

	g_WAVConsumedLength-=(g_WavOutputPcmBufSize);

	if(g_WAVConsumedLength<0)g_WAVConsumedLength=0;



	result=FS_Seek(fhd,0,FS_SEEK_SET);

	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV]FILE SEEK POSITON:%d",result);
	
	if(result<0 )
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_WAV_ERR]FS_seek error!");
		return MCI_ERR_BAD_FORMAT;
	}


	if((result= MMC_AudioWavDecOpen(fhd, WavPlayer))!=MCI_ERR_NO)
	{
		MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_WAV_ERR]MMC_AudioDecOpen RETURN FALSE!");
		MMC_AudioWavDecClose(WavPlayer);
		return result;
	}
	return  MCI_ERR_NO;
}


int32 Audio_WavGetID3 (char * pFileName)  
{
	return 0;	
}

int32 Audio_WavGetPlayInformation (MCI_PlayInf * MCI_PlayInfWAV)  
{
    if(g_WAVFILESIZE == 0)
        MCI_PlayInfWAV->PlayProgress = 0;
    else
    	MCI_PlayInfWAV->PlayProgress=(INT32)((((INT64)g_WAVConsumedLength+g_WAVHeaderLength)*10000)/g_WAVFILESIZE);
	
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_WAV]PlayProgress:%d",MCI_PlayInfWAV->PlayProgress);
	
	return MCI_ERR_NO;
}

int32 Audio_WavUserMsg(int32 nMsg)
{
       MCI_ERR_T result;
	switch (nMsg)
	{
		case MSG_MMC_WAV_PCM_INT:
			result = MMC_AudioWavDecPcm(WavPlayer);
                    if(result==MCI_ERR_END_OF_FILE)
			{
				
				MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_WAV]END OF FILE!");				
				g_continue_play_flag = 2;
			}
                    else if(result != MCI_ERR_NO)
                    {
            			MMC_AudioWavDecClose(WavPlayer);
        			MCI_AudioFinished(result);
        			mmc_SetCurrMode(MMC_MODE_IDLE);
                     }
			break;

#ifdef bt_support
		case MSG_MMC_WAV_SBC_VOC:
		//	  MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_WAV]MSG_MMC_WAV_SBC_VOC! g_sbc_frame =%d",g_sbc_frame);	
                       if(g_sbc_frame<1)
                       {
			    MMC_AudioWavDecVoC(WavPlayer);
				g_sbc_frame++;
                        }
			break;
#endif			
			
		case MSG_MMC_WAV_PCM_END:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_WAV]MSG_MMC_WAV_PCM_END\n");
			MMC_AudioWavDecClose(WavPlayer);
			MCI_AudioFinished(MCI_ERR_END_OF_FILE);
			mmc_SetCurrMode(MMC_MODE_IDLE);
			
			break;
			
		default:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_WAV_ERR]ERROR MESSAGE!");
			
			break;
	}
    return 0;
}


