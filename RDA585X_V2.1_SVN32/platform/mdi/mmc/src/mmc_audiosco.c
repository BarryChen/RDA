////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audioSCO.c
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#include "cswtype.h"
#include "mcip_debug.h"
#include "fs.h"

#include "mmc_audiosco.h"

#include "mmc.h"
#include "cpu_share.h"

#include "audio_api.h"

#include "aud_m.h"
#include "fs_asyn.h"

#include "cos.h"
#include "mci.h"
#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"

#include "string.h"
#include "hal_overlay.h"

#include "mmc_adpcm.h"
#include "resample.h"
#include "ars_m.h"
#include "event.h"

extern AUD_ITF_T   audioItf;

/*
* NAME:	MMC_AudioSCODecClose()
* DESCRIPTION: Close SCO decoder.
*/
static BOOL BTScoCallStarted = FALSE;
static UINT16 BTSco_connectHandle;

BOOL BTSco_AudCallStarted()
{
	return BTScoCallStarted;
}
VOID BTSco_AudCallReset()
{
	BTScoCallStarted = FALSE;
}

BOOL BTSco_AudFileHandle()
{
	return BTSco_connectHandle;
}

BOOL BTSco_AudCallStart()
{
    if(!BTScoCallStarted)
    {
        // Initial cfg
        if(gpio_detect_earpiece())
            audioItf = AUD_ITF_BT_EP;
        else
            audioItf = AUD_ITF_BT_SP;

        BTScoCallStarted = TRUE;
        DM_StartAudio();  
    }
    
    return TRUE;
}

BOOL BTSco_AudCallStop()
{
    if(BTScoCallStarted)
    {
        DM_StopAudio();  
    }
    
    return TRUE;
}

int32 Aduio_SCORecord(HANDLE fhd)
{
    BTSco_RecordStart(fhd);
	return  MCI_ERR_NO;
}

int32 Audio_SCOPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{

    hal_HstSendEvent(SYS_EVENT, 0x19820100);
    hal_HstSendEvent(SYS_EVENT, fhd);

    BTSco_connectHandle = fhd;
    BTSco_AudCallStart();
    return  MCI_ERR_NO;
}

int32 Audio_SCOStop (void) 	
{
    //dbg_TraceOutputText(0,"Audio_SCOStop BTScoCallStarted?%d",BTScoCallStarted);

    BTSco_AudCallStop();
    return  MCI_ERR_NO;
}


int32 Audio_SCOPause (void)	
{

	return  MCI_ERR_NO;
}


int32 Audio_SCOResume ( HANDLE fhd) 
{

	return  MCI_ERR_NO;
}


int32 Audio_SCOGetID3 (char * pFileName)  
{
	return 0;	
}

int32 Audio_SCOGetPlayInformation (MCI_PlayInf * MCI_PlayInfSCO)  
{
	MCI_PlayInfSCO->PlayProgress=0;
	
	//dbg_TraceOutputText(0,"[MCI_SCO]PlayProgress:%d",MCI_PlayInfSCO->PlayProgress);
	
	return MCI_ERR_NO;
}

int32 Audio_SCOUserMsg(int32 nMsg)
{
	switch (nMsg)
	{
		default:
			
			//dbg_TraceOutputText(0,"[MMC_SCO_ERR]ERROR MESSAGE!");
			
			break;
	}
    return 0;
}

