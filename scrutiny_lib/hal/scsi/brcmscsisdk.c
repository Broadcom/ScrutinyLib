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
#include "atlas.h"

SCRUTINY_STATUS bsdiDiscoverDevices()
{

    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    gPtrLoggerScsi->logiFunctionEntry ("bsdiDiscoverDevices()");

    /* First check if we need to do Inband or Serial device discovery */


        if (gPtrScrutinyDeviceManager->DiscoveryFlag == SCRUTINY_DISCOVERY_TYPE_SERIAL_DEBUG)
        {
            status = sdbiConnectSdb (gPtrScrutinyDeviceManager->PtrDiscoveryParams);

        gPtrLoggerScsi->logiFunctionExit ("bsdiDiscoverDevices(SDB Status=%x)", status);

        return (status);

    }

    /* We will have to do SCSI device, if they are missed in MPT */
    sgiLocateScsiDevices();

    gPtrLoggerScsi->logiFunctionExit ("bsdiDiscoverDevices()");

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS bsdiQualifyBroadcomScsiDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status;

    gPtrLoggerScsi->logiFunctionEntry ("bsdiQualifyBroadcomScsiDevice()");

    /* Check if it is Expander device */

    status = edmiIsBroadcomExpander (PtrDevice);

    if (SCRUTINY_STATUS_SUCCESS == status)
    {
        gPtrLoggerScsi->logiFunctionExit ("bsdiQualifyBroadcomScsiDevice (Expander Status=%x)", status);
		
        return (status);
    }

    /* Check if it is Switch device */

    status = sdmiIsBroadcomSwitch (PtrDevice);

    gPtrLoggerScsi->logiFunctionExit ("bsdiQualifyBroadcomScsiDevice (Switch Status=%x)", status);
    return (status);

}

SCRUTINY_STATUS bsdiMemoryRead32 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __OUT__ PU32 PtrData, __IN__ U32 SizeInBytes)
{

    U32 index, offset;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    gPtrLoggerScsi->logiFunctionEntry ("bsdiMemoryRead32 (PtrDevice=%x, Address=%x, PtrData=%x, SizeInBytes=%x)",
                                          PtrDevice != NULL, Address, PtrData != NULL, SizeInBytes);

    /* Check if we have SDB or SCSI Depending on that appropriately call the memory reads */
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {

        /* We have SDB interface, so issue SDB interface call */
        for (index = 0, offset = 0; index < (SizeInBytes / 4); index++, offset += 4)
        {
            status = sdbiMemoryRead32 (&PtrDevice->Handle.SdbHandle, (Address + offset), &PtrData[index]);

            if (status)
            {
                break;
            }
        }

    }

    else if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_SCSI)
    {
        /* We have a SCSI Device connected, issue a scsi call */
        status = bsdScsiMemoryRead32 (PtrDevice, Address, PtrData, SizeInBytes);
    }
#if !defined (OS_VMWARE)
    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_PCI)
    {

        /* We have SDB interface, so issue SDB interface call */
        for (index = 0, offset = 0; index < (SizeInBytes / 4); index++, offset += 4)
        {
            status = atlasiPciChimeToAxiReadRegister (PtrDevice, (Address + offset), &PtrData[index]);

            if (status)
            {
                break;
            }
        }

    }
#endif
    else
    {
        /* We don't support any other interfaces as of now. */
        status = SCRUTINY_STATUS_UNSUPPORTED;
    }

    if (status)
    {
        gPtrLoggerScsi->logiDebug ("Memory Read failed for Address=%x, SizeInBytes=%x, Status=%x",
                                      Address, SizeInBytes, status);
    }

    else
    {
        //gPtrLoggerScsi->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", Address, SizeInBytes);
        //gPtrLoggerScsi->logiDumpMemoryInVerbose ((PU8) PtrData, SizeInBytes, gPtrLoggerScsi);
    }

    //gPtrLoggerScsi->logiFunctionExit ("bsdiMemoryRead32 (Status=%x)", status);

    return (status);

}

SCRUTINY_STATUS bsdiMemoryRead8 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __OUT__ PU8 PtrData, __IN__ U32 SizeInBytes)
{

    U32 index;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    //gPtrLoggerScsi->logiFunctionEntry ("bsdiMemoryRead8 (PtrDevice=%x, Address=%x, PtrData=%x, SizeInBytes=%x)",
    //                                      PtrDevice != NULL, Address, PtrData != NULL, SizeInBytes);

    /* Check if we have SDB or SCSI Depending on that appropriately call the memory reads */
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        /* We have SDB interface, so issue SDB interface call */
        for (index = 0; index < SizeInBytes; index++)
        {
            status = sdbiMemoryRead8 (&PtrDevice->Handle.SdbHandle, (Address + index), &PtrData[index]);

            if (status)
            {
                break;
            }
        }

    }

    else
    {
        /* We don't support any other interfaces as of now. */
        status = SCRUTINY_STATUS_UNSUPPORTED;
    }

    if (status)
    {
        gPtrLoggerScsi->logiDebug ("Memory Read failed for Address=%x, SizeInBytes=%x, Status=%x",
                                      Address, SizeInBytes, status);
    }

    else
    {
        //gPtrLoggerScsi->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", Address, SizeInBytes);
        //gPtrLoggerScsi->logiDumpMemoryInVerbose ((PU8) PtrData, SizeInBytes, gPtrLoggerScsi);
    }

    gPtrLoggerScsi->logiFunctionExit ("bsdiMemoryRead32 (Status=%x)", status);

    return (status);

}

SCRUTINY_STATUS bsdiMemoryRead16 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __OUT__ PU16 PtrData, __IN__ U32 SizeInBytes)
{

    U32 index, offset;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    //gPtrLoggerScsi->logiFunctionEntry ("bsdiMemoryRead16 (PtrDevice=%x, Address=%x, PtrData=%x, SizeInBytes=%x)",
    //                                      PtrDevice != NULL, Address, PtrData != NULL, SizeInBytes);

    /* Check if we have SDB or SCSI Depending on that appropriately call the memory reads */
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        /* We have SDB interface, so issue SDB interface call */
        for (index = 0, offset = 0; index < (SizeInBytes / 2); index++, offset += 2)
        {
            status = sdbiMemoryRead16 (&PtrDevice->Handle.SdbHandle, (Address + offset), &PtrData[index]);

            if (status)
            {
                break;
            }
        }

    }

    else
    {
        /* We don't support any other interfaces as of now. */
        status = SCRUTINY_STATUS_UNSUPPORTED;
    }

    if (status)
    {
        gPtrLoggerScsi->logiDebug ("Memory Read failed for Address=%x, SizeInBytes=%x, Status=%x",
                                      Address, SizeInBytes, status);
    }

    else
    {
        //gPtrLoggerScsi->logiVerbose ("Memory Read Address=%x, SizeInBytes=%x", Address, SizeInBytes);
        //gPtrLoggerScsi->logiDumpMemoryInVerbose ((PU8) PtrData, SizeInBytes, gPtrLoggerScsi);
    }

    //gPtrLoggerScsi->logiFunctionExit ("bsdiMemoryRead16 (Status=%x)", status);

    return (status);

}

SCRUTINY_STATUS bsdiMemoryWrite32 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __IN__ PU32 PtrData)
{

    SCRUTINY_STATUS status;

    //gPtrLoggerScsi->logiFunctionEntry ("bsdiMemoryWrite32 (PtrDevice=%x, Address=%x, PtrData=%x)",
    //                                      PtrDevice != NULL, Address, PtrData != NULL);

    /* Check if we have SDB or SCSI Depending on that appropriately call the memory reads */
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        /* We have SDB interface, so issue SDB interface call */
        status = sdbiMemoryWrite32 (&PtrDevice->Handle.SdbHandle, Address, *PtrData);
    }

    else if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_SCSI)
    {
        /* We have a SCSI Device connected, issue a scsi call */
        status = bsdScsiMemoryWrite32 (PtrDevice, Address, PtrData, sizeof (U32));
    }
#if !defined (OS_VMWARE)
    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_PCI)
    {
        status = atlasiPciChimeToAxiWriteRegister (PtrDevice, Address, *PtrData);
    }
#endif
    else
    {
        /* We don't support any other interfaces as of now. */
        status = SCRUTINY_STATUS_UNSUPPORTED;
    }

    if (status)
    {
        gPtrLoggerScsi->logiDebug ("Memory write failed for Address=%x, Status=%x",
                                      Address, status);
    }

    else
    {
        //gPtrLoggerScsi->logiVerbose ("Memory write Address=%x, PtrData[0]=%x", Address, ((PU32) PtrData)[0]);
    }

    //gPtrLoggerScsi->logiFunctionExit ("bsdiMemoryWrite32 (Status=%x)", status);

    return (status);

}

SCRUTINY_STATUS bsdiMemoryWrite16 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __IN__ PU16 PtrData)
{

    SCRUTINY_STATUS status;

    //gPtrLoggerScsi->logiFunctionEntry ("bsdiMemoryWrite16 (PtrDevice=%x, Address=%x, PtrData=%x)",
    //                                      PtrDevice != NULL, Address, PtrData != NULL);

    /* Check if we have SDB or SCSI Depending on that appropriately call the memory reads */
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        /* We have SDB interface, so issue SDB interface call */
        status = sdbiMemoryWrite16 (&PtrDevice->Handle.SdbHandle, Address, *PtrData);
    }

    else
    {
        /* We don't support any other interfaces as of now. */
        status = SCRUTINY_STATUS_UNSUPPORTED;
    }

    if (status)
    {
        gPtrLoggerScsi->logiDebug ("Memory write failed for Address=%x, Status=%x",
                                      Address, status);
    }

    else
    {
        //gPtrLoggerScsi->logiVerbose ("Memory write Address=%x, PtrData=%x", Address, *PtrData);
    }

    //gPtrLoggerScsi->logiFunctionExit ("bsdiMemoryWrite16 (Status=%x)", status);

    return (status);

}

SCRUTINY_STATUS bsdiMemoryWrite8 (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Address, __IN__ PU8 PtrData)
{

    SCRUTINY_STATUS status;

    //gPtrLoggerScsi->logiFunctionEntry ("bsdiMemoryWrite8 (PtrDevice=%x, Address=%x, PtrData=%x)",
    //                                   PtrDevice != NULL, Address, PtrData != NULL);

    /* Check if we have SDB or SCSI Depending on that appropriately call the memory reads */
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        /* We have SDB interface, so issue SDB interface call */
        status = sdbiMemoryWrite8 (&PtrDevice->Handle.SdbHandle, Address, *PtrData);
    }

    else
    {
        /* We don't support any other interfaces as of now. */
        status = SCRUTINY_STATUS_UNSUPPORTED;
    }

    if (status)
    {
        gPtrLoggerScsi->logiDebug ("Memory write failed for Address=%x, Status=%x",
                                      Address, status);
    }

    else
    {
        //gPtrLoggerScsi->logiVerbose ("Memory write Address=%x, PtrData=%x", Address, *PtrData);
    }

    //gPtrLoggerScsi->logiFunctionExit ("bsdiMemoryWrite8 (Status=%x)", status);

    return (status);

}

SCRUTINY_STATUS bsdiPerformScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest)
{

    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;
    //SCRUTINY_IOC_STATUS  iocStatus = { 0 };

    gPtrLoggerScsi->logiFunctionEntry ("bsdiPerformScsiPassthrough (PtrDevice=%x, PtrScsiRequest=%x)",
                                         PtrDevice != NULL, PtrScsiRequest != NULL);

    /*
     * Check what device we are sending to. If we are having MPT Interface, we need to send with the device ID as a parameter.
     * Otherwise we can just send to the SCSI Device.
     */

    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC)
    {
        status = sgiPerformScsiPassthrough (PtrDevice, PtrScsiRequest);
    }
	#if defined (LIB_SUPPORT_CONTROLLER)
    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE)
    {
        status = mptiPerformScsiPassthrough (PtrDevice->Handle.ScsiHandle.PtrMpiAdapterDevice,
                                             PtrDevice->Handle.ScsiHandle.MpiSasDeviceHandle,
                                             PtrScsiRequest,
                                             &iocStatus);
    }

    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        status = mfiPerformScsiPassthrough (PtrDevice->Handle.ScsiHandle.PtrMpiAdapterDevice,
                                            PtrDevice->Handle.ScsiHandle.MpiSasDeviceHandle,
                                            PtrScsiRequest,
                                            &iocStatus);
    }
	#endif
    else
    {
        status = SCRUTINY_STATUS_UNSUPPORTED;
    }

    gPtrLoggerScsi->logiFunctionExit ("bsdiPerformScsiPassthrough (Status=%x)", status);

    return (status);

}

SCRUTINY_STATUS bsdScsiMemoryRead32 (__IN__ PTR_SCRUTINY_DEVICE PtrExpander, __IN__ U32 Address, __OUT__ PU32 PtrValue, __IN__ U32 SizeInBytes)
{

    SCRUTINY_STATUS status ;
    PTR_SCRUTINY_SCSI_PASSTHROUGH ptrScsiRequest = NULL;

    ptrScsiRequest = (PTR_SCRUTINY_SCSI_PASSTHROUGH) sosiMemAlloc (sizeof (SCRUTINY_SCSI_PASSTHROUGH));

    ptrScsiRequest->CdbLength = 10;
    ptrScsiRequest->PtrDataBuffer = (PVOID) sosiMemAlloc (SizeInBytes);
    ptrScsiRequest->DataBufferLength = SizeInBytes;

    *((PU32) ptrScsiRequest->PtrDataBuffer) = 0xDEADBEEF;

    ptrScsiRequest->Cdb[0x00]  = SCSI_COMMAND_READ_BUFFER;
    ptrScsiRequest->Cdb[0x01]  = SCSI_READ_MODE_VENDOR_SPECIFIC;
    ptrScsiRequest->Cdb[0x02]  = BRCM_SCSI_BUFFER_ID_MEMORY_RW_DWORD;
    ptrScsiRequest->Cdb[0x03]  = (U8) ((Address >> 24) & 0xFF);
    ptrScsiRequest->Cdb[0x04]  = (U8) ((Address >> 16) & 0xFF);
    ptrScsiRequest->Cdb[0x05]  = (U8) ((Address >>  8) & 0xFF);
    ptrScsiRequest->Cdb[0x06]  = (U8) ((Address      ) & 0xFF);
    ptrScsiRequest->Cdb[0x07]  = (U8) ((SizeInBytes >> 8) & 0xFF);
    ptrScsiRequest->Cdb[0x08]  = (U8) ((SizeInBytes & 0xFF));
    ptrScsiRequest->Cdb[0x09]  = 0x00;

    ptrScsiRequest->DataDirection = DIRECTION_READ;

    /* Copy the data buffer into the memory. */

    status = bsdiPerformScsiPassthrough (PtrExpander, ptrScsiRequest);

    if (SCRUTINY_STATUS_SUCCESS == status)
    {
        sosiMemCopy (PtrValue, ptrScsiRequest->PtrDataBuffer, SizeInBytes);
    }

    sosiMemFree (ptrScsiRequest->PtrDataBuffer);
    sosiMemFree (ptrScsiRequest);

    return (status);

}

SCRUTINY_STATUS bsdScsiMemoryWrite32 (__IN__ PTR_SCRUTINY_DEVICE PtrExpander, __IN__ U32 Address, __IN__ PU32 PtrValue, __IN__ U32 SizeInBytes)
{

    SCRUTINY_STATUS status;

    SCRUTINY_SCSI_PASSTHROUGH scsiRequest = { 0 };

    scsiRequest.CdbLength = 10;
    scsiRequest.PtrDataBuffer = (PVOID) sosiMemAlloc (SizeInBytes);
    scsiRequest.DataBufferLength = SizeInBytes;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_WRITE_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_READ_MODE_VENDOR_SPECIFIC;
    scsiRequest.Cdb[0x02]  = BRCM_SCSI_BUFFER_ID_MEMORY_RW_DWORD;
    scsiRequest.Cdb[0x03]  = (U8) ((Address >> 24) & 0xFF);
    scsiRequest.Cdb[0x04]  = (U8) ((Address >> 16) & 0xFF);
    scsiRequest.Cdb[0x05]  = (U8) ((Address >>  8) & 0xFF);
    scsiRequest.Cdb[0x06]  = (U8) ((Address)  & 0xFF);
    scsiRequest.Cdb[0x07]  = (U8) ((SizeInBytes >> 8) & 0xFF);
    scsiRequest.Cdb[0x08]  = (U8) ((SizeInBytes & 0xFF));
    scsiRequest.Cdb[0x09]  = 0x00;

    scsiRequest.DataDirection = DIRECTION_WRITE;

    sosiMemCopy (scsiRequest.PtrDataBuffer, PtrValue, SizeInBytes);

    status = bsdiPerformScsiPassthrough (PtrExpander, &scsiRequest);

    sosiMemFree (scsiRequest.PtrDataBuffer);

    return (status);

}

/**
 *
 *  @method  bsdGetFirmwareRegionVersion()
 *
 *  @param   PtrExpander    Pointer to Expander device
 *
 *  @return  STATUS_SUCCESS if the firmware version has been assigned
 *           successfully otherwise STATUS_FAILED will be returned.
 *
 *  @brief   This method will assign the FW version to the exapander
 *
 */

SCRUTINY_STATUS bsdGetFirmwareRegionVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 Region, __OUT__ PU32 PtrVersion)
{

    U32 sizeInBytes = 256;
    SCRUTINY_STATUS status;
    SCRUTINY_SCSI_PASSTHROUGH scsiRequest = { 0 };

    scsiRequest.CdbLength = 10;
    scsiRequest.PtrDataBuffer = (PVOID) sosiMemAlloc (sizeInBytes);
    scsiRequest.DataBufferLength = sizeInBytes;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_READ_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_READ_MODE_VENDOR_SPECIFIC;
    scsiRequest.Cdb[0x02]  = Region;
    scsiRequest.Cdb[0x03]  = 0x00;
    scsiRequest.Cdb[0x04]  = 0x00;
    scsiRequest.Cdb[0x05]  = 0x00;
    scsiRequest.Cdb[0x06]  = 0x00;
    scsiRequest.Cdb[0x07]  = 0x01;
    scsiRequest.Cdb[0x08]  = 0x00;
    scsiRequest.Cdb[0x09]  = 0x00;

    scsiRequest.DataDirection = DIRECTION_READ;

    /* Copy the data buffer into the memory. */

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    if (SCRUTINY_STATUS_SUCCESS == status)
    {

        //gPtrLoggerController->logiDebug ("Dumping the Read Buffer Data %x:", sizeInBytes);
        //gPtrLoggerController->logiDumpMemoryInVerbose (scsiRequest.PtrDataBuffer, sizeInBytes, gPtrLoggerController);

        *PtrVersion = ((PU32) scsiRequest.PtrDataBuffer)[0x9];
        sosiMemFree (scsiRequest.PtrDataBuffer);
    }

    return (status);

}

SCRUTINY_STATUS bsdiGetCurrentFirmwareVersion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrVersion)
{

    SCRUTINY_STATUS status;
    U32 version = 0xFFFFFFFF;
    U32 index;

    U8 region[] = { BRCM_SCSI_BUFFER_ID_RUNNING_FIRMWARE,
                    BRCM_SCSI_BUFFER_ID_VIRTUAL_ACTIVE_1,
                    BRCM_SCSI_BUFFER_ID_VIRTUAL_ACTIVE_2,
                    BRCM_SCSI_BUFFER_ID_REGION_BOOTLOADER };

    gPtrLoggerScsi->logiFunctionEntry ("bsdiGetCurrentFirmwareVersion (PtrDevice=%x)",
                                          PtrDevice != NULL);

    for (index = 0; index < 4; index ++)
    {
        status = bsdGetFirmwareRegionVersion (PtrDevice, region[index], &version);

        if ((version != 0xFFFFFFFF) && (version != 0x00000000) && (status == SCRUTINY_STATUS_SUCCESS))
        {
            *PtrVersion = version;
            gPtrLoggerScsi->logiDebug ("Broadcom SCSI Device - Firmware Version=%x", version);

            return (SCRUTINY_STATUS_SUCCESS);
        }

    }

    /* We couldn't get the version, just return as is */
    *PtrVersion = 0x0;

    gPtrLoggerScsi->logiFunctionExit ("bsdiGetCurrentFirmwareVersion (Status=Failed)");

    return (SCRUTINY_STATUS_FAILED);

}

/*
 *
 *  @method  cobiGetRegionSize()
 *
 *  @param   PtrExpander        pointer to Expander device
 *
 *  @param   BufferId           Buffer id
 *
 *  @param   PtrSize            The region size
 *
 *  @return  Status             '0' for success and  non-zero for failure
 *
 *  @brief                      This method will give the regionsize of the exapander
 *
 */

SCRUTINY_STATUS bsdiGetRegionSize (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 BufferId, __OUT__ PU32 PtrSize)
{

    U32 sizeInBytes;
    SCRUTINY_STATUS status;
    SCRUTINY_SCSI_PASSTHROUGH scsiRequest = { 0 };

    gPtrLoggerScsi->logiFunctionEntry ("bsdiGetRegionSize (PtrDevice=%x, BufferId=%x)",
                                          PtrDevice != NULL, BufferId);

    scsiRequest.CdbLength = 10;
    scsiRequest.PtrDataBuffer = (PVOID) &sizeInBytes;
    scsiRequest.DataBufferLength = sizeof (U32);
    scsiRequest.DataDirection = DIRECTION_READ;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_READ_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_READ_MODE_DESCRIPTOR;
    scsiRequest.Cdb[0x02]  = BufferId;
    scsiRequest.Cdb[0x03]  = 0x00;
    scsiRequest.Cdb[0x04]  = 0x00;
    scsiRequest.Cdb[0x05]  = 0x00;
    scsiRequest.Cdb[0x06]  = 0x00;
    scsiRequest.Cdb[0x07]  = 0x00;
    scsiRequest.Cdb[0x08]  = 0x04;
    scsiRequest.Cdb[0x09]  = 0x00;

    /* Copy the data buffer into the memory. */

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerScsi->logiFunctionExit ("bsdiGetRegionSize (Status=%x)", status);
        return (status);
    }

    /*
     * Descriptor mode size will return 24 bytes.
     */

    sizeInBytes = SWAP_ENDIANNESS (sizeInBytes);
    *PtrSize = sizeInBytes;

    gPtrLoggerScsi->logiFunctionExit ("bsdiGetRegionSize (PtrDevice=%x, BufferId=%x, Size=%x)",
                                          PtrDevice != NULL, BufferId, sizeInBytes);

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS bsdiGetRegion (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 BufferId, __OUT__ PU8 *PtrBuffer, __OUT__ PU32 PtrRegionSize)
{

    U32 sizeTotal = 0, chunkSize = 0;
    U32 offset = 0;
    PU8 ptrBuffer = NULL;
    SCRUTINY_STATUS status;
    SCRUTINY_SCSI_PASSTHROUGH scsiRequest = { 0 };

    gPtrLoggerScsi->logiFunctionEntry ("bsdiGetRegion (PtrDevice=%x, BufferId=%x)",
                                          PtrDevice != NULL, BufferId);

    *PtrBuffer = NULL;

    if (bsdiGetRegionSize (PtrDevice, BufferId, &sizeTotal) != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerScsi->logiFunctionExit ("bsdiGetRegion (Region Size Failed)");
        return (SCRUTINY_STATUS_FAILED);
    }

    ptrBuffer = (PU8) sosiMemAlloc (sizeTotal);
    *PtrRegionSize = sizeTotal;

    if (ptrBuffer == NULL)
    {
        gPtrLoggerScsi->logiFunctionExit ("bsdiGetRegion (Memory Allocation)");
        return (SCRUTINY_STATUS_FAILED);
    }

    scsiRequest.CdbLength = 10;
    scsiRequest.DataDirection = DIRECTION_READ;

    scsiRequest.Cdb[0] = SCSI_COMMAND_READ_BUFFER;
    scsiRequest.Cdb[1] = SCSI_READ_MODE_DATA;
    scsiRequest.Cdb[2] = BufferId;
    scsiRequest.Cdb[9] = 0;

    scsiRequest.DataDirection = DIRECTION_READ;

    while (sizeTotal)
    {
        if (sizeTotal < EXPANDER_SCSI_GENERIC_CHUNK_SIZE)
        {
            chunkSize = sizeTotal;
        }

        else
        {
            chunkSize = EXPANDER_SCSI_GENERIC_CHUNK_SIZE;
        }

        scsiRequest.PtrDataBuffer = (PVOID) &ptrBuffer[offset];
        scsiRequest.DataBufferLength = chunkSize;

        scsiRequest.Cdb[3] = (U8) (offset >> 16);
        scsiRequest.Cdb[4] = (U8) (offset >> 8);
        scsiRequest.Cdb[5] = (U8) (offset & 0xff);
        scsiRequest.Cdb[6] = (U8) (chunkSize >> 16);
        scsiRequest.Cdb[7] = (U8) (chunkSize >> 8);
        scsiRequest.Cdb[8] = (U8) (chunkSize & 0xFF);

        status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

        if (status)
        {

            gPtrLoggerScsi->logiDebug ("BRCM SCSI Get region failed. BufferId=%x, Offset=%x, ChunkSize=%x, TotalSize=%x, Status=%x, Scsistatus=%x.",
                                         BufferId, offset, chunkSize, *PtrRegionSize, status, scsiRequest.ScsiStatus);

            gPtrLoggerScsi->logiFunctionExit ("bsdiGetRegion (Scsi Failed=%x)");

            sosiMemFree (ptrBuffer);
            return (status);
        }

        sizeTotal = sizeTotal - chunkSize;
        offset = offset + chunkSize;

    }

    *PtrBuffer = ptrBuffer;

    gPtrLoggerScsi->logiFunctionExit ("bsdiGetRegion (Status=Success, Size=%x)", *PtrRegionSize);

    return (SCRUTINY_STATUS_SUCCESS);

}

/*
 *
 *  @method  bsdiDownloadRegion()
 *
 *  @param   PtrExpander        pointer to Expander device
 *
 *  @param   BufferId           Buffer id
 *
 *  @param   PtrSize            The region size
 *
 *  @return  Status             '0' for success and  non-zero for failure
 *
 *  @brief                      This method will download the buffer to the flash region
 *
 */

SCRUTINY_STATUS bsdiDownloadRegion (
                          __IN__ PTR_SCRUTINY_DEVICE PtrDevice, 
                          __IN__ U8 BufferId, 
                          __IN__ PU8 PtrBuffer, 
                          __IN__ U32 BufferSize)
{
    SCRUTINY_STATUS                 status;
    SCRUTINY_SCSI_PASSTHROUGH       scsiRequest = { 0 };
    PU8 ptrTemp = NULL;
    U32 offset = 0;
    U32 chunkSize = (256 * 1024);

    gPtrLoggerGeneric->logiFunctionEntry ("bsdiDownloadRegion (BufferId=0x%x, PtrBuffer=0x%x, BufferSize=0x%x)", 
                                          BufferId, PtrBuffer, BufferSize);

    scsiRequest.CdbLength = 10;
    
    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_WRITE_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_WRITE_MODE_WRITE_DATA;
    scsiRequest.Cdb[0x02]  = BufferId;
    scsiRequest.MaxIoTimeout = 10;
    scsiRequest.Cdb[0x09]  = 0x00;

    ptrTemp = PtrBuffer;
    scsiRequest.DataDirection = DIRECTION_WRITE;

    while (BufferSize)
    {
        if (BufferSize < chunkSize)
        {
            chunkSize = BufferSize;
        }

        scsiRequest.Cdb[0x03]  = (U8) ((offset >> 16) & 0xFF);;
        scsiRequest.Cdb[0x04]  = (U8) ((offset >> 8) & 0xFF);
        scsiRequest.Cdb[0x05]  = (U8) (offset & 0xFF);
        scsiRequest.Cdb[0x06]  = (U8) ((chunkSize >> 16) & 0xFF);
        scsiRequest.Cdb[0x07]  = (U8) ((chunkSize >> 8) & 0xFF);
        scsiRequest.Cdb[0x08]  = (U8) (chunkSize & 0xFF);
        

        scsiRequest.PtrDataBuffer = (PVOID) ptrTemp;
        scsiRequest.DataBufferLength = chunkSize;

        status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerGeneric->logiFunctionExit ("bsdiDownloadRegion (chunkSize=0x%x, offset=0x%x)", 
                                          chunkSize, offset);
            return (status);
        }

        offset = offset + chunkSize;
        BufferSize = BufferSize - chunkSize;
        ptrTemp = ptrTemp + chunkSize;
    }

    gPtrLoggerGeneric->logiFunctionExit("bsdiDownloadRegion ()");
    return (SCRUTINY_STATUS_SUCCESS);
}


/**
 *
 *  @method  cobiGetConfigPage()
 *
 *  @param   PtrExpander        pointer to Expander device
 *
 *  @param   Page               The config page no.
 *
 *  @param   Region             Region from where to get the page
 *
 *  @param   PtrValue           Pointer to value to be written
 *
 *  @param   PtrSize            size of config page
 *
 *  @return  Status             '0' for success and  non-zero for failure
 *
 *  @brief                      This method will give configpage of the expander
 *
 */

SCRUTINY_STATUS bsdiGetConfigPage (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U16 Page, __IN__ U8 Region, __OUT__ PU8 *PtrValue, __OUT__ PU32 PtrSize)
{

    U32             size = 0;
    U8              cfgHeader[4]={0};
    PU8             ptrTemp;
    SCRUTINY_STATUS status;
    SCRUTINY_SCSI_PASSTHROUGH scsiRequest = { 0 };

    gPtrLoggerScsi->logiFunctionEntry ("bsdiGetConfigPage (PtrDevice=%x, Page=%x, Region=%x)",
                                          PtrDevice != NULL, Page, Region);

    scsiRequest.CdbLength = 10;
    scsiRequest.PtrDataBuffer = (PVOID) &cfgHeader;
    scsiRequest.DataBufferLength = sizeof (U32);
    scsiRequest.DataDirection = DIRECTION_READ;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_READ_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_READ_MODE_VENDOR_SPECIFIC;
    scsiRequest.Cdb[0x02]  = 0xE6;                            /* Buffer ID for read/write config pages */
    scsiRequest.Cdb[0x03]  = (U8) ((Page >> 8) & 0xFF);
    scsiRequest.Cdb[0x04]  = (U8) ((Page     ) & 0xFF);
    scsiRequest.Cdb[0x05]  = 0x00;
    scsiRequest.Cdb[0x06]  = 0x04;
    scsiRequest.Cdb[0x07]  = Region;
    scsiRequest.Cdb[0x08]  = 0x00;
    scsiRequest.Cdb[0x09]  = 0x00;

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerScsi->logiDebug ("BRCM SCSI CFG Request Failed(Header). Page=%x, Region=%x, Status=%x, Scsistatus=%x.",
                                     Page, Region, status, scsiRequest.ScsiStatus);
        gPtrLoggerScsi->logiFunctionExit ("bsdiGetConfigPage (SCSI/Header Status=%x)", status);

        return (SCRUTINY_STATUS_FAILED);
    }

    scsiRequest.Cdb[0x05] = cfgHeader[1];
    scsiRequest.Cdb[0x06] = cfgHeader[0];

    size = cfgHeader[1] << 8 | cfgHeader[0];
    ptrTemp = (PU8) sosiMemAlloc (size);

    if (ptrTemp == NULL)
    {
        gPtrLoggerScsi->logiFunctionExit ("bsdiGetConfigPage (Memory/Alloc Size=%x)", size);
        return (SCRUTINY_STATUS_FAILED);
    }

    scsiRequest.PtrDataBuffer = ptrTemp;
    scsiRequest.DataBufferLength = size;

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerScsi->logiDebug ("BRCM SCSI CFG Request Failed. Page=%x, Region=%x, TotalSize=%x, Status=%x, Scsistatus=%x.",
                                      Page, Region, size, status, scsiRequest.ScsiStatus);
        sosiMemFree (ptrTemp);

        gPtrLoggerScsi->logiFunctionExit ("bsdiGetConfigPage (SCSI/Page Status=%x)", status);

        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrValue = ptrTemp;
    *PtrSize = size;

    gPtrLoggerScsi->logiFunctionExit ("bsdiGetConfigPage (Status=Success, Size=%x)", size);

    return (SCRUTINY_STATUS_SUCCESS);

}

