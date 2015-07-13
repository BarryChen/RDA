#ifndef _SCI_ASM_H_
#define _SCI_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'sci'."
#endif



//==============================================================================
// sci
//------------------------------------------------------------------------------
/// 
//==============================================================================
#define REG_SCI_BASE                0x01A10000

#define REG_SCI_BASE_HI             BASE_HI(REG_SCI_BASE)
#define REG_SCI_BASE_LO             BASE_LO(REG_SCI_BASE)

#define REG_SCI_CONFIG              REG_SCI_BASE_LO + 0x00000000
#define REG_SCI_STATUS              REG_SCI_BASE_LO + 0x00000004
#define REG_SCI_DATA                REG_SCI_BASE_LO + 0x00000008
#define REG_SCI_CLKDIV              REG_SCI_BASE_LO + 0x0000000C
#define REG_SCI_RXCNT               REG_SCI_BASE_LO + 0x00000010
#define REG_SCI_TIMES               REG_SCI_BASE_LO + 0x00000014
#define REG_SCI_CH_FILT             REG_SCI_BASE_LO + 0x00000018
#define REG_SCI_DBG                 REG_SCI_BASE_LO + 0x0000001C
#define REG_SCI_INT_CAUSE           REG_SCI_BASE_LO + 0x00000020
#define REG_SCI_INT_CLR             REG_SCI_BASE_LO + 0x00000024
#define REG_SCI_INT_MASK            REG_SCI_BASE_LO + 0x00000028

//Config
#define SCI_ENABLE                  (1<<0)
#define SCI_PARITY_MASK             (1<<1)
#define SCI_PARITY_SHIFT            (1)
#define SCI_PARITY_EVEN_PARITY      (0<<1)
#define SCI_PARITY_ODD_PARITY       (1<<1)
#define SCI_PERF                    (1<<2)
#define SCI_FILTER_DISABLE          (1<<3)
#define SCI_CLOCKSTOP               (1<<4)
#define SCI_AUTOSTOP_EN_H           (1<<5)
#define SCI_MSBH_LSBL               (1<<6)
#define SCI_LLI                     (1<<7)
#define SCI_PEGEN_LEN               (1<<8)
#define SCI_PARITY_EN               (1<<9)
#define SCI_STOP_LEVEL              (1<<10)
#define SCI_ARG_H                   (1<<16)
#define SCI_AFD_EN_H                (1<<17)
#define SCI_TX_RESEND_EN_H          (1<<18)
#define SCI_RESET                   (1<<20)
#define SCI_DLY_SEL                 (1<<21)
#define SCI_PAR_CHK_OFFSET(n)       (((n)&0x3F)<<24)
#define SCI_RESERVED(n)             (((n)&3)<<30)

//Status
#define SCI_RXDATA_RDY              (1<<0)
#define SCI_TX_FIFO_RDY             (1<<1)
#define SCI_FORMAT_DET              (1<<2)
#define SCI_ARG_DET                 (1<<3)
#define SCI_RESET_DET               (1<<4)
#define SCI_CLK_RDY_H               (1<<5)
#define SCI_CLK_OFF                 (1<<6)
#define SCI_RX_ERR                  (1<<8)
#define SCI_TX_ERR                  (1<<9)
#define SCI_RXOVERFLOW              (1<<10)
#define SCI_TXOVERFLOW              (1<<11)

#define SCI_AUTOSTOP_RXCOUNTER_MASK     (0xff<<22)

#define SCI_AUTOSTOP_STATE_MASK     (3<<30)
#define SCI_AUTOSTOP_STATE_SHIFT    (30)
#define SCI_AUTOSTOP_STATE_STARTUP_PHASE (0<<30)
#define SCI_AUTOSTOP_STATE_AUTO_ON  (1<<30)
#define SCI_AUTOSTOP_STATE_SHUTDOWN_PHASE (2<<30)
#define SCI_AUTOSTOP_STATE_CLOCK_OFF (3<<30)

//Data
#define SCI_DATA_IN(n)              (((n)&0xFF)<<0)
#define SCI_DATA_OUT(n)             (((n)&0xFF)<<0)

//ClkDiv
#define SCI_CLKDIV(n)               (((n)&0x1FF)<<0)
#define SCI_CLKDIV_16(n)            (((n)&0xFF)<<16)
#define SCI_MAINDIV(n)              (((n)&0x3F)<<24)
#define SCI_MAINDIV_MASK            (0x3F<<24)
#define SCI_CLK_OUT_INV             (1<<30)
#define SCI_CLK_INV                 (1<<31)

//RxCnt
#define SCI_RXCNT(n)                (((n)&0x3FF)<<0)
#define SCI_CLK_PERSIST             (1<<31)

//Times
#define SCI_CHGUARD(n)              (((n)&0xFF)<<0)
#define SCI_CHGUARD_MASK            (0xFF<<0)
#define SCI_CHGUARD_SHIFT           (0)
#define SCI_TURNAROUNDGUARD(n)      (((n)&15)<<8)
#define SCI_TURNAROUNDGUARD_MASK    (15<<8)
#define SCI_TURNAROUNDGUARD_SHIFT   (8)
#define SCI_WI(n)                   (((n)&0xFF)<<16)
#define SCI_WI_MASK                 (0xFF<<16)
#define SCI_WI_SHIFT                (16)
#define SCI_TX_PERT(n)              (((n)&0xFF)<<24)

//Ch_Filt
#define SCI_CH_FILT(n)              (((n)&0xFF)<<0)

//Int_Cause
#define SCI_RX_DONE                 (1<<0)
#define SCI_RX_HALF                 (1<<1)
#define SCI_WWT_TIMEOUT             (1<<2)
#define SCI_EXTRA_RX                (1<<3)
#define SCI_RESEND_OVFL             (1<<4)
#define SCI_ARG_END                 (1<<5)

//Int_Clr
//#define SCI_RX_DONE               (1<<0)
//#define SCI_RX_HALF               (1<<1)
//#define SCI_WWT_TIMEOUT           (1<<2)
//#define SCI_EXTRA_RX              (1<<3)
//#define SCI_RESEND_OVFL           (1<<4)
//#define SCI_ARG_END               (1<<5)

//Int_Mask
//#define SCI_RX_DONE               (1<<0)
//#define SCI_RX_HALF               (1<<1)
//#define SCI_WWT_TIMEOUT           (1<<2)
//#define SCI_EXTRA_RX              (1<<3)
//#define SCI_RESEND_OVFL           (1<<4)
//#define SCI_ARG_END               (1<<5)




#endif
