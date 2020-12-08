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

#include "libincludes.h"
#include "pcilinux.h"

static int mSliffDriverHandle;


SCRUTINY_STATUS dciLoadSliffDriver()
{

    /*
     * Open the file.
     */

    mSliffDriverHandle = open ("/dev/SliffDriver", O_RDWR | O_SYNC);

    if (mSliffDriverHandle < 0)
    {
        gPtrLoggerGeneric->logiDebug ("Unable to open the driver instance. Please restart the application.");

        return (SCRUTINY_STATUS_FAILED);

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS pcsiMapPciBar (__IN__ SCRUTINY_PCI_ADDRESS PciLocation, PTR_SLIFF_UINT64 PtrBusAddress, U32 TotalBytes, PVOID *PtrVirtualAddress)
{

    void *map_base, *virt_addr;
    unsigned long long map_size;
    unsigned long long map_mask;

    *PtrVirtualAddress = NULL;                  /* Make it null value */

    #if 0

    *PtrVirtualAddress = mmap (&PtrBusAddress->Quad,
                               TotalBytes,
                               PROT_READ | PROT_WRITE,
                               MAP_SHARED,
                               mSliffDriverHandle,
                               0);
    #else

    map_size = TotalBytes;
    map_mask = TotalBytes - 1;

    /* Map one page */
    map_base = mmap (0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, mSliffDriverHandle, (PtrBusAddress->Quad & ~map_mask));

    virt_addr = map_base + ((PtrBusAddress->Quad & ~map_mask) & map_mask);

    *PtrVirtualAddress = virt_addr;

    #endif

    if (*PtrVirtualAddress)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    return (SCRUTINY_STATUS_FAILED);

}

SCRUTINY_STATUS pcsiUnmapPciBar (PVOID PtrVirtualAddress, U32 Size)
{
    munmap (PtrVirtualAddress, Size);

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS pcsiWriteDword (__IN__ SCRUTINY_PCI_ADDRESS PciLocation,  U32 Offset, U32 Dword)
{

    int ioctlStatus = 0;

    SCRUTINY_DRIVER_PCI_CFG_SPACE   scrutinyLocation;

    sosiMemSet (&scrutinyLocation, 0, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE));

    scrutinyLocation.Bus = PciLocation.BusNumber;
    scrutinyLocation.Device = PciLocation.DeviceNumber;
    scrutinyLocation.Domain = 0; //PciLocation.SegmentNumber; //as of now we support only domain Zero
    scrutinyLocation.Function = PciLocation.FunctionNumber;

    scrutinyLocation.Offset = Offset;

    scrutinyLocation.Data[0] = Dword;

    ioctlStatus = ioctl (mSliffDriverHandle, SCRUTINY_DRIVER_IOCTL_WRITE_PCI, &scrutinyLocation);

    if (ioctlStatus < 0)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS pcsiReadDword (SCRUTINY_PCI_ADDRESS PciLocation,  U32 Offset, U32 *PtrDword)
{

    int ioctlStatus = 0;

    SCRUTINY_DRIVER_PCI_CFG_SPACE   scrutinyLocation;

    sosiMemSet (&scrutinyLocation, 0, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE));

    scrutinyLocation.Bus = PciLocation.BusNumber;
    scrutinyLocation.Device = PciLocation.DeviceNumber;
    scrutinyLocation.Domain = 0; //PciLocation.SegmentNumber; //as of now we support only domain Zero
    scrutinyLocation.Function = PciLocation.FunctionNumber;

    scrutinyLocation.Offset = Offset;

    ioctlStatus = ioctl (mSliffDriverHandle, SCRUTINY_DRIVER_IOCTL_DISCOVER_PCI, &scrutinyLocation);

    if (ioctlStatus < 0)
    {

    }

    ioctlStatus = ioctl (mSliffDriverHandle, SCRUTINY_DRIVER_IOCTL_READ_PCI, &scrutinyLocation);

    if (ioctlStatus < 0)
    {
        *PtrDword = 0xFFFFFFFF;
    }

    else
    {
        *PtrDword = scrutinyLocation.Data[0];
    }

    return (SCRUTINY_STATUS_SUCCESS);

}


