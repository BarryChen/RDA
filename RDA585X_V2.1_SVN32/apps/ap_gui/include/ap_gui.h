/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#ifndef _AP_GUI_H_
#define _AP_GUI_H_

#include "ap_common.h"
#include "ap_message.h"
#include "gui.h"

//����������ؼ���x����
#if APP_SUPPORT_RGBLCD==1
#define POSITION_A            (0)
#define POSITION_B            (20)
#define POSITION_C            (44)
#define POSITION_D            (68)
#define POSITION_E            (90)
#define POSITION_F            (110)
#define POSITION_BATT         (141)
#define POSITION_G            (0)
#elif APP_SUPPORT_LCD==1
#define POSITION_A            (0)
#define POSITION_B            (15)
#define POSITION_C            (38)
#define POSITION_D            (51)
#define POSITION_E            (68)
#define POSITION_F            (86)
#define POSITION_BATT         (114)
#define POSITION_G            (0)

#endif
#define GUI_MSG_FLAG_DISPLAY           1  // display and return
#define GUI_MSG_FLAG_CONFIRM          2  // display and wait user select yes or no
#define GUI_MSG_FLAG_KEY                  4  // display and wait user press any key
#define GUI_MSG_FLAG_WAIT                  8  // display and wait 2 seconds

typedef struct
{
	INT8 min;
	INT8 max;
	INT8 value;
	INT8 step;        //����,ÿ��+/- ��ֵ // step 0 for progress display
} slider_t;

typedef struct
{
	INT32 min;
	INT32 max;
	INT32 value;
} progress_t;

typedef struct
{
	INT8    mode;
	INT8    Language;
} Scroll_t;


typedef struct
{
	UINT16 HeadIcon;//ͷ��ͼ���IDֵ
	UINT16 HeadStr;//ͷ����Ϣ��IDֵ
	INT8 *string;//��Ϣ��ʾ������ʾ���ִ���ַ
} dialog_t;

#define  AP_MAX_FROMAT      9

extern const char *date_formats[AP_MAX_FROMAT];

#define GUI_DisplayLock(islock)                     {if(islock) GUI_ResShowPic(GUI_IMG_LOCK,0,0);}
#define GUI_DisplayNumberD(number, part)        GUI_DisplayNumbers(POSITION_D,part==0?0:8,3,GUI_IMG_SMALL_NUM, number)
#define GUI_DisplayNumberCD(number, part)       GUI_DisplayNumbers(POSITION_C,part==0?0:8,3,GUI_IMG_SMALL_NUM, number)
#define GUI_DisplayTimeF(time, part)        GUI_DisplayTime(POSITION_F,part==0?0:8,0,GUI_IMG_SMALL_NUM, time)
#define GUI_DisplayTimeEF(time, part)       GUI_DisplayTime(POSITION_E,part==0?0:8,GUI_TIME_DISPLAY_ALL,GUI_IMG_SMALL_NUM, time)
#define GUI_DisplayStringG(string)              GUI_DisplayText(POSITION_G,32,string)

/*Display String*/
#define GUI_DisplayIconA(id)                GUI_ResShowPic(id, POSITION_A, 0)
#define GUI_DisplayIconB(id)                GUI_ResShowPic(id, POSITION_B, 0)
#define GUI_DisplayIconC(id)                GUI_ResShowPic(id, POSITION_C, 0)
#define GUI_DisplayIconD(id)                GUI_ResShowPic(id, POSITION_D, 0)
#define GUI_DisplayIconE(id)                GUI_ResShowPic(id, POSITION_E, 0)

#define MENU_CALLBACK_QUERY_ACTIVE       1
#define MENU_CALLBACK_QUERY_VISIABLE     2
#define MENU_CALLBACK_QUERY_TEXT         3

typedef INT32 (*menu_callback)(INT32 type, INT32 value, INT32 param, UINT8 **string);

#define LIST_TYPE_MULTI_SELECT           0
#define LIST_TYPE_SINGLE_SELECT          1
#define LIST_TYPE_CUSTOM_IMAGE           2
#define LIST_TYPE_HORIZONTAL             4

#define LIST_CALLBACK_GET_ITEM           1   // return item select status and string
#define LIST_CALLBACK_SELECT_ITEM        2   // an item selected, retun 0 to end list
#define LIST_CALLBACK_CANCEL             3

typedef INT32 (*list_callback)(INT32 type, INT32 value, INT8 **string);

#define GUI_TIME_DISPLAY_AUTO               0
#define GUI_TIME_DISPLAY_HOUR_MINUTE        1
#define GUI_TIME_DISPLAY_MINUTE_SECOND      2
#define GUI_TIME_DISPLAY_ALL                3

INT32  GUI_Delete(file_location_t *location, UINT8 type, UINT8 string_id);
INT32  GUI_DeletePlayList(file_location_t *location, UINT8 type, UINT8 string_id);
INT32  GUI_Directory(file_location_t *location, UINT8 type, UINT8 string_id);
INT32  GUI_PlayList(file_location_t *location, UINT8 type, UINT8 string_id);
INT32 GUI_Display_Menu(UINT8 menu_Id, menu_callback callback);
INT32  GUI_Slider(slider_t *slider, const void (*callback)(INT32 res_id));
INT32  GUI_Display_Progress(const progress_t *progress);
void GUI_DisplayBattaryLever(void);
INT32 GUI_Display_List(UINT8 style, UINT16 image_id, UINT16 count, UINT16 active, list_callback callback, BOOL is_dynamic);

INT32  GUI_Dialog(dialog_t *dialog, INT32 string_id);
void   GUI_SetScrollRegin( const region_t *region, INT8 step);
void   GUI_Scroll( const INT8 *scrollstr, BOOL mode); //(�ַ�����ַ���Ƿ��ʼ��trueΪҪ��ʼ��)
UINT32 GUI_DisplayMessage(INT16 title, INT16 message, INT8 *string, UINT8 flag);
void GUI_DisplayNumbers(INT32 x, INT32 y, UINT8 size, UINT16 img_id, UINT32 number);
void GUI_DisplayTime(INT32 x, INT32 y, UINT8 type, UINT16 img_id, UINT32 time);
void GUI_DisplayTimeText(INT32 x, INT32 y, UINT8 type, UINT32 time);
void GUI_DisplayDateText(INT32 x, INT32 y, UINT8 islong, UINT32 date); // long = 0, year in two numbers, long = 1, year in four numbers
void GUI_DisplayTimeLEDs(UINT32 time);
UINT32 GUI_KeyNumberHandle(UINT32 input_num);

#define KEY_MAP_VALUE_MAXLEN               5
typedef struct
{
	UINT32 key;
	UINT8 len;
	char value[KEY_MAP_VALUE_MAXLEN];
} tKeyMapTable; /*kye map table*/

extern const tKeyMapTable _gKeyMapTable[];
extern const UINT8 _gKeyCount;
#define KEYMAP_COUNT _gKeyCount
typedef INT32 (*searchlist_callback)(INT32 type, INT32 value, INT8 **string, UINT16 *count, INT8 *searchtext);
#define SEARCHLIST_CALLBACK_GET_ITEM           1   // return item select status and string
#define SEARCHLIST_CALLBACK_SELECT_ITEM        2   // an item selected, retun 0 to end list
#define SEARCHLIST_CALLBACK_TEXT               3   // return text
#define SEARCHLIST_CALLBACK_CANCEL             4
INT32 GUI_DisplaySearchList(char* string, INT32 maxlen, searchlist_callback callback);

#define GUI_INPUT_TYPE_NUMBER              0
#define GUI_INPUT_TYPE_TEXT                1

#define GUI_INPUT_MAXLNE            48

typedef INT32 (*input_callback)(INT8* string);
// max len must < 48
INT32 GUI_InputText(INT8* string, INT32 strlen, INT8 type, BOOL ispwd, INT32 maxlen, input_callback callback);



#endif/*_AP_GUI_H_*/

