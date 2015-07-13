#ifndef _RF_IF_ASM_H_
#define _RF_IF_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'rf_if'."
#endif



//==============================================================================
// rf_if
//------------------------------------------------------------------------------
/// 
//==============================================================================
#define REG_RF_IF_BASE              0x01906000

#define REG_RF_IF_BASE_HI           BASE_HI(REG_RF_IF_BASE)
#define REG_RF_IF_BASE_LO           BASE_LO(REG_RF_IF_BASE)

#define REG_RF_IF_BUFFER            REG_RF_IF_BASE_LO + 0x00000000
#define REG_RF_IF_CTRL              REG_RF_IF_BASE_LO + 0x00000004
#define REG_RF_IF_STATUS            REG_RF_IF_BASE_LO + 0x00000008
#define REG_RF_IF_INTERRUPTION_CLEAR REG_RF_IF_BASE_LO + 0x0000000C
#define REG_RF_IF_TX_BURST_DESCRIPTOR_0 REG_RF_IF_BASE_LO + 0x00000010
#define REG_RF_IF_TX_BURST_DESCRIPTOR_1 REG_RF_IF_BASE_LO + 0x00000014
#define REG_RF_IF_TX_BURST_DESCRIPTOR_2 REG_RF_IF_BASE_LO + 0x00000018
#define REG_RF_IF_TX_BURST_DESCRIPTOR_3 REG_RF_IF_BASE_LO + 0x0000001C
#define REG_RF_IF_RX_OFFSET         REG_RF_IF_BASE_LO + 0x00000020

//Buffer
#define RF_IF_RX_TX_DATA(n)         (((n)&0xFFFFFFFF)<<0)

//Ctrl
#define RF_IF_ENABLE_DISABLE        (0<<0)
#define RF_IF_ENABLE_ENABLE         (1<<0)
#define RF_IF_DIGRF_ENABLE_DISABLE  (0<<1)
#define RF_IF_DIGRF_ENABLE_ENABLE   (1<<1)
#define RF_IF_RX_OVERFLOW_ENABLE_DISABLE (0<<4)
#define RF_IF_RX_OVERFLOW_ENABLE_ENABLE (1<<4)
#define RF_IF_RX_CAL_BYPASS_ENABLED (0<<5)
#define RF_IF_RX_CAL_BYPASS_BYPASSED (1<<5)
#define RF_IF_RX_SWAP_I_Q_NO        (0<<6)
#define RF_IF_RX_SWAP_I_Q_YES       (1<<6)
#define RF_IF_RX_FORCE_ADC_ON_NO    (0<<7)
#define RF_IF_RX_FORCE_ADC_ON_YES   (1<<7)
#define RF_IF_RX_FORCE_DEC_ON_NO    (0<<8)
#define RF_IF_RX_FORCE_DEC_ON_YES   (1<<8)
#define RF_IF_RX_FORCE_SOC          (1<<9)
#define RF_IF_RX_FIFO_RESET         (1<<10)
#define RF_IF_TX_OVERFLOW_ENABLE_DISABLE (0<<16)
#define RF_IF_TX_OVERFLOW_ENABLE_ENABLE (1<<16)
#define RF_IF_TX_UNDERFLOW_ENABLE_DISABLE (0<<17)
#define RF_IF_TX_UNDERFLOW_ENABLE_ENABLE (1<<17)
#define RF_IF_TX_FORCE_DAC_ON_NO    (0<<18)
#define RF_IF_TX_FORCE_DAC_ON_YES   (1<<18)
#define RF_IF_TX_FORCE_DAC_OFF_NO   (0<<19)
#define RF_IF_TX_FORCE_DAC_OFF_YES  (1<<19)
#define RF_IF_TX_FORCE_OEN_NO       (0<<20)
#define RF_IF_TX_FORCE_OEN_YES      (1<<20)
#define RF_IF_TX_FORCE_GMSK_ON_NO   (0<<21)
#define RF_IF_TX_FORCE_GMSK_ON_YES  (1<<21)
#define RF_IF_TX_SWAP_I_Q_NO        (0<<22)
#define RF_IF_TX_SWAP_I_Q_YES       (1<<22)
#define RF_IF_TX_FIFO_RESET         (1<<23)
#define RF_IF_DIGRF_RX_RATE_ONE     (0<<24)
#define RF_IF_DIGRF_RX_RATE_TWO     (1<<24)
#define RF_IF_DIGRF_RX_CLK_POL      (1<<25)
#define RF_IF_DIGRF_TX_MODE_STREAM  (0<<26)
#define RF_IF_DIGRF_TX_MODE_BLOCK   (1<<26)
#define RF_IF_DIGRF_TX_CLK_POL      (1<<27)
#define RF_IF_DIGRF_SAMPLE_SIZE(n)  (((n)&7)<<28)
#define RF_IF_DIGRF_ALIGNEMENT_SELECT (1<<31)

//Status
#define RF_IF_RX_FIFO_LEVEL(n)      (((n)&7)<<0)
#define RF_IF_TX_FIFO_LEVEL(n)      (((n)&3)<<3)
#define RF_IF_RX_OVERFLOW_CAUSE     (1<<8)
#define RF_IF_TX_OVERFLOW_CAUSE     (1<<9)
#define RF_IF_TX_UNDERFLOW_CAUSE    (1<<10)
#define RF_IF_RX_OVERFLOW_STATUS    (1<<16)
#define RF_IF_TX_OVERFLOW_STATUS    (1<<17)
#define RF_IF_TX_UNDERFLOW_STATUS   (1<<18)

//Interruption_clear
#define RF_IF_RX_OVERFLOW           (1<<0)
#define RF_IF_TX_OVERFLOW           (1<<1)
#define RF_IF_TX_UNDERFLOW          (1<<2)

//Tx_Burst_descriptor
#define RF_IF_NB_SYMBOLS(n)         (((n)&0xFF)<<0)
#define RF_IF_MODULATION            (1<<16)
#define RF_IF_END_BURST             (1<<24)

//Rx_Offset
#define RF_IF_RX_OFFSET_I(n)        (((n)&0xFFFF)<<0)
#define RF_IF_RX_OFFSET_Q(n)        (((n)&0xFFFF)<<16)




#endif
