/***********************************************************************
 *
 * MODULE NAME:    project_config.h
 * DESCRIPTION:    configuage of project
 * AUTHOR:         
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2011-2011 RDA Microelectronics.
 *     All rights reserved.
 *
 * REMARKS:
 *      define every feature to 1 for open, 0 for close
 *
 ***********************************************************************/
 #ifndef  __PROJECT_CONFIG_H__
 #define  __PROJECT_CONFIG_H__


#define APP_SUPPORT_BLUETOOTH           1
#define APP_SUPPORT_RECORD              0
#define APP_SUPPORT_BT_RECORD           0
#define APP_SUPPORT_FM                  0
#define APP_SUPPORT_FM_RECORD           0
#define APP_SUPPORT_FM_OUT              0
#define APP_SUPPORT_LINEIN              0
#define APP_SUPPORT_FOLLOW_COMPARE      0
#define APP_SUPPORT_LYRIC               0
#define APP_SUPPORT_FADE_INOUT          1
#define APP_SUPPORT_INDEPEND_VOLUMNE    0
#define APP_SUPPORT_PBAP                0
#define APP_SUPPORT_MSG_SEND            0
#define APP_SUPPORT_MAP                 0
#define APP_SUPPORT_OPP                 0
#define APP_SUPPORT_SPP                 0
#define APP_SUPPORT_BTHID               0
#define APP_SUPPORT_BTBATTERY           1
#define APP_SUPPORT_BT_AUTOVISIBLE      1
#define APP_SUPPORT_BT_REMOTE_VOL       1
#define APP_SUPPORT_MUSIC               0
#define APP_SUPPORT_USBDEVICE           0
#define APP_SUPPORT_USBAUDIO            0
#define APP_SUPPORT_USBSTORAGE_BG       0
#define APP_SUPPORT_CALIB_KEY           1
#define APP_SUPPORT_NUMBER_VOICE        1
#define APP_SUPPORT_DELAY_VOLUME        0

#define APP_SUPPORT_LCD                 0
#define APP_SUPPORT_RGBLCD              0
#define APP_SUPPORT_LED                 1   // led灯指示
#define APP_SUPPORT_LED8S               0   // 8 段数码管显示
#define APP_SUPPORT_USB                 0
#define APP_SUPPORT_FLASHDISK           0
#define APP_SUPPORT_MENU                0
#define APP_SUPPORT_REMOTECONTROL       0

/**********************************************
* suppport lanaguages
**********************************************/
#define APP_SUPPORT_LANG_ENGLISH        1   //English
#define APP_SUPPORT_LANG_FRENCH         0    //French
#define APP_SUPPORT_LANG_ITALIAN        0    //Italian
#define APP_SUPPORT_LANG_VIETNAMESE     0    //Vietnamese
#define APP_SUPPORT_LANG_RUSSIAN        0    //Russian
#define APP_SUPPORT_LANG_ARABIC         0    //Arabic
#define APP_SUPPORT_LANG_THAI           0    //Thai
#define APP_SUPPORT_LANG_SM_CHINESE     1    //SM Chinese
#define APP_SUPPORT_LANG_TR_CHINESE     0    //TR Chinese
#define APP_SUPPORT_LANG_GERMAN         0    //German
#define APP_SUPPORT_LANG_SPANISH        0    //Spanish
#define APP_SUPPORT_LANG_PORTUGUESE     0    //Portuguese
#define APP_SUPPORT_LANG_PERSIAN        0    //Persian
#define APP_SUPPORT_LANG_TURKISH        0    //Turkish
#define APP_SUPPORT_LANG_INDONESIAN     0    //Indonesian
#define APP_SUPPORT_LANG_MALAY          0    //Malay
#define APP_SUPPORT_LANG_JAPANESE       0    //JAPANESE
#define APP_SUPPORT_LANG_KOREAN         0    //KOREAN
#define APP_SUPPORT_LANG_DANISH         0    //DANISH

#define APP_SUPPORT_INDIC_LANGUAGES     0    //
#define APP_SUPPORT_LANG_HINDI          0    //Hindi, no input method
#define APP_SUPPORT_LANG_TELUGU         0    //Telugu
#define APP_SUPPORT_LANG_PUNJABI        0    //Punjabi
#define APP_SUPPORT_LANG_BENGALI        0    //Bengali
#define APP_SUPPORT_LANG_MARATHI        0    //Marathi
#define APP_SUPPORT_LANG_ORIYA          0    //Malay
#define APP_SUPPORT_LANG_TAMIL          0    //Malay
#define APP_SUPPORT_LANG_KANNADA        0    //Malay
#define APP_SUPPORT_LANG_MALAYALAM      0    //Malay

/**********************************************
* LCD dimension 
**********************************************/
#define LCD_WIDTH               128
#define LCD_HEIGHT              64

#define APP_DEFAULT_RESULT      RESULT_BT // 进入系统后首先运行的任务

#endif

