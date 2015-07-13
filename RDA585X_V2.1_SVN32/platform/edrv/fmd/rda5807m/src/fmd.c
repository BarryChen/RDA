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

#define RDA5807M_WR_ADDR    0x20
#define RDA5807M_RD_ADDR    (RDA5807M_WR_ADDR + 1)
#define DELAY(DURATION)		{ UINT32 j; for(j = DURATION * g_i2c_multiple ; j >0; j--){}}


#define SECOND        * HAL_TICK1S

#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR

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
PRIVATE UINT32 g_chanSpacing[FMD_CHANNEL_SPACING_QTY] = {100, 200, 50};
PRIVATE FMD_BAND_T g_band = FMD_BAND_QTY; // using FMD_BAND_QTY as a closed state marker
PRIVATE UINT16 g_fmRegSysCtrl; // BassBoost ForceMono and Mute state
PRIVATE FMD_CALLBACK_T* g_callback = NULL;
PRIVATE FMD_STATE_T g_state = FMD_STATE_IDLE;
PRIVATE HAL_SYS_CLOCK_OUT_ID_T g_fmdClockOutId;

/// when using SPI to avoid conflicts
//PRIVATE BOOL g_spiInUse = FALSE;

//extern void rdafm_iic_write_data(unsigned char regaddr, const unsigned short *data, unsigned char datalen);
//extern void rdafm_iic_read_data(unsigned char regaddr, unsigned short *data, unsigned char datalen);
UINT8 rda5807m_init[] =
{
	0xC0, 0x01, //02H
	0x00, 0x10,
	0x04, 0x00,
	0xc3, 0xaf, //05H:
	0x00, 0x00,
	0x3e, 0x02,       /// 0x5e, 0xc6,//0x3e, 0x3e, // 0x5e, 0xc7, //07H  0x1e 0x1e
#if 0
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
#endif
};

#define I2C_MASTER_ACK              (1<<0)
#define I2C_MASTER_RD               (1<<4)
#define I2C_MASTER_STO              (1<<8)
#define I2C_MASTER_WR               (1<<12)
#define I2C_MASTER_STA              (1<<16)

PRIVATE FMD_ERR_T fmd_I2cOpen(HAL_I2C_BPS_T bps)
{
	gpio_i2c_open();
	return FMD_ERR_NO;
}

VOID fmd_I2cClose(VOID)
{
	gpio_i2c_close();
}

PRIVATE FMD_ERR_T fmd_Write(UINT32 addr, UINT8* data, UINT8 length)
{
	UINT32 i;
	UINT32 g_scStatus;
	
	gpio_i2c_open();
	g_scStatus = hal_SysEnterCriticalSection();
	gpio_i2c_Start();
	gpio_i2c_TxByte(RDA5807M_WR_ADDR);
	for(i = 0; i < length; i ++)
	{
		gpio_i2c_TxByte(*(data + i));
	}
	gpio_i2c_Stop();
	hal_SysExitCriticalSection(g_scStatus);
	gpio_i2c_close();
	
	return FMD_ERR_NO;
}

PRIVATE FMD_ERR_T fmd_Read(UINT32 addr, UINT8* data, UINT8 length)
{
	UINT32 i;
	UINT32 rdData;
	UINT32 g_scStatus;
	
	gpio_i2c_open();
	g_scStatus = hal_SysEnterCriticalSection();
	gpio_i2c_Start();
	gpio_i2c_TxByte(RDA5807M_RD_ADDR);
	for(i = 0; i < length - 1; i ++)
	{
		gpio_i2c_RxByte((data + i), 0);
	}
	gpio_i2c_RxByte((data + i), 1);
	gpio_i2c_Stop();
	hal_SysExitCriticalSection(g_scStatus);
	gpio_i2c_close();
	
	return FMD_ERR_NO;
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
	
	// Check if already opened
	// -----------------------
	if (g_band != FMD_BAND_QTY)
	{
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
		// this chip has limited band support...
		return FMD_ERR_BAD_PARAMETER;
	}
	if (fmdTgtCfg == NULL)
	{
		g_band = FMD_BAND_QTY; // close marker
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
		chan = (initValues->freqKHz - g_baseFreq[g_band]) / CHAN_SPACE;
	}
	
	writeReg[0] = 0x00;
	writeReg[1] = 0x02;
	errStatus = fmd_Write(0x02, writeReg, 2);   // Soft Reset
	sxr_Sleep(100 MS_WAITING);
	errStatus = fmd_Write(0x02, rda5807m_init, sizeof(rda5807m_init) / sizeof(rda5807m_init[0]));
	sxr_Sleep(200 MS_WAITING);
	return errStatus;
}


unsigned long  fmd_FreqToChan(unsigned long frequency)
{
	char channelSpacing;
	unsigned long  bottomOfBand;
	unsigned long  channel;
	if ((rda5807m_init[3] & 0x0c) == 0x00)
	{
		bottomOfBand = 8700;
	}
	else if ((rda5807m_init[3] & 0x0c) == 0x04)
	{
		bottomOfBand = 7600;
	}
	else if ((rda5807m_init[3] & 0x0c) == 0x08)
	{
		bottomOfBand = 7600;
	}
	
	if ((rda5807m_init[3] & 0x03) == 0x00)
	{
		channelSpacing = 10;
	}
	else if ((rda5807m_init[3] & 0x03) == 0x01)
	{
		channelSpacing = 10;
	}
	else if ((rda5807m_init[3] & 0x03) == 0x02)
	{
		channelSpacing = 5;
	}
	channel = (frequency - bottomOfBand) / channelSpacing;
	return (channel);
}

PUBLIC char fmd_Search(unsigned long freq)
{
	char flag = 0;
	unsigned char i = 0, cnt = 0;
	unsigned long RDA5807P_reg_data[4] = {0};
	unsigned long RDA5807P_channel_seek[] = {0xc0, 0x01};
	unsigned long falseStation = 1;
	unsigned long curChan;
	unsigned char Write_Buffer[4] = {0};
	unsigned char Read_Buffer[4]  = {0};
	curChan = fmd_FreqToChan(freq);
	Write_Buffer[0] = RDA5807P_channel_seek[0];
	Write_Buffer[1] = RDA5807P_channel_seek[1];
	Write_Buffer[2] = curChan >> 2;
	Write_Buffer[3] = (((curChan & 0x0003) << 6) | 0x10) | (Write_Buffer[3] & 0x0f);
	
	fmd_Write(0x00, Write_Buffer, 4);
	sxr_Sleep(80 MILLI_SECONDS);
	fmd_Read(0x0a, Read_Buffer, 4);
	if(((Read_Buffer[2] & 0x01) == 0))
	{
		falseStation = 1;
	}
	else
	{
		falseStation = 0;
	}
	if(freq == 9600 || freq == 10800)
	{
		falseStation = 1;
	}
	
	if (falseStation == 1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
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
	UINT8 writeReg[4] = {0};
	UINT16 chan;
	
	readReg[0]  = rda5807m_init[2];
	readReg[1]  = rda5807m_init[3];
	writeReg[0] = rda5807m_init[0];
	writeReg[1] = rda5807m_init[1];
	
	chan = (freqKHz - g_baseFreq[g_band]) / CHAN_SPACE;
	writeReg[2] = (UINT8)(((chan & 0x03ff) >> 2) & 0xFF); //0x03
	//open I2S ,must use |0x30 ;if colse I2s ,use |0x10
	writeReg[3] = (UINT8)((((chan & 0x03ff) << 6) & 0x00C0) | (readReg[1] & 0x20) | 0x10 | ((g_band & 3) << 2) | (g_fmdConfig->channelSpacing & 3));
	errStatus = fmd_Write(0x02, writeReg, 4);
	
	sxr_Sleep(100 MILLI_SECONDS);
	
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
		//sxr_StopFunctionTimer((void (*)(void *))fmd_Scheduler);
	}
	// put radio in sleep mode (+mute and high Z ?)
	errStatus = fmd_Write(0x02, writeReg, 2); // 0xc000
	if (errStatus != FMD_ERR_NO)
	{
		return errStatus;
	}
	g_band = FMD_BAND_QTY; // close marker
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
	return FMD_ERR_NO;
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
	errStatus = fmd_Read(0x0a, readReg, 2);
	if (errStatus == FMD_ERR_NO)
	{
		*pFreqKHz = (((readReg[0] << 8) | readReg[1]) & 0x3ff) * CHAN_SPACE + g_baseFreq[g_band];
	}
	//hal_HstSendEvent(SYS_EVENT,0x88888801);
	//hal_HstSendEvent(SYS_EVENT,*pFreqKHz);
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
	return FMD_ERR_NO;
	if (pStereoStatus == NULL)
	{
		return FMD_ERR_BAD_PARAMETER;
	}
	
	errStatus = fmd_Read(0x0a, readReg, 2);
	SXS_TRACE(TSTDOUT, "reg0=%x,reg1=%x", readReg[0], readReg[1]);
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
	UINT8 readReg[4] = {0};
	FMD_ERR_T errStatus;
	if (pRSSI == NULL)
	{
		return FMD_ERR_BAD_PARAMETER;
	}
	
	errStatus = fmd_Read(0x0a, readReg, 4);
	SXS_TRACE(TSTDOUT, "reg0=%x,reg1=%x", readReg[0], readReg[1]);
	
	*pRSSI = ((readReg[2] & 0xfe) >> 1);
	return errStatus;
}

PUBLIC BOOL fmd_ValidStop(UINT32 freqKHz)
{
	UINT8 result;
	result = fmd_Search(freqKHz / 10);
	return result;
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
	return FMD_ERR_NO;
}

// =============================================================================
// fmd_I2sClose
// -----------------------------------------------------------------------------
/// This function close the I2S interface
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_I2sClose()
{
	return FMD_ERR_NO;
}

void RDA5807M_init()
{
	UINT8 rda5807m_reg[2] = {0x00, 0x02};
	UINT8 i, rd_data[10];
	BOOL  flag = FALSE;
	gpio_i2c_open();
	fmd_Write(0x00, rda5807m_reg, 2);  //soft reset
	sxr_Sleep(70 MS_WAITING);
	fmd_Write(0x00, rda5807m_init, sizeof(rda5807m_init) / sizeof(rda5807m_init[0]));
	sxr_Sleep(150 MS_WAITING);
	rda5807m_reg[0] = 0x00;
	rda5807m_reg[1] = 0x00;
	fmd_Write(0x00, rda5807m_reg, 2);  //soft off
	gpio_i2c_close();
}


