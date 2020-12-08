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

#ifndef __ATLAS__H__
#define __ATLAS__H__


SCRUTINY_STATUS atlasSGAssignFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDeviceEntry, __OUT__ PU32 PtrVersion);

SCRUTINY_STATUS atlasSGAssignSASAddress (__IN__ PTR_SCRUTINY_DEVICE PtrDeviceEntry);

SCRUTINY_STATUS atlasiQualifySGSwitch (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);

SCRUTINY_STATUS atlasiRegisterBasedQualifSwitch (__IN__ PTR_SCRUTINY_DEVICE PtrDeviceEntry);

SCRUTINY_STATUS atlasiAssignProductString (
    __IN__ PTR_SCRUTINY_DEVICE PtrDeviceEntry,
    __IN__ U32      ComponentValue,
    __IN__ U32      Revision
    );

SCRUTINY_STATUS atlasiQualifyPCIDevice (__IN__ PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation);

//SCRUTINY_STATUS atlasiAddPCIDevice (PTR_SCRUTINY_IAL_PCI_CONFIG_SPACE PtrIalConfigSpace, PTR_SCRUTINY_IAL_PCI_LOCATION PtrScrutinyLocation);

SCRUTINY_STATUS atlasiPciChimeToAxiWriteRegister (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __IN__ U32 Data);
SCRUTINY_STATUS atlasiPciChimeToAxiReadRegister (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __OUT__ U32 *PtrData);

SCRUTINY_STATUS atlasiFSMFallBackRegisterWrite (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __OUT__ U32 Data);
SCRUTINY_STATUS atlasiFSMFallBackRegisterRead (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __OUT__ U32 *PtrData);
SCRUTINY_STATUS atlasGetFlashTable (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice,
    __OUT__ PTR_ATLAS_FLASH_TABLE *PtrFlashTable
    );
SCRUTINY_STATUS atlasGetSwitchMode (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);

SCRUTINY_STATUS atlasPCIeConfigurationSpaceRead (PTR_SCRUTINY_DEVICE PtrDevice, U32 Port, U32 Offset, PU32 PtrValue);

SCRUTINY_STATUS atlasPCIeConfigurationSpaceWrite (  
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__  U32 Port, 
    __IN__  U32 Offset, 
    __IN__  U32 Value
);


#endif

