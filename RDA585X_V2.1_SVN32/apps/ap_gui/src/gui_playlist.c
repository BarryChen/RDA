/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"

#ifdef APP_SUPPORT_PLAYLIST
extern UINT8  PlayListFileName[];
extern UINT8  PlaylistClassTotal;
extern UINT16  PlaylistCurClass;
extern BOOL  g_ScrollFlag;
extern BOOL  ui_auto_update;
extern UINT16  PlaylistCurItem;
#endif

/********************************************************************************
* Description : show the directory.
*
* Arguments  :
*            location: get thedisk type
*            Invpath: the "\" or name
*            Namebuf:the string you will display
*            type : show file kind
*            string_id: language type
* Returns     : the focus line number
* Notes       :  only display the valid directory
*
********************************************************************************/
UINT8 ShowPlaylist(file_location_t *location, INT8 *Longnamebuf, UINT8 string_id)
{

#ifdef APP_SUPPORT_PLAYLIST
	region_t    region;
	UINT8        Invpath = 0;
	
	ui_auto_update = FALSE;
	
	GUI_ClearScreen(NULL);
	
	//display icon & string
	if( Longnamebuf[0] == '\\' && Longnamebuf[1] == 0x00)           //��ǰĿ¼���ϼ�Ŀ¼����ʾ
	{
		//display first class name in line 1 : ROOT icon
		Invpath = 0;                //mode 0
		if(location->disk == FS_DEV_TYPE_FLASH)
		{
			GUI_ResShowPic(DIRROOT, 0, 0);
		}
		else
		{
			GUI_ResShowPic(DIRCARD, 0, 0);
		}
		
		//display string after ROOT icon
		GUI_DisplayText(14, 0, PlayListFileName);//list��������ַ�����8����COMPOSER,A3L����Ҫ��ʾ
		
		//display next class name in line 2
		GUI_ResShowPic(DIRROOTA, 0, 16);               //display normal directory icon
		if( fselGetCurGroupL(Longnamebuf, LISTNAME_DISP_LENNGTH) )
		{
			//   unitochar(Longnamebuf, LISTNAME_DISP_LENNGTH, string_id);
			GUI_DisplayText(18, 16, Longnamebuf);
		}
	}
	else
	{
		//display first class name in line 1
		Invpath = 1;                    //mode 1
		GUI_ResShowPic(DIRROOTA, 0, 0);     //display normal directory icon
		
		if( fselGetCurGroupL(Longnamebuf, LISTNAME_DISP_LENNGTH) )
		{
			//     unitochar(Longnamebuf, LISTNAME_DISP_LENNGTH, string_id);
			GUI_DisplayText(18, 0, Longnamebuf);
		}
		
		if ( PlaylistCurClass >= PlaylistClassTotal )
		{
			goto Invert;    //display inverted line
		}
		
		//display next class name in line 2
		if( fselGetNextGroup(Longnamebuf) )         //�����ǰĿ¼�ĺ������з���Ҫ���Ŀ¼����ʾ�ڵڶ���
		{
			GUI_ResShowPic(DIRROOTA, 0, 16); //display normal directory icon
			//       unitochar(Longnamebuf, LISTNAME_DISP_LENNGTH, string_id);
			GUI_DisplayText(18, 16, Longnamebuf);
			fselGetPrevGroup(Longnamebuf);          //pointer point the old position
			//   unitochar(Longnamebuf, LISTNAME_DISP_LENNGTH, string_id);
		}
	}
	
Invert:                 //display inverted line
	if( Invpath == 0 )  //parent path
	{
		//region.x=14;    //the parent directory string begins form 14 in x coordinate
		region.y = 16;
	}
	else                //sub directory
	{
		//region.x=18;    //the sub directory string begins form 18 in x coordinate
		region.y = 0;  //the second row
	}
	
	region.x = 18;  //the sub directory string begins form 18 in x coordinate
	region.width = 128 - region.x;
	region.height = 16;
	GUI_InvertRegion(&region);            //ѡ�е�Ŀ¼����
	GUI_UpdateScreen(NULL);
	ui_auto_update = TRUE;
	
	{
		INT8 * pLongnamebuf = Longnamebuf;
		unsigned INT8 strcnt = 0;
		
		while ( *pLongnamebuf != 0 )
		{
			strcnt++;
			pLongnamebuf++;
		}
		if ( strcnt > (128 - region.x) / 8 )
		{
			return Invpath;
		}
		else
		{
			return Invpath | 0x80;
		}
	}
#else
	return 0;
#endif
}


/********************************************************************************
* Description : select the directory.
*
* Arguments  :
*            location: get thedisk type
*            type : show file kind
*            string_id: language type
* Returns     :
*            other:    Successful
*            0:    Fail, detail information in fselError
* Notes       :  only select the valid directory
*
********************************************************************************/

INT32 GUI_PlayList(file_location_t *location, UINT8 type, UINT8 string_id)
{
#ifdef APP_SUPPORT_PLAYLIST
	UINT8 listrootdispflag, i, pagebak;
	INT8 * Strname;
	UINT8 PriInv, result;        //�������¼
	UINT32 key;
	BOOL needscroll;
	UINT8 ScrollbeginCnt, focusline, TimeoutCnt;
	region_t   scrollregion;
	
	if ( !fselSetLocation(location) )
	{
		return RESULT_IGNORE;
	}
	
	if (PlaylistClassTotal == 0)
	{
		return RESULT_IGNORE;
	}
	
	////////////////////////////////��ҳ//////////////////////////////////////
#if 0
	Strname = (INT8 *)STRNAMEBUF; //Strname buffer���ڵ�6ҳ0x7000λ��
#endif
	////////////////////////////////��ҳ//////////////////////////////////////
	
	memset(Strname, 0x00, LISTNAME_DISP_LENNGTH + 1);
	
	focusline = ShowPlaylist(location, Strname, string_id); //display cur class level condition
	listrootdispflag = 0;
	
	while(MESSAGE_Get() != 0)
	{
		;
	}
	
	MESSAGE_Initial(NULL);
	
	needscroll = FALSE;
	ScrollbeginCnt = TimeoutCnt = 0;
	PlaylistCurItem = 0;
	
	while(1)
	{
		key = MESSAGE_Get();
		switch(key)
		{
		case AP_KEY_NEXT:
		case AP_KEY_NEXT | AP_KEY_HOLD:
			if ( listrootdispflag == 1 )       // display the listroot
			{
				Strname[0] = 0x00;
				focusline = ShowPlaylist(location, Strname , string_id);
				listrootdispflag = 0;
				PlaylistCurItem = 0;
			}
			else
			{
				fselGetNextGroup(Strname);
				focusline = ShowPlaylist(location, Strname , string_id);
			}
			needscroll = FALSE;
			ScrollbeginCnt = 0;
			break;
			
		case AP_KEY_PREV:
		case AP_KEY_PREV | AP_KEY_HOLD:
			if ( PlaylistCurClass == 1 )       // if it is the parent do nothing
			{
				Strname[0] = '\\';
				Strname[1] = 0x00;
				focusline = ShowPlaylist(location, Strname , string_id);
				listrootdispflag = 1;           // display the listroot
				PlaylistCurItem = 0;
			}
			else
			{
				fselGetPrevGroup(Strname);
				focusline = ShowPlaylist(location, Strname , string_id);
			}
			needscroll = FALSE;
			ScrollbeginCnt = 0;
			break;
			
		case AP_KEY_PLAY | AP_KEY_UP:
			fselSetLocation(location);         //restore the selecting result
			result = RESULT_IGNORE;
			goto funreturn;
			break;
			
		case AP_KEY_MODE | AP_KEY_LONG:
			if ( fselGetLocation(location) )   //store the selecting result
			{
				result = RESULT_NULL;
			}
			else
			{
				result = RESULT_IGNORE;
			}
			goto funreturn;
			break;
			
		case AP_KEY_MODE | AP_KEY_UP:
			break;
			
		case AP_MSG_WAIT_TIMEOUT:
			TimeoutCnt++;
			if ( TimeoutCnt >= 24 )  //192����˳�����ΪҪ����������
			{
				if (fselGetLocation(location))     //store the selecting result
				{
					result = RESULT_NULL;
				}
				else
				{
					result = RESULT_IGNORE;
				}
				goto funreturn;
			}
			break;
			
		case AP_MSG_SD_OUT:
			//��������SD������SD���ֱ��γ����ͷ��ذγ��ź�
			if (location->disk == FS_DEV_TYPE_TFLASH)
			{
				result = RESULT_SD_OUT;
				goto funreturn;
			}
			break;
			
		case AP_MSG_SD_IN:
			break;
			
		case AP_MSG_RTC:
			if ( (ScrollbeginCnt == 1) && (needscroll == FALSE) && ((focusline & 0x80) == 0) ) //���ִ��Ź���
			{
				needscroll = TRUE;
				g_ScrollFlag = FALSE;
				
				if ( focusline == 1 )
				{
					scrollregion.y  = 0;
				}
				else
				{
					scrollregion.y  = 16;
				}
				
				scrollregion.x      = 18;
				scrollregion.width  = 128 - scrollregion.x;
				scrollregion.height = 16;
				
				GUI_ClearScreen( &scrollregion );            //ѡ�е�Ŀ¼����
				GUI_SetScrollRegin( &scrollregion );
				GUI_Scroll(Strname, TRUE);         //����������ַ�����ַ���Ƿ��ʼ��trueΪҪ��ʼ��
				GUI_InvertRegion( &scrollregion );            //ѡ�е�Ŀ¼����
				GUI_UpdateScreen( &scrollregion );
			}
			else
			{
				ScrollbeginCnt++;
			}
			break;
			
		default:
			key = MESSAGE_HandleHotkey(key);
			if(key == RESULT_REDRAW)
			{
				focusline = ShowPlaylist(location, Strname , string_id);
			}
			else if( key != 0 )
			{
				fselGetLocation(location);      //store the selecting result
				result = key;
				goto funreturn;
			}
		} //switch()
		
		if ( g_ScrollFlag && needscroll )
		{
			INT8 i;
			
			g_ScrollFlag = FALSE;       //100ms��һ��
			
			GUI_InvertRegion( &scrollregion );          //ѡ�е�Ŀ¼����
			for ( i = 0; i < 4; i++ )
			{
				GUI_Scroll(Strname, FALSE);                 //����������ַ�����ַ���Ƿ��ʼ��trueΪҪ��ʼ��}
			}
			GUI_InvertRegion( &scrollregion );          //ѡ�е�Ŀ¼����
			GUI_UpdateScreen( &scrollregion );
		}
	}
	
funreturn:
	scrollregion.x = 0;
	scrollregion.y = 16;
	scrollregion.width = 128;
	scrollregion.height = 16;
	GUI_SetScrollRegin( &scrollregion );
	
	////////////////////////////////��ҳ//////////////////////////////////////
#if 0
	SetSWAPAddress(pagebak);
#endif
	////////////////////////////////��ҳ//////////////////////////////////////
	
	return result;
#else
	return 1;
#endif
}



