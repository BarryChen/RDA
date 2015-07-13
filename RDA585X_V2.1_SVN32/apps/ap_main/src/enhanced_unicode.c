/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#include "ap_common.h"
#include "ap_gui.h"


typedef struct
{
	WORD uni_code;
	BYTE latin_code;
} uni_latin;


const uni_latin ConvertTab[27] =                 //"27" meaning:0x20-5
{
	0x20ac, 0x80,            0x201a, 0x82, 0x0192, 0x83, 0x201e, 0x84, 0x2026, 0x85, 0x2020, 0x86, 0x2021, 0x87,
	0x02c6, 0x88, 0x2030, 0x89, 0x0160, 0x8a, 0x2039, 0x8b, 0x0152, 0x8c,            0x017d, 0x8e,
	0x2018, 0x91, 0x2019, 0x92, 0x201c, 0x93, 0x201d, 0x94, 0x2022, 0x95, 0x2013, 0x96, 0x2014, 0x97,
	0x02dc, 0x98, 0x2122, 0x99, 0x0161, 0x9a, 0x203a, 0x9b, 0x0153, 0x9c,            0x017e, 0x9e, 0x0178, 0x9f
};

#define FindTimes (sizeof(ConvertTab)/sizeof(ConvertTab[0]))   //Ҳ����ֱ�Ӹ�"27"

#ifdef SD_UNICODE

/*********************************************************************************
*
*
* Description :
*
* Arguments   : inchar ������ַ�����ַ   codeoffsetaddr ���ձ��е���ַ
*
* Returns     : outchar ������ַ�����ַ
*                       ��������е���Ч�ֽ�����Ϊ0��ʾû�ַ����
*
* Notes       :
*
*********************************************************************************/
WORD OneUniToChar(WORD unicode, const SD_FILE *fp)
{
	WORD code;
	unsigned long unicode1;
	
	unicode1 = (unicode) * 2;
	if(unicode > 127)
	{
		if(!SD_FSeek(fp, 0, unicode1))
		{
			return 0x20;
		}
		if(SD_FRead(fp, &code, 2))
		{
			return code;
		}
	}
	else     //��ASCII
	{
		return unicode;
	}
	return 0;
}


/*********************************************************************************
*
* Description : ������:OneUniToLatin1  ����:��һ��Latin1�ַ���unicodeת���ɶ�Ӧ��Latin1����
*
* Arguments   : ����:unicode ת����Latin1�ַ�unicodeֵ
*
* Returns     : WORD: ת����õ���Latin����.����:�������unicode��Latin�ַ�ʱ,�򷵻�0x0020!
*
*
* Notes       : for Latin1 unicode convert
*
*********************************************************************************/
WORD OneUniToLatin1(unsigned int unicode)
{
	BYTE i;
	
	if ((unicode <= 0x007f) || ((unicode >= 0x00a0) && (unicode <= 0x00ff))) //����д��:(0x00a0<=unicode<=0x00ff)!
	{
		return unicode;
	}
	else
	{
		for(i = 0; i < FindTimes; i++)
		{
			if (unicode == ConvertTab[i].uni_code)
			{
				return (WORD)ConvertTab[i].latin_code;                //�ҵ�,���ض�Ӧ��Latin1����!
			}
		}
		if (i == FindTimes)
		{
			return unicode;    //�������unicode��ת�����в��Ҳ���(��Latin�ַ�),�򷵻�0x0020!
		}
	}
}


/*********************************************************************************
*
*
* Description :
*
* Arguments   :
*
* Returns     :
*
* Notes       :
*
*********************************************************************************/
BOOL unitochar(char *str, int len, BYTE encode)
{
	int s_cnt = 0;
	int d_cnt = 0;
	WORD code;
	SD_FILE *fp;
	
	const char ftbl_B5[] = "FTBL_B5.$$$";
	const char ftbl_GB[] = "FTBL_GB.$$$";
	const char ftbl_JP[] = "FTBL_JP.$$$";
	const char ftbl_KR[] = "FTBL_KR.$$$";
	
	
	switch(encode)
	{
	case CP_CHS:            //���ļ���
		fp = SD_FOpen(ftbl_GB, 0);
		break;
	case CP_CHT:            //���ķ���
		fp = SD_FOpen(ftbl_B5, 0);
		break;
	case 3:                //����
		fp = SD_FOpen(ftbl_JP, 0);
		break;
	case 4:                //����
		fp = SD_FOpen(ftbl_KR, 0);
		break;
		
	default:                  //ASCII��������
		// for Latin1 unicode convert.
		for(d_cnt = 0, s_cnt = 0; (s_cnt < len) && ((str[s_cnt] != 0) || (str[s_cnt + 1] != 0)); )
		{
			code = OneUniToLatin1(str[s_cnt] + str[s_cnt + 1] * 0x100);
			str[d_cnt++] = code;
			s_cnt += 2;
		}
		str[d_cnt] = 0;   //��β��0
		return 1;         //converted complete
	}
	
	if(fp == NULL)
	{
		SD_FClose(fp);
		return 0;
	}
	
	for(d_cnt = 0, s_cnt = 0; ( (str[s_cnt] != 0) || (str[s_cnt + 1] != 0) ); )
	{
		code = OneUniToChar(str[s_cnt] + str[s_cnt + 1] * 0x100, fp);
		str[d_cnt++] = code; //���˵�λ
		if(code > 127)
		{
			str[d_cnt++] = code >> 8; //�Ѹ�λ�Ƶ���λȥ
		}
		s_cnt += 2;
	}
	str[d_cnt] = 0;
	SD_FClose(fp);
	return 1;
}


#endif

