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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_pwm.c $ //
//    $Author: admin $                                                        //
//    $Date: 2012-09-28 13:32:55 +0800 (周五, 28 九月 2012) $                     //
//    $Revision: 17191 $                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_pwm                                                         //
///                                                                           //
///     This file contains Garnet's PWM driver implementation          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"

#include "global_macros.h"
#include "pwm.h"
#include "sys_ctrl.h"

#include "hal_pwm.h"
#include "halp_pwm.h"
#include "halp_sys.h"
#include "halp_debug.h"

#include "hal_config.h"
#include "halp_config.h"



// =============================================================================
// hal_PwmUpdateDivider
// -----------------------------------------------------------------------------
/// Update the PWM divider to cope with the new system clock.
// =============================================================================
PRIVATE VOID hal_PwmUpdateDivider(HAL_SYS_FREQ_T freq)
{
    UINT32 divider;

    divider = freq / (HAL_SYS_FREQ_13M / HAL_PWM_BASE_DIVIDER);
    if (divider > 0)
    {
        divider -= 1;
    }
    if (divider > 0xFF)
    {
        divider = 0xFF;
    }

    hal_SysSetupClkPwm(divider);
}


// =============================================================================
// hal_PwmResourceMgmt
// -----------------------------------------------------------------------------
/// Checks if any components in the PWM module are active and requests or
/// releases the resource.  The return value is TRUE if the resource is
/// active and FALSE otherwise.
// =============================================================================
PROTECTED BOOL hal_PwmResourceMgmt(VOID)
{
    // If ANY PWL or PWT is active, request resource, otherwise
    // release resource
    if ( (hwp_pwm->PWL0_Config & PWM_PWL0_EN_H) != 0 ||
         (hwp_pwm->PWL1_Config & PWM_PWL1_EN_H) != 0 ||
         (hwp_pwm->PWT_Config & PWM_PWT_ENABLE) != 0 )
    {
        hal_SysRequestFreq(HAL_SYS_FREQ_PWM, HAL_SYS_FREQ_26M, hal_PwmUpdateDivider);

        UINT32 scStatus = hal_SysEnterCriticalSection();
        hal_PwmUpdateDivider(hal_SysGetFreq());
        hal_SysExitCriticalSection(scStatus);
        return TRUE;
    }
    else
    {
        hal_SysRequestFreq(HAL_SYS_FREQ_PWM, HAL_SYS_FREQ_32K, NULL);
        return FALSE;
    }
}

// =============================================================================
// hal_BuzzStart
// -----------------------------------------------------------------------------
/// This function makes the PWM module output a tone.
/// @param noteFreq This is the desired frequency for the generated
/// tone. (Minimum 152 Hz)
/// @param level Generated tone rough level, from 0 (mute) to 100 (full level).
// =============================================================================
PUBLIC VOID hal_BuzzStart(UINT16 noteFreq, UINT16 level)
{
    // noteDiv is the divider value to produce a tone of a given frequency.
    // To calculate the noteDiv value, Use the following formula:
    //
    //         noteDiv = FBASE/FNOTE
    //
    // where FBASE is the frequency of the PWM module clock (at the moment, it is
    // the system frequency, 26 or 39 MHz divided by 5).  FNOTE is the frequency of
    // the desired tone.
    //
    // The dutyCmp value can be used to set the approximate volume of the tone.
    // The dutyCmp value must be less than or equal to half the noteDiv value and
    // must be at least a value of 0x8, otherwise no tone will be generated.
    //
    UINT32 noteDiv;
    UINT32 dutyCmp;

    if (level == 0)
    {
        hwp_pwm->PWT_Config = 0;
    }
    else
    {
        noteDiv = HAL_PWM_FREQ/noteFreq;
        dutyCmp = (noteDiv * level / 100)/2;

        hwp_pwm->PWT_Config =
            (PWM_PWT_PERIOD(noteFreq) | PWM_PWT_DUTY(dutyCmp) | PWM_PWT_ENABLE);
    }
    hal_PwmResourceMgmt();
}

// =============================================================================
// hal_BuzzStop
// -----------------------------------------------------------------------------
/// Stop the PWM module from outputting a tone
// =============================================================================
PUBLIC VOID hal_BuzzStop(VOID)
{
    hwp_pwm->PWT_Config = 0;
    hal_PwmResourceMgmt();
}

// =============================================================================
// hal_PwlGlow
// -----------------------------------------------------------------------------
/// Set the average on time for the PWL0 output.
/// Thus, the integrated output represents a certain luminosity level
/// It can also be operated in pulse mode between two boundary values.
/// The time span between those boundaries is the \c pulsePeriod
///
/// @param  levelMin The average on-time can be set via this value if we
/// don't operate in pulse mode. A value of 
/// 0 will force the PWL output to 0 while a value of 0xff will force the 
/// value to 1. All intermediate values will generate an average duty cycle 
/// of (level/255 * 100). \n
/// If in pulse mode, this is the lower boundary for PWL pulse
/// @param levelMax Ignored if not in pulse mode, this is the upper 
/// boundary for PWL pulse
/// @param pulsePeriod Ignored if not in pulse mode, this is the period of
/// pulse when pulsing is enabled
/// @param pulse Enable the pulse mode if \c TRUE.
// =============================================================================
PUBLIC VOID hal_PwlGlow(UINT8 levelMin, UINT8 levelMax,
                       UINT8 pulsePeriod, BOOL pulse)
{
    UINT32    Val;
    Val = PWM_PWL0_SET_OE;        // Always enable
    
    if (pulse)
    
    {   
        // We operate in pulse mode
        Val |= PWM_PWL_MIN(levelMin) | PWM_PWL_MAX(levelMax)
                | PWM_PWL_PULSE_PER(pulsePeriod) | PWM_PWL_PULSE_EN 
                | PWM_PWL0_EN_H;
    }
    
    else
    
    {
        // we operate in normal mode
        if (levelMin == 0)
        {
            Val |= PWM_PWL0_FORCE_L;
        }
        else if (levelMin == 0xff)
        {
            Val |= PWM_PWL0_FORCE_H;
        }
        else
        {
            Val |= (PWM_PWL0_EN_H | PWM_PWL_MIN(levelMin));
        }
    }
    hwp_pwm->PWL0_Config = Val;
    hal_PwmResourceMgmt();
}

// =============================================================================
// hal_PwlSelLevel
// -----------------------------------------------------------------------------
///  Set the average on time for the selected PWL output
/// Thus, the integrated output represents a certain luminosity level
/// 
/// @param  level The average on-time can be set via this value. A value of 
/// 0 will force the PWL output to 0 while a value of 0xff will force the 
/// value to 1. All intermediate values will generate an average duty cycle 
/// of (level/255 * 100)
// =============================================================================
PUBLIC VOID hal_PwlSelLevel(HAL_PWL_ID_T id, UINT8 level)
{
    UINT32    Val;
    // Check Param validity
    if (HAL_PWL_NONE == id)
    {
        return;
    }
    // check glowing
    HAL_ASSERT(g_halCfg->pwmCfg.pwlGlowPosition != id, "hal_PwlSelLevel: PWL Id (%d) is configured as Glowing", id);
    // is ID valid ?
    HAL_ASSERT((HAL_PWL_0 == id) || (HAL_PWL_1 == id), "hal_PwlSelLevel: Invalid PWL Id (%d)",id);
    // PWL 1 and not inverted or PWL 0 and inverted
    // inverted is glow on PWL_1
    // boolean xor is equivalent to != (but only with plain boolean values)
    if ( (HAL_PWL_1 == g_halCfg->pwmCfg.pwlGlowPosition) != (HAL_PWL_1 == id))
    {
        Val = PWM_PWL1_SET_OE; // Always enable
        if (level == 0)
        {
            Val |= PWM_PWL1_FORCE_L;
        }
        else if (level == 0xff)
        {
            Val |= PWM_PWL1_FORCE_H;
        }
        else
        {
            Val |= (PWM_PWL1_EN_H | PWM_PWL1_THRESHOLD(level));
        }
        hwp_pwm->PWL1_Config = Val;
    }
    else
    {
        Val = PWM_PWL0_SET_OE; // Always enable
        if (level == 0)
        {
            Val |= PWM_PWL0_FORCE_L;
        }
        else if (level == 0xff)
        {
            Val |= PWM_PWL0_FORCE_H;
        }
        else
        {
            Val |= (PWM_PWL0_EN_H | PWM_PWL_MIN(level));
        }
        hwp_pwm->PWL0_Config = Val;
    }
    hal_PwmResourceMgmt();
}


// =============================================================================
// hal_LpgStart
// -----------------------------------------------------------------------------
/// This function starts the LPG with the appopriate settings for period and
/// on time.
///
/// @param period This sets the LPG period
/// @param onTime This sets the LPG on time
// =============================================================================
PUBLIC VOID hal_LpgStart(HAL_LPG_PERIOD_T period, HAL_LPG_ON_T onTime)
{
    // Check arg validity
    // FIXME
    /*
    HAL_ASSERT(((Period>=LPG_PER_125) && (Period<LPG_PER_QTY)),
               Period,
               "Improper LPG period" );
    HAL_ASSERT(((OnTime>=LPG_ON_01) && (OnTime<LPG_ON_QTY)),
               OnTime,
               "Improper LPG On-Time" );
    */
    hwp_pwm->LPG_Config = 0;
    hwp_pwm->LPG_Config = PWM_LPG_PERIOD(period)
                             | PWM_LPG_ONTIME(onTime)
                             | PWM_LPG_RESET_L;
}

// =============================================================================
// hal_LpgStop
// -----------------------------------------------------------------------------
/// Stops the LPG module.
// =============================================================================
PUBLIC VOID hal_LpgStop(VOID)
{
    hwp_pwm->LPG_Config = 0;
}

// =============================================================================
// hal_PwlGlowingOutputDisable
// -----------------------------------------------------------------------------
/// Disables the output of the glowing Pwl (forced to 0), or reenable it.
/// @param disable If \c TRUE, the output is disabled,
///                if \c FALSE, the output returns to its normal behaviour.
// =============================================================================
PUBLIC VOID hal_PwlGlowingOutputDisable(BOOL disable)
{
    if (disable)
    {
        hwp_pwm->PWL0_Config = PWM_PWL0_CLR_OE;
    }
    else
    {
        hwp_pwm->PWL0_Config = PWM_PWL0_SET_OE;
    }
}



// =============================================================================
// hal_PwlOutputDisable
// -----------------------------------------------------------------------------
/// Disables the output of the selected Pwl (forced to 0), or reenable it.
/// @param disable If \c TRUE, the output is disabled,
///                if \c FALSE, the output returns to its normal behaviour.
// =============================================================================
PUBLIC VOID hal_PwlOutputDisable(HAL_PWL_ID_T id, BOOL disable)
{
    if (HAL_PWL_NONE == id)
    {
        return;
    }
    // check glowing
    HAL_ASSERT(g_halCfg->pwmCfg.pwlGlowPosition != id, "hal_PwlOutputDisable: PWL Id (%d) is configured as Glowing", id);
    // is ID valid ?
    HAL_ASSERT((HAL_PWL_0 == id) || (HAL_PWL_1 == id), "hal_PwlOutputDisable: Invalid PWL Id (%d)",id);
    // PWL 1 and not inverted or PWL 0 and inverted
    if ( (HAL_PWL_1 == g_halCfg->pwmCfg.pwlGlowPosition) != (HAL_PWL_1 == id))
    {
        if (disable)
        {
            hwp_pwm->PWL1_Config = PWM_PWL1_CLR_OE;
        }
        else
        {
            hwp_pwm->PWL1_Config = PWM_PWL1_SET_OE;
        }
    }
    else
    {
        if (disable)
        {
            hwp_pwm->PWL0_Config = PWM_PWL0_CLR_OE;
        }
        else
        {
            hwp_pwm->PWL0_Config = PWM_PWL0_SET_OE;
        }
    }
}



// =============================================================================
// hal_PwlSetGlowing
// -----------------------------------------------------------------------------
/// Set which pin is the glowing pwl
// =============================================================================
PROTECTED VOID hal_PwlSetGlowing(HAL_PWL_ID_T id)
{
    // if pwl used for glow is PWL_1 we need to invert !
    if (HAL_PWL_1 == id)
    {
        hwp_pwm->PWL0_Config = PWM_PWL_SET_MUX;
    }
    else
    {
        hwp_pwm->PWL0_Config = PWM_PWL_CLR_MUX;
    }
}



