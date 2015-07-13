////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL$ //
//	$Author$                                                        // 
//	$Date$                     //   
//	$Revision$                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file apfs.h                                                               //
/// That file implementes the APFS service.                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef APFS_H
#define APFS_H


#include "cs_types.h"

#include "fs.h"
#include "mci.h"
 
extern PUBLIC MCI_ERR_T apfs_GetFileInformation (
                        CONST INT32 FileHander,
                         AudDesInfoStruct  * CONST DecInfo,
                        CONST mci_type_enum FileType   );



extern PUBLIC MCI_ERR_T apfs_GetDurationTime(
							INT32 FileHander, 
							mci_type_enum FileType,
							INT32 BeginPlayProgress,
							INT32 OffsetPlayProgress,
							MCI_ProgressInf* PlayInformation);

#endif


