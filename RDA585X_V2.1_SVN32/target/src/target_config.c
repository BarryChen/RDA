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
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file target_config.c                                                     //
/// That file implements the TGT module: configuration structure storage      //
/// and accessor implementation for all firmware's module needing a specific  //
/// configuration.                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


/* *************************************************************
 * DO NOT EDIT THIS FILE 
 * *************************************************************
 * To setup the board configuration, edit the appropriate board_config.h 
 * in the appropriate target.
 * This file must be recompiled for the setup to be taken into account 
 */


#include "cs_types.h"

#include "chip_id.h"

#include "hal_config.h"

#include "rfd_config.h"
#include "pmd_config.h"
#include "memd_config.h"
#include "gpio_config.h"
#include "tgt_m.h"
#include "tgt_aud_cfg.h"
#include "tgt_hal_cfg.h"
#include "tgt_pmd_cfg.h"
#include "csw_config.h"
#include "tgt_calib_m.h"
#include "tgt_calib_cfg.h"
/// MCD is optional so only link to MCD if needed
#ifdef TGT_WITH_MCD
#include "mcd_config.h"
#include "mcd_m.h"
#include "tgt_mcd_cfg.h"
#endif
#ifdef TGT_WITH_TS
#include "tsd_m.h"
#include "tsd_config.h"
//#include "tgt_tsd_cfg.h"
#endif
/// FM is optional so only link to FM if needed
#ifdef TGT_WITH_FM
#include "fmd_config.h"
#include "tgt_fmd_cfg.h"
#endif

/// GPIOI2C is optional so only link to GPIOI2C if needed
#ifdef TGT_WITH_GPIOI2C
#include "i2cgpio_config.h"
#include "tgt_gpioi2c_cfg.h"
#endif

/// BT is optional so only link to BT if needed
#ifdef TGT_WITH_BT
#include "btd_config.h"
#include "tgt_btd_cfg.h"
#include "btd_map.h"
#endif

#include "tgt_board_cfg.h"
#include "tgt_app_cfg.h"
#include "key_defs.h"
#include "hal_key.h"
#include "hal_gpio.h"
#include "hal_ebc.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "hal_mem_map.h"

#include "memd_config.h"

#include "aud_m.h"
#include "aud_null.h"
//#include "aud_analog.h"
//#include "aud_maxim.h"
#include "aud_codec_gallite.h"
#include "aud_fm.h"
#include "aud_linein.h"
#if (CSW_EXTENDED_API == 0)
#include "aud_bt.h"
#endif /* CSW_EXTENDED_API */


#include "hal_map.h"
#include "lcdd_map.h"
#include "memd_map.h"
#include "pmd_map.h"
#include "std_map.h"
#include "sx_map.h"
//#include "vois_map.h"
//#include "vpp_map.h"
// #include "uctls_map.h"
#ifdef WITH_MDI
#include "mdi_map.h"
#endif
#ifdef TGT_WITH_EXPKEY
#include "tgt_expkey_cfg.h"
#endif

#include "dsm_dev_driver.h"
#include "dsm_config.h"

#include "tgt_dsm_cfg.h"

#include "memd_m.h"
#include "GlobalConstants.h"
#include "bal_config.h"


/// Start address of the factory settings sector, in the flash
// Defined by the environment to the symbol _factory_start
// declared in hal_mem_map.h
#define g_tgtFactorySettings (*((TGT_FACTORY_SETTINGS_T*)&_factory_start))


PROTECTED CONST TGT_HW_MODULE_T g_tgtHwModule = 
{
    .target = STRINGIFY_VALUE(CT_TARGET),
    .chipAsic = STRINGIFY_VALUE(CT_ASIC),
    .chipDie = STRINGIFY_VALUE(CT_CHIP_DIE),
    .chipPkg = STRINGIFY_VALUE(CT_CHIP_PKG),
    .rfPa = STRINGIFY_VALUE(PA_MODEL),
    .flash = STRINGIFY_VALUE(FLSH_MODEL),
    .lcd = STRINGIFY_VALUE(LCD_MODEL),
    .cam = STRINGIFY_VALUE(CAMERA_MODEL),
    .bt = STRINGIFY_VALUE(BT_MODEL),
    .fm = STRINGIFY_VALUE(FM_MODEL),
    .atv = STRINGIFY_VALUE(ATV_MODEL),
};

// HAL configuration structure
PROTECTED CONST HAL_CFG_CONFIG_T g_tgtHalCfg = TGT_HAL_CONFIG;

#define TGT_AUD_FUNC_NAME(s, func) aud_ ## s ## func

#define TGT_STREAM_START_DRIVER(driver)         TGT_AUD_FUNC_NAME(driver, StreamStart)
#define TGT_STREAM_RECORD_DRIVER(driver)        TGT_AUD_FUNC_NAME(driver,StreamRecord)
#define TGT_STREAM_STOP_DRIVER(driver)          TGT_AUD_FUNC_NAME(driver,StreamStop)
#define TGT_STREAM_PAUSE_DRIVER(driver)         TGT_AUD_FUNC_NAME(driver,StreamPause)
#define TGT_SETUP_DRIVER(driver)                TGT_AUD_FUNC_NAME(driver,Setup)
#define TGT_TONE_DRIVER(driver)                 TGT_AUD_FUNC_NAME(driver,Tone)
#define TGT_TONE_PAUSE_DRIVER(driver)           TGT_AUD_FUNC_NAME(driver,TonePause)
#define TGT_CALIB_UPDATE_VALUES_DRIVER(driver)  TGT_AUD_FUNC_NAME(driver,CalibUpdateValues)



// AUD configuration structure
PROTECTED CONST AUD_CONFIG_T g_tgtAudCfg = 
{
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_AUD_CONFIG_RECEIVER_PARAM,
        TGT_AUD_CONFIG_RECEIVER_OUTPUT_PATH,
        TGT_AUD_CONFIG_RECEIVER_OUTPUT_TYPE,
        TGT_AUD_CONFIG_RECEIVER_INPUT_PATH,
        TGT_AUD_CONFIG_RECEIVER_MIXAUDIO
    },
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_AUD_CONFIG_EAR_PIECE_PARAM,
        TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_PATH,
        TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_TYPE,
        TGT_AUD_CONFIG_EAR_PIECE_INPUT_PATH,
        TGT_AUD_CONFIG_EAR_PIECE_MIXAUDIO
    },
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_AUD_CONFIG_LOUD_SPEAKER_PARAM,
        TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_PATH,
        TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_TYPE,
        TGT_AUD_CONFIG_LOUD_SPEAKER_INPUT_PATH,
        TGT_AUD_CONFIG_LOUD_SPEAKER_MIXAUDIO
    },
#if 0 //def TGT_WITH_BT
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_AUD_CONFIG_BT_PARAM,
        TGT_AUD_CONFIG_BT_OUTPUT_PATH,
        TGT_AUD_CONFIG_BT_OUTPUT_TYPE,
        TGT_AUD_CONFIG_BT_INPUT_PATH,
        TGT_AUD_CONFIG_BT_MIXAUDIO
    },
#else // TGT_WITH_BT
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    },
#endif // TGT_WITH_BT
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_BT_EP_DRIVER),
        TGT_AUD_CONFIG_BT_EP_PARAM,
        TGT_AUD_CONFIG_BT_EP_OUTPUT_PATH,
        TGT_AUD_CONFIG_BT_EP_OUTPUT_TYPE,
        TGT_AUD_CONFIG_BT_EP_INPUT_PATH,
        TGT_AUD_CONFIG_BT_EP_MIXAUDIO
    },
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_BT_SP_DRIVER),
        TGT_AUD_CONFIG_BT_SP_PARAM,
        TGT_AUD_CONFIG_BT_SP_OUTPUT_PATH,
        TGT_AUD_CONFIG_BT_SP_OUTPUT_TYPE,
        TGT_AUD_CONFIG_BT_SP_INPUT_PATH,
        TGT_AUD_CONFIG_BT_SP_MIXAUDIO
    },
#ifdef TGT_WITH_FM
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_AUD_CONFIG_FM_PARAM,
        TGT_AUD_CONFIG_FM_OUTPUT_PATH,
        TGT_AUD_CONFIG_FM_OUTPUT_TYPE,
        TGT_AUD_CONFIG_FM_INPUT_PATH,
        TGT_AUD_CONFIG_FM_MIXAUDIO
    },
#else // TGT_WITH_FM
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    },
#endif // TGT_WITH_FM
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),      
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),      
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),      
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),             
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_LINEIN_DRIVER),
        TGT_AUD_CONFIG_LINEIN_PARAM,
        TGT_AUD_CONFIG_LINEIN_OUTPUT_PATH,
        TGT_AUD_CONFIG_LINEIN_OUTPUT_TYPE,
        TGT_AUD_CONFIG_LINEIN_INPUT_PATH,
        TGT_AUD_CONFIG_LINEIN_MIXAUDIO
    },
};

PROTECTED CONST PMD_CONFIG_T g_tgtPmdCfg = TGT_PMD_CONFIG;

#ifdef INTERNAL_XCV_CONTROL_PASW
#include "hal_tcu.h"
#undef TGT_XCV_CONFIG
#undef TGT_PA_CONFIG
#undef TGT_SW_CONFIG
#define TGT_XCV_CONFIG    {.RST = TCO_UNUSED, .PDN  = TCO(11)               }
#define TGT_PA_CONFIG     {.ENA = TCO_UNUSED, .TXEN = TCO_UNUSED, .BS  = TCO_UNUSED }
#define TGT_SW_CONFIG     {.SW1 = TCO_UNUSED, .SW2  = TCO_UNUSED, .SW3 = TCO_UNUSED }
#endif

PROTECTED CONST XCV_CONFIG_T g_tgtXcvCfg = TGT_XCV_CONFIG;

PROTECTED CONST CSW_CONFIG_T g_tgtCswCfg = TGT_CSW_CONFIG;

// =============================================================================
// g_tgtDsmCfg
// -----------------------------------------------------------------------------
/// Dsm configuration.
// =============================================================================
PROTECTED CONST DSM_CONFIG_T g_tgtDsmCfg = TGT_DSM_CONFIG;


// gpio config define
PROTECTED CONST GPIO_CFG_CONFIG_T g_tgtGPIOCfg = TGT_GPIO_CONFIG_CFG;
// =============================================================================
// tgt_GetGPIOCfg
// -----------------------------------------------------------------------------
// This function is used by gpio driver to get gpio config.
// 
// =============================================================================
PUBLIC CONST GPIO_CFG_CONFIG_T* tgt_GetGPIOCfg(VOID)
{
    return &g_tgtGPIOCfg;
}


// Key Map Define
PROTECTED CONST UINT8 g_tgtKeyMap[] = KEY_MAP;
PROTECTED CONST BAL_CFG_KEY_T g_tgtKeyCfg = KEY_CONFIG;

// =============================================================================
// tgt_GetKeyMap
// -----------------------------------------------------------------------------
// This function is used by BAL_Task to get access to Key Map.
// 
// =============================================================================
PUBLIC CONST BAL_CFG_KEY_T* tgt_GetKeyCfg(VOID)
{
    return &g_tgtKeyCfg;
}

// =============================================================================
// tgt_GetKeyCode
// -----------------------------------------------------------------------------
/// This function returns the code of a key from its column number. It 
/// implements thus the key mapping, between the key detected and the 
/// keyboard of a given target.
/// @param key Key number as given by the HAL Key driver
/// @return The key code as mapped for a specifc target.
// =============================================================================
PUBLIC UINT8 tgt_GetKeyCode(UINT8 key)
{
    return g_tgtKeyMap[key];
}

// IRC define
PROTECTED CONST UINT16 g_tgtIRCStyle[IRC_MAX_STYLE] = IRC_STYLE_MAP;
PROTECTED CONST UINT8 g_tgtIRCMap[IRC_CODE_MAP_SIZE][IRC_MAX_STYLE] = IRC_MAP;
PROTECTED CONST BAL_CFG_IRC_T g_tgtIRCCfg = IRC_CONFIG;

// =============================================================================
// tgt_GetKeyMap
// -----------------------------------------------------------------------------
// This function is used by BAL_Task to get access to Key Map.
// 
// =============================================================================
PUBLIC CONST BAL_CFG_IRC_T* tgt_GetIRCCfg(VOID)
{
    return &g_tgtIRCCfg;
}


#ifdef TGT_WITH_MCD
// =============================================================================
// g_tgtMcdCfg
// -----------------------------------------------------------------------------
/// MCD configuration.
// =============================================================================
PROTECTED CONST TGT_MCD_CONFIG_T g_tgtMcdCfg = TGT_MCD_CONFIG;
#endif


#ifdef TGT_WITH_FM
// =============================================================================
// g_tgtFmdCfg
// -----------------------------------------------------------------------------
/// FM Driver configuration.
// =============================================================================
PROTECTED CONST TGT_FMD_CONFIG_T g_tgtFmdCfg = TGT_FMD_CONFIG;
#endif

#ifdef TGT_WITH_GPIOI2C
// =============================================================================
// 
// -----------------------------------------------------------------------------
/// GpioI2c Driver configuration.
// =============================================================================
PROTECTED  CONST TGT_GPIOI2C_CONFIG_T g_tgtGpioi2cCfg = TGT_GPIOI2C_CONFIG;
#endif

#ifdef TGT_WITH_GPIOSPI
// =============================================================================
// 
// -----------------------------------------------------------------------------
/// GpioI2c Driver configuration.
// =============================================================================
PROTECTED  CONST TGT_GPIOSPI_CONFIG_T g_tgtGpiospiCfg = TGT_GPIOSPI_CONFIG;
#endif

#ifdef TGT_WITH_BT
// =============================================================================
// g_tgtBtdCfg
// -----------------------------------------------------------------------------
/// BT Driver configuration.
// =============================================================================
PROTECTED CONST TGT_BTD_CONFIG_T g_tgtBtdCfg = TGT_BTD_CONFIG;
#endif
#ifdef TGT_WITH_TS
// =============================================================================
// g_tgtTsdCfg
// -----------------------------------------------------------------------------
/// TSD configuration.
// =============================================================================
PROTECTED CONST struct TSD_CONFIG_OPAL_STRUCT_T g_tgtTsdCfg = TGT_TSD_CONFIG;
#endif

// =============================================================================
// g_tgtMemdFlashCfg
// -----------------------------------------------------------------------------
/// MEMD Flash configuration configuration.
// =============================================================================
PROTECTED CONST MEMD_FLASH_CONFIG_T 
    HAL_BOOT_RODATA g_tgtMemdFlashCfg = TGT_MEMD_FLASH_CONFIG;


// =============================================================================
// g_tgtMemdRamCfg
// -----------------------------------------------------------------------------
/// MEMD Ram configuration.
// =============================================================================
#ifdef RDA_PSRAM_ADAPTIVE

#ifdef GALLITE_IS_8806
PROTECTED CONST MEMD_RAM_CONFIG_T 
    HAL_BOOT_RODATA g_tgtMemdRamCfg = TGT_MEMD_RAM_CONFIG;
#else // !GALLITE_IS_8806
PROTECTED CONST MEMD_RAM_CONFIG_T
    HAL_BOOT_RODATA g_tgtMemdRamCfg[RDA_EXTSAM_QTY] = 
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
{
    TGT_MEMD_RAM_CONFIG,\
    TGT_RAM_CONFIG_8808SAP,\
    TGT_RAM_CONFIG_8808S_EMC,\
    TGT_RAM_CONFIG_8808S_WINBOND,\
    TGT_RAM_CONFIG_8808S_ETRON,
};
#else
{
    TGT_MEMD_RAM_CONFIG,\
    TGT_RAM_CONFIG_8809_AP,\
    TGT_RAM_CONFIG_8809_ETRON,\
    TGT_RAM_CONFIG_8809_WINBOND,\
    TGT_RAM_CONFIG_8809_EMC,
};
#endif
#endif // !GALLITE_IS_8806

#else // !RDA_PSRAM_ADAPTIVE

PROTECTED CONST MEMD_RAM_CONFIG_T 
    HAL_BOOT_RODATA g_tgtMemdRamCfg = TGT_MEMD_RAM_CONFIG;

#ifdef RAM_CLK_IS_104M
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
PROTECTED CONST MEMD_RAM_CONFIG_T 
    HAL_BOOT_RODATA g_tgtMemd8808SRamCfg = TGT_MEMD_8808S_RAM_CONFIG;
#endif
#endif

#endif // !RDA_PSRAM_ADAPTIVE


PUBLIC CALIB_BUFFER_T HAL_UNCACHED_DATA_INTERNAL g_tgtCalibCfg = 
{
    .aud = 
    {
        .vocItfNums = CALIB_AUDIO_VOC_ITF_QTY,
        .gainItfNums = CALIB_AUDIO_GAIN_ITF_QTY,
        .vocItfMap = DEFAULT_CALIB_AUDIO_VOC_MAP,
        .gainItfMap = DEFAULT_CALIB_AUDIO_GAIN_MAP,
        .vocFilters = DEFAULT_CALIB_SDF_ALL,
        .audioGains = DEFAULT_CALIB_GAINS_ALL,
        .echo = {DEFAULT_CALIB_ECHO_CONFIG0, DEFAULT_CALIB_ECHO_CONFIG1},
        .sbcFilter = DEFAULT_CALIB_SBC_FILTER_CONFIG,
        .sbcEQ = DEFAULT_CALIB_SBC_EQ
    }
};


// =============================================================================
// tgt_GetBuildVerNo
// -----------------------------------------------------------------------------
/// This function is used to access the version number of this build.
// =============================================================================
PUBLIC CONST CHAR* tgt_GetBuildVerNo(VOID)
{
    const static char* verno = STRINGIFY_VALUE(CT_PRODUCT);
    return verno;
}

// =============================================================================
// tgt_GetBuildDateTime
// -----------------------------------------------------------------------------
/// This function is used to access the date time of this build.
// =============================================================================
PUBLIC CONST CHAR* tgt_GetBuildDateTime(VOID)
{
    const static char* datetime = STRINGIFY_VALUE(BUILD_TIME);
    return datetime;
}

// =============================================================================
// tgt_GetHwModule
// -----------------------------------------------------------------------------
/// This function is used to access the hardware module names on this target.
// =============================================================================
PUBLIC CONST TGT_HW_MODULE_T* tgt_GetHwModule(VOID)
{
    return &g_tgtHwModule;
}

// =============================================================================
// tgt_GetMobileId
// -----------------------------------------------------------------------------
/// This function is used to access the Mobile ID description.
// =============================================================================
PUBLIC CONST TGT_MOBILE_ID_T* tgt_GetMobileId(VOID)
{
    return &(g_tgtFactorySettings.mobileId);
}

// =============================================================================
// tgt_GetFactorySettings
// -----------------------------------------------------------------------------
/// This function is used to access the factory settings.
// =============================================================================
#if defined(FACT_SETTING_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

TGT_FACTORY_SETTINGS_T* tgt_GetFactorySettings(VOID)
{
    if(g_tgtFactorySettingIsDirty)
    {
        return (TGT_FACTORY_SETTINGS_T*)&g_tgtFactorySetting;
    }
    else
    {
        return (TGT_FACTORY_SETTINGS_T*)&_factory_start; 
    }
}

#else
TGT_FACTORY_SETTINGS_T* tgt_GetFactorySettings(VOID)
{
    return (TGT_FACTORY_SETTINGS_T*)&_factory_start;
}

#endif


// =============================================================================
// tgt_GetHalConfig
// -----------------------------------------------------------------------------
/// This function is used by HAL to get access to its configuration structure.
/// This is the only way HAL can get this information.
// =============================================================================
PUBLIC CONST HAL_CFG_CONFIG_T* tgt_GetHalConfig(VOID)
{
    return &g_tgtHalCfg;
}

// =============================================================================
// tgt_GetAudConfig
// -----------------------------------------------------------------------------
/// This function is used by Aud to get access to its configuration structure.
/// This is the only way Aud can get this information.
// =============================================================================
PUBLIC CONST AUD_ITF_CFG_T* tgt_GetAudConfig(VOID)
{
    return g_tgtAudCfg;
}


// =============================================================================
// tgt_GetPmdConfig
// -----------------------------------------------------------------------------
/// This function is used by PMD to get access to its configuration structure.
/// This is the only way PMD can get this information.
// =============================================================================
PUBLIC CONST PMD_CONFIG_T* tgt_GetPmdConfig(VOID)
{
   return &g_tgtPmdCfg;
}



#ifdef TGT_WITH_MCD
// =============================================================================
// tgt_GetMcdConfig
// -----------------------------------------------------------------------------
/// This function is used by MCD to get access to its configuration structure.
/// This is the only way MCD can get this information.
// =============================================================================
PUBLIC CONST TGT_MCD_CONFIG_T* tgt_GetMcdConfig(VOID)
{
    return &g_tgtMcdCfg;
}
#endif


#ifdef TGT_WITH_FM
// =============================================================================
// tgt_GetFmdConfig
// -----------------------------------------------------------------------------
/// This function is used by FMD to get access to its configuration structure.
/// This is the only way FMD can get this information.
// =============================================================================
PUBLIC CONST TGT_FMD_CONFIG_T* tgt_GetFmdConfig(VOID)
{
    return &g_tgtFmdCfg;
}
#endif


#ifdef TGT_WITH_GPIOI2C
// =============================================================================
// tgt_GetGpioi2cConfig   
// -----------------------------------------------------------------------------
/// This function is used by GpioI2c to get access to its configuration structure.
/// This is the only way GpioI2c can get this information.
// =============================================================================
PUBLIC CONST TGT_GPIOI2C_CONFIG_T* tgt_GetGpioi2cConfig(VOID)
{
    return &g_tgtGpioi2cCfg;
}
#endif

#ifdef TGT_WITH_GPIOSPI
// =============================================================================
// tgt_GetGpioi2cConfig   
// -----------------------------------------------------------------------------
/// This function is used by GpioI2c to get access to its configuration structure.
/// This is the only way GpioI2c can get this information.
// =============================================================================
PUBLIC CONST TGT_GPIOSPI_CONFIG_T* tgt_GetGpiospiConfig(VOID)
{
    return &g_tgtGpiospiCfg;
}
#endif


#ifdef TGT_WITH_BT
// =============================================================================
// tgt_GetBtdConfig
// -----------------------------------------------------------------------------
/// This function is used by BTD to get access to its configuration structure.
/// This is the only way BTD can get this information.
// =============================================================================
PUBLIC CONST TGT_BTD_CONFIG_T* tgt_GetBtdConfig(VOID)
{
    return &g_tgtBtdCfg;
}
#endif


#ifdef TGT_WITH_TS
// =============================================================================
// tgt_GetTsdConfig
// -----------------------------------------------------------------------------
/// This function is used by TSD to get access to its configuration structure.
/// This is the only way TSD can get this information.
// =============================================================================
PUBLIC CONST TSD_CONFIG_T* tgt_GetTsdConfig(VOID)
{
    return &g_tgtTsdCfg;
}
#endif
// tgt_GetMemdFlashConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Flash configuration structure.
// =============================================================================
PUBLIC CONST MEMD_FLASH_CONFIG_T* HAL_BOOT_FUNC tgt_GetMemdFlashConfig(VOID)
{
    return &g_tgtMemdFlashCfg;
}


#ifdef RDA_PSRAM_ADAPTIVE
extern UINT8 hal_BootSectorGetEbcRamId(VOID);
#endif

// =============================================================================
// tgt_GetMemdRamConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Ram configuration structure.
// =============================================================================
PUBLIC CONST MEMD_RAM_CONFIG_T* HAL_BOOT_FUNC tgt_GetMemdRamConfig(VOID)
{
    CONST MEMD_RAM_CONFIG_T* ramCfg;

#ifdef RDA_PSRAM_ADAPTIVE

#ifdef GALLITE_IS_8806
    ramCfg = &g_tgtMemdRamCfg;
#else
    ramCfg = &(g_tgtMemdRamCfg[hal_BootSectorGetEbcRamId()]);
#endif

#else // !RDA_PSRAM_ADAPTIVE

    ramCfg = &g_tgtMemdRamCfg;

#ifdef RAM_CLK_IS_104M
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) //8808 || 8808S
    if (HAL_SYS_CHIP_PROD_ID_8808S == hal_SysGetChipId(HAL_SYS_CHIP_PROD_ID))
        ramCfg =  &g_tgtMemd8808SRamCfg;
#endif
#endif

#endif // !RDA_PSRAM_ADAPTIVE

    return ramCfg;
}


#ifdef __PRJ_WITH_SPILCD__
// =============================================================================
// tgt_GetSpiLcdConfig
// -----------------------------------------------------------------------------
/// This function is used by SPILCD to get access to its configuration structure.
/// This is the only way SPILCD can get this information.
// =============================================================================
PUBLIC CONST SPILCD_CONFIG_STRUCT_T* tgt_GetSpiLcdConfig(VOID)
{
    return &g_tgtSpiLcdCfg;
}
#endif

PUBLIC CONST XCV_CONFIG_T* tgt_GetXcvConfig(VOID)
{
    return &g_tgtXcvCfg;
}


// =============================================================================
// tgt_GetDsmConfig
// -----------------------------------------------------------------------------
// This function is used by DSM to get access to its configuration structure.
// 
// =============================================================================
PUBLIC CONST DSM_CONFIG_T* tgt_GetDsmCfg(VOID)
{
    return &g_tgtDsmCfg;
}


// =============================================================================
// tgt_GetCswConfig
// -----------------------------------------------------------------------------
/// User heap configration
// =============================================================================
PUBLIC CONST CSW_CONFIG_T* tgt_GetCswConfig(VOID)
{
    return &g_tgtCswCfg;
}

// =============================================================================
// tgt_GetCalibConfig
// -----------------------------------------------------------------------------
/// Get Calibration data buffer
// =============================================================================
PUBLIC CALIB_BUFFER_T* tgt_GetCalibConfig(VOID)
{
    return &g_tgtCalibCfg;
}


// tgt_RegisterAllModules
// -----------------------------------------------------------------------------
/// This function call the register functions for each modules of platform.
// =============================================================================
PUBLIC VOID tgt_RegisterAllModules(VOID)
{

}
