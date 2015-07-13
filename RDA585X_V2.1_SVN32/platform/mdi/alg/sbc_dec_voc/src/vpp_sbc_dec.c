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

#include "mcip_debug.h"
#include "vpp_sbc_dec.h"

#include "tgt_calib_m.h"
#include "aud_m.h"

#define vpp_SBC_DEC_IN_STRUCT           (0   + 0x4000) 
#define vpp_SBC_DEC_OUT_STRUCT        (82  + 0x4000)
#define vpp_SBC_DEC_OUT_STATUS        (29  + 0x4000)
#define vpp_SBC_DEC_IN_LENGTH        (74  + 0x4000)

#define vpp_SBC_DEC_OUT_LENGTH        (78  + 0x4000)

#define vpp_SBC_DEC_INIT             (95+0x4000)

#define VPP_SBC_Code_ExternalAddr_addr             (68 + 0x4000)
#define VPP_SBC_MIXING_AUDIO_FLAGE                 (4158+74+0X4000)

#define SBC_DEC_PCM_OUT_ADDR_begain                (4248+74+0X4000)
#define SBC_DEC_PCM_OUT_ADDR_end                   (4250+74+0X4000)

#define sbc_detect_mute_addr                       (4260+74+0x4000)

#define SBC_48K_SET_DMA_OUT_LENGTH_FLAG            (4254+74+0x4000)
#define SBC_44_1K_TO_48K_ENABLE_FLAG               (4262+74+0x4000)
#define SBC_44_1K_TO_48K_InterPhase                (4263+74+0x4000)
#define SBC_44_1K_TO_48K_REMAIN_LENGTH             (4585+74+0x4000)

#define SBC_DEC_EQ_DATA_HISTORY                    (5206+74+0x4000)
#define SBC_DEC_EQ_I                               (5450+74+0x4000)
#define SBC_DEC_EQ_ENABLE                          (5452+74+0x4000)

#define SBC_DECODE_CONST_1                         (128)
#define SBC_DEC_EQ_CONST_48                        (4288)

#define SBC_DEC_EQ_GAIN                            (4294)


#define vpp_AudioJpeg_DEC_CODE_SIZE       (180*4)
#define vpp_AudioJpeg_DEC_CODE_ENTRY 0
#define vpp_AudioJpeg_DEC_CRITICAL_SECTION_MIN 0
#define vpp_AudioJpeg_DEC_CRITICAL_SECTION_MAX 0

#define SBC_DEC_EQ_DATA_HISTORY_LEN (480)



extern AUD_ITF_T audioItf;

//common code
extern const INT32 G_VppCommonDecCode[];

//sbc
const INT32 G_VppSBCDecCode[] = 
{
#include "sbcdec.tab"
};

const INT32 G_VppSBCDecConstX[] = 
{
#include "sbcdec_constx.tab"
};

const INT32 G_VppSBC_COMMON_FOR_SBCDEC[] = 
{
#include "audiojpeg_dec_common_for_sbcdec.tab"
};

//=============================================================================
// vpp_AudioJpegDecOpen function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_SBCDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler) 
{
    HAL_VOC_CFG_T cfg;
    vpp_SBC_DEC_IN_T * pDecIn;
    vpp_SBC_DEC_OUT_T *pDecStatus;
    INT32 * *pDMA;
	INT32 * pDataInit;
	INT16 *pVocEq; 
	
	diag_printf("[vpp_AudioJpeg_DEC]Opening VPP SBC_DEC\n"); 

    hal_HstSendEvent(0x88100000);
      
    cfg.vocCode              = G_VppSBC_COMMON_FOR_SBCDEC;
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
   
	//sbc dec
	pDMA = hal_VocGetPointer(VPP_SBC_Code_ExternalAddr_addr);

	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppSBCDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	pDMA=pDMA+1;
	*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppSBCDecConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);	

	pDecIn = hal_VocGetPointer(vpp_SBC_DEC_IN_STRUCT);
	pDecIn->reset=1;
    pDecIn->mode=15;
    pDecIn->EQ_Type=0;
	 pDecStatus = (vpp_SBC_DEC_OUT_T *)hal_VocGetPointer(vpp_SBC_DEC_OUT_STRUCT);
	 pDecStatus->ErrorStatus=0;
	 
   *((INT16*)hal_VocGetPointer(VPP_SBC_MIXING_AUDIO_FLAGE))=0;
   *((INT16 *)hal_VocGetPointer(vpp_SBC_DEC_INIT)) = 0; 

   //init pcm upsample from 44_1K TO 48K
   *((INT16*)hal_VocGetPointer(SBC_44_1K_TO_48K_InterPhase))=0;
   *((INT16*)hal_VocGetPointer(SBC_44_1K_TO_48K_REMAIN_LENGTH))=0;
   *((INT16*)hal_VocGetPointer(SBC_48K_SET_DMA_OUT_LENGTH_FLAG))=0;

   //init eq
   *((INT16*)hal_VocGetPointer(SBC_DEC_EQ_I))=0;
   *((INT16*)hal_VocGetPointer(SBC_DEC_EQ_ENABLE))=0;
   pDataInit = ((INT32*)hal_VocGetPointer(SBC_DEC_EQ_DATA_HISTORY));
   pVocEq = ((INT16*)hal_VocGetPointer(SBC_DEC_EQ_GAIN));
   memset((INT8*)pDataInit,0x00,SBC_DEC_EQ_DATA_HISTORY_LEN);
   memset((INT8*)pVocEq,0x00,20);


#ifdef A2DP_SAMPLE_44_1K_TO_48K
	vpp_SBCDecSampleCoverEnable();
#else
    vpp_SBCDecSampleCoverDisable();

#endif
    // move to STALL location (VoC irq generated)

    return  hal_VocWakeup(HAL_VOC_START);
            
}


//=============================================================================
// vpp_AudioJpegDecClose function
//-----------------------------------------------------------------------------
void vpp_SBCDecClose(void)
{
    hal_VocClose();   
    diag_printf("[vpp_AudioJpeg_DEC]Closing VPP\n");
    
}
//=============================================================================
// vpp_SBCDecStatus function
//-----------------------------------------------------------------------------
void vpp_SBCDecStatus(vpp_SBC_DEC_OUT_T * pDecStatus)
{  
    // copy status structure from VoC RAM to the destination.
    *pDecStatus = *((vpp_SBC_DEC_OUT_T *)hal_VocGetPointer(vpp_SBC_DEC_OUT_STRUCT));
    pDecStatus->ErrorStatus = *((INT16 *)hal_VocGetPointer(vpp_SBC_DEC_OUT_STATUS));
    
    pDecStatus->sample_length = *((INT32 *)hal_VocGetPointer(vpp_SBC_DEC_OUT_LENGTH));
    
     
}

//=============================================================================
// vpp_SBCDecSampleCoverStart function
//-----------------------------------------------------------------------------
void vpp_SBCDecSampleCoverEnable(void)
{  
	*((INT16*)hal_VocGetPointer(SBC_44_1K_TO_48K_ENABLE_FLAG))=1;
}
//=============================================================================
// vpp_SBCDecSampleCoverStart function
//-----------------------------------------------------------------------------
void vpp_SBCDecSampleCoverDisable(void)
{  
	*((INT16*)hal_VocGetPointer(SBC_44_1K_TO_48K_ENABLE_FLAG))=0;
}

//=============================================================================
// vpp_SBCDecDetectMute function
//-----------------------------------------------------------------------------
/// return 1:mute;
//=============================================================================

UINT16 vpp_SBCDecDetectMute(void)
{  
	return *((INT16*)hal_VocGetPointer(sbc_detect_mute_addr));
}

//=============================================================================
// vpp_SBCDecCheckOutPcmAddr function
//-----------------------------------------------------------------------------

void vpp_SBCDecCheckOutPcmAddr(UINT32 *pAddrBegain, UINT32 *pAddrEnd)
{  
    UINT32 *pSbcDecOutPcmAddrBegain = (UINT32 *)hal_VocGetPointer(SBC_DEC_PCM_OUT_ADDR_begain);
    UINT32 *pSbcDecOutPcmAddrEnd    = (UINT32 *)hal_VocGetPointer(SBC_DEC_PCM_OUT_ADDR_end);

	*pSbcDecOutPcmAddrBegain = hal_VocGetDmaiPointer(pAddrBegain,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	*pSbcDecOutPcmAddrEnd    = hal_VocGetDmaiPointer(pAddrEnd,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
}

//=============================================================================
// vpp_AudioJpegDecScheduleOneFrame function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_SBCDecScheduleOneFrame(vpp_SBC_DEC_IN_T *pDecIn)
{ 
	//hal_HstSendEvent(SYS_EVENT,0x88200000); 
	
    vpp_SBC_DEC_IN_T * pDecInVoC = (vpp_SBC_DEC_IN_T *)hal_VocGetPointer(vpp_SBC_DEC_IN_STRUCT);
	INT16 init,audiomode,vediomode;
    extern UINT16 *g_last_buffer_addr;
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    CONST CALIB_SBC_EQ_T* pUserEq = &(tgt_GetCalibConfig()->aud.sbcEQ);
	INT16 *pVocEq; 
	
 	*pDecInVoC=*pDecIn;
    g_last_buffer_addr = pDecInVoC->outStreamBufAddr;
    pDecInVoC->inStreamBufAddr   = hal_VocGetDmaiPointer(pDecIn->inStreamBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecInVoC->outStreamBufAddr = hal_VocGetDmaiPointer(pDecIn->outStreamBufAddr,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->reset = 0;
	pDecInVoC->EQ_Type = pDecIn->EQ_Type;

/*    if(audioItf == SND_ITF_LOUD_SPEAKER || audioItf == SND_ITF_BT_SP)
        *((INT16 *)hal_VocGetPointer(VPP_SBC_MIXING_AUDIO_FLAGE))=1;
    else
        *((INT16 *)hal_VocGetPointer(VPP_SBC_MIXING_AUDIO_FLAGE))=0;*/
    *((INT16 *)hal_VocGetPointer(VPP_SBC_MIXING_AUDIO_FLAGE)) = audioCfg[audioItf].mixAudio;
	
    *((INT16 *)hal_VocGetPointer(vpp_SBC_DEC_IN_LENGTH)) = 1024;
    init = *((INT16 *)hal_VocGetPointer(vpp_SBC_DEC_INIT));
    audiomode = *((INT16 *)hal_VocGetPointer(vpp_SBC_DEC_IN_STRUCT+36));
    pVocEq = ((INT16*)hal_VocGetPointer(SBC_DEC_EQ_GAIN));

    //set eq
	if(pUserEq->modeConfigMap[audioItf&1] == 0 || pUserEq->modeConfigMap[audioItf&1] == 1)
	{
	   int i;
	   for(i=0; i<10; i++)
	   {
	      *(pVocEq + i) = (INT16)pUserEq->sbcDB[pUserEq->modeConfigMap[audioItf&1]][i];
	   }
	}
	else
	{
	   memset((INT8*)pVocEq,0x00,20);
	}


    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
 
}

