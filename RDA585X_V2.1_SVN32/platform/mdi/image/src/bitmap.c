#include "cs_types.h"
#include "mci.h"
#include "image.h"
#include "fs.h"

#pragma pack(1)
/*BMP�ļ�ͷ*/
typedef struct
{
	U16 bfType;          //�ļ����ͣ�2Bytes��ֵΪox4D42(�ַ�BM)
	UINT32 bfSize;         // 4Bytes,�����ļ��Ĵ�С����λΪ�ֽ�
	U16 bfReserved1;     //������2Bytes����λΪ0
	U16 bfReserved2;     //������2Bytes����λΪ0
	UINT32 bfOffBits;      //���ļ�ͷ��ʼ��ʵ��ͼ������֮����ֽ�ƫ������4Bytes
} BITMAPFILEHEADER;


/*BMP�ļ���Ϣͷ*/
typedef struct  
{
	UINT32 biSize;             //BMP��Ϣͷ��С(�ֽ���)��4Bytes
	INT32 biWidth;             //ͼ��Ŀ�ȣ�������Ϊ��λ��4Bytes
	INT32 biHeight;            //ͼ��ĸ߶ȣ�������Ϊ��λ��4Bytes
	U16 biPlanes;            //˵��Ŀ���豸λ�������̶�ֵΪ1��2Bytes
	U16 biBitCount;          //ÿ���ر�������2Bytes����ֵΪ��1��4��8��16��24��32
	UINT32 biCompression;      //ͼ������ѹ��������4Bytes
	UINT32 biSizeImage;        //ͼ��Ĵ�С(�ֽ���)��4Bytes������������4������������ʹ��BI_RGB��ʽʱ������Ϊ0
	INT32 biXPelsPerMeter;     //ˮƽ�ֱ��ʣ�4Bytes����λ������/��
	INT32 biYPelsPerMeter;     //��ֱ�ֱ��ʣ�4Bytes����λ������/��
	UINT32 biClrUsed;          //ʵ��ʹ�õ�ɫ���е���ɫ������,4Bytes
	UINT32 biClrImportant;     //��ͼ����ʾ����ҪӰ�����ɫ��������Ŀ,4Bytes
} BITMAPINFOHEADER;

PRIVATE UINT16 table_entries[512];
PRIVATE UINT8 bitmap_buff[128];

PRIVATE void MMC_BitmapConvert(UINT8 *src, int length, UINT8 *dest, int bits)
{
    int i;
    UINT16 *pixcel = (UINT16*)dest;

    for(i=0;i<length;i+=bits/8)
    {
        if(bits==16)
        {
            *pixcel++ = (src[i]+(src[i+1]<<8));
        }
        else if(bits==24 | bits==32)
        {
            *pixcel++ = (((src[i]>>3)&0x1f))+(((src[i+1]>>2)&0x3f)<<5)+(((src[i+2]>>3)&0x1f)<<11);
        }
        else
            return;
        
    }
}

PUBLIC MCI_ERR_T MMC_ReadBitmapPicture(UINT8 *data, char* outbuf,long outbuf_len,
								 short width,short height,
								 short start_x,short start_y,
								 short end_x,short end_y,
								 DEC_UNIT* decUnit, short kind)
{
	int length;
	int x,y;
	int col,row,bits;
    int pos;
	int colors;

	BITMAPFILEHEADER filehead;
    BITMAPINFOHEADER infohead;
    UINT16 *pixels = (UINT16*)outbuf;

    INT32 file = (INT32)data;

    if(kind==1)
    {
		/* read bitmap file header */
		FS_Read(file, (char *)&filehead+2,sizeof(BITMAPFILEHEADER)-2);
		FS_Read(file, &infohead,sizeof(BITMAPINFOHEADER));
    }
    else
    {
        memcpy(&filehead, data, sizeof(BITMAPFILEHEADER));
        memcpy(&infohead, data+sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
    }
    
	/* compressed bitmap not support */
	if((infohead.biCompression!=0))
	{
		return MCI_ERR_BAD_FORMAT;
	}

	bits=infohead.biBitCount;
	row=infohead.biHeight;
	/* the bitmap file is saved in four-bytes in every line */
	col=((infohead.biWidth*bits+31)/32)*4;  /* col is the byte used by a line */

    decUnit->height = infohead.biHeight;
    decUnit->width = infohead.biWidth;
    decUnit->len = infohead.biHeight*infohead.biWidth*2;

    if(col> 128 || outbuf_len < decUnit->len)
        return MCI_ERR_OUT_OF_MEMORY;


    if(bits <= 8)
    {
		if(infohead.biClrUsed==0)
			colors=(1<<bits);
		else
			colors=infohead.biClrUsed;

    }
    else
        colors = 0;

    if(colors)
    {
        if(kind==1)
            FS_Read(file, table_entries,colors*4);
        else
            memcpy(table_entries,data+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), colors*4);

        MMC_BitmapConvert(table_entries, colors*4, table_entries, 32);
    }
    
    pos = filehead.bfOffBits + (row-end_y-1)*col;
    if(kind==1)
    {
        FS_Seek(file, pos, FS_SEEK_SET);
    }

    pixels += (end_y-start_y)*infohead.biWidth;
    for(y=0;y<end_y-start_y+1;y++)
    {
        if(kind==1)
            FS_Read(file, bitmap_buff,col);
        else
        {
            memcpy(bitmap_buff,data+pos, col);
            pos += col;
        }
        if(bits <= 8)
        {
			for(x=0;x<infohead.biWidth;x++)
			{
				int n=x*bits/8;
				int m=(16-bits-x*bits%8)%8;
				*pixels++=table_entries[(bitmap_buff[n]>>m)&((1<<bits)-1)];
			}
            pixels -= infohead.biWidth*2;
        }
        else
        {
            MMC_BitmapConvert(bitmap_buff, infohead.biWidth*bits/8, pixels, bits);
            pixels -= infohead.biWidth;
        }
        
    }

}

