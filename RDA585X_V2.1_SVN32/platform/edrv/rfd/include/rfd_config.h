#ifndef _RFD_CONFIG_H_
#define _RFD_CONFIG_H_

#include "cs_types.h"

typedef struct
{
    UINT8  RST;
    UINT8  PDN;
}XCV_CONFIG_T;

typedef struct
{
    UINT8  ENA;
    UINT8  TXEN;
    UINT8  BS;
}PA_CONFIG_T;

typedef struct
{
    UINT8  SW1;
    UINT8  SW2;
    UINT8  SW3;
}SW_CONFIG_T;

#endif
