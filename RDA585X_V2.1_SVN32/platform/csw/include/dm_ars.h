#if !defined(__DM_ARS_H__)
#define __DM_ARS_H__

#include "ars_m.h"


BOOL DM_ArsSetup(CONST ARS_AUDIO_CFG_T* cfg);
BOOL DM_ArsRecord(CONST ARS_ENC_STREAM_T* stream, CONST ARS_AUDIO_CFG_T* cfg, BOOL loop);
BOOL DM_ArsPause(BOOL pause);
BOOL DM_ArsStop(VOID);

UINT32* DM_ArsGetBufPosition(VOID);
BOOL DM_ArsReadData(UINT32 addedDataBytes);
UINT32 DM_ArsAvailableData(VOID);

#endif // __DM_ARS_H__

