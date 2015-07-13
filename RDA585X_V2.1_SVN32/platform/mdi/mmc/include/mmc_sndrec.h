#ifndef _MMC_SNDREC_H_
#define _MMC_SNDREC_H_
#include"cs_types.h"
#include"fs.h"
#include"mci.h"

#define SNDRECBUF_LEN (1024*2)  //(8000)
#define SNDRECBUF_LEN_AMR475 (900)
#define SNDRECBUF_LEN_AMR515 (1200)
#define SNDRECBUF_LEN_AMR59 (1200)
#define SNDRECBUF_LEN_AMR67 (750)
#define SNDRECBUF_LEN_AMR74 (750)
#define SNDRECBUF_LEN_AMR795 (750)
#define SNDRECBUF_LEN_AMR102 (1050)
#define SNDRECBUF_LEN_AMR122 (1200)


typedef enum
{
	SNDREC_DATA_PART_HALF,
	SNDREC_DATA_PART_END		
}SNDREC_DATA_PART;

void sndRec_msgHandle(SNDREC_DATA_PART part);
void sndrec_halfHandler();
void sndrec_endHandler();

extern INT32 Mmc_sndRecStart(HANDLE fhd,U8 quality,mci_type_enum format, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback,  MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback);
extern INT32 Mmc_sndRecStop(void);
extern INT32 Mmc_sndRecPause(void);
extern INT32 Mmc_sndRecResume(void);
extern INT32 Mmc_FmRecStart(HANDLE fhd,mci_type_enum format);



#endif //_MMC_SNDREC_H_
