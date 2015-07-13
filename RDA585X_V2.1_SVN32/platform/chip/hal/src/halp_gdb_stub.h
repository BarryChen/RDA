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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_gdb_stub.h $ //
//    $Author: huazeng $                                                        // 
//    $Date: 2011-11-03 20:23:45 +0800 (星期四, 03 十一月 2011) $                     //   
//    $Revision: 11457 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_gdb_stub.h
/// Defines used by GDB stub. WARNING: this file is included by an assembly
/// file.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef  _HALP_GDB_STUB_H_
#define  _HALP_GDB_STUB_H_


#define HAL_GDB_REGNUM           38
//  all register + command and parameter for command 
#define HAL_GDB_REGNUM_XTD       (HAL_GDB_REGNUM+3)
#define HAL_GDB_CONTEXT_SIZE     (HAL_GDB_REGNUM_XTD*4)

#define HAL_GDB_CMD_OFF          HAL_GDB_REGNUM
#define HAL_GDB_CMDPARAM_OFF     (HAL_GDB_REGNUM + 1)
#define HAL_GDB_CMDPARAM2_OFF    (HAL_GDB_REGNUM + 2)

//  command between jade cpus and debug server 
#define HAL_GDB_CMD_DONE         0x00

#define HAL_GDB_CMD_FLUSH_CACHE     0x01
#define HAL_GDB_CMD_STEP            0x02
#define HAL_GDB_CMD_CONTINUE        0x04
#define HAL_GDB_SET_BREAKPOINT      0x05
#define HAL_GDB_UNSET_BREAKPOINT    0x06



#define HAL_GDB_CMD_BREAKPOINT   0x10
#define HAL_GDB_CMD_PRINT        0x20


#ifndef CT_ASM

/// Function used to flush both caches.
PUBLIC VOID hal_GdbFlushCache(VOID);

/// Function used to flush the data cache.
PUBLIC VOID hal_GdbFlushCacheData(VOID);
PROTECTED VOID hal_DbgGdbHstSendEvent(UINT32 ch);

PROTECTED BOOL hal_GdbIsInGdbLoop(VOID);
#endif // !CT_ASM


#endif //  _HALP_GDB_STUB_H_ 
