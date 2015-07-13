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
  File       : sxs_cfg.h
--------------------------------------------------------------------------------

  Scope      : System configuration.

  History    :
--------------------------------------------------------------------------------
  Aug 25 03  | ADA   | Creation
================================================================================
*/

#ifndef __SXS_CFG_H__
#define __SXS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "cmn_defs.h"

typedef struct
{
 void (*TaskInit)(void);
 void (*MsgParse)(void *);
 void (*TimerParse)(u32 *);
 u8   MailBox;
} sxs_TaskParse_t;

typedef struct
{
 const sxr_TaskDesc_t  TaskDesc;
 const sxs_TaskParse_t *TaskParse;
} sxs_TaskCfg_t;

void sxs_Task (sxs_TaskParse_t *TaskParse);


#ifdef __cplusplus
 }
#endif

#endif

