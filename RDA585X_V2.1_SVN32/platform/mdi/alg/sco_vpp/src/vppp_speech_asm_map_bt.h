#ifndef MAP_ADDR_H
#define MAP_ADDR_H

/* This file defines all the variables as memory addresses.
   It's created by voc_map automatically.*/

/********************
 **  SECTION_SIZE  **
 ********************/

#define COMMON_GLOBAL_X_SIZE                              		7240
#define COMMON_GLOBAL_Y_SIZE                              		9232
#define AEC_GLOBAL_X_SIZE                                 		2434
#define AEC_GLOBAL_Y_SIZE                                 		0
#define AEC_LOCAL_X_SIZE                                  		0
#define AEC_LOCAL_Y_SIZE                                  		0
#define COMMON_LOCAL_X_SIZE                               		0
#define COMMON_LOCAL_Y_SIZE                               		160

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                        		(0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                        		(0 + RAM_Y_BEGIN_ADDR)
#define AEC_GLOBAL_X_BEGIN_ADDR                           		(7240 + RAM_X_BEGIN_ADDR)
#define AEC_GLOBAL_Y_BEGIN_ADDR                           		(9232 + RAM_Y_BEGIN_ADDR)
#define AEC_LOCAL_X_BEGIN_ADDR                            		(9674 + RAM_X_BEGIN_ADDR)
#define AEC_LOCAL_Y_BEGIN_ADDR                            		(9232 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                         		(9674 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                         		(9232 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
//VPP_SPEECH_DEC_IN_STRUCT
#define VPP_SPEECH_DEC_IN_STRUCT                			(0 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define INPUT_BITS2_ADDR                        			(0 + VPP_SPEECH_DEC_IN_STRUCT) //26 short

//VPP_SPEECH_ENC_OUT_STRUCT
#define VPP_SPEECH_ENC_OUT_STRUCT               			(26 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define OUTPUT_BITS2_ADDR                       			(0 + VPP_SPEECH_ENC_OUT_STRUCT) //22 short

//VPP_SPEECH_ENC_IN_STRUCT
#define VPP_SPEECH_ENC_IN_STRUCT                			(48 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define INPUT_SPEECH_BUFFER2_ADDR               			(0 + VPP_SPEECH_ENC_IN_STRUCT) //160 short
#define INPUT_SPEECH_BUFFER1_ADDR               			(160 + VPP_SPEECH_ENC_IN_STRUCT) //160 short

//VPP_SPEECH_DEC_OUT_STRUCT
#define VPP_SPEECH_DEC_OUT_STRUCT               			(368 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define OUTPUT_SPEECH_BUFFER2_ADDR              			(0 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short
#define OUTPUT_SPEECH_BUFFER1_ADDR              			(160 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short

#define GLOBAL_SDF_SAMPLS_ADDR                  			(688 + COMMON_GLOBAL_X_BEGIN_ADDR) //64 short
#define GLOBAL_MDF_SAMPLS_ADDR                  			(752 + COMMON_GLOBAL_X_BEGIN_ADDR) //64 short
//SPEECH_AEC_NS_DATA_ADDR
#define SPEECH_AEC_NS_DATA_ADDR                 			(816 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define SpxPpState_frame_size                   			(0 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_ps_size                      			(1 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_sampling_rate                			(2 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_nbands                       			(3 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_bank_bank_left               			(4 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_bank                         			(SpxPpState_bank_bank_left) //alias
#define SpxPpState_bank_bank_right              			(84 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_bank_filter_left             			(164 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_bank_filter_right            			(244 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_bank_nb_banks                			(324 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_bank_len                     			(325 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_denoise_enabled              			(326 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_vad_enabled                  			(327 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_dereverb_enabled             			(328 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_reverb_decay                 			(329 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_reverb_level                 			(330 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_speech_prob_start            			(331 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_speech_prob_continue         			(332 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_noise_suppress               			(333 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_echo_suppress                			(334 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_echo_suppress_active         			(335 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_echo_state_ADDR              			(336 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_speech_prob                  			(337 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_frame                        			(338 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_frame_N                      			(418 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_window                       			(498 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_ft                           			(658 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_gain                         			(818 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_gain_M                       			(898 + SPEECH_AEC_NS_DATA_ADDR) //24 short
#define SpxPpState_prior                        			(922 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_prior_M                      			(1002 + SPEECH_AEC_NS_DATA_ADDR) //24 short
#define SpxPpState_post                         			(1026 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_post_M                       			(1106 + SPEECH_AEC_NS_DATA_ADDR) //24 short
#define SpxPpState_gain2                        			(1130 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_gain2_M                      			(1210 + SPEECH_AEC_NS_DATA_ADDR) //24 short
#define SpxPpState_gain_floor                   			(1234 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_gain_floor_M                 			(1314 + SPEECH_AEC_NS_DATA_ADDR) //24 short
#define SpxPpState_zeta                         			(1338 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_zeta_M                       			(1418 + SPEECH_AEC_NS_DATA_ADDR) //24 short
#define SpxPpState_inbuf                        			(1442 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_outbuf                       			(1522 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_nb_adapt                     			(1602 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_was_speech                   			(1603 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_min_count                    			(1604 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_fft_lookup_ADDR              			(1605 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_fft_lookup_forward_substate_nfft			(1606 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_fft_lookup                   			(SpxPpState_fft_lookup_forward_substate_nfft) //alias
#define SpxPpState_fft_lookup_forward_substate  			(SpxPpState_fft_lookup_forward_substate_nfft) //alias
#define SpxPpState_fft_lookup_forward_substate_inverse			(1607 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_fft_lookup_forward_substate_factors			(1608 + SPEECH_AEC_NS_DATA_ADDR) //64 short
#define SpxPpState_fft_lookup_forward_substate_twiddles			(1672 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_fft_lookup_forward_tmpbuf    			(1832 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_fft_lookup_forward_super_twiddles			(1992 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_substate_nfft			(2152 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_fft_lookup_backward_substate 			(SpxPpState_fft_lookup_backward_substate_nfft) //alias
#define SpxPpState_fft_lookup_backward_substate_inverse			(2153 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_fft_lookup_backward_substate_factors			(2154 + SPEECH_AEC_NS_DATA_ADDR) //64 short
#define SpxPpState_fft_lookup_backward_substate_twiddles			(2218 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_tmpbuf   			(2378 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_super_twiddles			(2538 + SPEECH_AEC_NS_DATA_ADDR) //160 short
#define SpxPpState_fft_lookup_N                 			(2698 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_frame_shift                  			(2699 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_noise                        			(2700 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define SpxPpState_noise_M                      			(2860 + SPEECH_AEC_NS_DATA_ADDR) //24 word
#define SpxPpState_reverb_estimate              			(2908 + SPEECH_AEC_NS_DATA_ADDR) //104 word
#define SpxPpState_old_ps                       			(3116 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define SpxPpState_old_ps_M                     			(3276 + SPEECH_AEC_NS_DATA_ADDR) //24 word
#define SpxPpState_ps                           			(3324 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define SpxPpState_ps_M                         			(3484 + SPEECH_AEC_NS_DATA_ADDR) //24 word
#define SpxPpState_S                            			(3532 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define SpxPpState_Smin                         			(3692 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define SpxPpState_Stmp                         			(3852 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define SpxPpState_update_prob                  			(4012 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define SpxPpState_echo_noise                   			(4092 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define SpxPpState_echo_noise_M                 			(4252 + SPEECH_AEC_NS_DATA_ADDR) //24 word
#define SpxPpState_residual_echo                			(4300 + SPEECH_AEC_NS_DATA_ADDR) //104 word
#define Speaker_SpxPpState_nb_adapt             			(4508 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define Speaker_SpxPpState_min_count            			(4509 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define Speaker_SpxPpState_S                    			(4510 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define Speaker_SpxPpState_Smin                 			(4670 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define Speaker_SpxPpState_Stmp                 			(4830 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define Speaker_SpxPpState_noise                			(4990 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define Speaker_SpxPpState_noise_M              			(5150 + SPEECH_AEC_NS_DATA_ADDR) //24 word
#define Speaker_SpxPpState_old_ps               			(5198 + SPEECH_AEC_NS_DATA_ADDR) //80 word
#define Speaker_SpxPpState_old_ps_M             			(5358 + SPEECH_AEC_NS_DATA_ADDR) //24 word
#define Speaker_SpxPpState_zeta                 			(5406 + SPEECH_AEC_NS_DATA_ADDR) //80 short
#define Speaker_SpxPpState_zeta_M               			(5486 + SPEECH_AEC_NS_DATA_ADDR) //24 short
#define SpxPpState_was_speaker                  			(5510 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define SpxPpState_for_fill                     			(5511 + SPEECH_AEC_NS_DATA_ADDR) //1 short
#define CONST_0_ADDR                            			(5512 + SPEECH_AEC_NS_DATA_ADDR) //1 word
#define CONST_0x40000000_ADDR                   			(5514 + SPEECH_AEC_NS_DATA_ADDR) //1 word
#define CONST_15_ADDR                           			(5516 + SPEECH_AEC_NS_DATA_ADDR) //1 word

#define ReservedNS_ADDR                         			(6334 + COMMON_GLOBAL_X_BEGIN_ADDR) //86 short
#define GLOBAL_SPEAKER_MAX_ADDR                 			(6420 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_ATTENUATION_GAIN_ADDR        			(6421 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_NOISE_LIMITE_ADDR            			(6422 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVER_NOISE_LIMITE_ADDR       			(6423 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_SPEAKER_MAX_THRESHOLD_ADDR       			(6424 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_SPEAKER_MAX_OLD_ADDR             			(6425 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define COS_TABLE_ADDR                          			(6426 + COMMON_GLOBAL_X_BEGIN_ADDR) //129 short
#define TAN_TABLE_ADDR                          			(6555 + COMMON_GLOBAL_X_BEGIN_ADDR) //191 short
#define GLOBAL_NON_CLIP_TEMP_BUFFER             			(COS_TABLE_ADDR) //alias
#define GLOBAL_NON_CLIP_MIC_HISTORYDATA         			(6746 + COMMON_GLOBAL_X_BEGIN_ADDR) //160 short
#define GLOBAL_NON_CLIP_CROSSZEROCOUNTER        			(6906 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_CROSSZEPEAK             			(6907 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DIGITAL_GAIN_ADDR                			(6908 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DIGITAL_MAXVALUE_ADDR            			(6909 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_NOTCH_FILTER_XN12_YN12_ADDR      			(6910 + COMMON_GLOBAL_X_BEGIN_ADDR) //90 short
#define GLOBAL_NOTCH_FILTER_COEF_ADDR           			(7000 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_NOTCH_FILTER_COEF_ADDR0          			(7001 + COMMON_GLOBAL_X_BEGIN_ADDR) //15 short
#define MIC_FILT_ADDR                           			(7016 + COMMON_GLOBAL_X_BEGIN_ADDR) //224 short
#define CONST_0x7FFF_ADDR                       			(CONST_0x00007FFF_ADDR) //alias
#define CONST_0xFFFF_ADDR                       			(CONST_0x0000FFFF_ADDR) //alias
#define CONST_0x8000_ADDR                       			(CONST_0x00008000_ADDR) //alias
#define CONST_0x4000_ADDR                       			(CONST_0x00004000L_ADDR) //alias
#define CONST_0x5999_ADDR                       			((6+ STATIC_CONST_CDOWN_ADDR)) //alias

//GLOBAL_VARS
//VPP_SPEECH_INIT_STRUCT
#define VPP_SPEECH_INIT_STRUCT                  			(0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_LOADED_VOCODER_ADDR              			(0 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_EFR_ADDR              			(1 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_ENC_MODE_ADDR         			(2 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_DEC_MODE_ADDR         			(3 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_VOCODER_SOURCE_CODE_ADDR_ADDR    			(4 + VPP_SPEECH_INIT_STRUCT) //6 short
#define GLOBAL_AMR_EFR_SOURCE_CODE_ADDR_ADDR    			(10 + VPP_SPEECH_INIT_STRUCT) //4 short
#define GLOBAL_AMR_ENC_MODE_SOURCE_CODE_ADDR_ADDR			(14 + VPP_SPEECH_INIT_STRUCT) //16 short
#define GLOBAL_AMR_DEC_MODE_SOURCE_CODE_ADDR_ADDR			(30 + VPP_SPEECH_INIT_STRUCT) //16 short
#define GLOBAL_CONSTANTS_ADDR_ADDR              			(46 + VPP_SPEECH_INIT_STRUCT) //8 short
#define GLOBAL_RESET_ADDR                       			(54 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_IRQGEN_ADDR                      			(55 + VPP_SPEECH_INIT_STRUCT) //1 short

//VPP_SPEECH_AUDIO_CFG_STRUCT
#define VPP_SPEECH_AUDIO_CFG_STRUCT             			(56 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_ECHO_ES_ON                       			(0 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_ES_VAD                      			(1 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_ES_DTD                      			(2 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_EC_REL                      			(3 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_EC_MU                       			(4 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_EC_MIN                      			(5 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ENC_MICGAIN_ADDR                 			(6 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_DEC_SPKGAIN_ADDR                 			(7 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_SDF_ADDR                         			(8 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short
#define GLOBAL_MDF_ADDR                         			(10 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short
#define GLOBAL_IF1_flag_ADDR                    			(12 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short

#define GLOBAL_DTX_ADDR                         			(69 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RELOAD_MODE_ADDR                 			(70 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_AMR_EFR_RESET_ADDR               			(71 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_MICGAIN_ADDR                 			(72 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_SPKGAIN_ADDR                 			(73 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SDF_COEFFS_ADDR                  			(74 + COMMON_GLOBAL_Y_BEGIN_ADDR) //64 short
#define GLOBAL_BFI_ADDR                         			(138 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_UFI_ADDR                         			(139 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SID_ADDR                         			(140 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_TAF_ADDR                         			(141 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_MODE_ADDR                    			(142 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_MODE_ADDR                    			(143 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_USED_MODE_ADDR               			(144 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_EFR_FLAG_ADDR                    			(145 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_RESET_FLAG_ADDR                     			(146 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_MAIN_RESET_FLAG_OLD_ADDR            			(147 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MDF_COEFFS_ADDR                  			(148 + COMMON_GLOBAL_Y_BEGIN_ADDR) //64 short
#define GLOBAL_ECHO_SUPPRESS_EXP_COUNTER        			(212 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ECHO_CANCEL_EXP_MU               			(213 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ECHO_CANCEL_DEC_PWR              			(214 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_HIGH_PASS_FILTER_XN_1_ADDR       			(216 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER_XN_2_ADDR       			(217 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER_YN_1_ADDR       			(218 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER_YN_2_ADDR       			(219 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_COMFORTALBE_NOISE_ADDR           			(220 + COMMON_GLOBAL_Y_BEGIN_ADDR) //160 short
#define GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR  			(380 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_VAD_COUNTER                      			(381 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER2_XN_1_ADDR      			(382 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER2_XN_2_ADDR      			(383 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER2_YN_1_ADDR      			(384 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER2_YN_2_ADDR      			(385 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_DIGITAL_GAIN_ADDR            			(386 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_DIGITAL_MAXVALUE_ADDR        			(387 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVER_DIGITAL_GAIN_ADDR       			(388 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVER_DIGITAL_MAXVALUE_ADDR   			(389 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUTENERGY_ADDR                			(390 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_INPUTENERGY_ADDR                 			(392 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_COMFORTALBE_NOISE_BAK_ADDR       			(394 + COMMON_GLOBAL_Y_BEGIN_ADDR) //160 short
#define GLOBAL_SPEECH_RAMX_BACKUP_ADDR          			(554 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_SPEECH_CODE_BACKUP_ADDR          			(556 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AEC_RAMX_BACKUP_ADDR             			(558 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AEC_CODE_ADDR                    			(560 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AEC_CONSTX_ADDR                  			(562 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_RX_ADDR                          			(564 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_TX_ADDR                          			(566 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define UpLimit_Spk_ADDR                        			(568 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define DnLimit_Spk_ADDR                        			(570 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define Minus_UpLimit_Spk_ADDR                  			(572 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define Minus_DnLimit_Spk_ADDR                  			(574 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
//m_AECProcPara_ADDR
#define m_AECProcPara_ADDR                      			(576 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define AEC_ENABLE_FLAG_ADDR                    			(0 + m_AECProcPara_ADDR) //1 short
#define AGC_ENABLE_FLAG_ADDR                    			(1 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_ProcessMode               			(2 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_InitRegularFactor         			(3 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFUpdateFactor            			(4 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFCtrlSpeedUpFactor       			(5 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFFilterAmp               			(6 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoEstMethod             			(7 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseUpdateFactor         			(8 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SNRUpdateFactor           			(9 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SNRLimit                  			(10 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseOverEstFactor        			(11 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseGainLimit            			(12 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoUpdateFactor          			(13 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SERUpdateFactor           			(14 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SERLimit                  			(15 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoOverEstFactor         			(16 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoGainLimit             			(17 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_CNGFactor                 			(18 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AmpGain                   			(19 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseMin                  			(20 + m_AECProcPara_ADDR) //1 word
#define m_AECProcPara_PFPrevEchoEstLimit        			(22 + m_AECProcPara_ADDR) //1 word
#define m_AECProcPara_PFDTEchoOverEstFactor     			(24 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_pPFEchoGainLimitAmp       			(25 + m_AECProcPara_ADDR) //9 short
#define m_AECProcPara_NoiseGainLimitStep        			(34 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AmpThr_ADDR               			(35 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDataSmoothFactor      			(36 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLNoiseUpdateFactor     			(37 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrSigDet             			(38 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrDT                 			(39 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_DataSmoothFactor          			(40 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLChanlGain             			(41 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrNT                 			(42 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrDTDiff             			(43 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTHoldTime            			(44 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTStartTime           			(45 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTDUpdateFactor       			(46 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTDThreshold          			(47 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTD2ndThreshold       			(48 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_StrongEchoFlag            			(49 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara__PFCLDTDThrRatio          			(50 + m_AECProcPara_ADDR) //1 short

#define GLOBAL_SPEECH_RECORD_FLAG               			(627 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_AEC_RESET_ADDR                   			(628 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_MORPH_RESET_ADDR                 			(629 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define MORPHVOICE_ENABLE_FLAG_ADDR             			(630 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_SHIFT_ADDR                        			(631 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BLUETOOTH_RX_ADDR                			(632 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_MORPH_CODE_ADDR                  			(,y) //alias
#define GLOBAL_BLUETOOTH_TX_ADDR                			(634 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_MORPH_CONSTY_ADDR                			(,y) //alias
#define GLOBAL_SPEECH_RAMY_BACKUP_ADDR          			(636 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define ingSumPhase_ADDR                        			(638 + COMMON_GLOBAL_Y_BEGIN_ADDR) //129 short
#define gRover_ADDR                             			(767 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_VOCODER_RESET_ADDR               			(768 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUT_Vad_ADDR                  			(769 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUT_SP_ADDR                   			(770 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_AMR_FRAME_TYPE_ADDR                 			(771 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ENC_INPUT_ADDR_ADDR                     			(772 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_ADDR                    			(773 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ENC_INPUT_ADDR_BAK_ADDR                 			(774 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_BAK_ADDR                			(775 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
//SPEECH_AEC_NS_CONST_ADDR
#define SPEECH_AEC_NS_CONST_ADDR                			(776 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_NS_CONST_ADDR                    			(0 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define GLOBAL_NS_FFT_COF_ADDR                  			(2 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_ATAN01_M                          			(4 + SPEECH_AEC_NS_CONST_ADDR) //4 word
#define SPEEX_COS_PI_2_L                        			(12 + SPEECH_AEC_NS_CONST_ADDR) //4 word
#define SPEEX_SQRT_C                            			(20 + SPEECH_AEC_NS_CONST_ADDR) //4 short
#define SPEEX_UPDATE_NOISE_X                    			(24 + SPEECH_AEC_NS_CONST_ADDR) //4 short
#define SPEEX_HYPERGEOM_GAIN_TABLE              			(28 + SPEECH_AEC_NS_CONST_ADDR) //22 short
#define SPEEX_EXP_D                             			(50 + SPEECH_AEC_NS_CONST_ADDR) //4 short
#define SPEEX_CONST_0                           			(54 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_1                           			(56 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_2                           			(58 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_3                           			(60 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_4                           			(62 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_5                           			(64 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_6                           			(66 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_7                           			(68 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_8                           			(70 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_14                          			(72 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_16                          			(74 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_17                          			(76 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_19                          			(78 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_20                          			(80 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_29                          			(82 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_64                          			(84 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_97                          			(86 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_100                         			(88 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_128                         			(90 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_236                         			(92 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_256                         			(94 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_472                         			(96 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_512                         			(98 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_983                         			(100 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_1000                        			(102 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_2458                        			(104 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_3277                        			(106 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_4915                        			(108 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_6521                        			(110 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_6711                        			(112 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_8192                        			(114 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_9830                        			(116 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_10000                       			(118 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_10912                       			(120 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_16384                       			(122 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_19458                       			(124 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_19661                       			(126 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_20000                       			(128 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_20839                       			(130 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_21290                       			(132 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_21924                       			(134 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_22938                       			(136 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_23637                       			(138 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_24576                       			(140 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_25736                       			(142 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_26214                       			(144 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_29164                       			(146 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_29458                       			(148 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_32000                       			(150 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_32766                       			(152 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_32767                       			(154 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_32768                       			(156 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_65535                       			(158 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_65536                       			(160 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_131071                      			(162 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_131072                      			(164 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_0x80000                     			(166 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_1087164                     			(168 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_0x800000                    			(170 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_0x7fffffff                  			(172 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_NEG21290                    			(174 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_NEG15                       			(176 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_NEG2                        			(178 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_CONST_NEG32767                    			(180 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_VAR_RL7_ADDR                      			(182 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_REF_BUF                           			(184 + SPEECH_AEC_NS_CONST_ADDR) //80 short
#define FILTERBANK_NEW_SAMPLING                 			(264 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define FILTERBANK_NEW_TYPE                     			(265 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define FILTERBANK_NB_BANKS                     			(SpxPpState_bank_nb_banks) //alias
#define FILTERBANK_LEN                          			(SpxPpState_bank_len) //alias
#define FILTERBANK_DF                           			(266 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define FILTERBANK_MAX_MEL                      			(268 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define FILTERBANK_MEL_INTERVAL                 			(270 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPEEX_ST_WINDOW                         			(272 + SPEECH_AEC_NS_CONST_ADDR) //160 short
#define CONJ_WINDOW_LEN                         			(432 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPEEX_PRE_RETURN                        			(433 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN      			(434 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPX_KF_BFLY3_EPI3                       			(436 + SPEECH_AEC_NS_CONST_ADDR) //1 word
#define SPX_KF_BFLY_SCRATCH                     			(438 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_0                   			(SPX_KF_BFLY_SCRATCH) //alias
#define SPX_KF_BFLY_SCRATCH_0i                  			(439 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_1                   			(440 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_1i                  			(441 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_2                   			(442 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_2i                  			(443 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_3                   			(444 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_3i                  			(445 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_4                   			(446 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_4i                  			(447 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_5                   			(448 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_5i                  			(449 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_6                   			(450 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_6i                  			(451 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_7                   			(452 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_7i                  			(453 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_8                   			(454 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_8i                  			(455 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_9                   			(456 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_9i                  			(457 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_10                  			(458 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_10i                 			(459 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_11                  			(460 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_11i                 			(461 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_12                  			(462 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_12i                 			(463 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_YA                          			(464 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_YAi                         			(465 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_YB                          			(466 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_YBi                         			(467 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_FSTRIDE                     			(468 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_M                           			(469 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_N                           			(470 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_MM                          			(471 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPX_KF_BFLY_TMP                         			(472 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPEEX_PRE_RESET                         			(473 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPEEX_PRE_FRAME_NUM                     			(474 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define SPEEX_FOR_FILL                          			(475 + SPEECH_AEC_NS_CONST_ADDR) //1 short
#define Speaker_SpxPpState_inbuf                			(476 + SPEECH_AEC_NS_CONST_ADDR) //80 short
#define Speaker_SpxPpState_outbuf               			(556 + SPEECH_AEC_NS_CONST_ADDR) //80 short
#define PITCH_FILTER_SPEAKER_ADDR               			(636 + SPEECH_AEC_NS_CONST_ADDR) //314 short
#define PITCH_FILTER_SPEAKER_HISTORY_ADDR       			(950 + SPEECH_AEC_NS_CONST_ADDR) //240 short
#define PITCH_FILTER_MIC_HISTORY_ADDR           			(1190 + SPEECH_AEC_NS_CONST_ADDR) //240 short
#define Reserved2_ADDR                          			(1430 + SPEECH_AEC_NS_CONST_ADDR) //6576 short

#define GLOBAL_NON_CLIP_SPK_HISTORYDATA         			(8782 + COMMON_GLOBAL_Y_BEGIN_ADDR) //160 short
#define HIGH_PASS_FILTER_ENABLE_FLAG_ADDR       			(8942 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define NOTCH_FILTER_ENABLE_FLAG_ADDR           			(8943 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define NOISE_SUPPRESSER_ENABLE_FLAG_ADDR       			(8944 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define NOISE_SUPPRESSER_WITHOUT_SPEECH_ENABLE_FLAG_ADDR			(8945 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_AGC_acc_error_l                  			(8946 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AGC_MAXVAL                       			(8948 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AGC_Coeff_l                      			(8950 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_MIC_ENERGY_COUNTER_ADDR          			(8952 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NOISE_LIMITE_ADDR                			(8953 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_AEC_SPK_DIGITAL_GAIN_ADDR        			(8954 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_AEC_SPK_DIGITAL_MAXVALUE_ADDR    			(8955 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define CVSD_LOWPASS_AEC_SPACE_set_buffer       			(8956 + COMMON_GLOBAL_Y_BEGIN_ADDR) //32 short
//CVSD_LOWPASS_FILTER_STRUCT
#define CVSD_LOWPASS_FILTER_STRUCT              			(8988 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define CVSD_LOWPASS_DEC_FILTER_01_backup       			(0 + CVSD_LOWPASS_FILTER_STRUCT) //40 short
#define CVSD_LOWPASS_DEC_FILTER_02_backup       			(40 + CVSD_LOWPASS_FILTER_STRUCT) //12 short
#define CVSD_LOWPASS_DEC_FILTER_03_backup       			(52 + CVSD_LOWPASS_FILTER_STRUCT) //8 short
#define CVSD_LOWPASS_ENC_FILTER_01_backup       			(60 + CVSD_LOWPASS_FILTER_STRUCT) //40 short
#define CVSD_LOWPASS_ENC_FILTER_02_backup       			(100 + CVSD_LOWPASS_FILTER_STRUCT) //12 short
#define CVSD_LOWPASS_ENC_FILTER_03_backup       			(112 + CVSD_LOWPASS_FILTER_STRUCT) //8 short
#define CVSD_LOWPASS_FILTER01_DATA_temp         			(120 + CVSD_LOWPASS_FILTER_STRUCT) //4 short
#define CVSD_LOWPASS_FILTER02_DATA_temp         			(124 + CVSD_LOWPASS_FILTER_STRUCT) //8 short
#define CVSD_LOWPASS_FILTER03_DATA_temp         			(132 + CVSD_LOWPASS_FILTER_STRUCT) //16 short
#define CVSD_LOWPASS_FILTER_ROUND               			(148 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_DEC_OUTPUT_ROUND                   			(150 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_LOWPASS_DEC_ENC_FLAG               			(152 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_enc_delta                          			(154 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_dec_delta                          			(156 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_delta_min                          			(158 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_delta_max                          			(160 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_y_min                              			(162 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_y_max                              			(164 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_enc_xest_l                         			(166 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_dec_xest_l                         			(168 + CVSD_LOWPASS_FILTER_STRUCT) //1 word
#define CVSD_ENC_JUDGE_alpha_buf                			(170 + CVSD_LOWPASS_FILTER_STRUCT) //1 short
#define CVSD_DEC_JUDGE_alpha_buf                			(171 + CVSD_LOWPASS_FILTER_STRUCT) //1 short
#define CVSD_enc_first_frame_flag               			(172 + CVSD_LOWPASS_FILTER_STRUCT) //1 short
#define CVSD_dec_first_frame_flag               			(173 + CVSD_LOWPASS_FILTER_STRUCT) //1 short
#define CVSD_LOWPASS_CONST_FILTER3_Coef_TAB     			(174 + CVSD_LOWPASS_FILTER_STRUCT) //5 short
#define CVSD_LOWPASS_CONST_FILTER2_Coef_TAB     			(179 + CVSD_LOWPASS_FILTER_STRUCT) //7 short
#define CVSD_LOWPASS_CONST_FILTER1_Coef_TAB     			(186 + CVSD_LOWPASS_FILTER_STRUCT) //21 short
#define CVSD_LOWPASS_CONST_0x000f               			(207 + CVSD_LOWPASS_FILTER_STRUCT) //1 short
#define CVSD_LOWPASS_CONST_FILTER_GAIN_Coef_TAB 			(208 + CVSD_LOWPASS_FILTER_STRUCT) //4 short

//AEC_AF_CONV_TIME_STRUCT
#define AEC_AF_CONV_TIME_STRUCT                 			(9200 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define AEC_AF_CONV_TIME_ENABLE                 			(0 + AEC_AF_CONV_TIME_STRUCT) //1 short
#define AEC_AF_CONV_TIME1                       			(1 + AEC_AF_CONV_TIME_STRUCT) //1 short
#define AEC_AF_CONV_TIME2                       			(2 + AEC_AF_CONV_TIME_STRUCT) //1 short
#define AEC_AF_FRAME_COUNT                      			(3 + AEC_AF_CONV_TIME_STRUCT) //1 short
#define AEC_AF_SPEAKER_TH                       			(4 + AEC_AF_CONV_TIME_STRUCT) //1 short
#define AEC_AF_ECHO_GAIN_STEP                   			(5 + AEC_AF_CONV_TIME_STRUCT) //1 short
#define AEC_AF_ECHO_GAIN_LIMIT                  			(6 + AEC_AF_CONV_TIME_STRUCT) //1 short
#define AEC_AF_ECHO_GAIN_LIMIT_NOW              			(7 + AEC_AF_CONV_TIME_STRUCT) //1 short

#define PITCH_FILTER_ENABLE_FLAG_ADDR           			(9208 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define NS_ENABLE_FLAG_ADDR                     			(9209 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_EXTERN_NOISE_SUPPRESS_MIC_FLAG   			(9210 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_R1_INDEX                   			(9211 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_R2_INDEX                   			(9212 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_R3_INDEX                   			(9213 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_R1_MAX                     			(9214 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define PITCH_FILTER_R2_MAX                     			(9216 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define PITCH_FILTER_R3_MAX                     			(9218 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define PITCH_FILTER_R1_NORM                    			(9220 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define PITCH_FILTER_R2_NORM                    			(9222 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define PITCH_FILTER_R3_NORM                    			(9224 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define PITCH_FILTER_R_M                        			(9226 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_R_M_OLD                    			(9227 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_R_B                        			(9228 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_R_B_OLD                    			(9229 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_FILTER_COUNT                      			(9230 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define NS_INPUT_DATA_ADDR_ADDR                 			(9231 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define CONST_0x4CCC_ADDR                       			((148+ STATIC_CONST_table_gain_lowrates_ADDR)) //alias

/******************
 **  AEC_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define PassThrough                             			(0) //alias
#define AdaptiveFilter                          			(1) //alias
#define AdaptiveFilterPostFilter                			(2) //alias
#define NoiseSuppression                        			(3) //alias
#define LowDelayOpenLoopAF                      			(4) //alias
#define TwoStepNoiseSuppression                 			(5) //alias
#define AdaptiveFilterPostFilterEnhanced        			(6) //alias
#define ECHO_COHF                               			(0) //alias
#define ECHO_TF                                 			(1) //alias
#define r_SpkAnaFilterDelayLine                 			(0 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define r_MicAnaFilterDelayLine                 			(128 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define r_OutSynFilterDelayLine                 			(256 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define g_FilterBank_MulBuf                     			(384 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define g_SpkAnaFilterOutBufI                   			(512 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_SpkAnaFilterOutBufQ                   			(528 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_MicAnaFilterOutBufI                   			(544 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_MicAnaFilterOutBufQ                   			(560 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_OutSynFilterInBufI                    			(576 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_OutSynFilterInBufQ                    			(592 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define r_AF_SpkSigBufI                         			(608 + AEC_GLOBAL_X_BEGIN_ADDR) //72 short
#define r_AF_SpkSigBufQ                         			(680 + AEC_GLOBAL_X_BEGIN_ADDR) //72 short
//g_Adaptive_Filter_COEF_STRUCT
#define g_Adaptive_Filter_COEF_STRUCT           			(752 + AEC_GLOBAL_X_BEGIN_ADDR) //struct
#define r_AF_FilterBufI                         			(0 + g_Adaptive_Filter_COEF_STRUCT) //72 word
#define r_AF_FilterBufQ                         			(144 + g_Adaptive_Filter_COEF_STRUCT) //72 word

#define g_AF_FilterOutBufI                      			(1040 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve00_addr                          			(1049 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_FilterOutBufQ                      			(1050 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve01_addr                          			(1059 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_AF_OldSpkDataBufI                     			(1060 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve02_addr                          			(1069 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_AF_OldSpkDataBufQ                     			(1070 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve03_addr                          			(1079 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ErrDataBufI                        			(1080 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve04_addr                          			(1089 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ErrDataBufQ                        			(1090 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve05_addr                          			(1099 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ShortFilterBufI                    			(1100 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define g_AF_ShortFilterBufQ                    			(1108 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define g_AF_tempCmplxBufI                      			(1116 + AEC_GLOBAL_X_BEGIN_ADDR) //8 word
#define g_AF_tempCmplxBufQ                      			(1132 + AEC_GLOBAL_X_BEGIN_ADDR) //8 word
#define r_AF_SpkSigPower                        			(1148 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_SpkSigSmoothPower                  			(1166 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_RErrSpkI                           			(1184 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_RErrSpkQ                           			(1202 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_AF_StepSize                           			(1220 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve06_addr                          			(1229 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_SmoothErrPower                     			(1230 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_NoisePowerEstBuf                   			(1248 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_NoiseFloorDevCnt                   			(1266 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define g_DTD_ch                                			(1275 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_PF_PostSToNRatio                      			(1276 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define CLChnl                                  			(1285 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_PrevSigPowerNBuf                   			(1286 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevSigPowerEBuf                   			(1304 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_NoiseWeight                        			(1322 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_EC_ErrToOutRatio                      			(1331 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_PF_NoiseAmpWeight                     			(1332 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define m_AF_FrameCnt                           			(1341 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define m_Taps                                  			(1342 + AEC_GLOBAL_X_BEGIN_ADDR) //4 short
#define m_Registers                             			(1346 + AEC_GLOBAL_X_BEGIN_ADDR) //32 short
#define TEMP_r_AF_SpkSigBufIQ_addr_addr         			(1378 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_FilterBufIQ_addr_addr         			(1380 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_AF_FilterOutBufIQ_addr_addr      			(1382 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_OldSpkDataBufIQ_addr_addr     			(1384 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_AF_ErrDataBufIQ_addr_addr        			(1386 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_MicAnaFilterOutBufIQ_addr_addr   			(1388 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_SpkAnaFilterOutBufIQ_addr_addr   			(1390 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_SpkSigPower_addr_addr         			(1392 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define g_counter_subframe                      			(1394 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_ch                                    			(1395 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define real_int                                			(1396 + AEC_GLOBAL_X_BEGIN_ADDR) //16 word
#define imag_int                                			(1428 + AEC_GLOBAL_X_BEGIN_ADDR) //16 word
#define AnaFilterCoef                           			(1460 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define SynFilterCoef                           			(1588 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define wnreal                                  			(1716 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define wnimag                                  			(1724 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define AdaptTable                              			(1732 + AEC_GLOBAL_X_BEGIN_ADDR) //32 short
#define r_PF_CL_MicSmoothPower                  			(1764 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_MicNoise                        			(1768 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_SpkSmoothPower                  			(1772 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_SpkNoise                        			(1776 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_MicNoiseFloorDevCnt             			(1780 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_SpkNoiseFloorDevCnt             			(1782 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ifMicSigOn                      			(1784 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ifSpkSigOn                      			(1786 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ChnlState                       			(1788 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_State                           			(1790 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_CL_DTCountDown                     			(1791 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_NoisePowerSmoothEstBuf             			(1792 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_PriorSToNRatio                     			(1810 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define g_PF_PriorSToERatio                     			(1819 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_SpkPowerBuf                        			(1828 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_CCErrSpkVecBufI                    			(1846 + AEC_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_PF_CCErrSpkVecBufQ                    			(1990 + AEC_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_PF_EchoPowerEstBuf                    			(2134 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_EchoWeight                         			(2152 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_CL_DTCountUp                       			(2161 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_CL_ErrSmoothPower                  			(2162 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_AF_FilterAmpI                         			(2166 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_AF_FilterAmpQ                         			(2175 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_PrevEchoPowerEst                   			(2184 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevSpkPower                       			(2202 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_CL_DTDXcorrMicErrI                 			(2220 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicErrQ                 			(2224 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicEchoI                			(2228 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicEchoQ                			(2232 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicSmoothPower               			(2236 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDErrSmoothPower               			(2240 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDEchoSmoothPower              			(2244 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicNoisePower                			(2248 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicNoiseFloorDevCnt          			(2252 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDDecision                     			(2256 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTD2ndDecision                  			(2260 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define pInstantSNR_t                           			(2264 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define pInstantSNR_f                           			(2265 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define SumMicPower                             			(2266 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define SumSpkPower                             			(2268 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define SumErrPower                             			(2270 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define PFCLDTDOut                              			(2272 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_NON_CLIP_AEC_SPK_HISTORYDATA     			(2274 + AEC_GLOBAL_X_BEGIN_ADDR) //160 short

/*****************
 **  AEC_LOCAL  **
 *****************/

/********************
 **  COMMON_LOCAL  **
 ********************/

//Coolsand_NonClipDigitalGain
#define Coolsand_NonClipDigitalGain_Y_BEGIN_ADDR			(0 + COMMON_LOCAL_Y_BEGIN_ADDR)
#define GLOBAL_NON_CLIP_CROSSZEROINDEX          			(0 + Coolsand_NonClipDigitalGain_Y_BEGIN_ADDR) //160 short

//RAM_X: size 0x2800, used 0x25ca	RAM_Y: size 0x2800, used 0x24b0

#endif
