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

ITEMS_LOOKUP_TABLE  gExpGlobalMonitorItemsTable[] =
{
    { ITEM_NAME (EXP_TEMPERATURE), EXP_TEMPERATURE },
    { ITEM_NAME (EXP_ENCL_STATUS), EXP_ENCL_STATUS },
    { ITEM_NAME (EXP_SASADDR_LOW), EXP_SASADDR_LOW },
    { ITEM_NAME (EXP_SASADDR_HIGH), EXP_SASADDR_HIGH },
    { ITEM_NAME (EXP_FW_VER), EXP_FW_VER },
    { ITEM_NAME (EXP_TOTAL_PHYS), EXP_TOTAL_PHYS },
    { ITEM_NAME (EXP_SCE_STATUS_ZERO), EXP_SCE_STATUS_ZERO },
    { ITEM_NAME (EXP_SCE_STATUS_ONE), EXP_SCE_STATUS_ONE },
    { ITEM_NAME (EXP_SCE_STATUS_TWO), EXP_SCE_STATUS_TWO },
    { ITEM_NAME (EXP_SCE_STATUS_THREE), EXP_SCE_STATUS_THREE },
    { ITEM_NAME (EXP_SCE_STATUS_FOUR), EXP_SCE_STATUS_FOUR },
    { ITEM_NAME (EXP_SCE_STATUS_FIVE), EXP_SCE_STATUS_FIVE },
    { ITEM_NAME (EXP_SCE_STATUS_SIX), EXP_SCE_STATUS_SIX },
    { ITEM_NAME (EXP_SCE_STATUS_SEVEN), EXP_SCE_STATUS_SEVEN },
};

ITEMS_LOOKUP_TABLE  gExpPhyMonitorItemsTable[] = 
{
    { ITEM_NAME (EXP_LINK_UP), EXP_LINK_UP },
    { ITEM_NAME (EXP_PROGRAMMED_MAX_LINK_RATE), EXP_PROGRAMMED_MAX_LINK_RATE },
    { ITEM_NAME (EXP_PROGRAMMED_MIN_LINK_RATE), EXP_PROGRAMMED_MIN_LINK_RATE },
    { ITEM_NAME (EXP_HARDWARE_MAX_LINK_RATE), EXP_HARDWARE_MAX_LINK_RATE },
    { ITEM_NAME (EXP_HARDWARE_MIN_LINK_RATE), EXP_HARDWARE_MIN_LINK_RATE },
    { ITEM_NAME (EXP_NEGOTIATED_LINK_RATE), EXP_NEGOTIATED_LINK_RATE },
    { ITEM_NAME (EXP_INVALIDDWORDCOUNT), EXP_INVALIDDWORDCOUNT },
    { ITEM_NAME (EXP_RUNNINGDISPARITYERRORCOUNT), EXP_RUNNINGDISPARITYERRORCOUNT },
    { ITEM_NAME (EXP_LOSSDWORDSYNCHCOUNT), EXP_LOSSDWORDSYNCHCOUNT },    
    { ITEM_NAME (EXP_PHYRESETPROBLEMCOUNT), EXP_PHYRESETPROBLEMCOUNT },
    { ITEM_NAME (EXP_ATTACHED_DEVICE), EXP_ATTACHED_DEVICE },
    { ITEM_NAME (EXP_EDFB_ENABLE), EXP_EDFB_ENABLE },
    { ITEM_NAME (EXP_ZONE_GROUP), EXP_ZONE_GROUP },
    
};

static EXP_HEALTH_MONITOR_DATA      gExpHealthConfigData;


/**
 *
 * @method  ehmiGetExpEnclosureState()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PtrExpState         pointer to the returned expander state
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   read expander state from expander diag page 2
 *
 *
 */ 
SCRUTINY_STATUS ehmiGetExpEnclosureState (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice,
    __OUT__ PU8                 PtrExpState     
)
{
    SCRUTINY_STATUS     status;            
    SCRUTINY_SCSI_PASSTHROUGH       scsiRequest = { 0 };
    U8                  tempBuffer[8];
    
    gPtrLoggerExpanders->logiFunctionEntry ("ehmiGetExpEnclosureState (PtrDevice=%x, PtrExpState=%x)", 
        PtrDevice != NULL, PtrExpState != NULL);

    sosiMemSet (tempBuffer, 0, 8);

    scsiRequest.CdbLength = 6;
    scsiRequest.PtrDataBuffer = tempBuffer;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_RECEIVE_DIAGNOSTIC;
    scsiRequest.Cdb[0x01]  = 0x01;         // PCV = 1
    scsiRequest.Cdb[0x02]  = 0x02;         // page code = 2
    scsiRequest.Cdb[0x03]  = 0x00;
    scsiRequest.Cdb[0x04]  = 0x08;
    scsiRequest.Cdb[0x05]  = 0x00;
            
    scsiRequest.DataDirection = DIRECTION_READ;
    scsiRequest.DataBufferLength = 8;
    
    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
       gPtrLoggerExpanders->logiFunctionExit ("ehmiGetExpEnclosureState (status=0x%x)", status);
       return (status);
    }

    gPtrLoggerExpanders->logiDumpMemoryInVerbose (tempBuffer, 8, gPtrLoggerExpanders);

    if (tempBuffer[0] != 0x2)
    {
       gPtrLoggerExpanders->logiFunctionExit ("ehmiGetExpEnclosureState (status=0x%x)", SCRUTINY_STATUS_FAILED);
       return (SCRUTINY_STATUS_FAILED);
    }

    *PtrExpState = (U8) (tempBuffer[1] & 0x7);

    gPtrLoggerExpanders->logiFunctionExit ("ehmiGetExpEnclosureState (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);

}

// TODO: move the function expiGetPhyLinkStatus() to expphy.c 
/**
 *
 * @method  expiGetPhyLinkStatus()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PhyIdentifier       phy index
 * 
 * @param   PtrSasLinkStatus    data structure for link status
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   read link status of specific phy by sending smp Discover
 *
 *
 */ 
SCRUTINY_STATUS expiGetPhyLinkStatus (
    __IN__  PTR_SCRUTINY_DEVICE                 PtrDevice, 
    __IN__  U8                                  PhyIdentifier, 
    __OUT__ PTR_SCRUTINY_EXPANDER_LINK_STATUS   PtrSasLinkStatus
)
{

    SMP_DISCOVER_REQUEST                    smpDiscoverRequest;
    PTR_SMP_DISCOVER_REQUEST                ptrSmpDiscoverRequest = &smpDiscoverRequest;
    SMP_DISCOVER_RESPONSE                   smpDiscoverResponse;
    SCRUTINY_STATUS 			            status = SCRUTINY_STATUS_FAILED;
    U32 						            size = sizeof (SMP_DISCOVER_RESPONSE);
    U32                                     index;
    U32                                     regVal;

    gPtrLoggerExpanders->logiFunctionEntry ("expiGetPhyLinkStatus (PtrDevice=%x, PhyIdentifier=%x, PtrSasPhyErrCounter=%x)", 
        PtrDevice != NULL, PhyIdentifier, PtrSasLinkStatus != NULL);

    sosiMemSet (&smpDiscoverRequest, 0, sizeof (SMP_DISCOVER_REQUEST));
    sosiMemSet (&smpDiscoverResponse, 0, sizeof (SMP_DISCOVER_RESPONSE));

    smpDiscoverRequest.SMPFrameType = SMP_FRAME_TYPE_SMP_REQUEST;    
    smpDiscoverRequest.Function = SMP_FUNCTION_DISCOVER;
    smpDiscoverRequest.RequestLength = 0x02;
    smpDiscoverRequest.AllocRespLength = 0;   // 0 means the SMP return the first 52 bytes plus the CRC field
    smpDiscoverRequest.PhyIdentifier = PhyIdentifier;


    /* We need to swap the bytes */
    //the structure defined as big endian, need change to little endian    
    for (index = 0; index < (U32) ((sizeof (SMP_DISCOVER_REQUEST)) / 4); index++)
    {
        ((PU32) ptrSmpDiscoverRequest)[index] = swap32 (((PU32) ptrSmpDiscoverRequest)[index]);
    }
	
    status = edmiPerformSmpPassthrough (PtrDevice, (PU8) &smpDiscoverRequest, sizeof (SMP_DISCOVER_REQUEST), 
                                        (PU8) &smpDiscoverResponse, size);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	
        gPtrLoggerExpanders->logiDebug ("edmiPerformSmpPassthrough (SMPPassthrough Status=%x)", status);
        gPtrLoggerExpanders->logiFunctionExit ("expiGetPhyLinkStatus  (Status = %x) ",status);
        return (status);
    }                                    

    gPtrLoggerExpanders->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", &smpDiscoverResponse, 52); 
    gPtrLoggerExpanders->logiDumpMemoryInVerbose (((PU8) (&smpDiscoverResponse)), 52, gPtrLoggerExpanders);          

    gPtrLoggerExpanders->logiDebug("smpDiscoverResponse Function = %x, FunctionResult = %x",
        smpDiscoverResponse.Header.Fields.Function, smpDiscoverResponse.Header.Fields.FunctionResult);
    

    if ( (smpDiscoverResponse.Header.Fields.Function != SMP_FUNCTION_DISCOVER) || 
        (smpDiscoverResponse.Header.Fields.FunctionResult != SMP_RESPONSE_FUNCTION_RESULT_ACCEPTED) )
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiGetPhyLinkStatus (SMP Function Failed=%x, Result=%x)", 
            smpDiscoverResponse.Header.Fields.Function, smpDiscoverResponse.Header.Fields.FunctionResult);
		status = SCRUTINY_STATUS_IGNORE;
        gPtrLoggerExpanders->logiFunctionExit ("expiGetPhyLinkStatus  (Status = %x) ",status);
        return (status);
    }

    
    PtrSasLinkStatus->HardwareMaxLinkRate = smpDiscoverResponse.Response.Fields.HardwareMaxPhysLinkRate;
    PtrSasLinkStatus->HardwareMinLinkRate = smpDiscoverResponse.Response.Fields.HardwareMinPhysLinkRate;    
    PtrSasLinkStatus->ProgrammedMaxLinkRate = smpDiscoverResponse.Response.Fields.ProgMaxPhysLinkRate; 
    PtrSasLinkStatus->ProgrammedMinLinkRate = smpDiscoverResponse.Response.Fields.ProgMinPhysLinkRate; 
    PtrSasLinkStatus->NegotiatedLinkRate = smpDiscoverResponse.Response.Fields.NegotiatedLogicalLinkRate;
    PtrSasLinkStatus->ZoneGroup = smpDiscoverResponse.Response.Fields.ZoneGroup;
    PtrSasLinkStatus->VirtualPhy = smpDiscoverResponse.Response.Fields.VirtualPhy;
    
    if (smpDiscoverResponse.Response.Fields.AttachedDeviceType == 1) 
    {
        // SAS or SATA device
        if (smpDiscoverResponse.Response.Fields.AttachedSATADevice == 1)
        {
            PtrSasLinkStatus->AttachedDeviceType = DEVICE_TYPE_SATA_DEVICE;
        }
        else
        {
            PtrSasLinkStatus->AttachedDeviceType = DEVICE_TYPE_SAS_DEVICE;
        }
        
    }
    else if (smpDiscoverResponse.Response.Fields.AttachedDeviceType == 2)
    {
        PtrSasLinkStatus->AttachedDeviceType = DEVICE_TYPE_EXPANDER_DEVICE;
    }
    else 
    {
        // NO device or unknown device
        PtrSasLinkStatus->AttachedDeviceType = DEVICE_TYPE_NO_DEVICE;
    }

    
    if (PhyIdentifier < 32)
    {
        status = bsdiMemoryRead32 (PtrDevice, EXP_REGISTER_EDFB_ENABLE_LOW, &regVal, sizeof (U32));
    }
    else if (PhyIdentifier < 40) 
    {
        status = bsdiMemoryRead32 (PtrDevice, EXP_REGISTER_EDFB_ENABLE_HIGH, &regVal, sizeof (U32));
    }
    else
    {
        regVal = 0;
        status = SCRUTINY_STATUS_SUCCESS;
    }
    gPtrLoggerExpanders->logiDebug ("EDFB Enable register = %x", regVal);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("expiGetPhyLinkStatus  (Status = %x) ",status);
        return (status);
    }    

    if (PhyIdentifier < 32)
    {
        PtrSasLinkStatus->EDFBEnable = (U8)((regVal >> PhyIdentifier)&0x01);
    }
    else if (PhyIdentifier < 40) 
    {
        PtrSasLinkStatus->EDFBEnable = (U8)((regVal >> (PhyIdentifier - 32))&0x01);
    }
    else
    {
        PtrSasLinkStatus->EDFBEnable = 0;
    }


    gPtrLoggerExpanders->logiFunctionExit ("expiGetPhyLinkStatus  (Status = %x) ",status);
    
    return (status);

}


/**
 *
 * @method  expiGetSelfConfigStatus()
 *
 *
 * @param   PtrDevice           pointer to the device
 * 
 * @param   PtrSasLinkStatus    data structure for link status
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   read expander self configuration status  by sending smp REPORT SELF-CONFIGURATION STATUS 
 *
 *
 */ 
SCRUTINY_STATUS expiGetSelfConfigStatus (
    __IN__  PTR_SCRUTINY_DEVICE                 PtrDevice, 
    __OUT__ PTR_SCRUTINY_EXPANDER_SELF_CONFIG_STATUS   PtrSelfConfigStatus
)
{
    SCRUTINY_STATUS 			            status = SCRUTINY_STATUS_FAILED;
    SMP_REPORT_SELF_CONFIG_STATUS_REQUEST   smpSelfConfigStatusRequest;
    PTR_SMP_REPORT_SELF_CONFIG_STATUS_REQUEST   ptrSmpSelfConfigStatusRequest;
    PTR_SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE  ptrSmpSelfConfigStatusResponse;
    U32                                     index;
    PTR_SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR     ptrStatusDescriptor;
    U32                                      numDesc, lenDesc;

    gPtrLoggerExpanders->logiFunctionEntry ("expiGetSelfConfigStatus (PtrDevice=%x, PtrSelfConfigStatus=%x)", PtrDevice != NULL, PtrSelfConfigStatus != NULL);


    ptrSmpSelfConfigStatusResponse = sosiMemAlloc (SMP_BUFFER_SIZE);
    if (ptrSmpSelfConfigStatusResponse == NULL)
    {
        gPtrLoggerExpanders->logiFunctionExit ("expiGetSelfConfigStatus  (Status = %x) ", SCRUTINY_STATUS_NO_MEMORY);
        return (SCRUTINY_STATUS_NO_MEMORY);
    }
        
    sosiMemSet (&smpSelfConfigStatusRequest, 0, sizeof (SMP_REPORT_SELF_CONFIG_STATUS_REQUEST));
    sosiMemSet (ptrSmpSelfConfigStatusResponse, 0, SMP_BUFFER_SIZE);

    smpSelfConfigStatusRequest.SMPFrameType = SMP_FRAME_TYPE_SMP_REQUEST;
    smpSelfConfigStatusRequest.Function = SMP_FUNCTION_REPORT_SELF_CONFIG_STATUS;
    smpSelfConfigStatusRequest.RequestLength = 0x01;
    smpSelfConfigStatusRequest.AllocRespLength = (SMP_TGT_BUFFER_DWORDS - 1);
    smpSelfConfigStatusRequest.StartSelfConfigStatusDescIndex = 0x1;  

    ptrSmpSelfConfigStatusRequest = &smpSelfConfigStatusRequest;
    
    /* We need to swap the bytes */
    //the structure defined as big endian, need change to little endian    
    for (index = 0; index < (U32) ((sizeof (SMP_REPORT_SELF_CONFIG_STATUS_REQUEST)) / 4); index++)
    {
        ((PU32) ptrSmpSelfConfigStatusRequest)[index] = swap32 (((PU32) ptrSmpSelfConfigStatusRequest)[index]);
    }
	
    status = edmiPerformSmpPassthrough (PtrDevice, (PU8)&smpSelfConfigStatusRequest, sizeof (SMP_REPORT_SELF_CONFIG_STATUS_REQUEST), 
                                        (PU8) ptrSmpSelfConfigStatusResponse, SMP_BUFFER_SIZE);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
    	
        gPtrLoggerExpanders->logiDebug ("edmiPerformSmpPassthrough (SMPPassthrough Status=%x)", status);
        gPtrLoggerExpanders->logiFunctionExit ("expiGetSelfConfigStatus  (Status = %x) ",status);
        return (status);
    }                                    

    gPtrLoggerExpanders->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", ptrSmpSelfConfigStatusResponse, SMP_BUFFER_SIZE); 
    gPtrLoggerExpanders->logiDumpMemoryInVerbose (((PU8) (ptrSmpSelfConfigStatusResponse)), SMP_BUFFER_SIZE, gPtrLoggerExpanders);          

    gPtrLoggerExpanders->logiDebug("expiGetSelfConfigStatus Function = %x, FunctionResult = %x",
        ptrSmpSelfConfigStatusResponse->Header.Fields.Function, ptrSmpSelfConfigStatusResponse->Header.Fields.FunctionResult);
    

    if ( (ptrSmpSelfConfigStatusResponse->Header.Fields.Function != SMP_FUNCTION_REPORT_SELF_CONFIG_STATUS) || 
        (ptrSmpSelfConfigStatusResponse->Header.Fields.FunctionResult != SMP_RESPONSE_FUNCTION_RESULT_ACCEPTED) )
    {
    	
        gPtrLoggerExpanders->logiDebug ("expiGetSelfConfigStatus (SMP Function Failed=%x, Result=%x)", 
            ptrSmpSelfConfigStatusResponse->Header.Fields.Function, ptrSmpSelfConfigStatusResponse->Header.Fields.FunctionResult);
		status = SCRUTINY_STATUS_IGNORE;
        gPtrLoggerExpanders->logiFunctionExit ("expiGetSelfConfigStatus  (Status = %x) ",status);
        return (status);
    }

    if (ptrSmpSelfConfigStatusResponse->Response.Fields.CommonResponse.NumSelfConfigStatusDescriptors == 0) 
    {
        // NO descriptor
        status = SCRUTINY_STATUS_SUCCESS;
        gPtrLoggerExpanders->logiFunctionExit ("expiGetSelfConfigStatus  (Status = %x) ",status);
        return (status);
    }

    numDesc = ptrSmpSelfConfigStatusResponse->Response.Fields.CommonResponse.NumSelfConfigStatusDescriptors;
    lenDesc = ptrSmpSelfConfigStatusResponse->Response.Fields.CommonResponse.SelfConfigStatusDescLen * 4;
    ptrStatusDescriptor = &ptrSmpSelfConfigStatusResponse->Response.Fields.SceDescriptor[0];
    
    for (index=0; index < numDesc; index++)
    {
        PtrSelfConfigStatus->StatusDescriptors[index].StatusType = ptrStatusDescriptor->StatusType;
        PtrSelfConfigStatus->StatusDescriptors[index].Final = ptrStatusDescriptor->Final;
        PtrSelfConfigStatus->StatusDescriptors[index].PhyIdentifier = ptrStatusDescriptor->PhyIdentifier;
        PtrSelfConfigStatus->StatusDescriptors[index].SASAddress.High = ptrStatusDescriptor->SasAddress.Word.High;
        PtrSelfConfigStatus->StatusDescriptors[index].SASAddress.Low = ptrStatusDescriptor->SasAddress.Word.Low;

        ptrStatusDescriptor = (PTR_SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR) ( (PU8)ptrStatusDescriptor + lenDesc);
    }

    PtrSelfConfigStatus->TotalSelfConfigStatusDescriptors = numDesc;
    

    gPtrLoggerExpanders->logiFunctionExit ("expiGetSelfConfigStatus  (Status = %x) ",SCRUTINY_STATUS_SUCCESS);
    
    return (SCRUTINY_STATUS_SUCCESS);

}



/**
 *
 * @method  ehmGetHealthData()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PtrExpHealthInfo    returned expander health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   request the expander healthe data
 *
 *
 */
SCRUTINY_STATUS ehmiGetHealthData (
    __IN__   PTR_SCRUTINY_DEVICE             PtrDevice, 
    __OUT__  PTR_SCRUTINY_EXPANDER_HEALTH    PtrExpHealthInfo
)
{
    SCRUTINY_STATUS     status = SCRUTINY_STATUS_SUCCESS;
    U8                  index;

    gPtrLoggerExpanders->logiFunctionEntry ("ehmiGetHealthData (PtrDevice=%x, PtrExpHealthInfo=%x)", 
            PtrDevice != NULL, PtrExpHealthInfo != NULL);

    if (PtrExpHealthInfo == NULL)
    {
        gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    status = bsdScsiGetTemperatureValue (PtrDevice, &PtrExpHealthInfo->Temperature);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiDebug ("failed to get expander temperature: %x", status);
        gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", status);
        return (status);
    }

    status = ehmiGetExpEnclosureState (PtrDevice, &PtrExpHealthInfo->EnclosureStatus);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiDebug ("failed to get expander enclosure state: %x", status);
        gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", status);
        return (status);
    }

    status = expiGetTotalPhys (PtrDevice, &PtrExpHealthInfo->PhyInfo.TotalPhys);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiDebug ("failed to get expander phy number: %x", status);
        gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", status);
        return (status);
    } 

    for (index=0; index < PtrExpHealthInfo->PhyInfo.TotalPhys; index++)
    {
        status = expiGetPhyErrCounters (PtrDevice, index, &PtrExpHealthInfo->PhyInfo.ErrorCounters[index]);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerExpanders->logiDebug ("failed to get expander phy %d error counter: %x", index, status);
            gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", status);
            return (status);
        }     
        
        status = expiGetPhyLinkStatus (PtrDevice, index, &PtrExpHealthInfo->PhyInfo.LinkStatus[index]);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerExpanders->logiDebug ("failed to get expander phy %d Link Status: %x", index, status);
            gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", status);
            return (status);
        }
    }
    
    status = expiGetSelfConfigStatus (PtrDevice, &PtrExpHealthInfo->SelfConfigStatus);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiDebug ("failed to get expander self-config status: %x", status);
        gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", status);
        return (status);
    }    
    
    gPtrLoggerExpanders->logiFunctionExit ("ehmiGetHealthData (status=0x%x)", status);
    return SCRUTINY_STATUS_SUCCESS;
}

/**
 *
 * @method  ehmiDumpGlobalSegment()
 *
 *
 * @param   FileHandle           pointer to the file that will be dumped
 *
 * @param   PtrExpHealthData     point to input expander health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   dump the expander health data grobal segment
 *
 *
 */


SCRUTINY_STATUS ehmiDumpGlobalSegment (
    __IN__ SOSI_FILE_HANDLE  			FileHandle,
    __IN__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthData
)
{
    U32 index, bufSize;
    char  tempBuffer[512];
    SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerExpanders->logiFunctionEntry ("ehmiDumpGlobalSegment (FileHandle=%x, PtrExpHealthData=%x)", 
            								FileHandle != NULL, PtrExpHealthData != NULL);


    bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), "[%s]\n", SCRUTINY_CONFIG_SEG_NAME_EXP);
    status = sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);
    
    for (index = 0; index < EXP_GLOBAL_MAX_ITEMS; index ++)
    {
       bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s = 0x%X\n", gExpGlobalMonitorItemsTable[index].ItemName,
           					  PtrExpHealthData->ExpMonitorItems[index]);

       status = sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);
    }

    status = sosiFileWrite (FileHandle, (U8  *)"\n\n", 2);

    gPtrLoggerExpanders->logiFunctionExit ("ehmiDumpGlobalSegment (status=0x%x)", status);
	
    return (status);
}

/**
 *
 * @method  ehmiDumpPhySegment()
 *
 *
 * @param   FileHandle           pointer to the file that will be dumped
 *
 * @param   PtrExpHealthData     point to input expander health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   dump the expander health data phy segment
 *
 *
 */


SCRUTINY_STATUS ehmiDumpPhySegment (
    __IN__  SOSI_FILE_HANDLE  FileHandle,
    __IN__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthData
)
{
     U32 index, bufSize, phyIndex;
     char  tempBuffer[512];
     U32 startPhy, endPhy;
     SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerExpanders->logiFunctionEntry ("ehmiDumpPhySegment (FileHandle=%x, PtrExpHealthData=%x)", 
            FileHandle != NULL, PtrExpHealthData != NULL);

     startPhy = 0;

     for (phyIndex = 0; phyIndex < EXP_MAX_PHYS; )
     {
        if (PtrExpHealthData->PhyMonData[phyIndex].Valid == 0)
        {
            phyIndex++;
            continue;
        }
        index = phyIndex + 1;
        do 
        {   
            if (index < EXP_MAX_PHYS)
            {
                if (SCRUTINY_STATUS_SUCCESS == sosiMemCompare ((U8 *)&(PtrExpHealthData->PhyMonData[phyIndex]), 
                    (U8 *)&(PtrExpHealthData->PhyMonData[index]), sizeof (EXP_HEALTH_MONITOR_PHY_DATA)))
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

        startPhy = phyIndex; 
        endPhy  = index - 1;

        bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), "[%s %d - %d]\n", SCRUTINY_CONFIG_SEG_NAME_PHY,
            startPhy, endPhy);
        status = sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);

        for (index = 0; index < EXP_PHY_MAX_ITEMS; index++)
        {
            bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s = 0x%X\n", gExpPhyMonitorItemsTable[index].ItemName,
                PtrExpHealthData->PhyMonData[startPhy].PhyMonitorItems[index]);

            status = sosiFileWrite (FileHandle, (U8 *)tempBuffer, bufSize);
        }

        phyIndex = endPhy + 1;

        status = sosiFileWrite (FileHandle, (U8 *)"\n\n", 2);
     }

    gPtrLoggerExpanders->logiFunctionExit ("ehmiDumpPhySegment (status=0x%x)", status);
    return (status);

}

/**
 *
 * @method  ehmiDumpHealthConfigurationsToFiles()
 *
 *
 * @param   PtrFolderName        pointer to the fold name where dump file  will be 
 *
 * @param   PtrExpHealthData     point to input expander health data
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   dump the expander health data to file
 *
 *
 */


SCRUTINY_STATUS ehmiDumpHealthConfigurationsToFiles(  
    __IN__ const char*                PtrFolderName,
    __IN__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthData
)
{
    char                   inifileName[512];
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
    SOSI_FILE_HANDLE       fileHandle;
    
    gPtrLoggerExpanders->logiFunctionEntry ("ehmiDumpHealthConfigurationsToFiles (PtrFolderName=%x, PtrExpHealthData=%x)", 
            								PtrFolderName != NULL, PtrExpHealthData != NULL);
	

    if (PtrFolderName == NULL)
    {
        gPtrLoggerController->logiFunctionExit ("ehmiDumpHealthConfigurationsToFiles (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    sosiMemSet (inifileName, '\0', sizeof (inifileName));

    status = sosiMkDir (PtrFolderName);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerController->logiFunctionExit ("ehmiDumpHealthConfigurationsToFiles (status=0x%x)", status);
        return (status);
    }

    
    sosiStringCopy (inifileName, PtrFolderName);
    sosiStringCat (inifileName, EXP_HEALTH_CONFIG_LOG);
    
    fileHandle = sosiFileOpen (inifileName, "ab");

    ehmiDumpGlobalSegment (fileHandle, PtrExpHealthData);
    ehmiDumpPhySegment (fileHandle, PtrExpHealthData);
    
    sosiFileClose (fileHandle);
     
    gPtrLoggerExpanders->logiFunctionExit ("ehmiDumpHealthConfigurationsToFiles (status=0x%x)", status);
	
    return (status);
}

/**
 *
 * @method  ehmiEnumerateExpGlobalSegment()
 *
 *
 * @param   PtrDictionary        pointer to the dictionary that holding the decoded ini data  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the expander health data
 *
 *
 */


SCRUTINY_STATUS ehmiEnumerateExpGlobalSegment (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{
    U32 index, tableIndex;
    PTR_CONFIG_INI_ENTRIES   ptrEntry;
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;



    gPtrLoggerExpanders->logiFunctionEntry ("ehmiEnumerateExpGlobalSegment (PtrDictionary=%x)", 
            PtrDictionary != NULL);

    if ((sizeof (gExpGlobalMonitorItemsTable) / sizeof (ITEMS_LOOKUP_TABLE)) != EXP_GLOBAL_MAX_ITEMS)
    {
        /* Incompitible table and items */
        return SCRUTINY_STATUS_FAILED;
    }
    
    for (index = 0; index < PtrDictionary->TotalEntries; index++)
    {
        ptrEntry = &PtrDictionary->PtrIniEntries[index];

        for (tableIndex = 0; tableIndex < EXP_GLOBAL_MAX_ITEMS; tableIndex++)
        {
            if ((sosiStringCompare (ptrEntry->Key, gExpGlobalMonitorItemsTable[tableIndex].ItemName)) == SCRUTINY_STATUS_SUCCESS)
            {
                gExpHealthConfigData.ExpMonitorItems[gExpGlobalMonitorItemsTable[tableIndex].ItemIndex] = hexadecimalToDecimal (ptrEntry->Value);
                break;
            }
        }
        
        if (tableIndex == EXP_GLOBAL_MAX_ITEMS) 
        {
            return (SCRUTINY_STATUS_FAILED);
        }
    }

    gPtrLoggerExpanders->logiFunctionExit ("ehmiEnumerateExpGlobalSegment (status=0x%x)", status);
    return (status);
    
}

/**
 *
 * @method  ehmiEnumeratePhySegment()
 *
 *
 * @param   PtrDictionary        pointer to the dictionary that holding the decoded ini data  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the expander health data
 *
 *
 */


SCRUTINY_STATUS ehmiEnumeratePhySegment (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{
    PTR_CONFIG_INI_ENTRIES   ptrEntry;
    U32     startPhy, endPhy;
    U32     index = 0, tableIndex = 0;
    PU8     ptrChar;
    U8      temp[32];
    EXP_HEALTH_MONITOR_PHY_DATA     phySettings;
    U32     onlyOnePhy = 0;
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerExpanders->logiFunctionEntry ("ehmiEnumeratePhySegment (PtrDictionary=%x)", 
            PtrDictionary != NULL);


    if ((sizeof (gExpPhyMonitorItemsTable) / sizeof (ITEMS_LOOKUP_TABLE)) != EXP_PHY_MAX_ITEMS)
    {
        /* Incompitible table and items */
        return SCRUTINY_STATUS_FAILED;
    }

    sosiMemSet (&phySettings, 0, sizeof (phySettings));

    for (index = 0; index < INI_PARSER_MAXIMUM_CHAR_LENGTH; index++)
    {
        if (PtrDictionary->SegmentName[index] == '-') 
        {
            onlyOnePhy = 0;
            break;
        }
        if (PtrDictionary->SegmentName[index] == 0)
        {
            onlyOnePhy = 1;
            break;
        } 
    }


    
    /* firstly, get the range of phys */
    ptrChar = (PU8) PtrDictionary->SegmentName;
    
    /* skip 'PHY' */
    ptrChar += 3;  
    for (; ((*ptrChar != 0) && (sosiIsSpace(*ptrChar))); ptrChar++);

    sosiMemSet (temp, 0, sizeof (temp));
    for (index = 0; ((*ptrChar != 0) && (*ptrChar != '-') && (index < sizeof (temp))); ptrChar++, index++)
    {
        temp[index] = *ptrChar;
    }
    sosiStringTrim ((char *)temp);
    startPhy = sosiAtoi ((const char *)temp);
    ptrChar++;

    if (onlyOnePhy)
    {
        endPhy = startPhy;
    }
    else
    {
        sosiMemSet (temp, 0, sizeof (temp));
        for (index = 0; ((*ptrChar != 0) && (index < sizeof (temp))); ptrChar++, index++)
        {
            temp[index] = *ptrChar;
        }
        sosiStringTrim ((char *)temp);
        endPhy = sosiAtoi ((const char *)temp);
    }
    
    if (startPhy > endPhy)
    {
        /* something bad in the config file, just return */
        return (SCRUTINY_STATUS_FAILED);
    }

    for (index = 0; index < PtrDictionary->TotalEntries; index++)
    {
        ptrEntry = &PtrDictionary->PtrIniEntries[index];

        for (tableIndex = 0; tableIndex < EXP_PHY_MAX_ITEMS; tableIndex++)
        {
            if ((sosiStringCompare (ptrEntry->Key, gExpPhyMonitorItemsTable[tableIndex].ItemName)) == SCRUTINY_STATUS_SUCCESS)
            {
                phySettings.PhyMonitorItems[gExpPhyMonitorItemsTable[tableIndex].ItemIndex] = hexadecimalToDecimal (ptrEntry->Value);
                break;
            }
        }

        if (tableIndex == EXP_PHY_MAX_ITEMS)
        {
            return (SCRUTINY_STATUS_FAILED);
        }
        
    }
    phySettings.Valid = 1;
    for (index = startPhy; index <= endPhy; index++)
    {
        sosiMemCopy ( (U8*)&gExpHealthConfigData.PhyMonData[index], (U8*)&phySettings, sizeof (EXP_HEALTH_MONITOR_PHY_DATA));
    }

    gPtrLoggerExpanders->logiFunctionExit ("ehmiEnumeratePhySegment (status=0x%x)", status);
    return (status);     
}

 /**
 *
 * @method  ehmiEnumerateConfigDictionaries()
 *
 *
 * @param   PtrDictionary        pointer to the dictionary that holding the decoded ini data  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the expander health data
 *
 *
 */



SCRUTINY_STATUS ehmiEnumerateConfigDictionaries (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    PTR_CONFIG_INI_DICTIONARY  ptrTemp = NULL;
    SCRUTINY_STATUS    status = SCRUTINY_STATUS_SUCCESS;
  
    ptrTemp = PtrDictionary;

    gPtrLoggerExpanders->logiFunctionEntry ("ehmiEnumerateConfigDictionaries (PtrDictionary=%x)", 
            PtrDictionary != NULL);


    while (ptrTemp)
    {

        if (!ptrTemp->PtrIniEntries)
        {
            /* We don't have any entries. Thus get next list */
            ptrTemp = ptrTemp->PtrNext;
            continue;
        }

        if ((sosiStringCompare (ptrTemp->SegmentName, SCRUTINY_CONFIG_SEG_NAME_EXP)) == SCRUTINY_STATUS_SUCCESS)
        {
            status = ehmiEnumerateExpGlobalSegment (ptrTemp);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                break;
            }
        }
        else if ((sosiMemCompare ( (U8*)ptrTemp->SegmentName, (U8*)"PHY", 3)) == SCRUTINY_STATUS_SUCCESS)
        {
            // do phy section parse
            status = ehmiEnumeratePhySegment (ptrTemp);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                break;
            }
        }
        
        ptrTemp = ptrTemp->PtrNext;

    }

    gPtrLoggerExpanders->logiFunctionExit ("ehmiEnumerateConfigDictionaries (status=0x%x)", status);
    return (status); 

}


 /**
 *
 * @method  ehmiInitializeHealthConfigurations()
 *
 *
 * @param   PtrFolderName        pointer to the folder name that holding the health monitor ini file  
 *
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   decode the expander health data
 *
 *
 */


SCRUTINY_STATUS ehmiInitializeHealthConfigurations( __IN__ const char* PtrFolderName)
{
    char                   logFileName[512];
    char                   tempName[128];
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
    PTR_CONFIG_INI_DICTIONARY ptrConfigDictionary = NULL;
    
    gPtrLoggerExpanders->logiFunctionEntry ("ehmiInitializeHealthConfigurations (PtrFolderName=%x)", PtrFolderName != NULL);

    if (PtrFolderName == NULL)
    {
        gPtrLoggerController->logiFunctionExit ("ehmiInitializeHealthConfigurations (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
		
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    sosiMemSet (logFileName, '\0', sizeof (logFileName));
    sosiMemSet (tempName, '\0', sizeof (tempName));
    sosiMemSet (&gExpHealthConfigData, 0, sizeof (EXP_HEALTH_MONITOR_DATA));

    sosiStringCopy (logFileName, PtrFolderName);

    sosiStringCat (logFileName, EXP_HEALTH_CONFIG_INI);
    
    status = sosiMkDir (logFileName);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("ehmiInitializeHealthConfigurations (status=0x%x)", status);
		
        return (status);
    }	
    
    if (lcpiParserProcessINIFile (logFileName, &ptrConfigDictionary))
    {
        /* We don't have to fail for this as of now. */
        gPtrLoggerExpanders->logiFunctionExit ("ehmiInitializeHealthConfigurations (status=0x%x)", SCRUTINY_STATUS_FAILED);
		
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Check if we have any valid entries */
    if (!lcpiHasAnyValidEntries (ptrConfigDictionary))
    {
        gPtrLoggerExpanders->logiFunctionExit ("ehmiInitializeHealthConfigurations (status=0x%x)", SCRUTINY_STATUS_FAILED);
		
        return (SCRUTINY_STATUS_FAILED);
    }
 
    if (ehmiEnumerateConfigDictionaries (ptrConfigDictionary))
    {
    	gPtrLoggerExpanders->logiFunctionExit ("ehmiInitializeHealthConfigurations (status=0x%x)", SCRUTINY_STATUS_FAILED);
		
        status = SCRUTINY_STATUS_FAILED;
    }
        
    lcpiParserDestroyDictionary(ptrConfigDictionary);

    gPtrLoggerExpanders->logiFunctionExit ("ehmiInitializeHealthConfigurations (status=0x%x)", status);
	
    return (status); 
}

/**
 *
 *  @method  ehmiExpanderHealthCheck ()
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
 *  @brief                           This method do the expander health monitor check according to the config ini file content. 
 *
 */


SCRUTINY_STATUS  ehmiExpanderHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32  PtrErrorCode)
{
    SCRUTINY_STATUS                      status = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_EXPANDER_HEALTH         ptrExpHealthInfo;
    EXP_HEALTH_MONITOR_DATA              expHealthConfigDataReadback;
    U32                                  index;
    SCRUTINY_EXPANDER_HEALTH_ERROR_INFO  errorInfo;

    gPtrLoggerExpanders->logiFunctionEntry ("ehmiExpanderHealthCheck (PtrDevice=%x, PtrConfigFilePath=%x, DumpToFile=%x,PtrErrorCode=%x)", PtrDevice != NULL, PtrConfigFilePath != NULL, DumpToFile, PtrErrorCode!=NULL);

    sosiMemSet (&errorInfo, 0, sizeof(SCRUTINY_EXPANDER_HEALTH_ERROR_INFO));

    if (PtrConfigFilePath == NULL)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerExpanders->logiFunctionExit ("ehmiExpanderHealthCheck  (Status = %x) ",status);
        return (status);
    }
   
    status = ehmiInitializeHealthConfigurations (PtrConfigFilePath);
    if (status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("ehmiExpanderHealthCheck  (Status = %x) ",status);
        return (status);

    }
//    status = ehmiDumpHealthConfigurationsToFiles(PtrConfigFilePath, &gExpHealthConfigData);
//    if (status)
//    {
//        gPtrLoggerExpanders->logiFunctionExit ("ehmiExpanderHealthCheck  (Status = %x) ",status);
//        return (status);
//
//    }


    ptrExpHealthInfo = (PTR_SCRUTINY_EXPANDER_HEALTH) sosiMemAlloc (sizeof(SCRUTINY_EXPANDER_HEALTH));

    if (ptrExpHealthInfo == NULL)
    {
        status = SCRUTINY_STATUS_NO_MEMORY;		
        gPtrLoggerExpanders->logiFunctionExit ("ehmiExpanderHealthCheck  (Status = %x) ",status);
        return (status);

    }
	
    sosiMemSet (ptrExpHealthInfo, 0, sizeof(SCRUTINY_EXPANDER_HEALTH));
    
    status = ehmiGetHealthData (PtrDevice, ptrExpHealthInfo);
	
    if (status)
    {
        sosiMemFree (ptrExpHealthInfo);
        gPtrLoggerExpanders->logiFunctionExit ("ehmiExpanderHealthCheck  (Status = %x) ",status);
        return (status);

    }
    //below function always success.
    for (index = 0; index < EXP_MAX_PHYS; index++)
    {
        expHealthConfigDataReadback.PhyMonData[index].Valid = gExpHealthConfigData.PhyMonData[index].Valid;
    }
    
    status = ehmiFillExpHealthConfigDataReadBackData (PtrDevice,  ptrExpHealthInfo, &expHealthConfigDataReadback);
	
    if (DumpToFile)
    {
        status = ehmiDumpHealthConfigurationsToFiles(PtrConfigFilePath, &expHealthConfigDataReadback);
    }
	
    status = ehmiCompareHealthData (&gExpHealthConfigData, &expHealthConfigDataReadback, &errorInfo);
	
    //Currently we only return one U32 data to upper lay ,because C++ wrapper can not handle structure very well.
    //In the future ,if there are more than 32 errors return, we just need define more U32 variable, and no need change internal data structure. 
    *PtrErrorCode = errorInfo.ErrorDwords[0];
	
    sosiMemFree (ptrExpHealthInfo);

    status = SCRUTINY_STATUS_SUCCESS; 
    gPtrLoggerExpanders->logiFunctionExit ("ehmiExpanderHealthCheck  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ehmiFillExpHealthConfigDataReadBackData ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrExpHealthInfo        Pointer to expander health info.
 *
 *  @param   PtrExpHealthConfigDataReadBack            Pointer to read back configuration data.
 *

    @return  SCRUTINY_STATUS    Indication Success or Fail  
 *
 *  @brief                           This method fill the read back configuration data. 
 *
 */



SCRUTINY_STATUS ehmiFillExpHealthConfigDataReadBackData (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_EXPANDER_HEALTH PtrExpHealthInfo, __OUT__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthConfigDataReadBack)
{
    SCRUTINY_STATUS                 status = SCRUTINY_STATUS_SUCCESS;
    U32 phyIndex, tableIndex;
    U32 index, dwordOffset, bitShift;


    gPtrLoggerExpanders->logiFunctionEntry ("ehmiFillExpHealthConfigDataReadBackData (PtrDevice=%x, PtrExpHealthInfo=%x, PtrExpHealthConfigDataReadBack=%x)", PtrDevice != NULL, PtrExpHealthInfo != NULL,  PtrExpHealthConfigDataReadBack!=NULL);

    // fill exp global item data
    for (tableIndex = 0; tableIndex < EXP_GLOBAL_MAX_ITEMS; tableIndex++)
    {
        
        switch (tableIndex)
        {
            case EXP_TEMPERATURE:
            {
                PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_TEMPERATURE] = PtrExpHealthInfo->Temperature;
                break; 
            }
            case EXP_ENCL_STATUS:
            {
                PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_ENCL_STATUS] = PtrExpHealthInfo->EnclosureStatus;
                break; 

            }
            case EXP_SASADDR_LOW:
            {
                PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_SASADDR_LOW] = PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low;
                break;
            }
            
            case EXP_SASADDR_HIGH:
            {
                PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_SASADDR_HIGH] = PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High; 
                break;
            }
            case EXP_FW_VER:
            {
                PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_FW_VER]       = PtrDevice->DeviceInfo.u.ExpanderInfo.FWVersion;
                break;
            }
            case EXP_TOTAL_PHYS:
            {   //This will return physical phy number to customer, not include 3 virtual phys.
                PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_TOTAL_PHYS]   = (PtrExpHealthInfo->PhyInfo.TotalPhys - 3);
                break;
            }
            case EXP_SCE_STATUS_ZERO:
            case EXP_SCE_STATUS_ONE:
            case EXP_SCE_STATUS_TWO:
            case EXP_SCE_STATUS_THREE:
            case EXP_SCE_STATUS_FOUR:
            case EXP_SCE_STATUS_FIVE:
            case EXP_SCE_STATUS_SIX:
            case EXP_SCE_STATUS_SEVEN:
            {  
               
                //how to handle SCE status? there are many items read back,need figure out a good way.
                for (index = 0; index < PtrExpHealthInfo->SelfConfigStatus.TotalSelfConfigStatusDescriptors; index++)
                {
                    if( PtrExpHealthInfo->SelfConfigStatus.StatusDescriptors[index].StatusType )
                    {
                        dwordOffset = PtrExpHealthInfo->SelfConfigStatus.StatusDescriptors[index].StatusType / 32;
                        bitShift = PtrExpHealthInfo->SelfConfigStatus.StatusDescriptors[index].StatusType % 32;  
                        PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_SCE_STATUS_ZERO + dwordOffset]   |=  (0x01 << bitShift);
                    }
                }
                break;
            }            

        }
        
    }
    

    
    // fill exp phy item data
        
    for (phyIndex = 0; phyIndex < PtrExpHealthInfo->PhyInfo.TotalPhys; phyIndex++)
    {
        for (tableIndex = 0; tableIndex < EXP_PHY_MAX_ITEMS; tableIndex++)
        {
        
            switch (tableIndex)
            {
                case EXP_LINK_UP:
                {   if ((PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].NegotiatedLinkRate >= 8 ) && (PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].NegotiatedLinkRate <= 12 ))
                    {
                        PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_LINK_UP] = 1;
                    }
                    else
                    {
                        PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_LINK_UP] = 0;                    
                    }
                    break; 
                }                
                case EXP_PROGRAMMED_MAX_LINK_RATE:
                {
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_PROGRAMMED_MAX_LINK_RATE] = PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].ProgrammedMaxLinkRate;
                    break; 
                }
                case EXP_PROGRAMMED_MIN_LINK_RATE:
                {
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_PROGRAMMED_MIN_LINK_RATE] = PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].ProgrammedMinLinkRate;
                    break; 

                }
                case EXP_HARDWARE_MAX_LINK_RATE:
                {
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_HARDWARE_MAX_LINK_RATE] = PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].HardwareMaxLinkRate;
                    break;
                }
            
                case EXP_HARDWARE_MIN_LINK_RATE:
                {
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_HARDWARE_MIN_LINK_RATE] = PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].HardwareMinLinkRate; 
                    break;
                }
                case EXP_NEGOTIATED_LINK_RATE:
                {
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_NEGOTIATED_LINK_RATE]       = PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].NegotiatedLinkRate;
                    break;
                }
                case EXP_INVALIDDWORDCOUNT:
                {
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_INVALIDDWORDCOUNT]   = PtrExpHealthInfo->PhyInfo.ErrorCounters[phyIndex].InvalidDwordCount;
                    break;
                }
                case EXP_RUNNINGDISPARITYERRORCOUNT:
                {   //how to handle SCE status? there are many items read back,need figure out a good way.
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_RUNNINGDISPARITYERRORCOUNT]   =  PtrExpHealthInfo->PhyInfo.ErrorCounters[phyIndex].RunningDisparityErrorCount;
                    break;
                }            
                case EXP_LOSSDWORDSYNCHCOUNT:
                {   //how to handle SCE status? there are many items read back,need figure out a good way.
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_LOSSDWORDSYNCHCOUNT]   =  PtrExpHealthInfo->PhyInfo.ErrorCounters[phyIndex].LossDwordSynchCount;

                    break;
                }         
                case EXP_PHYRESETPROBLEMCOUNT:
                {   //how to handle SCE status? there are many items read back,need figure out a good way.
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_PHYRESETPROBLEMCOUNT]   =  PtrExpHealthInfo->PhyInfo.ErrorCounters[phyIndex].PhyResetProblemCount;
                    break;
                }         
                case EXP_ATTACHED_DEVICE:
                {   //how to handle SCE status? there are many items read back,need figure out a good way.
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_ATTACHED_DEVICE]   =  PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].AttachedDeviceType;
                    break;
                }         
                case EXP_EDFB_ENABLE:
                {   //how to handle SCE status? there are many items read back,need figure out a good way.
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_EDFB_ENABLE]   =  PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].EDFBEnable;
                    break;
                }         
                case EXP_ZONE_GROUP:
                {   //how to handle SCE status? there are many items read back,need figure out a good way.
                    PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_ZONE_GROUP]   =  PtrExpHealthInfo->PhyInfo.LinkStatus[phyIndex].ZoneGroup;
                    break;
                }         

            }
        
        }
    }
    
    // this function always success.
    status = SCRUTINY_STATUS_SUCCESS; 
    gPtrLoggerExpanders->logiFunctionExit ("ehmiFillExpHealthConfigDataReadBackData  (Status = %x) ",status);
    return (status); 
    
}


/**
 *
 *  @method  ehmiCompareHealthData ()
 *
 *  @param   PtrExpHealthConfigDataRef        Pointer to expander referce health info.
 *
 *  @param   PtrExpHealthConfigDataReadBack            Pointer to read back configuration data.
 *
 *  @param   PtrErrorInfo               pointer to  error code
 * 

    @return  SCRUTINY_STATUS    Indication Success or Fail  
 *
 *  @brief                           This method fill the read back configuration data. 
 *
 */



SCRUTINY_STATUS  ehmiCompareHealthData ( __IN__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthConfigDataRef, __IN__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthConfigDataReadBack, __OUT__ PTR_SCRUTINY_EXPANDER_HEALTH_ERROR_INFO  PtrErrorInfo)
{
    SCRUTINY_STATUS                 status = SCRUTINY_STATUS_SUCCESS;
    U32  phyIndex, tableIndex;
    U32  dwordOffset, bitShift;
    SCRUTINY_EXPANDER_HEALTH_ERROR_CODE healthErrorCode;

    gPtrLoggerExpanders->logiFunctionEntry ("ehmiCompareHealthData (PtrExpHealthConfigDataRef=%x, PtrExpHealthConfigDataReadBack=%x, PtrErrorInfo=%x)", PtrExpHealthConfigDataRef != NULL, PtrExpHealthConfigDataReadBack != NULL, PtrErrorInfo!=NULL);

    // fill exp global item data
    for (tableIndex = 0; tableIndex < EXP_GLOBAL_MAX_ITEMS; tableIndex++)
    {
        //every time initialize it as zero
        healthErrorCode = 0;
        switch (tableIndex)
        {
            case EXP_TEMPERATURE:
            {
                if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_TEMPERATURE] > PtrExpHealthConfigDataRef->ExpMonitorItems[EXP_TEMPERATURE])
                {
                    healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_TEMPERATURE;
                }
                break; 
            }
            case EXP_ENCL_STATUS:
            {
                if ((PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_ENCL_STATUS] & PtrExpHealthConfigDataRef->ExpMonitorItems[EXP_ENCL_STATUS]) != 0)
                {
                    healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_ENCL_STATUS;
                }
                break; 

            }
            case EXP_SASADDR_LOW:
            {
                if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_SASADDR_LOW] != PtrExpHealthConfigDataRef->ExpMonitorItems[EXP_SASADDR_LOW])
                {
                    healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_SASADDR_MISMATCH;
                }
                break;
            }
            
            case EXP_SASADDR_HIGH:
            {
                if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_SASADDR_HIGH] != PtrExpHealthConfigDataRef->ExpMonitorItems[EXP_SASADDR_HIGH])
                {
                    healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_SASADDR_MISMATCH;
                }                 
                break;
            }
            case EXP_FW_VER:
            {
                if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_FW_VER] != PtrExpHealthConfigDataRef->ExpMonitorItems[EXP_FW_VER])
                {
                    healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_FW_VER_MISMATCH;
                }
                break;
            }
            case EXP_TOTAL_PHYS:
            {   //This will return physical phy number to customer, not include 3 virtual phys.
                if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_TOTAL_PHYS] != PtrExpHealthConfigDataRef->ExpMonitorItems[EXP_TOTAL_PHYS])
                {
                    healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_TOTAL_PHYS_MISMATCH;
                }
                break;
            }
            case EXP_SCE_STATUS_ZERO:
            case EXP_SCE_STATUS_ONE:
            case EXP_SCE_STATUS_TWO:
            case EXP_SCE_STATUS_THREE:
            case EXP_SCE_STATUS_FOUR:
            case EXP_SCE_STATUS_FIVE:
            case EXP_SCE_STATUS_SIX:
            case EXP_SCE_STATUS_SEVEN:
            {
                //first check if sce has error
                if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[tableIndex])
                {
                    //if reference conf set some error trigger condition ,only these error will be reported
                    if (PtrExpHealthConfigDataRef->ExpMonitorItems[tableIndex])
                    {
                        if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[tableIndex] & PtrExpHealthConfigDataRef->ExpMonitorItems[tableIndex])
                        {
                            healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_SCE_STATUS;
                        }
                    }
                    else//if reference conf is 0, then any error will be reported.
                    {
                        if (PtrExpHealthConfigDataReadBack->ExpMonitorItems[tableIndex])
                        {
                            healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_SCE_STATUS;
                        }
                    
                    }
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
    

    
    // fill exp phy item data
        
    for (phyIndex = 0; phyIndex < PtrExpHealthConfigDataReadBack->ExpMonitorItems[EXP_TOTAL_PHYS]; phyIndex++)
    {
        if (!PtrExpHealthConfigDataRef->PhyMonData[phyIndex].Valid)
        {
            continue;
        }
        
        for (tableIndex = 0; tableIndex < EXP_PHY_MAX_ITEMS; tableIndex++)
        {
            //everytime initialize it as 0
            healthErrorCode = 0;
            switch (tableIndex)
            {
                case EXP_LINK_UP:
                {   
                    if (PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_LINK_UP])
                    {
                        if (!PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_LINK_UP])
                        {
                            healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_NOT_LINK_UP;
                        }
                    }
                    break; 
                }                
                case EXP_PROGRAMMED_MAX_LINK_RATE:
                {
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_PROGRAMMED_MAX_LINK_RATE] != PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_PROGRAMMED_MAX_LINK_RATE])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_PROGRAMMED_MAX_LINK_RATE_MISMATCH;
                    }
                    break; 
                }
                case EXP_PROGRAMMED_MIN_LINK_RATE:
                {
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_PROGRAMMED_MIN_LINK_RATE] != PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_PROGRAMMED_MIN_LINK_RATE])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_PROGRAMMED_MIN_LINK_RATE_MISMATCH;
                    }
                    break; 

                }
                case EXP_HARDWARE_MAX_LINK_RATE:
                {
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_HARDWARE_MAX_LINK_RATE] != PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_HARDWARE_MAX_LINK_RATE])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_HARDWARE_MAX_LINK_RATE_MISMATCH;
                    }
                    break;
                }
                case EXP_HARDWARE_MIN_LINK_RATE:
                {
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_HARDWARE_MIN_LINK_RATE] != PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_HARDWARE_MIN_LINK_RATE])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_HARDWARE_MIN_LINK_RATE_MISMATCH;
                    }
                    break;
                }
                case EXP_NEGOTIATED_LINK_RATE:
                {
                    //first need check if it is link up
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_LINK_UP])
                    {
                        if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_NEGOTIATED_LINK_RATE] != PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_NEGOTIATED_LINK_RATE])
                        {
                            healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_NEGOTIATED_LINK_RATE_MISMATCH;
                        }
                    }
                    break;
                }
                case EXP_INVALIDDWORDCOUNT:
                {
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_INVALIDDWORDCOUNT] >= PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_INVALIDDWORDCOUNT])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_INVALIDDWORDCOUNT;
                    }
                    break;
                }
                case EXP_RUNNINGDISPARITYERRORCOUNT:
                {   
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_RUNNINGDISPARITYERRORCOUNT] >= PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_RUNNINGDISPARITYERRORCOUNT])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_RUNNINGDISPARITYERRORCOUNT;
                    }
                    break;
                }            
                case EXP_LOSSDWORDSYNCHCOUNT:
                {   
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_LOSSDWORDSYNCHCOUNT] >= PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_LOSSDWORDSYNCHCOUNT])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_LOSSDWORDSYNCHCOUNT;
                    }
                    break;
                }         
                case EXP_PHYRESETPROBLEMCOUNT:
                {  
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_PHYRESETPROBLEMCOUNT] >= PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_PHYRESETPROBLEMCOUNT])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_PHYRESETPROBLEMCOUNT;
                    }
                    break;
                }         
                case EXP_ATTACHED_DEVICE:
                {   
                    //first need check if it is link up
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_LINK_UP])
                    {
                        if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_ATTACHED_DEVICE] != PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_ATTACHED_DEVICE])
                        {
                            healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_ATTACHED_DEVICE_MISMATCH;
                        }
                    }
                    break;
                }         
                case EXP_EDFB_ENABLE:
                {   
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_EDFB_ENABLE] !=  PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_EDFB_ENABLE])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_EDFB_MISMATCH;
                    }
                    break;
                }         
                case EXP_ZONE_GROUP:
                {  
                    if (PtrExpHealthConfigDataReadBack->PhyMonData[phyIndex].PhyMonitorItems[EXP_ZONE_GROUP] !=  PtrExpHealthConfigDataRef->PhyMonData[phyIndex].PhyMonitorItems[EXP_ZONE_GROUP])
                    {
                        healthErrorCode = SCRUTINY_EXPANDER_HEALTH_ERROR_ZONE_GROUP_MISMATCH;
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
    }
    
    // this function always success.
    status = SCRUTINY_STATUS_SUCCESS; 
    gPtrLoggerExpanders->logiFunctionExit ("ehmiCompareHealthData  (Status = %x) ",status);
    return (status); 
    


}


