


#include "cs_types.h"

#include "global_macros.h"
#include "sys_irq.h"

#include "boot_usb.h"
#include "bootp_debug.h"


// =============================================================================
// boot_IrqDispatch
// -----------------------------------------------------------------------------
/// BOOT romed IRQ dispatch.
/// Exceptions are handled in the rom (boot_rom.S) assemlby code.
/// Here, we check the cause is really USB before calling the USB handler,
/// otherwise sending an error if the interrupt cause is not supported.
// =============================================================================
PROTECTED VOID boot_IrqDispatch(VOID)
{
    if (hwp_sysIrq->Cause & SYS_IRQ_SYS_IRQ_USBC)
    {
        // Call the USB handler if, and only if,
        // the interruption is really for the USB.
        boot_UsbIrqHandler(SYS_IRQ_USBC);
    }
    else
    {
        // We received an unsupported (here) interruption
        BOOT_PROFILE_PULSE(BOOT_ROMED_XCPU_ERROR);

        // Stay here.
        while (1);
    }
}

