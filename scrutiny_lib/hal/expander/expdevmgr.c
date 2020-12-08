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
#include "expdevmgr.h"

SCRUTINY_STATUS edmiResetDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS         status = SCRUTINY_STATUS_SUCCESS;
    U32 dword = 0;
    
    gPtrLoggerExpanders->logiFunctionEntry ("edmiResetDevice (PtrDevice=%x)", PtrDevice != NULL);

    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        return (edmiScsiResetDevice (PtrDevice));
    }
                                   
    /* We can generate a reset */    
    /* Reset the expander, the ARM restarts from the reset vector
     * The EDS describes this reset as:
     * Write the value 0x0C0B_080A to this register to initiate a hardware
     * reset of the expander.
     */

    dword = 0x0C0B080A;
    status= bsdiMemoryWrite32 (PtrDevice, 0xC3800200, &dword);//REGISTER_ADDRESS_COBRA_RESET_EXPANDER=0xC3800200
   
    
    if (status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmiResetDevice (Memory Write Reset Exp Status=%x)", status);
        return (status);
    }
    
    sosiSleep (500) ; // sleep 500 mini sec

    gPtrLoggerExpanders->logiFunctionExit ("edmiResetDevice ( Status=%x)", status);
    return (status);
    

}

SCRUTINY_STATUS edmiScsiResetDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{
    U8 data[4];
    U8 SizeInBytes = 4;
    SCRUTINY_STATUS status;

    SCRUTINY_SCSI_PASSTHROUGH scsiRequest = { 0 };

    gPtrLoggerExpanders->logiFunctionEntry ("edmiScsiResetDevice (PtrDevice=%x)", PtrDevice != NULL);

    scsiRequest.CdbLength = 10;
    scsiRequest.PtrDataBuffer = (PVOID) sosiMemAlloc (SizeInBytes);
    scsiRequest.DataBufferLength = SizeInBytes;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_WRITE_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_READ_MODE_VENDOR_SPECIFIC;
    scsiRequest.Cdb[0x02]  = BRCM_SCSI_BUFFER_ID_CHIP_RESET;
    scsiRequest.Cdb[0x03]  = 0x00;
    scsiRequest.Cdb[0x04]  = 0x00;
    scsiRequest.Cdb[0x05]  = 0x00;
    scsiRequest.Cdb[0x06]  = 0x00;
    scsiRequest.Cdb[0x07]  = 0x00;
    scsiRequest.Cdb[0x08]  = 0x03;
    scsiRequest.Cdb[0x09]  = 0x00;

    scsiRequest.DataDirection = DIRECTION_WRITE;

    data[0x0] = 0x00;    /* 0 for Expander, 1 for watch dog */
    data[0x1] = 0x01;    /* time delay before reset */
    data[0x2] = 0x00;
    data[0x3] = 0x00;


    sosiMemCopy (scsiRequest.PtrDataBuffer, data, SizeInBytes);

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    sosiMemFree (scsiRequest.PtrDataBuffer);

    gPtrLoggerExpanders->logiFunctionExit ("edmiScsiResetDevice ( Status=%x)", status);
    return (status);


}


SCRUTINY_STATUS edmiCloseDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    gPtrLoggerExpanders->logiDebug ("Close Expander Device HandleType=%x", PtrDevice->HandleType);

#if defined (OS_LINUX) && !defined (OS_VMWARE)
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        /* We need to close the serial port. */
        spiClosePort (&PtrDevice->Handle.SdbHandle);
    }

#endif

    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC)
    {
        /* We have got SCSI Generic interface */
        sgiCloseDevice (PtrDevice);
    }

    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
             PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        /* We don't have anything to close on this interface. */
    }

    else
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    return (SCRUTINY_STATUS_SUCCESS);
}


SCRUTINY_STATUS edmiIsBroadcomExpander (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status;

    gPtrLoggerExpanders->logiDebug ("Qualify IsBroadcom Expander Device - HandleType=%x", PtrDevice->HandleType);


    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC)
    {
        /* We can check for Cobra/Cub as of now we don't have SMP interface support on the same. */
        status = edmQualifyExpanderOnScsiGenericInterface (PtrDevice);
    }

#if !defined (OS_VMWARE)

    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        /* We have to do qualify on the SDB Interface which means we will be able to read the
         * device specific registers irrespective of the control
         */
        status = edmQualifyExpanderOnSdbInterface (PtrDevice);
    }

#endif

    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
             PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        /* We have SMP support here. Do the qualification on the SMP support */
        status = edmQualifyExpanderOnSmpInterface (PtrDevice);

        if (status)
        {
        	//Pravin - Commenting it out, dont want to go via SG interface. We have a seperate method to the same
            //status = edmQualifyExpanderOnScsiGenericInterface (PtrDevice);
        }

    }

    else
    {
        /*
         * We don't know this interface yet, so just ignore this. We are not aware of this
         * interface for expander
         */

        status = SCRUTINY_STATUS_IGNORE;

    }

    gPtrLoggerExpanders->logiDebug ("Qualify IsBroadcom Expander Device - Status=%x", status);

    return (status);

}

SCRUTINY_STATUS edmQualifyExpanderOnSdbInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status;

    status = cobiRegisterBasedQualifyExpander (PtrDevice);

    if (SCRUTINY_STATUS_SUCCESS == status)
    {
        return (status);
    }

    /* We couldn't qualify the expander for Cub/Cobra. Thus check if it is Margay */

    status = margayiRegisterBasedQualifyExpander (PtrDevice);

    return (status);

}

SCRUTINY_STATUS edmQualifyExpanderOnScsiGenericInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status;
    U32 dword;

    gPtrLoggerExpanders->logiFunctionEntry ("edmQualifyExpanderOnScsiGenericInterface (PtrDevice=%x)", PtrDevice != NULL);

    /*
     * NOTE: This check is very important.
     *
     * Atlas and Cobra has two different chip id registers. These two addresses are
     * reserved for each other if we use vice versa. Using that, causing Expander/Switch
     * to raise WatchDog reset. In-order to avoid that, I came up using the flash signature.
     *
     * If incase of Atlas, the Base Address flash will not have bootloader. It has the flash
     * table chain image. Thus we will not have ARM branch instruction. Incase of Cobra,
     * Bootloader is must for communicating on the SG. So, there will be firmware always.
     *
     */

    status = bsdiMemoryRead32 (PtrDevice, REGISTER_ADDRESS_COBRA_FLASH_START, &dword, sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnScsiGenericInterface (MemoryRead Failed=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    if ((dword & 0xFFFFFF00) != 0xEA000000)
    {
        gPtrLoggerExpanders->logiVerbose ("Possibly Atlas on Cobra check identified and ignored %x", dword);
        gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnScsiGenericInterface (AtlasCheck Ignored)");
        return (SCRUTINY_STATUS_IGNORE);
    }

    /*
     * We have option only to detect Cub/Cobra expander. Margay is not supported as of now. Hence
     * Cub/Cobra supports Memory Read/Write interface for the SCSI as well.
     */

    status = cobiRegisterBasedQualifyExpander (PtrDevice);

    if (status)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnScsiGenericInterface (Qualify Failed=%x)", status);
        /* We don't know what device it is. So just return ignore */
        return (SCRUTINY_STATUS_IGNORE);
    }

    /* We can set the expander Host PCI Sas Address */

    PtrDevice->DeviceInfo.u.ExpanderInfo.HostPciAddress = PtrDevice->Handle.ScsiHandle.AdapterPCIAddress;
	PtrDevice->HandleType = SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC;

    gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnScsiGenericInterface (status=%x)", status);

    return (status);

}

SCRUTINY_STATUS edmiPerformSmpPassthrough (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrSMPRequest, __IN__ U32 SMPRequestSize, __OUT__ PU8 PtrSMPResponse, __IN__ U32 SMPResponseSize)
{

    SCRUTINY_SMP_PASSTHROUGH smpRequest;
    U32 index;
    //SCRUTINY_IOC_STATUS iocStatus;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    gPtrLoggerExpanders->logiFunctionEntry ("edmiPerformSmpPassthrough (PtrDevice=%x)", PtrDevice != NULL);

    gPtrLoggerExpanders->logiDebug("Expander SMP Passthrough request HandleType=%x, SMPFunction=%x, RequestSize=%x", PtrDevice->HandleType, PtrSMPRequest[1], SMPRequestSize);

    if (PtrDevice->HandleType != SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE &&
        PtrDevice->HandleType != SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmiPerformSmpPassthrough (Unsupported Handle)");
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    if (SMPRequestSize > 1024)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmiPerformSmpPassthrough (Invalid Request Length %x)", SMPRequestSize);
        return (SCRUTINY_STATUS_FAILED);
    }

    sosiMemSet (&smpRequest, 0, sizeof (SCRUTINY_SMP_PASSTHROUGH));

    sosiMemCopy (smpRequest.RequestData, PtrSMPRequest, SMPRequestSize);

    smpRequest.Size = sizeof (SCRUTINY_SMP_PASSTHROUGH);
    smpRequest.RequestDataLength = (U16) SMPRequestSize;

    smpRequest.SASAddress.High = PtrDevice->Handle.ScsiHandle.MpiSMPEnclosureWWID.High;
    smpRequest.SASAddress.Low  = PtrDevice->Handle.ScsiHandle.MpiSMPEnclosureWWID.Low;

    smpRequest.ResponseDataLength = (U16) SMPResponseSize;

    /* We will now check which one we will have. */
	#if defined (LIB_SUPPORT_CONTROLLER)
    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE)
    {
        status = mptiPerformSmpPassthrough (PtrDevice->Handle.ScsiHandle.PtrMpiAdapterDevice, &smpRequest, &iocStatus);
    }

    else if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        status = mfiPerformSmpPassthrough (PtrDevice->Handle.ScsiHandle.PtrMpiAdapterDevice, &smpRequest, &iocStatus);
    }
	#endif

    gPtrLoggerExpanders->logiDebug ("Expander SMP Request status %x", status);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmiPerformSmpPassthrough (Passthrough failed %x)", status);
        return (status);
    }

    sosiMemCopy (PtrSMPResponse, smpRequest.ResponseData, smpRequest.ResponseDataLength);

    /* We need to swap the bytes */

    for (index = 0; index < (U32) (smpRequest.ResponseDataLength / 4); index++)
    {
        ((PU32) PtrSMPResponse)[index] = swap32 (((PU32) PtrSMPResponse)[index]);
    }

    gPtrLoggerExpanders->logiFunctionExit ("edmiPerformSmpPassthrough (Status=0)");

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS edmQualifyExpanderOnSmpInterface (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;
    U32 size = sizeof (SMP_REPORT_MANUFACTURER_INFO_RESPONSE);

    SMP_REQUEST smpMfgRequest;
    SMP_REPORT_MANUFACTURER_INFO_RESPONSE smpMfgResponse;

    SMP_REQUEST smpReportRequest;
    SMP_REPORT_GENERAL_RESPONSE smpReportResponse;

    gPtrLoggerExpanders->logiFunctionEntry ("edmQualifyExpanderOnSmpInterface (PtrDevice=%x)", PtrDevice != NULL);

    sosiMemSet (&smpMfgRequest, 0, sizeof (SMP_REQUEST));
    sosiMemSet (&smpMfgResponse, 0, sizeof (SMP_REPORT_MANUFACTURER_INFO_RESPONSE));
    sosiMemSet (&smpReportRequest, 0, sizeof (SMP_REQUEST));
    sosiMemSet (&smpReportResponse, 0, sizeof (SMP_REPORT_GENERAL_RESPONSE));

    smpMfgRequest.SMPFrameType     = SMP_FRAME_TYPE_SMP_REQUEST;
    smpMfgRequest.Function         = SMP_FUNCTION_REPORT_MANUFACTURER_INFORMATION;

    size = sizeof (SMP_REPORT_MANUFACTURER_INFO_RESPONSE);

    status = edmiPerformSmpPassthrough (PtrDevice, (PU8) &smpMfgRequest, sizeof (SMP_REPORT_MANUFACTURER_INFO_REQUEST), (PU8) &smpMfgResponse, size);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnSmpInterface (SMPPassthrough Status=%x)", status);
        return (SCRUTINY_STATUS_IGNORE);
    }

    if (!smpMfgResponse.Header.Fields.Function || smpMfgResponse.Header.Fields.FunctionResult)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnSmpInterface (SMP Function Failed=%x, Result=%x)", smpMfgResponse.Header.Fields.Function, smpMfgResponse.Header.Fields.FunctionResult);
        return (SCRUTINY_STATUS_IGNORE);
    }

    gPtrLoggerExpanders->logiDebug ("Expander Qualify SMP Mfg Response ComponentId=%x, ComponentRev=%x, VendorId=%x", smpMfgResponse.Response.Fields.ComponentIdent,
                                  smpMfgResponse.Response.Fields.ComponentRevLevel,
                                  smpMfgResponse.Response.Fields.ComponentVendorIdent);

    status = margayiQualifyMargayChipSignature (PtrDevice, smpMfgResponse.Response.Fields.ComponentIdent, smpMfgResponse.Response.Fields.ComponentRevLevel);

    gPtrLoggerExpanders->logiDebug ("Expander SMP based Margay Qualification Status=%x", status);

    if (status)
    {
        /* Check if we have the Cub/Cobra */

        status = cobiQualifyCubCobraChipSignature (PtrDevice, smpMfgResponse.Response.Fields.ComponentIdent, smpMfgResponse.Response.Fields.ComponentRevLevel);

        gPtrLoggerExpanders->logiDebug ("Expander SMP based Cub/Cobra Qualification Status=%x", status);

    }

    if (status)
    {
        /* We dont have our own expander. Hence we will be able to perform only limited support. */
        PtrDevice->DeviceInfo.u.ExpanderInfo.IsBroadcomExpander = FALSE;
    }

    smpReportRequest.SMPFrameType   = SMP_FRAME_TYPE_SMP_REQUEST;
    smpReportRequest.Function       = SMP_FUNCTION_REPORT_GENERAL;

    size = sizeof (SMP_REPORT_GENERAL_RESPONSE);

    status = edmiPerformSmpPassthrough (PtrDevice, (PU8) &smpReportRequest, sizeof (SMP_REPORT_GENERAL_REQUEST), (PU8) &smpReportResponse, size);

    if (smpReportResponse.Header.Fields.FunctionResult && smpReportResponse.Header.Dword != 0x00)
    {
        gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnSmpInterface (SMP Report Function Failed=%x, Result=%x)", smpReportResponse.Header.Fields.Function, smpReportResponse.Header.Fields.FunctionResult);
        return (SCRUTINY_STATUS_IGNORE);
    }

    PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low = smpReportResponse.Response.Fields.EnclosureLogicalIdentifierHigh;
    PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High = smpReportResponse.Response.Fields.EnclosureLogicalIdentifierLow;

    PtrDevice->DeviceInfo.u.ExpanderInfo.EnclosureWwid.Low = smpReportResponse.Response.Fields.EnclosureLogicalIdentifierHigh;
    PtrDevice->DeviceInfo.u.ExpanderInfo.EnclosureWwid.High = smpReportResponse.Response.Fields.EnclosureLogicalIdentifierLow;

    PtrDevice->DeviceInfo.u.ExpanderInfo.NumPhys = smpReportResponse.Response.Fields.NumberOfPhys;

    PtrDevice->DeviceInfo.u.ExpanderInfo.HostPciAddress = PtrDevice->Handle.ScsiHandle.AdapterPCIAddress;

    if (PtrDevice->DeviceInfo.u.ExpanderInfo.IsBroadcomExpander)
    {
        /* This interface will support the SCSI interface. So asssign the firmware version based on the same */
        bsdiGetCurrentFirmwareVersion (PtrDevice, &PtrDevice->DeviceInfo.u.ExpanderInfo.FWVersion);
    }

    gPtrLoggerExpanders->logiFunctionExit ("edmQualifyExpanderOnSmpInterface()");

    return (SCRUTINY_STATUS_SUCCESS);

}


