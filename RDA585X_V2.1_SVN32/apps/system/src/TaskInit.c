/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of COOLSAND Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("COOLSAND SOFTWARE")
*  RECEIVED FROM COOLSAND AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. COOLSAND EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES COOLSAND PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE COOLSAND SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. COOLSAND SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY COOLSAND SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND COOLSAND'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE COOLSAND SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT COOLSAND'S OPTION, TO REVISE OR REPLACE THE COOLSAND SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  COOLSAND FOR SUCH COOLSAND SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 * TaskInit.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   Initilize the task structure for the entire application. Functions to allocate task resources,
 *   create task communication interfaces and synchronization mechanism for common resource sharing.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include "project_config.h"
#include "platform.h"
#include "event.h"

#ifndef BT_SUPPORT
#if APP_SUPPORT_BLUETOOTH==1
#error platform not support bluetooth
#endif
#endif

// Task的stack大小定义.
#define SYS_TASK_STACK_SIZE         512 	// 2k
#if APP_SUPPORT_RGBLCD==1
#define APP_TASK_STACK_SIZE		   1536 	// 6k
#else
#define APP_TASK_STACK_SIZE		    768 	// 2k
#endif
#define MED_TASK_STACK_SIZE	        512		// 2k
#define BT_TASK_STACK_SIZE  	   	512 	// 2k


// Task的优先级定义.
#define MED_TASK_PRIORITY	        	230 //30
#define SYS_TASK_PRIORITY		        230 //10
#define BT_TASK_PRIORITY				230 //80
#define APP_TASK_PRIORITY		        230 //200


extern TASK_ENTRY BAL_SysTask (void *pData);
extern TASK_ENTRY BAL_FSTaskEntry (void *pData);
extern TASK_ENTRY BAL_MMCTask (void *pData);
extern TASK_ENTRY BAL_BTTaskEntry (void *pData);
extern TASK_ENTRY BAL_MainTaskEntry (void *pData);


extern BOOL  hal_HstSendEvent(UINT32 level, UINT32 event);
extern BOOL BAL_Initalise(void );
BOOL pm_BatteryInit(VOID);
//extern u8 sxr_NewSemaphore (u8 InitValue);

BOOL BAL_TaskInit ( VOID )
{
	HANDLE hTask;
#ifndef MMI_ON_WIN32
	//	sxr_NewSemaphore(1);
	BAL_Initalise();
	
	hal_HstSendEvent(SYS_EVENT, 0x11113);
	hTask = COS_CreateTask(BAL_SysTask, MOD_SYS,
	                       NULL, NULL,
	                       SYS_TASK_STACK_SIZE,
	                       SYS_TASK_PRIORITY,
	                       COS_CREATE_DEFAULT, 0, "System Task");
	hal_HstSendEvent(SYS_EVENT, 0x11115);
	
	hTask = COS_CreateTask(BAL_MMCTask, MOD_MED,
	                       NULL, NULL,
	                       MED_TASK_STACK_SIZE,
	                       MED_TASK_PRIORITY,
	                       COS_CREATE_DEFAULT, 0, "Media Task");
	hal_HstSendEvent(SYS_EVENT, 0x11117);
	
#if APP_SUPPORT_BLUETOOTH
	hTask = COS_CreateTask(BAL_BTTaskEntry, MOD_BT,
	                       NULL, NULL,
	                       BT_TASK_STACK_SIZE,
	                       BT_TASK_PRIORITY,
	                       COS_CREATE_DEFAULT, 0, "Bluetooth Task");
#endif
	                       
#endif // MMI_ON_WIN32
	hal_HstSendEvent(SYS_EVENT, 0x11119);
	hTask = COS_CreateTask(BAL_MainTaskEntry, MOD_APP,
	                       NULL, NULL,
	                       APP_TASK_STACK_SIZE,
	                       APP_TASK_PRIORITY,
	                       COS_CREATE_DEFAULT, 0, "App Main Task");
	                       
	return TRUE;
}





