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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_voc.h $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                         //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_voc.h
/// This file describes the API of the HAL VOC  Driver
//                                                                            //
////////////////////////////////////////////////////////////////////////////////





#ifndef _HAL_VOC_H_
#define _HAL_VOC_H_

#include "cs_types.h"
#include "hal_error.h"

/// 
///  @defgroup voc HAL VoC Driver
///  This document describes the characteristics of the HAL VoC Driver 
///  Interface and how to use them via its Hardware Abstraction Layer API. 
///     
///  @par Use the VoC module :
///  @par
///   - \b Open. The VoC module and the VoC DMA cannot be used simultaneously by different
///         tasks or CPUs. In order to use the VoC module, it should be first opened
///         (i.e. reserved). The function <b> \c hal_VocOpen </b> checks if the VoC module
///         is not already reserved by another task and loads the VoC program that will
///         be executed in VoC RAM_I using the VoC DMA. If one of these operations fails
///         because either the VoC module or the VoC DMA are busy, an error is returned.
///         This happens if another task or CPU is using the driver. As the open procedure
///         includes a DMA operation to load the VoC code, its completion can either be 
///         checked by polling a status register or by waiting for a VoC DMA interrupt. 
///         In this latter case the program returns from <b> \c hal_VocOpen </b> without
///         the open procedure being finished and an internal variable is set to FALSE
///         preventing from any attempt to start the VoC module while the open procedure
///         is not finished. Upon reception of the VoC DMA irq, the handler sets this 
///         variable to TRUE. The VoC wakeup, VoC irq masks and the user's VoC handler
///         are also set in <b> \c hal_VocOpen </b>. There are no independent functions to set 
///         these parameters. The VoC code configuration variables, such as the VoC
///         code source pointer, the VoC program entry address and VoC critical section
///         corresponding to the VoC code reload function (if any) are memorized in 
///         internal variables.\n \n 
///   - \b Close. Symmetrically, when the VoC module is no longer needed it should be closed
///         using <b> \c hal_VocClose </b>. This function stops the execution of any running VoC
///         program and/or VoC DMA transfer and clears all VoC irq masks, event masks,
///         wakeup events and irq causes. If the stop occurs inside the critical section
///         it is possible that the VoC code state is corrupted (state variables not updated
///         or code reload not finished). In this case the loaded VoC code source pointer
///         variable is reset. If the stop occurs out of the critical section the loaded
///         VoC code source pointer variable is not reset, so the VoC code does not have 
///         to be reloaded if the VoC module is re-opened with the same code.
///         The VoC driver does not handle the LPS system ressource (or frequency) requests. 
///         This should be managed by VoC's user.\n \n
///   - \b Wakeup. Initially the VoC module is in stalled state (a state, in which
///         the program execution is stopped/paused). Function <b> \c hal_VocWake </b> 
///         is used to put the VoC module in running state (a state, in which the program
///         execution is started/resumed). There are several options to switch the VoC 
///         module from stalled to running state (let call this operation '(re)start VoC').
///         While hal_VocWake(HAL_VOC_START) is memoryless (it can only start VoC
///         when the state is stalled), hal_VocWake(HAL_VOC_WAKEUP_EVENT_X) will generate
///         a VoC wakeup event. Four of the VoC wakeup events are generated by the HW, 
///         and two are generated using <b> \c hal_VocWake </b>. A wakeup event is recorded
///         when the corresponding bit of the wakeup mask is set to 1 in <b> \c hal_VocOpen </b>.
///         A recorded event systematically (re)starts VoC when it is in stalled state
///         until it is cleared. This means that a recorded event will prevent the VoC
///         module from going in stalled stated. Also, A wakeup event will act as
///         hal_VocWake(HAL_VOC_START) when the VoC module is in stalled state.
///         Clearing the wakeup events is the responsability of the VoC program, 
///         so this is not part of the driver. \n \n
///   - \b Stop. Stopping brutally the VoC program can lead to the VoC code 
///         being put in an unknown state, because the VoC code reloading is managed 
///         by the VoC program itself. If stopping is necessary during an exception 
///         handling, function <b> \c hal_VocClose </b> should be used. This guarantees that 
///         the VoC module will be properly re-opened with <b> \c hal_VocOpen </b>. Note however
///         that the VoC code state variables will not be reset with <b> \c hal_VocClose </b>, 
///         because they depend on the VoC program. They need to be reset by the  
///         programmer in an initialization function, in which <b> \c hal_VocOpen </b> is 
///         called. \n \n
///   - \b Done. The VoC program can put the VoC module on purpose in stalled state. 
///         When the VoC module is in stalled state, the program 
///         execution can start/resume if a VoC wakeup event is recorded. This 
///         means that the VoC module can be temporarily stalled without the VoC 
///         program being completed. In parallel, only VoC state information
///         running or stalled can be available at driver level, so it is 
///         impossible to judge of the program advancement status by just using the
///         VoC driver. For this reason a function returning the VoC module status 
///         is not proposed in the VoC driver. The programmer should use the VoC 
///         interrupt and API variables defined in his VoC program to describe the 
///         program advancement status (by definition the API variables are accessible
///         both by the VoC program and the program controlling it on the XCPU). 
///         Obviously, this advancement status and API variables cannot be part of  
///         the VoC driver. \n \n
///   - \b Access the VoC memory. Function <b> \c hal_VocGetPointer </b> returns a pointer
///         to the VoC memory space. he input parameter is an UINT16 address as defined
///         in the VoC memory map of the user's VoC program. \n \n
///          
///  @par Use the VoC DMA module
///  @par
///         These functions allow configuring a DMA transfer from and to the 
///         VoC memory space. <b> \c hal_VocDmaStart </b>
///         starts a VoC DMA transfer. HAL_ERR_RESOURCE_BUSY error is returned is the 
///         resource is busy. Two modes can be selected to confirm that the DMA transfer 
///         tis finished. If the needIrq parameter is set to 0, the driver will not set 
///         the mask for the dmaDone IRQ, so function hal_VocDmaDone needs to be used 
///         to check if the DMA transfer is finished. If the needIrq parameter is set to 
///         1 the dmaDone mask will be set, so an IRQ will be sent to the XCPU when the 
///         transfer is finished. The validation of the successful transfer is done
///         in the user handler.
///         <b> \c hal_VocDmaDone </b> checks if the DMA transfer is finished. \n \n 
/// 
/// 
///   @{
///  

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// HAL_VOC_WAKEUP_ID_T
// ----------------------------------------------------------------------------
/// Defines the way that the VoC module should be woken up.
// ============================================================================
typedef enum  
{
    /// generate a wakeup event with ID=0
    HAL_VOC_WAKEUP_EVENT_0 = 0, 
    /// generate a wakeup event with ID=1
    HAL_VOC_WAKEUP_EVENT_1 = 1,
    /// start the VoC module (has effect only if the module is stalled)
    HAL_VOC_START  = 2 
            
} HAL_VOC_WAKEUP_ID_T;

// ============================================================================
// HAL_VOC_DMA_DIR_T
// ----------------------------------------------------------------------------
/// Defines the direction (read or write) of the DMA transfer
// ============================================================================
typedef enum  
{
    /// read VoC DMA read transfer
    HAL_VOC_DMA_READ = 0, 
    /// read VoC DMA write transfer
    HAL_VOC_DMA_WRITE = 1
            
} HAL_VOC_DMA_DIR_T;

// ============================================================================
// HAL_VOC_DMA_TRANSFER_T
// ----------------------------------------------------------------------------
/// Defines the type (single or burst) of the DMA transfer
// ============================================================================
typedef enum  
{
    /// single VoC DMA write transfer
    HAL_VOC_DMA_BURST = 0,
    /// burst VoC DMA read transfer
    HAL_VOC_DMA_SINGLE = 1
            
} HAL_VOC_DMA_TRANSFER_T;

// ============================================================================
// HAL_VOC_DMA_B2S_T
// ----------------------------------------------------------------------------
/// Defines the byte-to-short mode of the DMA transfer (VOC2.1 and later)
// ============================================================================
typedef enum  
{
    /// b2s disabled (normal burst)
    HAL_VOC_DMA_B2S_NO = 0,
    /// trace mode (b2s disabled, no stall of the voc core with single transfers)
    HAL_VOC_DMA_B2S_TRACE = 1,
    /// unsigned b2s
    HAL_VOC_DMA_B2S_UNSIGNED = 2,
    /// signed b2s
    HAL_VOC_DMA_B2S_SIGNED = 3
            
} HAL_VOC_DMA_B2S_T;

// ============================================================================
// HAL_VOC_WAKEUP_EVENT_T
// ----------------------------------------------------------------------------
/// This structure is used to represent the wake uo events status and mask.
/// These events are recorded in the VoC wakeup event handler and wakeup
/// the VoC module if the corresponding bits in the Event Mask are enabled.
// ============================================================================
typedef struct
{
    /// IFC0 event when the IFC0 reaches a half or full buffer condition
    UINT32 wakeupIfc0:1;
    /// IFC1 event when the IFC1 reaches a half or full buffer condition
    UINT32 wakeupIfc1:1;
    /// DMAE event when the VoC DMA transfer configured by the XCPU is finished.
    UINT32 wakeupDmae:1;
    /// DMAI event when the VoC DMA transfer configured by the VoC module is finished.
    UINT32 wakeupDmai:1;
    /// SOF0 event (generated with function hal_VocWakeupSoftEvent)
    UINT32 wakeupSof0:1;
    /// SOF1 event (generated with function hal_VocWakeupSoftEvent)
    UINT32 wakeupSof1:1; 
    
} HAL_VOC_WAKEUP_EVENT_T;

// ============================================================================
// HAL_VOC_IRQ_STATUS_T
// ----------------------------------------------------------------------------
/// This structure is used to represent the irq status and mask
// ============================================================================
typedef struct 
{
    /// IRQ from voc to the XCPU
    UINT32 voc:1;
    /// unused field
    UINT32 unused1:1; 
    /// IRQ upon finish of a DMA transfert to/from VOC configured by the VoC
    UINT32 dmaVoc:1;
    /// unused field
    UINT32 unused2:1; 
    
} HAL_VOC_IRQ_STATUS_T;

// ============================================================================
// HAL_VOC_IRQ_HANDLER_T
// ----------------------------------------------------------------------------
/// User handler function type
// ============================================================================
typedef void (*HAL_VOC_IRQ_HANDLER_T)(HAL_VOC_IRQ_STATUS_T*);

// ============================================================================
// HAL_VOC_CFG_T
// ----------------------------------------------------------------------------
/// General configuration of the VoC module
// ============================================================================
typedef struct 
{
    /// Pointer to the location of the VoC code which will be executed
    CONST INT32 * vocCode;
    /// Size in bytes of the VoC code
    UINT32 vocCodeSize;
    /// Value of the VoC program counter, on which to start the VoC program execution        
    UINT16 pcVal;
    /// Start of the VoC code critical section (inclusive)        
    UINT16 pcValCriticalSecMin;
    /// End of the VoC code critical section (exclusive)
    UINT16 pcValCriticalSecMax;
    /// Select if an IRQ should be generated at the end of the open procedure 
    BOOL needOpenDoneIrq;
    /// Set VoC wakeup event mask
    HAL_VOC_WAKEUP_EVENT_T eventMask;
    /// Set VoC interrupt mask
    HAL_VOC_IRQ_STATUS_T irqMask;
    /// Set VoC user defined interrupt handler function
    HAL_VOC_IRQ_HANDLER_T vocIrqHandler;
    
} HAL_VOC_CFG_T;
                        
// ============================================================================
// HAL_VOC_DMA_CFG_T
// ----------------------------------------------------------------------------
/// Configuration of a DMA transfer between a buffer in a VoC RAM and a buffer in
/// a external memory (any memory space mapped on AHB).
// ============================================================================
typedef struct 
{
    /// External (AHB) byte address
    INT32* extAddr;
    /// Local (VoC RAM) byte address.
    INT32* vocLocalAddr;
    /// Size of the transfer in bytes.
    INT32  size;
    /// 0 : read (extAddr is the source),
    /// 1 : write (extAddr is the destination).
    HAL_VOC_DMA_DIR_T  wr1Rd0;
    /// Select if an IRQ needs to be generated at the end of the transfer.
    BOOL   needIrq;

} HAL_VOC_DMA_CFG_T;


// ============================================================================
// FUNCTIONS
// ============================================================================

// ============================================================================
// hal_VocOpen
// ----------------------------------------------------------------------------
/// The VoC module and the VoC DMA cannot be used simultaneously by different
/// tasks or CPUs. In order to use the VoC module, it should be first opened
/// (i.e. reserved). The function <b> \c hal_VocOpen </b> checks if the VoC module
/// is not already reserved by another task and loads the VoC program that will
/// be executed in VoC RAM_I using the VoC DMA. If one of these operations fails
/// because either the VoC module or the VoC DMA are busy, an error is returned.
/// This happens if another task or CPU is using the driver. As the open procedure
/// includes a DMA operation to load the VoC code, its completion can either be 
/// checked by polling a status register or by waiting for a VoC DMA interrupt. 
/// In this latter case the program returns from <b> \c hal_VocOpen </b> without
/// the open procedure being finished and an internal variable is set to FALSE
/// preventing from any attempt to start the VoC module while the open procedure
/// is not finished. Upon reception of the VoC DMA irq, the handler sets this 
/// variable to TRUE. The VoC wakeup, VoC irq masks and the user's VoC handler
/// are also set in <b> \c hal_VocOpen </b>. There are no independent functions to set 
/// these parameters. The VoC code configuration variables, such as the VoC
/// code source pointer, the VoC program entry address and VoC critical section
/// corresponding to the VoC code reload function (if any) are memorized in 
/// internal variables. 
///
/// @param pCfg VoC configuration structure of type HAL_VOC_CFG_T 
/// @return error of type HAL_ERR_T. HAL_ERR_RESOURCE_BUSY is returned if the open 
/// procedure has failed because either the VoC module or VoC DMA are in use.
/// HAL_ERR_TIMEOUT is returned if the open completion is check by polling a status
/// register and the polling duration reaches a timeout. In all other cases
/// HAL_ERROR_NO is returned.
// ============================================================================
PUBLIC HAL_ERR_T hal_VocOpen (CONST HAL_VOC_CFG_T * pCfg);

// ============================================================================
// hal_VocClose
// ----------------------------------------------------------------------------
/// This function stops the execution of any running VoC
/// program and/or VoC DMA transfer and clears all VoC irq masks, event masks,
/// wakeup events and irq causes. If the stop occurs inside the critical section
/// it is possible that the VoC code state is corrupted (state variables not updated
/// or code reload not finished). In this case the loaded VoC code source pointer
/// variable is reset. If the stop occurs out of the critical section the loaded
/// VoC code source pointer variable is not reset, so the VoC code does not have 
/// to be reloaded if the VoC module is re-opened with the same code.
/// The VoC driver does not handle the LPS system ressource (or frequency) requests. 
/// This should be managed by VoC's user.
///
// ============================================================================
PUBLIC VOID hal_VocClose(VOID);

// ============================================================================
// hal_VocWake
// ----------------------------------------------------------------------------
/// This Function is used to put the VoC module in running state (a state, in which the
/// program execution is started/resumed). There are several options to switch the VoC 
/// module from stalled to running state (let call this operation '(re)start VoC').
/// While hal_VocWake(HAL_VOC_START) is memoryless (it can only start VoC
/// when the state is stalled), hal_VocWake(HAL_VOC_WAKEUP_EVENT_X) will generate
/// a VoC wakeup event. Four of the VoC wakeup events are generated by the HW, 
/// and two are generated using <b> \c hal_VocWake </b>. A wakeup event is recorded
/// when the corresponding bit of the wakeup mask is set to 1 in <b> \c hal_VocOpen </b>.
/// A recorded event systematically (re)starts VoC when it is in stalled state
/// until it is cleared. This means that a recorded event will prevent the VoC
/// module from going in stalled stated. Also, A wakeup event will act as
/// hal_VocWake(HAL_VOC_START) when the VoC module is in stalled state.
/// Clearing the wakeup events is the responsability of the VoC program, 
/// so this is not part of the driver.
///
/// @param wakeupId define the wakeup mode (start or soft wakeup event). 
/// @return error of type HAL_ERR_T. HAL_ERR_RESOURCE_BUSY is returned if the open 
/// either the VoC module is running or if the open procedure is not completed (because 
/// either not started or still ongoing). In all other cases HAL_ERROR_NO is returned.
// ============================================================================
PUBLIC HAL_ERR_T hal_VocWakeup(HAL_VOC_WAKEUP_ID_T wakeupId);

// ============================================================================
// hal_VocGetPointer
// ----------------------------------------------------------------------------
/// This function returns a pointer to the VoC memory space. 
/// The input parameter is an UINT16 address as defined in the VoC memory map
/// of the user's VoC program. \n
///
/// @param vocLocalAddr Local address in short (INT16) given by the voc memory 
/// map file used to compile the VoC code.
/// @return pointer to the VoC memory space
// ============================================================================
PUBLIC VOID* hal_VocGetPointer(INT32 vocLocalAddr);

// ============================================================================
// hal_VocGetDmaiPointer
// ----------------------------------------------------------------------------
/// This function returns a pointer used in the DMA configuration done by the.
/// VoC assembly code (DMAI). It is used when the system pointers need to be
/// passed (linked) as variables to the VoC assembly code. \n
///
/// @param vocExternAddr Extern address that willbe used for the DMAI transfer 
/// @param wr1Rd0 Select write or read transfer, valid only in single
/// @param sngl1brst0 Select single word or multiword (burst) transfer.
/// @param b2s Select byte-to-short mode of the transfer.
/// @return Configuration pointer for a DMAI transfer
// ============================================================================
PUBLIC INT32 * hal_VocGetDmaiPointer(INT32 * vocExternAddr, HAL_VOC_DMA_DIR_T wr1Rd0, HAL_VOC_DMA_TRANSFER_T sngl1brst0, HAL_VOC_DMA_B2S_T b2s);

// ============================================================================
// hal_VocDmaStart
// ----------------------------------------------------------------------------
/// Start a VoC DMA transfer. HAL_ERR_RESOURCE_BUSY error is returned is the 
/// resource is busy. Two modes can be selected to confirm that the DMA transfer 
/// tis finished. If the needIrq parameter is set to 0, the driver will not set 
/// the mask for the dmaDone IRQ, so function hal_VocDmaDone needs to be used 
/// to check if the DMA transfer is finished. If the needIrq parameter is set to 
/// 1 the dmaDone mask will be set, so an IRQ will be sent to the XCPU when the 
/// transfer is finished. The validation of the successful transfer is done
/// in the user handler.
///
/// @param pCfg Pointer to the VoC DMA configuration struction
/// @return error of type HAL_ERR_T. HAL_ERR_RESOURCE_BUSY is returned if the 
/// VoC DMA are in already in use or if the open procedure is not completed (because 
/// either not started or still ongoing). In all other cases HAL_ERROR_NO is returned.
// ============================================================================
PUBLIC HAL_ERR_T hal_VocDmaStart (CONST HAL_VOC_DMA_CFG_T * pCfg);

// ============================================================================
// hal_VocDmaDone
// ----------------------------------------------------------------------------
/// Checks if the DMA transfer is finished. 
/// @return TRUE if the VoC DMA transfer is finished, else FALSE.
// ============================================================================
PUBLIC BOOL hal_VocDmaDone(VOID);


///
/// @} // <-- End of the voc group
///

#endif

