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
#include "switchhealthmon.h"


ITEMS_LOOKUP_TABLE  gSwhGlobalMonitorItemsTable[] =
{
    { ITEM_NAME (SWH_TEMPERATURE), SWH_TEMPERATURE },
    { ITEM_NAME (SWH_ENCL_STATUS), SWH_ENCL_STATUS },
    { ITEM_NAME (SWH_SASADDR_LOW), SWH_SASADDR_LOW },
    { ITEM_NAME (SWH_SASADDR_HIGH), SWH_SASADDR_HIGH },
    { ITEM_NAME (SWH_FW_VER), SWH_FW_VER },
    { ITEM_NAME (SWH_TOTAL_PORTS), SWH_TOTAL_PORTS },
    { ITEM_NAME (SWH_TOTAL_HOST_PORTS), SWH_TOTAL_HOST_PORTS },
};

ITEMS_LOOKUP_TABLE  gSwhPortMonitorItemsTable[] = 
{
    { ITEM_NAME (SWH_LINK_UP), SWH_LINK_UP },
    { ITEM_NAME (SWH_MAX_LINK_RATE), SWH_MAX_LINK_RATE },
    { ITEM_NAME (SWH_NEGOTIATED_LINK_RATE), SWH_NEGOTIATED_LINK_RATE },
    { ITEM_NAME (SWH_MAX_LINK_WIDTH), SWH_MAX_LINK_WIDTH },
    { ITEM_NAME (SWH_NEGOTIATED_LINK_WIDTH), SWH_NEGOTIATED_LINK_WIDTH },
    { ITEM_NAME (SWH_PORTRECEIVERERRORS), SWH_PORTRECEIVERERRORS },
    { ITEM_NAME (SWH_BADTLPERRORS), SWH_BADTLPERRORS },
    { ITEM_NAME (SWH_BADDLLPERRORS), SWH_BADDLLPERRORS },    
    { ITEM_NAME (SWH_RECOVERYDIAGNOSTICSERRORS), SWH_RECOVERYDIAGNOSTICSERRORS },
    { ITEM_NAME (SWH_LINKDOWNCOUNT), SWH_LINKDOWNCOUNT },
    { ITEM_NAME (SWH_PORT_TYPE), SWH_PORT_TYPE },
        
};

static SWH_HEALTH_MONITOR_DATA      gSwhHealthConfigData;









/**
 *
 * @method  shmGetPortLinkStatus ()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PtrSwHealthInfo     returned switch health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   Fill link status of specific port
 *
 *
 */

SCRUTINY_STATUS shmFillPortLinkStatus (
    __IN__   PTR_SCRUTINY_DEVICE             PtrDevice, 
    __IN__   U32                             PortNum,
    __OUT__  PTR_SCRUTINY_SWITCH_HEALTH      PtrSwHealthInfo
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("shmFillPortLinkStatus (PtrDevice=%x, PortNum=0x%x, PtrSwHealthInfo=%x)", 
            PtrDevice != NULL, PortNum, PtrSwHealthInfo != NULL);
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("Failed to read Configuration Space register 0x%x", ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS);
        gPtrLoggerSwitch->logiFunctionExit ("shmFillPortLinkStatus (status=0x%x)", status);
        return (status);
    }

    PtrSwHealthInfo->PortStatus[PortNum].NegotiatedLinkWidth = (PCI_DEVICE_LINK_WIDTH)((dword & 0x3F00000) >> 20);
    PtrSwHealthInfo->PortStatus[PortNum].NegotiatedLinkSpeed = (PCI_DEVICE_LINK_SPEED)((dword & 0xF0000) >> 16);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_CAP, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("Failed to read Configuration Space register 0x%x", ATLAS_REGISTER_PMG_REG_PORT_LINK_CAP);
        gPtrLoggerSwitch->logiFunctionExit ("shmFillPortLinkStatus (status=0x%x)", status);
        return (status);
    }

    PtrSwHealthInfo->PortStatus[PortNum].MaxLinkWidth = (PCI_DEVICE_LINK_WIDTH)((dword & 0x3F0) >> 4); 
    PtrSwHealthInfo->PortStatus[PortNum].MaxLinkSpeed = (PCI_DEVICE_LINK_SPEED)(dword & 0xF);
    

    gPtrLoggerSwitch->logiFunctionExit ("shmFillPortLinkStatus (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  shmiGetHealthData()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PtrSwHealthInfo     returned switch health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   request the switch healthe data
 *
 *
 */
SCRUTINY_STATUS shmiGetHealthData (
    __IN__   PTR_SCRUTINY_DEVICE             PtrDevice, 
    __OUT__  PTR_SCRUTINY_SWITCH_HEALTH      PtrSwHealthInfo
)
{
    
    SCRUTINY_STATUS     status = SCRUTINY_STATUS_SUCCESS;
    U8                  index;
    
    gPtrLoggerSwitch->logiFunctionEntry ("shmiGetHealthData (PtrDevice=%x, PtrSwHealthInfo=%x)", 
            PtrDevice != NULL, PtrSwHealthInfo != NULL);

    if (PtrSwHealthInfo == NULL)
    {
        gPtrLoggerSwitch->logiFunctionExit ("shmiGetHealthData (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    sosiMemSet (PtrSwHealthInfo, 0, sizeof (SCRUTINY_SWITCH_HEALTH));

    status = bsdScsiGetTemperatureValue (PtrDevice, &PtrSwHealthInfo->Temperature);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("failed to get switch temperature: %x", status);
        gPtrLoggerSwitch->logiFunctionExit ("shmiGetHealthData (status=0x%x)", status);
        return (status);
    }

    status = ehmiGetExpEnclosureState (PtrDevice, &PtrSwHealthInfo->EnclosureStatus);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("failed to get switch enclosure state: %x", status);
        gPtrLoggerSwitch->logiFunctionExit ("shmiGetHealthData (status=0x%x)", status);
        return (status);
    }

    for (index = 0; index < ATLAS_PMG_MAX_PHYS; index++)
    {
        if (!spcIsPortEnabled (PtrDevice, index))
        {
            /* the port is disable, skip it */
            PtrSwHealthInfo->PortStatus[index].isEnabled = FALSE;
            continue; 
        }
	
	PtrSwHealthInfo->PortStatus[index].isEnabled = TRUE;

        status = shmFillPortLinkStatus (PtrDevice, index, PtrSwHealthInfo);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiDebug ("failed to get switch Port %d Max Link Width: %x", index, status);
            gPtrLoggerSwitch->logiFunctionExit ("shmiGetHealthData (status=0x%x)", status);
            return (status);
        }

        status = spcGetAdvancedErrorStatus (PtrDevice, index, &PtrSwHealthInfo->PortStatus[index].AerState);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiDebug ("failed to get switch Port %d AER status: %x", index, status);
            gPtrLoggerSwitch->logiFunctionExit ("shmiGetHealthData (status=0x%x)", status);
            return (status);
        }

        status = spcGetErrorCounters (PtrDevice, index, &PtrSwHealthInfo->PortStatus[index].ErrorCounter);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiDebug ("failed to get switch Port %d Error Counters: %x", index, status);
            gPtrLoggerSwitch->logiFunctionExit ("shmiGetHealthData (status=0x%x)", status);
            return (status);
        }
            
    }
    
    gPtrLoggerSwitch->logiFunctionExit ("shmiGetHealthData (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);
    
}


/**
 *
 * @method  shmiDumpGlobalSegment()
 *
 *
 * @param   FileHandle           pointer to the file that will be dumped
 *
 * @param   PtrSwhHealthData     point to input switch health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   dump the switch health data global segment
 *
 *
 */


SCRUTINY_STATUS shmiDumpGlobalSegment (
    __IN__ SOSI_FILE_HANDLE  FileHandle,
    __IN__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthData
)
{
    U32 index, bufSize;
    char  tempBuffer[512];
    SCRUTINY_STATUS status =SCRUTINY_STATUS_SUCCESS;
     
    gPtrLoggerSwitch->logiFunctionEntry ("shmiDumpGlobalSegment (PtrSwhHealthData=%x)", PtrSwhHealthData != NULL);

    bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), "[%s]\n", SCRUTINY_CONFIG_SEG_NAME_SWH);
    sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);
    
    for (index = 0; index < SWH_GLOBAL_MAX_ITEMS; index ++)
    {
       bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s = 0x%X\n", gSwhGlobalMonitorItemsTable[index].ItemName,
           PtrSwhHealthData->SwhMonitorItems[index]);

       sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);
    }

    sosiFileWrite (FileHandle, (U8 *)"\n\n", 2);

    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("shmiDumpGlobalSegment  (Status = %x) ",status);
    return (status);

}

/**
 *
 * @method  shmiDumpPortSegment()
 *
 *
 * @param   FileHandle           pointer to the file that will be dumped
 *
 * @param   PtrSwhHealthData     point to input switch health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   dump the switch health data port segment
 *
 *
 */


SCRUTINY_STATUS shmiDumpPortSegment (
    __IN__  SOSI_FILE_HANDLE  FileHandle,
    __IN__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthData
)
{
     U32 index, bufSize, portIndex;
     char  tempBuffer[512];
     U32 startPort, endPort;

    SCRUTINY_STATUS status =SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerSwitch->logiFunctionEntry ("shmiDumpPortSegment (PtrSwhHealthData=%x)", PtrSwhHealthData != NULL);
    
     startPort = 0;

     for (portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; )
     {
        //if valid bit == 0; just skip it.
        if (PtrSwhHealthData->PortMonData[portIndex].Valid == 0)
        {
            portIndex++;
            continue;
        }
        index = portIndex + 1;
        do 
        {   
            if (index < ATLAS_PMG_MAX_PHYS)
            {
                if (SCRUTINY_STATUS_SUCCESS == sosiMemCompare ((U8 *)&(PtrSwhHealthData->PortMonData[portIndex]), 
                    (U8 *)&(PtrSwhHealthData->PortMonData[index]), sizeof (SWH_HEALTH_MONITOR_PORT_DATA)))
                {
                    index++;
                }
                else 
                {
                    break;
                }
            }
            else
            {
                break;
            }
        } while (1);

        startPort = portIndex; 
        endPort  = index - 1;

        bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), "[%s %d - %d]\n", SCRUTINY_CONFIG_SEG_NAME_PORT,
            startPort, endPort);
        sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);

        for (index = 0; index < SWH_PORT_MAX_ITEMS; index++)
        {
            bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s = 0x%X\n", gSwhPortMonitorItemsTable[index].ItemName,
                PtrSwhHealthData->PortMonData[startPort].PortMonitorItems[index]);

            sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);
        }

        portIndex = endPort + 1;

        sosiFileWrite (FileHandle, (U8 *)"\n\n", 2);
     }

    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("shmiDumpPortSegment  (Status = %x) ",status);
    return (status);

}


/**
 *
 * @method  shmiDumpHealthConfigurationsToFiles()
 *
 *
 * @param   PtrFolderName        pointer to the fold name where dump file  will be 
 *
 * @param   PtrSwhHealthData     point to input switch health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   dump the switch health data to file
 *
 *
 */


SCRUTINY_STATUS shmiDumpHealthConfigurationsToFiles(  
    __IN__ const char*                PtrFolderName,
    __IN__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthData
)
{
    char                   inifileName[512];
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
    SOSI_FILE_HANDLE       fileHandle;

    //gPtrLoggerController->logiFunctionEntry ("chmInitializeHealthConfigurations (PtrFolderName=%x)", PtrFolderName != NULL);    
    gPtrLoggerSwitch->logiFunctionEntry ("shmiDumpHealthConfigurationsToFiles (PtrFolderName=%x)", PtrFolderName != NULL);
    
    if (PtrFolderName == NULL)
    {
        //gPtrLoggerController->logiFunctionExit ("chmInitializeHealthConfigurations (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    sosiMemSet (inifileName, '\0', sizeof (inifileName));

    status = sosiMkDir (PtrFolderName);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        //gPtrLoggerController->logiFunctionExit ("chmInitializeHealthConfigurations(status=0x%x)", status);
        return (status);
    }

    
    sosiStringCopy (inifileName, PtrFolderName);
    sosiStringCat (inifileName, SWH_HEALTH_CONFIG_LOG);
    
    fileHandle = sosiFileOpen (inifileName, "ab");

    shmiDumpGlobalSegment (fileHandle, PtrSwhHealthData);
    shmiDumpPortSegment (fileHandle, PtrSwhHealthData);
    
    sosiFileClose (fileHandle); 

    gPtrLoggerSwitch->logiFunctionExit ("shmiDumpHealthConfigurationsToFiles  (Status = %x) ",status);
    return (status);
}


/**
 *
 * @method  shmiEnumerateSwhGlobalSegment()
 *
 *
 * @param   PtrDictionary        pointer to the dictionary that holding the decoded ini data  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the switch health data
 *
 *
 */



SCRUTINY_STATUS shmiEnumerateSwhGlobalSegment (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{
    U32 index, tableIndex;
    PTR_CONFIG_INI_ENTRIES   ptrEntry;
    SCRUTINY_STATUS    status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerSwitch->logiFunctionEntry ("shmiEnumerateSwhGlobalSegment (PtrDictionary=%x)", PtrDictionary != NULL);


    if ((sizeof (gSwhGlobalMonitorItemsTable) / sizeof (ITEMS_LOOKUP_TABLE)) != SWH_GLOBAL_MAX_ITEMS)
    {
        /* Incompitible table and items */
        return SCRUTINY_STATUS_FAILED;
    }
    
    for (index = 0; index < PtrDictionary->TotalEntries; index++)
    {
        ptrEntry = &PtrDictionary->PtrIniEntries[index];

        for (tableIndex = 0; tableIndex < SWH_GLOBAL_MAX_ITEMS; tableIndex++)
        {
            if ((sosiStringCompare (ptrEntry->Key, gSwhGlobalMonitorItemsTable[tableIndex].ItemName)) == SCRUTINY_STATUS_SUCCESS)
            {
                gSwhHealthConfigData.SwhMonitorItems[gSwhGlobalMonitorItemsTable[tableIndex].ItemIndex] = hexadecimalToDecimal (ptrEntry->Value);
                break;
            }
        }
        
        if (tableIndex == 0xE) 
        {
            return (SCRUTINY_STATUS_FAILED);
        }
    }
    
    gPtrLoggerSwitch->logiFunctionExit ("shmiEnumerateSwhGlobalSegment  (Status = %x) ",status);
    return (status);
    
}

/**
 *
 * @method  shmiEnumeratePortSegment()
 *
 *
 * @param   PtrDictionary        pointer to the dictionary that holding the decoded ini data  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the switch health data
 *
 *
 */


SCRUTINY_STATUS shmiEnumeratePortSegment (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{
    PTR_CONFIG_INI_ENTRIES   ptrEntry;
    U32     startPort, endPort;
    U32     index = 0, tableIndex = 0;
    PU8     ptrChar;
    U8      temp[32];
    SWH_HEALTH_MONITOR_PORT_DATA     portSettings;
    U32     onlyOnePort = 0;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;
    
    gPtrLoggerSwitch->logiFunctionEntry ("shmiEnumeratePortSegment (PtrDictionary=%x)", PtrDictionary != NULL);


    if ((sizeof (gSwhPortMonitorItemsTable) / sizeof (ITEMS_LOOKUP_TABLE)) != SWH_PORT_MAX_ITEMS)
    {
        /* Incompitible table and items */
        return SCRUTINY_STATUS_FAILED;
    }

    sosiMemSet (&portSettings, 0, sizeof (portSettings));
    for (index = 0; index < INI_PARSER_MAXIMUM_CHAR_LENGTH; index++)
    {
        if (PtrDictionary->SegmentName[index] == '-') 
        {
            onlyOnePort = 0;
            break;
        }
        if (PtrDictionary->SegmentName[index] == 0)
        {
            onlyOnePort = 1;
            break;
        } 
    }
    
    /* firstly, get the range of phys */
    ptrChar = (PU8) PtrDictionary->SegmentName;
    
    /* skip 'PORT' */
    ptrChar += 4;  
    for (; ((*ptrChar != 0) && (sosiIsSpace(*ptrChar))); ptrChar++);

    sosiMemSet (temp, 0, sizeof (temp));
    for (index = 0; ((*ptrChar != 0) && (*ptrChar != '-') && (index < sizeof (temp))); ptrChar++, index++)
    {
        temp[index] = *ptrChar;
    }
    sosiStringTrim ((char *)temp);
    startPort = sosiAtoi ((const char *)temp);
    ptrChar++;

    if (onlyOnePort)
    {
        endPort = startPort;
    }
    else
    {

        sosiMemSet (temp, 0, sizeof (temp));
        for (index = 0; ((*ptrChar != 0) && (index < sizeof (temp))); ptrChar++, index++)
        {
            temp[index] = *ptrChar;
        }
        sosiStringTrim ((char *)temp);
        endPort = sosiAtoi ((const char *)temp);

    }
    if (startPort > endPort)
    {
        /* something bad in the config file, just return */
        return (SCRUTINY_STATUS_FAILED);
    }

    for (index = 0; index < PtrDictionary->TotalEntries; index++)
    {
        ptrEntry = &PtrDictionary->PtrIniEntries[index];

        for (tableIndex = 0; tableIndex < SWH_PORT_MAX_ITEMS; tableIndex++)
        {
            if ((sosiStringCompare (ptrEntry->Key, gSwhPortMonitorItemsTable[tableIndex].ItemName)) == SCRUTINY_STATUS_SUCCESS)
            {
                portSettings.PortMonitorItems[gSwhPortMonitorItemsTable[tableIndex].ItemIndex] = hexadecimalToDecimal (ptrEntry->Value);
                break;
            }
        }

        if (tableIndex == 0xD)
        {
            return (SCRUTINY_STATUS_FAILED);
        }
        
    }
    
    portSettings.Valid = 1;
    
    for (index = startPort; index <= endPort; index++)
    {
        sosiMemCopy ( (U8*)&gSwhHealthConfigData.PortMonData[index], (U8*)&portSettings, sizeof (SWH_HEALTH_MONITOR_PORT_DATA));
    }


    gPtrLoggerSwitch->logiFunctionExit ("shmiEnumeratePortSegment  (Status = %x) ",status);
    return (status);
     
}


 /**
 *
 * @method  shmiEnumerateConfigDictionaries()
 *
 *
 * @param   PtrDictionary        pointer to the dictionary that holding the decoded ini data  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the switch health data
 *
 *
 */


SCRUTINY_STATUS shmiEnumerateConfigDictionaries (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    PTR_CONFIG_INI_DICTIONARY  ptrTemp = NULL;
    SCRUTINY_STATUS    status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerSwitch->logiFunctionEntry ("shmiEnumerateConfigDictionaries (PtrDictionary=%x)", PtrDictionary != NULL);
  
    ptrTemp = PtrDictionary;

    while (ptrTemp)
    {

        if (!ptrTemp->PtrIniEntries)
        {
            /* We don't have any entries. Thus get next list */
            ptrTemp = ptrTemp->PtrNext;
            continue;
        }

        if ((sosiStringCompare (ptrTemp->SegmentName, SCRUTINY_CONFIG_SEG_NAME_SWH)) == SCRUTINY_STATUS_SUCCESS)
        {
            status = shmiEnumerateSwhGlobalSegment (ptrTemp);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                break;
            }
        }
        else if ((sosiMemCompare ( (U8*)ptrTemp->SegmentName, (U8*)"PORT", 4)) == SCRUTINY_STATUS_SUCCESS)
        {
            // do phy section parse
            status = shmiEnumeratePortSegment (ptrTemp);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                break;
            }
        }
        
        ptrTemp = ptrTemp->PtrNext;

    }
    gPtrLoggerSwitch->logiFunctionExit ("shmiEnumerateConfigDictionaries  (Status = %x) ",status);
    return (status);

}


 /**
 *
 * @method  shmiInitializeHealthConfigurations()
 *
 *
 * @param   PtrFolderName        pointer to the folder name that holding the health monitor ini file  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the switch health data
 *
 *
 */


SCRUTINY_STATUS shmiInitializeHealthConfigurations( __IN__ const char* PtrFolderName)
{
    char                   logfileName[512];
    char                   tempName[128];
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
    PTR_CONFIG_INI_DICTIONARY ptrConfigDictionary = NULL;

    //gPtrLoggerController->logiFunctionEntry ("chmInitializeHealthConfigurations (PtrFolderName=%x)", PtrFolderName != NULL);    
    gPtrLoggerSwitch->logiFunctionEntry ("shmiInitializeHealthConfigurations (PtrFolderName=%x)", PtrFolderName != NULL);



    if (PtrFolderName == NULL)
    {
        //gPtrLoggerController->logiFunctionExit ("chmInitializeHealthConfigurations (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    sosiMemSet (logfileName, '\0', sizeof (logfileName));
    sosiMemSet (tempName, '\0', sizeof (tempName));
    sosiMemSet (&gSwhHealthConfigData, 0, sizeof (SWH_HEALTH_MONITOR_DATA));
    
    status = sosiMkDir (PtrFolderName);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        //gPtrLoggerController->logiFunctionExit ("chmInitializeHealthConfigurations(status=0x%x)", status);
        return (status);
    }

    sosiStringCopy (logfileName, PtrFolderName);

    sosiStringCat (logfileName, SWH_HEALTH_CONFIG_INI);
    
    if (lcpiParserProcessINIFile (logfileName, &ptrConfigDictionary))
    {
        /* We don't have to fail for this as of now. */
        //gPtrLoggerController->logiFunctionExit ("chmInitializeHealthConfigurations(status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Check if we have any valid entries */
    if (!lcpiHasAnyValidEntries (ptrConfigDictionary))
    {
        //gPtrLoggerController->logiFunctionExit ("chmInitializeHealthConfigurations(status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
 
    if (shmiEnumerateConfigDictionaries (ptrConfigDictionary))
    {
        status = SCRUTINY_STATUS_FAILED;
    }
        
    lcpiParserDestroyDictionary(ptrConfigDictionary);

    gPtrLoggerSwitch->logiFunctionExit ("shmiInitializeHealthConfigurations  (Status = %x) ",status);
    return (status);
}

/**
 *
 *  @method  shmiSwitchHealthCheck ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrConfigFilePath       Pointer to the string which contain the config ini file path.
 *
 *  @param   DumpToFile              BOOLEAN variable ,when=TRUE mean the related read back status valve dump to a file.
 *
 *  @param   PtrErrorCode            Pointer to U32 variable, each bit conrespond to a error condition.
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 *  @brief                           This method do the switch health monitor check according to the config ini file content. 
 *
 */


SCRUTINY_STATUS  shmiSwitchHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32 PtrErrorCode)
{
    SCRUTINY_STATUS                         status = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_HEALTH              ptrSwhHealthInfo;
    PTR_SCRUTINY_SWITCH_PORT_PROPERTIES     ptrPciePortProperties;
    SWH_HEALTH_MONITOR_DATA                 swhHealthConfigDataReadback;
    U32                                     index;
    SCRUTINY_SWITCH_HEALTH_ERROR_INFO       errorInfo;

    gPtrLoggerSwitch->logiFunctionEntry ("shmiSwitchHealthCheck (PtrDevice=%x, PtrConfigFilePath=%x, DumpToFile=%x,PtrErrorCode=%x)", PtrDevice != NULL, PtrConfigFilePath != NULL, DumpToFile, PtrErrorCode!=NULL);

    
    sosiMemSet (&errorInfo, 0, sizeof(SCRUTINY_SWITCH_HEALTH_ERROR_INFO));
    if (PtrConfigFilePath == NULL)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("shmiSwitchHealthCheck  (Status = %x) ",status);
        return (status);

    }
    
    status = shmiInitializeHealthConfigurations (PtrConfigFilePath);
    if (status)
    {
        gPtrLoggerSwitch->logiFunctionExit ("shmiSwitchHealthCheck  (Status = %x) ",status);
        return (status);

    }

//    status = shmiDumpHealthConfigurationsToFiles (PtrConfigFilePath, &gSwhHealthConfigData);
//    if (status)
//    {
//        gPtrLoggerSwitch->logiFunctionExit ("shmiSwitchHealthCheck  (Status = %x) ",status);
//        return (status);
//
//    }


    ptrSwhHealthInfo = (PTR_SCRUTINY_SWITCH_HEALTH) sosiMemAlloc (sizeof(SCRUTINY_SWITCH_HEALTH));
    if (ptrSwhHealthInfo == NULL)
    {
        status = SCRUTINY_STATUS_NO_MEMORY;		
        gPtrLoggerSwitch->logiFunctionExit ("shmiSwitchHealthCheck  (Status = %x) ",status);
        return (status);

    }
    sosiMemSet (ptrSwhHealthInfo, 0, sizeof(SCRUTINY_SWITCH_HEALTH));
     
    ptrPciePortProperties = (PTR_SCRUTINY_SWITCH_PORT_PROPERTIES) sosiMemAlloc (sizeof(SCRUTINY_SWITCH_PORT_PROPERTIES));
    if (ptrPciePortProperties == NULL)
    {
        sosiMemFree (ptrSwhHealthInfo);
        status = SCRUTINY_STATUS_NO_MEMORY;        		
        gPtrLoggerSwitch->logiFunctionExit ("shmiSwitchHealthCheck  (Status = %x) ",status);
        return (status);

    }
    sosiMemSet (ptrPciePortProperties, 0, sizeof(SCRUTINY_SWITCH_PORT_PROPERTIES));
    
    status = shmiGetHealthData (PtrDevice, ptrSwhHealthInfo);
    if (status)
    {
        sosiMemFree (ptrPciePortProperties);
        sosiMemFree (ptrSwhHealthInfo);        
        gPtrLoggerExpanders->logiFunctionExit ("shmiSwitchHealthCheck  (Status = %x) ",status);
        return (status);

    }
    
    status = sppGetPciePortProperties ( PtrDevice, ptrPciePortProperties);
    if (status)
    {
        sosiMemFree (ptrPciePortProperties);
        sosiMemFree (ptrSwhHealthInfo);
        gPtrLoggerExpanders->logiFunctionExit ("shmiSwitchHealthCheck  (Status = %x) ",status);
        return (status);

    }
    
    //below function always success.
    for (index=0; index < ATLAS_PMG_MAX_PHYS; index++)
    {
        swhHealthConfigDataReadback.PortMonData[index].Valid = gSwhHealthConfigData.PortMonData[index].Valid;
    }
    status = shmiFillSwhHealthConfigDataReadBackData (PtrDevice,  ptrSwhHealthInfo, ptrPciePortProperties, &swhHealthConfigDataReadback);
    if (DumpToFile)
    {
        status = shmiDumpHealthConfigurationsToFiles (PtrConfigFilePath, &swhHealthConfigDataReadback);
    }
    status = shmiCompareHealthData (&gSwhHealthConfigData, &swhHealthConfigDataReadback, &errorInfo);
    //Currently we only return one U32 data to upper lay ,because C++ wrapper can not handle structure very well.
    //In the future ,if there are more than 32 errors return, we just need define more U32 variable, and no need change internal data structure. 
    *PtrErrorCode = errorInfo.ErrorDwords[0];
    
    sosiMemFree (ptrPciePortProperties);
    sosiMemFree (ptrSwhHealthInfo);

    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("shmiSwitchHealthCheck (status=0x%x)", status);    
    return (status);
}

/**
 *
 *  @method  shmiFillSwhHealthConfigDataReadBackData ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrSwhHealthInfo        Pointer to switch health info.
 *
 *  @param   PtrPciePortProperties        Pointer to switch pcie port properties.
 *
 *  @param   PtrSwhHealthConfigDataReadBack            Pointer to read back configuration data.
 *

    @return  SCRUTINY_STATUS    Indication Success or Fail  
 *
 *  @brief                           This method fill the read back configuration data. 
 *
 */


SCRUTINY_STATUS shmiFillSwhHealthConfigDataReadBackData (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SWITCH_HEALTH PtrSwhHealthInfo, __IN__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties, __OUT__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthConfigDataReadBack)
{
    U32 portNumber, tableIndex, portConfigEntryIndex;
    SCRUTINY_STATUS  status;

    gPtrLoggerSwitch->logiFunctionEntry ("shmiFillSwhHealthConfigDataReadBackData (PtrDevice=%x, PtrSwhHealthInfo=%x, PtrPciePortProperties=%x,PtrSwhHealthConfigDataReadBack=%x)", PtrDevice != NULL, PtrSwhHealthInfo != NULL, PtrPciePortProperties != NULL, PtrSwhHealthConfigDataReadBack!=NULL);
    


    // fill switch global item data
    for (tableIndex = 0; tableIndex < SWH_GLOBAL_MAX_ITEMS; tableIndex++)
    {
        
        switch (tableIndex)
        {
            case SWH_TEMPERATURE:
            {
                PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_TEMPERATURE] = PtrSwhHealthInfo->Temperature;
                break; 
            }
            case SWH_ENCL_STATUS:
            {
                PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_ENCL_STATUS] = PtrSwhHealthInfo->EnclosureStatus;
                break; 

            }
            case SWH_SASADDR_LOW:
            {
                PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_SASADDR_LOW] = PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.Low;
                break;
            }
            
            case SWH_SASADDR_HIGH:
            {
                PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_SASADDR_HIGH] = PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.High; 
                break;
            }
            case SWH_FW_VER:
            {
                PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_FW_VER]      = PtrDevice->DeviceInfo.u.SwitchInfo.FWVersion;
                break;
            }
            case SWH_TOTAL_PORTS:
            {
                PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_TOTAL_PORTS]     = PtrPciePortProperties->ToTalPhyNum;
                break;
            }
            case SWH_TOTAL_HOST_PORTS:
            {   
                PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_TOTAL_HOST_PORTS]   =  PtrPciePortProperties->TotalHostPort;
                break;
            }            

        }
        
    }
    


    
    // fill switch port item data
        
    for (portConfigEntryIndex = 0; portConfigEntryIndex < PtrPciePortProperties->TotalPortConfigEntry; portConfigEntryIndex++)
    {
        portNumber = PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].PortNumber;
        if (portNumber > PtrPciePortProperties->ToTalPhyNum)
        {
            break;
        }
        
        for (tableIndex = 0; tableIndex < SWH_PORT_MAX_ITEMS; tableIndex++)
        {
        
            switch (tableIndex)
            {
                case SWH_LINK_UP:
                {
                    
                    if(PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].NegotiatedLinkWidth)
                    { 
                        PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_LINK_UP] = 1;
                    }
                    else
                    {
                        PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_LINK_UP] = 0;
                    }
                    break; 
                }
                case SWH_MAX_LINK_RATE:
                {
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_MAX_LINK_RATE] = PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].MaxLinkSpeed;
                    break; 

                }
                case SWH_NEGOTIATED_LINK_RATE:
                {
                    if(PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].NegotiatedLinkWidth)
                    {
                        PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_NEGOTIATED_LINK_RATE] = PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].NegotiatedLinkSpeed;
                    }
                    else
                    {
                        PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_NEGOTIATED_LINK_RATE] = 0;                 
                    }
                    break;
                }
            
                case SWH_MAX_LINK_WIDTH:
                {
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_MAX_LINK_WIDTH] = PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].MaxLinkWidth;
                    break;
                }
                case SWH_NEGOTIATED_LINK_WIDTH:
                {
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_NEGOTIATED_LINK_WIDTH] = PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].NegotiatedLinkWidth;
                    break;
                }
                case SWH_PORTRECEIVERERRORS:
                {
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_PORTRECEIVERERRORS]   = PtrSwhHealthInfo->PortStatus[portNumber].ErrorCounter.PortReceiverErrors;
                    break;
                }
                case SWH_BADTLPERRORS:
                {                       
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_BADTLPERRORS]   = PtrSwhHealthInfo->PortStatus[portNumber].ErrorCounter.BadTLPErrors;
                    break;
                }            
                case SWH_BADDLLPERRORS:
                {   
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_BADDLLPERRORS]   = PtrSwhHealthInfo->PortStatus[portNumber].ErrorCounter.BadDLLPErrors;
                    break;
                }         
                case SWH_RECOVERYDIAGNOSTICSERRORS:
                {   
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_RECOVERYDIAGNOSTICSERRORS]   =  PtrSwhHealthInfo->PortStatus[portNumber].ErrorCounter.RecoveryDiagnosticsErrors;
                    break;
                }         
                case SWH_LINKDOWNCOUNT:
                {   
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_LINKDOWNCOUNT]   =  PtrSwhHealthInfo->PortStatus[portNumber].ErrorCounter.LinkDownCount;
                    break;
                }         
                case SWH_PORT_TYPE:
                {   
                    PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_PORT_TYPE]   =  PtrPciePortProperties->PortConfigurations[portConfigEntryIndex].PortType;
                    break;
                }         
 

            }
        
        }
    }
    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("shmiSwitchHealthCheck (status=0x%x)", status);    
    return (status);
    
}

/**
 *
 *  @method  shmiCompareHealthData ()
 *
 *  @param   PtrSwhHealthConfigDataRef        Pointer to switch referce health info.
 *
 *  @param   PtrSwhHealthConfigDataReadBack            Pointer to read back configuration data.
 *
 *  @param   PtrErrorInfo               pointer to  error code
 * 

    @return  SCRUTINY_STATUS    Indication Success or Fail  
 *
 *  @brief                           This method fill the read back configuration data. 
 *
 */


SCRUTINY_STATUS shmiCompareHealthData (__IN__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthConfigDataRef, __IN__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthConfigDataReadBack, __OUT__ PTR_SCRUTINY_SWITCH_HEALTH_ERROR_INFO PtrErrorInfo)
{
    U32 portNumber, tableIndex;
    SCRUTINY_STATUS  status;
    U32  dwordOffset, bitShift;
    SCRUTINY_SWITCH_HEALTH_ERROR_CODE  healthErrorCode;

    gPtrLoggerSwitch->logiFunctionEntry ("shmiCompareHealthData (PtrSwhHealthConfigDataRef=%x, PtrSwhHealthConfigDataReadBack=%x, PtrErrorInfo=%x)", PtrSwhHealthConfigDataRef != NULL, PtrSwhHealthConfigDataReadBack != NULL, PtrErrorInfo!=NULL);
    


    // fill switch global item data
    for (tableIndex = 0; tableIndex < SWH_GLOBAL_MAX_ITEMS; tableIndex++)
    {
        //every time initialize it as zero
        healthErrorCode = 0;
        switch (tableIndex)
        {
            case SWH_TEMPERATURE:
            {
                if (PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_TEMPERATURE] >= PtrSwhHealthConfigDataRef->SwhMonitorItems[SWH_TEMPERATURE])
                {
                    healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_OVER_TEMPERATURE;
                }
                break; 
            }
            case SWH_ENCL_STATUS:
            {
                if ((PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_ENCL_STATUS] & PtrSwhHealthConfigDataRef->SwhMonitorItems[SWH_ENCL_STATUS]) != 0)
                {
                    healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_ENCL_STATUS;
                }
                break; 

            }
            case SWH_SASADDR_LOW:
            {
                if (PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_SASADDR_LOW] != PtrSwhHealthConfigDataRef->SwhMonitorItems[SWH_SASADDR_LOW])
                {
                    healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_SASADDR_MISMATCH;
                }
                break;
            }
            
            case SWH_SASADDR_HIGH:
            {
                if (PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_SASADDR_HIGH] != PtrSwhHealthConfigDataRef->SwhMonitorItems[SWH_SASADDR_HIGH])
                {
                    healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_SASADDR_MISMATCH;
                }
                break;
            }
            case SWH_FW_VER:
            {
                if (PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_FW_VER] != PtrSwhHealthConfigDataRef->SwhMonitorItems[SWH_FW_VER])
                {
                    healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_FW_VER_MISMATCH;
                }
                break;
            }
            case SWH_TOTAL_PORTS:
            {
                if (PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_TOTAL_PORTS] != PtrSwhHealthConfigDataRef->SwhMonitorItems[SWH_TOTAL_PORTS])
                {
                    healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_TOTAL_PORTS_MISMATCH;
                }
                break;
            }
            case SWH_TOTAL_HOST_PORTS:
            {   
                if (PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_TOTAL_HOST_PORTS] != PtrSwhHealthConfigDataRef->SwhMonitorItems[SWH_TOTAL_HOST_PORTS])
                {
                    healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_TOTAL_HOST_PORTS_MISMATCH;
                }
                break;
            }            

        }
        if (healthErrorCode)
        {
            dwordOffset = healthErrorCode / 32;
            bitShift = healthErrorCode % 32;
            PtrErrorInfo->ErrorDwords[dwordOffset] |= (0x01 << bitShift);
        }
        
        
    }
    


    
    // fill switch port item data
        
    for (portNumber = 0; portNumber < PtrSwhHealthConfigDataReadBack->SwhMonitorItems[SWH_TOTAL_PORTS]; portNumber++)
    {
        if (!PtrSwhHealthConfigDataRef->PortMonData[portNumber].Valid)
        {
            continue;
        }
        
        for (tableIndex = 0; tableIndex < SWH_PORT_MAX_ITEMS; tableIndex++)
        {
            //every time initialize it as zero
            healthErrorCode = 0;
            switch (tableIndex)
            {
                case SWH_LINK_UP:
                {
                    if (PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_LINK_UP])
                    {
                        if (!PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_LINK_UP])
                        {
                            healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_NOT_LINK_UP;
                        }
                    }
                    break; 
                }
                case SWH_MAX_LINK_RATE:
                {
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_MAX_LINK_RATE] != PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_MAX_LINK_RATE])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_MAX_LINK_RATE_MISMATCH;
                    }
                    break; 

                }
                case SWH_NEGOTIATED_LINK_RATE:
                {
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_LINK_UP])
                    {
                        if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_NEGOTIATED_LINK_RATE] != PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_NEGOTIATED_LINK_RATE])
                        {
                            healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_NEGOTIATED_LINK_RATE_MISMATCH;
                        }
                    }
                    break;
                }
            
                case SWH_MAX_LINK_WIDTH:
                {
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_MAX_LINK_WIDTH] != PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_MAX_LINK_WIDTH])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_MAX_LINK_WIDTH_MISMATCH;
                    }
                    break;
                }
                case SWH_NEGOTIATED_LINK_WIDTH:
                {
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_LINK_UP])
                    {
                        if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_NEGOTIATED_LINK_WIDTH] != PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_NEGOTIATED_LINK_WIDTH])
                        {
                            healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_NEGOTIATED_LINK_WIDTH_MISMATCH;
                        }
                    }
                    break;
                }
                case SWH_PORTRECEIVERERRORS:
                {
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_PORTRECEIVERERRORS] >= PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_PORTRECEIVERERRORS])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_OVER_PORTRECEIVERERRORS;
                    }
                    break;
                }
                case SWH_BADTLPERRORS:
                {                       
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_BADTLPERRORS] >= PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_BADTLPERRORS])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_OVER_BADTLPERRORS;
                    }
                    break;
                }            
                case SWH_BADDLLPERRORS:
                {   
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_BADDLLPERRORS] >= PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_BADDLLPERRORS])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_OVER_BADDLLPERRORS;
                    }
                    break;
                }         
                case SWH_RECOVERYDIAGNOSTICSERRORS:
                {   
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_RECOVERYDIAGNOSTICSERRORS] >= PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_RECOVERYDIAGNOSTICSERRORS])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_OVER_RECOVERYDIAGNOSTICSERRORS;
                    }
                    break;
                }         
                case SWH_LINKDOWNCOUNT:
                {   
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_LINKDOWNCOUNT] >= PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_LINKDOWNCOUNT])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_OVER_LINKDOWNCOUNT;
                    }
                    break;
                }         
                case SWH_PORT_TYPE:
                {   
                    if (PtrSwhHealthConfigDataReadBack->PortMonData[portNumber].PortMonitorItems[SWH_PORT_TYPE] != PtrSwhHealthConfigDataRef->PortMonData[portNumber].PortMonitorItems[SWH_PORT_TYPE])
                    {
                        healthErrorCode = SCRUTINY_SWITCH_HEALTH_ERROR_PORT_TYPE_MISMATCH;
                    }
                    break;
                }         
 

            }
            if(healthErrorCode)
            {
                dwordOffset = healthErrorCode / 32;
                bitShift = healthErrorCode % 32;
                PtrErrorInfo->ErrorDwords[dwordOffset] |= (0x01 << bitShift);
            }
            
        
        }
    }
    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("shmiCompareHealthData (status=0x%x)", status);    
    return (status);
    
}


