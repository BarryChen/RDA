#include "ap_common.h"
#include "ap_calendar.h"
#include "ap_idle.h"
#include "ap_linein.h"
#include "ap_record.h"
#include "ap_setting.h"
#include "ap_fm.h"
#include "ap_music.h"
#include "ap_bluetooth.h"
#include "ap_usbdisk.h"
#include "tm.h"
#include "MainTask.h"
#include "event.h"
#include "gpio_edrv.h"
#include "mci.h"


#define AP_TEST_FM_FREQ1             104300
#define AP_TEST_FM_FREQ2             99700
#define AP_TEST_FM_FREQ3            103700


BOOL g_test_mode = 0;
extern INT32  g_current_fd;
UINT32 g_testfile_entry = 0;
BOOL play_end = 0;

static void play_finished( void )
{
	hal_HstSendEvent(SYS_EVENT, 0x11111111);
	play_end = TRUE;
}


void APP_Test_Mode(void)
{
	UINT32 key;
	
	g_test_mode = TRUE;
	
	// test leds
	MESSAGE_Sleep(1);
	LED_SetPattern(GUI_LED_TEST_PATTERN1, 1);
	MESSAGE_Sleep(1);
	LED_SetPattern(GUI_LED_TEST_PATTERN2, 1);
	SetPAVolume(0);
	
	hal_HstSendEvent(BOOT_EVENT, 0x7e570100); // 10%
	
	// test lcd
#if APP_SUPPORT_RGBLCD==1
	if(AP_Support_LCD())
	{
		GUI_ClearScreen(NULL);
		GUI_InvertRegion(NULL);
		GUI_UpdateScreen(NULL);
		MESSAGE_Sleep(1);
		GUI_InvertRegion(NULL);
		GUI_UpdateScreen(NULL);
		MESSAGE_Sleep(1);
	}
#elif APP_SUPPORT_LCD==1
	if(AP_Support_LCD())
	{
		GUI_ClearScreen(NULL);
		GUI_InvertRegion(NULL);
		GUI_UpdateScreen(NULL);
		MESSAGE_Sleep(1);
		GUI_InvertRegion(NULL);
		GUI_UpdateScreen(NULL);
		MESSAGE_Sleep(1);
	}
#elif APP_SUPPORT_LED8S==1
	GUI_ClearScreen(NULL);
	GUI_ResShowPic(0xffff, 0, 0);
	GUI_UpdateScreen(NULL);
	COS_Sleep(200);
	GUI_DisplayText(0, 0, "8");
	GUI_UpdateScreen(NULL);
	COS_Sleep(200);
	GUI_DisplayText(1, 0, "8");
	GUI_UpdateScreen(NULL);
	COS_Sleep(200);
	GUI_DisplayText(2, 0, "8");
	GUI_UpdateScreen(NULL);
	COS_Sleep(200);
	GUI_DisplayText(3, 0, "8");
	GUI_UpdateScreen(NULL);
	COS_Sleep(200);
#endif
	
	hal_HstSendEvent(BOOT_EVENT, 0x7e570300); // 30%
	
	// test fm
#if APP_SUPPORT_FM
	hal_HstSendEvent(SYS_EVENT, 0x11220010);
	{
		extern FM_play_status_t    *FMStatus;
		FMStatus = (FM_play_status_t*)NVRAMGetData(VM_AP_RADIO, sizeof(FM_play_status_t));
		FM_SendCommand(MC_OPEN, 0);
		FMStatus->freq = AP_TEST_FM_FREQ1;
		FM_SendCommand(MC_PLAY, 0);
		if(!fmd_ValidStop(FMStatus->freq))
		{
			FMStatus->freq = AP_TEST_FM_FREQ2;
			if(!fmd_ValidStop(FMStatus->freq))
			{
				FMStatus->freq = AP_TEST_FM_FREQ3;
				if(!fmd_ValidStop(FMStatus->freq))
				{
					goto test_fail;
				}
			}
		}
		SetPAVolume(0);
		FM_SendCommand(MC_CLOSE, 0);
	}
#endif
	
	hal_HstSendEvent(BOOT_EVENT, 0x7e570500); // 50%
	
	// test bluetooth
#if APP_SUPPORT_BLUETOOTH==1
	hal_HstSendEvent(SYS_EVENT, 0x11220020);
	{
		extern bt_vars_t *g_pBT_vars;
		extern INT8 g_bt_cur_device;
		g_bt_cur_device = -1;
		g_pBT_vars = (bt_vars_t*)NVRAMGetData(VM_AP_BLUETOOTH, sizeof(bt_vars_t));
		if(BT_Active_Bluetooth() != 0)
		{
			goto test_fail;    // test_fail
		}
	}
#endif
	
	
	hal_HstSendEvent(BOOT_EVENT, 0x7e570800); // 80%
	// test audio
#if APP_SUPPORT_MUSIC==1
	{
		hal_HstSendEvent(SYS_EVENT, 0x11220030);
		if(!MountDisk(FS_DEV_TYPE_TFLASH))
		{
			goto test_fail;    // test_fail
		}
		
		hal_HstSendEvent(SYS_EVENT, 0x11220040);
		if(!fselInit(FSEL_TYPE_MUSIC, FSEL_ALL_SEQUENCE, FSEL_TYPE_COMMONDIR, FS_DEV_TYPE_TFLASH))
		{
			goto test_fail;    // test_fail
		}
		if(!fselGetNextFile(&g_testfile_entry))
		{
			goto test_fail;    // test_fail
		}
		hal_HstSendEvent(SYS_EVENT, 0x11220042);
		hal_HstSendEvent(SYS_EVENT, g_testfile_entry);
		g_current_fd = FS_OpenDirect(g_testfile_entry, FS_O_RDONLY, 0);
		hal_HstSendEvent(SYS_EVENT, g_current_fd);
		if(g_current_fd < 0)
		{
			goto test_fail;    // test_fail
		}
		
		hal_HstSendEvent(SYS_EVENT, 0x11220050);
		if(MCI_ERR_NO != MCI_AudioPlay(0, g_current_fd, MCI_TYPE_DAF, play_finished, 0))
		{
			goto test_fail;
		}
		SetPAVolume(0);
		play_end = FALSE;
		GUI_DisplayMessage(0, 0, "Test Sucess!", GUI_MSG_FLAG_DISPLAY);
		hal_HstSendEvent(SYS_EVENT, 0x11220060);
		hal_HstSendEvent(BOOT_EVENT, 0x7e570a00); // 100%
		hal_HstSendEvent(BOOT_EVENT, 0x7e5752cc); // test success
		LED_SetPattern(GUI_LED_TEST_SUCESS, LED_LOOP_INFINITE);
		
		while(1)
		{
			key =  MESSAGE_Wait();
			if(play_end)
			{
				play_end = FALSE;
				MCI_AudioStop();
				MCI_AudioPlay(0, g_current_fd, MCI_TYPE_DAF, play_finished, 0);
			}
#if 1//warkey //按任意键打开音量播放音乐，MODE键退出并重启
			if(key == (AP_KEY_MODE | AP_KEY_DOWN))
			{
				MCI_AudioStop();
				FS_Close(g_current_fd);
				RestartSystem();
			}
			else if(((key >> 16) > 0) && ((key >> 16) < MAX_KEYS))
			{
				if((key & 0xffff) == AP_KEY_DOWN)
				{
					SetPAVolume(7);
				}
				else if((key & 0xffff) == AP_KEY_UP)
				{
					SetPAVolume(0);
					
					g_test_mode = FALSE;
				}
			}
#else//原始
			if(key == (AP_KEY_PLAY | AP_KEY_DOWN))
			{
				SetPAVolume(7);
			}
			else if(key == (AP_KEY_PLAY | AP_KEY_UP))
			{
				SetPAVolume(0);
				hal_HstSendEvent(BOOT_EVENT, 0x7e5752cc); // test success
				g_test_mode = FALSE;
			}
			else if(key == (AP_KEY_MODE | AP_KEY_DOWN))
			{
				MCI_AudioStop();
				FS_Close(g_current_fd);
				RestartSystem();
			}
#endif
		};
	}
#else
	hal_HstSendEvent(BOOT_EVENT, 0x7e570a00); // 100%
	
	LED_SetPattern(GUI_LED_TEST_SUCESS, LED_LOOP_INFINITE);
	
	hal_HstSendEvent(BOOT_EVENT, 0x7e5752cc); // test success
	g_test_mode = FALSE;
	while(1)
	{
		key =  MESSAGE_Wait();
		if(key == (AP_KEY_MODE | AP_KEY_DOWN))
		{
			RestartSystem();
		}
		if(key == (AP_KEY_POWER | AP_KEY_DOWN))
		{
			DM_DeviceSwithOff();
		}
	};
#endif
	
test_fail:
	GUI_DisplayMessage(0, 0, "Test Fail!", GUI_MSG_FLAG_DISPLAY);
	LED_SetPattern(GUI_LED_TEST_FAIL, LED_LOOP_INFINITE);
	hal_HstSendEvent(BOOT_EVENT, 0x7e57fa11); // test fail
	g_test_mode = FALSE;
	
	while(1)
	{
		key =  MESSAGE_Wait();
		if(key == (AP_KEY_MODE | AP_KEY_DOWN))
		{
			RestartSystem();
		}
		if(key == (AP_KEY_POWER | AP_KEY_DOWN))
		{
			DM_DeviceSwithOff();
		}
	}
}
