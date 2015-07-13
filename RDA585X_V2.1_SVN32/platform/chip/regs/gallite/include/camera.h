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
//    THIS FILE WAS GENERATED FROM ITS CORRESPONDING XML VERSION WITH COOLXML.  
//                                                                              
//                       !!! PLEASE DO NOT EDIT !!!                             
//                                                                              
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/regs/gallite/include/camera.h $                                                                   
//  $Author: huazeng $                                                                    
//  $Date: 2011-12-02 15:56:10 +0800 (周五, 02 十二月 2011) $                                                                      
//  $Revision: 12275 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'camera'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define FIFORAM_SIZE                             (80)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CAMERA_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_CAMERA_BASE             0x01A18000

typedef volatile struct
{
    REG32       CTRL;                         //0x00000000
    REG32       STATUS;                       //0x00000004
    REG32       DATA;                         //0x00000008
    REG32       IRQ_MASK;                     //0x0000000C
    REG32       IRQ_CLEAR;                    //0x00000010
    REG32       IRQ_CAUSE;                    //0x00000014
    REG32       CMD_SET;                      //0x00000018
    REG32       CMD_CLR;                      //0x0000001C
    REG32       DSTWINCOL;                    //0x00000020
    REG32       DSTWINROW;                    //0x00000024
    REG32 Reserved_00000028[118];               //0x00000028
    struct
    {
        REG32                      RAMData;                      //0x00000200
    } FIFORAM[FIFORAM_SIZE];
} HWP_CAMERA_T;

#define hwp_camera                  ((HWP_CAMERA_T*) KSEG1(REG_CAMERA_BASE))


//CTRL
#define CAMERA_ENABLE               (1<<0)
#define CAMERA_ENABLE_ENABLE        (1<<0)
#define CAMERA_ENABLE_DISABLE       (0<<0)
#define CAMERA_1_BUFENABLE               (1<<1)
#define CAMERA_1_BUFENABLE_ENABLE        (1<<1)
#define CAMERA_1_BUFENABLE_DISABLE       (0<<1)
#define CAMERA_DATAFORMAT(n)        (((n)&3)<<4)
#define CAMERA_DATAFORMAT_RGB565    (0<<4)
#define CAMERA_DATAFORMAT_YUV422    (1<<4)
#define CAMERA_DATAFORMAT_JPEG      (2<<4)
#define CAMERA_DATAFORMAT_RESERVE   (3<<4)
#define CAMERA_RESET_POL            (1<<8)
#define CAMERA_RESET_POL_INVERT     (1<<8)
#define CAMERA_RESET_POL_NORMAL     (0<<8)
#define CAMERA_PWDN_POL             (1<<9)
#define CAMERA_PWDN_POL_INVERT      (1<<9)
#define CAMERA_PWDN_POL_NORMAL      (0<<9)
#define CAMERA_VSYNC_POL            (1<<10)
#define CAMERA_VSYNC_POL_INVERT     (1<<10)
#define CAMERA_VSYNC_POL_NORMAL     (0<<10)
#define CAMERA_HREF_POL             (1<<11)
#define CAMERA_HREF_POL_INVERT      (1<<11)
#define CAMERA_HREF_POL_NORMAL      (0<<11)
#define CAMERA_PIXCLK_POL           (1<<12)
#define CAMERA_PIXCLK_POL_INVERT    (1<<12)
#define CAMERA_PIXCLK_POL_NORMAL    (0<<12)
#define CAMERA_VSYNC_DROP           (1<<14)
#define CAMERA_VSYNC_DROP_DROP      (1<<14)
#define CAMERA_VSYNC_DROP_NORMAL    (0<<14)
#define CAMERA_DECIMFRM(n)          (((n)&3)<<16)
#define CAMERA_DECIMFRM_ORIGINAL    (0<<16)
#define CAMERA_DECIMFRM_DIV_2       (1<<16)
#define CAMERA_DECIMFRM_DIV_3       (2<<16)
#define CAMERA_DECIMFRM_DIV_4       (3<<16)
#define CAMERA_DECIMCOL(n)          (((n)&3)<<18)
#define CAMERA_DECIMCOL_ORIGINAL    (0<<18)
#define CAMERA_DECIMCOL_DIV_2       (1<<18)
#define CAMERA_DECIMCOL_DIV_3       (2<<18)
#define CAMERA_DECIMCOL_DIV_4       (3<<18)
#define CAMERA_DECIMROW(n)          (((n)&3)<<20)
#define CAMERA_DECIMROW_ORIGINAL    (0<<20)
#define CAMERA_DECIMROW_DIV_2       (1<<20)
#define CAMERA_DECIMROW_DIV_3       (2<<20)
#define CAMERA_DECIMROW_DIV_4       (3<<20)
#define CAMERA_REORDER(n)           (((n)&7)<<24)
#define CAMERA_CROPEN               (1<<28)
#define CAMERA_CROPEN_ENABLE        (1<<28)
#define CAMERA_CROPEN_DISABLE       (0<<28)
#define CAMERA_BIST_MODE            (1<<30)
#define CAMERA_BIST_MODE_BIST       (1<<30)
#define CAMERA_BIST_MODE_NORMAL     (0<<30)
#define CAMERA_TEST                 (1<<31)
#define CAMERA_TEST_TEST            (1<<31)
#define CAMERA_TEST_NORMAL          (0<<31)

//STATUS
#define CAMERA_OVFL                 (1<<0)
#define CAMERA_VSYNC_R              (1<<1)
#define CAMERA_VSYNC_F              (1<<2)
#define CAMERA_DMA_DONE             (1<<3)
#define CAMERA_FIFO_EMPTY           (1<<4)

//DATA
#define CAMERA_RX_DATA(n)           (((n)&0xFFFFFFFF)<<0)

//IRQ_MASK
//#define CAMERA_OVFL               (1<<0)
//#define CAMERA_VSYNC_R            (1<<1)
//#define CAMERA_VSYNC_F            (1<<2)
//#define CAMERA_DMA_DONE           (1<<3)

//IRQ_CLEAR
//#define CAMERA_OVFL               (1<<0)
//#define CAMERA_VSYNC_R            (1<<1)
//#define CAMERA_VSYNC_F            (1<<2)
//#define CAMERA_DMA_DONE           (1<<3)

//IRQ_CAUSE
//#define CAMERA_OVFL               (1<<0)
//#define CAMERA_VSYNC_R            (1<<1)
//#define CAMERA_VSYNC_F            (1<<2)
//#define CAMERA_DMA_DONE           (1<<3)

//CMD_SET
#define CAMERA_PWDN                 (1<<0)
#define CAMERA_RESET                (1<<4)
#define CAMERA_FIFO_RESET           (1<<8)

//CMD_CLR
//#define CAMERA_PWDN               (1<<0)
//#define CAMERA_RESET              (1<<4)

//DSTWINCOL
#define CAMERA_DSTWINCOLSTART(n)    (((n)&0xFFF)<<0)
#define CAMERA_DSTWINCOLEND(n)      (((n)&0xFFF)<<16)

//DSTWINROW
#define CAMERA_DSTWINROWSTART(n)    (((n)&0xFFF)<<0)
#define CAMERA_DSTWINROWEND(n)      (((n)&0xFFF)<<16)

//RAMData
#define CAMERA_DATA(n)              (((n)&0xFFFFFFFF)<<0)
#define CAMERA_DATA_MASK            (0xFFFFFFFF<<0)
#define CAMERA_DATA_SHIFT           (0)

#endif

