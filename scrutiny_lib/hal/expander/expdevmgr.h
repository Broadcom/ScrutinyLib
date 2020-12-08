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


#ifndef __EXPANDER_DEVICE_MANAGER__H__
#define __EXPANDER_DEVICE_MANAGER__H__

SCRUTINY_STATUS edmiDiscoverDevices();

SCRUTINY_STATUS edmiResetDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS edmiScsiResetDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS edmiCloseDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);


SCRUTINY_STATUS edmQualifyExpanderOnSdbInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS edmQualifyExpanderOnScsiGenericInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS edmQualifyExpanderOnSmpInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS edmiIsBroadcomExpander (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);

SCRUTINY_STATUS edmQualifyCubCobraOnSdbInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS edmQualifyMargayOnSdbInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS edmQualifySignatureForMargay (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 ComponentId, __IN__ U32 RevisionId);


SCRUTINY_STATUS edmQualifySignatureForCobraCub (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 ComponentId, __IN__ U32 RevisionId);

SCRUTINY_STATUS edmiPerformSmpPassthrough (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrSMPRequest, __IN__ U32 SMPRequestSize, __OUT__ PU8 PtrSMPResponse, __IN__ U32 SMPResponseSize);

SCRUTINY_STATUS edmGetActiveFirmwareVersionSdb (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion);



#endif /* __CONTROLLER_DEVICE_MANGER__H__ */

