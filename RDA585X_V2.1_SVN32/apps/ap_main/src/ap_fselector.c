/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_common.h"

//global variable file select
UINT8    fselPLOrCommonDir = 0; //indication the current playmode
UINT8    fselType = 0;       //Type of select,  Music or voice.
UINT8    fselMode = 0;       //Mode of select, Sequence, repeat one ...
UINT8    fselError = 0;      //error flags;
INT8      fselDiskLetter = 0; //current Disk C,D,F,U,etc;
UINT16    fselFileNo = 0;     //Number of current file,=0 mean NULL, 1 mean first file
UINT16    fselTotal = 0;      //Total of current select file

#ifdef APP_SUPPORT_PLAYLIST
//extern playlist global var
extern  UINT16 fileoffset;
extern  UINT16 PlaylistCurClass;
extern  UINT8 PlaylistTotal;
extern  UINT8 ItemFileName[];
extern  UINT8 PlayListFileName[];
extern  UINT16 PlaylistCurItem;
extern  UINT16 ClassItemTotal;
extern  UINT16 PlaylistClassTotal;
extern  HANDLE  fpPlaylist;

WCHAR   playlistclassname[28];
WCHAR   m_szTemp[MAX_FILE_LEN];

UINT8    PlaylistStart = 0;  //播放列表起始位置
UINT8    fselDirNo[TOTALDIRLAYER] = {0};    //Number of current valid directory =0 mean NULL, 1 mean first valid directory
UINT8    fselDirLayer = 0;   //当前目录层次.与fselDirNo[]共同记录当前目录在目录树中的位置.
#endif


const TCHAR ExtVoice[] = "WAV";
#if defined(TARGET_WMA_SUPPORT_OFF) &&defined(TARGET_AAC_SUPPORT_OFF)

const TCHAR ExtMusic[] = "MP3.WAV";//"MP3.ASF.WMV.OGG";

#elif !defined(TARGET_WMA_SUPPORT_OFF)&&!defined(TARGET_AAC_SUPPORT_OFF)

const TCHAR ExtMusic[] = "MP3.WAV.WMA.AAC";    //"MP3.ASF.WMV";
#elif !defined(TARGET_WMA_SUPPORT_OFF)&&defined(TARGET_AAC_SUPPORT_OFF)

const TCHAR ExtMusic[] = "MP3.WAV.WMA";

#elif !defined(TARGET_AAC_SUPPORT_OFF)&&defined(TARGET_WMA_SUPPORT_OFF)

const TCHAR ExtMusic[] = "MP3.WAV.AAC";
#endif


BOOL  SelectPrev(UINT32* pEntry);


/********************************************************************************
* Description : seek to  specify Directory Entry
*
* Arguments  :
*        strDir: Dos 8.3 Directory name
*            num: number of the Directory Entry to seek
*
* Returns     :
*            TRUE: Successful
*            FALSE: Fail,  detail information in fselError
* Notes       :
*
********************************************************************************/
BOOL ToDirEntry(TCHAR *strDir, const UINT16 Num)
{
	UINT32 inode;
	UINT16   temp;
	
	if((Num > FSEL_MAX_DIRNUM) || ( Num < 1))
	{
		fselError = FSEL_ERR_OUTOF;
		return FALSE;
	}
	/*Get First Dir Entry*/
	if (ERR_SUCCESS != FS_GetNextEntry(&inode, NULL, FS_DIR_DIR))
	{
		fselError = FSEL_ERR_OUTOF;
		return FALSE;
	}
	
	FS_GetEntryName(inode, MAX_FILE_LEN, strDir); // first sub dir name
	
	if (Num == 1)
	{
		return TRUE;
	}
	
	///seek to the specify directory entry
	temp = 2;
	/*If The Given Dir Just Not One Dirs ,Then Find Next Dir,FindOut Special Dir Name*/
	while ( temp <= Num)
	{
		if (ERR_SUCCESS != FS_GetNextEntry(&inode,  NULL, FS_DIR_DIR_CONT))
		{
			fselError = FSEL_ERR_OUTOF;
			return FALSE;
		}
		else
		{
			FS_GetEntryName(inode, MAX_FILE_LEN, strDir);
		}
		temp++;
	}
	return TRUE;
}


/********************************************************************************
* Description : Get name buffer of current extend
*
* Arguments  :  None
* Returns     :  the buffer of current file extend name
* Notes       :     None
*
********************************************************************************/
TCHAR * GetExtBuf(void)
{
	if ( fselType == FSEL_TYPE_MUSIC)
	{
		return &ExtMusic[0];
	}
	else
	{
		return &ExtVoice[0];
	}
}

/********************************************************************************
* Description : Get the total of files in the current directory entry
* Arguments  :
*            strDir: Dos 8.3 directory name
*            bCheckValidDir: if check valid directory only
* Returns     :
*            The file total of current directory when bCheckValidDir = FALSE
            or Vaild BOOL flag when bCheckValidDir = TRUE
* Notes       : None
*
********************************************************************************/
UINT32  GetFileTotalCur(void)
{
	UINT32  total = 0;
	INT32  lEntry;
	UINT32 inode;
	
	FS_GetCurDirEntry(&lEntry);
	
	if (ERR_SUCCESS == FS_GetNextEntry(&inode, GetExtBuf(), FS_DIR_FILE))
	{
		total ++;
		
		while ( (ERR_SUCCESS == FS_GetNextEntry(&inode, GetExtBuf(),  FS_DIR_FILE_CONT)) && (total < FSEL_MAX_FILENUM))
		{
			total ++;
		}
	}
	
	if(total > FSEL_MAX_FILENUM)
	{
		total = FSEL_MAX_FILENUM;
	}
	
	FS_SetCurDirEntry(lEntry);
	return total;
}

/********************************************************************************
* Description : in current dir get the last dir
*
* Arguments  :
            strDir:output Dir name
* Returns    :
            TRUE: Successful
            FALSE: fail.
* Notes       :  None
*
********************************************************************************/
BOOL fsellastdir(WCHAR *strDir)
{
#ifdef APP_SUPPORT_PLAYLIST
	UINT32 inode;
	
	if( fselDirLayer > (TOTALDIRLAYER - 1) )
	{
		return FALSE;
	}
	
	fselDirNo[fselDirLayer] = 0;
	
	if ( ERR_SUCCESS != FS_GetNextEntry(&inode, NULL, FS_DIR_DIR) )
	{
		return FALSE;    // if no dir ,return error
	}
	
	fselDirNo[fselDirLayer] = 1;
	
	FS_GetEntryName(inode, MAX_FILE_LEN, strDir); // sub dir name
	
	while ( ERR_SUCCESS == FS_GetNextEntry(&inode, NULL, FS_DIR_DIR_CONT))
	{
		fselDirNo[fselDirLayer]++;
		FS_GetEntryName(inode, MAX_FILE_LEN, strDir); // sub dir name
	}
#endif
	return TRUE;
}


/********************************************************************************
* Description : Get the total of files in the current directory entry
*
* Arguments  :
*            strDir: Dos 8.3 directory name
*            bCheckValidDir: if check valid directory only
* Returns     :
*            The file total of current directory when bCheckValidDir = FALSE
            or Vaild BOOL flag when bCheckValidDir = TRUE
* Notes       : None
*
********************************************************************************/
UINT32  GetFileTotal(WCHAR *strDir)
{
	UINT32 total = 0;
#ifdef APP_SUPPORT_PLAYLIST
	UINT32 inode;
	if( ERR_SUCCESS != FS_ChangeDir(strDir))
	{
		fselError = FSEL_ERR_FS;
		return 0;
	}
	
	if (ERR_SUCCESS == FS_GetNextEntry(&inode,  GetExtBuf(), FS_DIR_FILE))
	{
		total ++;
		while ( ERR_SUCCESS == FS_GetNextEntry(&inode, GetExtBuf(), FS_DIR_FILE_CONT) && (total < FSEL_MAX_FILENUM))
		{
			total ++;
		}
	}
	
	if ( ERR_SUCCESS != FS_ChangeDir(FS_CD_PARENT) )
	{
		fselError = FSEL_ERR_FS;
		return total;
	}
#endif
	return total;
}



/********************************************************************************
* Description : Get a random number
*
* Arguments  : None
* Returns     :
            Random number
* Notes       :  None
*
********************************************************************************/
static UINT16 Random(void)
{
	static UINT16    fselRandCount = 0x1234;
	fselRandCount = (fselRandCount + 1103515245) & 0x3fff;
	return ( (fselRandCount / 65536) % (32767 + 1) );
}


/********************************************************************************
* Description : select the next file and get the file name
*
* Arguments  :
* Returns     :
* Notes       :
*
********************************************************************************/
BOOL SelectNext(UINT32 *pEntry)
{
	UINT8   temp;
	UINT32  count;
	
	if (fselFileNo == fselTotal && fselTotal != 0)
	{
		fselError = FSEL_ERR_OUTOF;
		return FALSE;
	}
	
	//search the file by current file ext name.
	if (fselFileNo == 0)
	{
		temp = FS_DIR_FILE;
	}
	else
	{
		temp = FS_DIR_FILE_CONT;
	}
	
	if( ERR_SUCCESS == FS_GetNextEntry(pEntry,  GetExtBuf(), temp))
	{
		fselFileNo ++;
		if(fselTotal == 0)
		{
			FS_GetEntryCount(&count, GetExtBuf(), FS_DIR_FILE);
			fselTotal = count;
		}
		return TRUE;
	}
	
	fselError = FSEL_ERR_FAIL;
	return FALSE;
}



/********************************************************************************
* Description : Seek to the next file
*
* Arguments  :
        strFile:output file name
* Returns     :
            TRUE: Successful
            FALSE: fail.
* Notes       :  目录树查找,先检查父目录再检查子目录(子目录1 -- 子目录99)
*
********************************************************************************/
BOOL SelectNextDisk(UINT32 *pEntry)
{
	UINT32 inode;
	//在当前目录向下找文件
	fselError = 0;
	
	if ( !SelectNext(pEntry) )
	{
		//寻找当前目录下的所有目录
CurrentDiragain:
		hal_TimWatchDogKeepAlive();
		if ( ERR_SUCCESS == FS_GetNextEntry(&inode,  NULL, FS_DIR_DIR) )
		{
CurrentDiragainNext:
			hal_TimWatchDogKeepAlive();
			if ( ERR_SUCCESS == FS_ChangeSubDir(inode) )
			{
				fselFileNo = 0;
				fselTotal  = 0;
				// 检查当前目录是否有合适的文件.
				if ( SelectNext(pEntry) )
				{
					fselError = 0;
					return TRUE;
				}
				else //子目录下的子目录循环查找
				{
					hal_TimWatchDogKeepAlive();
					goto CurrentDiragain;
				}
			}
			else    //出错了
			{
				fselInit(fselType, fselMode, 0, fselDiskLetter);
				fselError = FSEL_ERR_OUTOF;
				return FALSE;
			}
		}
		else    //寻找当前目录的下一个目录（平级寻找）
		{
ParentDiragainNext:
			hal_TimWatchDogKeepAlive();
			if ( ERR_SUCCESS == FS_ChangeSubDir(FS_PARENT_INO))
			{
				//寻找当前目录的下一个目录（平级寻找）
				if (ERR_SUCCESS == FS_GetNextEntry(&inode,  NULL, FS_DIR_DIR_CONT))
				{
					// 检查目录下是否有合适的文件.
					goto CurrentDiragainNext;
				}
				else    //到上一级目录
				{
					goto ParentDiragainNext;
				}
			}
			else
			{
				fselError = FSEL_ERR_OUTOF;
				return FALSE;
			}
		}
	}
	return TRUE;
}


/********************************************************************************
* Description : Seek to the prev file
*
* Arguments  :
        strFile:output file name
* Returns     :
            TRUE: Successful
            FALSE: fail.
* Notes       :  目录数检查,先子目录(子目录N -- 子目录1)再父目录.
*
********************************************************************************/
BOOL SelectPrevtDisk(UINT32 *pEntry)
{
	UINT32 inode;
	
	//在当前目录向下找文件
	fselError = 0;
	
	if ( !SelectPrev(pEntry) )
	{
		//检查父目录.
ParentDiragainNext:
		hal_TimWatchDogKeepAlive();
		if ( ERR_SUCCESS == FS_ChangeSubDir(FS_PARENT_INO))
		{
			if (ERR_SUCCESS == FS_GetPrevEntry(&inode,  NULL, FS_DIR_DIR_CONT))
			{
				goto CurrentDiragainNext;
			}
			else
			{
				//当前目录下有效的文件
				fselTotal = 0;
				fselFileNo = 0;
				if ( SelectPrev(pEntry) )
				{
					fselError = 0;
					return TRUE;
				}
				//  当前目录分支没有合适的文件,退回上级目录
				goto ParentDiragainNext;
			}
		}
		else
		{
			fselError = FSEL_ERR_OUTOF;
			return FALSE;
		}
		//寻找当前目录下的所有目录
CurrentDiragainNext:
		hal_TimWatchDogKeepAlive();
		if ( ERR_SUCCESS != FS_ChangeSubDir(inode))
		{
			fselError = FSEL_ERR_OUTOF;
			return FALSE;
		}
		
		if (ERR_SUCCESS == FS_GetPrevEntry(&inode,  NULL, FS_DIR_DIR))
		{
			goto CurrentDiragainNext;
		}
		else    // 寻找当前目录的下一个目录（平级寻找）
		{
			fselTotal = 0;
			fselFileNo = 0;
			if ( SelectPrev(pEntry) )
			{
				fselError = 0;
				return TRUE;
			}
			//  当前目录分支没有合适的文件,退回上级目录
			goto ParentDiragainNext;
		}
	}
	
	return TRUE;
}


/********************************************************************************
* Description : Initialize the FSEL module
*
* Arguments  :
            type:   Select type,  is FSEL_TYPE_MUSIC or FSEL_TYPE_VOICE
*            mode: Select mode, is FSEL_ALL_SEQUENCE or  FSEL_ALL_REPEATONE ...
*            DirLevel: Directory deep level, 0 or 1 now.
*
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :Must call it before call other fsel Module functions
*
********************************************************************************/
BOOL fselInit(UINT8 type,  UINT8 mode, UINT8  PlayListOrCommonDir, UINT8 CardOrFlash)
{
	fselType     = type;//music or voice
	fselMode     = mode;//六种模式，顺序，随机等
	
	fselFileNo = 0;
	fselError  = 0;
	fselTotal  = 0;
	
	fselDiskLetter = CardOrFlash;
	
	FS_ChangeDir(FS_CD_ROOT);
	
	fselPLOrCommonDir = FSEL_TYPE_COMMONDIR;
	
	if  (PlayListOrCommonDir == FSEL_TYPE_COMMONDIR)
	{
		fselPLOrCommonDir = FSEL_TYPE_COMMONDIR;
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (PlayListOrCommonDir == FSEL_TYPE_PLAYLIST)
	{
		fselPLOrCommonDir = FSEL_TYPE_PLAYLIST;
	}
#endif
	else
	{
		return FALSE;
	}
	return TRUE;
}



/********************************************************************************
* Description : Set the selected location which is specified by location.
*
* Arguments  :
*            location:     store drive name, path name, file name.
* Returns     :
*            TRUE:    Successful
*            FALSE:    Fail, detail information in fselError
* Notes       :  Should select path in the case of only file name is invalid
*
********************************************************************************/
UINT8 fselSetLocationC(file_location_t  *location)
{
	TCHAR name[12];
	UINT32 entry;
	INT32 ret;
	
	fselFileNo = 0;
	fselError  = 0;
	fselTotal  = 0;
	
	fselDiskLetter = location->disk;
	
	FS_GetShortName(location->subdir_entry, name);
	
	if(strcmp(name, location->subdir_name) != 0)
	{
		fselError = FSEL_ERR_SETDIR;
		return FALSE;
	}
	FS_ChangeSubDir(location->subdir_entry);
	
	if(location->file_entry != 0)
	{
		FS_GetShortName(location->file_entry, name);
		
		//modify by wuxiang
		//扩展名大小写会导致比较出问题，在此修改成不区分大小写的比较函数
		//if(strcmp(name, location->file_name) == 0)
		//modify end
		if(strnicmp(name, location->file_name, strlen(name)) == 0)
		{
			FS_SetCurDirEntry(location->file_entry);
			hal_HstSendEvent(SYS_EVENT, 0x19191919);
		}
		else
		{
			location->file_entry = 0;
		}
	}
	
	fselFileNo = 0;
	fselTotal  = 0;
	
	ret = FS_GetNextEntry(&entry, GetExtBuf(), FS_DIR_FILE);
	
	while(ret == ERR_SUCCESS)
	{
		fselTotal ++;
		if(entry == location->file_entry)
		{
			fselFileNo = fselTotal;
		}
		ret = FS_GetNextEntry(&entry, GetExtBuf(), FS_DIR_FILE_CONT);
	}
	
	FS_SetCurDirEntry(location->file_entry);
	
	return TRUE;
}

/********************************************************************************
* Description : Get the selected location
*
* Arguments  :
*            location:    the buffer to store location
* Returns     :
*            TRUE: Successful
*            FALSE: Fail, detail information in fselError.
* Notes       :  None
*
********************************************************************************/
BOOL fselGetLocationC(file_location_t  *location)
{
#ifdef APP_SUPPORT_PLAYLIST
	UINT8     i;
	INT32    lEntry;
	location->disk = fselDiskLetter;
	
	for(i = 0; i < TOTALDIRLAYER; i++)
	{
		location->DirNo[i] = fselDirNo[i];
	}
	
	FS_GetCurDirEntry(&location->file_entry);
	
	location->dirLayer  = fselDirLayer;
	
	if (fselFileNo != 0 )
	{
		fselGetByNO(&lEntry, fselFileNo);
		if ( ERR_SUCCESS != FS_GetShortName(lEntry, location->file_name) )
		{
			location->file_name[0] = 0x00;
			location->dirLayer     = 0;
			return FALSE;
		}
	}
	else
	{
		location->file_name[0] = 0x00;
	}
	
	// 只有在fselDirNo[0] == 0时,当前目录是根目录.
	if ( fselDirLayer > (TOTALDIRLAYER - 1) || fselDirNo[0] == 0)
	{
		m_szTemp[0]  = FS_CD_ROOT;
		fselDirNo[0] = 0;
		fselDirLayer = 0;
	}
	else
	{
		FS_ChangeDir(FS_CD_PARENT);
		ToDirEntry(m_szTemp, fselDirNo[fselDirLayer]);
	}
	FS_ChangeDir(m_szTemp);
	FS_SetCurDirEntry(location->file_entry);
#else
	FS_GetCurDirEntry(&location->file_entry);
	FS_GetShortName(location->file_entry, location->file_name);
	FS_GetSubDirEntry(&location->subdir_entry);
	FS_GetShortName(location->subdir_entry, location->subdir_name);
	
	app_trace(APP_MAIN_TRC, "Select dir %s, file %s", location->subdir_name, location->file_name);
#endif
	return TRUE;
	
}

/********************************************************************************
* Description : Select the next file according to type & mode, then get the name of file.
*
* Arguments  :
*            strFile:  msdos 8.3  file name of next file
* Returns     :
*            TRUE: Successful
*            FALSE: Fail, detail information in fselError
* Notes       :  None
*
********************************************************************************/
BOOL fselGetNext(UINT32 *pEntry)
{
	UINT32 num;
	
	switch(fselMode)
	{
	case FSEL_ALL_REPEATONE:
		{
			// handle the case of repeat one
			if((0 == fselFileNo) && (0 != fselTotal))
			{
				fselFileNo = 1;
			}
			return fselGetByNO(pEntry, fselFileNo);
		}
	case FSEL_DIR_RANDOM:
		{
			// handle the case of random
			num = (rand() % fselTotal) + 1;
			return fselGetByNO( pEntry, num);
		}
	case FSEL_DIR_SEQUENCE:
	case FSEL_DIR_INTRO:
		return SelectNext(pEntry);
	case FSEL_ALL_SEQUENCE:
		{
			return SelectNextDisk(pEntry);
		}
	case FSEL_ALL_REPEATALL:
		{
			if (SelectNextDisk(pEntry))
			{
				return TRUE;
			}
			else
			{
				fselInit(fselType, fselMode, 0, fselDiskLetter);
				return SelectNextDisk(pEntry);
			}
		}
	case FSEL_DIR_REPEATALL:
		{
			if (SelectNext(pEntry ))
			{
				return TRUE;
			}
			fselFileNo = 0;
			return SelectNext(pEntry);
		}
	default:
		{
			return FALSE;
		}
	}//switch
}




/********************************************************************************
* Description : Select  the file name according to number in current directory
*
* Arguments  :
*            strFile:  file name
*            Num  : File number in currect directory
* Returns     :
*            TRUE: Successful
*            FALSE: Fail, detail information in fselError.
* Notes       :  None
*
********************************************************************************/
BOOL fselGetByNO( UINT32 *strFile, const UINT32 Num)
{
	if( (Num > fselTotal) || (Num == 0))
	{
		fselError = FSEL_ERR_OUTOF;
		return FALSE;
	}
	
	fselFileNo = 0;
	
	while (Num > fselFileNo)
	{
		if(!SelectNext(strFile))
		{
			return FALSE;
		}
	}
	return TRUE;
}


/********************************************************************************
* Description : Select the next directory of current select directory
*
* Arguments  :
*                 strDir: Dos 8.3 directory name of the next.
*
* Returns     :
*    TRUE:  success.
*    FALSE: fail, detail info store in fselError.
* Notes       :  None
*
********************************************************************************/
BOOL fselNextDir(TCHAR *strDir)
{
#ifdef APP_SUPPORT_PLAYLIST
	UINT8 wtemp = fselDirNo[fselDirLayer];
	wtemp ++;
	if (! ToDirEntry(strDir, wtemp))
	{
		return FALSE;
	}
	fselDirNo[fselDirLayer] = wtemp;/*record the last Use Numbers*/
#endif
	return TRUE;
}

/********************************************************************************
* Description : enter the current directory
*
* Arguments :
*         strDir:output parent directory name
* Returns   :
*           TRUE:  success.
*           FALSE: fail, detail info store in fselError.
* Notes     :
*
********************************************************************************/
BOOL    fselEntCurrentDir(TCHAR *strDir)
{
	UINT32 total = GetFileTotal(strDir); //get the parement
	
	if (ERR_SUCCESS != FS_ChangeDir(strDir)) //enter the current dir
	{
		fselError = FSEL_ERR_FS;
		return FALSE;
	}
	
	fselTotal = total;
#ifdef APP_SUPPORT_PLAYLIST
	if(fselDirLayer < TOTALDIRLAYER)
	{
		fselDirNo[fselDirLayer] = 0;
	}
#endif
	fselFileNo = 0;
	return TRUE;
}

/********************************************************************************
* Description : return the parent directory
*
* Arguments :
*         strDir:output parent directory name
* Returns   :
*           TRUE:  success.
*           FALSE: fail, detail info store in fselError.
* Notes     :
*
********************************************************************************/
BOOL    fselRetParentDir(TCHAR *strDir)
{
#ifdef APP_SUPPORT_PLAYLIST
	UINT16 wtemp = 0;
	
	if ( (fselDirNo[fselDirLayer] == 0) && (fselDirLayer > 0) )
	{
		fselDirLayer --;
	}
	else
	{
		fselError = FSEL_ERR_OUTOF;
		return FALSE;
	}
	
	if ( ERR_SUCCESS != FS_ChangeDir(FS_CD_PARENT) ) //return parent directory
	{
		fselError = FSEL_ERR_FS;
		return FALSE;
	}
	
	if ( !ToDirEntry(strDir, fselDirNo[fselDirLayer]) ) //get the parent direcory name
	{
		return FALSE;
	}
#endif
	fselTotal  = GetFileTotalCur();  //get the parament
	fselFileNo = 0;
	return TRUE;
}



/********************************************************************************
* Description : Get the total of valid files according to select type and mode
*
* Arguments  : None
* Returns     : Total of valid files
* Notes       : None
*
********************************************************************************/
UINT16 fselGetTotal(void)
{
	if  (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
	{
		return fselTotal;
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST)
	{
		return PlaylistTotal;
	}
#endif
	return fselTotal;
}

/********************************************************************************
* Description : Get the No. of current selected file.
*
* Arguments  : None
* Returns     : Number of current selected file.
* Notes       : None
*
********************************************************************************/
UINT16 fselGetNo(void)
{
	if  (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
	{
		return  fselFileNo;
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST)
	{
		if (fileoffset - PlaylistStart > 0)
		{
			return fileoffset - PlaylistStart;
		}
		else
		{
			return 1;
		}
	}
#endif
	return  fselFileNo;
}

/********************************************************************************
* Description : Set the select mode.
*
* Arguments  :
            mode:  the new mode
* Returns     :  Successful or Fail
* Notes       : None
*
********************************************************************************/
BOOL fselSetMode( UINT8 mode)
{
	if( mode >=  FSEL_DIR_END)
	{
		return FALSE;
	}
	fselMode = mode;
	return TRUE;
}

/********************************************************************************
* Description : Get the Error id.
*
* Arguments  : None
* Returns     : Error id
* Notes       :  None
*
********************************************************************************/
UINT8 fselGetErrID(void)
{
	return fselError;
}

/********************************************************************************
* Description : Select the Prev file according to type & mode, then get the name of file.
*
* Arguments  :
            strFile: Dos 8.3 file name of selected file.
* Returns     :
            TRUE: Successful.
            FALSE: Fail, detail information in fselError
* Notes       :  None
*
********************************************************************************/
BOOL fselGetPrev(UINT32 *pEntry)
{
	UINT32 dir_entry;
	
	switch ( fselMode)
	{
	case FSEL_ALL_REPEATONE:
	case FSEL_DIR_RANDOM:
		return fselGetNext(pEntry);
	case FSEL_DIR_SEQUENCE:
	case FSEL_DIR_INTRO:
		return SelectPrev(pEntry);
	case FSEL_ALL_SEQUENCE:
		return SelectPrevtDisk(pEntry);
	case FSEL_ALL_REPEATALL:
		if (SelectPrevtDisk(pEntry))
		{
			return TRUE;
		}
		else
		{
			fselInit(fselType, fselMode, 0, fselDiskLetter);
			
			while((ERR_SUCCESS == FS_GetPrevEntry(&dir_entry,  NULL, FS_DIR_DIR)))
			{
				FS_ChangeSubDir(dir_entry);
			}
			return SelectPrevtDisk(pEntry);
		}
	case FSEL_DIR_REPEATALL:
		if (SelectPrev(pEntry ))
		{
			return TRUE;
		}
		fselFileNo = fselTotal + 1;
		return SelectPrev(pEntry);
	default:
		return FALSE;
	}
}

/********************************************************************************
* Description : Select the prev of current select directory
*
* Arguments  : none
*
*
* Returns     :
*    TRUE:  success.
*    FALSE: fail, detail info store in fselError.
* Notes       : rewind to the last valid directory when fselDir == 0;
*
********************************************************************************/
BOOL fselPrevDir(TCHAR *strDir)
{
#ifdef APP_SUPPORT_PLAYLIST
	UINT8 wtemp;
	
	if(fselDirNo[fselDirLayer] <= 1)
	{
		return FALSE;
	}
	
	//seek to the prev valid directory enty
	wtemp = fselDirNo[fselDirLayer];
	wtemp --;
	
	if ( !ToDirEntry(strDir, wtemp))
	{
		return FALSE;
	}
	
	fselDirNo[fselDirLayer] = wtemp;
#endif
	return TRUE;
}

/********************************************************************************
* Description : select the next file and get the file name
*
* Arguments  :
            strFile: Dos 8.3 file name of selected
* Returns     :
            TRUE: Successful
            FALSE: Fail, detail information in fselError
* Notes       : don't rewind
*
********************************************************************************/
BOOL SelectPrev(UINT32* pEntry)
{
	UINT8   temp;
	UINT32  count;
	
	if (fselFileNo == 0 && fselTotal != 0)
	{
		fselError = FSEL_ERR_OUTOF;
		return FALSE;
	}
	
	//search the file by current file ext name.
	if (fselTotal == 0 || fselFileNo > fselTotal)
	{
		temp = FS_DIR_FILE;
	}
	else
	{
		temp = FS_DIR_FILE_CONT;
	}
	
	if( ERR_SUCCESS == FS_GetPrevEntry(pEntry,  GetExtBuf(), temp))
	{
		if(fselTotal == 0)
		{
			FS_GetEntryCount(&count, GetExtBuf(), FS_DIR_FILE);
			fselFileNo = fselTotal = count;
		}
		else
		{
			fselFileNo--;
		}
		return TRUE;
	}
	
	fselError = FSEL_ERR_FAIL;
	return FALSE;
}



/********************************************************************************
* Description : get the next file in playlist or common file system
*
* Arguments  :
*             filename : output the next filename
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :in common type, it call fselGetNext
*
********************************************************************************/
BOOL fselGetNextFile(UINT32* pEntry)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)   //comon file system
	{
		return fselGetNext(pEntry);
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST)  //playlist file system
	{
		UINT8    iTime = 0;
		if (!fpPlaylist)    //must open the playlist
		{
			fselError = FSEL_ERR_PLAYLIST;
			return FALSE;
		}
		switch(fselMode)
		{
		case FSEL_ALL_REPEATONE:  //current file
			{
				// handle the case of repeat one
				if ( getplayItemparam(pEntry) == 1 )       //默认输入：PlaylistCurItem
				{
					return TRUE;
				}
				else
				{
					fselError = FSEL_ERR_PLAYLIST;
					return FALSE;
				}
			}
			break;
			
		case FSEL_DIR_RANDOM:  //random file in current class
			{
				// handle the case of random
				PlaylistCurItem = Random() % ClassItemTotal;
				
				if (playlistnext(pEntry))
				{
					return TRUE;
				}
				else
				{
					fselError = FSEL_ERR_PLAYLIST;
					return FALSE;
				}
			}
			break;
			
		case FSEL_DIR_SEQUENCE: //sequence get the next file in current class
		case FSEL_DIR_INTRO:
			{
				if ( PlaylistCurItem >= ClassItemTotal )
				{
					return FALSE;
				}
				else
				{
					return playlistnext(pEntry);
				}
			}
			break;
			
		case FSEL_ALL_SEQUENCE:     //all file sequence select the next file
			{
RETRY:
				hal_TimWatchDogKeepAlive();
				if ( PlaylistCurItem >= ClassItemTotal ) //当前class item已播放完
				{
					if ( PlaylistClassTotal == 0 )
					{
						fselError = FSEL_ERR_OUTOF;
						return FALSE;
					}
					else if ( PlaylistCurClass >= PlaylistClassTotal )  //the last class ,return
					{
						fselError = FSEL_ERR_OUTOF;
						return FALSE;
					}
					else if ( playlistclassNext(playlistclassname) ) //get the next class
					{
						if (playlistnext(pEntry))
						{
							return TRUE;
						}
						else
						{
							iTime ++;
							if (iTime > 5 )     //最多循环找后5个目录？？
							{
								return FALSE;
							}
							goto RETRY;
						}
					}
					else
					{
						return FALSE;
					}
				}
				else if (playlistnext(pEntry))  //the same class get the next file
				{
					return TRUE;
				}
				else
				{
					iTime ++;
					if (iTime > 5 )
					{
						return FALSE;
					}
					goto RETRY;
				}
			}
			break;
			
		case FSEL_ALL_REPEATALL:
			{
RETRYALL:
				hal_TimWatchDogKeepAlive();
				if ( PlaylistCurItem >= ClassItemTotal ) //get the next class file
				{
					if ( PlaylistCurClass >= PlaylistClassTotal ) //reached last class, get the first class
					{
						PlaylistCurClass = 0;      //轮转回第一个class-1
					}
					
					if (playlistclassNext(playlistclassname)) //next class
					{
						if (playlistnext(pEntry))
						{
							return TRUE;
						}
						else
						{
							iTime ++;
							if (iTime > 5 )
							{
								return FALSE;
							}
							goto RETRYALL;
						}
					}
					else
					{
						return FALSE;
					}
				}
				else if (playlistnext(pEntry)) //get the next file in the same class
				{
					return TRUE;
				}
				else
				{
					iTime ++;
					if (iTime > 5 )
					{
						return FALSE;
					}
					goto RETRYALL;
				}
			}
			break;
			
		case FSEL_DIR_REPEATALL:  // in the same class get the file
			{
				if ( PlaylistCurItem >=  ClassItemTotal ) // reached the last , get the first
				{
					PlaylistCurItem = 0;
				}
				return playlistnext(pEntry);
			}
			break;
			
		default:
			return FALSE;
		}   //switch
	}
#endif
	else
	{
		return FALSE;
	}
	
}

/********************************************************************************
* Description : get the prev file in playlist or common file system
*
* Arguments  :
*             filename : output the prev filename
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :in common type, it call fselGetPrev
*
********************************************************************************/
BOOL fselGetPrevFile(UINT32* pEntry)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)   //comon file system
	{
		return fselGetPrev(pEntry);
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST)   //playlist system
	{
		UINT8 iTime = 0;
		if (!fpPlaylist)    //must open the playlist file
		{
			fselError = FSEL_ERR_PLAYLIST;
			return FALSE;
		}
		switch(fselMode)    //begin select mode
		{
		case FSEL_ALL_REPEATONE:    //current file paly
			{
				// handle the case of repeat one
				if ( getplayItemparam(pEntry) == 1 )       //默认输入：PlaylistCurItem
				{
					return TRUE;
				}
				else
				{
					fselError = FSEL_ERR_PLAYLIST;
					return FALSE;
				}
			}
			break;
			
		case FSEL_DIR_RANDOM:   //random file play in current class
			{
				PlaylistCurItem = Random() % ClassItemTotal;
				
				if (playlistprev(pEntry))
				{
					return TRUE;
				}
				else
				{
					fselError = FSEL_ERR_PLAYLIST;
					return FALSE;
				}
			}
			break;
			
		case FSEL_DIR_SEQUENCE: //in current class sequence play
		case FSEL_DIR_INTRO:
			{
				if ( PlaylistCurItem <= 1 )
				{
					return FALSE;
				}
				else
				{
					return playlistprev(pEntry);
				}
			}
			break;
			
		case FSEL_ALL_SEQUENCE: // sequence play in all playlist file
			{
RETRY:
				hal_TimWatchDogKeepAlive();
				if ( PlaylistCurItem <= 1 ) //当前class item已播放完
				{
					if ( PlaylistClassTotal == 0 )
					{
						fselError = FSEL_ERR_OUTOF;
						return FALSE;
					}
					else if ( PlaylistCurClass <= 1  )  //the first class ,return
					{
						fselError = FSEL_ERR_OUTOF;
						return FALSE;
					}
					else if ( playlistclassPrev(playlistclassname) ) //get the prev class
					{
						if (playlistprev(pEntry))
						{
							return TRUE;
						}
						else
						{
							iTime ++;
							if (iTime > 5 )     //最多循环找后5个class？？
							{
								return FALSE;
							}
							goto RETRY;
						}
					}
					else
					{
						return FALSE;
					}
				}
				else if (playlistprev(pEntry)) // get the prev file in the same class
				{
					return TRUE;
				}
				else
				{
					iTime ++;
					if (iTime > 5 )
					{
						return FALSE;
					}
					goto RETRY;
				}
			}
			break;
			
		case FSEL_ALL_REPEATALL:  // All file repeat play
			{
RETRYALL:
				hal_TimWatchDogKeepAlive();
				if ( PlaylistCurItem <= 1 ) //get the next class file
				{
					if ( PlaylistCurClass <= 1 ) //reached first class, get the first class
					{
						PlaylistCurClass = PlaylistClassTotal + 1;
					}
					
					if (playlistclassPrev(playlistclassname))   //get the prev class
					{
						PlaylistCurItem = ClassItemTotal + 1;
						
						if (playlistprev(pEntry))
						{
							return TRUE;
						}
						else
						{
							iTime ++;
							if (iTime > 5 )
							{
								return FALSE;
							}
							goto RETRYALL;
						}
					}
					else
					{
						return FALSE;
					}
				}
				else if (playlistprev(pEntry)) //get the prev file in the same class
				{
					return TRUE;
				}
				else
				{
					iTime ++;
					if (iTime > 5 )
					{
						return FALSE;
					}
					goto RETRYALL;
				}
			}
			break;
			
		case FSEL_DIR_REPEATALL: //current dir repeat play
			{
				if ( PlaylistCurItem <=  1 ) // reached the last , get the first
				{
					PlaylistCurItem = ClassItemTotal + 1;
				}
				return playlistprev(pEntry);
			}
			break;
			
		default:
			return FALSE;
		}//switch
	}
#endif
	else
	{
		return FALSE;
	}
}

/********************************************************************************
* Description : get the next dir in playlist or class in common file system
*
* Arguments  :
*             groupname : output the prev groupname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselGetNextGroup(TCHAR *groupname)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)   //common file system
	{
		return fselNextDir( groupname );            //get next dir
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST)   // playlist file system
	{
		return playlistclassNext( groupname );     //get the next class
	}
#endif
	return FALSE;
}

/********************************************************************************
* Description : get the prev dir in playlist or class in common file system
*
* Arguments  :
*             groupname : output the prev groupname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselGetPrevGroup(TCHAR *groupname)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)   //common file system
	{
		return fselPrevDir(groupname);
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST) //play list file system
	{
		return  playlistclassPrev(groupname);
	}
#endif
	return FALSE;
}

/********************************************************************************
* Description : get the long group name in playlist or common file system
*
* Arguments  :
*             grouplongname : output the  current groupname(long name)
              iLen :the length of get group , it is invalid  in playlist
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselGetCurGroupL(TCHAR *grouplongname, UINT16 iLen)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR) //common file system
	{
		return ToDirEntry(grouplongname, 0); //fselDirNo[fselDirLayer]);  //in common fsel system , dir is the parent
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if ( fselPLOrCommonDir == FSEL_TYPE_PLAYLIST )     //play list file system
	{
		return getClassParam(grouplongname, iLen);    //默认输入：全局变量PlaylistCurClass
	}
#endif
	return FALSE;
}

/********************************************************************************
* Description : get the long file name in playlist or common file system
*
* Arguments  :
*             longfilename : output the  current filename(long name)
              iLen :the length of get group
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselGetCurFNameL(TCHAR *longfilename, UINT8 iLen)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)  // common file system
	{
		UINT32 LEntry = 0;
		fselGetByNO(&LEntry, fselFileNo);//through file no get the name
		return (ERR_SUCCESS == FS_GetShortName(LEntry, longfilename));
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST) //play list file system
	{
		return playlistnext(longfilename);
	}
#endif
	return FALSE;
}


/********************************************************************************
* Description : del the file in playlist or common file system
*
* Arguments  :
*             type : file or dir
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL    fselDelFile( UINT8 type)
{
	TCHAR strName[MAX_FILE_LEN];
#ifdef APP_SUPPORT_PLAYLIST
	TCHAR name[MAX_FILE_LEN];
#endif
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)   // common dir
	{
		// report the exception case
		if (( type == FSEL_DEL_FILE) && ( fselFileNo == 0)) //file no is valid
		{
			fselError = FSEL_ERR_EXCEPTION;
			return FALSE;
		}
#ifdef APP_SUPPORT_PLAYLIST
		if ((type == FSEL_DEL_DIRFILE) && (fselDirNo[fselDirLayer] == 0)) //dir no is valid
		{
			fselError = FSEL_ERR_EXCEPTION;
			return FALSE;
		}
#endif
		if (type > FSEL_DEL_DIRFILE)    // other type is invalid
		{
			fselError = FSEL_ERR_EXCEPTION;
			return FALSE;
		}
		
		//delete the file
		if ( type == FSEL_DEL_FILE )
		{
			UINT32  LEntry = 0;
			fselGetByNO( &LEntry, fselFileNo) ;
			
			if ( FS_GetShortName(LEntry, strName) == ERR_SUCCESS )
			{
				if ( FS_Delete(strName) == ERR_SUCCESS) //realy remove
				{
					fselFileNo--;   //change the parament
					fselTotal --;
					return TRUE;
				}
				else
				{
					fselError = FSEL_ERR_FS;
					return FALSE;
				}
			}
		}
#ifdef APP_SUPPORT_PLAYLIST
		//delete the dir
		if (ToDirEntry( strName, fselDirNo[fselDirLayer]))
		{
			if ( ERR_SUCCESS == FS_ChangeDir(strName) )  //enter it get the parament
			{
				UINT32 inode;
				
				if ( ERR_SUCCESS != FS_GetNextEntry(&inode, NULL, FS_DIR_DIR) ) //Not Null
				{
					return FALSE;
				}
				
				FS_GetEntryName(inode, MAX_FILE_LEN, name); // sub dir name
				
				if ( ERR_SUCCESS == FS_ChangeDir(FS_CD_PARENT) ) //return parent dir , del it
				{
					if ( ERR_SUCCESS == FS_RemoveDir(strName) )  //realy remove
					{
					
						fselDirNo[fselDirLayer]--;  //change the parament
						if (fselDirNo[fselDirLayer] == 0)  // in First , we will return the parent dir
						{
							if ( !fselRetParentDir(name) )
							{
								fselError = FSEL_ERR_FS;
								return FALSE;
							}
						}
						
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
			else
			{
				fselError = FSEL_ERR_FS;
				return FALSE;
			}
		}
		else    //Dir not match the dir number
		{
			fselError = FSEL_ERR_OUTOF;
			return FALSE;
		}
#endif
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST)//delete the palylist file
	{
		if ( type != FSEL_DEL_FILE)  //playlist only delete the file
		{
			fselError = FSEL_ERR_EXCEPTION;
			return FALSE;
		}
		
		if ( playlistdel(strName) )  //delete tag in the lst file
		{
			if ( FS_Delete(strName) == 0) //
			{
				//when we have changed the palylist , we need close the playlist
				if ( fselPlayListEnt(PlayListFileName) )
				{
					getClassParam(NULL, 28);        //只取ClassParam，不取class name
					if ( fselGetNextFile(strName) )
					{
						return TRUE;
					}
					else
					{
						fselError = FSEL_ERR_PLAYLIST;
						return FALSE;
					}
				}
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
#endif
	else
	{
		fselError = FSEL_ERR_EXCEPTION;
		return FALSE;
	}
}


/********************************************************************************
* Description : throgh the keeped values set the file selector parament
*
* Arguments  :location :the keeped vlaues
*
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
UINT8 fselSetLocation(file_location_t  *location)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)  //common file system
	{
		return fselSetLocationC(location);
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if ( fselPLOrCommonDir == FSEL_TYPE_PLAYLIST )  //play list file system
	{
		WCHAR    name[28];
		memcpy( name, location->subdir_name, 12 );        //restore the playlist file name
		memcpy( ItemFileName, location->file_name, 12 );
		
		if ( !fselPlayListEnt(name) )
		{
			return FALSE;               //open the play list err
		}
		
		PlaylistCurClass = location->subdir_entry;      //PlaylistCurClass <---------------
		
		if ( !getClassParam(NULL, 28) )   //默认输入：PlaylistCurClass,  只取信息，不取class名字
		{
			return FALSE;
		}
		
		PlaylistCurItem = location->file_entry;                  //PlaylistCurItem <-----------------
		
		if ( PlaylistCurItem != 0 )
		{
			if ( getplayItemparam(name) == 1 )  //默认输入：PlaylistCurItem   0: Fail list文件错误
			{
				//1: Successful,  2：//歌曲不存在
				return TRUE;
			}
			else
			{
				if ( playlistnext(name) )
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}
		else
		{
			return TRUE;
		}
	}
#endif
	else
	{
		return FALSE;
	}
}

/********************************************************************************
* Description : keep values of the file selector parament
*
* Arguments  :location :the keeped vlaues
*
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselGetLocation(file_location_t  *location)
{
	if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)   //comond file system
	{
		return fselGetLocationC(location);
	}
#ifdef APP_SUPPORT_PLAYLIST
	else if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST)  //play list file system
	{
		memcpy(location->subdir_name,     PlayListFileName, 12);   //keep the playlist file name
		memcpy(location->file_name, ItemFileName,     12);   //keep the song file name
		location->file_entry =  PlaylistCurItem;         //global variable, Initialized by 0
		location->subdir_entry  = PlaylistCurClass;      //keep the class no
		return TRUE;
	}
#endif
	else
	{
		return FALSE;
	}
}




