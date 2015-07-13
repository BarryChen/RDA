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

#ifndef _SX_VERSION_H_
#define _SX_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define SX_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef SX_VERSION_NUMBER
#define SX_VERSION_NUMBER                       (0)
#endif

#ifndef SX_VERSION_DATE
#define SX_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef SX_VERSION_STRING
#define SX_VERSION_STRING                       "SX version string not defined"
#endif

#ifndef SX_VERSION_STRING_WITH_BRANCH
#define SX_VERSION_STRING_WITH_BRANCH           SX_VERSION_STRING " Branch: " "none"
#endif

#define SX_VERSION_STRUCT                       {SX_VERSION_REVISION, \
                                                  SX_VERSION_NUMBER, \
                                                  SX_VERSION_DATE, \
                                                  SX_VERSION_STRING_WITH_BRANCH}

#endif // _SX_VERSION_H_
