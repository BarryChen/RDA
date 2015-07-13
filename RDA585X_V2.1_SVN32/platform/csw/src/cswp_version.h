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

#ifndef _CSW_VERSION_H_
#define _CSW_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define CSW_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef CSW_VERSION_NUMBER
#define CSW_VERSION_NUMBER                       (0)
#endif

#ifndef CSW_VERSION_DATE
#define CSW_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef CSW_VERSION_STRING
#define CSW_VERSION_STRING                       "CSW version string not defined"
#endif

#ifndef CSW_VERSION_STRING_WITH_BRANCH
#define CSW_VERSION_STRING_WITH_BRANCH           CSW_VERSION_STRING " Branch: " "none"
#endif

#define CSW_VERSION_STRUCT                       {CSW_VERSION_REVISION, \
                                                  CSW_VERSION_NUMBER, \
                                                  CSW_VERSION_DATE, \
                                                  CSW_VERSION_STRING_WITH_BRANCH}

#endif // _CSW_VERSION_H_
