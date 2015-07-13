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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/pmd/include/pmd_calib.h $ //
//  $Author: huazeng $                                                        // 
//  $Date: 2010-08-23 13:47:34 +0800 (星期一, 23 八月 2010) $                     //   
//  $Revision: 1532 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file pmd_calib.h  
/// This file contains the PMD driver calibration API.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


/// @addgroup pmd 
/// @{


#ifndef _PMD_CALIB_H_
#define _PMD_CALIB_H_

#include "cs_types.h"
#include "calib_m.h"



// ============================================================================
//  MACROS
// ============================================================================


// =============================================================================
//  TYPES
// =============================================================================


// ============================================================================
//  FUNCTIONS
// ============================================================================


// ============================================================================
// pmd_CalibStartCustom
// ----------------------------------------------------------------------------
/// Runs the custom calibration process of the power management.
/// For example, in Opal, this is used for the GPADC calibration.
/// The function must do the job, possibly using the "command" parameter,
/// and then put the result(s) in "calibStruct" and return a nice status.
///
/// @command Value sent by the calibration script to start the custom calib.
/// It is different from zero and can be used for customization purpose.
///
/// @pmdCalibStruct Calibration structure to fill with the result of
/// the calibration.
///
/// @return Status of the custom calibration. This value is send by the
/// calibration stub to the calibration script. It can be used for
/// customization purpose.
// ============================================================================
PUBLIC UINT32 pmd_CalibStartCustom(UINT32 command,
                                   CALIB_PMD_T* pmdCalibStruct);


// ============================================================================
// pmd_CalibGetDefault
// ----------------------------------------------------------------------------
/// Returns the default values of the PMD calibration.
/// Used by the Calib module to get the default of each PMD implementation.
///
/// @return Default values for the PMD calib parameters.
// ============================================================================
PUBLIC CONST CALIB_PMD_T* pmd_CalibGetDefault(VOID);


/// @} // <-- End of pmd group


#endif //_PMD_CALIB_H_


