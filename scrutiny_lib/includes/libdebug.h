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

#ifndef __SCRUTINYLIBDEBUG__H__
#define __SCRUTINYLIBDEBUG__H__


/* Debug levels are 4 bit currently*/
#define SCRUTINY_DEBUG_LOG_LEVEL_NONE       (0)
#define SCRUTINY_DEBUG_LOG_LEVEL_DEBUG      (1)
#define SCRUTINY_DEBUG_LOG_LEVEL_VERBOSE    (2)
#define SCRUTINY_DEBUG_LOG_LEVEL_FUNCTIONS  (4)

typedef struct __SCRUTINY_DEBUG_LOGGER_INTERNAL *PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL;
typedef struct __SCRUTINY_DEBUG_LOGGER_INTERNAL SCRUTINY_DEBUG_LOGGER_INTERNAL;

struct __SCRUTINY_DEBUG_LOGGER_INTERNAL
{

    VOID (*logiDebug) (const char*, ...);

    VOID (*logiVerbose) (const char*, ...);

    VOID (*logiFunctionEntry) (const char*, ...);

    VOID (*logiFunctionExit) (const char*, ...);

    VOID (*logiDumpMemoryInVerbose) (PU8 PtrBuffer, U32 SizeInBytes, PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule);

};

VOID ldliInitializeDefaultLogging();
VOID ldliInitializeLogging (__IN__ PTR_SCRUTINY_DEBUG_LOGGER PtrLogger);
VOID ldliInitializeIniFileLogging (__IN__ U32   DebugLevel);

VOID ldliEmptyHearse (const char *PtrArguments, ...);
VOID ldliEmptyHearseDumpHexMemory (PU8 PtrBuffer, U32 SizeInBytes, PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule);
VOID ldliDumpHexMemory (PU8 PtrBuffer, U32 SizeInBytes, PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule);




#endif /* __SCRUTINYLIBDEBUG__H__*/

