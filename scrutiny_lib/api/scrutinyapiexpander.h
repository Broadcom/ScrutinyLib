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

#ifndef __SCRUTINY_API_EXPANDER__H__
#define __SCRUTINY_API_EXPANDER__H__

/*
 * ----------------------------------------------------------------------------
 * Expander specific APIs
 * ----------------------------------------------------------------------------
 */


SCRUTINY_STATUS ScrutinyExpanderGetPhyErrCounters (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__ U32                                     NumPhys,
    __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS        PtrSasPhyErrCounter
);

SCRUTINY_STATUS ScrutinyExpanderGetConfigPage (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__  U16                             PageNumber,
    __IN__  U8                              Region,
    __OUT__ PU8                             PtrPageBuffer,
    __OUT__ PU32                            PtrPageBufferLength
);


SCRUTINY_STATUS ScrutinyExpanderScsiPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH       PtrScsiRequest
);


SCRUTINY_STATUS ScrutinyExpanderEnableDisablePhy (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__ U8                                      PhyIdentifier,
    __IN__ BOOLEAN                                 PhyEnable
);

SCRUTINY_STATUS ScrutinyExpanderMemoryRead (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __IN__ U32 Address,
    __OUT__ PVOID PtrValue,
    __IN__ U32 SizeInBytes
);

SCRUTINY_STATUS ScrutinyExpanderMemoryWrite (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __IN__ U32 Address,
    __IN__ PVOID PtrValue,
    __IN__ U32 SizeInBytes
);

SCRUTINY_STATUS ScrutinyExpanderResetPhy (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__ U8                                      PhyIdentifier,
    __IN__ U8                                      PhyOperation,
    __IN__ BOOLEAN                                 DoAllPhys
);

SCRUTINY_STATUS ScrutinyExpanderGetPhyErrorCounters (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__  U8                                      PhyIdentifier,
    __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS         PtrSasPhyErrCounter
);

SCRUTINY_STATUS  ScrutinyExpanderHealthCheck ( 
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE              PtrProductHandle,
    __IN__   const char*                              PtrConfigFilePath,
    __IN__   BOOLEAN                                  DumpToFile,
    __OUT__  PU32                                     PtrErrorCode
);


SCRUTINY_STATUS  ScrutinyExpanderGetHealth ( 
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __OUT__  PTR_SCRUTINY_EXPANDER_HEALTH       PtrExpHealthInfo
);


SCRUTINY_STATUS ScrutinyExpanderGetLogs (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __IN__ U32                                  Flags, 
    __IN__ const char*                          PtrFolderName
);


#endif /* __SCRUTINY_API_EXPANDER__H__ */

