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

#ifndef __LIB_INTERNAL__H__
#define __LIB_INTERNAL__H__

typedef VOID (*ScrutinyLogCallBack) (const char* PtrLogEntry, ...);


SCRUTINY_STATUS slibiInitializeLibrary();
SCRUTINY_STATUS slibiExitLibrary();
SCRUTINY_STATUS slibiInitializeLogging (__IN__ PTR_SCRUTINY_DEBUG_LOGGER PtrLogger);
SCRUTINY_STATUS slibiResetDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS slibiGetFirmwareCliOutput (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrCommand, __OUT__ PVOID PtrBuffer, __IN__ U32 BufferLength, __IN__ char *PtrFolderName);
SCRUTINY_STATUS slibiProductHandleToScrutinyDevice (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__ PTR_SCRUTINY_DEVICE *PPtrDevice);

SCRUTINY_STATUS slibiDiscoverDevices (__IN__ U32 DiscoveryFlag, __IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrDiscoveryFilter);


SCRUTINY_STATUS slibiGetTemperatureValue ( __IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PU32  PtrTemperature);

#if !defined (OS_VMWARE) 

#if defined (LIB_SUPPORT_CONTROLLER)
SCRUTINY_STATUS slibiControllerGetSnapDumpProperty (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ MR_SNAPDUMP_PROPERTIES *PtrSnapProp, __INOUT__  U8  *PtrSnapDumpCount);
SCRUTINY_STATUS slibiControllerSetSnapDumpProperty (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8  SnapState, __INOUT__ MR_SNAPDUMP_PROPERTIES *PtrSnapProp);
SCRUTINY_STATUS slibiControllerGetSnapDump(__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8  SnapDumpId, char *PtrFoldername);
SCRUTINY_STATUS slibiControllerTriggerSnapDump(__IN__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS slibiControllerDeleteSnapDump (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);
#endif
#endif

SCRUTINY_STATUS slibiOneTimeCaptureLogs (
    __IN__ PTR_SCRUTINY_DEVICE                PtrDevice,
    __IN__ U32                                Flags, 
    __IN__ const char*                        PtrFolderName
);
SCRUTINY_STATUS slibiCreateTargetFolder (
	__IN__  PTR_SCRUTINY_DEVICE		PtrDevice,    
    __IN__  const char*             PtrFolderName,
    __OUT__ char 					*PtrDirPath
);

SCRUTINY_STATUS slibiCreateFolder (
	__IN__  PTR_SCRUTINY_DEVICE		PtrDevice,    
    __IN__  const char*             PtrFolderName,
    __OUT__ char* 					PtrDirPath    
);

SCRUTINY_STATUS slibiControllerGetLogs (
    __IN__ PTR_SCRUTINY_DEVICE                PtrDevice,
    __IN__ U32                                Flags, 
    __IN__ const char*                        PtrFolderName
);

SCRUTINY_STATUS slibiGetCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      EXP_COREDUMP_TYPE               Type,
    __IN__      const char*                     PtrFolderName
);



SCRUTINY_STATUS slibiGetCoreDumpDataByIndex (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrImageSize,
    __INOUT__   PU32                            PtrIndex,
    __INOUT__   PTR_SCRUTINY_CORE_DUMP_ENTRY    PtrCoreDumpEntry
);

SCRUTINY_STATUS slibiEraseCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice
);


SCRUTINY_STATUS slibiGetTraceBuffer (
    __IN__ PTR_SCRUTINY_DEVICE              PtrDevice,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength,
    __IN__ const char*                      PtrFolderName
);

SCRUTINY_STATUS slibiGetHealthLogs (
    __IN__ PTR_SCRUTINY_DEVICE              PtrDevice,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength,
    __IN__  EXP_HEALTH_LOG_TYPE             Flags,
    __IN__ const char*                      PtrFolderName
);



SCRUTINY_STATUS slibiControllerMpiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_MPI_PASSTHROUGH PtrMpiRequest);
SCRUTINY_STATUS slibiControllerDcmdPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_DCMD_PASSTHROUGH PtrDcmdRequest);
SCRUTINY_STATUS slibiControllerSmpPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_SMP_PASSTHROUGH PtrSmpRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS  PtrIocStatus);
SCRUTINY_STATUS slibiControllerScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 DeviceHandle, __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS  PtrIocStatus);
SCRUTINY_STATUS slibiControllerConfigPagePassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__  PTR_SCRUTINY_CONTROLLER_CFG_PASSTHROUGH PtrConfigRequest);
SCRUTINY_STATUS slibiControllerToolboxClean (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Flags, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus);
SCRUTINY_STATUS slibiControllerGetDriveBusTarget (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU16 PtrDevHandle, __OUT__ PU32 PtrBus, __OUT__ PU32 PtrTarget);
SCRUTINY_STATUS slibiControllerGetSasPhyErrorCounters (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PhyId, __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS PtrSasPhyErrCounter);
SCRUTINY_STATUS slibiControllerGetPciPhyErrorCounters (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PhyId, __OUT__ PTR_SCRUTINY_PCI_LINK_ERROR_COUNTERS PtrPciPhyErrCounter);

SCRUTINY_STATUS slibiControllerHostboot (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrFirmware, __IN__ U32 FirmwareLength);

SCRUTINY_STATUS slibiControllerGetIocFaultCode (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrValue, __IN__ U32 SizeInBytes);
SCRUTINY_STATUS slibiControllerSetCoffeeFault (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);

SCRUTINY_STATUS slibiControllerHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrFolderName, __IN__ BOOLEAN DumpToFile, __OUT__ PU32  PtrErrorCode);

SCRUTINY_STATUS slibiControllerToolboxIstwiReadWrite (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_ISTWI_REQUEST PtrIstwiRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus);

SCRUTINY_STATUS slibiControllerToolboxLaneMargin (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_LANE_MARGIN_REQUEST PtrLaneMarginRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus);

SCRUTINY_STATUS slibiControllerToolboxMemoryRead (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus);
SCRUTINY_STATUS slibiControllerToolboxMemoryWrite (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus);

SCRUTINY_STATUS slibiControllerSpdmOperations (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SPDM_REQUEST PtrSpdmRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus);

#if defined(OS_UEFI)
SCRUTINY_STATUS slibiControllerGetIocState (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrValue);

SCRUTINY_STATUS slibiControllerSendDoorbellMsg (
	__IN__  PTR_SCRUTINY_DEVICE 	PtrDevice, 
	__IN__	PU32                    PtrRequestMessage,
    __IN__  U32                     RequestSize,
    __OUT__ PU32                    PtrReplyMessage,
    __OUT__ U32                     ReplySize
    );

#endif



SCRUTINY_STATUS slibiExpanderEnableDisablePhy (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier,  __IN__ BOOLEAN  PhyEnable);
SCRUTINY_STATUS slibiExpanderResetPhy (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __IN__ U8 PhyOperation, __IN__ BOOLEAN DoAllPhys);
SCRUTINY_STATUS slibiSwitchGetPciePortProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties);
SCRUTINY_STATUS slibiExpanderGetPhyErrorCounters (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS PtrSasPhyErrCounter);
SCRUTINY_STATUS slibiExpanderHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32  PtrErrorCode);
SCRUTINY_STATUS slibiSwitchHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32 PtrErrorCode);
SCRUTINY_STATUS slibiSwitchRxEqStatus (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS PtrPortRxEqStatus);
SCRUTINY_STATUS slibiSwitchTxCoeff (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF PtrPortTxCoeffStatus);
SCRUTINY_STATUS slibiSwitchHardwareEyeStart( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus );
SCRUTINY_STATUS slibiSwitchHardwareEyePoll( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus);
SCRUTINY_STATUS slibiSwitchHardwareEyeGet( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus);
SCRUTINY_STATUS slibiSwitchHardwareEyeClean( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus);
SCRUTINY_STATUS slibiSwitchSoftwareEye( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort);
SCRUTINY_STATUS slibiSwitchGetPowerOnSense (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__   PTR_SCRUTINY_SWITCH_POWER_ON_SENSE PtrSwPwrOnSense );
SCRUTINY_STATUS slibiSwitchGetCcrStatus (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__   PTR_SCRUTINY_SWITCH_CCR_STATUS PtrSwCcrStatus );




SCRUTINY_STATUS slibiExpanderScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest);
SCRUTINY_STATUS slibiSwitchScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest);
SCRUTINY_STATUS slibiExpanderGetConfigPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U16 Page, __IN__ U8 Region, __OUT__ PU8 *PtrValue, __OUT__ PU32 PtrSize);
SCRUTINY_STATUS slibiSwitchGetConfigPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U16 Page, __IN__ U8 Region, __OUT__ PU8 *PtrValue, __OUT__ PU32 PtrSize);

SCRUTINY_STATUS slibiBroadcomScsiMemoryRead  (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,
                                              __IN__ U32 Address,
                                              __OUT__ PVOID PtrValue,
                                              __IN__ U32 SizeInBytes);

SCRUTINY_STATUS  slibiExpanderGetHealth ( 
    __IN__   PTR_SCRUTINY_DEVICE                PtrDevice, 
    __OUT__  PTR_SCRUTINY_EXPANDER_HEALTH       PtrExpHealthInfo
);

SCRUTINY_STATUS  slibiSwitchGetHealth ( 
    __IN__   PTR_SCRUTINY_DEVICE                PtrDevice, 
    __OUT__  PTR_SCRUTINY_SWITCH_HEALTH         PtrSwHealthInfo
);



SCRUTINY_STATUS slibiSwitchGetPciPortErrorStatistic (
    __IN__ PTR_SCRUTINY_DEVICE PtrProductHandle, 
    __IN__ U32 Port, 
    __OUT__ PTR_SCRUTINY_SWITCH_ERROR_STATISTICS PtrPciePortErrStatistic
);


SCRUTINY_STATUS slibiBroadcomScsiMemoryWrite (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,
                                              __IN__ U32 Address,
                                              __IN__ PVOID PtrValue,
                                              __IN__ U32 SizeInBytes);


SCRUTINY_STATUS slibiExpanderGetLogs (
    __IN__ PTR_SCRUTINY_DEVICE                  PtrDevice,
    __IN__ U32                                  Flags, 
    __IN__ const char*                          PtrFolderName
);

SCRUTINY_STATUS slibiSwitchGetLogs (
    __IN__ PTR_SCRUTINY_DEVICE                PtrDevice,
    __IN__ U32                                Flags, 
    __IN__ const char*                        PtrFolderName
);

SCRUTINY_STATUS slibiSwitchGetLaneMarginCapacities (
    __IN__ PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__ U8 SwtichPort,  
    __IN__ U16 Lane,
    __OUT__ PU32 PtrMarginControlCapabilities,
    __OUT__ PU32 PtrNumSteps,
    __OUT__ PU32 PtrMaxOffset
);
 
SCRUTINY_STATUS slibiSwitchPerformLaneMargining    (
	__IN__ PTR_SCRUTINY_DEVICE                      PtrDevice, 
	__IN__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_REQUEST  PtrSwitchLaneMarginReq,
    __OUT__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE PtrSwitchLaneMarginResp 
);


SCRUTINY_STATUS slibiExpanderFlashMemoryDownload (
    __IN__ PTR_SCRUTINY_DEVICE    PtrDevice,
    __IN__ U32                    FlashAddress,
    __IN__ PU8                    PtrBuffer,
    __IN__ U32                    BufferSize,
    __IN__ ScrutinyLogCallBack    CallBack
);


#if defined(OS_LINUX)
SCRUTINY_STATUS  slibiSwitchSetAladinConfiguration (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __OUT__ PU32 PtrConfigStatus);
SCRUTINY_STATUS  slibiSwitchGetAladinTracedump (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrDumpFilePath, __OUT__ PU32 PtrDumpStatus);
SCRUTINY_STATUS  slibiSwitchPollAladinTrigger (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrTriggerStatus);
SCRUTINY_STATUS  slibiSwitchStopAladinTrace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrStopStatus);
#endif


#endif /* __LIB_INTERNAL__H__ */

