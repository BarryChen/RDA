////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2008, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
//  $HeadURL$
//  $Author$
//  $Date$
//  $Revision$
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file mcip_callback.h
///
/// MCI callback
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MCIP_CALLBACK_H_
#define _MCIP_CALLBACK_H_

#include "cs_types.h"

// =============================================================================
// MACROS                                                                       
// =============================================================================


// =============================================================================
// TYPES                                                                        
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================

VOID vid_send_play_finish_ind(UINT16 result);

VOID vid_send_file_ready_ind(UINT16 result);

VOID vid_send_seek_done_ind(UINT16 result);

VOID aud_send_media_play_finish_ind(UINT8 result);

VOID vid_file_info(UINT16 width, UINT16 height,
                   UINT32 nbFrames, UINT32 totalTime);

#endif // _MCIP_CALLBACK_H_
