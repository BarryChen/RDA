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
//  $HeadURL$ //
//	$Author$      xuml                                                  // 
//	$Date$                     //   
//	$Revision$                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file  mmc_analogtv.h                                                                   //
/// That file describes the analog TV interface.             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _MMC_ANALOGTV_H_
#define _MMC_ANALOGTV_H_

#include "cs_types.h"

#include "mci.h"
#include "atvd_m.h"

/// @file mmc_analogtv.h
/// @mainpage analog TV API
/// @page apbs_mainpage  analog TV API

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================
// =============================================================================
// MMC_ANALOG_MSGID
// -----------------------------------------------------------------------------
/// 
/// 
// =============================================================================

typedef enum
{
	//IFC interrupt
	MMC_ANALOG_TV_REALTIME_PROCESS=0x10,

} MMC_ANALOG_TV_MSGID;


// =============================================================================
// MMC_ANALOG_MSGID
// -----------------------------------------------------------------------------
/// 
/// 
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================
PUBLIC UINT32 mmc_AnalogTvAudioSetup(UINT8 volume);
PUBLIC UINT32 mmc_AnalogTvAudioOpen(VOID);
PUBLIC UINT32 mmc_AnalogTvAudioClose(VOID);

#endif
