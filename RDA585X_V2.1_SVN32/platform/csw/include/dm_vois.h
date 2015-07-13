#if !defined(__DM_VOIS_H__)
#define __DM_VOIS_H__

#include "vois_m.h"

BOOL DM_VoisRecordStart(INT32 *startAddress, INT32 length, VOIS_USER_HANDLER_T handler);
BOOL DM_VoisRecordStop(VOID);

#endif // __DM_VOIS_H__

