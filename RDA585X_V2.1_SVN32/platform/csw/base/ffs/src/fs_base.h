#ifndef _CII_FS_BASE_H
#define _CII_FS_BASE_H

// VFS Version.
#define FS_VFS_VERSION                      0x1002    

//系统初始化时进行磁盘扫描支持的最大目录层数。
#define FS_NR_SCANDISK_MAX_LAYERS  16

//系统中能打开file数,一个open操作对应一个struct file. 
#define FS_NR_OPEN_FILES_MAX    5//(2 * FS_NR_INODES_MAX ) // 32

// task_struct里面定义每个task能同时打开文件的最大数。
#define FS_NR_TASK_OPEN_FILE_MAX           FS_NR_OPEN_FILES_MAX 

// Create mode define
// FILE
#define FS_MODE_FILE 0
// Directory
#define FS_MODE_DIR  1

// fs status is power on.
#define FS_POWER_ON             0x01

// fs status is power off.
#define FS_POWER_OFF            0x00


#define FS_FLAG_NOT_FIND_FILE   0
#define FS_FLAG_FIND_FILE       1

#define FS_DEFAULT_NR_SUPER 10

#define PATTERN_CMP_MATCH           1
#define PATTERN_CMP_UNMATCH         2

#define DOT_FOUND                   1
#define DOTDOT_FOUND                2
#define DIR_ENTRY_FOUND             4
#define DIR_ENTRY_NOT_FOUND         5

// Minix文件系统中根目录使用1号i节点，但在FAT12及FAT16中，
// 1会被转换成根目录下的第二个目录项的物理位置导致出错。
#define FS_ROOT_INO                 (~0) 
#define FS_PARENT_INO               ((~0)-1) 

// use 1 for the fat16 root cluster to provide ino to all zero
#define FS_ROOT_CLUSTER             (1) // root cluster for FAT16

#define LEN_FOR_NULL_CHAR 1

enum e_GetEntryMode{
	GET_PARENT_ENTRY,
	GET_NEXT_ENTRY,
	GET_PREV_ENTRY,
	GET_ENTRY_COUNT,
	GET_ENTRY_NO,
	GET_ENTRY_CLUSTER
};

// ================
// The state  for  check file name begin.
// NV: Name Valid.
typedef enum {
   FS_NV_INIT,                                     // initialization
   FS_NV_PARTITIOIN,                         // dir entry partition "/" or "\\"
   FS_NV_BASE_ENTRY,                       // base entry name.
   FS_NV_WILDCARD_ENTRY,              // wildcard character as "*" "?"
   FS_NV_BASE_ENTRY_END,              // base entry end. 
   FS_NV_WILDCARD_ENTRY_END,     // wildcard character end. 
   FS_NV_NULL_ENTRY_END,              // NULL entry.
   FS_NV_INVALID_ENTRY_END        // invalid entry, include invalid character.as  
}FS_NV_STATUE;
// Define the character type judge macro.
// Dir entry partition character.include \ /
#define IS_NV_PART(ch)         (0x2f == ch || 0x5c == ch)

// Wildcard character. include * ?
#define IS_NV_WILDCARD(ch)  (0x2a == ch || 0x3f == ch)

// Invalid character. include  : "  < > |
#define IS_NV_INVALID(ch)    (0x3a == ch || \
                                                0x22 == ch || \
                                                0x3c == ch || \
                                                0x3e == ch || \
                                                0x7c == ch) 
                                                
// The base long name character. not include \ / * ? : "  < > |                                              
#define IS_NV_BASE(ch) (!IS_NV_PART(ch) &&\
                                         !IS_NV_WILDCARD(ch) &&\
                                         !IS_NV_INVALID(ch))
// Terminal character. NULL.
#define IS_TERMINAL(ch) (0 == ch)

// The state  for  check file name end.
// ================

#if defined(_MS_VC_VER_)
#define FS_SemInit() 
#define FS_Down()    
#define FS_Up()   
#else
//#define FS_SemInit()  { if (!g_fs_sem) { g_fs_sem = COS_CreateSemaphore(1); }} 
#define FS_Down()   sxr_TakeSemaphore(g_fs_sem)
#define FS_Up()     sxr_ReleaseSemaphore( g_fs_sem ) 
#endif

// FS的配置信息。
struct fs_cfg_info
{
    UINT32 uNrSuperBlock; // super-block的最大数。
    UINT32 uNrOpenFileMax; // 每个task可以同时打开文件的最大数。
};

struct super_block
{
    UINT32  s_dev; /*MUST be long, for we will convert it to pointer*/
    UINT32  s_magic;
    struct inode * s_root;
    struct inode *s_mountpoint;
    UINT32 s_time;
    //struct task_struct * s_wait;
    UINT8 s_lock;
    UINT8 s_rd_only;
    UINT8 padding[1];
    UINT8 s_fstype;
    struct inode *inode_chain;
    struct fat_fsi *fat_fsi;
    union 
    {
        struct fat_sb_info  fat_sb;
        void       *generic_sbp;
    } u;
};

struct inode
{
    union  // 放在前面是为了调试方便，没有其他用意
    {
        struct fat_inode_info		fat_i;
        //struct ffs_inode_info		ffs_i;
        void						*generic_ip;
    } u;
    
    UINT32 i_dev;				/*MUST be long, for we will convert it to pointer*/
    UINT32	i_ino;				/*inode number*/
    UINT32 i_size;		    /*the length of the file*/
    UINT32 i_mode;
    UINT32 i_count;
    UINT8  i_nlinks;
    UINT8  i_is_mountpoint;
    UINT8  i_lock;
    UINT8  pad[1];
    UINT32  i_fstype;
    UINT32 i_atime;       /*Time of last access to the file*/
    UINT32 i_mtime;      /*Time of last data modification*/
    UINT32 i_ctime;       /*Time of last status(or inode) change*/
    struct super_block	*i_sb;
    struct inode *next;
};

struct dentry
{
    struct inode * inode;
    WCHAR name [FS_FILE_NAME_LEN + LEN_FOR_NULL_CHAR];
    UINT32 append_data;		// append data
    UINT32 append_data2;
};

struct file
{
    UINT32 f_mode;
    UINT16 f_flags;
    UINT16 f_count;
    UINT32 f_pos;
    PVOID  f_pattern;
//    PVOID  f_prvdata;
    struct inode * f_inode;
    struct file *next;
};


/*
 * 目前这个tast_struct为FS内部定义并使用，系统的所有task共享根目录root及工作目录pwd；
 * 如果在多task的系统中每个task需要拥有各自的根目录及工作目录的话，
 * 则FS须使用kernel所定义的task_struct并做相应改动。
 */
struct task_struct
{
    struct inode * pwd;
    struct inode * root;
    struct file  ** filp;
    WCHAR *wdPath; // 当前工作目录的路径。
};

#define DENTRY_ALLOC()  (struct dentry*)DSM_MAlloc(sizeof(struct dentry))
#define DENTRY_FREE(psDe)     DSM_Free(psDe)



#define IS_COMPONENT_SEPATATOR( c ) ( _T_DSM('\\') == (c) || _T_DSM('/') == (c) )
#define IS_VALID_DRIVER_NAME( c ) ( (_T_DSM('c') <= (c) && _T_DSM('z') >= (c)) ||(_T_DSM('C') <= (c) && _T_DSM('Z') >= (c)) )


// local function 
void iput( struct inode * inode );
struct inode * iget( struct super_block *sb, UINT32 inr,UINT32 is_get_fatentry);

extern struct fs_cfg_info g_fs_cfg;

#define FD_IS_VALID( fd ) ((fd) >= 0 && (INT32)( fd ) < (INT32)g_fs_cfg.uNrOpenFileMax)

/*
在FS_FindXXX序列API中, 考虑到一致性, handle-for-finding和
handle-for-regular-file都是process的file{}数组的下标, 只不过
对前者, file的f_pos域被转义了: 它记录的是
directroy-inode中的dents链表中下一个待匹配的entry.

由于这个转义, 在FS_FindXXX序列API中, 必须检查
handle是否是用于finding的.
*/
#define FS_O_FIND		        00400

//BOOL is_unicode_name_too_long(CONST WCHAR *UniPathname );
BOOL is_file_name_valid( PCWSTR pathname, BOOL bIgnoreBaseName );
struct inode * dir_namei( PCWSTR pathname, INT32* namelen, CONST WCHAR** basename );
int find_entry( struct inode ** dir_i, struct dentry *de );
WCHAR* format_dir_name( PCWSTR pathname );
int pattern_match( WCHAR *name, WCHAR *pattern );
INT32 pattern_match_oem(UINT8* name,UINT8* pattern);
int ext_match(const UINT8 *name, const UINT8 *pattern );
int get_next_entry( struct inode * dir_i, UINT32 *ino, UINT8 type, UINT32 *pCount, const PTCHAR pExtName, UINT8 mode);
int do_find_entry( struct inode ** dir_i, struct dentry *de, UINT32 iFlag);
int get_parent_node(struct inode * dir_i, UINT32 *inode);
int find_entry_by_cluster(struct inode * dir_i, UINT32 startclu, UINT32 *inode);
struct inode * get_empty_inode( void );
struct inode * namei( PCWSTR pathname );
struct super_block * get_super( UINT32 devnr );
struct inode* inode_list_add(struct inode **ppsInodeList, struct inode *psInode);
INT32 inode_list_del(struct inode **ppsInodeList, struct inode *psInode);
INT32 fs_GlobalVarInit();
INT32 fs_GlobalVarFree();

INT32 fs_GetSuperBlockFree(struct super_block** psSuperBlock);
INT32 fs_FindSuperBlock(UINT32 iDev,struct super_block** psSuperBlock);
INT32 fs_SetWorkDirPath(WCHAR* WorkDirPath, WCHAR* ChangeDirPath);

INT32 FS_ScanDisk(PCSTR pszDevName,UINT8 iFsType);
INT32 fs_SetErrorCode(INT32 iErrCode);

struct file* fs_alloc_fd( void );
struct file* fs_fd_list_add(struct file **ppsFileList, struct file *psFD);
INT32 fs_fd_list_del(struct file **ppsFileList, struct file *psFD);
BOOL fs_is_file_opened(struct file **ppsFileList, struct file *psFD);

INT32 fs_close(INT32 fd);
INT32 fs_findclose(INT32 fd);

// Check the g_fs_file_table array if have opened files.
// if existed, return TRUE, else return FALSE.
BOOL fs_IsOpened(UINT32 iDev);


#define _ERR_FS_SUCCESS             ERR_SUCCESS    

#define _ERR_FS_NO_MORE_SB_ITEM       -101001 // no more super block item in the table.
#define _ERR_FS_NOT_MOUNT             -101002 // the device unmount.
#endif //_CII_FS_BASE_H

