/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/


#ifndef _DEVICE_H_
#define _DEVICE_H_


// index of 参数区, 512byte every block
enum vm_index
{
	//系统参数区
	VM_KERNEL = 1,      //由ap_kercfg赋初值
	VM_SYSTEM,       //由ap_main赋初值
	//各应用程序参数区，每个程序1KB
	VM_AP_MUSIC,
	VM_AP_VOICE,
	VM_AP_RECORD,
	VM_AP_UDISK ,
	VM_AP_RADIO ,
	VM_AP_SETTING,
	VM_AP_TIMER,
	VM_AP_MRECORD,    //for M-Record
	VM_AP_CALENDAR,
	VM_AP_BLUETOOTH,
	VM_Block_Count,
} ;


enum config_index
{
	CFG_SYSTEM = 1,
	CFG_DISPLAY,
	CFG_BLUETOOTH,
};

//以下magic用于判断nvrm里的数据是否合法,UINT16 类型
#define MAGIC_KVAL           0x1eed
#define MAGIC_COMVAL         0x1ead
#define MAGIC_MUSIC          0xbeef
#define MAGIC_VOICE          0xfee0
#define MAGIC_RECORD         0x3d3d
#define MAGIC_FMRADIO        0xad01
#define MAGIC_SETTING        0xbaba
#define MAGIC_UDISK          0xee77
#define MAGIC_TESTER         0x9801
#define MAGIC_TIMER          0x3935
#define MAGIC_UPGRADE        0x3951
#define MAGIC_CALENDAR       0xc1d2
#define MAGIC_MRECORD        0xcc00
#define MAGIC_BLUETOOTH      0x180e


//电池类型，battary type
#define BATT_TYPE_ALKALINE     0
#define BATT_TYPE_NIH          1
#define BATT_TYPE_LITHIUM      2

/*********************************************************************************
* Description : usb
*********************************************************************************/
typedef struct
{
	UINT16 VID;            //default：0x10d6
	UINT16 PID;            //default：0x1000
} vidpid_t;

typedef struct
{
	INT8 usbvendor[8];                //default："Generic"
	INT8 usbproductidentification[16];    //default："USB DISK"，不足补0x20
	INT8 usbproductversion[4];        //default：" 1.00"
	INT8 reserve[4];
} usbattri_t;

typedef struct
{
	INT8 descriptorlen;             //=sizeof(usbsetupinfo_t);      //该结构长度
	INT8 descriptortype;            //default：0x03
	UINT16 unicodestring[23];        //default："USB Mass Storage Class"。unicode格式，不足补0x20,0x00
	INT8 reserve[16];
} usbsetupinfo_t;


#define LED_LOOP_INFINITE       0xff

void LED_SetPattern(UINT8 pattern_id, UINT8 count);
void LED_SetBgPattern(UINT8 pattern_id);

/*********************************************************************************
* Description : battery
*********************************************************************************/
//获取当前的电量,-1代表正在充电
//para: none
//ret:0 ~ 100 电量值
INT32 GetBattery(void);

//获取U盘是否插线  0:没插线   其他:插线
BOOL GetUsbCableStatus(void);



/*********************************************************************************
* Description : analog
*********************************************************************************/
typedef enum
{
	FS_8K = 8,       //8k sample rate
	FS_11K025 = 11,  //11.025k sample rate
	FS_12K = 12,     //12k sample rate
	FS_16K = 16,     //16k sample rate
	FS_22K05 = 22,   //22.05k  sample rate
	FS_24K = 24,     //24k sample rate
	FS_32K = 32,     //32k sample rate
	FS_44K1 = 44,    //44.1k sample rate
	FS_48K = 48,     //48k sample rate
	FS_96K = 96      //96k sample rate
} rate_t;

//***********************************************************
/// ad input 的模式 对于gl3935      输只支持AD_MICIN
typedef enum
{
	AD_MICIN = 0,    //ad mic 输入
	AD_FMIN = 1,     //ad fm输入 only support by 51
	AD_LINEIN = 2,   //ad line in 输入 only support by 51
	AD_MIXIN = 3     //ad 混合输入 only support by 51
} adin_t;

typedef enum
{
	AI_MICGAIN = 0, //ai mic gain
	AI_LINEGAIN = 1, //ai line in gain(only support by 51)
	AI_FMGAIN = 2   //ai fm gain (only support by 51)
} aingainselect_t;

typedef struct
{
	INT8 micin: 1;   //此位为1,ai mic select
	INT8 micin20: 1; //enanble mic (20gain) 此两项不能同时选择，两个都为1，选micin20
	INT8 linein: 1;  //此位为1,fm输入被选择 only support by 51
	INT8 fmin: 1;    //此位为1,linein输入被选择 only support by 51
} ain_t;

//adc特性结构体
typedef struct
{
	adin_t adin;         //adc 输入选择
	rate_t rate;         //采样率选择
	UINT8 gain;           //增益选择
	UINT8 channelmode;    // 0/1--> stereo/mono
	UINT8 precision;      // 0/1--> 16bit/18bit
	INT8 reserve;        //保留
} adc_t;

//***********************************************************
typedef struct
{
	INT8 dacselect; //0: internal dac 1: external dac       //2003-12-5 10:50
	rate_t rate;    //dac 采样率选择
	INT8 reserve[2];//保留
} dac_t;

//***********************************************************
//      power amplifier input 的模式
//       对于gl3935  输只支持dacin : 也就是dac的输入
typedef struct
{
	INT8 dacin: 1;  //此位为1,dac in select
	INT8 fmin: 1;   //此位为1,fm输入被选择 only support by 51
	INT8 linein: 1; //此位为1,linein输入被选择 only support by 51
	INT8 micin: 1;  //此位为1,micin输入被选择 only support by 51
} pain_t;


typedef struct
{
	pain_t pa_in;  //输入选择
	INT8 volume;    //音量选择
	INT8 reserve[2];//保留
} pa_t;

#if 1//warkey 2.0
BOOL SetTONEVolume(INT32 volume);
BOOL GetToneStatus();
VOID SetPA_For_TonePlay();
VOID SetPA_For_ToneStop();
#endif

//设置音量
BOOL SetPAVolume(INT32 volume);
void SetPAMute(BOOL mute);

BOOL SetBackLight(INT8 BLight);//设置背光亮度。
BOOL OpenBacklight(void);	//打开背光
BOOL CloseBacklight(void);	//关闭背光

void APP_Sleep(void);
BOOL APP_Wakeup(void); // 成功唤醒返回TRUE,本来没有睡眠返回FALSE

//get nvram data buff address
void *NVRAMGetData(UINT8 index, UINT16 length);
//write nvram data to flash
BOOL NVRAMWriteData(void);

//****************************
//  驱动程序类型
#define DRV_VM          0x00    //虚拟内存驱动
#define DRV_STG         0x01    //用户存储器驱动
#define DRV_KY          0x02    //键盘驱动
#define DRV_UI          0x03    //显示驱动
#define DRV_FS          0x04    //文件系统驱动
#define DRV_SDFF        0x05    //SD卡快速驱动
#define DRV_I2C         0x06    //I2C驱动
#define DRV_SDRAM       0x07    //SDRAM驱动
//****************************
// 用户存储器类型
#define STG_NAF         0x00            //nand type flash
#define STG_SMC         0x10            //smc
#define STG_MMC         0x20            //mmc
#define STG_CFC         0x30            //cfc
#define STG_HD          0x40            //hd
#define STG_NAF_SB      0x00 | STG_NAF  //small block nand type flash
#define STG_NAF_MB      0x01 | STG_NAF  //small block nand type flash
#define STG_NAF_LB      0x02 | STG_NAF  //small block nand type flash    644
#define STG_NAF_LB1     0x03 | STG_NAF  //large block 1 nand type flash  648
#define STG_NAF_LB2     0x04 | STG_NAF  //large block 2 nand type flash  284  Toshiba MLC Flash(284)
#define STG_NAF_LB3     0x05 | STG_NAF  //large block 3 nand type flash  024
#define STG_NAF_LB4     0x06 | STG_NAF  //large block 4 nand type flash  285  Samsung MLC Flash(284)
#define STG_NAF_LB5     0x07 | STG_NAF  //large block 5 nand type flash  286  Hinix   MLC Flash(284)
#define STG_NAF_LB6     0x08 | STG_NAF  //large block 6 nand type flash  287  Micron  MLC Flash(284)
#define STG_NAF_LB7     0x09 | STG_NAF  //large block 7 nand type flash  288  ST      MLC Flash(284)


//探测物理存储设备是否存在
BOOL Check_CardOrUsb(void);


void SetContrast(INT8 ContrastValue);               //设置屏幕对比度
void StandbyScreen(BOOL bStandby);                //开关显示屏  bStandby=0:关显示屏 bStandby=1:开显示屏

BOOL Upgrade_Flash(INT32 file);
void RestartSystem(void);

/// Return the date of build of platform.
/// The format is 0xYYYYMMDD.
UINT32 GetPlatformBuildDate(void);
/// Return the version number
/// The format is 0xAABB, A is the big version, B is little version
UINT32 GetPlatformVersion(void);
/// Return the revision
UINT32 GetPlatformRevision(void);


#endif /*_USB_H */

