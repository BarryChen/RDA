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

#ifndef _AUD_VERSION_H_
#define _AUD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define AUD_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef AUD_VERSION_NUMBER
#define AUD_VERSION_NUMBER                       (0)
#endif

#ifndef AUD_VERSION_DATE
#define AUD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef AUD_VERSION_STRING
#define AUD_VERSION_STRING                       "AUD version string not defined"
#endif

#ifndef AUD_VERSION_STRING_WITH_BRANCH
#define AUD_VERSION_STRING_WITH_BRANCH           AUD_VERSION_STRING " Branch: " "none"
#endif

#define AUD_VERSION_STRUCT                       {AUD_VERSION_REVISION, \
                                                  AUD_VERSION_NUMBER, \
                                                  AUD_VERSION_DATE, \
                                                  AUD_VERSION_STRING_WITH_BRANCH}

#endif // _AUD_VERSION_H_
