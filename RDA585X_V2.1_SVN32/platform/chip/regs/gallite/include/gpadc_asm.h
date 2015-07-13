#ifndef _GPADC_ASM_H_
#define _GPADC_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'gpadc'."
#endif



//==============================================================================
// gpadc
//------------------------------------------------------------------------------
/// 
//==============================================================================
#define REG_GPADC_BASE              0x01A20000

#define REG_GPADC_BASE_HI           BASE_HI(REG_GPADC_BASE)
#define REG_GPADC_BASE_LO           BASE_LO(REG_GPADC_BASE)

#define REG_GPADC_CTRL              REG_GPADC_BASE_LO + 0x00000000
#define REG_GPADC_STATUS            REG_GPADC_BASE_LO + 0x00000004
#define REG_GPADC_DATA              REG_GPADC_BASE_LO + 0x00000008
#define REG_GPADC_DATA_CH0          REG_GPADC_BASE_LO + 0x0000000C
#define REG_GPADC_DATA_CH1          REG_GPADC_BASE_LO + 0x00000010
#define REG_GPADC_DATA_CH2          REG_GPADC_BASE_LO + 0x00000014
#define REG_GPADC_DATA_CH3          REG_GPADC_BASE_LO + 0x00000018
#define REG_GPADC_IRQ_MASK          REG_GPADC_BASE_LO + 0x0000001C
#define REG_GPADC_IRQ_CAUSE         REG_GPADC_BASE_LO + 0x00000020
#define REG_GPADC_IRQ_CLR           REG_GPADC_BASE_LO + 0x00000024
#define REG_GPADC_THRSHD            REG_GPADC_BASE_LO + 0x00000028
#define REG_GPADC_CMD_SET           REG_GPADC_BASE_LO + 0x0000002C
#define REG_GPADC_CMD_CLR           REG_GPADC_BASE_LO + 0x00000030

//ctrl
#define GPADC_CH0_EN                (1<<0)
#define GPADC_CH1_EN                (1<<1)
#define GPADC_CH2_EN                (1<<2)
#define GPADC_CH3_EN                (1<<3)
#define GPADC_ATP(n)                (((n)&7)<<8)
#define GPADC_ATP_122US             (0<<8)
#define GPADC_ATP_100MS             (1<<8)
#define GPADC_ATP_10MS              (2<<8)
#define GPADC_ATP_1MS               (3<<8)
#define GPADC_ATP_250MS             (4<<8)
#define GPADC_ATP_500MS             (5<<8)
#define GPADC_ATP_1S                (6<<8)
#define GPADC_ATP_2S                (7<<8)
#define GPADC_PD_MODE               (1<<16)
#define GPADC_PD_MODE_SW            (0<<16)
#define GPADC_PD_MODE_HW            (1<<16)
#define GPADC_MANUAL_MODE           (1<<20)
#define GPADC_CH_EN(n)              (((n)&15)<<0)
#define GPADC_CH_EN_MASK            (15<<0)
#define GPADC_CH_EN_SHIFT           (0)

//status
#define GPADC_THRSHD0_IRQ           (1<<0)
#define GPADC_THRSHD1_IRQ           (1<<4)
#define GPADC_EOC                   (1<<8)
#define GPADC_BUSY                  (1<<12)

//data
#define GPADC_DATA(n)               (((n)&0x3FF)<<0)

//data_ch0
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//data_ch1
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//data_ch2
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//data_ch3
//#define GPADC_DATA(n)             (((n)&0x3FF)<<0)

//irq_mask
//#define GPADC_THRSHD0_IRQ         (1<<0)
//#define GPADC_THRSHD1_IRQ         (1<<4)
//#define GPADC_EOC                 (1<<8)

//irq_cause
//#define GPADC_THRSHD0_IRQ         (1<<0)
//#define GPADC_THRSHD1_IRQ         (1<<4)
//#define GPADC_EOC                 (1<<8)

//irq_clr
//#define GPADC_THRSHD0_IRQ         (1<<0)
//#define GPADC_THRSHD1_IRQ         (1<<4)
//#define GPADC_EOC                 (1<<8)

//thrshd
#define GPADC_THRSHD0(n)            (((n)&0x3FF)<<0)
#define GPADC_THRSHD1(n)            (((n)&0x3FF)<<16)

//cmd_set
#define GPADC_GPADC_PD              (1<<0)
#define GPADC_GPADC_RESET           (1<<4)
#define GPADC_START_MEASURE         (1<<8)

//cmd_clr
//#define GPADC_GPADC_PD            (1<<0)
//#define GPADC_GPADC_RESET         (1<<4)




#endif
