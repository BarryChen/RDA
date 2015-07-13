//==============================================================================
//                                                                              
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.              
//                            All Rights Reserved                               
//                                                                              
//      This source code is the property of Coolsand Technologies and is        
//      confidential.  Any  modification, distribution,  reproduction or        
//      exploitation  of  any content of this file is totally forbidden,        
//      except  with the  written permission  of  Coolsand Technologies.        
//                                                                              
//==============================================================================
//                                                                              
//    THIS FILE WAS GENERATED FROM ITS CORRESPONDING XML VERSION WITH COOLXML.  
//                                                                              
//                       !!! PLEASE DO NOT EDIT !!!                             
//                                                                              
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/base/common/include/baseband_defs.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:26:37 +0800 (星期三, 07 七月 2010) $                                                                      
//  $Revision: 268 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _BASEBAND_DEFS_H_
#define _BASEBAND_DEFS_H_



// =============================================================================
//  MACROS
// =============================================================================
/// -------------------------------------------------------------------- Timings
/// and Sizes These timings are shared by different SW modules and are derived from
/// baseband signal processing capabilities and needs --------------------------------------------------------------------
/// Values in symbols
#define BB_RX_SCH_MARGIN                         (12)
#define BB_RX_NBURST_MARGIN                      (4)
/// One Qb is dropped
#define BB_BURST_TOTAL_SIZE                      (156)
/// Used for EQU_HBURST_MODE'
#define BB_BURST_HALF_SIZE                       (96)
#define BB_BURST_ACTIVE_SIZE                     (148)
#define BB_SCH_TOTAL_SIZE                        ((BB_BURST_ACTIVE_SIZE + 2*BB_RX_SCH_MARGIN))

// =============================================================================
//  TYPES
// =============================================================================



/**
@file
Defines related to the BaseBand signal processing
*/


#endif

