/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "string.h"
#include "ap_gui.h"
#include "tm.h"
//#include "hal_usb.h"


typedef struct
{
	UINT32 date;
	UINT32 time;
} BOOT_SECTOR_DATE_STRUCT_T;

#ifdef WIN32
BOOT_SECTOR_DATE_STRUCT_T g_date_and_time =
{ BUILD_DATE, BUILD_TIME};
#else
PROTECTED BOOT_SECTOR_DATE_STRUCT_T g_date_and_time
__attribute__((section (".build_date"))) =
{ BUILD_DATE, BUILD_TIME};
#endif

/*********************************************************************************
* Description : 根据序号生成新的可用文件名
*
* Arguments   : location->file_name, 原来的文件名
*               location->file_name, 输出新的文件名
*
* Returns     :  新文件的序号, 01~99
*        如果01~99 文件存在, 返回0
*
* Notes       :文件名字格式 : xxx000 WAV, xxx000 ACT, ***000 JPG...
*        如果输入的文件不存在,则直接返回
        检查目录满最长时间15s
        顺序产生下一个目录名最长时间1s.
*
* TODO:        不要使用 atoi(), 节省128byte  (ok!)
*********************************************************************************/
INT32 GenNewName(file_location_t *location)
{
	INT32 num;
	INT32 loops = 0;
	WCHAR new_name[12];
	INT32 f;
	
	new_name[0] = location->file_name[0];
	new_name[1] = location->file_name[1];
	new_name[2] = location->file_name[2];
	
	new_name[6] = '.';
	new_name[7] = location->file_name[8];
	new_name[8] = location->file_name[9];
	new_name[9] = location->file_name[10];
	new_name[10] = 0;
	// 检查文件夹是否含有001这个文件,如没有RETURN 1
	new_name[3] = '0';
	new_name[4] = '0';
	new_name[5] = '1';
	// 如果文件不存在
	f = FS_Open((PCSTR)new_name, FS_O_RDONLY, 0);
	
	if(f < 0)
	{
		location->file_name[3] = new_name[3];
		location->file_name[4] = new_name[4];
		location->file_name[5] = new_name[5];
		return 1;
	}
	else
	{
		FS_Close(f);
	}
	
	// 按指定的值,继续查找.
	new_name[3] = location->file_name[3] ;
	new_name[4] = location->file_name[4] ;
	new_name[5] = location->file_name[5] ;
	
	num = (new_name[3] - '0') * 100 + (new_name[4] - '0') * 10 + (new_name[5] - '0');
	if(num <= 0)
	{
		num = 1;
	}
	if(num > 200)
	{
		num = 1;
	}
	
	//如果文件存在,数字加1
	do
	{
		new_name[3] = ((num / 100) % 10) + '0';
		new_name[4] = ((num / 10) % 10) + '0';
		new_name[5] = (num % 10) + '0';
		
		// 如果文件不存在
		f = FS_Open((PCSTR)new_name, FS_O_RDONLY, 0);
		
		if(f < 0)
		{
			location->file_name[3] = new_name[3];
			location->file_name[4] = new_name[4];
			location->file_name[5] = new_name[5];
			return num;
		}
		else
		{
			FS_Close(f);
			num++;
			if(num > 200)
			{
				num = 1;
			}
		}
		
	}
	while(++loops < 1000);
	
	//循环1000次还找不到合适的文件
	return 0;
}




void FS_FileTimeConvert(fs_crt_time_t *pfileDateTime, UINT32 time)
{
	TM_FILETIME   fileTime;
	TM_SYSTEMTIME sysTime;
	
	memset(&fileTime, 0, sizeof(TM_FILETIME));
	fileTime.DateTime = time;
	memset(&sysTime, 0, sizeof(TM_SYSTEMTIME));
	
	TM_FileTimeToSystemTime(fileTime, &sysTime);
	
	pfileDateTime->day     = sysTime.uDay;
	pfileDateTime->hour    = sysTime.uHour;
	pfileDateTime->minute  = sysTime.uMinute;
	pfileDateTime->month   = sysTime.uMonth;
	pfileDateTime->second  = sysTime.uSecond;
	pfileDateTime->year    = sysTime.uYear;
}



/************************************************************************************
		FS_GetSpace()
 *描述
    获取u盘剩余空间，根据输入参数不同选择要求返回磁盘总的扇区数还是剩
    余扇区数
 *参数
    INT8 typechar  0表示调用将返回表示当前磁盘驱动器总空间的扇区数，
                   1表示返回当前磁盘驱动器剩余空间的扇区数
 *返回
    long diskspace
 ************************************************************************************/
UINT32   FS_GetSpace(UINT8 disk, UINT8 type)
{
	static FS_INFO psFSInfo;
	INT32 Result = -1;
	
	if(FS_DEV_TYPE_FLASH == disk) /*Tflash*/
	{
		Result = FS_GetFSInfo("vds0", &psFSInfo);
	}
	else if(FS_DEV_TYPE_TFLASH == disk) /*Tflash*/
	{
		Result = FS_GetFSInfo("mmc0", &psFSInfo);
	}
	else if(FS_DEV_TYPE_USBDISK == disk) /*USB*/
	{
		Result = FS_GetFSInfo("usb0", &psFSInfo);
	}
	else/*Unsurport Disk*/
	{
		return 0;
	}
	
	if(ERR_SUCCESS == Result)
	{
		if(FS_GET_SPACE_FREE == type) //返回剩余扇区数目
		{
			return (psFSInfo.iTotalSize - psFSInfo.iUsedSize);
		}
		else if(FS_GET_SPACE_TOTAL == type) //返回总的扇区数目
		{
			return psFSInfo.iTotalSize;
		}
	}
	return 0;
	
	
}

/// Return the date of build of system.
/// The format is 0xYYYYMMDD.
UINT32 AP_GetBuildDate(void)
{
	return g_date_and_time.date;//BUILD_DATE;
}

/// Return the time of build of system.
/// The format is 0xHHMMSS.  change the BUILD_TIME of compilerrules.mk
UINT32 AP_GetBuildTime(void)
{
	return g_date_and_time.time;//BUILD_TIME;
}


/// Return the version number
/// The format is 0xAABB, A is the big version, B is little version
UINT32 AP_GetVersion(void)
{
	return 0x0100; // 1.0
}


//激活文件系统分区是否存在，若存在，返回分区状态（文件系统类型，是否格式化）
//para: 逻辑驱动器号，工作模式（通常=0）
//ret:  TRUE/FALSE
BOOL MountDisk(UINT8 dev_type)
{
	UINT umss_status = umss_GetUsbVirtualStatus();
	if(umss_status == 0)
	{
		if(dev_type == FS_DEV_TYPE_TFLASH && GetUsbCableStatus() && hal_UsbGetMode() == 1) // usb disk use t flash
		{
			return FALSE;
		}
	}
	
	if(FS_MountDevice(dev_type) == ERR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//-------------------------------------------------
CharType_e CheckCharType(INT8 data)
{
	if ((data >= '0') && (data <= '9'))
	{
		return CharType_NUM;
	}
	
	if ((data >= 'A') && (data <= 'Z'))
	{
		return CharType_UPER;
	}
	
	if ((data >= 'a') && (data <= 'z'))
	{
		return CharType_DNER;
	}
	
	if (data == ' ')
	{
		return CharType_SPACE;
	}
	return CharType_OTHER;
}

//type must equ. 0 means 公历格式
BOOL TM_SetDateAndTime(date_t *date, ap_time_t *time)
{
	//kval_t  kval;
	TM_SYSTEMTIME systemTime/*,SysRtcTime*/;
	//UINT32 /*TotalMinute1*/TotalMinute2;
	memset(&systemTime, 0, sizeof(TM_SYSTEMTIME));
	
	TM_GetSystemTime(&systemTime);
	
	if(date != NULL)
	{
		systemTime.uYear  = date->year ;
		systemTime.uMonth = date->month;
		systemTime.uDay    = date->day;
		
	}
	if(time != NULL)
	{
		systemTime.uHour    = time->hour;
		systemTime.uMinute = time->minute;
		systemTime.uSecond = time->second;
	}
	
	//TotalMinute1 = ((SysRtcTime.uYear-2000)*365+SysRtcTime.uMonth*30+SysRtcTime.uDay)*24*60+SysRtcTime.uMinute;
	//TotalMinute2 = ((systemTime.uYear-2000)*365+systemTime.uMonth*30+systemTime.uDay)*24*60+systemTime.uHour*60+systemTime.uMinute;
	
	//if(TotalMinute1 < TotalMinute2)
	{
		//将systemtime 和 gwtime写回到VRAM中
		//NVRAMRead(&kval, VM_KERNEL, sizeof(kval_t));
		//kval.systemtime = TotalMinute2;   //全局变量
		//NVRAMWrite(&kval, VM_KERNEL, sizeof(kval_t));
		TM_SetSystemTime(&systemTime);
	}
	return TRUE;
}


INT8 TM_GetDateAndTime(date_t *date, ap_time_t *time)
{
	TM_SYSTEMTIME systemTime;
	memset(&systemTime, 0, sizeof(TM_SYSTEMTIME));
	
	if (TM_GetSystemTime(&systemTime))
	{
		if(date != NULL)
		{
			date->year = systemTime.uYear;
			date->month = systemTime.uMonth;
			date->day  = systemTime.uDay;
		}
		if(time != NULL)
		{
			time->hour  = systemTime.uHour;
			time->minute = systemTime.uMinute;
			time->second = systemTime.uSecond;
		}
		//app_trace(APP_MAIN_TRC,"uHour:%d,uMinute:%d,uSecond:%d,uDayOfWeek:%d.",systemTime.uHour,systemTime.uMinute,systemTime.uSecond,systemTime.uDayOfWeek);
		return 0;
	}
	app_trace(APP_MAIN_TRC, "Fatal Error : TM_GetDateAndTime, Return -1!");
	return -1;
}




