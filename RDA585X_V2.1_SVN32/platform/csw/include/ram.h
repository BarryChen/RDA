#ifndef _RAM_H_ 
#define _RAM_H_

#include <csw.h>


#define EV_RAM_MODEM_COM_OPENED_IND                         1213
#define EV_RAM_MODEM_COM_CLOSED_IND                         1214
#define EV_RAM_MODEM_TRANS_BEGIN_IND                        1215
#define EV_RAM_MODEM_TRANS_OVER_IND                         1216



typedef enum _COMM_STATE_
{
	COMM_IDLE=0,
	COMM_USED=1,
}COMM_STATE;


typedef struct _RAM_COMMUNICATION_MANAGE
{
	HANDLE RamManageSem ;
	DRV_UARTID UartID;
	COMM_STATE CommState;
	DRV_UARTCFG drv_uartcfg;
	VOID* pRamModemApiMsg;
}RAM_COMMUNICATION_MANAGE;



INT32 RAM_ModemTaskStart(UINT8  ModemTaskPriority);
VOID  RAM_MmiEventProc(CFW_EVENT* pEvent);
INT32 RAM_ModemOpen(VOID );
INT32 RAM_ModemClose(VOID );
VOID RAM_GetModemManageInfo(RAM_COMMUNICATION_MANAGE * pModemCmuManage);
VOID RAM_StopTrans(VOID);

#endif



