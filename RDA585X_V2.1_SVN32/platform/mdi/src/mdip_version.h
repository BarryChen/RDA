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

#ifndef _MDI_VERSION_H_
#define _MDI_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define MDI_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef MDI_VERSION_NUMBER
#define MDI_VERSION_NUMBER                       (0)
#endif

#ifndef MDI_VERSION_DATE
#define MDI_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef MDI_VERSION_STRING
#define MDI_VERSION_STRING                       "MDI version string not defined"
#endif

#ifndef MDI_VERSION_STRING_WITH_BRANCH
#define MDI_VERSION_STRING_WITH_BRANCH           MDI_VERSION_STRING " Branch: " "none"
#endif

#define MDI_VERSION_STRUCT                       {MDI_VERSION_REVISION, \
                                                  MDI_VERSION_NUMBER, \
                                                  MDI_VERSION_DATE, \
                                                  MDI_VERSION_STRING_WITH_BRANCH}

#endif // _MDI_VERSION_H_
