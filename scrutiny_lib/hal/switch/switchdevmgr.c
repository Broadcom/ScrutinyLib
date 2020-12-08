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
#include "atlasregisters.h"

SCRUTINY_STATUS sdmQualifySwitchUsingMemoryReads (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice);


SCRUTINY_STATUS sdmiResetDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS         status = SCRUTINY_STATUS_SUCCESS;
    U32 dword = 0;
    
    gPtrLoggerSwitch->logiFunctionEntry ("sdmiResetDevice (PtrDevice=%x)", PtrDevice != NULL);


    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        return (sdmiScsiResetDevice (PtrDevice));
    }



    
    /* First we need to check if the system is enabled for generating the reset */
    status = bsdiMemoryRead32 (PtrDevice,
                               REGISTER_ADDRESS_ATLAS_CCR_SYSTEM_RESET_ENABLE,
                               &dword,
                               sizeof (U32));

    if (status)
    {
        gPtrLoggerSwitch->logiDebug ("sdmiResetDevice (Memory Read Reset Enable Status=%x)", status);
        gPtrLoggerSwitch->logiFunctionExit ("sdmiResetDevice ( Status=%x)", status);
        return (status);
    }

                               
    if ((dword & 0x02) != 0x02)
    {
        /* We have to first enable the System reset bits */
        dword = 0x02;
        status= bsdiMemoryWrite32 (PtrDevice, REGISTER_ADDRESS_ATLAS_CCR_SYSTEM_RESET_ENABLE, &dword);
        
    }

    if (status)
    {
        gPtrLoggerSwitch->logiDebug ("sdmiResetDevice (Memory Write Reset Enable Status=%x)", status);
        gPtrLoggerSwitch->logiFunctionExit ("sdmiResetDevice ( Status=%x)", status);
        return (status);
    }

                                   
    /* We can generate a reset */
    dword = 0x02;
    status= bsdiMemoryWrite32 (PtrDevice, REGISTER_ADDRESS_ATLAS_CCR_SYSTEM_RESET_GEN, &dword);
   
    
    if (status)
    {
        gPtrLoggerSwitch->logiDebug ("sdmiResetDevice (Memory Write Reset Gen Status=%x)", status);
        gPtrLoggerSwitch->logiFunctionExit ("sdmiResetDevice ( Status=%x)", status);
        return (status);
    }
    
    sosiSleep (ATLAS_MAX_SLEEP_FOR_RESET_IN_MILLISECONDS) ; // sleep 500 mini sec

    gPtrLoggerSwitch->logiFunctionExit ("sdmiResetDevice ( Status=%x)", status);
    return (status);
}

SCRUTINY_STATUS sdmiScsiResetDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{
    U8 data[4];
    U8 SizeInBytes = 4;
    SCRUTINY_STATUS status;

    SCRUTINY_SCSI_PASSTHROUGH scsiRequest = { 0 };

    gPtrLoggerSwitch->logiFunctionEntry ("sdmiScsiResetDevice (PtrDevice=%x)", PtrDevice != NULL);

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

    gPtrLoggerSwitch->logiFunctionExit ("sdmiScsiResetDevice ( Status=%x)", status);
    return (status);


}



SCRUTINY_STATUS sdmiCloseDevice (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

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

SCRUTINY_STATUS sdmiIsBroadcomSwitch (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    SCRUTINY_STATUS status;

    /*
     * We have to do qualify on the SDB Interface which means we will be able to read the
     * device specific registers irrespective of the control
     */

    status = sdmQualifySwitchUsingMemoryReads (PtrDevice);

    if (status)
    {
        return (SCRUTINY_STATUS_IGNORE);
    }

    return (status);

}

SCRUTINY_STATUS sdmQualifySwitchUsingMemoryReads (__INOUT__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    /* Check if we have Atlas device, currently we have only Atlas  */

    if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_SCSI)
    {
        return (atlasiQualifySGSwitch (PtrDevice));
    }

    else
    {
        return (atlasiRegisterBasedQualifSwitch (PtrDevice));
    }

}

