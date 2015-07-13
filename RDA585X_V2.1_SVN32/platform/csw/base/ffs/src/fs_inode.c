#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "dsm_cf.h"
#include "fat_base.h"
#include "fat_local.h"
#include "fs.h"
#include "fs_base.h"


#ifdef _FS_SIMULATOR_ 
#include <time.h>
#endif

extern struct super_block* g_fs_super_block_table;
//extern struct file g_fs_file_table[ FS_NR_OPEN_FILES_MAX ];
//extern struct inode g_fs_inode_table[ FS_NR_INODES_MAX ];

static void wait_on_inode( struct inode * inode )
{
    inode = inode;
}


#if 0
// the flowing function unused.
static void lock_inode( struct inode * inode )
{
    inode = inode;
}

static void unlock_inode( struct inode * inode )
{
    inode = inode;
}

struct inode * new_inode( struct super_block *sb )
{
    struct inode * inode;

    inode = get_empty_inode();

    if( NULL == inode) // get_empty_inode failed
    {
        return NULL;
    }
    inode->i_count = 1; //not initiated in get_empty_inode
    inode->i_sb = sb;
    inode->i_dev = sb->s_dev;
    inode->i_fstype = sb->s_fstype;
    inode->i_size = 0;
    // 这里初始化的时间到了FAT文件系统里面后会被重新设置。
    inode->i_atime = inode->i_ctime = inode->i_mtime = DSM_GetFileTime();

    return inode;
}
#endif

void iput( struct inode * inode )
{
    if( NULL == inode)
    {
        D((DL_VFSERROR, "in iput, inode is NULL\n"));
        return;
    }
    
    wait_on_inode( inode );

    if( 0 == inode->i_count)
    {
        D((DL_VFSERROR, "in iput, inode->i_count = %d\n",inode->i_count));
        return;
    }
    
    if ( !inode->i_dev )             /*should be an empty inode in g_fs_inode_table*/
    {
        D((DL_VFSWARNING, "in iput, inode->i_dev = %d\n",inode->i_dev));
        inode->i_count--;
        return;
    }
    inode->i_count--;

    if ( inode->i_count > 0 )
    {
        return ;
    }

    if ( FS_TYPE_FFS == inode->i_fstype )
    {
        // ffs_clear_inode( inode );
    }
    else if(FS_TYPE_FAT == inode->i_fstype)
    {
        fat_write_inode( inode, TRUE );
        inode_list_del(&(inode->i_sb->inode_chain), inode);
    }
    else
    {
    }
    return ;
}


#if 0
struct inode * get_empty_inode( void )
{
    struct inode * inode;
    int i;

    for ( i = 0; i < FS_NR_INODES_MAX; i++ )
    {
        if ( !g_fs_inode_table[ i ].i_count )
            break;
    }

    if( FS_NR_INODES_MAX == i) //The maximum inode has created
    {
        return NULL;
    }
    
    inode = g_fs_inode_table + i;
    wait_on_inode( inode );

    DSM_MemSet( inode, 0, sizeof( *inode ) );
    return inode;
}
#endif


struct inode* get_empty_inode( void )
{
    struct inode * inode = NULL;

    inode = (struct inode *)DSM_MAlloc(SIZEOF(struct inode));
    if( NULL == inode)
    {
        DSM_ASSERT(0,"get_empty_inode: 1.malloc(0x%x) failed.",SIZEOF(struct inode)); 
        return NULL;
    }
    
    DSM_MemSet( inode, 0x00, sizeof( *inode ) );
    return inode;
}


struct inode* inode_list_add(struct inode **ppsInodeList, struct inode *psInode)
{
    if (NULL == ppsInodeList || NULL == psInode)
    {
        D ((DL_FATERROR, "inode_list_add: _ERR_FAT_PARAM_ERROR, ppsInodeList(0x%x), psInode(0x%x)", ppsInodeList, psInode));
        return NULL;
    }
    
    psInode->next = *ppsInodeList;
    *ppsInodeList = psInode;
    return psInode;
}


INT32 inode_list_del(struct inode **ppsInodeList, struct inode *psInode)
{
    struct inode **ppPrei;
    struct inode *pCuri;
    
    if (NULL == ppsInodeList || NULL == psInode)
    {
        D ((DL_FATERROR, "inode_list_del: _ERR_FAT_PARAM_ERROR, ppsInodeList(0x%x), psInode(0x%x)", ppsInodeList, psInode));
        return ERR_FS_INVALID_PARAMETER;
    }

    ppPrei = ppsInodeList;
    pCuri = *ppsInodeList;
    while (pCuri != NULL)
    {
        if (pCuri == psInode)
        {
            *ppPrei = pCuri->next;
            DSM_Free(pCuri);
            return ERR_SUCCESS;
        }
        else
        {
            ppPrei = &(pCuri->next);
            pCuri = pCuri->next;
        }
    }
    return ERR_FS_INVALID_PARAMETER;
    
}

/*
由对象的标识获取对象本身。
由于inr只能通过find_entry()找到匹配的dir_entry{}后再从中获取，因此，
调用iget()之前必然先调用do_find_entry()。
(例外: 对"/"其对应的inode的inr总是为1，从而可以直接调用iget()获取对应的m_inode{})
iget()并不对应于iput()。实际上，iput()应该和ihold()对应:-)
*/
struct inode * iget( struct super_block *sb, UINT32 inr, UINT32 is_get_fatentry)
{
    struct inode *inode;
    struct inode *empty;
    UINT32 dev_nr;
    long lRet = ERR_SUCCESS;

    /*TODO:
    考虑到可能多个process同时访问inode_table,
    必须进行锁保护
    */

    dev_nr = sb->s_dev;
    //inode = g_fs_inode_table;
    inode = sb->inode_chain;
    //while ( inode < FS_NR_INODES_MAX + g_fs_inode_table )
    while (inode != NULL)
    {
        if ( 0 == inode->i_count ||     /* unused inode */
              inode->i_dev != dev_nr ||
              inode->i_ino != inr )
        {
            //inode++;
            inode = inode->next;
            continue;
        }

        wait_on_inode( inode );

        inode->i_count++;

        if ( inode->i_is_mountpoint )
        {
            int i;

            for ( i = 0; i < (INT32)g_fs_cfg.uNrSuperBlock; i++ )
            {
                if ( g_fs_super_block_table[ i ].s_mountpoint == inode )
                {
                    sb = g_fs_super_block_table + i;
                    break;
                }
            }
            
            if ( (INT32)g_fs_cfg.uNrSuperBlock == i ) //Mounted inode hasn't got sb
            {
                D((DL_VFSERROR, "in iget, Mounted inode hasn't got sb, inode->i_dev = %d, inode->i_ino  = %d\n",inode->i_dev,inode->i_ino));
                return NULL;
            }
            iput( inode );
            dev_nr = sb->s_dev;
            inr = (UINT32)FS_ROOT_INO;
            inode = sb->inode_chain;
            continue;
        }

        return inode;
    }

    empty = get_empty_inode();

    if( NULL == empty ) // inode table is full
    {
        D((DL_VFSWARNING, "in iget, inode table is full\n"));
        return NULL;
    }

    inode = empty;
    inode->i_count = 1;
    inode->i_dev = dev_nr;
    inode->i_ino = inr;
    inode->i_sb = sb;
    inode->i_fstype = sb->s_fstype;
    // 这里初始化的时间到了FAT文件系统里面后会被重新设置。
    inode->i_atime = inode->i_mtime = inode->i_ctime = DSM_GetFileTime();
    inode = inode_list_add(&sb->inode_chain, inode);

    if ( FS_TYPE_FFS == inode->i_fstype )
    {
        //lRet = ffs_read_inode( inode );
        iput( inode );
        return NULL;
    }
    else if ( FS_TYPE_FAT == inode->i_fstype )
    {
        lRet = fat_read_inode( inode ,is_get_fatentry);
    }

    if ( lRet != ERR_SUCCESS ) // reading inode from flash failed
    {
        D((DL_VFSERROR, "in iget, fat_read_inode error,lRet = %d\n",lRet));
        iput( inode );
        return NULL;
    }
    return inode;
}


struct super_block * get_super( UINT32 devnr )
{
    UINT32 i;

    if ( !devnr )
        return NULL;

    for ( i = 0; i < g_fs_cfg.uNrSuperBlock; i++ )
    {
        if ( g_fs_super_block_table[ i ].s_dev == ( UINT32 ) devnr )
        {
            return g_fs_super_block_table + i;
        }
    }
    D((DL_FATWARNING, "in get_super,  no superblock is found\n"));
    return NULL;
}

