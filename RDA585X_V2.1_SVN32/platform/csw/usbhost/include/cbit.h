#ifndef _CBIT_H_
#define _CBIT_H_

#include "usbmscp.h"

//*****************************************************************************
// D E F I N E S
//*****************************************************************************

#define MAX_CBIT_STALL_COUNT 3

#define CBIT_COMMAND_COMPLETION_INTERRUPT   0x00

#define CBIT_STATUS_SUCCESS             0x00
#define CBIT_STATUS_FAIL                0x01
#define CBIT_STATUS_PHASE_ERROR         0x02
#define CBIT_STATUS_PERSISTENT_ERROR    0x03

//*****************************************************************************
//
// F U N C T I O N    P R O T O T Y P E S
//
//*****************************************************************************

uint32 CBIT_DataTransfer(PUSBMSC_DEVICE pUsbDevice, PTRANSPORT_COMMAND pCommand, PTRANSPORT_DATA pData, BOOL Direction);

#endif // _CBIT_H_

// EOF
