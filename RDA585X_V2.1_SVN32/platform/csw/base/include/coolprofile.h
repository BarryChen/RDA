/***********************************************
	FILENAME	:Coolprofile.h
	PURPOSE		:Coolprofile tool stub define
	REMARKS		:For TBM780
	DATE		:
************************************************/





#define u16 UINT16
#define u8  UINT8
#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000 
#define TBM_ENTRY(id) hal_PXTS_SendProfilingCode((id) & CPMASK)
#define TBM_EXIT(id) hal_PXTS_SendProfilingCode(((id) & CPMASK) | CPEXITFLAG)

extern void hal_PXTS_SendProfilingCode(u16 code);
