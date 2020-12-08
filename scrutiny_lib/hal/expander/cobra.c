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
#include "cobra.h"
#include "margay.h"

#define REGISTER_OFFSET_REPORT_GENERAL_2 0xC3800104


SCRUTINY_STATUS cobRegisterBasedGetActiveFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion);
SCRUTINY_STATUS cobRegisterBasedGetActiveFirmwareComponentVersion (__IN__ PTR_FW_HEADER PtrBootloaderHeader, __IN__ PTR_FW_HEADER PtrActiveHeader, __IN__ PTR_FW_HEADER PtrBackupHeader, __OUT__ PU32 PtrVersion);
SCRUTINY_STATUS cobRegisterBasedGetAllFirmwareHeaders (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_FLASH_TABLE PtrTable,
                                                       __OUT__ PTR_FW_HEADER PtrBootloaderHeader, __OUT__ PTR_FW_HEADER PtrActiveHeader, __OUT__ PTR_FW_HEADER PtrBackupHeader);
SCRUTINY_STATUS cobRegisterBasedUploadBootloaderFlashTable (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_FLASH_TABLE *PtrFlashTable);
SCRUTINY_STATUS cobRegisterBasedAssignSASAddressForCubCobra (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);
SCRUTINY_STATUS cobRegisterBasedGetPhyNumberForCubCobra (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);


/* This will be for both Cobra and Cub devices */

SCRUTINY_STATUS cobiRegisterBasedQualifyExpander (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{
    SCRUTINY_STATUS status;
    U32 dword;
    U32 revision;
    U32 component;

    gPtrLoggerExpanders->logiFunctionEntry ("cobiRegisterBasedQualifyExpander (PtrDevice=%x)", PtrDevice != NULL);

    gPtrLoggerExpanders->logiVerbose ("Qualify Cobra Expander with Handle type = %x", PtrDevice->HandleType);

    /* First we need to check for the Cobra/Cub device */
    status = bsdiMemoryRead32 (PtrDevice, REGISTER_ADDRESS_COBRA_DEVICE_SIGNATURE, &dword, sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("cobiRegisterBasedQualifyExpander (MemorRead Status=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    component = ((dword >> 16) & 0xFFFF);
    revision = ((dword >>  8) & 0xFF);

    gPtrLoggerExpanders->logiDebug ("Cobra Qualify Expander - Chip Signature = %x", dword);

    status = cobiQualifyCubCobraChipSignature (PtrDevice, component, revision);

    if (status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("cobiRegisterBasedQualifyExpander (Qualify Signature Status=%x)", status);
        return (status);
    }

    /* We can now store the Cobra/Cub device's SAS address */
    cobRegisterBasedAssignSASAddressForCubCobra (PtrDevice);

	cobRegisterBasedGetPhyNumberForCubCobra (PtrDevice);

    /* We can now store the Firmware version */

    if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_SCSI)
    {
        /* SCSI interface. So asssign the firmware version based on the same */
        bsdiGetCurrentFirmwareVersion (PtrDevice, &PtrDevice->DeviceInfo.u.ExpanderInfo.FWVersion);
    }

    else
    {
        /* We have to get the Firmware version using the memory reads */
        cobRegisterBasedGetActiveFirmwareVersion (PtrDevice, &PtrDevice->DeviceInfo.u.ExpanderInfo.FWVersion);
    }

    gPtrLoggerExpanders->logiFunctionExit ("cobiRegisterBasedQualifyExpander (Status=%x)", status);

    return (SCRUTINY_STATUS_SUCCESS);

}


SCRUTINY_STATUS cobRegisterBasedGetPhyNumberForCubCobra (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{	
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;
	U32  phyNum = 0;

	gPtrLoggerExpanders->logiFunctionEntry ("cobRegisterBasedGetPhyNumberForCubCobra (PtrDevice=%x)", PtrDevice != NULL);

    status = bsdiMemoryRead32 (PtrDevice,
							   REGISTER_OFFSET_REPORT_GENERAL_2,
							   &phyNum,
							   sizeof (U32));

	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedGetPhyNumberForCubCobra (Status=%x)", status);
			
        return (SCRUTINY_STATUS_FAILED);
    }

    if (PtrDevice->DeviceInfo.u.ExpanderInfo.ComponentId == 0x023d)
    {
        phyNum = 24 + 3; //24 Actual phy's and 3 Virtual Phy's
    }
    else if (PtrDevice->DeviceInfo.u.ExpanderInfo.ComponentId == 0x023c)
    {
        phyNum = 28 + 3; //28 Actual phy's & 3 Virtaul Phy's
    }
    else
    {
        phyNum = (phyNum) >> 16;
        phyNum = ((phyNum) & 0x7F);
    }

	PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = phyNum;

	gPtrLoggerExpanders->logiDebug ("Phy Number %d \n", PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys);

	gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedGetPhyNumberForCubCobra (Status=%x)", status);

    return (SCRUTINY_STATUS_SUCCESS);

}


SCRUTINY_STATUS cobiQualifyCubCobraChipSignature (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 ComponentId, __IN__ U32 RevisionId)
{

    if (ComponentId == 0x0230)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 48;
    }

    else if (ComponentId == 0x0231)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 44;
    }

    else if (ComponentId == 0x0232)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 40;
    }

    else if (ComponentId == 0x0233)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 36;
    }

    else if (ComponentId == 0x0235)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 28;
    }

    else if (ComponentId == 0x0236)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 24;
    }

    else if (ComponentId == 0x0238)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 48; //Cub
    }

    else if (ComponentId == 0x0239)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 44;
    }

    else if (ComponentId == 0x023a)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 40;
    }

    else if (ComponentId == 0x023b)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 36;
    }

    else if (ComponentId == 0x023c)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 28;
    }

    else if (ComponentId == 0x023d)
    {
        PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = 24;
    }

    else
    {
        /* We don't support this, throw error message */
        return (SCRUTINY_STATUS_IGNORE);
    }

    PtrDevice->ProductFamily = SCRUTINY_PRODUCT_FAMILY_EXPANDER;
    PtrDevice->DeviceInfo.ProductFamily = SCRUTINY_PRODUCT_FAMILY_EXPANDER;
    PtrDevice->DeviceInfo.u.ExpanderInfo.ComponentId = ComponentId;
    PtrDevice->DeviceInfo.u.ExpanderInfo.RevisionId = RevisionId;
    PtrDevice->DeviceInfo.u.ExpanderInfo.IsBroadcomExpander = TRUE;

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS cobRegisterBasedAssignSASAddressForCubCobra (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    gPtrLoggerExpanders->logiFunctionEntry ("cobRegisterBasedAssignSASAddressForCubCobra (PtrDevice=%x)", PtrDevice != NULL);

    sosiMemSet (&PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress, 0,
               sizeof (PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress));

    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_COBRA_EXP_LINK_REGS (0) + 0x48,
                               (PU32) &PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High,
                               sizeof (U32));

    if (SCRUTINY_STATUS_SUCCESS != status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedAssignSASAddressForCubCobra (MemReadHigh=%x)", status);
        return (SCRUTINY_STATUS_FAILED);
    }

    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_COBRA_EXP_LINK_REGS (0) + 0x4C,
                               (PU32) &PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low,
                               sizeof (U32));

    if (SCRUTINY_STATUS_SUCCESS != status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedAssignSASAddressForCubCobra (MemReadLow=%x)", status);
        return (SCRUTINY_STATUS_FAILED);
    }

    PtrDevice->DeviceInfo.u.ExpanderInfo.EnclosureWwid.Low = PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low;
    PtrDevice->DeviceInfo.u.ExpanderInfo.EnclosureWwid.High = PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High;

    gPtrLoggerExpanders->logiDebug ("Cobra/Cub SAS Address is %x:%x", PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High,
                                   PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low);

    gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedAssignSASAddressForCubCobra()");

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS cobRegisterBasedUploadBootloaderFlashTable (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_FLASH_TABLE *PtrFlashTable)
{

    /* First get the firmware header and see what is the size of the flash table */

    PU8 ptrFlashTable;
    FW_HEADER fwHeader = { 0 };
    U32 data;

    gPtrLoggerExpanders->logiFunctionEntry ("cobRegisterBasedUploadBootloaderFlashTable (PtrDevice=%x, HandleType=%x)",
                                          PtrDevice != NULL, PtrDevice->HandleType);

    if (bsdiMemoryRead32 (PtrDevice, REGISTER_ADDRESS_COBRA_FLASH_START, (PU32) &fwHeader, sizeof (FW_HEADER)))
    {
        gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedUploadBootloaderFlashTable (FwHeader Read Failed)");
        return (SCRUTINY_STATUS_FAILED);
    }

    gPtrLoggerExpanders->logiDebug ("Cobra/Cub Flash Table Bootloader firmware signature 0=%x, 1=%x, 2=%x",
                                  fwHeader.Signature0, fwHeader.Signature1, fwHeader.Signature2);

    if (fwHeader.Signature0 != MPI_FW_HEADER_SIGNATURE_0 ||
        fwHeader.Signature1 != MPI_FW_HEADER_SIGNATURE_1 ||
        fwHeader.Signature2 != MPI_FW_HEADER_SIGNATURE_2)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Now get the flash table */
    if (bsdiMemoryRead32 (PtrDevice, fwHeader.FlashTableOffset + REGISTER_ADDRESS_COBRA_FLASH_START, &data, sizeof (U32)))
    {
        gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedUploadBootloaderFlashTable (Flash table header Read failed)");
        return (SCRUTINY_STATUS_FAILED);
    }

    data = (data >> 16) & 0xFF;
    data = (data * 12) + 4;

    /*
     * Now we have got the region table. Upload all .
     */

    ptrFlashTable = (PU8) sosiMemAlloc (data);

    /* Now get the flash table */

    if (bsdiMemoryRead32 (PtrDevice, fwHeader.FlashTableOffset + REGISTER_ADDRESS_COBRA_FLASH_START, (PU32) ptrFlashTable, data))
    {
        gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedUploadBootloaderFlashTable (Flash table Read failed)");
        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrFlashTable = (PTR_FLASH_TABLE) ptrFlashTable;

    gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedUploadBootloaderFlashTable()");

    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 * @method  edmGetAllFirmwareHeaders()
 *
 * @param   PtrTable    Flash table for printing the firmware versions.
 *
 * @return  STATUS_SUCCESS if the print operations are successfull otherwise
 *          STATUS_FAILED will be returned.
 *
 * @brief   Prints the version data from flash table by enumerating firmware
 *          regions.
 *
 */

SCRUTINY_STATUS cobRegisterBasedGetAllFirmwareHeaders (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_FLASH_TABLE PtrTable,
                                                       __OUT__ PTR_FW_HEADER PtrBootloaderHeader, __OUT__ PTR_FW_HEADER PtrActiveHeader, __OUT__ PTR_FW_HEADER PtrBackupHeader)
{

    U32 index;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;

    for (index = 0; index < PtrTable->NumFlashTableEntries; index++)
    {
        if (PtrTable->FlashRegionEntries[index].RegionType == HALI_FLASH_BOOT_LOADER)
        {

            if (bsdiMemoryRead32 (PtrDevice,
                                  PtrTable->FlashRegionEntries[index].RegionOffset + REGISTER_ADDRESS_COBRA_FLASH_START,
                                  (PU32) PtrBootloaderHeader,
                                  sizeof (FW_HEADER)))
            {
                status = SCRUTINY_STATUS_FAILED;
                break;
            }

        }

        else if (PtrTable->FlashRegionEntries[index].RegionType == HALI_FLASH_FIRMWARE_COPY_1)
        {
            if (bsdiMemoryRead32 (PtrDevice,
                                  PtrTable->FlashRegionEntries[index].RegionOffset + REGISTER_ADDRESS_COBRA_FLASH_START,
                                  (PU32) PtrActiveHeader,
                                  sizeof (FW_HEADER)))
            {
                status = SCRUTINY_STATUS_FAILED;
                break;
            }
        }

        else if (PtrTable->FlashRegionEntries[index].RegionType == HALI_FLASH_FIRMWARE_COPY_2)
        {
            if (bsdiMemoryRead32 (PtrDevice,
                                  PtrTable->FlashRegionEntries[index].RegionOffset + REGISTER_ADDRESS_COBRA_FLASH_START,
                                  (PU32) PtrBackupHeader,
                                  sizeof (FW_HEADER)))
            {
                status = SCRUTINY_STATUS_FAILED;
                break;
            }
        }

    }

    return (status);

}

SCRUTINY_STATUS cobRegisterBasedGetActiveFirmwareComponentVersion (__IN__ PTR_FW_HEADER PtrBootloaderHeader, __IN__ PTR_FW_HEADER PtrActiveHeader, __IN__ PTR_FW_HEADER PtrBackupHeader, __OUT__ PU32 PtrVersion)
{

    BOOLEAN active, backup;

    active = TRUE, backup = TRUE;

    gPtrLoggerExpanders->logiFunctionEntry ("cobRegisterBasedGetActiveFirmwareComponentVersion()");

    if (PtrActiveHeader->ArmBranchInstruction0 == 0xFFFFFFFF && PtrActiveHeader->FWVersion.Word == 0xFFFFFFFF &&
        PtrActiveHeader->Checksum == 0xFFFFFFFF && PtrActiveHeader->FwImageSize == 0xFFFFFFFF)
    {
        active = FALSE;
    }

    if (PtrBackupHeader->ArmBranchInstruction0 == 0xFFFFFFFF && PtrBackupHeader->FWVersion.Word == 0xFFFFFFFF &&
        PtrBackupHeader->Checksum == 0xFFFFFFFF && PtrBackupHeader->FwImageSize == 0xFFFFFFFF)
    {
        backup = FALSE;
    }

    if (active == TRUE && backup == FALSE)
    {
        gPtrLoggerExpanders->logiDebug ("Cobra/Cub Firmware Active Version selected %08x", PtrActiveHeader->FWVersion.Word);
        *PtrVersion = PtrActiveHeader->FWVersion.Word;
    }

    else if (active == FALSE && backup == TRUE)
    {
        gPtrLoggerExpanders->logiDebug ("Cobra/Cub Firmware Backup Version selected %08x", PtrBackupHeader->FWVersion.Word);
        *PtrVersion = PtrBackupHeader->FWVersion.Word;
    }

    else if (active == TRUE && backup == TRUE)
    {
        /* We have to  check nwe firmware signature */
        if (PtrBackupHeader->NewImageSignature1 == PtrActiveHeader->NewImageSignature1)
        {
            /* We have a fast boot. */
            gPtrLoggerExpanders->logiDebug ("Cobra/Cub Fast Boot Firmware Backup Version selected %08x", PtrBackupHeader->FWVersion.Word);
            *PtrVersion = PtrBackupHeader->FWVersion.Word;
        }

        else if (PtrActiveHeader->NewImageSignature1 != 0xFFFFFFFF)
        {
            gPtrLoggerExpanders->logiDebug ("Cobra/Cub Mix Active Firmware Version selected %08x", PtrActiveHeader->FWVersion.Word);
            *PtrVersion = PtrActiveHeader->FWVersion.Word;
        }

        else
        {
            gPtrLoggerExpanders->logiDebug ("Cobra/Cub Fallback Backup Firmware Version selected %08x", PtrBackupHeader->FWVersion.Word);
            *PtrVersion = PtrBackupHeader->FWVersion.Word;
        }
    }

    else
    {
        gPtrLoggerExpanders->logiDebug ("Cobra/Cub Bootloader firmware Version selected %08x", PtrBootloaderHeader->FWVersion.Word);
        *PtrVersion = PtrBootloaderHeader->FWVersion.Word;
    }

    gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedGetActiveFirmwareComponentVersion()");

    return (SCRUTINY_STATUS_SUCCESS);

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

SCRUTINY_STATUS cobRegisterBasedGetActiveFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion)
{

    PTR_FLASH_TABLE ptrFlashTable = NULL;

    FW_HEADER bootloaderHeader = { 0 };
    FW_HEADER activeHeader = { 0 };
    FW_HEADER backupHeader = { 0 };

    gPtrLoggerExpanders->logiFunctionEntry ("cobRegisterBasedGetActiveFirmwareVersion()");

    *PtrVersion = 0;

    if (cobRegisterBasedUploadBootloaderFlashTable (PtrDevice, &ptrFlashTable))
    {
        /* Error message must have already been displayed. Hence just return the STATUS */
        return (SCRUTINY_STATUS_FAILED);
    }

    if (cobRegisterBasedGetAllFirmwareHeaders (PtrDevice, ptrFlashTable, &bootloaderHeader, &activeHeader, &backupHeader))
    {
        sosiMemFree (ptrFlashTable);
        return (SCRUTINY_STATUS_FAILED);
    }

    cobRegisterBasedGetActiveFirmwareComponentVersion (&backupHeader, &activeHeader, &backupHeader, PtrVersion);

    sosiMemFree (ptrFlashTable);

    gPtrLoggerExpanders->logiFunctionExit ("cobRegisterBasedGetActiveFirmwareVersion()");

    return (SCRUTINY_STATUS_SUCCESS);

}

