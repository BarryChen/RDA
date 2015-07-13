/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#ifndef _AP_MUSIC_H_
#define _AP_MUSIC_H_

#include "ap_gui.h"
#include "ap_media.h"

//���������ĵ��ڼ���
#define MUSIC_MAX_VOLUME           AUD_MAX_LEVEL

//------------------------------
#define FollowReadTempShortName   "FLWWAV  TMP"
#define FollowReadTempLongName    L"FLWWAV.TMP"
#define scrolltime_def           260/20           //20ms * 13 = 260ms//����Ƶ�ʷ�Ƶϵ��
#define ABReplayMiniTime_def     400/20           //20ms * 10 = 200ms//AB��������С����
//������Щ��������ʱ��Ƚϳ�����ÿ�׸��һʱ�䶼�ȼ������ʱ�䣬�����������ֹͣ
//  ״̬�³���NEXT���ʱ�ٶȽ������ֲ��������·�������ֹͣ״̬��ѡ��һ�׸�ʱ
//  ��ʼʱ��Ϊ0��û�м����ʱ�䣩������ʱҪ������һ�ף���ղ����׸��ʱ��Ͳ�
//  ���㣬����ֱ��������һ�ף�������ȴ� 2���ӻ�δ�а�������ʼ���㵱ǰ������
//  ��ʱ�䲢��ʱ��ʾ�� ���������������ȴ���ʱ�� (�����ֵ/2����Ҫ���ʱ�䣬
//  ����� +- 0.5��)
#define CaltimeCNT_def           2                //��1��û�а����Ϳ�ʼ����ʱ��

#define RESULT_CURR_UI           NULL//�ڵ�ǰ����
#define RESULT_LAST_UI           RESULT_USER1+1//����һ����
#define RESULT_UI_STOP           RESULT_USER1+2//�� STOP ����
#define RESULT_UI_FOLLOW           RESULT_USER1+3//�� STOP ����

//�ڶ��ַ�ʽ�ص� STOP ����,���ַ�ʽ����ֹͣ����
#define RESULT_UI_STOP2          RESULT_USER1+3
#define RESULT_UI_PLAY           RESULT_USER1+4//�� PLAY ����
#define RESULT_RE_INIT           RESULT_USER1+5

#define Music_Reset_Head         0
#define Music_Reset_End          1
#define Music_Cur                2//ѡ��ǰ
#define Music_Next               3//ѡ��һ��
#define Music_Prev               4//ѡ��һ��

//g_DiskStateFlag mask
#define SetDiskOKFlag_def         0x80
#define ClrDiskOKFlag_def         0x7f
#define SetDiskNoFile_def         0x40
#define ClrDiskNoFile_def         0xbf
#define SetCardOKFlag_def         0x08
#define ClrCardOKFlag_def         0xf7
#define SetCardNoFile_def         0x04
#define ClrCardNoFile_def         0xfb

#define BASE_SRS_WOW              0x40
#define eqcount_def               8
#define DEF_RecLowBatt            5//���������¼���ĵ�ѹ

#define  AUDPLY_LYRIC_MAX_LINE_SIZE   64   /* pre-defined max lyric buffer size for one line : bytes ; must be two bytes aligned */

//�ж��ǵ͵�Ĵ���������ѹֵ����DEF_RecLowBattֵ�����������ֵʱȷ��Ϊ�͵�
#define DEF_RecLowBatTime         5
#define ExproTimeDef              5//�������ʱֵ
#define IntCntValDef              5  // second

//���¶����˽ṹ�еĿ���λ
#define on_AllItem_def             0x80
#define on_PLAYCNT_def             0x20
#define on_BEGINDAT_def            0x10
#define on_EXPDATE_def             0x08
#define on_REMAINTM_def            0x04
#define on_REMAINST_def            0x02
#define on_DEALCLKBCK_def          0x01

#define DisplayItem0_def           0
#define DisplayItem1_def           1
#define DisplayItem2_def           2
#define DisplayItem3_def           3
#define DisplayItem4_def           4
#define DisplayItem5_def           5
#define DisplayItem6_def           6

#define RemainPlayCntDisp_def      DisplayItem0_def
#define OriginalPlayCntDisp_def    DisplayItem1_def
#define BeginDateTimeDisp_def      DisplayItem2_def
#define ExpDateTimeDisp_def        DisplayItem3_def
#define RemainTimeDisp_def         DisplayItem4_def
#define RemainStoreTimeDisp_def    DisplayItem5_def
#define DealOnClkBckDisp_def       DisplayItem6_def
#define lincensinf_maxitem_def     DisplayItem6_def

//===========================================================
//���涨��ṹ����
//===========================================================
/*for compiler compatible */
typedef struct
{
	UINT16 TimeMs;
	UINT8 TimeSecond;
	UINT8 TimeMinute;
	UINT8 TimeHour;
} Full_time_t;

typedef enum
{
	MusicStop,   // stop mode (0)
	NormalPlay,         //��ͨģʽ����ʾ(1)
	LoopPlay,         //��������(2)
	FollowRec,//����¼��(3)
	CompareAB,//�Աȷ�AB(4)
	ComparePlay//�ԱȲ���(5)
} Replay_status_t;

typedef enum
{
	Stop_key,
	Play_play,
	Play_key
} Music_sel_mod_t;

typedef enum
{
	mplaylist,
	martists,
	malbum,
	mgenre,
	mcomposers,
	mlistnor
} play_listmod_t;

typedef struct
{
	UINT8 min;
	UINT8 sec;
	UINT8 p1s;
	UINT16 buffaddr;
} LrcTime_t;

//AP_MUSICʹ�õ�ϵͳ����
typedef struct
{
	INT8 volume;               //
	INT8 repeat_mode; //repeat �˵������  //+1
	INT8 loop_mode;
	eq_t eq_type;              //+2
	//INT8 play_rate;     //�����ٶ� //+3
	UINT8 replaytimes;          //��������// +4
	//UINT8 replaygap;            //�������//+5
	UINT8 maxReplayTime;          //��������// +4
	//Open_mod_t MusicOpenMod;    //�򿪷�ʽ +6
	file_location_t location;  //+ 9
	UINT32 BreakTime;
	//ap_time_t BreakPTDisTime;     //�ϵ�ʱ�䱸�� +
	//play_listmod_t play_listmod;//�����б�
	//UINT8 fselmod;               //�ļ�ѡ������ģʽ
	//UINT8 PLmod;                   //play list��ģʽ
	//UINT8 Srs;                   // SRS WOW ��Ч
	//UINT8 Wow;
	//UINT8 Bass;
	UINT8 Lyric_Size;
	BOOL show_lyric;
#if APP_SUPPORT_FM_OUT
	BOOL fm_out;
	UINT32 freq;  // fm output freq
#endif
	UINT16 magic;                // +0
} music_vars_t;

//===========================================================

//--------- [UpDisplayFlag] ---Ҫ��ˢ�±�־
#define up_musictypeflag     0x8000
#define up_cardflag         0x4000
#define up_musicbitrate      0x2000
#define up_musicloopmode     0x1000
#define up_musicsequence     0x0800
#define up_musiceqmode       0x0400
#define up_musiccurrenttime  0x0200
#define up_musictotaltime    0x0100
#define up_musicbattery      0x0080
#define up_musicinformation  0x0040
#define up_musicscroll      0x0020
#define up_musiclyric       0x0010
#define up_musicfmout       0x0008
#define up_followmode          0x0004

//-----------------------------���ˢ�±�־
#define clr_musictypeflag    0x7fff
#define clr_lyricflag        0xbfff
#define clr_musicbitrate     0xdfff
#define clr_musicloopmode    0xefff
#define clr_musicsequence    0xf7ff
#define clr_MPRLmode         clr_musicsequence
#define clr_musiceqmode      0xfbff
#define clr_musiccurrenttime 0xfdff
#define clr_musictotaltime   0xfeff
#define clr_musicbattery     0xff7f
#define clr_musicinformation 0xffbf



//=========����STOP�������==========
#define SetStopInterface()          \
    {                                   \
    GUI_ClearScreen(NULL);       \
    ui_auto_update=FALSE;               \
    UpDisplayFlag=UpDisplayFlag |       \
                  up_musictypeflag |    \
                  up_cardflag |        \
                  up_musicbitrate |     \
                  up_musicloopmode |    \
                  up_musicsequence |    \
                  up_musiceqmode |      \
                  up_musiccurrenttime | \
                  up_musictotaltime |   \
                  up_musicbattery |     \
                  up_musicfmout; \
    }

//=================================

//=========����ˢ��ȫ�������=====
#define SetFullInterface()          \
    {                                   \
    GUI_ClearScreen(NULL);                  \
    ui_auto_update=FALSE;               \
    UpDisplayFlag=UpDisplayFlag |       \
                  up_musictypeflag |    \
                  up_cardflag |        \
                  up_musicbitrate |     \
                  up_musicloopmode |    \
                  up_musicsequence |    \
                  up_musiceqmode |      \
                  up_musiccurrenttime | \
                  up_musictotaltime |   \
                  up_musicbattery |     \
                  up_musicinformation |  \
                  up_musicfmout   ;  \
    }

//======================================

//=========����ˢ��ȫ�������=====
#define SetStopToPlayInterface() \
    {                                \
    ui_auto_update=FALSE;            \
    UpDisplayFlag=UpDisplayFlag |    \
                  up_musictypeflag | \
                  up_cardflag |     \
                  up_musicbitrate |  \
                  up_musicloopmode | \
                  up_musicsequence | \
                  up_musiceqmode |   \
                  up_musictotaltime | \
                  up_musicbattery |   \
                  up_musicinformation | \
                  up_musicfmout; \
    }

//======================================

#define StartIntro(mode)
/*\
    {                                         \
    g_introCntMode = mode;                    \
    / *g_OldSecTempBackup = g_music_vars.BreakPTDisTime.second;* / \
    g_introCNT = 0;                           \
    }
*/


//===================��ʷ���ʱ�������(��ʾ����ʱ������ܳ������ֵ)============עҪ�ñ�׼ʱ��д��
#define         MaxOneScreenTimeMin        0                   //����ָ������   ��0-59��                     //
#define         MaxOneScreenTimeSec        5                   //����ָ�������� ��0-59��                     //��ʾ5.5��
#define         MaxOneScreenTime100Ms      5                   //����ָ����100������ 9 ��ʾ 900ms��0-9��      //
//=======================================================================



void MUSIC_GetPara( void );/*Read Para From Flash */
INT32 MUSIC_Main(void);
INT32 MUSIC_DisplayPlay( void );
INT32 MUSIC_DisplayStop( void );
INT32 MUSIC_FollowRec( void );    //����¼��
INT32 MUSIC_LyricDisplay( void );

INT32 ui_play_menu( void );
INT32 ui_stop_menu( void );
INT32 ui_SRS_menu( void );
INT32 ui_EQ_menu( void );
INT32 ui_loop_menu( void );
INT32 ui_follow_compare( void );
INT32 ui_followAB( void );     //����AB
INT32 ui_compareAB( void );    //�Ƚ�AB
INT32 ui_comparePlay( void );  //�Ƚϲ���
INT32 ui_srs_user_menu(void);

#ifdef _LICENSE_SUPPORT_
INT32 ui_licensinf(INT32 string_id);
#endif

//---------------------------

//-------class display-------
void RealDisplay( void );
void RefreshMainScreen( void );
void MUSIC_AudioBitRateDisplay( void );          //��ʾ������
void MUSIC_AudioTypeDisplay(void);//��ʾ��������ͼ��
//---------------------------

//-------class functions-----
INT32 play_key_next( UINT32 key );
INT32 play_key_prev( UINT32 key );
INT32 ToNextMusic( Music_sel_mod_t mod );
INT32 MUSIC_GetInformation( void );
BOOL  MUSIC_SelectAudio( UINT8 Direct, Music_sel_mod_t mod );
void  SelectCurMusic(void);//ѡ��ǰ����
INT32 MUSIC_HandleKey(UINT32 key);
INT32 ChangeVolume( UINT32 key );
INT32 DealMusicFileEnd( INT8 DisMod );
INT32 DealPlayKey( void );
void  FadeOut( BOOL DisMod );
INT32 PrevMusicInPlay(void);

INT32 InitFileSelector(void);
INT32 UpdateFileSelector(void);
void  ErrReport(UINT8 status);
//------------------------------

INT8 NextItem(licensinf_t *infbuf, INT8 cnt,  INT32  lan_id);
INT8 PrevItem(licensinf_t *infbuf, INT8 cnt,  INT32 lan_id);
void OpenMusic( void );
void DealForExitRec( void );


#if APP_SUPPORT_LYRIC==1//֧�ָ����ʾ����

// load the lyric matching the music file specified by strMusicFile. position initialized.
// return the page mode , 0 means fail.
BOOL  lyricLoad( const UINT32 file_entry);

// Get the lyric by time, limited len, return next time
BOOL  lyricGetNext( INT8 * strLyric, UINT8 len, UINT32 *nextTime);

// Load lyric background
BOOL  lyricPreLoad(void);

#endif


UINT32 MUSIC_GetPlaySeconds(void);//Get Cur Play Time

/* ------�����ļ�tag���� ------*/
BOOL mp3GetID3Info(const UINT32 fileEntry, ID3Info_t *ID3Info, INT32 string_id);
BOOL oggGetTAGInfo(const TCHAR *filename, ID3Info_t *ID3Info, INT32 string_id);

INT32 MUSIC_Entry(INT32 param);

#endif     /*_AP_MUSIC_H_*/



