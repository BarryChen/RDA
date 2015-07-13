/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_gui.h"



//common variables
extern UINT16 fselFileNo;
#ifdef APP_SUPPORT_PLAYLIST
extern UINT16 fileoffset;
extern UINT16 PlaylistCurClass;
#endif
extern BOOL ui_auto_select;
extern BOOL ui_auto_update;
extern TCHAR* ExtMusic[];
extern TCHAR* ExtVoice[];


const region_t region = {64, 0, 64, 16};


/********************************************************************************
* Description : delete all the audio file of the selector directory.
*
* Arguments  :
* Returns     :
*            other:    Successful
*            0:    Fail, detail information in fselError
* Notes       :
*
********************************************************************************/
BOOL del_audio_files(TCHAR *Ext)
{
	TCHAR  DelStrname[MAX_FILE_LEN];
	UINT32 inode;
	
	while(FS_GetNextEntry(&inode, Ext, FS_DIR_FILE) == ERR_SUCCESS)//ѭ��ɾ��������չ�����ļ�
	{
		FS_GetEntryName(inode, MAX_FILE_LEN, DelStrname); // get file name
		if(FS_Delete(DelStrname) != ERR_SUCCESS)
		{
			return FALSE;
		}
	}
	return TRUE;
}


/********************************************************************************
* Description : draw the file infomation of the selector playlist file.
*
* Arguments  : location  :keep the file selector parament
               string_id :display language type
               filename: the filename of the selector playlist file
* Returns     :
* Notes       :
*
********************************************************************************/
void draw_del_filepl(file_location_t *location, TCHAR *filename, UINT8 string_id)
{
	ID3Info_t    ID3;
	BOOL         File_Flag = TRUE;
	INT32          NameNum;
	HANDLE       hfile;
	TCHAR        Longnamebuf_del[MAX_FILE_LEN];
	
	GUI_ClearScreen(NULL);
	ui_auto_update = FALSE;
	
	ID3.TIT2_length = 30;
	ID3.TPE1_length = 30;
	ID3.TALB_length = 30;
	ID3.TENC_length = 0;
#if 0
	ID3.TIT2_buffer = location->ID3buffer;
	ID3.TPE1_buffer = location->ID3buffer + 30;
	ID3.TALB_buffer = location->ID3buffer + 30 * 2;
	ID3.TENC_buffer = location->ID3buffer + 30 * 3; //ID3.TENC_length=0ʱ,Ҳ�븳�ϵ�ַ!���ɾ���ļ�����������!
	
	if(memcmp(filename + 8, music_ext[0], 3) == 0)      //�����MP3�ļ�ʱ�Ĵ���
	{
		if(mp3GetID3Info(filename, &ID3, string_id))                    //��ȡID3��Ϣ
		{
			if ( (location->ID3buffer[0] != 0) || (location->ID3buffer[30] != 0) || (location->ID3buffer[60] != 0) )
			{
				strcat(location->ID3buffer, " ");
				strcat(location->ID3buffer, location->ID3buffer + 30);
				strcat(location->ID3buffer, " ");
				strcat(location->ID3buffer, location->ID3buffer + 30 * 2);
				File_Flag = FALSE;
				GUI_DisplayText(0, 16, location->ID3buffer);
			}
		}
	}
	else if(memcmp(filename + 8, music_ext[1], 3) == 0
	        || memcmp(filename + 8, music_ext[2], 3) == 0
	        || memcmp(filename + 8, music_ext[3], 3) == 0)     //��������������ļ�ʱ�Ĵ���
	{
		ID3.TALB_length = 0;
		if (wmaGetTAGInfo(filename, &ID3))                //��ȡTAG��Ϣ
		{
			unitochar(location->ID3buffer, 30, string_id);            //TAG��Ϣ��UNICODE���ͣ�����Ҫת��ΪCHAR
			unitochar(location->ID3buffer + 30, 30, string_id);
			if ( (location->ID3buffer[0] != 0) || (location->ID3buffer[30] != 0) )
			{
				strcat(location->ID3buffer, " ");
				strcat(location->ID3buffer, location->ID3buffer + 30);
				File_Flag = FALSE;
				GUI_DisplayText(0, 16, location->ID3buffer);
			}
		}
	}
	else if(memcmp(filename + 8, voice_ext[0], 3) == 0 || memcmp(filename + 8, voice_ext[1], 3) == 0)    //�����¼���ļ�ʱ�Ĵ���
	{
		File_Flag = FALSE;
		GUI_DisplayText(0, 16, filename);
	}
	else
	{
		APP_ASSERT(FALSE);    //���ܳ����������
	}
#endif
	if(File_Flag)                                                    //��ȡ���ļ���
	{
		hfile = FS_Open(filename, FS_O_RDONLY, 0);
		
		NameNum = FS_GetEntryName(hfile, sizeof(Longnamebuf_del), Longnamebuf_del);
		
		if(NameNum != 0)
		{
			// unitochar(Longnamebuf_del,NameNum*2+2,string_id);
			GUI_DisplayText(0, 16, Longnamebuf_del);
		}
		else
		{
			GUI_DisplayText(0, 16, filename);
		}
		
	}
	// GUI_ResShowPic(GUI_IMG_DELL,0,0);
	GUI_ResShowMultiString(GUI_STR_CDELFILE, string_id, 16, 0);
	GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
	
	GUI_UpdateScreen(NULL);
	ui_auto_update = TRUE;
}

/********************************************************************************
* Description : draw the class infomation of the selector playlist.
*
* Arguments  : location  :keep the file selector parament
               string_id :display language type
* Returns     :
* Notes       :
*
********************************************************************************/
void draw_del_dirpl(file_location_t *location, UINT8 string_id)
{
#ifdef APP_SUPPORT_PLAYLIST
	INT8 Longnamebuf_del[28];
	INT8 buf[5];
	INT32 NameNum, Cardflag = 0;
	UINT8    PlaylistCurClass;
	
	GUI_ClearScreen(NULL);
	ui_auto_update = FALSE;
	
	GUI_ResShowPic(GUI_IMG_DELALL, 0, 0);
	GUI_ResShowMultiString(GUI_STR_CDELALL, string_id, 16, 0);
	GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
	
	if(location->disk == FS_DEV_TYPE_USBDISK)
	{
		Cardflag = DIRROOT - DIRCARD;
	}
	else if(location->disk != FS_DEV_TYPE_TFLASH)
	{
		APP_ASSERT(FALSE);
	}
	
	//    PlaylistCurClass = location->dirLayer; //get the class parament
	
	getClassParam(Longnamebuf_del, 28);
	if(Longnamebuf_del[0] == ':')                            //��Ŀ¼��ʾ
	{
		GUI_ResShowPic(DIRROOT - Cardflag, 0, 16);
		strcpy(buf, "ROOT");
		GUI_DisplayText(14, 16, buf);
	}
	else                                                //��Ŀ¼��ʾ�����ȡ��Ŀ¼�ĳ���
	{
		GUI_ResShowPic(DIRROOTA - Cardflag, 0, 16);
		// unitochar(Longnamebuf_del, 30, string_id);   //
		GUI_DisplayText(18, 16, Longnamebuf_del);
	}
	GUI_UpdateScreen(NULL);
	ui_auto_update = TRUE;
#endif
	
}



/********************************************************************************
* Description : draw the selector file of the selector directory.
*
* Arguments  : location  :keep the file selector parament
               string_id :display language type
* Returns     :
*            other:    Successful
*            0:    Fail, detail information in fselError
* Notes       :
*
********************************************************************************/
void draw_del_file(file_location_t *location, UINT8 string_id)
{
#ifdef APP_SUPPORT_PLAYLIST
	ID3Info_t    ID3;
	BOOL         File_Flag = TRUE;
	UINT8         NameNum;
	
	GUI_ClearScreen(NULL);
	ui_auto_update = FALSE;
	
	ID3.TIT2_length = 30;
	ID3.TPE1_length = 30;
	ID3.TALB_length = 30;
	ID3.TENC_length = 0;
#if 0
	ID3.TIT2_buffer = location->ID3buffer;
	ID3.TPE1_buffer = location->ID3buffer + 30;
	ID3.TALB_buffer = location->ID3buffer + 30 * 2;
	ID3.TENC_buffer = location->ID3buffer + 30 * 3; //ID3.TENC_length=0ʱ,Ҳ�븳�ϵ�ַ!���ɾ���ļ�����������!
	
	if(memcmp(&location->filename[8], music_ext[0], 3) == 0)        //�����MP3�ļ�ʱ�Ĵ���
	{
		if(mp3GetID3Info(location->filename, &ID3, string_id))                    //��ȡID3��Ϣ
		{
			if ( (location->ID3buffer[0] != 0) || (location->ID3buffer[30] != 0) || (location->ID3buffer[60] != 0) )
			{
				strcat(location->ID3buffer, " ");
				strcat(location->ID3buffer, location->ID3buffer + 30);
				strcat(location->ID3buffer, " ");
				strcat(location->ID3buffer, location->ID3buffer + 30 * 2);
				File_Flag = FALSE;
				GUI_DisplayText(0, 16, location->ID3buffer);
			}
		}
	}
	else if(memcmp(&location->filename[8], music_ext[1], 3) == 0
	        || memcmp(&location->filename[8], music_ext[2], 3) == 0
	        || memcmp(&location->filename[8], music_ext[3], 3) == 0)       //��������������ļ�ʱ�Ĵ���
	{
		ID3.TALB_length = 0;
		if (wmaGetTAGInfo(location->filename, &ID3))                //��ȡTAG��Ϣ
		{
			unitochar(location->ID3buffer, 30, string_id);            //TAG��Ϣ��UNICODE���ͣ�����Ҫת��ΪCHAR
			unitochar(location->ID3buffer + 30, 30, string_id);
			
			if ( (location->ID3buffer[0] != 0) || (location->ID3buffer[30] != 0) )
			{
				strcat(location->ID3buffer, " ");
				strcat(location->ID3buffer, location->ID3buffer + 30);
				File_Flag = FALSE;
				GUI_DisplayText(0, 16, location->ID3buffer);
			}
		}
	}
	else if(memcmp(&location->filename[8], voice_ext[0], 3) == 0
	        || memcmp(&location->filename[8], voice_ext[1], 3) == 0)        //�����¼���ļ�ʱ�Ĵ���
	{
		File_Flag = FALSE;
		GUI_DisplayText(0, 16, location->file_name);
	}
	else
	{
		APP_ASSERT(FALSE);    //���ܳ����������
	}
#endif
	if(File_Flag)                                                    //��ȡ���ļ���
	{
		//	    unitochar(location->filename,NameNum*2+2,string_id);
		GUI_DisplayText(0, 16, location->file_name);
	}
	GUI_ResShowPic(GUI_IMG_DELL, 0, 0);
	GUI_ResShowMultiString(GUI_STR_CDELFILE, string_id, 16, 0);
	GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
	
	GUI_UpdateScreen(NULL);
	ui_auto_update = TRUE;
#endif
}

/********************************************************************************
* Description : draw the selector directory.
*
* Arguments  : location  :keep the file selector parament
               string_id :display language type
* Returns     :
*
* Notes       :
*
********************************************************************************/
void draw_del_dir(file_location_t *location, UINT8 string_id)
{
#ifdef APP_SUPPORT_PLAYLIST
	INT8 buf[5];
	UINT8 NameNum, Cardflag = 0;
	
	GUI_ClearScreen(NULL);
	ui_auto_update = FALSE;
	
	GUI_ResShowPic(GUI_IMG_DELALL, 0, 0);
	GUI_ResShowMultiString(GUI_STR_CDELALL, string_id, 16, 0);
	GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
	
	if(location->disk == FS_DEV_TYPE_USBDISK)
	{
		Cardflag = DIRROOT - DIRCARD;
	}
	else if(location->disk != FS_DEV_TYPE_TFLASH)
	{
		APP_ASSERT(FALSE);
	}
	
	/*Longnamebuf_del[0]=0x3a;
	FS_ChangeDir(Longnamebuf_del);*/    //CD����Ŀ¼
	//    if(location->path[0] == ':')                            //��Ŀ¼��ʾ
	{
		GUI_ResShowPic(DIRROOT - Cardflag, 0, 16);
		strcpy(buf, "ROOT");
		GUI_DisplayText(14, 16, buf);
	}
	//    else                                                //��Ŀ¼��ʾ�����ȡ��Ŀ¼�ĳ���
	{
		GUI_ResShowPic(DIRROOTA - Cardflag, 0, 16);
		FS_ChangeDir(FS_CD_PARENT);
		
		//        unitochar(location->path,NameNum*2+2,string_id);
		//        GUI_DisplayText(0, 16, location->path);
		//        FS_SetCurDirEntry(location->ClusterNo);
	}
	//    FS_ChangeDir(location->path);                            //CD��ɾ������Ŀ¼
	GUI_UpdateScreen(NULL);
	ui_auto_update = TRUE;
#endif
}


/********************************************************************************
* Description : delete  the  files of the selector directory.
*
* Arguments  : location  :keep the file selector parament
               string_id :display language type
               type: the kind of the files
* Returns     :
*            other:    Successful
*            0:    Fail, detail information in fselError
* Notes       :
*
********************************************************************************/
INT32 GUI_Delete(file_location_t *location, UINT8 type, UINT8 string_id)
{
#ifdef APP_SUPPORT_PLAYLIST
	BOOL  need_draw = TRUE;
	UINT8  delflag = 0;
	INT8  strName[28];
	UINT8  i, playlistclassbak;
	UINT32  key;
	
	if ( !fselSetLocation(location) )
	{
		return RESULT_IGNORE;
	}
	
	
	while(1)
	{
		if(need_draw)
		{
			if(type == DELETE_FILE)
			{
				draw_del_file(location, string_id);            //ɾ�������ļ�����ʾ
				while( (key = MESSAGE_Get()) != NULL );                        //��ȡ��Ϣ
			}
			else if(type == DELETE_MUSIC || type == DELETE_VOICE)
			{
				draw_del_dir(location, string_id);            //ɾ��һ��Ŀ¼���������ֻ�¼���ļ�����ʾ
				while( (key = MESSAGE_Get()) != NULL );                        //��ȡ��Ϣ
			}
			else
			{
				return RESULT_IGNORE;                    //������������Բ��˳�����
			}
			need_draw = FALSE;
		}
		
		key = MESSAGE_Get();                          //��ȡ��Ϣ
		switch(key)
		{
		case AP_KEY_NEXT:                            //YES��NO֮���л�
		case AP_KEY_PREV:
		case AP_KEY_NEXT | AP_KEY_HOLD:
		case AP_KEY_PREV | AP_KEY_HOLD:
			if(delflag == 0)
			{
				delflag = 1;
				GUI_ResShowPic(GUI_IMG_CDEL_YES, 100, 0);
			}
			else
			{
				delflag = 0;
				GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
			}
			break;
			
		case AP_KEY_MODE | AP_KEY_UP:
			if(delflag == 1)                      //YESʱ����MODE���Ĵ���
			{
				GUI_ClearScreen(&region);                            //�岿����Ļ
				GUI_ResShowMultiString(GUI_STR_CDELING, string_id, 16, 0);     //��ʾ��ɾ����...��
				GUI_UpdateScreen(NULL);
				if(type == DELETE_FILE)
				{
					if ( !fselDelFile(FSEL_DEL_FILE) )         //�����ļ�ѡ����
					{
						return RESULT_IGNORE;
					}
					else
					{
						if (fselGetLocation(location))
						{
							return RESULT_NULL;
						}
						else
						{
							return RESULT_IGNORE;
						}
					}
				}
				else if(type == DELETE_MUSIC)
				{
					if( !del_audio_files(ExtMusic) )
					{
						GUI_ResShowMultiString(GUI_STR_CDELFAIL, string_id, 16, 0);     //ɾ��ʧ�ܽ�����ʾ
						GUI_UpdateScreen(NULL);
						MESSAGE_Sleep(4);
						return RESULT_IGNORE;
					}
					fselFileNo = 0;
					fselGetLocationC(location);
					return RESULT_NULL;
				}
				else if(type == DELETE_VOICE)
				{
					if( !del_audio_files(ExtVoice) )
					{
						GUI_ResShowMultiString(GUI_STR_CDELFAIL, string_id, 16, 0);     //ɾ��ʧ�ܽ�����ʾ
						GUI_UpdateScreen(NULL);
						MESSAGE_Sleep(4);
						return RESULT_IGNORE;
					}
					fselFileNo = 0;
					fselGetLocationC(location);
					return RESULT_NULL;
				}
				else
				{
					APP_ASSERT(FALSE);
				}
				return RESULT_IGNORE;
			}
			else
			{
				return RESULT_IGNORE;//NOʱ����MODE���Ĵ���
			}
			
		case AP_MSG_WAIT_TIMEOUT:                        //8���˳�
			if(ui_auto_select)
			{
				return RESULT_IGNORE;
			}
			break;
			
		case AP_KEY_PLAY | AP_KEY_UP:
			return RESULT_IGNORE;
			
		case AP_MSG_SD_OUT:
			//����׼��ɾ��SD�����ļ�ʱ��SD�����γ����ͷ��ذγ��ź�
			if (location->disk == FS_DEV_TYPE_TFLASH)
			{
				return RESULT_SD_OUT;
			}
			break;
			
		case AP_MSG_SD_IN:
			break;
			
		default:
			key = MESSAGE_HandleHotkey(key);                  //�ȼ�����
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
#else
	return RESULT_IGNORE;
#endif
}


/********************************************************************************
* Description : delete  the  files of the playlist.
*
* Arguments  : location  :keep the file selector parament
               string_id :display language type
               type: the kind of the files
* Returns     :
*            other:    Successful
*            0:    Fail, detail information in fselError
* Notes       :
*
********************************************************************************/
INT32 GUI_DeletePlayList(file_location_t *location, UINT8 type, UINT8 string_id)
{
#ifdef APP_SUPPORT_PLAYLIST
	BOOL  need_draw = TRUE;
	UINT8  delflag = 0;
	WCHAR strName[MAX_FILE_LEN];
	UINT8  i;
	INT8  key = type;
	
	//    tcscpy(strName, location->path );
	
	if (!fselPlayListEnt(strName))
	{
		return RESULT_IGNORE;    //open the play list
	}
	
	//    PlaylistCurClass = location->dirLayer; //get the class parament
	
	if ( !getClassParam(strName, 28) )
	{
		return RESULT_IGNORE;
	}
	
	while(1)
	{
		if(need_draw)
		{
			if ( type == DELETE_FILE)
			{
				//                fileoffset = (UINT16)(location->ClusterNo & 0xFFFF);  //get the fileoffset
				
				if ( !playlistnext(strName) )
				{
					return RESULT_IGNORE;
				}
				
				draw_del_filepl(location, strName, string_id);            //ɾ�������ļ�����ʾ
			}
			else if(type == DELETE_MUSIC || type == DELETE_VOICE)
			{
				draw_del_dirpl(location, string_id);            //ɾ��һ��Ŀ¼���������ֻ�¼���ļ�����ʾ
			}
			else
			{
				return RESULT_IGNORE;                    //������������Բ��˳�����
			}
			need_draw = FALSE;
		}
		
		key = MESSAGE_Wait();                          //��ȡ��Ϣ
		switch(key)
		{
		case AP_KEY_NEXT:                            //YES��NO֮���л�
		case AP_KEY_PREV:
		case AP_KEY_NEXT | AP_KEY_HOLD:
		case AP_KEY_PREV | AP_KEY_HOLD:
			if(delflag == 0)
			{
				delflag = 1;
				GUI_ResShowPic(CDEL_YES, 100, 0);
			}
			else
			{
				delflag = 0;
				GUI_ResShowPic(GUI_IMG_CDEL_NO, 100, 0);
			}
			break;
			
		case AP_KEY_MODE | AP_KEY_UP:
			if(delflag == 1)                      //YESʱ����MODE���Ĵ���
			{
				GUI_ClearScreen(&region);                            //�岿����Ļ
				GUI_ResShowMultiString(GUI_STR_CDELING, string_id, 16, 0);     //��ʾ��ɾ����...��
				GUI_UpdateScreen(NULL);
				if(type == DELETE_FILE)
				{
					if ( !fselDelFile(FSEL_DEL_FILE) )         //�����ļ�ѡ����
					{
						return RESULT_IGNORE;
					}
					else
					{
						if (fselGetLocation(location))
						{
							return RESULT_NULL;
						}
						else
						{
							return RESULT_IGNORE;
						}
					}
				}
				else if(type == DELETE_MUSIC)
				{
					goto DELPLAYLISTDIR;
				}
				else if(type == DELETE_VOICE)
				{
DELPLAYLISTDIR:
					while (1)
					{
						//                        PlaylistCurClass = location->dirLayer; //get the class parament
						if ( !getClassParam(strName, 28) )
						{
							return RESULT_IGNORE;
						}
						if ( !playlistnext(strName) ) //������
						{
							if (fselGetLocation(location))
							{
								return RESULT_NULL;
							}
							else
							{
								return RESULT_IGNORE;
							}
						}
						if ( !fselDelFile(FSEL_DEL_FILE) )
						{
							return RESULT_IGNORE;    //ɾ�ļ����޸�playlist
						}
					}
					return RESULT_IGNORE;
				}
				else
				{
					APP_ASSERT(FALSE);
				}
				return RESULT_IGNORE;
			}
			else
			{
				return RESULT_IGNORE;//NOʱ����MODE���Ĵ���
			}
			
		case AP_MSG_WAIT_TIMEOUT:                        //8���˳�
			if(ui_auto_select)
			{
				return RESULT_IGNORE;
			}
			break;
			
		case AP_KEY_PLAY | AP_KEY_UP:
			return RESULT_IGNORE;
			
		case AP_MSG_SD_OUT:
			//����׼��ɾ��SD�����ļ�ʱ��SD�����γ����ͷ��ذγ��ź�
			if (location->disk == FS_DEV_TYPE_TFLASH)
			{
				return RESULT_SD_OUT;
			}
			break;
		case AP_MSG_SD_IN:
			break;
			
		default:
			key = MESSAGE_HandleHotkey(key);                  //�ȼ�����
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
#else
	return 0;
#endif
}





