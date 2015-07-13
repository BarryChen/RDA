/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2004, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      assert.h                                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      This file redefines the standard assert.                         */
/*                                                                       */
/*************************************************************************/

#include "cs_types.h"
#include "hal_debug.h"
#include "assert.h"

void __assert(const char* fmt)
{
    hal_DbgAssert(fmt);   
}


