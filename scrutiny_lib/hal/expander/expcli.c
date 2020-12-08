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

static U16 CmdSequenceId = 1; 

/**
 *
 * @method  ecPrepareCommandBuffer()
 *
 *
 * @param   PtrCommand  Command that will have to be executed in
 *                      Inband CLI.
 *
 * @param   PtrBuffer   Write buffer, that contains the command
 *                      which needs to be executed. This buffer
 *                      is directly used by the firmware
 *
 * @return  STATUS      Indication Success or Fail
 *
 * @brief   Preparing the write buffer data, which will be used
 *          for Write CDB command.
 *
 *
 */

SCRUTINY_STATUS cobPrepareCommandBuffer (__IN__ PU8 PtrCommand, __INOUT__ PU8 PtrBuffer)
{

    U32 length = 0;

    gPtrLoggerExpanders->logiFunctionEntry ("cobPrepareCommandBuffer (PtrCommand=0x%x, PtrBuffer=0x%x)", PtrCommand != NULL, PtrBuffer != NULL);

    sosiMemSet (PtrBuffer, 0, EXP_MAX_CLI_CDB_SIZE);

    PtrBuffer[0x00] = 0x00; /* request type is zero currently. */
    PtrBuffer[0x01] = 0x00; /* .. */
    PtrBuffer[0x02] = (U8) (CmdSequenceId >> 8);
    PtrBuffer[0x03] = (U8) (CmdSequenceId & 0xFF);

    CmdSequenceId++;

    length = sosiStringLength ((const char *)PtrCommand);

    sosiMemCopy (&PtrBuffer[0x04], (void *) PtrCommand, length);

    gPtrLoggerExpanders->logiFunctionExit ("cobPrepareCommandBuffer (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);

}



/**
 *
 * @method  ecWriteCliCommandBuffer()
 *
 *
 * @param   PtrCommand  Command that will have to be executed in
 *                      Inband CLI.
 *
 * @return  STATUS      Indication Success or Fail
 *
 * @brief   Preparing the write buffer data, which will be used
 *          for Write CDB command. Then the prepared write
 *          buffer will sent over as a SCSI Write Buffer command
 *
 *
 */

SCRUTINY_STATUS ecWriteCliCommandBuffer (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrCommand)
{

    SCRUTINY_STATUS                 status;
    U8                              commandBuffer[EXP_MAX_CLI_CDB_SIZE];
    SCRUTINY_SCSI_PASSTHROUGH       scsiRequest = { 0 };

    gPtrLoggerExpanders->logiFunctionEntry ("ecWriteCliCommandBuffer (PtrDevice=%x, PtrCommand=%x)", PtrDevice != NULL, PtrCommand != NULL);

    cobPrepareCommandBuffer (PtrCommand, commandBuffer);
    scsiRequest.CdbLength = 10;
    scsiRequest.PtrDataBuffer = commandBuffer;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_WRITE_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_WRITE_MODE_VENDOR_SPECIFIC;
    scsiRequest.Cdb[0x02]  = BRCM_SCSI_BUFFER_ID_SCSI_TOOLBOX;
    scsiRequest.Cdb[0x03]  = 0x00;
    scsiRequest.Cdb[0x04]  = 0x00;
    scsiRequest.Cdb[0x05]  = 0x00;
    scsiRequest.Cdb[0x06]  = (U8) ((EXP_MAX_CLI_CDB_SIZE >> 16) & 0xFF);
    scsiRequest.Cdb[0x07]  = (U8) ((EXP_MAX_CLI_CDB_SIZE >> 8) & 0xFF);;
    scsiRequest.Cdb[0x08]  = (U8) ((EXP_MAX_CLI_CDB_SIZE & 0xFF));
    scsiRequest.Cdb[0x09]  = 0x00;

    scsiRequest.DataDirection = DIRECTION_WRITE;
    scsiRequest.DataBufferLength = EXP_MAX_CLI_CDB_SIZE;

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    gPtrLoggerExpanders->logiFunctionExit ("ecReadCliCommandBuffer (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  ecReadCurrentBuffer()
 *
 *
 * @param   Offset  Offset from which the buffer has to be
 *                  filled
 *
 * @param   Size    Size of the buffer that has to be filled by
 *                  the firmware
 *
 * @param   PtrBuffer   Buffer in which the firmware will fill
 *                      the data
 *
 * @return  STATUS      Indication Success or Fail
 *
 * @brief   Reading the CLI output buffer from firmware. The
 *          firmware provides an option to read the buffer in
 *          chunks and this will be taken care by the addition
 *          offset and size parameters.
 *
 *
 */

SCRUTINY_STATUS ecReadCliCommandBuffer (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Offset, __IN__ U32 Size, __INOUT__ PU8 PtrBuffer)
{
    SCRUTINY_STATUS                 status;
    SCRUTINY_SCSI_PASSTHROUGH       scsiRequest = { 0 };

    gPtrLoggerExpanders->logiFunctionEntry ("ecReadCliCommandBuffer (PtrDevice=%x, Offset=0x%x, Size=0x%x, PtrBuffer=%x)",
             PtrDevice != NULL, Offset, Size, PtrBuffer != NULL);

    scsiRequest.CdbLength = 10;
    scsiRequest.PtrDataBuffer = PtrBuffer;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_READ_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_WRITE_MODE_VENDOR_SPECIFIC;
    scsiRequest.Cdb[0x02]  = BRCM_SCSI_BUFFER_ID_SCSI_TOOLBOX;
    scsiRequest.Cdb[0x03]  = (U8) ((Offset >> 16) & 0xFF);
    scsiRequest.Cdb[0x04]  = (U8) ((Offset >> 8) & 0xFF);
    scsiRequest.Cdb[0x05]  = (U8) ((Offset) & 0xFF);
    scsiRequest.Cdb[0x06]  = (U8) ((Size >> 16) & 0xFF);
    scsiRequest.Cdb[0x07]  = (U8) ((Size >> 8) & 0xFF);
    scsiRequest.Cdb[0x08]  = (U8) ((Size) & 0xFF);
    scsiRequest.Cdb[0x09]  = 0x00;


    scsiRequest.DataDirection = DIRECTION_READ;
    scsiRequest.DataBufferLength = Size;

    status = bsdiPerformScsiPassthrough (PtrDevice, &scsiRequest);

    gPtrLoggerExpanders->logiDumpMemoryInVerbose (PtrBuffer, Size, gPtrLoggerExpanders);

    gPtrLoggerExpanders->logiFunctionExit ("ecReadCliCommandBuffer (status=0x%x)", status);
    return (status);

}

/**
 *
 * @method  eciGetFirmwareCliOutput()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PtrCommand          input CLI command
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   BufferLength        as input, it's the buffer lenght, 
 *                              as output, it's size of the CLI result
 *
 * @param	PtrFolderName	    Pointer to a foldername string, if customer provide folder name, then the result will save to the folder. 
 * 
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail 
 *
 * @brief   request the FW to execute the input cli command
 *
 *
 */
SCRUTINY_STATUS eciGetFirmwareCliOutput (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrCommand,
                    __OUT__ PVOID PtrBuffer, __IN__ U32 BufferLength, __IN__ char *PtrFolderName)
{
    SCRUTINY_STATUS     status;
    U32 offset = 0;
    U32 readOffset = 0;
    U32 sizeRead = 0;
    //U16 sequence;
    BOOLEAN haveBuffer = FALSE;
    U32 totalLength = 0;
    U8 cmdBuffer[EXP_MAX_CLI_DATA_BUFFER + 64];

    ///////tag
    U32 localBufferLength = 0xFFFFFFFF;//initial value is 0xFFFFFFFF
    SOSI_FILE_HANDLE         ptrFile = NULL;
	char                     logFileName[256];
	char                     localFile[512];
    U8                       transferToFile = 0;
    U8                       transferToHost = 0;
    //////tag
    
    gPtrLoggerExpanders->logiFunctionEntry ("eciGetFirmwareCliOutput (PtrDevice=%x, PtrCommand=%x, PtrBuffer=%x, BufferLength=0x%x, PtrFolderName=%x)", 
            PtrDevice != NULL, PtrCommand != NULL, PtrBuffer != NULL, BufferLength, PtrFolderName != NULL);

    ///////tag
    if((PtrBuffer == NULL) && (PtrFolderName == NULL))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerExpanders->logiFunctionExit ("eciGetFirmwareCliOutput  (Status = %x) ",status);
        return (status);	
    }
    if (PtrBuffer != NULL)
    {
        transferToHost = 1;
    }
    if (PtrFolderName != NULL)
    {
        transferToFile = 1;
        transferToHost = 0;//if PtrFolderName not NULL, then we ignore PtrBuffer and override transferToHost
    }
    
    ///////tag


    // CLI command is too long for execution
    if (sosiStringLength ((const char *) PtrCommand) > EXP_MAX_CLI_CDB_SIZE)
    {
        
        gPtrLoggerExpanders->logiDebug ("Input Command Too long");
        gPtrLoggerExpanders->logiFunctionExit ("eciGetFirmwareCliOutput (status=0x%x)", SCRUTINY_STATUS_UNSUPPORTED);
        return SCRUTINY_STATUS_UNSUPPORTED;
    }

    status = ecWriteCliCommandBuffer (PtrDevice, PtrCommand);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("eciGetFirmwareCliOutput (status=0x%x)", status);
        return status;
    }
    
    if (PtrBuffer != NULL)
    {
        sosiMemSet (PtrBuffer, '\0', BufferLength);
    }
    
    ///////tag
    if (transferToFile)
    {
        if (PtrFolderName != NULL)
        {
            //use sosiMkDir from Shaw, currently the function is not here, after Shaw's code check in ,it can work.
            status = sosiMkDir (PtrFolderName);
			
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerExpanders->logiFunctionExit ("eciGetFirmwareCliOutput (status=0x%x)", status);
                return status;
            }        
        }

        ///////tag

        
        ///////tag
        #if defined (OS_LINUX)
        sosiSprintf (logFileName, sizeof (logFileName), "/%s.%s", PtrCommand,  "log" );// for both expander and switch , the SASAddress at the same location.
	    #elif defined (OS_WINDOWS)
        sosiSprintf (logFileName, sizeof (logFileName), "\\%s.%s", PtrCommand,  "log" );
        #endif		

        //sosiStringCopy (logFileName, "FwCliOut.log");
	    if (PtrFolderName != NULL)
	    {
		    sosiStringCopy (localFile, PtrFolderName);
        
		    sosiStringCat (localFile, logFileName);
		    ptrFile = sosiFileOpen (localFile, "wb");
	    }
        
        if (!ptrFile)
	    {	

            status = SCRUTINY_STATUS_FILE_OPEN_FAILED;
            gPtrLoggerExpanders->logiFunctionExit ("eciGetFirmwareCliOutput  (Status = %x) ",status);
            return (status);	
	    }	
    }
    ///////tag
    //now we have file opened.
    
    ///////tag    
    if (PtrBuffer == NULL) 
    {
        localBufferLength = 0xFFFFFFFF;//I will not allocate buffer, but set buffer length to very big vaule.
    }
    else
    {
        localBufferLength = BufferLength;
    }
    
    if (transferToFile == 1)//ignore and override PtrBuffer case
    {
        localBufferLength = 0xFFFFFFFF;//I will not allocate buffer, but set buffer length to very big vaule.
    }
    
    ///////tag    

    while (!haveBuffer)
    {

        sosiMemSet (cmdBuffer, 0, EXP_MAX_CLI_DATA_BUFFER + 64);

        status = ecReadCliCommandBuffer (PtrDevice, readOffset, EXP_MAX_CLI_DATA_BUFFER, cmdBuffer);

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            ///////tag
            if (ptrFile)
		    {
			    sosiFileClose (ptrFile); 
		    }        
            ///////tag

            gPtrLoggerExpanders->logiDebug ("Read Cli Command Fail");
            gPtrLoggerExpanders->logiFunctionExit ("eciGetFirmwareCliOutput (status=0x%x)", status);
            return status;
        }

        haveBuffer = (cmdBuffer[0] & 0x8) > 0 ? TRUE : FALSE;

        //sequence = cmdBuffer[3] << 8 | cmdBuffer[2]; //(data, 2);
        sizeRead = cmdBuffer[5] << 8 | cmdBuffer[4]; //(data, 4);
        totalLength += sizeRead;

        readOffset += sizeRead;
        readOffset += 6;

        if (totalLength > localBufferLength) // when no buffer allocated by user,or transferToFile = 1,  I set localBufferLength=0xFFFFFFFF ,so we can read the whole output
        {
            haveBuffer = TRUE;
            sizeRead = localBufferLength - offset;
        }
        if (transferToHost)
        {
            if ((PtrBuffer != NULL))
            {
                sosiMemCopy (((char *)PtrBuffer + offset), &cmdBuffer[6], sizeRead);
            }
        }
        if (transferToFile)
        {
            if (ptrFile)
            {
                sosiFileWrite (ptrFile,((U8*)(&cmdBuffer[6])), sizeRead);
            }
        }

        offset += sizeRead;

    }

					
    ///////tag
    if (ptrFile)
	{
		sosiFileClose (ptrFile); 
	}        
    ///////tag

    gPtrLoggerExpanders->logiFunctionExit ("eciGetFirmwareCliOutput (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
	
    return (SCRUTINY_STATUS_SUCCESS);
}



