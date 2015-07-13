////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: fmradio_api.h
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



uint32 LILY_FM_RDA5800_init (uint8 scl,uint8 sda) ;			

uint32 LILY_FM_RDA5800_close(void);

uint32 LILY_FM_RDA5800_open(uint32 bUseLastFreq);

uint32 LILY_FM_RDA5800_tune(uint32 iFrequency,uint32 isStereo);

uint32 LILY_FM_RDA5800_mute(uint32 isMute);

uint32 LILY_FM_RDA5800_setVolume(uint32 iVol);

uint32 LILY_FM_RDA5800_seek(uint32 isSeekUp);

uint32 LILY_FM_RDA5800_I2S(void);

uint32 LILY_FM_RDA5800_STOPI2S(void);




