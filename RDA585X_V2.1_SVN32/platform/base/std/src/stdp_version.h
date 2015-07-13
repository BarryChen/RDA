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

#ifndef _STD_VERSION_H_
#define _STD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define STD_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef STD_VERSION_NUMBER
#define STD_VERSION_NUMBER                       (0)
#endif

#ifndef STD_VERSION_DATE
#define STD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef STD_VERSION_STRING
#define STD_VERSION_STRING                       "STD version string not defined"
#endif

#ifndef STD_VERSION_STRING_WITH_BRANCH
#define STD_VERSION_STRING_WITH_BRANCH           STD_VERSION_STRING " Branch: " "none"
#endif

#define STD_VERSION_STRUCT                       {STD_VERSION_REVISION, \
                                                  STD_VERSION_NUMBER, \
                                                  STD_VERSION_DATE, \
                                                  STD_VERSION_STRING_WITH_BRANCH}

#endif // _STD_VERSION_H_
