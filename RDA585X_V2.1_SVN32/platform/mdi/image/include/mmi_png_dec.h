#ifndef MMI_PNG_DEC_H 
#define MMI_PNG_DEC_H

#ifndef SHEEN_VC_DEBUG
#include "cswtype.h"
#else
#include "zconf.h"
#endif
//typedef unsigned int UINT32;
//typedef unsigned char BYTE;
//typedef unsigned char UINT8;
#define MAXPNG_BUF 100*1024 // 100K
#define PNGS8  8
#define PNGQ8 256
#define RESMASK (PNGQ8-1)


typedef struct {
	char * PNGSTREAM;
	INT32  byteleft;
} Png_StreamBuf;

int MMF_PngDecodePicture(U8 *FileContent,
			  UINT32 FileSize,
			  U8 *PNGOutBuffer,         
			  UINT32 OutBufferSize,     
			  U8 *AlphaOutBuffer,	
			  UINT32 AlphaBufferSize,
			  U8 *BkgrdBuffer,		
			  U16 BkgrdClipX1,	
			  U16 BkgrdClipX2,	
			  U16 BkgrdClipY1,	
			  U16 BkgrdClipY2,	
			  S16 BkgrdOffsetX, 
			  S16 BkgrdOffsetY, 
			  U16 BkgrdWidth,
			  U16 BkgrdHeight,	
			  U16 ZoomWidth,	
			  U16 ZoomHeight,	
			  U16 DecMode,
			  UINT32 *ImgWidth,
			  UINT32 *ImgHeigh 
			  );
int PNG_display(unsigned short preview_startx,unsigned short preview_starty);
VOID PNG_Init(VOID);

#endif

