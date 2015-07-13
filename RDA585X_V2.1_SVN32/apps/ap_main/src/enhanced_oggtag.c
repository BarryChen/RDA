/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_common.h"
#include "project_config.h"


const UINT8 ogg_TagInfor_keyword[8]       = {0x07, 0x03, 0x56, 0x4f, 0x52, 0x42, 0x49, 0x53}; // 0x03+"VOBIS"
const UINT8 ogg_Tag_TITLE_keyword[7]    = {0x06, 'T', 'I', 'T', 'L', 'E', '='}; //title ��һ���ֽڱ�ʾ����
const UINT8 ogg_Tag_ARTIST_keyword[8] = {0x07, 'A', 'R', 'T', 'I', 'S', 'T', '='}; //ARIST ��һ���ֽڱ�ʾ����
const UINT8 ogg_Tag_ALBUM_keyword[7] = {0x06, 'A', 'L', 'B', 'U', 'M', '='}; //album  ��һ���ֽڱ�ʾ����

HANDLE ogg_TagFileHandle;
BOOL   ogg_TAGInfoFlag;//Ϊ���ʾ��TAG��Ϣ
UINT32  ogg_TagInforlen;//TAG��Ϣ�ĳ���
UINT32  ogg_TagInforCount;//TAG��Ϣ������

//fsb file system use
INT32  FBS_bytepos;//�ļ�λ���ڵ�ǰҳ�е�ƫ��
UINT32 FBS_pagepos;//�ļ�λ��ҳ��
UINT32 FBS_filepos;//�ļ�λ�õ�byte ��
UINT32 FBS_filelen;//�ļ�����(UINT8 ��)
UINT8  FBS_TmpBuf[512];


BOOL GetAllItemInfor(ID3Info_t *ID3Info);
BOOL  ReadLongData(UINT32 *longtypeout);
BOOL oggSearchMem(const INT8 *memBuff, UINT32 scnt);
BOOL  FSeekByte(UINT32 ByteOffset,  HANDLE hFile );
BOOL  FReadByte(INT8 *pdata, HANDLE hFile);
UINT32 FTellByte(HANDLE hFile);


/*********************************************************************************
*      BOOL  ReadLongData(long *longtypeout)
*
* Description :��һ��LONG������
*
* input:  long *longtypeout//�����long ������
*
* BOOL return: TRUE: �����������Ч
*                 FALSE: �Ѷ����ļ�β
*
*********************************************************************************/
BOOL  ReadLongData(UINT32 *longtypeout)
{
	INT8 i;
	UINT8 Tmp[4];//����ת��LONG�����ݵ�BUF
	
	for (i = 0; i < 4; i++)
	{
		if (!FReadByte(&Tmp[i], ogg_TagFileHandle))
		{
			return FALSE;
		}
	}
	*longtypeout = *(long *)Tmp;
	return TRUE;
}

//=============================================================
/*
    for UINT8 file system

*/
//=============================================================

HANDLE FOpenByte(INT8 *filename, INT8 mod)
{
	HANDLE fhandle;
	
	FBS_bytepos = 0;
	FBS_pagepos = 0;
	FBS_filepos = 0;
	fhandle = FS_Open(filename, mod, 0);
	
	if (fhandle == NULL)
	{
		return NULL;
	}
	
	//������ļ�����
	FBS_filelen = FS_GetFileSize(fhandle);
	
	FS_Read(fhandle, FBS_TmpBuf, 512); //��һPAGE����
	
	return fhandle;
	
}

BOOL  FSeekByte(UINT32 ByteOffset,  HANDLE hFile )
{
	long PageOffset;
	
	if (ByteOffset >= FBS_filelen)
	{
		//�����ļ�����
		return FALSE;
	}
	
	FBS_filepos = ByteOffset;
	PageOffset = ByteOffset / 512;
	FBS_bytepos = ByteOffset % 512;
	
	if (FBS_pagepos == PageOffset)
	{
		//ͬһPAGE ���ö�
		return TRUE;
	}
	FBS_pagepos == PageOffset;
	
	
	if(ERR_SUCCESS != FS_Seek(hFile, FBS_pagepos, FS_SEEK_SET))
	{
		return FALSE;
	}
	return (ERR_SUCCESS == FS_Read(hFile, FBS_TmpBuf, 512)); //��һPAGE����
	
}

BOOL  FReadByte(INT8 *pdata, HANDLE hFile)
{
	*pdata = FBS_TmpBuf[FBS_bytepos];
	FBS_filepos++;
	
	if (FBS_bytepos >= 512 - 1)
	{
		//�����ݲ����Ͷ�һ��PAGE����
		if (ERR_SUCCESS != FS_Read(hFile, FBS_TmpBuf, 512)) //��һPAGE����
		{
			return FALSE; // --->> FALSE
		}
		FBS_bytepos = 0;
	}
	else
	{
		FBS_bytepos++;
	}
	return TRUE;
}


UINT32 FTellByte(HANDLE hFile)
{
	return FBS_filepos;
}

/*********************************************************************************
*              BOOL GetAllItemInfor(ID3Info_t *ID3Info)
* Description : ȡ������Ϣ
* Arguments :
* Returns     :
* Notes       :
*********************************************************************************/
BOOL oggGetTAGInfo(const TCHAR *filename, ID3Info_t *ID3Info, INT32 string_id)
{
	BOOL result;
	
	ogg_TAGInfoFlag = FALSE;
	
	ogg_TagFileHandle = FOpenByte(filename, FS_OPEN_NORMAL);
	
	if(ogg_TagFileHandle == NULL )
	{
		return FALSE;
	}
	
	//----��TAGͷ��־
	result = oggSearchMem(ogg_TagInfor_keyword, 0x2000);
	if(!result)
	{
		FS_Close(ogg_TagFileHandle);
		return FALSE;
	}
	
	//����һ��ĳ���
	result = ReadLongData(&ogg_TagInforlen);
	if (!result)
	{
		FS_Close(ogg_TagFileHandle);
		return FALSE;
	}
	
	//��һ�����ݲ�Ҫ,����
	result = FSeekByte(FTellByte(ogg_TagFileHandle) + ogg_TagInforlen, ogg_TagFileHandle);
	if (!result)
	{
		FS_Close(ogg_TagFileHandle);
		return FALSE;
	}
	
	//����Ϣ��������
	result = ReadLongData(&ogg_TagInforCount);
	if ( (!result) && (ogg_TagInforCount == 0) )
	{
		FS_Close(ogg_TagFileHandle);
		return FALSE;
	}
	
	
	//��ȡID3��Ϣ
	GetAllItemInfor(ID3Info);
	
	FS_Close(ogg_TagFileHandle);
	return ogg_TAGInfoFlag;
}


/*********************************************************************************
*                                 BOOL GetAllItemInfor(ID3Info_t *ID3Info)
* Description : ȡ������Ϣ
* Arguments :
* Returns     :
* Notes       :
*********************************************************************************/
BOOL GetAllItemInfor(ID3Info_t *ID3Info)
{
	INT8 i;
	long readpt_sav;//��ָ�뱸��
	long searchcnt;//����������
	long tmp;
	INT32  j;
	
	readpt_sav = FTellByte (ogg_TagFileHandle);
	
	for (i = 0; i < ogg_TagInforCount; i++)
	{
		if (!ReadLongData(&ogg_TagInforlen))
		{
			return FALSE;
		}
		
		if (!FSeekByte(FTellByte(ogg_TagFileHandle) + ogg_TagInforlen, ogg_TagFileHandle))
		{
			return FALSE;
		}
	}
	
	searchcnt = FTellByte (ogg_TagFileHandle) - readpt_sav;
	
	//�� title ------------------------------------------
	if (ID3Info->TIT2_length != 0)
	{
		FSeekByte(readpt_sav, ogg_TagFileHandle);
		if (oggSearchMem(ogg_Tag_TITLE_keyword, searchcnt))
		{
			tmp = FTellByte (ogg_TagFileHandle);
			FSeekByte(tmp - 6 - 4, ogg_TagFileHandle);
			ReadLongData(&ogg_TagInforlen)    ;
			j  = (MIN(ogg_TagInforlen, ID3Info->TIT2_length) ) - 1;
			FSeekByte(tmp, ogg_TagFileHandle);
			for (i = 0; i < j; i++)
			{
				FReadByte(ID3Info->TIT2_buffer + i, ogg_TagFileHandle);
			}
			ID3Info->TIT2_buffer[i] = 0;
			ogg_TAGInfoFlag = TRUE;
		}
	}
	
	//�� ARTIST ------------------------------------------
	if (ID3Info->TPE1_length != 0)
	{
		FSeekByte(readpt_sav, ogg_TagFileHandle);
		if (oggSearchMem(ogg_Tag_ARTIST_keyword, searchcnt))
		{
			tmp = FTellByte (ogg_TagFileHandle);
			FSeekByte(tmp - 7 - 4, ogg_TagFileHandle);
			ReadLongData(&ogg_TagInforlen)    ;
			j  = (MIN(ogg_TagInforlen, ID3Info->TPE1_length)) - 1;
			FSeekByte(tmp, ogg_TagFileHandle);
			for (i = 0; i < j; i++)
			{
				FReadByte(ID3Info->TPE1_buffer + i, ogg_TagFileHandle);
			}
			ID3Info->TPE1_buffer[i] = 0;
			ogg_TAGInfoFlag = TRUE;
		}
	}
	
	//�� ALBUM ------------------------------------------
	if (ID3Info->TALB_length != 0)
	{
		FSeekByte(readpt_sav, ogg_TagFileHandle);
		if (oggSearchMem(ogg_Tag_ALBUM_keyword, searchcnt))
		{
			tmp = FTellByte (ogg_TagFileHandle);
			FSeekByte(tmp - 6 - 4, ogg_TagFileHandle);
			ReadLongData(&ogg_TagInforlen)    ;
			j  = (MIN(ogg_TagInforlen, ID3Info->TALB_length) ) - 1;
			FSeekByte(tmp, ogg_TagFileHandle);
			for (i = 0; i < j; i++)
			{
				FReadByte(ID3Info->TALB_buffer + i, ogg_TagFileHandle);
			}
			ID3Info->TALB_buffer[i] = 0;
			ogg_TAGInfoFlag = TRUE;
		}
	}
	
	return ogg_TAGInfoFlag;
	
}

/********************************************************************************
����ԭ�ͣ� BOOL oggSearchMem(const INT8 *memBuff, long scnt)
input:
    const INT8 *memBuff: Ҫ���BUFF����ַ����һ���ֽ�ΪBUFF�ĳ��ȣ�
    long scnt:    Դ����ĳ���

output:
    BOOL return: �ҵ��ַ���ʱΪ��

********************************************************************************/
BOOL oggSearchMem(const INT8 *memBuff, UINT32 scnt)
{
	INT32 j = 0;
	INT8 k;
	BOOL result;
	
	while(scnt-- > 0 )
	{
		result = FReadByte(&k, ogg_TagFileHandle);
		if (!result)
		{
			return FALSE;
		}
		
		if (CheckCharType(k) == CharType_DNER)
		{
			k = k - 0x20;//��Ϊ��д������
		}
		
		if (k != memBuff[j + 1]) //��Ϊ��һ��BUFF������ĳ���
		{
			j = 0;
		}
		else
		{
			j++;
		}
		
		if (j >= memBuff[0])
		{
			//�Ƿ�ȫ��������
			return TRUE;      //--->> TRUE
		}
	}
	return FALSE;
	
}









