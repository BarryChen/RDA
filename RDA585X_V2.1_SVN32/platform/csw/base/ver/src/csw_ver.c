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
//
//  $HeadURL: http://svn.rdamicro.com/svn/developing/Sources/csw/branches/Total_BJ/base/ver/src/csw_ver.c $
//  $Author: mathieu $
//  $Date: 2008/07/21 10:15:54 $
//  $Revision: 19508 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file csw_ver.c
/// Implement the functions returning the 
//
////////////////////////////////////////////////////////////////////////////////


#include "chip_id.h"
#include "cs_types.h"
#include "csw_ver.h"

#include "hal_map_engine.h"



// =============================================================================
// GLOBAL VARIABLES
// =============================================================================



// =============================================================================
// g_cswMmiMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of the MMI, when the MMI had filled it with the
/// function csw_VerSetMmiVersion().
// =============================================================================
PROTECTED HAL_MAP_VERSION_T g_cswMmiMapVersion;



// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
//csw_VerGetRevision
// -----------------------------------------------------------------------------
/// Return the revision of a given module.
/// 
/// @param  moduleId Id the module.
/// @return The SVN revision of that version of the module.
// =============================================================================
PUBLIC UINT32 platform_VerGetRevision(void)
{
    return SVN_REVISION;
}


// =============================================================================
//csw_VerGetNumber
// -----------------------------------------------------------------------------
/// Return the version number of a given module.
/// 
/// @param  moduleId Id the module.
/// @return The version number of that version of the module.
// =============================================================================
PUBLIC UINT32 platform_VerGetNumber(void)
{
    return 0x0201;  // 2.1
}


// =============================================================================
//csw_VerGetDate
// -----------------------------------------------------------------------------
/// Return the date of build of a given module.
/// The format is 0xYYYYMMDD.
/// 
/// @param  moduleId Id the module.
/// @return The build date of that version of the module.
// =============================================================================
PUBLIC UINT32 platform_VerGetDate(void)
{
    return BUILD_DATE;
}



