#ifndef _RCM_H_
#define _RCM_H_

BOOL SRVAPI RCM_FlashGetSpace(UINT8 nModule, UINT32*pStart, UINT32* pSize);
BOOL SRVAPI RCM_GetRamSpace(UINT32*pRamAddr, UINT32* pRamSize);
BOOL SRVAPI RCM_ReleaseRamSpace(VOID);
BOOL SRVAPI RCM_IsValidHeap(PVOID p); // Check the addres is valid heap.

//
//add by lixp
//

typedef struct _RCM_PBK_CAP {
    UINT32	m_PbkMinNum;      // size in byte
	UINT32	m_PbkMcMaxNumber; // size in byte
	UINT32	m_PbkRcMaxNum;    // size in byte
	UINT32	m_PbkLdNum;       // size in byte.
}RCM_PBK_CAP;

typedef struct _RCM_SMS_CAP {
    UINT32	m_SmsMaxNum;    // size in byte.
	UINT32	m_SmsCbMaxNum;  // size in byte.
	UINT32	m_SmsSrMaxNum;  // size in byte.
}RCM_SMS_CAP;

UINT32 SRVAPI RCM_GetPbkCap(RCM_PBK_CAP* pPbkCap ) ;
UINT32 SRVAPI RCM_GetSmsCap(RCM_SMS_CAP* pSmsCap ) ;


#endif // _H_

