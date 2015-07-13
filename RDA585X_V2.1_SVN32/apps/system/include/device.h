/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/


#ifndef _DEVICE_H_
#define _DEVICE_H_


// index of ������, 512byte every block
enum vm_index
{
	//ϵͳ������
	VM_KERNEL = 1,      //��ap_kercfg����ֵ
	VM_SYSTEM,       //��ap_main����ֵ
	//��Ӧ�ó����������ÿ������1KB
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

//����magic�����ж�nvrm��������Ƿ�Ϸ�,UINT16 ����
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


//������ͣ�battary type
#define BATT_TYPE_ALKALINE     0
#define BATT_TYPE_NIH          1
#define BATT_TYPE_LITHIUM      2

/*********************************************************************************
* Description : usb
*********************************************************************************/
typedef struct
{
	UINT16 VID;            //default��0x10d6
	UINT16 PID;            //default��0x1000
} vidpid_t;

typedef struct
{
	INT8 usbvendor[8];                //default��"Generic"
	INT8 usbproductidentification[16];    //default��"USB DISK"�����㲹0x20
	INT8 usbproductversion[4];        //default��" 1.00"
	INT8 reserve[4];
} usbattri_t;

typedef struct
{
	INT8 descriptorlen;             //=sizeof(usbsetupinfo_t);      //�ýṹ����
	INT8 descriptortype;            //default��0x03
	UINT16 unicodestring[23];        //default��"USB Mass Storage Class"��unicode��ʽ�����㲹0x20,0x00
	INT8 reserve[16];
} usbsetupinfo_t;


#define LED_LOOP_INFINITE       0xff

void LED_SetPattern(UINT8 pattern_id, UINT8 count);
void LED_SetBgPattern(UINT8 pattern_id);

/*********************************************************************************
* Description : battery
*********************************************************************************/
//��ȡ��ǰ�ĵ���,-1�������ڳ��
//para: none
//ret:0 ~ 100 ����ֵ
INT32 GetBattery(void);

//��ȡU���Ƿ����  0:û����   ����:����
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
/// ad input ��ģʽ ����gl3935      ��ֻ֧��AD_MICIN
typedef enum
{
	AD_MICIN = 0,    //ad mic ����
	AD_FMIN = 1,     //ad fm���� only support by 51
	AD_LINEIN = 2,   //ad line in ���� only support by 51
	AD_MIXIN = 3     //ad ������� only support by 51
} adin_t;

typedef enum
{
	AI_MICGAIN = 0, //ai mic gain
	AI_LINEGAIN = 1, //ai line in gain(only support by 51)
	AI_FMGAIN = 2   //ai fm gain (only support by 51)
} aingainselect_t;

typedef struct
{
	INT8 micin: 1;   //��λΪ1,ai mic select
	INT8 micin20: 1; //enanble mic (20gain) �������ͬʱѡ��������Ϊ1��ѡmicin20
	INT8 linein: 1;  //��λΪ1,fm���뱻ѡ�� only support by 51
	INT8 fmin: 1;    //��λΪ1,linein���뱻ѡ�� only support by 51
} ain_t;

//adc���Խṹ��
typedef struct
{
	adin_t adin;         //adc ����ѡ��
	rate_t rate;         //������ѡ��
	UINT8 gain;           //����ѡ��
	UINT8 channelmode;    // 0/1--> stereo/mono
	UINT8 precision;      // 0/1--> 16bit/18bit
	INT8 reserve;        //����
} adc_t;

//***********************************************************
typedef struct
{
	INT8 dacselect; //0: internal dac 1: external dac       //2003-12-5 10:50
	rate_t rate;    //dac ������ѡ��
	INT8 reserve[2];//����
} dac_t;

//***********************************************************
//      power amplifier input ��ģʽ
//       ����gl3935  ��ֻ֧��dacin : Ҳ����dac������
typedef struct
{
	INT8 dacin: 1;  //��λΪ1,dac in select
	INT8 fmin: 1;   //��λΪ1,fm���뱻ѡ�� only support by 51
	INT8 linein: 1; //��λΪ1,linein���뱻ѡ�� only support by 51
	INT8 micin: 1;  //��λΪ1,micin���뱻ѡ�� only support by 51
} pain_t;


typedef struct
{
	pain_t pa_in;  //����ѡ��
	INT8 volume;    //����ѡ��
	INT8 reserve[2];//����
} pa_t;

#if 1//warkey 2.0
BOOL SetTONEVolume(INT32 volume);
BOOL GetToneStatus();
VOID SetPA_For_TonePlay();
VOID SetPA_For_ToneStop();
#endif

//��������
BOOL SetPAVolume(INT32 volume);
void SetPAMute(BOOL mute);

BOOL SetBackLight(INT8 BLight);//���ñ������ȡ�
BOOL OpenBacklight(void);	//�򿪱���
BOOL CloseBacklight(void);	//�رձ���

void APP_Sleep(void);
BOOL APP_Wakeup(void); // �ɹ����ѷ���TRUE,����û��˯�߷���FALSE

//get nvram data buff address
void *NVRAMGetData(UINT8 index, UINT16 length);
//write nvram data to flash
BOOL NVRAMWriteData(void);

//****************************
//  ������������
#define DRV_VM          0x00    //�����ڴ�����
#define DRV_STG         0x01    //�û��洢������
#define DRV_KY          0x02    //��������
#define DRV_UI          0x03    //��ʾ����
#define DRV_FS          0x04    //�ļ�ϵͳ����
#define DRV_SDFF        0x05    //SD����������
#define DRV_I2C         0x06    //I2C����
#define DRV_SDRAM       0x07    //SDRAM����
//****************************
// �û��洢������
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


//̽������洢�豸�Ƿ����
BOOL Check_CardOrUsb(void);


void SetContrast(INT8 ContrastValue);               //������Ļ�Աȶ�
void StandbyScreen(BOOL bStandby);                //������ʾ��  bStandby=0:����ʾ�� bStandby=1:����ʾ��

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

