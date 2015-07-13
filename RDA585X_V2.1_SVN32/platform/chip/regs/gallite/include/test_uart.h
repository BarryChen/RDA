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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/regs/gallite/include/test_uart.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (Wed, 07 Jul 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _TEST_UART_H_
#define _TEST_UART_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'test_uart'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// TEST_UART_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_TEST_UART_BASE          0x018A0000

typedef volatile struct
{
    REG32                          RX_TX_Divisor_Latch_L;        //0x00000000
    REG32                          Interrupt_En_Divisor_Latch_H; //0x00000004
    REG32                          Interrupt_Identity_Fifo_Ctrl; //0x00000008
    REG32                          Line_Ctrl;                    //0x0000000C
    REG32                          Modem_Ctrl;                   //0x00000010
    REG32                          Line_Status;                  //0x00000014
    REG32                          Modem_Status;                 //0x00000018
    REG32                          Scratch;                      //0x0000001C
} HWP_TEST_UART_T;

#define hwp_testUart                ((HWP_TEST_UART_T*) KSEG1(REG_TEST_UART_BASE))


//Interrupt_En_Divisor_Latch_H
#define TEST_UART_ERBFI             (1<<0)
#define TEST_UART_ETBEI             (1<<1)
#define TEST_UART_ELSI              (1<<2)
#define TEST_UART_EDSSI             (1<<3)
#define TEST_UART_PTIME             (1<<7)

//Interrupt_Identity_Fifo_Ctrl
#define TEST_UART_INTERRUPT_IDENTITY_MODEM_STATUS_CHANGED (0<<0)
#define TEST_UART_INTERRUPT_IDENTITY_NO_IRQ (1<<0)
#define TEST_UART_INTERRUPT_IDENTITY_THR_EMPTY (2<<0)
#define TEST_UART_INTERRUPT_IDENTITY_DATA_AVAILABLE (4<<0)
#define TEST_UART_INTERRUPT_IDENTITY_RECEIVER_STAT (6<<0)
#define TEST_UART_INTERRUPT_IDENTITY_CHAR_TIMEOUT (12<<0)
#define TEST_UART_FIFO_STAT_EN      (3<<6)
#define TEST_UART_FIFO_STAT_DI      (0<<6)
#define TEST_UART_FIFO_EN           (1<<0)
#define TEST_UART_RX_FIFO_RST       (1<<1)
#define TEST_UART_TX_FIFO_RST       (1<<2)
#define TEST_UART_DMA_MODE          (1<<3)
#define TEST_UART_TX_FIFO_TH_EMPTY  (0<<4)
#define TEST_UART_TX_FIFO_TH_2CHAR  (1<<4)
#define TEST_UART_TX_FIFO_TH_QUART  (2<<4)
#define TEST_UART_TX_FIFO_TH_HALF   (3<<4)
#define TEST_UART_RX_FIFO_TH_1CHAR  (0<<6)
#define TEST_UART_RX_FIFO_TH_QUART  (1<<6)
#define TEST_UART_RX_FIFO_TH_HALF   (2<<6)
#define TEST_UART_RX_FIFO_TH_2LESS  (3<<6)

//Line_Ctrl
#define TEST_UART_CHAR(n)           (((n)&3)<<0)
#define TEST_UART_CHAR_MASK         (3<<0)
#define TEST_UART_CHAR_SHIFT        (0)
#define TEST_UART_CHAR_5_BIT        (0<<0)
#define TEST_UART_CHAR_6_BIT        (1<<0)
#define TEST_UART_CHAR_7_BIT        (2<<0)
#define TEST_UART_CHAR_8_BIT        (3<<0)
#define TEST_UART_STOP_BIT          (1<<2)
#define TEST_UART_STOP_BIT_MASK     (1<<2)
#define TEST_UART_STOP_BIT_SHIFT    (2)
#define TEST_UART_STOP_BIT_1_BIT    (0<<2)
#define TEST_UART_STOP_BIT_2_BIT    (1<<2)
#define TEST_UART_PARITY            (1<<3)
#define TEST_UART_PARITY_MASK       (1<<3)
#define TEST_UART_PARITY_SHIFT      (3)
#define TEST_UART_PARITY_DIS        (0<<3)
#define TEST_UART_PARITY_EN         (1<<3)
#define TEST_UART_PARITY_POL        (1<<4)
#define TEST_UART_PARITY_POL_MASK   (1<<4)
#define TEST_UART_PARITY_POL_SHIFT  (4)
#define TEST_UART_PARITY_POL_EVEN   (0<<4)
#define TEST_UART_PARITY_POL_ODD    (1<<4)
#define TEST_UART_BREAK             (1<<6)
#define TEST_UART_DIV_LATCH_PROG    (1<<7)

//Modem_Ctrl
#define TEST_UART_AUTO_FLOW_CTRL_1  (1<<1)
#define TEST_UART_IRDA_2            (1<<2)
#define TEST_UART_LOOPBACK          (1<<4)
#define TEST_UART_AUTO_FLOW_CTRL_5  (1<<5)
#define TEST_UART_IRDA_6            (1<<6)
#define TEST_UART_IRDA_OUTPUT(n)    (((n)&17)<<2)
#define TEST_UART_IRDA_OUTPUT_MASK  (17<<2)
#define TEST_UART_IRDA_OUTPUT_SHIFT (2)
#define TEST_UART_AUTO_FLOW_CTRL(n) (((n)&17)<<1)
#define TEST_UART_AUTO_FLOW_CTRL_MASK (17<<1)
#define TEST_UART_AUTO_FLOW_CTRL_SHIFT (1)

//Line_Status
#define TEST_UART_DATA_READY        (1<<0)
#define TEST_UART_OVERRUN_ERR       (1<<1)
#define TEST_UART_PARITY_ERR        (1<<2)
#define TEST_UART_FRAMING_ERR       (1<<3)
#define TEST_UART_BREAK_INT         (1<<4)
#define TEST_UART_THRE              (1<<5)
#define TEST_UART_TEMT              (1<<6)
#define TEST_UART_RX_FIFO_ERR       (1<<7)

//Modem_Status
#define TEST_UART_DCTS              (1<<0)
#define TEST_UART_DDSR              (1<<1)
#define TEST_UART_TERI              (1<<2)
#define TEST_UART_DDCD              (1<<3)
#define TEST_UART_CTS               (1<<4)
#define TEST_UART_DSR               (1<<5)
#define TEST_UART_RI                (1<<6)
#define TEST_UART_DCD               (1<<7)





#endif

