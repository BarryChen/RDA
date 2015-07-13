/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#ifndef _AP_SETTING_H_
#define _AP_SETTING_H_


//系统设置栏内的接口函数
INT32 SET_ClockSetting(void);
INT32 SET_LanguageSetting(void);
INT32 SET_PowerDownSetting(void);
INT32 SET_BackLightTimeSetting(void);
INT32 SET_BackLightSetting(void);
INT32 SET_BackLightColorSetting(void);
INT32 SET_ContrastSetting(void);
INT32 SET_StorageSetting(void);
INT32 SET_FirmwareSetting(void);
INT32 SET_SystemReset(void);
INT32 SET_UpdateSetting(void);
INT32 SET_BTSetting(void);
INT32 SET_DataFormatSetting(void);

/*Alarm Sub Menu items*/
INT32 SET_ALARMClockSetting(void);
INT32 SET_ALARMTimesSetting(void);

INT32 GUI_ClockSetting(date_t *date, ap_time_t *time, UINT16 img_id, UINT16 str_id);


INT32 SET_Entry(INT32 param);/*外部接口函数*/

#endif  /* _AP_SETTING_H_*/



