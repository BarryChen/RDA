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
//	$Author$                                                         // 
//	$Date$                     //   
//	$Revision$                                                        //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// 
/// @defgroup cswSoundRec CSW Sound Recorder
/// @{
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _CSW_SOUND_RECORDER_H_
#define _CSW_SOUND_RECORDER_H_

#if (CSW_EXTENDED_API == 1)

#include "cs_types.h"
#include "mci_sound_recorder.h"

// ============================================================================
// Defines 
// ============================================================================

// ============================================================================
// INT32 Mmc_sndRecResume(VOID)
// ============================================================================
#define    Mmc_sndRecResume mci_SndRecResume 

// ============================================================================
// INT32 Mmc_sndRecPause(VOID) 
// ============================================================================
#define    Mmc_sndRecPause mci_SndRecPause  

// ============================================================================
// INT32 Mmc_sndRecStop(VOID)
// ============================================================================
#define    Mmc_sndRecStop mci_SndRecStop   


#endif // CSW_EXTENDED_API

/// @}

#endif //_CSW_SOUND_RECORDER_H_


