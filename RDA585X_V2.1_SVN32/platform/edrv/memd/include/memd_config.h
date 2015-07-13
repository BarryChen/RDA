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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/memd/include/memd_config.h $ //
//	$Author: admin $                                                        // 
//	$Date: 2012-12-01 12:08:32 +0800 (周六, 01 十二月 2012) $                     //   
//	$Revision: 17939 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file memd_config.h                                                       //
/// That file defines the configuration structures used by MEMD.              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _MEMD_CONFIG_H
#define _MEMD_CONFIG_H

#include "cs_types.h"
#include "chip_id.h"
#include "hal_ebc.h"

// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// MEMD_FLASH_CONFIG_T
// -----------------------------------------------------------------------------
/// That type describes the settings to apply to configure the flash driver
/// to get the optimal performances.
// =============================================================================
typedef struct
{
    CONST HAL_EBC_CS_CFG_T csConfig;
} MEMD_FLASH_CONFIG_T;

// =============================================================================
// MEMD_RAM_CONFIG_T
// -----------------------------------------------------------------------------
/// That type describes the settings to apply to configure the ram driver
/// to get the optimal performances.
// =============================================================================
typedef struct
{
    CONST HAL_EBC_CS_CFG_T csConfig;
} MEMD_RAM_CONFIG_T;

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//{TGT_MEMD_RAM_CONFIG,TGT_RAM_CONFIG_8808SAP,TGT_RAM_CONFIG_8808S_EMC,TGT_RAM_CONFIG_8808S_WINBOND,TGT_RAM_CONFIG_8809_AP};
//{TGT_MEMD_RAM_CONFIG,TGT_RAM_CONFIG_8808SAP,TGT_RAM_CONFIG_8808S_EMC,TGT_RAM_CONFIG_8808S_WINBOND,TGT_RAM_CONFIG_8809_AP,RDA8808S_ETRON};

typedef enum
{    
    RDA_APTIC,
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)     
    RDA8808S_APTIC,
    RDA8808S_EMC,   //MC323SP16BKR
    RDA8808S_WINBOND, //W955D6G
    RDA8808S_ETRON,    
#else      
    RDA8809_APTIC,
    RDA8809_ETRON,
    RDA8809_WINBOND,
    RDA8809_EMC,
#endif
    RDA_EXTSAM_QTY

}RDA_PSRAM_TYPE_T;
// =============================================================================
//  FUNCTIONS
// =============================================================================


#endif // _MEMD_CONFIG_H

