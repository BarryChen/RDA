
#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "fat_base.h"
#include "fs.h"
#include "fs_base.h"
#include "fat_local.h"
#include "reg.h"

#if defined(_FS_SIMULATOR_)
#define FS_SemInit() 
#define FS_Down()    
#define FS_Up()   
#else
#define FS_Down()   sxr_TakeSemaphore(g_fs_sem)
#define FS_Up()     sxr_ReleaseSemaphore( g_fs_sem ) 
#endif

#define FS_INVALID_SEM_HANDLE    0xff
// FS API semarphore.
HANDLE g_fs_sem = FS_INVALID_SEM_HANDLE;

extern struct super_block* g_fs_super_block_table;
extern struct task_struct g_fs_current;
extern struct file* g_fs_file_list;
extern FS_DEV_INFO* g_pFsDevInfo;

extern UINT32 g_list_current_entry;


/********************************************************************************/
// Function:
//     Get Current Root Mount device dev type and fs type
// Parameter:
//     pDevType[out]: the device name type.
//     pFsType[out]: the file system type.
// Return value:
//     if is formated return ERR_SUCCESS,else return ERR_FS_NOT_MOUNT;
//*******************************************************************************
INT32 FS_GetRootType(FS_DEV_TYPE *pDevType, UINT8 *pFsType)
{
    UINT32 uDevNo;
    
    if(g_fs_current.root == NULL)
        return ERR_FS_NOT_MOUNT;

    uDevNo = g_fs_current.root->i_sb->s_dev;

    *pDevType = DSM_GetDevType(uDevNo);
    *pFsType = g_fs_current.root->i_fstype;

    return ERR_SUCCESS;
}

/********************************************************************************/
// Function:
//     Detect the device if is formated.
// Parameter:
//     pszDevName[in]:  the device name.
//     iFsType[in]: the file system type.
// Return value:
//     if is formated return ERR_FS_HAS_FORMATED,else return ERR_FS_NOT_FORMAT;
//*******************************************************************************
INT32 FS_HasFormatted(PCSTR pszDevName, UINT8 iFsType)
{
    UINT32 uDevNo = 0;
    // BOOL bIsMMC = FALSE;
    INT32 iResult;
    INT32 iRet;    
    
    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    switch(iFsType)
    {
    case FS_TYPE_FAT:
        uDevNo = DSM_DevName2DevNo(pszDevName);
        if(INVALID_DEVICE_NUMBER == uDevNo)
        {        
            D( ( DL_FATERROR, "in fs_hasformatted DevNo is invalid, pszDevName = %s\n", pszDevName) );
            D( ( DL_FATERROR, "in fs_hasformatted DevNo is invalid,uDevNo = 0x%x.\n", uDevNo ) );
            return ERR_FS_DEVICE_NOT_REGISTER;
        }
        
        if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
        {
            // bIsMMC = TRUE;
            iRet = DRV_DEV_ACTIVE(uDevNo);
            if( ERR_SUCCESS != iRet )
            {
                D( ( DL_FATERROR, "check formatted or not[%s]: Open t-flash memory device failed.\n", pszDevName ) );
                return ERR_FS_OPEN_DEV_FAILED;
            }
        }
        
        FS_Down();
        iResult = fat_HasFormated(uDevNo, iFsType);
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_IsOpened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
       FS_Up();        
        iRet = fs_SetErrorCode(iResult);
        break;
    case FS_TYPE_FFS:
    default:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
        iRet = ERR_FS_UNKNOWN_FILESYSTEM;
        break;
    }
    
    
    return iRet;
}

//********************************************************************************
// Function:
//   This function is format the specified partitition.
// Parameter:
//   pszDevType[in]: the name of device.
//   iFsType[in]: the type of file system.
//   iFlag[in]: The option of format, 0:Quickly;1:Normal.
// Return value:
//      ERR_SUCCESS indicates a successful operation. 
//         And unsuccessful operation is denoted by the err code as following.
//      ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      ERR_FAT_MALLOC_FAILED       allocte memory failed.
//*******************************************************************************

INT32 FS_Format(PCSTR pszDevName, UINT8 iFsType, UINT8 iFlag)
{
    UINT32 uDevNo = 0;
    UINT32 iBlockSize;
    UINT32 iBlockCount;
    UINT32 iDiskTotSec;
    UINT8 szVolLab[FBR_VOL_LAB_SIZE + 1];
    UINT8 szOEMName[FBR_OEM_NAME_SIZE + 1];
    UINT8 iOpt;
    INT32 iResult;
    INT32 iRet;
    // BOOL bIsMMC = FALSE;
    struct super_block *sb = NULL;

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    iRet = ERR_SUCCESS;
    D( ( DL_FATDETAIL, "FS_Format devname = %s, FsType = %d, Flag = %d.\n",
                    pszDevName, iFsType,iFlag ));  

    FS_Down();    
    switch(iFsType)
    {
    case FS_TYPE_FAT:
        uDevNo = DSM_DevName2DevNo(pszDevName);
        if(INVALID_DEVICE_NUMBER == uDevNo)
        {
            D( ( DL_FATERROR, "in FS_Format DevNo is invalid, pszDevName = %s\n", pszDevName) );
            D( ( DL_FATERROR, "in FS_Format DevNo is invalid,uDevNo = %x0x\n", uDevNo ) );
            iRet = ERR_FS_DEVICE_NOT_REGISTER;
            goto step0_failed;
        }
        
        //  Allow only format the partiton for FS module .
        if ( NULL != pszDevName &&  
             DSM_MODULE_FS_ROOT != DSM_GetDevModuleId( uDevNo) &&
             DSM_MODULE_FS != DSM_GetDevModuleId( uDevNo)
           )
        {
            D( ( DL_FATERROR, "Device[%s] is not permitted to access for FS",pszDevName) );
            iRet = ERR_FS_OPERATION_NOT_GRANTED;
            goto step0_failed;
        } 
        
         if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
        {
            // bIsMMC = TRUE;
            iRet = DRV_DEV_ACTIVE(uDevNo);
            if( ERR_SUCCESS != iRet )
            {
                D( ( DL_FATERROR, "Format device[%s]: Open t-flash memory failed.\n", pszDevName ) );
                iRet = ERR_FS_OPEN_DEV_FAILED;
                goto step0_failed;
            }
        }

        
        // If the s_root and s_mountpoint are equal, the superblock will be for ROOT FS. 
        
        sb = get_super( uDevNo );
        if (sb)
        {
            D( ( DL_FATERROR,"Format a device which is mounting is NOT permitted" )); 
            iRet = ERR_FS_FORMAT_MOUNTING_DEVICE;                
            goto step1_failed;
        }
 
        iBlockSize = 0;
        iBlockCount = 0;
        D( ( DL_FATDETAIL, " Call DRV_GET_DEV_INFO  uDevNo = %d.\n", uDevNo));  
        iResult = DRV_GET_DEV_INFO(uDevNo,&iBlockCount,&iBlockSize);
        if(ERR_SUCCESS != iResult)
        {
            D( ( DL_FATERROR,
                    " DRV_GET_DEV_INFO  failed!,iResult = %d.\n",
                    iResult)); 
            iRet = ERR_FS_GET_DEV_INFO_FAILED;                
            goto step1_failed;
     }
     else
        {            
            D( ( DL_FATDETAIL,
                    " DRV_GET_DEV_INFO  success!,iBlockCount = %d,iBlockSize = %d.\n",
                    iBlockCount,
                    iBlockSize)); 
            iDiskTotSec = (iBlockSize / DEFAULT_SECSIZE) * iBlockCount;
            
            DSM_StrCpy((INT8*)szVolLab,(INT8*)FAT_VOLLABLE);
            DSM_StrCpy((INT8*)szOEMName,(INT8*)FAT_OEMNAME);

            iOpt = iFlag;
            D( ( DL_FATDETAIL,
                    "Call fat_format with uDevNo = %d, iDiskTotSec = %d,szVolLab = %s,szOEMName = %s,iOpt = %d.\n",
                    uDevNo,
                    iDiskTotSec,
                    (UINT8*)szVolLab,
                    (UINT8*)szOEMName,
                    iOpt));         
            CSW_TRACE(BASE_DSM_TS_ID,
                    "Call fat_format with uDevNo = %d, iDiskTotSec = %d,szVolLab = %s,szOEMName = %s,iOpt = %d.\n",
                    uDevNo,
                    iDiskTotSec,
                    (UINT8*)szVolLab,
                    (UINT8*)szOEMName,
                    iOpt);         
            iResult = fat_format(uDevNo, iDiskTotSec,(UINT8*)szVolLab,(UINT8*)szOEMName,iOpt);

            D( ( DL_FATDETAIL, "fat_format return, iResult = %d.\n", iResult));  
            if(_ERR_FAT_SUCCESS == iResult)
            {
                iRet = ERR_SUCCESS;
            }
            else
            {
                iRet = fs_SetErrorCode(iResult);
            }
        }
        break;    
    case FS_TYPE_FFS:        
    default:
        iRet = ERR_FS_UNKNOWN_FILESYSTEM;
        break;
    }
        
step1_failed:    
step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
      
        iRet = DRV_DEV_FLUSH(uDevNo);
        if(ERR_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, " FS_Format flush cache error.  return error = 0x%x.",iRet) );
        }       

        if(!fs_IsOpened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    return iRet;
}

/*
We custom the structure file{} for FS_Find*** APIs:
1. Use f_pos field to save the next dir-entry to be found next time
2. Use f_mode and  f_flags fields to save the pointer to the pattern for FindNextFile().
    The memory will be freed at FindClose().
*/
INT32 FS_FindFirstFile( PCSTR pszFileName, PFS_FIND_DATA  pFindData )
{
    WCHAR *path = NULL;
    WCHAR *pattern = NULL;
    INT32 patternlen;
    struct inode *dir_i;
    struct file * f = NULL;
    struct dentry *psDe = NULL;
    INT32 fd;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;
    
    if(NULL == pFindData) // invalid pathname or pFindData is NULL
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, parameter error" ) );
        return  ERR_FS_INVALID_PARAMETER;
    }
    
    iULen = DSM_UnicodeLen((UINT8*)pszFileName);    
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    // path name character validate, Ignore base name.
    if( !is_file_name_valid((PCWSTR) pszFileName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }
    
    FS_Down();
    
    for ( fd = 0; fd < (INT32)g_fs_cfg.uNrOpenFileMax; fd++ )
    {
        if ( NULL == g_fs_current.filp[ fd ] )                 /*unused fd found*/
        {
            break;
        }
    }
    if((INT32)g_fs_cfg.uNrOpenFileMax == fd) // No fd is available
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, fd overflow, fd = %d",fd ) );
        iRet = ERR_FS_FDS_MAX;
        goto step0_failed;
    }

    f = fs_alloc_fd();
    if (NULL == f)
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, fs_alloc_fd() return NULL") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }
    f = fs_fd_list_add(&g_fs_file_list, f);
    
    path = format_dir_name((PCWSTR)pszFileName );
    if(NULL == path) // malloc failed
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, path is NULL" ) );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }

    dir_i = dir_namei( path, &patternlen, (CONST WCHAR**)&pattern );
     // path is invalid 
    if(NULL == dir_i )
    {
        D((DL_FATERROR, "++++ Call dir_namei() return NULL! ++++\n"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    // patternlen is 0
    if(0 == patternlen) 
    {
        D((DL_FATERROR, "++++ Call dir_namei() return. ERR_FS_NO_BASENAME! ++++\n"));
        iRet = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "In FS_FindFirstFile malloc dentry failed.\n") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }

    // Copying  string of filename overflow impossibility,because has checked the file path befor.
    DSM_MemSet( psDe, '\0', sizeof(struct dentry) );
    DSM_TcStrCpy( psDe->name, pattern);

    psDe->append_data = FAT_CSO2INO(0, 0, 0); 
    psDe->append_data2 = FS_FLAG_FIND_FILE;
    
    iRet = do_find_entry(&dir_i, psDe, FAT_LOOK_GET_NAME);
    if ( DIR_ENTRY_NOT_FOUND == iRet )
    {
        D((DL_FATWARNING, "+++ do_find_entry() return failed\n"));
        iRet = ERR_FS_NO_MORE_FILES;
        goto step1_failed;
    }

    // Modify by suwg at 2007/7/6
    f->f_pattern = DSM_MAlloc((DSM_UnicodeLen((UINT8*)pattern) + LEN_FOR_NULL_CHAR * SIZEOF(WCHAR)));
    if(NULL == f->f_pattern)
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, f_pattern is NULL" ) );
        DSM_ASSERT(0,"FS_FindFirstFile: 1.malloc(0x%x) failed.",(DSM_UnicodeLen((UINT8*)pattern) + LEN_FOR_NULL_CHAR * SIZEOF(WCHAR))); 
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step2_failed;
    }

    g_fs_current.filp[ fd ] = f;
    f->f_count = 1;
    f->f_inode = dir_i;
    f->f_flags = FS_O_FIND;
    f->f_pos = psDe->append_data;

    DSM_TcStrCpy( f->f_pattern, pattern );     

    pFindData->st_mode = psDe->inode->i_mode;  
    pFindData->st_size = psDe->inode->i_size;
    pFindData->atime = psDe->inode->i_atime;
    pFindData->mtime = psDe->inode->i_mtime;
    pFindData->ctime = psDe->inode->i_ctime;

    iULen = DSM_UnicodeLen((UINT8*)psDe->name);
    if(iULen > FS_PATH_UNICODE_LEN)
    {
        iRet =  ERR_FS_READ_DIR_FAILED;
        goto step2_failed;
    }

    DSM_TcStrCpy((PCWSTR)pFindData->st_name, psDe->name);
    iRet = ERR_SUCCESS;
    CSW_TRACE(BASE_DSM_TS_ID,
                "#fs debug find first  success. fd= %d,DevNo = %d.",
                fd,f->f_inode->i_dev);
step2_failed:
    iput( psDe->inode );

step1_failed:

    //if failuer, free the inode,else free it in FS_FindClose.
    
    if ( ERR_SUCCESS != iRet )
    {
        iput( dir_i );
    }

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
    
step0_failed:

    if (iRet != ERR_SUCCESS && f != NULL)
    {
        if(f->f_pattern != NULL)
        {
            DSM_Free(f->f_pattern);
        }
        fs_fd_list_del(&g_fs_file_list, f);
    }
    
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }

    FS_Up();
     
    return ( ERR_SUCCESS == iRet ) ? fd : iRet;
}


INT32 FS_FindNextFile( INT32 fd, PFS_FIND_DATA pFindData)
{
    struct file * f;
    struct inode * dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet;
    UINT16 iULen = 0;

    if(NULL ==pFindData)
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, parameter error, pFindData is NULL" ) );
        return ERR_FS_INVALID_PARAMETER;
    }

    if(!FD_IS_VALID( fd ))
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, parameter error, fd is invalid,fd = %d",fd ) );
        return ERR_FS_INVALID_PARAMETER;
    }

    f = g_fs_current.filp[fd];
    if(!f || !fs_is_file_opened(&g_fs_file_list, f))
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, f is NULL" ) );
        return ERR_FS_BAD_FD;
    }

    // handle is not for finding
    if(0 == (f->f_flags & FS_O_FIND))
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, handle is not for finding, f->flag = %d",f->f_flags) );
        return ERR_FS_BAD_FD;
    }
    if ( 0 == f->f_pos )
    {
        return ERR_FS_NO_MORE_FILES;
    }

    FS_Down();
    
    dir_i = f->f_inode;
    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        FS_Up();
        return ERR_FS_NO_MORE_MEMORY;
    }
    DSM_MemSet( psDe, '\0', sizeof(struct dentry) );
    DSM_TcStrCpy( psDe->name, f->f_pattern);

    psDe->append_data = f->f_pos;
    psDe->append_data2 = FS_FLAG_FIND_FILE;
    
    iRet = do_find_entry( &dir_i, psDe, FAT_LOOK_GET_NAME );
    if(DIR_ENTRY_FOUND == iRet)
    {
        f->f_pos = psDe->append_data;
        // directory or regular file
        pFindData->st_mode = psDe->inode->i_mode; 
        pFindData->st_size = psDe->inode->i_size;
        pFindData->atime = psDe->inode->i_atime;
        pFindData->mtime = psDe->inode->i_mtime;
        pFindData->ctime = psDe->inode->i_ctime;
        // NameFat2User(psDe.inode->u.fat_i.entry.DIR_Name, pFindData->st_name,0x0);
        // DSM_StrCpy(pFindData->st_name, psDe.name);
        iULen = DSM_UnicodeLen((UINT8*)psDe->name);
        if(iULen > FS_FILE_NAME_UNICODE_LEN)
        {
            DENTRY_FREE(psDe);
            FS_Up();
            return ERR_FS_READ_DIR_FAILED;
        }
        DSM_TcStrCpy((PCWSTR)pFindData->st_name, psDe->name);
        iput( psDe->inode );
        iRet = ERR_SUCCESS;
    }
    else if ( DIR_ENTRY_NOT_FOUND == iRet )
    {       
        iRet = ERR_FS_NO_MORE_FILES;
    }
    else
    {
        iRet = fs_SetErrorCode(iRet);
    }
    DENTRY_FREE(psDe);
    FS_Up();
    return iRet;
}


INT32 FS_FindClose(INT32 fd)
{
    INT32 iRet; 
    struct file * filp;
    UINT32 uDevNo = 0;

    if( !FD_IS_VALID( fd ) ) // fd is invalid
    {
      //  DSM_ASSERT(0,"find_close error1. fd = 0x%x",fd);
        return ERR_FS_BAD_FD; 
    } 
    filp = g_fs_current.filp[ fd ];
    if(!filp)
    {
     //   DSM_ASSERT(0,"find_close error2. fd = 0x%x",fd);
        return ERR_FS_BAD_FD; 
    }
    uDevNo = filp->f_inode->i_dev;
    
    FS_Down();
    
    iRet = fs_findclose(fd);
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
       
    }    
     if(iRet == 0)
     {
         CSW_TRACE(BASE_DSM_TS_ID,
                "#fs debug find close success. fd= %d,uDevNo = %d.",
                fd,uDevNo);
     }
     else
     {
          CSW_TRACE(BASE_DSM_TS_ID,
                "#fs debug find close failed. fd= %d,uDevNo = %d,ret_value = %d.",
                fd,uDevNo,iRet);
          // DSM_ASSERT(0,"find_close error3. fd = 0x%x",fd);
      }
      FS_Up();
    
    return iRet;  
}

//---------------------------------------------------------------------------------------------------------------------
// FS_GetFileAttr
// This funciton to get the file or directory attribute .
//---------------------------------------------------------------------------------------------------------------------
// @pszFileName[in]
// File name,the string format is unicode.
// @pFileAttr[out]
// Output the attributes of the specified file or director.
// Return
// Suceeds return ERR_SUCCESS,else return flowing error code:
// ERR_FS_INVALID_PARAMETER: Invalid parameter.
// ERR_FS_FILE_NOT_EXIST:The file is not exist.
//---------------------------------------------------------------------------------------------------------------------
INT32 FS_GetFileAttr( PCSTR pszFileName, FS_FILE_ATTR* pFileAttr)
{
    BOOL bResult;
    INT32 iRet = ERR_SUCCESS;    
    struct inode *inode = NULL;    
    UINT16 iULen = 0;
    
    if( !pszFileName || !pFileAttr ) // hFile is invalid
    {    
        return ERR_FS_INVALID_PARAMETER;
    }
    
    iULen = DSM_UnicodeLen((UINT8*)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    bResult = is_file_name_valid( (PCWSTR)pszFileName, TRUE );
    if(!bResult)
    {        
        D((DL_FATWARNING, "Invalid file name"));
        return ERR_FS_INVALID_PARAMETER;
    }
    
    FS_Down();
    inode = namei((PCWSTR) pszFileName );
    if(!inode)
    {
        D( ( DL_FATERROR, "directory entry does not exist" ) );        
        iRet = ERR_FS_FILE_NOT_EXIST;
    }
    else
    {
        pFileAttr->i_no = inode->i_ino;
        pFileAttr->i_mode = inode->i_mode;
        pFileAttr->i_size = inode->i_size;
        pFileAttr->i_count = (inode->i_count - 1);
        pFileAttr->i_atime = inode->i_atime;
        pFileAttr->i_mtime = inode->i_mtime;
        pFileAttr->i_ctime = inode->i_ctime;
        iput( inode );
    }
    if(inode)
    {
      if(DSM_GetDevType(inode->i_dev) == DSM_MEM_DEV_TFLASH)
      {
        if(!fs_IsOpened(inode->i_dev))
        {
          DRV_DEV_DEACTIVE(inode->i_dev);
        }
      }
    }
    FS_Up();    
    return iRet;
}


//---------------------------------------------------------------------------------------------------------------------
// FS_SetFileAttr
//---------------------------------------------------------------------------------------------------------------------
// @pszFileName
// File name,the string format is unicode.
// @iMode
// File or directory attibute. The following values can be combined by using the bitwise OR operator.
// FS_ATTR_RO:read-only
// FS_ATTR_HIDDEN:hidden 
// FS_ATTR_SYSTEM:system 
// FS_ATTR_ARCHIVE:archived 
// The macro FS_IS_DIR(m) be used to check the dir entry mode if is a directory.Diretory:TRUE;File:FALSE.
// Return
// Sucesse return ERR_SUCCESS,else return flowing error code:
// ERR_FS_INVALID_PARAMETER: Invalid parameter.
// ERR_FS_NO_MORE_MEMORY:No more memory to alloc.
// ERR_FS_PATHNAME_PARSE_FAILED:Parse the path failed.
// ERR_FS_NO_BASENAME:No base file name.
// ERR_FS_FILE_NOT_EXIST:The file is not exist.
//---------------------------------------------------------------------------------------------------------------------
INT32 FS_SetFileAttr(PCSTR pszFileName,UINT32 iMode)
{
    WCHAR *path = NULL;
    WCHAR *pattern = NULL;
    INT32 patternlen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;
    
       
    iULen = DSM_UnicodeLen((UINT8*)pszFileName);    
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    // path name character validate, Ignore base name.
    if( !is_file_name_valid((PCWSTR) pszFileName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }
    
    FS_Down();  
    
    path = format_dir_name((PCWSTR)pszFileName );
    if(NULL == path) // malloc failed
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, path is NULL" ) );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }

    dir_i = dir_namei( path, &patternlen, (CONST WCHAR**)&pattern );
    if(NULL == dir_i ) // path is invalid 
    {
        D((DL_FATERROR, "++++ Call dir_namei() return NULL! ++++\n"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    if(0 == patternlen) // patternlen is 0
    {
        D((DL_FATERROR, "++++ Call dir_namei() return. ERR_FS_NO_BASENAME! ++++\n"));
        iRet = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "In FS_FindFirstFile malloc dentry failed.\n") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }

    DSM_MemSet( psDe, '\0', sizeof(struct dentry) );
    DSM_TcStrCpy( psDe->name, pattern);

    psDe->append_data = FAT_CSO2INO(0, 0, 0); 
    psDe->append_data2 = FS_FLAG_FIND_FILE;
    
    iRet = do_find_entry(&dir_i, psDe, FAT_LOOK_EXISTED);
    if ( DIR_ENTRY_NOT_FOUND == iRet )
    {
        D((DL_FATWARNING, "+++ do_find_entry() return failed\n"));
        iRet = ERR_FS_FILE_NOT_EXIST;
        goto step1_failed;
    }

    psDe->inode->i_mode |= ((iMode & FS_ATTR_RO)  == FS_ATTR_RO) ? FS_ATTR_RO : FS_ATTR_MARK;
    psDe->inode->i_mode |= ((iMode & FS_ATTR_HIDDEN)  == FS_ATTR_HIDDEN) ? FS_ATTR_HIDDEN : FS_ATTR_MARK;
    psDe->inode->i_mode |= ((iMode & FS_ATTR_SYSTEM)  == FS_ATTR_SYSTEM) ? FS_ATTR_SYSTEM : FS_ATTR_MARK;
    psDe->inode->i_mode |= ((iMode & FS_ATTR_ARCHIVE)  == FS_ATTR_ARCHIVE) ? FS_ATTR_ARCHIVE : FS_ATTR_MARK;

    if(psDe->inode->u.fat_i.entry.DIR_Attr != psDe->inode->i_mode)
    {
        psDe->inode->u.fat_i.entry.DIR_Attr = (UINT8)psDe->inode->i_mode;
        psDe->inode->u.fat_i.Dirty = TRUE;
    }

    iput( psDe->inode );
    iRet = ERR_SUCCESS;  

step1_failed:
    
    iput( dir_i );
    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
    
step0_failed:    

    FS_Up();     
    return iRet;
}

//---------------------------------------------------------------------------------------------------------------------
// FS_GetFileAttrByHandle
// This funciton to get the file or directory attribute .
//---------------------------------------------------------------------------------------------------------------------
// @pszFileName[in]
// File name,the string format is unicode.
// @pFileAttr[out]
// Output the attributes of the specified file or director.
// Return
// Suceeds return ERR_SUCCESS,else return flowing error code:
// ERR_FS_INVALID_PARAMETER: Invalid parameter.
// ERR_FS_FILE_NOT_EXIST:The file is not exist.
//---------------------------------------------------------------------------------------------------------------------
INT32 FS_GetFileAttrByHandle(INT32 fd, FS_FILE_ATTR* pFileAttr)
{
    struct file * file;
    struct inode * inode;
    INT32 iLocalLen;
    INT32 iRet = ERR_SUCCESS;

    if( NULL == pFileAttr)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    if ( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_Read, parameter error, fd = %d\n",fd) );
        DSM_ASSERT(0,"read error2. fd = 0x%x.",fd);
        return ERR_FS_BAD_FD;
    }
    
    FS_Down();
    
    file = g_fs_current.filp[ fd ];

    if ( !file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        D( ( DL_FATERROR, "in FS_GetFileAttrByHandle\n") );
        iRet = ERR_FS_BAD_FD;
        DSM_ASSERT(0,"read error3. fd = 0x%x.",fd);
        goto label_exit;
    }

    inode = file->f_inode;

    pFileAttr->i_no = inode->i_ino;
	pFileAttr->i_mode = inode->i_mode;
	pFileAttr->i_size = inode->i_size;
    pFileAttr->i_count = (inode->i_count - 1);
	pFileAttr->i_atime = inode->i_atime;
	pFileAttr->i_mtime = inode->i_mtime;
	pFileAttr->i_ctime = inode->i_ctime;

label_exit:
    FS_Up();    
    return iRet;
}

INT32 FS_GetFSInfo(PCSTR pszDevName,FS_INFO* psFSInfo)
{
    UINT32 uDevNo = 0;
    UINT32 iFatEntry;
    //INT32 iOldFatEntry;
    UINT32 iSizePerClus = 0;
    UINT64 iFreeSize = 0;
    struct super_block* pSB = NULL;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
   

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    FS_Down();
    
    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        FS_Up();
        return ERR_FS_DEVICE_NOT_REGISTER;
    }
    
    iResult = fs_FindSuperBlock(uDevNo,&pSB);
    if(_ERR_FS_SUCCESS == iResult)
    {
        switch(pSB->s_fstype)
        {
            case FS_TYPE_FAT: 
               
                //psFSInfo->iTotalSize = (UINT64)(pSB->u.fat_sb.iDataSec)*(pSB->u.fat_sb.iBytesPerSec); 
           
                iFreeSize = 0;
                iFatEntry = 0;
                // iOldFatEntry = 0;
                iSizePerClus = (pSB->u.fat_sb.iBytesPerSec) * (pSB->u.fat_sb.iSecPerClus);
                psFSInfo->iTotalSize = (UINT64)(pSB->u.fat_sb.iDataSec/pSB->u.fat_sb.iSecPerClus)*iSizePerClus; 
                iResult = fat_stat_free_cluster(&(pSB->u.fat_sb), &iFatEntry);
                if(ERR_SUCCESS != iResult)
                {
                    iRet = fs_SetErrorCode(iResult);
                }
                
                iFreeSize = (UINT64)iFatEntry*iSizePerClus;

               
                psFSInfo->iUsedSize = (psFSInfo->iTotalSize - iFreeSize);
                break;

            case FS_TYPE_FFS:
            default:
                iRet = ERR_FS_UNKNOWN_FILESYSTEM;
                break;
        }
    }
    else
    {
        iRet = fs_SetErrorCode(iResult);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }   
     
    FS_Up();
    return iRet;
}


INT32 FS_GetCurDir(UINT32 uSize, PSTR pCurDir)
{
    INT32 iErrCode = ERR_SUCCESS;
    INT16 uLen;

    FS_Down();
    if (NULL == pCurDir)
    {
        D((DL_FATERROR, "FS_GetCurDir: pCurDir is null"));
        iErrCode = ERR_FS_INVALID_PARAMETER;
        goto _get_cur_dir_end;
    }

    if (NULL == g_fs_current.wdPath)
    {
        iErrCode = ERR_FS_OPERATION_NOT_GRANTED;
        goto _get_cur_dir_end;
    }
    
    uLen = DSM_UnicodeLen((UINT8*)g_fs_current.wdPath);
    if ((UINT32)uLen + LEN_FOR_NULL_CHAR > uSize / SIZEOF(WCHAR))
    {
        D((DL_FATERROR, "FS_GetCurDir: uLen(%d), uSize(%d), ERR_DSS_FS_NAME_BUFFER_TOO_SHORT", uLen, uSize));
        iErrCode = ERR_FS_NAME_BUFFER_TOO_SHORT;
        goto _get_cur_dir_end;
    }

    DSM_TcStrCpy((PCWSTR)pCurDir, g_fs_current.wdPath);
    iErrCode = ERR_SUCCESS;
    
_get_cur_dir_end:
    
    FS_Up();
    return iErrCode;
}


INT32 FS_Rename( PCSTR pszOldname, PCSTR pszNewname )
{
    BOOL bRet;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    WCHAR *basename;
    INT32 namelen;
    struct inode *dir_old, *dir_new;
    struct dentry *psOld_de = NULL, *psDe = NULL;
    INT16 iULen = 0;
    UINT32 uDevNo = 0;
    
    if (NULL == pszOldname || NULL ==pszNewname)
    {
        DSM_ASSERT(0,"rename error1. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }
    
    iULen = DSM_UnicodeLen((UINT8*)pszOldname);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        DSM_ASSERT(0,"rename error2. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }
        
    bRet = is_file_name_valid((PCWSTR) pszOldname, FALSE );
    if(!bRet)
    {        
        D((DL_FATWARNING, "invalid old pathname"));
       // DSM_ASSERT(0,"rename error3. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }

    iULen = DSM_UnicodeLen((UINT8*)pszNewname);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    bRet = is_file_name_valid( (PCWSTR)pszNewname, FALSE );
    if(!bRet)
    {
        D((DL_FATWARNING, "invalid new pathname"));
      //  DSM_ASSERT(0,"rename error4. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    dir_old = dir_namei((PCWSTR) pszOldname, &namelen, (CONST WCHAR**)&basename );
    if(!dir_old)
    {
        D((DL_FATWARNING, "old path is invalid"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        //DSM_ASSERT(0,"rename error5. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step0_failed;
    }
    if(!namelen)
    {
        D((DL_FATWARNING, "file name not specified"));
        iRet = ERR_FS_NO_BASENAME;
       // DSM_ASSERT(0,"rename error6. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step1_failed;
    }

    psOld_de = DENTRY_ALLOC();
    if (NULL == psOld_de)
    {
        D((DL_FATERROR, "FS_Rename: DENTRY_ALLOC psOld_de is NULL.\n"));
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"rename error7. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step1_failed;
    }

    DSM_MemSet( psOld_de, 0, sizeof( *psOld_de ) );
    DSM_TcStrCpy(psOld_de->name, basename);

    uDevNo = dir_old->i_dev;
    iResult = find_entry( &dir_old, psOld_de );
    if(-1 == iResult)
    {
        D((DL_FATWARNING, "file not found"));
        iRet = ERR_FS_NO_DIR_ENTRY;
       // DSM_ASSERT(0,"rename error8. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step1_failed;
    }
    
    // Some process is dealing with the file. It will not be removed.
    if( psOld_de->inode->i_count > 1)
    {
        D((DL_FATWARNING, "Some process is dealing with the file. It will not be renamed."));
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        // DSM_ASSERT(0,"rename error9. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step2_failed;
    }
    
    dir_new = dir_namei((PCWSTR) pszNewname, &namelen, (CONST WCHAR**)&basename );
    if(!dir_new)
    {
        D((DL_FATWARNING, "old path is invalid"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
       // DSM_ASSERT(0,"rename error10. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step2_failed;
    }
    if(!namelen)
    {
        D((DL_FATWARNING, "file name not specified"));
        iRet = ERR_FS_NO_BASENAME;
        //DSM_ASSERT(0,"rename error11. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    if ( FS_IS_DIR( psOld_de->inode->i_mode ) && dir_new != dir_old )
    {
        D((DL_FATWARNING, "Move directory is not permitted"));
        iRet = ERR_FS_RENAME_DIFF_PATH;
        DSM_ASSERT(0,"rename error12. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D((DL_FATERROR, "FS_Rename: DENTRY_ALLOC psDe is NULL.\n"));
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"rename error13. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }
    
    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename);
    
    iResult = find_entry( &dir_new, psDe );
    if(!iResult)
    {
        D((DL_FATWARNING, "file has existed"));
        iput( psDe->inode );
        iRet = ERR_FS_FILE_EXIST;
        //DSM_ASSERT(0,"rename error14. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }
    
    iResult = fat_do_link( psOld_de, dir_new, psDe,psOld_de->inode->i_mode  );
    if(iResult)
    {
        D((DL_FATWARNING, "do link failed"));
        iRet = fs_SetErrorCode(iResult);
        //DSM_ASSERT(0,"rename error15. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }
    
    iResult = fat_do_unlink(dir_old, psOld_de, TRUE );
    if(iResult)
    {
        D((DL_FATWARNING, "do unlink failed"));      
        iRet = fs_SetErrorCode(iResult);
        //DSM_ASSERT(0,"rename error16. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    iRet = ERR_SUCCESS;

step3_failed:
    iput( dir_new );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }

step2_failed:
    iput( psOld_de->inode );
step1_failed:
    iput( dir_old );
    
    if (psOld_de != NULL)
    {
        DENTRY_FREE(psOld_de);
    }
    
step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }
    FS_Up();
    return iRet;
}

//==============================================================
// FS_MountDevice 
//==============================================================


INT32 FS_MountDevice(FS_DEV_TYPE dev_type)
{
	INT32 iRet = 0;
    TCHAR *dev_name;

	INT i = 0;

    if(dev_type == FS_DEV_TYPE_TFLASH)
        dev_name=TEXT("mmc0");
    else if(dev_type == FS_DEV_TYPE_USBDISK)
        dev_name=TEXT("usb0");
    else
        return ERR_INVALID_PARAMETER;

	iRet = FS_HasFormatted(dev_name, FS_TYPE_FAT);
	DSM_HstSendEvent(0x09089010);
	DSM_HstSendEvent(iRet);
        
	if ( iRet ==  ERR_FS_HAS_FORMATED)
    {
		for ( i = 0; i < 3; i++ )
		{

			iRet = FS_MountRoot(dev_name);
			DSM_HstSendEvent(0x09090035);
			DSM_HstSendEvent(iRet);
			if ( iRet == ERR_SUCCESS )
			{
				break;
			}
            else if(iRet == ERR_FS_HAS_MOUNTED)
            {
    			iRet = FS_UnmountRoot();
            }
        }
	}
    return iRet;
}

/* NO semaphore protection */
INT32 FS_MountRoot( PCSTR pszRootDevName )
{    
    INT32 iErrCode = ERR_SUCCESS;
    UINT32 uDevNo = 0;
    UINT32 i;
    struct super_block * sb = NULL;
    struct inode *root_i;
    UINT8 szDevName[16] = {0,};
    
    if ( g_fs_current.root )
    {
        D( ( DL_FATWARNING, "Root file system has mounted" ) );
        return ERR_FS_HAS_MOUNTED;
    }
    
       
    g_fs_current.pwd = NULL;
    g_fs_current.root = NULL;
    DSM_MemSet( g_fs_current.filp, 0x00, (sizeof( struct file * ) ) * g_fs_cfg.uNrOpenFileMax);

    // Clean super block table
    for ( i = 0; i < g_fs_cfg.uNrSuperBlock; i++ ) // FS_NR_SUPER is 1 here.
    {
        g_fs_super_block_table[ i ].s_dev = INVALID_DEVICE_NUMBER;
        //g_fs_super_block_table[ i ].s_lock = 0;   
        g_fs_super_block_table[ i ].s_root = NULL;
        g_fs_super_block_table[ i ].s_mountpoint = NULL;
    }
    
    fs_GetSuperBlockFree(&sb);

    if(NULL == pszRootDevName)
    {
        iErrCode = DSM_GetFsRootDevName(szDevName);
        if(iErrCode != ERR_SUCCESS)
        {
               DSM_ASSERT(0,"FS_MountRoot1:iErrCode = %d",iErrCode);
               return ERR_FS_DEVICE_NOT_REGISTER;
        }
        uDevNo = DSM_DevName2DevNo( szDevName );
    }
    else
    {
        uDevNo = DSM_DevName2DevNo(pszRootDevName );
    }
    
    //uDevNo = DRV_DevName2DevNr( pszRootDevName ? pszRootDevName : szDevName );
    if (INVALID_DEVICE_NUMBER == uDevNo)
    {
        DSM_ASSERT(0,"FS_MountRoot2:iErrCode = %d",iErrCode);
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    if (DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH  )
    {
        iErrCode = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iErrCode )
        {
            D( ( DL_FATERROR, "Mount root filesystem: Open t-flash memory device failed" ) );
             DSM_ASSERT(0,"FS_MountRoot3:iErrCode = %d",iErrCode);
            return ERR_FS_OPEN_DEV_FAILED;
        }
    }
    
    /* We ASSUME that root filesystem is FAT */
    iErrCode = fat_read_super(sb, uDevNo);
    if( ERR_SUCCESS != iErrCode )
    {
        D( ( DL_FATERROR, "Mount root filesystem: failed to read superblock" ) );        
         DSM_ASSERT(0,"FS_MountRoot4:iErrCode = %d",iErrCode);
        goto step0_failed;
    }

    root_i = iget(sb, (UINT32)FS_ROOT_INO,FALSE);
    if( NULL == root_i)
    {
        D( ( DL_FATERROR, "Mount root filesystem: failed to read root inode" ) );
        iErrCode = ERR_FS_MOUNT_READ_ROOT_INODE_FAILED;
         DSM_ASSERT(0,"FS_MountRoot5:iErrCode = %d",iErrCode);
        goto step0_failed;
    }
    
    root_i->i_is_mountpoint = FALSE;
    sb->s_mountpoint = sb->s_root = root_i;
    
    g_fs_current.pwd = root_i;   
    g_fs_current.root = root_i;  

    root_i->i_count += 3;

    DSM_TcStrCpy(g_fs_current.wdPath, (PCWSTR)_L_DSM("/"));   
    
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        DRV_SET_WCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE);
        DRV_SET_RCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE*2);
        if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }
    return ERR_SUCCESS;
    
step0_failed:    
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {       
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }    
    sb->s_dev = INVALID_DEVICE_NUMBER;
    return fs_SetErrorCode(iErrCode);    
	
}


INT32 FS_UnmountRoot(void)
{
    struct inode *mount_point_i, *inode;
    struct super_block * sb;
    INT32 iRet;
    INT32 i;
    UINT8 DevName[ 8 ] = { 0, };
    UINT32 fd;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;
    FAT_CACHE* psFatCache = NULL;
 
    FS_Down();

    mount_point_i = g_fs_current.root;  

    /*
    不能根据mount_point_i->i_is_mountpoint是否为TRUE来判断pszOEM下
    是否已经mount了一个文件系统,但可以根据mount_point_i的i_ino是否是FS_ROOT_INO
    来判断,在FS_Mount()中采用了该方法
    */
    for ( i = 0, sb = g_fs_super_block_table; i < (INT32)g_fs_cfg.uNrSuperBlock; i++, sb++ )
    {
        if ( INVALID_DEVICE_NUMBER != sb->s_dev && sb->s_root == mount_point_i )
        {            
            break;
        }
    }
    if( (INT32)g_fs_cfg.uNrSuperBlock == i ) // super block not found
    {
        D( ( DL_FATERROR, "NO file system mounts" ) );
        iRet = ERR_FS_NOT_FIND_SB;
        goto step1_failed;
    }
    uDevNo = sb->s_dev;
    iRet = DSM_DevNr2Name( sb->s_dev, DevName );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_FATERROR, "Device not found for mountpoint" ) );
        iRet = ERR_FS_INVALID_DEV_NUMBER;
        goto step1_failed;
    }

    if (0) // !bForce 
    {
        for ( inode = sb->inode_chain; inode != NULL; inode = inode->next )
        {
            /*why we skip mount_point_i? because it is surely in inode table:-)*/
            if ( inode->i_count > 0 && inode->i_dev == sb->s_dev && inode != mount_point_i )
            {
                D( ( DL_FATERROR, "File system to unmount is being used" ) );
                iRet = ERR_FS_DEVICE_BUSY;
                goto step1_failed;
            }
        }
    }
    else
    {
        struct file *f;
        for ( fd = 0; fd < g_fs_cfg.uNrOpenFileMax; fd++ )
        {
            f = g_fs_current.filp[ fd ];
            if (!f)
            {
                continue;
            }
            
            inode = f->f_inode;
            if ( inode->i_dev == sb->s_dev )
            {
                if(f->f_flags & FS_O_FIND) // fd is for finding
                {
                    iRet = fs_findclose( fd );
                    if( ERR_SUCCESS != iRet )
                    {
                        D( ( DL_FATERROR, "FindClose(%d) failed[%d]", fd, iRet ) );
                        iRet = fs_SetErrorCode(iRet);
                        goto step1_failed;
                    }
                }
                else
                {
                    iRet = fs_close(fd);
                    if( ERR_SUCCESS != iRet )
                    {
                        D( ( DL_FATERROR, "Close(%d) failed[%d]", fd, iRet ) );
                        iRet = fs_SetErrorCode(iRet);
                        goto step1_failed;
                    }
                }
            }
        }

        // 不管当前的工作目录是否在挂载设备上，都将被换成根设备的根目录？
        // TODO...
        iput( g_fs_current.pwd );
        g_fs_current.pwd = g_fs_current.root;
        g_fs_current.root->i_count++;
        
        psFatCache = fat_get_fat_cache(uDevNo);
        if(psFatCache->iSecNum != 0)
        {
              fat_synch_fat_cache(&(sb->u.fat_sb), psFatCache);
        }
        psFatCache->iDirty = 0;
        psFatCache->iSecNum = 0;
        psFatCache->psFileInodeList = NULL;        
        
    }

    /*How to check mount_point_i is root inode or not?*/
    if( g_fs_current.root != mount_point_i && mount_point_i->i_count != 2 ) // mount point  is busy
    {
        D( ( DL_FATERROR, "root is under reference" ) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto step1_failed;
    }
    else if ( g_fs_current.root != mount_point_i )
    {/* Unmount non-root-FS */
        sb->s_mountpoint->i_is_mountpoint = FALSE;
        sb->s_root->i_count = sb->s_root->i_count > 1 ? 1 : sb->s_root->i_count;
        iput( sb->s_root );
        // force set the mount point i_count to 1, to relese the node.
        sb->s_mountpoint->i_count = 1;
        iput( sb->s_mountpoint );
    }
    else
    {
        /* Unmount root-FS from "/." is permitted */
        sb->s_mountpoint->i_is_mountpoint = FALSE;
        iput( sb->s_root ); //i.e, iput( mount_point_i );
        iput( sb->s_mountpoint );
        iput(g_fs_current.pwd);
        iput(g_fs_current.root);
        
        g_fs_current.pwd = NULL;
        g_fs_current.root = NULL;
        DSM_MemSet( g_fs_current.filp, 0x00, (sizeof( struct file * ) ) * g_fs_cfg.uNrOpenFileMax);
    }

    if (sb->fat_fsi != NULL)
        DSM_Free(sb->fat_fsi);

    DSM_MemSet( sb, 0, sizeof(*sb) );
     sb->s_dev = INVALID_DEVICE_NUMBER;
 
    iRet = ERR_SUCCESS;    
step1_failed:
    if(mount_point_i->i_count >= 1)
    {
        mount_point_i->i_count = 1;
        iput( mount_point_i );
    }
step0_failed:    
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }
    if(iRet == 0)
    {
        CSW_TRACE(BASE_DSM_TS_ID,
        		"#fs debug unmount(%d) success.",
        		uDevNo);
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID,
        		"#fs debug unmount(%d) failed.err_code = %d.",
        		uDevNo,iRet);
    }
    FS_Up();
    return iRet;
}


INT32 FS_Mount( PCSTR pszDevName, PCSTR pszMountPoint, INT32 iFlag, UINT8 iType )
{
    struct inode *mount_point_i, *root_i;
    UINT32 uDevNo = 0;
    struct super_block * sb;
    INT32 iRet = ERR_SUCCESS;
    INT32 iResult = ERR_SUCCESS;
    UINT16 iULen = 0;
    
    //cheat compiler
    iFlag = iFlag; 
    
    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    iULen = DSM_UnicodeLen((UINT8*)pszMountPoint);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }    
    
    uDevNo = DSM_DevName2DevNo( pszDevName );
    if (INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "Unknown device name[%s]", pszDevName ) );
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    // file name character validate.
    if( !is_file_name_valid( (PCWSTR)pszMountPoint, FALSE ))
    {
        D( ( DL_FATERROR, "in FS_Mount , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }
    
    FS_Down();
    
    mount_point_i = namei( (PCWSTR)pszMountPoint );  

    if(!mount_point_i) // mount point not found
    {
        D( ( DL_FATERROR, "Mountpoint does not exist!\n" ) );
        iRet = ERR_FS_INVALID_DIR_ENTRY;
        goto step0_failed;
    }

    if(!FS_IS_DIR( mount_point_i->i_mode ))  // mount point is not directory
    {
        D( ( DL_FATERROR, "Mountpoint MUST be a directory" ) );
        iRet = ERR_FS_NOT_DIRECTORY;
        goto step1_failed;
    }

    if( mount_point_i->i_count != 1 ) // mount point is busy
    {
        D( ( DL_FATERROR, "[%s] is under reference, i_count = %d.\n", pszMountPoint, mount_point_i->i_count) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto step1_failed;
    }

    if( mount_point_i->i_ino == (unsigned int)FS_ROOT_INO )
    {
        D( ( DL_FATERROR, "some fs has mounted on the mountpoint[%s]", pszMountPoint ) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto step1_failed;
    }
    
    sb = get_super( uDevNo );
    if(sb)  // file system has been mounted
    {
        D( ( DL_FATERROR, "file system on the device[%s] HAS mounted", pszDevName ) );
        iRet = ERR_FS_HAS_MOUNTED;
        goto step1_failed;
    }

    for ( sb = g_fs_super_block_table; sb < g_fs_cfg.uNrSuperBlock + g_fs_super_block_table; sb++ )
    {
        if ( INVALID_DEVICE_NUMBER == sb->s_dev )
            break;
    }
    if(sb == g_fs_cfg.uNrSuperBlock + g_fs_super_block_table) // g_fs_super_block_table is full
    {
        D( ( DL_FATERROR, "Too many file systems has mounted" ) );
        iRet = ERR_FS_MOUNTED_FS_MAX;
        goto step1_failed;
    }
        
    switch( iType )
    {
        case FS_TYPE_FAT:
            if ( DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH )
            {                
                iRet = DRV_DEV_ACTIVE(uDevNo);
                if( ERR_SUCCESS != iRet )
                {
                    D( ( DL_FATERROR, "Mount filesystem: Open t-flash memory device failed" ) );
                    iRet =  ERR_FS_OPEN_DEV_FAILED;
                    goto step1_failed;
                }
            }
            
            iResult = fat_read_super( sb, uDevNo );
            if(iResult != ERR_SUCCESS) // read fat super block from device failed
            {
                D( ( DL_FATERROR, "Read the super block for the file system to mount failed" ) );
                iRet = fs_SetErrorCode(iResult);
                goto step2_failed;
            }
            
            break;
        default:  // unknown file system iType
            iRet = ERR_FS_UNKNOWN_FILESYSTEM;
            D((DL_FATERROR, "unknown file system type"));
            goto step1_failed;
            
            break;
    }

    root_i = iget( sb, (UINT32)FS_ROOT_INO,FALSE );
    if(!root_i)  // Unable to read root i-node
    {
        D( ( DL_FATERROR, "Read the root inode for the file system to mount failed" ) );
        iRet = ERR_FS_MOUNT_READ_ROOT_INODE_FAILED;
        goto step2_failed;
    }
    
    mount_point_i->i_is_mountpoint = TRUE;
    
    /*DO NOT increase the reference for the inode, for mount_point_i is a emporary pointer*/
    sb->s_mountpoint = mount_point_i;

    /*DO NOT increase the reference for the inode, for root_i is a emporary pointer*/
    sb->s_root = root_i;

    // NOTE! we don't iput( mount_point_i ), we do that in umount 

    // Setup cache size for mmc drier.    
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       DRV_SET_WCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE);
       DRV_SET_RCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE*4);
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }   
    FS_Up();
    
    // If succeful,not free it until device unmount.
    // else free it befor return.
    return ERR_SUCCESS;
    
step2_failed:    
    sb->s_dev = INVALID_DEVICE_NUMBER;
    
step1_failed:
    iput( mount_point_i );
step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }   
    FS_Up();
    return iRet;
}


INT32 FS_Unmount( PCSTR pszMountPoint, BOOL bForce )
{
    struct inode *mount_point_i, *inode;
    struct super_block * sb;
    INT32 iRet;
    INT32 i;
    UINT8 DevName[ 8 ] = { 0, };
    UINT32 fd;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;
    FAT_CACHE* psFatCache = NULL;
 
    iULen = DSM_UnicodeLen((UINT8*)pszMountPoint);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    // file name character validate.
    if( !is_file_name_valid( (PCWSTR)pszMountPoint, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_Unmount , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }
       
    FS_Down();
    mount_point_i = namei( (PCWSTR)pszMountPoint );  

    if( !mount_point_i )
    {
        D( ( DL_FATERROR, "Mountpoint does not exist" ) );
        iRet = ERR_FS_INVALID_DIR_ENTRY;
        goto step0_failed;
    }
    
    if(!FS_IS_DIR( mount_point_i->i_mode ))  // mount point is not directory
    {
        D( ( DL_FATERROR, "Mountpoint MUST be a directory file" ) );
        iRet = ERR_FS_NOT_DIRECTORY;
        goto step1_failed;
    }    
   
    for ( i = 0, sb = g_fs_super_block_table; i < (INT32)g_fs_cfg.uNrSuperBlock; i++, sb++ )
    {
        if ( INVALID_DEVICE_NUMBER != sb->s_dev && sb->s_root == mount_point_i )
        {            
            break;
        }
    }
    if( (INT32)g_fs_cfg.uNrSuperBlock == i ) // super block not found
    {
        D( ( DL_FATERROR, "NO file system mounts on [%s]", pszMountPoint ) );
        iRet = ERR_FS_NOT_FIND_SB;
        goto step1_failed;
    }
    uDevNo = sb->s_dev;
    iRet = DSM_DevNr2Name( sb->s_dev, DevName );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_FATERROR, "Device not found for mountpoint [%s]", pszMountPoint ) );
        iRet = ERR_FS_INVALID_DEV_NUMBER;
        goto step1_failed;
    }

    if ( !bForce )
    {
        for ( inode = sb->inode_chain; inode != NULL; inode = inode->next )
        {
            /*why we skip mount_point_i? because it is surely in inode table:-)*/
            if ( inode->i_count > 0 && inode->i_dev == sb->s_dev && inode != mount_point_i )
            {
                D( ( DL_FATERROR, "File system to unmount is being used" ) );
                iRet = ERR_FS_DEVICE_BUSY;
                goto step1_failed;
            }
        }
    }
    else
    {
        struct file *f;
        for ( fd = 0; fd < g_fs_cfg.uNrOpenFileMax; fd++ )
        {
            f = g_fs_current.filp[ fd ];
            if (!f)
            {
                continue;
            }
            
            inode = f->f_inode;
            if ( inode->i_dev == sb->s_dev )
            {
                if(f->f_flags & FS_O_FIND) // fd is for finding
                {
                    iRet = fs_findclose( fd );
                    if( ERR_SUCCESS != iRet )
                    {
                        D( ( DL_FATERROR, "FindClose(%d) failed[%d]", fd, iRet ) );
                        iRet = fs_SetErrorCode(iRet);
                        goto step1_failed;
                    }
                }
                else
                {
                    iRet = fs_close(fd);
                    if( ERR_SUCCESS != iRet )
                    {
                        D( ( DL_FATERROR, "Close(%d) failed[%d]", fd, iRet ) );
                        iRet = fs_SetErrorCode(iRet);
                        goto step1_failed;
                    }
                }
            }
        }

        iput( g_fs_current.pwd );
        g_fs_current.pwd = g_fs_current.root;
        g_fs_current.root->i_count++;
        
        psFatCache = fat_get_fat_cache(uDevNo);
        if(psFatCache->iSecNum != 0)
        {
              fat_synch_fat_cache(&(sb->u.fat_sb), psFatCache);
        }
        psFatCache->iDirty = 0;
        psFatCache->iSecNum = 0;
        psFatCache->psFileInodeList = NULL;        
        
    }

    /*How to check mount_point_i is root inode or not?*/
    if( g_fs_current.root != mount_point_i && mount_point_i->i_count != 2 ) // mount point  is busy
    {
        D( ( DL_FATERROR, "[%s] is under reference", pszMountPoint ) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto step1_failed;
    }
    else if ( g_fs_current.root != mount_point_i )
    {/* Unmount non-root-FS */
        sb->s_mountpoint->i_is_mountpoint = FALSE;
        sb->s_root->i_count = sb->s_root->i_count > 1 ? 1 : sb->s_root->i_count;
        iput( sb->s_root );
        // force set the mount point i_count to 1, to relese the node.
        sb->s_mountpoint->i_count = 1;
        iput( sb->s_mountpoint );
    }
    else
    {
        /* Unmount root-FS from "/." is permitted */        
        sb->s_mountpoint->i_is_mountpoint = FALSE;
        iput( sb->s_root ); //i.e, iput( mount_point_i );
        iput( sb->s_mountpoint );
        iput(g_fs_current.pwd);
        iput(g_fs_current.root);
        
        g_fs_current.pwd = NULL;
        g_fs_current.root = NULL;
        DSM_MemSet( g_fs_current.filp, 0x00, (sizeof( struct file * ) ) * g_fs_cfg.uNrOpenFileMax);
    }
    
    DSM_MemSet( sb, 0, sizeof(*sb) );
     sb->s_dev = INVALID_DEVICE_NUMBER;
 
    iRet = ERR_SUCCESS;    
step1_failed:
    if(mount_point_i->i_count >= 1)
    {
        mount_point_i->i_count = 1;
        iput( mount_point_i );
    }
step0_failed:    
    
    if(ERR_SUCCESS == iRet)
    {
        if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
        {
            iRet = DRV_DEV_FLUSH(uDevNo);
            if(ERR_SUCCESS != iRet)
            {
                D( ( DL_FATERROR, " unmount:flush cache error.  return error = 0x%x.",iRet) );
            }       
        }
        CSW_TRACE(BASE_DSM_TS_ID,
                "#fs debug unmount(%d) success.",
                uDevNo);
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID,
                "#fs debug unmount(%d) failed.err_code = %d.",
                uDevNo,iRet);
    }
    FS_Up();
    return iRet;
}


/*
Upon successful completion, TRUE is returned, else FALSE is returned. 
*/
INT32 FS_ChangeDir( PCSTR pszDirName )
{
    WCHAR *path;
    struct inode * dir_i, * inode;
    struct dentry *psDe = NULL;
    WCHAR *basename;
    INT32 namelen;
    INT32 iErrCode;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;

    iULen = DSM_UnicodeLen((UINT8*)pszDirName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    if( !is_file_name_valid( (PCWSTR)pszDirName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, pszDirName is invalid" ) );
        return ERR_FS_INVALID_PARAMETER;
    }

    path = format_dir_name((PCWSTR) pszDirName );
    if(!path)
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, path is invalid" ) );
        return ERR_FS_NO_MORE_MEMORY;
    }

    FS_Down();
    /*namei()--begin*/
    dir_i = dir_namei( path, &namelen, (CONST WCHAR**)&basename );
    if ( !dir_i )
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, dir_i is invalid" ) );
        iErrCode = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }

    if ( !namelen ) // path is invalid
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, namelen = %d",namelen) );
        iErrCode = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, malloc psDe failed!.\n") );
        iErrCode = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }
    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename);

    uDevNo = dir_i->i_dev;
    iErrCode = find_entry( &dir_i, psDe );
    if(iErrCode != 0)
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, find_entry error iErrCode = %d",iErrCode) );
        iErrCode = ERR_FS_NO_DIR_ENTRY;
        goto step1_failed;
    }

    inode = psDe->inode;
    /*namei()--end*/

    if ( !FS_IS_DIR( inode->i_mode ) )
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, inode is not dir") );
        iput( inode );
        iErrCode = ERR_FS_NOT_DIRECTORY;
        goto step1_failed;
    }

    //iErrCode = fs_SetWorkDirPath(g_fs_current.wdPath, (PCWSTR)pszDirName);
    //if (iErrCode != ERR_SUCCESS)
    {
        //D((DL_FATERROR, "FS_ChangeDir: fs_SetWorkDirPath return %d", iErrCode));
		//DSM_HstSendEvent(pszDirName);
        //iput(inode);
        //goto step1_failed;
    }

	if(pszDirName[0] == '.' && pszDirName[2] == '.')
    {   
        if(g_fs_current.pwd->i_ino == FS_ROOT_INO)
        {
            iErrCode = ERR_FS_INVALID_PARAMETER;
            iput(inode);
            goto step1_failed;
        }
		g_list_current_entry = g_fs_current.pwd->i_ino;
    }
	else
		g_list_current_entry = FAT_CSO2INO(0, 0, 0); 
    
    iput( g_fs_current.pwd );
    g_fs_current.pwd = inode;   /*we have add the refcnt for inode in find_entry()*/

    iErrCode = ERR_SUCCESS;
step1_failed:
    iput( dir_i );
    
    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
    
step0_failed:
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }

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



//---------------------------------------------------------------------------------------------------------------------
// FS_Open
//---------------------------------------------------------------------------------------------------------------------
// @pszFileName
// File name,the string format is unicode.
// @iFlag
// Open flag of access permitted.
// Specifies the action to take.The following values can be combined by using the bitwise OR operator.
// FS_O_RDONLY:      Read only.
// FS_O_WRONLY:      Write only.
// FS_O_RDWR:        Read and Write. 
// FS_O_ACCMODE:     Access. 
// FS_O_CREAT:       If the file exists, this flag has no effect except as noted under FS_O_EXCL below. Otherwise, the file shall be created.
// FS_O_EXCL:        If FS_O_CREAT and FS_O_EXCL are set, the function shall fail if the file exists.
// FS_O_TRUNC:       If the file exists, and is a regular file, and the file is successfully opened FS_O_WRONLY or FS_O_RDWR, its length shall be truncated to 0.
// FS_O_APPEND         If set, the file offset shall be set to the end of the file prior to each write.
// @iMode
// File or directory attibute. The following values can be combined by using the bitwise OR operator.If file has existed,the oprate of open  will effect when flag 
// is been set FS_O_CREAT and FS_O_TRUNC.
// FS_ATTR_RO          read-only 
// FS_ATTR_HIDDEN   hidden 
// FS_ATTR_SYSTEM   system 
// FS_ATTR_ARCHIVE archived 
// Return
// Sucesse return ERR_SUCCESS,else return flowing error code:
// ERR_FS_INVALID_PARAMETER: Invalid parameter.
// ERR_FS_NO_MORE_MEMORY:No more memory to alloc.
// Todo
//---------------------------------------------------------------------------------------------------------------------
INT32 FS_Open(PCSTR pszFileName, UINT32 iFlag, UINT32 iMode)
{
    INT32 fd;
    struct inode * dir_i = NULL;
    struct file * f = NULL;
    WCHAR *basename = NULL;
    INT32 namelen;
    struct dentry *psDe = NULL;
    INT32 iErrCode = ERR_SUCCESS;
    INT16 iULen = 0;
    UINT8 iFatMode;

    // change general mode(32bit) to fat mode(8bit).        
    iFatMode= FS_ATTR_MARK;
    iFatMode |= ((iMode & FS_ATTR_RO)  == FS_ATTR_RO) ? FS_ATTR_RO : FS_ATTR_MARK;
    iFatMode |= ((iMode & FS_ATTR_HIDDEN)  == FS_ATTR_HIDDEN) ? FS_ATTR_HIDDEN : FS_ATTR_MARK;
    iFatMode |= ((iMode & FS_ATTR_SYSTEM)  == FS_ATTR_SYSTEM) ? FS_ATTR_SYSTEM : FS_ATTR_MARK;
    iFatMode |= ((iMode & FS_ATTR_ARCHIVE)  == FS_ATTR_ARCHIVE) ? FS_ATTR_ARCHIVE : FS_ATTR_MARK;
    
    iULen = (UINT16)DSM_UnicodeLen((UINT8*)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {        
        D( ( DL_FATERROR, "in FS_Open ,file name length  too long.iULen = %d.",iULen) );       
        DSM_ASSERT(0,"open1. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }
    CSW_TRACE(BASE_DSM_TS_ID, "open file,mode = 0x%x,name_len = %d.",iMode,iULen);  
    //DSM_Dump((UINT8*)pszFileName,iULen + 2,16);
    
    // file name character validate.
    if( !is_file_name_valid((PCWSTR) pszFileName, FALSE ))
    {  
        D( ( DL_FATERROR, "in FS_Open ,file name is invalid.") ); 
        //DSM_ASSERT(0,"open2. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        return  ERR_FS_INVALID_PARAMETER;
    }

    // To truncate the file, we should have the write access to the file
    if ( ( iFlag & FS_O_TRUNC ) && ( ( iFlag & FS_O_ACCMODE ) == FS_O_RDONLY ) )
    {    
        D((DL_FATERROR, "FS_Open: ERR_FS_INVALID_PARAMETER1,iFlag = 0x%x.\n",iFlag));
        DSM_ASSERT(0,"open3. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    // To append the file, we should have the write access to the file
    if ( ( iFlag & FS_O_APPEND ) && ( ( iFlag & FS_O_ACCMODE ) == FS_O_RDONLY ) )
    {
        D((DL_FATERROR, "FS_Open: ERR_FS_INVALID_PARAMETER2,iFlag = 0x%x.\n",iFlag));
        DSM_ASSERT(0,"open4. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    // The low byte greate less 3.
    if((iFlag & 0xf) > FS_O_ACCMODE )
    {
        D((DL_FATERROR, "FS_Open: ERR_FS_INVALID_PARAMETER3.iFlag = 0x%x.\n",iFlag));
        DSM_ASSERT(0,"open5. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();
    
    // Get a valid file descriptor
    for ( fd = 0; fd < (INT32)g_fs_cfg.uNrOpenFileMax; fd++ )
    {
        if ( NULL == g_fs_current.filp[ fd ] )                 /*unused fhd found*/
            break;
    }

    if(fd == (INT32)g_fs_cfg.uNrOpenFileMax)
    {
        D((DL_FATWARNING, "FS_Open: FS_NR_OPEN_FILES_MAX.\n"));
        iErrCode = ERR_FS_FDS_MAX;
        DSM_ASSERT(0,"open6. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        goto step0_failed;
    }

    f = fs_alloc_fd(); 
    if (NULL == f)
    {
        D( ( DL_FATERROR, "in FS_Open, fs_alloc_fd() return NULL") );
        iErrCode = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"open7. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        goto step0_failed;
    }

    f = fs_fd_list_add(&g_fs_file_list, f); 
    dir_i = dir_namei((PCWSTR) pszFileName, &namelen, (CONST WCHAR**)&basename );
    if ( !dir_i )
    {
        D((DL_FATERROR, "FS_Open: ERR_FS_PATHNAME_PARSE_FAILED.\n"));
        iErrCode = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    if(0 == namelen)
    {
        iErrCode = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_Open, malloc psDe failed!.\n") );
        iErrCode = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }

    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename );

    if ( 0 == find_entry( &dir_i, psDe ) )
    {
        D( ( DL_FATDETAIL, "in FS_Open, --->psDe.inode.i_ino = %d\n",psDe->inode->i_ino) );
        
        if(( iFlag & FS_O_CREAT ) && ( iFlag &FS_O_EXCL )) 
        {
            D( ( DL_FATERROR, "in FS_Open the file already existed, and flag is FS_O_CREAT") );
            iErrCode = ERR_FS_FILE_EXIST;
            goto step2_failed;
        }

        if ( FS_IS_DIR( psDe->inode->i_mode ) ) // Attempt to open a directroy file
        {
            D( ( DL_FATERROR, "in FS_Open, the inode is not a file,but a dir\n") );
            iErrCode = ERR_FS_IS_DIRECTORY;
            goto step2_failed;
        }
        
        if ( iFlag & FS_O_TRUNC )
        {
           switch ( dir_i->i_fstype )
           {
            case FS_TYPE_FAT:
                if(!(iFlag & FS_O_CREAT))
                {
                    iFatMode = (UINT8)psDe->inode->i_mode;
                }
                iErrCode = fat_trunc_file( dir_i, psDe, iFatMode );
                if(ERR_SUCCESS != iErrCode) // create file failed
                {
                    D( ( DL_FATERROR, "in FS_Open, trunc file error, ErrorCode = %d\n",iErrCode) );
                    //lErrCode = ERR_FS_TRUNC_FILE_FAILED;
                    goto step2_failed;
                }
            break;

        default:
                D( ( DL_FATERROR, "in FS_Open, unknown file system type_1\n") );
                iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
                goto step2_failed;
                break;
            }
        }    
    }
    else
    {
        // entry not found        
        if ( !( iFlag & FS_O_CREAT ) ) 
        {
            // FS_O_CREAT flag not setting.
            D( ( DL_FATERROR, "in FS_Open the file not existed!") );
            iErrCode = ERR_FS_NO_DIR_ENTRY;
            goto step1_failed;
        }
        
        // FS_O_CREAT flag is set
        switch ( dir_i->i_fstype )
        {
        case FS_TYPE_FFS:
            break;
        case FS_TYPE_FAT:             
            iErrCode = fat_create_file( dir_i, psDe, iFatMode);        
            if(_ERR_FAT_SUCCESS != iErrCode)
            {            
                D(( DL_FATERROR,"in FS_Open() fat_create_file() failed,lErrCode = %d.\n",iErrCode)); 
                goto step1_failed;
            }
            break;
        // unknown fs type
        default: 
            iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
            D((DL_FATERROR, "in FS_Open, unknown file system type."));
            goto step1_failed;
            break;
        }        
    }          

   
    g_fs_current.filp[ fd ] = f;
    f->f_mode = psDe->inode->i_mode;
    f->f_flags = ( UINT16 ) iFlag;
    f->f_count = 1;
    f->f_inode = psDe->inode;
    /* Whether FS_O_APPEND flag is specified or not, we always set file pointer to 0,
         because we may read the file before the first write. */
    f->f_pos = 0;

    iput( dir_i );
    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }    
    
  //  CSW_TRACE(BASE_DSM_TS_ID,
   //             "#fs debug open success = %d",
  //              fd);
    FS_Up();
    CSW_TRACE(BASE_DSM_TS_ID, "open hd = 0x%x",fd);  
    return fd;
    
step2_failed:
    iput( psDe->inode );
    
step1_failed:
    iput( dir_i );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
    
step0_failed:
    if (f != NULL)
    {
        fs_fd_list_del(&g_fs_file_list, f);
    }
    FS_Up();
    if(iErrCode < 0)
    {
        // D( ( DL_FATERROR, "in FS_Open, error,iErrCode = %d.\n",iErrCode) );
    }
    return fs_SetErrorCode(iErrCode);
}


INT32 FS_Create(PCSTR pszFileName, UINT32 iMode)
{
    return FS_Open( pszFileName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, iMode );
}

INT32 FS_Close(INT32 fd)
{
    INT32 iRet; 
    struct file * filp;
    UINT32 uDevNo = 0;

    if( !FD_IS_VALID( fd ) ) // fd is invalid
    {
        return ERR_FS_BAD_FD; 
    } 
    filp = g_fs_current.filp[ fd ];
    if(!filp)
    {
        return ERR_FS_BAD_FD; 
    }
    uDevNo = filp->f_inode->i_dev;
    
    FS_Down();    
    iRet = fs_close(fd);
    if(ERR_SUCCESS == iRet && DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        iRet = DRV_DEV_FLUSH(uDevNo);
        if(ERR_SUCCESS != iRet)
        {
         D( ( DL_FATERROR, " cache:FS_Close flush cache error.  return error = 0x%x.",iRet) );
        }       
     }
     
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {       
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }          
   FS_Up();
   CSW_TRACE(BASE_DSM_TS_ID, "close hd = 0x%x",fd);  
    return iRet;    
}

INT32 FS_Delete(PCSTR pszFileName)
{
    WCHAR* basename;
    INT32 namelen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet;
    INT32 iErrCode = ERR_SUCCESS;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;
   
    iULen = (UINT16)DSM_UnicodeLen((UINT8*)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        DSM_ASSERT(0,"delete file1,pszFilename = 0x%x,",pszFileName);         
        return ERR_FS_INVALID_PARAMETER;
    }
    
    CSW_TRACE(BASE_DSM_TS_ID, "delete file:");  
    //DSM_Dump((UINT8*)pszFileName,iULen + 2,16);
    
    if(!is_file_name_valid( (PCWSTR)pszFileName, FALSE )) // invalid pathname
    {
        D( ( DL_FATERROR, "in FS_Delete, FileName is invalid\n") );
   //     DSM_ASSERT(0,"delete file2,pszFilename = 0x%x,",pszFileName);        
        return ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();
    
    dir_i = dir_namei( (PCWSTR)pszFileName, &namelen, (CONST WCHAR**)&basename );
    if( !dir_i ) // path is invalid
    {
        D( ( DL_FATERROR, "in FS_Delete, dir is invalid\n") );
        iErrCode = ERR_FS_PATHNAME_PARSE_FAILED;
        // DSM_ASSERT(0,"delete file3,pszFilename = 0x%x,",pszFileName);        
        goto step0_failed;
    }
    
    if(!namelen) // file name not specified
    {
        D( ( DL_FATERROR, "in FS_Delete, namelen = %d\n",namelen) );
        iErrCode = ERR_FS_NO_BASENAME;
        // DSM_ASSERT(0,"delete file4,pszFilename = 0x%x,",pszFileName);        
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_Delete, malloc psDe failed!.\n") );
        iErrCode = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"delete file5,pszFilename = 0x%x,",pszFileName);        
        goto step1_failed;
    }
    
    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename );

    uDevNo = dir_i->i_dev; 
    iRet = find_entry( &dir_i, psDe );
    if(-1 == iRet)
    {
        D( ( DL_FATERROR, "in FS_Delete, find_entry return -1, find no entry\n") );
        iErrCode = ERR_FS_NO_DIR_ENTRY;
       // DSM_ASSERT(0,"delete file6,pszFilename = 0x%x,",pszFileName);        
        goto step1_failed;
    }

    if(FS_IS_DIR( psDe->inode->i_mode )) // entry is directory
    {
        D( ( DL_FATERROR, "in FS_Delete, entry is directoryy\n") );
        iErrCode = ERR_FS_IS_DIRECTORY;
        DSM_ASSERT(0,"delete file7,pszFilename = 0x%x,",pszFileName);        
        goto step2_failed;
    }

    if(psDe->inode->i_dev != dir_i->i_dev) // Not permition
    {
        D( ( DL_FATERROR, "in FS_Delete, psDe.inode->i_dev = %d, dir_i->i_dev = %d\n",psDe->inode->i_dev,dir_i->i_dev) );
        iErrCode = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"delete file8,pszFilename = 0x%x,",pszFileName);        
        goto step2_failed;
    }

    // Some process is dealing with the file. It will not be removed.
    if(psDe->inode->i_count > 1)
    {
        D( ( DL_FATERROR, "in FS_Delete, psDe.inode->i_count = %d\n",psDe->inode->i_count) );
        iErrCode = ERR_FS_OPERATION_NOT_GRANTED;
        // DSM_ASSERT(0,"delete file9,pszFilename = 0x%x,",pszFileName);        
        goto step2_failed;
    }
        
    if ( FS_TYPE_FAT == dir_i->i_fstype )
    {
        iRet = fat_do_unlink( dir_i, psDe, FALSE );
        if(ERR_SUCCESS != iRet) // unlink failed.
        {
            iErrCode = fs_SetErrorCode(iRet);
            //DSM_ASSERT(0,"delete file10,pszFilename = 0x%x,",pszFileName);        
            goto step2_failed;
        }
    }
    else  // unknown filesystem type
    {
        D( ( DL_FATERROR, "in FS_Delete, unknown file system type\n") );
        iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
        DSM_ASSERT(0,"delete file11,pszFilename = 0x%x,",pszFileName);        
        goto step2_failed;
    }
        
    dir_i->i_mtime = DSM_GetFileTime();
    
    iErrCode = ERR_SUCCESS;

step2_failed:
    iput( psDe->inode );
step1_failed:
    iput( dir_i );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }  
    FS_Up();
    return iErrCode;
}


INT32 FS_Seek(INT32 fd, INT32 iOffset, UINT8 iOrigin)
{
    struct file * file;
    INT32 tmp = 0;
    INT32 iRet = ERR_SUCCESS;

     // DSM_ASSERT(fd >= 0);
    if ( !FD_IS_VALID( fd ) )
    {        
        D( ( DL_FATERROR, "in FS_Seek, invalide fd = %d\n",fd) );    
        // DSM_ASSERT(0,"seek error1. fd = 0x%x",fd);
        return ERR_FS_BAD_FD;
    }

 //   FS_Down();
    
    file = g_fs_current.filp[ fd ];

    if ( !( file ) || !fs_is_file_opened(&g_fs_file_list, file) || !( file->f_inode ) )
    {
        D( ( DL_FATERROR, "in FS_Seek, file is invalid\n") );
        // DSM_ASSERT(0,"seek error2. fd = 0x%x",fd);
        return ERR_FS_INVALID_PARAMETER;
    }

    if ( file->f_flags & FS_O_FIND ) // handle is for finding
    {
        D( ( DL_FATERROR, "in FS_Seek, file->f_flags is for finding, = %d\n",file->f_flags) );
        // DSM_ASSERT(0,"seek error3. fd = 0x%x",fd);
        return ERR_FS_OPERATION_NOT_GRANTED;
    }

    switch ( iOrigin )
    {
        case FS_SEEK_SET:
            tmp = 0 + iOffset;
            file->f_pos = tmp < 0 ? 0 : (tmp > (INT32)file->f_inode->i_size ? (INT32)file->f_inode->i_size: tmp);
            iRet = file->f_pos;
            break;
            
        case FS_SEEK_CUR:
            tmp = file->f_pos + iOffset;
            file->f_pos = tmp < 0 ? 0 : (tmp > (INT32)file->f_inode->i_size ? (INT32)file->f_inode->i_size: tmp);
            iRet = file->f_pos;
            break;
            
        case FS_SEEK_END:
            tmp = file->f_inode->i_size + iOffset;
            file->f_pos = tmp < 0 ? 0 : (tmp > (INT32)file->f_inode->i_size ? (INT32)file->f_inode->i_size: tmp);
            iRet = file->f_pos;
            break;
            
        default:
            iRet = ERR_FS_INVALID_PARAMETER;
    }

 //   FS_Up();
    return iRet;    
}


INT32 FS_IsEndOfFile(INT32 fd)
{
    struct file * filp;
    INT32 iRet;

    if(!FD_IS_VALID( fd ))
    {
        D( ( DL_FATERROR, "in FS_IsEndOfFile, fd = %d, is invalid\n",fd) );
        // DSM_ASSERT(0,"is_end_of_file error1. fd = 0x%x",fd);
        return ERR_FS_BAD_FD;
    }
    
    FS_Down();
    
    filp = g_fs_current.filp[ fd ];
    if(!filp || !fs_is_file_opened(&g_fs_file_list, filp)) // fd has closed
    {
        iRet = ERR_FS_BAD_FD;
        // DSM_ASSERT(0,"is_end_of_file error2. fd = 0x%x",fd);
        goto label_exit;
    }
    if ( filp->f_flags & FS_O_FIND ) // handle is for finding
    {
        D( ( DL_FATERROR, "in FS_IsEndOfFile, filp->f_flags = %d\n",filp->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto label_exit;
    }

    iRet = (( filp->f_inode->i_size == (UINT32)filp->f_pos ) ? 1 : 0);

label_exit:

    FS_Up();
    return iRet;    
}


INT32 FS_Read(INT32 fd, UINT8 *pBuf, UINT32 iLen)
{
    struct file * file;
    struct inode * inode;
    INT32 iLocalLen;
    INT32 iResult = 0;
    INT32 iRet;
   // UINT32 tst_begin_time;
   // UINT32 tst_read_time;

    //tst_begin_time = hal_TimGetUpTime();

    if( NULL == pBuf || iLen < 0)
    {
        D( ( DL_FATERROR, "in FS_Read, parameter error, iLen = %d\n",iLen) );
        DSM_ASSERT(0,"read error1. pBuf = 0x%x,iLen = 0x%x.",pBuf,iLen);
        return ERR_FS_INVALID_PARAMETER;
    }
    
    if ( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_Read, parameter error, fd = %d\n",fd) );
        // DSM_ASSERT(0,"read error2. fd = 0x%x.",fd);
        return ERR_FS_BAD_FD;
    }
    
    FS_Down();
    
    file = g_fs_current.filp[ fd ];

    if ( iLen < 0 || !file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        D( ( DL_FATERROR, "in FS_Read, iLen = %d\n",iLen) );
        iRet = ERR_FS_BAD_FD;
        // DSM_ASSERT(0,"read error3. fd = 0x%x.",fd);
        goto label_exit;
    }
    
    iLocalLen = iLen;

    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        D( ( DL_FATERROR, "in FS_Read, 0_file->f_flags = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"read error4. fd = 0x%x.",fd);
        goto label_exit;
    }
    if (FS_O_WRONLY == ( file->f_flags & FS_O_ACCMODE ) )
    {
        D( ( DL_FATERROR, "in FS_Read, 1_file->f_flags = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"read error5. fd = 0x%x.",fd);
        goto label_exit;
    }

    if ( 0 == iLocalLen )
    {
        iRet = 0;
        goto label_exit;
    }
    
    inode = file->f_inode;

    if (FS_IS_DIR( inode->i_mode ) )
    {
        D( ( DL_FATERROR, "in FS_Read, not regular file\n") );
        iRet = ERR_FS_NOT_REGULAR;
        DSM_ASSERT(0,"read error6. fd = 0x%x.",fd);
        goto label_exit;
    }

   if ( iLocalLen + file->f_pos > inode->i_size )
    {
        iLocalLen = inode->i_size - file->f_pos;
    }
    
    if ( iLocalLen <= 0 )/*after seek opteration, len may be negative*/
    {
        iRet = 0;
        goto label_exit;
    }

    if ( FS_TYPE_FAT == inode->i_fstype )
    {
        iResult = fat_file_read( inode, file, (INT8*)pBuf, iLocalLen);
        if(0 <= iResult)
        {
            iRet = iResult;
        }
        else
        {
           D( ( DL_FATERROR, "Call fat_file_read() failed. Local error code:%d.\n", iResult) );
           iRet = fs_SetErrorCode(iResult);     
           //DSM_ASSERT(0,"read error7. fd = 0x%x,iResult = %d.",fd,iResult);
           goto label_exit;
        }
        
    }
    else
    {
        D( ( DL_FATERROR, "in FS_Read, unknown file system\n") );
        iRet = ERR_FS_UNKNOWN_FILESYSTEM;
        DSM_ASSERT(0,"read error8. fd = 0x%x,fstype = 0x%x.",fd,inode->i_fstype);
        goto label_exit;
    }     
    
label_exit:
    FS_Up();
    //tst_read_time = hal_TimGetUpTime() - tst_begin_time; 
    //CSW_TRACE(BASE_DSM_TS_ID, "read hd = 0x%x,size = 0x%x,time = 0x%x.",fd,iLen,tst_read_time);
    //DSM_Dump(pBuf, iLen, 16);
    return iRet;
}


INT32 FS_Write(INT32 fd, UINT8 *pBuf, UINT32 iLen)
{
    struct file * file;
    struct inode * inode;
    INT32 iResult;
    INT32 iRet;
    
    if( NULL == pBuf || iLen < 0)
    {
        D( ( DL_FATERROR, "in FS_Write, invalid parameter\n") );
        DSM_ASSERT(0,"write error1. pBuf = 0x%x,iLen = 0x%x.",pBuf,iLen);
        return ERR_FS_INVALID_PARAMETER;
    }
    //CSW_TRACE(BASE_DSM_TS_ID, "write hd = 0x%x,size = 0x%x.",fd,iLen);
    //DSM_Dump(pBuf, iLen, 16);
    if( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_Write, invalid fd = %d\n",fd) );
        // DSM_ASSERT(0,"write error2. fd = 0x%x.",fd);
         return ERR_FS_BAD_FD;
    }
    
    FS_Down();
    
    file = g_fs_current.filp[ fd ];
    
    if (NULL == file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        D( ( DL_FATERROR, "in FS_Write, file is NULL.\n") );
        iRet = ERR_FS_BAD_FD;
        // DSM_ASSERT(0,"write error3. fd = 0x%x.",fd);
        goto label_exit;
    }
    
    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        D( ( DL_FATERROR, "0_in FS_Write,file->f_flags  = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"write error4. fd = 0x%x.",fd);
        goto label_exit;
    }
    
    if (FS_O_RDONLY == ( file->f_flags & FS_O_ACCMODE ) )
    {
        D( ( DL_FATERROR, "1_in FS_Write,file->f_flags  = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"write error5. fd = 0x%x.",fd);
        goto label_exit;
    }
    
    if ( 0 == iLen )
    {
        iRet = 0;
        goto label_exit;
    }

    inode = file->f_inode;
    
    if (FS_IS_DIR( inode->i_mode ) )
    {
        D( ( DL_FATERROR, "1_in FS_Write,file is not regular\n") );
        iRet = ERR_FS_NOT_REGULAR;
        DSM_ASSERT(0,"write error6. fd = 0x%x.",fd);
        goto label_exit;
    }
    
    if ( FS_TYPE_FAT == inode->i_fstype )
    {
        iResult = fat_file_write( inode, file, (INT8*)pBuf, iLen );
        if(_ERR_FAT_SUCCESS == iResult)        
        {
            iRet = iLen;
        }
        else
        {
            D( ( DL_FATERROR, "1_in FS_Write,fat_file_write error\n") );            
            iRet = fs_SetErrorCode(iResult);            
            // DSM_ASSERT(0,"write error7. fd = 0x%x,iResult = %d.",fd,iResult);
        }
    }
    else
    {
        D( ( DL_FATERROR, "1_in FS_Write,unknow filesystem\n") );
        DSM_ASSERT(0,"write error8. fd = 0x%x,fstype = 0x%x.",fd,inode->i_fstype);
        iRet = ERR_FS_UNKNOWN_FILESYSTEM;
    }      

label_exit:
    FS_Up();
    return iRet;
}


INT32 FS_MakeDir(PCSTR pszDirName, UINT32 iMode)
{
    WCHAR *path = NULL;
    WCHAR *basename;
    INT32 namelen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet;
    INT32 iErrCode = ERR_SUCCESS;
    UINT16 iULen = 0;
    INT32  uDevNo = 0;
    UINT8 iFileMode;
    
    iFileMode= FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_RO)  == FS_ATTR_RO) ? FS_ATTR_RO : FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_HIDDEN)  == FS_ATTR_HIDDEN) ? FS_ATTR_HIDDEN : FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_SYSTEM)  == FS_ATTR_SYSTEM) ? FS_ATTR_SYSTEM : FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_ARCHIVE)  == FS_ATTR_ARCHIVE) ? FS_ATTR_ARCHIVE : FS_ATTR_MARK;
    iFileMode |= FS_ATTR_DIR;
    
    iULen = (UINT16)DSM_UnicodeLen((UINT8*)pszDirName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        //DSM_Dump((UINT8*)pszDirName,iULen,16); 
        DSM_ASSERT(0,"len error. len= 0x%x,pszDirName = 0x%x",iULen,pszDirName);
        return ERR_FS_INVALID_PARAMETER;
    }

    CSW_TRACE(BASE_DSM_TS_ID, "make dir,mode = 0x%x filename(0x%x):",iMode,iULen);      
    
    // file name character validate.
    if(!is_file_name_valid( (PCWSTR)pszDirName, FALSE ))
    {
        D( ( DL_FATERROR, "in FS_MakeDir,DirName is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);        
      //  DSM_ASSERT(0,"file name is invalid.pszDirName = 0x%x",pszDirName);
        return ERR_FS_INVALID_PARAMETER;
    }
    
    path = format_dir_name( (PCWSTR)pszDirName );
    if(!path)
    {
        D( ( DL_FATERROR, "in FS_MakeDir, format_dir_name is NULL\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        DSM_ASSERT(0,"path is null.pszDirName = 0x%x,iULen = %d.",pszDirName,iULen);
        return ERR_FS_NO_MORE_MEMORY;
    }

    FS_Down();
    
    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_MakeDir(), malloc psDe failed!.\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"psDe is null,malloc failed.pszDirName = 0x%x",pszDirName);
        goto step0_failed;
    }
    
    DSM_MemSet(psDe, 0x00, sizeof(struct dentry));
    dir_i = dir_namei( path, &namelen, (CONST WCHAR**)&basename );
    if ( !dir_i ) // path is invalid
    {
        DENTRY_FREE(psDe);
        psDe = NULL;
        D( ( DL_FATERROR, "in FS_MakeDir,dir_i is NULL\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_PATHNAME_PARSE_FAILED;       
        DSM_ASSERT(0,"dir_i is null.pszDirName = 0x%x",pszDirName);
        goto step0_failed;
    }
    if(0 == namelen) // namelen is 0
    {
        D( ( DL_FATERROR, "in FS_MakeDir,namelen = %d\n",namelen) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_NO_BASENAME;
        DSM_ASSERT(0,"namelen = 0.pszDirName = 0x%x",pszDirName);
        goto step1_failed;
    }

    DSM_TcStrCpy( psDe->name, basename);

    uDevNo = dir_i->i_dev; 
    iRet = find_entry( &dir_i, psDe );
    if(!iRet)
    {
        // D( ( DL_FATERROR, "in FS_MakeDir,dentry is exist. iRet = %d.\n",iRet) );
        // DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_FILE_EXIST;
        goto step1_failed;
    }
    
    if ( FS_TYPE_FFS == dir_i->i_fstype )
    {
        D( ( DL_FATERROR, "in FS_MakeDir,unknown filesystem, i_fstype = %d.\n",dir_i->i_fstype) );
        // DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
        goto step1_failed;
    }
    else if ( FS_TYPE_FAT == dir_i->i_fstype )
    {
        iRet = fat_create_file( dir_i, psDe, iFileMode);
        if(iRet)
        {    
            D( ( DL_FATERROR, "in FS_MakeDir,fat_create_file error, iRet = %d\n",iRet) );
            //DSM_Dump((UINT8*)pszDirName,iULen,16);
              iErrCode = fs_SetErrorCode(iRet);            
            goto step1_failed;
        }
    }
    else
    {
        D( ( DL_FATERROR, "in FS_MakeDir,unknown file system,fstype = 0x%x.\n",dir_i->i_fstype) );
        // DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
        goto step1_failed;
    }

    iErrCode = ERR_SUCCESS;

step1_failed:
    if ( psDe->inode )
    {
        iput( psDe->inode );
    }
    iput( dir_i );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
    
step0_failed:
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }
    
    FS_Up();
    return iErrCode;    
}


INT32 FS_RemoveDir(PCSTR pszDirName)
{
    WCHAR *path;
    WCHAR * basename;
    INT32 namelen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet;
    INT32 iErrCode = ERR_SUCCESS;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;

    CSW_TRACE(BASE_DSM_TS_ID, "remove dir:");  
    iULen = (UINT16)DSM_UnicodeLen((UINT8*)pszDirName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        D( ( DL_FATERROR, "remove dir: len error.len = 0x%x,\n",iULen) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);        
        DSM_ASSERT(0,"file name is invalid.pszDirName = 0x%x",pszDirName);        
        return ERR_FS_INVALID_PARAMETER;
    }
    
    //DSM_Dump((UINT8*)pszDirName,iULen,16);
    if(!is_file_name_valid( (PCWSTR)pszDirName, FALSE ))
    {
        D( ( DL_FATERROR, "remove dir: DirName is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);        
        DSM_ASSERT(0,"remove dir:file name is invalid.pszDirName = 0x%x",pszDirName);        
        return ERR_FS_INVALID_PARAMETER;
    }

    path = format_dir_name( (PCWSTR)pszDirName );
    if(!path)
    {
        D( ( DL_FATERROR, "remove dir: path is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);        
        DSM_ASSERT(0,"remove dir:path is null.pszDirName = 0x%x",pszDirName); 
        return ERR_FS_NO_MORE_MEMORY;
    }
    
    FS_Down();
    
    dir_i = dir_namei( path, &namelen, (CONST WCHAR**)&basename );
    if ( !dir_i ) // path is invalid
    {
        D( ( DL_FATERROR, "remove dir: dir_i is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_PATHNAME_PARSE_FAILED;          
        DSM_ASSERT(0,"remove dir:dir is null.pszDirName = 0x%x",pszDirName); 
        goto step0_failed;
    }
    if(0 == namelen) // namelen is 0
    {
        D( ( DL_FATERROR, "remove dir: namelen = %d\n",namelen) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_NO_BASENAME;
        DSM_ASSERT(0,"remove dir:path is null.pszDirName = 0x%x",pszDirName); 
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "remove dir: malloc psDe failed!.\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"remove dir:psDe is null.pszDirName = 0x%x",pszDirName);         
        goto step1_failed;
    }

    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename);
    
    uDevNo = dir_i->i_dev;
    
    iRet = find_entry( &dir_i, psDe ); // directory not found
    if(-1 == iRet)
    {
        D( ( DL_FATERROR, "remove dir: find_entry error, iRet = %d\n",iRet) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_NO_DIR_ENTRY;
        goto step1_failed;
    }

    if(!FS_IS_DIR( psDe->inode->i_mode )) // Not directory
    {
        D( ( DL_FATERROR, "remove dir: psDe->inode->i_mode = %d\n",psDe->inode->i_mode) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_NOT_DIRECTORY;
        DSM_ASSERT(0,"remove dir:FS_IS_DIR = false.pszDirName = 0x%x",pszDirName);       
        goto step2_failed;
    }
        
    if(psDe->inode->i_dev != dir_i->i_dev)
    {
        D( ( DL_FATERROR, "remove dir: psDe->inode->i_dev = %d, dir_i->i_dev = %d\n",psDe->inode->i_dev,dir_i->i_dev) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"remove dir:dev error.pszDirName = 0x%x,dev1 = %d,dev2 = %d.",pszDirName,psDe->inode->i_dev,dir_i->i_dev);  
        goto step2_failed;
    }
    /*
    After calling find_entry(), its refcnt is increased by one. Its refcnt will be greater than 1 when 
    SOME process is accessing the directory. For example, try to delete itself under the directory.
    NOTE: if we run the command "rd ..", the refcnt of the .. inode may be 1, but it will not be 
    empty!!!
    */
    if(psDe->inode->i_count > 1) // Not permition
    {
        D( ( DL_FATERROR, "remove dir:psDe->inode->i_count = %d\n",psDe->inode->i_count) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_OPERATION_NOT_GRANTED;
        //DSM_ASSERT(0,"remove dir:i_count  > 0.pszDirName = 0x%x,i_count = 0x%x.",pszDirName,psDe->inode->i_count);  
        goto step2_failed;
    }

    if ( FS_TYPE_FFS == dir_i->i_fstype )
    {
        // iRet = ffs_do_unlink( dir_i, psDe, FALSE );
        // if(iRet)
        // {
        //     iErrCode = ERR_FS_FAILURE;
        //     goto step2_failed;
        // }
        D( ( DL_FATERROR, "remove dir:unknow file system,fstype = 0x%x\n",dir_i->i_fstype) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
        DSM_ASSERT(0,"remove dir:fstype unsurpport.pszDirName = 0x%x,fstype = 0x%x.",pszDirName,dir_i->i_fstype);  
        goto step2_failed;
    }
    else if ( FS_TYPE_FAT == dir_i->i_fstype )
    {
        iRet = fat_do_unlink( dir_i, psDe, FALSE );
        if(iRet)
        {
            D( ( DL_FATERROR, "in FS_RemoveDir, unlink failed iRet = %d\n",iRet) );
            //DSM_Dump((UINT8*)pszDirName,iULen,16);
            iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
            iErrCode = fs_SetErrorCode(iRet);;            
            //DSM_ASSERT(0,"remove dir:unlink failed.pszDirName = 0x%x,iRet = %d.",pszDirName,iRet); 
            goto step2_failed;
        }
    }
    else
    {
        D( ( DL_FATERROR, "0_in FS_RemoveDir, unknown file system\n",iRet) );
        iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
        DSM_ASSERT(0,"remove dir:unknow fstype.pszDirName = 0x%x,fstype = %d.",pszDirName,dir_i->i_fstype); 
        goto step2_failed;
    }
    
    dir_i->i_mtime = DSM_GetFileTime();
    iErrCode = ERR_SUCCESS;

step2_failed:
    iput( psDe->inode );
    
step1_failed:
    iput( dir_i );
    
    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
    
step0_failed:
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }    
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }    
    
    FS_Up();
    return iErrCode;    
}


//Return:
// ERR_SUCCESS
// ERR_FS_INVALID_PARAMETER
// ERR_FS_DEVICE_NOT_REGISTER
// ERR_FS_OPEN_DEV_FAILED
// ERR_FS_NOT_FORMAT
// ERR_FS_NO_MORE_MEMORY
// ERR_FS_UNKNOWN_FILESYSTEM
INT32 FS_ScanDisk(PCSTR pszDevName,UINT8 iFsType)
{    
    UINT32 uDevNo = 0;    
    INT32 iResult;
    INT32 iRet;

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    iRet = ERR_SUCCESS;
    D( ( DL_FATWARNING, "FS_ScanDisk begin,pszDevName = %s, iFsType = %d.\n",
                    pszDevName, iFsType));  
    switch(iFsType)
    {
    case FS_TYPE_FAT:
        uDevNo = DSM_DevName2DevNo(pszDevName);
        if(INVALID_DEVICE_NUMBER == uDevNo)
        {
            D( ( DL_FATERROR, "in FS_ScanDisk,uDevNo = %d\n",uDevNo) );
            D( ( DL_FATERROR, "in FS_ScanDisk, DevName = %s\n",pszDevName) );
            return ERR_FS_DEVICE_NOT_REGISTER;
        }

        if ( DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH )
        {
            iRet = DRV_DEV_ACTIVE(uDevNo);
            if( ERR_SUCCESS != iRet )
            {
                D( ( DL_FATERROR, "Device[%s]: Open t-flash memory device failed.\n", pszDevName ) );
                return ERR_FS_OPEN_DEV_FAILED;
            }            
        }  

        FS_Down();
        D( ( DL_FATDETAIL, "Call fat_scan_disk() with uDevNo = %d.\n", uDevNo));  
        iResult = fat_scan_disk(uDevNo);
        D( ( DL_FATDETAIL, "fat_scan_disk() return, iResult = %d.\n", iResult));  
        FS_Up();
        
        if(_ERR_FAT_SUCCESS == iResult)
        {
            iRet = ERR_SUCCESS;
        }
        else
        {
            iRet = fs_SetErrorCode(iResult);
        }
        break;
        
    case FS_TYPE_FFS:
        
    default:
        D( ( DL_FATERROR, "in FS_ScanDisk, unknown file system\n") );
        iRet = ERR_FS_UNKNOWN_FILESYSTEM;
        break;
    }
        
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }  
    
    return iRet;
    
}


// This  function changes the size of a file.
// Move the file point to end after change the file size.
INT32 FS_ChangeSize(INT32 fd, UINT32 nFileSize)
{
    struct file *file = NULL;
    struct inode *psInode;
    INT32 iRet = ERR_SUCCESS;
    UINT32 uDevNo = 0;
    
    if( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_ChangeSize,fd = %d, ERR_FS_INVALID_PARAMETER",fd) );
        return ERR_FS_BAD_FD;
    }

    FS_Down();
    file = g_fs_current.filp[fd];
    if(NULL == file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        D( ( DL_FATERROR, "in FS_ChangeSize, file is NULL\n") );
        iRet = ERR_FS_BAD_FD;
        goto end;
    }

    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto end;
    }

    if (FS_O_RDONLY == ( file->f_flags & FS_O_ACCMODE ) )
    {
        D( ( DL_FATERROR, "FS_ChangeSize, file->f_flags  = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto end;
    }
    
    psInode = file->f_inode;
    if (nFileSize == psInode->i_size)
    {
        D( ( DL_WARNING, "in FS_ChangeSize, SizeToChange = %d, fileSize = %d", nFileSize, psInode->i_size) );
        iRet = ERR_SUCCESS;
        goto end;
    }
    
    switch ( psInode->i_fstype )
    {
    case FS_TYPE_FFS:
        break;

    case FS_TYPE_FAT:
        if (nFileSize > psInode->i_size)
        {
            iRet = fat_file_extend(psInode, file, nFileSize - psInode->i_size);
            if(iRet != _ERR_FAT_SUCCESS)
            {            
                D(( DL_FATERROR, "FS_ChangeSize: fat_file_extend() failed, iRet = %d.\n", iRet)); 
                goto end;
            }
        }
        else
        {
            iRet = fat_file_truncate( psInode, nFileSize );
            if(iRet != _ERR_FAT_SUCCESS)
            {            
                D(( DL_FATERROR, "FS_ChangeSize: fat_file_truncate() failed, iRet = %d.\n", iRet)); 
                goto end;
            }
            if (file->f_pos > psInode->i_size)
            {
                file->f_pos = psInode->i_size;
            }
        }
        uDevNo = psInode->i_dev;
        iRet = ERR_SUCCESS;
        break;

    default: // unknown fs type
        iRet = ERR_FS_UNKNOWN_FILESYSTEM;
        D( ( DL_FATERROR, "FS_ChangeSize: unknown file system type\n") );
        goto end;
    }
    
end:
    if(iRet < 0)
    {
        iRet = fs_SetErrorCode(iRet);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
       if(!fs_IsOpened(uDevNo))
       {
          DRV_DEV_DEACTIVE(uDevNo);
       }
    }  
    FS_Up();
    return iRet; 

}


INT32 FS_Flush(INT32 fd)
{
    struct file * filp;
    INT32 iRet;

    if( !FD_IS_VALID( fd ) ) // fd is invalid
    {
        return ERR_FS_BAD_FD; 
    }

    FS_Down();
    filp = g_fs_current.filp[ fd ];

    if(!filp || !fs_is_file_opened(&g_fs_file_list, filp)) // fd has closed
    {
        iRet = ERR_FS_BAD_FD;
        goto label_exit;
    }

    if(filp->f_flags & FS_O_FIND) // handle is for finding
    {
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto label_exit;
    }
    
    if(!filp->f_count) // f_count should not be ZERO
    {
        iRet = ERR_FS_BAD_FD;
        goto label_exit;
    }

    switch ( filp->f_inode->i_fstype )
    {
    case FS_TYPE_FFS:
        iRet = ERR_FS_NOT_SUPPORT;
        goto label_exit;        
        break;

    case FS_TYPE_FAT:       
        iRet = fat_write_inode( filp->f_inode, FALSE );
        if(iRet != _ERR_FAT_SUCCESS)
        {            
            D(( DL_FATERROR, "FS_Flush: fat_write_inode() failed, iRet = %d.\n", iRet)); 
            goto label_exit;
        }
        else
        {
            iRet = ERR_SUCCESS;
        }
        break;

    default: // unknown fs type
        iRet = ERR_FS_UNKNOWN_FILESYSTEM;
        D( ( DL_FATERROR, "FS_Flush: unknown file system type\n") );
        goto label_exit;
    }
    
label_exit:
    FS_Up();
    return iRet;
}


INT32 FS_GetFileSize(INT32 fd)
{
    struct file *file;
    struct inode *inode;
    
    if( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_GetFileSize,fd is invalid ,= % d\n",fd) );
        return ERR_FS_BAD_FD;
    }
    FS_Down();
    file = g_fs_current.filp[fd];
    if(NULL == file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileSize,file is NULL\n") );
        return ERR_FS_BAD_FD;
    }
    
    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileSize, handle is for finding\n") );
        return ERR_FS_OPERATION_NOT_GRANTED;
    }
     inode = file->f_inode;
     if ( FS_IS_DIR( inode->i_mode ) )
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileSize,file is not regular \n") );
        return ERR_FS_NOT_REGULAR;
        
    }
    else
    {
        D(( DL_FATDETAIL, "iSize = %d\n",inode->i_size));
        FS_Up();
        return inode->i_size;
    }
}


INT32 FS_GetDirSize( PCSTR pszFileName,UINT64 *pSize)
{
    WCHAR *path = NULL;
    struct inode *dir_i;
    UINT64 iSize = 0;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;

    iULen = DSM_UnicodeLen((UINT8*)pszFileName);    
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    
    // path name character validate, Ignore base name.
    if( !is_file_name_valid((PCWSTR) pszFileName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_GetDirSize , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }
    
    FS_Down();
    
    path = format_dir_name((PCWSTR)pszFileName );
    if(NULL == path) // malloc failed
    {
        D( ( DL_FATERROR, "in FS_GetDirSize, path is NULL" ) );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }
    dir_i = namei((PCWSTR)pszFileName);
    if(NULL == dir_i)
    {
        D((DL_FATERROR, "FS_GetDirSize, Call namei() return NULL."));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    iRet = fat_get_dir_size(dir_i,&iSize,TRUE);
    if (0 != iRet )
    {
        D((DL_FATWARNING, "FS_GetDirSize: fat_get_dir_size() return failed.err_code = %d.",iRet));
        iRet = ERR_FS_NO_MORE_FILES;
        goto step1_failed;
    }
    *pSize = iSize;
    iRet = ERR_SUCCESS;
    CSW_TRACE(BASE_DSM_TS_ID,
                "FS_GetDirSize  success. iSize= 0x%x,DevNo = %d.",
                iSize,dir_i->i_dev);
    iput(dir_i);

step1_failed:
  
    if ( ERR_SUCCESS != iRet )
    {
        iput( dir_i );
    }
   
step0_failed:

    if(path != NULL)
    {
        DSM_Free(path);
    }
    FS_Up();
     
    return fs_SetErrorCode(iRet);
}


INT32 FS_GetFileName(INT32 hFile, INT32 iNameBufferSize, PSTR pFileName)
{
    struct file * psFile;
    struct inode * psInode;
    WCHAR *pUniName = NULL;
    UINT16 uUniLen;
    INT32 iErrCode;
    
    if( !FD_IS_VALID( hFile ) )
    {
        D( ( DL_FATERROR, "in FS_GetFileName, invalid fd = %d\n",hFile) );
        iErrCode = ERR_FS_BAD_FD;
        return iErrCode;
    }
    pUniName = NAME_BUF_ALLOC();
    if (!pUniName)
    {
        D( ( DL_FATERROR, "In FS_GetFileName 1. malloc pUniName failed.") );
        DSM_ASSERT(0, "In FS_GetFileName 1. malloc pUniName failed.");
        iErrCode = ERR_FS_NO_MORE_MEMORY;
        return iErrCode;
    }
    
    FS_Down();
    psFile = g_fs_current.filp[ hFile ];    
    if (NULL == psFile || !fs_is_file_opened(&g_fs_file_list, psFile))
    {
        D( ( DL_FATERROR, "in FS_GetFileName, psFile is NULL.\n") );
        iErrCode = ERR_FS_BAD_FD;
        goto failed;
    }
    if(psFile->f_flags & FS_O_FIND) // handle is for finding
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileName, handle is for finding\n") );
        iErrCode = ERR_FS_OPERATION_NOT_GRANTED;
        goto failed;
    }
    psInode = psFile->f_inode;
    
    if ( FS_TYPE_FAT == psInode->i_fstype )
    {
        iErrCode = (UINT32)fat_get_file_name( psInode,  pUniName);
        if(_ERR_FAT_SUCCESS == iErrCode)        
        {
            //DSM_MemZero(pUniName, FAT_MAX_LONG_PATH_LEN + 2);
            uUniLen = DSM_UnicodeLen((UINT8*)pUniName);
            if ((UINT32)(uUniLen + LEN_FOR_NULL_CHAR) > iNameBufferSize / SIZEOF(WCHAR))
            {
                iErrCode = ERR_FS_NAME_BUFFER_TOO_SHORT;
                goto failed;
            }
            DSM_TcStrCpy((PCWSTR)pFileName, pUniName);
            iErrCode = ERR_SUCCESS;
        }
        else
        {
            D( ( DL_FATERROR, "fat_get_file_name error, errorCode = %d.\n", iErrCode) );
            iErrCode = fs_SetErrorCode(iErrCode);            
        }
    }
    else
    {
        D( ( DL_FATERROR, "FS_GetFileName: unknow filesystem\n") );
        iErrCode = ERR_FS_UNKNOWN_FILESYSTEM;
    }      

failed:
    if (pUniName)
    {
        NAME_BUF_FREE(pUniName);
    }
    FS_Up();
    return iErrCode;
}

#ifdef _T_UPGRADE_PROGRAMMER

INT32 FS_PowerOn(VOID)
{
    INT32 iResult;
    
    iResult = fs_GlobalVarInit();
    if (iResult != ERR_SUCCESS)
    {
        D((DL_FATERROR, "FS_PowerOn: fs_GlobalVarInit() failed, errorCode = %d.", iResult));
        return iResult;
    }
    
    if (FS_INVALID_SEM_HANDLE == g_fs_sem) 
    { 
        g_fs_sem = sxr_NewSemaphore(1);
    }
    
    return ERR_SUCCESS;
}

#else

INT32 FS_PowerOn(VOID)
{
    UINT8 iFsType = FS_TYPE_FAT;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    UINT32 i;

    iResult = fs_GlobalVarInit();
    if (iResult != ERR_SUCCESS)
    {
        D((DL_FATERROR, "FS_PowerOn: fs_GlobalVarInit() failed, errorCode = %d.", iResult));
        return iResult;
    }
    
    if (FS_INVALID_SEM_HANDLE == g_fs_sem) 
    { 
        g_fs_sem = sxr_NewSemaphore(1);
    }
#if 0	
    
    for(i = 0;i < g_fs_cfg.uNrSuperBlock; i++)
    {
        if(g_pFsDevInfo[i].dev_type != FS_DEV_TYPE_FLASH) 
        {
            continue;
        }
        
        iResult = FS_ScanDisk((PCSTR)g_pFsDevInfo[i].dev_name, iFsType); 
        if (ERR_SUCCESS == iResult)
        {
            D( ( DL_FATDETAIL, "scan device[%s] success.\n",g_pFsDevInfo[i].dev_name) );
        }
        else
        {
            D( ( DL_FATERROR, "scan device[%s] failing. error code = %d.\n",g_pFsDevInfo[i].dev_name,iResult) );
           // iRet = iResult;
        }
    }
#endif    
    return 0;//iRet;
}

#endif



INT32 FS_PowerOff(VOID)
{
    if (FS_INVALID_SEM_HANDLE != g_fs_sem) 
    { 
        sxr_FreeSemaphore(g_fs_sem);
        g_fs_sem = FS_INVALID_SEM_HANDLE;
    }
    return fs_GlobalVarFree();
}


INT32 fs_SetErrorCode(INT32 iErrCode)
{    
    INT32 iRet = ERR_SUCCESS;
    
    switch(iErrCode)
    {
        case _ERR_FAT_SUCCESS:          iRet = ERR_SUCCESS                 ;break;
        case _ERR_FAT_PARAM_ERROR:      iRet = ERR_FS_INVALID_PARAMETER    ;break;
        case _ERR_FAT_WRITE_SEC_FAILED: iRet = ERR_FS_WRITE_SECTOR_FAILED  ;break;
        case _ERR_FAT_READ_SEC_FAILED:  iRet = ERR_FS_READ_SECTOR_FAILED   ;break;
        case _ERR_FAT_MALLOC_FAILED:    iRet = ERR_FS_NO_MORE_MEMORY       ;break;
        case _ERR_FAT_DISK_FULL:        iRet = ERR_FS_DISK_FULL            ;break;
        case _ERR_FAT_ROOT_FULL:        iRet = ERR_FS_ROOT_FULL            ;break;
        case _ERR_FAT_READ_EXCEED:      iRet = ERR_FS_READ_FILE_EXCEED     ;break;
        case _ERR_FAT_WRITE_EXCEED:     iRet = ERR_FS_WRITE_FILE_EXCEED    ;break;
        case _ERR_FAT_NOT_SUPPORT:      iRet = ERR_FS_NOT_SUPPORT          ;break;
        case _ERR_FAT_BPB_ERROR:        iRet = ERR_FS_DATA_DESTROY         ;break;
        case _ERR_FAT_FILE_TOO_MORE:    iRet = ERR_FS_FILE_TOO_MORE        ;break;
        case _ERR_FAT_FILE_NOT_EXISTED: iRet = ERR_FS_FILE_NOT_EXIST       ;break;
        case _ERR_FAT_DIFF_DEV:         iRet = ERR_FS_DEVICE_DIFF          ;break;
        case _ERR_FAT_HAS_FORMATED:     iRet = ERR_FS_HAS_FORMATED         ;break;
        case _ERR_FAT_NOT_FORMAT:       iRet = ERR_FS_NOT_FORMAT           ;break;
        case _ERR_FAT_NO_MORE_INODE:    iRet = ERR_FS_NO_MORE_MEMORY        ;break;
        case _ERR_FS_NO_MORE_SB_ITEM:   iRet = ERR_FS_NO_MORE_SB_ITEM      ;break;
        case _ERR_FS_NOT_MOUNT:         iRet = ERR_FS_NOT_MOUNT            ;break; 
        case _ERR_FAT_DIR_NOT_NULL:     iRet = ERR_FS_DIR_NOT_EMPTY        ;break; 
        case _ERR_FAT_ERROR:            iRet = ERR_FS_DATA_DESTROY         ;break; 
        default:                       
            iRet = iErrCode;
            break;
    }
    return iRet;
}
INT32 FS_ReleaseDev(PCSTR pszDevName)
{
  UINT32 uDevNo;
  uDevNo = DSM_DevName2DevNo(pszDevName);
  if(INVALID_DEVICE_NUMBER == uDevNo)
  {
    D( ( DL_FATERROR, "in FS_ReleaseDev DevNo is invalid, pszDevName = %s\n", pszDevName) );
    D( ( DL_FATERROR, "in FS_ReleaseDev DevNo is invalid,uDevNo = %x0x\n", uDevNo ) );
    return ERR_FS_DEVICE_NOT_REGISTER;      
  }        
  if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
  {
    FS_Down(); 
    if(!fs_IsOpened(uDevNo))
    {
      DRV_DEV_DEACTIVE(uDevNo);
    }
    FS_Up();
  }
  return ERR_SUCCESS;
}

INT32 FS_Truncate(INT32 fd, UINT32 nFileSize)
{
   return  FS_ChangeSize(fd,nFileSize);
}

INT32  FS_GetDeviceInfo(UINT32* pDeviceCount,FS_DEV_INFO** ppFSDevInfo)
{
    if(NULL == pDeviceCount || 
        NULL == ppFSDevInfo)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    *ppFSDevInfo = g_pFsDevInfo;
    *pDeviceCount = g_fs_cfg.uNrSuperBlock;
    return ERR_SUCCESS;
}

UINT32  FS_GetVersion(VOID)
{
    return ((FS_VFS_VERSION << 16) | FAT_VER);
}

UINT8* g_fsDumpBuff = NULL;
INT32 FS_Dump(PCSTR pszDevName, UINT32 iBegSec,UINT32 iSectCount,PCSTR pszFileName)
{
    UINT32 uDevNo = 0;
    UINT32 iBlockSize;
    UINT32 iBlockCount;
    INT32 iResult;
    INT32 iRet;    
    UINT32 i;
    UINT8 szBuff[512];
    INT32 fd = -1;     
    UINT32 iDumpNumber = 0;
    UINT8* pDumpBuff = NULL;
    
    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }
   
    iRet = ERR_SUCCESS;
#if 1 
    sxr_Sleep(30);
    CSW_TRACE(BASE_DSM_TS_ID, 
        "FS_Dump devname = %s, iBegSec = 0x%x, iSectCount = 0x%x.\n",
        pszDevName,
        iBegSec,
        iSectCount
        );  
    sxr_Sleep(30);     
#endif  
    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in FS_Dump DevNo is invalid, pszDevName = %s\n", pszDevName) );
        D( ( DL_FATERROR, "in FS_Dump DevNo is invalid,uDevNo = %x0x\n", uDevNo ) );
        iRet = ERR_FS_DEVICE_NOT_REGISTER;
        goto step0_failed;
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        iRet = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iRet )
        {
            D( ( DL_FATERROR, "in FS_Dump device[%s]: Open t-flash memory failed.\n", pszDevName ) );
            iRet = ERR_FS_OPEN_DEV_FAILED;
            goto step0_failed;
        }
    }
    
    CSW_TRACE(BASE_DSM_TS_ID, "in FS_Dump Call DRV_GET_DEV_INFO  uDevNo = %d.\n", uDevNo);  
    iResult = DRV_GET_DEV_INFO(uDevNo,&iBlockCount,&iBlockSize);
    if(ERR_SUCCESS != iResult)
    {
        D( ( DL_FATERROR,
            "in FS_Dump DRV_GET_DEV_INFO  failed!,iResult = %d.\n",
            iResult)); 
        iRet = ERR_FS_GET_DEV_INFO_FAILED;
        goto step1_failed;
    }
    else
    {
        D( ( DL_FATDETAIL,
            "in FS_Dump DRV_GET_DEV_INFO  success!,iBlockCount = %d,iBlockSize = %d.\n",
            iBlockCount,
            iBlockSize)); 
        if(iBegSec + iSectCount > iBlockCount)
        {
            D( ( DL_FATERROR,
                "in FS_Dump   parameter error! iBeginSec = 0x%x, iEndSec = 0x%x.\n",
                iResult)); 
        }
        if(pszFileName)
       {
            fd = FS_Open( pszFileName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
            if(fd < 0)
            {      
                D( ( DL_FATERROR,
                    "in FS_Dump   Open file failed!fd = %d.\n",
                    fd));
                iRet = iResult;
                goto step0_failed;
            }
            for(i = iBegSec; i < iBegSec + iSectCount; i++)
            {
                iResult = DRV_BLOCK_READ(uDevNo,i,szBuff);
                if(iResult != ERR_SUCCESS)
                {
                    D( ( DL_FATERROR,
                        "in FS_Dump   read block failed! uDevNo = 0x%x, block no = 0x%x,err code = %d.\n",
                        uDevNo,
                        i,
                        iResult)); 
                    iRet = ERR_FS_READ_SECTOR_FAILED;
                    break;  
                }
                iResult = FS_Write(fd,szBuff,DEFAULT_SECSIZE);
                if(iResult != DEFAULT_SECSIZE)
                {
                D( ( DL_FATERROR,
                    "in FS_Dump   write file failed! uDevNo = 0x%x, sector no = 0x%x,err code = %d.\n",
                    uDevNo,
                    i,
                    iResult)
                    ); 
                iRet = ERR_FS_WRITE_SECTOR_FAILED;
                break;    
                }
#if 1
               // sxr_Sleep(5);
                CSW_TRACE(BASE_DSM_TS_ID, "dump sector no = %d.\n",i);  
                DSM_HstSendEvent(i);
                //sxr_Sleep(5);
#endif
                iDumpNumber ++;
            }
        }
        else
        {
            pDumpBuff = DSM_MAlloc(iSectCount*512); 
            if(NULL == pDumpBuff)
            {
                DSM_ASSERT(0,"FS_Dump: 1.malloc(0x%x) failed.",(iSectCount*512)); 
            }
            for(i = iBegSec; i < iBegSec + iSectCount; i++)
            {
                iResult = DRV_BLOCK_READ(uDevNo,i,szBuff);
                if(iResult != ERR_SUCCESS)
                {
                    D( ( DL_FATERROR,
                        "in FS_Dump   read block failed! uDevNo = 0x%x, block no = 0x%x,err code = %d.\n",
                        uDevNo,
                        i,
                        iResult)); 
                    iRet = ERR_FS_READ_SECTOR_FAILED;
                    break;  
                }
                DSM_MemCpy(pDumpBuff,szBuff,512);
#if 1
               // sxr_Sleep(30);
                CSW_TRACE(BASE_DSM_TS_ID, "dump sector no = %d.\n",i);  
                //sxr_Sleep(30);
#endif
                iDumpNumber ++;
            }
            g_fsDumpBuff = pDumpBuff;
            
        }
        iRet = ERR_SUCCESS;
    }

    step1_failed:    
    step0_failed:
    if(fd >= 0)
    {
        FS_Close(fd);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_IsOpened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
  
#if 1 
    sxr_Sleep(30);
    CSW_TRACE(BASE_DSM_TS_ID, "FS_Dump Finish,dump sector count = %d.\n",        
        iDumpNumber);  
    
    CSW_TRACE(BASE_DSM_TS_ID, "g_fsDumpBuff = 0x%x,iSectCount = 0x%x.",        
        g_fsDumpBuff,iSectCount);  
    DSM_HstSendEvent(0xffffeeee);
    DSM_HstSendEvent(iDumpNumber);
    sxr_Sleep(30);    
#endif
    return iRet;
}

// Function:FS_IsValidFileName
// Chech the file name if valid.It is allowable that with full path. 
// This function will check the per directory name or file name.it is't allowable that include wildcard character in the directory,
// the last name include wildcard character is allowed when input parameter bIgnoreBaseName is TRUE.
// Parameter:
// @pszFileName: The path name in unicode format.
// @bIgnoreBaseName: The flag of ignore base name. if it is TRUE, allow filename is NULL or include wildcard characters,else 
//            not allow.
// Return
//   The path name is valid,return TRUE.
//   The path name is invalid, return FALSE.
BOOL FS_IsValidFileName( PCSTR pszFileName, BOOL bIgnoreBaseName )
{
     return is_file_name_valid((PCWSTR)pszFileName,bIgnoreBaseName);
}

