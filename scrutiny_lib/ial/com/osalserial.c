/*
 *
 * Copyright (C) 2019 - 2020 Broadcom.
 * The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1.Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 *
 * 2.Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * 3.Neither the name of the copyright holder nor the names of its contributors
 *   may be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
*/


#include "libincludes.h"

/**
 *
 * @name    wsbInitializeBuffers()
 *
 * @param   PtrBuffer       The serial buffer handle which needs to be
 *                          intialized.
 *
 * @return  SCRUTINY_STATUS_SUCCESS - On success
 *
 * @brief   Critical section and the serial buffer will be initialized.
 *
 */

SCRUTINY_STATUS wsbiInitializeBuffers (__OUT__ PTR_IAL_SERIAL_BUFFER PtrBuffer)
{

    #if defined (OS_WINDOWS)
        InitializeCriticalSection (&PtrBuffer->CriticalSectionHandle);
    #elif defined (OS_LINUX)
        sem_init (&PtrBuffer->CriticalSectionHandle, 0, 1);
    #endif

    PtrBuffer->CurrentPosition   = 0;
    PtrBuffer->BytesUnRead       = 0;
    PtrBuffer->PtrBuffer         = NULL;

    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 * @name    wsbiAddDataByte()
 *
 * @param   PtrBuffer       The buffer to which the read data stored.
 *
 * @param   Data            One byte data that needs to be stored.
 *
 * @return  SCRUTINY_STATUS_SUCCESS - On success
 *
 * @brief   Only one read byte of data to be stored.
 *
 */

SCRUTINY_STATUS wsbiAddDataByte (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer, __IN__ U8 Data)
{
    return (wsbiAddData (PtrBuffer, &Data, 1));
}

/**
 *
 * @name    wsbiAddData()
 *
 * @param   PtrBuffer       The buffer to which the read data stored.
 *
 * @param   PtrData         Data that needs to be stored.
 *
 * @param   Size            Length of the data (PtrData).
 *
 * @return  SCRUTINY_STATUS_SUCCESS - On success
 *
 * @brief   The read serial data will be stored into the Serial Buffer.
 *
 *
 */

SCRUTINY_STATUS wsbiAddData (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer, __IN__ U8 *PtrData, __IN__ U32 Size)
{

    U32 size = 0;

    PU8 ptrResult = NULL;

    size = PtrBuffer->BytesUnRead + Size;

    ptrResult = (PU8) sosiMemAlloc (size);

    if (PtrBuffer->PtrBuffer)
    {
        sosiMemCopy (&ptrResult[0], PtrBuffer->PtrBuffer, PtrBuffer->BytesUnRead);
        sosiMemCopy (&ptrResult[PtrBuffer->BytesUnRead], PtrData, Size);
    }

    else
    {
        sosiMemCopy (&ptrResult[0], PtrData, Size);
    }

    if (PtrBuffer->PtrBuffer)
    {
        sosiMemFree (PtrBuffer->PtrBuffer);
    }

    PtrBuffer->PtrBuffer = ptrResult;
    PtrBuffer->BytesUnRead += Size;

    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 * @name    wsbiFlush()
 *
 * @param   PtrBuffer       The buffer handle which needs the reset.
 *
 * @return  SCRUTINY_STATUS_SUCCESS - On success
 *
 * @brief   The specified buffer will be freed and gets a reset.
 *
 *
 */

SCRUTINY_STATUS wsbiFlush (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer)
{
    /*
     * Enter into critical section
     */

    wsbiLockBuffer (PtrBuffer);

    wsbiClearAndReset (PtrBuffer);

    /*
     * Leave aside critical section
     */

    wsbiUnlockBuffer (PtrBuffer);

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    wsbiClearAndReset()
 *
 * @param   PtrBuffer       The buffer handle which needs the reset.
 *
 * @return  SCRUTINY_STATUS_SUCCESS - On success
 *
 * @brief   The specified buffer will be freed and gets a reset.
 *
 *
 */

SCRUTINY_STATUS wsbiClearAndReset (__INOUT__ PTR_IAL_SERIAL_BUFFER PtrBuffer)
{

    if (PtrBuffer->PtrBuffer)
    {
        sosiMemFree (PtrBuffer->PtrBuffer);
        PtrBuffer->PtrBuffer = NULL;
    }

    PtrBuffer->BytesUnRead      = 0;
    PtrBuffer->CurrentPosition  = 0;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    wsbiGetData()
 *
 * @param   PtrBuffer       The buffer from which the data has to be retrieved.
 *
 * @param   PtrData         Buffer to which the read data to be stored into.
 *
 * @param   PtrSize         Read buffer size.
 *
 * @param   MaxSize         Max Size of the buffer that has to be read.
 *
 * @return  SCRUTINY_STATUS_SUCCESS - On success and SCRUTINY_STATUS_FAILED  - On failure.
 *
 * @brief   The data will be retrieved from the buffer and stored into the user
 *          specified buffer. If the MaxData specified value is not present, the
 *          method will hold for a while and returns the available data.
 *          If the requested amount of the data is not present and buffer
 *          remains zero for the MAX TIMEOUT value, Error message will be
 *          thrown.
 *
 */

#ifdef OS_WINDOWS
#define MAX_READ_CYCLES_FOR_GET_DATA    (0xA00000)
#else
#define MAX_READ_CYCLES_FOR_GET_DATA    (10000)
#endif


SCRUTINY_STATUS wsbiGetData (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer, __OUT__ U8 *PtrData, __OUT__ U32 *PtrSize, __IN__ U32 MaxSize)
{

    U32 size = 0;
    PU8 ptrResult;
    volatile U32 retryCount = 0;
    volatile U32 lastDataCount = 0;

    /* Hold and wait here */
    do
    {

        if (PtrBuffer->BytesUnRead >= MaxSize)
        {
            break;
        }

        if (lastDataCount == PtrBuffer->BytesUnRead)
        {
            retryCount++;
        }

        else
        {
            retryCount = 0;
            lastDataCount = PtrBuffer->BytesUnRead;
        }

        #if defined (OS_WINDOWS)
            SwitchToThread();
            sosiSleep (0);
        #else
            sosiSleep (0);
        #endif

    } while (retryCount < MAX_READ_CYCLES_FOR_GET_DATA);

    wsbiLockBuffer (PtrBuffer);

    size = PtrBuffer->BytesUnRead > MaxSize ? MaxSize : PtrBuffer->BytesUnRead;

    sosiMemCopy (PtrData, PtrBuffer->PtrBuffer, size);

    *PtrSize = size;

    if (size < PtrBuffer->BytesUnRead)
    {
        ptrResult = (PU8) sosiMemAlloc (PtrBuffer->BytesUnRead - size);
        sosiMemCopy (ptrResult, &PtrBuffer->PtrBuffer[size], PtrBuffer->BytesUnRead - size);

        sosiMemFree (PtrBuffer->PtrBuffer);

        PtrBuffer->PtrBuffer = ptrResult;
        PtrBuffer->BytesUnRead = PtrBuffer->BytesUnRead - size;

    }

    else if (size > PtrBuffer->BytesUnRead)
    {
        /* Something wrong, we got request more than the size */
    }

    else
    {
        wsbiClearAndReset (PtrBuffer);
    }

    /*
     * Set the size and buffer
     */

    wsbiUnlockBuffer (PtrBuffer);

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @name    wsbiLockBuffer()
 *
 * @param   PtrBuffer       Serial buffer which needs to be locked with the
 *                          critical sections.
 *
 * @return  None
 *
 * @brief   If there is a read data occured, the buffer will have to be locked
 *          and then stored in the buffer.
 *
 */

VOID wsbiLockBuffer (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer)
{

    #if defined (OS_WINDOWS)
        EnterCriticalSection (&PtrBuffer->CriticalSectionHandle);
    #elif defined (OS_LINUX)
        sem_wait (&PtrBuffer->CriticalSectionHandle);
    #endif

}

/**
 *
 * @name    wsbiUnlockBuffer()
 *
 * @param   PtrBuffer       Serial buffer which needs to be unclocked from the
 *                          critical sections.
 *
 * @return  None
 *
 * @brief   This method will unlock the buffer or the data that needs to be
 *          stored into the buffer has been compelted.
 *
 */

VOID wsbiUnlockBuffer (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer)
{
    #if defined (OS_WINDOWS)
        LeaveCriticalSection (&PtrBuffer->CriticalSectionHandle);
    #elif defined (OS_LINUX)
        sem_post (&PtrBuffer->CriticalSectionHandle);
    #endif
}

