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
#include "margay.h"
#include "margayfwheader.h"
#include "cobregisters.h"

SCRUTINY_STATUS margayRegisterBasedGetActiveFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion);
SCRUTINY_STATUS margayRegisterBasedGetFirmwareVersionIfActiveFW (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_MARGAY_FLASH_REGION_ENTRY PtrRegionEntry, __OUT__ PU32 PtrVersion);
SCRUTINY_STATUS margayRegisterBasedGetFlashChainedImage (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 FlashOffset, __IN__ U32 FirstImageOffset, __IN__ U8 ImageType, __OUT__ PU8 *PtrImage, __OUT__ PU32 PtrSize);
SCRUTINY_STATUS margayRegisterBasedUploadBootloaderFlashTable (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_MARGAY_FLASH_TABLE *PtrFlashTable);
BOOLEAN margayRegisterBasedPowerOnSenseIsCFIEnabled (__IN__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS margayRegisterBasedPowerOnSenseRegisterGet (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrRegister);
SCRUTINY_STATUS margayRegisterBasedAssignSASAddress (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);

/* These routines are explicity for Margay devices only */

SCRUTINY_STATUS margayiRegisterBasedQualifyExpander (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{
    SCRUTINY_STATUS status;
    U32 dword;
    U32 revision;
    U32 component;

    gPtrLoggerExpanders->logiFunctionEntry ("margayiRegisterBasedQualifyExpander (PtrDevice=%x)", PtrDevice != NULL);

    gPtrLoggerExpanders->logiVerbose ("Qualify Margay Expander with Handle type = %x", PtrDevice->HandleType);

    /* First we need to check for the Cobra/Cub device */
    status = bsdiMemoryRead32 (PtrDevice, REGISTER_ADDRESS_MARGAY_COMPONENT_ID_SIGNATURE, &dword, sizeof (U32));

    if (status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("margayiRegisterBasedQualifyExpander (MemorRead Status=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    component = ((dword >> 16) & 0xFFFF);
    revision = ((dword >>  8) & 0xFF);

    gPtrLoggerExpanders->logiDebug ("Margay Qualify Expander - Chip Signature = %x", dword);

    status = margayiQualifyMargayChipSignature (PtrDevice, component, revision);

    if (status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("margayiRegisterBasedQualifyExpander (Qualify Signature Status=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    margayRegisterBasedAssignSASAddress (PtrDevice);

    margayRegisterBasedGetActiveFirmwareVersion (PtrDevice, &PtrDevice->DeviceInfo.u.ExpanderInfo.FWVersion);

    gPtrLoggerExpanders->logiFunctionExit ("margayiRegisterBasedQualifyExpander (Status=%x)", status);

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS margayiQualifyMargayChipSignature (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 ComponentId, __IN__ U32 RevisionId)
{

    /*
     * 0x0240_00_00 (48 port Expander)
     * 0x242_00_00 (40 port Expander)
     * 0x244_00_00 (32 port Expander)
     * 0x246_00_00 (24 port Expander)
     */

    if (ComponentId == 0x240)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 48;
    }

    else if (ComponentId == 0x242)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 40;
    }

    else if (ComponentId == 0x244)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 32;
    }

    else if (ComponentId == 0x246)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 24;
    }

    else
    {
        return (SCRUTINY_STATUS_IGNORE);
    }

    PtrDevice->ProductFamily = SCRUTINY_PRODUCT_FAMILY_EXPANDER;
    PtrDevice->DeviceInfo.ProductFamily = SCRUTINY_PRODUCT_FAMILY_EXPANDER;
    PtrDevice->DeviceInfo.u.ExpanderInfo.ComponentId = ComponentId;
    PtrDevice->DeviceInfo.u.ExpanderInfo.RevisionId = RevisionId;
    PtrDevice->DeviceInfo.u.ExpanderInfo.IsBroadcomExpander = TRUE;

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS margayRegisterBasedAssignSASAddress (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status;

    gPtrLoggerExpanders->logiFunctionEntry ("margayRegisterBasedAssignSASAddress (PtrDevice=%x)", PtrDevice != NULL);

    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_MARGAY_SAS_ADDRESS_HIGH,
                               &PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High,
                               sizeof (U32));

    if (SCRUTINY_STATUS_SUCCESS != status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedAssignSASAddress (MemReadHigh=%x)", status);
        return (status);
    }

    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_MARGAY_SAS_ADDRESS_LOW,
                               &PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low,
                               sizeof (U32));

    if (SCRUTINY_STATUS_SUCCESS != status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedAssignSASAddress (MemReadLow=%x)", status);
        return (status);
    }

    gPtrLoggerExpanders->logiDebug ("Margay SAS Address is %x:%x", PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High,
                                   PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low);

    gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedAssignSASAddress()");

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS margayRegisterBasedPowerOnSenseRegisterGet (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrRegister)
{

    U32 data;
    SCRUTINY_STATUS status;


    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_MARGAY_POWER_ON_SENSE,
                               &data,
                               sizeof (U32));

    if (SCRUTINY_STATUS_SUCCESS != status)
    {
        return (status);
    }

    *PtrRegister = data;

    return (SCRUTINY_STATUS_SUCCESS);

}

BOOLEAN margayRegisterBasedPowerOnSenseIsCFIEnabled (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{
    U32 dword;

    if (margayRegisterBasedPowerOnSenseRegisterGet (PtrDevice, &dword))
    {
        return (FALSE);
    }

    if ((dword & 0xC0) == 0x80)
    {
        return (TRUE);
    }

    else
    {
        return (FALSE);
    }

}

SCRUTINY_STATUS margayRegisterBasedUploadBootloaderFlashTable (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_MARGAY_FLASH_TABLE *PtrFlashTable)
{

    /* First get the firmware header and see what is the size of the flash table */

    PU8 ptrFlashTable;
    MARGAY_FW_HEADER fwHeader;
    U32 data;
    U32 flashBaseAddress;

    gPtrLoggerExpanders->logiFunctionEntry ("margayRegisterBasedUploadBootloaderFlashTable (PtrDevice=%x, HandleType=%x)",
                                          PtrDevice != NULL, PtrDevice->HandleType);

    if (margayRegisterBasedPowerOnSenseIsCFIEnabled (PtrDevice))
    {
        flashBaseAddress = REGISTER_ADDRESS_MARGAY_XMEM_FLASH_START;
    }

    else
    {
        flashBaseAddress = REGISTER_ADDRESS_MARGAY_SPI_FLASH_START;
    }

    gPtrLoggerExpanders->logiDebug ("Margay Bootloader Flashbase address %x", flashBaseAddress);

    if (bsdiMemoryRead32 (PtrDevice, flashBaseAddress, (PU32) &fwHeader, sizeof (MARGAY_FW_HEADER)))
    {
        gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedUploadBootloaderFlashTable (Flash table header Read failed)");
        /* We will say that this is an invalid header. */
        return (SCRUTINY_STATUS_FAILED);
    }

    gPtrLoggerExpanders->logiDebug ("Margay Flash Table Bootloader firmware signature 0=%x, 1=%x, 2=%x",
                                  fwHeader.Signature0, fwHeader.Signature1, fwHeader.Signature2);

    if (fwHeader.Signature0 != MARGAY_MPI_FW_HEADER_SIGNATURE_0 ||
        fwHeader.Signature1 != MARGAY_MPI_FW_HEADER_SIGNATURE_1 ||
        fwHeader.Signature2 != MARGAY_MPI_FW_HEADER_SIGNATURE_2)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Now get the flash table */
    if (bsdiMemoryRead32 (PtrDevice, fwHeader.FlashTableOffset + flashBaseAddress, &data, 4))
    {
        gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedUploadBootloaderFlashTable (Flash table header Read failed)");

        /* We will say that this is an invalid header. */
        return (SCRUTINY_STATUS_FAILED);
    }

    data = (data >> 16) & 0xFF;
    data = (data * 12) + 4;

    /*
     * Now we have got the region table. Upload all .
     */

    ptrFlashTable = (PU8) sosiMemAlloc (data);

    /* Now get the flash table */

    if (bsdiMemoryRead32 (PtrDevice, fwHeader.FlashTableOffset + flashBaseAddress, (PU32) ptrFlashTable, data))
    {
        gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedUploadBootloaderFlashTable (Flash table Read failed)");

        /* We will say that this is an invalid header. */
        sosiMemFree (ptrFlashTable);
        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrFlashTable = (PTR_MARGAY_FLASH_TABLE) ptrFlashTable;

    gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedUploadBootloaderFlashTable()");

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 *  @method  atlasiFwImgGetConcatenatedImageByImageType()
 *
 *  @param   PtrImage   Firmware Image
 *
 *  @param   ImageSize  Size of the Image
 *
 *  @param   ImageType  Type of the image which needs to be checked in the given
 *                      image
 *
 *  @param   PtrOffset  Offset of the Image type that user wanted to get.
 *
 *  @return  Returns STATUS_SUCCESS if the image is available or STATUS_FAILED
 *           if the image is not available
 *
 *  @brief   Iterates through the firmware image and identifies whether the
 *           image has a concatenated chained image of the given type or not.
 *
 */

SCRUTINY_STATUS margayRegisterBasedGetFlashChainedImage (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 FlashOffset, __IN__ U32 FirstImageOffset, __IN__ U8 ImageType, __OUT__ PU8 *PtrImage, __OUT__ PU32 PtrSize)
{

    PU8 ptrImage;

    PTR_MARGAY_CHAINED_HEADER ptrChainedHeader;
    U32 runningOffset = 0;

    /*
     * From the firmware image get the chained header which is located on the next
     * image offset and iterate through the pages to validate each component.
     */

    if (MARGAY_HAS_NEXT_CHAIN_IMAGE (FirstImageOffset) != TRUE)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    runningOffset = FirstImageOffset;

    while (TRUE)
    {

        ptrImage = (PU8) sosiMemAlloc (sizeof (MARGAY_CHAINED_HEADER));

        if (bsdiMemoryRead32 (PtrDevice, FlashOffset + runningOffset, (PU32) ptrImage, sizeof (MARGAY_CHAINED_HEADER)))
        {
            sosiMemFree (ptrImage);
            return (SCRUTINY_STATUS_FAILED);
        }

        ptrChainedHeader = (PTR_MARGAY_CHAINED_HEADER) ptrImage;

        if (((ptrChainedHeader->ChainedHeaderId.HeaderType == MARGAY_HEADER_TYPE_LSI) ||
             (ptrChainedHeader->ChainedHeaderId.HeaderType == MARGAY_HEADER_TYPE_OEM) ||
             (ptrChainedHeader->ChainedHeaderId.HeaderType == MARGAY_HEADER_TYPE_FIRMWARE) ) &&
             ptrChainedHeader->ImageIdentifier.ImageType == ImageType)
        {

            /* We have got the image. Now upload all the image data. */

            ptrImage = (PU8) sosiMemRealloc (ptrImage, ptrChainedHeader->ImageSize, sizeof (MARGAY_CHAINED_HEADER));

            if (bsdiMemoryRead32 (PtrDevice,
                                  FlashOffset + runningOffset + ptrChainedHeader->OffsetToContent,
                                  (PU32) (&ptrImage[ptrChainedHeader->OffsetToContent]),
                                  ptrChainedHeader->ImageSize - sizeof (MARGAY_CHAINED_HEADER)))
            {
                sosiMemFree (ptrImage);
                return (SCRUTINY_STATUS_FAILED);
            }

            *PtrImage = ptrImage;
            *PtrSize = ptrChainedHeader->ImageSize;

            return (SCRUTINY_STATUS_SUCCESS);
        }

        if (MARGAY_HAS_NEXT_CHAIN_IMAGE (ptrChainedHeader->NextImageOffset) != TRUE)
        {
            sosiMemFree (ptrImage);
            break;
        }

        else
        {
            /* Go to next image */
            runningOffset += ptrChainedHeader->NextImageOffset;
            sosiMemFree (ptrImage);

        }

    }

    return (SCRUTINY_STATUS_FAILED);

}

/**
 *
 * @method  atlasPrintFwInfo()
 *
 * @param   RegionId        Region Id for which the Firmware version information
 *                          to be printed
 *
 * @param   RegionOffset    Offset for where the region resides to upload the
 *                          firmware data
 *
 * @return  STATUS_SUCCESS if the region is uploaded and printed successfully
 *          otherwise STATUS_FAILED will be returned
 *
 * @brief   Prints the version data for the given region
 *
 */

SCRUTINY_STATUS margayRegisterBasedGetFirmwareVersionIfActiveFW (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_MARGAY_FLASH_REGION_ENTRY PtrRegionEntry, __OUT__ PU32 PtrVersion)
{

    MARGAY_FW_HEADER fwHeader = { 0 };

    PU8 ptrFwStatus = NULL;
    U32 fwStatusSize = 0;

    PTR_MARGAY_FWSTATUS_IMAGE ptrFwStatusHeader;
    U32 flashBaseAddress;

    if (MARGAY_HALI_MEM_INTF_TYPE_XMEM == PtrRegionEntry->Flags.DevType)
    {
        flashBaseAddress = REGISTER_ADDRESS_MARGAY_XMEM_FLASH_START;
    }

    else
    {
        flashBaseAddress = REGISTER_ADDRESS_MARGAY_SPI_FLASH_START;
    }

    /*
     * First get the image and convert the same into the firmware header
     */

    if (bsdiMemoryRead32 (PtrDevice, flashBaseAddress + PtrRegionEntry->RegionOffset, (PU32) &fwHeader, sizeof (MARGAY_FW_HEADER)))
    {
        /* We will say that this is an invalid header. */
        return (SCRUTINY_STATUS_FAILED);
    }

    if (fwHeader.Signature0 != MARGAY_MPI_FW_HEADER_SIGNATURE_0 ||
        fwHeader.Signature1 != MARGAY_MPI_FW_HEADER_SIGNATURE_1 ||
        fwHeader.Signature2 != MARGAY_MPI_FW_HEADER_SIGNATURE_2)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* We will get the next Image offset from here. */

    if (margayRegisterBasedGetFlashChainedImage (PtrDevice,
                                                 flashBaseAddress,
                                                 PtrRegionEntry->RegionOffset + fwHeader.NextImageOffset,
                                                 MARGAY_CONCAT_IMAGE_TYPE_FWSTATUS,
                                                 &ptrFwStatus,
                                                 &fwStatusSize))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Check if we are having an active firmware version. */

    ptrFwStatusHeader = (PTR_MARGAY_FWSTATUS_IMAGE) &ptrFwStatus[sizeof (MARGAY_CHAINED_HEADER)];

    if (ptrFwStatusHeader->CurrentState == 0xFFFFFFFF)
    {

        sosiMemFree (ptrFwStatus);
        *PtrVersion = fwHeader.FWVersion.Word;

        gPtrLoggerExpanders->logiDebug ("Margay firmware Version selected %08x", *PtrVersion);

        return (SCRUTINY_STATUS_SUCCESS);
    }

    sosiMemFree (ptrFwStatus);

    return (SCRUTINY_STATUS_FAILED);

}


/**
 *
 * @method  edmGetActiveFirmwareVersionSdb()
 *
 * @return  STATUS_SUCCESS if the version information for all the regions are
 *          printed successfully otherwise STATUS_FAILED will be returned.
 *
 * @brief   Prints the version details for the firmware regions.
 *
 */

SCRUTINY_STATUS margayRegisterBasedGetActiveFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion)
{

    U32 index;
    PTR_MARGAY_FLASH_TABLE ptrFlashTable = NULL;
    U8 region;

    *PtrVersion = 0;

    gPtrLoggerExpanders->logiFunctionEntry ("margayRegisterBasedGetActiveFirmwareVersion()");

    if (margayRegisterBasedUploadBootloaderFlashTable (PtrDevice, &ptrFlashTable))
    {
        /* Error message must have already been displayed. Hence just return the SCRUTINY_STATUS_FAILED */
        return (SCRUTINY_STATUS_FAILED);
    }

    for (index = 0; index < ptrFlashTable->NumFlashTableEntries; index++)
    {
        region = ptrFlashTable->FlashRegionEntries[index].RegionType;

        switch (region)
        {
            case MARGAY_HALI_FLASH_BOOT_LOADER:
            case MARGAY_HALI_FLASH_FIRMWARE_COPY_1:
            case MARGAY_HALI_FLASH_FIRMWARE_COPY_2:
            {
                if (margayRegisterBasedGetFirmwareVersionIfActiveFW (PtrDevice, &ptrFlashTable->FlashRegionEntries[index], PtrVersion))
                {
                    continue;
                }

                else
                {
                    break;
                }

            }

            default:
            {
                continue;
            }

        }

    }

    sosiMemFree (ptrFlashTable);

    gPtrLoggerExpanders->logiFunctionExit ("margayRegisterBasedGetActiveFirmwareVersion()");

    return (SCRUTINY_STATUS_SUCCESS);

}

