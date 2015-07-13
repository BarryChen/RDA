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
//  $HeadURL: http://subversion-server/svn/developing/modem2G/trunk/platform/edrv/lcdd/include/lcdd_m.h $ //
//	$Author: victor $                                                         // 
//	$Date: 2008-06-25 15:52:02 +0200 (Wed, 25 Jun 2008) $                     //   
//	$Revision: 14428 $                                                        //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //

#if (CSW_EXTENDED_API == 1)

#include "mci.h"

// Init
BOOL    MCI_TaskInit(VOID){ return TRUE; }

// Video Record
UINT32  MCI_VideoRecordAdjustSetting(INT32 adjustItem, INT32 value) { return 0; }
UINT32  MCI_VideoRecordPause(VOID){ return 0; }
UINT32  MCI_VideoRecordPreviewStart(MMC_VDOREC_SETTING_STRUCT *previewPara){ return 0; }
UINT32  MCI_VideoRecordPreviewStop(VOID){ return 0; }
UINT32  MCI_VideoRecordResume(VOID){ return 0; }
UINT32  MCI_VideoRecordStart(char *pfilename){ return 0; }
UINT32  MCI_VideoRecordStop(VOID){ return 0; }
UINT32  MCI_DisplayVideoInterface (VOID){ return 0; }
VOID vdorec_cancel_saveFile(VOID){}

// Camera
VOID set_mmc_camera_preview(){}
VOID set_multicaputure(INT32 i){}

#endif // CSW_EXTENDED_API


