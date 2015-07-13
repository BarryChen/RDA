
#ifndef _CORDIC_H_
#define _CORDIC_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'cordic'."
#endif





#include "globals.h"




// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CORDIC
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_CORDIC_BASE               0x01911000

typedef volatile struct
{
    REG32                          yin;                    //0x00000000
    REG32                          xin;                    //0x00000004
    REG32                          cmd;                  //0x00000008
    REG32                          status;              //0x00000008
    REG32                          dout;                 //0x0000000C
} HWP_CORDIC_T;

#define hwp_cordic                    ((HWP_CORDIC_T*) KSEG1(REG_CORDIC_BASE))


//command
#define CORDIC_CMD_RST                  (0<<0)
#define CORDIC_CMD_START              (1<<0)


//status
#define CORDIC_OP_FREE             (0xFFFFFFFF)


#endif

