#ifndef _MMC_JPEG_ENC_H_
#define _MMC_JPEG_ENC_H_
typedef enum
{
     NO_ENC_ERRO,
     ENC_FAILTOCREAT,
     ENC_FAILTOCLOSE,
     ENC_WRONGSIZE
}JPEGENC_ERROR;

extern  JPEGENC_ERROR  MMC_JpegEncode(char* filename,short quality,short ImgWidth,short ImgHeight,short* InBuff,unsigned long* outlen);
#endif
