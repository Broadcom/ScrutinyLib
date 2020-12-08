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

#ifndef __LINUX_SERIAL__H__
#define __LINUX_SERIAL__H__

/**
 *
 * @brief
 *
 * Serial port handle for Linux specific operations. This structure contains the
 * main handle information for native system calls, buffers and the
 * configuraitons used for opening the serial port.
 *
 *
 */


/**
 *
 * @brief
 *
 * When the Serial port read occurs and when the data is excessively returned
 * large for certial communications/commands issued to the device, the buffer
 * gets overlapped and overwritten. To avoid the scenario, we have the internal
 * buffer handler mechanism which immediately reads the serial port and flushes
 * the serial port data. The data can be read in a whole by a decent wait time.
 * The following strucutre definition contains the buffer information for the
 * Windows Serial port.
 *
 *
 */

typedef struct __IAL_SERIAL_BUFFER
{

    U8*                 PtrBuffer;              /** Buffer data where the Serial read will be stored into. */

    sem_t               CriticalSectionHandle;  /** Critial section to lock the buffer when there is a change. */

    volatile U32        CurrentPosition;        /** Current position of the buffer. */
    volatile U32        BytesUnRead;            /** How many number of bytes which are not read */

} IAL_SERIAL_BUFFER, *PTR_IAL_SERIAL_BUFFER;


typedef struct __IAL_SERIAL_HANDLE
{

    int ReadHandle;             /** Handle which is used for reading the serial port. */
    int WriteHandle;            /** Handle which is used for writing into the serial port */

    pthread_t ReadThread;       /** Thread handle for the Reading serial port. */

    SCRUTINY_IAL_SERIAL_CONFIG SerialConfig; /** Common Scrutiny OSAL specific Serial configuration */

    PTR_IAL_SERIAL_BUFFER PtrSerialBuffer;  /** Buffer handle */

}SCRUTINY_IAL_SERIAL_HANDLE, *PTR_SCRUTINY_IAL_SERIAL_HANDLE;


SCRUTINY_STATUS spiOpenPort (__IN__ PTR_SCRUTINY_IAL_SERIAL_CONFIG PtrConfig, __OUT__ PTR_SCRUTINY_IAL_SERIAL_HANDLE *PtrHandle);
SCRUTINY_STATUS spiClosePort (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialParams);
SCRUTINY_STATUS spiReadData (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialParams, __OUT__ U8 *PtrData, __OUT__ U32* PtrReadSize, __IN__ U32 MaxSize);
SCRUTINY_STATUS spiWriteData (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialParams, __IN__ U8 *PtrData, __IN__ U32 Size, __OUT__ U32 *PtrWritten);
SCRUTINY_STATUS spiFlushPort (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrSerialParams);
SCRUTINY_STATUS spiCreateJsonSerial (__IN__ PTR_SCRUTINY_IAL_SERIAL_HANDLE PtrHandle);

#endif /* __LINUX_SERIAL__H__ */

