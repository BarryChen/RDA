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

#ifndef _BTD_VERSION_H_
#define _BTD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define BTD_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef BTD_VERSION_NUMBER
#define BTD_VERSION_NUMBER                       (0)
#endif

#ifndef BTD_VERSION_DATE
#define BTD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef BTD_VERSION_STRING
#define BTD_VERSION_STRING                       "BTD version string not defined"
#endif

#ifndef BTD_VERSION_STRING_WITH_BRANCH
#define BTD_VERSION_STRING_WITH_BRANCH           BTD_VERSION_STRING " Branch: " "none"
#endif

#define BTD_VERSION_STRUCT                       {BTD_VERSION_REVISION, \
                                                  BTD_VERSION_NUMBER, \
                                                  BTD_VERSION_DATE, \
                                                  BTD_VERSION_STRING_WITH_BRANCH}

#endif // _BTD_VERSION_H_
