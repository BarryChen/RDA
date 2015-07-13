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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/lcdd/ili9320/gouda/src/drv_lcd_ili9320.c $ //
//	$Author: pengzg $                                                        // 
//	$Date: 2011-04-22 14:11:23 +0800 (Fri, 22 Apr 2011) $                     //   
//	$Revision: 7192 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file drv_lcd.c                                                           //
/// That file provides an implementation for the AU LCD screen.               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "lcdd_config.h"
#include "lcdd_types.h"
#include "hal_timers.h"
#include "hal_gouda.h"
#include "hal_gpio.h"

#include "pmd_m.h"
#include "sxr_tls.h"

// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================

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

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// Sleep status of the LCD
PRIVATE BOOL g_lcddInSleep = FALSE;

// =============================================================================
//  FUNCTIONS
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
#define LCD_X_MAX			128-1			//��Ļ��X���������
#define LCD_Y_MAX			64-1			//��Ļ��Y���������


#define LCD_DataWrite(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCD_RegWrite(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}
PUBLIC LCDD_ERR_T lcdd_Init(VOID);

PUBLIC LCDD_ERR_T lcdd_update_screen(UINT8* Data, LCDD_ROI_T *rect)
{
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;
	unsigned char i,j;
	unsigned char uiTemp;
	uiTemp = LCD_Y_MAX;
	uiTemp = uiTemp>>3;
	//lcdd_Init();
    //hal_HstSendEvent(SYS_EVENT, 0x11030700);
    //hal_HstSendEvent(SYS_EVENT, g_lcddInSleep);
    if(g_lcddInSleep)
        hal_GoudaWakeup();
    //hal_HstSendEvent(SYS_EVENT,0x852963);
    //hal_HstSendEvent(SYS_EVENT,Data);
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
    if(g_lcddInSleep)
        hal_GoudaSleep();
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Open(VOID)
{
	UINT32 i=0;

    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

lcdd_SetBrightness(7);

#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &lcddReadConfig.config, 0);
#else
    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
#endif    

    sxr_Sleep(20 MS_WAITING);

    lcdd_Init();

    g_lcddInSleep = FALSE;

    return LCDD_ERR_NO;
}

PUBLIC LCDD_ERR_T lcdd_Init(VOID)
{
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
	
	LCD_RegWrite(M_LCD_BEGIN_LINE+32);		
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to 
/// #lcdd_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Close(VOID)
{
    return LCDD_ERR_NO;
}

 
// ============================================================================
// lcdd_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or 
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetContrast(UINT32 contrast)
{
    return LCDD_ERR_NO;
}


// ============================================================================
// lcdd_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode. 
///                    If \c FALSE, cancel standby mode. 
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or 
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetStandbyMode(BOOL standbyMode)
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
// lcdd_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_WakeUp(VOID)
{
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;
    hal_GoudaWakeup();
    //lcdd_Open();
    g_lcddInSleep = FALSE;
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
    hal_GoudaSleep();
    g_lcddInSleep = TRUE;
    return LCDD_ERR_NO;
}


// ============================================================================
// lcdd_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or 
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_GetScreenInfo(LCDD_SCREEN_INFO_T* screenInfo)
{
    return LCDD_ERR_NO;
}


// ============================================================================
// lcdd_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented now for the driver
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


