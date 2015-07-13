

#ifndef _DM_M_H_
#define _DM_M_H

#include "cs_types.h"
#include "cos.h"

PUBLIC BOOL dm_Init(VOID);
PUBLIC UINT8 dm_GetStdKey(UINT8 key);
PUBLIC UINT8 DM_BuzzerSongMsg(COS_EVENT* pEvent);

#endif // _DM_M_H_



