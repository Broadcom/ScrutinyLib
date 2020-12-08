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


#ifndef __EXPANDER_TRACE__H__

#define __EXPANDER_TRACE__H__

#define TRACE_EMBEDDED_STRING_HEADER_OFFSET     (16)
#define TRACE_EMBEDDED_STRING_HEADER_LENGTH     (16)

typedef struct __SCRUTINY_EXP_TRACE_STRING
{

    U8  Subsystem;

    U32 TotalStrings;

    U32 StringsLength;

    U32 TraceMask;

    PU8 PtrEmbeddedStrings;

} SCRUTINY_EXP_TRACE_STRING, *PTR_SCRUTINY_EXP_TRACE_STRING;


typedef struct __SCRUTINY_EXP_TRACE_BUFFER
{

	U32  EmbeddedStringsLengthInBytes;
	U32  SubsytemCounts;

    PTR_SCRUTINY_EXP_TRACE_STRING PtrEmbeddedStrings[32];

    BOOLEAN StringsIncluded;

    U32  TraceVersion;
    U32  TraceLength;

} SCRUTINY_EXP_TRACE_BUFFER, *PTR_SCRUTINY_EXP_TRACE_BUFFER;



SCRUTINY_STATUS etiGetTraceBuffer (
    __IN__ PTR_SCRUTINY_DEVICE       PtrDevice,
    __OUT__ PVOID                    PtrBuffer,
    __IN__ PU32                      PtrBufferLength,
    __IN__ const char*               PtrFolderName
    );

#endif

