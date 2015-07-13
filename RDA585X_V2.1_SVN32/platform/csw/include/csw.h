/******************************************************************************/
/*                                                                            */
/*              Copyright (C) 2005, Coolsand Technologies, Inc.               */
/*                            All Rights Reserved                             */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* This source code is property of Coolsand. The information contained in     */
/* this file is confidential. Distribution, reproduction, as well as          */
/* exploitation,or transmisison of any content of this file is not            */
/* allowed except if expressly permitted.                                     */
/* Infringements result in damage claims!                                     */
/*                                                                            */
/* FILE NAME                                                                  */
/*      csw.h                                                                 */
/*                                                                            */
/* DESCRIPTION                                                                */
/*      all basic types or interface definitions on CSW.                      */
/*                                                                            */
/******************************************************************************/
#if !defined(__CSW_H__)
#define __CSW_H__

#include <cswtype.h>
#include <errorcode.h>
#include <stdkey.h>
#include <event.h>
#include <cos.h>
#include <cfw.h>
#include <dm.h>
#include <sul.h>
#include <tm.h>
#include <drv.h>
#include <dm.h>
#include <pm.h>
#include <ds.h>
#include <ts.h>
#include <fs.h>
#include <csw_shell.h>

//CSW default least power on voltage
#define DEFAULTLEAST_VOLTAGE            3200
//CSW support min least power on voltage for user set
#define MINLEAST_VOLTAGE                3000
//CSW support max least power on voltage for user set
#define MAXLEAST_VOLTAGE                3500

//CSW support user set min memory size for cos_malloc 
#define MIN_MMI_MEMORY_SIZE             (50*1024)
//CSW support user set max memory size for cos_malloc 
#define MAX_MMI_MEMORY_SIZE             (3000*1024) //Modify by lixp for WAP issue 20081125
//CSW support user default memory size for cos_malloc 
#define DEFAULT_MMI_MEMORY_SIZE         (400*1024)




BOOL SRVAPI BAL_ApplicationInit (
  VOID
);

VOID SRVAPI BAL_MmiTask (
  VOID* pData
);
//The struct for user set default power on parameter
typedef struct _mmi_default_value
{
    UINT16  nMinVol ;      //least voltage for power on mobile
    UINT32  nMemorySize ;  //memory size for cos_malloc 
}MMI_Default_Value, *pMMI_Default_Value, **ppMMI_Default_Value ;
//The function CSW provide sample, user must implement for set default parameter
VOID BAL_SetMMIDefaultValue(VOID) ;
#endif  // H

