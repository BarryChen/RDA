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
/// @file                                                                     //
/// That file Implements the functions to register the module.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"

//#include "mdi_m.h"

#include "hal_map_engine.h"

#include "mdi_map.h"


//# define HAL_MAP_ID_MDI HAL_MAP_ID_AUD
// =============================================================================
//  MACROS
// =============================================================================

#define MDI_BEIJING_VERSION_STRING "MDI module in branch of Total_integration"
#include "mdip_version.h"



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// g_mdiMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of the module.
/// The macro used is generated by the make process.
// =============================================================================
PROTECTED HAL_MAP_VERSION_T g_mdiMapVersion = MDI_VERSION_STRUCT;



// =============================================================================
// g_mdiMapAccess
// -----------------------------------------------------------------------------
/// Contains the addresses of the structures of the module that will be
/// accessible from a remote PC (e.g. through CoolWatcher).
// =============================================================================
PROTECTED MDI_MAP_ACCESS_T g_mdiMapAccess =
{
    // Put your initialization here.
};



// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// mdi_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID mdi_RegisterYourself(VOID)
{
	g_mdiMapVersion.string=MDI_BEIJING_VERSION_STRING;
    	hal_MapEngineRegisterModule(HAL_MAP_ID_MDI, &g_mdiMapVersion, &g_mdiMapAccess);
}
