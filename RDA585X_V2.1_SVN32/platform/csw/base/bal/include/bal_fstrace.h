////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2008, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
//  $HeadURL$
//  $Author$
//  $Date$
//  $Revision$
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file bal_fstrace.h
///
/// fs Trace service
//
////////////////////////////////////////////////////////////////////////////////

// marco
#define SECOND        * HAL_TICK1S
#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR
#define FS_TRACE_TIMER_CNT (1 SECOND)

 // (0xabcd| SXS_TIMER_EVT_ID)
#define FSTRACE_TIMER1 PRV_CFW_TRACE_TO_FLASH_TIMER_ID
#define MAX_TRACE_LEN 512

typedef struct
{
  u32 eventId;
  u32 nParam1;
//  u32 nParam2;//for delete nParam2
//  u32 nParam3;//for delete nParam3
} fsTrace_event_t;


// declaration of function

// =============================================================================
// bal_fsTraceProcess
// -----------------------------------------------------------------------------
/// Read a bunch of data from the debug remote buffer and output to tFlash card.
// =============================================================================
PUBLIC VOID bal_fsTraceProcess(VOID);

// =============================================================================
// bal_fstraceStart
// -----------------------------------------------------------------------------
/// start tFlash trace recorsing
// =============================================================================
PUBLIC VOID  bal_fstraceStart(VOID);

// =============================================================================
// bal_fstraceStop
// -----------------------------------------------------------------------------
/// stop tFlash trace recorsing
// =============================================================================
PUBLIC VOID  bal_fstraceStop(VOID);


