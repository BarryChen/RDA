////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/lcdd/otm2201h/gouda/src/drv_lcd_otm2201h.c $ //
//	$Author: pengzg $                                                        // 
//	$Date: 2011-04-22 14:11:23 +0800 (星期五, 22 四月 2011) $                     //   
//	$Revision: 7192 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file drv_lcd.c                                                           //
/// That file provides an implementation for the otm2201h LCD screen.               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "lcdd_config.h"
#include "lcdd_types.h"
#include "hal_timers.h"
#include "hal_gouda.h"

#include "pmd_m.h"
#include "sxr_tls.h"

// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================


#define RDA_LCD_BASE		0xa1a20000
#define RDA_LCD_CON			(RDA_LCD_BASE + 0)
#define RDA_LCD_TXDATA		(RDA_LCD_BASE + 0x4)
#define RDA_LCD_RXDATA		(RDA_LCD_BASE + 0x8)
#define RDA_LCD_SPISTATUE	(RDA_LCD_BASE + 0xc)
#define RDA_LCD_INT			(RDA_LCD_BASE + 0x10)

//LCD��������
#define		M_LCD_ON		0xaf
#define		M_LCD_OFF		0xae
//�����ϵ����ģʽ
#define		M_LCD_POWER_BC	0x2c
#define		M_LCD_POWER_VR	0x2a
#define		M_LCD_POWER_VC	0x29
#define		M_LCD_POWER_ALL	0x2f
//V5�ڲ���ѹ���ڵ������á���
#define		M_LCD_SETR_0	0x20
#define		M_LCD_SETR_1	0x21
#define		M_LCD_SETR_2	0x22
#define		M_LCD_SETR_3	0x23
#define		M_LCD_SETR_4	0x24
#define		M_LCD_SETR_5	0x25
#define		M_LCD_SETR_6	0x26
#define		M_LCD_SETR_7	0x27
//ƫѹ����
#define		M_LCD_BIAS_9	0xa2		//V5ʱѡ��ѡ������
#define		M_LCD_BIAS_7	0xa1		//V3ʱѡ��ѡ������
#define		M_LCD_ELE_VOL	0x81		//��������ģʽ����ʾ���ȣ�
#define		M_LCD_VDD_SET	M_LCD_SETR_4
#define		M_LCD_VDD		M_LCD_BIAS_9
#define		M_LCD_COM_NOR	0xc0		//������ʽ
#define		M_LCD_COM_REV	0xc8		//����
//Segment����ѡ��
#define		M_LCD_SEG_NOR	0xa0		//����
#define		M_LCD_SEG_REV	0xa1		//����
//ȫ������/�䰵ָ��
#define		M_LCD_ALL_LIGNT	0xa5		//LCD ALL paint ON
#define		M_LCD_ALL_LOW	0xa4		//Normal Display mode
//���෴����ʾ����ָ�RAM�����ݲ���
#define		M_LCD_ALL_NOR	0xa6		//����
#define		M_LCD_ALL_REV	0xa7		//����
//��ָ̬ʾ������ָ��
#define		M_LCD_STATIC_ON	0xad		//ON
#define		M_LCD_STATIC_OFF 0xac		//OFF
//������ʾ��ʼ�ж�ӦRAM�к�
#define		M_LCD_BEGIN_LINE 0x40		//����������ɼӵ�β����Ϊ0~63
//���õ�ǰҳ����
#define		M_LCD_COL_PAGE	0xb0		//����ָ���ɼ�β��0~8
//���õ�ǰ�л���
#define		M_LCD_COL_LINE_LOW	0x04		//����ָ�����λ��Ч
#define		M_LCD_COL_LINE_HIG	0x10		//����ָ�����λ��Ч
#define LCD_INITIAL_COLOR	0x00			//����LCD����ʼ��ʱ�ı���ɫ
#define LCD_X_MAX			128-1			//��Ļ��X�����������
#define LCD_Y_MAX			64-1			//��Ļ��Y�����������


#define LCM_WR_REG(Addr, Data)  { while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO);}
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}

#define LCDD_BUILD_CMD_WR_CMD(c,i,r) do{c[i].isData=FALSE; c[i].value=r;}while(0)
#define LCDD_BUILD_CMD_WR_DAT(c,i,d) do{c[i].isData=TRUE; c[i].value=d;}while(0)
#define LCDD_BUILD_CMD_WR_REG(c,i,r,d) do{LCDD_BUILD_CMD_WR_CMD(c,i,r); LCDD_BUILD_CMD_WR_DAT(c,i+1,d);}while(0)

#define LCDD_TIME_MUTEX_RETRY 50
// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width 
#define LCDD_DISP_X		        176

// Number of pixels in the display height 
#define LCDD_DISP_Y             220

#define  LCD_OTM2201H_ID        0x0164

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// LCDD screen config.
PRIVATE CONST LCDD_CONFIG_T g_tgtLcddCfg = TGT_LCDD_CONFIG;

PRIVATE BOOL g_lcddRotate = FALSE;

// wheter lcddp_GoudaBlitHandler() has to close ovl layer 0
PRIVATE BOOL g_lcddAutoCloseLayer = FALSE;

// Sleep status of the LCD
PRIVATE BOOL g_lcddInSleep = FALSE;

// =============================================================================
//  FUNCTIONS
// =============================================================================
void LCD_RegWrite(UINT8 Command)
{
	unsigned char Num;
	UINT32 i=0;
	while((RDA_READ_REG(RDA_LCD_SPISTATUE)&1)==1);
	RDA_WRITE_REG(RDA_LCD_CON,RDA_READ_REG(RDA_LCD_CON)&0xfffffff7); // command bit cleare 0. 0 is command
	while((RDA_READ_REG(RDA_LCD_SPISTATUE)&1)==1);
	RDA_WRITE_REG(RDA_LCD_TXDATA,Command);
}

void LCD_DataWrite(UINT8 Dat)
{
	unsigned char Num;
	UINT32 i=0;
	while((RDA_READ_REG(RDA_LCD_SPISTATUE)&1)==1);
	RDA_WRITE_REG(RDA_LCD_CON,RDA_READ_REG(RDA_LCD_CON)|8); // command bit set 1, 1 is data
	while((RDA_READ_REG(RDA_LCD_SPISTATUE)&1)==1);
	RDA_WRITE_REG(RDA_LCD_TXDATA,Dat);
}

PUBLIC LCDD_ERR_T lcdd_update_screen(UINT8* Data, LCDD_ROI_T *rect)
{
	unsigned char i,j;
	unsigned char uiTemp;
	uiTemp = LCD_Y_MAX;
	uiTemp = uiTemp>>3;
	for(i=0;i<=uiTemp;i++)								//��LCD������ʼ������ʾ����
	{
		LCD_RegWrite(0xb0+i);
		LCD_RegWrite(0x01);
		LCD_RegWrite(0x10);
		for(j=0;j<=LCD_X_MAX;j++)
		{
			/*hwp_sysIfc->std_ch[7].start_addr  =  (UINT32) LCD_DATA[];
    		hwp_sysIfc->std_ch[7].tc          =  sizeof(LCD_DATA);
			hwp_sysIfc->std_ch[7].control     = (0x400521);*/
			//LCD_DataWrite(LCD_DATA[i*(LCD_X_MAX+1)+j]);
			LCD_DataWrite(Data[i*(LCD_X_MAX+1)+j]);
		}
	}
}


PUBLIC LCDD_ERR_T led8s_update_screen(UINT16 flag, UINT8* Data)
{

}

// ============================================================================
// lcddp_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Open(VOID)
{
    //hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);

	UINT32 i=0;
	//hal_HstSendEvent(SYS_EVENT,0X9638520);
    RDA_WRITE_REG(RDA_LCD_INT,0);
	for(;i<4000;i++)
		asm volatile("nop\n\t");
	RDA_WRITE_REG(RDA_LCD_INT,1);

	RDA_WRITE_REG(RDA_LCD_CON, 0x1|2<<1|0<<8|1<<16);

	LCD_RegWrite(M_LCD_ON);							//LCD On
	LCD_RegWrite(M_LCD_POWER_ALL);					//�����ϵ����ģʽ
	
	LCD_RegWrite(M_LCD_ELE_VOL);					//��������ģʽ����ʾ���ȣ�
	LCD_RegWrite(0x0f);								//ָ������0x0000~0x003f
	
	LCD_RegWrite(M_LCD_VDD_SET);					//V5�ڲ���ѹ���ڵ�������
	LCD_RegWrite(M_LCD_VDD);						//LCDƫѹ���ã�V3ʱѡ
	
	LCD_RegWrite(M_LCD_COM_REV);					//Com ɨ�跽ʽ����
	LCD_RegWrite(M_LCD_SEG_NOR);					//Segment����ѡ��
	LCD_RegWrite(M_LCD_ALL_LOW);					//ȫ������/�䰵ָ��
	LCD_RegWrite(M_LCD_ALL_NOR);					//��������ʾ����ָ��
	
	LCD_RegWrite(M_LCD_STATIC_OFF);					//�رվ�ָ̬ʾ��
	LCD_RegWrite(0x00);								//ָ������
	
	LCD_RegWrite(M_LCD_BEGIN_LINE+32);					//������ʾ��ʼ�ж�ӦRAM
	//LCD_Fill(0XFF);//(LCD_INITIAL_COLOR);

	
    g_lcddInSleep = FALSE;

    return LCDD_ERR_NO;
}

// ============================================================================
// lcddp_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to 
/// #lcddp_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Close(VOID)
{
    
    //hal_GoudaClose();

	RDA_WRITE_REG(RDA_LCD_CON, 0);
    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or 
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetContrast(UINT32 contrast)
{
    //#warning This function is not implemented yet
    return LCDD_ERR_NO;
}



// ============================================================================
// lcddp_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode. 
///                    If \c FALSE, cancel standby mode. 
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or 
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetStandbyMode(BOOL standbyMode)
{
    if (standbyMode)
    {
        lcddp_Sleep();
    }
    else
    {
        lcddp_WakeUp();
    }
    return LCDD_ERR_NO;
}


// ============================================================================
// lcdd_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Sleep(VOID)
{


    return LCDD_ERR_NO;
}


// ============================================================================
// lcdd_PartialOn
// ----------------------------------------------------------------------------
/// Set the Partial mode of the LCD
/// @param vsa : Vertical Start Active
/// @param vea : Vertical End Active
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_PartialOn(UINT16 vsa, UINT16 vea)
{
    return LCDD_ERR_NO;
}


// ============================================================================
// lcdd_PartialOff
// ----------------------------------------------------------------------------
/// return to Normal Mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_PartialOff(VOID)
{
    return LCDD_ERR_NO;
}

// ============================================================================
// lcddp_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_WakeUp(VOID)
{


    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or 
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_GetScreenInfo(LCDD_SCREEN_INFO_T* screenInfo)
{
    {
        screenInfo->width = LCDD_DISP_X;
        screenInfo->height = LCDD_DISP_Y;
        screenInfo->bitdepth = LCDD_COLOR_FORMAT_RGB_565;
        screenInfo->nReserved = 0;
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcddp_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented for the ebc version of the driver
// ============================================================================
PUBLIC BOOL lcdd_Busy(VOID)
{
    return FALSE;
}



PUBLIC char* lcdd_GetStringId(VOID)
{
   static char mzl02_id_str[] = "mzl02\n"; 
   return mzl02_id_str; 
}

