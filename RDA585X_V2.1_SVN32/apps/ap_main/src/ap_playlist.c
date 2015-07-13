/********************************************************************************
*                File selector
*            enhanced library
*
*        (c) Copyright, Rda Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_common.h"


#ifdef APP_SUPPORT_PLAYLIST
HANDLE   fpPlaylist;
LISTITEM * plistitem;
WCHAR    PlayListFileName[12];
WCHAR    ItemFileName[12];

UINT16    PlaylistClassTotal;
UINT16    PlaylistItemTotal;
UINT16    PlaylistCurClass;
UINT16    PlaylistCurItem;

UINT32    ClassStrOffset;
UINT16    ClassStrLen;
UINT16    ClassItemTotal;
UINT32    Class1stItemOffset;
UINT8    PlaylistTotal;;//�õ��ܲ����б���?
INT8    listtype[2];
UINT16    fileoffset;

//global variable should be resident
extern UINT8 fselError;  // store error id;


INT8    fSelBuf[512];    //read file buffer



static void DmaCpy1(UINT16 descadd, UINT8 descpg, UINT16 srcadd, UINT8 srcpg, UINT16 Length);


/********************************************************************************
* Description : enter the play list mode system
*
* Arguments  :
*             filename : the playlist file name
              retInfo: classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselPlayListEnt(WCHAR *filename)    //filename: ARTIST.A3L, ALBUM.A3L, ....
{
	WCHAR *lName[] = {L"PLAYLIST"};
	
	memcpy(PlayListFileName, filename, 12);  //keep the play list file name
	
	FS_ChangeDir(FS_CD_ROOT);    //return the root
	
	if ( ERR_SUCCESS != FS_ChangeDir(lName)) //enter the playlist
	{
		fselError = FSEL_ERR_FS;
		return FALSE;
	}
	
	if ( fpPlaylist )
	{
		FS_Close(fpPlaylist);
	}
	
	fpPlaylist = FS_Open(filename, FS_O_RDONLY, 0); //open the play list filename
	
	if ( !fpPlaylist )
	{
		fselError = FSEL_ERR_FS;
		return FALSE;
	}
	
	if ( (listtype[0] != filename[0]) || (listtype[1] != filename[1]) ) //�Ƚ�ǰ�����ַ����㹻�ֱ治ͬ��list�ļ�
	{
		PlaylistCurClass = 0;               //��ʼ��
		PlaylistCurItem  = 0;                //��ʼ��
		listtype[0] = filename[0];
		listtype[1] = filename[1];
	}
	
	fileoffset = 0;
	
	if ( ERR_SUCCESS == FS_Seek(fpPlaylist, fileoffset, FS_SEEK_SET)) //get the class parament
	{
	
		if ( ERR_SUCCESS == FS_Read(fpPlaylist, fSelBuf, 512) )
		{
			PlaylistClassTotal = * ((UINT16 *)&fSelBuf[0x20]);        //ȡ��ClassTotal
			
			if ( PlaylistClassTotal == 0 )
			{
				fselError = FSEL_ERR_PLAYLIST;
				FS_ChangeDir(FS_CD_ROOT);
				return FALSE;
			}
			else
			{
				PlaylistItemTotal = * ((UINT16 *)&fSelBuf[0x22]);     //ȡ��ItemTotal
				FS_ChangeDir(FS_CD_ROOT);
				return TRUE;
			}
		}
		else    //Read err
		{
			FS_ChangeDir(FS_CD_ROOT);
			return FALSE;
		}
	}
	else        //Seek err
	{
		FS_ChangeDir(FS_CD_ROOT);
		return FALSE;
	}
}

/********************************************************************************
* Description : get the class parament
*
* Arguments  :
*             classname : output the next classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by playlistclassPrev and playlistclassNext
*
********************************************************************************/
BOOL getClassParam(WCHAR *classname, UINT16 strlen)
{
	WCHAR *str;
	CATMEMB * pcatmemb;
	
	if ( (PlaylistCurClass == 0) || (PlaylistCurClass > PlaylistClassTotal) )
	{
		PlaylistCurClass = 1;
	}
	
	fileoffset = (PlaylistCurClass - 1) / 32 + 1;     //��ǰclass��¼��list�ļ��ڼ�������
	pcatmemb = (CATMEMB * )(fSelBuf + ((PlaylistCurClass - 1) % 32) * 16); //��ǰclass��¼��������ַ
	
	if (ERR_SUCCESS == FS_Seek(fpPlaylist, fileoffset, FS_SEEK_SET))   //seek
	{
	
		if( ERR_SUCCESS == FS_Read(fpPlaylist, fSelBuf, 512) )  //read
		{
			ClassStrOffset = pcatmemb->stroffset;
			ClassStrLen = pcatmemb->strlength;
			ClassItemTotal = pcatmemb->MembItemNum;
			Class1stItemOffset = pcatmemb->FistItemOffset;
			
			if ( classname == NULL )    //ֻȡ��Ϣ����ȡclass����
			{
				return TRUE;
			}
			else
			{
				fileoffset = ClassStrOffset / 0x200;         //��ǰclass��¼��list�ļ��ڼ�������
				FS_Seek(fpPlaylist, fileoffset, FS_SEEK_SET);
				FS_Read(fpPlaylist, fSelBuf, 512);
				
				DmaCpy1( FILEDATABUF, 0x06, (UINT16)&fSelBuf, 0x08, 0x200 - 1 );
				
				str = (INT8*)FILEDATABUF + ClassStrOffset % 0x200;
				if ( (str + ClassStrLen) > ((INT8*)FILEDATABUF + 0x200) ) //strԽ����
				{
					FS_Read(fpPlaylist, fSelBuf, 512);
					DmaCpy1( FILEDATABUF + 0x200, 0x06, (UINT16)&fSelBuf, 0x08, 0x200 - 1 );
				}
				
				if ( (strlen == -1) || (strlen > ClassStrLen) )
				{
					INT8 * p;
					
					memcpy(classname, str, ClassStrLen);        //class name <--------
					classname[ClassStrLen - 2] = 0x20;      //�ӿո����ڹ���
					classname[ClassStrLen - 1] = 0x0;        //�ӿո����ڹ���
					p = (INT8 *)&classname[ClassStrLen - 1];
					p++;
					* p = 0x0;
					p++;
					* p = 0x0;      //��һ��unicode������
				}
				else if ( strlen <= ClassStrLen )
				{
					memcpy(classname, str, strlen);
					classname[strlen - 4] = 0x20;      //�ӿո����ڹ���
					classname[strlen - 3] = 0x0;        //�ӿո����ڹ���
					classname[strlen - 2] = 0x0;        //�ӿո����ڹ���
					classname[strlen - 1] = 0x0;        //�ӿո����ڹ���
				}
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}




static void DmaCpy1(UINT16 descadd, UINT8 descpg, UINT16 srcadd, UINT8 srcpg, UINT16 Length)
{

}

/********************************************************************************
* Description : read the sector of current item in list file
*
* Arguments  : sector buffer

* Returns    :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
static BOOL ReadCurItemSector(INT8 * Buffer)    //ͬһbank,�����static,ʹ�䲻��bank call
{
	UINT16 itemfileoffset;
	long itemoffset;
	
	itemoffset = Class1stItemOffset + (PlaylistCurItem - 1) * sizeof(LISTITEM);
	itemfileoffset = itemoffset / 0x200;        //��ǰitem��¼��list�ļ��ڼ�������
	
	plistitem = (LISTITEM *)(Buffer + itemoffset % 0x200);
	
	fileoffset = itemfileoffset;
	
	if ( ERR_SUCCESS == FS_Seek(fpPlaylist, fileoffset, FS_SEEK_SET) ) //seek
	{
		if( ERR_SUCCESS == FS_Read(fpPlaylist, Buffer, 512)) //read
		{
			return TRUE;
		}
		else
		{
			fselError = FSEL_ERR_FS;
			return FALSE;
		}
	}
	else
	{
		fselError = FSEL_ERR_FS;
		return FALSE;
	}
	
}

/********************************************************************************
* Description : get the play list filename
*
* Arguments  :
*             filename : the selector filename

* Returns     :*
*               0: Fail, list�ļ�����
                1: Successful,
                2��//����������
* Notes       :
*
********************************************************************************/
UINT8 getplayItemparam(WCHAR *filename)
{
	if ( ReadCurItemSector(fSelBuf) )   //Ĭ�����룺PlaylistCurItem
	{
		if ( plistitem->FirstCluster == 0xffffffff || plistitem->FileEntry  == 0xffffffff )
		{
			return 2;   //����������
		}
		
		if ( FS_SetCurDirEntry(plistitem->FirstCluster) )        //��λĿ¼
		{
			if ( FS_SetCurDirEntry(plistitem->FileEntry) )  //��λ�ļ�
			{
				if ( ERR_SUCCESS == FS_GetEntryName(plistitem->FileEntry, MAX_FILE_LEN, filename) )
				{
					//���ֱ�Ӵӵ�ǰĿ¼����Ҹ�Ŀ¼�����������NameBuf�ĵ�һ���ֽڳ�ʼ��Ϊ20h����,CharNumΪ0���ʾ�ӵ�ǰĿ¼��ֱ�ӻ�ȡ�ļ�8+3���͵Ķ���
					memcpy(ItemFileName, filename, MAX_FILE_LEN);  //�����ļ���
					return 1;
				}
				else
				{
					return 2;   //����������
				}
			}
			else
			{
				return 2;   //����������
			}
		}
		else
		{
			return 2;       //����������
		}
	}
	else
	{
		return 0;   //list�ļ�����
	}
}

/********************************************************************************
* Description : get the next playlist class
*
* Arguments  :
*             classname : output the next classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by fselGetNextGroup.
*
********************************************************************************/
BOOL playlistclassNext(WCHAR *classname)
{
	if ( PlaylistClassTotal == 0 )    //total class = 0
	{
		return FALSE;
	}
	
	PlaylistCurItem = 0;    //��ʼ��
	
	if ( PlaylistCurClass < PlaylistClassTotal )  //0 ~ PlaylistClassTotal
	{
		PlaylistCurClass++;  //next no
	}
	else
	{
		return FALSE;       //�ѵ����һ��class
	}
	
	return ( getClassParam(classname, LISTNAME_DISP_LENNGTH) ); //Ĭ�����룺PlaylistCurClass
}

/********************************************************************************
* Description : get the Prev playlist class
*
* Arguments  :
*             classname : output the next classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by fselGetPrevGroup.
*
********************************************************************************/
BOOL playlistclassPrev(WCHAR *classname)
{
	if ( PlaylistClassTotal == 0 )    //classtotal = 0
	{
		return FALSE;
	}
	
	PlaylistCurItem = 0;    //��ʼ��
	
	if ( PlaylistCurClass > 1 )             // if the prev class is the first, get the first again
	{
		PlaylistCurClass--;
	}
	else
	{
		return FALSE;           //�ѵ���ǰһ��class
	}
	
	return ( getClassParam(classname, LISTNAME_DISP_LENNGTH) );   //Ĭ�����룺PlaylistCurClass
}

/********************************************************************************
* Description : get the next song in curretn class
*
* Arguments  :
*             filename : the selector filename

* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL playlistnext(WCHAR *filename)
{
	UINT8 iRet;
	
RETRYNEXT:
	if ( PlaylistCurItem < ClassItemTotal )     //1 ~ ClassItemTotal
	{
		PlaylistCurItem++;
	}
	
	iRet = getplayItemparam(filename);  //Ĭ�����룺PlaylistCurItem
	
	if ( iRet == 2 )                    //play list index has removed
	{
		if ( PlaylistCurItem < ClassItemTotal )
		{
			goto RETRYNEXT;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return iRet;
	}
}


/********************************************************************************
* Description : get the prev song in curretn class
*
* Arguments  :
*             filename : the selector filename

* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL playlistprev(WCHAR *filename)
{
	UINT8 iRet;
	
RETRYPREV:
	if ( PlaylistCurItem > 1 )     //1 ~ ClassItemTotal
	{
		PlaylistCurItem--;
	}
	
	iRet = getplayItemparam(filename);  //Ĭ�����룺PlaylistCurItem
	
	if ( iRet == 2 )                    //play list index has removed
	{
		if ( PlaylistCurItem > 1 )
		{
			goto RETRYPREV;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return iRet;
	}
}


/********************************************************************************
* Description : del the current file in play list
*
* Arguments  :
*             filename : return the current selector file name
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by fselPlayDel
*
********************************************************************************/
BOOL playlistdel(WCHAR *filename)
{
	if ( getplayItemparam( filename ) == 1 )     //ȡ��Ҫɾ���ļ����ļ���
	{
		if ( ReadCurItemSector(fSelBuf) )           //Ĭ�����룺PlaylistCurItem
		{
			plistitem->FirstCluster = 0xffffffff;       //list�ļ���item����Ϊ��Ч
			plistitem->FileEntry    = 0xffffffff;
			
			if ( ERR_SUCCESS == FS_Write(fpPlaylist, fSelBuf, sizeof(fSelBuf))) //!!!!!!!!!!!!!!!!!sdk90�ļ�ϵͳĿǰ��֧���޸��ļ�!!!!!!!!!!!!!!!!!!!!!!!!
			{
				return TRUE;
			}
			else
			{
				fselError = FSEL_ERR_FS;
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}


/********************************************************************************
* Description : close the palylist file and return to common file system
*
* Arguments  :
*
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselPlayListExit(void)
{
	fselError = 0;
	if ( FS_Close(fpPlaylist) < 0 )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

#endif



