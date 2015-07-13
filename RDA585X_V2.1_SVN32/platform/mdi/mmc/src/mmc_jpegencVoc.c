
#include "vpp_audiojpegenc.h"
#include "mmc.h"
#include "cs_types.h"
#include"mcip_debug.h"

volatile int jpgEnframecount=0;
volatile int jpgEnFirstIsr=0;

void JPEGENCIsr(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]JPEGENCIsr");

    if (jpgEnFirstIsr)
        jpgEnframecount=1;
    else
        jpgEnFirstIsr=1;
}
boolean getJpegEncodeFinish(void)
{
    //MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]getJpegEncodeFinish");
    return jpgEnframecount;
}
INT32 initVocJpegEncode(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]initVocJpegEncode");
    jpgEnframecount=0;
    jpgEnFirstIsr=0;

    if(HAL_ERR_RESOURCE_BUSY == vpp_AudioJpegEncOpen((HAL_VOC_IRQ_HANDLER_T)JPEGENCIsr)) 
        return -1;
    else 
        return 0;
}

void quitVocJpegEncode(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]quitVocJpegEncode");
    vpp_AudioJpegEncClose();
}

uint32 getJpegEncodeLen(void)
{	
    vpp_AudioJpeg_ENC_OUT_T  g_vppaudiojpeg_encDecStatus;

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]getJpegEncodeLen");

    //abtain the outputlen
    vpp_AudioJpegEncStatus(&g_vppaudiojpeg_encDecStatus);
    return g_vppaudiojpeg_encDecStatus.output_len;
    //do what you want....
}

void  MMC_jpgEn (UINT16 srcw, UINT16 srch, UINT16 imgw, UINT16 imgh, UINT16 quality, char *bufin, char *bufout, UINT16 yuvmode)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0, "[MMC_JPEGENC]MMC_jpgEn width: %d;width:%d", imgw, imgh);

    vpp_AudioJpeg_ENC_IN_T g_vppaudiojpeg_encDecIn;
    //encode a picture
    g_vppaudiojpeg_encDecIn.SampleRate = ((srch<<16)|srcw);
    g_vppaudiojpeg_encDecIn.imag_width = imgw;
    g_vppaudiojpeg_encDecIn.imag_height = imgh;
    g_vppaudiojpeg_encDecIn.imag_quality = quality;
    g_vppaudiojpeg_encDecIn.inStreamBufAddr = (INT32*)bufin;
    g_vppaudiojpeg_encDecIn.outStreamBufAddr = (INT32*)(bufout + 608);
    g_vppaudiojpeg_encDecIn.mode = MMC_MJPEG_ENCODE;
    g_vppaudiojpeg_encDecIn.channel_num = yuvmode;

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 
                0, 
                "[MMC_JPEGENC]MMC_jpgEn......BEGIN TIME=%d",
                hal_TimGetUpTime());

    vpp_AudioJpegEncScheduleOneFrame(&g_vppaudiojpeg_encDecIn);

    while(getJpegEncodeFinish()==0)
    {
        ;
    }
    //mmi_trace(1,"voc encode a frame");
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 
                0, 
                "[MMC_JPEGENC]ENCODING DONE! END TIME=%d",
                hal_TimGetUpTime());

    jpgEnframecount = 0;
    VPP_WRITE_JPEGHeadr(g_vppaudiojpeg_encDecIn.imag_quality,
                g_vppaudiojpeg_encDecIn.imag_width,
                g_vppaudiojpeg_encDecIn.imag_height,
                (uint8*) bufout);
hal_HstSendEvent(0xbcbc0104);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 
                0, 
                "[MMC_JPEGENC]VPP_WRITE_JPEGHeadr DONE! END TIME=%d",
                hal_TimGetUpTime());
}


