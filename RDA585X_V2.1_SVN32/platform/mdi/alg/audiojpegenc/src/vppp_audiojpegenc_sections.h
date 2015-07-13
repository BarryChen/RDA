#ifndef VPPP_AUDIOJPEGENC_SECTIONS_H
#define VPPP_AUDIOJPEGENC_SECTIONS_H


// all size and laddr defines are given in word size

#define RAM_I_START               (RAM_I_MIN/2) 
#define RAM_I_END                 (RAM_I_MAX/2) 

#define SECTION_1_START ( 0x100/2 + RAM_I_START) 

 

#define SECTION_AAC_MP3_COMMON_SIZE         (SECTION_1_START       - RAM_I_START) 

#define SECTION_MP3_ENC_SIZE				2000//(RAM_I_END       - SECTION_1_START) 
#define SECTION_AMR_ENC_SIZE				4400//(RAM_I_END       - SECTION_1_START) 



// start of different image sections in extern RAM


#define AAC_MP3_COMMON_START          RAM_I_START
#define MP3_DEC_START                (RAM_I_START+SECTION_AAC_MP3_COMMON_SIZE)
#define AMR_DEC_START                (RAM_I_START+SECTION_AAC_MP3_COMMON_SIZE)

#endif//VPPP_AUDIOJPEGENC_SECTIONS_H