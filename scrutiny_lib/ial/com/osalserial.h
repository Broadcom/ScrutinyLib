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

#ifndef __OSAL_SERIAL__H__
#define __OSAL_SERIAL__H__

/**
 *
 * @brief
 *
 * Just a max. length which shall be used while reading from the
 * Serial port.
 *
 *
 */

#ifdef OS_SOLARIS
#define MAX_READ_SIZE  (2048)
#else
#define MAX_READ_SIZE  (512)
#endif

/**
 *
 * @brief
 *
 * Maximum number of COM ports any system can have.
 *
 */

#define MAX_SERIAL_PORTS   (255)


SCRUTINY_STATUS wsbiAddDataByte (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer, __IN__ U8 Data);
SCRUTINY_STATUS wsbiInitializeBuffers (__OUT__ PTR_IAL_SERIAL_BUFFER PtrBuffer);
VOID wsbiUnlockBuffer (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer);
VOID wsbiLockBuffer (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer);
SCRUTINY_STATUS wsbiGetData (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer, __OUT__ U8 *PtrData, __OUT__ U32 *PtrSize, __IN__ U32 MaxSize);
SCRUTINY_STATUS wsbiClearAndReset (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer);
SCRUTINY_STATUS wsbiFlush (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer);
SCRUTINY_STATUS wsbiAddData (__IN__ PTR_IAL_SERIAL_BUFFER PtrBuffer, __IN__ U8 *PtrData, __IN__ U32 Size);


#endif /* __OSAL_SERIAL__H__ */

