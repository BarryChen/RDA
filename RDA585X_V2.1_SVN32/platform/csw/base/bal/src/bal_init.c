/******************************************************************************/
/*              Copyright (C) 2005, Coolsand Technologies, Inc.                */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    bal_init.c                                                    */
/*                                                                            */
/* Description:                                                               */
/*                   */
/******************************************************************************/
#if 0
#include "csw.h"
#include "stdkey.h"
#include "event.h"
#include "base_prv.h"
#include "drv_prv.h"
#include "csw_ver.h"
#include "csw_map.h"
#include "vds_api.h"
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
#include "vds_cache.h"
#endif
#include "drv_flash.h"
#include "fs.h"
#include "chip_id.h"
#include "pmd_m.h"
#include "sxs_io.h"
#include "sxr_tksd.h"
#include "hal_host.h"


extern BOOL DRV_FlashInit(void);
extern VOID BAL_CFWApsTask (  VOID* pData);
extern VOID DM_CheckPowerOnCause();
extern VOID csw_MemIint() ;
extern BOOL PM_CheckValidPowerOnVoltage(UINT16 nVoltage);
extern HANDLE COS_CreateTask_Prv(PTASK_ENTRY pTaskEntry, PVOID pParameter, PVOID pStackAddr, UINT16 nStackSize,
                                 UINT8 nPriority, UINT16 nCreationFlags, UINT16 nTimeSlice, PCSTR pTaskName);
extern VOID BAL_SetMMIDefaultValue(VOID);
extern INT32 VDS_Init();
extern INT32 REG_Init();
extern INT32 CFW_CfgInit();
extern INT32 SMS_DM_Init();
extern INT32 DSM_DevInit();
extern VOID fsTace_task_create();
extern UINT32 TM_PowerOnPatch();
#ifdef WIFI_SUPPORT
extern VOID wifi_task_init(VOID);
#endif


#define MAX_BAL_TASK_NUM      4

HANDLE g_BalTasks[MAX_BAL_TASK_NUM];

MMI_Default_Value g_MMI_Default_Value;

VOLATILE BOOL g_BalFlashInitProgramDone = FALSE;


VOID CSW_CheckMMIDefaultValue(VOID)
{
    if((MINLEAST_VOLTAGE > g_MMI_Default_Value.nMinVol)||(MAXLEAST_VOLTAGE < g_MMI_Default_Value.nMinVol))
        g_MMI_Default_Value.nMinVol = DEFAULTLEAST_VOLTAGE ;
    if((MIN_MMI_MEMORY_SIZE > g_MMI_Default_Value.nMemorySize)||(MAX_MMI_MEMORY_SIZE < g_MMI_Default_Value.nMemorySize))
        g_MMI_Default_Value.nMemorySize = DEFAULT_MMI_MEMORY_SIZE ;
}

HANDLE BAL_TH(UINT8 pri) 
{
    if(pri == COS_BK_TASK_PRI)
    {
        return g_BalTasks[3];
    }
    else 
    {
        return g_BalTasks[BAL_TASK_NUM(pri)];
    }
}

BOOL BAL_TaskInit()
{
    UINT8 i = 0;
    INT32 err_code = 0x00;
    UINT32 fs_dev_count = 0;
    FS_DEV_INFO* fs_dev_info = NULL;
    UINT32 fs_root_dev_count = 0;
    
    csw_RegisterYourself();

  // hal_fastOSTimInit();

  BAL_DevHandlerInit();
  TS_Init_Inner();

  // TS_SetOutputMask(CSW_TS_ID, 1);           
  // TS_SetOutputMask(CFW_SIM_TS_ID, 1);    
  // TS_SetOutputMask(CFW_NW_TS_ID, 1);    
  // TS_SetOutputMask(CFW_SMS_TS_ID, 1);  
  // TS_SetOutputMask(CFW_CC_TS_ID, 1);   
  // TS_SetOutputMask(CFW_SS_TS_ID, 1);    
  // TS_SetOutputMask(CFW_PM_TS_ID, 1);    
  // TS_SetOutputMask(BASE_FFS_TS_ID, 1);    
  // TS_SetOutputMask(BASE_TM_TS_ID, 1);    
  // TS_SetOutputMask(CFW_SHELL_TS_ID, 1);    
  // TS_SetOutputMask(CFW_AOM_TS_ID, 1);   

// open 1,2,3,4
  // sxs_IoCtx.TraceBitMap [TGET_ID(_CSW)] =  1|(1 << 1) | (1 << 2)|(1<<3);

  // open shell trace
  sxs_IoCtx.TraceBitMap[TGET_ID(_CSW)] = (1 << 1);

  // 
  // sxs_IoCtx.TraceBitMap [TGET_ID(_MMI)] = 0;
  // sxs_IoCtx.TraceBitMap [TGET_ID(_MM)] = 0;

  BAL_SetMMIDefaultValue();

// CSW_CheckMMIDefaultValue() ;
  CSW_CheckMMIDefaultValue(); // add wys 2007-06-20

  csw_MemIint();
  PM_CheckValidPowerOnVoltage(3000);

/*
    if(0x00 ==  CFW_GetValidMarker())//0x00 ???????
    {
        	CSW_TRACE(100, TSTXT("norm modle"));

        if(!PM_CheckValidPowerOnVoltage(g_MMI_Default_Value.nMinVol))
        {
            hal_ShutDown();
        }
    }
    else
    {
                	CSW_TRACE(100, TSTXT("e modle"));

    }

  //deleted by mabo to power on at low power voltage,20070829
  //add wys 2007-06-20
    if(0x00 ==  CFW_GetValidMarker())//0x00 ???????
    {
        	CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_TaskInit norm modle"));

        if(!PM_CheckValidPowerOnVoltage(g_MMI_Default_Value.nMinVol))
        {
            hal_ShutDown();
        }
    }
    else
    {
           CSW_TRACE(BASE_BAL_TS_ID, TSTXT("BAL_TaskInit e modle"));

    }
//add end
*/
	
    CSW_TRACE(BASE_BAL_TS_ID, TSTXT("\nBAL_TaskInit Start. \n"));

    DRV_FlashPowerUp();
    //mabo deleted,20070813
    //DRV_FlashInit(CSW_DVR_FLASH_INIT_PARA) ;
    //romuald added 20080425
    DRV_FlashInit();

    // ML should be initialized before VDS_Init
    ML_Init();
    ML_SetCodePage(ML_ISO8859_1);

    // No flash read access is allowed when flash is being programmed
    sxr_EnterScSchedule();
    UINT32 status = hal_SysEnterCriticalSection();

    err_code = VDS_Init();   // Initialize VDS. added bye nie. 20070322
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID,"VDS_Init() OK.\n");        
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID,"VDS_Init() ERROR, Error code: %d.\n", err_code);
    }

    hal_SysExitCriticalSection(status);
    sxr_ExitScSchedule();
   
    err_code = DSM_DevInit();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID,"DSM_DevInit OK.\n");
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID,"DSM_DevInit ERROR, Error code: %d. \n",err_code);
    }

    err_code = REG_Init();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("REG_Init() OK.\n"));
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("REG_Init() Fail!Error code:%d.\n"),err_code);
    }
        
    err_code = CFW_CfgInit();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("CFW_CfgInit OK.\n"));
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("CFW_CfgInit ERROR, Error code: 0x%08x \n"), err_code);
    }

    err_code = SMS_DM_Init();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("SMS_DM_Init OK.\n"));
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("SMS_DM_Init ERROR, Error code: %d.\n"), err_code);
    }
    
    err_code = FS_PowerOn();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID,"FS Power On Check OK.\n");
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID,"FS Power On Check ERROR, Error code: %d. \n",err_code);
    }

    // Get FS device table.
    err_code = FS_GetDeviceInfo(&fs_dev_count, &fs_dev_info);
    if(err_code != ERR_SUCCESS)
    {
        CSW_TRACE(BASE_BAL_TS_ID,"Device not register.\n");
        fs_dev_count = 0;
    }

    // Check the state of root device ,if not format, format it.
    // Mount root device.
    for(i = 0; i < fs_dev_count; i++)
    {
        // format the flash device.
        if(fs_dev_info[i].dev_type == FS_DEV_TYPE_FLASH)
        {
            err_code = FS_HasFormatted(fs_dev_info[i].dev_name,FS_TYPE_FAT);

            if(ERR_FS_HAS_FORMATED == err_code)
            {
                CSW_TRACE(BASE_BAL_TS_ID,"The flash device %s has formated.\n",fs_dev_info[i].dev_name);
            }
            else if(ERR_FS_NOT_FORMAT == err_code)
            {        

                CSW_TRACE(BASE_BAL_TS_ID,"The flash device %s not format.\n",fs_dev_info[i].dev_name);
                err_code = FS_Format(fs_dev_info[i].dev_name,FS_TYPE_FAT,0);
                if(ERR_SUCCESS == err_code)
                {
                    CSW_TRACE(BASE_BAL_TS_ID,"The flash device %s format ok.\n",fs_dev_info[i].dev_name);
                }
                else if(ERR_FS_NOT_FORMAT == err_code)
                {
                    CSW_TRACE(BASE_BAL_TS_ID,"The flash device %s format error.Error code:%d.\n",fs_dev_info[i].dev_name,err_code);            
                }    
            }
            else 
            {
                CSW_TRACE(BASE_BAL_TS_ID,"The flash device %s has formated error.Error code:%d.\n",fs_dev_info[i].dev_name,err_code);
                
            }
             
            // Mount root device.
            if(TRUE == fs_dev_info[i].is_root)
            {
                if(fs_root_dev_count > 0)
                {
                  CSW_TRACE(BASE_BAL_TS_ID,"The FS root device too more:%d.\n",fs_root_dev_count);
                }
                else 
                {
                    err_code = FS_MountRoot(NULL);
                    if(ERR_SUCCESS == err_code)
                    {
                        CSW_TRACE(BASE_BAL_TS_ID,"FS MountRoot(%s) OK.\n",fs_dev_info[i].dev_name);    
                        fs_root_dev_count ++;
                    }
                    else
                    {
                        CSW_TRACE(BASE_BAL_TS_ID,"FS MountRoot(%s) ERROR, Error code: %d. \n",fs_dev_info[i].dev_name,err_code);
                        hal_HstSendEvent(SYS_EVENT,0xaaaa000e);
                        hal_DbgAssert("mount root failed.err_code = %d",err_code);
                    }
                }
            }
        }
    }

    
    g_BalFlashInitProgramDone = TRUE;
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER) 
    err_code = VDS_CacheInit();   // Initialize VDS Cache.
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID,"VDS_CacheInit() OK.\n");        
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID,"VDS_CacheInit() ERROR, Error code: %d.\n", err_code);
    }    
#endif
//#ifdef SIM_SWITCH_ENABLE
#if 0
    //
    //Add by lixp at 070407
    //SimSwitch(UINT8 SimNum)
    //#define SIM_1 1 //simcard 1
    //#define SIM_2 2 //simcard 2
    //
    UINT8 nSIMCARD_INDEX = 0x00;
    err_code = CFW_CfgGetSimSwitch(&nSIMCARD_INDEX);
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("CFW_CfgGetSimSwitch OK.\n"));
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID, TSTXT("CFW_CfgGetSimSwitch ERROR, Error code: 0x%08x \n"), err_code);
    }
    
    SimSwitch(nSIMCARD_INDEX);
    CSW_TRACE(BASE_BAL_TS_ID, TSTXT("nSIMCARD_INDEX is %d\n"),nSIMCARD_INDEX);
#endif

#ifdef FSTRACE_SUPPORT
    fsTace_task_create();
#endif

    TM_PowerOnPatch();

    // Start application tasks

    // TODO: Should task scheduling be disabled? If yes, how about the interrupts?
    // Note that sxr_Sleep should be banned when task scheduling is disabled.
    //sxr_EnterScSchedule();

    for(i = 0; i< MAX_BAL_TASK_NUM; i++) 
        g_BalTasks[i] = HNULL;
    
    g_BalTasks[BAL_TASK_NUM(BAL_SYS_TASK_PRIORITY)] = COS_CreateTask_Prv(BAL_SysTask, NULL, NULL, 
        BAL_SYS_TASK_STACK_SIZE, BAL_SYS_TASK_PRIORITY, COS_CREATE_DEFAULT, 0, "BAL_SysTask");

#ifdef CFW_TCPIP_SUPPORT
    g_BalTasks[BAL_TASK_NUM(BAL_CFW_ADV_TASK_PRIORITY)] = COS_CreateTask_Prv(BAL_CFWApsTask, NULL, NULL, 
        BAL_CFW_ADV_TASK_STACK_SIZE, BAL_CFW_ADV_TASK_PRIORITY, COS_CREATE_DEFAULT, 0, "BAL_CFWApsTask");
#endif

    g_BalTasks[BAL_TASK_NUM(BAL_DEV_MONITOR_TASK_PRIORITY)] = COS_CreateTask_Prv(BAL_DevMonitorTask, NULL, NULL, 
        BAL_DEV_MONITOR_TASK_STACK_SIZE, BAL_DEV_MONITOR_TASK_PRIORITY, COS_CREATE_DEFAULT, 0, "BAL_DevMonitorTask");

    g_BalTasks[3] = COS_CreateTask_Prv(BAL_BackgroundTask, NULL, NULL, 
        BAL_BG_TASK_STACK_SIZE, COS_BK_TASK_PRI, COS_CREATE_DEFAULT, 0, "BAL_BackgroundTask");

    BAL_ApplicationInit();

#ifdef WIFI_SUPPORT
    wifi_task_init();
#endif

    //sxr_ExitScSchedule();

    return TRUE;
}


extern void cdfu_init(void);
extern void cdfu_overlay_2_1(void);
extern void cdfu_overlay_3_1(void);

VOID BAL_DecompressCode(VOID)
{
#ifndef MMI_ON_WIN32

    while (!g_BalFlashInitProgramDone)
    {
        COS_Sleep(5);
    }

#if defined(COMPRESS_OPTIMIZE_FLASH_SIZE) || defined(COMPRESS_3264) || \
    defined(COMPRESS_6464) || defined(COMPRESS_3232) || defined(COMPRESS_SLIMPDA6464)
    cdfu_init();
#endif

#if defined(COMPRESS_3232) ||defined(COMPRESS_SLIMPDA6464)
    cdfu_overlay_2_1();
    cdfu_overlay_3_1();
#endif

#endif // !MMI_ON_WIN32
}


BOOL IsCFWEventAvail(VOID)
{
    if( sxr_SbxHot( 11) )
         return TRUE;
    else
        return FALSE;
}
#endif

