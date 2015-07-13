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

#ifndef _I2CGPIO_VERSION_H_
#define _I2CGPIO_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define I2CGPIO_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef I2CGPIO_VERSION_NUMBER
#define I2CGPIO_VERSION_NUMBER                       (0)
#endif

#ifndef I2CGPIO_VERSION_DATE
#define I2CGPIO_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef I2CGPIO_VERSION_STRING
#define I2CGPIO_VERSION_STRING                       "I2CGPIO version string not defined"
#endif

#ifndef I2CGPIO_VERSION_STRING_WITH_BRANCH
#define I2CGPIO_VERSION_STRING_WITH_BRANCH           I2CGPIO_VERSION_STRING " Branch: " "none"
#endif

#define I2CGPIO_VERSION_STRUCT                       {I2CGPIO_VERSION_REVISION, \
                                                  I2CGPIO_VERSION_NUMBER, \
                                                  I2CGPIO_VERSION_DATE, \
                                                  I2CGPIO_VERSION_STRING_WITH_BRANCH}

#endif // _I2CGPIO_VERSION_H_
