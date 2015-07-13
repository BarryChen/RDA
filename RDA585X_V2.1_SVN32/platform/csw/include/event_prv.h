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
/*       event_prv.h                                                          */
/*                                                                            */
/* DESCRIPTION                                                                */
/*   event constant definitions and macros.                                   */
/*                                                                            */
/******************************************************************************/

#ifndef __EVENT_PRV_H__
#define __EVENT_PRV_H__

#define EV_CFW_INIT_REQ          (EV_CSW_REQ_BASE+1)
#define EV_CFW_XXX_END_          (EV_CSW_REQ_BASE+1)

#if (EV_CFW_XXX_END_ >= (EV_CSW_REQ_END_))
#error 'Error: invalid EV_CFW_XXX_END_ definition!'
#endif


#endif // _H_


