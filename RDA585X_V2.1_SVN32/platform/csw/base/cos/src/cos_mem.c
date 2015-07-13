#include "sxs_srl.h"
#include "sxs_type.h"
#include "sxs_lib.h"
#include "sxr_ops.h"
#include "errorcode.h"
#include "sxr_mem.hp"
//#define HAVE_NO_TS_IF
#include "ts.h"
#include "cswtype.h"
#include "csw.h"
#include "csw_config.h"
#include "sxs_io.h"
#include "sxr_mem.h"
#include "sxr_usrmem.h"


/*
 * for memory less than 256byte, use memory poll,
 * for memory great than 256byte, use list to reduce times malloc/free
 */

/*
 * memory pool: 64byte * 256block
 * for memory less than 64, alloc from head
 * for memory greate than 64 and less than 256, alloc from bottom
 */

#define MEMORY_BLOCK_SIZE       32
#define MEMORY_POOL_SIZE      16 ///  32
#define MEMORY_BIG_SIZE       512 // 1024
#define HEAP_COUNT               3

typedef struct st_mem_block t_mem_block;

struct st_mem_block {
    UINT32 length;
    t_mem_block *next;
};

typedef struct t_host_mem_info {
    HANDLE mem_mutex;
    BOOL   uncache_independ;

    UINT32 *pHeap_Base[3];
    UINT32 heap_Size[3];

    t_mem_block *mem_pool[MEMORY_POOL_SIZE];
    t_mem_block *mem_list;          /* memory block list greate than 1024 bytes */
    t_mem_block *uncache_mem_list;          /* uncache memory block list */
	//t_mem_block *alloc_list;

	UINT32 alloc_length;
	UINT32 free_length;
} t_mem_info;

t_mem_info cos_mem_info;

U32 *g_pHeapTop; // for test

// ============================================================================= 
// COS_Malloc 
// ----------------------------------------------------------------------------- 
/// For application malloc
// ============================================================================= 
void trace_mem()
{
	hal_HstSendEvent(SYS_EVENT,g_pHeapTop);
	//hal_HstSendEvent(SYS_EVENT,g_pHeapBase);
}

t_mem_block *alloc_mem_block(int blk_index, UINT32 size)
{
    t_mem_block *pMem = NULL;
    struct t_host_mem_info *pMemInfo = &cos_mem_info;
    
    if(pMemInfo->heap_Size[blk_index] >= size+sizeof(t_mem_block))
    {
        pMem = (t_mem_block*)(pMemInfo->pHeap_Base[blk_index]);
        pMem->length = size;
        pMem->next = NULL;
        pMemInfo->pHeap_Base[blk_index] += (pMem->length+sizeof(t_mem_block))>>2;
        pMemInfo->heap_Size[blk_index] -= (pMem->length+sizeof(t_mem_block));
        //CSW_TRACE(BASE_COS_TS_ID, "COS_Malloc allocate in heap[%d],size=%d, pHeap_Base=%x,heap_size=%d", blk_index, size, pMemInfo->pHeap_Base[blk_index], pMemInfo->heap_Size[blk_index]);
    }
    return pMem;
}

t_mem_block *find_mem_block(t_mem_block *header, UINT32 size)
{
    t_mem_block *pMem = NULL, *pNext;

    pNext = pMem = header;
    while(pMem)
    {
        if(pMem->length >= size)
        {
            if(pMem->length > (size << 1))
            {
            	pMem = NULL;
                break;
            }
            if(pNext != pMem)
                pNext->next = pMem->next;
            break;
        }
        pNext = pMem;
        pMem = pMem->next;
    }
    return pMem;
}

PVOID COS_Malloc(UINT32 size)
{
    struct t_host_mem_info *pMemInfo = &cos_mem_info;
    UINT8 *ptr = NULL;
    UINT32 blk_index;
    t_mem_block *pMem = NULL, *pNext;

    UINT32 csStatus = hal_SysEnterCriticalSection();
    //CSW_TRACE(BASE_COS_TS_ID, "COS_Malloc size:%d", size);
    /* alloc in memory pool */
    if(size<=MEMORY_BIG_SIZE)
    {
        blk_index = (size-1)/MEMORY_BLOCK_SIZE;
        if(pMemInfo->mem_pool[blk_index] != NULL)
        {
            pMem = pMemInfo->mem_pool[blk_index];
            pMemInfo->mem_pool[blk_index] = pMem->next;
        }
    }
    else
    {
        pMem = find_mem_block(pMemInfo->mem_list, size);
        if(pMem != NULL && pMem == pMemInfo->mem_list)
        {
            pMemInfo->mem_list = pMem->next;
        }
    }
	if(pMem)
		pMemInfo->free_length-=pMem->length+16;
	
    if(pMem == NULL)
    {
        size = ((size+MEMORY_BLOCK_SIZE-1)&(~(MEMORY_BLOCK_SIZE-1))); // 32 bytes aligned
        for(blk_index=0;blk_index<HEAP_COUNT;blk_index++)
        {
            pMem = alloc_mem_block(blk_index, size);
            if(pMem != NULL)
                break;
        }
        if(pMem == NULL)
            hal_DbgAssert("The memory heap overflow!");
    }

    hal_SysExitCriticalSection(csStatus);
    
    pMem->next = NULL;
    ptr = (UINT8*)(pMem+1);
    
    /* clear the memory */
    if(ptr != NULL)
    {
        memset(ptr, 0, size);
#ifdef COS_MEM_TEST
		if(pMemInfo->alloc_list != NULL)
			pMemInfo->alloc_list->p2 = pMem;
		pMem->p1 = pMemInfo->alloc_list;
		pMem->p2 = NULL;
		pMemInfo->alloc_list = pMem;
		pMemInfo->alloc_length+=pMem->length+16;
#endif
    }

 //   CSW_TRACE(BASE_BAL_TS_ID, "COS_Malloc: alloc %d length at ptr=0x08%x\n", size, ptr);  
	//hal_HstSendEvent(SYS_EVENT,g_pHeapTop);
    return ptr;
}

PVOID SRVAPI COS_Malloc_NoCache ( UINT32 size)
{
    struct t_host_mem_info *pMemInfo = &cos_mem_info;
    t_mem_block *pMem = NULL, *pNext;
    UINT8 *ptr = NULL;
    UINT32 blk_index;

    if(!pMemInfo->uncache_independ)
    {
        ptr = COS_Malloc(size);
        if(ptr)
            ptr = (UINT32)ptr|0x20000000;
        return ptr;
    }

    UINT32 csStatus = hal_SysEnterCriticalSection();

    pMem = find_mem_block(pMemInfo->uncache_mem_list, size);
	if(pMem)
		pMemInfo->free_length-=pMem->length+16;
    if(pMem != NULL && pMem == pMemInfo->uncache_mem_list)
    {
        pMemInfo->uncache_mem_list = pMem->next;
    }

    if(pMem == NULL)
    {
        size = (size+MEMORY_BLOCK_SIZE-1)&(~(MEMORY_BLOCK_SIZE-1)); // 32 bytes aligned
        for(blk_index=HEAP_COUNT-1;blk_index>0;blk_index--) // heap 0 can be used by uncache buffer
        {
            if(pMemInfo->heap_Size[HEAP_COUNT-1] >= size+sizeof(t_mem_block))
            {
                pMem = alloc_mem_block(blk_index, size);
                if(pMem != NULL)
                    break;
            }
        }
        if(pMem == NULL)
            hal_DbgAssert("The memory heap overflow!");
    }

    hal_SysExitCriticalSection(csStatus);
    
    pMem->next = NULL;
    ptr = (UINT8*)((UINT32)(pMem+1)|0x20000000);
    
    /* clear the memory */
    if(ptr != NULL)
    {
        memset(ptr, 0, size);
#ifdef COS_MEM_TEST
		if(pMemInfo->alloc_list != NULL)
			pMemInfo->alloc_list->p2 = pMem;
		pMem->p1 = pMemInfo->alloc_list;
		pMem->p2 = NULL;
		pMemInfo->alloc_list = pMem;
		pMemInfo->alloc_length+=pMem->length+16;
#endif
    }
    return ptr;
}

BOOL COS_Free(PVOID ptr)
{
    struct t_host_mem_info *pMemInfo = &cos_mem_info;

    t_mem_block *pMem;
    t_mem_block *pNext;
    UINT32 blk_index;
    UINT32 csStatus;
    BOOL header;

    if(ptr == NULL)
        return FALSE;

    if(!pMemInfo->uncache_independ)
        ptr = (UINT32)ptr&~0x20000000;

    pMem = (t_mem_block*)(((UINT32)ptr&~0x20000000)-sizeof(t_mem_block));

#ifdef COS_MEM_TEST
	{
		pMemInfo->alloc_length-=pMem->length+16;
		if(pMem->p2 != NULL)
			 pMem->p2->p1 = pMem->p1;
		if(pMem->p1 != NULL)
			 pMem->p1->p2 = pMem->p2;
		if(pMemInfo->alloc_list == pMem)
			pMemInfo->alloc_list = pMem->p1;
		pMem->p1 = NULL;
		pMem->p2 = NULL;
	}
#endif

    /* memory in pool */
    if(pMem->length<=MEMORY_BIG_SIZE && ((((UINT32)ptr)&0x20000000) == 0))
    {
        blk_index = (pMem->length)/MEMORY_BLOCK_SIZE-1;
        csStatus = hal_SysEnterCriticalSection();
        pMem->next = pMemInfo->mem_pool[blk_index];
        pMemInfo->mem_pool[blk_index] = pMem;
		pMemInfo->free_length+=pMem->length+16;
        hal_SysExitCriticalSection(csStatus);
    }
    else
    {
        if(pMem->next != NULL)
        {
            hal_DbgAssert("The free memory may already be freed 0x%x!", pMem);
            return TRUE;
        }

        //COS_WaitForSemaphore(pMemInfo->mem_mutex, COS_WAIT_FOREVER);
        csStatus = hal_SysEnterCriticalSection();

        for(blk_index=0;blk_index<HEAP_COUNT;blk_index++)
        {
            if(pMemInfo->pHeap_Base[blk_index] == ((UINT8*)ptr+pMem->length))
            {
                pMemInfo->pHeap_Base[blk_index] -= (pMem->length+sizeof(t_mem_block))>>2;
                pMemInfo->heap_Size[blk_index] += (pMem->length+sizeof(t_mem_block));
                //CSW_TRACE(BASE_COS_TS_ID, "COS_Free return memory block 0x%x to heap[%d], pHeap_Base=%x,heap_size=%d", ptr, blk_index, pMemInfo->pHeap_Base[blk_index], pMemInfo->heap_Size[blk_index]);
                pMem = NULL;
                break;
            }
        }

        if(pMem != NULL)
        {
        	pMemInfo->free_length+=pMem->length+16;
            if(((UINT32)ptr)&0x20000000)
                pNext = pMemInfo->uncache_mem_list;
            else
                pNext = pMemInfo->mem_list;
            
            if(pNext == NULL)
            {
                header = TRUE;
                pMem->next = NULL;
            }
            else
            {
                while(pNext->next && pNext ->next->length < pMem->length)
                {
                    //pAssert(pNext != pMem);
                    pNext = pNext->next;
                };
                
                if(pNext ->length < pMem->length)
                {
                    header = FALSE;
                    pMem->next = pNext->next;
                    pNext->next = pMem;
                }
                else
                {
                    header = TRUE;
                    pMem->next = pNext;
                }
            }
            if(header)
            {
                if(((UINT32)ptr)&0x20000000)
                    pMemInfo->uncache_mem_list = pMem;
                else
                    pMemInfo->mem_list = pMem;
            }
                
            //COS_ReleaseSemaphore(pMemInfo->mem_mutex);
            
        }
        hal_SysExitCriticalSection(csStatus);
    }

 //   CSW_TRACE(BASE_BAL_TS_ID, "COS_Free: free memory at ptr=0x08%x\n", ptr);  

  return FALSE;
}

PVOID  COS_MallocDbg (UINT32 nSize, CONST UINT8* pszFileName, UINT32 nLine)
{
	UINT8* p = COS_Malloc(nSize);
    
	CSW_TRACE(BASE_BAL_TS_ID, "COS_MallocDbg+: 0x%08x, size %d, %s[%d] \n", p, nSize, pszFileName, nLine);
    
    return (PVOID)p;
}

BOOL COS_FreeDbg (VOID* pMemAddr, CONST UINT8* pszFileName, UINT32 nLine)
{
    CSW_TRACE(BASE_BAL_TS_ID, "COS_FreeDbg-: 0x%08x, %s[%d] \n", pMemAddr, pszFileName, nLine);
    
    return COS_Free(pMemAddr);
}

VOID COS_MemIint()
{
    VOLATILE UINT32 heap_size;
    memset(&cos_mem_info, 0, sizeof(cos_mem_info));

    heap_size = (UINT32)&_heap_size;

    if(heap_size == 0) // no external memory
    {
        cos_mem_info.uncache_independ = TRUE;
        cos_mem_info.pHeap_Base[0] = &_bcpu_ram_heap_start;
        cos_mem_info.heap_Size[0] = &_bcpu_ram_heap_size;
    }
    else
    {
        cos_mem_info.uncache_independ = FALSE;
        cos_mem_info.pHeap_Base[0] = (UINT32*)&_heap_start;
        cos_mem_info.heap_Size[0] = (UINT32)&_heap_size;
    }
    cos_mem_info.pHeap_Base[1] = &_dualport_heap_start;
    cos_mem_info.heap_Size[1] = &_dualport_heap_size;
    cos_mem_info.pHeap_Base[2] = &_sys_sram_heap_start;
    cos_mem_info.heap_Size[2] = &_sys_sram_heap_size;

    cos_mem_info.mem_mutex = NULL;//COS_CreateSemaphore(1);
}


// =============================================================================
// COS_PageProtectSetup
// -----------------------------------------------------------------------------
/// This function setups a protection page
///
/// @param nPageNum Name of the page we want to configure  
/// @param nMode Protection mode
/// @param nStartAddr Address of the beginning of the page
/// @param nEndAddr  End address of the page. This address is not included in 
/// the page
// =============================================================================  
PUBLIC UINT32 COS_PageProtectSetup( COS_PAGE_NUM nPageNum, COS_PAGE_SPY_MODE nMode, UINT32 nStartAddr, UINT32 nEndAddr )
{
  if(( nPageNum < COS_PAGE_NUM_4 )||( nPageNum > COS_PAGE_NUM_5 )|| ( nMode < COS_PAGE_NO_TRIGGER) || ( nMode > COS_PAGE_READWRITE_TRIGGER ))
  {
    return ERR_INVALID_PARAMETER;
  }
  
  hal_DbgPageProtectSetup( nPageNum, nMode, nStartAddr, nEndAddr );

  return ERR_SUCCESS;
}


// =============================================================================
// COS_PageProtectEnable
// -----------------------------------------------------------------------------
/// Enable the protection of a given page.
///
/// @param nPageNum Page to enable
// =============================================================================
PUBLIC UINT32 COS_PageProtectEnable( COS_PAGE_NUM nPageNum )
{
  if(( nPageNum < COS_PAGE_NUM_4 )||( nPageNum > COS_PAGE_NUM_5 ))
  {
    return ERR_INVALID_PARAMETER;
  }
  
  hal_DbgPageProtectEnable(nPageNum);

  return ERR_SUCCESS;
}


// =============================================================================
// COS_PageProtectDisable
// -----------------------------------------------------------------------------
/// Disable the protection of a given page
///
/// @param nPageNum Page to disable
// =============================================================================

PUBLIC UINT32 COS_PageProtectDisable( COS_PAGE_NUM nPageNum )
{
  if(( nPageNum < COS_PAGE_NUM_4 )||( nPageNum > COS_PAGE_NUM_5 ))
  {
    return ERR_INVALID_PARAMETER;
  }

  hal_DbgPageProtectDisable(nPageNum);

  return ERR_SUCCESS;
}


