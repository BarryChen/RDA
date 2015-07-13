#ifndef SA_PROFILE_H
#define SA_PROFILE_H

#include "hal_debug.h"

#ifdef SA_PROFILING

#define SA_PROFILE_PAL_FRAMEEND_START	{hal_PXTS_SendCode(0x8010); hal_PXTS_SendCode(0x0001);} 
#define SA_PROFILE_PAL_FRAMEEND_END		{hal_PXTS_SendCode(0x8010); hal_PXTS_SendCode(0x4001);} 

#define SA_PROFILE_USF_RDY_IRQ			{hal_PXTS_SendCode(0x80C4);}
#define SA_PROFILE_EQU_COMPLETE_IRQ		{hal_PXTS_SendCode(0x80C5);}
#define SA_PROFILE_SCH_DEC_IRQ			{hal_PXTS_SendCode(0x80C6);}
#define SA_PROFILE_NB_DEC_IRQ			{hal_PXTS_SendCode(0x80C7);}

#define SA_PROFILE_IRQ_START			{hal_PXTS_SendCode(0x80C0);}
#define SA_PROFILE_IRQ_END				{hal_PXTS_SendCode(0x80C1);}

#define SA_PROFILE_TREAT_IRQ_START(i)	{hal_PXTS_SendCode(0x80C2); hal_PXTS_SendCode(i);}
#define SA_PROFILE_TREAT_IRQ_END(i)		{hal_PXTS_SendCode(0x80C3); hal_PXTS_SendCode(i);}

#else

#define SA_PROFILE_PAL_FRAMEEND_START	
#define SA_PROFILE_PAL_FRAMEEND_END		
#define SA_PROFILE_USF_RDY_IRQ			
#define SA_PROFILE_EQU_COMPLETE_IRQ		
#define SA_PROFILE_SCH_DEC_IRQ			
#define SA_PROFILE_NB_DEC_IRQ			
#define SA_PROFILE_IRQ_START			
#define SA_PROFILE_IRQ_END				
#define SA_PROFILE_TREAT_IRQ_START(i)	
#define SA_PROFILE_TREAT_IRQ_END(i)		

#endif


#endif // SA_PROFILE_H
