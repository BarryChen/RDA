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


#if (CSW_EXTENDED_API_LCD == 1)

#include "mci_lcd.h"

// This is bad - this is coming from the MMI
// and the platform should be independant of the MMI
EXPORT UINT32 get_lcd_frame_buffer_address(VOID);

// ============================================================================
// Init_DI
// ----------------------------------------------------------------------------
// ============================================================================
VOID Init_DI(UINT32 background, VOID** unused)
{
    mci_LcdInit(background);
}

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
VOID BlockWrite_DI(UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
#ifdef __PRJ_WITH_SPILCD__
    mci_LcdBlockWrite((UINT16*)(get_lcd_frame_buffer_address() | 0xa0000000),startx,starty,endx,endy);
#else
    mci_LcdBlockWrite((UINT16*)get_lcd_frame_buffer_address(),startx,starty,endx,endy);
#endif
}

UINT8 partial_display_align_DI(VOID)
{
    return 0;
}

VOID lcd_update_image_DI(UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height)
{
}

VOID lcd_chipinit_DI(VOID)
{
}

#endif // CSW_EXTENDED_API_LCD


