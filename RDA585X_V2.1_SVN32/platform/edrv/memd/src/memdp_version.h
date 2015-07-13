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
//    THIS FILE WAS GENERATED AUTOMATICALLY BY THE MAKE PROCESS.
//                                                                              
//                       !!! PLEASE DO NOT EDIT !!!                             
//                                                                              
//==============================================================================

#ifndef _MEMD_VERSION_H_
#define _MEMD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define MEMD_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef MEMD_VERSION_NUMBER
#define MEMD_VERSION_NUMBER                       (0)
#endif

#ifndef MEMD_VERSION_DATE
#define MEMD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef MEMD_VERSION_STRING
#define MEMD_VERSION_STRING                       "MEMD version string not defined"
#endif

#ifndef MEMD_VERSION_STRING_WITH_BRANCH
#define MEMD_VERSION_STRING_WITH_BRANCH           MEMD_VERSION_STRING " Branch: " "none"
#endif

#define MEMD_VERSION_STRUCT                       {MEMD_VERSION_REVISION, \
                                                  MEMD_VERSION_NUMBER, \
                                                  MEMD_VERSION_DATE, \
                                                  MEMD_VERSION_STRING_WITH_BRANCH}

#endif // _MEMD_VERSION_H_
