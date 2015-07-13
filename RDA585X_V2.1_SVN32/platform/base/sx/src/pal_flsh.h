/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File         pal_flsh.h
--------------------------------------------------------------------------------

  Scope      : PAL flash header file.

  History    :
--------------------------------------------------------------------------------
 Jun 13 2003 |  ADA  | Creation
================================================================================
*/

#ifndef __PAL_FLSH_H__
#define __PAL_FLSH_H__

#include "sxs_type.h"

#ifdef __SXS_FLASH_INTEL__
#include "pal_flhi.h"
#else
#include "pal_flhr.h"
#endif

#endif

