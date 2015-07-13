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

#ifndef _LCDD_VERSION_H_
#define _LCDD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define LCDD_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef LCDD_VERSION_NUMBER
#define LCDD_VERSION_NUMBER                       (0)
#endif

#ifndef LCDD_VERSION_DATE
#define LCDD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef LCDD_VERSION_STRING
#define LCDD_VERSION_STRING                       "LCDD version string not defined"
#endif

#ifndef LCDD_VERSION_STRING_WITH_BRANCH
#define LCDD_VERSION_STRING_WITH_BRANCH           LCDD_VERSION_STRING " Branch: " "none"
#endif

#define LCDD_VERSION_STRUCT                       {LCDD_VERSION_REVISION, \
                                                  LCDD_VERSION_NUMBER, \
                                                  LCDD_VERSION_DATE, \
                                                  LCDD_VERSION_STRING_WITH_BRANCH}

#endif // _LCDD_VERSION_H_
