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
#include "atlasfwheader.h"
#include "atlasregisters.h"
#include "atlas.h"

static U32 gSBR_OFFSET_TABLE[] = {
    0x000400, 0x040400, 0x080400, 0x0C0400, 0x100400
};

SCRUTINY_STATUS atlasiRegisterBasedQualifSwitch (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    U32 chip;
    U32 revision;

    SCRUTINY_STATUS status;

    gPtrLoggerGeneric->logiFunctionEntry ("atlasiRegisterBasedQualifSwitch (PtrDevice=%x, HandleType=%x)", PtrDevice != NULL, PtrDevice->HandleType);

    gPtrLoggerGeneric->logiVerbose ("Qualify Atlas Switch with Handle type = %x", PtrDevice->HandleType);

    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_ATLAS_CCR_CHIP_DEVICE_ID,
                               &chip,
                               sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerGeneric->logiFunctionExit ("atlasiRegisterBasedQualifSwitch (DevId Read Failed=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_ATLAS_CCR_CHIP_REVISION,
                               &revision,
                               sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerGeneric->logiFunctionExit ("atlasiRegisterBasedQualifSwitch (Revision Failed=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    gPtrLoggerGeneric->logiDebug ("Atlas component signatures ChipId=%x, RevisonId=%x", chip, revision);

    /*
     * We need to assign the Product string.
     */

    status = atlasiAssignProductString (PtrDevice, chip, revision);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerGeneric->logiFunctionExit ("atlasiRegisterBasedQualifSwitch (Assign Product=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    PtrDevice->ProductFamily = SCRUTINY_PRODUCT_FAMILY_SWITCH;
    PtrDevice->DeviceInfo.ProductFamily = SCRUTINY_PRODUCT_FAMILY_SWITCH;
    atlasGetSwitchMode (PtrDevice);
    
    gPtrLoggerGeneric->logiFunctionExit ("atlasiRegisterBasedQualifSwitch()");

    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 *  @method  atlasiQualifySwitch()
 *
 *  @param   PtrDevice  Pointer to Switch device
 *
 *  @return  STATUS     '0' for success and  non-zero for failure
 *
 *  @brief   This method will assign the FW version, SAS address and
 *           product string to the exapander
 *
 */

SCRUTINY_STATUS atlasiQualifySGSwitch (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status;

    gPtrLoggerGeneric->logiFunctionEntry ("atlasiQualifySGSwitch (PtrDevice=%x, HandleType=%x)", PtrDevice != NULL, PtrDevice->HandleType);

    status = atlasiRegisterBasedQualifSwitch (PtrDevice);

    if (status)
    {
        gPtrLoggerGeneric->logiFunctionExit ("atlasiQualifySGSwitch (status=%x)", status);

        return (status);
    }

    /*
     * We need to add SAS Address and firmware version.
     */

    atlasSGAssignSASAddress (PtrDevice);

    /*
     * We need to add firmware version.
     */

    atlasSGAssignFirmwareVersion (PtrDevice, &PtrDevice->DeviceInfo.u.SwitchInfo.FWVersion);

    PtrDevice->DeviceInfo.u.SwitchInfo.PciAddress = PtrDevice->Handle.ScsiHandle.AdapterPCIAddress;
    atlasGetSwitchMode (PtrDevice);

    gPtrLoggerGeneric->logiFunctionExit ("atlasiQualifySGSwitch()");

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS atlasSGAssignFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion)
{

    U32 regionSize = 0;
    PU8 ptrFirmware = NULL;
    PTR_ATLAS_FW_HEADER ptrFwHeader = NULL;
    U32 version = 0xFFFFFFFF;
    U32 index;

    U8 region[] = { BRCM_SCSI_BUFFER_ID_RUNNING_FIRMWARE,
                    BRCM_SCSI_BUFFER_ID_VIRTUAL_ACTIVE_1,
                    BRCM_SCSI_BUFFER_ID_VIRTUAL_ACTIVE_2,
                    BRCM_SCSI_BUFFER_ID_REGION_BACKUP_FIRMWARE };

    gPtrLoggerGeneric->logiFunctionEntry ("atlasSGAssignFirmwareVersion (PtrDevice=%x)", PtrDevice != NULL);

    for (index = 0; index < 4; index++)
    {
        /* We can get the firmware version by getting the current firmware */

        if (bsdiGetRegion (PtrDevice, region[index], &ptrFirmware, &regionSize))
        {
            continue;
        }

        ptrFwHeader = (PTR_ATLAS_FW_HEADER) ptrFirmware;

        version = ptrFwHeader->FWVersion.Word;

        sosiMemFree (ptrFirmware);

        if ((version != 0xFFFFFFFF) && (version != 0x00000000))
        {
            gPtrLoggerGeneric->logiDebug ("Atlas SCSI Device - Firmware Version=%x", version);

            *PtrVersion = version;
            return (SCRUTINY_STATUS_SUCCESS);
        }

    }

    gPtrLoggerGeneric->logiFunctionExit ("atlasSGAssignFirmwareVersion (Status=Failed)");

    /* We couldn't get the version, just return as is */
    *PtrVersion = 0x0;

    return (SCRUTINY_STATUS_FAILED);


}



/**
 *
 *  @method  atlasiAssignProductString()
 *
 *  @param   PtrDevice          pointer to Switch device
 *
 *  @param   ComponentValue     componenet value to get the ID of Switch
 *
 *  @param   Revision           Revision of the product for which the text
 *                              needed
 *
 *  @return  STATUS_SUCCESS if the product string has been assigned successfuly
 *           otherwise STATUS_FAILED will be returned.
 *
 *  @brief   This method will assign the product string to the exapander
 *
 */

SCRUTINY_STATUS atlasiAssignProductString (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 ComponentValue, __IN__ U32 Revision)
{

    U32 vendorId;
    U32 chipId;

    /*
     * --------------------------------------------------------------------------
     * - Component ID (16bit wide)          - Vendor ID (16bit Wide)            -
     * --------------------------------------------------------------------------
     *
     */

    chipId    = ((ComponentValue >> 16) & 0xFFFF);
    vendorId  =  (ComponentValue & 0xFFFF);

    if (REGISTER_DATA_ATLAS_VENDOR_ID == vendorId)
    {

        if ((chipId & REGISTER_DATA_MASK_ATLAS_CHIP_ID) == REGISTER_DATA_ATLAS_CHIP_ID_HIGH)
        {
            PtrDevice->DeviceInfo.u.SwitchInfo.ComponentId = (U16) chipId;
        }

        else
        {
            return (SCRUTINY_STATUS_FAILED);
        }

    }

    else
    {

        /*
         * Return failing because, we don't support this product as it could be non-atlas
         * Switchs.
         */

        return (SCRUTINY_STATUS_FAILED);

    }

    /* Revision values are 'A0' 'B0'. So we don't have to check seperately and print.*/

    PtrDevice->DeviceInfo.u.SwitchInfo.RevisionId = (U16)Revision;

    return (SCRUTINY_STATUS_SUCCESS);

}


SCRUTINY_STATUS atlasSGAssignSASAddress (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    U32 pageSize = 0;
    PU8 ptrPage = NULL;
    SCRUTINY_STATUS status;

    gPtrLoggerGeneric->logiFunctionEntry ("atlasSGAssignSASAddress (PtrDevice=%x)", PtrDevice != NULL);

    status  = bsdiGetConfigPage (PtrDevice,
                                 0xE000,
                                 BRCM_SCSI_DEVICE_CONFIG_PAGE_REGION_ANY,
                                 &ptrPage,
                                 &pageSize);

    if (status)
    {
        gPtrLoggerGeneric->logiFunctionExit ("atlasSGAssignSASAddress (ConfigPageFailed Status=%x)", status);
        return (SCRUTINY_STATUS_FAILED);
    }

    PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.Low = (U32) ((PU32) ptrPage)[1];
    PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.High  = (U32) ((PU32) ptrPage)[2];

    gPtrLoggerGeneric->logiDebug ("Atlas SCSI SAS Address=%x:%x", PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.High,
                                  PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.Low);

    sosiMemFree (ptrPage);

    gPtrLoggerGeneric->logiFunctionExit ("atlasSGAssignSASAddress()");

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  atlasiIsSBRBootloaderRegionAvailable()
 *
 * @param   PtrDevice   pointer to scrutiny device
 *
 * @param   Region      Flash Region Id only SBRBootloader 1 and SBRBootloader 2
 *                      accepted. Foreverythong else FALSE will be returned.
 *
 * @return  TRUE if the region is available, FALSE if the region is not
 *          available
 *
 * @brief   Checks the SBRBootloader region is available or not.
 *
 */
BOOLEAN atlasIsSBRBootloaderRegionAvailable (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 Region)
{

    U32 sbrHeader = 0;
    U32 flashTableLength = 0;
    U32 regionOffset = 0;
    U32 tempOffset = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("atlasiIsSBRBootloaderRegionAvailable (PtrDevice=%x, Region=%x)",
                                          PtrDevice != NULL, Region);

    if ((ATLAS_FLASH_SBR_BOOTLOADER_1 != Region) &&
        (ATLAS_FLASH_SBR_BOOTLOADER_2 != Region))
    {
        return (FALSE);
    }

    if (ATLAS_FLASH_SBR_BOOTLOADER_2 == Region)
    {
        regionOffset = gSBR_OFFSET_TABLE[1] - ATLAS_HALI_FLASH_SBR_START_OFFSET;
    }

    /* First we need to read the no. of flash table entries and we have to calculate appropriately. */
    tempOffset = REGISTER_ADDRESS_ATLAS_FLASH + regionOffset + ATLAS_HALI_FLASH_FLASHTABLE_OFFSET;

    if (bsdiMemoryRead32 (PtrDevice, tempOffset, (PU32)  &flashTableLength, sizeof (U32)))
    {
        gPtrLoggerSwitch->logiFunctionExit ("atlasiIsSBRBootloaderRegionAvailable (flashTableLength Read Failed)");
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Read a DWORD for the SBR and see if the header is valid. */
    tempOffset = REGISTER_ADDRESS_ATLAS_FLASH + regionOffset + ATLAS_HALI_FLASH_SBR_START_OFFSET;

    if (bsdiMemoryRead32 (PtrDevice, tempOffset, (PU32)  &sbrHeader, sizeof (U32)))
    {
        gPtrLoggerSwitch->logiFunctionExit ("atlasiIsSBRBootloaderRegionAvailable (sbrHeader Read Failed)");
        return (SCRUTINY_STATUS_FAILED);
    }


    /** NOTE: This check is just to enable whether we have the correct
     *  data. Although we don't have a header to check, we are seeing if the
     *  table length is correct enough and SBR is available. */

    if ((ATLAS_MAX_FLASH_TABLE_ENTRIES > flashTableLength) &&
        (ATLAS_SBR_HW_SIGNATURE != sbrHeader))
    {
        return (FALSE);
    }

    return (TRUE);

}

/**
 *
 * @method  atlasiFlashTableGetRegionOffset()
 *
 * @param   PtrFlashTable       Flash Table from which the region offset to get
 *
 * @param   RegionId            Region id for which the offset to be extracted
 *
 * @param   PtrRegionOffset     Output varabile where the region offset to be
 *                              stored.
 *
 * @return  STATUS_SUCCESS if the region is found and offset extracted otherwise
 *          if not found, STATUS_FAILED will be returned.
 *
 * @brief   Returns the Region offset from the flash table for the given Region
 *          Id.
 *
 */

SCRUTINY_STATUS atlasFlashTableGetRegionOffset (
    __IN__ PTR_ATLAS_FLASH_TABLE PtrFlashTable, 
    __IN__ U8 RegionId, 
    __OUT__ PU32 PtrRegionOffset)
{

    U32 index;

    *PtrRegionOffset = 0;

    if (PtrFlashTable->NumFlashTableEntries > ATLAS_MAX_FLASH_TABLE_ENTRIES)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    for (index = 0; index < PtrFlashTable->NumFlashTableEntries; index++)
    {
        if (PtrFlashTable->FlashRegionEntries[index].RegionType == RegionId)
        {
            *PtrRegionOffset = PtrFlashTable->FlashRegionEntries[index].RegionOffset;
            return (SCRUTINY_STATUS_SUCCESS);
        }
    }

    /* We are not able to get the region id. */

    return (SCRUTINY_STATUS_FAILED);

}

/**
 *
 * @method  atlasDeviceFwSignatureCheck()
 *
 * @param   PtrFlashTable       Flash table which needs to be checked if it is
 *                              correct
 *
 * @return  STATUS_SUCCESS if the flash table contains the valid offsets and
 *          expeted results otherwise STATUS_FAILED.
 *
 * @brief   Uploads the Firmware copy region and checks, if the region has the
 *          valid firmware by seeing the header.
 *
 */

SCRUTINY_STATUS atlasDeviceFwSignatureCheck (
    __IN__ PTR_SCRUTINY_DEVICE PtrDevice,
    __IN__ PTR_ATLAS_FLASH_TABLE PtrFlashTable, 
    __IN__ U8 Region)
{

    U32 regionOffset = 0;
    U32 signatureHeader[4] = { 0 };

    if (atlasFlashTableGetRegionOffset (PtrFlashTable, Region, &regionOffset))
    {
        /* We didnt find in any of the copy. Hence return failure. */
        return (SCRUTINY_STATUS_FAILED);
    }

    /* This is simple hack, if the offset goes out of 8 MB, we may have a wrong data. */
    if (regionOffset > (8 * 1024 * 1024))
    {
        /* We don't have valid table. */
        return (SCRUTINY_STATUS_FAILED);
    }

    /*
     * We seem to be good. Check if the place has a valid hader. We will upload 16 bytes to
     * check only signatures.
     */
    if (bsdiMemoryRead32 (PtrDevice, REGISTER_ADDRESS_ATLAS_FLASH + regionOffset, (PU32)  signatureHeader, 16))
    {
        return (SCRUTINY_STATUS_FAILED);
    }


    if (signatureHeader[1] == MPI_ATLAS_FW_HEADER_SIGNATURE_0 &&
        signatureHeader[2] == MPI_ATLAS_FW_HEADER_SIGNATURE_1 &&
        signatureHeader[3] == MPI_ATLAS_FW_HEADER_SIGNATURE_2)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    /* We have something wrong in the header. */

    return (SCRUTINY_STATUS_FAILED);
}

/**
 *
 * @method  atlasiFlashTableIsValid()
 *
 * @param   PtrFlashTable       Flash table which needs to be checked if it is
 *                              correct
 *
 * @return  STATUS_SUCCESS if the flash table contains the valid offsets and
 *          expeted results otherwise STATUS_FAILED.
 *
 * @brief   Uploads the Firmware copy region and checks, if the region has the
 *          valid firmware by seeing the header.
 *
 */

SCRUTINY_STATUS atlasFlashTableIsValid (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_ATLAS_FLASH_TABLE PtrFlashTable)
{

    U8* ptrBlankData = NULL;

    /* First we will see, if the flash table is valid. */

    ptrBlankData = (PU8) sosiMemAlloc (sizeof (ATLAS_FLASH_TABLE));

    sosiMemSet (ptrBlankData, 0xFF, sizeof (ATLAS_FLASH_TABLE));

    if (sosiMemCompare ((PU8) PtrFlashTable, ptrBlankData, sizeof (ATLAS_FLASH_TABLE)) == SCRUTINY_STATUS_SUCCESS)
    {
        sosiMemFree (ptrBlankData);

        /* We have a blank data. Which means, we don't have a valid firmware in it. */
        return (SCRUTINY_STATUS_FAILED);
    }

    /* We don't need blank data here after. */
    sosiMemFree (ptrBlankData);

    /* We don't have a valid flash. But check if the data has any valid parts. */

    if (PtrFlashTable->NumFlashTableEntries > ATLAS_MAX_FLASH_TABLE_ENTRIES)
    {
        /* We don't have a valid flash table entries. */
        return (SCRUTINY_STATUS_FAILED);
    }

    if (atlasDeviceFwSignatureCheck (PtrDevice, PtrFlashTable, ATLAS_FLASH_FIRMWARE_COPY_1))
    {
        /* We didnt find in Copy 1. Check Copy 2. */
        if (atlasDeviceFwSignatureCheck (PtrDevice, PtrFlashTable, ATLAS_FLASH_FIRMWARE_COPY_2))
        {
            return (SCRUTINY_STATUS_FAILED);
        }
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 *  @method  atlasiGetFlashTable()
 *
 *  @param   PtrData            PtrData into which the Flash table will be
 *                              uploaded into.
 *                              Note: User will have to free this pointer upon
 *                              usage.
 *
 *  @param   PtrSizeInBytes     Size in bytes allocated in PtrData.
 *
 *  @return  STATUS_SUCCESS if the flash table has been retrieved successfully
 *           otherwise STATUS_FAILED will be returned.
 *
 *  @brief   Method will check the Bootloader and extracts the Flash Table and
 *           uploaded into. If the SBR read fails, STATUS_FAILED will be
 *           returned and PtrData will not get allocated.
 *
 */

SCRUTINY_STATUS atlasGetFlashTable (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice,
    __OUT__ PTR_ATLAS_FLASH_TABLE *PtrFlashTable
    )
{
    U32 regionOffset = 0;
    U32 flashTableLength = sizeof (ATLAS_FLASH_TABLE);
    U32 tempOffset = 0;
    PTR_ATLAS_FLASH_TABLE ptrFlashTable;

    ptrFlashTable = sosiMemAlloc (sizeof (ATLAS_FLASH_TABLE));
    if (ptrFlashTable == NULL)
    {
        *PtrFlashTable = NULL;
        return SCRUTINY_STATUS_FAILED;
    }

    /*
     * Check if we have the Bootloader Region 1. Since both are expected to be an identical
     * copies, one of the region having a valid data is sufficient
     */

    if (!atlasIsSBRBootloaderRegionAvailable (PtrDevice, ATLAS_FLASH_SBR_BOOTLOADER_1))
    {
        if (!atlasIsSBRBootloaderRegionAvailable (PtrDevice, ATLAS_FLASH_SBR_BOOTLOADER_2))
        {
            /* We don't have any valid data available */
            return (SCRUTINY_STATUS_FAILED);
        }

        else
        {
            regionOffset = gSBR_OFFSET_TABLE[1] - ATLAS_HALI_FLASH_SBR_START_OFFSET;
        }
    }

    tempOffset = REGISTER_ADDRESS_ATLAS_FLASH + ATLAS_HALI_FLASH_FLASHTABLE_OFFSET + regionOffset;

    if (bsdiMemoryRead32 (PtrDevice, tempOffset, (PU32) ptrFlashTable, flashTableLength))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* We got the flash table. Now, check if the table is good. */

    if (atlasFlashTableIsValid (PtrDevice, (PTR_ATLAS_FLASH_TABLE) ptrFlashTable))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrFlashTable = ptrFlashTable;

    return (SCRUTINY_STATUS_SUCCESS);
}


SCRUTINY_STATUS atlasGetSwitchMode (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{
    SCRUTINY_STATUS status;
    U32             switchMode;
    U32             sesEpDisable;
    
    gPtrLoggerGeneric->logiFunctionEntry ("atlasGetSwitchMode (PtrDevice=%x, HandleType=%x)", PtrDevice != NULL, PtrDevice->HandleType);


    //first read switch mode bit, if value = 0, means base mode (include with/without firmware)
    //                            if value = 1, means synthetic mode
    //in base mode ,to tell it is with/without firmware, then read sesEpDisable bit 
    //                            if sesEpDisable = 0, means has firmware
    //                            if sesEpDisable = 1, means has not firmware  



    //Read switch mode information
    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_ATLAS_CCR_ATLAS_MODE,
                               &switchMode,
                               sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {   // I don't want return, so continue 
        gPtrLoggerGeneric->logiDebug ("atlasGetSwitchMode (Read Switch Mode Failed=%x)", status);
        PtrDevice->DeviceInfo.u.SwitchInfo.SwitchMode = SWITCH_MODE_UNKNOWN;
    }
    else
    {

        gPtrLoggerGeneric->logiDebug ("Atlas switchMode=%x ", switchMode);

        status = bsdiMemoryRead32 (PtrDevice,
                                   REGISTER_ADDRESS_ATLAS_CCR_PCIE_MISC_CONFIG,
                                   &sesEpDisable,
                                   sizeof (U32));

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            // I don't want return, so continue
            gPtrLoggerGeneric->logiDebug ("atlasGetSwitchMode (Read Switch SES ep disable Failed=%x)", status);
            PtrDevice->DeviceInfo.u.SwitchInfo.SwitchMode = SWITCH_MODE_UNKNOWN;
        }
        else
        {
            gPtrLoggerGeneric->logiDebug ("Atlas sesEpDisable=%x ", sesEpDisable);
            switchMode = switchMode & 0x03;
            sesEpDisable = (sesEpDisable >> 22) &0x1; 

            if (switchMode == 0)
            {
                if (sesEpDisable == 0)
                {
                    PtrDevice->DeviceInfo.u.SwitchInfo.SwitchMode = SWITCH_MODE_BASE_WITH_iSSW;
                }
                else
                {
                    PtrDevice->DeviceInfo.u.SwitchInfo.SwitchMode = SWITCH_MODE_BASE;
                }
            }
            else if (switchMode == 1)
            {
                PtrDevice->DeviceInfo.u.SwitchInfo.SwitchMode = SWITCH_MODE_iSSW;
            }
            else
            {
                PtrDevice->DeviceInfo.u.SwitchInfo.SwitchMode = SWITCH_MODE_UNKNOWN;
            }
        }
    }
    //End read switch mode information

    gPtrLoggerGeneric->logiFunctionExit ("atlasGetSwitchMode() Status = %x", status);

    return (status);
}
/**
 *
 * @method  atlasPCIeConfigurationSpaceRead()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   Port           port for the regiser
 *
 * @param   Offset         register offset to be retrieved
 * 
 * @param   PtrValue       pointer to the register value
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read the configration register value of specific port
 *
 *
 */
SCRUTINY_STATUS atlasPCIeConfigurationSpaceRead (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__  U32 Port, 
    __IN__  U32 Offset, 
    __OUT__ PU32 PtrValue
)
{

    U32 baseAddress = ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE + (Port * ATLAS_REGISTER_SIZE_PORT_CONFIG_SPACE_SIZE);

    baseAddress += Offset;

    return (bsdiMemoryRead32 (PtrDevice, baseAddress, PtrValue, sizeof (U32)));

}

/**
 *
 * @method  atlasPCIeConfigurationSpaceWrite()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   Port           port for the regiser
 *
 * @param   Offset         register offset to be retrieved
 * 
 * @param   PtrValue       written value
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   write the configration register value of specific port
 *
 *
 */
SCRUTINY_STATUS atlasPCIeConfigurationSpaceWrite ( 
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__  U32 Port, 
    __IN__  U32 Offset, 
    __IN__  U32 Value
)
{

    U32 baseAddress = ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE + (Port * ATLAS_REGISTER_SIZE_PORT_CONFIG_SPACE_SIZE);

    baseAddress += Offset;

    return (bsdiMemoryWrite32 (PtrDevice, baseAddress, &Value));
}

