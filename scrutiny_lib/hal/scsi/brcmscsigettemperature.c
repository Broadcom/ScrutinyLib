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



/**
 *
 *  @method  bsdScsiGetTemperatureValue()
 *
 *  @param   PtrDevice          pointer to  device
 *
 *  @param   PtrValue           Pointer to value to be written
 *
 *  @return  Status             '0' for success and  non-zero for failure
 *
 *  @brief                      This method will get temperature of the device which support temperature in SES page
 *                              return value is in Celsius degree
 *
 */


SCRUTINY_STATUS bsdScsiGetTemperatureValue (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrValue)
{
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;
    U32         	elementIndex;
    U32         	page;
    U8          	data[4] = {0};
    
    gPtrLoggerScsi->logiFunctionEntry ("bsdScsiGetTemperatureValue (PtrDevice=%x, PtrValue=%x)", PtrDevice != NULL, PtrValue != NULL);

    status = bsdScsiGetTemperatureElementIndex (PtrDevice, &elementIndex);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetTemperatureValue  (Status = %x) ", status);
        
        return (status);
    }
	
    gPtrLoggerScsi->logiDebug ("elementIndex = %x.", elementIndex);
    
    status = bsdScsiGetEnclosureStatusDiagnosticsPageSegment (PtrDevice, &page, elementIndex);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerScsi->logiDebug ("Unable to get the status diagnostics page segment.");
        gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetTemperatureValue  (Status = %x) ", status);

        return (status);
    }

    data[3] = (U8)((page >> 24) & 0xFF);
    data[2] = (U8)((page >> 16) & 0xFF);
    data[1] = (U8)((page >> 8) & 0xFF);
    data[0] = (U8)((page & 0xFF));
    *PtrValue = ((data[2] &0xFF) - 20);

    gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetTemperatureValue  (Status = %x) ",status);

    return (status);
     
}



/**
 *
 *  @method  bsdScsiGetTemperatureElementIndex()
 *
 *  @param   PtrDevice          pointer to  device
 *
 *  @param   PtrValue           Pointer to value to be written
 *
 *  @return  Status             '0' for success and  non-zero for failure
 *
 *  @brief                      This method will get temperature element index from SES page 
 *
 */


SCRUTINY_STATUS bsdScsiGetTemperatureElementIndex (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrValue)
{
    U32         				enclosureDescriptorLength, totalDescriptors, runningOffset;
    U8          				elementType,pageCode;
    U32         				index, elementIndex, diagnosticsPageHeaderSize;
    U8          				*ptrData;
    SCRUTINY_STATUS      		status = SCRUTINY_STATUS_FAILED;
    SCRUTINY_SCSI_PASSTHROUGH   scsiRequest = { 0 };
    

    gPtrLoggerScsi->logiFunctionEntry ("bsdScsiGetTemperatureElementIndex (PtrDevice=%x, PtrValue=%x)", PtrDevice != NULL, PtrValue != NULL);
    
    //Use Receive Diagnostic command to read Diagnostic Page1    
    ptrData = (PU8) sosiMemAlloc (SIZE_TO_READ_TEMP);
	
    if (ptrData == NULL)
    {
        status = SCRUTINY_STATUS_NO_MEMORY;
		
        gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetTemperatureElementIndex  (Status = %x) ",status);
        return (status);

    }   
	
    pageCode = 0x01;
	
    scsiRequest.CdbLength  = 6;
    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_RECEIVE_DIAGNOSTIC;  /* Op Code */
    scsiRequest.Cdb[0x01]  = 0x01;                             /* PCV Bit */
    scsiRequest.Cdb[0x02]  = pageCode;                         /* Page Code */
    scsiRequest.Cdb[0x03]  = 0xFF;                             /*Allocation Length SIZE_TO_READ_TEMP high byte */
    scsiRequest.Cdb[0x04]  = 0xFC;                             /*Allocation Length SIZE_TO_READ_TEMP low byte */
    scsiRequest.Cdb[0x05]  = 0x00;


    scsiRequest.DataDirection = DIRECTION_READ;
    scsiRequest.PtrDataBuffer = (PVOID) ptrData;
    scsiRequest.DataBufferLength = SIZE_TO_READ_TEMP;

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    gPtrLoggerScsi->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", ptrData, 0x1000);  /*will dump SIZE_TO_READ_TEMP, now just dupm 0x1000*/
    gPtrLoggerScsi->logiDumpMemoryInVerbose ((PU8) ptrData, 0x1000, gPtrLoggerScsi);          /*will dump SIZE_TO_READ_TEMP, now just dupm 0x1000*/

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sosiMemFree (ptrData);
		
        gPtrLoggerScsi->logiDebug ("SCSI read CDB failed for Element Index.");
        gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetTemperatureElementIndex  (Status = %x) ",status);
        return (status);
    }

    diagnosticsPageHeaderSize = 0x08;
    enclosureDescriptorLength = ptrData[diagnosticsPageHeaderSize + 3];
    totalDescriptors = ptrData[diagnosticsPageHeaderSize + 2];
    runningOffset = diagnosticsPageHeaderSize + enclosureDescriptorLength + 4;
    elementIndex = 0;

    for (index = 0; index < totalDescriptors; index++)
    {
        elementType = ptrData[runningOffset];

        if (elementType == 0x04)        /* Element for Temperature Sensor */
        {
        	*PtrValue = elementIndex + 1;
            
            sosiMemFree (ptrData);

			status = SCRUTINY_STATUS_SUCCESS; 
            gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetTemperatureElementIndex  (Status = %x) ",status);           
            return (status);
        }

        elementIndex += ptrData[runningOffset + 1];
        elementIndex++;
        runningOffset += 4;

    }

    sosiMemFree (ptrData);

	status = SCRUTINY_STATUS_FAILED;
    gPtrLoggerScsi->logiDebug (" Element Index Not Available.");
    gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetTemperatureElementIndex  (Status = %x) ",status);
	
    return (status);


}



/**
 *
 *  @method  bsdScsiGetEnclosureStatusDiagnosticsPageSegment()
 *
 *  @param   PtrDevice          pointer to  device
 *
 *  @param   PtrValue           Pointer to value to be written
 *
 *  @return  Status             '0' for success and  non-zero for failure
 *
 *  @brief                      This method will find temperature segment in SES page   
 *
 */


SCRUTINY_STATUS bsdScsiGetEnclosureStatusDiagnosticsPageSegment (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PU32 PtrDword, __IN__ U32 ElementIndex)
{

    U32         				index,elementOffset, enclosureStatusPageHeaderSize;
    U8          				*ptrData;
    U8          				pageCode;
    U8          				pay[4] = { 0 };
    SCRUTINY_STATUS      		status = SCRUTINY_STATUS_FAILED;
    SCRUTINY_SCSI_PASSTHROUGH   scsiRequest = { 0 };
    
    gPtrLoggerScsi->logiFunctionEntry ("bsdScsiGetEnclosureStatusDiagnosticsPageSegment (PtrDevice=%x, PtrDword=%x, ElementIndex=%x)", PtrDevice != NULL, PtrDword != NULL, ElementIndex);
    
    //Use Receive Diagnostic command to read Diagnostic Page2    
    ptrData = (PU8) sosiMemAlloc (SIZE_TO_READ_TEMP);

	if (ptrData == NULL)
    {
        status = SCRUTINY_STATUS_NO_MEMORY;
        gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetEnclosureStatusDiagnosticsPageSegment  (Status = %x) ",status);
        return (status);

    }   
   
    pageCode = 0x02;
    
    scsiRequest.CdbLength = 6;
    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_RECEIVE_DIAGNOSTIC;  /* Op Code */
    scsiRequest.Cdb[0x01]  = 0x01;                             /* PCV Bit */
    scsiRequest.Cdb[0x02]  = pageCode;                         /* Page Code */
    scsiRequest.Cdb[0x03]  = 0xFF;                             /*Allocation Length SIZE_TO_READ_TEMP high byte */
    scsiRequest.Cdb[0x04]  = 0xFC;                             /*Allocation Length SIZE_TO_READ_TEMP low byte */
    scsiRequest.Cdb[0x05]  = 0x00;

    scsiRequest.DataDirection = DIRECTION_READ;
    scsiRequest.PtrDataBuffer = (PVOID) ptrData;
    scsiRequest.DataBufferLength = SIZE_TO_READ_TEMP;

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    gPtrLoggerScsi->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", ptrData, 0x1000);  /*will dump SIZE_TO_READ_TEMP, now just dupm 0x1000*/
    gPtrLoggerScsi->logiDumpMemoryInVerbose ((PU8) ptrData, 0x1000, gPtrLoggerScsi);  

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sosiMemFree (ptrData);
        gPtrLoggerScsi->logiDebug ("SCSI read CDB failed for Element Index.");
        gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetEnclosureStatusDiagnosticsPageSegment  (Status = %x) ", status);
        return (status);
    }

    elementOffset = ElementIndex * 4;
    enclosureStatusPageHeaderSize = 0x08;
    index = elementOffset + enclosureStatusPageHeaderSize;

    pay[0] = ptrData[index + 0];
    pay[1] = ptrData[index + 1];
    pay[2] = ptrData[index + 2];
    pay[3] = ptrData[index + 3];

    *PtrDword = (U32)((pay[3] & 0xFF) << 24);
    *PtrDword = *PtrDword | ((pay[2] & 0xFF) << 16);
    *PtrDword = *PtrDword | ((pay[1] & 0xFF) << 8);
    *PtrDword = *PtrDword | (pay[0] & 0xFF);
    *PtrDword = *PtrDword & 0xFFFFFFFF;

    sosiMemFree (ptrData);

    gPtrLoggerScsi->logiFunctionExit ("bsdScsiGetEnclosureStatusDiagnosticsPageSegment  (Status = %x) ", status);
    return (status);

}

