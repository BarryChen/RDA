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

#ifndef _HAL_VERSION_H_
#define _HAL_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define HAL_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef HAL_VERSION_NUMBER
#define HAL_VERSION_NUMBER                       (0)
#endif

#ifndef HAL_VERSION_DATE
#define HAL_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef HAL_VERSION_STRING
#define HAL_VERSION_STRING                       "HAL version string not defined"
#endif

#ifndef HAL_VERSION_STRING_WITH_BRANCH
#define HAL_VERSION_STRING_WITH_BRANCH           HAL_VERSION_STRING " Branch: " "none"
#endif

#define HAL_VERSION_STRUCT                       {HAL_VERSION_REVISION, \
                                                  HAL_VERSION_NUMBER, \
                                                  HAL_VERSION_DATE, \
                                                  HAL_VERSION_STRING_WITH_BRANCH}

#endif // _HAL_VERSION_H_
