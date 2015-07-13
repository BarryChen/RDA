////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2010, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://subversion-server/svn/developing/modem2G/branches/aint_dev/platform/edrv/fmd/include/fmd_m.h $ //
//	$Author: julien $                                                        // 
//	$Date: 2008-03-05 15:00:45 +0100 (Wed, 05 Mar 2008) $                     //   
//	$Revision: 17366 $                                                        //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//       
/// @file fmd.c
/// That file implement the FM driver for RDA5870 chip.
//
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "bal_fstrace.h"
#include "fmd_config.h"
#include "fmd_m.h"
#include "tgt_fmd_cfg.h"
#include "fmdp_debug.h"

#include "pmd_m.h"

#include "hal_i2c.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "hal_timers.h"

#include "sxr_tim.h"

#include "sxr_tls.h"
//#include "cmn_defs.h"

#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#endif
#include "event.h"

// =============================================================================
//  MACROS
// =============================================================================

#define BASE_FREQ_USEUROPE  87000
#define BASE_FREQ_JAPAN     76000
#define BASE_FREQ_WORLD     76000
#define BASE_FREQ_EAST_EUROPE     65000

#define CHAN_SPACE          g_chanSpacing[g_fmdConfig->channelSpacing]

// MACROS for PRIVATE FUNCTION USE ONLY
#define FMD_I2C_BPS             g_fmdConfig->i2cBps

#define HAL_I2C_BUS_ID 			g_fmdConfig->i2cBusId

// =============================================================================
//  TYPES
// =============================================================================


//=============================================================================
// FMD_STATE_T
//-----------------------------------------------------------------------------
/// State of the tune or seek pooling process (using function timers)
//=============================================================================
typedef enum
{
    /// Initial state, no operation in process
    FMD_STATE_IDLE,
    /// Pooling states for tune and seek
    FMD_STATE_TUNE_POLL,
    FMD_STATE_SEEK_POLL,
    /// Once polling is done, we need to stop, is SPI is not avalaible, this is delayed to those states
    FMD_STATE_TUNE_STOP,
    FMD_STATE_SEEK_STOP,

    FMD_STATE_QTY
} FMD_STATE_T;


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE CONST TGT_FMD_CONFIG_T*  g_fmdConfig;

PRIVATE UINT32 g_baseFreq[FMD_BAND_QTY] = {BASE_FREQ_USEUROPE, BASE_FREQ_JAPAN, BASE_FREQ_WORLD, BASE_FREQ_EAST_EUROPE};
PRIVATE UINT32 g_chanSpacing[FMD_CHANNEL_SPACING_QTY] = {100,200,50};
PRIVATE FMD_BAND_T g_band = FMD_BAND_QTY; // using FMD_BAND_QTY as a closed state marker
PRIVATE UINT16 g_fmRegSysCtrl; // BassBoost ForceMono and Mute state
PRIVATE FMD_CALLBACK_T* g_callback = NULL;
PRIVATE FMD_STATE_T g_state = FMD_STATE_IDLE;
PRIVATE HAL_SYS_CLOCK_OUT_ID_T g_fmdClockOutId;

/// when using SPI to avoid conflicts
//PRIVATE BOOL g_spiInUse = FALSE;

//extern void rdafm_iic_write_data(unsigned char regaddr, const unsigned short *data, unsigned char datalen);
//extern void rdafm_iic_read_data(unsigned char regaddr, unsigned short *data, unsigned char datalen);
/***************************************************
RDA5802
****************************************************/
uint8 RDA5802_initialization_reg[]={
0xC0, 0x01, //02H: 
0x00, 0x00,        
0x04, 0x00,        
0x88, 0xAF, //05H: 
0x00, 0x00,        
0x5E, 0xC6,        
0x50, 0x96,        
0x00, 0x00,        
0x40, 0x00, //0AH: 
0x00, 0x8F,        
0x00, 0x00,        
0x00, 0x00,        
0x00, 0x00,        
0x00, 0x00,        
0xF0, 0x05, //10H: 
0x90, 0x00,        
0xF4, 0x84,        
0x70, 0x01,        
0x40, 0xF0,        
0x21, 0x80, //15H: 
0x7A, 0xD0,        
0x3E, 0x40,        
0x55, 0xA9,        
0xE8, 0x48,        
0x50, 0x80, //1AH: 
0x00, 0x00,        
0x37, 0xB6,        
0x40, 0x0C,        
0x07, 0x9B,        
0x4C, 0x1D,        
0x81, 0x11, //20H: 
0x45, 0xC0,        
};
#define SMIC_PATCH 0x27

uint8 RDA5802E_initialization_reg[]={
0xC4, 0x01, //02H:
0x1b, 0x90,
0x04, 0x00,
0x86, 0xAd, //05H://86
0x80, 0x00,
0x5F, 0x1A, //07H  change the seek time;
0x5e, 0xc6,
0x00, 0x00,
0x40, 0x6e, //0AH: 
0x2d, 0x80,
0x58, 0x03,
0x58, 0x04,
0x58, 0x04,
0x58, 0x04,
0x00, 0x47, //10H: 
0x90, 0x00,
0xF5, 0x87,
0x7F, 0x0B, //13H: reset ADC's parameters
0x04, 0xF1,
0x5E, 0xc0, //15H: 0x42, 0xc0
0x41, 0xe0,
0x50, 0x6f,
0x55, 0x92,
0x00, 0x7d,
0x10, 0xC0, //1AH
0x07, 0x80,
0x41, 0x1d, //1CH,
0x40, 0x06,
0x1f, 0x9B,
0x4c, 0x2b, //1FH. 
0x81, 0x10, //20H: 
0x45, 0xa0, // 21H
0x19, 0x3F, //22H: change AFC threshold
0xaf, 0x40, 
0x06, 0x81,
0x1b, 0x2a, //25H
0x0D, 0x04, //26H, shutdown Rssi_autodown function
0x80, 0x9F, //0x80, 0x2F, 
0x17, 0x8A,
0xD3, 0x49,
0x11, 0x42,
0xA0, 0xC4, //2BH
0x3E, 0xBB,
0x00, 0x00,
0x58, 0x04,
0x58, 0x04, //2FH
0x58, 0x04,
0x00, 0x74,
0x3D, 0x00,
0x03, 0x0C,
0x2F, 0x68,
0x38, 0x77, //35H
};
uint8 RDA5802H_initialization_reg[]={
0xc4, 0x01, //02h:         bit10 ???? ????1.8v samsung flash
0x00, 0x00,
0x04, 0x00,
0x86, 0xad, //05h://86    
0x40,0x00,
0x56,0xc6,
0x00,0x00,
0x00,0x00,
0x00,0x00,  //0x0ah
0x00,0x00,
0x00,0x00,
0x00,0x00,
0x00,0x00,
0x00,0x00,
0x00,0x06,  //0x10h
0x00,0x19,// //0x00,0x09,//0830
0x2a,0x11,
0x00,0x2e,
0x2a,0x30,
0xb8,0x3c,  //0x15h
0x90,0x00,
0x2a,0x91,
0x84,0x12,
0x00,0xa8,
0xc4,0x00,  //0x1ah
0xe0,0x00,
0x30,0x1d,//0x24,0x9d,1ch
0x81,0x6a,
0x46,0x08,
0x00,0x86,  //0x1fh
0x06,0x61,// 0x16,0x61, 20h
0x00,0x00,
0x10,0x9e,
0x24,0xc9,//   0x24,0x47,//0830//23h
0x04,0x08,//0830
0x06,0x08,  //0x06,0x08,//0830  //0x25h
0xe1,0x05,
0x3b,0x6c,
0x2b,0xec,
0x09,0x0f,
0x34,0x14,  //0x2ah
0x14,0x50,
0x09,0x6d,
0x2d,0x96,
0x01,0xda,
0x2a,0x7b,
0x08,0x21,   //0x30h
0x13,0xd5,
0x48,0x91,
0x00,0xbc,
0x08,0x96,//0830  0x34h
0x15,0x3c,
0x0b,0x80,
0x25,0xc7,
0x00,0x00,//38
0x3C,0x58,
0x2A,0x50,
0x26,0x2C,
0x06,0x08,
0x02,0x00,
0x00,0x00,

};

uint8 RDA5802N_initialization_reg[] =
{
0xc4,0x05, //02h 
0x00,0x00,
0x04,0x00,
0xc6,0xed, //05h
0x60,0x00,
0x72,0x1a,
0x00,0x00,
0x00,0x00,
0x00,0x00,  //0x0ah
0x00,0x00,
0x00,0x00,
0x00,0x00,
0x00,0x00,
0x00,0x00,
0x00,0x00,  //0x10h
0x00,0x19,
0x2a,0x11,
0xB0,0x42,  
0x2A,0x11,  
0xb8,0x31,  //0x15h 
0xc0,0x00,
0x2a,0x91,
0x94,0x00,
0x00,0xa8,
0xc4,0x00,  //0x1ah
0xF7,0xcF,   
0x2a,0xDC,  //0x1ch
0x80,0x6F, 
0x46,0x08,
0x00,0x86,
0x06,0x61, //0x20H
0x00,0x00,
0x10,0x9E,
0x23,0xC8,
0x04,0x06,
0x0E,0x1C, //0x25H
};

//#define HAL_I2C_BUS_ID HAL_I2C_BUS_ID_2
#define FMD_CHIP_ADRESS 0x11//0010001
#define FMD_CHIP_ADRESS_T 0x11

#define I2C_MASTER_ACK              (1<<0)
#define I2C_MASTER_RD               (1<<4)
#define I2C_MASTER_STO              (1<<8)
#define I2C_MASTER_WR               (1<<12)
#define I2C_MASTER_STA              (1<<16)

void rdafm_iic_write_data(unsigned char regaddr, unsigned char *data, unsigned char datalen)
{
	HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;
	UINT8 Data_half[2];
	UINT8 i=0;
	
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,(FMD_CHIP_ADRESS_T<<1),I2C_MASTER_WR | I2C_MASTER_STA);
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,regaddr,I2C_MASTER_WR);

	for(i=0;i<datalen-1;i++,data++)//data
	{
		halErr=hal_I2cSendRawByte(HAL_I2C_BUS_ID,*data,I2C_MASTER_WR);
	}
	halErr=hal_I2cSendRawByte(HAL_I2C_BUS_ID,*data,I2C_MASTER_WR|I2C_MASTER_STO);

//	SXS_TRACE(TSTDOUT, "rdabt_iic_rf_write_data: %d\n.",halErr);
}

void rdafm_iic_read_data(unsigned char regaddr, unsigned char *data, unsigned char datalen)
{
	UINT8 Data_half[2];
	UINT8 i=0;
	
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,(FMD_CHIP_ADRESS_T<<1),I2C_MASTER_WR | I2C_MASTER_STA);
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,regaddr,I2C_MASTER_WR);
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,((FMD_CHIP_ADRESS_T<<1)|1),I2C_MASTER_WR | I2C_MASTER_STA);

	for(i=0;i<datalen-1;i++,data++)//data
	{	
		(*data)=hal_I2cReadRawByte(HAL_I2C_BUS_ID,I2C_MASTER_RD);
	}
	(*data)=hal_I2cReadRawByte(HAL_I2C_BUS_ID,I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO);

//	SXS_TRACE(TSTDOUT, "rdabt_iic_rf_read_data: %d\n.");
}



PRIVATE FMD_ERR_T fmd_I2cOpen(HAL_I2C_BPS_T bps) 
{
#ifdef I2C_BASED_ON_GPIO
        gpio_i2c_open();
#else
	  hal_I2cOpen(HAL_I2C_BUS_ID);
#endif
    return FMD_ERR_NO;
}

 VOID fmd_I2cClose(VOID)
{
#ifdef I2C_BASED_ON_GPIO
       gpio_i2c_close();
#else
	 hal_I2cClose(HAL_I2C_BUS_ID); 
#endif
}

 VOID fmd_WriteSingle(UINT32 addr, UINT16 data)
{
#ifdef I2C_BASED_ON_GPIO
     gpio_i2c_write_data(FMD_CHIP_ADRESS,(unsigned char * )&addr,1, (unsigned char * )&data, 1);   
#else	
     hal_I2cSendByte(HAL_I2C_BUS_ID,FMD_CHIP_ADRESS,addr,data);
#endif
      //  sxs_fprintf(TSTDOUT,"fmd_WriteSingle  addr=%x,data=%x",addr,data);
		
}

PRIVATE FMD_ERR_T fmd_Write(UINT32 addr, UINT8* data,UINT8 length)
{

    fmd_I2cOpen(FMD_I2C_BPS);

#ifdef I2C_BASED_ON_GPIO    	
    gpio_i2c_write_data(FMD_CHIP_ADRESS,(unsigned char * )&addr,1, data, length);
#else
//     hal_I2cSendData(HAL_I2C_BUS_ID,FMD_CHIP_ADRESS, addr,data,length);
         rdafm_iic_write_data(addr,data, length);

#endif
       fmd_I2cClose();

    //sxs_fprintf(TSTDOUT,"fmd_Write  addr=%x,data[0]=%x,data[1]=%x",addr,data[0],data[1]);
    return FMD_ERR_NO;
}

PRIVATE FMD_ERR_T fmd_Read(UINT32 addr, UINT8* data,UINT8 length)
{
    fmd_I2cOpen(FMD_I2C_BPS);
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_read_data(FMD_CHIP_ADRESS,(UINT8* )&addr,1, data, length);
 #else
 //     hal_I2cGetData(HAL_I2C_BUS_ID,FMD_CHIP_ADRESS,addr,data,length);
	rdafm_iic_read_data(addr, data, length);
  #endif
     fmd_I2cClose();
    //sxs_fprintf(TSTDOUT,"fmd_Read  addr=%x,data[0]=%x,data[1]=%x",addr,data[0],data[1]);
    return FMD_ERR_NO;
}

//=============================================================================
// fmd_Scheduler
//-----------------------------------------------------------------------------
/// Process tune and seek operations
/// @param data used to send register read value to next time processing
/// in case SPI is not available.
//=============================================================================
 VOID fmd_Scheduler(UINT32 data)
{
    FMD_ERR_T errStatus = FMD_ERR_NO;
    UINT8 readReg[2] = {0};

    FMD_ASSERT(g_state < FMD_STATE_QTY, "FMD state unknown: %d", g_state);
    // the POLLING step
    if ((g_state == FMD_STATE_TUNE_POLL) || (g_state == FMD_STATE_SEEK_POLL))
    {
        //errStatus = fmd_Read(0x0a, &rdReg);
        fmd_Read(0x0a,readReg,2);
        SXS_TRACE(TSTDOUT,"chan(fmd_Scheduler) = %x,reg1=%x",readReg[0],readReg[1]);    
        if (errStatus == FMD_ERR_NO)
        {
            // no error, check STC bit
            if (readReg[0] & 0x40)
            {
                // done: next step:
                SXS_TRACE(TSTDOUT,"chan(readReg0)#########");  
                if (g_state == FMD_STATE_TUNE_POLL)
                {
                    g_state = FMD_STATE_TUNE_STOP;
                }
                else if (g_state == FMD_STATE_SEEK_POLL)
                {
                    g_state = FMD_STATE_SEEK_STOP;
                }
            }
        }
    }
    g_state = FMD_STATE_IDLE;
}

PUBLIC FMD_ERR_T fmd_Open(FMD_BAND_T band,
                          FMD_CALLBACK_T* callback,
                          FMD_INIT_T* initValues)
{
    CONST TGT_FMD_CONFIG_T* fmdTgtCfg = tgt_GetFmdConfig();
    FMD_ERR_T errStatus;
    UINT8 dacVol;
    UINT16 chan;
    UINT8 readReg[8] = {0};
    UINT8 writeReg[8] = {0};
	UINT8 TestReg[2] = {0};
	UINT32 i = 0;
	UINT16 fm_ChipID;
    
    // Check if already opened
    // -----------------------
    if (g_band != FMD_BAND_QTY)
    {
        	hal_HstSendEvent(SYS_EVENT,0x000000e1);

        return FMD_ERR_ALREADY_OPENED;
    }
    // Check the parameters and store them
    // -----------------------------------
    switch (band)
    {
        case FMD_BAND_US_EUROPE:
        case FMD_BAND_JAPAN:
        case FMD_BAND_WORLD:
        case FMD_BAND_EAST_EUROPE:
            g_band = band;
            break;
        default:
    	hal_HstSendEvent(SYS_EVENT,0x000000e2);
            // this chip has limited band support...
            return FMD_ERR_BAD_PARAMETER;
    }
    if (fmdTgtCfg == NULL)
    {
        g_band = FMD_BAND_QTY; // close marker
    	hal_HstSendEvent(SYS_EVENT,0x000000e3);
        return FMD_ERR_BAD_PARAMETER;
    }
    g_fmdConfig = fmdTgtCfg;
    g_callback = callback;
    // default : mute , stereo, no bass boost
    g_fmRegSysCtrl = 0;
    dacVol = 0;
    chan = 0;
    // set the required initial state
    // ------------------------------
    if (initValues != NULL)
    {
        // set given parameters 
        dacVol = g_fmdConfig->volumeVal[initValues->volume];
        // compute the mute bit
        if (initValues->volume == FMD_ANA_MUTE)
        {
            g_fmRegSysCtrl &= ~0x4000;
        }
        else
        {
            g_fmRegSysCtrl |= 0x4000;
        }
        if (initValues->bassBoost == TRUE)
        {
            g_fmRegSysCtrl |= 0x1000;
        }
        else
        {
            g_fmRegSysCtrl &= ~0x1000;
        }
        if (initValues->forceMono == TRUE)
        {
            g_fmRegSysCtrl |= 0x2000;
        }
        else
        {
            g_fmRegSysCtrl &= ~0x2000;
        }
        chan = (initValues->freqKHz - g_baseFreq[g_band])/CHAN_SPACE;
    }

    // enable the clock
    g_fmdClockOutId = hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
    if (HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE == g_fmdClockOutId)
    {
    	hal_HstSendEvent(SYS_EVENT,0x000000e4);

        return FMD_ERR_RESOURCE_BUSY;
    }

    // enable the POWER
    pmd_EnablePower(PMD_POWER_FM, TRUE);

    sxr_Sleep(50 MILLI_SECONDS);
 

    // Wait for at least powerOnTime
    sxr_Sleep(g_fmdConfig->powerOnTime);
    // Soft Reset
    writeReg[0] = 0;
    writeReg[1] = 0x02;
    errStatus = fmd_Write(0x02, writeReg,2);

    // Wait about 5ms
    sxr_Sleep(5 MILLI_SECONDS);

	//read chip id
	fmd_Read(0x0e,(UINT8*)readReg,2);
	fm_ChipID = (readReg[0] << 8) | readReg[1];
	
	hal_HstSendEvent(SYS_EVENT,0xFF00ff1F);
	hal_HstSendEvent(SYS_EVENT,readReg[0]);
	hal_HstSendEvent(SYS_EVENT,readReg[1]);
	hal_HstSendEvent(SYS_EVENT,fm_ChipID);
	hal_HstSendEvent(SYS_EVENT,0xF100ff2F);

	if(0x5808 == fm_ChipID)
	{
		RDA5802N_initialization_reg[6] = (UINT8)(0x80 |(g_fmdConfig->seekRSSIThreshold & 0x7f));//0x05
    	errStatus = fmd_Write(0x02, RDA5802N_initialization_reg,sizeof(RDA5802N_initialization_reg)/sizeof(RDA5802N_initialization_reg[0]));
		sxr_Sleep(20 MILLI_SECONDS);
		fmd_Read(0x06,(UINT8*)readReg,2);
		hal_HstSendEvent(SYS_EVENT,0xFFeeff1F);
		hal_HstSendEvent(SYS_EVENT,readReg[0]);
		hal_HstSendEvent(SYS_EVENT,readReg[1]);
		hal_HstSendEvent(SYS_EVENT,fm_ChipID);
		hal_HstSendEvent(SYS_EVENT,0xF1eeff2F);
	}
	else 
	if(0x5804 == fm_ChipID)
	{		
		//errStatus = OperationRDAFM_2w(WRITE, (uint8 *)&RDA5802E_initialization_reg[0],sizeof(RDA5802E_initialization_reg));
		RDA5802E_initialization_reg[6] = (UINT8)(0x80 |(g_fmdConfig->seekRSSIThreshold & 0x7f));//0x05
    	errStatus = fmd_Write(0x02, RDA5802E_initialization_reg,sizeof(RDA5802E_initialization_reg)/sizeof(RDA5802E_initialization_reg[0]));
	}
	else
	if(0x5801 == fm_ChipID)
	{	
		//errStatus = OperationRDAFM_2w(WRITE, (uint8 *)&RDA5802H_initialization_reg[0],sizeof(RDA5802H_initialization_reg));
		RDA5802H_initialization_reg[6] = (UINT8)(0x80 |(g_fmdConfig->seekRSSIThreshold & 0x7f));//0x05
    	errStatus = fmd_Write(0x02, RDA5802H_initialization_reg,sizeof(RDA5802H_initialization_reg)/sizeof(RDA5802H_initialization_reg[0]));

		sxr_Sleep(80);	
		//waiting for FmReady
		do
		{
			i++;
			if(i > 10) return 0; 
			
		fmd_Tune(87600);

		sxr_Sleep(9);   	

		//read REG0A&0B	
		fmd_Read(0xa,&(readReg[0]), 4);
		if((readReg[3]& 0x80) == 0)
			{
				writeReg[0] = 0xC0;
				writeReg[1] = 0x00;	
				errStatus = fmd_Write(0x02, (uint8 *)&writeReg[0], 2);
				sxr_Sleep(12);	
				writeReg[0] = 0xC0;
				writeReg[1] = 0x01;
				errStatus = fmd_Write(0x02, (uint8 *)&writeReg[0], 2);
				sxr_Sleep(12);

		}
		}while((readReg[3] & 0x80) == 0);
	}
	else
	{		
		fmd_Read(0x0c,(UINT8*)readReg,2);
 		fm_ChipID = (readReg[0] << 8) | readReg[1];

		hal_HstSendEvent(SYS_EVENT,0xFF00001F);
		hal_HstSendEvent(SYS_EVENT,readReg[0]);
		hal_HstSendEvent(SYS_EVENT,readReg[1]);	
		hal_HstSendEvent(SYS_EVENT,fm_ChipID);
		hal_HstSendEvent(SYS_EVENT,0xF100002F);
	}
	
	if(0x5802 == fm_ChipID || 0x5803 == fm_ChipID)
	{
		if( 0x5802 == fm_ChipID )
			RDA5802_initialization_reg[52] = SMIC_PATCH;

		//errStatus = OperationRDAFM_2w(WRITE, (uint8 *)&RDA5802_initialization_reg[0],sizeof(RDA5802_initialization_reg));
		RDA5802_initialization_reg[6] = (UINT8)(0x80 |(g_fmdConfig->seekRSSIThreshold & 0x7f));//0x05
    	errStatus = fmd_Write(0x02, RDA5802_initialization_reg,sizeof(RDA5802_initialization_reg)/sizeof(RDA5802_initialization_reg[0]));

	}
	else
	{
		errStatus = 1;
	}

    // Register configuration
    // System control    
    #if 0
    RDA5802E_initialization_reg[0] = (UINT8)(0x80 | ((g_fmRegSysCtrl&0xFF00)>>8));//0x02
    RDA5802E_initialization_reg[1] = (UINT8)(0x01 | (g_fmRegSysCtrl&0x00FF));
    RDA5802E_initialization_reg[2] = (UINT8)(((chan & 0x03ff)>>2) & 0xFF);//0x03
    RDA5802E_initialization_reg[3] = (UINT8)((((chan & 0x03ff) << 6)&0x00C0) | 0x10 | ((g_band & 3) << 2) | (g_fmdConfig->channelSpacing & 3));
    RDA5802E_initialization_reg[4] = 0x04;
    RDA5802E_initialization_reg[5] = 0;
    RDA5802E_initialization_reg[7] = (0xa0 | (dacVol & 0x0f));
   #endif
   // RDA5802E_initialization_reg[6] = (UINT8)(0x80 |(g_fmdConfig->seekRSSIThreshold & 0x7f));//0x05
   // errStatus = fmd_Write(0x02, RDA5802E_initialization_reg,sizeof(RDA5802E_initialization_reg)/sizeof(RDA5802E_initialization_reg[0]));

    //must delay for tune
    sxr_Sleep(100 MILLI_SECONDS);
    return FMD_ERR_NO;
}

// =============================================================================
// fmd_Tune
// -----------------------------------------------------------------------------
/// This function tunes the FM channel to the desired frequency.
/// Once the tune operation has locked the channel, the callback registered 
/// at #fmd_Open will be called.
/// @param freqKHz  FM frequency
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_Tune(UINT32 freqKHz)
{
    FMD_ERR_T errStatus;
    UINT8 readReg[2] = {0};
    UINT8 writeReg[2] = {0};
    UINT16 chan;

    sxr_Sleep(10 MILLI_SECONDS);
    errStatus = fmd_Read(0x03, readReg,2);

    chan = (freqKHz - g_baseFreq[g_band])/CHAN_SPACE;
    writeReg[0] = (UINT8)(((chan & 0x03ff)>>2) & 0xFF);//0x03
    //open I2S ,must use |0x30 ;if colse I2s ,use |0x10
    writeReg[1] = (UINT8)((((chan & 0x03ff) << 6)&0x00C0) | (readReg[1]&0x20)| 0x10 | ((g_band & 3) << 2) | (g_fmdConfig->channelSpacing & 3));
    errStatus = fmd_Write(0x03, writeReg,2);

    return errStatus;
}

// =============================================================================
// fmd_Close
// -----------------------------------------------------------------------------
/// This function closes the FM driver and desactivate the FM chip 
/// and stops any pending #fmd_Tune or #fmd_Seek operation.
/// @return             \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_Close(VOID)
{
    FMD_ERR_T errStatus;
    UINT8 writeReg[2] = {0};

    // check oppened
    if (g_band == FMD_BAND_QTY)
    {
        //FMD_ASSERT(g_spiInUse, "closing FMD while not open");
        return FMD_ERR_ALREADY_OPENED; // ALREADY_CLOSED here !!
    }

    // cancel pending actions
    if (g_state != FMD_STATE_IDLE)
    {
        g_state = FMD_STATE_IDLE;
//        sxr_StopFunctionTimer((void (*)(void *))fmd_Scheduler);
    }
    // put radio in sleep mode (+mute and high Z ?)
    errStatus = fmd_Write(0x02, writeReg,2); // 0xc000
    if (errStatus != FMD_ERR_NO)
    {
        return errStatus;
    }

    // close
    // cut the power, cut the clock
    pmd_EnablePower(PMD_POWER_FM, FALSE);
    hal_SysClkOutClose(g_fmdClockOutId);

    g_band = FMD_BAND_QTY; // close marker
    return FMD_ERR_NO;
}

// =============================================================================
// fmd_Seek
// -----------------------------------------------------------------------------
/// This function seeks to the next FM channel.
/// Once the seek operation has found a channel, the callback registered 
/// at #fmd_Open will be called.
/// @param seekUp   \c TRUE : increasing frequency, \c FALSE: decreasing frequency
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_Seek(BOOL seekUp)
{
    FMD_ERR_T errStatus;
    UINT8 writeReg[2] = {0};
    if (g_state != FMD_STATE_IDLE)
    {
        return FMD_ERR_RESOURCE_BUSY;
    }
    if (seekUp)
    {
        // Seek up
	 writeReg[0] = (UINT8)(0x83 | ((g_fmRegSysCtrl&0xFF00)>>8));//0x02
        writeReg[1] = (UINT8)(0x01 | (g_fmRegSysCtrl&0x00FF));
    }
    else
    {
        // Seek down
	 writeReg[0] = (UINT8)(0x81 | ((g_fmRegSysCtrl&0xFF00)>>8));//0x02
        writeReg[1] = (UINT8)(0x01 | (g_fmRegSysCtrl&0x00FF));
    }
    
    errStatus = fmd_Write(0x02, writeReg,2); 
    if (errStatus != FMD_ERR_NO)
    {
        return errStatus;
    }
    // schedule read & callback
    g_state = FMD_STATE_TUNE_POLL;
    //sxr_StartFunctionTimer(100 MILLI_SECONDS, fmd_Scheduler, 0, 0);
    return FMD_ERR_NO;
}


// =============================================================================
// fmd_SetVolume
// -----------------------------------------------------------------------------
/// This function sets the volume and other audio related paramters.
/// @param volume       analog volume
/// @param bassBoost    enable/disable bass boost
/// @param forceMono    enable/disable mono output of stereo radios
/// @return             \c FMD_ERR_NO or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_SetVolume(FMD_ANA_LEVEL_T volume,
                               BOOL bassBoost,
                               BOOL forceMono)
{
    FMD_ERR_T errStatus=0;
    UINT8 writeReg[2] = {0};
    UINT8 dacVol;
    FMD_ASSERT(volume < FMD_ANA_VOL_QTY, "FMD volume out of range (%d)", volume);
    // compute the mute bit
    if (volume == FMD_ANA_MUTE)
    {
        g_fmRegSysCtrl &= ~0x4000;
    }
    else
    {
        g_fmRegSysCtrl |= 0x4000;
    }
    writeReg[0] = (UINT8)(0x80 | ((g_fmRegSysCtrl&0xFF00)>>8));//0x02
    writeReg[1] = (UINT8)(0x01 | (g_fmRegSysCtrl&0x00FF));
    errStatus = fmd_Write(0x02, writeReg,2);
    sxr_Sleep(50 MILLI_SECONDS);
    if (bassBoost == TRUE)
    {
        g_fmRegSysCtrl |= 0x1000;
    }
    else
    {
        g_fmRegSysCtrl &= ~0x1000;
    }
    if (forceMono == TRUE)
    {
        g_fmRegSysCtrl |= 0x2000;
    }
    else
    {
        g_fmRegSysCtrl &= ~0x2000;
    }
    // get volume value from config
    dacVol = g_fmdConfig->volumeVal[volume];
    writeReg[0] = (g_fmdConfig->seekRSSIThreshold & 0x7f);//0x05
    writeReg[1] = (0xa0 | (dacVol & 0x0f));
	
  //  errStatus = fmd_Write(0x05, writeReg,2);
    return errStatus;
}



// =============================================================================
// fmd_GetFreq
// -----------------------------------------------------------------------------
/// @param pFreqKHz     pointer to a UINT32 receiving the channel frequency
/// @return             \c FMD_ERR_NO, \c FMD_ERR_RESOURCE_BUSY or
///                     \c FMD_ERR_BAD_PARAMETER if \p pFreqKHz is \c NULL
// =============================================================================
PUBLIC FMD_ERR_T fmd_GetFreq(UINT32 *pFreqKHz)
{
    UINT8 readReg[2] = {0};
    FMD_ERR_T errStatus;
    if (pFreqKHz == NULL)
    {
        return FMD_ERR_BAD_PARAMETER;
    }
    errStatus = fmd_Read(0x0a, readReg,2);
    SXS_TRACE(TSTDOUT,"reg0=%x,reg1=%x",readReg[0],readReg[1]);   
    if (errStatus == FMD_ERR_NO)
    {
        *pFreqKHz = (((readReg[0]<<8) |readReg[1]) & 0x3ff) * CHAN_SPACE + g_baseFreq[g_band];
    }

    return errStatus;
}


// =============================================================================
// fmd_GetStereoStatus
// -----------------------------------------------------------------------------
/// @param pStereoStatus    pointer to a BOOL receiving the stereo status:
///                         \c TRUE the channel is received in stereo,
///                         \c FALSE the channel is recieved in mono.
/// @return                 \c FMD_ERR_NO, \c FMD_ERR_RESOURCE_BUSY or
///                         \c FMD_ERR_BAD_PARAMETER if \p pStereoStatus is \c NULL
// =============================================================================
PUBLIC FMD_ERR_T fmd_GetStereoStatus(BOOL *pStereoStatus)
{
    UINT8 readReg[2] = {0};
    FMD_ERR_T errStatus;
    if (pStereoStatus == NULL)
    {
        return FMD_ERR_BAD_PARAMETER;
    }

    errStatus = fmd_Read(0x0a, readReg,2);
    SXS_TRACE(TSTDOUT,"reg0=%x,reg1=%x",readReg[0],readReg[1]);   
    if (errStatus == FMD_ERR_NO)
    {
        if ((readReg[0] & 0x04) == 0)
        {
            *pStereoStatus = FALSE;
        }
        else
        {
            *pStereoStatus = TRUE;
        }
    }

    return errStatus;
}


// =============================================================================
// fmd_GetRssi
// -----------------------------------------------------------------------------
/// @param pRSSI        pointer to a UINT32 receiving a measure of the channel quality, should not be used as the meaning of this is dependant on the actual FM chip.
/// @return             \c FMD_ERR_NO, \c FMD_ERR_RESOURCE_BUSY or
///                     \c FMD_ERR_BAD_PARAMETER if \p pRSSI is \c NULL
// =============================================================================
DEPRECATED PUBLIC FMD_ERR_T fmd_GetRssi(BYTE* pRSSI)
{
    UINT8 readReg[2] = {0};
    UINT32 freq = 0;
    FMD_ERR_T errStatus;
    if (pRSSI == NULL)
    {
        return FMD_ERR_BAD_PARAMETER;
    }
    *pRSSI = 0;
    errStatus = fmd_Read(0x0b, readReg,2);
    SXS_TRACE(TSTDOUT,"reg0=%x,reg1=%x",readReg[0],readReg[1]);   


    while (((readReg[1] & 0x80) == 0 ) && (freq <20))
    {
        sxr_Sleep(5 MILLI_SECONDS);
        errStatus = fmd_Read(0x0b, readReg,2);
        SXS_TRACE(TSTDOUT,"wait ready reg0=%x,reg1=%x",readReg[0],readReg[1]);   
        freq++;
    }

    if(freq <20)
    {
                if ((readReg[0] & 0x01) == 0 )
                {
                    *pRSSI = 0;
                } 
                else
                {
                    *pRSSI = ((readReg[0] & 0xfe) >> 1);
                     errStatus=fmd_GetFreq(&freq);
                     SXS_TRACE(TSTDOUT,"fmd_GetFreq%d",freq);   
                }
    }
    else
                *pRSSI = 0;
    return errStatus;

}

PUBLIC BOOL fmd_ValidStop(UINT32 freqKHz)
{
    FMD_ERR_T errStatus;
    UINT8 readReg[4] = {0};
    UINT8 writeReg[2] = {0};
    UINT16 chan;
	UINT8 i = 0;


    SXS_TRACE(TSTDOUT,"fmd_ValidStop freqKHz= %d",freqKHz);   	

    errStatus = fmd_Read(0x03, readReg,2);

    chan = (freqKHz - g_baseFreq[g_band])/CHAN_SPACE;
    writeReg[0] = (UINT8)(((chan & 0x03ff)>>2) & 0xFF);//0x03
    //open I2S ,must use |0x30 ;if colse I2s ,use |0x10
    writeReg[1] = (UINT8)((((chan & 0x03ff) << 6)&0x00C0) | (readReg[1]&0x20)| 0x10 | ((g_band & 3) << 2) | (g_fmdConfig->channelSpacing & 3));
    errStatus = fmd_Write(0x03, writeReg,2);


    sxr_Sleep(35 MILLI_SECONDS);
	//waiting for FmReady
	do
	{
		i++;
		if(i>10) return 0; 
		sxr_Sleep(10 MILLI_SECONDS);
		//read REG0A&0B	
		errStatus = fmd_Read(0x0a, readReg,4);
        //add some log
        SXS_TRACE(TSTDOUT,"fmd_ValidStop readReg[0]=0x%x",readReg[0]);   	
        SXS_TRACE(TSTDOUT,"fmd_ValidStop readReg[1]=0x%x",readReg[1]);   	
        SXS_TRACE(TSTDOUT,"fmd_ValidStop readReg[2]=0x%x",readReg[2]);   	
        SXS_TRACE(TSTDOUT,"fmd_ValidStop readReg[3]=0x%x",readReg[3]);   	
hal_HstSendEvent(SYS_EVENT,readReg);
	}while((readReg[3]&0x80)==0);	

	if ((readReg[2] &0x01)==0) return 0;
		
    return 1;
}
// =============================================================================
// fmd_GetRssiThreshold
// -----------------------------------------------------------------------------
/// @return             a Threshold used to compare with RSSI measure of the channel quality, only useful with fmd_GetRssi.
// =============================================================================
DEPRECATED PUBLIC UINT32 fmd_GetRssiThreshold(VOID)
{
    return g_fmdConfig->seekRSSIThreshold;
}


// =============================================================================
// fmd_I2sOpen
// -----------------------------------------------------------------------------
/// This function open the I2S interface
/// @param fmdI2sCfg I2S configuration 
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_I2sOpen(FMD_I2S_CFG_T fmdI2sCfg)
{
    FMD_ERR_T errStatus = 0;
    UINT8 readReg[2] = {0};
    UINT8 writeReg[2] = {0};
#if 0
     {
    U32 DataBuf;
#define CODEC_CALIB_SETTING 0x0d
    hal_AbbRegRead(CODEC_CALIB_SETTING, &DataBuf);
    DataBuf |= 0x00000C00;
    hal_AbbRegWrite(CODEC_CALIB_SETTING, DataBuf);

    }

    writeReg[0] =  0x12;    
    // writeReg[1] =  0x80;
    writeReg[1] =((fmdI2sCfg.freq & 0xF)<<4);
    errStatus = fmd_Write(0x06, writeReg,2);  //i2s slave,  48k

    writeReg[0] = 0x00; 
    writeReg[1] = 0x40;                                  //enable i2s 
    errStatus = fmd_Write(0x04, writeReg,2);
    sxr_Sleep(50 MILLI_SECONDS);

   // close ear 
    errStatus = fmd_Read(0x03, readReg,2);
    //  SXS_TRACE(TSTDOUT,"fmd_Read=%x,fmd_Read=%x",readReg[0],readReg[1]);    
    writeReg[0] = readReg[0];
    writeReg[1] = readReg[1]|(1<<5);   
    errStatus = fmd_Write(0x03, writeReg,2);

    sxr_Sleep(50 MILLI_SECONDS);
    errStatus = fmd_Read(0x09, readReg,2);
    // SXS_TRACE(TSTDOUT,"fmd_Read=%x,fmd_Read=%x",readReg[0],readReg[1]);  	
    writeReg[0] = readReg[0];
    writeReg[1] = readReg[1]|(1<<0);
    errStatus = fmd_Write(0x09, writeReg,2);
hal_HstSendEvent(SYS_EVENT,0xFFFF000A);
#endif
    return errStatus;
}

// =============================================================================
// fmd_I2sClose
// -----------------------------------------------------------------------------
/// This function close the I2S interface
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_I2sClose()
{
    FMD_ERR_T errStatus = 0;
    UINT8 readReg[2] = {0};
    UINT8 writeReg[2] = {0};
#if 0
    {
        U32 DataBuf;

        hal_AbbRegRead(CODEC_CALIB_SETTING, &DataBuf);
        DataBuf &= 0xfffff3ff;
        hal_AbbRegWrite(CODEC_CALIB_SETTING, DataBuf);

    }


    errStatus = fmd_Read(0x09, readReg,2);
    writeReg[0] = readReg[0];
    writeReg[1] = readReg[1]&0xfe;
    errStatus = fmd_Write(0x09, writeReg,2);
    //enable earphone
    sxr_Sleep(50 MILLI_SECONDS);
    errStatus = fmd_Read(0x03, readReg,2);
    writeReg[0] = readReg[0];
    writeReg[1] = readReg[1]&0xDF;
    errStatus = fmd_Write(0x03, writeReg,2);
   //disable  i2s
    writeReg[0] = 0x00;
    writeReg[1] = 0x00;
    errStatus = fmd_Write(0x04, writeReg,2);
    sxr_Sleep(50 MILLI_SECONDS);
	hal_HstSendEvent(SYS_EVENT,0xFFFF000B);
    #endif
    return errStatus;
}

// =============================================================================
// fmd_PreInit
// -----------------------------------------------------------------------------
/// This function initial fm in handset power on.
// =============================================================================
PROTECTED  VOID  fmd_PreInit(VOID)
{
    UINT8 writeReg[2] = {0};

    // enable the clock
    g_fmdClockOutId = hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
    g_fmdConfig = tgt_GetFmdConfig();

    // enable the POWER
    pmd_EnablePower(PMD_POWER_FM, TRUE);

    // Wait for at least powerOnTime
    hal_TimDelay(30 MS_WAITING);

    // Soft Reset
    writeReg[0] = 0;
    writeReg[1] = 0x02;
    fmd_Write(0x02, writeReg,2);

    // Wait about 5ms
    hal_TimDelay(5 MS_WAITING);

    // put radio in sleep mode
    writeReg[0] = 0x00;
    writeReg[1] = 0x00;
    fmd_Write(0x02, writeReg,2); 

    // cut the power, cut the clock
    pmd_EnablePower(PMD_POWER_FM, FALSE);
    hal_SysClkOutClose(g_fmdClockOutId);

}

