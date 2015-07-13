////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: record_api.h
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#include "cs_types.h"



uint32 LILY_RecordStart (int8 *file_name_p) ;	//	MCI_MEDIA_PLAY_REQ,			

uint32 LILY_RecordPause(void);

uint32 LILY_RecordResume(void);

uint32 LILY_RecordStop(void);

