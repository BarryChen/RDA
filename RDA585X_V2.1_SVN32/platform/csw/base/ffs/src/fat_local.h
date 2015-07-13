#ifndef _CII_FAT_LOCAL_H
#define _CII_FAT_LOCAL_H


#define DATA_DIRTY  1
#define DATA_CLEAN  0

struct file;
struct inode;
struct super_block;
struct dentry;

INT32 fat_format(UINT32 uDevNo, UINT32 iDiskTotSec,UINT8* vollab,UINT8* oemname,UINT8 opt);
INT32 fat_HasFormated(UINT32 uDevNo, UINT8 iType);

INT32 fat_file_read(struct inode *inode, struct file * filp, INT8 * buf, INT32 len);
INT32 fat_file_write(struct inode *inode, struct file * filp, INT8 * buf, UINT32 lenToWrite);
INT32 fat_file_extend(struct inode *inode, struct file * filp, UINT32 SizeExtend);
INT32 fat_read_inode ( struct inode *inode,UINT32 is_get_fatentry);
INT32 fat_do_find_entry( struct inode * dir_i, struct dentry *de, UINT32 *ino,UINT32 iFlag);
INT32 fat_write_inode( struct inode * inode, BOOL bFreeFlag );
INT32 fat_create_file( struct inode *dir_i, struct dentry *de, UINT8 mode );
INT32 fat_trunc_file( struct inode *dir_i, struct dentry *de, int mode );
INT32 fat_do_unlink( struct inode *dir_i, struct dentry *de, BOOL bRename );
INT32 fat_read_super( struct super_block *sb, UINT32 uDevNo );
INT32 fat_do_link( struct dentry *old_de, struct inode *dir_i, struct dentry *de, UINT32 type );

INT32 fat_dump_fat_list( PCSTR pathname );
INT32 fat_dump_fat(UINT32 begin_clus_nr, UINT32 counter );
INT32 fat_scan_disk(UINT32 uDevNo);
INT32 fat_get_file_name(struct inode * inode, WCHAR *fileName);
INT32 fat_file_truncate(struct inode *psInode, UINT32 nFileSize);
INT32 fat_get_dir_size(struct inode *inode,UINT64* pSize,BOOL IgnoreDir);


/*
VFS用inode number来标识一个file。
在MinixFS中，inode number是inode{}在磁盘上的inode-list中的index；
在FAT中，我们也希望能够根据inode number来标识file的入口即dir_entry{}
在磁盘上的位置。

用三元组( clus_no, sec_no, offset_in_sec)来标识一个entry；并根据该
三元组来生成inode number；由于一个sector(512byte)可储存entry(32byte)16个，
那么我们可以压缩到用0到15来表示entry在sector中的偏移。
31-->10 bits: cluster number
9  -->  4 bits: sector number
3  -->  0 bits: entry offset 
*/

// 由于i节点中只用了22位来表示簇号的缘故，像0x0fffffff这样的结尾标志被转换后会
// 有几位数据丢失，当有INO转CLU后其值为0x3fffff时我们必须假设这是个结尾标志，
// 所以我们的系统支持存储设备的容量实际上缩小了一些，但也已经足够了。
#define FAT_INO2CLUS(ino) ( (UINT32) ( ( ((ino)>>10) & 0x3fffff ) == 0x3fffff ? 0x0fffffff : ( ((ino)>>10) & 0x3fffff ) ) )
#define FAT_INO2SEC(ino) ( (UINT16) ( (((ino)&0x3ff)>>4) & 0x3f ) )
#define FAT_INO2OFF(ino) ( (UINT16) (( (ino)&0xf )<<5)) // 左移5位得到entry在sector中的物理偏移。

#define FAT_CSO2INO(clus, bsec, eoff)  (UINT32) ( (((clus)&0x3fffff)<<10) | (((bsec)&0x3f)<<4) | (((eoff)>>5)&0xf) )

#endif //_CII_FAT_LOCAL_H

