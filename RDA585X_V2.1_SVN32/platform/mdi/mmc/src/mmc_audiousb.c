////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audioUSB.c
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

#include "mmc_audiousb.h"

#include "mmc.h"
#include "cpu_share.h"
#include "csw_csp.h"

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
#include "ars_m.h"
#include "event.h"

#define MMC_USB_BUFF_SIZE               (480) // in pcm sample, total use 480*4 buffs, two for aif play, two for sco receive = 480*4*2bytes

static INT8 Mmc_USB_Pcm_Half;
static INT8 Mmc_USB_MIC_Half;

static INT8 g_continue_play_flag;

USB_PLAY *USBPlayer;

extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

extern HAL_AIF_STREAM_T micStream;

/*
* NAME:	MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_AudioUSBVocISR(void)
{
//	MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_USBSBC]VoC INT!");
//  	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_USB_SBC_VOC);
} 

/*
* NAME:	MMC_AudioUSBHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/

void MMC_AudioUSBHalfPcmISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_USB]HALF!");
	Mmc_USB_Pcm_Half = 0;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_USB_PCM_INT);
}

/*
* NAME:	MMC_AudioUSBEndPcmISR()
* DESCRIPTION: ISR for pcm end buffer exhausted.
*/
void MMC_AudioUSBEndPcmISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_USB]END!");
	Mmc_USB_Pcm_Half = 1;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_USB_PCM_INT);

}


/*
* NAME:	MMC_AudioUSBHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/

void MMC_AudioUSBHalfMicISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_USB]HALF!");
	Mmc_USB_MIC_Half = 0;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_USB_MIC_INT);
}

/*
* NAME:	MMC_AudioUSBEndPcmISR()
* DESCRIPTION: ISR for pcm end buffer exhausted.
*/
void MMC_AudioUSBEndMicISR(void)
{
	MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_USB]END!");
	Mmc_USB_MIC_Half = 1;
	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_USB_MIC_INT);

}

void MMC_USBSetFreq(UINT32 freq)
{
}

void MMC_USBPcmOutStart(USB_PLAY *pUSBPlayer)
{
        MCI_ERR_T result;
	
	// Stream out
	audioStream.startAddress  	= (UINT32 *)pUSBPlayer->USBOutput.Buffer;
	audioStream.length        	= pUSBPlayer->USBOutput.Size;
	audioStream.channelNb     	= HAL_AIF_STEREO;
    audioStream.sampleRate = HAL_AIF_FREQ_48000HZ;

	audioStream.voiceQuality   = !TRUE;    
	audioStream.halfHandler   	= MMC_AudioUSBHalfPcmISR;
	audioStream.endHandler    = MMC_AudioUSBEndPcmISR;
	
   	{
   	  	  // Initial cfg
   	  	  if(gpio_detect_earpiece())
    		audioItf = AUD_ITF_EAR_PIECE;
          else
            audioItf = AUD_ITF_LOUD_SPEAKER;
   	}

	//audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;
	
	result = aud_StreamStart(audioItf, &audioStream, &audio_cfg);

    // steam in
    micStream.startAddress  	= (UINT32 *)(pUSBPlayer->USBInput.data+MMC_USB_BUFF_SIZE);
	micStream.length        	= pUSBPlayer->USBInput.size;//MP3DEC_PCM_OUTPUT_BUFFER_SIZE;//AIF_FIFO_LEN * 4;
	micStream.channelNb     	= 1;//pUSBPlayer->USBOutput.Channel;//HAL_AIF_STEREO;//HAL_AIF_MONO;
	micStream.sampleRate    = HAL_AIF_FREQ_8000HZ;

	micStream.voiceQuality   = TRUE;    
	micStream.halfHandler    = MMC_AudioUSBHalfMicISR;
	micStream.endHandler    = MMC_AudioUSBEndMicISR;

    //result = aud_StreamRecord(audioItf, &micStream, &audio_cfg);
}



/*
* NAME:	MMC_AudioUSBDecOpen()
* DESCRIPTION: Initialize USB decoder.
*/
MCI_ERR_T MMC_AudioUSBDecOpen (USB_PLAY *pUSBPlayer, HANDLE sco_handle)
{
    if(pUSBPlayer == NULL)
        pUSBPlayer = USBPlayer = COS_Malloc(sizeof(USB_PLAY));
    USB_INPUT *input = &pUSBPlayer->USBInput;
    USB_OUTPUT *output = &pUSBPlayer->USBOutput;

    pUSBPlayer->in_flag = 0;
    input->sco_handle = sco_handle;

    output->Buffer= (uint16 *)pcmbuf_overlay;
    output->length = 0;
    output->Size = MMC_USB_BUFF_SIZE*4;
    memset(output->Buffer, 0, output->Size*2);

    input->data = pcmbuf_overlay+MMC_USB_BUFF_SIZE*2;
    input->length = MMC_USB_BUFF_SIZE;
    input->size = MMC_USB_BUFF_SIZE*2;//(MCI_INPUT_BUF_SIZE/pUSBPlayer->packet_size-1)*pUSBPlayer->packet_size*2;
    memset(input->data, 0, input->size*2);

    g_continue_play_flag = 1;

    hal_HstSendEvent(USB_EVENT,0x10280020);

    hal_HstSendEvent(USB_EVENT,output->Buffer);
    hal_HstSendEvent(USB_EVENT,output->Size);
    hal_HstSendEvent(USB_EVENT,input->data);
    hal_HstSendEvent(USB_EVENT,input->size);

    return MCI_ERR_NO;
}

int16 *MMC_USBGetRXBuffer(void)
{
	USB_OUTPUT *output = &USBPlayer->USBOutput;
    if(!g_continue_play_flag)
        return NULL;
    if(output->length >= MMC_USB_BUFF_SIZE*6)
    {
        hal_HstSendEvent(SYS_EVENT,0x84400002);
        output->length -= USBPlayer->packet_size/2;
    }
    return output->Buffer + output->length;
}

void MMC_USBClearRXBuffer(void)
{
    USB_OUTPUT *output = &USBPlayer->USBOutput;
    if(MMC_USBGetRXBuffer())
    {
        memset(output->Buffer, 0, MMC_USB_BUFF_SIZE*6);
        output->length = 0;
    }
}
UINT8 uaudCodecOpen = 0;
void MMC_USBReceiveData(int32 length)
{
    USB_PLAY *pUSBPlayer = USBPlayer;
	USB_OUTPUT *output = &pUSBPlayer->USBOutput;
	USB_INPUT *input = &pUSBPlayer->USBInput;
	UINT32 status;

    if(!g_continue_play_flag)
        return;

    status = hal_EnterCriticalSection();
    
    output->length += length/2;
    pUSBPlayer->packet_size = length;
    
    hal_ExitCriticalSection(status);
    
    if(pUSBPlayer->in_flag == 1)
    {
      //  hal_HstSendEvent(SYS_EVENT,0x10280060);
      //  hal_HstSendEvent(SYS_EVENT,input->length);

        if(input->length+length/2 > MMC_USB_BUFF_SIZE)
        {
         //   hal_HstSendEvent(SYS_EVENT,0x84400004);
        }
        else
        {
            //rdabt_uart_tx_sco_data(input->data+input->length, input->sco_handle, length);
            input->length += length/2;
        }
    }
    else if(output->length >= MMC_USB_BUFF_SIZE*4)
    {
        hal_HstSendEvent(USB_EVENT,0x10270030);
        hal_HstSendEvent(USB_EVENT,output->length);
        pUSBPlayer->in_flag = 1;
    	MMC_USBPcmOutStart(pUSBPlayer);
    	uaudCodecOpen = 1;
    }
}

MCI_ERR_T  MMC_AudioUSBMicData(USB_PLAY *pUSBPlayer)
{
	USB_INPUT *input = &pUSBPlayer->USBInput;
    UINT32 sco_length;
    INT16 *PCM_q=(INT16 *)input->data;
    sco_length = pUSBPlayer->packet_size/2;

    while(input->length < MMC_USB_BUFF_SIZE/2)
    {
        //hal_HstSendEvent(SYS_EVENT,0x84400003);
        //rdabt_uart_tx_sco_data(input->data+input->length, input->sco_handle, sco_length);
        input->length += sco_length/2;
    }
    if(Mmc_USB_MIC_Half == 0) // half
    {
        memcpy(PCM_q+input->length-MMC_USB_BUFF_SIZE/2, PCM_q+input->length, MMC_USB_BUFF_SIZE*3-input->length*2);
    }
    else // end
    {
        memcpy(PCM_q+input->length-MMC_USB_BUFF_SIZE/2, PCM_q+input->length, (MMC_USB_BUFF_SIZE-input->length)*2);
        memcpy(PCM_q+MMC_USB_BUFF_SIZE/2, PCM_q+MMC_USB_BUFF_SIZE*3/2, MMC_USB_BUFF_SIZE);
    }
    input->length -= MMC_USB_BUFF_SIZE/2;
 //   hal_HstSendEvent(SYS_EVENT,0x10270040);
 //   hal_HstSendEvent(SYS_EVENT,input->length);
	return MCI_ERR_NO;    
}
/*
* NAME:	MMC_AudioUSBDecPcm()
* DESCRIPTION: Initialize USB decoder.
*/
#define PLAY_OR_NOT_TICK 160
extern volatile UINT32 last_music_tick;
volatile UINT32 now_tick;
//extern BOOL  g_usb_connected;
MCI_ERR_T MMC_AudioUSBDecPcm(USB_PLAY *pUSBPlayer)
{
	USB_OUTPUT *output = &pUSBPlayer->USBOutput;
    UINT32 status;
    
    status = hal_EnterCriticalSection();
    if(output->length < MMC_USB_BUFF_SIZE*3)
    {
        now_tick = hal_TimGetUpTime();
        if(now_tick - last_music_tick > PLAY_OR_NOT_TICK)
        {
            if(Mmc_USB_Pcm_Half==0) // half
    	        memset(output->Buffer, 0, MMC_USB_BUFF_SIZE*2);
    	    else
    	        memset(output->Buffer+MMC_USB_BUFF_SIZE, 0, MMC_USB_BUFF_SIZE*2);
        }
    	
      //  hal_HstSendEvent(SYS_EVENT,0x84400001);
      //    hal_HstSendEvent(SYS_EVENT,now_tick - last_music_tick);
     //   hal_HstSendEvent(SYS_EVENT,output->length);
        return MCI_ERR_NO;
    }
  
	if(Mmc_USB_Pcm_Half==0) // half
	{
        memcpy(output->Buffer, output->Buffer+MMC_USB_BUFF_SIZE*2, MMC_USB_BUFF_SIZE*2);
        memcpy(output->Buffer+MMC_USB_BUFF_SIZE*2, output->Buffer+MMC_USB_BUFF_SIZE*3, output->length*2-MMC_USB_BUFF_SIZE*3);
	}
	else if(Mmc_USB_Pcm_Half==1) // end
	{
        memcpy(output->Buffer+MMC_USB_BUFF_SIZE, output->Buffer+MMC_USB_BUFF_SIZE*2, output->length*2-MMC_USB_BUFF_SIZE*2);
    }
    output->length -= MMC_USB_BUFF_SIZE;
    hal_ExitCriticalSection(status);

  //  hal_HstSendEvent(SYS_EVENT,0x10270035);
  //  hal_HstSendEvent(SYS_EVENT,output->length);

    return MCI_ERR_NO;
}

/*
* NAME:	MMC_AudioUSBDecClose()
* DESCRIPTION: Close USB decoder.
*/
MCI_ERR_T MMC_AudioUSBDecClose (USB_PLAY *pUSBPlayer)
{
    uaudCodecOpen = 0;
        g_continue_play_flag = 0;
	aud_StreamStop(audioItf);
	mmc_MemFreeAll();
	
//tianwq		hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
    COS_Free(USBPlayer);
    USBPlayer = NULL;
	
	return  MCI_ERR_NO;
	
}


int32 Audio_USBPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
        MMC_AudioUSBDecOpen(USBPlayer, fhd);
        
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_USB]Audio_USBPlay");
	return  MCI_ERR_NO;
}

int32 Audio_USBStop (void) 	
{
	MMC_AudioUSBDecClose(USBPlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_USBPause (void)	
{
        MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_USB] Audio_USBPause!!!!");
	//MMC_AudioUSBDecClose(&USBPlayer);	
	return  MCI_ERR_NO;
}


int32 Audio_USBResume ( HANDLE fhd) 
{
	MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_USB]Audio_USBResume!");
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_USB] Audio_USBResume!!!!");
	return  MCI_ERR_NO;
}


int32 Audio_USBGetID3 (char * pFileName)  
{
	return 0;	
}

int32 Audio_USBGetPlayInformation (MCI_PlayInf * MCI_PlayInfUSB)  
{
	MCI_PlayInfUSB->PlayProgress=0;
	
	MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_USB]PlayProgress:%d",MCI_PlayInfUSB->PlayProgress);
	
	return MCI_ERR_NO;
}

int32 Audio_USBUserMsg(int32 nMsg)
{
	switch (nMsg)
	{
		case MSG_MMC_USB_PCM_INT:
			if(MMC_AudioUSBDecPcm(USBPlayer)==MCI_ERR_END_OF_FILE)
			{
				MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_USB]END OF FILE!");				
				g_continue_play_flag = 2;
			}
			break;
			
			
		case MSG_MMC_USB_PCM_END:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_USB]MSG_MMC_USB_PCM_END\n");
			MMC_AudioUSBDecClose(USBPlayer);
			MCI_AudioFinished(MCI_ERR_END_OF_FILE);
			mmc_SetCurrMode(MMC_MODE_IDLE);
			
			break;

             case MSG_MMC_USB_MIC_INT:
                MMC_AudioUSBMicData(USBPlayer);
                break;

		default:
			
			MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_USB_ERR]ERROR MESSAGE!");
			
			break;
	}
    return 0;
}


