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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_sys_ifc.h $ //
//    $Author: yangtt $                                                        // 
//    $Date: 2012-01-09 11:35:21 +0800 (星期一, 09 一月 2012) $                     //   
//    $Revision: 13085 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_sys_ifc.h
///     Prototypes for private IFC functions and defines
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef  _HALP_SYS_IFC_H_
#define  _HALP_SYS_IFC_H_

#include "cs_types.h"

#include "chip_id.h"
#include "sys_ifc.h"



#define HAL_UNKNOWN_CHANNEL      0xff



/// IFC transaction type Id
/// @todo Add UART 2?
typedef enum {
/// Sending data to the sim
    HAL_IFC_SCI_TX,
/// Receiving data from the sim
    HAL_IFC_SCI_RX,
/// Sending data on the SPI
    HAL_IFC_SPI_TX,
/// Receiving data from the SPI
    HAL_IFC_SPI_RX,
/// Sending data on the SPI
    HAL_IFC_SPI2_TX,
/// Receiving data from the SPI
    HAL_IFC_SPI2_RX,
/// Sending data on the SPI
    HAL_IFC_SPI3_TX,
/// Receiving data from the SPI
    HAL_IFC_SPI3_RX,
/// Sending data on the debug uart
    HAL_IFC_DEBUG_UART_TX,
/// Receiving data from the debug uart
    HAL_IFC_DEBUG_UART_RX,
/// Sending data on the uart 1
    HAL_IFC_UART1_TX,
/// Receiving data from uart 1
    HAL_IFC_UART1_RX,
/// Sending data on the uart 2
    HAL_IFC_UART2_TX,
/// Receiving data from uart 2
    HAL_IFC_UART2_RX,
/// Sending data on the SDMMC
    HAL_IFC_SDMMC_TX,
/// Receiving data from the SDMMC
    HAL_IFC_SDMMC_RX,
/// Sending data on the CAMERA: Doesn't exist
    HAL_IFC_CAMERA_TX_RESERVED,
/// Receiving data from the CAMERA
    HAL_IFC_CAMERA_RX,
    #if (CHIP_HAS_SDIO == 1)
/// Sending data on the SDMMC //  HAL_IFC_SDMMC2_TX =54,
    HAL_IFC_SDMMC2_TX ,
/// Receiving data from the SDMMC
    HAL_IFC_SDMMC2_RX , // HAL_IFC_SDMMC2_RX =55,
    #endif

/// No transaction requested
    HAL_IFC_NO_REQWEST
} HAL_IFC_REQUEST_ID_T;

// =============================================================================
// HAL_IFC_MODE_T
// -----------------------------------------------------------------------------
/// Define the mode used to configure an IFC transfer. This enum describes
/// the width (8 or 32 bits) and if the transfer is autodisabled or manually
/// disabled.
// =============================================================================
typedef enum
{
    HAL_IFC_SIZE_8_MODE_MANUAL  = (0 | 0),
    HAL_IFC_SIZE_8_MODE_AUTO    = (0 | SYS_IFC_AUTODISABLE),
#if (CHIP_HAS_SYS_IFC_SIZE == 1)
    HAL_IFC_SIZE_32_MODE_MANUAL = (SYS_IFC_SIZE | 0),
    HAL_IFC_SIZE_32_MODE_AUTO   = (SYS_IFC_SIZE | SYS_IFC_AUTODISABLE),
#else
    HAL_IFC_SIZE_32_MODE_MANUAL = (0 | 0),
    HAL_IFC_SIZE_32_MODE_AUTO   = (0 | SYS_IFC_AUTODISABLE),
#endif
} HAL_IFC_MODE_T;


// =============================================================================
// hal_IfcOpen
// -----------------------------------------------------------------------------
/// Open the System IFC. Initialize some state variables. 
// =============================================================================
PROTECTED VOID hal_IfcOpen(VOID);


// =============================================================================
// hal_IfcGetOwner
// -----------------------------------------------------------------------------
/// Get the owner ID of an IFC channel (i.e. the module using the channel).
/// @param channel number to check
/// @return The owner number associated with the channel
// =============================================================================
PROTECTED HAL_IFC_REQUEST_ID_T hal_IfcGetOwner(UINT8 channel);


// =============================================================================
// hal_IfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of a channel owned by a request.
/// 
/// The channel is only released if the specified request
/// owns the channel.
/// 
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel to release
// =============================================================================
PROTECTED VOID hal_IfcChannelRelease(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel);


// =============================================================================
// hal_IfcChannelFlush
// -----------------------------------------------------------------------------
/// Empty the FIFO a specified channel.
///
/// If #requestId does not match #channel owner, does not do anything.
/// 
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel whose FIFO is to be clear.
// =============================================================================
PROTECTED VOID hal_IfcChannelFlush(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel);


// =============================================================================
// hal_IfcChannelIsFifoEmpty
// -----------------------------------------------------------------------------
/// Returns \c TRUE when the FIFO is empty.
///
/// If #requestId does not match #channel owner, returns TRUE.
/// 
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel whose FIFO is to be checked.
// =============================================================================
PROTECTED BOOL hal_IfcChannelIsFifoEmpty(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel);


// =============================================================================
// hal_IfcTransferStart
// -----------------------------------------------------------------------------
/// Start an IFC transfer
/// 
/// This is a non blocking function that starts the transfer
/// and returns the hand. 
/// 
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param memStartAddr. Start address of the buffer where data 
/// to be sent are located or where to put the data read, according
/// to the request defined by the previous parameter
/// @param xferSize Number of bytes to transfer. The maximum size 
/// is 2^20 - 1 bytes.
/// @param ifcMode Mode of the transfer (Autodisable or not, 8 or 32 bits)
/// @return Channel got or HAL_UNKNOWN_CHANNEL.
// =============================================================================
PROTECTED UINT8 hal_IfcTransferStart(HAL_IFC_REQUEST_ID_T requestId, UINT8* memStartAddr, UINT32 xferSize, HAL_IFC_MODE_T ifcMode);


// =============================================================================
// hal_IfcGetTc
// -----------------------------------------------------------------------------
/// Returns the transfer count of the IFC.
/// If #requestId does not match #channel owner, returns 0.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Number of the channel whose transfer we are concerned for.
/// @return The number of bytes remaining to be transfered.
// =============================================================================
PROTECTED UINT32 hal_IfcGetTc(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel);


// =============================================================================
// hal_IfcExtendTransfer
// -----------------------------------------------------------------------------
/// Extend the transfer size of the channel of 
/// the number ox bytes passed as a parameter. 
///
/// Should be used only when a channel has been locked.
///
/// @param xferSize Number of bytes to add to the
/// transfer size of this channel
// =============================================================================
PROTECTED VOID hal_IfcExtendTransfer(UINT8 channel, UINT16 xferSize);


// =============================================================================
// hal_IfcGetChannel
// -----------------------------------------------------------------------------
/// Get an IFC channel opend in NO auto-disable mode
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param ifcMode Mode of the transfer (Autodisable or not, 8 or 32 bits)
/// @return The channel number got or HAL_UNKNOWN_CHANNEL
// =============================================================================
PROTECTED UINT8 hal_IfcGetChannel(HAL_IFC_REQUEST_ID_T requestId, HAL_IFC_MODE_T ifcMode);


// =============================================================================
// hal_IfcSetStartAddress
// -----------------------------------------------------------------------------
/// Set the start address of a previously opened (with hal_IfcGetChannel)
/// IFC channel.
///
/// Should be used only when a channel has been locked.
///
/// @param channel Opened channel whose start address will be set.
/// @param startAddress Set start address of the channel.
// =============================================================================
PROTECTED VOID hal_IfcSetStartAddress(UINT8 channel, CONST UINT8* startAddress);



#endif //  HAL_SYS_IFC_H 

