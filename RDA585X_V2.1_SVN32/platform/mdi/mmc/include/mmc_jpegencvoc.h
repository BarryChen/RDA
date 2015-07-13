
#ifndef  MMC_JPEGENCVOC
#define  MMC_JPEGENCVOC

void JPEGENCIsr(void);
boolean getJpegEncodeFinish(void);
INT32 initVocJpegEncode(void);
void quitVocJpegEncode(void);
uint32 getJpegEncodeLen(void);
void  MMC_jpgEn (UINT16 srcw,  // width of source
                UINT16 srch,        // height of source
                UINT16 imgw,      // width of the jpeg image
                UINT16 imgh,       // height of the jpeg image 
                UINT16 quality,    // quality of the jpeg image
                char *bufin,         // buffer of the source
                char *bufout,       // buffer of the jpeg image
                UINT16 yuvmode  // 1 - YUV, 0 - RGB565
        );

#endif

