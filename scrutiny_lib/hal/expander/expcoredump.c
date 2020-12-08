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
#include "atlas.h"

// instead of passing the open file handler as parameter, using a local global to store it
static SOSI_FILE_HANDLE    gFileHandle;
static char                gTempName[128];
static char                gCoreDumpFileName[512];

/**
 *
 * @method  ecdUploadCoreDumpRegion()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   PPtrBuffer     returned buffer pointer
 * 
 * @param   PtrRegionSize  pointer to the returned region size 
 *
 * @return  STATUS         Indication Success or Fail
 *
 * @brief   upload core dump region from the device to the
 *          allocated buffer, the caller need to release the
 *          buffer
 *
 *
 */
SCRUTINY_STATUS ecdUploadCoreDumpRegion (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __OUT__ PU8 *PPtrBuffer, 
    __OUT__ PU32 PtrRegionSize)
{

    PTR_FLASH_TABLE ptrFlashTable;
    U32 index;
    PU8 ptrCoreDump;
    U32 tempIndex = 0;
    U32 tempSize = 0;
    PTR_ATLAS_FLASH_TABLE  ptrAtlasFlashTable;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdUploadCoreDumpRegion (PtrDevice=0x%x, PPtrBuffer=0x%x, PtrRegionSize=0x%x)", 
                                            PtrDevice, PPtrBuffer, PtrRegionSize);

    if ((PPtrBuffer == NULL) || (PtrRegionSize == NULL))
    {
        gPtrLoggerExpanders->logiFunctionExit ("ecdUploadCoreDumpRegion (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    /*
     * First we have to read the flash table
     */

    ptrFlashTable = NULL;

    if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER) 
    {
        if (cobRegisterBasedUploadBootloaderFlashTable (PtrDevice, &ptrFlashTable))
        {
            gPtrLoggerExpanders->logiDebug ("Failed to get flash table");
            gPtrLoggerExpanders->logiFunctionExit ("ecdUploadCoreDumpRegion (status=0x%x)", SCRUTINY_STATUS_FAILED);
            return (SCRUTINY_STATUS_FAILED);
        }
    } 
    else if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        if (atlasGetFlashTable (PtrDevice, &ptrAtlasFlashTable))
        {
            gPtrLoggerExpanders->logiDebug ("Failed to get flash table");
            gPtrLoggerExpanders->logiFunctionExit ("ecdUploadCoreDumpRegion (status=0x%x)", SCRUTINY_STATUS_FAILED);
            return (SCRUTINY_STATUS_FAILED);
        }

        /* except the number of talbe entry, Flash Table structure for Expander and Atlas should be same, cast it */
        ptrFlashTable = (PTR_FLASH_TABLE) ptrAtlasFlashTable;
    }
    
    /* enumerate and find the region offset */

    for (index = 0; index < ptrFlashTable->NumFlashTableEntries; index++)
    {
        if (ptrFlashTable->FlashRegionEntries[index].RegionType == HALI_FLASH_COREDUMP)
        {
            break;
        }
    }

    /* No storage was available for saving the coredump image. */
    if (index >= ptrFlashTable->NumFlashTableEntries)
    {
        sosiMemFree ((void*) ptrFlashTable);
        gPtrLoggerExpanders->logiDebug ("No core dump region");
        gPtrLoggerExpanders->logiFunctionExit ("ecdUploadCoreDumpRegion (status=0x%x)", SCRUTINY_STATUS_NO_COREDUMP);
        return (SCRUTINY_STATUS_NO_COREDUMP);
    }

    ptrCoreDump = (PU8) sosiMemAlloc (ptrFlashTable->FlashRegionEntries[index].RegionSize);

    if (ptrCoreDump == NULL) 
    {
        sosiMemFree ((void*) ptrFlashTable);
        gPtrLoggerExpanders->logiDebug ("Failed to alloc mem");
        gPtrLoggerExpanders->logiFunctionExit ("ecdUploadCoreDumpRegion (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    /*
     * Upload the region.
     */
 
    for (tempIndex = 0, tempSize = 2048; tempIndex < ptrFlashTable->FlashRegionEntries[index].RegionSize; tempIndex += 2048)
    {

        /* We will upload 1024 bytes in one cycle and rotate the char */
        if ((tempIndex + 2048) > ptrFlashTable->FlashRegionEntries[index].RegionSize)
        {
            tempSize = ptrFlashTable->FlashRegionEntries[index].RegionSize - tempIndex;
        }

        if (bsdiMemoryRead32 (PtrDevice,
                                 ptrFlashTable->FlashRegionEntries[index].RegionOffset + REGISTER_ADDRESS_COBRA_FLASH_START + tempIndex,
                                 (PU32) &ptrCoreDump[tempIndex],
                                 tempSize))
        {
            /* We will say that this is an invalid header. */
            sosiMemFree ((void*) ptrFlashTable);
            sosiMemFree (ptrCoreDump);
            gPtrLoggerExpanders->logiDebug ("Failed to upload region");
            gPtrLoggerExpanders->logiFunctionExit ("ecdUploadCoreDumpRegion (status=0x%x)", SCRUTINY_STATUS_FAILED);
            return (SCRUTINY_STATUS_FAILED);
        }


    }
    
        /* We have got the data and give the entries to buffer */
    
    *PPtrBuffer = ptrCoreDump;
    *PtrRegionSize = ptrFlashTable->FlashRegionEntries[index].RegionSize;
    
    sosiMemFree ((void*) ptrFlashTable);

    gPtrLoggerExpanders->logiFunctionExit ("ecdUploadCoreDumpRegion (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  ecdIsCoreDumpValid()
 *
 *
 * @param   PtrCoreDumpHeader      pointer to header of the
 *                                 coredump data
 *
 *
 * @return  Boolean                TRUE or FALSE
 *
 * @brief   check whether the signature of the core dump region
 *          is valid
 *
 *
 */
BOOLEAN ecdIsCoreDumpValid (__IN__ PTR_EXP_COREDUMP_IMAGE_HEADER PtrCoreDumpHeader)
{
	if (PtrCoreDumpHeader->Signature == EXP_COREDUMP_IMAGE_SIGNATURE)
		return TRUE;
	else
		return FALSE;
}

/**
 *
 * @method  ecdDecodeCoreDumpImageHeader()
 *
 *
 * @param   PtrBuffer      pointer to output buffer 
 *
 * @param   PtrBufferSize     pointer to the input buffer length
 *                            and as output, it's the buffer
 *                            used
 * 
 * @param   PtrCoreDumpBuffer  original core dump data
 * 
 * @param   PtrNextOffset    Next offset of core dump data after
 *                           the header
 * 
 * @return  STATUS          Indication Success or Fail
 *
 * @brief   decode the core dump header for human-readable
 *          strings
 *
 *
 */
SCRUTINY_STATUS ecdDecodeCoreDumpImageHeader (
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      PU8                             PtrCoreDumpBuffer,
    __OUT__     PU32                            PtrNextOffset
)
{
    SCRUTINY_STATUS        status;
    PTR_EXP_COREDUMP_IMAGE_HEADER   ptrCoreDumpImageHeader;
    char*                   sourceStr;
    char*                   tempBuffer;
    U32                     tempBufferOffset = 0;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdDecodeCoreDumpImageHeader (PtrBuffer=0x%x, PtrBufferSize=0x%x, PtrCoreDumpBuffer=0x%x, "
                                            "PtrNextOffset=0x%x)", PtrBuffer != NULL, PtrBufferSize != NULL, 
                                            PtrCoreDumpBuffer != NULL, PtrNextOffset != NULL);

    ptrCoreDumpImageHeader = (PTR_EXP_COREDUMP_IMAGE_HEADER) PtrCoreDumpBuffer;

    tempBuffer = (char*) sosiMemAlloc (EXP_COREDUMP_INTERNAL_BUFFER_SIZE);
    if (tempBuffer == NULL)
    {
        gPtrLoggerExpanders->logiDebug ("Failed to Alloc mem");
        gPtrLoggerExpanders->logiFunctionExit ("ecdDecodeCoreDumpImageHeader (status=0x%x)", SCRUTINY_STATUS_NO_MEMORY);
        return (SCRUTINY_STATUS_NO_MEMORY);
    }
    
    switch (ptrCoreDumpImageHeader->Source)
    {
        case EXP_COREDUMP_SOURCE_WATCHDOG:
        {
            sourceStr = "Watchdog";
            break;
        }

        case EXP_COREDUMP_SOURCE_EXCEPTION_HDLR:
        {
            sourceStr = "ARM Exception Handler";
            break;
        }

        case EXP_COREDUMP_SOURCE_FWFAULT:
        {
            sourceStr = "Firmware Fault";
            break;
        }

        case EXP_COREDUMP_SOURCE_CLI:
        {
            sourceStr = "CLI Command";
            break;
        }
        default:
        {
            sourceStr = "Unknown Source";
            break;
        }
    }

    tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset, EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset, 
        "Source of coredump:  %s\r\n", sourceStr);


    tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                        EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                        "CoreDump TimeStamp:  <%02d:%02d:%02d:%02d.%03d>\r\n",
                                        (ptrCoreDumpImageHeader->SystemClockHigh / (24*60*60*1000)),
                                        ((ptrCoreDumpImageHeader->SystemClockLow / (60*60*1000)) % 24),
                                        ((ptrCoreDumpImageHeader->SystemClockLow / (60*1000)) % 60),
                                        ((ptrCoreDumpImageHeader->SystemClockLow / 1000) % 60),
                                        ((ptrCoreDumpImageHeader->SystemClockLow % 1000)));

    tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                     "Coredump Version:    0x%X\r\n", ptrCoreDumpImageHeader->HeaderVersion);

    tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                     "Coredump Length:     0x%X dwords (%d bytes)\r\n",
                                     ptrCoreDumpImageHeader->ImageLength, DWORDS_TO_BYTES (ptrCoreDumpImageHeader->ImageLength));

    /* Indicate whether the write complete flag is set.  For this flag
     * 0==TRUE and 1==FALSE
     */
    tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                     "Write Complete:      %s\r\n",
                                     ptrCoreDumpImageHeader->Flags.Bits.WriteFinished == 0 ? "TRUE" : "FALSE");

    if( ptrCoreDumpImageHeader->Flags.Bits.WriteFinished == 1 )
    {
        tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                     "\r\nWARNING!!! - It appears that "
                                    "the coredump creation was interrupted.\r\n"
                                    "Not all data may be available.\r\n");
    }                                 

    *PtrNextOffset = DWORDS_TO_BYTES (ptrCoreDumpImageHeader->HeaderSize);

    if (gFileHandle != NULL)
    {
        status = sosiFileWrite (gFileHandle, (PU8)tempBuffer, tempBufferOffset);
    }
    else 
    {

        if (tempBufferOffset <= *PtrBufferSize)
        {
            sosiMemCopy (PtrBuffer, tempBuffer, tempBufferOffset);
            status = SCRUTINY_STATUS_SUCCESS;
        }
        else 
        {
            sosiMemCopy (PtrBuffer, tempBuffer, *PtrBufferSize);
            status = SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH;
        }
        *PtrBufferSize = tempBufferOffset;
    }
    
    sosiMemFree (tempBuffer);
    gPtrLoggerExpanders->logiFunctionExit ("ecdDecodeCoreDumpImageHeader (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  ecdCoreDumpCalculateSectionChecksum()
 *
 *
 * @param   PtrSectionHeader      pointer to core dump section
 *                                Header
 * 
 * 
 * @return  U8                   calculated checksum
 *
 * @brief   calculate core dump section checksum
 *
 *
 */
U8 ecdCoreDumpCalculateSectionChecksum (PTR_EXP_COREDUMP_SECTION_HEADER PtrSectionHeader)
{
    U8 checksum = 0;
    U8 dataByte = 0;
    U32 currOffset;
    U32 sectionLen;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpCalculateSectionChecksum (PtrSectionHeader=0x%x)", PtrSectionHeader);

    sectionLen = DWORDS_TO_BYTES (PtrSectionHeader->SectionLength);

    for( currOffset = 0; currOffset < sectionLen; currOffset++ )
    {
        dataByte = *( (PU8)PtrSectionHeader + currOffset );
        checksum = checksum + dataByte;
    }

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpCalculateSectionChecksum (checksum=0x%x)", checksum);
	
    return checksum;
}

/**
 *
 * @method  haliRegsAccessGetBondOption()
 *
 *
 * @param   ComponentID      id from the device
 *
 * 
 * @return  HALI_EXP_BOND_OPTION          returned expander bond
 *          option
 *
 * @brief   decode the core dump header for human-readable
 *          strings
 *
 *
 */
HALI_EXP_BOND_OPTION haliRegsAccessGetBondOption ( U16 ComponentID )
{
    HALI_EXP_BOND_OPTION bondOption;

    switch (ComponentID)
    {
        case HAL_CUB_BOND_OPTION_48:
        case HAL_CORBA_BOND_OPTION_48:
        case HAL_MARGAY_BOND_OPTION_48:
        {
            bondOption = HALI_BOND_OPTION_0;
            break;
        }
        case HAL_CUB_BOND_OPTION_44:
        case HAL_CORBA_BOND_OPTION_44:
        {
            bondOption = HALI_BOND_OPTION_1;
            break;
        }
        case HAL_CUB_BOND_OPTION_40:
        case HAL_CORBA_BOND_OPTION_40:
        case HAL_MARGAY_BOND_OPTION_40:
        {
            bondOption = HALI_BOND_OPTION_2;
            break;
        }
        case HAL_CUB_BOND_OPTION_36:
        case HAL_CORBA_BOND_OPTION_36:
        {
            bondOption = HALI_BOND_OPTION_3;
            break;
        }
        case HAL_MARGAY_BOND_OPTION_32:
        {
            bondOption = HALI_BOND_OPTION_4;
            break;
        }
        case HAL_CUB_BOND_OPTION_28:
        case HAL_CORBA_BOND_OPTION_28:
        {
            bondOption = HALI_BOND_OPTION_5;
            break;
        }
        case HAL_CUB_BOND_OPTION_24:
        case HAL_CORBA_BOND_OPTION_24:
        case HAL_MARGAY_BOND_OPTION_24:
        {
            bondOption = HALI_BOND_OPTION_6;
            break;
        }
        default:
        {
            bondOption = HALI_BOND_OPTION_7;
            break;
        }
    }

    return bondOption;
}

/**
 *
 * @method  haliRegsAccessGetExpRevLevel()
 *
 *
 * @param   ComponentRevLevel      revision from the device
 *
 * 
 * @return  HALI_EXPANDER_REV_LEVEL          returned expander
 *          HW revision
 *
 * @brief   get HW revision
 *
 *
 */
HALI_EXPANDER_REV_LEVEL haliRegsAccessGetExpRevLevel ( U8 ComponentRevLevel )
{
    HALI_EXPANDER_REV_LEVEL expRevLevel;

    /* Component revision level */
    switch( ComponentRevLevel )
    {
        case HAL_EXP_REVISION_A0:
        {
            expRevLevel = HALI_EXPANDER_REV_LEVEL_0;
            break;
        }
        case HAL_EXP_REVISION_B0:
        {
            expRevLevel = HALI_EXPANDER_REV_LEVEL_1;
            break;
        }
        case HAL_EXP_REVISION_C0:
        {
            expRevLevel = HALI_EXPANDER_REV_LEVEL_2;
            break;
        }
        case HAL_EXP_REVISION_C1:
        {
            expRevLevel = HALI_EXPANDER_REV_LEVEL_3;
            break;
        }
        default:
        {
            expRevLevel = HALI_EXPANDER_REV_LEVEL_UNKNOWN;
            break;
        }
    }

    return expRevLevel;
}


/**
 *
 * @method  ecdCoreDumpPrintFwRevSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded FW revision section
 *
 *
 */
void ecdCoreDumpPrintFwRevSection (    
    __IN__  PU8                             PtrSectionData,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
)
{
    PTR_EXP_COREDUMP_SECTION_FWREV ptrFwRevData;
    const char *str;
    char *bondOptionString[] = {"48","44","40","36","32","28","24","Unknown"};
    char *expRevString[] = {"A0","B0","C0","C1","Unknown"};
    HALI_EXP_BOND_OPTION bondOption;
    HALI_EXPANDER_REV_LEVEL expRevLevel;
    U32                     tempBufferOffset = 0;
    char *ptrBuffer = (char *)PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpPrintFwRevSection (PtrSectionData=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, PtrBuffer, PtrBufferSize);

    ptrFwRevData = (PTR_EXP_COREDUMP_SECTION_FWREV) PtrSectionData;

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "\r\n-------------------------\r\n"
                                    "FIRMWARE REVISION DETAILS\r\n"
                                    "-------------------------\r\n\r\n"
                                    );
    
    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Component Type:  "
                                     );

                                     
    if( ptrFwRevData->ComponentType == HALI_COMPONENT_TYPE_CUB_R )
    {
        tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Cub-R/Cobra-R\r\n"
                                     );
    }
    else
    {
        tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Cub/Cobra\r\n"
                                     );
    }


    bondOption = haliRegsAccessGetBondOption (ptrFwRevData->ComponentID);
    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Component ID:  0x%04x (Bond Option: %s)\r\n",
                                    ptrFwRevData->ComponentID, bondOptionString[bondOption]
                                    );


    
    expRevLevel = haliRegsAccessGetExpRevLevel (ptrFwRevData->CompRevLevel);
    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                     "Component Revision Level:  %u (%s)\r\n",
                                    ptrFwRevData->CompRevLevel, expRevString[expRevLevel]
                                    );

    switch ( ptrFwRevData->ActiveFwRegion )
    {
        case HALI_COREDUMP_ACTIVE_FWREGION_BOOT:
        {
            str = "Boot Image";
            break;
        }

        case HALI_COREDUMP_ACTIVE_FWREGION_COPY1:
        {
            str = "Firmware Copy 1";
            break;
        }

        case HALI_COREDUMP_ACTIVE_FWREGION_COPY2:
        {
            str = "Firmware Copy 2";
            break;
        }

        case HALI_COREDUMP_ACTIVE_FWREGION_RAM:
        {
            str = "RAM Image";
            break;
        }

        default:
        {
            str = "Unknown Image";
            break;
        }
    }
    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                     "\r\nActive Firmware:  %s\r\n", str
                                    );

    if ( ptrFwRevData->ActiveFwRegion != HALI_COREDUMP_ACTIVE_FWREGION_RAM )
    {
        tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                         EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                         "\r\nFirmware Revision:  %d.%d.%d.%d\r\n",
                                        ptrFwRevData->FWVersion.Struct.Major,
                                        ptrFwRevData->FWVersion.Struct.Minor,
                                        ptrFwRevData->FWVersion.Struct.Unit,
                                        ptrFwRevData->FWVersion.Struct.Dev
                                        );
    }
    else
    {
        tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                         EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                         "\r\nFirmware Revision:  Not Available\r\n" );
    }

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                    EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "HAL Revision:  %d.%d.%d.%d\r\n",
                                    ptrFwRevData->HalLibVersion.Struct.Major,
                                    ptrFwRevData->HalLibVersion.Struct.Minor,
                                    ptrFwRevData->HalLibVersion.Struct.Unit,
                                    ptrFwRevData->HalLibVersion.Struct.Dev
                                    );

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                    EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "SES Revision:  %d.%d.%d.%d\r\n",
                                    ptrFwRevData->SesLibVersion.Struct.Major,
                                    ptrFwRevData->SesLibVersion.Struct.Minor,
                                    ptrFwRevData->SesLibVersion.Struct.Unit,
                                    ptrFwRevData->SesLibVersion.Struct.Dev
                                    );
    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                    EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "SCE Revision:  %d.%d.%d.%d\r\n",
                                    ptrFwRevData->SceLibVersion.Struct.Major,
                                    ptrFwRevData->SceLibVersion.Struct.Minor,
                                    ptrFwRevData->SceLibVersion.Struct.Unit,
                                    ptrFwRevData->SceLibVersion.Struct.Dev
                                    );
    
    *PtrBufferSize = tempBufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpPrintFwRevSection ()");
}

/**
 *
 * @method  ecdCoreDumpPrintExecContextSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded execution context section
 *
 *
 */
void ecdCoreDumpPrintExecContextSection ( 
    __IN__  PU8                             PtrSectionData,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
)
{
    PU32 ptrMem;
    PTR_HALI_COREDUMP_SECTION_EXEC_CONTEXT ptrECData;
    U32 stackAddress;
    U32 count;
    U32 numDwords;
    U32 bufferOffset = 0;
    char*  ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpPrintExecContextSection (PtrSectionData=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, PtrBuffer, PtrBufferSize);

    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "\r\n-------------------------\r\n"
                                    "EXECUTION CONTEXT DETAILS\r\n"
                                    "-------------------------\r\n\r\n"
                                     );

    ptrECData = (PTR_HALI_COREDUMP_SECTION_EXEC_CONTEXT) PtrSectionData;

    if( ptrECData->Flags.Bits.Valid == TRUE )
    {
        /* stash the stack pointer address */
        stackAddress = ptrECData->Registers.r13_prev;

        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "Processor Registers:\r\n"
                                     );

        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   Previous Stack Pointer (R13 prev): 0x%08x\r\n",
                                    ptrECData->Registers.r13_prev
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   Previous Link Register (R14 prev): 0x%08x\r\n",
                                    ptrECData->Registers.r14_prev
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   SPSR:                              0x%08x\r\n",
                                    ptrECData->Registers.cpsr_prev
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R14 (return address):              0x%08x\r\n",
                                    ptrECData->Registers.r14
                                     );


        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R0:                                0x%08x\r\n",
                                    ptrECData->Registers.r0
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R1:                                0x%08x\r\n",
                                        ptrECData->Registers.r1
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R2:                                0x%08x\r\n",
                                     ptrECData->Registers.r2
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R3:                                0x%08x\r\n",
                                    ptrECData->Registers.r3
                                     );

        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R4:                                0x%08x\r\n",
                                    ptrECData->Registers.r4
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R5:                                0x%08x\r\n",
                                        ptrECData->Registers.r5
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R6:                                0x%08x\r\n",
                                     ptrECData->Registers.r6
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R7:                                0x%08x\r\n",
                                    ptrECData->Registers.r7
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R8:                                0x%08x\r\n",
                                    ptrECData->Registers.r8
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R9:                                0x%08x\r\n",
                                        ptrECData->Registers.r9
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R10:                               0x%08x\r\n",
                                     ptrECData->Registers.r10
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R11:                               0x%08x\r\n",
                                    ptrECData->Registers.r11
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "   R12:                               0x%08x\r\n",
                                    ptrECData->Registers.r12
                                     );

        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "\r\nStack Data:\r\n"
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "\r\n Address  %9s%d%9s%d%9s%d%8s%d\r\n", " ", 0, " ", 4, " ", 8, " ", 12
                                     );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "=================================================="
                                     );

        numDwords = BYTES_TO_DWORDS( HALI_COREDUMP_EXEC_CONTEXT_STACK_SIZE );
        ptrMem = (PU32)&ptrECData->StackData[0];
        for( count = 0; count < numDwords ; count++ )
        {
            /* Print Address using the SP value from the execution context */
            if( count % 4 == 0 )
            {
                bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "\r\n%08x  ", stackAddress
                                     );
            }

            /* Display memory contents. */
            bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%2s%08x", " ", *((PU32)ptrMem)
                                     );
           

            /* Move to Next memory location. */
            ptrMem++;
            stackAddress+=4;
        }

    }
    else
    {
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "No valid execution context found.\r\n"
                                     );
    }
	
	bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                    EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "\r\n"
                                     );
    *PtrBufferSize = bufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpPrintExecContextSection ()" );
}

/**
 *
 * @method  ecdCoreDumpPrintThreadsSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded running threads section
 *
 *
 */
void ecdCoreDumpPrintThreadsSection ( 
    __IN__  PU8                             PtrSectionData,
    __IN__  U32                             SectionLength,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
)
{
    U32 threadCount;
    PTR_HALI_COREDUMP_THREAD ptrThread;
    U32 count;
    U32 bufferOffset = 0;
    char*  ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpPrintThreadsSection (PtrSectionData=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, PtrBuffer, PtrBufferSize);

    threadCount = ( DWORDS_TO_BYTES (SectionLength) -
                    sizeof (EXP_COREDUMP_SECTION_HEADER) ) /
                   sizeof (HALI_COREDUMP_THREAD);
      
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "\r\n-------------------------\r\n"
                                 "THREAD DETAILS\r\n"
                                 "-------------------------\r\n\r\n"
                                 "===============================================================\r\n"
                                 ); 
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "%-8s %-19s %5s %8s %6s %5s %-5s %-8s %8s %-7s %-8s\r\n",
                                 "", "", "", "Run  ", "Prior-", "Pre- ", "Time", "", "", " Stack", "Max Stack"
                                 ); 
    
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "%-8s %-19s %5s %8s %-6s %5s %5s %-8s %8s %-7s %-8s\r\n",
                                 " Id", "Name", "State", "Count",
                                 "ity", "Thrsh", "Slice", "Next-Id", "Suspd-Id", " Size", "Usage"
                                 ); 
								 
	bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
								 "===============================================================\r\n"
                                 ); 

    ptrThread = (PTR_HALI_COREDUMP_THREAD) PtrSectionData;

    for( count = 0; count < threadCount; count++ )
    {        
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "%8x %-22.22s %2x %8x %4d   %3d   %3d   %8x %8x  %3d   %3d\r\n",

                                 ptrThread->ID, ptrThread->Name, ptrThread->State, ptrThread->NumTimesRan,
                                 ptrThread->Priority, ptrThread->PreemptionThreshold, ptrThread->TimeSlice,
                                 ptrThread->NextID, ptrThread->NextSuspendedID,
                                 ptrThread->StackSize, ptrThread->MaxStackUsage
                                 ); 

        ptrThread += 1;
    }

    *PtrBufferSize = bufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpPrintThreadsSection ()");
}

/**
 *
 * @method  ecdCoreDumpPrintMemorySection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded memory pool section
 *
 *
 */
void ecdCoreDumpPrintMemorySection ( 
    __IN__  PU8                             PtrSectionData,
    __IN__  U32                             SectionLength,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
)
{
    U32 sectionDataSize;
    PTR_HALI_COREDUMP_MEMPOOL_COMMON ptrPoolCommon;
    PTR_HALI_COREDUMP_BYTEPOOL ptrBytePool;
    PTR_HALI_COREDUMP_NETXPOOL ptrNetxPool;
    U32 bufferOffset = 0;
    char*  ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpPrintMemorySection (PtrSectionData=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, PtrBuffer, PtrBufferSize);

    sectionDataSize = DWORDS_TO_BYTES (SectionLength) - sizeof (EXP_COREDUMP_SECTION_HEADER);

    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "\r\n-------------------------\r\n"
                                    "MEMORY POOL DETAILS\r\n"
                                    "-------------------------\r\n\r\n"
                                     );

    ptrPoolCommon = (PTR_HALI_COREDUMP_MEMPOOL_COMMON) PtrSectionData;

    /* make sure we have at least one byte pool to display */
    if( ptrPoolCommon->MemoryType == HALI_COREDUMP_MEMORYTYPE_BYTE )
    {
        ptrBytePool = (PTR_HALI_COREDUMP_BYTEPOOL) ptrPoolCommon;
        sectionDataSize += sizeof (HALI_COREDUMP_BYTEPOOL);

        /* Display the header for printing */
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "============================================================\r\n"
                                     );

        /* Display header for printing byte bool info */
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%15s %-20s %-15s %-15s\r\n",
                                     "Byte  ",
                                     " ",
                                     "Free",
                                     "Num   "
                                     );   
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%15s %-20s %-15s %-15s\r\n",
                                     "Pool ID",
                                     "Name",
                                     "Bytes",
                                     "Fragments"
                                     );   
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "============================================================\r\n"
                                     );

        do
        {
            bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%15x %-20s %-15d %-15d\r\n",
                                     ptrBytePool->Common.ID, ptrBytePool->Name,
                                     ptrBytePool->BytesAvailable, ptrBytePool->NumFragments
                                     );

            ptrBytePool += 1;
            ptrPoolCommon = (PTR_HALI_COREDUMP_MEMPOOL_COMMON) ptrBytePool;

            sectionDataSize += sizeof(HALI_COREDUMP_BYTEPOOL);

        } while ( (ptrPoolCommon->MemoryType == HALI_COREDUMP_MEMORYTYPE_BYTE) &&
                (sectionDataSize <= SectionLength) );
    }


    /* make sure we have at least one netx pool to display */
    if( ptrPoolCommon->MemoryType == HALI_COREDUMP_MEMORYTYPE_NETXPACKET )
    {
        ptrNetxPool = (PTR_HALI_COREDUMP_NETXPOOL) ptrPoolCommon;

        /* Display the header for printing */
        /* Display the header for printing */
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "\r\n\r\n============================================================\r\n"
                                     );

        /* Display header for printing byte bool info */
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%15s %-27s %-15s %-15s\r\n",
                                     "Netx Packet",
                                     " ",
                                     "Total",
                                     "Free "
                                     );   
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%15s %-27s %-15s %-15s\r\n",
                                     "ID   ",
                                     "Name",
                                     "Packets",
                                     "Packets"
                                     );   
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "============================================================\r\n"
                                     );
        
        do
        {
            bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%15x %-27s %-15d %-15d\r\n",
                                     ptrNetxPool->Common.ID, ptrNetxPool->Name,
                                     ptrNetxPool->TotalPackets, ptrNetxPool->FreePackets
                                     );


            ptrNetxPool += 1;
            ptrPoolCommon = (PTR_HALI_COREDUMP_MEMPOOL_COMMON) ptrNetxPool;

            sectionDataSize += sizeof(PTR_HALI_COREDUMP_NETXPOOL);

        }while ( (ptrPoolCommon->MemoryType == HALI_COREDUMP_MEMORYTYPE_NETXPACKET) &&
                (sectionDataSize <= SectionLength) );
    }

    *PtrBufferSize = bufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpPrintMemorySection ()");
}

/**
 *
 * @method  ecdCoreDumpPrintEdfbCountersSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded expander EDFB section
 *
 *
 */
void ecdCoreDumpPrintEdfbCountersSection ( 
    __IN__  PU8                             PtrSectionData,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
)
{
    PTR_HALI_COREDUMP_SECTION_EDFB_COUNTERS ptrEdfbData;
    U32 bufferOffset = 0;
    char*  ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpPrintEdfbCountersSection (PtrSectionData=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, PtrBuffer, PtrBufferSize);

    ptrEdfbData = (PTR_HALI_COREDUMP_SECTION_EDFB_COUNTERS) PtrSectionData;
    
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "\r\n-------------------------\r\n"
                                 "EDFB COUNTER DETAILS\r\n"
                                 "-------------------------\r\n\r\n"
                                 "Global Counters (aggregate across all EDFB enabled phys)\r\n"
                                 "--------------------------------------------------------\r\n\n"
                                 );

    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "Hard Reset Timeouts            %d\r\n",
                                 ptrEdfbData->HardResetTimeouts
                                 );

    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "Drive Signature Wait Timeouts  %d\r\n",
                                 ptrEdfbData->DriveSignatureWaitTimeouts
                                 );
    
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "SAS Errors Logged              %d\r\n",
                                 ptrEdfbData->SASErrorsLogged
                                 );
    
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "SAS Error Log Full             %d\r\n",
                                 ptrEdfbData->SASErrorLogFull
                                 );
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "SAS Error Unhandled Ints       %d\r\n",
                                 ptrEdfbData->SASErrorUnhandledInts
                                 );
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "SAS Error Log TimeOut Ints     %d\r\n",
                                 ptrEdfbData->SASErrorLogTimeOuts
                                 );
    
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "SAS Error Handler Trap Ints    %d\r\n",
                                 ptrEdfbData->SASErrorTraps
                                 );
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "SATA Errors Logged             %d\r\n",
                                 ptrEdfbData->SATAErrorsLogged
                                 );
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "SATA Error Log Full            %d\r\n",
                                 ptrEdfbData->SATAErrorLogFull
                                 );


    if( ptrEdfbData->EC17HitBT1237 != HALI_EDFB_PHY_SETTINGS_INVALID_COUNTER )
    {
        
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "EC17 encountering BT1237       %d\r\n",
                                     ptrEdfbData->EC17HitBT1237
                                     );
    }
    if( ptrEdfbData->HitBT1495 != HALI_EDFB_PHY_SETTINGS_INVALID_COUNTER )
    {
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "RxBBCrdTO w/ FIFO Full         %d\r\n",
                                     ptrEdfbData->HitBT1495
                                     );

    }
    if( ptrEdfbData->HitBT1467 != HALI_EDFB_PHY_SETTINGS_INVALID_COUNTER )
    {
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "Abort Task Realign Error       %d\r\n",
                                     ptrEdfbData->HitBT1467
                                     );
    }

    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "\r\n"
                                 );

    *PtrBufferSize = bufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpPrintEdfbCountersSection ()");
}

/**
 *
 * @method  ecdCoreDumpPrintEdfbCountersSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded Interrupt profiling section
 *
 *
 */
void ecbCoreDumpPrintIsrProfilingSection ( 
    __IN__  PU8                             PtrSectionData,
    __IN__  U32                             SectionLength,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
)
{
    PTR_IRQ_PERFORMANCE_TRACKING_VAR ptrIrqStat;
    U32 intSource;
    U32 numSources;
    U32 bufferOffset = 0;
    char*  ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecbCoreDumpPrintIsrProfilingSection (PtrSectionData=0x%x, SectionLength=0x%x, "
                                            "PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, SectionLength, PtrBuffer, PtrBufferSize);
    
    numSources = ( DWORDS_TO_BYTES (SectionLength) - sizeof (EXP_COREDUMP_SECTION_HEADER) ) / sizeof (IRQ_PERFORMANCE_TRACKING_VAR);
    
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "\r\n-------------------------\r\n"
                                     "ISR PROFILING DETAILS\r\n"
                                     "-------------------------\r\n\r\n"
                                     "Interrupt Source                   Count     Max Time     Min Time     Avg Time\r\n"
                                     "===============================================================================\r\n"
                                 );
    
    ptrIrqStat = (PTR_IRQ_PERFORMANCE_TRACKING_VAR) PtrSectionData;
    for (intSource = 0; intSource < numSources; intSource++ )
    {
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%-30.30s", ptrIrqStat->IrqName
                                 );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%10d     ", ptrIrqStat->Count
                                 );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%5d us     ", ptrIrqStat->MaxTime
                                 );
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%5d us     ", ptrIrqStat->MinTime == 0xFFFF ? 0 : ptrIrqStat->MinTime
                                 );
        
        if ( (ptrIrqStat->TotalTime == 0xFFFFFFFF) ||  (ptrIrqStat->Count == 0xFFFFFFFF) )
        {
            bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "overflow\r\n"
                                 );

        }
        else
        {
	    if (ptrIrqStat->Count == 0)
	    {
	   	bufferOffset += sosiSprintf (ptrBuffer + bufferOffset, 
					EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
					"%5d us\r\n", ptrIrqStat->Count
				);	
	    }
	    else 
	    {
            	bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "%5d us\r\n", ptrIrqStat->TotalTime / ptrIrqStat->Count
                                 );
	    }
        }
	
	ptrIrqStat += 1;
    }

    *PtrBufferSize = bufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("ecbCoreDumpPrintIsrProfilingSection ()");
}

/**
 *
 * @method  ecdCoreDumpPrintTraceLogSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded FW trace log section
 *
 *
 */
void ecdCoreDumpPrintTraceLogSection ( 
    __IN__  PU8                             PtrSectionData,
    __IN__  U32                             SectionLength,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize )
{
    U32 remaining;
    U32 bufferOffset = 0;
    char*  ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpPrintTraceLogSection (PtrSectionData=0x%x, SectionLength=0x%x, "
                                            "PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, SectionLength, PtrBuffer, PtrBufferSize);

    /* number of trace buffer bytes to process */
    remaining = DWORDS_TO_BYTES (SectionLength) - sizeof (EXP_COREDUMP_SECTION_HEADER);
    
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "\r\n-------------------------\r\n"
                                     "TRACE LOG DETAILS\r\n"
                                     "-------------------------\r\n\r\n"
                                 );
    /* trace buffer might be larger than the internal buffer, do some check */
    if (remaining > (EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset)) 
    {
        remaining = EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset;
    }
	
    /* The trace buffer should already printable, simply copy it out for performance */
    sosiMemCopy (ptrBuffer + bufferOffset, (VOID *)PtrSectionData, remaining);

    *PtrBufferSize = bufferOffset + remaining;

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpPrintTraceLogSection ()");

}
    
/**
 *
 * @method  ecdCoreDumpPrintOemSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded OEM section
 *
 *
 */
void ecdCoreDumpPrintOemSection ( 
    __IN__  PU8                             PtrSectionData,
    __IN__  U32                             SectionLength,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize 
)
{
    PU32 ptrMem;    
    U32 dataOffset = 0;
    U32 numDwords;
    U32 count;

    U32 bufferOffset = 0;
    char*  ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdCoreDumpPrintOemSection (PtrSectionData=0x%x, SectionLength=0x%x, "
                                            "PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, SectionLength, PtrBuffer, PtrBufferSize);

    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                     "\r\n-------------------------\r\n"
                                     "OEM DETAILS\r\n"
                                     "-------------------------\r\n\r\n"
                                 );
    numDwords = SectionLength - BYTES_TO_DWORDS(sizeof(EXP_COREDUMP_SECTION_HEADER));

    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "\r\n Address  %9s%d%9s%d%9s%d%8s%d\r\n", " ", 0, " ", 4, " ", 8, " ", 12
                                     );
    bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                    "=================================================="
                                     );    

    ptrMem = (PU32)PtrSectionData;
    
    for( count = 0; count < numDwords ; count++ )
    {
        /* Print Address using the SP value from the execution context */
        if( count % 4 == 0 )
        {
            bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                "\r\n%08x  ", dataOffset
                                 );
        }

        /* Display memory contents. */
        bufferOffset += sosiSprintf (ptrBuffer + bufferOffset,
                                 EXP_COREDUMP_INTERNAL_BUFFER_SIZE - bufferOffset,
                                 "%2s%08x", " ", *((PU32)ptrMem)
                                 );
       

        /* Move to Next memory location. */
        ptrMem++;
        dataOffset+=4;
    }

    *PtrBufferSize = bufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("ecdCoreDumpPrintOemSection ()");
}

/**
 *
 * @method  ecdDecodeCoreDumpSection()
 *
 *
 * @param   PtrSectionHeader    pointer to original core dump
 *                              section header
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   entry point function for all sections decode
 *          function
 *
 *
 */
SCRUTINY_STATUS ecdDecodeCoreDumpSection (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      PTR_EXP_COREDUMP_SECTION_HEADER PtrSectionHeader
    )
{
    SCRUTINY_STATUS     status;
    U8                  checksum;
    char*               tempBuffer;
    U32                 tempBufferOffset = 0;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdDecodeCoreDumpSection (PtrBuffer=0x%x, PtrBufferSize=0x%x, PtrSectionHeader=0x%x)", 
                                            PtrBuffer, PtrBufferSize, PtrSectionHeader);

    gPtrLoggerExpanders->logiDebug ("ecdDecodeCoreDumpSection() - buffer size %x", *PtrBufferSize);

    /* buffer used for error outputs */
    tempBuffer = (char*) sosiMemAlloc (EXP_COREDUMP_INTERNAL_BUFFER_SIZE);
	
    if (tempBuffer == NULL)
    {
        if (gFileHandle == NULL)
        {
            *PtrBufferSize = 0;
        }
		
        gPtrLoggerExpanders->logiDebug ("Failed to Alloc mem");
        gPtrLoggerExpanders->logiFunctionExit ("ecdDecodeCoreDumpSection (status=0x%x)", SCRUTINY_STATUS_NO_MEMORY);
		
        return (SCRUTINY_STATUS_NO_MEMORY);
    }
    
    checksum = ecdCoreDumpCalculateSectionChecksum (PtrSectionHeader);
	
    if (checksum == 0)
    {
        
        switch ( PtrSectionHeader->SectionType )
        {
            case EXP_COREDUMP_SECTIONTYPE_REVISION:
            {
                if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
                {
                    ecdCoreDumpPrintFwRevSection ( (PU8) (PtrSectionHeader + 1), (PU8)tempBuffer, &tempBufferOffset);
                }
                else
                {
                    scdCoreDumpPrintFwRevSection ( (PU8) (PtrSectionHeader + 1), (PU8)tempBuffer, &tempBufferOffset);
                }
                
                break;
            }

            case EXP_COREDUMP_SECTIONTYPE_EXECUTION_CONTEXT:
            {
                ecdCoreDumpPrintExecContextSection ( (PU8) (PtrSectionHeader + 1), (PU8)tempBuffer, &tempBufferOffset);
                break;
            }

            case EXP_COREDUMP_SECTIONTYPE_THREADS:
            {
                ecdCoreDumpPrintThreadsSection ( (PU8) (PtrSectionHeader + 1), PtrSectionHeader->SectionLength, 
                    (PU8)tempBuffer, &tempBufferOffset);
                break;
            }

            case EXP_COREDUMP_SECTIONTYPE_MEMORY:
            {
                ecdCoreDumpPrintMemorySection((PU8) (PtrSectionHeader + 1), PtrSectionHeader->SectionLength, 
                    (PU8)tempBuffer, &tempBufferOffset); 
                break;
            }

            case EXP_COREDUMP_SECTIONTYPE_EDFBCOUNTERS:
            {
                ecdCoreDumpPrintEdfbCountersSection ( (PU8) (PtrSectionHeader + 1), (PU8)tempBuffer, &tempBufferOffset);
                break;
            }

            case EXP_COREDUMP_SECTIONTYPE_ISR_PROFILING:
            {
                ecbCoreDumpPrintIsrProfilingSection ((PU8) (PtrSectionHeader + 1), PtrSectionHeader->SectionLength, 
                    (PU8)tempBuffer, &tempBufferOffset); 
                break;
            }

            case EXP_COREDUMP_SECTIONTYPE_TRACELOG:
            {
                ecdCoreDumpPrintTraceLogSection ( (PU8) (PtrSectionHeader + 1), PtrSectionHeader->SectionLength, 
                    (PU8)tempBuffer, &tempBufferOffset );
                break;
            }

            case EXP_COREDUMP_SECTIONTYPE_OEM:
            {
                ecdCoreDumpPrintOemSection ( (PU8) (PtrSectionHeader + 1), PtrSectionHeader->SectionLength,
                    (PU8)tempBuffer, &tempBufferOffset);
                break;
            }
            default:
            {
                tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                             EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                             "\r\nError: Found an unknown section type.\r\n");
                tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                             EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                            "Section Type: 0x%x\r\n", PtrSectionHeader->SectionType);
                tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                             EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                            "Section Version: 0x%x\r\n", PtrSectionHeader->Version);
                tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                             EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                            "Checksum Byte in header: 0x%x\r\n", PtrSectionHeader->Checksum);
                tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                             EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                            "SectionLength: 0x%x\r\n", PtrSectionHeader->SectionLength);
            }
        }
    }
    else 
    {
        /* the checksum is bad, return SUCCESS to skip this section but still try to 
        decode the following sections */
        tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "\r\nERROR: Found a section but the checksum was bad (0x%02x).\r\n", 
                                    checksum);
        tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Section Type: 0x%x\r\n", PtrSectionHeader->SectionType);
        tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Section Version: 0x%x\r\n", PtrSectionHeader->Version);
        tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Checksum Byte in header: 0x%x\r\n", PtrSectionHeader->Checksum);
        tempBufferOffset += sosiSprintf (tempBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "SectionLength: 0x%x\r\n", PtrSectionHeader->SectionLength);
    }

    if (gFileHandle != NULL)
    {
        status = sosiFileWrite (gFileHandle, (PU8)tempBuffer, tempBufferOffset);
    }
    else 
    {

        if (tempBufferOffset <= *PtrBufferSize)
        {
            sosiMemCopy (PtrBuffer, tempBuffer, tempBufferOffset);
            status = SCRUTINY_STATUS_SUCCESS;
        }
        else 
        {
            sosiMemCopy (PtrBuffer, tempBuffer, *PtrBufferSize);
            status = SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH;
        }
        *PtrBufferSize = tempBufferOffset;
    }

    sosiMemFree (tempBuffer);
    gPtrLoggerExpanders->logiDebug ("ecdDecodeCoreDumpSection returned size %x", *PtrBufferSize);
    gPtrLoggerExpanders->logiFunctionExit ("ecdDecodeCoreDumpSection (status=0x%x)", status);
    return (status);
    
}

/**
 *
 * @method  ecdDecodeCoreDump()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @param   PtrCoreDumpBuffer   original core dump data
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   entry point for decode core dump data
 *
 *
 */
SCRUTINY_STATUS ecdDecodeCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      PU8                             PtrCoreDumpBuffer
)
{
    SCRUTINY_STATUS        status;
    U32                    nextOffset = 0;  // track the core dump buffer
    U32                    runningSize, remainSize = 0;   // track the decoded buffer
    PTR_EXP_COREDUMP_SECTION_HEADER     ptrSectionHeader;
    PU8                    ptrCurrentPosition = PtrBuffer;
    U32                    totalSize = 0;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdDecodeCoreDump (PtrDevice=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x, PtrCoreDumpBuffer=0x%x)",
                                            PtrDevice != NULL, PtrBuffer != NULL, PtrBufferSize != NULL, PtrCoreDumpBuffer != NULL);

    if (gFileHandle == NULL)
    {
        runningSize = remainSize = *PtrBufferSize;
    }

    status = ecdDecodeCoreDumpImageHeader (PtrBuffer, &runningSize, PtrCoreDumpBuffer, &nextOffset);
    
    if (gFileHandle == NULL)
    {
        totalSize += runningSize;
        
        /* move the point and update the remain buffer size */ 
        if (status == SCRUTINY_STATUS_SUCCESS) 
        {
            ptrCurrentPosition += runningSize;
            remainSize -= runningSize;
            runningSize = remainSize;
        } 
        else if (status == SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH)
        {
            // the input buffer is not large enough, still go forward to calculate the required size
            remainSize = runningSize = 0;
        }
        else 
        {
            gPtrLoggerExpanders->logiDebug ("Failed to decode core dump image Header");
            gPtrLoggerExpanders->logiFunctionExit ("ecdDecodeCoreDump() - status 0x%x", status);
            return (status);
        }
    }
    
    while (nextOffset < (DWORDS_TO_BYTES ( ( (PTR_EXP_COREDUMP_IMAGE_HEADER) PtrCoreDumpBuffer)->ImageLength) ) )
    {
        ptrSectionHeader = (PTR_EXP_COREDUMP_SECTION_HEADER) (PtrCoreDumpBuffer + nextOffset);

        status = ecdDecodeCoreDumpSection (PtrDevice, ptrCurrentPosition, &runningSize, ptrSectionHeader);

        if (gFileHandle == NULL)
        {
            totalSize += runningSize;

            gPtrLoggerExpanders->logiDebug ("ecdDecodeCoreDump() - TotalSize %x", totalSize);
            
            if (status == SCRUTINY_STATUS_SUCCESS)
            {
                ptrCurrentPosition += runningSize;
                remainSize -= runningSize;
                runningSize = remainSize;
            }
            else if (status == SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH)
            {
                remainSize = runningSize = 0;
            }
            else 
            {
                break;
            }
        }

        nextOffset += DWORDS_TO_BYTES (ptrSectionHeader->SectionLength);
        
    }

    if (gFileHandle == NULL)
    {
        *PtrBufferSize = totalSize;
    }
    
    gPtrLoggerExpanders->logiFunctionExit ("ecdDecodeCoreDump (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  ecdiGetCoreDump()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @param   Type                request decoded or original core
 *                              dump data
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   upload core dump data from device and decide whether
 *          return original or decoded data
 *
 *
 */
SCRUTINY_STATUS ecdiGetCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      EXP_COREDUMP_TYPE               Type,
    __IN__      const char*                     PtrFolderName
)
{
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_FAILED;
    PU8                    ptrCoreDumpBuffer;
    U32                    bufferSize;
    
    gPtrLoggerExpanders->logiFunctionEntry ("ecdiGetCoreDump (PtrDevice=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x, Type=%d, PtrFolderName=%x)",
                                            PtrDevice != NULL, PtrBuffer != NULL, PtrBufferSize != NULL, Type, PtrFolderName != NULL);                            
                                            
    status = ecdUploadCoreDumpRegion (PtrDevice, &ptrCoreDumpBuffer, &bufferSize);    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("ecdiGetCoreDump (status=0x%x)", status);
        return (status);
    }

    if (!ecdIsCoreDumpValid ((PTR_EXP_COREDUMP_IMAGE_HEADER)ptrCoreDumpBuffer))
    {
        sosiMemFree (ptrCoreDumpBuffer);
        gPtrLoggerExpanders->logiFunctionExit ("ecdiGetCoreDump (status=0x%x)", status);
        return (SCRUTINY_STATUS_NO_COREDUMP);
    }
	
	/* Open a file only when there is core dump */
	if (PtrFolderName != NULL)
    {
        // saving to file
        sosiMemSet (gCoreDumpFileName, '\0', sizeof (gCoreDumpFileName));
        sosiMemSet (gTempName, '\0', sizeof (gTempName));
        
        status = sosiMkDir (PtrFolderName);
		
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerExpanders->logiDebug ("The input folder is neither accessible nor created");
            gPtrLoggerExpanders->logiFunctionExit ("ecdiGetCoreDump() - status 0x%x", status);
            return (status);
        }

        sosiStringCopy (gCoreDumpFileName, PtrFolderName);

        if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
        {
            sosiSprintf (gTempName, sizeof (gTempName), "coredump.txt");
        }
        else
        {
            sosiSprintf (gTempName, sizeof (gTempName), "coredump.txt");
        }

        sosiStringCat (gCoreDumpFileName, gTempName);
        gFileHandle = sosiFileOpen (gCoreDumpFileName, "wb");

        if (!gFileHandle)
		{	
			gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs() - status 0x%x", SCRUTINY_STATUS_FILE_OPEN_FAILED);
			return (SCRUTINY_STATUS_FILE_OPEN_FAILED);	
		}	
    }
    else
    {
        // copying to buffer
        if ( (PtrBuffer == NULL) || (PtrBufferSize == NULL) )
        {
            gPtrLoggerExpanders->logiFunctionExit ("ecdiGetCoreDump (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
            return (SCRUTINY_STATUS_INVALID_PARAMETER);
        }

        gFileHandle = NULL;
    }

    gPtrLoggerExpanders->logiDumpMemoryInVerbose ((PU8)ptrCoreDumpBuffer, bufferSize, gPtrLoggerExpanders);

    if (Type == EXP_COREDUMP_RAW)
    {
        status = SCRUTINY_STATUS_SUCCESS;

        if (gFileHandle != NULL)
        {
            status = sosiFileWrite (gFileHandle, (PU8)ptrCoreDumpBuffer, bufferSize);
        }
        else 
        {
            // copy to buffer
            if (*PtrBufferSize < bufferSize)
            {
                status = SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH;
                bufferSize = *PtrBufferSize;
            }
            else
            {
                *PtrBufferSize = bufferSize;
            }
            
            sosiMemCopy (PtrBuffer, ptrCoreDumpBuffer, bufferSize);
        }
    } 
    else 
    {
        status = ecdDecodeCoreDump (PtrDevice, PtrBuffer, PtrBufferSize, ptrCoreDumpBuffer);
    }

    if (gFileHandle != NULL)
    {
        sosiFileClose (gFileHandle); 
    }

    sosiMemFree (ptrCoreDumpBuffer);
    gPtrLoggerExpanders->logiFunctionExit ("ecdiGetCoreDump (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  ecdiEraseCoreDump()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   Erase core dump data region from the device, does
 *          NOT support SDB interface.
 *
 *
 */
SCRUTINY_STATUS ecdiEraseCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice
)
{
    SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;
    U32             sizeTotal = 0;
    PU8             ptrBuffer;
    U8              bufferId = BRCM_SCSI_BUFFER_ID_DIRECT_REGION_0 + HALI_FLASH_COREDUMP;

    gPtrLoggerExpanders->logiFunctionEntry ("ecdiEraseCoreDump (PtrDevice=0x%x)", PtrDevice);

    /* do not support erase core dump region when there is no FW */
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB) 
    {
        gPtrLoggerExpanders->logiFunctionExit ("ecdiEraseCoreDump (status=0x%x)", SCRUTINY_STATUS_UNSUPPORTED);
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    status = bsdiGetRegionSize (PtrDevice, bufferId, &sizeTotal);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiDebug ("Failed to get region size ");
        gPtrLoggerExpanders->logiFunctionExit ("ecdiEraseCoreDump (status=0x%x)", status);
        return (status);
    }

    ptrBuffer = sosiMemAlloc (sizeTotal);
    if (ptrBuffer == NULL)
    {
        gPtrLoggerExpanders->logiDebug ("Failed to Alloc mem");
        gPtrLoggerExpanders->logiFunctionExit ("ecdiEraseCoreDump (status=0x%x)", SCRUTINY_STATUS_NO_MEMORY);
        return (SCRUTINY_STATUS_NO_MEMORY);
    }

    /* We have to reset the image values */
    sosiMemSet (ptrBuffer, 0xFF, sizeTotal);

    status = bsdiDownloadRegion (PtrDevice, bufferId, ptrBuffer, sizeTotal);
   
    sosiMemFree (ptrBuffer);

    gPtrLoggerExpanders->logiFunctionExit ("ecdiEraseCoreDump (status=0x%x)", status);
    return (status);
}

