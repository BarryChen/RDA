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
  File         sxs_defs.h
--------------------------------------------------------------------------------

  Scope      : Compilation flags definition.

  History    :
--------------------------------------------------------------------------------
 Aug 13 2003 |  ADA  | Creation
================================================================================
*/

#ifndef __SXS_DEFS_H__
#define __SXS_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__arm) || defined (__TARGET__) || defined (__SERIAL__)
#define __EMBEDDED__
#endif

#ifdef __cplusplus
}
#endif

#endif

