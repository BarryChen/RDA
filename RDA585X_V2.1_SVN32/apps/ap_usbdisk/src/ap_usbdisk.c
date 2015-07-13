/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_common.h"
#include "gui.h"
#include "ap_usbdisk.h"
#include "mci.h"
#include "ap_message.h"
#include "gui_const.h"
#include "ap_gui.h"
#include "dm.h"

#ifndef MMI_ON_WIN32
#include "mcd_m.h"
#include "uctls_callback.h"

// =============================================================================
// g_umssStorageCallbackMc
// -----------------------------------------------------------------------------
/// Callback of MC storage
// =============================================================================
EXPORT PUBLIC CONST UMSS_STORAGE_CALLBACK_T     g_umssStorageMcCallback;
#endif


#define USB_PLUGIN_STORAGE              (AP_MSG_USER+1)
#define USB_PLUGIN_AUDIO                (AP_MSG_USER+2)


extern BOOL ui_timeout_exit;
extern UINT32 g_uAudIsPlaying;

BOOL g_usb_connected = FALSE;
UINT8 g_usb_audio_linein = 0;
UINT8 g_usb_audio_tcard = 0;

#if APP_SUPPORT_USBAUDIO==1

#define USB_VOL_BASE 0xe4c2
#define USB_VOL_TOP  0xfff0

//USB HID report descriptor constants
#define USB_HID_USAGE_PAGE 0x05
#define USB_HID_USAGE 0x09
#define USB_HID_COLL_START 0xa1//collection start
#define USB_HID_COLL_END 0xc0//collection end
#define USB_HID_LOGICAL_MIN 0x15
#define USB_HID_LOGICAL_MAX 0x25
#define USB_HID_USAGE_MIN 0x19
#define USB_HID_USAGE_MAX 0x29
#define USB_HID_REPORT_COUNT 0x95
#define USB_HID_REPORT_SIZE 0x75
#define USB_HID_INPUT_DES 0x81//input descriptor
#define USB_HID_OUTPUT_DES 0x91//output descriptor

PRIVATE UINT8 ReportDescriptor[] =
{
	USB_HID_USAGE_PAGE, 0x0c, //Usage page:Consumer Device
	USB_HID_USAGE, 0x01, //Usage:Consumer Contorl
	USB_HID_COLL_START, 0x01, //Collection start
	USB_HID_LOGICAL_MIN, 0x00, //Logical Minimum:0
	USB_HID_LOGICAL_MAX, 0x01, //Logical Maximun:1
	USB_HID_USAGE_PAGE, 0x0c, //Usage page:Consumer Device
	USB_HID_USAGE, 0xe9, //Usage:Volume Up 0x02
	USB_HID_USAGE, 0xea, //Usage:Volume Down 0x04
	USB_HID_USAGE, 0xe2, //Usage:Mute  0x01
	USB_HID_USAGE, 0xcd, //Usage:Play/Pause 0x08
	//USB_HID_USAGE,0xb1,//Usage:Pause 0x10
	//USB_HID_USAGE,0xb5,//Usage:Next 0x20
	//USB_HID_USAGE,0xb6,//Usage:Previous 0x40
	USB_HID_REPORT_COUNT, 0x04, //Report Count:7
	USB_HID_REPORT_SIZE, 0x01, //Report Size :1
	USB_HID_INPUT_DES, 0x42, //Input:data,var,abs,null
	USB_HID_REPORT_COUNT, 0x04, //Report Count:1
	USB_HID_REPORT_SIZE, 0x01, //Report Size:1
	USB_HID_INPUT_DES, 0x01, //Input:cnst,ary,abs
	USB_HID_COLL_END//End Collection
};

PRIVATE UINT8 ReportDescriptor1[] =
{
	0x05, 0x0C, // Usage Page (Consumer Devices)
	0x09, 0x01, // Usage (Consumer Control)
	0xA1, 0x01, // Collection (Application)
	0x05, 0x0C, // Usage Page (Consumer Devices)
	0x15, 0x00, // Logical Minimum (0)
	0x25, 0x01, // Logical Maximum (1)
	0x75, 0x01, // Report Size (1)
	0x95, 0x07, // Report Count (7)
	0x09, 0xB5, // Usage (Scan Next Track)
	0x09, 0xB6, // Usage (Scan Previous Track)
	0x09, 0xB7, // Usage (Stop)
	0x09, 0xCD, // Usage (Play / Pause)
	0x09, 0xE2, // Usage (Mute)
	0x09, 0xE9, // Usage (Volume Up)
	0x09, 0xEA, // Usage (Volume Down)
	0x81, 0x02, // Input (Data, Variable, Absolute)
	0x95, 0x01, // Report Count (1)
	0x81, 0x01, // Input (Constant)
	0xC0 // End Collection
};
//UINT16 Desc_Length = sizeof(ReportDescriptor);

UINT32 keycode = 0;
#endif

BOOL GetUsbCableStatus(void)
{
	return g_usb_connected;
}

INT32 USB_Entry_Storage(VOID)
{
	INT32 result = 0;
	INT32 wait_exit = 1;
	
	GUI_ClearScreen(NULL);//清全屏幕
#if APP_SUPPORT_RGBLCD==1
	GUI_ResShowPic(GUI_IMG_UDISK, 0, 0);//g_displayconfig.usb_x, g_displayconfig.usb_y);
#elif APP_SUPPORT_LCD==1
	GUI_ResShowPic(GUI_IMG_UDISK, 0, 16);
#endif
	GUI_UpdateScreen(NULL);
	
	LED_SetPattern(GUI_LED_USB_DISK, LED_LOOP_INFINITE);
	//   media_PlayInternalAudio(GUI_AUDIO_MASS_STORAGE, 1, FALSE);
	
	while(1)
	{
		UINT32 key;
		
		key = MESSAGE_Wait();
		
		switch(key)
		{
#if 0//warkey 2.1  //支持后台运行时也不自动退出
#if APP_SUPPORT_USBSTORAGE_BG==1
		case AP_MSG_RTC:
			if(wait_exit > 0)
			{
				wait_exit--;
			}
			else
			{
				return RESULT_MAIN;
			}
			break;
#endif
#endif
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
		case AP_MSG_RTC:
			if(Get_Low_Bat_Refresh())
			{
				LED_SetPattern(GUI_LED_USB_DISK, LED_LOOP_INFINITE);
				Clean_Low_Bat_Refresh();
			}
			break;
#endif
			
		case AP_KEY_MODE | AP_KEY_UP:
#ifdef XDL_USBAUDIO_SUPPORT
		case AP_KEY_MODE | AP_KEY_PRESS:
		case AP_MSG_SD_OUT:
			hal_HstSendEvent(APP_EVENT, 0x13091604);
			MESSAGE_SetEvent(USB_PLUGIN_AUDIO);
			return 0;
#endif
			return RESULT_UDISK;
		default:
			result = MESSAGE_HandleHotkey(key);
			if (result != 0 && result != RESULT_UDISK)
			{
				return result;
			}
		}
	}
	
	return 0;
}

#if APP_SUPPORT_USBAUDIO==1

#define USBAUDIO_MAX_VOLUME        AUD_MAX_LEVEL
INT8        g_usbaudio_volumne;

extern VOID uAudGetVolFromCmd(UINT8*, UINT16*);
extern VOID uAudSetVolFromCmd(UINT8, UINT16);

extern volatile UINT8 reset_flag_hid;
extern volatile UINT8 reset_flag_aud;
extern UINT32 g_uAudIsPlaying;

INT32 USB_Entry_Audio(VOID)
{
	INT32 result = 0;
	UINT32 counter = 0;
	int key_down_flag=0;
	GUI_ClearScreen(NULL);//清全屏幕
#if APP_SUPPORT_RGBLCD==1
	GUI_ResShowPic(GUI_IMG_UAUDIO, 0, 0);//g_displayconfig.usb_x, g_displayconfig.usb_y);
#elif APP_SUPPORT_LCD==1
	GUI_ResShowPic(GUI_IMG_UAUDIO, 0, 16);
#endif
	GUI_UpdateScreen(NULL);
	
	LED_SetPattern(GUI_LED_USB_AUDIO, LED_LOOP_INFINITE);
	gpio_SetMute(FALSE);//warkey 2.0
	while(1)
	{
		UINT32 key;
		
		key = MESSAGE_Wait();
		
		switch(key)
		{
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
		case AP_MSG_RTC:
			if(Get_Low_Bat_Refresh())
			{
				LED_SetPattern(GUI_LED_USB_AUDIO, LED_LOOP_INFINITE);
				Clean_Low_Bat_Refresh();
			}
			if(key_down_flag)
			{
				key_down_flag=0;
				keycode = 0x00;
				uhid_SendKeycode(&keycode, 1);				
			}
			break;
#endif
#ifndef MMI_ON_WIN32
		case AP_KEY_VOLADD | AP_KEY_DOWN:
			hal_HstSendEvent(USB_EVENT, 0xabcd2221);
			// uhid_SendKeycode(0x01);
			// keycode = 0x01;
			key_down_flag=1;
			keycode = 0x20;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_VOLSUB | AP_KEY_DOWN:
			hal_HstSendEvent(USB_EVENT, 0xabcd3333);
			// uhid_SendKeycode(0x02);
			key_down_flag=1;
			keycode = 0x40;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_EQ | AP_KEY_DOWN: // for mute
			hal_HstSendEvent(USB_EVENT, 0xabcd5555);
			//  uhid_SendKeycode(0x04);
			key_down_flag=1;
			keycode = 0x10;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_LOOP | AP_KEY_DOWN: // for stop
			hal_HstSendEvent(USB_EVENT, 0xabcd4444);
			//  uhid_SendKeycode(0x04);
			key_down_flag=1;
			keycode = 0x04;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_PLAY | AP_KEY_DOWN:
			hal_HstSendEvent(USB_EVENT, 0xabcd1111);
			//  uhid_SendKeycode(0x08);
			key_down_flag=1;
			keycode = 0x08;
			uhid_SendKeycode(&keycode, 1);
			break;
#ifdef SUPPORT_PREV_NEXT_SWAP//warkey 2.1 
		case AP_KEY_PREV | AP_KEY_PRESS:
#else
		case AP_KEY_PREV | AP_KEY_DOWN:
#endif
			hal_HstSendEvent(SYS_EVENT, 0xabcd7788);
			key_down_flag=1;
			keycode = 0x02;
			uhid_SendKeycode(&keycode, 1);
			break;
#ifdef SUPPORT_PREV_NEXT_SWAP//warkey 2.1
		case AP_KEY_NEXT | AP_KEY_PRESS:
#else
		case AP_KEY_NEXT | AP_KEY_DOWN:
#endif
			hal_HstSendEvent(SYS_EVENT, 0xabcd8877);
			key_down_flag=1;
			keycode = 0x01;
			uhid_SendKeycode(&keycode, 1);
			break;
			#if 0
		case AP_KEY_VOLADD | AP_KEY_UP:
			//  uhid_SendKeycode(0x00);
			keycode = 0x00;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_VOLSUB | AP_KEY_UP:
			//   uhid_SendKeycode(0x00);
			keycode = 0x00;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_PLAY | AP_KEY_UP:
			//   uhid_SendKeycode(0x00);
			keycode = 0x00;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_EQ | AP_KEY_UP: // for mute
			//   uhid_SendKeycode(0x00);
			keycode = 0x00;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_LOOP | AP_KEY_UP: // for stop
			//   uhid_SendKeycode(0x00);
			keycode = 0x00;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_PREV | AP_KEY_UP:
			hal_HstSendEvent(SYS_EVENT, 0xabcd1122);
			keycode = 0x00;
			uhid_SendKeycode(&keycode, 1);
			break;
		case AP_KEY_NEXT | AP_KEY_UP:
			hal_HstSendEvent(SYS_EVENT, 0xabcd2211);
			keycode = 0x00;
			uhid_SendKeycode(&keycode, 1);
			break;
			#endif
		case EV_FS_USB_VOL_CHANGE:
			{
				UINT8 mute = 0;
				UINT16 vol = 0;
				//        hal_HstSendEvent(USB_EVENT, 0x12040001);
				uAudGetVolFromCmd(&mute, &vol);
				//        hal_HstSendEvent(USB_EVENT, mute);
				//        hal_HstSendEvent(USB_EVENT, vol);
				
				if(mute == 1)
				{
					SetPAMute(1);
				}
				else
				{
					if(vol < USB_VOL_BASE)
					{
						SetPAMute(1);
						break;
					}
					
					g_usbaudio_volumne = (vol - USB_VOL_BASE) / ((USB_VOL_TOP - USB_VOL_BASE) / USBAUDIO_MAX_VOLUME) + 1;
					if(g_usbaudio_volumne > USBAUDIO_MAX_VOLUME)
					{
						g_usbaudio_volumne = USBAUDIO_MAX_VOLUME;
					}
					g_comval->Volume = g_usbaudio_volumne;
					SetPAVolume(g_usbaudio_volumne);
				}
			}
			break;
#endif
			
		case AP_KEY_MODE|AP_KEY_PRESS:
			return RESULT_MAIN;
		case AP_KEY_MODE | AP_KEY_UP:
		
			return RESULT_UDISK;
		default:
#ifndef MMI_ON_WIN32
			if(reset_flag_hid == 1 && reset_flag_aud == 1)
			{
				if(counter == 0)
				{
					keycode = 0x04;
					uhid_SendKeycode(&keycode, 1);
					counter++;
				}
				else if(counter == 1)
				{
					keycode = 0x00;
					uhid_SendKeycode(&keycode, 1);
					counter++;
				}
			}
#endif
			if ((result = MESSAGE_HandleHotkey(key)) != 0)
			{
				if(result == RESULT_LINE_IN)
				{
					hal_HstSendEvent(SYS_EVENT, 0x13071601);
					g_usb_audio_linein = 1;
				}
				else if(result == RESULT_MUSIC)
				{
					hal_HstSendEvent(SYS_EVENT, 0x13071602);
					g_usb_audio_tcard = 1;
				}
				
				return result;
			}
			
		}
	}
	
	return 0;
}
#endif

#if APP_SUPPORT_USBDEVICE==1
extern INT32           g_last_reslt;
INT32 USB_Entry(INT32 param)
{
#if APP_SUPPORT_USBAUDIO==1
	g_usbaudio_volumne = g_comval->Volume;
	SetPAVolume(g_usbaudio_volumne);
#ifndef MMI_ON_WIN32
	uAudSetVolFromCmd(0, g_usbaudio_volumne);
#endif
#endif
	if(!GetUsbCableStatus())
	{
		return g_last_reslt;
	}
	while(1)
	{
		INT32 result = 0;
		
		//GUI_ClearScreen(NULL);//清全屏幕
		COS_Sleep(10);
#if APP_SUPPORT_MENU==1
		if(!g_usb_audio_linein && !g_usb_audio_tcard) //check returning usb from line-in or not
		{
			ui_timeout_exit = FALSE;
			result = GUI_Display_Menu(GUI_MENU_USBPLUGIN, NULL);
			ui_timeout_exit = TRUE;
		}
		else
		{
			MESSAGE_SetEvent(USB_PLUGIN_AUDIO);
			g_usb_audio_linein = 0;//clear the flag
			g_usb_audio_tcard = 0;
			hal_HstSendEvent(SYS_EVENT, 0x13071802);
		}
		
		//GUI_UpdateScreen(NULL);
#else
#ifdef XDL_USBAUDIO_SUPPORT
		if (MountDisk(FS_DEV_TYPE_TFLASH))
		{
			hal_HstSendEvent(APP_EVENT, 0x13091600);
			MESSAGE_SetEvent(USB_PLUGIN_STORAGE);
		}
		else
		{
			hal_HstSendEvent(APP_EVENT, 0x13091601);
			MESSAGE_SetEvent(USB_PLUGIN_AUDIO);
		}
#else
		if (MountDisk(FS_DEV_TYPE_TFLASH))
		{
			hal_HstSendEvent(APP_EVENT, 0x13091602);
			MESSAGE_SetEvent(USB_PLUGIN_STORAGE);
		}
		else
		{
			hal_HstSendEvent(APP_EVENT, 0x13091603);
			MESSAGE_SetEvent(USB_PLUGIN_STORAGE);
			//return 0;
		}
#endif
#endif
		if (result != 0)
		{
			break;
		}
		
		while(1)
		{
			UINT32 key;
			
			key = MESSAGE_Wait();
			
			switch(key)
			{
			case USB_PLUGIN_STORAGE:
			
#ifndef MMI_ON_WIN32
				hal_UsbSetMode(HAL_USB_MODE_MASS);
				uctls_Open(0x0000, 0x0000, 0x0101, "USB Stroage Devive 1.02");
				umss_Open();
				umss_AddPart(&g_umssStorageMcCallback);
				uctls_ChargerStatus(0);
#endif
				result = USB_Entry_Storage();
				
#ifndef MMI_ON_WIN32
				//             if(result != RESULT_MAIN)
#ifdef XDL_USBAUDIO_SUPPORT
				uctls_Close();
#endif
#endif
				hal_HstSendEvent(SYS_EVENT, 0x06274444);
				break;
			case USB_PLUGIN_AUDIO:
#if APP_SUPPORT_USBAUDIO==1
			
			
#ifndef MMI_ON_WIN32
				hal_UsbSetMode(HAL_USB_MODE_AUDIO);
				uctls_Open(0x0000, 0x0000, 0x0101, "USB Audio Device 1.04");
				uaud_Open();
				uhid_Open(ReportDescriptor1, sizeof(ReportDescriptor1));
				mediaSendCommand(MC_PLAY_USB, 0);
				
				uctls_ChargerStatus(0);
#endif
				result = USB_Entry_Audio();
				
#ifndef MMI_ON_WIN32
				reset_flag_hid = 0;
				reset_flag_aud = 0;
				
				uctls_Close();
				g_uAudIsPlaying = 0;
#endif
				mediaSendCommand(MC_STOP, 0);
#endif
				break;
			case AP_KEY_MODE | AP_KEY_UP:
				result = RESULT_UDISK;
				break;
			default:
				result = MESSAGE_HandleHotkey(key);
			}
			
			if (result != 0)
			{
				break;
			}
		}
		
		if (result == RESULT_EXIT_UDISK)
		{
			g_usb_connected = FALSE;
		}
		
		if(result == RESULT_LINE_IN || result == RESULT_MUSIC || result == RESULT_BT_ACTIVE) //Line-in or SD card detected
		{
			return result;
		}
		
		if (result != 0 && result != RESULT_UDISK)
		{
			break;
		}
	}
	
	return 0;                       //拔线,弹出               //拔线,弹出
	
}

#endif

