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

#ifndef __SCRUTINY_API__H__
#define __SCRUTINY_API__H__

#if defined (LIB_SUPPORT_CONTROLLER_IT) || defined (LIB_SUPPORT_CONTROLLER_MR)
#include "scrutinyapicontroller.h"
#endif

#if defined (LIB_SUPPORT_SWITCH)
#include "scrutinyapiswitch.h"
#endif

#if defined (LIB_SUPPORT_EXPANDER)
#include "scrutinyapiexpander.h"
#endif

/*
 * ----------------------------------------------------------------------------
 * Library Specific APIs
 * ----------------------------------------------------------------------------
 */

#if defined(OS_UEFI)

SCRUTINY_STATUS ScrutinyInitialize (
    __IN__ EFI_HANDLE ImageHandle,
    __IN__ EFI_SYSTEM_TABLE *PtrSystemTable
);

#else

SCRUTINY_STATUS ScrutinyInitialize();

#endif

SCRUTINY_STATUS ScrutinyInitializeLogging (
    __IN__ PTR_SCRUTINY_DEBUG_LOGGER PtrLogger
);

SCRUTINY_STATUS ScrutinyExit();

SCRUTINY_STATUS ScrutinyGetLibraryVersion (
    __OUT__ PU32 PtrLibraryVersion
);

SCRUTINY_STATUS ScrutinyDiscoverDevices (
    __IN__ U32                             DiscoveryFlag,
    __IN__ PTR_SCRUTINY_DISCOVERY_PARAMS   PtrDiscoveryFilter
);


SCRUTINY_STATUS ScrutinyGetDeviceCount (
    __OUT__ PU32  PtrDeviceCount
);

SCRUTINY_STATUS ScrutinyGetDeviceInfo (
    __IN__  U32                             DeviceIndex,
    __IN__  U32                             Size,
    __OUT__ PTR_SCRUTINY_DEVICE_INFO        PtrDeviceInfo
);

SCRUTINY_STATUS ScrutinyResetDevice (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle
);

SCRUTINY_STATUS ScrutinyGetFirmwareCliOutput (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __IN__ PU8                              PtrCommand,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ U32                              BufferLength,
    __IN__ char                             *PtrFolderName
);


SCRUTINY_STATUS ScrutinyGetTraceBuffer (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength
);


SCRUTINY_STATUS ScrutinyGetHealthLogs (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength,
    __IN__  EXP_HEALTH_LOG_TYPE             Flags      
);

SCRUTINY_STATUS ScrutinyGetTemperature (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE		PtrProductHandle,
    __OUT__ PU32                            PtrTemperature
);

SCRUTINY_STATUS ScrutinyGetCoreDumpDataByIndex (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrImageSize,
    __INOUT__   PU32                            PtrIndex,
    __INOUT__   PTR_SCRUTINY_CORE_DUMP_ENTRY    PtrCoreDumpEntry
);

SCRUTINY_STATUS ScrutinyEraseCoreDump (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle
);


SCRUTINY_STATUS ScrutinyGetCoreDump (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      EXP_COREDUMP_TYPE               Type
);

SCRUTINY_STATUS ScrutinyOneTimeCaptureLogs (
    __IN__ U32                                  Flags, 
    __IN__ const char*                          PtrFolderName
);



#if 0
SCRUTINY_STATUS ScrutinyiGetDeviceInfoByHandle (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __IN__ U32                              Size,
    __IN__ PTR_SCRUTINY_DEVICE_INFO         PtrDeviceInfo
);
#endif



#endif /* __SCRUTINY_API__H__ */

