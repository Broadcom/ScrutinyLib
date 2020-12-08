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

#ifndef __LINUX_SCSI_GENERIC__H__
#define __LINUX_SCSI_GENERIC__H__


#define INVALID_HANDLE_VALUE -1


/*
 */

#define SYSFS_DEVNODE_DIR           "/sys/class/scsi_generic"
#define OSAL_SAS_NODE_NAME_FORMAT   "sg"


#define MAX_SCSI_DEVS           (4096)

SCRUTINY_STATUS sgiLocateScsiDevices();

SCRUTINY_STATUS sgOpenDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);


SCRUTINY_STATUS sgiCloseDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDeviceEntry);

SCRUTINY_STATUS sgiReadCdb (PTR_SCRUTINY_DEVICE    PtrDeviceEntry, U8 *PtrCdb, U32 CdbLen,
                           U8 *PtrBuffer, U32 Size, U32 *PtrSizeRead);

SCRUTINY_STATUS sgiWriteCdb (PTR_SCRUTINY_DEVICE   PtrDeviceEntry, U8 *PtrCdb, U32 CdbLen,
                            U8 *PtrBuffer, U32 Size, U32 *PtrSizeWritten);

SCRUTINY_STATUS sgiIsOpened (PTR_SCRUTINY_DEVICE   PtrDeviceEntry);

SCRUTINY_STATUS sgLinuxScanSelect (const struct dirent * PtrEntry);

U32 sgLinuxScanDirectory (const char * PtrDirectory);

SCRUTINY_STATUS sgLinuxCheckDevice (U32 Index);


SCRUTINY_STATUS sgReadCdb (PTR_SCRUTINY_DEVICE     PtrDeviceEntry, U8 *PtrCdb, U32 CdbLen,
                  U8 *PtrBuffer, U32 Size, U32 *PtrSizeRead);

SCRUTINY_STATUS sgWriteCdb (PTR_SCRUTINY_DEVICE    PtrDeviceEntry, U8 *PtrCdb, U32 CdbLen,
                   U8 *PtrBuffer, U32 Size, U32 *PtrSizeWritten);

SCRUTINY_STATUS sgLinuxCheckDevice (U32 Index);

SCRUTINY_STATUS sgAddExpanderDevice (U32 CurrentHandle, const char* PtrFile, U32 Index);

SCRUTINY_STATUS sgiPerformScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest);



#endif

