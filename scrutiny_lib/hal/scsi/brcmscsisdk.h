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

#ifndef __BROADCOM_SCSI_DEVICE__H__
#define __BROADCOM_SCSI_DEVICE__H__

/*
 * HAL Direct Flash Access Buffer ID(s) (0x80 - 0x8F)
 */
#define BRCM_SCSI_BUFFER_ID_DIRECT_REGION_0                 (0x80)

#define BRCM_SCSI_BUFFER_ID_RUNNING_FIRMWARE                (0x00)
#define BRCM_SCSI_BUFFER_ID_CHIP_RESET                      (0xE9)
#define BRCM_SCSI_BUFFER_ID_ETHERNET_CONFIG                 (0xE6)
#define BRCM_SCSI_BUFFER_ID_ACTIVE_LOGS                     (0xE5)
#define BRCM_SCSI_BUFFER_ID_ACTIVE_TRACES                   (0xE7)
#define BRCM_SCSI_BUFFER_ID_ACTIVE_LOGS_DECODED             (0xEA)

#define BRCM_SCSI_BUFFER_ID_MEMORY_RW_BYTE                  (0xC0)
#define BRCM_SCSI_BUFFER_ID_MEMORY_RW_WORD                  (0xC1)
#define BRCM_SCSI_BUFFER_ID_MEMORY_RW_DWORD                 (0xC2)

#define BRCM_SCSI_BUFFER_ID_SCSI_TOOLBOX                    (0xC4)

#define BRCM_SCSI_BUFFER_ID_VIRTUAL_BOOTLOADER              (0xE0)
#define BRCM_SCSI_BUFFER_ID_VIRTUAL_ACTIVE_1                (0xE1)
#define BRCM_SCSI_BUFFER_ID_VIRTUAL_ACTIVE_2                (0xE2)
#define BRCM_SCSI_BUFFER_ID_VIRTUAL_CONFIG_PAGES            (0xE3)
#define BRCM_SCSI_BUFFER_ID_ACTIVE_TRACES                   (0xE7)

#define BRCM_SCSI_BUFFER_ID_REGION_BOOTLOADER               (0x80)
#define BRCM_SCSI_BUFFER_ID_REGION_ACTIVE_FIRMWARE          (0x81)
#define BRCM_SCSI_BUFFER_ID_REGION_BACKUP_FIRMWARE          (0x82)
#define BRCM_SCSI_BUFFER_ID_REGION_MANUFACTURING            (0x83)
#define BRCM_SCSI_BUFFER_ID_REGION_PERSISTENT_1             (0x84)
#define BRCM_SCSI_BUFFER_ID_REGION_PERSISTENT_2             (0x85)
#define BRCM_SCSI_BUFFER_ID_REGION_LOG_1                    (0x86)
#define BRCM_SCSI_BUFFER_ID_REGION_LOG_2                    (0x87)
#define BRCM_SCSI_BUFFER_ID_REGION_OEM_1                    (0x88)
#define BRCM_SCSI_BUFFER_ID_REGION_OEM_2                    (0x89)

#define BRCM_SCSI_DEVICE_CONFIG_PAGE_REGION_ANY             (0)
#define BRCM_SCSI_DEVICE_CONFIG_PAGE_REGION_PERSISTENT      (1)
#define BRCM_SCSI_DEVICE_CONFIG_PAGE_REGION_MFG             (2)


SCRUTINY_STATUS bsdiDiscoverDevices();

SCRUTINY_STATUS bsdiQualifyBroadcomScsiDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);

SCRUTINY_STATUS bsdiMemoryRead32 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __OUT__ PU32 PtrData, __IN__ U32 SizeInBytes);
SCRUTINY_STATUS bsdiMemoryRead16 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __OUT__ PU16 PtrData, __IN__ U32 SizeInBytes);
SCRUTINY_STATUS bsdiMemoryRead8 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __OUT__ PU8 PtrData, __IN__ U32 SizeInBytes);

SCRUTINY_STATUS bsdiGetCurrentFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion);

SCRUTINY_STATUS bsdiMemoryWrite32 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __IN__ PU32 PtrData);
SCRUTINY_STATUS bsdiMemoryWrite16 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __IN__ PU16 PtrData);
SCRUTINY_STATUS bsdiMemoryWrite8 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __IN__ PU8 PtrData);

SCRUTINY_STATUS bsdScsiMemoryWrite32 (__IN__ PTR_SCRUTINY_DEVICE PtrExpander, __IN__ U32 Address, __IN__ PU32 PtrValue, __IN__ U32 SizeInBytes);


SCRUTINY_STATUS bsdScsiMemoryRead32 (__IN__ PTR_SCRUTINY_DEVICE PtrExpander, __IN__ U32 Address, __OUT__ PU32 PtrValue, __IN__ U32 SizeInBytes);

SCRUTINY_STATUS bsdiPerformScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest);

SCRUTINY_STATUS bsdiGetConfigPage (__IN__ PTR_SCRUTINY_DEVICE PtrExpander, __IN__ U16 Page, __IN__ U8 Region, __OUT__ PU8 *PtrValue, __OUT__ PU32 PtrSize);

SCRUTINY_STATUS bsdiGetRegion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 BufferId, __OUT__ PU8 *PtrBuffer, __OUT__ PU32 PtrRegionSize);

SCRUTINY_STATUS bsdiGetRegionSize (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 BufferId, __OUT__ PU32 PtrSize);

SCRUTINY_STATUS bsdiDownloadRegion (
                          __IN__ PTR_SCRUTINY_DEVICE PtrDevice, 
                          __IN__ U8 BufferId, 
                          __IN__ PU8 PtrBuffer, 
                          __IN__ U32 BufferSize);


#endif /* __BROADCOM_SCSI_DEVICE__H__ */

