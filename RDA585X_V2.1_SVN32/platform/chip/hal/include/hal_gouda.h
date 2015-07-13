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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_gouda.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2012-07-19 19:34:01 +0800 (星期四, 19 七月 2012) $                     //   
//    $Revision: 16402 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_gouda.h
///
/// This file decribes the GOUDA driver API.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#ifndef _HAL_GOUDA_H_
#define _HAL_GOUDA_H_

#include "cs_types.h"
#include "hal_error.h"



/// @defgroup gouda HAL GOUDA Driver
/// This document describes the characteristics of the GOUDA module and how 
/// to use them via its Hardware Abstraction Layer API.
///
/// The GOUDA is a dedicated controller that is capable of driving
/// a parallel LCD module with a multi layer engine.
///
/// @{


// =============================================================================
// MACROS
// =============================================================================

/// Number of different set of timings available for the GOUDA LCD
/// configuration. This number should be equal to the number
/// of different system frequencies, so that the GOUDA LCD interface
/// can be configured optimaly for every system frequency.
#if (CHIP_HAS_ASYNC_TCU)
#define HAL_GOUDA_LCD_TIMINGS_QTY   12
#else
#define HAL_GOUDA_LCD_TIMINGS_QTY   8
#endif


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// HAL_GOUDA_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type of the user handler function called when a GOUDA related interrupt
/// is triggered.
// =============================================================================
typedef VOID (*HAL_GOUDA_IRQ_HANDLER_T)(VOID);


// =============================================================================
// HAL_GOUDA_SPI_LINE_T
// -----------------------------------------------------------------------------
/// Define the type of the SLCD SPI line.
// =============================================================================
typedef enum {
    HAL_GOUDA_SPI_LINE_4 = 0,
    HAL_GOUDA_SPI_LINE_3 = 1,
    HAL_GOUDA_SPI_LINE_4_START_BYTE = 2,
} HAL_GOUDA_SPI_LINE_T;


// =============================================================================
// HAL_GOUDA_LCD_TIMINGS_T
// -----------------------------------------------------------------------------
/// This types describes the timing used by the GOUDA LCD interface.
/// An array os those will be needed to describe the GOUDA LCD
/// timings for each system frequency.
// =============================================================================
typedef union
{
    struct
    {
        /// Address setup time (RS to WR, RS to RD) in clock number
        UINT32 tas:3;
        UINT32 :1;
        /// Adress hold time in clock number
        UINT32 tah:3;
        UINT32 :1;
        /// Control  pulse width low, in clock number
        UINT32 pwl:6;
        UINT32 :2;
        /// Control pulse width high, in clock number
        UINT32 pwh:6;
        UINT32 :10;
    };

    /// Use this 'reg' field of the union
    /// to access this timing configuration as
    /// a 32 bits value than can be written to
    /// a register.
    UINT32 reg;
} HAL_GOUDA_LCD_TIMINGS_T;


// =============================================================================
// HAL_GOUDA_LCD_CS_T
// -----------------------------------------------------------------------------
/// Defines the Chip Select use to select the LCD screen connected to the 
/// GOUDA_LCD controller.
// =============================================================================
typedef enum
{
    HAL_GOUDA_LCD_CS_0 = 0,
    HAL_GOUDA_LCD_CS_1 = 1,

    HAL_GOUDA_LCD_CS_QTY,

    HAL_GOUDA_LCD_MEMORY_IF = 2,
    HAL_GOUDA_LCD_IN_RAM = 3

} HAL_GOUDA_LCD_CS_T;


// =============================================================================
// HAL_GOUDA_LCD_OUTPUT_FORMAT_T
// -----------------------------------------------------------------------------
/// Describes the Output format of the LCD bus.
/// The MSB select also the AHB access size (8-bit or 16-bit) when Memory destination is selected.
/// Must set to RGB565 when RAM type destination selected
// =============================================================================
typedef enum
{
    /// 8-bit - RGB3:3:2 - 1cycle/1pixel - RRRGGGBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_8_BIT_RGB332    = 0,
    /// 8-bit - RGB4:4:4 - 3cycle/2pixel - RRRRGGGG/BBBBRRRR/GGGGBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_8_bit_RGB444    = 1,
    /// 8-bit - RGB5:6:5 - 2cycle/1pixel - RRRRRGGG/GGGBBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_8_bit_RGB565    = 2,
    /// 16-bit - RGB3:3:2 - 1cycle/2pixel - RRRGGGBBRRRGGGBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB332   = 4,
    /// 16-bit - RGB4:4:4 - 1cycle/1pixel - XXXXRRRRGGGGBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB444   = 5,
    /// 16-bit - RGB5:6:5 - 1cycle/1pixel - RRRRRGGGGGGBBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB565   = 6

} HAL_GOUDA_LCD_OUTPUT_FORMAT_T;

// =============================================================================
// HAL_GOUDA_LCD_CONFIG_T
// -----------------------------------------------------------------------------
/// Describes the configuration of the GOUDA LCD interface.
// =============================================================================
typedef union
{
    struct
    {
        HAL_GOUDA_LCD_CS_T              cs:2;
        UINT32 :2;
        HAL_GOUDA_LCD_OUTPUT_FORMAT_T   outputFormat:3;
#if (CHIP_HAS_ASYNC_TCU)
        BOOL                            highByte:1;
#else
        UINT32 :1;
#endif
        /// \c FALSE is active low, \c TRUE is active high.
        BOOL                            cs0Polarity:1;
        /// \c FALSE is active low, \c TRUE is active high.
        BOOL                            cs1Polarity:1;
        /// \c FALSE is active low, \c TRUE is active high.
        BOOL                            rsPolarity:1;
        /// \c FALSE is active low, \c TRUE is active high.
        BOOL                            wrPolarity:1;
        /// \c FALSE is active low, \c TRUE is active high.
        BOOL                            rdPolarity:1;
        UINT32 :3;
        /// Number of command to be send to the LCD command (up to 31)
        UINT32                          nbCmd:5;
        UINT32 :3;
        /// Start command transfer only. Autoreset
        BOOL                            startCmdOnly:1;
        UINT32 :7;
    };

    /// Use this 'reg' field of the union
    /// to access this timing configuration as
    /// a 32 bits value than can be written to
    /// a register.
    UINT32 reg;
} HAL_GOUDA_LCD_CONFIG_T;

// =============================================================================
// HAL_GOUDA_SERIAL_LCD_CONFIG_T
// -----------------------------------------------------------------------------
/// Describes the configuration of the GOUDA serial LCD interface.
// =============================================================================
typedef union
{
    struct
    {
        BOOL isSlcd:1;
        UINT32 deviceId:6;
        UINT32 clkDivider:8;
        UINT32 dummyCycle:3;
        UINT32 lineType:2;
        UINT32 rxByte:3;
        BOOL isRead:1;
    };

    UINT32 reg;
} HAL_GOUDA_SERIAL_LCD_CONFIG_T;

// =============================================================================
// HAL_GOUDA_LCD_CMD_T
// -----------------------------------------------------------------------------
/// Describes a command send to LCD
// =============================================================================
typedef struct
{
    /// \FALSE for a command, \TRUE for a data
    BOOL    isData;
    /// value of data or command
    UINT16  value;
} HAL_GOUDA_LCD_CMD_T;

// =============================================================================
// HAL_GOUDA_WINDOW_T
// -----------------------------------------------------------------------------
/// Describes a window by a top left point
/// and a bottom right point
// =============================================================================
typedef struct
{
    /// top left point
    UINT16  tlPX;
    UINT16  tlPY;
    /// bottom right point
    UINT16  brPX;
    UINT16  brPY;

} HAL_GOUDA_WINDOW_T;

// =============================================================================
// HAL_GOUDA_IMG_FORMAT_T
// -----------------------------------------------------------------------------
/// Describes the image input format
// =============================================================================
typedef enum
{
    HAL_GOUDA_IMG_FORMAT_RGB565,
    HAL_GOUDA_IMG_FORMAT_RGBA,
    HAL_GOUDA_IMG_FORMAT_UYVY,
    HAL_GOUDA_IMG_FORMAT_YUYV,
    HAL_GOUDA_IMG_FORMAT_IYUV
} HAL_GOUDA_IMG_FORMAT_T;

// =============================================================================
// HAL_GOUDA_CKEY_MASK_T
// -----------------------------------------------------------------------------
/// Describes the color mask for Chroma Keying.
/// This allows a range of color as transparent.
// =============================================================================
typedef enum
{
    /// exact color match
    HAL_GOUDA_CKEY_MASK_OFF = 0,
    /// disregard 1 LSBit of each color component for matching
    HAL_GOUDA_CKEY_MASK_1LSB = 1,
    /// disregard 2 LSBit of each color component for matching
    HAL_GOUDA_CKEY_MASK_2LSB = 3,
    /// disregard 3 LSBit of each color component for matching
    HAL_GOUDA_CKEY_MASK_3LSB = 7
} HAL_GOUDA_CKEY_MASK_T;

// =============================================================================
// HAL_GOUDA_OVL_LAYER_ID_T
// -----------------------------------------------------------------------------
/// Describes the overlay layers Ids.
// =============================================================================
typedef enum
{
    HAL_GOUDA_OVL_LAYER_ID0,
    HAL_GOUDA_OVL_LAYER_ID1,
    HAL_GOUDA_OVL_LAYER_ID2,

    HAL_GOUDA_OVL_LAYER_ID_QTY
} HAL_GOUDA_OVL_LAYER_ID_T;

// =============================================================================
// HAL_GOUDA_OVL_LAYER_DEF_T
// -----------------------------------------------------------------------------
/// Describes the configuration of an overlay layer.
// =============================================================================
typedef struct
{
    ///@todo check with UINT16* (?)
    UINT32* addr;
    HAL_GOUDA_IMG_FORMAT_T fmt;
    /// buffer width in bytes including optional padding, when 0 will be calculated
    UINT16  stride;
    HAL_GOUDA_WINDOW_T pos;
    UINT8   alpha;
    BOOL    cKeyEn;
    UINT16  cKeyColor;
    HAL_GOUDA_CKEY_MASK_T cKeyMask;
} HAL_GOUDA_OVL_LAYER_DEF_T;


// =============================================================================
// HAL_GOUDA_VID_LAYER_DEPTH_T
// -----------------------------------------------------------------------------
/// Describes the video layer depth
// =============================================================================
typedef enum
{
    /// Video layer behind all Overlay layers
    HAL_GOUDA_VID_LAYER_BEHIND_ALL = 0,
    /// Video layer between Overlay layers 1 and 0
    HAL_GOUDA_VID_LAYER_BETWEEN_1_0 ,
    /// Video layer between Overlay layers 2 and 1
    HAL_GOUDA_VID_LAYER_BETWEEN_2_1 ,
    /// Video layer on top of all Overlay layers
    HAL_GOUDA_VID_LAYER_OVER_ALL

} HAL_GOUDA_VID_LAYER_DEPTH_T;



// =============================================================================
// HAL_GOUDA_VID_LAYER_DEF_T
// -----------------------------------------------------------------------------
/// Describes the configuration of the video layer.
// =============================================================================
typedef struct
{
    UINT32* addrY;
    UINT32* addrU;
    UINT32* addrV;
    HAL_GOUDA_IMG_FORMAT_T fmt;
    UINT16  height;
    UINT16  width;
    UINT16  stride;
    HAL_GOUDA_WINDOW_T pos;
    UINT8   alpha;
    BOOL    cKeyEn;
    UINT16  cKeyColor;
    HAL_GOUDA_CKEY_MASK_T cKeyMask;
    HAL_GOUDA_VID_LAYER_DEPTH_T depth;
} HAL_GOUDA_VID_LAYER_DEF_T;


// =============================================================================
// FUNCTIONS
// =============================================================================

#ifdef DUAL_LCD
// =============================================================================
// hal_GoudaOpen
// -----------------------------------------------------------------------------
/// Open the LCD controller of Gouda
/// Parameters are used to describe the screen connexion configuration and 
/// the controller timings to use depending on the system clock frequency.
/// @param config Pointer to the controller configuration.
/// @param timings Controller timing array (must be provided and valid,
///   even if unused in case of #HAL_GOUDA_LCD_MEMORY_IF)
/// @param ebcCsAddr address of ebc CS for #HAL_GOUDA_LCD_MEMORY_IF,
///   can be 0 for gouda CS.
/// @param Rst used to judge whether reset Gouda(LCD) or not
// =============================================================================
PUBLIC VOID hal_GoudaOpen(CONST HAL_GOUDA_LCD_CONFIG_T* config, CONST HAL_GOUDA_LCD_TIMINGS_T* timings, UINT32 ebcCsAddr,BOOL Rst);


// =============================================================================
// hal_GoudaSerialOpen
// -----------------------------------------------------------------------------
/// Open the serial LCD controller of Gouda
/// Parameters are used to describe the screen connexion configuration and 
/// the controller timings to use depending on the system clock frequency.
/// @param spiLineType The SPI line type of the serial LCD.
/// @param spiFreq The SPI frequency of the serial LCD.
/// @param config Pointer to the controller configuration.
/// @param ebcCsAddr address of ebc CS for #HAL_GOUDA_LCD_MEMORY_IF,
///   can be 0 for gouda CS.
/// @param Rst used to judge whether reset Gouda(LCD) or not
// =============================================================================
PUBLIC VOID hal_GoudaSerialOpen(HAL_GOUDA_SPI_LINE_T spiLineType, UINT32 spiFreq, CONST HAL_GOUDA_LCD_CONFIG_T* config, UINT32 ebcCsAddr,BOOL Rst);

PUBLIC VOID hal_GoudaSetCsPin(BOOL cs);

#else
// =============================================================================
// hal_GoudaOpen
// -----------------------------------------------------------------------------
/// Open the LCD controller of Gouda
/// Parameters are used to describe the screen connexion configuration and 
/// the controller timings to use depending on the system clock frequency.
/// @param config Pointer to the controller configuration.
/// @param timings Controller timing array (must be provided and valid,
///   even if unused in case of #HAL_GOUDA_LCD_MEMORY_IF)
/// @param ebcCsAddr address of ebc CS for #HAL_GOUDA_LCD_MEMORY_IF,
///   can be 0 for gouda CS.
// =============================================================================
PUBLIC VOID hal_GoudaOpen(CONST HAL_GOUDA_LCD_CONFIG_T* config, CONST HAL_GOUDA_LCD_TIMINGS_T* timings, UINT32 ebcCsAddr);


// =============================================================================
// hal_GoudaSerialOpen
// -----------------------------------------------------------------------------
/// Open the serial LCD controller of Gouda
/// Parameters are used to describe the screen connexion configuration and 
/// the controller timings to use depending on the system clock frequency.
/// @param spiLineType The SPI line type of the serial LCD.
/// @param spiFreq The SPI frequency of the serial LCD.
/// @param config Pointer to the controller configuration.
/// @param ebcCsAddr address of ebc CS for #HAL_GOUDA_LCD_MEMORY_IF,
///   can be 0 for gouda CS.
// =============================================================================
PUBLIC VOID hal_GoudaSerialOpen(HAL_GOUDA_SPI_LINE_T spiLineType, UINT32 spiFreq, CONST HAL_GOUDA_LCD_CONFIG_T* config, UINT32 ebcCsAddr);

#endif
// =============================================================================
// hal_GoudaClose
// -----------------------------------------------------------------------------
/// Close Gouda.
// =============================================================================
PUBLIC VOID hal_GoudaClose(VOID);


// =============================================================================
// hal_GoudaWriteReg
// -----------------------------------------------------------------------------
/// Write a command and then a data to the LCD screen (Or a value at a register
/// address, depending on the point of view).
/// 
/// @param addr Address of the register to write, or command to send
/// to the LCD screen
/// @param data Corresponding data to write to the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaWriteReg(UINT16 addr, UINT16 data);


// =============================================================================
// hal_GoudaReadData
// -----------------------------------------------------------------------------
/// Read a data from the LCD screen (Or a value at a register
/// address, depending on the point of view).
/// 
/// @param addr Address of the register to read
/// @param pData Pointer of the corresponding data to read from the LCD.
/// @param len Data length to read from the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaReadData(UINT16 addr, UINT8 *pData, UINT32 len);


// =============================================================================
// hal_GoudaReadReg
// -----------------------------------------------------------------------------
/// Read a data from the LCD screen (Or a value at a register
/// address, depending on the point of view).
/// 
/// @param addr Address of the register to read
/// @param pData Pointer of the corresponding data to read from the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaReadReg(UINT16 addr, UINT16 *pData);


// =============================================================================
// hal_GoudaWriteCmd
// -----------------------------------------------------------------------------
/// Write a command to the LCD.
///
/// @param addr Command to write.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaWriteCmd(UINT16 addr);


// =============================================================================
// hal_GoudaWriteData
// -----------------------------------------------------------------------------
/// Write a data to the LCD screen.
///
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaWriteData(UINT16 data);

// =============================================================================
// hal_GoudaBlitRoi
// -----------------------------------------------------------------------------
/// Send a block of pixel data to the LCD screen.
/// 
/// @param pRoi Pointer to the roi description to send to the screen.
/// @param nbCmd number of commands to send to LCD before pixel data
/// @param pCmds commands to send to LCD before pixel data can be \c NULL if \p nbCmd is 0
/// @param handler If not NULL, user function that is called when the transfer
/// is finished.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaBlitRoi(CONST HAL_GOUDA_WINDOW_T* pRoi, CONST UINT32 nbCmd, CONST HAL_GOUDA_LCD_CMD_T* pCmds, HAL_GOUDA_IRQ_HANDLER_T handler);

// =============================================================================
// hal_GoudaBlitRoi2Ram
// -----------------------------------------------------------------------------
/// Send a block of pixel data to a flat ram buffer in RGB565 format
/// WARNING: only even pixel number are supported, that is to say size must
/// be a multiple of 4 bytes.
/// 
/// @param pBuffer Pointer to the ram buffer
/// @param width Buffer width in pixel, if 0 the buffer is considered to have
/// the same width as the roi.
/// @param pRoi Pointer to the roi description to send to the screen.
/// @param handler If not NULL, user function that is called when the transfer
/// is finished.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaBlitRoi2Ram(UINT32* pBuffer, CONST UINT16 width, CONST HAL_GOUDA_WINDOW_T* pRoi, HAL_GOUDA_IRQ_HANDLER_T handler);


// =============================================================================
// hal_GoudaIsActive
// -----------------------------------------------------------------------------
/// Get the activity status
/// @return \c TRUE when active
// =============================================================================
PUBLIC BOOL hal_GoudaIsActive(VOID);


// =============================================================================
// hal_GoudaSetBgColor
// -----------------------------------------------------------------------------
/// Set the background color
/// @param color in RGB565 format
// =============================================================================
PUBLIC VOID hal_GoudaSetBgColor(UINT16 color);

// =============================================================================
// hal_GoudaGetBgColor
// -----------------------------------------------------------------------------
/// Get the background color
/// @param color in RGB565 format
// =============================================================================
PUBLIC UINT16 hal_GoudaGetBgColor(VOID);

// =============================================================================
// hal_GoudaOvlLayerOpen
// -----------------------------------------------------------------------------
/// Open and configure an overlay layer.
///
/// @param id the layer Id
/// @param def the configuration
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the layer is already enabled.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID_T id, HAL_GOUDA_OVL_LAYER_DEF_T* def);

// =============================================================================
// hal_GoudaOvlLayerClose
// -----------------------------------------------------------------------------
/// Close an overlay layer.
// =============================================================================
PUBLIC VOID hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID_T id);


// =============================================================================
// hal_GoudaVidLayerOpen
// -----------------------------------------------------------------------------
/// Open and configure the video layer.
///
/// @param def the configuration
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the layer is already enabled.
// =============================================================================
PUBLIC HAL_ERR_T hal_GoudaVidLayerOpen(HAL_GOUDA_VID_LAYER_DEF_T* def);


// =============================================================================
// hal_GoudaVidLayerClose
// -----------------------------------------------------------------------------
/// Close the video layer.
// =============================================================================
PUBLIC VOID hal_GoudaVidLayerClose(VOID);

// =============================================================================
// hal_GoudaVidLayerState
// -----------------------------------------------------------------------------
/// Get an video layer state.
// =============================================================================
PUBLIC UINT32 hal_GoudaVidLayerState(VOID);

// =============================================================================
// hal_GoudaInitResetPin
// -----------------------------------------------------------------------------
/// Initialize the status of LCD reset pin.
// =============================================================================
PUBLIC VOID hal_GoudaInitResetPin(VOID);

// =============================================================================
// hal_GoudaGetTiming
// -----------------------------------------------------------------------------
/// Get lcd timing.
// =============================================================================
PUBLIC UINT32 hal_GoudaGetTiming(VOID);

// =============================================================================
// hal_GoudaSetTiming
// -----------------------------------------------------------------------------
/// Set lcd timing.
// =============================================================================
PUBLIC VOID hal_GoudaSetTiming(UINT32 lcd_timing);

/// @} end of group
#endif // _HAL_GOUDA_H_


