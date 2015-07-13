
#ifndef	_DSM_CONFIG_H_
#define	_DSM_CONFIG_H_


// =============================================================================
// DSM_CONFIG_T
// -----------------------------------------------------------------------------
// This structue discripte the DSM partition config information,include partition number and partition table.
// dsmPartitionNumber: Partition nmuber.
// dsmPartitionInfo: Array of partition information.
// =============================================================================
typedef struct _dsm_config_t
{
    // Partition number.
    UINT32                  dsmPartitionNumber;
    
    // Partiton table.
#ifdef WIN32    
    DSM_PARTITION_CONFIG    dsmPartitionInfo[16];   
#else
    DSM_PARTITION_CONFIG    dsmPartitionInfo[];   
#endif
} DSM_CONFIG_T;
CONST DSM_CONFIG_T* tgt_GetDsmCfg(VOID);
#endif

