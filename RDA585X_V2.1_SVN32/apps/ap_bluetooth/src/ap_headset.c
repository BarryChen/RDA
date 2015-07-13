
/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_common.h"
#include "ap_gui.h"

#if APP_SUPPORT_BLUETOOTH==1

#include "bt.h"
#include "hfp.h"
#include "a2dp.h"
#include "avrcp.h"
#include "obex.h"
#include "ap_headset.h"
#include "ap_media.h"
#include "ap_bluetooth.h"
#include "MainTask.h"
#include "mci.h"

#define BT_MAX_INPUT_NUMBER         48

extern UINT32 g_bt_ops_state;
extern UINT8 g_bt_msg[32];
extern BOOL ui_auto_update;
extern bt_state_t g_bt_state;
extern UINT8 g_bt_need_draw;
extern UINT8 g_bt_device_change;
extern INT8  g_bt_cur_device;
extern bt_vars_t *g_pBT_vars;
extern UINT8 g_bt_connect_pending;
extern INT32 g_current_module;
extern UINT32 g_bt_call_time;
#if APP_SUPPORT_BTHID==1
extern UINT8 g_hid_connect;
#endif
extern Phonebook_info *phonebook_info;
extern UINT8 *pb_addr;
extern Pb_Sort *pb_sort;
extern int pb_length;
extern UINT16 pb_index;
extern u_int32 *inputbuf_overlay;
#if XDL_APP_SUPPORT_TONE_TIP==1
extern UINT8 lowpower_flag_for_tone;
#endif
extern UINT8 pbap_buf;
extern BOOL music_playing;
extern UINT8 last_mode;
//extern BOOL play_start_cnf;

#if APP_SUPPORT_PBAP==1
BOOL download_abort = FALSE;
extern UINT32 pb_tid;
extern UINT32 pb_cid;
extern UINT32 pb_get_pos;
#endif

//add by wuxiang
#if APP_SUPPORT_OPP==1
extern u_int32 have_recv_file_size;
extern u_int32 recv_file_size;
#endif
extern UINT16 call_status;
//add end
extern UINT16 g_bt_sco_handle;

UINT8 bt_call_wait_volume = 0;
BOOL support_avrcp_status = FALSE;
UINT8 g_prev_module = 0;
t_a2dp_stream_config_ind g_bt_a2dp_config = {0};
t_bdaddr g_bt_a2dp_bdaddr;
UINT8 g_hfp_connect = 0;
UINT8 g_avdtp_connect = 0;
UINT8 g_avrcp_connect = 0;
UINT8 g_a2dp_play = 0;
UINT8 g_sco_play = 0;
UINT8 avrcp_play_pending = 0;
UINT8 g_avrcp_status = AVRCP_STATUS_ERROR;
BOOL g_music_playing = FALSE; //warkey 2.1
//UINT8 g_vol_set_by_ag = 0xff;
UINT16 call_status = 0;
#if APP_SUPPORT_NUMBER_VOICE==1
BOOL is_outgoing = FALSE;
#endif
UINT8 seid = 0;
UINT16 cid = 0;
INT32 wait_quit = 1;
#if APP_SUPPORT_BTBATTERY==1
INT8 is_iphone = 0;
INT8 g_cur_battery = 0;
#endif
#if XDL_APP_SUPPORT_TONE_TIP==1
INT8 vol_tone_flag = 0;
INT8 last_play_status = AVRCP_STATUS_STOPPED;
#endif

#if APP_SUPPORT_NUMBER_VOICE==1
#define BT_VOICE_NUMBER_COUNT        10

typedef struct tVoiceIdMapTag
{
	u_int8 VoiceNumber;
	u_int8 MediaId;
} tVoiceIdMap;

typedef enum
{
	ePlayVoice_Number,
	ePlayVoice_Call,
	ePlayVoice_None,
} tPlayVoiceType;

static const tVoiceIdMap VoiceIdMap[BT_VOICE_NUMBER_COUNT] =
{
	{'0', GUI_AUDIO_NUMBER_0,},
	{'1', GUI_AUDIO_NUMBER_1,},
	{'2', GUI_AUDIO_NUMBER_2,},
	{'3', GUI_AUDIO_NUMBER_3,},
	{'4', GUI_AUDIO_NUMBER_4,},
	{'5', GUI_AUDIO_NUMBER_5,},
	{'6', GUI_AUDIO_NUMBER_6,},
	{'7', GUI_AUDIO_NUMBER_7,},
	{'8', GUI_AUDIO_NUMBER_8,},
	{'9', GUI_AUDIO_NUMBER_9,},
};

static UINT8 gBTNumberPlayVioceMute = 0;
static void MuteTimeout(void *param)
{
	UINT8 *TimerId = (UINT8*)param;
	if (*TimerId)
	{
		COS_KillTimer(*TimerId);
		*TimerId = 0;
	}
	
	return;
}


static VOID BT_PlayVoiceForIncomingCallNumber(u_int8 VoiceNumber)
{
	u_int8 i = 0;
	u_int8 vPlayVoiceId = 0;
	
	if (!(VoiceNumber - '0' >= 0 && VoiceNumber - '0' <= 9))
	{
		return;
	}
	
	if (VoiceIdMap[VoiceNumber - '0'].VoiceNumber != VoiceNumber)
	{
		for (i = 0; i < BT_VOICE_NUMBER_COUNT; i++)
		{
			if (VoiceIdMap[i].VoiceNumber == VoiceNumber)
			{
				vPlayVoiceId = VoiceIdMap[i].MediaId;
				break;
			}
		}
	}
	else
	{
		vPlayVoiceId = VoiceIdMap[VoiceNumber - '0'].MediaId;
	}
	
	hal_HstSendEvent(SYS_EVENT, 0x11111100);
	hal_HstSendEvent(SYS_EVENT, vPlayVoiceId);
	media_PlayInternalAudio(vPlayVoiceId, 1, FALSE);
	
	return;
}
#endif


/* SBC configuration bit fields */
#define SBC_SAMPLING_FREQ_16000        128   /* Octet 0 */
#define SBC_SAMPLING_FREQ_32000         64
#define SBC_SAMPLING_FREQ_44100         32
#define SBC_SAMPLING_FREQ_48000         16
#define SBC_CHANNEL_MODE_MONO            8
#define SBC_CHANNEL_MODE_DUAL_CHAN       4
#define SBC_CHANNEL_MODE_STEREO          2
#define SBC_CHANNEL_MODE_JOINT_STEREO    1
#define SBC_BLOCK_LENGTH_4             128   /* Octet 1 */
#define SBC_BLOCK_LENGTH_8              64
#define SBC_BLOCK_LENGTH_12             32
#define SBC_BLOCK_LENGTH_16             16
#define SBC_SUBBANDS_4                   8
#define SBC_SUBBANDS_8                   4
#define SBC_ALLOCATION_SNR               2
#define SBC_ALLOCATION_LOUDNESS          1
#define SBC_BITPOOL_MIN                  2   /* Octet 2 (min bitpool)  /  Octet 3 (max bitpool) */
#define SBC_BITPOOL_MAX                250
#define SBC_BITPOOL_MEDIUM_QUALITY      32
#define SBC_BITPOOL_HIGH_QUALITY        50

static bt_a2dp_audio_codec_cap_struct codec_cap =
{
	{
		2, //min bit pool
		46,//0x32, //max bit pool
		0x1, //block len:16
		0x1, //subband num: 8,
		0x1, //alloc method: SNR.
		0x02, //sample rate:44.1K
		0x01  //channel mode:joint stereo
	}
};

static const u_int8 sbc_caps_sink[] =
{
	AVDTP_MEDIA_CODEC_SBC,
	
	SBC_SAMPLING_FREQ_16000     + SBC_SAMPLING_FREQ_32000    + SBC_SAMPLING_FREQ_44100    + SBC_SAMPLING_FREQ_48000    +
	SBC_CHANNEL_MODE_DUAL_CHAN + SBC_CHANNEL_MODE_STEREO    + SBC_CHANNEL_MODE_JOINT_STEREO,
	
	SBC_BLOCK_LENGTH_4          + SBC_BLOCK_LENGTH_8         + SBC_BLOCK_LENGTH_12        + SBC_BLOCK_LENGTH_16        +
	SBC_SUBBANDS_8             + SBC_ALLOCATION_SNR         + SBC_ALLOCATION_LOUDNESS,
	
	SBC_BITPOOL_MIN,
	46,
};


BOOL is_real_play = FALSE;
extern media_status_t g_media_status;
void BT_StopAudio(void)
{
	if(g_a2dp_play)
	{
		app_trace(APP_BT_TRC, "BT_StopAudio");
		app_trace(APP_BT_TRC, "support_avrcp_status:%d", support_avrcp_status);
		Audio_SBCStop();
		if(support_avrcp_status == 1)
		{
			hal_HstSendEvent(SYS_EVENT, 0x19880037);
			g_avrcp_status = AVRCP_STATUS_STOPPED;
		}
		app_trace(APP_BT_TRC, "BT_StopAudio");
		app_trace(APP_BT_TRC, "g_bt_state:%d", g_bt_state);
		app_trace(APP_BT_TRC, "g_avrcp_status:%d", g_avrcp_status);
		app_trace(APP_BT_TRC, "g_a2dp_play:%d", g_a2dp_play);
		app_trace(APP_BT_TRC, "support_avrcp_status:%d", support_avrcp_status);
		g_a2dp_play = 0;
		is_real_play = FALSE;
		hal_HstSendEvent(SYS_EVENT, 0x19880031);
		mediaSendCommand(MC_STOP, 0);
		Avdtp_Clean_Data();
	}
	
	return;
}


INT32 BT_InputNumber(void)
{
	UINT32 key;
	BOOL need_draw = FALSE;
	INT8 input_number[BT_MAX_INPUT_NUMBER + 2];
	UINT32 input_length = 0;
	
	MESSAGE_Initial(g_comval);
	memset(input_number, 0, sizeof(input_number));
	while(1)
	{
		if(input_length > BT_MAX_INPUT_NUMBER)
		{
			if(call_status == BT_CALL_STATUS_ACTIVE)
			{
				// remove fist number
				memcpy(input_number, input_number + 1, BT_MAX_INPUT_NUMBER);
			}
			input_length = BT_MAX_INPUT_NUMBER;
			input_number[input_length] = 0;
		}
		
		if (need_draw || g_bt_need_draw)
		{
			ui_auto_update = FALSE;
#if APP_SUPPORT_RGBLCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);
				BT_DisplayIcon(0, 0);
				GUI_DisplayBattaryLever();
				if(g_bt_state == BT_CALL && call_status == BT_CALL_STATUS_ACTIVE)
				{
					GUI_DisplayTimeText(40, 0, GUI_TIME_DISPLAY_AUTO, (g_bt_call_time / 2) * 1000);
				}
				GUI_DisplayText(0, 16, input_number);
				if(input_length > 16)
				{
					GUI_DisplayText(0, 32, input_number + 16);
				}
				if(input_length > 32)
				{
					GUI_DisplayText(0, 48, input_number + 32);
				}
				GUI_UpdateScreen(NULL);
			}
#elif APP_SUPPORT_LCD==1
			if(AP_Support_LCD())
			{
				GUI_ClearScreen(NULL);
				BT_DisplayIcon(0, 0);
				GUI_DisplayBattaryLever();
				if(g_bt_state == BT_CALL && call_status == BT_CALL_STATUS_ACTIVE)
				{
					GUI_DisplayTimeText(40, 0, GUI_TIME_DISPLAY_AUTO, (g_bt_call_time / 2) * 1000);
				}
				GUI_DisplayText(0, 16, input_number);
				if(input_length > 16)
				{
					GUI_DisplayText(0, 32, input_number + 16);
				}
				if(input_length > 32)
				{
					GUI_DisplayText(0, 48, input_number + 32);
				}
				GUI_UpdateScreen(NULL);
			}
#elif APP_SUPPORT_LED8S==1
			{
				if(input_length > 4)
				{
					GUI_DisplayText(0, 0, input_number + input_length - 4);
				}
				else
				{
					GUI_DisplayText(0, 0, input_number);
				}
				GUI_UpdateScreen(NULL);
			}
#endif
			ui_auto_update = TRUE;
			need_draw = FALSE;
		}
		
		key =  MESSAGE_Wait();
		
		if((key & 0xffff) == AP_KEY_PRESS || (key & 0xffff) == AP_KEY_HOLD)
		{
			switch(key & 0xffff0000)
			{
			case AP_KEY_STAR:
				input_number[input_length] = '*';
				if(call_status == BT_CALL_STATUS_ACTIVE)
				{
					HF_Send_DTMF(input_number + input_length);
				}
				input_length++;
				need_draw = TRUE;
				break;
				
			case AP_KEY_HASH:
				input_number[input_length] = '#';
				if(call_status == BT_CALL_STATUS_ACTIVE)
				{
					HF_Send_DTMF(input_number + input_length);
				}
				input_length++;
				need_draw = TRUE;
				break;
				
			case AP_KEY_NUM0:
				input_number[input_length] = '0';
				need_draw = TRUE;
				if(call_status == BT_CALL_STATUS_ACTIVE)
				{
					HF_Send_DTMF(input_number + input_length);
				}
				input_length++;
				break;
				
			case AP_KEY_MODE:
				return 0;
				break;
				
			case AP_KEY_PLAY:
				if(call_status == BT_CALL_STATUS_NONE)
				{
					HF_Set_Number(input_number);
					HF_Call_Request(HF_CALL_NUM, 0);
				}
				return 0;
				break;
				
			case AP_KEY_PREV: // remove last number
				if(call_status != BT_CALL_STATUS_ACTIVE && input_length > 0)
				{
					input_number[--input_length] = 0;
					need_draw = TRUE;
				}
				break;
				
			default:
				if((key & 0xffff0000) <= AP_KEY_NUM9)
				{
					input_number[input_length] = (key >> 16) + '0';
					if(call_status == BT_CALL_STATUS_ACTIVE)
					{
						HF_Send_DTMF(input_number + input_length);
					}
					input_length++;
					need_draw = TRUE;
				}
				break;
			}
		}
		else
		{
			key = BT_HandleKey(key);
			if(key != 0)
			{
				return key;
			}
		}
	}
	
	return 0;
}


#if APP_SUPPORT_NUMBER_VOICE==1
tPlayVoiceType vVoiceType = ePlayVoice_None;
#endif
UINT8 reject_call_timer = 0;
void reject_call_timeout(void *param)
{
	hal_HstSendEvent(SYS_EVENT, 0x19880926);
	hal_HstSendEvent(SYS_EVENT, call_status);
	if(call_status != BT_CALL_STATUS_END && call_status != BT_CALL_STATUS_NONE)
	{
		HF_Set_Call_Accept();
		HF_Call_Answer(HF_CALL_REJECT);
	}
}


INT32 BT_Call(void)
{
	UINT32 key;
	INT32  result;
	INT32 wait_volume = 2;
	char number[20] = {0};
	UINT32 i = 0;
	u_int8 length = 0;
	COS_EVENT ev = {0};
	t_bdaddr address;
	
#if APP_SUPPORT_NUMBER_VOICE==1
	vVoiceType = ePlayVoice_Call;
	u_int8 vCurrentPlayNumberIndex = 0;
	UINT8 vVoicePlayTimer = 0;
	u_int8 vPhoneNumber[30] = {0};
	u_int8 vNumberLength = 0;
	bool play_number_voice_flag = FALSE; //warkey 2.1
	if(g_bt_ops_state & BT_OPS_CALL_STATUS_OUTGOING)
	{
		vVoiceType = ePlayVoice_None;
		clean_bt_ops_state(BT_OPS_CALL_STATUS_OUTGOING);
	}
#endif
	gpio_SetMute(TRUE);//warkey 2.0
	app_trace(APP_BT_TRC, "enter BT_Call");
	
	g_bt_state     = BT_CALL;
	g_bt_need_draw = TRUE;
	
	MESSAGE_Initial(g_comval);
	
	while(1)
	{
		if(call_status == BT_CALL_STATUS_NONE || call_status == BT_CALL_STATUS_END)
		{
			if(g_bt_state>= BT_CONNECTED&&g_bt_sco_handle)
			{
				MGR_ReleaseSynchronous(g_bt_sco_handle);
				mediaSendCommand(MC_STOP, 0);
				g_sco_play = 0;
			}
			hal_HstSendEvent(SYS_EVENT, 0x19884800);
			hal_HstSendEvent(SYS_EVENT, call_status);
			hal_HstSendEvent(SYS_EVENT, g_bt_state);
			if(g_bt_state >= BT_CONNECTED)
			{
				if(g_bt_state == BT_CONNECTED)
				{
					MESSAGE_SetEvent(EV_BT_CONNECTED_IND);
				}
				else if(g_bt_state == BT_CALL)
				{
					MESSAGE_SetEvent(EV_BT_CALL_IND);
				}
				else if(g_bt_state == BT_PLAY)
				{
					MESSAGE_SetEvent(EV_BT_PLAY_IND);
				}
				else if(g_bt_state == BT_FILE)
				{
					MESSAGE_SetEvent(EV_BT_FILE_RSV_IND);
				}
				return RESULT_BT_ACTIVE;
			}
			break;
		}
		//        HF_Get_Number(number, &length);
		//        app_trace(APP_BT_TRC , "length = %d, number = %s", length, number);
		g_bt_state     = BT_CALL;
		
#if APP_SUPPORT_NUMBER_VOICE==1
		if(/*call_status == BT_CALL_STATUS_ACTIVE&&*/g_bt_sco_handle && g_sco_play == 0 && vVoiceType == ePlayVoice_None)
#else
		if(/*call_status == BT_CALL_STATUS_ACTIVE&&*/g_bt_sco_handle && g_sco_play == 0)
#endif
		{
			hal_HstSendEvent(SYS_EVENT, 0x96009600);
			gpio_SetMute(TRUE);
			mediaSendCommand(MC_PLAY_SCO, g_bt_sco_handle);
#if APP_SUPPORT_DELAY_VOLUME==1
			SetInternalPAVolume(0); // wait 1 second to open pa
			wait_volume = 2;
#else
			
			if(g_pBT_vars->volume > MAX_BT_CALL_VOL_LEVEL) //warkey 2.1
			{
				SetPAVolume(MAX_BT_CALL_VOL_LEVEL);
			}
			else
			{
				SetPAVolume(g_pBT_vars->volume);
			}
#endif
			
			if(g_pBT_vars->volume == 0)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19880080);
			}
			else
			{
				hal_HstSendEvent(SYS_EVENT, 0x19880081);
			}
			g_sco_play = 1;
			while(BTSco_GetStatus());
		}
		
		if(g_bt_need_draw)
		{
			BT_DisplayMain();
			g_bt_need_draw = FALSE;
		}
		
		key = MESSAGE_Wait();
		
		if(call_status == BT_CALL_STATUS_ACTIVE && (MESSAGE_IsNumberKey(key) || key == (AP_KEY_DOWN | AP_KEY_STAR) || key == (AP_KEY_DOWN | AP_KEY_HASH)))
		{
			result = BT_InputNumber();
			if(result != 0)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19884801);
				hal_HstSendEvent(SYS_EVENT, call_status);
				hal_HstSendEvent(SYS_EVENT, key);
				goto Endcall;
			}
			g_bt_need_draw = 1;
			continue;
		}
		
		switch(key)
		{
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				g_bt_need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
			//g_bt_need_draw=TRUE;
			if(g_bt_state == BT_CALL && call_status == BT_CALL_STATUS_ACTIVE)
			{
				g_bt_call_time++;
			}
#if APP_SUPPORT_DELAY_VOLUME==1
			if(wait_volume > 0)
			{
				wait_volume --;
				if(wait_volume == 0)
				{
					if(!GetToneStatus())
					{
						if(g_pBT_vars->volume > MAX_BT_CALL_VOL_LEVEL) //warkey 2.1
						{
							SetInternalPAVolume(MAX_BT_CALL_VOL_LEVEL);
						}
						else
						{
							SetInternalPAVolume(g_pBT_vars->volume);
						}
						
						if(g_pBT_vars->volume)
						{
							gpio_SetMute(FALSE);
						}
						else
						{
							gpio_SetMute(TRUE);
						}
					}
					else
					{
						wait_volume++;
					}
				}
			}
			
#if 1  /* shining */
			if(bt_call_wait_volume > 0)
			{
				bt_call_wait_volume --;
				if(bt_call_wait_volume == 0)
				{
					if(g_pBT_vars->volume > MAX_BT_CALL_VOL_LEVEL) //warkey 2.1
					{
						SetInternalPAVolume(MAX_BT_CALL_VOL_LEVEL);
					}
					else
					{
						SetInternalPAVolume(g_pBT_vars->volume);
					}
					
					if(g_pBT_vars->volume)
					{
						gpio_SetMute(FALSE);
					}
					else
					{
						gpio_SetMute(TRUE);
					}
				}
			}
#endif
#endif
			
#if APP_SUPPORT_NUMBER_VOICE==1
			app_trace(APP_BT_TRC, "vVoiceType %d, vNumberLength :%d", vVoiceType, vNumberLength);
			app_trace(APP_BT_TRC, "call_status %d, vVoicePlayTimer :%d", call_status, vVoicePlayTimer);
			//if (vVoicePlayTimer++ % 2 == 0)
			{
				if (call_status == BT_CALL_STATUS_ACTIVE)
				{
					vVoiceType = ePlayVoice_None;
					play_number_voice_flag = FALSE; //warkey 2.1
					vCurrentPlayNumberIndex = 0;
				}
				else if (vVoiceType == ePlayVoice_Call)
				{
					vVoiceType = ePlayVoice_Number;
					memset(vPhoneNumber, 0, sizeof(vPhoneNumber));
					HF_Get_Number(vPhoneNumber, &vNumberLength);
					MESSAGE_Sleep(2);
				}
				else if (vVoiceType == ePlayVoice_Number && call_status == BT_CALL_STATUS_INCOMING)
				{
					if(play_number_voice_flag == TRUE) //warkey 2.1
					{
						break;
					}
					media_StopInternalAudio();
#if 0
					if (vNumberLength > 0)
					{
						if (vCurrentPlayNumberIndex < vNumberLength)
						{
							BT_PlayVoiceForIncomingCallNumber(vPhoneNumber[vCurrentPlayNumberIndex]);
							vCurrentPlayNumberIndex++;
						}
						else if(vCurrentPlayNumberIndex >= vNumberLength)                 //Delay 1s for play the last phone number voice.
						{
							vCurrentPlayNumberIndex = 0;
							vVoiceType = ePlayVoice_None;
							media_StopInternalAudio();
						}
					}
					else
					{
						HF_Get_Clcc();
						HF_Get_Number(vPhoneNumber, &vNumberLength);
					}
#else//warkey 2.1
					if (vNumberLength <= 0)
					{
						HF_Get_Clcc();
						HF_Get_Number(vPhoneNumber, &vNumberLength);
					}
					else
					{
						play_number_voice_flag = TRUE;
						//gpio_SetMute(TRUE);
						vCurrentPlayNumberIndex = 0;
						BT_PlayVoiceForIncomingCallNumber(vPhoneNumber[vCurrentPlayNumberIndex]);
						vCurrentPlayNumberIndex++;
						hal_HstSendEvent(APP_EVENT, 0x13110501);
					}
#endif
				}
			}
#endif
			break;
			
#if APP_SUPPORT_NUMBER_VOICE==1//warkey 2.1
		case EV_BT_HUMAN_VOICE_PLAY:
			if(play_number_voice_flag == TRUE && vVoiceType == ePlayVoice_Number)
			{
				if (vCurrentPlayNumberIndex < vNumberLength)
				{
					BT_PlayVoiceForIncomingCallNumber(vPhoneNumber[vCurrentPlayNumberIndex]);
					vCurrentPlayNumberIndex++;
					hal_HstSendEvent(APP_EVENT, 0x13110502);
				}
				else
				{
					play_number_voice_flag = FALSE;
					vCurrentPlayNumberIndex = 0;
					vVoiceType = ePlayVoice_None;
					media_StopInternalAudio();
					hal_HstSendEvent(APP_EVENT, 0x13110503);
				}
			}
			break;
#endif
			
		case 0xaaaa1111:	// xundon
		
			break;
			
		case AP_MSG_STANDBY:
			//do nothing
			break;
			
		case AP_KEY_PLAY|AP_KEY_PRESS:
			hal_HstSendEvent(SYS_EVENT, 0x19885600);
			hal_HstSendEvent(SYS_EVENT, call_status);
			if(HF_Get_Current_Profile() == ACTIVE_PROFILE_HEADSET)
			{
				HF_Send_Button_Press();
			}
			else if(call_status == BT_CALL_STATUS_ACTIVE)
			{
				HF_Call_Release();
			}
			else if(call_status == BT_CALL_STATUS_INCOMING)
			{
				media_StopInternalAudio();
				hal_HstSendEvent(SYS_EVENT, 0x19885601);
				HF_Call_Answer(HF_CALL_ACCEPT);
			}
			else if(call_status == BT_CALL_STATUS_OUTGOING || call_status == BT_CALL_STATUS_ALERT)
			{
				HF_Call_Release();
			}
			break;
			
		case AP_KEY_PLAY|AP_KEY_HOLD:
			if(MESSAGE_IsHold())
			{
				hal_HstSendEvent(SYS_EVENT, 0x1988aaaa);
				hal_HstSendEvent(SYS_EVENT, call_status);
				if(call_status == BT_CALL_STATUS_INCOMING)
				{
					reject_call_timer = COS_SetTimer(1000, reject_call_timeout, NULL, COS_TIMER_MODE_SINGLE);
					HF_Call_Answer(HF_CALL_REJECT);
				}
				else
				{
					HF_Call_Release();
				}
				
				media_StopInternalAudio();
			}
			break;
			
		case AP_KEY_NEXT|AP_KEY_PRESS:
		case AP_KEY_PREV|AP_KEY_PRESS:
			hal_HstSendEvent(SYS_EVENT, 0x19882005);
			hal_HstSendEvent(SYS_EVENT, g_bt_sco_handle);
			if(g_bt_sco_handle)
			{
				MGR_ReleaseSynchronous(g_bt_sco_handle);
			}
			else
			{
				t_bdaddr address;
				HF_Get_Current_BdAddr(&address);
				MGR_SetupSynchronous(address);
			}
			break;
			
		default:                                        //热键处理
			result = BT_HandleKey(key);
			if(key != 0)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19882000);
				hal_HstSendEvent(SYS_EVENT, key);
				hal_HstSendEvent(SYS_EVENT, result);
			}
			
			if(result != 0)
			{
Endcall:
				if(result == RESULT_BT_ACTIVE)
				{
					key = MESSAGE_Wait();
					hal_HstSendEvent(SYS_EVENT, 0x19880022);
					hal_HstSendEvent(SYS_EVENT, key);
					if((key == EV_BT_PLAY_IND && g_bt_sco_handle) || key == EV_BT_CALL_IND)
					{
						break;
					}
				}
				hal_HstSendEvent(SYS_EVENT, 0x19880023);
				hal_HstSendEvent(SYS_EVENT, key);
				if(result == RESULT_BT_ACTIVE)
				{
					MESSAGE_SetEvent(key);
				}
				else
				{
					break;
				}
				return result;
			}
		}
	}
	
	return 0;
}


extern UINT32 event_detected_bt_pending;
extern UINT32 event_detected_displaying_log;
UINT32 old_phone_timer_handle = 0;
BOOL short_play_status = TRUE;
UINT8 retry_count = 0;

void BT_Play_Timeout(void *param)
{
	COS_KillTimer(old_phone_timer_handle);
	old_phone_timer_handle = 0;
	hal_HstSendEvent(SYS_EVENT, 0x1988cf02);
	hal_HstSendEvent(SYS_EVENT, is_real_play);
	hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
	if(!is_real_play && g_a2dp_play && retry_count < 1)
	{
		hal_HstSendEvent(SYS_EVENT, 0x1988cf01);
		hal_HstSendEvent(SYS_EVENT, g_pBT_vars->volume);
		{
			Avrcp_Send_Key(OPID_PLAY);
			short_play_status = TRUE;
			retry_count++;
		}
		old_phone_timer_handle = COS_SetTimer(2000, BT_Play_Timeout, NULL, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		short_play_status = TRUE;
		retry_count = 0;
	}
	
	return;
}


void recall_timeout(void *param)
{
	hal_HstSendEvent(SYS_EVENT, 0xccccbbbb);
	clean_bt_ops_state(BT_OPS_RECALL);
	
	return;
}


INT32 BT_MusicPlay(void)
{
	UINT32 key;
	UINT32 status = 0;
	INT32 result;
	INT32 wait_volume = 0;
	INT32 fast_forward = 0;
	INT32 time_val = 0;
	u_int32 sbc_length;
	
	app_trace(APP_BT_TRC, "enter BT_MusicPlay");
	
	g_bt_state = BT_PLAY;
	g_bt_need_draw = TRUE;
	MESSAGE_Initial(g_comval);
	//gpio_SetMute(FALSE);//warkey 2.0
	if(g_avrcp_connect == 0)
	{
		Avrcp_Connect_Req(g_bt_a2dp_bdaddr);
	}
	
	hal_HstSendEvent(SYS_EVENT, 0x1988cc00);
	
	if((g_bt_ops_state & BT_OPS_MODULE_CHANGE) && (g_music_playing == FALSE))
	{
		if(!g_bt_sco_handle)
		{
			hal_HstSendEvent(APP_EVENT, 0x13120607);
			clean_bt_ops_state(BT_OPS_MODULE_CHANGE);
			if(g_prev_module == FUN_FM)
			{
				g_prev_module = 0;
				return RESULT_RADIO;
			}
			else if(g_prev_module == FUN_MUSIC)
			{
				g_prev_module = 0;
				return RESULT_MUSIC;
			}
			else if(g_prev_module == FUN_LINEIN)
			{
				g_prev_module = 0;
				return RESULT_LINE_IN;
			}
		}
		else
		{
			hal_HstSendEvent(APP_EVENT, 0x13120608);
		}
	}
	
	while(1)
	{
		if(g_bt_ops_state & BT_OPS_RECALL)
		{
			break;
		}
	#if 0//phone_tone_play_return	
		if(g_bt_ops_state & BT_OPS_MODULE_CHANGE)//warkey 2.1
		{
			if(support_avrcp_status && g_avrcp_status != AVRCP_STATUS_PLAYING && wait_volume == 0)
			{
				hal_HstSendEvent(APP_EVENT, 0x13121002);
				hal_HstSendEvent(APP_EVENT, g_prev_module);
				BT_StopAudio();
				g_bt_state = BT_CONNECTED;
				if(g_music_playing == TRUE)
				{
					g_music_playing = FALSE;
				}
				clean_bt_ops_state(BT_OPS_MODULE_CHANGE);
				if(g_prev_module == FUN_FM)
				{
					g_prev_module = 0;
					return RESULT_RADIO;
				}
				else if(g_prev_module == FUN_MUSIC)
				{
					g_prev_module = 0;
					return RESULT_MUSIC;
				}
				else if(g_prev_module == FUN_LINEIN)
				{
					g_prev_module = 0;
					return RESULT_LINE_IN;
				}
			}
		}
	#endif
		
		if(g_bt_state != BT_PLAY)
		{
			is_real_play = FALSE;
			if(g_bt_state < BT_CONNECTED)
			{
				return 0;
			}
			
			if(wait_quit == 0)
			{
				hal_HstSendEvent(SYS_EVENT, 0x1988c005);
				hal_HstSendEvent(SYS_EVENT, g_sco_play);
				if(g_a2dp_play && !(g_bt_ops_state & BT_OPS_MUSIC_PLAY) && (g_avrcp_status == AVRCP_STATUS_PLAYING || !support_avrcp_status))
				{
					hal_HstSendEvent(SYS_EVENT, 0x1988c104);
					//Avrcp_Send_Key(OPID_PAUSE);
					//short_play_status = FALSE;
					BT_StopAudio();
					//COS_Sleep(600);
					//is_play_music = TRUE;
				}
				
				if(g_sco_play)
				{
					mediaSendCommand(MC_STOP, 0);
					g_sco_play = 0;
				}
				//g_bt_need_draw = TRUE;
				wait_quit = 10; // wait 5second to return
			}
		}
#if XDL_APP_SUPPORT_TONE_TIP==1
		else if(g_a2dp_play == 0 && vol_tone_flag != 1)
#else
		else if(g_a2dp_play == 0)
#endif
		{
			g_a2dp_play = 1;
			hal_HstSendEvent(SYS_EVENT, 0x1988c00c);
			hal_HstSendEvent(SYS_EVENT, Get_Sep_State());
			//if(2 != Get_Sep_State())
			{
				hal_HstSendEvent(SYS_EVENT, 0x1988c00d);
				Set_Sep_State_Playing();
				//COS_Sleep(500);
			}
			
			if(!mediaSendCommand(MC_PLAY_A2DP, &g_bt_a2dp_config.audio_cap))
			{
				g_a2dp_play = 0;
				hal_HstSendEvent(SYS_EVENT, 0x1988c007);
				set_bt_ops_state(BT_OPS_MUSIC_PLAY_FAILED);
				break;
			}
			else
			{
				short_play_status = TRUE;
				hal_HstSendEvent(SYS_EVENT, 0x1988c006);
#if APP_SUPPORT_DELAY_VOLUME==1
				//SetPAVolume(0); // wait 2 second to open pa
				wait_volume = 4;
#else
				gpio_SetMute(FALSE);//added by gary    iphone 4s 灭屏再开锁，按播放后无声。
				SetPAVolume(g_pBT_vars->volume);
#endif
				//if(g_avrcp_status == AVRCP_STATUS_ERROR)
				//    g_avrcp_status = AVRCP_STATUS_PLAYING;
				wait_quit = 0;
				hal_HstSendEvent(SYS_EVENT, 0x1988c009);
				clean_bt_ops_state(BT_OPS_MUSIC_PLAY);
				//old_phone_timer_handle = COS_SetTimer(2000, BT_Play_Timeout, NULL, COS_TIMER_MODE_SINGLE);
			}
		}
		
#if XDL_APP_SUPPORT_TONE_TIP==1
		if(vol_tone_flag == 2)
		{
			vol_tone_flag = 0;
			if(last_play_status == AVRCP_STATUS_PLAYING)
			{
				last_play_status = AVRCP_STATUS_STOPPED;
				//short_pause = FALSE;
				hal_HstSendEvent(SYS_EVENT, 0x77772211);
				//gpio_SetMute(FALSE);
				Avrcp_Send_Key(OPID_PLAY);
				wait_volume = 2;
			}
		}
#endif
		
		if(g_bt_need_draw)
		{
			BT_DisplayMain();
			g_bt_need_draw = FALSE;
		}
		
		key = MESSAGE_Wait();
		
		switch(key)
		{
		case AP_MSG_STANDBY:
			//do nothing
			break;
			
		case AP_MSG_WAIT_TIMEOUT:
			if(wait_quit > 0)
			{
				wait_quit = 1; // pass down for return
			}
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				g_bt_need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
#if XDL_APP_SUPPORT_TONE_TIP==1
			if(vol_tone_flag == 1)
			{
				vol_tone_flag = 2;
				BT_StopAudio();
				if(lowpower_flag_for_tone == 1)
				{
#if APP_SUPPORT_BTBATTERY==1
					if(g_bt_state >= BT_CONNECTED && g_hfp_connect)
					{
						BT_HFPSendBattery();
					}
#endif
					media_PlayInternalAudio(GUI_AUDIO_LOWPOWER, 1, FALSE);
				}
				else if(lowpower_flag_for_tone == 2)
				{
					media_PlayInternalAudio(GUI_AUDIO_CHARGE_COMPLETE, 1, FALSE);
				}
				else if(lowpower_flag_for_tone == 4)
				{
					media_PlayInternalAudio(GUI_AUDIO_BT_POWEROFF, 1, FALSE);
				}
				else if(lowpower_flag_for_tone==5)
				{
					media_PlayInternalAudio(GUI_AUDIO_BT_CONNECT, 1, FALSE);
				}
				else
				{
					media_PlayInternalAudio(GUI_AUDIO_ALARM1, 1, FALSE);
				}
				while(GetToneStatus())
				{
					MESSAGE_Sleep_EXT(1);
				}
				if(lowpower_flag_for_tone == 4)
				{
					last_play_status = AVRCP_STATUS_PAUSED;
					MESSAGE_SetEvent(0xad11dead);
				}
			}
#endif
			
			//g_bt_need_draw=TRUE;
			if(wait_quit > 0)
			{
				wait_quit --;
				if(wait_quit == 0)
				{
					g_bt_need_draw = TRUE;
					return 0;
				}
			}
			
#if APP_SUPPORT_DELAY_VOLUME==1
			if(wait_volume > 0)
			{
				wait_volume --;
				if(wait_volume == 0)
				{
					app_trace(5, ">>>>>>>>>>>>wait_volume!");
					SetPAVolume(g_pBT_vars->volume);
					if(g_pBT_vars->volume == 0)
					{
						gpio_SetMute(TRUE);
					}
					else
					{
						gpio_SetMute(FALSE);
					}
				}
			}
#endif
			
			break;
			
#if XDL_APP_SUPPORT_TONE_TIP==1
		case 0xaaaa1111:	// xundon
			hal_HstSendEvent(SYS_EVENT, 0x77772200);
			hal_HstSendEvent(SYS_EVENT, vol_tone_flag);
			if(vol_tone_flag == 0)
			{
				vol_tone_flag = 1;
				
				if((!support_avrcp_status && g_a2dp_play) || g_avrcp_status == AVRCP_STATUS_PLAYING && g_a2dp_play)
				{
					Avrcp_Send_Key(OPID_PAUSE);
					//short_pause = TRUE;
					last_play_status = AVRCP_STATUS_PLAYING;
				}
				else
				{
					last_play_status = AVRCP_STATUS_PAUSED;
				}
				BT_StopAudio();
			}
			break;
#endif
			
		case AP_KEY_PLAY|AP_KEY_PRESS:
			hal_HstSendEvent(SYS_EVENT, 0x19884900);
			hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
			hal_HstSendEvent(SYS_EVENT, support_avrcp_status);
			hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
			hal_HstSendEvent(SYS_EVENT, short_play_status);
			
			if((!support_avrcp_status && g_a2dp_play) || g_avrcp_status == AVRCP_STATUS_PLAYING && g_a2dp_play)
			{
				if(short_play_status)
				{
					Avrcp_Send_Key(OPID_PAUSE);
					short_play_status = FALSE;
				}
				else
				{
					Avrcp_Send_Key(OPID_PLAY);
					short_play_status = TRUE;
				}
				
				//    return 0;
				//    g_avrcp_status = AVRCP_STATUS_PAUSED;
			}
			else
			{
				if(short_play_status)
				{
					Avrcp_Send_Key(OPID_PAUSE);
					short_play_status = FALSE;
				}
				else
				{
					hal_HstSendEvent(SYS_EVENT, 0x1988fd00);
					Avrcp_Send_Key(OPID_PLAY);
					short_play_status = TRUE;
				}
				//    g_avrcp_status = AVRCP_STATUS_PLAYING;
			}
			break;
			
		case AP_KEY_PLAY|AP_KEY_DOUBLE:
			hal_HstSendEvent(SYS_EVENT, 0xaaaaaaaa);
#if !defined(NOT_SUPPORT_HFP)
			//if(MESSAGE_IsHold())
			{
				//is_play_music = TRUE;
				//Avrcp_Send_Key(OPID_PAUSE);
				//g_a2dp_play = 0;
				BT_StopAudio();
				//COS_Sleep(500);
				hal_HstSendEvent(SYS_EVENT, 0xbbbbbbbb);
				HF_Call_Request(HF_CALL_REDIAL, 0);
				set_bt_ops_state(BT_OPS_RECALL);
				COS_SetTimer(1000, recall_timeout, NULL, COS_TIMER_MODE_SINGLE);
			}
#endif
			break;
			
		case AP_KEY_NEXT|AP_KEY_PRESS:
			Avrcp_Send_Key(OPID_FORWARD);
			break;
			
		case AP_KEY_PREV|AP_KEY_PRESS:
			Avrcp_Send_Key(OPID_BACKWARD);
			break;
			
		case AP_KEY_NEXT|AP_KEY_HOLD:
			if(fast_forward == 0)
			{
				Avrcp_Send_Press(OPID_FAST_FORWARD);
				fast_forward = 1;
			}
			break;
			
		case AP_KEY_PREV|AP_KEY_HOLD:
			if(fast_forward == 0)
			{
				Avrcp_Send_Press(OPID_REWIND);
				fast_forward = -1;
			}
			break;
			
		case AP_KEY_NEXT|AP_KEY_UP:
			if(fast_forward)
			{
				Avrcp_Send_Release(OPID_FAST_FORWARD);
			}
			fast_forward = 0;
			break;
			
		case AP_KEY_PREV|AP_KEY_UP:
			if(fast_forward)
			{
				Avrcp_Send_Release(OPID_REWIND);
			}
			fast_forward = 0;
			break;
			
		case AP_KEY_EQ | AP_KEY_PRESS:
			{
				g_pBT_vars->eq_type = (g_pBT_vars->eq_type + 1) % 8;
				mediaSendCommand(MC_SETEQ, g_pBT_vars->eq_type);
			}
			break;
			
		default:                                        //热键处理
			result = BT_HandleKey(key);
			if(result != 0)
			{
				if(result == RESULT_BT_ACTIVE)
				{
					key = MESSAGE_Wait();
				}
				if(result == RESULT_BT_ACTIVE && key == EV_BT_A2DP_PLAY_IND)
				{
					break;
				}
				//GUI_DisplayMessage(0, GUI_STR_WAITPASS, NULL, GUI_MSG_FLAG_DISPLAY);
				if(g_a2dp_play)
				{
					hal_HstSendEvent(SYS_EVENT, 0x1988c004);
                        Avrcp_Send_Key(OPID_PAUSE);
                        //Avdtp_Send_Suspend();
                        BT_StopAudio();
                        //COS_Sleep(600);
                        //is_play_music = TRUE;
                        short_play_status = FALSE;
					//    play_start_cnf = TRUE;
					//while(g_bt_state == BT_PLAY && g_a2dp_play)
					//    MESSAGE_Wait();
				}
				
				if(g_sco_play)
				{
					mediaSendCommand(MC_STOP, 0);
					g_sco_play = 0;
				}
				
				if(result == RESULT_BT_ACTIVE)
				{
					MESSAGE_SetEvent(key);
				}
				hal_HstSendEvent(SYS_EVENT, 0x19870006);
				hal_HstSendEvent(SYS_EVENT, result);
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)		
				{
					g_bt_state = BT_CONNECTED;
				}
				else 
				{
					g_bt_state = BT_IDLE;
				}
				//support_avrcp_status = FALSE;
				if(g_music_playing == TRUE) //warkey 2.1
				{
					g_music_playing = FALSE;
				}
				return result;
			}
		}
	};
	if(g_music_playing == TRUE) //warkey 2.1
	{
		g_music_playing = FALSE;
	}
	
	return 0;
}


extern UINT8 media_GetInternalStatus();
BOOL play_in_noconnect = FALSE;
UINT8 bt_connect_reminder = 0;
#ifndef WIN32
UINT8 uart2_send_data_timer = 0;
UINT8 uart2_send_buff[255] = {"abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij"};

void uart2_send_data_func(void *param)
{
	uart2_send_data(uart2_send_buff, 100);
	COS_KillTimer(uart2_send_data_timer);
	uart2_send_data_timer = 0;
	uart2_send_data_timer = COS_SetTimer(1000, uart2_send_data_func, NULL, COS_TIMER_MODE_SINGLE);
	
	return;
}
#endif

#if APP_SUPPORT_MAP==1
extern void common_var_init();
extern void BT_Map();
#endif

INT32 BT_Connect(void)
{
	UINT32 key;
	INT32 result;
	INT32 wait_volume = 0;
	app_trace(APP_BT_TRC, "enter BT_Connect");

	g_bt_need_draw = TRUE;
	MESSAGE_Initial(g_comval);
	Set_Sep_State_Playing();
	
#if APP_SUPPORT_MENU==0
	if(g_pBT_vars->default_device != g_bt_cur_device)
	{
		g_pBT_vars->default_device = g_bt_cur_device;
		NVRAMWriteData();
	}
#endif
	gpio_SetMute(TRUE);//warkey 2.0
	hal_HstSendEvent(SYS_EVENT, 0x13070501);
	hal_HstSendEvent(SYS_EVENT, media_GetInternalStatus());
	
	hal_HstSendEvent(SYS_EVENT, 0x19884800);
	hal_HstSendEvent(SYS_EVENT, g_hfp_connect);
	hal_HstSendEvent(SYS_EVENT, g_avdtp_connect);
#if APP_SUPPORT_MAP==1	
	if(g_bt_ops_state & BT_OPS_MAP_MSG_REQ)
        {
            clean_bt_ops_state(BT_OPS_MAP_MSG_REQ);
            common_var_init();
            COS_Sleep(100);
            BT_Map();
        }
#endif
	/*
	if(g_hfp_connect == 0)
	{
		BT_Connect_Handsfree(g_bt_cur_device);
	}
	if(g_avdtp_connect == 0)
	{
		BT_Connect_A2DP(g_bt_cur_device);
	}
	*/
	
	while(1)
	{
		if(g_bt_ops_state & BT_OPS_MODULE_CHANGE)//warkey 2.1
		{
			if(!g_bt_sco_handle)
			{
				hal_HstSendEvent(APP_EVENT, 0x13120609);
				clean_bt_ops_state(BT_OPS_MODULE_CHANGE);
				if(g_prev_module == FUN_FM)
				{
					g_prev_module = 0;
					return RESULT_RADIO;
				}
				else if(g_prev_module == FUN_MUSIC)
				{
					g_prev_module = 0;
					return RESULT_MUSIC;
				}
				else if(g_prev_module == FUN_LINEIN)
				{
					g_prev_module = 0;
					return RESULT_LINE_IN;
				}
			}
			else
			{
				hal_HstSendEvent(APP_EVENT, 0x13120610);
			}
		}
		//        hal_HstSendEvent(SYS_EVENT, 0x1306250e);
		//        hal_HstSendEvent(SYS_EVENT, g_bt_state);
		
		if(g_bt_state != BT_CONNECTED)
		{
			hal_HstSendEvent(SYS_EVENT, 0x19880132);
			hal_HstSendEvent(SYS_EVENT, g_bt_state);
			if(g_sco_play)
			{
				mediaSendCommand(MC_STOP, 0);
			}
			BT_StopAudio();//2013-7-1
			g_sco_play = 0 ;
			COS_Sleep(500);
			if(g_bt_state > BT_CONNECTED)
			{
				if(g_bt_state == BT_CALL)
				{
					MESSAGE_SetEvent(EV_BT_CALL_IND);
				}
				else if(g_bt_state == BT_PLAY)
				{
					MESSAGE_SetEvent(EV_BT_A2DP_PLAY_IND);
				}
				else if(g_bt_state == BT_FILE)
				{
					MESSAGE_SetEvent(EV_BT_FILE_RSV_IND);
				}
				
				return RESULT_BT_ACTIVE;
			}
			break;
		}
		
		if(call_status != BT_CALL_STATUS_NONE && call_status != BT_CALL_STATUS_END)
		{
			MESSAGE_SetEvent(EV_BT_CALL_IND);
			return RESULT_BT_ACTIVE;
		}
		
		if(g_bt_need_draw)
		{
			BT_DisplayMain();
			g_bt_need_draw = FALSE;
		}
		/*if(g_bt_sco_handle == 0 && g_sco_play == 1)
		{
		    hal_HstSendEvent(SYS_EVENT, 0x19880032);
		    mediaSendCommand(MC_STOP, 0);
		    g_sco_play = 0;
		}
		else*/ //if(g_sco_play == 0 && g_bt_sco_handle)
		
		if(g_sco_play == 0 && g_bt_sco_handle && media_GetInternalStatus() != PLAYING_INTERNAL)
		{
			hal_HstSendEvent(SYS_EVENT, 0x1988cc01);
			mediaSendCommand(MC_PLAY_SCO, g_bt_sco_handle);
#if APP_SUPPORT_DELAY_VOLUME==1
			wait_volume = 2;
			SetPAVolume(0);
#else
			SetPAVolume(g_pBT_vars->volume);
#endif
			g_sco_play = 1;
	     while(BTSco_GetStatus());
		}
		/*
		        if(!g_bt_sco_handle && is_play_music)
		        {
		            hal_HstSendEvent(SYS_EVENT, 0x13062702);
		            hal_HstSendEvent(SYS_EVENT, g_sco_play);
		            hal_HstSendEvent(SYS_EVENT, play_start_cnf);
		            if(!g_sco_play)
		            {
		                if(play_start_cnf)
		                {
		                    Avrcp_Send_Key(OPID_PLAY);
		                    BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
		                    g_sco_play = 1;
		                    is_play_music = FALSE;
		                    play_start_cnf = FALSE;
		                }
		            }
		        }
		*/
		
		key = MESSAGE_Wait();
		if(MESSAGE_IsNumberKey(key))
		{
			result = BT_InputNumber();
			if(result != 0)
			{
				goto connect_end;
			}
			g_bt_need_draw = 1;
			continue;
		}
		
		switch(key)
		{
		case AP_MSG_RTC:
#if !defined(CLOSE_BT_SWITCH_MODE)		
		if(last_mode!=RESULT_NULL)
		{
			hal_HstSendEvent(APP_EVENT, 0x14070203);
			return last_mode;
		}
#endif			
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				g_bt_need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
#if APP_SUPPORT_DELAY_VOLUME==1
			if(wait_volume > 0)
			{
				wait_volume --;
				if(wait_volume == 0)
				{
					SetPAVolume(g_pBT_vars->volume);
					if(g_pBT_vars->volume)
					{
						gpio_SetMute(FALSE);
					}
					else
					{
						gpio_SetMute(TRUE);
					}
				}
			}
#endif
			break;
			
		case AP_KEY_PLAY|AP_KEY_PRESS:
			hal_HstSendEvent(SYS_EVENT, 0x19888700);
			hal_HstSendEvent(SYS_EVENT, g_avdtp_connect);
			hal_HstSendEvent(SYS_EVENT, g_avrcp_connect);
			if(g_avdtp_connect == 1)
			{
				if(g_avrcp_connect == 0)
				{
					Avrcp_Connect_Req(g_bt_a2dp_bdaddr);
					avrcp_play_pending = 1;
				}
				else
				{
					hal_HstSendEvent(SYS_EVENT, 0x1988fd01);
					//    hal_HstSendEvent(SYS_EVENT, play_start_cnf);
					//    if(play_start_cnf)
					{
						Avrcp_Send_Key(OPID_PLAY);
						short_play_status = TRUE;
						//         play_start_cnf = FALSE;
					}
					//     BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
				}
			}
			else
			{
				t_bdaddr addr;
				HF_Get_Current_BdAddr(&addr);
				AVDTP_Setup_Connect(addr);
				play_in_noconnect = TRUE;
			}
			
			break;
			
		case AP_KEY_PLAY|AP_KEY_DOUBLE:
#if !defined(NOT_SUPPORT_HFP)
			//if(MESSAGE_IsHold())
			{
				HF_Call_Request(HF_CALL_REDIAL, 0);
			}
#endif
			break;
			
		case AP_KEY_NEXT|AP_KEY_PRESS:
			Avrcp_Send_Key(OPID_FORWARD);
			break;
			
		case AP_KEY_PREV|AP_KEY_PRESS:
			Avrcp_Send_Key(OPID_BACKWARD);
			break;
			
		default:                                        //热键处理
			result = BT_HandleKey(key);
			if(key != 0 && key != AP_MSG_RTC)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19883000);
				hal_HstSendEvent(SYS_EVENT, key);
				hal_HstSendEvent(SYS_EVENT, result);
			}
			if(result != 0)
			{
connect_end:
				if(result == RESULT_BT_ACTIVE)
				{
					key = MESSAGE_Wait();
				}
				if(result != RESULT_BT_ACTIVE && g_bt_sco_handle)
				{
					hal_HstSendEvent(SYS_EVENT, 0x1988fc00);
					//GUI_DisplayMessage(0, GUI_STR_WAITPASS, NULL, GUI_MSG_FLAG_DISPLAY);
					//MGR_ReleaseSynchronous(g_bt_sco_handle);
					mediaSendCommand(MC_STOP, 0);
					//g_sco_play = 0;
					//while(g_bt_sco_handle)
					//    MESSAGE_Wait();
				}
				
				if(result == RESULT_BT_ACTIVE)
				{
					MESSAGE_SetEvent(key);
				}
				hal_HstSendEvent(SYS_EVENT, 0x1988fc01);
				hal_HstSendEvent(SYS_EVENT, result);
				hal_HstSendEvent(SYS_EVENT, key);
				return result;
			}
		}
	};
	
	return 0;
}


#if APP_SUPPORT_PBAP==1
extern int phonebook_step;
extern int history_step;
#endif
#if APP_SUPPORT_MAP==1
extern int mas_step;
extern int mns_step;
#endif

UINT8 bt_connect_timer = 0;
t_bdaddr bt_addr = {0};
UINT8 profile = 0;
INT32 connect_count = 1;

//蓝牙断开后自动重连
void bt_auto_con_timeout(void *param)
{
	COS_KillTimer(bt_connect_timer);
	bt_connect_timer = 0;
	connect_count <<= 1;
	if(g_bt_state < BT_CONNECTED && connect_count < 64)
	{
		hal_HstSendEvent(SYS_EVENT, 0x19991212);
		bt_connect_timer = COS_SetTimer(20000, bt_auto_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
#if !defined(NOT_SUPPORT_HFP)
		BT_Connect_Handsfree(g_bt_cur_device);
#else
		BT_Connect_A2DP(g_bt_cur_device);
#endif
	}
	else
	{
		hal_HstSendEvent(SYS_EVENT, 0x19991213);
		connect_count = 1;
	}
}


void bt_auto_con_a2dp_timeout(void *param)
{
	static int a2dp_reconnect_count = 0;
	if((g_bt_state < BT_CONNECTED) && (a2dp_reconnect_count < 6))
	{
		hal_HstSendEvent(SYS_EVENT, 0x19991214);
		COS_SetTimer(20000, bt_auto_con_a2dp_timeout, NULL, COS_TIMER_MODE_SINGLE);
		BT_Connect_A2DP(g_bt_cur_device);
		a2dp_reconnect_count++;
	}
	else
	{
		a2dp_reconnect_count = 0;
		hal_HstSendEvent(SYS_EVENT, 0x19991215);
	}
	
	return;
}


extern Message_info *message_curr[4];
extern Message_info *message_prev[4];
extern Message_info *message_next[4];
extern char *msg_show[4];
//extern char name_show[4][PB_LENGTH*10];
#if APP_SUPPORT_PBAP==1
extern int find_str(const char *src_str, const char *spec_str, int str_len );
extern int phonebook_size;
extern int history_size;
extern int current_get_pos;
extern Message_info *message_curr[4];
extern Message_info *message_prev[4];
extern Message_info *message_next[4];
extern History_info *history_curr[4];
extern History_info *history_prev[4];
extern History_info *history_next[4];
extern History_info *history_show[4];
extern int current_pn_index;
extern int end_pn_index;
extern int obex_start;
extern int obex_size;
extern BOOL first_show_history;
extern BOOL downLoading;

int last_size = 0;
int last_start = 0;
static int data_cnt = 0;
void deal_pbap_hf_data(char *data, enum PhoneBook_Type type)
{
	int ret = 0;
	int index = 0;
	int tmp_index  = 0;
	char pb_size_data[4] = {0};
	char number_tmp[20] = {0};
	char name_tmp[PB_LENGTH * 2] = {0};
	char name[PB_LENGTH * 2] = {0};
	int size_tmp = 0;
	int pos = 0;
	int unicode_len = 0;
	int pb_pos = 0;
	ret = find_str(&data[index], "(", 1);
	if(ret >= 0)
	{
		index += ret;
		index += 1;
		ret = find_str(&data[index], "-", 1);
		if(ret >= 0)
		{
			index += ret;
			index += 1;
			tmp_index = index;
			ret = find_str(&data[index], ")", 1);
			if(ret > 0)
			{
				index += ret;
				if((index - tmp_index) > sizeof(pb_size_data))
				{
					strncpy(pb_size_data, &data[tmp_index], sizeof(pb_size_data));
				}
				else
				{
					strncpy(pb_size_data, &data[tmp_index], index - tmp_index);
				}
				size_tmp = atoi(pb_size_data);
				switch(type)
				{
				case PHONE_BOOK_PHONE_BOOK:
					phonebook_size = size_tmp;
					phonebook_step = 3;
					last_size = phonebook_size > 10 ? 10 : phonebook_size;
					if(last_size >= 1)
					{
						HF_Get_PhoneBook(1, last_size);
					}
					pb_sort = (Pb_Sort *)(inputbuf_overlay + 4096);
					NVRAMResetPhoneBook(phonebook_size, sizeof(Phonebook_info), sizeof(Pb_Sort));
					break;
					
				case PHONE_BOOK_INCOMING_HISTORY:
				case PHONE_BOOK_OUTCOMING_HISTORY:
				case PHONE_BOOK_MISSED_CALL:
					history_size = size_tmp;
					history_step = 3;
					last_size = history_size > 4 ? 4 : history_size;
					HF_Get_PhoneBook(1, last_size);
					break;
				}
			}
		}
	}
	else
	{
		while(pos < 4 && pb_pos < 10)
		{
			/*
			while(data[index] != 0x0d || data[index+1] != 0x0a)
			{
				index++;
			}
			*/
			
			ret = find_str(&data[index], "+CPBR", 5);
			if(ret < 0)
			{
				break ;
			}
			index += ret;
			index += 5;
			ret = find_str(&data[index], "\"", 1);
			if(ret < 0)
			{
				break ;
			}
			index += ret;
			index += 1;
			tmp_index = index;
			ret = find_str(&data[index], "\"", 1);
			if(ret < 0)
			{
				break ;
			}
			index += ret;
			if((index - tmp_index) > sizeof(number_tmp))
			{
				strncpy(number_tmp, &data[tmp_index], sizeof(number_tmp));
			}
			else
			{
				strncpy(number_tmp, &data[tmp_index], index - tmp_index);
			}
			index += 1;
			ret = find_str(&data[index], "\"", 1);
			if(ret < 0)
			{
				break ;
			}
			index += ret;
			index += 1;
			tmp_index = index;
			ret = find_str(&data[index], "\"", 1);
			if(ret < 0)
			{
				break ;
			}
			index += ret;
			if((index - tmp_index) > sizeof(name))
			{
				strncpy(name, &data[tmp_index], sizeof(name));
			}
			else
			{
				strncpy(name, &data[tmp_index], index - tmp_index);
			}
			index += 1;
			/*
			unicode_len = UTF8_to_Unicode(name_tmp, name);
			if(unicode_len > 0)
			{
				memset(&name_tmp[unicode_len],0, PB_LENGTH*2 - unicode_len);
				gui_unicode2local(name_tmp, unicode_len);
			}*/
			code_trans(name_tmp, name, 40, TRUE);
			switch(type)
			{
			case PHONE_BOOK_PHONE_BOOK:
				//name_tmp[strlen(name_tmp) - 2] = 0;
				/*
				if(current_get_pos == 0)
				{
					strncpy(name_curr[pos], name_tmp, strlen(name_tmp));
					strcpy(name_data_curr[pos][0], number_tmp);
				}
				else if(current_get_pos == 1)
				{
					strncpy(name_prev[pos], name_tmp, strlen(name_tmp));
					strcpy(name_data_prev[pos][0], number_tmp);
				}
				else if(current_get_pos == 2)
				{
					strncpy(name_next[pos], name_tmp, strlen(name_tmp));
					strcpy(name_data_next[pos][0], number_tmp);
				}*/
				strncpy(phonebook_info->name, name_tmp, PB_LENGTH);
				strncpy(phonebook_info->tel, number_tmp, PB_LENGTH);
				strncpy(pb_sort->pin_yin, phonebook_info->pinyin, 4);
				NVRAMWritePhoneBook(phonebook_info, pb_index);
				pb_sort->index = pb_index;
				pb_index++;
				pb_sort ++;
				pb_pos++;
				pos = 0;
				if(pb_pos >= 10)
				{
					pos = 5;
				}
				break;
			case PHONE_BOOK_INCOMING_HISTORY:
			case PHONE_BOOK_OUTCOMING_HISTORY:
			case PHONE_BOOK_MISSED_CALL:
				if(current_get_pos == 0)
				{
					strncpy(history_curr[pos]->name, name_tmp, PB_LENGTH);
					strncpy(history_curr[pos]->tel, number_tmp, PB_LENGTH);
				}
				else if(current_get_pos == 1)
				{
					strncpy(history_prev[pos]->name, name_tmp, PB_LENGTH);
					strncpy(history_prev[pos]->tel, number_tmp, PB_LENGTH);
				}
				else if(current_get_pos == 2)
				{
					strncpy(history_next[pos]->name, name_tmp, PB_LENGTH);
					strncpy(history_next[pos]->tel, number_tmp, PB_LENGTH);
				}
				break;
			}
			memset(name, 0, sizeof(name));
			memset(name_tmp, 0, sizeof(name_tmp));
			memset(number_tmp, 0, sizeof(number_tmp));
			phonebook_info_clean(phonebook_info);
			pos++;
			if(pos >= 4)
			{
				pb_pos = 10;
			}
		}
		
		/*
		if(type == PHONE_BOOK_PHONE_BOOK && obex_size == 0 && first_show_pb && phonebook_size > 4)
		{
			current_get_pos = 1;
		
		  	obex_start = ((phonebook_size>>2)<<2)+1;
		  	obex_size = phonebook_size % 4;
		  	if(obex_size == 0)
		  	{
		  		obex_start = phonebook_size - 4 - 1;
		  		obex_size = 4;
		  	}
		  	name_data_cpy(name_show, name_curr);
			HF_Get_PhoneBook(obex_start, obex_start+obex_size-1);
		}*/
		if(type == PHONE_BOOK_PHONE_BOOK)
		{
			if(last_size < phonebook_size)
			{
				HF_Get_PhoneBook(last_size + 1, last_size + 10);
				last_size += 10;
			}
			else
			{
				pb_sort = (Pb_Sort *)(inputbuf_overlay + 4096);
				py_sort(pb_sort, pb_index);
				NVRAMWritePhoneBookIndex((UINT8 *)pb_sort, pb_index);
				
				phonebook_size = pb_index;
				downLoading = FALSE;
				MESSAGE_SetEvent(OBEX_SHOW_PHONEBOOK);
				NVRAMGetPhoneBookParam(&pb_length, &pb_index, &phonebook_size);
				pb_addr = NVRAMGetPhoneBookAddress();
				pb_addr += DSM_GetPhoneBookOffset();
			}
		}
		else if((type == PHONE_BOOK_INCOMING_HISTORY
		         ||
		         type == PHONE_BOOK_OUTCOMING_HISTORY
		         ||
		         type == PHONE_BOOK_MISSED_CALL
		        )
		        && first_show_history && obex_size == 0
		        && history_size > 4)
		{
			current_get_pos = 1;
			obex_start = ((history_size >> 2) << 2) + 1;
			obex_size = history_size % 4;
			if(obex_size == 0)
			{
				obex_start = history_size - 4 - 1;
				obex_size = 4;
			}
			history_info_cpy(history_show, history_curr);
			HF_Get_PhoneBook(obex_start, obex_start + obex_size - 1);
		}
		else if(pos > 0)
		{
			current_get_pos = 0;
			downLoading = FALSE;
			if(type == PHONE_BOOK_PHONE_BOOK)
			{
				MESSAGE_SetEvent(OBEX_SHOW_PHONEBOOK);
			}
			else
			{
				MESSAGE_SetEvent(OBEX_SHOW_HISTORY);
			}
		}
	}
	//HF_Pbap_Clean();
}
#endif

#define PROFILE_CONNECT

#ifdef PROFILE_CONNECT
UINT32 a2dp_con_timer = 0;
UINT32 hfp_con_timer = 0;
UINT8 a2dp_con_count = 0;
UINT8 hfp_con_count = 0;

void a2dp_con_timeout(void *param)
{
	if(g_avdtp_connect == 0)
	{
		BT_Connect_A2DP(g_bt_cur_device);
	}
	else
	{
		COS_KillTimer(a2dp_con_timer);
		a2dp_con_timer = 0;
		a2dp_con_count = 0;
		return;
	}
	if(a2dp_con_count < 3)
	{
		a2dp_con_count++;
		a2dp_con_timer = COS_SetTimer(8000, a2dp_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
	}
}

void hfp_con_timeout(void *param)
{
	if(g_hfp_connect == 0)
	{
		BT_Connect_Handsfree(g_bt_cur_device);
	}
	else
	{
		COS_KillTimer(hfp_con_timer);
		hfp_con_timer = 0;
		hfp_con_count = 0;
		return;
	}
	if(hfp_con_count < 3)
	{
		hfp_con_count++;
		hfp_con_timer = COS_SetTimer(8000, hfp_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
	}
}
#endif
#if 0
u_int8 sco_disconnect_timer = 0;
u_int8 sco_connection_timer = 0;

void sco_connection_timeout(void *param)
{
	t_bdaddr address;
	if(!g_bt_sco_handle)
	{
		HF_Get_Current_BdAddr(&address);
		MGR_SetupSynchronous(address);
	}
}

void sco_disconnect_timeout(void *param)
{
	if(g_bt_sco_handle)
	{
		MGR_ReleaseSynchronous(g_bt_sco_handle);
	}
}
#endif
char *char_code = "=\"UTF-8\"";

UINT32	start = 1, end = 1;
void BT_HandleHFPMessage(u_int16 msg_id, void* msg_data)
{
	INT32 i;
	u_int8 number[30] = {0};
	u_int8 length = 0;
	//hal_HstSendEvent(APP_EVENT, 0x13102800);
	//hal_HstSendEvent(APP_EVENT, msg_id);
	switch(msg_id)
	{
#if APP_SUPPORT_PBAP==1
	case HFG_PHONE_BOOK_IND:
		deal_pbap_hf_data(msg_data, PHONE_BOOK_PHONE_BOOK);
		break;
	case HFG_INCOMING_HISTORY_IND:
		deal_pbap_hf_data(msg_data, PHONE_BOOK_INCOMING_HISTORY);
		break;
	case HFG_OUTCOMING_HISTORY_IND:
		deal_pbap_hf_data(msg_data, PHONE_BOOK_OUTCOMING_HISTORY);
		break;
	case HFG_MISSED_CALL_HISTORY_IND:
		deal_pbap_hf_data(msg_data, PHONE_BOOK_MISSED_CALL);
		break;
#endif
	case HFG_ACL_CONNECT_REQ:
		{
			t_hfp_connect_ind *msg_p = (t_hfp_connect_ind *)msg_data;
			if(g_hfp_connect == 0)
			{
				HF_Accept_SLC(0, msg_p->bt_addr);
			}
			else
			{
				HF_Reject_SLC(msg_p->bt_addr);
			}
			break;
		}
	case HFG_ACL_CONNECT_IND:
		{
			t_hfp_connect_ind *msg_p = (t_hfp_connect_ind *)msg_data;
			if(msg_p->result == RDABT_NOERROR)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19870007);
				if(g_bt_state < BT_CONNECTED)
				{
					g_bt_state = BT_CONNECTED;
				}
				g_hfp_connect = 1;
				g_bt_cur_device = BT_Find_Device(msg_p->bt_addr);
				g_bt_device_change = 1;
				//MGR_InitBonding(g_pBT_vars->device_list[g_bt_cur_device].addr);
#if APP_SUPPORT_PBAP==1
				phonebook_step = 1;
				history_step = 1;
#endif
#if APP_SUPPORT_MAP==1
				mas_step = 1;
				mns_step = 1;
#endif
				if(bt_connect_timer)
				{
					COS_KillTimer(bt_connect_timer);
					bt_connect_timer = 0;
					connect_count = 1;
				}
				HF_Get_CallStatus(&call_status);
				hal_HstSendEvent(SYS_EVENT, 0x20130702);
				hal_HstSendEvent(SYS_EVENT, call_status);
				if(g_bt_state != BT_CALL
				   && call_status != 0
				   && call_status != BT_CALL_STATUS_END
				   && g_bt_sco_handle)
				{
					HF_Get_Clcc();
					BT_Send_Msg_Up(EV_BT_CALL_IND, 0);
				}
#if defined(CLOSE_BT_SWITCH_MODE)
				if(g_current_module != FUN_BT)
				{
					hal_HstSendEvent(SYS_EVENT, 0x11091248);
					BT_Disconnect_Device();
					COS_Sleep(1500);
					BT_Close_Bluetooth();
					break;
				}
#endif
#if defined(BT_DEVICE_SAVE)
				if(g_current_module == FUN_BT&&g_avdtp_connect==1)
				{
					BT_Send_Msg_Up(EV_BT_CONNECT_IND, 0);
				}
#endif
#ifdef PROFILE_CONNECT
				if(g_avdtp_connect == 0 && a2dp_con_timer == 0)
				{
					a2dp_con_timer = COS_SetTimer(6000, a2dp_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
				}
				else
				{
					if(a2dp_con_timer)
					{
						COS_KillTimer(a2dp_con_timer);
						a2dp_con_timer = 0;
					}
					if(hfp_con_timer)
					{
						COS_KillTimer(hfp_con_timer);
						hfp_con_timer = 0;
					}
				}
#endif
				BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BTHFPCONNECTED);
				MGR_SetConnectableMode(FALSE);
#if APP_SUPPORT_BTBATTERY==1
				is_iphone = -1; // try as iphone
				g_cur_battery = -1; // force send battery at least once
				BT_HFPSendBattery();
#else
				HF_Activate_CLIP();
#endif
				
			}
			else if(g_bt_connect_pending)
			{
				hal_HstSendEvent(SYS_EVENT, 0xeeff0011);
				hal_HstSendEvent(SYS_EVENT, msg_p->result);
				BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BT_CONNECT_FAIL);
			}
			g_bt_connect_pending = 0;
		}
		break;
	case HFG_ACL_DISCONNECT_IND:
		{
			if((g_bt_state < BT_CONNECTED) && (g_hfp_connect == 0)) //warkey 2.1  //防止回连时手机拒绝配对后来此消息后一直输出蓝牙断开提示音
			{
				hal_HstSendEvent(APP_EVENT, 0x13102600);
				break;
			}
			t_hfp_connect_ind *msg_p = (t_hfp_connect_ind *)msg_data;
#ifdef PROFILE_CONNECT			
			if(a2dp_con_timer)
			{
				COS_KillTimer(a2dp_con_timer);
				a2dp_con_timer =0;
			}
			if(hfp_con_timer)
			{
				COS_KillTimer(hfp_con_timer);
				hfp_con_timer =0;
			}
#endif			
			g_hfp_connect = 0;
			g_bt_sco_handle = 0;
			call_status = BT_CALL_STATUS_NONE;
#ifndef WIN32
			BTSco_Stop();//2013-7-1
#endif
			if(g_sco_play)//2013-7-3
			{
				mediaSendCommand(MC_STOP, 0);
				g_sco_play = 0;
			}
			
			bt_connect_reminder = 0;
			
			if(g_avdtp_connect || g_avrcp_connect)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19870008);
				if(g_a2dp_play)
				{
					hal_HstSendEvent(SYS_EVENT, 0x19871008);
					g_bt_state = BT_PLAY;
				}
				else
				{
					hal_HstSendEvent(SYS_EVENT, 0x19872008);
					hal_HstSendEvent(SYS_EVENT, g_avdtp_connect);
					hal_HstSendEvent(SYS_EVENT, g_avrcp_connect);
					g_bt_state = BT_CONNECTED;
				}
			}
			else
			{
#if 0
				for(i = 0; i < 4; i++)
				{
					if(name_curr[i])
					{
						pFree(name_curr[i]);
						name_curr[i] = 0;
					}
					if(name_prev[i])
					{
						pFree(name_prev[i]);
						name_prev[i] = 0;
					}
					if(name_next[i])
					{
						pFree(name_next[i]);
						name_next[i] = 0;
					}
					/*
					if(name_show[i])
					{
						pFree(name_show[i]);
						name_show[i] = 0;
					}*/
				}
#endif
#if defined(BT_DEVICE_SAVE)
				hal_HstSendEvent(SYS_EVENT, 0xbbbbcccc);
				g_hfp_connect = 0;
				g_avdtp_connect = 0;
				g_avrcp_connect = 0;
				BT_Send_Msg_Up(EV_BT_DISCONNECT_IND, 0);
#endif
				g_bt_state = BT_IDLE;
				if(!(g_bt_ops_state & BT_OPS_CLOSED))
				{
					MGR_SetConnectableMode(TRUE);
				}
				g_bt_ops_state = 0;
				//                media_PlayInternalAudio(GUI_AUDIO_BT_DISCONNECT, 1, FALSE);
			}
			g_bt_device_change = 1;
			connect_count = 1;
			hal_HstSendEvent(SYS_EVENT, 0x19992014);
			hal_HstSendEvent(SYS_EVENT, msg_p->result);
			if((msg_p->result != 0) && (((g_pBT_vars->device_list[g_bt_cur_device].cod >> 8) & 0x1f) == 2)) // auto connect
			{
				connect_count = 1;
				bt_connect_timer = COS_SetTimer(5000, bt_auto_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
				//BT_Connect_Handsfree(g_bt_cur_device);
				//   HF_Connect_SLC(0, msg_p->bt_addr, msg_p->profile);
				//   strcpy(bt_addr.bytes, msg_p->bt_addr.bytes);
				//  profile = msg_p->profile;
			}
			//BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BTHFPDISCONNECTED);
			if(g_bt_connect_pending > 0)
			{
				BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BT_CONNECT_FAIL);
			}
			g_bt_connect_pending = 0;
			MESSAGE_SetEvent(0); // wake up ap task
		}
		break;
	case HFG_SPEAKER_GAIN_IND:
		{
#if APP_SUPPORT_BT_REMOTE_VOL==1
			t_hfp_gain_ind *msg = (t_hfp_gain_ind*)msg_data;
			UINT8 vol = msg->gain * AUD_MAX_LEVEL / 15;  // max value of gain is 15
			g_pBT_vars->volume = vol;
			g_comval->Volume = vol;
			SetPAVolume(vol);
#endif
			/*
			if(msg->gain > g_pBT_vars->volume)
			{
			    g_vol_set_by_ag = msg->gain;
			    hal_HstSendEvent(SYS_EVENT, 0x1988a001);
			    if(!is_first_play)
			    {
			        hal_HstSendEvent(SYS_EVENT, 0x1988a002);
			        MESSAGE_SetEvent(AP_KEY_VOLADD | AP_KEY_DOWN);
			        MESSAGE_SetEvent(AP_KEY_VOLADD | AP_KEY_PRESS);
			    }
			    else
			    {
			        hal_HstSendEvent(SYS_EVENT, 0x1988a003);
			        is_first_play = FALSE;
			    }
			}
			else if(msg->gain < g_pBT_vars->volume)
			{
			    hal_HstSendEvent(SYS_EVENT, 0x1988a004);
			    g_vol_set_by_ag = msg->gain;
			    if(!is_first_play)
			    {
			        hal_HstSendEvent(SYS_EVENT, 0x1988a005);
			        MESSAGE_SetEvent(AP_KEY_VOLSUB | AP_KEY_DOWN);
			        MESSAGE_SetEvent(AP_KEY_VOLSUB | AP_KEY_PRESS);
			    }
			    else
			    {
			        hal_HstSendEvent(SYS_EVENT, 0x1988a006);
			        is_first_play = FALSE;
			    }
			}*/
		}
		break;
	case HFG_CALL_STATUS_IND:
		call_status = (UINT32)msg_data;
		hal_HstSendEvent(SYS_EVENT, 0x19880ba0);
		hal_HstSendEvent(SYS_EVENT, call_status);
		hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
		if(call_status & BT_CALL_STATUS_ACTIVE)
		{
			call_status = BT_CALL_STATUS_ACTIVE;
		}
		
		if(call_status != 0 && g_a2dp_play == 1)
		{
			BT_StopAudio();
		}
		
		if(call_status == 0)
		{
			hal_HstSendEvent(APP_EVENT, 0x13120603);
			if(g_bt_state == BT_CALL)
			{
				g_bt_state = BT_CONNECTED;
			}
			//add by wuxiang
			call_status = BT_CALL_STATUS_END;
#if APP_SUPPORT_NUMBER_VOICE==1
			vVoiceType = ePlayVoice_None;
			if(g_bt_ops_state & BT_OPS_CALL_STATUS_OUTGOING)
			{
				clean_bt_ops_state(BT_OPS_CALL_STATUS_OUTGOING);
			}
#endif
#ifndef WIN32
			//BTSco_AudCallStop();
#endif
			hal_HstSendEvent(SYS_EVENT, 0x19880071);
			hal_HstSendEvent(SYS_EVENT, g_bt_sco_handle);
			//hal_HstSendEvent(SYS_EVENT, is_play_music);
			hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
			
			hal_HstSendEvent(SYS_EVENT, 0x19880077);
			hal_HstSendEvent(SYS_EVENT, g_bt_sco_handle);
			
			//sco_disconnect_timer = COS_SetTimer(3000, sco_disconnect_timeout, NULL, COS_TIMER_MODE_SINGLE);
			/*
			if(g_bt_sco_handle)
			{
				MGR_ReleaseSynchronous(g_bt_sco_handle);
			}*/
#ifndef WIN32
			BTSco_Stop();
#endif
#if APP_SUPPORT_PBAP==1
			if(download_abort)
			{
				download_abort = FALSE;
				BT_Phone_Book();
			}
#endif
		}
		else if(g_bt_state != BT_CALL)
		{
			hal_HstSendEvent(APP_EVENT, 0x13120602);
			if(g_current_module != FUN_BT)
			{
				g_prev_module = g_current_module;
				hal_HstSendEvent(APP_EVENT, 0x13120601);
				set_bt_ops_state(BT_OPS_MODULE_CHANGE);
			}
#if APP_SUPPORT_PBAP == 1
			if(phonebook_step > 1)
			{
				if(!(g_bt_ops_state & BT_OPS_MUSIC_PLAY))
				{
					download_abort = TRUE;
				}
				else
				{
					download_abort = FALSE;
				}
				common_var_init();
			}
#endif
#if APP_SUPPORT_NUMBER_VOICE == 1
			if(call_status == BT_CALL_STATUS_OUTGOING)
			{
				set_bt_ops_state(BT_OPS_CALL_STATUS_OUTGOING);
			}
#endif
			g_bt_state = BT_CALL;
			hal_HstSendEvent(SYS_EVENT, 0x19880072);
			BT_Send_Msg_Up(EV_BT_CALL_IND, 0);
			
			if(call_status == BT_CALL_STATUS_ACTIVE)
			{
				if(!g_sco_play)
				{
					media_StopInternalAudio();
				}
				/*if(!g_bt_sco_handle)
				{
					t_bdaddr address;
					HF_Get_Current_BdAddr(&address);
					MGR_SetupSynchronous(address);
				}*/
			}
		}
		/*else if(call_status == BT_CALL_STATUS_ACTIVE)
		{
			if(!g_sco_play)
			{
				media_StopInternalAudio();
			}
			if(!g_bt_sco_handle)
			{
				t_bdaddr address;
				HF_Get_Current_BdAddr(&address);
				MGR_SetupSynchronous(address);
			}
		}*/
		
		if(call_status != 0 && call_status != BT_CALL_STATUS_END)
		{
			//u_int8 length;
			//HF_Get_Number(NULL, &length);
			//if(length==0)
			HF_Get_Clcc();
		}
		break;
	case HFG_RING_IND:
		if(call_status == BT_CALL_STATUS_NONE)
		{
			call_status = BT_CALL_STATUS_INCOMING;
			//g_bt_state = BT_CALL;
			hal_HstSendEvent(SYS_EVENT, 0x19880073);
			BT_Send_Msg_Up(EV_BT_CALL_IND, 0);
		}
		hal_HstSendEvent(SYS_EVENT, 0x201300f0);
		hal_HstSendEvent(SYS_EVENT, g_sco_play);
		hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
		hal_HstSendEvent(SYS_EVENT, g_current_module);
		
		if(!g_sco_play && !g_a2dp_play && g_current_module == FUN_BT
#if APP_SUPPORT_NUMBER_VOICE==1
		   && vVoiceType == ePlayVoice_None
#endif
		  )
		{
			media_PlayInternalAudio(GUI_AUDIO_BT_RING, 1, FALSE);
		}
		break;
	case HFG_DATA_IND:
		if(msg_data)
		{
			app_trace(APP_BT_TRC, "Receive HFP AT CMD: '%s'", msg_data);
#if APP_SUPPORT_BTBATTERY==1
			if(strncmp((char*)msg_data + 2, "+XAPL=", 6) == 0)
			{
				char temp[25];
				is_iphone = 1;
				sprintf(temp, "AT+IPHONEACCEV=1,1,%d\r\n", g_cur_battery);
				HF_Send_Raw_Data(&temp, strlen(temp));
				HF_Activate_CLIP();
			}
#endif
		}
		break;
#if APP_SUPPORT_PBAP==1
	case HFG_PHONE_BOOK_GET_CODE_IND:
		HF_Set_PhoneBook_Code(char_code);
		break;
	case HFG_PHONE_BOOK_SET_CODE_IND:
		HF_Set_PhoneBook_PATH(PHONE_BOOK_PHONE, PHONE_BOOK_PHONE_BOOK);
		break;
	case HFG_PHONE_BOOK_Q_CODE_IND:
		//	HF_Set_PhoneBook_PATH(PHONE_BOOK_PHONE, PHONE_BOOK_PHONE_BOOK);
		break;
	case HFG_PHONE_BOOK_SET_PATH_IND:
		HF_Get_PhoneBook_Size();
		break;
	case HFG_PHONE_BOOK_GET_CODE_ERR_IND:
		break;
	case HFG_PHONE_BOOK_SET_CODE_ERR_IND:
		//	HF_Set_PhoneBook_PATH(PHONE_BOOK_PHONE, PHONE_BOOK_PHONE_BOOK);
		break;
	case HFG_PHONE_BOOK_Q_CODE_ERR_IND:
		break;
	case HFG_PHONE_BOOK_SET_PATH_ERR_IND:
		break;
	case HFG_PHONE_BOOK_ERR_IND:
		break;
	case HFG_PHONE_BOOK_IND:
		app_trace(APP_BT_TRC, "---------------HFG_PHONE_BOOK_IND---------------");
		app_trace(APP_BT_TRC, "start:%d, end:%d", start, end);
		if(end <= 400)
		{
			HF_Get_PhoneBook(start, end);
		}
		start += 1;
		end += 1;
		//deal_pbap_hf_data(msg_data, PHONE_BOOK_PHONE_BOOK);
		break;
	case HFG_INCOMING_HISTORY_IND:
		//deal_pbap_hf_data(msg_data, PHONE_BOOK_INCOMING_HISTORY);
		break;
	case HFG_OUTCOMING_HISTORY_IND:
		//deal_pbap_hf_data(msg_data, PHONE_BOOK_OUTCOMING_HISTORY);
		break;
	case HFG_MISSED_CALL_HISTORY_IND:
		//deal_pbap_hf_data(msg_data, PHONE_BOOK_MISSED_CALL);
		break;
#endif
	default:
		break;
	}
}

void BT_HFPSendBattery(void)
{
#if  APP_SUPPORT_BTBATTERY==1
	char temp[30];
	UINT8 percent = 0, charging = 0, mpc = 0;
	
	//app_trace(APP_BT_TRC, "BT_HFPSendBattery,g_hfp_connect=%d,g_cur_battery=%d",g_hfp_connect,g_cur_battery);
	if(g_hfp_connect)
	{
		PM_GetBatteryInfo(&charging, &percent, &mpc);
		percent = percent / 10;
		if(percent >= 10)
		{
			percent = 9;
		}
		if(percent != g_cur_battery)
		{
			g_cur_battery = percent;
			if(is_iphone != 0)
			{
				char  *iphone_str = "AT+XAPL=rdabt-9621-0100,1\r\n";
				HF_Send_Raw_Data(iphone_str, strlen(iphone_str));
				HF_Set_XAPL_pending();
			}
			if(is_iphone != 1)
			{
				//sprintf(temp, "AT+XEVENT=RDA,1,%d\r\n", g_cur_battery);
				//HF_Send_Raw_Data(temp,strlen(temp));
			}
			if(is_iphone == -1)
			{
				is_iphone = 0;
			}
		}
	}
#endif
}

BOOL a2dp_data_flag = FALSE;
UINT8 a2dp_data_timer = 0;

void a2dp_data_timeout_end(void *param)
{
	a2dp_data_flag = FALSE;
}

void a2dp_data_timeout(void *param)
{
	hal_HstSendEvent(SYS_EVENT, 0x19885800);
	a2dp_data_flag = TRUE;
	a2dp_data_timer = 0;
	COS_SetTimer(200, a2dp_data_timeout_end, NULL, COS_TIMER_MODE_SINGLE);
}

UINT32 musicplay_timer = 0;
static void cycle_detect_musicplay()
{
	if(music_playing==0)
	{
		BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
		musicplay_timer=COS_SetTimer(2000, cycle_detect_musicplay, NULL, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		COS_KillTimer(musicplay_timer);
		musicplay_timer=0;
	}
}

void BT_HandleA2DPMessage(u_int16 msg_id, void* msg_data)
{
	// hal_HstSendEvent(SYS_EVENT,0x10250050);
	//  hal_HstSendEvent(SYS_EVENT,msg_id);
	switch(msg_id)
	{
	case A2DP_SIGNAL_CONNECT_CNF:
	case A2DP_SIGNAL_CONNECT_IND:
		{
			a2dp_connect_ind_msg_t *msg_p = (a2dp_connect_ind_msg_t*)msg_data;
			if(msg_p->result == RDABT_NOERROR && !g_avdtp_connect && (g_bt_state != BT_PLAY||g_avrcp_connect==1))
			{
				//Avdtp_Send_Discover(msg_p->cid);
				//g_bt_connected = 1;
				g_avdtp_connect = 1;
				g_a2dp_play = 0;
				//g_avrcp_status = AVRCP_STATUS_ERROR;
				hal_HstSendEvent(SYS_EVENT, 0x1987000a);
#if !defined(NOT_SUPPORT_HFP)
				HF_Get_CallStatus(&call_status);
				hal_HstSendEvent(SYS_EVENT, call_status);
				if(g_bt_state != BT_CALL &&
				   call_status != 0 && call_status != BT_CALL_STATUS_END)
				{
					HF_Get_Clcc();
					BT_Send_Msg_Up(EV_BT_CALL_IND, 0);
				}
#endif
				if(g_bt_state < BT_CONNECTED)
				{
					g_bt_state = BT_CONNECTED;
				}
				g_bt_cur_device = BT_Find_Device(msg_p->bdaddr);
#if defined(CLOSE_BT_SWITCH_MODE)
				if(g_current_module != FUN_BT)
				{
					hal_HstSendEvent(SYS_EVENT, 0x11091249);
					BT_Disconnect_Device();
					COS_Sleep(1500);
					BT_Close_Bluetooth();
					break;
				}
#endif
#if defined(BT_DEVICE_SAVE)
				if(g_current_module == FUN_BT)
				{
#if !defined(NOT_SUPPORT_HFP)
					if(g_hfp_connect==1)
#endif
					BT_Send_Msg_Up(EV_BT_CONNECT_IND, 0);
				}
#endif
				g_bt_device_change = 1;
				g_bt_a2dp_bdaddr = msg_p->bdaddr;
				//BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BTA2DPCONNECTED);
				MGR_SetConnectableMode(FALSE);
				cid = msg_p->cid;
				if(A2DP_SIGNAL_CONNECT_CNF == msg_id)
				{
					//Avdtp_Send_Discover(msg_p->cid);//20140619 解决苹果台式机无法回连
				}
				
				if(g_avrcp_connect == 0 && play_in_noconnect)
				{
					Avrcp_Connect_Req(g_bt_a2dp_bdaddr);
					avrcp_play_pending = 1;
				}
#ifdef PROFILE_CONNECT
#if !defined(NOT_SUPPORT_HFP)
				if(g_hfp_connect == 0 && hfp_con_timer == 0)
				{
					hfp_con_timer = COS_SetTimer(6000, hfp_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
				}
				else
#endif
				{
					if(a2dp_con_timer)
					{
						COS_KillTimer(a2dp_con_timer);
						a2dp_con_timer = 0;
					}
					if(hfp_con_timer)
					{
						COS_KillTimer(hfp_con_timer);
						hfp_con_timer = 0;
					}
				}
#endif
				//  play_start_cnf = TRUE;
			}
			else if(g_bt_connect_pending == 1)
			{
				BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BT_CONNECT_FAIL);
			}
			g_bt_connect_pending = 0;
			set_bt_ops_state(BT_OPS_AVRCP_STATUS_ERROR);
		}
		break;
	case A2DP_SIGNAL_DISCONNECT_IND:
	case A2DP_SIGNAL_DISCONN_CNF:
		hal_HstSendEvent(SYS_EVENT, 0x1988c003);
		BT_StopAudio();
#ifdef PROFILE_CONNECT		
		if(a2dp_con_timer)
		{
			COS_KillTimer(a2dp_con_timer);
			a2dp_con_timer =0;
		}
		if(hfp_con_timer)
		{
			COS_KillTimer(hfp_con_timer);
			hfp_con_timer =0;
		}
#endif		
		bt_connect_reminder = 0;
		
		g_avdtp_connect = 0;
		g_a2dp_play = 0;
		if((g_hfp_connect && (((g_pBT_vars->device_list[g_bt_cur_device].cod >> 8) & 0x1f) == 2)) || g_avrcp_connect)
		{
			hal_HstSendEvent(SYS_EVENT, 0x1987000b);
#if APP_SUPPORT_OPP==1
			if(g_bt_state != BT_FILE)
#endif
				g_bt_state = BT_CONNECTED;
		}
		else
#if APP_SUPPORT_OPP==1
			if(g_bt_state != BT_FILE)
#else
		{
			//            media_PlayInternalAudio(GUI_AUDIO_BT_DISCONNECT, 1, FALSE);
			if(!(g_bt_ops_state & BT_OPS_CLOSED))
			{
				MGR_SetConnectableMode(TRUE);
			}
#if defined(BT_DEVICE_SAVE)
			g_hfp_connect = 0;
			g_avdtp_connect = 0;
			g_avrcp_connect = 0;
			hal_HstSendEvent(SYS_EVENT, 0xbbbbeeee);
			BT_Send_Msg_Up(EV_BT_DISCONNECT_IND, 0);
			g_bt_state = BT_IDLE;
#else
			g_bt_state = BT_IDLE;
#endif
			g_bt_ops_state = 0;
		}
#endif
		a2dp_connect_ind_msg_t *msg_p = (a2dp_connect_ind_msg_t*)msg_data;
		hal_HstSendEvent(SYS_EVENT, 0xbbbbeee1);
		hal_HstSendEvent(SYS_EVENT, msg_p->result);
		if((msg_p->result != 0) && (((g_pBT_vars->device_list[g_bt_cur_device].cod >> 8) & 0x1f) != 2))
		{
			hal_HstSendEvent(SYS_EVENT, 0xbbbbeee2);
			COS_SetTimer(5000, bt_auto_con_a2dp_timeout, NULL, COS_TIMER_MODE_SINGLE);
			//BT_Connect_A2DP(g_bt_cur_device);
		}
		g_bt_device_change = 1;
		MESSAGE_SetEvent(0); // wake up ap task
		
		break;
	case A2DP_SEND_SEP_DISCOVER_IND:
		Avdtp_Response_Discover((UINT32)msg_data);
		break;
	case A2DP_SEND_SEP_GET_CAPABILITIES_IND:
		{
			t_a2dp_capabilities_get_ind *msg_p = (t_a2dp_capabilities_get_ind*)msg_data;
			Avdtp_Response_Get_Capabilities(msg_p->connect_id, msg_p->acp_seid);
		}
		break;
	case A2DP_SEND_SEP_SET_CONFIG_IND:
		{
			memcpy(&g_bt_a2dp_config, msg_data, sizeof(g_bt_a2dp_config));
			if(g_bt_a2dp_config.audio_cap.codec_type != AVDTP_MEDIA_CODEC_SBC
			   && g_bt_a2dp_config.audio_cap.codec_type != AVDTP_MEDIA_CODEC_MPEG1_2_AUDIO
			   && g_bt_a2dp_config.audio_cap.codec_type != AVDTP_MEDIA_CODEC_MPEG2_4_AAC)
			{
				Avdtp_Send_Res(AVDTP_SET_CONFIGURATION, AVDTP_MESSAGE_TYPE_REJECT, avdtp_unsupported_configuration);
			}
			else
			{
				Avdtp_Send_Res(AVDTP_SET_CONFIGURATION, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
			}
		}
		break;
	case A2DP_SEND_SEP_RECONFIG_IND:
		memcpy(&g_bt_a2dp_config, msg_data, sizeof(g_bt_a2dp_config));
		if(g_bt_a2dp_config.audio_cap.codec_type != AVDTP_MEDIA_CODEC_SBC
		   && g_bt_a2dp_config.audio_cap.codec_type != AVDTP_MEDIA_CODEC_MPEG1_2_AUDIO)
		{
			Avdtp_Send_Res(AVDTP_RECONFIGURE, AVDTP_MESSAGE_TYPE_REJECT, avdtp_unsupported_configuration);
		}
		else
		{
			Avdtp_Send_Res(AVDTP_RECONFIGURE, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
		}
		break;
	case A2DP_SEND_SEP_OPEN_IND:
		Avdtp_Send_Res(AVDTP_OPEN, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
		break;
	case A2DP_SEND_SEP_START_IND:
		//hal_HstSendEvent(APP_EVENT, 0x20130d19);
		/*
		if(call_status != BT_CALL_STATUS_NONE)
		{
		    if(2 != Get_Sep_State())
		        Avdtp_Send_Res(AVDTP_START, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
		    COS_Sleep(2000);
		}
		*/
#if APP_SUPPORT_PBAP == 1
#ifdef BT_PBAP_DATA_SAVE
		if(call_status != BT_CALL_STATUS_NONE || phonebook_step >= 2 || history_step >= 2 || g_sco_play == 1)
		{
			hal_HstSendEvent(SYS_EVENT, 0x1988d001);
    	    Avdtp_Send_Res(AVDTP_START, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
            g_a2dp_play = 2;
    		//Avdtp_Send_Res(AVDTP_START, AVDTP_MESSAGE_TYPE_REJECT, avdtp_sep_in_use);
		}
		else
#endif
#else 
		if(call_status != BT_CALL_STATUS_NONE || g_sco_play == 1)
		{
			hal_HstSendEvent(SYS_EVENT, 0x1988d001);
			hal_HstSendEvent(SYS_EVENT, call_status);
			hal_HstSendEvent(SYS_EVENT, g_sco_play);
			if((g_bt_ops_state & BT_OPS_MUSIC_PLAY)&& call_status == BT_CALL_STATUS_END&& g_sco_play == 0)
			{
				clean_bt_ops_state(BT_OPS_MUSIC_PLAY);
				//if(2 != Get_Sep_State())
				Avdtp_Send_Res(AVDTP_START, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
				hal_HstSendEvent(SYS_EVENT, 0x1988d002);
				hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
				hal_HstSendEvent(SYS_EVENT, support_avrcp_status);
				hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
				wait_quit = 0;
			}
			else
			{
				//if(2 != Get_Sep_State())
				Avdtp_Send_Res(AVDTP_START, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
				g_a2dp_play = 0;
				if(call_status != BT_CALL_STATUS_NONE)
                    COS_Sleep(200);
			}
			if(call_status  == BT_CALL_STATUS_NONE 
				||
			    call_status == BT_CALL_STATUS_END)
			{
				if(support_avrcp_status 
					&& 
					g_avrcp_status == AVRCP_STATUS_PLAYING
					&&
					g_bt_state != BT_PLAY)
				{
					is_real_play = TRUE;
			        	BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
			            g_bt_state = BT_PLAY;
				}
			}
		}
		else
#endif
		{
			Avdtp_Send_Res(AVDTP_START, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
			hal_HstSendEvent(SYS_EVENT, 0x1988d003);
			hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
			hal_HstSendEvent(SYS_EVENT, support_avrcp_status);
			hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
			is_real_play = TRUE;
		        	BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
		            g_bt_state = BT_PLAY;
			wait_quit = 0;
		}
		break;
	case A2DP_MEDIA_DATA_IND:
		if(g_a2dp_play&&(GetToneStatus()==0))
		{
			COS_EVENT ev = {0};
			ev.nEventId = MSG_MMC_AUDIODEC_A2DP;
			ev.nParam1 = msg_data;
			COS_SendEvent(MOD_MED,  &ev , COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		}
		else if(g_a2dp_play==0&&(GetToneStatus()==0)&&(g_current_module == FUN_BT)&&((call_status==BT_CALL_STATUS_NONE)||(call_status==BT_CALL_STATUS_END)))
		{
			Avdtp_Clean_Data();
			if(music_playing==0&&musicplay_timer==0)
			{
				musicplay_timer=COS_SetTimer(20, cycle_detect_musicplay, NULL, COS_TIMER_MODE_SINGLE);
			}
		}
		else
		{
			Avdtp_Clean_Data();
		}
		if(g_current_module != FUN_BT)
		{
			break;
		}
#if 1
		app_trace(APP_BT_TRC, "A2DP_MEDIA_DATA_IND");
		app_trace(APP_BT_TRC, "g_bt_state:%d", g_bt_state);
		app_trace(APP_BT_TRC, "g_avrcp_status:%d", g_avrcp_status);
		app_trace(APP_BT_TRC, "g_a2dp_play:%d", g_a2dp_play);
		app_trace(APP_BT_TRC, "support_avrcp_status:%d", support_avrcp_status);
		app_trace(APP_BT_TRC, "g_bt_ops_state:%d", g_bt_ops_state);
		
		if(g_bt_ops_state & BT_OPS_RECALL)
		{
			break;
		}
#if 0		
		if(g_bt_state != BT_PLAY)
		{
			is_real_play = TRUE;
	        	BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
	             g_bt_state = BT_PLAY;  
		}
		if((g_current_module != FUN_BT) && (is_real_play == TRUE) && (g_bt_state == BT_PLAY)) //warkey 2.1
		{
			g_prev_module = g_current_module;
			g_music_playing = TRUE;
			set_bt_ops_state(BT_OPS_MODULE_CHANGE);
			hal_HstSendEvent(APP_EVENT, 0x13121001);
		}
#else//phone_tone_play_return		
		if(!support_avrcp_status && g_avrcp_status == AVRCP_STATUS_ERROR)
		{
			if(g_bt_ops_state & BT_OPS_AVRCP_STATUS_ERROR)
			{
				if(g_avrcp_connect)
				{
					Get_Avrcp_Status();
				}
				clean_bt_ops_state(BT_OPS_AVRCP_STATUS_ERROR);
			}
		}
		
		if(support_avrcp_status && ((g_avrcp_status == AVRCP_STATUS_ERROR)||(g_avrcp_status==AVRCP_STATUS_STOPPED)))
		{
			support_avrcp_status = FALSE;
		}
		
		if(g_bt_state == BT_PLAY)
		{
			is_real_play = TRUE;
		}
		
		if(!g_a2dp_play && (g_bt_state != BT_PLAY)
		   &&
		   (call_status == BT_CALL_STATUS_NONE
		    ||
		    call_status == BT_CALL_STATUS_END)
		   && support_avrcp_status)
		{
			if(g_avrcp_status == AVRCP_STATUS_PLAYING)
				{
			is_real_play = TRUE;
			BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
			g_bt_state = BT_PLAY;
				}
			else
				{
					//NULL packet, do nothing
					hal_HstSendEvent(SYS_EVENT, 0x19885550);
					hal_HstSendEvent(SYS_EVENT, g_current_module);
					hal_HstSendEvent(SYS_EVENT, g_bt_state);
					hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
					hal_HstSendEvent(SYS_EVENT, a2dp_data_flag);
					hal_HstSendEvent(SYS_EVENT, g_bt_ops_state);
					hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
				}
		}
		else if(!support_avrcp_status || g_current_module == FUN_BT)
		{
			if(g_bt_state != BT_PLAY && !g_a2dp_play&&!(g_bt_ops_state & BT_OPS_BT_MODE_EXIT))
			{
				is_real_play = TRUE;
				BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
				g_bt_state = BT_PLAY;
			}
		}
		else
		{
			hal_HstSendEvent(SYS_EVENT, 0x19885500);
			hal_HstSendEvent(SYS_EVENT, g_current_module);
			hal_HstSendEvent(SYS_EVENT, g_bt_state);
			hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
			hal_HstSendEvent(SYS_EVENT, a2dp_data_flag);
			hal_HstSendEvent(SYS_EVENT, g_bt_ops_state);
			hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
			if(g_current_module != FUN_BT
			   &&
			   !(g_bt_ops_state & BT_OPS_BT_MODE_EXIT)
			   &&
			   support_avrcp_status
			   &&
			   g_avrcp_status == AVRCP_STATUS_STOPPED)
			{
#if 1
				if(!a2dp_data_timer)
				{
					a2dp_data_timer = COS_SetTimer(4000, a2dp_data_timeout, NULL, COS_TIMER_MODE_SINGLE);
				}
#else//warkey 2.1
				a2dp_data_flag = TRUE;
#endif
				
				if(g_bt_state != BT_PLAY && !g_a2dp_play && a2dp_data_flag)
				{
					if((support_avrcp_status && g_avrcp_status == AVRCP_STATUS_PLAYING) || (support_avrcp_status == FALSE)) //warkey 2.1
					{
						is_real_play = TRUE;
						BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
						g_bt_state = BT_PLAY;
					}
				}
				else
				{
					a2dp_data_flag = FALSE;
				}
			}
			else
			{
				//Get_Avrcp_Status();
				//Avdtp_Clean_Data();
				is_real_play = TRUE;
				BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
				g_bt_state = BT_PLAY;
			}
		}
		if((g_current_module != FUN_BT) && (is_real_play == TRUE) && (g_bt_state == BT_PLAY)) //warkey 2.1
		{
			g_prev_module = g_current_module;
			g_music_playing = TRUE;
			set_bt_ops_state(BT_OPS_MODULE_CHANGE);
			hal_HstSendEvent(APP_EVENT, 0x13121001);
		}
#endif
#endif
		break;
	case A2DP_SEND_SEP_CLOSE_IND:
		hal_HstSendEvent(SYS_EVENT, 0x1987000c);
		if(g_bt_state >= BT_CONNECTED)
		{
			g_bt_state = BT_CONNECTED;
		}
		hal_HstSendEvent(SYS_EVENT, 0x1988c002);
		BT_StopAudio();
		Avdtp_Send_Res(AVDTP_CLOSE, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
		break;
	case A2DP_SEND_SEP_PAUSE_IND:
		set_bt_ops_state(BT_OPS_RECV_PAUSE);
		hal_HstSendEvent(SYS_EVENT, 0x1987000d);
		hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
		hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
		short_play_status = FALSE;
		Avdtp_Send_Res(AVDTP_SUSPEND, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
		if(old_phone_timer_handle)
		{
			COS_KillTimer(old_phone_timer_handle);
			old_phone_timer_handle = 0;
		}
		a2dp_data_flag = FALSE;
		if(a2dp_data_timer)
		{
			COS_KillTimer(a2dp_data_timer);
			a2dp_data_timer = 0;
		}
		if(g_bt_state == BT_PLAY)
		{
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
			g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
		}
		
#if APP_SUPPORT_OPP==1
		if(have_recv_file_size < recv_file_size)
		{
			g_bt_state = BT_FILE;
		}
#endif
		if(is_real_play)
		{
			set_bt_ops_state(BT_OPS_MUSIC_PAUSE);
		}
		//play_start_cnf = TRUE;
		hal_HstSendEvent(SYS_EVENT, 0x1988c001);
		BT_StopAudio();
		hal_HstSendEvent(SYS_EVENT, g_sco_play);
		//if(!is_real_play)
		//    Avrcp_Send_Key(OPID_PLAY);
#if APP_SUPPORT_PBAP==1
#ifdef BT_PBAP_DATA_SAVE
		if(g_bt_ops_state & BT_OPS_PBAP_PHONEBOOK_REQ)
		{
			clean_bt_ops_state(BT_OPS_PBAP_PHONEBOOK_REQ);
			common_var_init();
			COS_Sleep(100);
			BT_Phone_Book();
		}
		else if(g_bt_ops_state & BT_OPS_PBAP_HISTORY_REQ)
		{
			clean_bt_ops_state(BT_OPS_PBAP_HISTORY_REQ);
			common_var_init();
			COS_Sleep(100);
			BT_Call_History();
		}
		else if(g_bt_ops_state & BT_OPS_MAP_MSG_REQ)
		{
			clean_bt_ops_state(BT_OPS_MAP_MSG_REQ);
			common_var_init();
			COS_Sleep(100);
			BT_Map();
		}
#endif
#endif
		break;
	case A2DP_SEND_SEP_ABORT_IND:
		hal_HstSendEvent(SYS_EVENT, 0x1987000e);
#if APP_SUPPORT_OPP==1
		if(g_bt_state != BT_FILE)
#endif
	 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
			g_bt_state = BT_CONNECTED;
			else 
							g_bt_state = BT_IDLE;
		hal_HstSendEvent(SYS_EVENT, 0x1988c000);
		BT_StopAudio();
		Avdtp_Send_Res(AVDTP_ABORT, AVDTP_MESSAGE_TYPE_ACCEPT, 0);
		break;
	case AVRCP_CONNECT_IND:
		{
			t_avrcp_connect_inf *msg = (t_avrcp_connect_inf*)msg_data;
			if(msg->result != RDABT_NOERROR)
			{
				avrcp_play_pending = 0;
				break;
			}
		}
#if defined(CLOSE_BT_SWITCH_MODE)
		if(g_current_module != FUN_BT)
		{
			hal_HstSendEvent(SYS_EVENT, 0x11091250);
			BT_Disconnect_Device();
			COS_Sleep(1500);
			BT_Close_Bluetooth();
			break;
		}
#endif
		if(avrcp_play_pending)
			//	&& (g_avrcp_status == AVRCP_STATUS_PLAYING
			//	|| !support_avrcp_status))
		{
			hal_HstSendEvent(SYS_EVENT, 0x19886410);
			//hal_HstSendEvent(SYS_EVENT, play_start_cnf);
			//if(play_start_cnf)
			{
				Avrcp_Send_Key(OPID_PLAY);
				short_play_status = TRUE;
				play_in_noconnect = FALSE;
				BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
				//    play_start_cnf = FALSE;
			}
			//BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
		}
		g_avrcp_connect = 1;
		avrcp_play_pending = 0;
		break;
	case AVRCP_DISCONNECT_CNF:
		hal_HstSendEvent(SYS_EVENT, 0x1988900a);
	case AVRCP_DISCONNECT_IND:
		hal_HstSendEvent(SYS_EVENT, 0x19889001);
		g_avrcp_connect = 0;
		avrcp_play_pending = 0;
		if((g_hfp_connect && (((g_pBT_vars->device_list[g_bt_cur_device].cod >> 8) & 0x1f) == 2)) || g_avdtp_connect)
		{
			hal_HstSendEvent(SYS_EVENT, 0x1987000f);
#if APP_SUPPORT_OPP==1
			if(g_bt_state != BT_FILE)
#endif
		 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
				g_bt_state = BT_CONNECTED;
			else
							g_bt_state = BT_IDLE;

		}
		else
#if defined(BT_DEVICE_SAVE)
		{
			g_hfp_connect = 0;
			g_avdtp_connect = 0;
			g_avrcp_connect = 0;
			hal_HstSendEvent(SYS_EVENT, 0xbbbbdddd);
			BT_Send_Msg_Up(EV_BT_DISCONNECT_IND, 0);
			MGR_SetConnectableMode(TRUE);
			g_bt_state = BT_IDLE;
		}
#else
		{
			if(!(g_bt_ops_state & BT_OPS_CLOSED))
			{
				MGR_SetConnectableMode(TRUE);
			}
			g_bt_state = BT_IDLE;
		}
#endif
		
		if(g_bt_connect_pending > 0)
		{
			BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BT_CONNECT_FAIL);
		}
		g_bt_connect_pending = 0;
		break;
	case AVRCP_PLAYER_STATUS_IND:
		{
			extern u_int32 *inputbuf_overlay;
			u_int32 sbc_length = 0;
			
			t_avrcp_status_ind *msg = (t_avrcp_status_ind*)msg_data;
#if 0//warkey 2.1 //phone_tone_play_return	
			if((g_current_module != FUN_BT) && (g_avrcp_status != AVRCP_STATUS_PLAYING && msg->status == AVRCP_STATUS_PLAYING))
			{
				g_prev_module = g_current_module;
				g_music_playing = TRUE;
				hal_HstSendEvent(APP_EVENT, 0x13121003);
				set_bt_ops_state(BT_OPS_MODULE_CHANGE);
			}
#endif
			g_avrcp_status = msg->status;
			hal_HstSendEvent(SYS_EVENT, 0x1988e000);
			hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
			hal_HstSendEvent(SYS_EVENT, support_avrcp_status);
			hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
			hal_HstSendEvent(SYS_EVENT, g_bt_state);
			hal_HstSendEvent(SYS_EVENT, call_status);
			
			support_avrcp_status = TRUE;
			if(g_avrcp_status == AVRCP_STATUS_PAUSED)
			{
				set_bt_ops_state(BT_OPS_RECV_PAUSE);
				short_play_status = FALSE;
#if APP_SUPPORT_PBAP==1
#ifdef BT_PBAP_DATA_SAVE
				if(g_bt_ops_state & BT_OPS_PBAP_PHONEBOOK_REQ)
				{
					clean_bt_ops_state(BT_OPS_PBAP_PHONEBOOK_REQ);
					common_var_init();
					COS_Sleep(100);
					BT_Phone_Book();
				}
				else if(g_bt_ops_state & BT_OPS_PBAP_HISTORY_REQ)
				{
					clean_bt_ops_state(BT_OPS_PBAP_HISTORY_REQ);
					common_var_init();
					COS_Sleep(100);
					BT_Call_History();
				}
				else if(g_bt_ops_state & BT_OPS_MAP_MSG_REQ)
				{
					clean_bt_ops_state(BT_OPS_MAP_MSG_REQ);
					common_var_init();
					COS_Sleep(100);
					BT_Map();
				}
#endif
#endif	
			}
			else if(g_avrcp_status == AVRCP_STATUS_PLAYING)
			{
				short_play_status = TRUE;
				if(g_avdtp_connect==0)
				{
					Avdtp_Send_Start();
				}
			}
			
#if 0//phone_tone_play_return			
			if(g_bt_ops_state & BT_OPS_AVRCP_STATUS_ERROR)
			{
				support_avrcp_status = FALSE;
				g_avrcp_status = AVRCP_STATUS_ERROR;
				clean_bt_ops_state(BT_OPS_AVRCP_STATUS_ERROR);
				break;
			}
#endif
#if 1
			if((g_avrcp_status == AVRCP_STATUS_PLAYING
			    || !support_avrcp_status))
			{
				if((g_a2dp_play == 0 && g_bt_state != BT_PLAY && call_status == BT_CALL_STATUS_NONE)
					||g_current_module==FUN_BT)
				{
					//		           	 	Avdtp_Clean_Data();
					hal_HstSendEvent(SYS_EVENT, 0x1988e002);
					//					if(sbc_length == 0)
					{
						if(2 != Get_Sep_State())
						{
							hal_HstSendEvent(SYS_EVENT, 0x1988c00e);
						}
						BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
						is_real_play = TRUE;
						g_bt_state = BT_PLAY;
					}
				}
			}
			if((g_current_module != FUN_BT) && (is_real_play == TRUE) && (g_bt_state == BT_PLAY)) //warkey 2.1
		{
			g_prev_module = g_current_module;
			g_music_playing = TRUE;
			set_bt_ops_state(BT_OPS_MODULE_CHANGE);
			hal_HstSendEvent(APP_EVENT, 0x13121001);
			}
#endif
		}
		break;
	case A2DP_SEND_SEP_DISCOVER_CNF:
		{
			u_int8 *msg_p = (u_int8*)msg_data;
			int i, j;
			u_int8 data_length = msg_p[0];
			
			msg_p += 3;
			seid = 0;
			for (i = j = 0;  i < (data_length - 2); i += 2)
			{
				if((msg_p[i + 1] >> 4) == BT_A2DP_MEDIA_AUDIO && ((msg_p[i + 1] >> 3) & 1) == BT_A2DP_SOURCE && ((msg_p[i] >> 1) & 1) == 0)
				{
					seid = ((msg_p[i]) >> 2);
					break;
				}
			}
			
			if(seid == 0)
			{
				AVDTP_Disconnect();
			}
			else
			{
				Avdtp_Send_GetCapabilities(seid, cid);
			}
		}
		break;
	case A2DP_SEND_SEP_GET_CAPABILITIES_CNF:
		{
			UINT8 *msg_p = (UINT8 *)msg_data;
			UINT8 num = 0, losc = 0;
			msg_p += 2; //skip the packet type, message type, and singnaling ID
			while(*msg_p != AVDTP_SERVICE_MEDIA_CODEC && num < 7)
			{
				losc = *(msg_p + 1);
				if(losc != 0)
				{
					msg_p += (losc + 2);
				}
				else
				{
					msg_p += 2;
				}
				num++;
			}
			
			msg_p ++;
			losc = *msg_p;
			msg_p++;
			
			if(((msg_p[2] & 0xF0) >> 4) & 0x02)
			{
				codec_cap.sbc.sample_rate = 0x02;
			}
			else
			{
				codec_cap.sbc.sample_rate = 0x01;
			}
			hal_HstSendEvent(SYS_EVENT, 0x19880701);
			hal_HstSendEvent(SYS_EVENT, msg_p[2]);
			hal_HstSendEvent(SYS_EVENT, codec_cap.sbc.sample_rate);
			Avdtp_Send_SetConfiguration(cid, seid, seid, A2DP_SBC, &codec_cap);
		}
		break;
	case A2DP_SEND_SEP_SET_CONFIG_CNF:
		{
			u_int8 *msg_p = (u_int8*)msg_data;
			u_int8 result = (msg_p[0] & 3);
			if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
			{
				Avdtp_Send_Open(seid);
			}
			else
			{
				AVDTP_Disconnect();
			}
		}
		break;
	case A2DP_SEND_SEP_OPEN_CNF:
		{
			AVDTP_Start_Media_Channel_Connection();
		}
		break;
	case A2DP_SEND_SEP_PAUSE_CNF:
		{
			u_int8 *resp = (u_int8*)msg_data;
			hal_HstSendEvent(SYS_EVENT, 0x19870010);
			hal_HstSendEvent(SYS_EVENT, resp[0]);
			if((resp[0] & 0x0f) == AVDTP_MESSAGE_TYPE_ACCEPT)
			{
            	    if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
				g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
#if APP_SUPPORT_OPP==1
				if(have_recv_file_size < recv_file_size)
				{
					g_bt_state = BT_FILE;
				}
#endif
				set_bt_ops_state(BT_OPS_MUSIC_PAUSE);
			}
		}
		break;
	case A2DP_SEND_SEP_CLOSE_CNF:
		hal_HstSendEvent(SYS_EVENT, 0x20130713);
		AVDTP_Disconnect();
		BT_StopAudio();
		g_avdtp_connect = 0;
		break;
	case A2DP_SEND_SEP_START_CNF:
		{
			u_int8 *resp = (u_int8*)msg_data;
			hal_HstSendEvent(SYS_EVENT, 0x19870020);
			hal_HstSendEvent(SYS_EVENT, resp[0]);
			//hal_HstSendEvent(SYS_EVENT, play_start_cnf);
			//play_start_cnf = TRUE;
			if((resp[0] & 0x0f) == AVDTP_MESSAGE_TYPE_ACCEPT)
			{
				g_bt_state = BT_PLAY;
				wait_quit = 0;
				if(g_a2dp_play == 0 && call_status == BT_CALL_STATUS_NONE)
				{
					BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
				}
			}
		}
		break;
	default:
		break;
	}
	if(g_avdtp_connect)
	{
		Set_Sep_State_Playing();
	}
}


void BT_RegisterHeadsetService(void)
{
#if !defined(NOT_SUPPORT_HFP)
	HF_Activate(ACTIVE_PROFILE_HANDSFREE | ACTIVE_PROFILE_HEADSET);
#endif
	Avrcp_RegisterService(AVRCP_CONTROLLER);
	Avdtp_Register_Service(SEP_SINK);
	Avdtp_Register_Sep(A2DP_SBC, SEP_SINK, sbc_caps_sink, sizeof(sbc_caps_sink));
	Avdtp_Register_Sep(A2DP_MPEG_AUDIO, SEP_SINK, NULL, 0);
	
	//Avdtp_Register_Sep(A2DP_MPEG_AAC, SEP_SINK, NULL, 0);
	//Avdtp_Register_Sep(A2DP_MPEG_AAC, SEP_SINK, NULL, 0);
	g_avrcp_connect = 0;
}

#endif



