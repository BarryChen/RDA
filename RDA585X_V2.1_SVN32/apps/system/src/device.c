

/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_common.h"
#include "ap_bluetooth.h"
#include "gui_const.h"
#include "device.h"
#ifndef WIN32
#include "pmd_m.h"
#include "sxs_io.h"
#else
#define PMD_LEVEL_LCD 1
#endif

#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1
#include "ap_message.h"
#define   CLIP_LOWBAT                         3550//mv
#define   CANCEL_CLIP_LOWBAT           3750//mv
#define   SHUTDOWN_LOWBAT              3400//mv
#define   FULL_POWER              4050//mv
#define   LOWBAT_DETECT_TIME          30//s
#define   LOWBAT_CYCLE_CLIP_TIME   90//s
#define   LOWBAT_SHUTDOWN_TIME    20//s
#define   CHARGE_DETECT_TIME           15//s
#define   FULL_POWER_DETECT_TIME   180//s
//static int forbid_led_flag=0;//低电禁止控制LED
//static int lowbat_index,lowbat_mode,lowbat_times,lowbat_interval;
#endif
BOOL  g_cardexistornot = FALSE;
INT8 g_backLight = 0;
UINT8 g_cur_volume = 0;
extern BOOL tone_status;//warkey 2.0
BOOL bt_close_flag = 0; //warkey 2.0
//static int led_interval[LED_COUNT]={0};//warkey
//static int led_modify_flag[LED_COUNT]={0};//warkey
//static int led_active_status[LED_COUNT]={1,1,1};//warkey
//static int led_delay_interval[LED_COUNT]={0};//warkey
//static INT32 last_index=-1,last_mode=-1,last_times=-1,last_interval=-1;//warkey
//struct {
//    BYTE count;
//    BYTE mode;
//} g_leds[LED_COUNT];

extern void  hal_SpiCodecVolume(UINT8 vol);
extern void *DSM_GetUserData(UINT8 index, UINT32 length);
extern INT32 DSM_WriteUserData(void);
extern INT32 DSM_UserDataClear(VOID);
extern VOID DSM_Upgrade_Flash(INT file, UINT8 *BTADDR);
extern VOID bal_SetSleepState(BOOL sleep);
extern VOID gpio_SetLED(int index, int on);

/*********************************************************************************
* Description : 打开a/d转换
*
* Arguments   :
*
* Notes       :
*
*********************************************************************************/
#if 1//warkey 2.0
void SetBtCloseFlag(BOOL flag)
{
	if(flag > 1 || flag < 0)
	{
		return;
	}
	bt_close_flag = flag;
	return;
}

BOOL GetBtCloseFlag()
{
	return bt_close_flag;
}

BOOL SetTONEVolume(INT32 volume)
{
	if(volume > 0)
	{
		DM_SetAudioVolume(volume);
	}
	else
	{
		DM_SetAudioVolume(g_cur_volume);
	}
	return TRUE;
}

BOOL GetToneStatus()
{
	return tone_status;
}

VOID SetPA_For_TonePlay()
{
	gpio_set_tone_status(TRUE);
}

VOID SetPA_For_ToneStop()
{
	gpio_set_tone_status(FALSE);
}
#endif
//设置音量
BOOL SetPAVolume(INT32 volume)
{
	if (volume == 0 && g_cur_volume != 0)
	{
		gpio_SetMute(TRUE);//静音
	}
	DM_SetAudioVolume(volume);
	if (volume > 0 && g_cur_volume == 0)
	{
		gpio_SetMute(FALSE);//放开静音
	}
	g_cur_volume = volume; //bak vol
	return TRUE;
}

//设置音量不控制功放
BOOL SetInternalPAVolume(INT32 volume)
{
	DM_SetAudioVolume(volume);
	g_cur_volume = volume; //bak vol
	return TRUE;
}

void SetPAMute(BOOL mute)
{
	if(mute)
	{
		DM_SetAudioVolume(0);
	}
	else
	{
		DM_SetAudioVolume(g_cur_volume);
	}
}

static UINT8 g_sleep_state = 0;

void APP_Sleep(void)
{
	return;
	extern int   audioItf;
	app_trace(APP_MAIN_TRC, "Enter sleep mode");
#ifndef WIN32
	if(g_sleep_state)
	{
		return;
	}
	SetPAMute(TRUE);
	g_sleep_state = 1;
	aud_CodecCommonStreamStop(audioItf);
	//gpio_SetLED(0);
	hal_AudDeepPowerDown();
	// Power off audio module
	hal_AudForcePowerDown();
	// Disable GPADC
	pmd_GpadcDisable();
	// Restore charger h/w setting
	//pmd_RestoreChargerAtPowerOff();
	//for(int i=1; i<=10;i++)COS_KillTimer(i);
#if APP_SUPPORT_LCD==1
	CloseBacklight();
	lcdd_Sleep();
#endif
#ifdef MCD_TFCARD_SUPPORT
	mcd_Close();
#endif
	//hal_UartClose(1);
	//  pmd_SetPowerMode(0);//PMD_LOWPOWER);
	//hal_LpsDeepSleep(0);
	hal_TimWatchDogClose();
	bal_SetSleepState(TRUE);
	hal_SysRequestFreq(0, 32768, NULL);//sys clock HAL_SYS_FREQ_32K
#endif
}

BOOL APP_Wakeup(void)
{
	return;
	BOOL result = FALSE;
#ifndef WIN32
	if(g_sleep_state)
	{
		SetPAMute(FALSE);
		app_trace(APP_MAIN_TRC, "Wakeup from sleep mode");
		pmd_SetPowerMode(1);
		bal_SetSleepState(FALSE);
		hal_SysRequestFreq(0, 104000000, NULL);//HAL_SYS_FREQ_104M
        //hal_TimWatchDogOpen(10 SECOND);
#if APP_SUPPORT_LCD==1
		lcdd_WakeUp();
#endif
		result = TRUE;
	}
	g_sleep_state = 0;
	//OpenBacklight();
#endif
	return result;
}

BOOL Check_CardOrUsb(void)//检查卡是否存在，或者是否有U盘
{
	return g_cardexistornot;
}

#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1
static bool low_bat_begin = FALSE;
static bool low_bat_led_refresh = FALSE;
void Clean_Low_Bat_Refresh()
{
	low_bat_led_refresh = FALSE;
}

bool Get_Low_Bat_Refresh()
{
	return low_bat_led_refresh;
}

bool Get_Low_Bat_Status()
{
	return low_bat_begin;
}
static UINT8 save_led_id = 0, save_led_count = 0;
#endif

#if XDL_APP_SUPPORT_TONE_TIP==1
UINT8 lowpower_flag_for_tone = 0; //0--报警提示音1--低电提示音2--充电完成提示音4--关机提示音5--蓝牙连接成功提示音
#endif
INT32 GetBattery(void)
{
	UINT8 percent = 0, charging = 0, mpc = 0;
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1
	UINT16 batLevel;
	static int charge_flag = 0, full_flag = 0, charge_count = 0;
	static int clip_voltage_detect_count = 0, poweroff_voltage_detect_count = 0, charge_voltage_detect_count, lowpower_flag = 0;
	
	batLevel = pmd_GetBatteryLevel(&percent);
	//hal_HstSendEvent(SYS_EVENT,0x55555555);
	//hal_HstSendEvent(SYS_EVENT,charging);
	//hal_HstSendEvent(SYS_EVENT,percent);
	//hal_HstSendEvent(SYS_EVENT,batLevel);
	if(lowpower_flag == 0)
	{
		if(batLevel < CLIP_LOWBAT)
		{
			clip_voltage_detect_count++;
			if(clip_voltage_detect_count > LOWBAT_DETECT_TIME) //低电开始闪灯提示
			{
				hal_HstSendEvent(SYS_EVENT, 0x55555555);
				lowpower_flag = 1;
				clip_voltage_detect_count = 0;
				LED_SetPattern(GUI_LED_LOWPOWER, LED_LOOP_INFINITE);
#if XDL_APP_SUPPORT_TONE_TIP==1
				lowpower_flag_for_tone = 1;
				MESSAGE_SetEvent(0xaaaa1111);
#else
				media_PlayInternalAudio(GUI_AUDIO_LOWPOWER, 1);
#endif
				//lowbat_index=last_index;
				//lowbat_mode=last_mode;
				//lowbat_times=last_times;
				//lowbat_interval=last_interval;
				//SetLedStatus_Flash(LED_MP3,1,0,2);
				//SetLedStatus_Flash(LED_FM,1,0,2);
				//SetLedStatus_Flash(LED_BT,1,0,2);
				//forbid_led_flag=1;
			}
		}
		else
		{
			if(charge_flag == 1)
			{
				if(full_flag == 0)
				{
					if(batLevel >= FULL_POWER)
					{
						charge_count++;
						if(charge_count >= FULL_POWER_DETECT_TIME)
						{
							full_flag = 1;
							charge_count = 0;
							hal_HstSendEvent(SYS_EVENT, 0x88888888);
#if XDL_APP_SUPPORT_TONE_TIP==1
							lowpower_flag_for_tone = 2;
							MESSAGE_SetEvent(0xaaaa1111);
#else
							MESSAGE_SetEvent(0x66666);
#endif
						}
					}
					else
					{
						charge_count = 0;
					}
				}
				else
				{
					if(batLevel < 4000)
					{
						charge_count++;
						if(charge_count >= 600)
						{
							full_flag = 0;
							charge_count = 0;
						}
					}
					else
					{
						charge_count = 0;
					}
				}
			}
			else
			{
				charge_count = 0;
				if(full_flag)
				{
					full_flag = 0;
				}
			}
			clip_voltage_detect_count = 0;
		}
	}
	else
	{
		if(batLevel < SHUTDOWN_LOWBAT)
		{
			if(charge_voltage_detect_count > 0)
			{
				charge_voltage_detect_count = 0;
			}
			poweroff_voltage_detect_count++;
			if(poweroff_voltage_detect_count > LOWBAT_SHUTDOWN_TIME) //低电发关机消息
			{
				hal_HstSendEvent(SYS_EVENT, 0x44444444);
				poweroff_voltage_detect_count = 0;
				//forbid_led_flag=0;
				//SetLedStatus_Flash(LED_MP3,2,0,0);
				//SetLedStatus_Flash(LED_FM,2,0,0);
				//SetLedStatus_Flash(LED_BT,2,0,0);
				MESSAGE_SetEvent(AP_KEY_POWER | AP_KEY_PRESS);
			}
		}
		else if(batLevel > CANCEL_CLIP_LOWBAT)
		{
			if(poweroff_voltage_detect_count)
			{
				poweroff_voltage_detect_count = 0;
			}
			charge_voltage_detect_count++;
			if(charge_voltage_detect_count > CHARGE_DETECT_TIME) //检测到电压上升,应该是充电器进来了,关掉低电提示
			{
				hal_HstSendEvent(SYS_EVENT, 0x66666666);
				lowpower_flag = 0;
				charge_voltage_detect_count = 0;
				//forbid_led_flag=0;
				//SetLedStatus_Flash(LED_MP3,2,0,0);
				//SetLedStatus_Flash(LED_FM,2,0,0);
				//SetLedStatus_Flash(LED_BT,2,0,0);
				//SetLedStatus_Flash(lowbat_index,lowbat_mode,lowbat_times,lowbat_interval);
			}
		}
		else
		{
			clip_voltage_detect_count++;
			if(clip_voltage_detect_count > LOWBAT_CYCLE_CLIP_TIME)
			{
				clip_voltage_detect_count = 0;
				hal_HstSendEvent(SYS_EVENT, 0x55555555);
#if XDL_APP_SUPPORT_TONE_TIP==1
				lowpower_flag_for_tone = 1;
				MESSAGE_SetEvent(0xaaaa1111);
#else
				media_PlayInternalAudio(GUI_AUDIO_LOWPOWER, 1);
#endif
			}
			charge_voltage_detect_count = 0;
			poweroff_voltage_detect_count = 0;
		}
	}
	if(low_bat_begin != lowpower_flag) //warkey 2.1
	{
		low_bat_begin = lowpower_flag;
		if(low_bat_begin == FALSE)
		{
			low_bat_led_refresh = TRUE;
			if(save_led_count == -1)
			{
				LED_SetBgPattern(save_led_id);
			}
			else if(save_led_count == LED_LOOP_INFINITE)
			{
				LED_SetPattern(save_led_id, save_led_count);
			}
			save_led_id = 0;
			save_led_count = 0;
		}
	}
	PM_GetBatteryInfo(&charging, &percent, &mpc);
	//   app_trace(APP_MAIN_TRC, "GetBattery charge:%d, percent=%d", charging, percent);
	if(charging && !charge_flag && !full_flag)
	{
		charge_flag = 1;
		hal_HstSendEvent(SYS_EVENT, 0x130902ca);
		lowpower_flag = 0;
		charge_voltage_detect_count = 0;		
	}
	else if(!charging)
	{
		if(charge_flag)
		{
			charge_flag = 0;
			hal_HstSendEvent(SYS_EVENT, 0x130902cc);
		}
	}
	else
	{
		//hal_HstSendEvent(SYS_EVENT,0x130902ce);
	}
	if(charging && percent < 100)
	{
		return -1;
	}
	else
	{
		return percent;
	}
#else
	PM_GetBatteryInfo(&charging, &percent, &mpc);
	//   app_trace(APP_MAIN_TRC, "GetBattery charge:%d, percent=%d", charging, percent);
	if(charging && percent < 100)
	{
		return -1;
	}
	else
	{
		return percent;
	}
#endif
}

BOOL SetBackLight(INT8 BLight)//设置背光亮度。
{
#if APP_SUPPORT_LCD==1
	if(AP_Support_LCD())
	{
		g_backLight = BLight;
		pmd_SetLevel(2, BLight);
	}
#endif
	return TRUE;
}
BOOL OpenBacklight(void)//打开背光
{
#if APP_SUPPORT_LCD==1
	if(AP_Support_LCD())
	{
		pmd_SetLevel(PMD_LEVEL_LCD, g_backLight);
	}
#endif
	return TRUE;
}
BOOL CloseBacklight(void)	//关闭背光
{
#if APP_SUPPORT_LCD==1
	if(AP_Support_LCD())
	{
		pmd_SetLevel(PMD_LEVEL_LCD, 0);
	}
#endif
	return TRUE;
}

#if APP_SUPPORT_LED==1
UINT8 *g_led_cur_pattern = NULL;
UINT8 g_led_cur_count = 0;
UINT8 g_led_cur_pos = 0;
UINT8 g_led_cur_length = 0;
UINT8 g_led_cur_pattern_id = 0;
UINT8 g_led_cur_timer_id = 0;

// background led pattern, for bluetooth
UINT8 *g_led_bg_pattern = NULL;
UINT8 g_led_bg_count = 0;
UINT8 g_led_bg_pattern_id = GUI_LED_BT_CLOSED;
UINT8 g_led_bg_pos = 0;
UINT8 g_led_bg_length = 0;
UINT8 g_led_bg_timer_id = 0;

void LedSetCurPattern(INT8 pattern_id)
{
	UINT32 length, i;
	UINT8 *pattern;
	pattern = gui_get_led_pattern(pattern_id, &length);
	
	g_led_cur_pattern = pattern;
	g_led_cur_pos = 0;
	g_led_cur_length = strlen(pattern);
	g_led_cur_pattern_id = pattern_id;
}

void LedTimeout(void *param)
{
	int delay;
	char c;
	
	do
	{
		delay = 0;
		if(g_led_cur_pattern[g_led_cur_pos] == 0 || g_led_cur_pos >= g_led_cur_length) // one loop end
		{
			if(g_led_cur_count <= 1)
			{
				break;
			}
			if(LED_LOOP_INFINITE != g_led_cur_count)
			{
				g_led_cur_count --;
			}
			g_led_cur_pos = 0;
		}
		c = g_led_cur_pattern[g_led_cur_pos];
		if(c >= '0' && c <= '9') // delay
		{
			delay = (c - '0') * 10 + g_led_cur_pattern[g_led_cur_pos + 1] - '0';
			g_led_cur_pos += 2;
			break;
		}
		else if(c >= 'A' && c <= 'H')
		{
			// open led
			gpio_SetLED(c - 'A', 1);
		}
		else if(c >= 'a' && c <= 'h')
		{
			// close led
			gpio_SetLED(c - 'a', 0);
		}
		g_led_cur_pos ++;
	}
	while(1);
	
	if(delay != 0)
	{
		g_led_cur_timer_id = COS_SetTimer(delay * 100, LedTimeout, param, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		g_led_cur_timer_id = 0;
	}
}


void LED_SetPattern(UINT8 pattern_id, UINT8 count)
{
	UINT32 length, i;
	UINT8 *pattern;
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
	if(Get_Low_Bat_Status())
	{
		if(pattern_id==GUI_LED_NONE&&count==0xde)//关机灭LED
		{
			hal_HstSendEvent(SYS_EVENT, 0x14070301);
			count=1;
		}
		else
		{
			if(count == LED_LOOP_INFINITE)
			{
				save_led_id = pattern_id;
				save_led_count = count;
			}
			return;
		}
	}
#endif
	if(pattern_id == g_led_cur_pattern_id)
	{
		return;
	}
	pattern = gui_get_led_pattern(pattern_id, &length);
	if(pattern == NULL || length == 0 || strlen(pattern) == 0)
	{
		return;
	}
	
	// check pattern for loop
	if(count > 1)
	{
		for(i = 0; i < length; i++)
		{
			if(pattern[i] > '0' && pattern[i] <= '9') // have a number greater than 0
			{
				break;
			}
		}
		if(i == length) // if no delay, force count to 1
		{
			count = 1;
		}
	}
	LedSetCurPattern(pattern_id);
	g_led_cur_count = count;
	
	if(g_led_cur_timer_id != 0)
	{
		COS_KillTimer(g_led_cur_timer_id);
		g_led_cur_timer_id = 0;
	}
	
	if(g_led_bg_timer_id != 0)         // added by gary   led light
	{
		COS_KillTimer(g_led_bg_timer_id);
		g_led_bg_timer_id = 0;
	}
	LedTimeout(NULL);
}

void LedBgTimeout(void *param)
{
	int delay;
	char c;
	
	do
	{
		delay = 0;
		if(g_led_bg_pattern[g_led_bg_pos] == 0 || g_led_bg_pos >= g_led_bg_length) // one loop end
		{
			if(g_led_bg_count <= 1)
			{
				break;
			}
			if(LED_LOOP_INFINITE != g_led_bg_count)
			{
				g_led_bg_count --;
			}
			g_led_bg_pos = 0;
		}
		c = g_led_bg_pattern[g_led_bg_pos];
		if(c >= '0' && c <= '9') // delay
		{
			delay = (c - '0') * 10 + g_led_bg_pattern[g_led_bg_pos + 1] - '0';
			g_led_bg_pos += 2;
			break;
		}
		else if(c >= 'A' && c <= 'H')
		{
			// open led
			gpio_SetLED(c - 'A', 1);
		}
		else if(c >= 'a' && c <= 'h')
		{
			// close led
			gpio_SetLED(c - 'a', 0);
		}
		g_led_bg_pos ++;
	}
	while(1);
	
	if(delay != 0)
	{
		g_led_bg_timer_id = COS_SetTimer(delay * 100, LedBgTimeout, param, COS_TIMER_MODE_SINGLE);
	}
	else
	{
		g_led_bg_timer_id = 0;
	}
}

void LED_SetBgPattern(UINT8 pattern_id)
{
	UINT32 length, i;
	UINT8 *pattern;
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
	if(Get_Low_Bat_Status())
	{
		save_led_id = pattern_id;
		save_led_count = -1;
		return;
	}
#endif
	if(pattern_id == g_led_bg_pattern_id)				//delete by gary
	{
		return;    //delete by gary
	}
	pattern = gui_get_led_pattern(pattern_id, &length);
	if(pattern == NULL || length == 0 || strlen(pattern) == 0)
	{
		return;
	}
	
	g_led_bg_pattern = pattern;
	g_led_bg_pos = 0;
	g_led_bg_length = strlen(pattern);
	g_led_bg_pattern_id = pattern_id;
	
	for(i = 0; i < length; i++)
	{
		if(pattern[i] > '0' && pattern[i] <= '9') // have a number greater than 0
		{
			break;
		}
	}
	if(i == length) // if no delay, set count to 1
	{
		g_led_bg_count = 1;
	}
	else
	{
		g_led_bg_count = LED_LOOP_INFINITE;
	}
	
	if(g_led_bg_timer_id != 0)
	{
		COS_KillTimer(g_led_bg_timer_id);
		g_led_bg_timer_id = 0;
	}
	
	if(g_led_cur_timer_id != 0)                      // added by gary   led light
	{
		COS_KillTimer(g_led_cur_timer_id);
		g_led_cur_timer_id = 0;
	}
	LedBgTimeout(NULL);
}

#endif


//get nvram data buff address
void *NVRAMGetData(UINT8 index, UINT16 length)
{
	return DSM_GetUserData(index, length);
}

//write nvram data to flash
//由于VMWrite非常的慢，请不要频繁的调用，一般是在ap退出或对速度要求不高的地方使用。
BOOL NVRAMWriteData(void)
{
	DSM_WriteUserData();
	return TRUE;
}

// 清空电话本并设置容量
void NVRAMResetPhoneBook(UINT16 size, UINT8 sizeofItem, UINT8 sizeofIndex)
{
	DSM_ResetPhoneBookAddress(size, sizeofItem, sizeofIndex);
}

void NVRAMErasePhoneBook(void)
{
	DSM_Erase_Phonebook();
}

// 取得电话本存储区域地址和长度
void NVRAMGetPhoneBookParam(UINT32 *length, UINT16 *size, UINT16 *valid_size)
{
	DSM_GetPhoneBookParam(length, size, valid_size);
}

void *NVRAMGetPhoneBookAddress()
{
	return DSM_GetPhoneBookAddress();
}

int NVRAMGetPhoneBookOffset(void)
{
	return DSM_GetPhoneBookOffset();
}

// 写电话本
void NVRAMWritePhoneBook(UINT8 *data, UINT16 index)
{
	DSM_WritePhoneBook(data, index);
}

// 写电话本索引
void NVRAMWritePhoneBookIndex(UINT8 *index, UINT16 size)
{
	DSM_WritePhoneBookIndex(index, size);
}


void SetContrast(INT8 ContrastValue)              //设置屏幕对比度
{
}


#ifndef WIN32
void app_trace(UINT16 level, char* fmt, ...)
{
#ifndef APP_NO_TRACE
	va_list args;
	
	va_start(args, fmt);
	sxs_vprintf(_APP | TLEVEL(level), fmt, args);
	va_end(args);
#endif
}
#endif

