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

#ifndef _PMD_VERSION_H_
#define _PMD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define PMD_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef PMD_VERSION_NUMBER
#define PMD_VERSION_NUMBER                       (0)
#endif

#ifndef PMD_VERSION_DATE
#define PMD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef PMD_VERSION_STRING
#define PMD_VERSION_STRING                       "PMD version string not defined"
#endif

#ifndef PMD_VERSION_STRING_WITH_BRANCH
#define PMD_VERSION_STRING_WITH_BRANCH           PMD_VERSION_STRING " Branch: " "none"
#endif

#define PMD_VERSION_STRUCT                       {PMD_VERSION_REVISION, \
                                                  PMD_VERSION_NUMBER, \
                                                  PMD_VERSION_DATE, \
                                                  PMD_VERSION_STRING_WITH_BRANCH}

#endif // _PMD_VERSION_H_
