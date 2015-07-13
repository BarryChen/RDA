/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2003-2006, Coolsand Technologies, Inc.     */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      vpp_speech.c                                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*     This file contains the VPP API  functions.                        */
/*                                                                       */
/*************************************************************************/
#define VPP_PRINTF

#include "cs_types.h"

#include "hal_error.h"
#include "hal_voc.h"
#include "hal_speech_bt.h"
#include "hal_dma.h"

#include "vppp_speech_bt.h"
#include "vppp_speech_asm_bt.h"
#include "vppp_speech_map_bt.h"

#include "vpp_speech_bt.h"
 
#define ECHO_COHF                               			(0) //alias
#define ECHO_TF                                 			(1) //alias

#define PassThrough                             			(0) 
#define AdaptiveFilter                          			(1) 
#define AdaptiveFilterPostFilter                			(2) 
#define NoiseSuppression                        			(3) 
#define LowDelayOpenLoopAF                      			(4) 
#define TwoStepNoiseSuppression                 			(5) 
#define AdaptiveFilterPostFilterEnhanced        			(6) 


/// The audio gain value standing for mute.
#define CALIB_AUDIO_GAIN_VALUE_MUTE              (-128)

// Global variable storing the voc irq handler
PRIVATE VPP_SPEECH_WAKEUP_MODE_T g_vppSpeechWakeupMode   = VPP_SPEECH_WAKEUP_NO; 

//<1472
#define GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE (1350)

INT32* g_BTRxBuf= NULL;
INT32* g_BTTxBuf= NULL;
INT32* g_Bluetooth_RxBuf= NULL;
INT32* g_Bluetooth_TxBuf= NULL;

INT32* g_vppMicNoiseSuppressRAMX= NULL;
INT32* g_vppReceiverNoiseSuppressRAMX= NULL;

//The audio micgain value
INT16 gMicGain = 8;
INT16 gEchoGainLimit = 255;

PUBLIC HAL_ERR_T vpp_SpeechInitBuffer(VOID) 
{
    extern uint32 *pBtSpeechVocOverlay;
	extern uint32 *pBtSpeechNSPcmOverlay;
    g_BTRxBuf = pBtSpeechVocOverlay;
    g_BTTxBuf = pBtSpeechVocOverlay+160;
    g_Bluetooth_RxBuf = pBtSpeechVocOverlay+320;
    g_Bluetooth_TxBuf = pBtSpeechVocOverlay+480;
	
	g_vppMicNoiseSuppressRAMX = pBtSpeechNSPcmOverlay;
	g_vppReceiverNoiseSuppressRAMX = pBtSpeechNSPcmOverlay + GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE;
#ifdef USE_VOC_CVSD
	memset((UINT8 *)g_Bluetooth_RxBuf,0x55,640*2);//init for CVSD
#endif
    memset((UINT8 *)g_vppMicNoiseSuppressRAMX,0x0,GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE*4*2);//init for CVSD
}

#if 0
#define SUBBAND_CHANNEL_NUM  16
#define TOTAL_FILTER_LEN  128
#define FILTER_LEN  (TOTAL_FILTER_LEN/SUBBAND_CHANNEL_NUM)
#define SUBBANDFILTER_NUM  (SUBBAND_CHANNEL_NUM/2+1)
#define AF_BAK_SIZE (SUBBANDFILTER_NUM*FILTER_LEN*2)
UINT32 gAdaptiveFilterCoefBak[AF_BAK_SIZE]=
{
    0
};
#endif
//=============================================================================
// vpp_SpeechBTOpen function
//-----------------------------------------------------------------------------
PUBLIC HAL_ERR_T vpp_SpeechBTOpen(    HAL_VOC_IRQ_HANDLER_T       vocIrqHandler, 
                                    VPP_SPEECH_WAKEUP_MODE_T    wakeupMode)
{
    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;
    VPP_SPEECH_AUDIO_CFG_T audioCfg;
    VPP_SPEECH_INIT_T *pInit = hal_VocGetPointer(VPP_SPEECH_INIT_STRUCT); 
	

    
    cfg.vocCode              = g_vppBTSpeechMainCode;
    cfg.vocCodeSize          = g_vppBTSpeechMainCodeSize;
    cfg.pcVal                = VPP_SPEECH_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_SPEECH_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_SPEECH_CRITICAL_SECTION_MAX;
    cfg.needOpenDoneIrq      = FALSE;
    cfg.irqMask.voc         = (vocIrqHandler) ? 1 : 0;
    cfg.irqMask.dmaVoc      = 0;
    cfg.vocIrqHandler       = vocIrqHandler;
    
    cfg.eventMask.wakeupIfc0 = 0;
    cfg.eventMask.wakeupIfc1 = 0;
    cfg.eventMask.wakeupDmae = 0;
    cfg.eventMask.wakeupDmai = 1;
    cfg.eventMask.wakeupSof0 = 0;
    cfg.eventMask.wakeupSof1 = 0;

    switch (wakeupMode)
    {
            case VPP_SPEECH_WAKEUP_SW_DECENC : 
            case VPP_SPEECH_WAKEUP_SW_ENC    : cfg.eventMask.wakeupSof0 = 1; break;
            case VPP_SPEECH_WAKEUP_SW_DEC    : cfg.eventMask.wakeupSof1 = 1; break;
            case VPP_SPEECH_WAKEUP_HW_DECENC :  
            case VPP_SPEECH_WAKEUP_HW_ENC    : cfg.eventMask.wakeupIfc0 = 1; break;
            case VPP_SPEECH_WAKEUP_HW_DEC    : cfg.eventMask.wakeupIfc1 = 1; break;
            default                          : break;
    }
              
    g_vppSpeechWakeupMode = wakeupMode;
    
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status) 
    {
        return status;
    }
    else if (HAL_ERR_RESOURCE_RESET == status) 
    {
        UINT16 i;

        // Init code state structure
        for (i = 0; i < 4; i++)
        {
            pInit->codeStateTab[i] = -1;
        }

        //pInit->codePtrTab[VPP_SPEECH_EFR_AMR_IDX]          = hal_VocGetDmaiPointer(g_vppEfrAmrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_FR_IDX]               = hal_VocGetDmaiPointer(g_vppFrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_HR_IDX]               = hal_VocGetDmaiPointer(g_vppHrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_IDX]              = hal_VocGetDmaiPointer(g_vppAmrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
       // pInit->codePtrTab[VPP_SPEECH_EFR_IDX]              = hal_VocGetDmaiPointer(g_vppEfrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_475_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode475,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_515_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode515,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_59_IDX]       = hal_VocGetDmaiPointer(g_vppAmrEncCode59,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_67_IDX]       = hal_VocGetDmaiPointer(g_vppAmrEncCode67,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_74_IDX]       = hal_VocGetDmaiPointer(g_vppAmrEncCode74,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_795_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode795,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_102_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode102,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_ENC_122_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode122,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_475_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode475,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_515_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode515,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_59_IDX]       = hal_VocGetDmaiPointer(g_vppAmrDecCode59,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_67_IDX]       = hal_VocGetDmaiPointer(g_vppAmrDecCode67,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_74_IDX]       = hal_VocGetDmaiPointer(g_vppAmrDecCode74,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_795_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode795,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_102_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode102,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->codePtrTab[VPP_SPEECH_AMR_DEC_122_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode122,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        //pInit->constPtrTab[VPP_SPEECH_EFR_AMR_CONST_X_IDX] = hal_VocGetDmaiPointer(g_vppEfrAmrConstX,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->constPtrTab[VPP_SPEECH_EFR_AMR_CONST_Y_IDX] = hal_VocGetDmaiPointer(g_vppEfrAmrConstY,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->constPtrTab[VPP_SPEECH_HR_CONST_X_IDX]      = hal_VocGetDmaiPointer(g_vppHrConstX,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        //pInit->constPtrTab[VPP_SPEECH_HR_CONST_Y_IDX]      = hal_VocGetDmaiPointer(g_vppHrConstY,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
		
	

    }

   //  *((INT32 *)hal_VocGetPointer(GLOBAL_SPEECH_RAMX_BACKUP_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppBTSpeechRAMX,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
   //  *((INT32 *)hal_VocGetPointer(GLOBAL_SPEECH_CODE_BACKUP_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppBTSpeechCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
  //   *((INT32 *)hal_VocGetPointer(GLOBAL_AEC_RAMX_BACKUP_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppBTAecRAMX,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     *((INT32 *)hal_VocGetPointer(GLOBAL_AEC_CODE_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppBTAecCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     *((INT32 *)hal_VocGetPointer(GLOBAL_AEC_CONSTX_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppBTAecConstantX,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

 //    *((INT32 *)hal_VocGetPointer(GLOBAL_SPEECH_RAMY_BACKUP_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppBTSpeechRAMY,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     //*((INT32 *)hal_VocGetPointer(GLOBAL_MORPH_CODE_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppMorphCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     //*((INT32 *)hal_VocGetPointer(GLOBAL_MORPH_CONSTY_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppMorphConstantY,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);



     *((INT32 *)hal_VocGetPointer(GLOBAL_RX_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_BTRxBuf,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     *((INT32 *)hal_VocGetPointer(GLOBAL_TX_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_BTTxBuf,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

	 *((INT32 *)hal_VocGetPointer(GLOBAL_BLUETOOTH_RX_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_Bluetooth_RxBuf,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
     *((INT32 *)hal_VocGetPointer(GLOBAL_BLUETOOTH_TX_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_Bluetooth_TxBuf,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

 	 *((INT32 *)hal_VocGetPointer(GLOBAL_MIC_NOISESUPPRESS_RAMX_BACKUP_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppMicNoiseSuppressRAMX,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
 	 *((INT32 *)hal_VocGetPointer(GLOBAL_RECEIVER_NOISESUPPRESS_RAMX_BACKUP_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppReceiverNoiseSuppressRAMX,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
   
     *((INT32 *)hal_VocGetPointer(GLOBAL_NOISESUPPRESS_CONSTX_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppNoiseSuppressConstantX ,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
	 *((INT32 *)hal_VocGetPointer(GLOBAL_NOISESUPPRESS_CODE_ADDR)) = (INT32 )hal_VocGetDmaiPointer(g_vppNoiseSuppressCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    #ifdef USE_VOC_CVSD
/**********************************************CVSD******************************************************/
{
	int i;
	for(i=0;i<74;i++)
	{
	    *((INT32 *)hal_VocGetPointer(CVSD_LOWPASS_FILTER_STRUCT)) = 0;
	}
	
	*((INT32 *)hal_VocGetPointer(CVSD_LOWPASS_FILTER_ROUND)) = 0x00000020;//32 ,0.5
	*((INT32 *)hal_VocGetPointer(CVSD_DEC_OUTPUT_ROUND)) = 0x00000266;//;512:0.5; 266:0.6

	*((INT32 *)hal_VocGetPointer(CVSD_enc_delta)) = 0x00000000;
	*((INT32 *)hal_VocGetPointer(CVSD_dec_delta)) = 0x00000000;
	*((INT32 *)hal_VocGetPointer(CVSD_delta_min)) = 0x00002800;
	*((INT32 *)hal_VocGetPointer(CVSD_delta_max)) = 0x00140000;//extend 10 bits;
	*((INT32 *)hal_VocGetPointer(CVSD_y_min)) = 0xFE000000;
	*((INT32 *)hal_VocGetPointer(CVSD_y_max)) = 0x01fffc00;
	*((INT32 *)hal_VocGetPointer(CVSD_enc_xest_l)) = 0x00000000;
	*((INT32 *)hal_VocGetPointer(CVSD_dec_xest_l)) = 0x00000000;
	*((INT32 *)hal_VocGetPointer(CVSD_ENC_JUDGE_alpha_buf)) = 0x000a000a;//dec and enc
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag)) = 0x00000000;//dec and enc


	
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+2))=0x00edff9f;//CVSD_LOWPASS_CONST_FILTER3_Coef_TAB[5]
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+4))=0x030b04c8; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+6))=0x0037ffc0;//CVSD_LOWPASS_CONST_FILTER2_Coef_TAB[7] 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+8))=0x007fff60; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+10))=0x032d0574; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+12))=0xfff8ff31; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+14))=0x006f0012;//CVSD_LOWPASS_CONST_FILTER1_Coef_TAB[21] 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+16))=0x003affc1; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+18))=0x0031ffc7; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+20))=0xffffffdf; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+22))=0xff0e0044; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+24))=0x02c806c1; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+26))=0x00e2fea9; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+28))=0x0072ff5f; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+30))=0x0036ffaf; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+32))=0x001effda; 
	*((INT32 *)hal_VocGetPointer(CVSD_enc_first_frame_flag+34))=0x000f0066;	


}

#endif

/**********************************************AEC******************************************************/
{
	
	VPP_SPEECH_AEC_INIT_T *Aec_Init =hal_VocGetPointer(m_AECProcPara_ADDR);

    	Aec_Init->AecEnbleFlag=FALSE;
	Aec_Init->AgcEnbleFlag=FALSE;

	Aec_Init->ProcessMode=AdaptiveFilterPostFilterEnhanced;
	Aec_Init->InitRegularFactor=8;
	Aec_Init->AFUpdateFactor=8;

	Aec_Init->AFCtrlSpeedUpFactor=32;
	Aec_Init->AFFilterAmp=32;


	Aec_Init->EchoEstMethod=ECHO_COHF;

	Aec_Init->NoiseUpdateFactor=64;
	Aec_Init->SNRUpdateFactor=4;
	Aec_Init->SNRLimit=13;
	Aec_Init->NoiseOverEstFactor=1;

	Aec_Init->NoiseGainLimit=40;
	Aec_Init->EchoUpdateFactor=32;
	Aec_Init->SERUpdateFactor=16;
	Aec_Init->SERLimit=3;

	Aec_Init->EchoOverEstFactor=2;
	Aec_Init->EchoGainLimit=gEchoGainLimit;
	Aec_Init->CNGFactor=2;
	Aec_Init->NoiseMin=0;

	Aec_Init->PFPrevEchoEstLimit=1023;

	Aec_Init->PFDTEchoOverEstFactor=20;

	Aec_Init->pPFEchoGainLimitAmp[0]=20;
	Aec_Init->pPFEchoGainLimitAmp[1]=100;
	Aec_Init->pPFEchoGainLimitAmp[2]=10;
	Aec_Init->pPFEchoGainLimitAmp[3]=8;
	Aec_Init->pPFEchoGainLimitAmp[4]=6;
	Aec_Init->pPFEchoGainLimitAmp[5]=4;
	Aec_Init->pPFEchoGainLimitAmp[6]=2;
	Aec_Init->pPFEchoGainLimitAmp[7]=1;
	Aec_Init->pPFEchoGainLimitAmp[8]=1;

	

	Aec_Init->AmpGain=16;

	Aec_Init->NoiseGainLimitStep=0;
	Aec_Init->AmpThr=0x7f;

	Aec_Init->PFCLDataSmoothFactor=128;
	Aec_Init->PFCLNoiseUpdateFactor=64;
	Aec_Init->PFCLThrSigDet=160;
	Aec_Init->PFCLThrDT=4;

	Aec_Init->DataSmoothFactor=256;
	Aec_Init->PFCLChanlGain=128;

	Aec_Init->PFCLThrNT=64;
	Aec_Init->PFCLThrDTDiff=6;
	Aec_Init->PFCLDTHoldTime=160;
	Aec_Init->PFCLDTStartTime=16;



	Aec_Init->PFCLDTDUpdateFactor=2;
	Aec_Init->PFCLDTDThreshold=115;
	Aec_Init->PFCLDTD2ndThreshold=64;		
	Aec_Init->StrongEchoFlag=0;
	Aec_Init->PFCLDTDThrRatio=71;

	


	INT16 *NotchFIlterCoef =hal_VocGetPointer(GLOBAL_NOTCH_FILTER_COEF_ADDR);

	NotchFIlterCoef[0]=31919;
	NotchFIlterCoef[1]=30529;
	NotchFIlterCoef[2]=28258;
	NotchFIlterCoef[3]=25169;
	
	NotchFIlterCoef[4]=21354;
	NotchFIlterCoef[5]=16922;
	NotchFIlterCoef[6]=12002;
	NotchFIlterCoef[7]=6734;

	NotchFIlterCoef[8]=1272;
	NotchFIlterCoef[9]=-4227;
	NotchFIlterCoef[10]=-9603;
	NotchFIlterCoef[11]=-14703;

	NotchFIlterCoef[12]=-19377;
	NotchFIlterCoef[13]=-23492;
	NotchFIlterCoef[14]=-26928;
	NotchFIlterCoef[15]=0;


}
/**********************************************AEC******************************************************/

/**********************************************Morph Voice******************************************************/


     *((INT16 *)hal_VocGetPointer(MORPHVOICE_ENABLE_FLAG_ADDR)) =0;

     *((INT16 *)hal_VocGetPointer(PITCH_SHIFT_ADDR)) =0x4000;


	*((INT16 *)hal_VocGetPointer(HIGH_PASS_FILTER_ENABLE_FLAG_ADDR)) =TRUE;
	*((INT16 *)hal_VocGetPointer(NOTCH_FILTER_ENABLE_FLAG_ADDR)) =FALSE;//
#if  !defined(SPEECH_NS_MIC_VOC_CLOSE)
	
	*((INT16 *)hal_VocGetPointer(MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =TRUE;
#else

	*((INT16 *)hal_VocGetPointer(MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =FALSE;
#endif

#if  !defined(SPEECH_NS_RECEIVER_VOC_CLOSE)
	*((INT16 *)hal_VocGetPointer(RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =TRUE;
#else

	*((INT16 *)hal_VocGetPointer(RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =FALSE;
#endif
/**********************************************Morph Voice******************************************************/



/*************************record speech*********************/

     *((INT16 *)hal_VocGetPointer(GLOBAL_SPEECH_RECORD_FLAG)) =0;


/*************************record speech*********************/


/*************************noise*********************/

     *((INT16 *)hal_VocGetPointer(GLOBAL_EXTERN_PROCESS_MIC_HANDLE_FLAG)) =FALSE;

/*************************extern process mic handle flag*********************/

/*************************mic noise suppress flag*********************/

     *((INT16 *)hal_VocGetPointer(GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR)) =1;
     *((INT16 *)hal_VocGetPointer(GLOBAL_MIC_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR)) =8192;

/*************************mic noise suppress flag*********************/







    // irq generation
    switch (wakeupMode)
    {
        case VPP_SPEECH_WAKEUP_SW_ENC: 
        case VPP_SPEECH_WAKEUP_HW_ENC    : pInit->irqgen = VPP_SPEECH_IRQGEN_ENC; break;
        case VPP_SPEECH_WAKEUP_SW_DECENC :
        case VPP_SPEECH_WAKEUP_HW_DECENC :  
        case VPP_SPEECH_WAKEUP_SW_DEC    : 
        case VPP_SPEECH_WAKEUP_HW_DEC    : pInit->irqgen = VPP_SPEECH_IRQGEN_DEC; break;
        default                          : pInit->irqgen = VPP_SPEECH_IRQGEN_NONE; break;
    }
    
    // reset VPP speech
    vpp_SpeechBTReset();

    // default audio configuration
    audioCfg.echoEsOn   = 0;
    audioCfg.echoEsVad  = 0;
    audioCfg.echoEsDtd  = 0;
    audioCfg.echoExpRel = 0;
    audioCfg.echoExpMu  = 0;
    audioCfg.echoExpMin = 0;
    audioCfg.encMute    = VPP_SPEECH_UNMUTE;
    audioCfg.decMute    = VPP_SPEECH_UNMUTE;
    audioCfg.sdf        = NULL;
    audioCfg.mdf        = NULL;
    
    vpp_SpeechBTAudioCfg(&audioCfg);

    vpp_SpeechBTSetOutAlgGain(0);
    vpp_SpeechBTSetInAlgGain(0);

	//vpp_SpeechAdaFilterCoefInit();
	//vpp_SpeechAdaConvInit();
    // invalidate the encoder out buffer
    ((HAL_SPEECH_ENC_OUT_T *)hal_VocGetPointer(VPP_SPEECH_ENC_OUT_STRUCT))->encMode = HAL_INVALID;
    
    // move to STALL location (VoC irq generated)
    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
            return HAL_ERR_RESOURCE_BUSY;
    }
 
    return status;
            
}

//=============================================================================
// vpp_SpeechBTClose function
//-----------------------------------------------------------------------------
/// Close VPP Speech, clear VoC wakeup masks. This function is 
/// called each time a voice call is stopped.
//=============================================================================
PUBLIC VOID vpp_SpeechBTClose(VOID)
{

	//vpp_SpeechAdaFilterCoefBak();
    hal_VocClose();
#ifdef SMALL_BSS_RAM_SIZE
    if(g_vppSpeechRAMX_alloc)
    {
        mmc_MemFreeAll_BssRam(g_vppSpeechRAMX_alloc);
        g_vppSpeechRAMX_alloc=NULL;
    }
    if(g_vppSpeechRAMY_alloc)
    {
        mmc_MemFreeAll_BssRam(g_vppSpeechRAMY_alloc);
        g_vppSpeechRAMY_alloc=NULL;
    }
    if(g_vppSpeechCode_alloc)
    {
        mmc_MemFreeAll_BssRam(g_vppSpeechCode_alloc);
        g_vppSpeechCode_alloc=NULL;
    }
    if(g_vppAecRAMX_alloc)
    {
        mmc_MemFreeAll_BssRam(g_vppAecRAMX_alloc);
        g_vppAecRAMX_alloc=NULL;
    }	
#endif


    g_vppSpeechWakeupMode = VPP_SPEECH_WAKEUP_NO;
    
}

//=============================================================================
// vpp_SpeechBTReset function
//-----------------------------------------------------------------------------
/// Reset VPP (including swap buffer initialization).
//=============================================================================
PUBLIC VOID vpp_SpeechBTReset(VOID)
{
    ((VPP_SPEECH_INIT_T *)hal_VocGetPointer(VPP_SPEECH_INIT_STRUCT))->reset = 1; 
}

//=============================================================================
// vpp_SpeechBTAudioCfg function
//-----------------------------------------------------------------------------
/// Configure the audio parameters of VPP Speech.
/// @param pAudioCfg : pointer to the structure containing the audio configuration.
//=============================================================================
PUBLIC VOID vpp_SpeechBTAudioCfg(VPP_SPEECH_AUDIO_CFG_T *pAudioCfg)
{
        
    VPP_SPEECH_AUDIO_CFG_T *pDest = hal_VocGetPointer(VPP_SPEECH_AUDIO_CFG_STRUCT);
    
    // copy structure
    *pDest = *pAudioCfg;

    // convert to DMAI pointers
    pDest->sdf = hal_VocGetDmaiPointer(pDest->sdf,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDest->mdf = hal_VocGetDmaiPointer(pDest->mdf,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    
}

//============================================================================
// vpp_SpeechBTCodecCfg function
//-----------------------------------------------------------------------------
/// Configure the codec parameters in the decoder structure of VPP Speech.
/// @param pCodecCfg : pointer to the structure containing the decoder configuration.
//=============================================================================
PUBLIC VOID vpp_SpeechBTCodecCfg(HAL_SPEECH_DEC_IN_T *pCodecCfg)
{
    *(HAL_SPEECH_DEC_IN_T*) vpp_SpeechBTGetRxCodBuffer() = *pCodecCfg;
}


//=============================================================================
// vpp_SpeechBTScheduleOneFrame function
//-----------------------------------------------------------------------------
/// Schedule one decoding and/or one encoding frame. Convert to Vpp codec mode. 
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_SpeechBTScheduleOneFrame(VPP_SPEECH_WAKEUP_MODE_T    wakeupMode)
{

    HAL_ERR_T status = HAL_ERR_NO;
	
    if (wakeupMode == VPP_SPEECH_WAKEUP_SW_DEC)
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_1);
        
        if (HAL_ERR_NO != status) 
        {
                return status;
        }
    }

    if (wakeupMode == VPP_SPEECH_WAKEUP_SW_ENC)
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
        
        if (HAL_ERR_NO != status) 
        {
                return status;
        }
    }

	
/*
    if ((g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_DECENC) || (g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_DEC))
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_1);
        
        if (HAL_ERR_NO != status) 
        {
                return status;
        }
    }

    if ((g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_DECENC) || (g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_ENC))
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);        
    }
*/
    return status;
 
}




//=============================================================================
// vpp_SpeechBTGetRxPcmBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the output (Rx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechBTGetRxPcmBuffer(VOID)
{
    //return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_DEC_OUT_STRUCT);

    return (HAL_SPEECH_PCM_BUF_T *)g_BTRxBuf;
}

//=============================================================================
// vpp_SpeechBTGetTxPcmBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the input (Tx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechBTGetTxPcmBuffer(VOID)
{
//    return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_ENC_IN_STRUCT);

    return (HAL_SPEECH_PCM_BUF_T *)g_BTTxBuf;
}
//=============================================================================
// vpp_SpeechBTGetTxPcmBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the input (Tx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechBTGetBluetoothRxPcmBuffer(VOID)
{
//    return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_ENC_IN_STRUCT);

    return (HAL_SPEECH_PCM_BUF_T *)g_Bluetooth_RxBuf;
}
//=============================================================================
// vpp_SpeechBTGetTxPcmBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the input (Tx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechBTGetBluetoothTxPcmBuffer(VOID)
{
//    return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_ENC_IN_STRUCT);

    return (HAL_SPEECH_PCM_BUF_T *)g_Bluetooth_TxBuf;
}

//=============================================================================
// vpp_SpeechBTGetRxPcmBuffer_test function
//-----------------------------------------------------------------------------
/// @return pointer to the output (Rx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechBTGetRxPcmBuffer_test(VOID)
{
//    return (HAL_SPEECH_PCM_BUF_T *)g_BTRxBuf;
    return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_DEC_OUT_STRUCT);
}

//=============================================================================
// vpp_SpeechBTGetTxPcmBuffer_test function
//-----------------------------------------------------------------------------
/// @return pointer to the input (Tx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechBTGetTxPcmBuffer_test(VOID)
{
//    return (HAL_SPEECH_PCM_BUF_T *)g_BTTxBuf;

    return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_ENC_IN_STRUCT);
}

//=============================================================================
// vpp_SpeechBTGetRxCodBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the input (Rx) coded buffer
//=============================================================================
PUBLIC HAL_SPEECH_DEC_IN_T * vpp_SpeechBTGetRxCodBuffer(VOID)
{
    return (HAL_SPEECH_DEC_IN_T *) hal_VocGetPointer(VPP_SPEECH_DEC_IN_STRUCT);
}

//=============================================================================
// vpp_SpeechBTGetTxCodBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the output (Tx) coded buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_ENC_OUT_T * vpp_SpeechBTGetTxCodBuffer(VOID)
{
    return (CONST HAL_SPEECH_ENC_OUT_T *) hal_VocGetPointer(VPP_SPEECH_ENC_OUT_STRUCT);
}



//=============================================================================
// vpp_SpeechBTSetOutAlgGain function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC VOID vpp_SpeechBTSetOutAlgGain(INT32 alg)
{
    UINT32 algGain = vpp_SpeechBTOutAlgGainDb2Val(alg);
	UINT16 Aec_DigitalGain;

	switch (algGain)
	{
		case 0:			
			algGain=0x10;
			break;
		case 1:			
			algGain=0x16;           ///1.3
			break;
		case 2:			
			algGain=0x20;       ///1.3^2
			break;
		case 3:			
			algGain=0x2d;     ///1.3^3
			break;
		default:
            algGain=0x10;
			break;	
	}

    * ( UINT16 *) hal_VocGetPointer(GLOBAL_RECEIVER_DIGITAL_GAIN_ADDR)=algGain;
    * ( UINT16 *) hal_VocGetPointer(GLOBAL_RECEIVER_DIGITAL_MAXVALUE_ADDR)=(INT16)(((INT32)(32767*16))/algGain);

   	Aec_DigitalGain=((0xa*0x10*0x10)/algGain);

    * ( UINT16 *) hal_VocGetPointer(GLOBAL_AEC_SPK_DIGITAL_GAIN_ADDR)=Aec_DigitalGain;
    * ( UINT16 *) hal_VocGetPointer(GLOBAL_AEC_SPK_DIGITAL_MAXVALUE_ADDR)=(INT16)(((INT32)(32767*16))/Aec_DigitalGain);
}

//=============================================================================
// vpp_SpeechBTSetAecEnableFlag function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC VOID vpp_SpeechBTSetAecEnableFlag(INT8 AecEnbleFlag,INT8 AgcEnbleFlag)
{
    VPP_SPEECH_AEC_INIT_T *Aec_Init = hal_VocGetPointer(m_AECProcPara_ADDR);
    Aec_Init->AecEnbleFlag=AecEnbleFlag;
    Aec_Init->AgcEnbleFlag=AgcEnbleFlag;
}


//=============================================================================
// vpp_SpeechBTSetInAlgGain function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC VOID vpp_SpeechBTSetInAlgGain(INT32 alg)
{
    UINT32 algGain = vpp_SpeechBTInAlgGainDb2Val(alg);
	hal_HstSendEvent( 0xeeeeffff);
	hal_HstSendEvent( alg);
    if (algGain != 0)
	{
        algGain=algGain>>2;
        * ( UINT16 *) hal_VocGetPointer(GLOBAL_MIC_DIGITAL_GAIN_ADDR)=algGain;
        * ( UINT16 *) hal_VocGetPointer(GLOBAL_MIC_DIGITAL_MAXVALUE_ADDR)=(INT16)(((INT32)(32767*16))/algGain);
	}
	else
	{
	    * ( UINT16 *) hal_VocGetPointer(GLOBAL_MIC_DIGITAL_GAIN_ADDR)=0;
	    * ( UINT16 *) hal_VocGetPointer(GLOBAL_MIC_DIGITAL_MAXVALUE_ADDR)=32767;
	}
}



//=============================================================================
// vpp_SpeechBTSetMorphVoice function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC VOID vpp_SpeechBTSetMorphVoice(INT16 EnableFlag, INT16 MorphPara)
{
    *((INT16 *)hal_VocGetPointer(MORPHVOICE_ENABLE_FLAG_ADDR)) =EnableFlag;
    *((INT16 *)hal_VocGetPointer(PITCH_SHIFT_ADDR)) =MorphPara;
}



//=============================================================================
// vpp_SpeechBTSetAecPara function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC VOID vpp_SpeechBTSetAecPara(INT8 NoiseGainLimit,INT8 NoiseMin,INT8 NoiseGainLimitStep,INT8 AmpThr,INT8 StrongEchoFlag)
{
        
	VPP_SPEECH_AEC_INIT_T *Aec_Init =hal_VocGetPointer(m_AECProcPara_ADDR);

#if  !defined(SPEECH_NS_MIC_VOC_CLOSE)

	Aec_Init->NoiseGainLimit=127;
#else
	Aec_Init->NoiseGainLimit=NoiseGainLimit;
#endif
	Aec_Init->NoiseMin=NoiseMin;
	Aec_Init->NoiseGainLimitStep=NoiseGainLimitStep;
	Aec_Init->AmpThr=AmpThr;

	Aec_Init->StrongEchoFlag =0;

	Aec_Init->AFFilterAmp=32;

	Aec_Init->PFCLThrDT =4;
	Aec_Init->PFCLChanlGain = 128;

	Aec_Init->PFCLDTStartTime = 16;

	Aec_Init->PFCLDTDThreshold = 115;
	Aec_Init->PFCLDTD2ndThreshold = 64;	

	Aec_Init->PFCLDTDThrRatio=71;	


   	if (StrongEchoFlag==0)
	{
	      *((INT16 *)hal_VocGetPointer(GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR)) =1;
	     *((INT16 *)hal_VocGetPointer(GLOBAL_MIC_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR)) =8192;
		
	}
	else
	{
	     *((INT16 *)hal_VocGetPointer(GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR)) =8;
	     *((INT16 *)hal_VocGetPointer(GLOBAL_MIC_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR)) =32767;		

/*		Aec_Init->AFFilterAmp=128;

		Aec_Init->PFCLThrDT =64;
		Aec_Init->PFCLChanlGain = 128;

		Aec_Init->PFCLDTStartTime = 80;

		Aec_Init->PFCLDTDThreshold = 102;
		Aec_Init->PFCLDTD2ndThreshold = 64;		
		Aec_Init->PFCLDTDThrRatio=80;*/

	}
 


	 return;
    
}


//=============================================================================
// vpp_SpeechSetFilterPara function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC VOID vpp_SpeechBTSetFilterPara(UINT8 flags)
{
        
	*((INT16 *)hal_VocGetPointer(HIGH_PASS_FILTER_ENABLE_FLAG_ADDR)) = (flags>>0)&1; //HighPassFilterFlag;
	*((INT16 *)hal_VocGetPointer(NOTCH_FILTER_ENABLE_FLAG_ADDR)) = (flags>>1)&1; // NotchFilterFlag;
#if  !defined(SPEECH_NS_MIC_VOC_CLOSE)
	
	*((INT16 *)hal_VocGetPointer(MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =TRUE;//(flags>>2)&1;;
#else

	*((INT16 *)hal_VocGetPointer(MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =TRUE;
#endif
#if  !defined(SPEECH_NS_RECEIVER_VOC_CLOSE)
	*((INT16 *)hal_VocGetPointer(RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =TRUE;//(flags>>3)&1;;
#else

	*((INT16 *)hal_VocGetPointer(RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)) =TRUE;
#endif
	
	 return;
    
}



//=============================================================================
// vpp_SpeechBTSetMorphVoice function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC VOID vpp_SpeechBTSetReocrdSpeech(INT16 EnableFlag)
{
        
     *((INT16 *)hal_VocGetPointer(GLOBAL_SPEECH_RECORD_FLAG)) =EnableFlag;

}


//=============================================================================
// vpp_SpeechGetType function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
PUBLIC INT16 vpp_SpeechGetType(VOID)
{
        
   return  *((INT16 *)hal_VocGetPointer(GLOBAL_EXTERN_PROCESS_MIC_HANDLE_FLAG));

}

//=============================================================================
// vpp_SpeechSetMorphVoice function
//-----------------------------------------------------------------------------
/// 
//=============================================================================
/*PUBLIC VOID vpp_SpeechSetProcessMicEnableFlag(INT16 EnableFlag )
{

     *((INT16 *)hal_VocGetPointer(GLOBAL_EXTERN_PROCESS_MIC_ENABLE_FLAG)) =EnableFlag;

   return ;

}*/


PUBLIC UINT32 vpp_SpeechBTInAlgGainDb2Val(INT32 db)
{
    CONST UINT32 micAlgVal[] =
    {
        0x40, 0x5a, 0x80, 0xb5,
        0x100, 0x16a, 0x200, 0x2d4,
        0x400, 0x5a8, 0x800, 0xb50,
        0x1000, 0x16a0, 0x2000, 0x2d40,
    };
    if (db == CALIB_AUDIO_GAIN_VALUE_MUTE)
        return 0;
    else if (db < 0)
        db = 0;
    else if (db > 45)
        db = 45;
    return micAlgVal[db/3];
}


PUBLIC UINT32 vpp_SpeechBTOutAlgGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 9)
        db = 9;
    return db/3;
}
#if 0

//=============================================================================
//vpp_SpeechAecNoclibSpeakerForPitch
//-----------------------------------------------------------------------------
/// @return pointer to the output (Tx) coded buffer
//=============================================================================
PUBLIC CONST INT16 * vpp_SpeechAecHandleMod(VOID)
{
    return ( INT16 *) hal_VocGetPointer(GLOBAL_EXTERN_NOISE_SUPPRESS_MIC_FLAG);
}

PUBLIC  INT16 * vpp_SpeechBTGetNSMicBuffer(VOID)
{
    return (INT16 *)((UINT32)g_BluetoothNsMicBuf|0x20000000);//no cache;
}



//=============================================================================
//vpp_SpeechAecNoclibSpeakerForPitch
//-----------------------------------------------------------------------------
/// @return pointer to the output (Tx) coded buffer
//=============================================================================
PUBLIC  INT16 * vpp_SpeechAecNoclibSpeakerForPitch(VOID)
{
    return ( INT16 *) hal_VocGetPointer(AEC_NOCLIB_SPEAKER_FOR_NS_STRUCT);
}

//=============================================================================
//vpp_SpeechAdaFilterCoefInit
//-----------------------------------------------------------------------------
/// @return pointer to the output (Tx) coded buffer
//=============================================================================
PUBLIC  VOID vpp_SpeechAdaFilterCoefInit(VOID)
{
	 UINT16 i;
     UINT32 * pAdaFilterCoeftemp;
	 	
     pAdaFilterCoeftemp = ( UINT32 *)hal_VocGetPointer(g_Adaptive_Filter_COEF_STRUCT);
	 for(i=0; i<AF_BAK_SIZE; i++)
	 {
	     hal_HstSendEvent(SYS_EVENT,0x11000000+*(gAdaptiveFilterCoefBak+i));
         *(pAdaFilterCoeftemp+i) = *(gAdaptiveFilterCoefBak+i);
	 }
}

//=============================================================================
//vpp_SpeechAdaFilterCoefBak
//-----------------------------------------------------------------------------
/// @return pointer to the output (Tx) coded buffer
//=============================================================================
PUBLIC  VOID vpp_SpeechAdaFilterCoefBak(VOID)
{
     UINT16 i;
     UINT32 * pAdaFilterCoeftemp;
     pAdaFilterCoeftemp = ( UINT32 *) hal_VocGetPointer(g_Adaptive_Filter_COEF_STRUCT);
	 for(i=0; i<AF_BAK_SIZE; i++)
	 {
	     hal_HstSendEvent(SYS_EVENT,0x11000000+*(pAdaFilterCoeftemp+i));
         *(gAdaptiveFilterCoefBak+i) = *(pAdaFilterCoeftemp+i);
	 }
}

//=============================================================================
//vpp_SpeechAdaFilterCoefInit
//-----------------------------------------------------------------------------
/// @return pointer to the output (Tx) coded buffer
//=============================================================================
PUBLIC  VOID vpp_SpeechAdaConvInit(VOID)
{
	 VPP_SPEECH_AF_INIT_T *pAecAFConvTimeStruct = (VPP_SPEECH_AF_INIT_T *)hal_VocGetPointer(AEC_AF_CONV_TIME_STRUCT);

		
	 pAecAFConvTimeStruct->Aec_AF_Conv_Enable = 1;
	 
     pAecAFConvTimeStruct->Aec_AF_Conv_Time1 = 50*5;//3S
     pAecAFConvTimeStruct->Aec_AF_Conv_Time2 = 50*(5+5);//5S
     pAecAFConvTimeStruct->Aec_AF_Frame_Count = 0;
     pAecAFConvTimeStruct->Aec_AF_Echo_Gain_Limit = 128;//0-511;
	 pAecAFConvTimeStruct->Aec_AF_Echo_Gain_Step = 1;
     pAecAFConvTimeStruct->Aec_AF_Speaker_TH = 256;//0-32767;
}
#endif

#if 1
//=============================================================================
// vpp_AudioJpegDecScheduleOneFrame function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_SpeechScheduleOneFrame()
{ 
    if(gMicGain<=1)
	{
        gMicGain = 1;
	}

	*((INT16 *)hal_VocGetPointer(NS_MIC_GAIN_ADDR)) = gMicGain;
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
}
#endif
