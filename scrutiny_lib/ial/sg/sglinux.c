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
#include "sglinux.h"
#include "scsigeneric.h"

/**
 *
 * @method  sgiLocateExpanders ()
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will scan the directory to locate expander
 *
 *
 */

SCRUTINY_STATUS sgiLocateScsiDevices()
{
    struct stat statbuf;

    //U32 devices = 0;

    if ((stat (SYSFS_DEVNODE_DIR, &statbuf) >= 0) && (S_ISDIR (statbuf.st_mode)))
    {
        /* Find the number of SCSI devices based on SG directory entries */
      //  devices = sgLinuxScanDirectory (SYSFS_DEVNODE_DIR);

        sgLinuxScanDirectory (SYSFS_DEVNODE_DIR);

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sgiScanSelect ()
 *
 * @param   PtrEntry        Pointer to the scsi device
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will scan the directory to locate expander
 *
 *
 */

SCRUTINY_STATUS sgLinuxScanSelect (__IN__ const struct dirent * PtrEntry)
{
    int idx;

    if (sscanf (PtrEntry->d_name, OSAL_SAS_NODE_NAME_FORMAT"%d", &idx) == 1)
    {

        if (( idx >= 0 ) && (idx < MAX_SCSI_DEVS))
        {

            /*
             * We found one entry go and play around with it now.
             */

            return (sgLinuxCheckDevice (idx));

        }

    }

    return (SCRUTINY_STATUS_FAILED);

}

/*
 *
 *
 * @method  lsgScanDirectory ()
 *
 * @param   PtrDirectory        Pointer to the directory
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will Scan the sys file system for SCSI devices
 *
 *
 */

U32 sgLinuxScanDirectory (__IN__ const char * PtrDirectory)
{
    U32 num;
    register struct dirent *d;

    DIR *dirp;

    if ((dirp = opendir (PtrDirectory)) == NULL)
    {
        return (0);
    }

    num = 0;

    while ((d = readdir (dirp)) != NULL)
    {

        if (sgLinuxScanSelect (d) != SCRUTINY_STATUS_SUCCESS)
        {
            continue;
        }

        num++;

    }

    closedir (dirp);

    return (num);

}

/**
 *
 * @method  lsgCheckDevice ()
 *
 * @param   Index       Index of the device
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will check the expander device and add to the other expander device list
 *
 *
 */

SCRUTINY_STATUS sgLinuxCheckDevice (__IN__ U32 Index)
{

    char    file[64];
    int     handle = 0;
    int     ioctlRetVal = -1;
    Sg_scsi_id  sgData;
    SCRUTINY_STATUS  status = SCRUTINY_STATUS_FAILED;

    sprintf (file, "%s%d", "/dev/sg", Index);

    handle = open (file, O_RDWR | O_NONBLOCK);

    if (handle < 0)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    ioctlRetVal = ioctl (handle, SG_GET_SCSI_ID, &sgData);

    if ((ioctlRetVal < 0) || (sgData.scsi_type != TYPE_ENCLOSURE))
    {

        close (handle);

        return (SCRUTINY_STATUS_FAILED);
    }

    /*
     * We found some device. Lets investiagate more and get
     * back with results
     */

    status = sgAddExpanderDevice (handle, file, Index);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        close (handle);
    }

    return (status);

}

SCRUTINY_STATUS sgAssignPCILocations (char *PtrBuffer, PTR_SCRUTINY_PCI_ADDRESS PtrPciLocation)
{

    U32 domain = 0;
    U32 bus, device, function;

    /* This is the value 0000:04:00.0   Domain:Bus:Device.Function */
    sscanf(PtrBuffer, "%x:%x:%x.%x", &domain, &bus, &device, &function);

    PtrPciLocation->BusNumber = (U16) bus;
    PtrPciLocation->DeviceNumber = (U16) device;
    PtrPciLocation->FunctionNumber = (U16) function;
    PtrPciLocation->SegmentNumber = (U16) domain;

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS sgAddExpanderDevice (__IN__ U32 CurrentHandle, __IN__ const char* PtrFile, __IN__ U32 Index)
{
    SCRUTINY_STATUS         status = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_DEVICE     ptrDevice = NULL;
    char buf[1024] = "";
    int rc;

    ptrDevice = (PTR_SCRUTINY_DEVICE) sosiMemAlloc (sizeof (SCRUTINY_DEVICE));

    sosiMemSet (ptrDevice, 0, sizeof (SCRUTINY_DEVICE));

    ptrDevice->Handle.ScsiHandle.Index = Index;
    ptrDevice->Handle.ScsiHandle.SgDeviceHandle = CurrentHandle;

    ptrDevice->HandleType = SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC;

    rc = ioctl (CurrentHandle, 0x5387, buf);   //SCSI_IOCTL_GET_PCI: This is removed in the header. Not sure which header has this.

    if (rc == 0)
    {
        sgAssignPCILocations (buf, &ptrDevice->Handle.ScsiHandle.AdapterPCIAddress);
    }

    status = bsdiQualifyBroadcomScsiDevice (ptrDevice);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sosiMemFree (ptrDevice);
        return (status);
    }

    ptrDevice->DeviceInfo.HandleType = ptrDevice->HandleType;

    /* We have everything now. Add this device into the global structure */
    status = ldmiAddScrutinyDevice (gPtrScrutinyDeviceManager, ptrDevice);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sosiMemFree (ptrDevice);
        return (status);
    }

    return (status);

}



/**
 *
 * @method  sgiFreeDevice ()
 *
 * @param   PtrExpander     Pointer to the expander
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will close the device
 *
 *
 */

SCRUTINY_STATUS sgiFreeDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    sgiCloseDevice (PtrDevice);

    return (SCRUTINY_STATUS_SUCCESS);
}

/**
 *
 * @method  sgiOpenDevice ()
 *
 * @param   PtrExpander     Pointer to the Expander
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will open the expander device
 *
 *
 */

SCRUTINY_STATUS sgOpenDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    char    file[64];
    int     handle = 0;
    int     ioctlRetVal = -1;
    Sg_scsi_id  sgData;

    if (PtrDevice == NULL)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    sprintf (file, "%s%d", "/dev/sg", PtrDevice->Handle.ScsiHandle.Index);

    handle = open (file, O_RDWR | O_NONBLOCK);

    if (handle < 0)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    ioctlRetVal = ioctl (handle, SG_GET_SCSI_ID, &sgData);

    if ((ioctlRetVal < 0) || (sgData.scsi_type != TYPE_ENCLOSURE))
    {
        close (handle);

        return (SCRUTINY_STATUS_FAILED);
    }

    PtrDevice->Handle.ScsiHandle.SgDeviceHandle = handle;

    return (SCRUTINY_STATUS_SUCCESS);

}

/*
 *
 *
 * @method  sgiCloseDevice ()
 *
 * @param   PtrExpander     Pointer to the expander
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will close the expadner device
 *
 *
 */

SCRUTINY_STATUS sgiCloseDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    if (PtrDevice == NULL)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (PtrDevice->Handle.ScsiHandle.SgDeviceHandle <= 0)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    close ((int) PtrDevice->Handle.ScsiHandle.SgDeviceHandle);

    PtrDevice->Handle.ScsiHandle.SgDeviceHandle = INVALID_HANDLE_VALUE;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sgReadCdb()
 *
 * @param   PtrExpander    Pointer to PTR_DEVICE_INFO
 *
 * @param   PtrCdb         Pointer to U8
 *
 * @param   CdbLen         Length of CDB
 *
 * @param   PtrBuffer      Pointer to buffer
 *
 * @param   Size           Data transfer length
 *
 * @param   PtrSizeRead    Pointer to U32
 *
 * @return  status indicating success or fail
 *
 * @brief   This function is an interface to sgReadCdb.
 *
 *
 */

SCRUTINY_STATUS sgReadCdb (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 *PtrCdb, __IN__ U32 CdbLen, __IN__ U8 *PtrBuffer, __IN__ U32 Size, __IN__ U32 *PtrSizeRead)
{

    unsigned char  senseBuff[32];
    Sg_io_hdr      ioHdr;
    int            ioctlRetVal;

    sosiMemSet (&ioHdr, 0, sizeof (Sg_io_hdr));
    sosiMemSet (senseBuff, 0, sizeof (senseBuff));

    /*Build IO Header for the specified Buffer */
    ioHdr.interface_id    = 'S';
    ioHdr.cmd_len         = CdbLen;
    ioHdr.mx_sb_len       = sizeof (senseBuff);
    ioHdr.dxfer_direction = SG_DXFER_FROM_DEV;
    ioHdr.dxfer_len       = Size;
    ioHdr.dxferp          = PtrBuffer;
    ioHdr.cmdp            = (U8 *) PtrCdb;
    ioHdr.sbp             = senseBuff;
    ioHdr.timeout         = 600000;

    if ((ioctlRetVal = ioctl ((int) PtrDevice->Handle.ScsiHandle.SgDeviceHandle, SG_IO, &ioHdr)) < 0)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (ioHdr.status == 2) /* Magic number #2 is CHECK_CONDITION */
    {
        if (ioHdr.sb_len_wr > 0)
        {
            /*
             *  0x06 = UNIT_ATTENTION
             *  0x29 = POWER ON, RESET Or BUS DEVICE RESET OCCURRED
             *  SenseBuff[2] = Sense key (0-3 bits)
             *  SenseBuff[12] = Additional Sense code (ASC)
             */

            if (( 0x06 == (senseBuff[2] & 0xF )) && (senseBuff[12] == 0x29))
            {
                return (SCRUTINY_STATUS_RETRY);
            }

        }

        // gPtrScrutinyLogger->logiError ("SCSI Status %d, Sense Key %d, Additional Sense Key %d  and Additional Sense Key Qualifier %d",
        //                                ioHdr.status, (senseBuff[2] & 0xF), senseBuff[12], senseBuff[13]);

        return (SCRUTINY_STATUS_FAILED);

    }

    *PtrSizeRead = ioHdr.dxfer_len - ioHdr.resid;

    return (SCRUTINY_STATUS_SUCCESS);

}

/*
 *
 *
 * @method  sgiIsOpened ()
 *
 * @param   PtrExpander     Pointer to the expander
 *
 * @return  status indicating success or fail
 *
 *
 * @brief   This function will check if the device is opened or not
 *
 *
 */

SCRUTINY_STATUS sgiIsOpened (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    if (PtrDevice->Handle.ScsiHandle.SgDeviceHandle != INVALID_HANDLE_VALUE)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    return (SCRUTINY_STATUS_FAILED);

}

/**
 *
 * @method  sgWriteCdb ()
 *
 * @param   PtrExpander     Pointer to the expander
 *
 * @param   PtrCdb          Pointer to the cdb
 *
 * @param   CdbLen          Length of the cdbmments
 *
 * @param   Size            Size to be written
 *
 * @param   PtrSizeWritten  pointer to the Size written
 *
 * @return  status indicating success or fail
 *
 * @brief   This function will Scan the sys file system for SCSI devices
 *
 *
 */

SCRUTINY_STATUS sgWriteCdb (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 *PtrCdb, __IN__ U32 CdbLen, __IN__ U8 *PtrBuffer, __IN__ U32 Size, __IN__ U32 *PtrSizeWritten)
{

    Sg_io_hdr   ioHdr;
    int         ioctlRetVal;
    U8          senseBuff[32];

    sosiMemSet (&ioHdr, 0, sizeof (Sg_io_hdr));
    sosiMemSet (senseBuff, 0, sizeof (senseBuff));

    /*
     * Build IO Header for the specified Buffer
     */

    ioHdr.interface_id    = 'S';
    ioHdr.cmd_len         = CdbLen;
    ioHdr.mx_sb_len       = sizeof (senseBuff);
    ioHdr.dxfer_direction = SG_DXFER_TO_DEV;
    ioHdr.dxfer_len       = Size;
    ioHdr.dxferp          = PtrBuffer;
    ioHdr.cmdp            = (U8 *)PtrCdb;
    ioHdr.sbp             = senseBuff;
    ioHdr.timeout         = 600000;

    if ((ioctlRetVal = ioctl ((int) PtrDevice->Handle.ScsiHandle.SgDeviceHandle, SG_IO, &ioHdr)) < 0)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (ioHdr.status == 2) /* Magic number #2 is CHECK_CONDITION */
    {
        if (ioHdr.sb_len_wr > 0)
        {
            /*
            0x06 = UNIT_ATTENTION
            0x29 = POWER ON, RESET Or BUS DEVICE RESET OCCURRED
            SenseBuff[2] = Sense key (0-3 bits)
            SenseBuff[12] = Additional Sense code (ASC)
            */

            if (((0x06 == (senseBuff[2] & 0xF)) && (senseBuff[12] == 0x29)) ||
                 ((0x05 == (senseBuff[2] & 0xF)) && (0x2C == (senseBuff[12])) && (0x00 == (senseBuff[13] & 0xF))))
            {
                return (SCRUTINY_STATUS_RETRY);
            }

        }

        return (SCRUTINY_STATUS_FAILED);

    }

    /* Need to find out how many bytes are written*/
    *PtrSizeWritten = Size;

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS sgiPerformScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest)
{

    SCRUTINY_STATUS  status = SCRUTINY_STATUS_FAILED;

    U8 retryCount = 3;
    U32 sizeDone = 0;

    if (sgOpenDevice (PtrDevice) != SCRUTINY_STATUS_SUCCESS)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    while (retryCount-- > 0)
    {

        if (PtrScsiRequest->DataDirection == DIRECTION_WRITE)
        {
            status = sgWriteCdb (PtrDevice, PtrScsiRequest->Cdb, PtrScsiRequest->CdbLength, PtrScsiRequest->PtrDataBuffer, PtrScsiRequest->DataBufferLength, &sizeDone);
        }

        else
        {
            status = sgReadCdb (PtrDevice, PtrScsiRequest->Cdb, PtrScsiRequest->CdbLength, PtrScsiRequest->PtrDataBuffer, PtrScsiRequest->DataBufferLength, &sizeDone);
        }

        if (status == SCRUTINY_STATUS_SUCCESS)
        {
            break;
        }

        if (status == SCRUTINY_STATUS_RETRY)
        {
            continue;
        }

        break;

    }

    /* We will not worry much about closing status, since we don't need to carry if there was a failure as well */
    sgiCloseDevice (PtrDevice);

    return (status);

}

