#include "cs_types.h"

#include "stdarg.h"
#include "string.h"
#include "stdio.h"

#include "sxs_io.h"
#include "sxs_srl.h"

#include "dbg.h"

#define DBG_TRACE_MAX_LEVEL  16

PRIVATE UINT8 g_dbgTraceStart = 0;

// =============================================================================
// dbg_TraceOpen
// -----------------------------------------------------------------------------
/// Open Trace Module
// =============================================================================
DBG_ERR_T dbg_TraceOpen(BOOL clearLevels) 
{
    g_dbgTraceStart       = 1;
    if (clearLevels)
    {
        sxs_IoCtx.TraceBitMap [TGET_ID(_APP)] = 0;
    }
    return DBG_ERR_NO;
}

// =============================================================================
// dbg_TraceClose
// -----------------------------------------------------------------------------
// Close Trace Module
// =============================================================================
DBG_ERR_T dbg_TraceClose() 
{
    g_dbgTraceStart       = 0;
    return DBG_ERR_NO;
}

// =============================================================================
// dbg_TraceSetOutputMask
// -----------------------------------------------------------------------------
// Set enabled trace level
// =============================================================================
DBG_ERR_T dbg_TraceSetOutputMask(UINT16 nIndex, BOOL bMode)
{
    if(nIndex >= DBG_TRACE_MAX_LEVEL)
        return DBG_ERR_UNSUPPORTED;
 
    if (bMode)
    {
        sxs_IoCtx.TraceBitMap [TGET_ID(_APP)] |= (1<<nIndex);
    }
    else
    {
        sxs_IoCtx.TraceBitMap [TGET_ID(_APP)] &= ~(1<<nIndex);
    }

    return DBG_ERR_NO;
}


// =============================================================================
// dbg_TraceVariadicOutputText
// -----------------------------------------------------------------------------
// Output a trace, variadic format.
// =============================================================================
PUBLIC DBG_ERR_T dbg_TraceVariadicOutputText(UINT16 nIndex, PCSTR fmt, va_list ap)
{
    if(g_dbgTraceStart == 0)
    {
        return DBG_ERR_UNSUPPORTED;
    }
    
    if(nIndex >= DBG_TRACE_MAX_LEVEL)
    {
        return DBG_ERR_UNSUPPORTED;
    }
    
    CHAR uartBuf[256];

    vsnprintf(uartBuf, sizeof(uartBuf), fmt, ap);

    if (nIndex == 0x00)
    {
        // Forced trace
        SXS_TRACE(_APP | TSTDOUT, uartBuf);		
    }
    else
    {
        // Regular trace
        SXS_TRACE(_APP | TLEVEL(nIndex), uartBuf);		
    }
    
    return DBG_ERR_NO;
}

// =============================================================================
// dbg_TraceOutputText
// -----------------------------------------------------------------------------
// Output a trace
// =============================================================================
PUBLIC DBG_ERR_T dbg_TraceOutputText(UINT16 nIndex, PCSTR fmt, ...)
{
    DBG_ERR_T returnStatus;
    va_list ap;

    va_start(ap, fmt);
    returnStatus = dbg_TraceVariadicOutputText(nIndex, fmt, ap);
    va_end(ap);
    
    return returnStatus;
}


