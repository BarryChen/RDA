/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_gui.h"


extern BOOL   ui_auto_update ;

/*===================================================================
�ӿ�: INT32 GUI_Dialog(dialog_t *dialog, INT32 string_id)
input:
    typedef struct
    {
    UINT16 HeadIcon;//ͷ��ͼ���IDֵ
    UINT16 HeadStr;//ͷ����Ϣ��IDֵ
    INT8 *string;//��Ϣ��ʾ������ʾ���ִ���ַ
    }dialog_t;

    dialog_t dialog: Ҫ��ʾ�ĶԻ������Ϣ
    INT32 string_id: ��ʾ�õ�����

output:
    return INT32:  NULL: ѡ����OK
                 RESULT_IGNORE: ʱ�䵽��δ��ȷ��,����PLAY��,��ѡ����NO
                 ������0��ֵ: ���ȼ�����
//===================================================================*/

INT32 GUI_Dialog(dialog_t *dialog, INT32 string_id)
{
	UINT32 key;
	BOOL Selectflag = FALSE; //FALSE(NO); TRUE(YES)
	BOOL need_draw  = TRUE;
	
	GUI_ClearScreen(NULL);
	ui_auto_update = FALSE;
	
	
	//��ʾҪ����ʾ����Ϣ
	if (dialog->string != NULL)
	{
		GUI_DisplayText(0, 16, dialog->string);
	}
	
	//��ʾͷ��ͼ��
	if (dialog->HeadIcon)
	{
		GUI_ResShowPic(dialog->HeadIcon, 0, 0);
	}
	
	//��ʾͷ������Ϣ
	if (dialog->HeadStr)
	{
		GUI_ResShowMultiString(dialog->HeadStr, string_id, 16, 0);
	}
	
	//��ʾѡ��ť��Ĭ��N0��
	GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
	
	while(1)
	{
		GUI_UpdateScreen(NULL);
		key = MESSAGE_Wait();                          //��ȡ��Ϣ
		
		switch (key)
		{
		case AP_KEY_NEXT:                            //YES��NO֮���л�
		case AP_KEY_PREV:
		case AP_KEY_NEXT | AP_KEY_HOLD:
		case AP_KEY_PREV | AP_KEY_HOLD:
			if(Selectflag)
			{
				Selectflag = FALSE;
				GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
			}
			else
			{
				Selectflag = TRUE;
				GUI_ResShowPic(GUI_IMG_CDEL_YES, 100, 0);
			}
			break;
			
		case AP_KEY_MODE | AP_KEY_UP:
			if(Selectflag)
			{
				//YESʱ����MODE���Ĵ���
			}
			else
			{
				return RESULT_IGNORE;//NOʱ����MODE���Ĵ���
			}
			
		case AP_MSG_WAIT_TIMEOUT://8���˳�
		case AP_KEY_PLAY | AP_KEY_UP:
			return RESULT_IGNORE;
			
			
		default:
			key = MESSAGE_HandleHotkey(key); //�ȼ�����
			if(key == RESULT_REDRAW)
			{
				need_draw = TRUE;
			}
			else if(key != 0)
			{
				return key;
			}
		}//switch(key)
	}//while(1)
}

