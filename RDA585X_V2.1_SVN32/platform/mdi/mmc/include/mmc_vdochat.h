#ifndef __MMC_VDOCHAT_H__
#define __MMC_VDOCHAT_H__


//declaration for customer
int   mmc_vdochat_powerOn(BOOL bIsRGB);
void mmc_vdochat_powerOff(void);

void mmc_vdochat_previewStart(UINT32 startX,UINT32 startY,UINT32 preW,UINT32 preH);
void mmc_vdochat_previewStop(void);

UINT8* mmc_vdochat_getData(UINT16 width, UINT16 height);

typedef enum
{
	//STOP  MESSAGE
	VDOCHAT_PRIVIEW=0x30,
	VDOCHAT_SETUP=0x31,
	VDOCHAT_ERR =  0x32,

} VDOCHAT_MSGID;

//declaration for ourself
 void vdochat_Callback(UINT8 id,  UINT8 ActiveBufNum);
void vdochat_usrmsgHandle(COS_EVENT *pnMsg) ;

#endif//__MMC_VDOCHAT_H__

