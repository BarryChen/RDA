#ifdef _FS_SIMULATOR_

#else
#include <base_prv.h>
#include <drv_flash.h>
#include <drv_usb.h>

#ifndef _T_UPGRADE_PROGRAMMER
//tianwq #include "umss_m.h"
#endif //_T_UPGRADE_PROGRAMMER
#endif //_MS_VC_VER_


#include "dsm_cf.h"
#include "dsm_dbg.h"
//#include "vds_local.h"
#include "vds_api.h"
#include "hal_mem_map.h"
#include "hal_host.h"

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
#include "vds_cache.h"
#endif
#include "event.h"

//extern VDS_PARTITION g_VDS_partition_table[];
extern UINT32 g_vds_partition_count;

// Device description table.this table include name,module id,handle,state,device type and point of option function.
DSM_DEV_DESCRIP* g_pDevDescrip = NULL;

// Device description table recodes number.The arry 0 is reserved,so the value equal to (recode number + 1) 
UINT32 g_iDevDescripCount = 0;

// Device simple information table.
DSM_DEV_INFO* g_pDsmDevInfo = NULL;

// Device simple information table record number.
UINT32 g_iDsmDevCount = 0;

// The semaphore of  DSM device  access.
HANDLE g_hDevSem = (HANDLE)NULL;
EXPORT HAL_HOST_CONFIG_t g_halHostConfig;


VOID DSM_SemInit(VOID);
VOID DSM_SemDown(VOID);
VOID DSM_SemUp(VOID);

/**************************************************************************************/
// Function: This function init the device table.
//   Firstly, get the partition config, get the flash partition count and t-flash partition count throught scan the partition config table;
//   Secondly, add the flash partiton into the device talbe, add get the device handle for every flash partition;
//   Thirdly,  add the t-flash partiton into the device talbe, add get the device handle for every t-flash partition;
// Parameter: 
// None
// Return value: 
// TODO
// Node: the device table uint 0 is reserved. when access the device with dev_no, the zero value is invalid device No.
/***************************************************************************************/
INT32 DSM_DevInit( VOID )
{
    UINT32 iPartCount = 0;
    UINT32 iDeviceCount = 0;
    DSM_PARTITION_CONFIG* pPartConfigInfo = NULL;
    CONST DSM_CONFIG_T* pDsmConfig = NULL;
    UINT32 i,n;
    INT32 dev_handle;
    INT32 iRet; 


    // Init the semaphore  
    DSM_SemInit();
    
     // Get the partition config information.
    pDsmConfig = tgt_GetDsmCfg();    
  
    pPartConfigInfo = (DSM_PARTITION_CONFIG*)pDsmConfig->dsmPartitionInfo;
    iPartCount = pDsmConfig->dsmPartitionNumber;
    
   

    // Malloc for device table.
    iDeviceCount = iPartCount + 1;
     g_pDevDescrip = DSM_MAlloc(SIZEOF(DSM_DEV_DESCRIP)*iDeviceCount);
    if(NULL == g_pDevDescrip)
    {
        CSW_TRACE(BASE_FFS_TS_ID, "In DSM_DevInit,malloc failed.\n");
        DSM_ASSERT(0,"DSM_DevInit:1. malloc(0x%x) failed.",(SIZEOF(DSM_DEV_DESCRIP)*iDeviceCount));
        return ERR_DRV_NO_MORE_MEM;
    }
    DSM_MemSet(g_pDevDescrip,0,SIZEOF(DSM_DEV_DESCRIP)*iDeviceCount);
    
    // step1:Initialize the device descrip table.    
    // The array 0 is reseved,when dev_no is zero, we consider it is invalid device no,so set the n = 1, 
    n = 1;    
    for ( i = 0; i < iPartCount; i++ )
    {            
        DSM_StrCpy(g_pDevDescrip[n].dev_name,pPartConfigInfo[i].szPartName);
        g_pDevDescrip[n].dev_handle = -1;
        g_pDevDescrip[n].module_id = pPartConfigInfo[i].eModuleId;
        g_pDevDescrip[n].dev_state = DEV_STATE_CLOSE;
        g_pDevDescrip[n].dev_type = pPartConfigInfo[i].eDevType;

        // flash partition. When open successed,this partition will been add to the device descrip table.
        if(pPartConfigInfo[i].eDevType == DSM_MEM_DEV_FLASH)
        {
#if defined(VDS_SUPPORT) // tianwq
            g_pDevDescrip[n].open = (PF_DEV_OPEN)VDS_Open;
            g_pDevDescrip[n].close = (PF_DEV_ClOSE)VDS_Close;
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
            g_pDevDescrip[n].bread = (PF_DEV_BLOCKREAD)VDS_CacheReadBlock;
            g_pDevDescrip[n].bwrite = (PF_DEV_BLOCKWRITE)VDS_CacheWriteBlock; 
            g_pDevDescrip[n].brevert = (PF_DEV_BLOCKREVERT)VDS_CacheRevertBlock;
#else
            g_pDevDescrip[n].bread = (PF_DEV_BLOCKREAD)VDS_ReadBlock;
            g_pDevDescrip[n].bwrite = (PF_DEV_BLOCKWRITE)VDS_WriteBlock; 
            g_pDevDescrip[n].brevert = (PF_DEV_BLOCKREVERT)VDS_RevertBlock;
#endif
            g_pDevDescrip[n].get_dev_info = (PF_DEV_GETDEVINFO)VDS_GetPartitionInfo; 
            g_pDevDescrip[n].set_rcache_size = (PF_DEV_SET_CACHE_SIZE)VDS_SetRCacheSize; 
            g_pDevDescrip[n].get_rcache_size = (PF_DEV_GET_CACHE_SIZE)VDS_GetRCacheSize; 
            g_pDevDescrip[n].set_wcache_size = (PF_DEV_SET_CACHE_SIZE)VDS_SetWCacheSize; 
            g_pDevDescrip[n].get_wcache_size = (PF_DEV_GET_CACHE_SIZE)VDS_GetWCacheSize; 
            g_pDevDescrip[n].active = (PF_DEV_ACTIVE)VDS_Active; 
            g_pDevDescrip[n].deactive = (PF_DEV_ACTIVE)VDS_Deactive; 
            g_pDevDescrip[n].flush = (PF_DEV_ACTIVE)VDS_Flush; 
            /* 
            // Open the device, get the device handle.
            iRet = (*g_pDevDescrip[n].open)( ((UINT8*)g_pDevDescrip[n].dev_name), &dev_handle );
            if(ERR_SUCCESS == iRet)
            {
                CSW_TRACE(BASE_FFS_TS_ID, "Open device[%s] successfully with handle[%d] returned.",
                g_pDevDescrip[n].dev_name, dev_handle );    
                g_pDevDescrip[n].dev_state = DEV_STATE_OPEN;
            }
            else
            {
                CSW_TRACE(BASE_FFS_TS_ID, "Open device[%s] failed with errcode[%d].",
                g_pDevDescrip[n].dev_name, iRet );     
                continue;               
            }
            g_pDevDescrip[n].dev_handle = dev_handle;    
            */
#ifndef _T_UPGRADE_PROGRAMMER
#if 0 // tianwq(CHIP_HAS_USB == 1) 
            if(DSM_StrCaselessCmp(pPartConfigInfo[i].szPartName,"VDS1") == 0)
            {
                UINT32 iBlkNr = 0;
                UINT32 iBlkSz = 0;           
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)     
                init_uFlash_Func (VDS_WriteCacheBlock, VDS_ReadCacheBlock);
#else
                init_uFlash_Func (VDS_WriteBlock, VDS_ReadBlock);
#endif // USER_DATA_CACHE_SUPPORT               
                VDS_GetPartitionInfo(i,&iBlkNr,&iBlkSz);
                init_uFlash_Param(i,iBlkNr,iBlkSz);                
            }
#endif // CHIP_HAS_USB
#endif  //_T_UPGRADE_PROGRAMMER
#endif
        }
        // t-flash partiton. Don't check the option of opening,and have no use for device handle. 
#ifdef MCD_TFCARD_SUPPORT
        else if(pPartConfigInfo[i].eDevType == DSM_MEM_DEV_TFLASH)
        {
            g_pDevDescrip[n].open = (PF_DEV_OPEN)DRV_OpenTFlash;
            g_pDevDescrip[n].close = (PF_DEV_ClOSE)DRV_CloseTFlash;
            g_pDevDescrip[n].bread = (PF_DEV_BLOCKREAD)DRV_ReadTFlash;
            g_pDevDescrip[n].bwrite = (PF_DEV_BLOCKWRITE)DRV_WriteTFlash; 
            g_pDevDescrip[n].get_dev_info = (PF_DEV_GETDEVINFO)DRV_GetDevInfoTFlash; 
//            g_pDevDescrip[n].set_rcache_size = (PF_DEV_SET_CACHE_SIZE)DRV_SetRCacheSizeTFlash; 
//            g_pDevDescrip[n].get_rcache_size = (PF_DEV_GET_CACHE_SIZE)DRV_GetRCacheSizeTFlash; 
//            g_pDevDescrip[n].set_wcache_size = (PF_DEV_SET_CACHE_SIZE)DRV_SetWCacheSizeTFlash; 
//            g_pDevDescrip[n].get_wcache_size = (PF_DEV_GET_CACHE_SIZE)DRV_GetWCacheSizeTFlash; 
            g_pDevDescrip[n].active = (PF_DEV_ACTIVE)DRV_ActiveTFlash; 
            g_pDevDescrip[n].deactive = (PF_DEV_ACTIVE)DRV_DeactiveTFlash; 
            g_pDevDescrip[n].flush = (PF_DEV_ACTIVE)DRV_FlushTFlash; 
        }  
#endif
#ifdef MCD_USB_HOST_SUPPORT
        else if(pPartConfigInfo[i].eDevType == DSM_MEM_DEV_USB)
        {
            g_pDevDescrip[n].open = (PF_DEV_OPEN)DRV_OpenUSBDisk;
            g_pDevDescrip[n].close = (PF_DEV_ClOSE)DRV_CloseUSBDisk;
            g_pDevDescrip[n].bread = (PF_DEV_BLOCKREAD)DRV_ReadUSBDisk;
            g_pDevDescrip[n].bwrite = (PF_DEV_BLOCKWRITE)DRV_WriteUSBDisk; 
            g_pDevDescrip[n].get_dev_info = (PF_DEV_GETDEVINFO)DRV_GetDevInfoUSBDisk; 
//            g_pDevDescrip[n].set_rcache_size = (PF_DEV_SET_CACHE_SIZE)DRV_SetRCacheSizeUSBDisk; 
//            g_pDevDescrip[n].get_rcache_size = (PF_DEV_GET_CACHE_SIZE)DRV_GetRCacheSizeUSBDisk; 
//            g_pDevDescrip[n].set_wcache_size = (PF_DEV_SET_CACHE_SIZE)DRV_SetWCacheSizeUSBDisk; 
//            g_pDevDescrip[n].get_wcache_size = (PF_DEV_GET_CACHE_SIZE)DRV_GetWCacheSizeUSBDisk; 
            g_pDevDescrip[n].active = (PF_DEV_ACTIVE)DRV_ActiveUSBDisk; 
            g_pDevDescrip[n].deactive = (PF_DEV_ACTIVE)DRV_DeactiveUSBDisk; 
            g_pDevDescrip[n].flush = (PF_DEV_ACTIVE)DRV_FlushUSBDisk; 
        }  
#endif        
        // Open the device, get the device handle.
        if(g_pDevDescrip[n].open == NULL) // unsupport device
        {
            CSW_TRACE(BASE_FFS_TS_ID, "Device[%s] NOT support by this target.",
            g_pDevDescrip[n].dev_name);     
            continue;
        }
        
        iRet = (*g_pDevDescrip[n].open)( ((UINT8*)g_pDevDescrip[n].dev_name), (UINT32*)&dev_handle );
        if(ERR_SUCCESS == iRet)
        {
            CSW_TRACE(BASE_FFS_TS_ID, "Open device[%s] successfully with handle[%d] returned.",
            g_pDevDescrip[n].dev_name, dev_handle );    
            g_pDevDescrip[n].dev_state = DEV_STATE_OPEN;
        }
        else
        {
            CSW_TRACE(BASE_FFS_TS_ID, "Open device[%s] failed with errcode[%d].",
            g_pDevDescrip[n].dev_name, iRet );     
            continue;               
        }
        g_pDevDescrip[n].dev_handle = dev_handle;  
        CSW_TRACE(BASE_FFS_TS_ID, "Device[%s] is added to Device dscription,DevNo = %d.\n", g_pDevDescrip[n].dev_name, n);     
        n ++;
        
    }
 
    // Todo step2 initialize the table for others partition.

    // Step3: init dsm device information table.
     g_iDevDescripCount = n;
     g_iDsmDevCount = n - 1;
   
    if(g_iDsmDevCount > 0)
    {  
        g_pDsmDevInfo = DSM_MAlloc(SIZEOF(DSM_DEV_INFO)*g_iDsmDevCount);
        if(NULL == g_pDsmDevInfo)
        {
            CSW_TRACE(BASE_FFS_TS_ID, "In DSM_DevInit,malloc failed.");
            DSM_ASSERT(0,"DSM_DevInit: 2.malloc(0x%x) failed.",(SIZEOF(DSM_DEV_INFO)*g_iDsmDevCount));
            return ERR_DRV_NO_MORE_MEM;
        }
        DSM_MemSet(g_pDsmDevInfo,0,SIZEOF(DSM_DEV_INFO)*g_iDsmDevCount);
        for(i = 1; i < g_iDevDescripCount; i++)
        {  
             g_pDsmDevInfo[i - 1].dev_no  = i;     
             DSM_StrCpy( g_pDsmDevInfo[i - 1].dev_name,g_pDevDescrip[i].dev_name);
             g_pDsmDevInfo[i - 1].dev_type = g_pDevDescrip[i].dev_type;
             g_pDsmDevInfo[i - 1].module_id = g_pDevDescrip[i].module_id;            
        }
    }
    return ERR_SUCCESS;
}



UINT32 DSM_DevName2DevNo( PCSTR pszDevName )
{
    UINT32 i;

    if ( !pszDevName )
    {
        return INVALID_DEVICE_NUMBER;
    }
	
    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
       // CSW_TRACE(BASE_FFS_TS_ID, "g_pDevDescrip[%d].dev_name = %s,input device name= %s\n", i,g_pDevDescrip[i].dev_name,pszDevName );
       if ( g_pDevDescrip[i].dev_name && 0 == DSM_StrCaselessCmp( pszDevName, g_pDevDescrip[i].dev_name ) )
       {
          break;
       }
    }

    if ( i == g_iDevDescripCount )
    {
        return INVALID_DEVICE_NUMBER;
    }

    return i;
}


INT32 DSM_DevNr2Name( UINT32 uDevNo, PSTR pszDevName )
{
    if ( INVALID_DEVICE_NUMBER == uDevNo || uDevNo >= g_iDevDescripCount || !pszDevName )
    {
        return ERR_DRV_INVALID_DEVNO;
    }

    DSM_StrCpy( pszDevName, g_pDevDescrip[uDevNo].dev_name );

    return ERR_SUCCESS;
}

INT32 DSM_GetDevSpaceSize(PSTR pszDevName,UINT32* pBlkCount,UINT32* pBlkSize)
{
    UINT32 i;
    INT32 iResult;
    UINT32 uBlkCount = 0;
    UINT32 uBlkSize = 0;

    if ( !pszDevName )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }

    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
        // CSW_TRACE(BASE_FFS_TS_ID, "g_pDevDescrip[%d].dev_name = %s,input device name= %s\n", i,g_pDevDescrip[i].dev_name,pszDevName );
        if ( g_pDevDescrip[i].dev_name && 0 == DSM_StrCaselessCmp( pszDevName, g_pDevDescrip[i].dev_name ) )
        {
            iResult = g_pDevDescrip[i].get_dev_info(g_pDevDescrip[i].dev_handle,&uBlkCount,&uBlkSize);
            if(ERR_SUCCESS != iResult)
            {
                return ERR_DRV_DEV_NOT_INIT;
            }
            else
            {
                *pBlkCount = uBlkCount;
                *pBlkSize = uBlkSize;
                break;
            }
        }
    }

    if ( i == g_iDevDescripCount )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }
    
    return ERR_SUCCESS;
}


UINT32 DSM_GetDevHandle( PCSTR pszDevName )
{
    UINT32 i;

    if ( !pszDevName )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }
   
    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
       // CSW_TRACE(BASE_FFS_TS_ID, "g_pDevDescrip[%d].dev_name = %s,input device name= %s\n", i,g_pDevDescrip[i].dev_name,pszDevName );
       if ( g_pDevDescrip[i].dev_name && 0 == DSM_StrCaselessCmp( pszDevName, g_pDevDescrip[i].dev_name ) )
       {
          break;
       }
    }

    if ( i == g_iDevDescripCount )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }
    else
    {
        return g_pDevDescrip[i].dev_handle;
    }
}



INT32 DSM_GetDevType(UINT32 uDevNo)
{
    if(uDevNo == INVALID_DEVICE_NUMBER || (UINT32)uDevNo >= g_iDevDescripCount)
    {
       return ERR_DRV_INVALID_DEVNO;
    }
    
    return  g_pDevDescrip[uDevNo].dev_type;   
      
    
}

INT32 DSM_GetFsRootDevName(PSTR pszDevName )
{
    UINT32 i;
    
     for ( i = 1; i < g_iDevDescripCount; i++ )
    {       
       if ( g_pDevDescrip[i].module_id == DSM_MODULE_FS_ROOT)
       {
          break;
       }
    }
    if ( i == g_iDevDescripCount )
    {
        return ERR_DRV_GET_DEV_FAILED;
    }

    DSM_StrCpy(pszDevName,g_pDevDescrip[i].dev_name);
    return ERR_SUCCESS;
}


INT32 DSM_GetDevNrOnModuleId(DSM_MODULE_ID eModuleId)
{
    UINT32 i;
    
     for ( i = 1; i < g_iDevDescripCount; i++ )
    {       
       if ( g_pDevDescrip[i].module_id == eModuleId)
       {
          break;
       }
    }

    if ( i == g_iDevDescripCount )
    {
        return INVALID_DEVICE_NUMBER;
    }
    return i;
    
}

INT32 DSM_GetDevCount(VOID)
{
    return g_iDsmDevCount;
}


DSM_MODULE_ID DSM_GetDevModuleId(UINT32 uDevNo)
{
    if(uDevNo == INVALID_DEVICE_NUMBER || (UINT32)uDevNo >= g_iDevDescripCount)
    {
       return ERR_DRV_INVALID_DEVNO;
    }
    return g_pDevDescrip[uDevNo].module_id;  
}

//Function
//This function to get the DSM device information.
//    INT32 DSM_GetDeviceInfo(UINT32* pDeviceCount,DSM_DEV_INFO** ppDevInfo);
//Parameter
//   pDeviceCount [out]: Output the count of device.
//   ppDevInfo[out]: Output a array for DSM_DEV_INFO structure.
//Return value
//   Upon successful completion, 0 shall be returned. Otherwise, Error Code is returned. The following error codes may be returned.
//   ERR_DRV_INVALID_PARAMETER: The point of parameter is NULL.
INT32 DSM_GetDeviceInfo(UINT32* pDeviceCount,DSM_DEV_INFO** ppDevInfo)
{
       // Check input parameters. 
       if(NULL == pDeviceCount ||
          NULL == ppDevInfo)
       {
            return ERR_DRV_INVALID_PARA;
       }
       
       // Device not initialized.
       if(NULL == g_pDsmDevInfo ||
          0 == g_iDsmDevCount)
       {
            return ERR_DRV_DEV_NOT_INIT;
       }

        // output the dsm device information
       *ppDevInfo = g_pDsmDevInfo;
       *pDeviceCount  = g_iDsmDevCount;
       return ERR_SUCCESS;
       
}


///////////////////////////////////////////////////////////////////////
//  Read the partition on byte.
// If read to the end of partition, output the real reading size.
/////////////////////////////////////////////////////////////////////
INT32 DSM_Read( UINT32 uDevNr, UINT32 ulAddrOffset, UINT8* pBuffer,
                UINT32 ulBytesToRead, UINT32* pBytesRead )
{
    INT32 iResult;
    UINT32 ulModuleSize;
    UINT32 ulBlockOffset;
    UINT32 uBlockNO;    
    UINT32 uNrBlock = 0;
    UINT32 uBlockSize = 0;
    
    UINT8 pBlockBuffer[DSM_DEFAULT_SECSIZE] = {0,};
    UINT32 ulPos;
    UINT32 ulLenRead = 0;
    

    // Check the input parameter.
    if ( INVALID_DEVICE_NUMBER == uDevNr ||
          uDevNr >= g_iDevDescripCount ||
          NULL == pBuffer ||
          NULL == pBytesRead)
    {
        CSW_TRACE(BASE_FFS_TS_ID, "Invalid parameter,dev_no = %d,pbuffer = 0x%x,pbytesread = 0x%x.\n",
                                     uDevNr,
                                     pBuffer,
                                     pBytesRead
                                     );
        return ERR_DRV_INVALID_PARA;
    }

    DSM_SemDown();

    // Get the block count and block size. 
    iResult = DRV_GET_DEV_INFO( uDevNr,&uNrBlock,&uBlockSize);   
    if(ERR_SUCCESS != iResult)
    {
          CSW_TRACE(BASE_FFS_TS_ID, "get dev info failed,dev_nr = %d,err code = %d.\n",
                                     uDevNr,
                                     iResult
                                     );          
        DSM_SemUp();
        return ERR_DRV_GET_DEV_FAILED;
    }

    // Judge the address offset if exceed the moudule size.
    ulModuleSize = uBlockSize* uNrBlock;
    if ( ulAddrOffset >= ulModuleSize )
    {
        CSW_TRACE(BASE_FFS_TS_ID, "block number to read overflow total block nuber. to read = 0x%x,total = 0x%x.\n",
                                     ulAddrOffset,
                                     ulModuleSize
                                     );   
        DSM_SemUp();
        return ERR_DRV_INVALID_PARA;
    }

    // Figure out the spare size.
    if ( ulAddrOffset + ulBytesToRead > ulModuleSize )
    {
        ulBytesToRead = ulModuleSize - ulAddrOffset;
    }
    
    ulPos = ulAddrOffset;
    ulBlockOffset = ulPos % uBlockSize;
    uBlockNO = ulPos / uBlockSize;

     // Read the first block.
    if ( ulBlockOffset != 0 )
    {
       
        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_readblock(first) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                     g_pDevDescrip[uDevNr].dev_handle,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        if ( ulBlockOffset + ulBytesToRead <= uBlockSize )
        {
            DSM_MemCpy( pBuffer + ulLenRead, pBlockBuffer + ulBlockOffset, ulBytesToRead );
            ulPos += ulBytesToRead;
            ulLenRead += ulBytesToRead;
        }
        else
        {
            DSM_MemCpy( pBuffer + ulLenRead, pBlockBuffer + ulBlockOffset, uBlockSize - ulBlockOffset );
            ulPos += ( uBlockSize - ulBlockOffset );
            ulLenRead += ( uBlockSize - ulBlockOffset );
        }
    }

    // Read middle block.
    while ( ulLenRead + uBlockSize <= ulBytesToRead )
    {
        uBlockNO = ulPos / uBlockSize;
        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBuffer + ulLenRead );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_readblock(next) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                     uDevNr,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        ulPos += uBlockSize;
        ulLenRead += uBlockSize;
    }

    // Read the last block.
    if ( ulLenRead < ulBytesToRead )
    {
        uBlockNO =  ulPos / uBlockSize;
        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_readblock(last) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                     uDevNr,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        DSM_MemCpy( pBuffer + ulLenRead, pBlockBuffer, ulBytesToRead - ulLenRead );
        ulLenRead += ( ulBytesToRead - ulLenRead );
    }

    *pBytesRead = ulLenRead;
    DSM_SemUp();
    return ERR_SUCCESS;

}



///////////////////////////////////////////////////////////////////////
//  Write the partition on byte.
// If write to the end of partition, output the real writing size.
/////////////////////////////////////////////////////////////////////
INT32 DSM_Write( UINT32 uDevNr, UINT32 ulAddrOffset, CONST UINT8* pBuffer,
                UINT32 ulBytesToWrite, UINT32* pBytesWritten )
{
    INT32 iResult;
    UINT32 ulModuleSize;
    UINT32 ulBlockOffset;
    UINT32 uBlockNO;    
    UINT32 uNrBlock = 0;
    UINT32 uBlockSize = 0;
    UINT8 pBlockBuffer[DSM_DEFAULT_SECSIZE] = {0,};
    UINT32 ulPos;
    UINT32 ulLenWriten = 0; 

     // Check the input parameter.
     if ( INVALID_DEVICE_NUMBER == uDevNr || 
           uDevNr >= g_iDevDescripCount ||
           NULL == pBuffer ||       
           NULL == pBytesWritten)
    {
        CSW_TRACE(BASE_FFS_TS_ID, "Invalid parameter,dev_no = %d,pbuffer = 0x%x,pbyteswritten = 0x%x.\n",
                                     uDevNr,
                                     pBuffer,
                                     pBytesWritten
                                     );
        return ERR_DRV_INVALID_PARA;
    }
     
    DSM_SemDown();

    // Get the block count and block size. 
    iResult = DRV_GET_DEV_INFO( uDevNr,&uNrBlock,&uBlockSize);   
    if(ERR_SUCCESS != iResult)
    {
        CSW_TRACE(BASE_FFS_TS_ID, "get dev info failed,dev_nr = %d,err code = %d.\n",
                                     uDevNr,
                                     iResult
                                     ); 
        DSM_SemUp();
        return ERR_DRV_GET_DEV_FAILED;
    }

    // Judge the address offset if exceed the moudule size.
    ulModuleSize = uBlockSize* uNrBlock;    
    if ( ulAddrOffset >= ulModuleSize )
    {
        CSW_TRACE(BASE_FFS_TS_ID, "block number to write overflow total block nuber. to write = 0x%x,total = 0x%x.\n",
                                     ulAddrOffset,
                                     ulModuleSize
                                     );   
        DSM_SemUp();
        return ERR_DRV_INVALID_PARA;
    }

    // Figure out the spare size.
    if ( ulAddrOffset + ulBytesToWrite > ulModuleSize )
    {
        ulBytesToWrite = ulModuleSize - ulAddrOffset;
    }
    
    ulPos = ulAddrOffset;
    ulBlockOffset = ulPos % uBlockSize;
    uBlockNO = ulPos / uBlockSize ;

    // Write the first block.
    if ( ulBlockOffset != 0 )
    {        
        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_readblock(write before) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                     uDevNr,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        if ( ulBlockOffset + ulBytesToWrite <= uBlockSize )
        {
            DSM_MemCpy( pBlockBuffer + ulBlockOffset, pBuffer + ulLenWriten, ulBytesToWrite );
            ulPos += ulBytesToWrite;
            ulLenWriten += ulBytesToWrite;
        }
        else
        {
            DSM_MemCpy( pBlockBuffer + ulBlockOffset, pBuffer + ulLenWriten, uBlockSize - ulBlockOffset );
            ulPos += ( uBlockSize - ulBlockOffset );
            ulLenWriten += ( uBlockSize - ulBlockOffset );
        }

        iResult = DRV_BLOCK_WRITE(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_writeblock(first) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                     uDevNr,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_WRITE_FAILED;
        }
    }

    // Write middle block.
    while ( ulLenWriten + uBlockSize <= ulBytesToWrite )
    {
        uBlockNO =  ulPos / uBlockSize;
	 DSM_MemCpy(pBlockBuffer,pBuffer + ulLenWriten,DSM_DEFAULT_SECSIZE);
        iResult = DRV_BLOCK_WRITE(uDevNr, uBlockNO, ( UINT8* )  pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_writeblock(next) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                     uDevNr,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_WRITE_FAILED;
        }

        ulPos += uBlockSize;
        ulLenWriten += uBlockSize;
    }

    // Write the last block.
    if ( ulLenWriten < ulBytesToWrite )
    {
        uBlockNO = ulPos / uBlockSize ;
        iResult = DRV_BLOCK_READ( uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_readblock(last befor) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                     uDevNr,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        DSM_MemCpy( pBlockBuffer, pBuffer + ulLenWriten, ulBytesToWrite - ulLenWriten );
        iResult = DRV_BLOCK_WRITE(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(BASE_FFS_TS_ID, "vds_writeblock(last) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",
                                    uDevNr,
                                     uBlockNO,
                                     iResult
                                     );
            DSM_SemUp();
            return ERR_DRV_WRITE_FAILED;
        }
        ulLenWriten += ( ulBytesToWrite - ulLenWriten );
    }

    *pBytesWritten = ulLenWriten;
    DSM_SemUp();
    return ERR_SUCCESS;

}

#define USER_DATA_FLAG          (0xa5d8f34e)
#define USER_DATA_BLOCK_SIZE    (512)  
#define USER_DATA_BLOCK_NUM     (8)    // Total 4KB in flash
#define USER_DATA_SIZE          (2048) // 2KB in RAM

struct UserData_Header_t {
    UINT32 flag;
    UINT16 length;
    UINT16 map;
} *p_Data_Header;

static UINT16 *user_data_buff = NULL;
static UINT16 user_data_length;
static UINT32 *user_data_start = NULL;
//-------------------------------------------------------------------------------------------------
//  Clear the data on flash user region.
//  Must reset the system after called this function.
//  Return value:
//     ERR_SUCCESS:                      Clear data succed.
//     ERR_DRV_GET_DEV_FAILED:   Get the device information failed.
//     ERR_DRV_ERASE_FAILED:       Erase flash sector failed. 
// 
//------------------------------------------------------------------------------------------------
INT32 DSM_UserDataClear(VOID)
{
    UINT32 *flag = user_data_buff;
    memset(user_data_buff, 0, USER_DATA_SIZE);
    p_Data_Header->flag = USER_DATA_FLAG;
    p_Data_Header->length = sizeof(struct UserData_Header_t)/2+1;
    p_Data_Header->map = 1;
    return ERR_SUCCESS;
}

void *DSM_GetUserData(UINT8 index, UINT32 length)
{
    int pos = sizeof(struct UserData_Header_t)/2;

    if(user_data_buff == NULL)
    {
        user_data_buff = COS_Malloc(USER_DATA_SIZE);
        p_Data_Header = user_data_buff;
        DSM_UserDataClear();
    }

    if(p_Data_Header->flag != USER_DATA_FLAG)
    {
        CSW_TRACE(BASE_FFS_TS_ID,  "Get UserData flag error:%x",p_Data_Header->flag);
        DSM_UserDataClear();
    }

    while(pos < USER_DATA_SIZE/2)
    {
        if(user_data_buff[pos] == index || user_data_buff[pos] == 0)
        {
            if(user_data_buff[pos] == 0)
            {
                user_data_buff[pos] = index;
                user_data_buff[pos+1] = length;
                //user_data_buff[pos+((length+3)&~3)/2 + 2] = 0;
                p_Data_Header->length += ((length+3)&~3)/2 + 2;
                user_data_buff[p_Data_Header->length-1] = 0;
            }
            return &user_data_buff[pos+2];
        }
        pos += ((user_data_buff[pos+1]+3)&~3)/2 + 2;
    }
    hal_HstSendEvent(SYS_EVENT,0xfffffffd); // user data overflow
}

INT32 DSM_ReadUserData(void)
{
    UINT32* src;
    UINT32* dst;
    UINT32 memorymap,i,j;

    if(user_data_buff == NULL)
    {
        user_data_buff = COS_Malloc(USER_DATA_SIZE);
        p_Data_Header = user_data_buff;
        DSM_UserDataClear();
    }

    for(i=0; i<USER_DATA_BLOCK_NUM; i++)
    {
        memcpy(p_Data_Header,user_data_start+i*USER_DATA_BLOCK_SIZE/4,sizeof(struct UserData_Header_t));
        if(p_Data_Header->flag==USER_DATA_FLAG)
            break;
    }
    
    memorymap=p_Data_Header->map;
    CSW_TRACE(BASE_FFS_TS_ID,  "Read Flash i=%d,memorymap=%x",i,memorymap);
    memorymap = memorymap>>i;
    for(j=0;i<USER_DATA_BLOCK_NUM;i++)
    {
        if((memorymap&1)!=1)
        {
            memorymap=memorymap>>1;
            continue;
        }
        memcpy(&user_data_buff[j*(USER_DATA_BLOCK_SIZE/2)], (UINT32*) (user_data_start+i*USER_DATA_BLOCK_SIZE/4), USER_DATA_BLOCK_SIZE);
        j++;
        memorymap=memorymap>>1;
    }
    return ERR_SUCCESS;
}

INT32 DSM_WriteUserData(void)
{
    UINT32 write,err,i,j,memorymap=0,k,head_pos;
    UINT16 *tmp_usr_buff;

#if 0    
    hal_HstSendEvent(SYS_EVENT,0x03210010);
    hal_HstSendEvent(SYS_EVENT,&_flash_start);
    hal_HstSendEvent(SYS_EVENT,user_data_start);
    hal_HstSendEvent(SYS_EVENT,USER_DATA_BLOCK_NUM);
#endif
    UINT32 status = hal_SysEnterCriticalSection();

    tmp_usr_buff = (UINT16*)COS_Malloc(USER_DATA_BLOCK_SIZE);
    
    memd_FlashErase((UINT8*)((UINT32)user_data_start-(UINT32)&_flash_start), (UINT8*)((UINT32)user_data_start-(UINT32)&_flash_start)+(USER_DATA_BLOCK_NUM*USER_DATA_BLOCK_SIZE));
    p_Data_Header->flag = 0xffffffff;
	p_Data_Header->map = 0xffff; // write memory map at last
    for(i=0,k=0;i<=USER_DATA_BLOCK_NUM;i++)
    {
        //CSW_TRACE(BASE_FFS_TS_ID,  "Write Flash i=%d,k=%d",i,k);
        if( i >= USER_DATA_BLOCK_NUM)
        {
            //memd_FlashErase((UINT8*)((UINT32)user_data_start-(UINT32)&_flash_start),(UINT8*)((UINT32)user_data_start-(UINT32)&_flash_start+8));
            p_Data_Header->flag = USER_DATA_FLAG; // restore flag
            hal_SysExitCriticalSection(status);
            COS_Free(tmp_usr_buff);
            return 1;//no enough memory
        }
        
        //memd_FlashErase((UINT8*)((UINT32)user_data_start+k*USER_DATA_BLOCK_SIZE-(UINT32)&_flash_start),(UINT8*)((UINT32)user_data_start+(k+1)*USER_DATA_BLOCK_SIZE-(UINT32)&_flash_start));
        memd_FlashWrite((UINT8*)((UINT32)user_data_start+i*USER_DATA_BLOCK_SIZE-(UINT32)&_flash_start), USER_DATA_BLOCK_SIZE, &write, &user_data_buff[k*(USER_DATA_BLOCK_SIZE/2)]);
        memcpy((UINT32*)tmp_usr_buff,(UINT32*) (((UINT32)user_data_start+i*USER_DATA_BLOCK_SIZE)|0x20000000), USER_DATA_BLOCK_SIZE);
        err = 0;
        for(j=0;j<USER_DATA_BLOCK_SIZE/2;j++)
        {
            if(tmp_usr_buff[j]!=user_data_buff[k*USER_DATA_BLOCK_SIZE/2+j])
            {
                err = 1;
                break;
            }
        }
        //CSW_TRACE(BASE_FFS_TS_ID,  "Check Flash i=%d,k=%d,err=%d",i,k,err);
        if(!err)
        {
            if(k==0)
                head_pos = i;
            memorymap|=(1<<i);
            k++;
            if(k*USER_DATA_BLOCK_SIZE>=p_Data_Header->length*2)
                break;
        }
        
    }

    CSW_TRACE(BASE_FFS_TS_ID, "Address = %x, map = %x",((UINT32)user_data_start-(UINT32)&_flash_start),memorymap);
    p_Data_Header->flag = USER_DATA_FLAG;
    p_Data_Header->map = memorymap;
    memd_FlashWrite((UINT8*)((UINT32)user_data_start+head_pos*USER_DATA_BLOCK_SIZE-(UINT32)&_flash_start),sizeof(struct UserData_Header_t),&write,p_Data_Header);
    //memd_FlashErase((UINT8*)((UINT32)user_data_start-(UINT32)&_flash_start), (UINT8*)USER_DATA_SIZE);
    //memd_FlashWrite((UINT8*)((UINT32)user_data_start-(UINT32)&_flash_start), size, &write, user_data_buff);
    hal_SysExitCriticalSection(status);
    COS_Free(tmp_usr_buff);
    return ERR_SUCCESS;
}


#define PHONE_BOOK_LENGTH      81920 // 80KB
#define PHONE_BOOK_FLAG        0xD5BC2D18
static UINT32 *phone_book_data_start = NULL;

struct PhoneBook_Header_t {
    UINT32 flag;
    UINT16 count;
    UINT8  item_size;
    UINT8  index_size;
	UINT32 valid_count;
} phone_header;

void DSM_Erase_Phonebook(void)
{
    int i = 0;
	UINT32 scStatus = 0;
    
    for(i = 0; i < PHONE_BOOK_LENGTH/4096;i++)
	{
		scStatus = hal_SysEnterCriticalSection();
		memd_FlashErase((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+4096*i, (UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+4096*(i+1));
		hal_SysExitCriticalSection(scStatus);
	}
}

void DSM_ResetPhoneBookAddress(UINT16 size, UINT8 sizeofItem, UINT8 sizeofIndex)
{
	int i = 0;
	UINT32 scStatus = 0;
    phone_header.flag = PHONE_BOOK_FLAG;
    phone_header.count = size;
    phone_header.item_size = sizeofItem;
    phone_header.index_size = sizeofIndex;
    phone_header.valid_count = 0;
	
	for(i = 0; i < PHONE_BOOK_LENGTH/4096;i++)
	{
		scStatus = hal_SysEnterCriticalSection();
		memd_FlashErase((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+4096*i, (UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+4096*(i+1));
		hal_SysExitCriticalSection(scStatus);
	}
	
/*
    CSW_TRACE(BASE_FFS_TS_ID,  "user_data_start:%x, phone_book_data_start:%x",user_data_start, phone_book_data_start);
    CSW_TRACE(BASE_FFS_TS_ID,  "_flash_start:%x",&_flash_start);
    CSW_TRACE(BASE_FFS_TS_ID,  "sub1:%x",((UINT32)user_data_start-(UINT32)&_flash_start));
	CSW_TRACE(BASE_FFS_TS_ID,  "size1:%x",((UINT8*)((UINT32)user_data_start-(UINT32)&_flash_start)+(USER_DATA_BLOCK_NUM*USER_DATA_BLOCK_SIZE)));
	CSW_TRACE(BASE_FFS_TS_ID,  "sub2:%x",((UINT32)phone_book_data_start-(UINT32)&_flash_start));
	CSW_TRACE(BASE_FFS_TS_ID,  "size2:%x",((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+PHONE_BOOK_LENGTH));

	while(1);*/
    // earse flash
//    memd_FlashErase((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start), (UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+PHONE_BOOK_LENGTH);
}

VOID *DSM_GetPhoneBookAddress(void)
{
    return phone_book_data_start;
}

VOID DSM_GetPhoneBookParam(UINT32 *length, UINT16 *size, UINT16 *valid_count)
{
    memcpy(&phone_header, phone_book_data_start, sizeof(phone_header));

    if(phone_header.flag == PHONE_BOOK_FLAG)
    	{
	    if(length)
	        *length = PHONE_BOOK_LENGTH-sizeof(phone_header);
	    if(size)
	        *size = phone_header.count;
		if(valid_count)
			*valid_count = phone_header.valid_count;
    	}
	else  phone_header.count = 0;
}

int DSM_GetPhoneBookOffset(void)
{
	return sizeof(phone_header)+phone_header.count*phone_header.index_size+512;
}

void DSM_WritePhoneBook(UINT8 *data, UINT16 index)
{
    UINT32 write;
 //   CSW_TRACE(BASE_FFS_TS_ID,  "write addr:%x",((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+sizeof(phone_header)+phone_header.count*phone_header.index_size+index*phone_header.item_size+512));
    // write flash
 //   memd_FlashWrite((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+512,phone_header.item_size,&write,data);
	{
		UINT32 scStatus = hal_SysEnterCriticalSection();
		memd_FlashWrite((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)
		+sizeof(phone_header)+phone_header.count*phone_header.index_size+index*phone_header.item_size+512
		,phone_header.item_size,&write,data);
		hal_SysExitCriticalSection(scStatus);
	}
}

void DSM_WritePhoneBookIndex(UINT8 *index, UINT16 size)
{
    UINT32 write;
    // write flash
    //phone_header.count = size;
    phone_header.valid_count = size;
 //   CSW_TRACE(BASE_FFS_TS_ID,  "memd_FlashWrite addr1:%x",((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+sizeof(phone_header)));
  //  CSW_TRACE(BASE_FFS_TS_ID,  "memd_FlashWrite addr2:%x",((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)));
  //  CSW_TRACE(BASE_FFS_TS_ID, "size :%d, phone_header.index_size:%d", size, phone_header.index_size);
	{
		UINT32 scStatus = hal_SysEnterCriticalSection();
    memd_FlashWrite((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start),sizeof(phone_header),&write,&phone_header);
    memd_FlashWrite((UINT8*)((UINT32)phone_book_data_start-(UINT32)&_flash_start)+sizeof(phone_header),size*phone_header.index_size,&write,index);
//	CSW_TRACE(BASE_FFS_TS_ID, "write size :%x", write);
			hal_SysExitCriticalSection(scStatus);
	}
//	while(1);
}


void DSM_UserDataInit(UINT32 *start_addr)
{
    UINT32 address = (UINT32)start_addr;
    address = (address+0xfff)&~0xfff; // 4KB aligned
    user_data_start = (UINT32*)address;

    address += USER_DATA_BLOCK_SIZE*USER_DATA_BLOCK_NUM;
    address = (address+0xfff)&~0xfff; // 4KB aligned
    phone_book_data_start = (UINT32*)address;

    address += PHONE_BOOK_LENGTH;
    address = (address+0xfff)&~0xfff; // 4KB aligned
    
}

#define UPGRADE_FLASH_BLOCK_SIZE        0x4000
extern INT16 g_CdacDelta;
VOID DSM_Upgrade_Flash(INT file, UINT8 *BTADDR)
{

    INT32 length ;
    INT32 ADDRESS, addr_offset, offset=0;
    UINT8 buf[1024], bt_addr[8], cdac[4];
    int data_cnt =0;

    cdac[0] = (UINT8)(((INT32)g_CdacDelta) &0xFF);
    cdac[1] = (UINT8)((((INT32)g_CdacDelta)>>8) &0xFF);
    cdac[2] = (UINT8)((((INT32)g_CdacDelta)>>16) &0xFF);
    cdac[3] = (UINT8)((((INT32)g_CdacDelta)>>24) &0xFF);

    FS_Seek(file, 0x8, FS_SEEK_SET);
    length=FS_Write(file, cdac, 4);

#ifdef BT_SUPPORT
    bt_addr[0] = 0x01;
    bt_addr[1] = 0x00;
    memcpy((void*)&bt_addr[2], BTADDR, 6);

    FS_Seek(file, 0xc, FS_SEEK_SET);
    FS_Read(file, &addr_offset,4);
    FS_Seek(file, addr_offset&0x00ffffff, FS_SEEK_SET);
    
    FS_Read(file, (char *)buf,1024);
    if(buf[0] == 0x07)
    {
        data_cnt = ((buf[3]<<8)|buf[2]);;
        for(int i=0; i<data_cnt; i++)
        {
            if(buf[8+i*4]==0x03)
            {
                offset =(2+data_cnt)*4+((buf[8+i*4+3]<<8)|buf[8+i*4+2])+32;
                hal_HstSendEvent(SYS_EVENT, 0x1248aba0);
                hal_HstSendEvent(SYS_EVENT, addr_offset);
                hal_HstSendEvent(SYS_EVENT, BTADDR);
                break;
            }
        }
    }


    ADDRESS = (addr_offset&0x00ffffff)+offset;
    
    FS_Seek(file, ADDRESS, FS_SEEK_SET);
    length=FS_Write(file, bt_addr, 8);
#endif

    FS_Close(file);

    hal_HstSendEvent(BOOT_EVENT, 0x09558000);
    memd_updateFlash();    
   

}


VOID DSM_SemInit(VOID)
{
    g_hDevSem = sxr_NewSemaphore(1);
    if((HANDLE)NULL == g_hDevSem)
    {
        D( ( DL_WARNING, "VDS_ModuleSemInit failed."));
    }
}


VOID DSM_SemDown(VOID)
{    
    if((HANDLE)NULL != g_hDevSem)
    {
        sxr_TakeSemaphore(g_hDevSem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleDown failed."));
    }

}


VOID DSM_SemUp(VOID)
{
    
    if((HANDLE)NULL != g_hDevSem)
    {
        sxr_ReleaseSemaphore(g_hDevSem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleUp failed."));
    }
}

    
