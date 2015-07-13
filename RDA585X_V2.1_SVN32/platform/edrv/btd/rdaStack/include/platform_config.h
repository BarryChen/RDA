/***********************************************************************
 *
 * MODULE NAME:    platform_config.h
 * DESCRIPTION:    Host Side Stack Win32 specific configuration options 
 * AUTHOR:         Martin Mellody
 *
 * SOURCE CONTROL: $Id: platform_config.h,v 1.1 2008/01/03 06:55:12 tianwq Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2002 RDA Microelectronics.
 *     All rights reserved.
 *
 ***********************************************************************/

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

/********************************** Base Types ***************************************************/
typedef unsigned char   u_int8;       /* unsigned integer,  8 bits long */
typedef unsigned short  u_int16;     /* unsigned integer, 16 bits long */
typedef unsigned int    u_int32;       /* unsigned integer, 32 bits long */
#ifndef __CS_TYPE_H__
typedef unsigned char boolean;   /* boolean, true=1, false=0 */
#endif

typedef signed char     s_int8;         /* unsigned integer,  8 bits long */
typedef signed short    s_int16;       /* unsigned integer,  8 bits long */
typedef signed long     s_int32;        /* unsigned integer,  8 bits long */

typedef unsigned int    u_int;

/****************** Features ***************/
#define RDABT_SSP_SUPPORT                   1
#define RDABT_EIR_SUPPORT                   1 
#define RDABT_LE_SUPPORT                    0
#define RDABT_DHKEY_SUPPORT                 0
/* for bt 3.0 */
#define AMP_MANAGER_SUPPORT                 0
#define PAL_IN_HOST                         0

#define RDABT_THREAD_EVENT_SUPPORT          0        // event for call bt profile from other thread
#define COMBINED_HOST                       0

#define MEMORY_POOL_SUPPORT                 0        // stack internal memory manager

/******************* Manager ****************/
/* The local and remote names are truncated to the defines below - see GAP spec. */
#define MGR_MAX_REMOTE_NAME_LEN             16
#define MGR_MAX_LOCAL_NAME_LEN              48

/****************** L2CAP *******************/
#define L2CAP_CONFIG_VALUES_BELOW_HCI       1
#define L2CAP_FRAGMENT_ACL_SUPPORT          1
#define L2CAP_RETRANS_SUPPORT               0

/*************** Test and Debug *************/
/* Debug option */
#ifdef WIN32
#define pDEBUG                              1
#else
#ifdef DEBUG
#define pDEBUG                              1
#else
#define pDEBUG                              0
#endif
#endif
#define NO_SEC_CHECK

#if pDEBUG
#define pDEBUGLEVEL                         pLOGNOTICE
#define pDEBUGLAYERS                        pLOGALWAYS
#endif

/*************************************** platform sepcific defines ************************************************/
#ifdef APIDECL1
#undef APIDECL1
#endif

#ifdef APIDECL2
#undef APIDECL2
#endif

#define APIDECL1
#define APIDECL2

#define PLATFORMSTRING "RDA5850"

/* define endian of platform */

#define BTHOST_BIGENDIAN 0

#endif /* PLATFORM_CONFIG_H */

