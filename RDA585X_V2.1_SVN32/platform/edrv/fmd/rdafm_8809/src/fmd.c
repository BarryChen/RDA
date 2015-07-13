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


#ifdef ABB_HP_DETECT
extern VOID aud_FmEnableHpDetect(BOOL enable);
#endif

#ifdef FM_LDO_WORKAROUND
extern VOID pmd_EnableDcdcPower(BOOL on);
#endif


// =============================================================================
//  MACROS
// =============================================================================

#define ADJUST_DAC_GAIN 0
#define PREP_WRITE_REG_DATA(a) {writeReg[0]=((a)&0xff00)>>8;writeReg[1]=(a)&0xff;}

#define BASE_FREQ_USEUROPE  87000
#define BASE_FREQ_JAPAN     76000
#define BASE_FREQ_WORLD     76000
#define BASE_FREQ_EAST_EUROPE     65000

#define CHAN_SPACE          g_chanSpacing[g_fmdConfig->channelSpacing]

// MACROS for PRIVATE FUNCTION USE ONLY
#define FMD_I2C_BPS             g_fmdConfig->i2cBps

#define HAL_I2C_BUS_ID 			g_fmdConfig->i2cBusId

#define SECOND        * HAL_TICK1S
#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR

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
	UINT8 i=0;
	
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,(FMD_CHIP_ADRESS_T<<1),I2C_MASTER_WR | I2C_MASTER_STA);
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,regaddr,I2C_MASTER_WR);

	for(i=0;i<datalen-1;i++,data++)
	{
		halErr=hal_I2cSendRawByte(HAL_I2C_BUS_ID,*data,I2C_MASTER_WR);
	}
	halErr=hal_I2cSendRawByte(HAL_I2C_BUS_ID,*data,I2C_MASTER_WR|I2C_MASTER_STO);
}

void rdafm_iic_read_data(unsigned char regaddr, unsigned char *data, unsigned char datalen)
{
	UINT8 i=0;
	
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,(FMD_CHIP_ADRESS_T<<1),I2C_MASTER_WR | I2C_MASTER_STA);
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,regaddr,I2C_MASTER_WR);
	hal_I2cSendRawByte(HAL_I2C_BUS_ID,((FMD_CHIP_ADRESS_T<<1)|1),I2C_MASTER_WR | I2C_MASTER_STA);

	for(i=0;i<datalen-1;i++,data++)//data
	{	
		(*data)=hal_I2cReadRawByte(HAL_I2C_BUS_ID,I2C_MASTER_RD);
	}
	(*data)=hal_I2cReadRawByte(HAL_I2C_BUS_ID,I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO);
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

    return FMD_ERR_NO;
}

PRIVATE FMD_ERR_T fmd_Read(UINT32 addr, UINT8* data,UINT8 length)
{
    fmd_I2cOpen(FMD_I2C_BPS);
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_read_data(FMD_CHIP_ADRESS,(UINT8* )&addr,1, data, length);
 #else
    //hal_I2cGetData(HAL_I2C_BUS_ID,FMD_CHIP_ADRESS,addr,data,length);
	rdafm_iic_read_data(addr, data, length);
  #endif
    fmd_I2cClose();

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

    FMD_TRACE(EDRV_FMD_TRC,0,"fmd_Scheduler: state=%d", g_state);

    // the POLLING step
    if ((g_state == FMD_STATE_TUNE_POLL) || (g_state == FMD_STATE_SEEK_POLL))
    {
        //errStatus = fmd_Read(0x0a, &rdReg);
        fmd_Read(0x0a,readReg,2);
        FMD_TRACE(EDRV_FMD_TRC,0,"chan(fmd_Scheduler) = %x,reg1=%x",readReg[0],readReg[1]);    
        if (errStatus == FMD_ERR_NO)
        {
            // no error, check STC bit
            if (readReg[0] & 0x40)
            {
                // done: next step:
                FMD_TRACE(EDRV_FMD_TRC,0,"chan(readReg0)#########");  
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

#if (ADJUST_DAC_GAIN)
PRIVATE VOID fmd_SetDacGain(UINT8 db)
{
    UINT8 writeReg[2];
    UINT32 tmp6db, tmp1db;

    // DAC gain bits <5:0>:
    // <5:3> x 6dB, <2> x 2dB, <1> x 2dB, <0> x 1dB

    // The default value is 7 (5dB).
    // This value can be varied in +-3dB. The gains smaller than 2dB will
    // lead to SNR degradation, and the gains larger than 8dB will lead
    // to sound distortion (due to signal saturation).

    if (db > 15) db = 15;
    tmp6db = db / 6;
    tmp1db = db % 6;

    if (tmp1db >= 4) tmp1db |= 0x2;

    PREP_WRITE_REG_DATA(0x0080|((tmp6db&0x7)<<3)|(tmp1db&0x7));
    fmd_Write(0x1f, writeReg, 2);
}
#endif
PUBLIC FMD_ERR_T fmd_Open(FMD_BAND_T band,
                          FMD_CALLBACK_T* callback,
                          FMD_INIT_T* initValues)
{
    CONST TGT_FMD_CONFIG_T* fmdTgtCfg = tgt_GetFmdConfig();
    UINT8 dacVol;
    UINT16 chan;
    UINT8 writeReg[2];

    FMD_TRACE(EDRV_FMD_TRC,0,"fmd_Open");

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
        chan = (initValues->freqKHz - g_baseFreq[g_band])/CHAN_SPACE;
    }

    // enable the clock
    g_fmdClockOutId = hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
    if (HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE == g_fmdClockOutId)
    {
        return FMD_ERR_RESOURCE_BUSY;
    }
    // enable the POWER
    pmd_EnablePower(PMD_POWER_FM, TRUE);
#ifdef FM_LDO_WORKAROUND
    pmd_EnableDcdcPower(FALSE);
#endif

    // Wait for at least powerOnTime
    sxr_Sleep(g_fmdConfig->powerOnTime);

    // Soft reset and enable
    PREP_WRITE_REG_DATA(0x0002);
    fmd_Write(0x02, writeReg, 2);
    sxr_Sleep(300 MILLI_SECONDS);
    PREP_WRITE_REG_DATA(g_fmRegSysCtrl|0x8001);
    fmd_Write(0x02, writeReg, 2);
    sxr_Sleep(10 MILLI_SECONDS);
    // Differential mode
    PREP_WRITE_REG_DATA(0x88cf);
    fmd_Write(0x05, writeReg, 2);
    PREP_WRITE_REG_DATA(0x6000);
    fmd_Write(0x06, writeReg, 2);
    PREP_WRITE_REG_DATA(0x6800);
    fmd_Write(0x5b, writeReg, 2);

#ifdef AUD_3_IN_1_SPK
    // Hw default: 0x2bec
    PREP_WRITE_REG_DATA(0x03ec);
    fmd_Write(0x28, writeReg, 2);
#endif

#if (ADJUST_DAC_GAIN)
    fmd_SetDacGain(dacVol);
#endif
#if 1
    UINT8 readReg[2];
    fmd_Read(0x02, readReg, 2);
    FMD_TRACE(EDRV_FMD_TRC,0,"0x02: %02x %02x", readReg[0], readReg[1]);
    fmd_Read(0x05, readReg, 2);
    FMD_TRACE(EDRV_FMD_TRC,0,"0x05: %02x %02x", readReg[0], readReg[1]);
    fmd_Read(0x06, readReg, 2);
    FMD_TRACE(EDRV_FMD_TRC,0,"0x06: %02x %02x", readReg[0], readReg[1]);
    fmd_Read(0x5b, readReg, 2);
    FMD_TRACE(EDRV_FMD_TRC,0,"0x5b: %02x %02x", readReg[0], readReg[1]);
#endif

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

    FMD_TRACE(EDRV_FMD_TRC,0,"fmd_Tune: freqKHz=%d",freqKHz);

#ifdef ABB_HP_DETECT
    aud_FmEnableHpDetect(FALSE);
#endif

    errStatus = fmd_Read(0x03, readReg,2);

    chan = (freqKHz - g_baseFreq[g_band])/CHAN_SPACE;
    writeReg[0] = (UINT8)(((chan & 0x03ff)>>2) & 0xFF);//0x03
    //open I2S ,must use |0x30 ;if colse I2s ,use |0x10
    writeReg[1] = (UINT8)((((chan & 0x03ff) << 6)&0x00C0) | (readReg[1]&0x20)| 0x10 | ((g_band & 3) << 2) | (g_fmdConfig->channelSpacing & 3));
    errStatus = fmd_Write(0x03, writeReg,2);

#ifdef ABB_HP_DETECT
    sxr_Sleep(50 MS_WAITING);
    aud_FmEnableHpDetect(TRUE);
#endif

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

    FMD_TRACE(EDRV_FMD_TRC,0,"fmd_Close");

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
        COS_KillTimer((void (*)(void *))fmd_Scheduler);
    }
    // put radio in sleep mode (+mute and high Z ?)
    errStatus = fmd_Write(0x02, writeReg,2); // 0xc000
    if (errStatus != FMD_ERR_NO)
    {
        return errStatus;
    }

#ifdef FM_LDO_WORKAROUND
    pmd_EnableDcdcPower(TRUE);
#endif
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

    FMD_TRACE(EDRV_FMD_TRC,0,"fmd_Seek");

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
    COS_SetTimer(100 MILLI_SECONDS, fmd_Scheduler, 0, 0);
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
    FMD_ASSERT(volume < FMD_ANA_VOL_QTY, "FMD volume out of range (%d)", volume);

    FMD_TRACE(EDRV_FMD_TRC,0,"fmd_SetVolume: vol=%d, boost=%d, mono=%d",
        volume, bassBoost, forceMono);

    // compute the mute bit
    if (volume == FMD_ANA_MUTE)
    {
        g_fmRegSysCtrl &= ~0x4000;
    }
    else
    {
        g_fmRegSysCtrl |= 0x4000;
    }
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
    writeReg[0] = (UINT8)(0x80 | ((g_fmRegSysCtrl&0xFF00)>>8));//0x02
    writeReg[1] = (UINT8)(0x01 | (g_fmRegSysCtrl&0x00FF));
    errStatus = fmd_Write(0x02, writeReg, 2);

#if 0
    sxr_Sleep(50 MILLI_SECONDS);

    // get volume value from config
    UINT8 dacVol;
    dacVol = g_fmdConfig->volumeVal[volume];
    writeReg[0] = (RDA5876_initialization_reg[4] & 0xf0) | (g_fmdConfig->seekRSSIThreshold & 0x0f);//0x05
    writeReg[1] = (RDA5876_initialization_reg[5] & 0xf0) | (dacVol & 0x0f);
    errStatus = fmd_Write(0x05, writeReg, 2);
#endif
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

    FMD_TRACE(EDRV_FMD_TRC,0,"reg0=%x,reg1=%x",readReg[0],readReg[1]);   

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

    FMD_TRACE(EDRV_FMD_TRC,0,"reg0=%x,reg1=%x",readReg[0],readReg[1]);   

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
DEPRECATED PUBLIC FMD_ERR_T fmd_GetRssi(UINT8* pRSSI)
{
    UINT8 readReg[2] = {0};
    FMD_ERR_T errStatus;

    if (pRSSI == NULL)
    {
        return FMD_ERR_BAD_PARAMETER;
    } 

    errStatus = fmd_Read(0x0b, readReg, 2);

    FMD_TRACE(EDRV_FMD_TRC,0,"reg0=%x,reg1=%x",readReg[0],readReg[1]);

    *pRSSI = ((readReg[0] & 0xfe) >> 1); 

    return errStatus;
}

PUBLIC BOOL fmd_ValidStop(UINT32 freqKHz)
{
    BOOL result;
    FMD_ERR_T errStatus;
    UINT8 readReg[4] = {0};
    UINT8 writeReg[2] = {0};
    UINT16 chan;
	UINT8 i = 0;

#ifdef ABB_HP_DETECT
    aud_FmEnableHpDetect(FALSE);
#endif

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
#if 0
        //add some log
        FMD_TRACE(EDRV_FMD_TRC,0,"fmd_ValidStop readReg[0]=0x%x",readReg[0]);   	
        FMD_TRACE(EDRV_FMD_TRC,0,"fmd_ValidStop readReg[1]=0x%x",readReg[1]);   	
        FMD_TRACE(EDRV_FMD_TRC,0,"fmd_ValidStop readReg[2]=0x%x",readReg[2]);   	
        FMD_TRACE(EDRV_FMD_TRC,0,"fmd_ValidStop readReg[3]=0x%x",readReg[3]);   	
#endif
	} while((readReg[3]&0x80)==0);	

#ifdef ABB_HP_DETECT
    aud_FmEnableHpDetect(TRUE);
#endif

	if ((readReg[2] & 0x01) == 0)
	{
        result = FALSE;
	}
    else
    {
        result = TRUE;
    }

    FMD_TRACE(EDRV_FMD_TRC,0,"fmd_ValidStop: freqKHz=%d, result=%d",
        freqKHz, result);

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
#if 0	
    UINT8 readReg[2] = {0};
    UINT8 writeReg[2] = {0};
    FMD_ERR_T errStatus;
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
    //  FMD_TRACE(EDRV_FMD_TRC,0,"fmd_Read=%x,fmd_Read=%x",readReg[0],readReg[1]);    
    writeReg[0] = readReg[0];
    writeReg[1] = readReg[1]|(1<<5);   
    errStatus = fmd_Write(0x03, writeReg,2);

    sxr_Sleep(50 MILLI_SECONDS);
    errStatus = fmd_Read(0x09, readReg,2);
    // FMD_TRACE(EDRV_FMD_TRC,0,"fmd_Read=%x,fmd_Read=%x",readReg[0],readReg[1]);  	
    writeReg[0] = readReg[0];
    writeReg[1] = readReg[1]|(1<<0);
    errStatus = fmd_Write(0x09, writeReg,2);
    return errStatus;
#else
    return FMD_ERR_NO;
#endif    
}

// =============================================================================
// fmd_I2sClose
// -----------------------------------------------------------------------------
/// This function close the I2S interface
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_I2sClose()
{
#if 0 
    UINT8 readReg[2] = {0};
    UINT8 writeReg[2] = {0};
    FMD_ERR_T errStatus;

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
    return errStatus;
#else
    return FMD_ERR_NO;
#endif    
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

