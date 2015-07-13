#include "cs_types.h"

// =======================================================
// memcpy
// -------------------------------------------------------
/// Copy n bytes from src to dest
/// This implementation access to memory only with read 32 bits and write 32
/// bits. And use a buffer of 2 words for realign the destination
///
/// @param dest Destination buffer
/// @param src Source buffer
/// @param n Copy size
// =======================================================
PUBLIC VOID *memcpy(VOID *dest, CONST VOID *src, UINT32 n)
{
    INT32                     i;
    INT32                     destOffset;
    INT32                     srcOffset;
    INT32                     offset;
    INT32                     offset0;
    INT32                     offset1;
    UINT32                    v;
    UINT32*                   destAddr;
    UINT32*                   srcAddr;
    UINT32                    fifo[2];
    CONST INT32               addrMask = sizeof(UINT32)-1;

    destOffset  = (((UINT32)dest) &  addrMask) * 8;
    srcOffset   = (((UINT32)src)  &  addrMask) * 8;
    offset      = srcOffset - destOffset;

    destAddr    = (UINT32*) (((UINT32)dest) & ~addrMask);
    srcAddr     = (UINT32*) (((UINT32)src ) & ~addrMask);

    // No data to copy
    if(n == 0)
    {
        return destAddr;
    }

    // Same alignement on source and destination ?
    if(offset == 0)
    {
        // Destination offset not aligned on word ?
        if(destOffset)
        {
            // Write first bytes

            fifo[0]   = *srcAddr;
            v         = fifo[0];

            for(i = destOffset/8; i < (INT32)sizeof(UINT32) && n; ++i, --n)
            {
                ((UINT8*)destAddr)[i] = (v >> (i*8)) & 0xFF;
            }

            ++srcAddr;
            ++destAddr;
        }

        for(; n >= sizeof(UINT32); n -= sizeof(UINT32))
        {
            // Write source on destination with 32 bits access
            fifo[0]   = *srcAddr;
            v         = fifo[0];
            *destAddr = v;

            ++srcAddr;
            ++destAddr;
        }


        if(n != 0)
        {
            // Write last bytes

            fifo[0]   = *srcAddr;
            v         = fifo[0];

            for(i = 0; n; ++i, --n)
            {
                ((UINT8*)destAddr)[i] = (v >> (i*8)) & 0xFF;
            }

            ++srcAddr;
            ++destAddr;
        }
    }
    else
    {
        if(offset <= 0)
        {
            offset0 = -offset;
            offset1 = (sizeof(UINT32) * 8) + offset;
        }
        else
        {
            offset1 = offset;
            offset0 = (sizeof(UINT32) * 8) - offset;
        }

        fifo[1] = *srcAddr;
        ++srcAddr;

        // Destination offset not aligned on word ?
        if(destOffset)
        {
            // Write first bytes
            if(offset > 0)
            {
                fifo[0] = fifo[1];
                fifo[1] = *srcAddr;
                ++srcAddr;
            }

            v         = (fifo[1] << offset0) | (fifo[0] >> offset1);

            for(i = destOffset/8; i < (INT32)sizeof(UINT32) && n; ++i, --n)
            {
                ((UINT8*)destAddr)[i] = (v >> (i*8)) & 0xFF;
            }

            ++destAddr;
        }

        for(i = 0; n >= sizeof(UINT32); n -= sizeof(UINT32), i ^= 1)
        {
            // Adjust alignement between source and destination and write in
            // destination buffer with 32 bits access
            fifo[i]   = *srcAddr;
            v         = (fifo[i] << offset0) | (fifo[i^1] >> offset1);
            *destAddr = v;

            ++srcAddr;
            ++destAddr;
        }

        // End of buffer
        if(n != 0)
        {
            // Write last bytes

            fifo[i]   = *srcAddr;
            v         = (fifo[i] << offset0) | (fifo[i^1] >> offset1);

            for(i = 0; n; ++i, --n)
            {
                ((UINT8*)destAddr)[i] = (v >> (i*8)) & 0xFF;
            }

            ++srcAddr;
            ++destAddr;
        }
    }

    return (VOID*)destAddr;
}
