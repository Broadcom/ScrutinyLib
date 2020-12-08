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

#ifndef __PCI_SCAN__H__
#define __PCI_SCAN__H__

SCRUTINY_STATUS dciLoadSliffDriver();
SCRUTINY_STATUS hliFindLSIDevices();
SCRUTINY_STATUS peiInitializeBARRegions (__IN__ PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation, __OUT__ PTR_SCRUTINY_PCI_HANDLE PtrHandle);
SCRUTINY_STATUS pcsiMapPciBar (__IN__ SCRUTINY_PCI_ADDRESS PciLocation, PTR_SLIFF_UINT64 PtrBusAddress, U32 TotalBytes, PVOID *PtrVirtualAddress);
SCRUTINY_STATUS pcsiUnmapPciBar (PVOID PtrVirtualAddress, U32 Size);
SCRUTINY_STATUS pcsiWriteDword (__IN__ SCRUTINY_PCI_ADDRESS PciLocation,  U32 Offset, U32 Dword);
SCRUTINY_STATUS pcsiReadDword (SCRUTINY_PCI_ADDRESS PciLocation,  U32 Offset, U32 *PtrDword);


#endif /* __PCI_SCAN__H__ */

