
#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "fat_base.h"
#include "fs.h"
#include "fs_base.h"
#include "fat_local.h"
#include "reg.h"

extern struct task_struct g_fs_current;
extern HANDLE g_fs_sem;
extern struct file* g_fs_file_list;
extern UINT16 fselTotal;
UINT32 g_list_current_entry;


INT32 SRVAPI FS_GetNextEntry(
    UINT32 *pINode, 
    const PTCHAR pExtName,
    BYTE type)
{

  INT32 iRet = ERR_SUCCESS;
  struct inode *dir_i = g_fs_current.pwd;
  UINT32 ino;
  struct inode * inode = NULL;

  if(dir_i == NULL)
    return ERR_FS_NOT_MOUNT;

  if(type & 0x80)
	  ino = FAT_CSO2INO(0, 0, 0); 
  else
  	  ino = g_list_current_entry;

  FS_Down();

  iRet = get_next_entry(dir_i, &ino, type, NULL, pExtName, GET_NEXT_ENTRY);


	if ( DIR_ENTRY_NOT_FOUND == iRet )
	{
		D((DL_FATWARNING, "+++ do_find_entry() return failed\n"));
		iRet = ERR_FS_NO_MORE_FILES;
		goto failed;
	}
	if (DIR_ENTRY_FOUND == iRet)
	{
	   *pINode = g_list_current_entry = ino;
	   iRet = ERR_SUCCESS;
	}

failed:  
	FS_Up();

  return iRet;
}


INT32 SRVAPI FS_GetPrevEntry(
    UINT32 *pINode, 
    const PTCHAR pExtName,
    BYTE type)
{
  INT32 iRet = ERR_SUCCESS;
  struct inode *dir_i = g_fs_current.pwd;
  UINT32 ino;
  struct inode * inode = NULL;

  if(dir_i == NULL)
    return ERR_FS_NOT_MOUNT;

  if(type & 0x80)
	  ino = FAT_CSO2INO(0, 0, 0); 
  else
	  ino = g_list_current_entry;
    
  FS_Down();

  iRet = get_next_entry(dir_i, &ino, type, NULL, pExtName, GET_PREV_ENTRY);


	if ( DIR_ENTRY_NOT_FOUND == iRet )
	{
		D((DL_FATWARNING, "+++ do_find_entry() return failed\n"));
		iRet = ERR_FS_NO_MORE_FILES;
		goto failed;
	}
	if (DIR_ENTRY_FOUND == iRet)
	{
	   *pINode = g_list_current_entry = ino;
	   iRet = ERR_SUCCESS;
	}

failed:  
	FS_Up();
  return iRet;
}


INT32 SRVAPI FS_GetEntryCount(
    UINT32 *pCount, 
    const PTCHAR pExtName,
    BYTE type)
{

  INT32 iRet = ERR_SUCCESS;
  struct inode *dir_i = g_fs_current.pwd;
  UINT32 ino;
  struct inode * inode = NULL;
  *pCount = 0;

  if(dir_i == NULL)
    return ERR_FS_NOT_MOUNT;

  ino = FAT_CSO2INO(0, 0, 0); 

  FS_Down();

  iRet = get_next_entry(dir_i, &ino, type, pCount, pExtName, GET_ENTRY_COUNT);

  FS_Up();

  if (DIR_ENTRY_FOUND == iRet)
  {
     iRet = ERR_SUCCESS;
  }
  return iRet;
}


INT32 SRVAPI FS_OpenDirect(UINT32 ino,	UINT32 iFlag, UINT32 iMode)
{
	INT32 fd;
	struct inode * inode = NULL;
	struct file * f = NULL;
	WCHAR *basename = NULL;
	INT32 namelen;
	INT32 iErrCode = ERR_SUCCESS;
	UINT16 iULen = 0;
	UINT32 iFileMode;


	iFileMode= FS_ATTR_MARK;
	iFileMode |= ((iMode & FS_ATTR_RO)	== FS_ATTR_RO) ? FS_ATTR_RO : FS_ATTR_MARK;
	iFileMode |= ((iMode & FS_ATTR_HIDDEN)	== FS_ATTR_HIDDEN) ? FS_ATTR_HIDDEN : FS_ATTR_MARK;
	iFileMode |= ((iMode & FS_ATTR_SYSTEM)	== FS_ATTR_SYSTEM) ? FS_ATTR_SYSTEM : FS_ATTR_MARK;
	iFileMode |= ((iMode & FS_ATTR_ARCHIVE)  == FS_ATTR_ARCHIVE) ? FS_ATTR_ARCHIVE : FS_ATTR_MARK;

    if(ino == 0)
    {
		D((DL_FATERROR, "FS_OpenDirect: ino = 0.\n"));
        return ERR_FS_FILE_NOT_EXIST;
    }

	// cannot create file use ino
	if( iFlag & FS_O_CREAT)	
	{	 
		D((DL_FATERROR, "FS_OpenDirect: ERR_FS_INVALID_PARAMETER1,iFlag = 0x%x.\n",iFlag));
		return ERR_FS_INVALID_PARAMETER;
	}
	
	// To truncate the file, we should have the write access to the file
	if ( ( iFlag & FS_O_TRUNC ) && ( ( iFlag & FS_O_ACCMODE ) == FS_O_RDONLY ) )
	{	 
		D((DL_FATERROR, "FS_OpenDirect: ERR_FS_INVALID_PARAMETER1,iFlag = 0x%x.\n",iFlag));
		return ERR_FS_INVALID_PARAMETER;
	}
	// To append the file, we should have the write access to the file
	if ( ( iFlag & FS_O_APPEND ) && ( ( iFlag & FS_O_ACCMODE ) == FS_O_RDONLY ) )
	{
		D((DL_FATERROR, "FS_OpenDirect: ERR_FS_INVALID_PARAMETER2,iFlag = 0x%x.\n",iFlag));
		return ERR_FS_INVALID_PARAMETER;
	}
	// The low byte greate less 3.
	if((iFlag & 0xf) > FS_O_ACCMODE )
	{
		D((DL_FATERROR, "FS_OpenDirect: ERR_FS_INVALID_PARAMETER3.iFlag = 0x%x.\n",iFlag));
		return ERR_FS_INVALID_PARAMETER;
	}
	FS_Down();
	
	// Get a valid file descriptor
	for ( fd = 0; fd < (INT32)g_fs_cfg.uNrOpenFileMax; fd++ )
	{
		if ( NULL == g_fs_current.filp[ fd ] )				   /*unused fhd found*/
			break;
	}

	if(fd == (INT32)g_fs_cfg.uNrOpenFileMax)
	{
		D((DL_FATWARNING, "FS_OpenDirect: FS_NR_OPEN_FILES_MAX.\n"));
		iErrCode = ERR_FS_FDS_MAX;
		goto step0_failed;
	}

	f = fs_alloc_fd(); // 向内存动态分配一个新的文件描述符。
	if (NULL == f)
	{
		D( ( DL_FATERROR, "in FS_OpenDirect, fs_alloc_fd() return NULL") );
		iErrCode = ERR_FS_NO_MORE_MEMORY;
		goto step0_failed;
	}

	f = fs_fd_list_add(&g_fs_file_list, f); // 添加到文件描述符链表。
	
    inode = iget( (g_fs_current.pwd)->i_sb, ino, TRUE); 
    
    if( NULL == inode ) // iget failed
    {
        D((DL_VFSERROR, "FS_OpenDirect: iget inode is NULL.ino = %d\n",ino));
        iErrCode = DIR_ENTRY_NOT_FOUND;
		goto step0_failed;
    }

	// If the file is existd, creating action must be done
	if(( iFlag & FS_O_CREAT ) && ( iFlag &FS_O_EXCL )) 
	{
		D( ( DL_FATERROR, "in FS_OpenDirect the file already existed, and flag is FS_O_CREAT") );
		iErrCode = ERR_FS_FILE_EXIST;
		goto step2_failed;
	}

	// 找到是一个目录而不是文件则出错。
	if ( FS_IS_DIR( inode->i_mode ) ) // Attempt to open a directroy file
	{
		D( ( DL_FATERROR, "in FS_OpenDirect, the inode is not a file,but a dir\n") );
		iErrCode = ERR_FS_IS_DIRECTORY;
		goto step2_failed;
	}

	if ( iFlag & FS_O_TRUNC )
	{
	   switch ( g_fs_current.pwd->i_fstype )
	   {
		case FS_TYPE_FAT:
			if(!(iFlag & FS_O_CREAT))
			{
				iFileMode = inode->i_mode;
			}
			iErrCode = fat_trunc_file( g_fs_current.pwd, inode, iFileMode );
			if(ERR_SUCCESS != iErrCode) // create file failed
			{
				D( ( DL_FATERROR, "in FS_OpenDirect, trunc file error, ErrorCode = %d\n",iErrCode) );
				//lErrCode = ERR_FS_TRUNC_FILE_FAILED;
				goto step2_failed;
			}
			break;

		default:
			D( ( DL_FATERROR, "in FS_OpenDirect, unknown file system type_1\n") );
			iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
			goto step2_failed;
			break;
		}
	}	 
exit_success:
	// TODO: 此时才关联，是否会让其他process抢了该file{}
	g_fs_current.filp[ fd ] = f;
	f->f_mode = inode->i_mode;
	f->f_flags = ( UINT16 ) iFlag;
	f->f_count = 1;
	f->f_inode = inode;
	/* Whether FS_O_APPEND flag is specified or not, we always set file pointer to 0,
		 because we may read the file before the first write. */
	f->f_pos = 0;
	
  //  CSW_TRACE(BASE_DSM_TS_ID,
   //			"#fs debug open success = %d",
  //			fd);
	FS_Up();
	CSW_TRACE(BASE_DSM_TS_ID, "open hd = 0x%x",fd);  
	return fd;
	
step2_failed:
	iput( inode );
	
step0_failed:
	if (f != NULL)
	{
		fs_fd_list_del(&g_fs_file_list, f);
	}
	FS_Up();
	if(iErrCode < 0)
	{
		D( ( DL_FATERROR, "in FS_OpenDirect, error,iErrCode = %d.\n",iErrCode) );
	}
	return fs_SetErrorCode(iErrCode);
}


INT32 SRVAPI FS_GetEntryName(
    UINT32 iNode,
	INT32 iNameBufferLen,
	PTCHAR pFileName
)
{
    UINT32 bclu;
    UINT16 bsec, eoff;
    INT32 iRet;
    WCHAR *pUniName = NULL;
    UINT16 uUniLen;

    pUniName = NAME_BUF_ALLOC();

    if (!pUniName)
    {
        D( ( DL_FATERROR, "in fat_get_file_name, inode = %d,fileName = 0x%x.\n",iNode,pUniName) ); 
        return _ERR_FAT_PARAM_ERROR;
    }
    
    bclu = FAT_INO2CLUS(iNode);
    bsec = FAT_INO2SEC(iNode);
    eoff = FAT_INO2OFF(iNode);
    
    FS_Down();
    if ( FS_TYPE_FAT == g_fs_current.pwd->i_fstype )
    {
	    iRet = fat_get_name_for_entrys(&(g_fs_current.pwd->i_sb->u.fat_sb), pUniName, bclu,  bsec, eoff);
		if(_ERR_FAT_SUCCESS == iRet)		
        {
            //DSM_MemZero(pUniName, FAT_MAX_LONG_PATH_LEN + 2);
            uUniLen = DSM_UnicodeLen((UINT8*)pUniName);
            if ((UINT32)(uUniLen + LEN_FOR_NULL_CHAR) > iNameBufferLen / SIZEOF(WCHAR))
            {
                //iRet = ERR_FS_NAME_BUFFER_TOO_SHORT;
                //goto failed;
                uUniLen = iNameBufferLen / SIZEOF(WCHAR) - LEN_FOR_NULL_CHAR;
            }
            DSM_TcStrNCpy((PCWSTR)pFileName, pUniName, (uUniLen + LEN_FOR_NULL_CHAR));
            iRet = ERR_SUCCESS;
        }
        else
        {
            D( ( DL_FATERROR, "fat_get_file_name error, errorCode = %d.\n", iRet) );
            iRet = fs_SetErrorCode(iRet);			
        }
	}
	else
	{
		D( ( DL_FATERROR, "FS_GetFileName: unknow filesystem\n") );
		iRet = ERR_FS_UNKNOWN_FILESYSTEM;
	}	   

failed:

	if (pUniName)
	{
		NAME_BUF_FREE(pUniName);
	}
	FS_Up();

    if(iRet != ERR_SUCCESS)
    {
        UINT8 fatname[12],shortname[13];
        D( ( DL_FATWARNING, "FS_GetFileName fail, call FS_GetShortName\n") );
        iRet = FS_GetShortName(iNode, fatname);
        
        if(iRet == ERR_SUCCESS)
        {
            NameFat2User(fatname, shortname, 0);

            iRet = DSM_OEM2Uincode(shortname, (UINT16)DSM_StrLen(shortname), &pUniName, &uUniLen,NULL);
            if(ERR_SUCCESS == iRet)
            {
                DSM_MemCpy(pFileName,pUniName,uUniLen+2);
                DSM_Free(pUniName);
            }
            else
                memset(pFileName, 0,iNameBufferLen);
        }
        else
            memset(pFileName, 0,iNameBufferLen);
    }

	return iRet;
}

INT32 SRVAPI FS_GetShortName(UINT32 iNode, PTCHAR pFileName)
{
	UINT32 bclu;
	UINT16 bsec, eoff;
	INT32 iRet;

	if(iNode == FS_ROOT_INO)
	{
		memset(pFileName, 0, 12);
		pFileName[0] = '/';
		return ERR_SUCCESS;
	}

	bclu = FAT_INO2CLUS(iNode);
	bsec = FAT_INO2SEC(iNode);
	eoff = FAT_INO2OFF(iNode);
	
	FS_Down();
	if ( FS_TYPE_FAT == g_fs_current.pwd->i_fstype )
	{
		iRet = fat_get_short_name(&(g_fs_current.pwd->i_sb->u.fat_sb), pFileName, bclu,  bsec, eoff);
		if(_ERR_FAT_SUCCESS == iRet)		
		{
			iRet = ERR_SUCCESS;
		}
		else
		{
			D( ( DL_FATERROR, "fat_get_file_name error, errorCode = %d.\n", iRet) );
			iRet = fs_SetErrorCode(iRet);			
		}
	}
	else
	{
		D( ( DL_FATERROR, "FS_GetFileName: unknow filesystem\n") );
		iRet = ERR_FS_UNKNOWN_FILESYSTEM;
	}	   

failed:

	FS_Up();

	return iRet;
}


INT32 SRVAPI FS_ChangeSubDir(UINT32 iNode)
{
	struct inode * inode;
	struct dentry *psDe = NULL;
	WCHAR *basename;
	INT32 namelen;
	INT32 iErrCode;
	UINT16 iULen = 0;
	UINT32 uDevNo = 0;
	UINT32 startclu = 0;

    if(iNode == 0)
    {
		D((DL_FATERROR, "FS_ChangeSubDir: iNode = 0.\n"));
        return ERR_FS_FILE_NOT_EXIST;
    }

	FS_Down();

	if(iNode == FS_PARENT_INO)
	{
		if(g_fs_current.pwd == g_fs_current.root)
		{
			iErrCode = ERR_FS_NOT_DIRECTORY;
			goto step0_failed;
		}
		iNode = g_fs_current.pwd->i_ino;
		iErrCode = get_parent_node(g_fs_current.pwd, &iNode);
		if(iErrCode != DIR_ENTRY_FOUND)
		{
			iErrCode = ERR_FS_NOT_DIRECTORY;
			goto step0_failed;
		}
		startclu = g_fs_current.pwd->u.fat_i.entry.DIR_FstClusLO;
		startclu |= (UINT32)(g_fs_current.pwd->u.fat_i.entry.DIR_FstClusHI << 16);
		
	}

	if(iNode == FS_ROOT_INO)
	{
		inode = g_fs_current.root;
		if(g_fs_current.pwd != inode)
			inode->i_count ++ ;
		goto change_success;
	}
	
    inode = iget( (g_fs_current.pwd)->i_sb, iNode,TRUE); 
    
    if( NULL == inode ) // iget failed
    {
        D((DL_VFSERROR, "do_find_entry: iget de->inode is NULL.ino = %d\n",iNode));
        iErrCode = DIR_ENTRY_NOT_FOUND;
		goto step0_failed;
    }

	if ( !FS_IS_DIR( inode->i_mode ) )
	{
		D( ( DL_FATERROR, "in FS_ChangeDir, inode is not dir") );
		iput( inode );
		iErrCode = ERR_FS_NOT_DIRECTORY;
		goto step0_failed;
	}

/*	iErrCode = fs_SetWorkDirPath(g_fs_current.wdPath, (PCWSTR)pszDirName);
	if (iErrCode != ERR_SUCCESS)
	{
		D((DL_FATERROR, "FS_ChangeDir: fs_SetWorkDirPath return %d", iErrCode));
		iput(inode);
		goto step1_failed;
	}*/
change_success:

	if(startclu == 0)
		g_list_current_entry = FAT_CSO2INO(0, 0, 0);
	else
	{
		iErrCode = find_entry_by_cluster(inode, startclu, &g_list_current_entry);
		if(iErrCode != DIR_ENTRY_FOUND)
		{
			g_list_current_entry = FAT_CSO2INO(0, 0, 0);
		}
	}

	if(g_fs_current.pwd != inode)
	{
		iput( g_fs_current.pwd );
		g_fs_current.pwd = inode;	/*we have add the refcnt for inode in find_entry()*/
	}

	//hal_HstSendEvent(SYS_EVENT,0x61000000);
	//hal_HstSendEvent(SYS_EVENT,iNode);
	//hal_HstSendEvent(SYS_EVENT,g_list_current_entry);
	
	iErrCode = ERR_SUCCESS;
	
step0_failed:

	if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
	{
	   if(!fs_IsOpened(uDevNo))
	   {
		  DRV_DEV_DEACTIVE(uDevNo);
	   }
	}
	FS_Up();
	return fs_SetErrorCode(iErrCode);
}

INT32 SRVAPI FS_GetSubDirEntry(INT32 *pDirEntry)
{
  *pDirEntry = g_fs_current.pwd->i_ino;
  return ERR_SUCCESS;
}


INT32 SRVAPI FS_GetCurDirEntry(INT32 *pDirEntry)
{
  *pDirEntry = g_list_current_entry;
  return ERR_SUCCESS;
}

INT32 SRVAPI FS_SetCurDirEntry(
    INT32 DirEntry
)
{
  // check entry valid
  g_list_current_entry = DirEntry;
  return ERR_SUCCESS;
}


