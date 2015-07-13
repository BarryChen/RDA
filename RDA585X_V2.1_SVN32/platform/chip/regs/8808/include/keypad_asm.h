#ifndef _KEYPAD_ASM_H_
#define _KEYPAD_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'keypad'."
#endif


/// Number of key in the keypad
#define KEY_NB                                   (64)
/// Number of key in the low data register
#define LOW_KEY_NB                               (32)
/// Number of key in the high data register
#define HIGH_KEY_NB                              (32)

//==============================================================================
// keypad
//------------------------------------------------------------------------------
/// 
//==============================================================================
#define REG_KEYPAD_BASE             0x01A05000

#define REG_KEYPAD_BASE_HI          BASE_HI(REG_KEYPAD_BASE)
#define REG_KEYPAD_BASE_LO          BASE_LO(REG_KEYPAD_BASE)

#define REG_KEYPAD_KP_DATA_L        REG_KEYPAD_BASE_LO + 0x00000000
#define REG_KEYPAD_KP_DATA_H        REG_KEYPAD_BASE_LO + 0x00000004
#define REG_KEYPAD_KP_STATUS        REG_KEYPAD_BASE_LO + 0x00000008
#define REG_KEYPAD_KP_CTRL          REG_KEYPAD_BASE_LO + 0x0000000C
#define REG_KEYPAD_KP_IRQ_MASK      REG_KEYPAD_BASE_LO + 0x00000010
#define REG_KEYPAD_KP_IRQ_CAUSE     REG_KEYPAD_BASE_LO + 0x00000014
#define REG_KEYPAD_KP_IRQ_CLR       REG_KEYPAD_BASE_LO + 0x00000018

//KP_DATA_L
#define KEYPAD_KP_DATA_L(n)         (((n)&0xFFFFFFFF)<<0)
#define KEYPAD_KP_DATA_L_MASK       (0xFFFFFFFF<<0)
#define KEYPAD_KP_DATA_L_SHIFT      (0)

//KP_DATA_H
#define KEYPAD_KP_DATA_H(n)         (((n)&0xFFFFFFFF)<<0)
#define KEYPAD_KP_DATA_H_MASK       (0xFFFFFFFF<<0)
#define KEYPAD_KP_DATA_H_SHIFT      (0)

//KP_STATUS
#define KEYPAD_KEYIN_STATUS(n)      (((n)&0xFF)<<0)
#define KEYPAD_KEYIN_STATUS_MASK    (0xFF<<0)
#define KEYPAD_KEYIN_STATUS_SHIFT   (0)
#define KEYPAD_KP_ON                (1<<31)
#define KEYPAD_KP_ON_MASK           (1<<31)
#define KEYPAD_KP_ON_SHIFT          (31)

//KP_CTRL
#define KEYPAD_KP_EN                (1<<0)
#define KEYPAD_KP_DBN_TIME(n)       (((n)&0xFF)<<2)
#define KEYPAD_KP_ITV_TIME(n)       (((n)&0x3F)<<10)
#define KEYPAD_KP_IN_MASK(n)        (((n)&0xFF)<<16)
#define KEYPAD_KP_OUT_MASK(n)       (((n)&0xFF)<<24)

//KP_IRQ_MASK
#define KEYPAD_KP_EVT0_IRQ_MASK     (1<<0)
#define KEYPAD_KP_EVT1_IRQ_MASK     (1<<1)
#define KEYPAD_KP_ITV_IRQ_MASK      (1<<2)

//KP_IRQ_CAUSE
#define KEYPAD_KP_EVT0_IRQ_CAUSE    (1<<0)
#define KEYPAD_KP_EVT1_IRQ_CAUSE    (1<<1)
#define KEYPAD_KP_ITV_IRQ_CAUSE     (1<<2)
#define KEYPAD_KP_EVT0_IRQ_STATUS   (1<<16)
#define KEYPAD_KP_EVT1_IRQ_STATUS   (1<<17)
#define KEYPAD_KP_ITV_IRQ_STATUS    (1<<18)

//KP_IRQ_CLR
#define KEYPAD_KP_IRQ_CLR           (1<<0)




#endif
