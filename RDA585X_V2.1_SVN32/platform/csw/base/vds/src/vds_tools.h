#ifndef _VDS_TOOLS_H_
#define _VDS_TOOLS_H_

BOOL vds_GetLineFromStr( char *line, int n, char *str, int *offset );
UINT32 vds_CRC32( CONST VOID *pvData, UINT32 iLen );
UINT32 vds_GetPBDCRC( VDS_PBD* psPBD );
INT32 vds_SetPBDCRC( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, UINT32 iCRC );
BOOL vds_IsCRCChecked( UINT8* pBuff, UINT32 iLen, UINT32 iCRC, UINT32 iMode );


void vds_str_to_hex( char *str, long *len, char *buf );
void vds_hex_to_str( char *hex, long len, char *buf, char separator );


#endif // _VDS_TOOLS_H_




