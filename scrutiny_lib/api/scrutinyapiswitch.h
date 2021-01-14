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

#ifndef __SCRUTINY_API_SWITCH__H__
#define __SCRUTINY_API_SWITCH__H__



/*
 * ----------------------------------------------------------------------------
 * Switch specific APIs
 * ----------------------------------------------------------------------------
 */

SCRUTINY_STATUS ScrutinySwitchGetConfigPage (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__  U16                             PageNumber,
    __IN__  U8                              Region,
    __OUT__ PU8*                            PtrPageBuffer,
    __OUT__ PU32                            PtrPageBufferLength
);

SCRUTINY_STATUS ScrutinySwitchScsiPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH     PtrScsiRequest
);

SCRUTINY_STATUS ScrutinySwitchGetPciPortErrorStatistics (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE              PtrProductHandle, 
    __IN__ U32                                      Port, 
    __OUT__ PTR_SCRUTINY_SWITCH_ERROR_STATISTICS    PtrPciePortErrStatistic
);

SCRUTINY_STATUS ScrutinySwitchMemoryRead (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __IN__  U32     Address,
    __OUT__ PU32    PtrValue,
    __IN__  U32     SizeInBytes
);

SCRUTINY_STATUS ScrutinySwitchMemoryWrite (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __IN__ U32 Address,
    __IN__ PU32 PtrValue,
    __IN__ U32 SizeInBytes
);

SCRUTINY_STATUS ScrutinySwitchGetPciePortProperties (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE              PtrProductHandle, 
    __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES     PtrPciePortProperties
);


SCRUTINY_STATUS  ScrutinySwitchHealthCheck ( 
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__   const char*                             PtrConfigFilePath,
    __IN__   BOOLEAN                                 DumpToFile,
    __OUT__  PU32                                    PtrErrorCode
);


#if !defined (OS_UEFI)

SCRUTINY_STATUS  ScrutinySwitchRxEqStatus ( 
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __IN__   U32                                        StartPort,
    __IN__   U32                                        NumberOfPort,
    __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS    PtrPortRxEqStatus                                   
);

SCRUTINY_STATUS  ScrutinySwitchTxCoeff ( 
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __IN__   U32                                        StartPort,
    __IN__   U32                                        NumberOfPort,
    __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF 	        PtrPortTxCoeffStatus                                   
);

SCRUTINY_STATUS ScrutinySwitchHardwareEyeStart (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __IN__   U32                                        StartPort, 
    __IN__   U32                                        NumberOfPort, 
    __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE            PtrPortHwEyeStatus 
);

SCRUTINY_STATUS ScrutinySwitchHardwareEyePoll (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __IN__   U32                                        StartPort, 
    __IN__   U32                                        NumberOfPort, 
    __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE            PtrPortHwEyeStatus 
);

SCRUTINY_STATUS ScrutinySwitchHardwareEyeGet (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __IN__   U32                                        StartPort, 
    __IN__   U32                                        NumberOfPort, 
    __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE            PtrPortHwEyeStatus 
);

SCRUTINY_STATUS ScrutinySwitchHardwareEyeClean (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __IN__   U32                                        StartPort, 
    __IN__   U32                                        NumberOfPort, 
    __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE            PtrPortHwEyeStatus 
);

SCRUTINY_STATUS ScrutinySwitchSoftwareEye (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __IN__   U32                                        StartPort, 
    __IN__   U32                                        NumberOfPort 
);

#endif

SCRUTINY_STATUS ScrutinySwitchGetPowerOnSense (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __OUT__   PTR_SCRUTINY_SWITCH_POWER_ON_SENSE        PtrSwPwrOnSense 
);

SCRUTINY_STATUS ScrutinySwitchGetCcrStatus (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE                PtrProductHandle,
    __OUT__   PTR_SCRUTINY_SWITCH_CCR_STATUS            PtrSwCcrStatus 
);

SCRUTINY_STATUS ScrutinySwitchGetLogs (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __IN__ U32                                  Flags, 
    __IN__ const char*                          PtrFolderName
);

SCRUTINY_STATUS  ScrutinySwitchGetHealth ( 
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __OUT__  PTR_SCRUTINY_SWITCH_HEALTH         PtrSwHealthInfo
);


SCRUTINY_STATUS ScrutinySwitchGetLaneMarginCapacities (
	__IN__ PTR_SCRUTINY_PRODUCT_HANDLE  PtrProductHandle,
	__IN__ U8                           SwtichPort,  
	__IN__ U16                          Lane,
	__OUT__ PU32                        PtrMarginControlCapabilities,
	__OUT__ PU32                        PtrNumSteps,
	__OUT__ PU32                        PtrMaxOffset
);

SCRUTINY_STATUS ScrutinySwitchPerformLaneMargining    (
	__IN__ PTR_SCRUTINY_PRODUCT_HANDLE                  PtrProductHandle,
	__IN__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_REQUEST      PtrSwitchLaneMarginReq,
    __OUT__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE    PtrSwitchLaneMarginResp 
);

SCRUTINY_STATUS ScrutinySwitchGetPciePortPerformance (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE                  PtrProductHandle, 
    __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance
);


#if defined(OS_LINUX)

SCRUTINY_STATUS  ScrutinySwitchSetAladinConfiguration (
	__IN__  PTR_SCRUTINY_PRODUCT_HANDLE 	PtrProductHandle, 
	__IN__  const char* 					PtrConfigFilePath, 
	__OUT__ PU32 							PtrConfigStatus
);

SCRUTINY_STATUS ScrutinySwitchGetAladinTracedump (
	__IN__  PTR_SCRUTINY_PRODUCT_HANDLE 	PtrHandle, 
	__OUT__ const char* 					PtrDumpFilePath, 
	__OUT__ PU32 							PtrDumpStatus
);

SCRUTINY_STATUS ScrutinySwitchPollAladinTrigger (
	__IN__  PTR_SCRUTINY_PRODUCT_HANDLE 	PtrProductHandle, 
	__OUT__ PU32 							PtrTriggerStatus
);

SCRUTINY_STATUS ScrutinySwitchStopAladinCapture (
	__IN__  PTR_SCRUTINY_PRODUCT_HANDLE 	PtrProductHandle, 
	__OUT__ PU32 							PtrStopStatus
);

#endif


#endif /* __SCRUTINY_API_SWITCH__H__ */

