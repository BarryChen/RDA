#include "fat_base.h"
#include "fs.h"
#include "dsm_dbg.h"
#include "fs_base.h"


struct task_struct g_fs_current = {NULL, NULL, NULL, NULL};

struct super_block* g_fs_super_block_table = NULL;

struct file* g_fs_file_list = NULL;

//struct inode g_fs_inode_table[ FS_NR_INODES_MAX ] = {{{{0,},},},};


struct fs_cfg_info g_fs_cfg = {FS_DEFAULT_NR_SUPER, FS_NR_OPEN_FILES_MAX};
FS_DEV_INFO* g_pFsDevInfo = NULL;
extern FAT_CACHE* g_FatCache;
extern UINT32 g_FatCahcheNum;


INT32 fs_GlobalVarInit()
{
    UINT32 iResult;
    UINT32 i = 0,n = 0;
    DSM_DEV_INFO* pDevInfo = NULL;
    UINT32 uDsmDevCount = 0;
    UINT32 uFsDevCount = 0;

    // Get Device Info and create fs device table.
    iResult = DSM_GetDeviceInfo(&uDsmDevCount,&pDevInfo);
    if(ERR_SUCCESS != iResult)
    {
       D((DL_FATERROR, "fs_GlobalVarInit: GetDeviceInfo failed.iResult = %d",iResult));
        return ERR_FS_GET_DEV_INFO_FAILED;
    }
    // compute the device count.
    for(i = 0; i < uDsmDevCount; i++)
    {
        if(DSM_MODULE_FS_ROOT == pDevInfo[i].module_id ||
            DSM_MODULE_FS == pDevInfo[i].module_id)
        {
             uFsDevCount ++;
        }

    }
    // check the FS device count.
    if(uFsDevCount == 0)
    {
        D((DL_FATERROR, "fs_GlobalVarInit: the fs device count is zero."));
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    // malloc FS device table.
    g_pFsDevInfo = DSM_MAlloc(SIZEOF(FS_DEV_INFO)*uFsDevCount);
    if(NULL == g_pFsDevInfo)
    {
        D((DL_FATERROR, "fs_GlobalVarInit: malloc fs device table failed!"));
        DSM_ASSERT(0,"fs_GlobalVarInit: 1.malloc(0x%x) failed.",(SIZEOF(FS_DEV_INFO)*uFsDevCount)); 
        return ERR_FS_NO_MORE_MEMORY;
    }
   
    // set the device talble.
    for(i = 0; i < uDsmDevCount; i++)
    {
        // device table.
        if(DSM_MODULE_FS_ROOT == pDevInfo[i].module_id)
        {
               DSM_StrCpy(g_pFsDevInfo[n].dev_name,pDevInfo[i].dev_name);
               g_pFsDevInfo[n].dev_type = pDevInfo[i].dev_type == DSM_MEM_DEV_FLASH ? FS_DEV_TYPE_FLASH : FS_DEV_TYPE_TFLASH;
               g_pFsDevInfo[n].is_root = TRUE;
               n++;
        }
        if(DSM_MODULE_FS == pDevInfo[i].module_id)
        {
               DSM_StrCpy(g_pFsDevInfo[n].dev_name,pDevInfo[i].dev_name);
               g_pFsDevInfo[n].dev_type = pDevInfo[i].dev_type == DSM_MEM_DEV_FLASH ? FS_DEV_TYPE_FLASH : FS_DEV_TYPE_TFLASH;
               g_pFsDevInfo[n].is_root = FALSE;
               n++;
        }	 
    }
    
    g_fs_cfg.uNrSuperBlock = uFsDevCount;


    // malloc FAT chache.	   
     g_FatCache = (FAT_CACHE*)DSM_MAlloc(SIZEOF(FAT_CACHE));//*uFsDevCount);
    if(NULL == g_FatCache)
    {
        D((DL_FATERROR, "fs_GlobalVarInit: malloc fs fat cache failed!"));
        DSM_ASSERT(0,"fs_GlobalVarInit: 2.malloc(0x%x) failed.",(SIZEOF(FAT_CACHE)*uFsDevCount)); 
        goto failed;
    }	
	
     // set fat cache.
     n = 0;
     g_FatCache->iSecNum = 0;
     g_FatCache->psFileInodeList = NULL;
     g_FatCache->iDirty = 0;
#if 0
     for(i = 0; i < uDsmDevCount; i++) 
     {
         if(DSM_MODULE_FS_ROOT == pDevInfo[i].module_id ||
  	      DSM_MODULE_FS == pDevInfo[i].module_id )
         {
            // fat chache.	
            g_FatCache[n].iDevNo= pDevInfo[i].dev_no;
            g_FatCache[n].iSecNum = 0;
            g_FatCache[n].psFileInodeList = NULL;
            g_FatCache[n].iDirty = 0;
            
            if(DSM_MEM_DEV_TFLASH == pDevInfo[i].dev_type)
            {
                DRV_SET_WCACHE_SIZE(pDevInfo[i].dev_no,DRV_DEF_CACHE_BLOCK_CNT*DEFAULT_SECSIZE);
                DRV_SET_RCACHE_SIZE(pDevInfo[i].dev_no,DRV_DEF_CACHE_BLOCK_CNT*DEFAULT_SECSIZE*8);
            }
            n ++;
         }
     }
#endif

    g_FatCahcheNum = n;
    g_fs_super_block_table = DSM_MAlloc((SIZEOF(struct super_block)) * g_fs_cfg.uNrSuperBlock);
    if (NULL == g_fs_super_block_table)
    {
        D((DL_FATERROR, "fs_GlobalVarInit: malloc g_fs_super_block_table failed!"));
        DSM_ASSERT(0,"fs_GlobalVarInit: 3.malloc(0x%x) failed.",((SIZEOF(struct super_block)) * g_fs_cfg.uNrSuperBlock));         
        goto failed;
    }
    DSM_MemZero(g_fs_super_block_table, (SIZEOF(struct super_block)) * g_fs_cfg.uNrSuperBlock);

    g_fs_current.filp = DSM_MAlloc((sizeof( struct file * ) ) * g_fs_cfg.uNrOpenFileMax);
    if (NULL == g_fs_current.filp)
    {
        D((DL_FATERROR, "fs_GlobalVarInit: malloc g_fs_current.filp failed!"));
        DSM_ASSERT(0,"fs_GlobalVarInit: 4.malloc(0x%x) failed.",((sizeof( struct file * ) ) * g_fs_cfg.uNrOpenFileMax)); 
        goto failed;
    }
    
    g_fs_current.wdPath = DSM_MAlloc((FS_PATH_LEN + LEN_FOR_NULL_CHAR) * SIZEOF(WCHAR));
    if (NULL == g_fs_current.wdPath)
    {
        D((DL_FATERROR, "fs_GlobalVarInit: malloc g_fs_current.wdPath failed!"));
        DSM_ASSERT(0,"fs_GlobalVarInit: 5.malloc(0x%x) failed.",((FS_PATH_LEN + LEN_FOR_NULL_CHAR) * SIZEOF(WCHAR))); 
        goto failed;
    }
    g_fs_current.pwd = NULL;
    g_fs_current.root = NULL;
    DSM_MemSet( g_fs_current.filp, 0x00, ((SIZEOF( UINT32) ) *FS_NR_OPEN_FILES_MAX));
    DSM_MemSet( g_fs_current.wdPath, 0x00, (FS_PATH_LEN + LEN_FOR_NULL_CHAR) * SIZEOF(WCHAR));

    return ERR_SUCCESS;

failed:
    if (g_fs_super_block_table != NULL)
    {
        DSM_Free(g_fs_super_block_table);
        g_fs_super_block_table = NULL;
    }
    
    if (g_fs_current.filp != NULL)
    {
        DSM_Free(g_fs_current.filp);
        g_fs_current.filp = NULL;
    }
    if (g_fs_current.wdPath != NULL)
    {
        DSM_Free(g_fs_current.wdPath);
        g_fs_current.wdPath = NULL;
    }
    g_fs_current.pwd = NULL;
    g_fs_current.root = NULL;

    if (g_FatCache != NULL)
    {
        DSM_Free(g_FatCache);
        g_FatCache = NULL;
    }

    return ERR_FS_NO_MORE_MEMORY;
}


INT32 fs_GlobalVarFree()
{
    UINT32 i;
    
    if (g_fs_super_block_table != NULL)
    {
        for(i = 0; i < g_fs_cfg.uNrSuperBlock; i++)
        {
            if(NULL != (g_fs_super_block_table+i)->fat_fsi)
            {
                DSM_Free((g_fs_super_block_table+i)->fat_fsi);
            }
        }
        DSM_Free(g_fs_super_block_table);
        g_fs_super_block_table = NULL;
    }
    
    if (g_fs_current.filp != NULL)
    {
        DSM_Free(g_fs_current.filp);
        g_fs_current.filp = NULL;
    }
    if (g_fs_current.wdPath != NULL)
    {
        DSM_Free(g_fs_current.wdPath);
        g_fs_current.wdPath = NULL;
    }
    g_fs_current.pwd = NULL;
    g_fs_current.root = NULL;

    if (g_FatCache != NULL)
    {
        DSM_Free(g_FatCache);
        g_FatCache = NULL;
    }

    return ERR_SUCCESS;
}


INT32 fs_SetWorkDirPath(WCHAR *WorkDirPath, WCHAR *ChangeDirPath)
{
    WCHAR *p = NULL;
    WCHAR *tmp = NULL;
    WCHAR *p2 = NULL;
    WCHAR *dirName;
    INT32 nameLen;
    WCHAR c;
    WCHAR c2;
    UINT8 cheat = 1;
    
    if (!WorkDirPath || !ChangeDirPath)
    {
        D( ( DL_VFSERROR, "in FS_SetWorkDirPath, parameter error" ) );
        return ERR_FS_INVALID_PARAMETER;
    }

    tmp = (WCHAR*)DSM_MAlloc((FS_PATH_LEN + LEN_FOR_NULL_CHAR) * SIZEOF(WCHAR));
    if (NULL == tmp)
    {
        D((DL_VFSERROR, "fs_SetWorkDirPath: Malloc failed!"));
        DSM_ASSERT(0,"fs_SetWorkDirPath: 1.malloc(0x%x) failed.",((FS_PATH_LEN + LEN_FOR_NULL_CHAR) * SIZEOF(WCHAR))); 
        return ERR_FS_NO_MORE_MEMORY;
    }

    DSM_TcStrCpy(tmp, WorkDirPath);
    p = tmp;
    
    if (_T_DSM('\\') == *ChangeDirPath || _T_DSM('/') == *ChangeDirPath)
    {
        // ChangeDirPath为绝对路径。
        *p = 0;
    }
    else
    {
        // ChangeDirPath为相对路径，初始化p指向当前工作路径的字符串末尾。
        while (*p)
        {
            p++;
        }
        while (p > tmp && (_T_DSM('/') == (c = *(p - 1)) || _T_DSM('\\') == c))
        {
            p--;
            *p = 0;
        }
    }

    while (_T_DSM('\\') == *ChangeDirPath || _T_DSM('/') == *ChangeDirPath)
    {
        ChangeDirPath++;
    }
    while (cheat)
    {
        while (_T_DSM(' ') == *ChangeDirPath)
        {// 去掉名字前面的空格。
            ChangeDirPath++;
        }
        dirName = ChangeDirPath;
        while ( (c = *(ChangeDirPath++)) != 0 && c != _T_DSM('\\') && c != _T_DSM('/'))
        {
            ;
        }
        p2 = ChangeDirPath - 1;
        while (p2 > dirName && (_T_DSM(' ') == *p2))
        {// 去掉名字后面的空格。
            p2--;
        }
        if ((nameLen = p2 - dirName) > 0)
        {
            if (1 == nameLen && 0 == DSM_TcStrNCmp(dirName, (PCWSTR)_T_DSM("."), 1))
            {
               ;
            }
            else if (2 == nameLen && 0 == DSM_TcStrNCmp(dirName, (PCWSTR)_T_DSM(".."), 2))
            {
                while (p > tmp && (c2 = *(--p)) != _T_DSM('\\') && c2 != _T_DSM('/'))
                {
                    ;//
                }
                *p = 0;
            }
            else
            {
                if ((p + (nameLen + 1)) > (tmp + FS_PATH_LEN * SIZEOF(WCHAR)))
                {
                    DSM_Free(tmp);
                    return ERR_FS_PATHNAME_PARSE_FAILED;
                }
                DSM_TcStrNCpy(p, (PCWSTR)_L_DSM("/"), 1);
                p++;
                DSM_TcStrNCpy(p, dirName, nameLen);
                p += nameLen;
            }
        }
        if (!c)
        {
            break;
        }
    }
    
    if (p == tmp)
    {
        DSM_TcStrCpy(p, (PCWSTR)_L_DSM("/"));
    }
    else
    {
        *p = _T_DSM('\0');
    }
    DSM_TcStrCpy(WorkDirPath, tmp);
    DSM_Free(tmp);
    return ERR_SUCCESS;

}


INT32 fs_GetSuperBlockFree(struct super_block** psSuperBlock)
{
    INT32 iRet = _ERR_FS_NO_MORE_SB_ITEM;
    INT32 i;

    for ( i = 0; i < (INT32)g_fs_cfg.uNrSuperBlock; i++ ) // FS_NR_SUPER is 1 here.
    {
        if( NULL == g_fs_super_block_table[ i ].s_root)
        {
            *psSuperBlock = g_fs_super_block_table + i;
            iRet = _ERR_FS_SUCCESS;
            break;
        }
    }
    if(iRet != _ERR_FS_SUCCESS)
    {
        D((DL_VFSWARNING, "in fs_GetSuperBlockFree  super_block_table is full\n"));
    }
    return iRet;
}


INT32 fs_FindSuperBlock(UINT32 iDev,struct super_block** psSuperBlock) 
{
    INT32 iRet = _ERR_FS_NOT_MOUNT;
    INT32 i;

    for ( i = 0; i < (INT32)g_fs_cfg.uNrSuperBlock; i++ ) // FS_NR_SUPER is 1 here.
    {
        if( iDev == g_fs_super_block_table[ i ].s_dev)
        {
            *psSuperBlock = g_fs_super_block_table + i ;
            iRet = _ERR_FS_SUCCESS;
            break;
        }
    }
    if(_ERR_FS_SUCCESS != iRet)
    {
        D((DL_VFSWARNING, "in fs_FindSuperBlock  super_block is not found,iDev =  \n",iDev));        
    }
    return iRet;
}


struct file* fs_alloc_fd( void )
{
    struct file * fd = NULL;

    fd = (struct file *)DSM_MAlloc(SIZEOF(struct file));
    if( NULL == fd)
    {
        DSM_ASSERT(0,"fs_alloc_fd: 1.malloc(0x%x) failed.",(SIZEOF(struct file))); 
        return NULL;
    }
    
    DSM_MemSet( fd, 0x00, sizeof(*fd) );
    return fd;
}


struct file* fs_fd_list_add(struct file **ppsFileList, struct file *psFD)
{
    if (NULL == ppsFileList || NULL == psFD)
    {
        D ((DL_FATERROR, "fs_fd_list_add: _ERR_FAT_PARAM_ERROR, ppsFileList(0x%x), psFD(0x%x)", ppsFileList, psFD));
        return (struct file *)NULL;
    }
    
    psFD->next = *ppsFileList;
    *ppsFileList = psFD;
    return psFD;
}


INT32 fs_fd_list_del(struct file **ppsFileList, struct file *psFD)
{
    struct file **ppPrei;
    struct file *pCuri;
    
    if (NULL == ppsFileList || NULL == psFD)
    {
        D ((DL_FATERROR, "fs_fd_list_del: _ERR_FAT_PARAM_ERROR, ppsFileList(0x%x), psFD(0x%x)", ppsFileList, psFD));
        return ERR_FS_INVALID_PARAMETER;
    }

    ppPrei = ppsFileList;
    pCuri = *ppsFileList;
    while (pCuri != NULL)
    {
        if (pCuri == psFD)
        {
            *ppPrei = pCuri->next;
            DSM_Free(pCuri);
            return ERR_SUCCESS;
        }
        else
        {
            ppPrei = &(pCuri->next);
            ppPrei = ppPrei;
            pCuri = pCuri->next;
        }
    }
    return ERR_FS_INVALID_PARAMETER;
    
}


BOOL fs_is_file_opened(struct file **ppsFileList, struct file *psFD)
{
    struct file **ppPrei;
    struct file *pCuri;
    
    if (NULL == ppsFileList || NULL == psFD)
    {
        D ((DL_FATERROR, "fs_fd_list_del: _ERR_FAT_PARAM_ERROR, ppsFileList(0x%x), psFD(0x%x)", ppsFileList, psFD));
        return FALSE;
    }

    ppPrei = ppsFileList;
    pCuri = *ppsFileList;
    while (pCuri != NULL)
    {
        if (pCuri == psFD)
        {
            return TRUE;
        }
        else
        {
            ppPrei = &(pCuri->next);
            ppPrei = ppPrei;
            pCuri = pCuri->next;
        }
    }
    return FALSE;
}

BOOL fs_IsOpened(UINT32 iDev)
{
    struct file **ppPrei;
    struct file *pCuri;
    
    if (NULL == g_fs_file_list )
    {        
        return FALSE;
    }

    ppPrei = &g_fs_file_list;
    pCuri = g_fs_file_list;
    while (NULL != pCuri)
    {
        if(NULL != pCuri->f_inode)
        {
            if (pCuri->f_inode->i_dev == iDev)
            {
                return TRUE;
            }
            else
            {
                ppPrei = &(pCuri->next);
                pCuri = pCuri->next;
            }
        }
        else
        {
            ppPrei = &(pCuri->next);
            ppPrei = ppPrei;
            pCuri = pCuri->next;
        }

    }
    return FALSE;
}



