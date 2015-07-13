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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/boot/8809/src/boot_romstring.c $
//  $Author: huazeng $
//  $Date: 2012-01-11 15:37:31 +0800 (星期三, 11 一月 2012) $
//  $Revision: 13156 $
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_romstring.h
/// That file describes the Dark Minds Behind This All.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define STRINGIFY_VALUE(s) STRINGIFY(s)
#define STRINGIFY(s) #s 

const char boot_romCsString[] =
    STRINGIFY_VALUE(CT_ASIC_CAPD) "\n"
    "Copyright Coolsand Technologies 2002-2012\n";


const char boot_romDT[] =
    "Coolsand\n"
    "RDA\n";



