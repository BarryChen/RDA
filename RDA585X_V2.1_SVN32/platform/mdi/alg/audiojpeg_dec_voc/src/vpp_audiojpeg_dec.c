/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2003-2006, Coolsand Technologies, Inc.     */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      vpp.c                                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*     This file contains the VPP API  functions.                        */
/*                                                                       */
/*************************************************************************/
//#include "stdio.h"

#include "cs_types.h"

#include "hal_error.h"
#include "hal_voc.h"

#include "vpp_audiojpeg_dec_asm.h"
#include "mcip_debug.h"
#include "vpp_audiojpeg_dec.h"

#include "tgt_calib_m.h"

#include "aud_m.h"
#include "audp_debug.h"

CONST INT16 f_sbc_persition[30]= 
{
	2, 2, 2, 4, 4, 
	4, 4, 4, 4, 8, 8, 8, 8, 
	8, 8, 16, 16, 16, 16, 16, 32, 
	32, 32, 32, 32, 64, 64, 64, 63,
	0
};

BOOL vpp_AudioJpegDecVocOpen_status=FALSE;

   extern AUD_LEVEL_T audio_cfg;
//   extern HAL_AIF_STREAM_T audioStream;
   extern AUD_ITF_T audioItf;

#ifdef SMALL_BSS_RAM_SIZE
#ifndef mmc_MemMalloc_BssRam
 #define  mmc_MemMalloc_BssRam(size) COS_Malloc_NoCache((UINT32)size)
 #define mmc_MemFreeAll_BssRam(p)      COS_Free((PVOID)p)
 #endif
#endif

//=============================================================================
// vpp_AudioJpegDecOpen function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_AudioJpegDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler) 
{
    HAL_VOC_CFG_T cfg;
    vpp_AudioJpeg_DEC_IN_T * pDecIn;
    vpp_AudioJpeg_DEC_OUT_T *pDecStatus;
    INT32 * *pDMA;
    INT16 *pFilter;

    CALIB_BUFFER_T* calibPtr = tgt_GetCalibConfig();
	
   diag_printf("[vpp_AudioJpeg_DEC]Opening VPP AUDIO_DEC\n"); 

    if(calibPtr->aud.vocItfMap[audioItf]>=0)
        pFilter = &(calibPtr->aud.vocFilters[calibPtr->aud.vocItfMap[audioItf]].sdfFilter[0]);
    else
    {
        pFilter = mmc_MemMalloc(sizeof(CALIB_AUDIO_VOC_FILTERS_T));
        memset(pFilter, 0, sizeof(CALIB_AUDIO_VOC_FILTERS_T));
    }  
    cfg.vocCode              = G_VppCommonDecCode;
    cfg.vocCodeSize        = vpp_AudioJpeg_DEC_CODE_SIZE;
    cfg.pcVal                   = vpp_AudioJpeg_DEC_CODE_ENTRY;
    cfg.pcValCriticalSecMin  = vpp_AudioJpeg_DEC_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = vpp_AudioJpeg_DEC_CRITICAL_SECTION_MAX;
    cfg.needOpenDoneIrq      = FALSE;
    cfg.irqMask.voc          = (vocIrqHandler) ? 1 : 0;
    cfg.irqMask.dmaVoc       = 0;
    cfg.vocIrqHandler        = vocIrqHandler;
    
    cfg.eventMask.wakeupIfc0 = 0;
    cfg.eventMask.wakeupIfc1 = 0;
    cfg.eventMask.wakeupDmae = 0;
    cfg.eventMask.wakeupDmai = 0;
    cfg.eventMask.wakeupSof0 = 0;
    cfg.eventMask.wakeupSof1 = 0;
    
    // load the VPP AUDIO_DEC  code and configure the VoC resource        
    switch (hal_VocOpen(&cfg))
    {
        // error opening the resource
        case HAL_ERR_RESOURCE_BUSY:                 
            diag_printf("[vpp_AudioJpeg_DEC]##WARNING##Error opening VoC resource\n"); 
            return HAL_ERR_RESOURCE_BUSY;
                                        
        // first open, load the constant tables                            
        case HAL_ERR_RESOURCE_RESET:              
            diag_printf("[vpp_AudioJpeg_DEC]First open.\n");			
            break;

        default:           
            diag_printf("[vpp_AudioJpeg_DEC]No first open.\n"); 
            break;
    }                          

   diag_printf("[vpp_AudioJpeg_DEC]Initializing the DMA addr.\n");
      //mp3
   	pDMA = hal_VocGetPointer(VPP_MP3_Code_ExternalAddr_addr);
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3DecConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3DecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
   *((INT32*)hal_VocGetPointer(VPP_AUDIO_MIXING_MP3_ADDR))=FALSE;
	//aac
	pDMA = hal_VocGetPointer(VPP_AAC_Code_ExternalAddr_addr);

	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3Layer12Dec_DMA_ConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);	
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

	//amr
	pDMA = hal_VocGetPointer(VPP_AMR_Code_ExternalAddr_addr);

	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAmrDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3Dec_EQ_DMA_ConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAmrDecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

#ifndef TARGET_AAC_SUPPORT_OFF
    pDMA= hal_VocGetPointer(VPP_AAC_Tab_huffTabSpec_START_addr);
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecHuffTabSpec,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecCos4sin4tab,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecTwidTabOdd,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecSinWindow,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
#endif

 *((INT16*)hal_VocGetPointer( VPP_GLOBAL_NON_CLIP_BUFFER_MP3_CLEAR_FLAG))=1;

#ifdef VIDEO_PLAYER_SUPPORT
	//jpeg
	pDMA = hal_VocGetPointer(VPP_JPEG_Code_ExternalAddr_addr);

	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJpegDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJpegDecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);


	
	pDMA = hal_VocGetPointer(VPP_AAC_Tab_huffTabSpec_START_addr);
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecHuffTabSpec,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecCos4sin4tab,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecTwidTabOdd,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecSinWindow,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

      //h263 zoom
	pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
     *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	 pDMA=pDMA+1;
     *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
#endif

#ifdef bt_support
       //sbc encode
	pDMA = hal_VocGetPointer(VPP_SBC_ENC_Code_ExternalAddr_addr);
     *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppSBCCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
#endif

	pDMA = hal_VocGetPointer(VPP_Speak_AntiDistortion_Filter_Coef_addr);
   //  *pDMA=hal_VocGetDmaiPointer((INT32*)(&(calibPtr->bb->audio[4].vocFilters.sdfFilter[0])),  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    *pDMA=hal_VocGetDmaiPointer((INT32*)pFilter,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     
	pDMA = hal_VocGetPointer(VPP_GLOBAL_NON_CLIP_HISTORYDATA_L_MP3_ADDR);

#ifdef SMALL_BSS_RAM_SIZE

	G_Mp3NonCliPLBuf_alloc                     = (UINT8*)mmc_MemMalloc_BssRam(G_Mp3NonCliPLBuf_Len+3);
	//G_Mp3NonCliPRBuf_alloc                     = (UINT8*)mmc_MemMalloc_BssRam(G_Mp3NonCliPRBuf_Len+3);
	G_Mp3NonCliPLBuf =(UINT32*) ((UINT32)(G_Mp3NonCliPLBuf_alloc +3) & ~0x3);
	//G_Mp3NonCliPRBuf =(UINT32*) ((UINT32)(G_Mp3NonCliPRBuf_alloc+3) & ~0x3);

	hal_HstSendEvent(SYS_EVENT,0xaccccccc);
	hal_HstSendEvent(SYS_EVENT,G_Mp3NonCliPLBuf_alloc);
	hal_HstSendEvent(SYS_EVENT,G_Mp3NonCliPLBuf);
	//hal_HstSendEvent(SYS_EVENT,G_Mp3NonCliPRBuf_alloc);
	//hal_HstSendEvent(SYS_EVENT,G_Mp3NonCliPRBuf);

#endif


     *pDMA=hal_VocGetDmaiPointer((INT32*)G_Mp3NonCliPLBuf,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     
	//pDMA = hal_VocGetPointer(VPP_GLOBAL_NON_CLIP_HISTORYDATA_R_MP3_ADDR);
    // *pDMA=hal_VocGetDmaiPointer((INT32*)G_Mp3NonCliPRBuf,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     
	pDMA = hal_VocGetPointer(VPP_GLOBAL_DIGITAL_GAIN_MP3_ADDR);

     *pDMA=(INT32 *)(((32767)<<16)|(0x10));

     for(INT32 i=0;i<30;i++)
     {
        //calibPtr->bb->audio[4].vocFilters.sdfFilter[32+i]=f_sbc_persition[i];
        pFilter[32+i]=f_sbc_persition[i];
     }

     for(INT32 i=0;i<288;i++)
     {
        G_Mp3NonCliPLBuf[i]=0;
           
     }

	pDecIn = hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
	pDecIn->reset=1;
	pDecIn->EQ_Type = -1;  	


    
	 pDecStatus = (vpp_AudioJpeg_DEC_OUT_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_OUT_STRUCT);
	 pDecStatus->ErrorStatus=0;
	 pDecStatus->mode=-1;//not 0~9;
 

    // move to STALL location (VoC irq generated)

      vpp_AudioJpegDecVocOpen_status=TRUE;

    return  hal_VocWakeup(HAL_VOC_START);
            
}


//=============================================================================
// vpp_AudioJpegDecClose function
//-----------------------------------------------------------------------------
void vpp_AudioJpegDecClose(void)
{
    if(vpp_AudioJpegDecVocOpen_status==TRUE)
	    hal_VocClose();   

#ifdef SMALL_BSS_RAM_SIZE
   /*if(G_Mp3NonCliPRBuf_alloc)
	{
		mmc_MemFreeAll_BssRam(G_Mp3NonCliPRBuf_alloc);
		G_Mp3NonCliPRBuf_alloc=0;
    	}*/
     if(G_Mp3NonCliPLBuf_alloc)
    	{
	    mmc_MemFreeAll_BssRam(G_Mp3NonCliPLBuf_alloc);
	    G_Mp3NonCliPLBuf_alloc=0;
    	}
#endif
    vpp_AudioJpegDecVocOpen_status=FALSE;    
    diag_printf("[vpp_AudioJpeg_DEC]Closing VPP\n");
    
}
//=============================================================================
// vpp_AudioJpegDecStatus function
//-----------------------------------------------------------------------------
void vpp_AudioJpegDecStatus(vpp_AudioJpeg_DEC_OUT_T * pDecStatus)
{  
    // copy status structure from VoC RAM to the destination.
    *pDecStatus = *((vpp_AudioJpeg_DEC_OUT_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_OUT_STRUCT));
}

PUBLIC UINT32 vpp_AudioJpegDecOutAlgGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 9)
        db = 9;
    return db/3;
}

//=============================================================================
// vpp_AudioJpegDecScheduleOneFrame function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_AudioJpegDecScheduleOneFrame(vpp_AudioJpeg_DEC_IN_T *pDecIn)
{
    vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
    extern UINT16 *g_last_buffer_addr;
    INT32 * *pDMA;
    //CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    CALIB_BUFFER_T* calibPtr = tgt_GetCalibConfig();
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    UINT32  algGain;

    //diag_printf("vpp_AudioJpegDecScheduleOneFrame: input->data[0~3]=[%02x,%02x,%02x,%02x]", pDecIn->inStreamBufAddr[0], pDecIn->inStreamBufAddr[1], pDecIn->inStreamBufAddr[2], pDecIn->inStreamBufAddr[3]);
    //if(audioItf==SND_ITF_BT_SP) // for loud speaker, mix audio
     *((INT32*)hal_VocGetPointer(VPP_AUDIO_MIXING_MP3_ADDR))=audioCfg[audioItf].mixAudio;
 
     pDecIn->audioItf = audioItf;
	
 	  *pDecInVoC=*pDecIn;
  	g_last_buffer_addr = pDecIn->outStreamBufAddr;
    pDecInVoC->inStreamBufAddr   = hal_VocGetDmaiPointer(pDecIn->inStreamBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecInVoC->inStreamUBufAddr   = hal_VocGetDmaiPointer(pDecIn->inStreamUBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecInVoC->inStreamVBufAddr   = hal_VocGetDmaiPointer(pDecIn->inStreamVBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecInVoC->outStreamBufAddr = hal_VocGetDmaiPointer(pDecIn->outStreamBufAddr,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->reset=0;

	//hal_HstSendEvent(SYS_EVENT,0x88200000);    
	//hal_HstSendEvent(SYS_EVENT,pDecIn->inStreamBufAddr);
	//hal_HstSendEvent(SYS_EVENT,pDecIn->outStreamBufAddr);
	
    // diag_printf("[vpp_AudioJpeg_DEC]Codecmode:%d\n",pDecInVoC->mode); 	

    pDMA = hal_VocGetPointer(VPP_GLOBAL_DIGITAL_GAIN_MP3_ADDR);

    CALIB_AUDIO_OUT_GAINS_T *pOutGains;

    AUD_ASSERT(calibPtr->aud.gainItfMap[audioItf]>=0, "AUD: No Gain Calibration for itf: %d", audioItf);
    
    pOutGains = &calibPtr->aud.audioGains[calibPtr->aud.gainItfMap[audioItf]].outGains[audio_cfg.spkLevel];
    switch (vpp_AudioJpegDecOutAlgGainDb2Val(pOutGains->musicOrLsAlg))
    {
    case 0:
    {
       *pDMA=(INT32 *)(((32767)<<16)|(0x10));
    }
    break;

    case 1:
    {
        *pDMA=(INT32 *)(((32767*0x10/0x16)<<16)|(0x16));
    }
    break;

    case 2:
    {
        *pDMA=(INT32 *)(((32767*0x10/0x20)<<16)|0x20);
    }
    break;

    case 3:
    {
        *pDMA=(INT32 *)(((32767*0x10/0x2d)<<16)|0x2d);
    }
    break;

    default:
    {
        *pDMA=(INT32 *)(((32767)<<16)|(0x10));
    }
    break;
    }
/*
    if(audio_cfg.spkLevel%2==0)
    {
        *pDMA=(INT32 *)(((32767*0x10/0x20)<<16)|0x20);

    }
    else
    {
        *pDMA=(INT32 *)(((32767)<<16)|(0x10));
    }

*/  
    // schedule next frame on sof1 event
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
}

#ifdef VIDEO_PLAYER_SUPPORT
HAL_ERR_T  vpp_AudioJpegDecSetCurMode(INT16 pVoc_Video_Mode, INT16 pVoc_Audio_Mode)
{
	INT16 * pointVoC = hal_VocGetPointer(VPP_Current_Audio_Mode_addr);
	vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
	
	*pointVoC=pVoc_Audio_Mode;
	pointVoC=pointVoC+1;
	*pointVoC=pVoc_Video_Mode;

	pDecInVoC->reset=0;//not reload code
	pDecInVoC->BsbcEnable=0;//bluetooth
	
	return HAL_ERR_NO;
}


HAL_ERR_T  vpp_AudioJpegDecSetInputMode(INT16 pVoc_Input_Mode)
{
	vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
	
   	pDecInVoC->mode=pVoc_Input_Mode;
	pDecInVoC->BsbcEnable=0;
    
    	return HAL_ERR_NO;
}
/*
set voc audio/video decode mode
DecMode:	Codecmode enum
AVMode:		0= video, 1= audio.
Reset:		0=keep current mode,1=reset current mode in voc.
NOTE:		set Reset=1 for the first time.
sheen
2011.09.11
*/
HAL_ERR_T  vpp_AVDecSetMode(INT16 DecMode, INT16 AVMode, INT16 Reset)
{
	INT16 * pCurInVoC = hal_VocGetPointer(VPP_Current_Audio_Mode_addr);
	vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);

	pDecInVoC->mode=DecMode;
	pDecInVoC->reset=Reset;// 1 = set both current audio & video mode in valide and reload code
	pDecInVoC->BsbcEnable=0;//bluetooth
	
	if(AVMode==0)
	{//set next audio mode invalide in voc
		*pCurInVoC= -1;
	}
	else
	{//set next video mode invalide in voc
		pCurInVoC=pCurInVoC+1;
		*pCurInVoC= -1;
	}

	return HAL_ERR_NO;
}

INT16  vpp_AudioJpegDecGetMode(void)
{
	vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
	return pDecInVoC->mode;
}

//=============================================================================
// vpp_AudioJpegSetReuseCode function
//reuse with VPP_H263_Zoom_Code_ExternalAddr_addr
//code_id: 0=zoom, 1=rmvb, 2=264, 3=mpeg4 ...
//set after vpp_AudioJpegDecOpen			
//-----------------------------------------------------------------------------
void vpp_AudioJpegSetReuseCode(INT16 code_id)
{
	INT32 * *pDMA;

	switch(code_id)
	{
		case 0:
			//h263 zoom
			pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			pDMA=pDMA+1;
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			break;
		case 1:
			//rmvb
			pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppRMDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			pDMA=pDMA+1;
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppRMDecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			break;
		case 2:
			//h264
			pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH264DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			pDMA=pDMA+1;
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH264DecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			break;
		case 3:
			//mpeg4
			pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMpeg4DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			pDMA=pDMA+1;
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMpeg4DecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			break;
		case 4:
			//h263
			pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			pDMA=pDMA+1;
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263DecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			break;
		case 5:
			//jpeg
			pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
			*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJpegDec2Code,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			//pDMA=pDMA+1;
			//*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJPEGDec2Consty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
			break;

		default:
			break;
	}
}
#endif

