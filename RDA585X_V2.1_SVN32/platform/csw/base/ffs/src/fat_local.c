
#include "dsm_cf.h"
#include "fs.h"
#include "fat_local.h"
#include "fat_base.h"
#include "fs_base.h"
#include "dsm_dbg.h"
typedef enum
{
    CLUS_STATE_FST,
    CLUS_STATE_NEXT,
    CLUS_STATE_LAST,
}CLUS_STATE;

typedef enum 
{
    FAT_SCAN_DIR_BASE,
    FAT_SCAN_DIR,
    FAT_SCAN_FILE,
    FAT_SCAN_END, 
    FAT_SCAN_ERROR,
}FAT_SCAN_STATE;

typedef struct _search_dir
{
    UINT32 iClus;                           // Cluster number.       
    UINT16 iEOff;                           // dir enrty offset.
    UINT8 iSec;                              // sector number. 
    UINT32 iCurClus;                      // current cluster number.
    UINT8 iCurSec;                        // current  sector number. 
    UINT8 iIsDirty;                        // current cluster number or current  sector number is dirty. 1:dirty,0:clean. 
    UINT8 iIsRead;                        // If need read. 1:need;0 not need.
    UINT8 iIsGetNextClus;            // If need read next cluster.1: need; 0:not need. 
    CLUS_STATE eCluState;          // cluse state.
    UINT8 iLCnt;                         // long dir entry count.
    UINT32 iClusCnt;                    // cluster count.
    UINT32 iSecCnt;                     // sector count.
    UINT32 iPreClus;                    // per-cluster number.
    UINT8 iPreSec;                      // per-sector number. 
    UINT8 *szBuff;//[DEFAULT_SECSIZE];    // buffer for dir.
}SEARCH_DIR;
extern struct task_struct g_fs_current;
extern struct super_block* g_fs_super_block_table;
extern FAT_CACHE* g_FatCache;


INT32 fat_read_super( struct super_block *sb, UINT32 uDevNo)
{
    INT32 lErrCode;
    
    sb->s_dev = uDevNo;
    sb->s_magic = FAT_MAGIC;
    sb->s_fstype = FS_TYPE_FAT;   
    
    lErrCode = fat_read_sb_info(sb);
    if( _ERR_FAT_SUCCESS != lErrCode)
    {
        D( ( DL_FATERROR,"in fat_read_super, fat_read_sb_info failed, ErrorCode = %d\n",lErrCode));    
    }
    return lErrCode;
}


/*
供VFS使用，VFS在设置好inode{}中不依赖于FAT的消息后，再调用本函数
让FAT将自己的私有消息填好再把inode{}回传给VFS
*/
INT32 fat_read_inode ( struct inode *inode,UINT32 is_get_fatentry)
{
    FAT_SB_INFO* fsbi = NULL;
    INT32 lErrCode = _ERR_FAT_SUCCESS;
    
    //bsec是sector在cluster中的index；而bsec2是sector在volume中的index
    UINT32 bclu;
    UINT16 bsec;                                        // Sector index in cluster.
    UINT16 bsec2 = 0;                                // Sector index in valume.
    UINT16 soff;                                         // The first dir entry offset in sector.
    FAT_DIR_ENTRY entry;    
    UINT8  *ebuf;
    UINT8  ecount = 0;
    
    //struct inode* curi;
    //struct inode **prei;
    
    fsbi = &(inode->i_sb->u.fat_sb);
    if( NULL == inode)
    {
        D( ( DL_FATERROR, "in fat_read_inode. inode is null \n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    ebuf = FAT_SECT_BUF_ALLOC();

    DSM_MemSet(&entry, 0x00, sizeof(FAT_DIR_ENTRY));
    
    if((unsigned int)FS_ROOT_INO == inode->i_ino) // Read root inode 
    {
        entry.DIR_Attr |= ATTR_DIR;
        entry.DIR_FileSize = fsbi->iRootEntCnt*FAT_DIR_ENTRY_SIZE;
        // FAT12及FAT16不存在物理的根目录，FAT32根目录则在BPB中有指定的起始簇。
        if (FAT32 == fsbi->iFATn)
        {
            entry.DIR_FstClusLO = (UINT16)fsbi->iFat32RootClus;
            entry.DIR_FstClusHI = (UINT16)((fsbi->iFat32RootClus >> 16) & 0x0000FFFF);
        }
        else
        {
            entry.DIR_FstClusLO = FS_ROOT_CLUSTER;
            entry.DIR_FstClusHI = 0;
        }
        inode->u.fat_i.entry = entry;
        inode->i_mode =  (entry.DIR_Attr & 0x000000ff);
        inode->u.fat_i.BeginSec    = 0;
        inode->u.fat_i.EntryOffset = 0;
        inode->u.fat_i.EntryCount  = 0;
        inode->u.fat_i.Dirty = DATA_CLEAN;
    }
    else
    {
        bclu = FAT_INO2CLUS(inode->i_ino);
        bsec = FAT_INO2SEC(inode->i_ino);
        soff = FAT_INO2OFF(inode->i_ino);

        if(bclu >= 2)
        {
            bsec2 = (UINT16)(fsbi->iDataStartSec + ((bclu-2) * fsbi->iSecPerClus)+ bsec);  
            lErrCode = fat_read_entries(fsbi, bclu, bsec, soff, ebuf, &ecount);
        }
        else if(bclu == FS_ROOT_CLUSTER)
        {
            bsec2 = (UINT16)(fsbi->iRootDirStartSec + bsec); 
            lErrCode = fat_read_root_entries(fsbi, bsec, soff, ebuf, &ecount);
        }
	   else
        {
            D( ( DL_FATERROR, "in fat_read_inode. Invalid custer number:0x%x",bclu) );
            DSM_ASSERT(0, "in fat_read_inode. Invalid custer number:0x%x",bclu);
     	   }
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D( ( DL_FATERROR, "in fat_read_inode. read entries error ErrorCode = %d\n",lErrCode) );
            goto end;
        }
        
        Buf2FDE(&ebuf[(ecount-1)*DEFAULT_DIRENTRYSIZE], &entry);
        
        inode->i_mode = (entry.DIR_Attr & 0x000000ff);
        
        inode->u.fat_i.BeginSec = bsec2;
        inode->u.fat_i.EntryOffset = soff;
        inode->u.fat_i.EntryCount = ecount;
        inode->u.fat_i.Dirty = DATA_CLEAN;
    
        Buf2FDE(&ebuf[(ecount-1)*DEFAULT_DIRENTRYSIZE], &entry);

        inode->u.fat_i.entry = entry;

        if (TRUE == is_get_fatentry && !(entry.DIR_Attr & ATTR_DIR) && (((UINT32)entry.DIR_FstClusLO | (UINT32)(entry.DIR_FstClusHI << 16)) != 0))
        {
            // 如果是常规文件并且该文件非空则将其占用的所有cluster读出并压缩
            // 放在fat_i.CluChain中，这样可以提高文件的读写速度；如果是目录则不必。
            lErrCode = fat_read_fatentries(fsbi, (UINT32)((UINT32)entry.DIR_FstClusLO | (UINT32)(entry.DIR_FstClusHI << 16)), &(inode->u.fat_i.CluChain)); 
            if(_ERR_FAT_SUCCESS != lErrCode)
            {
                D( ( DL_FATERROR, "in fat_read_inode. fat_read_fatentries error ErrorCode =%d \n",lErrCode) );
            	goto end;
            }
        }
    }

    inode->i_size = entry.DIR_FileSize;
    inode->i_ctime = DSM_UniteFileTime(entry.DIR_CrtDate,entry.DIR_CrtTime,entry.DIR_CrtTimeTenth);
    inode->i_atime = DSM_UniteFileTime(entry.DIR_LstAccDate,0,0);
    inode->i_mtime = DSM_UniteFileTime(entry.DIR_WrtDate,entry.DIR_WrtTime,0);
end:
    FAT_SECT_BUF_FREE(ebuf);

    return lErrCode;
}

//
// find 'de' on the directory 'dir_i', output ino.
// mainly to get the ino
//
INT32 fat_do_find_entry( struct inode * dir_i, struct dentry *de, UINT32 *ino,UINT32 iFlag)
{
    FAT_SB_INFO *sb_info = NULL;
    FAT_DIR_ENTRY fentry;
    UINT32 bclu;
    UINT16 bsec, eoff;
    INT32 lErrCode = _ERR_FAT_SUCCESS;
    UINT16 ecount;
    INT32 i;
    
    //
    // The 'append_data' is used to save the postion information for finding function
    // and it is made of three members: 1)begining cluster, 2)begining sector(based on cluster)
    // and 3) offset on the begining sector
    // 
    sb_info = &(dir_i->i_sb->u.fat_sb);
    
    if(FS_FLAG_FIND_FILE == de->append_data2)
    {
        bclu = FAT_INO2CLUS(de->append_data);
        bsec = FAT_INO2SEC(de->append_data);
        eoff = FAT_INO2OFF(de->append_data);
    }
    else
    {
        bclu = 0;
        bsec = 0;
        eoff = 0;
    }
    
    if(((unsigned int)FS_ROOT_INO == dir_i->i_ino 
         || 0 == (UINT32)(dir_i->u.fat_i.entry.DIR_FstClusLO | (UINT32)(dir_i->u.fat_i.entry.DIR_FstClusHI<<16)))
         && sb_info->iFATn != FAT32)
    { // FAT12或FAT16的根目录。FAT12与FAT16的根目录没有起始簇须特殊处理，
       //而FAT32的根目录则有起始簇。
        lErrCode = fat_lookup_entry(&(dir_i->i_sb->u.fat_sb), \
                                    FS_ROOT_CLUSTER, de->name, &fentry, &bclu, &bsec, &eoff, &ecount,iFlag,FAT_STR_TYPE_UNICODE);
        if(lErrCode != _ERR_FAT_SUCCESS)
        {
            goto end;
        }

        // Make up inode number for the VFS
        
        if ((fentry.DIR_Attr & ATTR_DIR) && 0 == ((UINT32)(fentry.DIR_FstClusHI << 16) | (UINT32)fentry.DIR_FstClusLO))
        {
            // 一级子目录中对应根目录的目录项".."其"DIR-FstClu"为0，
            // 所以可以根据"DIR-FstClu"为0这个特点判断一个目录项对
            // 应的是否为根目录而作特殊处理。
            *ino = (UINT32)FS_ROOT_INO;
        }
        else
        {
            // bsec = (UINT16)(bsec - sb_info->iRootDirStartSec);  20070128. 不要减去iRootDirStartSec了。
            // 因为了传出的就是相对于根目录开始扇区的偏移扇区数
            *ino = FAT_CSO2INO(bclu, bsec, eoff);
        }
        
        // The following code just for find function
        for(i=0; i<ecount; i++)
        {
            eoff += DEFAULT_DIRENTRYSIZE;
            if(eoff == sb_info->iBytesPerSec)
            {
                eoff = 0;
                bsec++;
            }
        }
        
        de->append_data = FAT_CSO2INO(bclu, bsec, eoff);
    }
    else
    {
        UINT32 startclu;
        if(0 == bclu)
        {
            startclu = dir_i->u.fat_i.entry.DIR_FstClusLO;
            startclu |= (UINT32)(dir_i->u.fat_i.entry.DIR_FstClusHI << 16);
        }
        else
        {
            startclu = bclu;
        }

        if(fat_is_last_cluster(sb_info, startclu))
        {
            lErrCode = _ERR_FAT_READ_EXCEED;
            goto end;
        }
        
        lErrCode = fat_lookup_entry(&(dir_i->i_sb->u.fat_sb), \
            startclu, de->name, &fentry, &bclu, &bsec, &eoff, &ecount, iFlag,FAT_STR_TYPE_UNICODE);
        if(lErrCode != _ERR_FAT_SUCCESS)
        {
            goto end;
        }

        if ((fentry.DIR_Attr & ATTR_DIR) && 0 == ((UINT32)(fentry.DIR_FstClusHI << 16) | (UINT32)fentry.DIR_FstClusLO))
        {
            // 一级子目录中对应根目录的目录项".."其"DIR-FstClu"为0，
            // 所以可以根据"DIR-FstClu"为0这个特点判断一个目录项对
            // 应的是否为根目录而作特殊处理。
            *ino = (UINT32)FS_ROOT_INO;
        }
        else
        {
            // bsec = (UINT16)(bsec - sb_info->iRootDirStartSec);  20070128. 不要减去iRootDirStartSec了。
            // 因为了传出的就是相对于根目录开始扇区的偏移扇区数
            *ino = FAT_CSO2INO(bclu, bsec, eoff);
        }

        if (FS_FLAG_FIND_FILE == de->append_data2)
        {
            // The following code just for find function
            for(i=0; i<ecount; i++)
            {
                eoff += DEFAULT_DIRENTRYSIZE;

                if(eoff == sb_info->iBytesPerSec)
                {
                    bsec++;
                    eoff = 0;
                }

                if(bsec == sb_info->iSecPerClus)
                {
                    bsec = 0;
                    lErrCode = fat_get_next_cluster(sb_info, bclu, &bclu);
                    if (lErrCode != _ERR_FAT_SUCCESS)
                    {
                        D((DL_FATERROR, "fat_do_find_entry: fat_get_next_cluster() return %d", lErrCode));
                        goto end;
                    }
                }
            }
            
            de->append_data = FAT_CSO2INO(bclu, bsec, eoff);
        }
    }

end:
    if(_ERR_FAT_SUCCESS != lErrCode)
    {
        D( ( DL_FATDETAIL, "in fat_do_find_entry.fat_lookup_entry error ErrorCode = %d\n",lErrCode) );
        return lErrCode;
    }
    else
    {
        return _ERR_FAT_SUCCESS;
    }
}


//
// Write inode.
// This function used by VFS module.
// VFS module set the inode for FAT,update the dentry. 
//
INT32 fat_write_inode( struct inode * inode, BOOL bFreeFlag )
{
    FAT_CACHE *psFATCache = NULL;    
    FAT_SB_INFO* fsbi;
    //struct inode* curi;
    //struct inode **prei;
    UINT8  *secbuf;
    INT32 iRet;

    secbuf = FAT_SECT_BUF_ALLOC();

    iRet = _ERR_FAT_SUCCESS;
    fsbi = &(inode->i_sb->u.fat_sb);
    //curi = fsbi->inode_chain;
    //prei = &(fsbi->inode_chain);

    //while(curi)
    {
        //if(curi->i_ino== inode->i_ino)    
        {
            //(*prei) = curi->u.fat_i.next;

            //
            // Write the entry of this inode into disk 
            // if the file size is change.
            // It changes file size or others.
            //
            if (fat_is_inode_relating_with_fatcache(inode, &psFATCache))
            {
                if(psFATCache->iSecNum != 0)
                {
                    iRet = fat_synch_fat_cache(fsbi, psFATCache);
                    if(_ERR_FAT_SUCCESS != iRet)
                    {
                        D( ( DL_FATERROR, "in fat_write_inode, fat_synch_fat_cache failed,iRet = %d\n",iRet) );
                        goto end;
                    }
                }
            }
            iRet = fat_update_dentry(inode, secbuf);
            if(_ERR_FAT_SUCCESS != iRet)
            {
                D( ( DL_FATERROR, "in fat_write_inode, fat_update_dentry failed,iRet = %d\n",iRet) );
                goto end;
            }

            if (bFreeFlag)
            {
                fat_free_fatentries(&(inode->u.fat_i.CluChain));			
                //DSM_MemSet( inode, 0, sizeof( struct inode ) );
            }
            //break;
        }

        //prei = &(curi->u.fat_i.next);
        //curi = curi->u.fat_i.next;
    }

end:
    FAT_SECT_BUF_FREE(secbuf);
    return iRet;
    
}

INT32 fat_create_file( struct inode *dir_i, struct dentry *de, UINT8 mode )
{
    struct fat_dir_entry fde;
    UINT32 bclu = 0;
    UINT16 bsec = 0, eoff = 0;
    UINT32 ino;
    struct inode *inode;
    struct inode *empty;
    INT32 lErrCode =  _ERR_FAT_SUCCESS;
    struct super_block* sb = dir_i->i_sb;
    UINT32 dev_nr = sb->s_dev;
    UINT32 fileTime;
    
    
    DSM_MemSet(&fde, 0x00, sizeof(struct fat_dir_entry));

    empty = get_empty_inode();
    if( NULL == empty ) // inode table is full
    {
        D( ( DL_FATERROR, "in fat_create_file,get_empty_inode error,\n") );
        return _ERR_FAT_NO_MORE_INODE;
    }
    
    ino = FAT_CSO2INO(bclu, bsec, eoff);
    inode = empty;
    inode->i_count = 1;
    inode->i_dev = dev_nr;
    //inode->i_ino, set it below.
    inode->i_sb = sb;
    inode->i_fstype = sb->s_fstype;
    inode = inode_list_add(&sb->inode_chain, inode);

    
    
    fde.DIR_Attr = mode;
    inode->i_mode = mode;
    fileTime = DSM_GetFileTime();
    inode->i_mtime = inode->i_ctime = fileTime;
    fde.DIR_CrtDate = DSM_GetFileTime2Date(fileTime);
    fde.DIR_CrtTime = DSM_GetFileTime2Time(fileTime);
    inode->i_atime = DSM_UniteFileTime(fde.DIR_CrtDate, 0,0);
    fde.DIR_CrtTimeTenth = 0;  // need handle it
    fde.DIR_FileSize = 0;
    fde.DIR_FstClusHI = 0;
    fde.DIR_FstClusLO = 0;
    fde.DIR_LstAccDate = fde.DIR_CrtDate;
    // de->DIR_Name, need set in fat_add_root_entry() or fat_add_entry() function.
    fde.DIR_NTRes = 0;
    fde.DIR_WrtDate = fde.DIR_CrtDate;
    fde.DIR_WrtTime = fde.DIR_CrtTime;

    if((unsigned int)FS_ROOT_INO == dir_i->i_ino && dir_i->i_sb->u.fat_sb.iFATn != FAT32)
    {
        D( ( DL_FATDETAIL, "in fat_create_file, fat_add_root_entry return: bsec = %d, eoff = %d.\n", bsec, eoff) );
        lErrCode = fat_add_root_entry(dir_i, &fde, &bsec, &eoff, de->name, FALSE);
        bclu = FS_ROOT_CLUSTER;
        // bsec = (UINT16)(bsec - sb_info->iRootDirStartSec);  20070128. 不要减去iRootDirStartSec了。
        // 因为了传出的就是相对于根目录开始扇区的偏移扇区数
        D( ( DL_FATDETAIL, "in fat_create_file, fat_add_root_entry return: bsec = %d, eoff = %d.\n", bsec, eoff) );
       
    }
    else
    {
        lErrCode = fat_add_entry(dir_i, &fde, &bclu, &bsec, &eoff, de->name, FALSE);   
        D( ( DL_FATDETAIL, "in fat_create_file, fat_add_entry return:\nbclu = %d, bsec = %d, eoff = %d.\n", bclu, bsec, eoff) );
    }

    if(_ERR_FAT_SUCCESS == lErrCode)
    {	
        ino = FAT_CSO2INO(bclu, bsec, eoff);
        inode->i_ino = ino;
        //DSM_HstSendEvent(0x08010210);
        //DSM_HstSendEvent(ino);
        D( ( DL_FATDETAIL, "in fat_create_file,call fat_read_inode() with: inode_no = %d.\n", ino) );
        lErrCode = fat_read_inode( inode ,TRUE);
        //DSM_HstSendEvent(lErrCode);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            iput(inode);
            D( ( DL_FATERROR, "in fat_create_file,fat_read_inode error.\n") );
            lErrCode = lErrCode;
        }
        else
        {
            de->inode = inode;
            lErrCode = lErrCode;
        }        
    }
    else
    {
        D(( DL_FATDETAIL,	"in fat_create_file() after add entry,lErrCode = %d.\n", lErrCode));  
        iput(inode);		        
    }

    return lErrCode;   
}

//
// Operation steps:
//
// 1) set all of the FAT items to 0x00, except the first one.
// 2) set the fat_dir_entry.DIR_FileSize to ZERO.
// 
INT32 fat_trunc_file( struct inode *dir_i, struct dentry *de, int mode )
{
    FAT_SB_INFO* sb_info = NULL;
    FAT_DIR_ENTRY* fde = NULL;    
    INT32 lErrCode =  _ERR_FAT_SUCCESS;
    UINT16 bsec = 0;
    UINT16 eoff = 0;
    UINT32 bclu;
    UINT8 entbuf[DEFAULT_DIRENTRYSIZE];
    UINT8 ecount;
    UINT8 *secbuf;
    UINT32 firstCluster;
    UINT32 fileTime;
    
    sb_info = &(dir_i->i_sb->u.fat_sb);
    fde = &(de->inode->u.fat_i.entry);    
    
    bsec = (UINT16)(de->inode->u.fat_i.BeginSec);
    eoff = (UINT16)(de->inode->u.fat_i.EntryOffset);
    
    firstCluster = (UINT32)((UINT32)(fde->DIR_FstClusLO) | (UINT32)(fde->DIR_FstClusHI << 16));
    
    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR,"in fat_trunc_file,1.FAT_SECT_BUF_ALLOC() failed.")); 
        DSM_ASSERT(0,"in fat_trunc_file,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;        
    }
    
    // set the fat_dir_entry.DIR_FileSize to ZERO.
    // Update the size in inode for the opened file.
    bclu = FAT_INO2CLUS(de->inode->i_ino);
    bsec = FAT_INO2SEC(de->inode->i_ino);
    eoff = FAT_INO2OFF(de->inode->i_ino);

    fileTime = DSM_GetFileTime();
    de->inode->i_ctime = de->inode->i_mtime = fileTime;
    fde->DIR_WrtDate = DSM_GetFileTime2Date(fileTime);
    fde->DIR_WrtTime = DSM_GetFileTime2Time(fileTime);  
    fde->DIR_CrtDate = fde->DIR_WrtDate;
    fde->DIR_CrtTime = fde->DIR_WrtTime;
    
    de->inode->i_atime = DSM_UniteFileTime(fde->DIR_WrtDate, 0,0);
    fde->DIR_LstAccDate = fde->DIR_WrtDate;
    fde->DIR_FstClusLO = 0;
    fde->DIR_FstClusHI = 0;
    fde->DIR_FileSize = 0;
    de->inode->i_size = 0;
    de->inode->i_mode = mode;
    fde->DIR_Attr = mode;
    if(bclu > FS_ROOT_CLUSTER)
    {
        lErrCode = fat_read_entries(sb_info, bclu, bsec, eoff, secbuf, &ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D((DL_FATERROR, "in fat_trunc_file,fat_read_entries error, ErrorCode = %d\n",lErrCode));
            goto end;
        }

        FDE2Buf(fde, entbuf);
        DSM_MemCpy((UINT8*)(secbuf+(ecount-1)*DEFAULT_DIRENTRYSIZE), entbuf, DEFAULT_DIRENTRYSIZE);

        lErrCode = fat_write_entries(sb_info, bclu, bsec, eoff, secbuf, ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D((DL_FATERROR, "in fat_trunc_file,fat_write_entries error, ErrorCode = %d\n",lErrCode));
            goto end;
        }
    }
    else
    {
        lErrCode= fat_read_root_entries(sb_info, bsec, eoff, secbuf, &ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D((DL_FATERROR, "in fat_trunc_file,fat_read_root_entries error, ErrorCode = %d\n",lErrCode));
            goto end;
        }
        
        FDE2Buf(fde, entbuf);
        DSM_MemCpy((UINT8*)(secbuf+(ecount-1)*DEFAULT_DIRENTRYSIZE), entbuf, DEFAULT_DIRENTRYSIZE);

        lErrCode= fat_write_root_entries(sb_info, bsec, eoff, secbuf, ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D((DL_FATERROR, "in fat_trunc_file,fat_write_root_entries error, ErrorCode = %d\n",lErrCode));
            goto end;
        }
    }

    // 如果该文件非空： set all of the FAT items to 0x00, include the first one.
    if (firstCluster != 0)
    {
        lErrCode = fat_trunc_fatentry(sb_info, firstCluster);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D((DL_FATERROR, "in fat_trunc_file,fat_trunc_fatentry error, ErrorCode = %d\n",lErrCode));
            goto end;
        }
        fat_free_fatentries(&(de->inode->u.fat_i.CluChain));
    }
/* 空文件不占用cluster，因此删去此段代码。su_wenguang 2007/5/30
    lErrCode = fat_read_fatentries(sb_info, de->inode->u.fat_i.entry.DIR_FstClusLO, 
        &(de->inode->u.fat_i.CluChain));
    if(_ERR_FAT_SUCCESS != lErrCode)
    {
        D((DL_FATERROR, "in fat_trunc_file,fat_read_fatentries error, ErrorCode = %d\n",lErrCode));
        goto end;
    }*/
    
end:
    if(NULL != secbuf) 
    {
        FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
    }
    
    return lErrCode;
}

/*
Upon successful completion, 0 is returned, else errno is returned. 

在FS_UnlinkByHandle()中调用本函数时，如果要unlink的direntry关联的是
directory file，则应该将direntry关联的inode的nlinks置为0，以保证
随后的iput()中会释放该inode。

在FS_Rename()中调用本函数时，如果要unlink的direntry关联的是
directory file，则应该将direntry关联的inode的nlinks减1而不是置为0，
以保证随后的iput()中不会释放该inode。

BOOL bRename: call me from FS_Rename() or not.
*/
INT32 fat_do_unlink( struct inode *dir_i, struct dentry *de, BOOL bRename )
{
    INT32 lErrCode = _ERR_FAT_SUCCESS;
    UINT32 bclu;
    UINT16 bsec, soff;

    bclu = FAT_INO2CLUS(de->inode->i_ino);
    bsec = FAT_INO2SEC(de->inode->i_ino);  // 如果是根目录，得到的是根目录区中扇区的偏移扇区。
    soff = FAT_INO2OFF(de->inode->i_ino);

    if(bclu > 0)
    {
        lErrCode = fat_remove_entry(&(dir_i->i_sb->u.fat_sb), 
                    &(de->inode->u.fat_i.entry), bclu, bsec, soff, bRename);
    }
    else
    {
        lErrCode = fat_remove_entry(&(dir_i->i_sb->u.fat_sb), 
                    &(de->inode->u.fat_i.entry), 0, bsec, soff, bRename);
    }

    return lErrCode;
}


// 新文件名在目录项de中，目录项的其他信息可以从old_de中得到。
INT32 fat_do_link( struct dentry *old_de, struct inode *dir_i, struct dentry *de, UINT32 type )
{
    FAT_DIR_ENTRY old_entry;
    INT32 lErrCode = _ERR_FAT_SUCCESS;
    UINT32 bclu = 0;
    UINT16 bsec = 0, eoff = 0;
    UINT32 fileTime;

    type = type; // WARNING: maybe need to do it.
    
    if(dir_i->i_sb->s_dev != old_de->inode->i_sb->s_dev)
    {
        D((DL_FATERROR, "fat_do_link(), link difference device!\n"));
        return _ERR_FAT_DIFF_DEV;
    }

    old_entry = old_de->inode->u.fat_i.entry;
    fileTime = DSM_GetFileTime();
    old_entry.DIR_CrtDate = DSM_GetFileTime2Date(fileTime);
    old_entry.DIR_CrtTime = DSM_GetFileTime2Time(fileTime);
    old_entry.DIR_LstAccDate = old_entry.DIR_CrtDate;
    old_entry.DIR_WrtDate = old_entry.DIR_CrtDate;
    old_entry.DIR_WrtTime = old_entry.DIR_CrtTime;

    if((unsigned int)FS_ROOT_INO == dir_i->i_ino && dir_i->i_sb->u.fat_sb.iFATn != FAT32)
    {
        lErrCode = fat_add_root_entry(dir_i, &old_entry, &bsec, &eoff, de->name, TRUE);
    }
    else
    {
        lErrCode = fat_add_entry(dir_i, &old_entry, &bclu, &bsec, &eoff, de->name, TRUE);
    }
    
    if(_ERR_FAT_SUCCESS != lErrCode)
    {
        D((DL_FATERROR, "fat_do_link(), add entry or add_root_entry error!\n"));
        return lErrCode;
    }
	
    return lErrCode;
}

INT32 fat_HasFormated(UINT32 uDevNo, UINT8 iType)
{
    UINT32 iSecOff = 0;
    UINT32 iFatSz = 0;
    UINT32 iFatIndex = 0;
    UINT32 i;
    struct super_block sSB;
    struct super_block* pSB = NULL;
    struct fat_sb_info* pFatSB;
    UINT8* pBuff = NULL;
    UINT32 iFatSymbol = 0;
    INT32 iResult = 0;
    INT32 iRet = _ERR_FAT_HAS_FORMATED;

    iType = iType;
    pSB = &sSB;

    DSM_MemSet(&sSB,0,sizeof(struct super_block));
     
    // malloc the buffer to stored the fbr content.
    pBuff = FAT_SECT_BUF_ALLOC();
    if(NULL == pBuff)
    {
        D(( DL_FATERROR,"in fat_HasFormated,1.FAT_SECT_BUF_ALLOC() failed.")); 
        DSM_ASSERT(0,"in fat_HasFormated,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }      
    
    pSB->s_dev = uDevNo;
    // Read the boot sector Region
    iResult = fat_read_sb_info(pSB);
    if(_ERR_FAT_SUCCESS == iResult)
    {		
        pFatSB = &(pSB->u.fat_sb);        
	
        if(FAT32 == pFatSB->iFATn || FAT16 == pFatSB->iFATn || FAT12 == pFatSB->iFATn)
	{			
            // Read the FAT Region
            iSecOff = pFatSB->iFATStartSec;
            iFatIndex = 0;

            iFatSz = pFatSB->iFATSize;
            
            // only check the first fat table.
            while((iFatIndex < FAT_FAT_COUNT - 1) && (_ERR_FAT_HAS_FORMATED == iRet))
            { 	
                //DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);
                for(i = 0; i < 1; i++)
                {
                    iResult = DRV_BLOCK_READ( uDevNo, iSecOff, pBuff );
                    if(_ERR_FAT_SUCCESS != iResult)
                    {
                        iRet = _ERR_FAT_READ_SEC_FAILED;
                        D((DL_FATERROR, "in fat_HasFormated(), DRV_BLOCK_READ! ErrorCode = %d \n",iResult));
                        break;
                    }

                    // Because some factory set the fat symbol parameter is zero or other value,we ignore the checking for it on FAT32/FAT16 partition.
                    // but the fat12 is VDS device, judge the device if is format through checking the symbol parameter.
                    iFatSymbol = 0x0;
                    if(FAT32 == pFatSB->iFATn)
                    {                        
                        DSM_MemCpy(&iFatSymbol,pBuff,4);
                        if (iFatSymbol != 0)//symbol:0x0ffffff8
                        {
                            DSM_MemCpy(&iFatSymbol,pBuff + 4,4);
                            if (iFatSymbol != 0)
                            {
                                continue;
                            }
                            else
                            {
                                continue;
                            }

                        }
                        else
                        {
                            continue;
                        }
                    }
                    else if(FAT16 == pFatSB->iFATn)
                    {                     
                        DSM_MemCpy(&iFatSymbol,pBuff,4);
                        if (iFatSymbol != 0) // symbol:0xfff8
                        {
                              continue;
                        }
                        else
                        {
                              continue;
                        }
                        
                    }
                    else if(FAT12 == pFatSB->iFATn)
                    {            			
                        DSM_MemCpy(&iFatSymbol,pBuff,2);
                        if (iFatSymbol != 0)//(0xfff8 == iFatSymbol)
                        {
                            continue;
                        }                        
                    }

                    D((DL_FATERROR, "in fat_HasFormated(), iFatSymbol(0x%x) error \n", iFatSymbol));
                    iRet = _ERR_FAT_NOT_FORMAT;
                    break;
                }    

                iSecOff += iFatSz;			
                iFatIndex ++;
            }
        }
        else
        {
            D((DL_FATWARNING, "in fat_HasFormated The fat type not support!iFATn = %d.\n",pFatSB->iFATn));
            iRet = _ERR_FAT_NOT_SUPPORT;
        }
    } 
    else    
    {
        if(_ERR_FAT_BPB_ERROR == iResult)
        {
            iRet = _ERR_FAT_NOT_FORMAT;
        }
        else
        {
            D((DL_FATERROR, "in fat_HasFormated fat_read_sb_info failed!iResult = %d.\n",iResult));
            iRet = iResult;
        }
    }  
	
    if(NULL != pBuff)
    {        
        FAT_SECT_BUF_FREE((SECT_BUF*)pBuff);
    }
    
    if(sSB.fat_fsi != NULL)
    {
        DSM_Free(sSB.fat_fsi);
    }
    
    return iRet;
}



//********************************************************************************
// Function:
//   This function is format the logistic disk.
// Parameter:
//   size[in], the disk max size for byte.
//   vollab[in],point to the string of the voluem lable.
//   oemname[in],Point to the string of OEM name.
//   opt[in], The option of format, 0:Quickly;1:Normal.
// Return value:
//      _ERR_FAT_SUCCESS indicates a successful operation. 
//         And unsuccessful operation is denoted by the err code as following.
//      _ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      _ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      _ERR_FAT_MALLOC_FAILED       allocte memory failed.
//*******************************************************************************

INT32 fat_format(UINT32 uDevNo, UINT32 iDiskTotSec,UINT8* vollab,UINT8* oemname,UINT8 opt)
{   
    UINT8 vollab_len;
    UINT8 oemname_len;
    UINT32 iDataSecCount;
    UINT32 iSecOff = 0;
    UINT32 iFatSz = 0;
    UINT32 iFatIndex = 0;
    UINT32 i;
    FAT_BOOT_RECORD sFbr;
    UINT8* pBuff;
    UINT32 iFatType;
    UINT32 iDataClus;
    UINT16 iTrailSig;
    INT32 iRemainsSize;
    UINT32 iRemainsSec;
    UINT32 iRootDirSec;
    INT32 iResult;    
    INT32 iRet = _ERR_FAT_SUCCESS;
    UINT32 iBadClu = 0;
    FAT_CACHE* psFatCache = NULL;
    FAT_FSI sFsi;

    // Input parameter check.
    if((0 == iDiskTotSec) || ((0 != opt) &&(1 != opt)))
    {
         D( ( DL_FATERROR, "in fat_format, iDiskTotSec = %d,opt = %d.\n",iDiskTotSec,opt) ); 
        return _ERR_FAT_PARAM_ERROR;
    }   
    if(NULL == vollab || NULL == oemname)
    {
        D( ( DL_FATERROR, "in fat_format, vollab = %d,oemname = 0x%x.\n",vollab,oemname) ); 
        return _ERR_FAT_PARAM_ERROR;
    }
	
    // Malloc the buffer to stored the fbr content.
    pBuff = FAT_SECT_BUF_ALLOC();   
    if(NULL == pBuff)
    {
        D(( DL_FATERROR,"in fat_format,1.FAT_SECT_BUF_ALLOC() failed.")); 
        DSM_ASSERT(0,"in fat_format,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }      
    // Clean up the cache of FAT if the cache device is formated device.
    psFatCache = fat_get_fat_cache(uDevNo);
    if(NULL == psFatCache)
    {
        D((DL_FATERROR, "fat_format: fat cache is null,s_dev = 0x%x.\n", uDevNo));
        return _ERR_FAT_CACHE_IS_NULL;
    }
    else
    {
       psFatCache->iDirty = 0;
       psFatCache->iSecNum = 0;
       psFatCache->psFileInodeList = NULL;
    }
    //DRV_SET_WCACHE_SIZE(uDevNo,DRV_DEF_CACHE_BLOCK_CNT*DEFAULT_SECSIZE*16);
    // Get FBR and other format parameter.
    vollab_len = (UINT8)DSM_StrLen(vollab);
    oemname_len = (UINT8)DSM_StrLen(oemname);    
    iFatType = 0;
    iDataClus = 0;
    fat_GetFormatInfo(iDiskTotSec,
                vollab,
                vollab_len,
                oemname,
                oemname_len,
                &sFbr,
                &iFatType,
                &iDataClus,
                &iRootDirSec
		);
    D( ( DL_FATDETAIL,"in fat_format() fat_GetFormatInfo return,iFatType = %d,iDataClus = %d,iRootDirSec = %d.:\n",
        iFatType,
        iDataClus,
        iRootDirSec));
  
    D( ( DL_FATDETAIL,"sFbr:BPB_HiddSec = %d,BPB_TotSec32 = %d,BPB_BytesPerSec = %d,BPB_RsvdSecCnt = %d,BPB_RootEntCnt = %d.\n",
        sFbr.BPB_HiddSec,
        sFbr.BPB_TotSec32,
        sFbr.BPB_BytesPerSec,
        sFbr.BPB_RsvdSecCnt,
        sFbr.BPB_RootEntCnt
        ));
    D( ( DL_FATDETAIL,"BPB_TotSec16 = %d,BPB_FATSz16 = %d,BPB_SecPerTrk = %d,BPB_NumHeads = %d,BPB_SecPerClus = %d,BPB_NumFATs = %d.\n",
        sFbr.BPB_TotSec16,
        sFbr.BPB_FATSz16,
        sFbr.BPB_SecPerTrk,
        sFbr.BPB_NumHeads,
        sFbr.BPB_SecPerClus,
        sFbr.BPB_NumFATs
       ));
    D( ( DL_FATDETAIL,"BPB_Media = %d.\n",
        sFbr.BPB_Media
        ));

    // Hide region
    DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);
    D((DL_FATDETAIL, "In fat_format() Write the Hide Region begin,iSecOff = %d.\n", iSecOff));
    for(i = 0; i < sFbr.BPB_HiddSec; i++)
    {        
         D((DL_FATDETAIL, "In fat_format() Write the Hide Sector,iSecOff = %d.\n", iSecOff));
        iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff, pBuff );        
        if(_ERR_FAT_SUCCESS != iResult)
        {
            D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_1 error,err code = %d.\n",iResult));
            iRet = _ERR_FAT_WRITE_SEC_FAILED;
            goto end;
        }
        else
        {
        	iSecOff ++;
        }
    }  
    D((DL_FATDETAIL, "In fat_format() Write the Hide Region end,iSecOff = %d.\n", iSecOff));


     // FAT Region
     
     // Set the FAT offset.
     iSecOff = sFbr.BPB_HiddSec + sFbr.BPB_RsvdSecCnt;    
    D((DL_FATDETAIL, "In fat_format() Write the FAT Region begin,iSecOff = %d.\n", iSecOff));

    // Get the count of remain setctor in FAT region.
    iFatSz = iFatType == FAT32? sFbr.u.sub_rec32.BPB_FATSz32 : sFbr.BPB_FATSz16;
    if(FAT32 == iFatType)
    {
        iRemainsSize = ((iFatSz * sFbr.BPB_BytesPerSec) - 8) - (iDataClus*4);
    }
    else if(FAT16 == iFatType)
    {
        iRemainsSize = ((iFatSz * sFbr.BPB_BytesPerSec) - 4) - (iDataClus*2);
    }
    else if(FAT12 == iFatType)
    {
        iRemainsSize = ((iFatSz * sFbr.BPB_BytesPerSec) - 3) - ((iDataClus*3)/2);
    }
    else
    {
        D((DL_FATERROR, "In fat_format() unknow fat type: %d.\n",iFatType));
        iRet = _ERR_FAT_NOT_SUPPORT;
        goto end;
    }

    iRemainsSize = iRemainsSize >= 0 ? iRemainsSize : 0;
    
    iRemainsSec = iRemainsSize/sFbr.BPB_BytesPerSec;

    iRemainsSize = (iRemainsSize % sFbr.BPB_BytesPerSec);

    // Write fat region.
    while(iFatIndex < sFbr.BPB_NumFATs)
    {
        // The first sector in FAT.
        // cluster 0-2 is reseved.
        DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);        
        if(iFatType == FAT32)
        {
            DSM_MemSet(pBuff, 0xff, 12);
            pBuff[0] = 0xf8;
            pBuff[3] = 0x0f;
            pBuff[7] = 0x0f;
            pBuff[11] = 0x0f;
        }
        else if(iFatType == FAT16)
        {
            DSM_MemSet(pBuff,0xff,4);
            pBuff[0]= 0xf8;  
        }
        else
        {
            DSM_MemSet(pBuff,0xff,3);
            pBuff[0]= 0xf8;                 
        }
        
        if(1 == iFatSz)
        {
            DSM_MemSet(pBuff + (DEFAULT_SECSIZE - iRemainsSize),0xff, (UINT16)iRemainsSize);
        }           

        D((DL_FATDETAIL, "In fat_format() Write the fat table first sector,iSecOff = %d,iFatIndex = %d.\n", iSecOff,iFatIndex));
        iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );
        if(_ERR_FAT_SUCCESS != iResult)
        {
            D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_2 error,return value = %d.\n",iResult));
            iRet = _ERR_FAT_WRITE_SEC_FAILED;
            goto end;
        }
        else
        {
        	iSecOff++;
        }
        
        // Write generic sectors in fat.
        for(i = 1; i < (iFatSz - iRemainsSec) && _ERR_FAT_SUCCESS == iResult; i++)
        {   
            D((DL_FATDETAIL, "In fat_format() Write the fat table,iSecOff = %d.\n", iSecOff));
            DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);
            if(i == iFatSz - iRemainsSec - 1)
            {
                DSM_MemSet(pBuff + (DEFAULT_SECSIZE - iRemainsSize),0xff, (UINT16)iRemainsSize);
            }
	    
            iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );            
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_3 error,err_code = %d.\n",iResult));
                iRet = _ERR_FAT_WRITE_SEC_FAILED;
                goto end;
            }
            else
            {
                 iSecOff ++;
            }
        }

        // Write remain sectors in fat.
        for(i = 0; i < iRemainsSec && _ERR_FAT_SUCCESS == iResult; i++)
        {         
            DSM_MemSet(pBuff,0xff,DEFAULT_SECSIZE);
            D((DL_FATDETAIL, "In fat_format() Write the fat table(remains),iSecOff = %d.\n", iSecOff));         
            iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );            
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_4 error,err_code = %d.\n",iResult));
                iRet = _ERR_FAT_WRITE_SEC_FAILED;
                goto end;
            }
            else
            {
                iSecOff ++;
            }
        }
        iFatIndex ++;
    } 
    D((DL_FATDETAIL, "In fat_format() Write the FAT Region end,iSecOff = %d.\n", iSecOff));

    // Root Directory Region(FAT16)
    D((DL_FATDETAIL, "In fat_format() Write the Root DIR Region begin,iSecOff = %d.\n", iSecOff));
    DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);
    
    for(i = 0; i < iRootDirSec; i++)
    {
        D((DL_FATDETAIL, "In fat_format() Write the Root DIR ,iSecOff = %d.\n", iSecOff));
        iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );      
        if(_ERR_FAT_SUCCESS != iResult)
        {
            D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_5 error,err_code = %d.\n",iResult));
            iRet = _ERR_FAT_WRITE_SEC_FAILED;
            goto end;    
        }
        else 
        {
             iSecOff++;
        }
    }
    D((DL_FATDETAIL, "In fat_format() Write the Root DIR Region end,iSecOff = %d.\n", iSecOff));

    // Data Region
    // SecCount include iDataSecCount and iRootSecCount.
    if(0 < sFbr.BPB_TotSec32)
    {
        iDataSecCount = sFbr.BPB_TotSec32 
                    - (sFbr.BPB_RsvdSecCnt + (sFbr.u.sub_rec32.BPB_FATSz32*sFbr.BPB_NumFATs))
                    - iRootDirSec; 
    }
    else
    {
        iDataSecCount = sFbr.BPB_TotSec16 
                    - (sFbr.BPB_RsvdSecCnt + (sFbr.BPB_FATSz16*sFbr.BPB_NumFATs))
                    - iRootDirSec; 
    }
    if(FS_FORMAT_NORMAL == opt)
    {        
        DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);
        D((DL_FATDETAIL, "In fat_format() Write the Data Region begin,iSecOff = %d.\n", iSecOff));        
        // Write the data region.
        for(i = 0; i < iDataSecCount; i++)
        {
            D((DL_FATDETAIL, "iSecOff = %d.\n", iSecOff));    
            if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_FLASH)
            {
                 iResult = DRV_BLOCK_REVERT( uDevNo, iSecOff + sFbr.BPB_HiddSec);        
            }
            else
            {
                  iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );
            }
            if(_ERR_FAT_SUCCESS != iResult)
            {
                if ((i / sFbr.BPB_SecPerClus) + 2 == iBadClu)
                {
                    continue;
                }
                else
                {
                    iBadClu = (i / sFbr.BPB_SecPerClus) + 2;
                }
                
                // Set current custer is bad.
                D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_6 error(%d), we will set cluster(%d) is bad.\n", iResult, iBadClu));
                iResult = fat_format_set_fat_entry(uDevNo, iFatType, &sFbr, iBadClu, CLUSTER_BAD, pBuff);
                if (iResult != _ERR_FAT_SUCCESS)
                {
                    iRet = iResult;
                    goto end;
                }
                
                // If the badclu is root custer, change the root custer to the next.
                if (FAT32 == iFatType && iBadClu == sFbr.u.sub_rec32.BPB_RootClus)
                {                    
                    sFbr.u.sub_rec32.BPB_RootClus++;
                    if (sFbr.u.sub_rec32.BPB_RootClus >= iDataClus + 2)
                    {
                        D((DL_FATERROR, "in fat_format can not change BPB_RootClus\n"));
                        iRet = _ERR_FAT_WRITE_SEC_FAILED;
                        goto end;
                    }
                }
                // The pBuff is used by set fat entry,so reset it.
                DSM_MemSet(pBuff, 0x00, DEFAULT_SECSIZE);                
            }

            iSecOff++;
        }
        D((DL_FATDETAIL, "In fat_format() Write the Data Region end,iSecOff = %d.\n", iSecOff));
    }

    // Clean up the custer of root directory.
    if (iFatType == FAT32 && FS_FORMAT_QUICK== opt)
    { 
        DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);
        for(i = 0; i < sFbr.BPB_SecPerClus; i++)
        {
            D((DL_FATDETAIL, "in fat_foramt Claer up the cluster 2 ,iSecOff = %d.\n", iSecOff));
            iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );           
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_7 error,err_code = %d.\n",iResult));
                iRet = _ERR_FAT_WRITE_SEC_FAILED;
                goto end;    
            }
            else 
            {
                iSecOff++;
            }
        }
    }

   // Set the custer of root directory.
    if (FAT32 == iFatType)
    {
        iResult = fat_format_set_fat_entry(uDevNo, iFatType, &sFbr, sFbr.u.sub_rec32.BPB_RootClus, CLUSTER_NULL, pBuff);
        if (iResult != _ERR_FAT_SUCCESS)
        {
            iRet = iResult;
            goto end;
        }
    }

    // Reserved Region.        
    if(FAT32 == iFatType)
    {
         for(i = sFbr.BPB_RsvdSecCnt; i > 0 ; i--)
         {
               iSecOff = sFbr.BPB_HiddSec + i - 1;
               DSM_MemSet(pBuff,0,DEFAULT_SECSIZE);
               // Reseved sector.
               if(((i - 1)%sFbr.u.sub_rec32.BPB_BkBootSec) > 2)
               { 
                   // Do nothing.
               }
               // The third sector in reserved regin.
               else if((i - 1) < (UINT32)(2*sFbr.u.sub_rec32.BPB_BkBootSec) &&
                          ((i - 1)%sFbr.u.sub_rec32.BPB_BkBootSec) == 2)
               {
                   *(pBuff + DEFAULT_SECSIZE - 2) = 0x55;
                   *(pBuff + DEFAULT_SECSIZE - 1) = 0xAA;
               }
               // The FSI sector.generally,the second sector is FSI sector in reserved regin .
               else if((i - 1) < (UINT32)(2*sFbr.u.sub_rec32.BPB_BkBootSec) &&
                         ((i - 1)%sFbr.u.sub_rec32.BPB_BkBootSec) == sFbr.u.sub_rec32.BPB_FSInfo)
               {                   
                   fat_set_fsinfo(&sFsi,iDataClus);
                   FSI2Buf(&sFsi,pBuff);
               }
               // FBR. The first sector in reserved regin.
               else if((i - 1) < (UINT32)(2*sFbr.u.sub_rec32.BPB_BkBootSec) &&
                         ((i - 1)%sFbr.u.sub_rec32.BPB_BkBootSec) == 0)
               {
                   FBR2Buf(&sFbr,pBuff);    
                   iTrailSig = FAT_FSI_TRAILSIG;
                   DSM_MemCpy(pBuff + (DEFAULT_SECSIZE - SIZEOF(UINT16)),&iTrailSig,SIZEOF(UINT16));
               }               
               
               D((DL_FATDETAIL, "In fat_format() Write the Rsv Sector,iSecOff = %d.\n", iSecOff));
               iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );            
               if(_ERR_FAT_SUCCESS != iResult)
               {
                   D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_1 error,err_code = %d.\n",iResult));
                   iRet = _ERR_FAT_WRITE_SEC_FAILED;
                   goto end;
               }
         }
    }   
    else
    {     
           iSecOff = sFbr.BPB_HiddSec;
           FBR2Buf(&sFbr,pBuff);    
           iTrailSig = FAT_FSI_TRAILSIG;
           DSM_MemCpy(pBuff + (DEFAULT_SECSIZE - SIZEOF(UINT16)),&iTrailSig,SIZEOF(UINT16));
           D((DL_FATDETAIL, "In fat_format() Write the Rsv Sector,iSecOff = %d.\n", iSecOff));
           iResult = DRV_BLOCK_WRITE( uDevNo, iSecOff + sFbr.BPB_HiddSec, pBuff );            
           if(_ERR_FAT_SUCCESS != iResult)
           {
               D((DL_FATERROR, "in fat_format DRV_BLOCK_WRITE_1 error,err_code = %d.\n",iResult));
               iRet = _ERR_FAT_WRITE_SEC_FAILED;
               goto end;
           }
    }
    D((DL_FATDETAIL, "In fat_format() Write other reserved Region end,iSecOff = %d.\n", iSecOff));    

end:
    if(NULL != pBuff)
    {
        FAT_SECT_BUF_FREE((SECT_BUF*)pBuff);
    }
    D((DL_FATDETAIL, "In fat_format() befor return, iSecOff = %d\n", iSecOff));
    return iRet;
}


INT32 fat_set_dentry(struct fat_sb_info* sb_info,UINT32 iClus,UINT8 iSec,UINT8 iLCnt)
{
    UINT8 *szBuff;//[DEFAULT_SECSIZE];
    INT32 iResult;
    UINT8* p = NULL;
    UINT32 i;

    if(iLCnt >= DEFAULT_SECSIZE/DEFAULT_DIRENTRYSIZE)
    {
        return _ERR_FAT_PARAM_ERROR;
    }

    szBuff = FAT_SECT_BUF_ALLOC();
    
    if(iClus == 0 && iSec < sb_info->iRootDirSecCnt)
    {
        iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, sb_info->iRootDirStartSec + iSec,szBuff);
    }
    else if(iClus >= 2 &&
    iClus <= sb_info->iDataSec/sb_info->iSecPerClus + 2 && 
    iSec < sb_info->iSecPerClus)
    {
        iResult = fat_read_cluster(sb_info,iClus,iSec,szBuff);             
    }
    else
    {
        iResult = _ERR_FAT_PARAM_ERROR;
    }

    if(ERR_SUCCESS == iResult)
    {
        p = szBuff + DEFAULT_SECSIZE - DEFAULT_DIRENTRYSIZE;
        for(i = 0; i < iLCnt; i++)
        {
            *p = 0xe5;
            p -= DEFAULT_DIRENTRYSIZE;               
        }
        if(iClus == 0 && iSec < sb_info->iRootDirSecCnt)
        {
            iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev, sb_info->iRootDirStartSec + iSec,szBuff);
        }
        else if(iClus >= 2 &&
                iClus <= sb_info->iDataSec/sb_info->iSecPerClus + 2 && 
                iSec < sb_info->iSecPerClus)
        {
            iResult = fat_write_cluster(sb_info,iClus,iSec,szBuff);             
        }    
    }
    FAT_SECT_BUF_FREE(szBuff);
    return iResult;
}


INT32 fat_scan_disk(UINT32 uDevNo)
{
    FAT_SCAN_STATE scan_state;
    INT32  iRet = _ERR_FAT_SUCCESS;
    INT32 iResult;
    SEARCH_DIR* pFatDir = NULL;   
    UINT8* pFat = NULL;
    UINT8* p = NULL;
    struct super_block sb;    
    struct fat_sb_info* sb_info;
    UINT32 iClusCount;
    UINT32 iClus = 0x0;    
    FAT_DIR_ENTRY sFDE;
    UINT32 iNextClus;    
    INT32 d = 0;    // dir level
    UINT32 n = 0;   // fat item array  index.         

    sb.s_dev = uDevNo;
    iResult = fat_read_sb_info(&sb);
    if(_ERR_FAT_SUCCESS != iResult)
    {
        D((DL_FATERROR, "in fat_scan_disk fat_read_sb_info error, ErrorCode = %d\n",iResult));
        return iResult;
    }
    sb_info = &(sb.u.fat_sb);     
    iClusCount = sb_info->iDataSec/sb_info->iSecPerClus + 2;

    pFat = DSM_MAlloc(iClusCount);
    if(NULL == pFat)
    {
        D((DL_FATERROR, "in fat_scan_disk DSM_MAlloc error, malloc failed\n"));        
        DSM_ASSERT(0,"fat_scan_disk: 1.malloc(0x%x) failed.",iClusCount); 
        return _ERR_FAT_MALLOC_FAILED; 
    }
    DSM_MemSet(pFat,0,(UINT16)iClusCount);

    CSW_TRACE(DL_FATERROR, "in fat_scan_disk uDevNo = %d,iClusCount = 0x%x.",uDevNo,iClusCount);

           
       pFatDir = DSM_MAlloc((sizeof(SEARCH_DIR)*FS_NR_DIR_LAYERS_MAX));
    if(NULL == pFatDir)
    {
        D((DL_FATERROR, "in fat_scan_disk DSM_MAlloc error, malloc failed\n"));        
        DSM_ASSERT(0,"fat_scan_disk: 2.malloc(0x%x) failed.",(sizeof(SEARCH_DIR)*FS_NR_DIR_LAYERS_MAX)); 
        return _ERR_FAT_MALLOC_FAILED; 
    }
    DSM_MemSet(pFatDir,0,(SIZEOF(SEARCH_DIR)*FS_NR_DIR_LAYERS_MAX));

    d  = 0;
    pFatDir[d].iClus = 0;
    pFatDir[d].iSec = (UINT8)0;
    pFatDir[d].iEOff = 0;
    pFatDir[d].eCluState = CLUS_STATE_FST;
    pFatDir[d].iIsRead = 1;
    pFatDir[d].iIsDirty = 0;
    pFatDir[d].iCurSec = 0xff;
    pFatDir[d].iCurClus =0xffffffff;
    pFatDir[d].iLCnt = 0;
    pFatDir[d].iClusCnt = 0;
    pFatDir[d].iSecCnt = 0;
    pFatDir[d].iPreClus = 0xffffffff;
    pFatDir[d].iPreSec = 0xff;    
    scan_state = FAT_SCAN_DIR;    

    while(d >= 0)
    {        
        if(d >= FS_NR_DIR_LAYERS_MAX)
        {
            D((DL_FATERROR, "fat_scan_disk: path too deep, break!d = %d >= 16",d));
            break;
        }

        if(1 == pFatDir[d].iIsRead &&
        (pFatDir[d].iCurClus != pFatDir[d].iClus ||
        pFatDir[d].iCurSec != pFatDir[d].iSec) )
        {
            // write dirty cluster or block.
            if(1 == pFatDir[d].iIsDirty)
            {
                if(2 <= pFatDir[d].iCurClus)
                {
                    if(pFatDir[d].iCurClus < iClusCount &&
                    pFatDir[d].iCurSec < sb_info->iSecPerClus)
                    {
                        iResult = fat_write_cluster(sb_info,
                                                pFatDir[d].iCurClus,
                                                (UINT8)(pFatDir[d].iCurSec), 
                                                pFatDir[d].szBuff);
                    }
                    else
                    {
                        D((DL_FATERROR, "fat_scan_disk: flush1 cluster is out ,iCurClus = 0x%x,iCurSec = 0x%x,d = %d.",
                            pFatDir[d].iCurClus,
                            pFatDir[d].iCurSec,
                            d));
                            iResult = 0;                     
                    }                    
                }
                else
                {                    
                    if(pFatDir[d].iCurSec < sb_info->iRootDirSecCnt)
                    {
                        iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev,
                        sb_info->iRootDirStartSec + pFatDir[d].iCurSec,
                        pFatDir[d].szBuff);                
                    }
                    else
                    {
                        D((DL_FATERROR, "fat_scan_disk: flush1 sec is out ,iCurClus = 0x%x,iCurSec = 0x%x,d = %d.",
                        pFatDir[d].iCurClus,
                        pFatDir[d].iCurSec,
                        d));
                        iResult = _ERR_FAT_WRITE_SEC_FAILED;
                    }
                }
                if(_ERR_FAT_SUCCESS != iResult)
                {  
                    D((DL_FATERROR, "fat_scan_disk: write block or custer  fail, iResult = %d,iCurClus = 0x%x,iCurSec = 0x%x,d = %d",
                        iResult,
                        pFatDir[d].iCurClus,
                        pFatDir[d].iCurSec,
                        d));
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    break;
                }
                else
                {
                    pFatDir[d].iIsDirty = 0;
                    CSW_TRACE(DL_FATERROR, "fat_scan_disk: write block or custer  ok, iResult = %d,iCurClus = 0x%x,iCurSec = 0x%x,d = %d",
                                iResult,
                                pFatDir[d].iCurClus,
                                pFatDir[d].iCurSec,
                                d);
                }
            }

            DSM_MemSet(pFatDir[d].szBuff,0,DEFAULT_SECSIZE);
            if(2 <= pFatDir[d].iClus)
            {
                if(pFatDir[d].iClus < iClusCount &&
                pFatDir[d].iSec < sb_info->iSecPerClus)
                {
                    iResult = fat_read_cluster(sb_info,
                                            pFatDir[d].iClus,
                                            (UINT8)(pFatDir[d].iSec), 
                                            pFatDir[d].szBuff);
                }
                else
                {
                D((DL_FATERROR, "fat_scan_disk: custer  out,iClus = 0x%x,iSec = 0x%x,d = %d",
                    iResult,
                    pFatDir[d].iClus,
                    d));
                iResult = _ERR_FAT_READ_SEC_FAILED;                 
                }
            }
            else
            {                    
                if(pFatDir[d].iSec < sb_info->iRootDirSecCnt)
                {
                    iResult = DRV_BLOCK_READ( sb_info->sb->s_dev,
                                            sb_info->iRootDirStartSec + pFatDir[d].iSec,
                                            pFatDir[d].szBuff);                
                }
                else
                {
                    D((DL_FATERROR, "fat_scan_disk: sec  out,iClus = 0x%x,iSec = 0x%x,d = %d",
                    iResult,
                    pFatDir[d].iClus,
                    d));
                    iResult = _ERR_FAT_READ_SEC_FAILED;
                }
            }
            if(_ERR_FAT_SUCCESS != iResult)
            {  
                D((DL_FATERROR, "fat_scan_disk: read block or custer  error, iResult = %d,iClus = 0x%x,iSec = 0x%x,d = %d",
                    iResult,
                    pFatDir[d].iClus,
                    pFatDir[d].iSec,
                    d));
                iRet = _ERR_FAT_READ_SEC_FAILED;
                break;
            }
            else
            {
                pFatDir[d].iIsDirty = 0;
                pFatDir[d].iCurClus = pFatDir[d].iClus;
                pFatDir[d].iCurSec = pFatDir[d].iSec;
                pFatDir[d].iEOff = 0;
            }
            pFatDir[d].iIsRead = 0;            
        }       

        switch(scan_state)
        {                      
        case FAT_SCAN_DIR:
            p = pFatDir[d].szBuff + pFatDir[d].iEOff;             
            // Look up short dir entry.get the first cluster number.
            // long,delete,jap_delete or dot entry,move to next dir entry.
            // free entry,goto parent dir.           

            if(pFatDir[d].iEOff >= sb_info->iBytesPerSec)
            {
                if(pFatDir[d].iClus == 0)
                {
                    pFatDir[d].iPreSec = pFatDir[d].iSec;
                    if((UINT32)(pFatDir[d].iSec) == sb_info->iRootDirSecCnt)
                    {
                        d--;
                        CSW_TRACE(DL_FATERROR, "fat_scan_disk:FAT_SCAN_DIR: is last sec,d = %d.",d);
                    }
                    else
                    {
                        pFatDir[d].iSec++;
                        pFatDir[d].iIsRead = 1;                    
                    }
                }
                else
                {
                    pFatDir[d].iPreSec = pFatDir[d].iSec;
                    pFatDir[d].iPreClus = pFatDir[d].iClus;                   
                    if(pFatDir[d].iSec + 1 == sb_info->iSecPerClus)
                    {
                        if(CLUS_STATE_LAST == pFatDir[d].eCluState) // is last clust. 
                        {
                            d--;
                            // CSW_TRACE(DL_FATERROR, "fat_scan_disk:FAT_SCAN_DIR: is last clust,d = %d.",d);
                        }
                        else
                        {  
                            pFatDir[d].iIsRead = 1;
                            pFatDir[d].iIsGetNextClus = 1;
                        }
                    }
                    else
                    {
                        pFatDir[d].iSec ++;
                        pFatDir[d].iEOff = 0;
                        pFatDir[d].iIsRead = 1;
                    }
                }
                break;
            }

            if(FAT_IS_FREE_ENTRY(p))
            {
                        if(pFatDir[d].eCluState != CLUS_STATE_LAST && pFatDir[d ].iClus >= 2 && pFatDir[d ].iClus < iClusCount)
                        {

                            n = pFatDir[d ].iClus;
                            iNextClus = 0;
                            iResult = fat_get_next_cluster(sb_info,pFatDir[d].iClus,&iNextClus);                    
                            if(ERR_SUCCESS == iResult)
                            {
                                if(iNextClus >= 2 && iNextClus< iClusCount)
                                {
                                    pFat[n] = 2;
                                    D((DL_FATERROR, "fat_scan_disk: Find free entry, set clus to ~0,iClus = 0x%x,sec = 0x%x,eoff = 0x%x,d = %d.",pFatDir[d ].iClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d));
                                }
                            }
                        }
                d--;
            }
            else if(FAT_IS_SHORT_ENTRY(p))
            {
                if(FAT_IS_JAP_DELETE_ENTRY(p))
                {
                    pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;
                }
                else if('.' == *p)
                {
                    pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;                 
                }
                else
                {                                                    
                    iClus = FAT_GET_DIR_FSTCLUSLO(p);    
                    if(iClus >= 2 && iClus < iClusCount)
                    {
                        if(0 == pFat[iClus])
                        {
                            pFat[iClus] = 1;                         
                            pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;  
                            pFatDir[d].iLCnt = 0;
                            d++;
                            pFatDir[d].iClus = iClus;
                            pFatDir[d ].iSec = 0;
                            pFatDir[d ].iEOff = 0;

                            pFatDir[d ].eCluState = CLUS_STATE_FST;    
                            pFatDir[d ].iLCnt = 0;                        
                            pFatDir[d ].iClusCnt = 1;
                            pFatDir[d ].iPreClus = 0xffffffff;
                            pFatDir[d ].iPreSec = 0xff;
                            Buf2FDE(p,&sFDE);
                            if(FAT_IS_DIR_ENTRY(p))
                            {
                                pFatDir[d].iIsRead = 1;
                                scan_state = FAT_SCAN_DIR;                
                            }
                            else
                            {                
                                pFatDir[d].iIsRead = 0;
                                pFatDir[d].iIsGetNextClus = 1;
                                scan_state = FAT_SCAN_FILE;
                            }
                        }
                        else
                        {
                            D((DL_FATERROR, "fat_scan_disk:FAT_SCAN_DIR:file fstclus is used repeatedly,fst_clus = 0x%x,sec = 0x%x,eoff = 0x%x,d = %d.",pFatDir[d].iClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d));                        
                            *p = 0xe5;
                            p -= DEFAULT_DIRENTRYSIZE;
                            for(;pFatDir[d].iLCnt > 0 && p  >= pFatDir[d].szBuff; pFatDir[d].iLCnt--)
                            {
                                *p = 0xe5;
                                p -= DEFAULT_DIRENTRYSIZE;                            
                            }
                            if(pFatDir[d].iLCnt > 0)
                            {
                                // todo: delete long entries on pre-cluster or pre-sector.
                                fat_set_dentry(sb_info,pFatDir[d].iPreClus,(pFatDir[d].iPreSec),pFatDir[d].iLCnt);
                                pFatDir[d].iLCnt = 0;
                            }
                            pFatDir[d].iIsDirty = 1;
                            pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;  
                        } 
                    }
                    else
                    {
                        D((DL_FATERROR, "fat_scan_disk:file:entry fstclus is error,fst_clus = 0x%x,sec = 0x%x,eoff = 0x%x,remove the dir entry,d = %d.",iClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d));
                        *p = 0xe5;
                        p -= DEFAULT_DIRENTRYSIZE;
                        for(;pFatDir[d].iLCnt > 0 && p  >= pFatDir[d].szBuff; pFatDir[d].iLCnt--)
                        {
                            *p = 0xe5;
                            p -= DEFAULT_DIRENTRYSIZE;                            
                        }
                        if(pFatDir[d].iLCnt > 0)
                        {
                            // todo: delete long entries on pre-cluster or pre-sector.
                            fat_set_dentry(sb_info,pFatDir[d].iPreClus,pFatDir[d].iPreSec,pFatDir[d].iLCnt);
                            pFatDir[d].iLCnt = 0;
                        }
                        pFatDir[d].iIsDirty = 1;
                        pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;  
                    }                                
                }            
            } 
            else // long dir entry.
            {
                pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;
                pFatDir[d].iLCnt ++;          
            }
            break;     
        case FAT_SCAN_FILE:
            if(CLUS_STATE_LAST == pFatDir[d ].eCluState)
            {                       
                if(pFatDir[d ].iClusCnt*sb_info->iBytesPerSec*sb_info->iSecPerClus < sFDE.DIR_FileSize)
                {
                       D((DL_FATERROR, "in fat_scan_disk set sFDE.DIR_FileSize = 0x%x to 0x%x.",
                           sFDE.DIR_FileSize,
                           pFatDir[d ].iClusCnt*sb_info->iBytesPerSec*sb_info->iSecPerClus
                           ));
                    sFDE.DIR_FileSize = pFatDir[d ].iClusCnt*sb_info->iBytesPerSec*sb_info->iSecPerClus;                     
                    FDE2Buf(&sFDE,p);
                    d--;
                    pFatDir[d].iIsDirty = 1;                    
                }        
                else
                {
                    d--;
                }
                scan_state = FAT_SCAN_DIR;
            }
            else
            {
                pFatDir[d].iIsGetNextClus = 1;
            }
        break;
        case FAT_SCAN_END:
            d = -1;
            iRet = ERR_SUCCESS;           
            break;
            case FAT_SCAN_ERROR:
            d = -1;
            D((DL_FATERROR, "fat_scan_disk case FAT_SCAN_ERROR"));
            break;
            default:
            D((DL_FATERROR, "fat_scan_disk case default"));
            d = -1;
            break;
        }

        if(d >= 0)
        {            
            if(1 == pFatDir[d].iIsGetNextClus)
            {
                switch(pFatDir[d].eCluState)
                {
                case CLUS_STATE_FST:       
                    //CSW_TRACE(DL_FATERROR, "fat_scan_disk:CLUS_STATE_FST:fst_clus = 0x%x,sec = 0x%x,eoff = 0x%x,d = %d.",pFatDir[d].iClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d);
                    pFatDir[d].eCluState = CLUS_STATE_NEXT;
                case CLUS_STATE_NEXT: 
                    if(!fat_is_last_cluster(sb_info,pFatDir[d].iClus))
                    {
                        iNextClus = 0;
                        iResult = fat_get_next_cluster(sb_info,pFatDir[d].iClus,&iNextClus);
                        if(ERR_SUCCESS == iResult)
                        {  
                            if(iNextClus >= 2 && iNextClus< iClusCount)
                            { 
                                n = iNextClus;
                                if(0 == pFat[n])
                                {                        
                                    pFatDir[d ].iPreClus = pFatDir[d].iClus;
                                    pFatDir[d].iClus = n;
                                    pFatDir[d ].iSec = 0;
                                    pFatDir[d ].iEOff = 0;                                              
                                    pFatDir[d ].iClusCnt++;
                                    pFat[n] = 1;                                    
                                    // CSW_TRACE(DL_FATERROR, "fat_scan_disk:CLUS_STATE_NEXT:next_clus = 0x%x,d = %d.",iNextClus,d);
                                }
                                else
                                {
                                    D((DL_FATERROR, "fat_scan_disk CLUS_STATE_NEXT: next_clus is used repeatedly,set it to ~0,next_clus = 0x%x,sec = 0x%x,eoff = 0x%x,d = %d.",iNextClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d));                        
                                    n = pFatDir[d].iClus;
                                    if(FAT12 == sb_info->iFATn)
                                    {
                                        pFatDir[d].iClus = 0xfff; 
                                    }
                                    else if(FAT16 == sb_info->iFATn)
                                    {
                                        pFatDir[d].iClus = 0xffff;
                                    }
                                    else
                                    {
                                        pFatDir[d].iClus = 0xffffffff;
                                    }
                                    pFatDir[d].iIsRead = 0;                                     
                                    pFatDir[d].eCluState = CLUS_STATE_LAST;
                                    pFat[n] = 2;
                                }
                            }
                            else if(fat_is_last_cluster(sb_info,iNextClus))
                            {
                                // CSW_TRACE(DL_FATERROR, "fat_scan_disk dir entry fstclus(0x%x)  is last.",pFatDir[d ].iClus);
                                pFatDir[d ].iClus = iNextClus;                                
                                pFatDir[d].iIsRead = 0; 
                                pFatDir[d].eCluState = CLUS_STATE_LAST;
                                // CSW_TRACE(DL_FATERROR, "fat_scan_disk:CLUS_STATE_NEXT: next_clus is last clus,next_clus = 0x%x,d = %d.",iNextClus,d);
                            }
                            else
                            {
                                D((DL_FATERROR, "fat_scan_disk:CLUS_STATE_NEXT: next_clus is invalid,set it to ~0,next_clus = 0x%x,sec = 0x%x,eoff = 0x%x,d = %d.",iNextClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d));
                                n = pFatDir[d ].iClus;
                                pFat[n] = 2;
                                if(FAT12 == sb_info->iFATn)
                                {
                                    pFatDir[d].iClus = 0xfff; 
                                }
                                else if(FAT16 == sb_info->iFATn)
                                {
                                    pFatDir[d].iClus = 0xffff;
                                }
                                else
                                {
                                    pFatDir[d].iClus = 0x0ffffffff;
                                }                                    
                                pFatDir[d].iIsRead = 0; 
                                pFatDir[d].eCluState = CLUS_STATE_LAST;
                            }
                        }
                        else
                        {
                            iRet =  _ERR_FAT_READ_SEC_FAILED;  
                            scan_state = FAT_SCAN_ERROR;
                        }                            
                    }
                    else
                    {
                        pFatDir[d].iIsRead = 0; 
                        pFatDir[d].eCluState = CLUS_STATE_LAST;
                    }
                    
                    break;
                case CLUS_STATE_LAST:   
                    CSW_TRACE(DL_FATERROR, "fat_scan_disk:CLUS_STATE_LAST: clus = 0x%x,sec = 0x%x,eoff = 0x%x,d = %d.",pFatDir[d ].iClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d);
                    break;
                default:
                    D((DL_FATERROR, "fat_scan_disk:eCluState is default: clus = 0x%x,sec = 0x%x,eoff = 0x%x,d = %d.",pFatDir[d ].iClus,pFatDir[d ].iSec,pFatDir[d ].iEOff,d));
                    break;
                    
                }
                pFatDir[d].iIsGetNextClus = 0;
            }
        }
    }
    if(iResult == ERR_SUCCESS)
    {
        for(d = 0; d < FS_NR_DIR_LAYERS_MAX; d++)
        {
            if(1 == pFatDir[d].iIsDirty)
            {
                if(2 <= pFatDir[d].iCurClus)
                {
                    if(pFatDir[d].iCurClus < iClusCount)
                    {
                        iResult = fat_write_cluster(sb_info,
                                                pFatDir[d].iCurClus,
                                                (UINT8)(pFatDir[d].iCurSec), 
                                                pFatDir[d].szBuff);
                    }
                    else
                    {
                        D((DL_FATERROR, "fat_scan_disk: flush2 cluster is out ,iCurClus = 0x%x,iCurSec = 0x%x,d = %d",
                            pFatDir[d].iCurClus,
                            pFatDir[d].iCurSec,
                            d));
                        iResult = _ERR_FAT_WRITE_SEC_FAILED;
                    }
                }
                else
                {                    
                    if(pFatDir[d].iCurSec < sb_info->iRootDirSecCnt)
                    {
                        iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev,
                        sb_info->iRootDirStartSec + pFatDir[d].iCurSec,
                        pFatDir[d].szBuff);                
                    }
                    else
                    {
                        D((DL_FATERROR, "fat_scan_disk: flush2 sec is out ,iCurClus = 0x%x,iCurSec = 0x%x,d = %d",
                        pFatDir[d].iCurClus,
                        pFatDir[d].iCurSec,
                        d));
                        iResult = _ERR_FAT_WRITE_SEC_FAILED;
                    }
                }
                if(_ERR_FAT_SUCCESS != iResult)
                {  
                    D((DL_FATERROR, "fat_scan_disk: flush2 block or custer  fail, iResult = %d,iCurClus = 0x%x,iCurSec = 0x%x,d = %d",
                        iResult,
                        pFatDir[d].iCurClus,
                        pFatDir[d].iCurSec,
                        d));
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    break;
                }
                else
                {
                    pFatDir[d].iIsDirty = 0;
                    CSW_TRACE(DL_FATERROR, "fat_scan_disk: flush2 block or custer  ok, iResult = %d,iCurClus = 0x%x,iCurSec = 0x%x,d = %d",
                                iResult,
                                pFatDir[d].iCurClus,
                                pFatDir[d].iCurSec,
                                d);
                }
            }
        }
        iResult = fat_set_fat_entry_all(sb_info,pFat,iClusCount);
        iRet = iResult;                   
    }
    if(NULL != pFat)
    {
        DSM_Free(pFat);
        pFat = NULL;
    }

    if(NULL != pFatDir)
    {
        DSM_Free(pFatDir);
        pFatDir = NULL;
    }
    if(sb.fat_fsi != NULL)
    {
        DSM_Free(sb.fat_fsi);
    }

    return iRet;
}

INT32 fat_set_fat_entry_all(FAT_SB_INFO *sb_info, UINT8* pFat,UINT32 iClusCount)
{
    UINT32 iCurClu = 2;
    UINT32 iValue = 0;
    INT32 iResult;
    FAT_CACHE *psFatCache = NULL;
    UINT32 iTstFreeClusCnt = 0;
    UINT32 iTstUsedClusCnt = 0;
    // UINT32 i;
    
    for(iCurClu = 2; iCurClu < iClusCount ; iCurClu++)
    {       
        if(0 == pFat[iCurClu])
        {           
            iResult = fat_get_fat_entry_value(sb_info,iCurClu,&iValue);
            if(ERR_SUCCESS == iResult)
            {   
                if(iValue != CLUSTER_FREE)
                {
                    iResult = fat_set_fat_entry_value(sb_info,iCurClu,CLUSTER_FREE,FALSE,NULL);            
                    if(ERR_SUCCESS == iResult)
                    {
                        CSW_TRACE(DL_FATERROR, "fat_set_fat_entry_all: set Cluster(0x%x) to 0 ok.\n",iCurClu);
                        iTstFreeClusCnt ++;
                    }
                    else
                    {
                        D((DL_FATERROR, "fat_set_fat_entry_all: set Cluster(0x%x) to 0 fail.\n",iCurClu));
                    }
                }
                else
                {          
                    iTstFreeClusCnt ++;
                }
            }
            else
            {
                D((DL_FATERROR, "fat_set_fat_entry_all: 1.get  Cluster(0x%x) value fail.\n",iCurClu));
            }            
        }
        else if(2 == pFat[iCurClu])
        {                      
            iResult = fat_get_fat_entry_value(sb_info,iCurClu,&iValue);
            if(ERR_SUCCESS == iResult)
            {            
                if(!fat_is_last_cluster(sb_info,iValue))
                {
                    iResult = fat_set_fat_entry_value(sb_info,iCurClu,CLUSTER_NULL,FALSE,NULL); 
                    if(ERR_SUCCESS == iResult)
                    {
                        CSW_TRACE(DL_FATERROR, "fat_set_fat_entry_all: set Cluster(0x%x) to ~0 ok.\n",iCurClu);
                        iTstUsedClusCnt++;
                    }
                    else
                    {                        
                        D((DL_FATERROR, "fat_set_fat_entry_all: set Cluster(0x%x) to ~0 fail.\n",iCurClu));
                    }
                }
                else
                {
                    //D((DL_FATERROR, "fat_set_fat_entry_all: the Cluster(0x%x) is ~0 already.\n",iCurClu));
                    iTstUsedClusCnt++;
                }
            }
            else
            {
                D((DL_FATERROR, "fat_set_fat_entry_all: 2.get  Cluster(0x%x) value fail.\n",iCurClu));
            }
            
        }
        else if(1 == pFat[iCurClu])
        {            
            
            iTstUsedClusCnt++;
        }                   
    }
    CSW_TRACE(DL_FATERROR, "fat_set_fat_entry_all: iTstFreeClusCnt(0x%x) +iTstUsedClusCnt(0x%x)  = 0x%x.",iTstFreeClusCnt,iTstUsedClusCnt, iTstFreeClusCnt+iTstUsedClusCnt);
    CSW_TRACE(DL_FATERROR, "fat_set_fat_entry_all: iClusCount = 0x%x.",iClusCount);
   // for(i = 0; i < iClusCount ; i += 32)
   // {
   // DSM_Dump(pFat + i,iClusCount -i >= 32 ? 32:iClusCount -i,8);
   // }
    psFatCache = fat_get_fat_cache(sb_info->sb->s_dev);
    if(NULL == psFatCache)
    {
        D((DL_FATERROR, "fat_set_fat_entry_all: fat cache is null,s_dev = 0x%x.\n", sb_info->sb->s_dev));
        return _ERR_FAT_CACHE_IS_NULL;
    }
    
    iResult = fat_synch_fat_cache(sb_info, psFatCache);
    if(_ERR_FAT_SUCCESS != iResult)
    {
        D( ( DL_FATERROR, "in fat_set_fat_entry_all,fat_synch_fat_cache() return = %d.\n",iResult) );
    }

    return ERR_SUCCESS;
}


INT32 fat_get_file_name(struct inode * inode, WCHAR *fileName)
{
    UINT32 bclu;
    UINT16 bsec, eoff;
    INT32 iRet;

    if (!inode || !fileName)
    {
        D( ( DL_FATERROR, "in fat_get_file_name, inode = %d,fileName = 0x%x.\n",inode,fileName) ); 
        return _ERR_FAT_PARAM_ERROR;
    }
    
    bclu = FAT_INO2CLUS(inode->i_ino);
    bsec = FAT_INO2SEC(inode->i_ino);
    eoff = FAT_INO2OFF(inode->i_ino);
    
    iRet = fat_get_name_for_entrys(&(inode->i_sb->u.fat_sb), fileName, bclu,  bsec, eoff);
    return iRet;
}


INT32 fat_get_dir_size(struct inode *inode,UINT64* pSize,BOOL IgnoreDir)
{
    FAT_SCAN_STATE scan_state;
    INT32  iRet = _ERR_FAT_SUCCESS;
    INT32 iResult;
    SEARCH_DIR* pFatDir = NULL;   
    UINT64 iSize = 0;
    UINT8* p = NULL;
    FAT_DIR_ENTRY sFDE;
    struct fat_sb_info* sb_info;
    UINT32 iClusCount;    
    UINT32 iNextClus;
    INT32 d = 0;    // dir level
    //UINT32 n = 0;   // fat item array  index.         

    *pSize = 0;
     pFatDir = DSM_MAlloc((sizeof(SEARCH_DIR)*FS_NR_DIR_LAYERS_MAX));
     if(NULL == pFatDir)
     {
        D((DL_FATERROR, "in fat_get_dir_size DSM_MAlloc error, malloc failed\n"));        
        DSM_ASSERT(0,"fat_get_dir_size: malloc(0x%x) failed.",(sizeof(SEARCH_DIR)*FS_NR_DIR_LAYERS_MAX)); 
        return _ERR_FAT_MALLOC_FAILED; 
    }
     
    DSM_MemSet(pFatDir,0,(SIZEOF(SEARCH_DIR)*FS_NR_DIR_LAYERS_MAX));
    
    sb_info = &(inode->i_sb->u.fat_sb);
    iClusCount = sb_info->iDataSec/sb_info->iSecPerClus + 2;    
    DSM_MemSet(pFatDir,0,(SIZEOF(SEARCH_DIR)*FS_NR_DIR_LAYERS_MAX));

    d  = 0;
    pFatDir[d].iClus = FAT_INO2CLUS(inode->i_ino);
    pFatDir[d].iSec = (UINT8)FAT_INO2SEC(inode->i_ino);
    pFatDir[d].iEOff = FAT_INO2OFF(inode->i_ino);
    pFatDir[d].eCluState = CLUS_STATE_FST;
    pFatDir[d].iIsRead = 1;
    scan_state = FAT_SCAN_DIR_BASE;
    
    
    while(d >= 0)
    {
        if(pFatDir[d].iClus != 0 && (pFatDir[d].iClus < 2 || pFatDir[d].iClus >= iClusCount))
        {
            break;
        }
        // Read cluster.
        if(1 == pFatDir[d].iIsRead) 
        {
            DSM_MemSet(pFatDir[d].szBuff,0,DEFAULT_SECSIZE);
            if(2 <= pFatDir[d].iClus)
            {
                if(pFatDir[d].iClus < iClusCount &&
                   pFatDir[d].iSec < sb_info->iSecPerClus)
                {                    
                    iResult = fat_read_cluster(sb_info,
                                       pFatDir[d].iClus,
                                       (UINT8)(pFatDir[d].iSec), 
                                       pFatDir[d].szBuff);
                 }
                 else
                 {
                     iResult = _ERR_FAT_READ_SEC_FAILED;                     
                 }
            }
            else
            {                    
                 if(pFatDir[d].iSec < sb_info->iRootDirSecCnt)
                 {
                    iResult = DRV_BLOCK_READ( sb_info->sb->s_dev,
                                          sb_info->iRootDirStartSec + pFatDir[d].iSec,
                                          pFatDir[d].szBuff);                
                 }
                 else
                 {
                      iResult = _ERR_FAT_READ_SEC_FAILED;  
                 }
            }
            if(_ERR_FAT_SUCCESS != iResult)
            {  
                D((DL_FATERROR, "fat_get_dir_size DRV_BLOCK_READ_0 error, iResult = %d\n",iResult));
                iRet = _ERR_FAT_READ_SEC_FAILED;
                break;
            }
            pFatDir[d].iIsRead = 0;            
        }       
        
     
        
        switch(scan_state)
        {
           case FAT_SCAN_DIR_BASE:             
            // Look up short dir entry.get the first cluster number.
            // long or delete, move to next dir entry.
            // free entry,jap_delete entry or dot entry is error.
            // the correct dir entry list as:
            // long entry
            // long entry
            // ...
            // short entry
            p = pFatDir[d].szBuff + pFatDir[d].iEOff;
            if(pFatDir[d].iEOff >= sb_info->iBytesPerSec)
            {
               if(pFatDir[d].iClus == 0)
               {
                    if((UINT32)(pFatDir[d].iSec + 1) == sb_info->iRootDirSecCnt)
                    {
                        d--;
                    }
                    else
                    {
                        pFatDir[d].iSec++;
                    }
               }
               else
               {
                   if(pFatDir[d].iSec + 1 == sb_info->iSecPerClus)
                   {
                       if(CLUS_STATE_LAST == pFatDir[d].eCluState) // is last clust. 
                       {
                            d--;
                       }
                       else
                       {                            
                            pFatDir[d].iIsRead = 1;
                            pFatDir[d].iIsGetNextClus = 1;
                       }
                   }
                   else
                   {
                       pFatDir[d].iSec ++;
                       pFatDir[d].iEOff = 0;
                       pFatDir[d].iIsRead = 1;
                   }
               }
               break;
            }
            
            if(FAT_IS_SHORT_ENTRY(p))
            {
                if(FAT_IS_JAP_DELETE_ENTRY(p))
                {
                    scan_state = FAT_SCAN_ERROR;
                }
                else if('.' == *p)
                {
                    scan_state = FAT_SCAN_ERROR;                 
                }
                else if(FAT_IS_DIR_ENTRY(p))
                { 
                    //pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;                  
                    pFatDir[d ].iClus = FAT_GET_DIR_FSTCLUSLO(p);
                    if(pFatDir[d ].iClus >= 2 && FALSE == IgnoreDir)
                    {
                        iSize += (sb_info->iBytesPerSec*sb_info->iSecPerClus);
                    }
                    pFatDir[d ].iSec = 0;
                    pFatDir[d ].iEOff = 0;
                    pFatDir[d ].eCluState = CLUS_STATE_FST;   
                    pFatDir[d].iIsRead = 1;
                    scan_state = FAT_SCAN_DIR;
                }                
                else
                {              
                   scan_state = FAT_SCAN_ERROR;
                }
            }            
            else if(FAT_IS_FREE_ENTRY(p))
            {
                scan_state = FAT_SCAN_ERROR;
            }
            else
            {                
                pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;
            }
            break; 
            
           case FAT_SCAN_DIR:
            // Look up short dir entry.get the first cluster number.
            // long,delete,jap_delete or dot entry,move to next dir entry.
            // free entry,goto parent dir.           
            p = pFatDir[d].szBuff + pFatDir[d].iEOff;
            if(pFatDir[d].iEOff >= sb_info->iBytesPerSec)
            {
               if(pFatDir[d].iClus == 0)
               {
                    if((UINT32)(pFatDir[d].iSec + 1) == sb_info->iRootDirSecCnt)
                    {
                        d--;
                    }
                    else
                    {
                        pFatDir[d].iSec++;
                    }
               }
               else
               {
                   if(pFatDir[d].iSec + 1 == sb_info->iSecPerClus)
                   {
                       if(CLUS_STATE_LAST == pFatDir[d].eCluState) // is last clust. 
                       {
                            d--;
                       }
                       else
                       {                            
                            pFatDir[d].iEOff = 0;
                            pFatDir[d].iSec = 0;         
                            pFatDir[d].iIsRead = 1;
                            pFatDir[d].iIsGetNextClus = 1;
                       }
                   }
                   else
                   {
                       pFatDir[d].iSec ++;
                       pFatDir[d].iEOff = 0;
                       pFatDir[d].iIsRead = 1;
                   }
               }
               break;
            }
            
            if(FAT_IS_FREE_ENTRY(p))
            {
                 d--;
            }
            else if(FAT_IS_SHORT_ENTRY(p))
            {
                if(FAT_IS_JAP_DELETE_ENTRY(p))
                {
                    pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;
                }
                else if('.' == *p)
                {
                    pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;                 
                }
                else if(FAT_IS_DIR_ENTRY(p))
                { 
                    pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;
                    d++;         
                    pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;
                    pFatDir[d ].iClus = FAT_GET_DIR_FSTCLUSLO(p);
                    if(pFatDir[d ].iClus >= 2 && FALSE == IgnoreDir)
                    {
                        iSize += (sb_info->iBytesPerSec*sb_info->iSecPerClus);
                    }
                    pFatDir[d ].iSec = 0;
                    pFatDir[d ].iEOff = 0;
                    pFatDir[d].iIsRead = 1;   
                    pFatDir[d ].eCluState = CLUS_STATE_FST;                   
                }                
                else 
                {              
                    Buf2FDE(p,&sFDE);
                    pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;                    
                    iSize += sFDE.DIR_FileSize;               
                }
            }            
            else
            {
                pFatDir[d].iEOff  += DEFAULT_DIRENTRYSIZE;
            }
            break;          
           case FAT_SCAN_END:
            d = -1;            
            iRet = ERR_SUCCESS;           
            break;
           case FAT_SCAN_ERROR:
            d = -1;
            D((DL_FATERROR, "fat_get_dir_size case FAT_SCAN_ERROR"));
            break;
           default:
            D((DL_FATERROR, "fat_get_dir_size case default"));
            d = -1;
            break;
        }
        
        
        if(1 == pFatDir[d].iIsGetNextClus)
        {
            switch(pFatDir[d].eCluState)
            {
                case CLUS_STATE_FST:       
                pFatDir[d].eCluState = CLUS_STATE_NEXT;
                case CLUS_STATE_NEXT: 
                    if(!fat_is_last_cluster(sb_info,pFatDir[d].iClus))
                    {
                        iNextClus = 0;
                        iResult = fat_get_next_cluster(sb_info,pFatDir[d].iClus,&iNextClus);
                        if(ERR_SUCCESS == iResult)
                        {
                            if(FALSE == IgnoreDir)
                            {
                                iSize += (sb_info->iBytesPerSec*sb_info->iSecPerClus);
                            }
                            pFatDir[d].iClus = iNextClus;
                            if(fat_is_last_cluster(sb_info,pFatDir[d].iClus))
                            {
                                pFatDir[d].eCluState = CLUS_STATE_LAST;
                                pFatDir[d].iIsRead = 0;
                            }
                        }
                        else
                        {
                            pFatDir[d].iIsRead = 0;
                            iRet =  _ERR_FAT_BPB_ERROR;
                            scan_state = FAT_SCAN_ERROR;
                        }
                            
                    }
                    else
                    {
                        pFatDir[d].iIsRead = 0;
                        pFatDir[d].eCluState = CLUS_STATE_LAST;
                    }
                    
                    break;
                case CLUS_STATE_LAST:
                    pFatDir[d].iIsRead = 0;
                    break;
                default:
                    D((DL_FATERROR, "fat_get_dir_size case iClusState = defualt."));
                    break;
                    
            }
            pFatDir[d].iIsGetNextClus = 0;
        }
        
    }
    
    if(iRet == ERR_SUCCESS)
    {
        *pSize = iSize;   
    }
    
    if(NULL != pFatDir)
    {
        DSM_Free(pFatDir);
        pFatDir = NULL;
    }
    return iRet;
}


