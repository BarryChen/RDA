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

#ifndef _BOOT_VERSION_H_
#define _BOOT_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define BOOT_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef BOOT_VERSION_NUMBER
#define BOOT_VERSION_NUMBER                       (0)
#endif

#ifndef BOOT_VERSION_DATE
#define BOOT_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef BOOT_VERSION_STRING
#define BOOT_VERSION_STRING                       "BOOT version string not defined"
#endif

#ifndef BOOT_VERSION_STRING_WITH_BRANCH
#define BOOT_VERSION_STRING_WITH_BRANCH           BOOT_VERSION_STRING " Branch: " "none"
#endif

#define BOOT_VERSION_STRUCT                       {BOOT_VERSION_REVISION, \
                                                  BOOT_VERSION_NUMBER, \
                                                  BOOT_VERSION_DATE, \
                                                  BOOT_VERSION_STRING_WITH_BRANCH}

#endif // _BOOT_VERSION_H_
