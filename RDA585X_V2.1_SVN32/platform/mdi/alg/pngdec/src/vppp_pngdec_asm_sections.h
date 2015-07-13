
// NOTE : all size and laddr defines are given in word size

// definition of the sections
#define SECTION_0_START            (((RAM_I_MIN)+0x0000)/2)
#define SECTION_1_START            (((RAM_I_MIN)+0x0200)/2)

// start of the images
#define SECTION_MAIN_START          SECTION_0_START
#define SECTION_HI_START            SECTION_1_START
#define SECTION_BYE_START           SECTION_1_START

// end of the images
#define SECTION_MAIN_END            (((RAM_I_MIN)+0x0100)/2)
#define SECTION_HI_END              (((RAM_I_MIN)+0x0280)/2)
#define SECTION_BYE_END             (((RAM_I_MIN)+0x0280)/2)

// size of the images
#define SECTION_MAIN_SIZE           (SECTION_MAIN_END - SECTION_MAIN_START)
#define SECTION_HI_SIZE             (SECTION_HI_END   - SECTION_HI_START)
#define SECTION_BYE_SIZE            (SECTION_BYE_END  - SECTION_BYE_START)
