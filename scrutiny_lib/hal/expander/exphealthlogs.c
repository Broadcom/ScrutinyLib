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

#define  EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE    (1024)

static char sTempBuffer [EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE];
static U32  sTempSize;


/**
 *
 * @method  ehlFormatDecodedHealthLogs()
 *
 *
 * @param   PtrHealthLogs       pointer device health log
 *
 * @param   LogsSize            size of health log
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferLength     buffer length
 * 
 * @param   PtrFile             File Handler 
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   copy and format the decoded log into the output buffer
 *
 *
 */
SCRUTINY_STATUS ehlFormatDecodedHealthLogs (
    __IN__  PU8 PtrHealthLogs , 
    __IN__  U32 LogsSize, 
    __OUT__ PU8 PtrBuffer, 
    __OUT__ PU32 PtrBufferLength,
    __IN__  SOSI_FILE_HANDLE  PtrFile
)
{
    U32 index = 0;
    SCRUTINY_STATUS    status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerExpanders->logiFunctionEntry ("ehlFormatDecodedHealthLogs (PtrHealthLogs=%x, LogsSize=0x%x, PtrBuffer=%x, PtrBufferLength=%x)", 
            PtrHealthLogs != NULL, LogsSize, PtrBuffer != NULL,  PtrBufferLength != NULL);

    if (PtrFile == NULL)
    {
        if (LogsSize > *PtrBufferLength)
        {
            *PtrBufferLength = LogsSize + SCRUTINY_LIB_EXTRA_BUFFER_SIZE;
            gPtrLoggerExpanders->logiDebug ("INSUFFICIENT buffer 0x%x", LogsSize );
    		gPtrLoggerExpanders->logiFunctionExit ("ehlFormatHealthLogs (status=0x^x)", SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH);                                       
            return (SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH);
        }

        for (index = 0; index < LogsSize; index++)
        {
            PtrBuffer[index] = PtrHealthLogs[index];
        }

        *PtrBufferLength = LogsSize;
    }
    else
    {
        status = sosiFileWrite (PtrFile, (PU8)PtrHealthLogs, LogsSize);
    }


    gPtrLoggerExpanders->logiFunctionExit ("ehlFormatHealthLogs (status=0x^x)", status);  
    return (status);
}


/**
 *
 * @method  ehlFormatHealthLogs()
 *
 *
 * @param   PtrHealthLogs       pointer device health log
 *
 * @param   LogsSize            size of health log
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferLength     buffer length
 * 
 * @param   PtrFile             File handler 
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   copy and format the binary log into the output buffer
 *
 *
 */
SCRUTINY_STATUS ehlFormatHealthLogs (
    __IN__  PU8 PtrHealthLogs , 
    __IN__  U32 LogsSize, 
    __OUT__ PU8 PtrBuffer, 
    __OUT__ PU32 PtrBufferLength,
    __IN__  SOSI_FILE_HANDLE  PtrFile
    )
{
    U32     index = 0;
    PU32    ptrTemp = NULL;
    PS8     ptrChar = NULL;
    U32     totalSize = 0;
	U32 	stringSize = 0;

    PTR_EXP_LOG_ENTRY_STRING    ptrLogEntryString;
    SCRUTINY_STATUS    			status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerExpanders->logiFunctionEntry ("ehlFormatHealthLogs (PtrHealthLogs=%x, LogsSize=0x%x, PtrBuffer=%x," 
                        "PtrBufferLength=%x, PtrFile=%x)", PtrHealthLogs != NULL, LogsSize, PtrBuffer != NULL,  
                        PtrBufferLength != NULL, PtrFile != NULL);

    stringSize = sizeof (EXP_LOG_ENTRY_STRING) - (sizeof (EXP_LOG_ENTRY_HEADER) + sizeof (EXP_LOG_DATA_HEADER));

    /*
    * Refer the header structure LOG_ENTRY_STRING in cobhealth.h.
    */

    while (LogsSize)
    {

        ptrLogEntryString = (PTR_EXP_LOG_ENTRY_STRING) PtrHealthLogs;

        ptrChar = (PS8) PtrHealthLogs + 24;
        ptrTemp = (PU32) ptrChar;

        sTempSize = 0;

        sosiMemSet ((char *) sTempBuffer, '\0', EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE);

        /*
        * Checking here if the log is valid or not by checking the time stamp.
        * If the log is invalid, sending back abort so that we wont fetch more
        * logs and exit from there.
        */

        if (ptrLogEntryString->LogEntryHeader.Timestamp.Low == 0x00 &&
            ptrLogEntryString->LogEntryHeader.Timestamp.High == 0x00)
        {
            break;
        }
            
        if (ptrLogEntryString->LogEntryHeader.Timestamp.High == 0xFFFFFFFF &&
            ptrLogEntryString->LogEntryHeader.Timestamp.Low  == 0xFFFFFFFF)
        {
            break;
        }

        sTempSize = sosiSprintf (sTempBuffer, (EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize), 
                            "Timestamp           : 0x%08x : 0x%08x \n", 
                            ptrLogEntryString->LogEntryHeader.Timestamp.High,
                            ptrLogEntryString->LogEntryHeader.Timestamp.Low);

        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                            "Log Sequence        : 0x%04x\n", 
                            ptrLogEntryString->LogEntryHeader.LogSequence);

        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                            "Log Entry Qualifier : 0x%04x\n", 
                            ptrLogEntryString->LogEntryHeader.LogEntryQualifier);

        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                            "Log Code            : 0x%08x\n", 
                            ptrLogEntryString->LogDataHeader.LogCode);

        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                            "Locale              : 0x%04x\n", 
                            ptrLogEntryString->LogDataHeader.Locale);

        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                            "Argument Type       : 0x%02x\n", 
                            ptrLogEntryString->LogDataHeader.ArgumentType);

        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                            "Log Class           : 0x%02x\n", 
                            ptrLogEntryString->LogDataHeader.LogClass);
        
        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                            "Log Specific Data   : ");


        for (index = 0 ; index < stringSize/4 ; index++)
        {
            if (index == 0)
            {
                sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, "0x%08x ", *ptrTemp);
            }
            else
            {
                sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, "%20s: 0x%08x ", "", *ptrTemp);
            }


            if ( 0x20 < *ptrChar  )
            {
                sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, "%c", *ptrChar);
            }

            if ( 0x20 < *(ptrChar + 1) )
            {
                sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, "%c", *(ptrChar + 1) );
            }

            if ( 0x20 < *(ptrChar + 2) )
            {
                sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, "%c", *(ptrChar + 2) );
            }

            if ( 0x20 < *(ptrChar + 3) )
            {
                sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, "%c", *(ptrChar + 3) );
            }

            ptrChar = ptrChar + 4;
            ptrTemp = ptrTemp + 1;

            sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, "\n" );
            
        }

        sTempSize += sosiSprintf (sTempBuffer+sTempSize, EXP_HEALTH_LOGS_TEMP_BUFFER_SIZE - sTempSize, 
                "----------------------------------------------\n");

        if (PtrFile == NULL)
        {
            // save to buffer
            if ((totalSize + sTempSize) > *PtrBufferLength) 
            {
                status = SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH;
            } 
            else 
            {
                sosiMemCopy ((PtrBuffer + totalSize), sTempBuffer, sTempSize);
            }

            totalSize += sTempSize;
        }
        else
        {
            // save to file
            status = sosiFileWrite (PtrFile,(PU8)sTempBuffer, sTempSize);
					
        	if (status != SCRUTINY_STATUS_SUCCESS)
        	{
            	gPtrLoggerExpanders->logiFunctionExit ("ehlFormatHealthLogs (status=0x%x)", status);
                return (status);
        	}

        }

        // move to next log entry
        PtrHealthLogs += 64;
        LogsSize = LogsSize - 64;
    }

    if (PtrFile == NULL)
    {
        if (status == SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH)
        {
            *PtrBufferLength = totalSize + SCRUTINY_LIB_EXTRA_BUFFER_SIZE;
        }
        else
        {
            *PtrBufferLength = totalSize; 
        }
    }
    

    gPtrLoggerExpanders->logiDebug ("total Size = 0x%x", totalSize);
    gPtrLoggerExpanders->logiFunctionExit ("ehlFormatHealthLogs (status=0x%x)", status);
    return (status);

}

/**
 *
 * @method  ehlUploadLogsRegion()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   PPtrBuffer     returned buffer pointer
 * 
 * @param   BufferId       Buffer ID to be uploaded
 * 
 * @param   PtrRegionSize  pointer to the returned region size 
 *
 * @return  STATUS         Indication Success or Fail
 *
 * @brief   upload health log region from the device to the
 *          allocated buffer, the caller need to release the
 *          buffer
 *
 *
 */
SCRUTINY_STATUS ehlUploadLogsRegion (
    __IN__     PTR_SCRUTINY_DEVICE       PtrDevice,
    __IN__     U8      BufferId, 
    __INOUT__  PU8*    PPtrBuffer, 
    __INOUT__  PU32    PtrRegionSize)
{

    U32 						sizeToRead = EXP_HL_SIZE_TO_READ;
    U32 						address = 0;
    U32 						sizeTotal = 0;
    PU8 						ptrHealthLogs = NULL;
    SCRUTINY_SCSI_PASSTHROUGH   scsiRequest = { 0 };
    SCRUTINY_STATUS status;

    gPtrLoggerExpanders->logiFunctionEntry ("ehlUploadLogsRegion (PtrDevice=%x, BufferId=0x%x, PPtrBuffer=%x, PtrRegionSize=%x)", 
            PtrDevice != NULL, BufferId, PPtrBuffer != NULL,  PtrRegionSize != NULL);

    status = bsdiGetRegionSize (PtrDevice, BufferId, &sizeTotal);
    if ( status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiDebug ("Failed to upload log region");
        gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegion (status=0x%x)", status);
        return (status);
    }

    *PPtrBuffer = NULL;
    *PtrRegionSize = sizeTotal;

    ptrHealthLogs = (PU8) sosiMemAlloc (sizeTotal);
	
    if (ptrHealthLogs == NULL)
    {
        gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegion (status=0x%x)", SCRUTINY_STATUS_NO_MEMORY);
        return (SCRUTINY_STATUS_NO_MEMORY);
    }

    sosiMemSet (ptrHealthLogs, 0, sizeTotal);

    scsiRequest.CdbLength = 10;
    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_READ_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_READ_MODE_DATA;
    scsiRequest.Cdb[0x02]  = BufferId;
    scsiRequest.Cdb[0x09]  = 0x00;

    while (sizeTotal)
    {
        if (sizeTotal < EXP_HL_SIZE_TO_READ)
        {
            sizeToRead = sizeTotal;
        }

        scsiRequest.Cdb[0x03]  = (U8) ((address >> 16) & 0xFF);
        scsiRequest.Cdb[0x04]  = (U8) ((address >> 8) & 0xFF);
        scsiRequest.Cdb[0x05]  = (U8) (address & 0xFF);
        scsiRequest.Cdb[0x06]  = (U8) ((sizeToRead >> 16) & 0xFF);
        scsiRequest.Cdb[0x07]  = (U8) ((sizeToRead >> 8) & 0xFF);
        scsiRequest.Cdb[0x08]  = (U8) (sizeToRead & 0xFF);

        scsiRequest.DataDirection = DIRECTION_READ;
        scsiRequest.PtrDataBuffer = (PVOID) (ptrHealthLogs + address);
        scsiRequest.DataBufferLength = sizeToRead;

        status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            sosiMemFree (ptrHealthLogs);
            gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegion (status=0x%x)", status);
            return (status);
        }

        sizeTotal = sizeTotal - sizeToRead;
        address = address + sizeToRead;
    }
    

    *PPtrBuffer = ptrHealthLogs;

    gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegion (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);
}

/**
 *
 * @method  ehlLogsRegionIsCurrent()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   RegionIndex    input region index
 * 
 * @param   PtrFlashTable  pointer to the flash table
 * 
 * @return  STATUS         SUCCESS if it's current region, otherwise, fail
 *
 * @brief   check whether the input region is the current log region
 *
 *
 */
SCRUTINY_STATUS ehlLogsRegionIsCurrent (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__  U32  RegionIndex, 
    __IN__  PTR_FLASH_TABLE PtrFlashTable)
{
    FW_HEALTH_LOG_FLASH_REGION_HEADER  	healthLogHeader;
    SCRUTINY_STATUS   					status;

    gPtrLoggerExpanders->logiFunctionEntry ("ehlLogsRegionIsCurrent (PtrDevice=%x, RegionIndex=0x%x, PtrFlashTable=%x)", 
            PtrDevice != NULL, RegionIndex, PtrFlashTable != NULL);

    status = bsdiMemoryRead32 (PtrDevice,
                                 PtrFlashTable->FlashRegionEntries[RegionIndex].RegionOffset + REGISTER_ADDRESS_COBRA_FLASH_START,
                                 (PU32) &healthLogHeader,
                                 sizeof (FW_HEALTH_LOG_FLASH_REGION_HEADER));
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiDebug ("Failed to upload log region header");
        gPtrLoggerExpanders->logiFunctionExit ("ehlLogsRegionIsCurrent (status=0x%x)", status);
        return (SCRUTINY_STATUS_FAILED);
    }

    if ((healthLogHeader.Signature != FW_HEALTH_LOG_FRH_SIGNATURE) || 
        (healthLogHeader.Flags.u.Bits.CurrentRegion != 1))
    {
        gPtrLoggerExpanders->logiDebug ("Header signature is not correct or is not current");
        gPtrLoggerExpanders->logiFunctionExit ("ehlLogsRegionIsCurrent (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    gPtrLoggerExpanders->logiDebug ("Signature 0x%x, CurrentRegion 0x%x", healthLogHeader.Signature,
                                    healthLogHeader.Flags.u.Bits.CurrentRegion);

    gPtrLoggerExpanders->logiFunctionExit ("ehlLogsRegionIsCurrent (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);
}

/**
 *
 * @method  ehlUploadLogsRegionMemoryRead()
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   PPtrBuffer     pointer to output buffer 
 * 
 * @param   PtrRegionSize  pointer to size of region
 * 
 * @return  STATUS         Indication Success or Fail
 *
 * @brief   upload the log region by memory read, used for SDB interface
 *
 *
 */
SCRUTINY_STATUS ehlUploadLogsRegionMemoryRead (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __OUT__ PU8 *PPtrBuffer, 
    __OUT__ PU32 PtrRegionSize)
{

    PTR_FLASH_TABLE 		ptrFlashTable;
    U32 					index;
    PU8 					ptrLogs;
    U32 					tempIndex = 0;
    U32 					tempSize = 0;
    PTR_ATLAS_FLASH_TABLE  	ptrAtlasFlashTable;

    gPtrLoggerExpanders->logiFunctionEntry ("ehlUploadLogsRegionMemoryRead (PtrDevice=%x, PPtrBuffer=%x, PtrRegionSize=%x)", 
                                            PtrDevice != NULL, PPtrBuffer != NULL, PtrRegionSize != NULL);
    gPtrLoggerExpanders->logiDebug ("Device ProductFamily %x", PtrDevice->ProductFamily);
    /*
     * First we have to read the flash table
     */

    ptrFlashTable = NULL;

    if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER) 
    {
        if (cobRegisterBasedUploadBootloaderFlashTable (PtrDevice, &ptrFlashTable))
        {
            gPtrLoggerExpanders->logiDebug ("Failed to upload Flash table");
            gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegionMemoryRead (status=0x%x)", SCRUTINY_STATUS_FAILED);
            return (SCRUTINY_STATUS_FAILED);
        }
    } 

	else if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        if (atlasGetFlashTable (PtrDevice, &ptrAtlasFlashTable))
        {
            gPtrLoggerExpanders->logiDebug ("Failed to upload Flash table");
            gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegionMemoryRead (status=0x%x)", SCRUTINY_STATUS_FAILED);
            return (SCRUTINY_STATUS_FAILED);
        }

        /* except the number of talbe entry, Flash Table structure for Expander and Atlas should be same, cast it */
        ptrFlashTable = (PTR_FLASH_TABLE) ptrAtlasFlashTable;
    }
    
    /* enumerate and find the region offset */

    for (index = 0; index < ptrFlashTable->NumFlashTableEntries; index++)
    {
        if (ptrFlashTable->FlashRegionEntries[index].RegionType == HALI_FLASH_LOG_1 || 
            ptrFlashTable->FlashRegionEntries[index].RegionType == HALI_FLASH_LOG_2)
        {
            if (ehlLogsRegionIsCurrent (PtrDevice, index, ptrFlashTable) == SCRUTINY_STATUS_SUCCESS)
            {
                break;
            }
        }
    }

    if (index >= ptrFlashTable->NumFlashTableEntries)
    {
		sosiMemFree ((void*) ptrFlashTable);
		gPtrLoggerExpanders->logiDebug ("NO LOG Region Found");
        gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegionMemoryRead (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }


    ptrLogs = (PU8) sosiMemAlloc (ptrFlashTable->FlashRegionEntries[index].RegionSize);

    if (ptrLogs == NULL) 
    {
        sosiMemFree ((void*) ptrFlashTable);
        gPtrLoggerExpanders->logiDebug ("Failed to alloc memory");
        gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegionMemoryRead (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }


    /*
     * Upload the region.
     */
 
    for (tempIndex = 0, tempSize = 1024; tempIndex < ptrFlashTable->FlashRegionEntries[index].RegionSize; tempIndex += 1024)
    {

        /* We will upload 1024 bytes in one cycle and rotate the char */
        if ((tempIndex + 1024) > ptrFlashTable->FlashRegionEntries[index].RegionSize)
        {
            tempSize = ptrFlashTable->FlashRegionEntries[index].RegionSize - tempIndex;
        }

        if (bsdiMemoryRead32 (PtrDevice,
								ptrFlashTable->FlashRegionEntries[index].RegionOffset + REGISTER_ADDRESS_COBRA_FLASH_START + tempIndex,
                               	(PU32) &ptrLogs[tempIndex],
                               	tempSize))
        {
            /* We will say that this is an invalid header. */
            sosiMemFree ((void*) ptrFlashTable);
            sosiMemFree (ptrLogs);
            gPtrLoggerExpanders->logiDebug ("Upload Failed, %x", tempIndex);
            gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegionMemoryRead (status=0x%x)", SCRUTINY_STATUS_FAILED);
            return (SCRUTINY_STATUS_FAILED);
        }


    }
    
	/* We have got the data and give the entries to buffer */
    
    *PPtrBuffer = ptrLogs;
    *PtrRegionSize = ptrFlashTable->FlashRegionEntries[index].RegionSize;
    
    sosiMemFree ((void*) ptrFlashTable);

    gPtrLoggerExpanders->logiFunctionExit ("ehlUploadLogsRegionMemoryRead (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 * @method  ehliGetHealthLogs()
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   PtrBuffer      pointer to output buffer 
 * 
 * @param   PtrBufferLength  pointer to size of input buffer
 *
 * @param   Flags          raw or decoded log to be retrieved
 * 
 * @param   PtrFolderName  folder name to save the log file
 * 
 * @return  STATUS         Indication Success or Fail
 *
 * @brief   API for the health log
 *
 *
 */
SCRUTINY_STATUS ehliGetHealthLogs (
    __IN__  PTR_SCRUTINY_DEVICE       PtrDevice,
    __OUT__ PVOID                     PtrBuffer,
    __IN__  PU32                      PtrBufferLength,
    __IN__  EXP_HEALTH_LOG_TYPE       Flags,
    __IN__ const char*                PtrFolderName
    )
{
    PU8                    ptrHealthLogsBuffer;
    U32                    bufferSize;
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_FAILED;
    SOSI_FILE_HANDLE       ptrFile = NULL;
    char                   logfileName[512];
    char                   tempName[128];

    gPtrLoggerExpanders->logiFunctionEntry ("ehliGetHealthLogs (PtrDevice=%x, PtrBuffer=%x, PtrBufferLength=%x, Flags=0x%x, PtrFolderName=%x)", 
                                            PtrBuffer != NULL, PtrBufferLength != NULL, PtrBufferLength != NULL, Flags, PtrFolderName != NULL);


    if (PtrFolderName != NULL)
    {
        // saving to file
        sosiMemSet (logfileName, '\0', sizeof (logfileName));
        sosiMemSet (tempName, '\0', sizeof (tempName));
        
        status = sosiMkDir (PtrFolderName);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs() - status 0x%x", status);
            return (status);
        }

        sosiStringCopy (logfileName, PtrFolderName);

        if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
        {
            sosiSprintf (tempName, sizeof (tempName), "healthlog.txt");
        }
        else
        {
            sosiSprintf (tempName, sizeof (tempName), "healthlog.txt");
        }

        sosiStringCat (logfileName, tempName);
        ptrFile = sosiFileOpen (logfileName, "wb");

        if (!ptrFile)
		{	
			gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs() - status 0x%x", SCRUTINY_STATUS_FILE_OPEN_FAILED);
			return (SCRUTINY_STATUS_FILE_OPEN_FAILED);	
		}	
    }
    else
    {
        // copying to buffer
        if (PtrBuffer == NULL || PtrBufferLength == NULL)
        {
            gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs() - status 0x%x", SCRUTINY_STATUS_INVALID_PARAMETER);
            return (SCRUTINY_STATUS_INVALID_PARAMETER);
        }
    }

                                            
    if (EXP_HEALTH_LOGS_RAW == Flags)
    {
        if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB) 
        {
            status = ehlUploadLogsRegionMemoryRead (PtrDevice, &ptrHealthLogsBuffer, &bufferSize);
        }
        else 
        {
            status = ehlUploadLogsRegion (PtrDevice, BRCM_SCSI_BUFFER_ID_ACTIVE_LOGS, &ptrHealthLogsBuffer, &bufferSize);
        }

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            
            if (ptrFile != NULL)
            {
                sosiFileClose (ptrFile); 
            }
            gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs (status=0x%x)", status);
            return (status);
        }

        status = ehlFormatHealthLogs (ptrHealthLogsBuffer, bufferSize, PtrBuffer, PtrBufferLength, ptrFile);

        sosiMemFree (ptrHealthLogsBuffer);
    }
	
    else if (EXP_HEALTH_LOGS_DECODED == Flags)
    {
        if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB) 
        {
            
            if (ptrFile != NULL)
            {
                sosiFileClose (ptrFile); 
            }
            gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs (status=0x%x)", SCRUTINY_STATUS_UNSUPPORTED);
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }
        
        status = ehlUploadLogsRegion (PtrDevice, BRCM_SCSI_BUFFER_ID_ACTIVE_LOGS_DECODED, &ptrHealthLogsBuffer, &bufferSize);

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            
            if (ptrFile != NULL)
            {
                sosiFileClose (ptrFile); 
            }
            gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs (status=0x%x)", status);
            return (status);
        }

        status = ehlFormatDecodedHealthLogs(ptrHealthLogsBuffer, bufferSize, PtrBuffer, PtrBufferLength, ptrFile);
    
        sosiMemFree (ptrHealthLogsBuffer);
    }
	
    else if (EXP_HEALTH_LOGS_RAW_MEMREAD == Flags)
    {       
        status = ehlUploadLogsRegionMemoryRead (PtrDevice, &ptrHealthLogsBuffer, &bufferSize);
		
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            
            if (ptrFile != NULL)
            {
                sosiFileClose (ptrFile); 
            }
            gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs (status=0x%x)", status);
            return (status);
        } 

        status = ehlFormatHealthLogs (ptrHealthLogsBuffer + sizeof (FW_HEALTH_LOG_FLASH_REGION_HEADER), bufferSize, 
                                    PtrBuffer, PtrBufferLength, ptrFile);

        sosiMemFree (ptrHealthLogsBuffer);
        
    }

    if (ptrFile != NULL)
    {
        sosiFileClose (ptrFile); 
    }

    gPtrLoggerExpanders->logiFunctionExit ("ehliGetHealthLogs (status=0x%x)", status);
    return (status);
}

