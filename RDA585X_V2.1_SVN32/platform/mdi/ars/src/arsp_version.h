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

#ifndef _ARS_VERSION_H_
#define _ARS_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define ARS_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef ARS_VERSION_NUMBER
#define ARS_VERSION_NUMBER                       (0)
#endif

#ifndef ARS_VERSION_DATE
#define ARS_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef ARS_VERSION_STRING
#define ARS_VERSION_STRING                       "ARS version string not defined"
#endif

#ifndef ARS_VERSION_STRING_WITH_BRANCH
#define ARS_VERSION_STRING_WITH_BRANCH           ARS_VERSION_STRING " Branch: " "none"
#endif

#define ARS_VERSION_STRUCT                       {ARS_VERSION_REVISION, \
                                                  ARS_VERSION_NUMBER, \
                                                  ARS_VERSION_DATE, \
                                                  ARS_VERSION_STRING_WITH_BRANCH}

#endif // _ARS_VERSION_H_
