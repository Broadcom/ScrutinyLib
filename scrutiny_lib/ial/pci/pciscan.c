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


#if defined(OS_WINDOWS)
    #include "pciwin.h"
#endif

#if defined(OS_UEFI)
    #include "pciefi.h"
#endif

#if defined(OS_LINUX)
    #include "pcilinux.h"
#endif

SCRUTINY_STATUS atlasiQualifyPCIDevice (__IN__ PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation);

#define LSI_MAX_BUS     0x100
#define LSI_MAX_DEV     0x020
#define LSI_MAX_FUNC    0x008


SCRUTINY_STATUS hlFindDevicesInBus (U16 Bus);

SCRUTINY_STATUS hlFindDevicesInDev (U16 Bus, U16 Dev);

SCRUTINY_STATUS hlVerifyDeviceSignature (U16 Bus, U16 Dev, U16 Func);

SCRUTINY_STATUS hlIsLSIDevice (U32 PciSignature);

SCRUTINY_STATUS hlConfigureAndAddDevice (SCRUTINY_PCI_ADDRESS *PtrBus);

SCRUTINY_STATUS hliFindLSIDevices()
{
    U16 bus = 0;

    /* First we need to initialize the PCI driver */

    if (dciLoadSliffDriver())
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    for (bus = 0; bus < LSI_MAX_BUS; bus++)
    {
        hlVerifyDeviceSignature (bus, 0, 0);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

#if 0
SCRUTINY_STATUS hlFindDevicesInBus (U16 Bus)
{

    U16 dev;

    for (dev = 0; dev < LSI_MAX_DEV; dev++)
    {
        hlFindDevicesInDev (Bus, dev);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS hlFindDevicesInDev (U16 Bus, U16 Dev)
{
    U16 func;

    for (func = 0; func < LSI_MAX_FUNC; func++)
    {
        hlVerifyDeviceSignature (Bus, Dev, func);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}
#endif

SCRUTINY_STATUS hlVerifyDeviceSignature (U16 Bus, U16 Dev, U16 Func)
{

    SCRUTINY_PCI_ADDRESS busIndex;
    U32 dword = 0xFFFFFFFF;

    busIndex.BusNumber = Bus;
    busIndex.DeviceNumber = Dev;
    busIndex.FunctionNumber = Func;

    /*
     * Read 4 bytes from config space and get the device id information
     */

    if (pcsiReadDword (busIndex, PCI_CONFIG_SPACE_OFFSET_VENDOR_ID, &dword) != SCRUTINY_STATUS_SUCCESS)
    {

        /*
         * Unable to read
         */

        gPtrLoggerGeneric->logiDebug ("Unable to read the PCI slot.");

        return (SCRUTINY_STATUS_FAILED);

    }

    if (dword == 0xFFFFFFFF) {

        /*
         * Seems nothing in this slot.
         */

        return (SCRUTINY_STATUS_FAILED);

    }

    /*
     * Device found
     */

    if (hlIsLSIDevice (dword) != SCRUTINY_STATUS_SUCCESS)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /*
     * Initialize the device and add the Device Managers.
     */

    return (hlConfigureAndAddDevice (&busIndex));

}

SCRUTINY_STATUS hlIsLSIDevice (U32 PciSignature)
{

    /*
     * Check with our LSI Vendor ID.
     */

    if ((PciSignature & 0xFFFF) == 0x1000)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    return (SCRUTINY_STATUS_FAILED);

}

SCRUTINY_STATUS hlConfigureAndAddDevice (SCRUTINY_PCI_ADDRESS *PtrBus)
{

    /*
     * Add to the Device manager
     */

    return (atlasiQualifyPCIDevice (PtrBus));

}

SCRUTINY_STATUS cdeMapBarRegions (PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation, PTR_SCRUTINY_PCI_HANDLE PtrHandle)
{
    U8 index;
    SLIFF_UINT64 address;
    SCRUTINY_STATUS status;

    for (index = 0; index < 4; index++)
    {

        if (PtrHandle->BarRegions[index].PhysicalAddress.Quad == 0)
        {
            continue;
        }

        address.Quad = PtrHandle->BarRegions[index].PhysicalAddress.Quad;

        /* NOTE: Not considering the IO Bar mask because we are not having any IO bars and they are
           deprecated. */

        address.u.Low &= 0xFFFFF000;

        status = pcsiMapPciBar (*PtrDeviceLocation, &address, PtrHandle->BarRegions[index].BarSize, &PtrHandle->BarRegions[index].PtrVirtualAddress);

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerGeneric->logiVerbose ("Mapping '%08x%08x' for BAR%d (Failed)", address.u.High, address.u.Low, index);

            continue;
        }

        else
        {
            gPtrLoggerGeneric->logiVerbose ("Mapping '%08x%08x' for BAR%d (Success), bytes %x", address.u.High, address.u.Low, index, PtrHandle->BarRegions[index].BarSize);
        }

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS peiInitializeBARRegions (__IN__ PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation, __OUT__ PTR_SCRUTINY_PCI_HANDLE PtrHandle)
{
    U32 offset = 0;
    U32 barIndex = 0;
    U32 lower, upper, mask = 0;

    /* Initialize the BAR region structure */

    //sosiMemSet (PtrHandle->BarRegions, 0, sizeof (IAL_PCI_BAR_REGIONS) * MAX_BAR_REGIONS);

    offset = PCI_CONFIG_SPACE_OFFSET_BAR_0;

    /* For header type 0x1 we have only two BAR regions */

    while (offset < PCI_CONFIG_SPACE_OFFSET_BAR_0 + (sizeof (U32) * 2))
    {

        PtrHandle->BarRegions[barIndex].PhysicalAddress.Quad = 0;
        PtrHandle->BarRegions[barIndex].PtrVirtualAddress = NULL;

        if (pcsiReadDword (*PtrDeviceLocation, offset, &lower))
        {
            /* Unable to read the value. Invalid return. */
            gPtrLoggerGeneric->logiDebug ("Unable to read the BAR register value.");
            return (SCRUTINY_STATUS_FAILED);
        }

        if (lower == 0xFFFFFFFF || lower == 0)
        {
            offset += 4;
            barIndex++;

            continue;
        }

        #if defined(OS_UEFI)
        PtrHandle->BarRegions[barIndex].ConfigSpaceOffset = barIndex;
        #else
        PtrHandle->BarRegions[barIndex].ConfigSpaceOffset = offset;
        #endif

        if ((lower & 1) == 0)
        {
            PtrHandle->BarRegions[barIndex].IsMemBar = TRUE;

            #if 0
            if (lower & PCI_CFG_BASE_ADDR_MEMORY_PREFECTHABLE)
            {
                gPtrLoggerGeneric->logiInfo ("Memory is prefetchable.");
            }
            #endif

            mask = PCI_CONFIG_SPACE_OFFSET_MEM_BAR_MASK;

            pcsiWriteDword (*PtrDeviceLocation, offset, mask);
            pcsiReadDword (*PtrDeviceLocation, offset, &mask);

            mask &= PCI_CONFIG_SPACE_OFFSET_MEM_BAR_MASK;

            PtrHandle->BarRegions[barIndex].BarSize  = ~mask + 1;
            PtrHandle->BarRegions[barIndex].PhysicalAddress.u.Low = lower & PCI_CONFIG_SPACE_OFFSET_MEM_BAR_MASK;

            pcsiWriteDword (*PtrDeviceLocation, offset, lower);

            offset += 4;

            if (lower & 4)
            {
                PtrHandle->BarRegions[barIndex].Is64BitAddress = TRUE;
                pcsiReadDword (*PtrDeviceLocation, offset, &upper);

                PtrHandle->BarRegions[barIndex].PhysicalAddress.u.High = upper;
                offset += 4;
            }

        }

        else
        {
            // IO space
            mask = PCI_CONFIG_SPACE_OFFSET_IO_BAR_MASK;

            pcsiWriteDword (*PtrDeviceLocation, offset, mask);
            pcsiReadDword (*PtrDeviceLocation, offset, &mask);

            mask &= PCI_CONFIG_SPACE_OFFSET_IO_BAR_MASK;

            PtrHandle->BarRegions[barIndex].BarSize  = ~mask + 1;
            PtrHandle->BarRegions[barIndex].PhysicalAddress.u.Low = lower & PCI_CONFIG_SPACE_OFFSET_IO_BAR_MASK;

            pcsiWriteDword (*PtrDeviceLocation, offset, lower);

            offset += 4;

        }

        barIndex++;

    }

    return (cdeMapBarRegions (PtrDeviceLocation, PtrHandle));

}

