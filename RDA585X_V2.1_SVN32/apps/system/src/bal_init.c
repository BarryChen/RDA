/******************************************************************************/
/*              Copyright (C) 2005, Coolsand Technologies, Inc.                */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    bal_init.c                                                    */
/*                                                                            */
/* Description:                                                               */
/*                   */
/******************************************************************************/

#include "platform.h"
#include "ap_common.h"
#include "ts.h"

#ifndef ERR_SUCCESS
#define ERR_SUCCESS                                             0
#endif

extern VOID DM_CheckPowerOnCause();
extern INT32 VDS_Init();
extern INT32 DSM_DevInit();


BOOL BAL_Initalise()
{
	UINT8 i = 0;
	INT32 err_code = 0x00;
	
	DM_CheckPowerOnCause();
	
	gpio_initialise();
	
#if defined(VDS_SUPPORT) // tianwq
	err_code = VDS_Init();   // Initialize VDS. added bye nie. 20070322
	
	if(ERR_SUCCESS == err_code)
	{
		CSW_TRACE(BASE_BAL_TS_ID, "VDS_Init() OK.\n");
	}
	else
	{
		CSW_TRACE(BASE_BAL_TS_ID, "VDS_Init() ERROR, Error code: %d.\n", err_code);
	}
#endif
	
	err_code = DSM_DevInit();
	if(ERR_SUCCESS == err_code)
	{
		CSW_TRACE(BASE_BAL_TS_ID, "DSM_DevInit OK.\n");
	}
	else
	{
		CSW_TRACE(BASE_BAL_TS_ID, "DSM_DevInit ERROR, Error code: %d. \n", err_code);
	}
	
#ifdef MCD_TFCARD_SUPPORT
	err_code = FS_PowerOn();
	if(ERR_SUCCESS == err_code)
	{
		CSW_TRACE(BASE_BAL_TS_ID, "FS Power On Check OK.\n");
	}
	else
	{
		CSW_TRACE(BASE_BAL_TS_ID, "FS Power On Check ERROR, Error code: %d. \n", err_code);
	}
#endif
	
	//DM_Audio_Side_test(2); // loopback mode for test audio
	return TRUE;
}


