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

#ifndef __SCRUTINY_API_CONTROLLER__H__
#define __SCRUTINY_API_CONTROLLER__H__


/*
 * ----------------------------------------------------------------------------
 * Controller specific APIs
 * ----------------------------------------------------------------------------
 */

SCRUTINY_STATUS ScrutinyControllerMpiPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __INOUT__ PTR_SCRUTINY_MPI_PASSTHROUGH  PtrMpiRequest
);

SCRUTINY_STATUS ScrutinyControllerDcmdPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __INOUT__ PTR_SCRUTINY_DCMD_PASSTHROUGH     PtrDcmdRequest
);

SCRUTINY_STATUS ScrutinyControllerSmpPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __INOUT__ PTR_SCRUTINY_SMP_PASSTHROUGH      PtrSmpRequest,
    __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus
);

SCRUTINY_STATUS ScrutinyControllerConfigPagePassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE  PtrProductHandle,
    __INOUT__  PTR_SCRUTINY_CONTROLLER_CFG_PASSTHROUGH  PtrConfigRequest
);

SCRUTINY_STATUS ScrutinyControllerScsiPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __IN__ U32                                  DeviceHandle,
    __INOUT__  PTR_SCRUTINY_SCSI_PASSTHROUGH    PtrScsiRequest,
    __OUT__ PTR_SCRUTINY_IOC_STATUS             PtrIocStatus
);

#if defined(OS_UEFI)

SCRUTINY_STATUS ScrutinyControllerSendDoorBellMsg (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __IN__  PU32                             PtrRequestMessage,
    __IN__  U32                              RequestSize,
    __OUT__ PU32                             PtrReplyMessage,
    __OUT__ U32                              ReplySize
    );
#endif

SCRUTINY_STATUS ScrutinyControllerGetSnapDumpProperty (
    __IN__    PTR_SCRUTINY_PRODUCT_HANDLE           PtrProductHandle, 
    __INOUT__ MR_SNAPDUMP_PROPERTIES                *PtrSnapProp,
    __INOUT__ PU8                                   PtrSnapCount);

SCRUTINY_STATUS ScrutinyControllerSetSnapDumpProperty ( 
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle, 
    __IN__ U8                                      SnapState, 
    __INOUT__ MR_SNAPDUMP_PROPERTIES              *PtrSnapProp
    );
SCRUTINY_STATUS ScrutinyControllerTriggerSnapDump  (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle);

SCRUTINY_STATUS ScrutinyControllerGetSnapDump ( 
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __IN__ U8  SnapDumpID,
    __IN__ char *PtrFoldername);

SCRUTINY_STATUS ScrutinyControllerDeleteSnapDump (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle);


SCRUTINY_STATUS ScrutinyControllerToolboxClean (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __IN__      U32                         Flags,
    __OUT__     PTR_SCRUTINY_IOC_STATUS     PtrIocStatus
);

SCRUTINY_STATUS ScrutinyControllerToolboxLaneMargin (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE         PtrProductHandle, 
    __IN__  PTR_SCRUTINY_LANE_MARGIN_REQUEST    PtrLaneMarginRequest, 
    __OUT__ PTR_SCRUTINY_IOC_STATUS             PtrIocStatus
);

SCRUTINY_STATUS ScrutinyControllerMemoryRead (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest,
    __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus
);

SCRUTINY_STATUS ScrutinyControllerMemoryWrite (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest,
    __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus
);

SCRUTINY_STATUS ScrutinyControllerSpdmOps (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle, 
    __IN__  PTR_SCRUTINY_SPDM_REQUEST       PtrSpdmRequest, 
    __OUT__ PTR_SCRUTINY_IOC_STATUS         PtrIocStatus);

SCRUTINY_STATUS ScrutinyControllerToolboxIstwiReadWrite (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __IN__ PTR_SCRUTINY_ISTWI_REQUEST PtrIstwiRequest,
    __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus
);


SCRUTINY_STATUS ScrutinyControllerGetDriveBusTarget (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE     PtrAdapterHandle,
    __IN__ PU16                            PtrDevHandle,
    __OUT__ PU32                           PtrBus,
    __OUT__ PU32                           PtrTarget
);

SCRUTINY_STATUS ScrutinyControllerGetSasPhyErrorCounters (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE         PtrProductHandle,
    __IN__ U32                                 PhyId,
    __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS    PtrSasPhyErrCounter
);

SCRUTINY_STATUS ScrutinyControllerGetPciPhyErrorCounters (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE         PtrProductHandle,
    __IN__  U32                                 PhyId,
    __OUT__ PTR_SCRUTINY_PCI_LINK_ERROR_COUNTERS PtrPciePhyErrCounter
);

SCRUTINY_STATUS ScrutinyControllerGetHealth (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__  U32                                     Flags,
    __OUT__ PTR_SCRUTINY_CONTROLLER_INVENTORY       PtrControllerHealth
);

SCRUTINY_STATUS ScrutinyControllerHealthCheck (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__  const char*                             PtrFolderName,
    __IN__  BOOLEAN                                 DumpToFile,
    __OUT__ PU32                                    PtrErrorCode
);


SCRUTINY_STATUS ScrutinyControllerSetCoffeeFault (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle
);


#if defined (OS_UEFI)

SCRUTINY_STATUS ScrutinyControllerHostboot (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __IN__ PU8 PtrFirmware,
    __IN__ U32 FirmwareLength
);

SCRUTINY_STATUS ScrutinyControllerGetIocState (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __OUT__  PU32                                    PtrState
);

SCRUTINY_STATUS ScrutinyControllerGetIocFaultCode (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __OUT__ PU32 PtrValue,
    __IN__  U32 SizeInBytes
);

#endif


#if defined (OS_WINDOWS) || defined (OS_LINUX) || defined (OS_VMWARE)

SCRUTINY_STATUS ScrutinyControllerAfdSendTriggerCommand (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__ HBA_AFD_TRIGGER_TYPE            TriggerType,
    __IN__ HBA_AFD_ACTION                  TriggerAction,
    __IN__ PVOID                           PtrBuffer,
    __IN__ PU32                            PtrSize
);

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferRegister (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__ U8                              BufferType,
    __IN__ U8                              ExtendedType,
    __IN__ U32                             BufferSize,
    __IN__ U32                             DiagnosticFlags,
    __IN__ U32                             ProductSpecific[23],
    __IN__ PU32                            PtrUniqueId
);

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferQuery (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__ U8                              BufferType,
    __IN__ PU16                            PtrFlags,
    __IN__ PU32                            PtrBufferSize,
    __IN__ PU32                            PtrUniqueId
);

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferExtendedQuery  (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__      U8                              BufferType,
    __OUT__		PTR_SCRUTINY_EXTENDED_QUERY		PtrExtendedQuery,
    __INOUT__   PU32                            PtrUniqueId
);


SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferRead (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__ U8                              BufferType,
    __IN__ PU32                            PtrUniqueId,
    __OUT__ PU32                           PtrDataBuffer,
    __OUT__ PU32                           PtrBufferSize
);

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferRelease (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__ U8                              BufferType,
    __IN__ PU32                            PtrUniqueId
);

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferUnregister (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__ U8                              BufferType,
    __IN__ PU32                            PtrUniqueId
);

#endif

SCRUTINY_STATUS ScrutinyControllerGetLogs (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __IN__ U32                                  Flags, 
    __IN__ const char*                          PtrFolderName
);



#endif /* __SCRUTINY_API_CONTROLLER__H__ */

