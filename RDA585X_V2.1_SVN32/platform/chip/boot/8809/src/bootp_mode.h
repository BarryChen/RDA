////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/8809/src/bootp_mode.h $ //
//	$Author: huazeng $                                                        // 
//	$Date: 2011-11-30 18:24:46 +0800 (星期三, 30 十一月 2011) $                     //   
//	$Revision: 12233 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_boot_asm.h                                                     //
/// That file provides defines used by the assembly boot code.                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _HALP_BOOT_MODE_H_
#define _HALP_BOOT_MODE_H_


#define BOOT_MODE_STD                   0

#define BOOT_MODE_NO_AUTO_PU          (1<<0)
#define BOOT_MODE_FORCE_MONITOR       (1<<1)
#define BOOT_MODE_UART_MONITOR_ENABLE (1<<2)
#define BOOT_MODE_USB_MONITOR_DISABLE (1<<3)
#define BOOT_MODE_NAND_FLASH          (1<<4)
#define BOOT_MODE_ADMUX_OR_NAND_PAGE_2K (1<<5)


#endif // _HALP_BOOT_MODE_H_


