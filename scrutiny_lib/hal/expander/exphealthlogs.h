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

#ifndef __EXPANDER_HEALTH_LOGS__H__

#define __EXPANDER_HEALTH_LOGS__H__

#define EXP_HL_SIZE_TO_READ 393216

#define     HALI_FLASH_LOG_1            (0x06)  /**< Flash Region Id for Logs, At a time one Log
                                                     Region is Active */
#define     HALI_FLASH_LOG_2            (0x07)  /**< Flash Region Id for Logs */

/* Signature that is stored at the top of each log region */
#define FW_HEALTH_LOG_FRH_SIGNATURE                 (0xBBC0FFEE)
/* Version marker for the flash region header */
#define FW_HEALTH_LOG_FRH_VERSION_0001              (0xFFFE)
/* Header size in bytes */
#define FW_HEALTH_LOG_FRH_HEADER_SIZE_0001          (0x0040)
/* Record size information */
#define FW_HEALTH_LOG_FRH_ENTRY_SIZE_0001           (0x0040)

#define FW_HEALTH_LOG_FRH_FLAG_CURRENT_REGION       (1)

typedef struct _FW_HEALTH_LOG_FRH_FLAGS
{
    union
    {
        struct _FW_HEALTH_LOG_FRH_FLAGS_BITS
        {
            U32 CurrentRegion:1;
        } Bits;

        U32 Dword;
    } u;
} FW_HEALTH_LOG_FRH_FLAGS, *PTR_FW_HEALTH_LOG_FRH_FLAGS;


/**
 * This header is stored at the top of each log flash region.
 */
typedef struct _FW_HEALTH_LOG_FLASH_REGION_HEADER
{
    U32 Signature;
    /* Use a reserved field to round the header out to 64 bytes - an even
     * increment of the log entry size.
     */
    FW_HEALTH_LOG_FRH_FLAGS Flags;
    U16 HeaderVersion;
    U16 HeaderSize;
    U16 EntrySize;
    U16 ReservedWord;
    U32 Reserved[12];
} FW_HEALTH_LOG_FLASH_REGION_HEADER, *PTR_FW_HEALTH_LOG_FLASH_REGION_HEADER;


/* The header for all log entries */

typedef struct _EXP_LOG_ENTRY_HEADER
{
    U64 Timestamp;          // 0x00
    U32 Reserved08;         // 0x08
    U16 LogSequence;        // 0x0c
    U16 LogEntryQualifier;  // 0x0e
} EXP_LOG_ENTRY_HEADER, *PTR_EXP_LOG_ENTRY_HEADER;

/* Log data header. */

typedef struct _EXP_LOG_DATA_HEADER
{
    U32 LogCode;            // 0x00
    U16 Locale;             // 0x04
    U8  ArgumentType;       // 0x06
    S8  LogClass;           // 0x07
} EXP_LOG_DATA_HEADER, *PTR_EXP_LOG_DATA_HEADER;

/* Log string header*/

typedef struct _EXP_LOG_ENTRY_STRING
{
    EXP_LOG_ENTRY_HEADER    LogEntryHeader;
    EXP_LOG_DATA_HEADER     LogDataHeader;
    char                    ArgString[40];      // 0x00
} EXP_LOG_ENTRY_STRING, *PTR_EXP_LOG_ENTRY_STRING;



SCRUTINY_STATUS ehliGetHealthLogs (
    __IN__ PTR_SCRUTINY_DEVICE       PtrDevice,
    __OUT__ PVOID                    PtrBuffer,
    __IN__ PU32                      PtrBufferLength,
    __IN__  EXP_HEALTH_LOG_TYPE      Flags,
    __IN__ const char*               PtrFolderName
    );


#endif


