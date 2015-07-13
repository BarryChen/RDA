/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#ifndef _AP_COMMON_H_
#define _AP_COMMON_H_

#include "project_config.h"
#include "platform.h"
#include "string.h"
#include "device.h"
#include "globalconstants.h"
#include "event.h"
#include "fs.h"
#include "assert.h"
#include "stdio.h"

#ifndef PCWSTR
#define PCWSTR const UINT16*
#endif

#ifndef PWSTR
#define PWSTR UINT16*
#endif

#ifndef WCHAR
#define WCHAR UINT16
#endif

#define FILEDATABUF             0x7200
#define LISTNAME_DISP_LENNGTH   512     //������2�ı�������Ϊunicode��2BYTE/�ַ�

//��Ҫ��Ӧ��ģ�飬����ʱ��������������
#define RESULT_NULL             0
#define RESULT_CALENDAR         1
#define RESULT_STANDBY          2
#define RESULT_IDLE             3
#define RESULT_UDISK            4
#define RESULT_MUSIC            5
#define RESULT_BT               6
#define RESULT_RADIO            7
#define RESULT_SYSTEM           8
#define RESULT_RECORD_START     9
#define RESULT_RECORD_NOSTART   10
#define RESULT_MAIN             11
#define RESULT_TIMER            12
#define RESULT_FMREC_START      13
#define RESULT_FMREC_NOSTART    14
#define RESULT_BT_ACTIVE        15
#define RESULT_RECORD_SWITCH    16
#define RESULT_ALARM            17
#define RESULT_LINE_IN          18
#define RESULT_EXIT_UDISK       19
#define RESULT_CHARGING         20
#define RESULT_KEYCALIB         21
//return message id
#define RESULT_REDRAW           50    //������Ҫ�ػ�
#define RESULT_POWERON          51    //��standby����
#define RESULT_IGNORE           52    //�ؼ�ȡ��ѡ��
#define RESULT_TIMEOUT          53    //�ؼ�ȡ��ѡ��
#define RESULT_SD_IN            54
#define RESULT_SD_OUT           55

//user define id
#define RESULT_USER1            100

//controls constant
#define DELETE_FILE             0
#define DELETE_MUSIC            1
#define DELETE_VOICE            2
#define DELETE_ALL              3    //this was not impletemented

#define DIRECTORY_MUSIC         FSEL_TYPE_MUSIC
#define DIRECTORY_VOICE         FSEL_TYPE_VOICE
//#define DIRECTORY_ALL           2

#define MAX_FILE_LEN            12

//#define __OGG_SUPPORT

//����
#define PLAYING_STOP            0x00
#define PLAYING_PLAYING         0x01
#define PLAYING_PAUSE           0x02
#define PLAYING_AB              0x03
#define PLAYING_ABPAUSE         0x04
#define PLAYING_SEARCHING       0x05
#define PLAYING_REACH_END       0x06
#define PLAYING_REACH_A         0x07
#define PLAYING_REACH_B         0x08
#define PLAYING_SEACH_FAIL      0x09
#define PLAYING_SEACH_SUCC      0x0A
#define PLAYING_WAIT_A          0x0B//�Ȼص�A��
#define PLAYING_WAIT_B          0x0D//��DSP��B�㶨��(basal ����)
#define PLAYING_ERROR           0x0C
#define PLAYING_REACH_HEAD      0x0E//�����ͷ
#define PLAYING_INTERNAL        0x0f//

#define RECORDING_STOP          0x10
#define RECORDING_RECORDING     0x11
#define RECORDING_PAUSE         0x12
#define RECORDING_WAITING       0x13
#define RECORDING_EXCEPTION     0x14
#define RECORDING_OUT_SPACE     0x15
#define RECORDING_DISK_ERROR    0x16
#define RECORDING_VOR_RECORDING 0x17
#define RECORDING_VOR_WAITING   0x18

#define SIGNAL_NON              0x00//û���ź�
#define SIGNAL_REACH_V          0x01//��V��
#define SIGNAL_REACH_B          0x02//��B��
#define SIGNAL_CLEAR_B          0x03//��B�㣬��ͬʱҪ��B��

//���嵭�뵭������
#define FadeNon_def              0
#define FadeIn_def               1
#define FadeOut_def              2
#define FadeOut2_def             3//��ʱfadeout    

/* ------ �ļ�ѡ����� ------*/
//file system layer
#define TOTALDIRLAYER            8
//file selector defines
#define FSEL_MAX_FILENUM        999
#define FSEL_MAX_DIRNUM         100

// select type defines
#define FSEL_TYPE_MUSIC         0
#define FSEL_TYPE_VOICE         1
#define FSEL_TYPE_ALLFILE       2
#define DIRECTORY_ALL           3

// select playlist or common dir
#define FSEL_TYPE_COMMONDIR     0
#define FSEL_TYPE_PLAYLIST      1

// select mode defines
#define FSEL_ALL_SEQUENCE       0
#define FSEL_ALL_REPEATONE      1
#define FSEL_DIR_SEQUENCE       2
#define FSEL_DIR_REPEATALL      3
#define FSEL_ALL_REPEATALL      4
#define FSEL_DIR_RANDOM         5
#define FSEL_DIR_INTRO          6
#define FSEL_DIR_END            7


// delete type defines
#define FSEL_DEL_FILE           0
#define FSEL_DEL_DIRFILE        1

// error information defines
#define FSEL_ERR_OUTOF          1
#define FSEL_ERR_EMPTY          2
#define FSEL_ERR_SETDIR         3
#define FSEL_ERR_SETFILE        4
#define FSEL_ERR_EXCEPTION      5
#define FSEL_ERR_FS             20
#define FSEL_ERR_FAIL           21
#define FSEL_ERR_DISK           22
#define FSEL_ERR_PLAYLIST       23


#define CP_ASCII    1         //����ע�͵���#define�Ǵ����!����ԭ��ASCII��CHS��ͬһ��������û�����ֳ���.
#define CP_CHS      0         //�ڼ���Latin1֮��,��BUG������!�ʱ�����������!       
#define CP_CHT      2

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// file system type
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define FS_DIR_FILE             0x80   //  ��ʾ������������չ�����ӵ�ǰĿ¼��DIR��һ���ļ�
#define FS_DIR_DIR              0x81   //  ��ʾ������������չ�����ӵ�ǰĿ¼��DIR��һ��Ŀ¼
#define FS_DIR_FILE_CONT        0x00   //  ��ʾ������������չ����DIR��һ���ļ�
#define FS_DIR_DIR_CONT         0x01   //  ��ʾ������������չ����DIR��һ��Ŀ¼

#define FS_OPEN_NORMAL          0x0   //  ��ʾ�������ļ�
#define FS_OPEN_LN_NORMAL       0x80  //  ��ʾ���볤�ļ����������ļ�

#define FS_CD_ROOT              L"/"   //  ��ʾ�ı䵱ǰĿ¼����Ŀ¼����ǰFAT16֧�֣�
#define FS_CD_PARENT            L".."  //  ��ʾ�ı䵱ǰĿ¼����һ����Ŀ¼����ǰFAT16��֧�֣�

#define FS_GET_SPACE_TOTAL      0x00  //  ��ʾ��ȡ������ǰ�������ܿռ��������   
#define FS_GET_SPACE_FREE       0x01  //  ��ʾ��ȡ������ǰ������ʣ��ռ��������

//ret
#define FS_INFO_RW_ERR          1   //   ���̶�д����
#define FS_INFO_WRITE_PROTECT   2   //   ����д����
#define FS_INFO_UNFORMATTED     3   //   ����δ��ʽ��
#define FS_INFO_OVEFLOW         4   //   �ļ����������ļ��߽磬��Ŀ¼��������Ŀ¼�߽�
#define FS_INFO_NOT_EXIST       5   //   �ļ�������Ŀ���ļ�����Ŀ¼������Ŀ¼��
#define FS_INFO_SPACE_OUT       6   //   ��ʾ�ļ�������Ŀ¼����û�д��̿ռ䣬����д���ݻ�����չ��Ŀ¼
#define FS_INFO_DIR_OUT         7   //   �ļ�������Ŀ¼Ŀ¼����
#define FS_INFO_NOT_EMPTY       8   //   ɾ��Ŀ¼ʱ���أ���ʾɾ����Ŀ¼Ϊ�ǿ�Ŀ¼

// AppSupportFlag
#define AP_SUPPORT_LCD                   0x01
#define AP_SUPPORT_LED                   0x02
#define AP_SUPPORT_MENU                0x04
#define AP_SUPPORT_RECORD              0x08
#define AP_SUPPORT_FM                0x10
#define AP_SUPPORT_FMREC                0x20
#define AP_SUPPORT_BT                0x40
#define AP_SUPPORT_BT_A2DP               0x80
#define AP_SUPPORT_BT_OPP                0x100

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
#define APP_MAIN_TRC            1
#define APP_GUI_TRC             2
#define APP_BT_TRC              3
#define APP_MUSIC_TRC           4
#define APP_RECORD_TRC          5
#define APP_FM_TRC              6
#define APP_SETTING_TRC         7

/*
 * APP_ASSERT(value, message), ȷ��value��ֵΪ�棬����ͣס������ʾ�ļ������к� and message
 */

#define APP_ASSERT assert

/*
FUNC:
	Output trace information.
PARAM:
[level]
	Output level.
[fmt]
	The trace information that you want to output. Refer the usage of ANSI C printf().
**/
void app_trace(UINT16 level, char* fmt, ...);

//#define hal_HstSendEvent(SYS_EVENT,ch)

typedef enum
{
	CharType_NUM,    //���֣�0-9��
	CharType_UPER,    //��д��ĸ��A-Z��
	CharType_DNER,    //Сд��ĸ��a-z��
	CharType_SPACE,    //�ո� (" ")
	CharType_OTHER    //����ֵ
} CharType_e;

typedef struct
{
	UINT16 year;
	UINT8 month;
	UINT8 day;
} date_t;

typedef struct
{
	UINT8 hour;
	UINT8 minute;
	UINT8 second;
} ap_time_t;

typedef struct
{
	UINT8 loop_mode;
	UINT8 eq_mode;
	UINT8 repeat_count;
	UINT8 repeat_time;
} music_cfg_t;

#if APP_SUPPORT_FM==1           /*Surport fm*/
typedef struct
{
	UINT8   fm_band;             //if set the fm record
	UINT32 fm_sendfreq;           //if open the fm send
} fm_t;
#endif

#if APP_SUPPORT_BLUETOOTH==1     /*Surport bt*/

#define SIZE_OF_BDADDR  6
#define SIZE_OF_LINKKEY 16

#define MAX_BT_DEVICE_LIST      10
#define MAX_BT_DEVICE_NAME      16

#define MAX_BT_CALL_VOL_LEVEL  20//warkey 2.1
typedef struct
{
	UINT8 password[SIZE_OF_LINKKEY];
	UINT8 local_name[MAX_BT_DEVICE_NAME];
	UINT16 fix_address;
	UINT8 bt_address[SIZE_OF_BDADDR];
} btconfig_t;

#endif

//��ǰ����,Ŀ¼,�ļ�
typedef struct
{
	UINT8   disk;             //FS_DEV_TYPE��������usb�豸
	UINT32  subdir_entry;     //subdir no file_inode
	UINT32  file_entry;       //cluster no,���浱ǰĿ¼���ڵ���ţ����߲����б��������,file_inode
#ifdef APP_SUPPORT_PLAYLIST
	UINT8   DirNo[TOTALDIRLAYER];
	UINT8   dirLayer;
#endif
	INT8 subdir_name[MAX_FILE_LEN];        //8.3+'\0'
	INT8 file_name[MAX_FILE_LEN];    //8.3+'\0'
} file_location_t;


//************************************
//ϵͳ������������
//************************************
typedef struct
{
	//�Աȶ�ѡ��6~21
	INT8 DisplayContrast;
	
	//��������ʱ��,Ĭ��ֵΪ0ah=5��
	INT8 LightTime;
	
	//standby��ʱ, in minutes
	INT8 StandbyTime;
	
	//��������
	INT8 langid;
	
	//�������ɫ  7Color BackLight.
	//INT8 BLightColor;
	
	//���������.
	INT8 BackLight;
	
	INT8 DateFormat; /*0: year-month-day; 1: day-month-year; 2: month-day-year.*/
	
	INT8 Volume; // system volume
	
	music_cfg_t music_cfg;
	
	//������ѡ�����ѡ��
#if APP_SUPPORT_FM==1           /*Surport fm*/
	fm_t fm_value;
#endif
	
	UINT16 magic;
} comval_t;

#define SERIAL_NUMBER_MAX_SIZE              32

typedef struct
{
	UINT32 AppSupportFlag;
	UINT8  serial_no[SERIAL_NUMBER_MAX_SIZE];
} sysconfig_t;

typedef struct
{
	UINT8 lcd_width;
	UINT8 lcd_heigth;
	// display number
	UINT8 big_num_width;
	UINT8 small_num_width;
	// display time, width of ':'
	UINT8 big_colon_width;
	UINT8 small_colon_width;
	// battary icon postion
	UINT8 position_batt;
	// message position
	UINT8 message_y;
	// log image
	UINT8 log_image_count;
	UINT8 log_image_time;
	// color
	UINT16 back_color;
	UINT16 font_color;
} displayconfig_t;


typedef struct
{
	INT8    ListDesc[32];       //list�ļ�����������MYLIST / ARTIST / ALBUM / GENRE / COMPOSER / RATED
	UINT16    CatMembNum;         //���Ա��
	UINT16    ListItemNum;        //list������
	UINT8    Reserved[512 - 36]; //ͷ��Ϣռ��1����
} LISTHEAD;


typedef struct
{
	UINT8 switchflag;//�������¸���(Ϊ1��λ��ʾ��Ӧ��򿪣�Ϊ0ʱ��ʾ�ر�)
	INT32 RemainPlayCnt;//���¿ɲ��ŵĴ���
	INT32 originalPlayCnt;//ԭʼ�ɲ��Ŵ���
	
	date_t BeginDate;//��ʼ��������
	ap_time_t BeginTime;
	
	date_t ExpDate;//��ֹ��������(2004-12-30)
	ap_time_t ExpTime;
	
	INT32 RemainTime;//(��һ���ù�֮�����õ�Сʱ������ͬһ�׸��ǹ̶�ֵ)
	INT32 RemainStoreTime;//���¿ɴ洢��ʱ��    (����)//���ڱ����˽ӿڣ�����ʱ����
	INT8 DealOnClkBck;//0: ʱ�ӻص�ʱɾ�� 1��ʱ�ӻص�ʧЧ
} licensinf_t;

typedef struct                  //16�ֽ�
{
	UINT32    stroffset;          //���ִ��ļ���ƫ�ƣ��ֽ�Ϊ��λ
	UINT16	  strlength;          //���ִ�����, �ֽ�Ϊ��λ, ��������unicode
	UINT16    MembItemNum;        //�����Ա���ж���list��
	UINT32    FistItemOffset;     //�����Ա��list�����ļ���ƫ��,�ֽ�Ϊ��λ
	INT8    reserved[4];
} CATMEMB;


typedef struct
{
	UINT32    FirstCluster;       //����Ŀ¼���״غ�
	UINT32    FileEntry;          //�ļ�Ŀ¼���
} LISTITEM;                   //ÿITEMռ��8���ֽ�


//�ļ�����ʱ��
typedef struct
{
	UINT16 year;
	UINT8 month;
	UINT8 day;
	UINT8 hour;
	UINT8 minute;
	UINT8 second;
} fs_crt_time_t;

typedef struct
{
	UINT8   TIT2_length;
	INT8     *TIT2_buffer;
	UINT8   TPE1_length;
	INT8     *TPE1_buffer;
	UINT8   TALB_length;
	INT8     *TALB_buffer;
	UINT8   TENC_length;
	INT8     *TENC_buffer;
} ID3Info_t;


extern comval_t         *g_comval;
extern sysconfig_t      g_sysconfig;
extern displayconfig_t  g_displayconfig;

UINT32 FS_GetSpace(UINT8 disk, UINT8 type);

/// Return the date of build of system.
/// The format is 0xYYYYMMDD.
UINT32 AP_GetBuildDate(void);
/// Return the version number
/// The format is 0xAABB, A is the big version, B is little version
UINT32 AP_GetVersion(void);

INT32 ResShowMultiLongStr(UINT16 id, UINT8 string_id, INT32 timeCount);
INT8  TM_GetDateAndTime(date_t *date, ap_time_t *time); //��ȡ�����գ�Сʱ��
//ʱ��Ϊ24Сʱ�ƹ�����ʽ
BOOL  TM_SetDateAndTime(date_t *date, ap_time_t *time);
//���ø�������ʱ�䣬��ʽ:seconds pass 1970/01/01 00:00:00������ʽ
BOOL  TM_SetGWDateAndTime(date_t *date, ap_time_t *time);


//�����ļ�ϵͳ�����Ƿ���ڣ������ڣ����ط���״̬���ļ�ϵͳ���ͣ��Ƿ��ʽ����
//para: �߼��������ţ�����ģʽ��ͨ��=0��
//ret:  TRUE/FALSE
BOOL MountDisk(UINT8 disk);


// Initialize fsel Module, Must call it before call other fsel Module functions
BOOL fselInit(UINT8 type,  UINT8 mode, UINT8  PlayListOrCommonDir, UINT8 CardOrFlash);
//open the playlist file
BOOL fselPlayListEnt(TCHAR *filename);
//close the playlist file and reset the fsel parament
BOOL fselPlayListExit(void);
//get the fsel next file
BOOL fselGetNextFile(UINT32 *entry);
//get the fsel prev file
BOOL fselGetPrevFile(UINT32 *entry);
//get the next group
BOOL fselGetNextGroup(TCHAR *groupname);
//get the prev group
BOOL fselGetPrevGroup(TCHAR *groupname);
//get the group long name
BOOL fselGetCurGroupL(TCHAR *grouplongname, UINT16 iLen);
//get the long filename
BOOL fselGetCurFNameL(TCHAR *longfilename, UINT8 iLen);
// Set  location of fsel module
UINT8 fselSetLocation(file_location_t  *location);
// Get location of fsel module
BOOL fselGetLocation(file_location_t  *location);
// Enter the current dir
BOOL fselEntCurrentDir(TCHAR *strDir);
//Return the parent dir
BOOL fselRetParentDir(TCHAR *strDir);
// HANDLE and match the case of current file delected.
BOOL fselDelFile(UINT8 type);
// Set select mode
BOOL fselSetMode( UINT8 mode);
// Get total of files in currect directory
UINT16 fselGetTotal(void);
//Get the number of currect selected file
UINT16 fselGetNo(void);
// Get error id, it is the fail information of last called fsel module function.
UINT8 fselGetErrID(void);
// Get the next file, location changed
BOOL fselGetNext(UINT32 *entry);
//Get the previous file, location changed
BOOL fselGetPrev(UINT32 *entry);
// Get file according to file number, location changed
BOOL fselGetByNO( UINT32 *strFile, const UINT32 Num);
// Seek for next valid directory
BOOL fselNextDir(TCHAR *strDir);
// Seek for previous valid directory
BOOL fselPrevDir(TCHAR *strDir);
//if location->filename is not exists, just return
INT32 GenNewName(file_location_t *loation);
//del the list
BOOL playlistdel(TCHAR *filename);
BOOL playlistclassNext(TCHAR *classname);
BOOL playlistclassPrev(TCHAR *classname);
BOOL playlistnext(TCHAR *filename);
BOOL playlistprev(TCHAR *filename);
//common file:Set location of fsel module
UINT8 fselSetLocationC(file_location_t  *location);
// common file :Get location of fsel module
BOOL fselGetLocationC(file_location_t  *location);



UINT32 tcslen(PCWSTR str);
INT32  tcscmp(PCWSTR s1, PCWSTR s2);
INT32  tcsncmp(PCWSTR s1, PCWSTR s2, UINT32 n);
WCHAR* tcscpy(PWSTR to, PCWSTR from);
WCHAR* tcsncpy(PWSTR dst, PCWSTR src, UINT32 n);
WCHAR* tcscat(PWSTR s, PCWSTR append);
WCHAR* tcsncat(PWSTR dst, PCWSTR src, UINT32 len);
WCHAR* tcschr(PCWSTR p, WCHAR ch);
WCHAR* tcsrchr(PCWSTR p, WCHAR ch);
WCHAR* tcsupr(PWSTR p);

void FS_FileTimeConvert(fs_crt_time_t *pfileDateTime, UINT32 time);

#define AP_Support_LCD()          (g_sysconfig.AppSupportFlag&AP_SUPPORT_LCD)
#define AP_Support_LED()          (g_sysconfig.AppSupportFlag&AP_SUPPORT_LED)
#define AP_Support_MENU()         (g_sysconfig.AppSupportFlag&AP_SUPPORT_MENU)
#define AP_Support_RECORD()       (g_sysconfig.AppSupportFlag&AP_SUPPORT_RECORD)
#define AP_Support_FM()           (g_sysconfig.AppSupportFlag&AP_SUPPORT_FM)
#define AP_Support_FMREC()        (g_sysconfig.AppSupportFlag&AP_SUPPORT_FMREC)
#define AP_Support_BT()           (g_sysconfig.AppSupportFlag&AP_SUPPORT_BT)
#define AP_Support_A2DP()         (g_sysconfig.AppSupportFlag&AP_SUPPORT_BT_A2DP)
#define AP_Support_OPP()          (g_sysconfig.AppSupportFlag&AP_SUPPORT_BT_OPP)

#endif  /* _AP_COMMON_H_*/

