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


PTR_SCRUTINY_DEVICE_MANAGER gPtrScrutinyDeviceManager = NULL;

/**
 *
 * @method  ScrutinyInitialize ()
 *
 * @return  lib_status  Indicating Success or Fail
 *
 * @brief  Initialization of library calls.
 *
*/

#if defined(OS_UEFI)

SCRUTINY_STATUS ScrutinyInitialize (__IN__ EFI_HANDLE ImageHandle, __IN__ EFI_SYSTEM_TABLE *PtrSystemTable)
{

    gLibUdkImageHandle = ImageHandle;
    gLibUdkSystemTable = PtrSystemTable;

    gBS = PtrSystemTable->BootServices;
    gRT = PtrSystemTable->RuntimeServices;

    return (slibiInitializeLibrary());

}

#else

SCRUTINY_STATUS ScrutinyInitialize()
{
    return (slibiInitializeLibrary());
}

#endif



/**
 *
 * @method  ScrutinyInitializeLogging ()
 *
 * @param   PTR_SCRUTINY_DEBUG_LOGGER Pointer to a logger function
 *
 * @return   LIB_status  Indicating Success or Fail
 *
 * @brief   Basic entry point for logger module, user can use this API to use their own logger method
 *
*/

SCRUTINY_STATUS ScrutinyInitializeLogging (__IN__ PTR_SCRUTINY_DEBUG_LOGGER PtrLogger)
{
    return (slibiInitializeLogging (PtrLogger));
}

/**
 *
 * @method  ScrutinyiExit ()
 *
 * @return  lib_status  Indicating Success or Fail
 *
 * @brief  Exit point for all library calls.
 *
 */

SCRUTINY_STATUS ScrutinyExit()
{

    slibiExitLibrary();

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  	crutinyGetLibraryVersion ()
 *
 * @param   	PtrLibraryVersion Pointer variable which holds library version
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API returns library version
 *
*/

SCRUTINY_STATUS ScrutinyGetLibraryVersion (__OUT__ PU32 PtrLibraryVersion)
{

    U32 version = (SCRUTINY_LIBRARY_VERSION_MAJOR << 24) |
                  (SCRUTINY_LIBRARY_VERSION_MINOR << 16) |
                  (SCRUTINY_LIBRARY_VERSION_BUILD << 8) |
                  (SCRUTINY_LIBRARY_VERSION_DEV);

    *PtrLibraryVersion = version;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  	ScrutinyDiscoverDevices ()
 *
 * @param   	DsicoveryFlag Indicating the method of discovery
 *
 * @param	PtrDiscoveryFilter Pointer to discovery params which holds basic discovery params
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API discovers all the supported devices connected
 *
*/

SCRUTINY_STATUS ScrutinyDiscoverDevices (__IN__ U32 DiscoveryFlag, __IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrDiscoveryFilter)
{

    /* Check if the library is initialized or not. */

    if (!gPtrScrutinyDeviceManager)
    {
        return (SCRUTINY_STATUS_LIBRARY_NOT_INITIALIZED);
    }

    /* call the internal discovery proess */

    return (slibiDiscoverDevices (DiscoveryFlag, PtrDiscoveryFilter));

}

/**
 *
 * @method  	ScrutinyGetDeviceCount ()
 *
 * @param	PtrDeviceCount Pointer variable to hold the device count
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API returns the device count
 *
*/

SCRUTINY_STATUS ScrutinyGetDeviceCount (__OUT__ PU32  PtrDeviceCount)
{
    /* Check if the library is initialized or not. */

    if (!gPtrScrutinyDeviceManager)
    {
        return (SCRUTINY_STATUS_LIBRARY_NOT_INITIALIZED);
    }

    if (gPtrScrutinyDeviceManager->DeviceCount == 0)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrDeviceCount = gPtrScrutinyDeviceManager->DeviceCount;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  	ScrutinyGetDeviceInfo ()
 *
 * @param	DeviceIndex Index of the device for which informaiton has to be fetched
 *
 * @param	Size Size of the device info structure
 *
 * @param	PtrDeviceInfo Pointer to device info structure
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API returns the device information
 *
*/

SCRUTINY_STATUS ScrutinyGetDeviceInfo (__IN__  U32 DeviceIndex, __IN__  U32 Size, __OUT__ PTR_SCRUTINY_DEVICE_INFO PtrDeviceInfo)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    /* Check if the library is initialized or not. */

    if (!gPtrScrutinyDeviceManager)
    {
        return (SCRUTINY_STATUS_LIBRARY_NOT_INITIALIZED);
    }

    if (Size != sizeof (SCRUTINY_DEVICE_INFO))
    {
        return (SCRUTINY_STATUS_LIBRARY_VERSION_CONFLICT);
    }

    if (DeviceIndex > gPtrScrutinyDeviceManager->DeviceCount)
    {
        return (SCRUTINY_STATUS_INVALID_DEVICE_INDEX);
    }

    ptrDevice = gPtrScrutinyDeviceManager->PtrDeviceList[(DeviceIndex)];

    sosiMemCopy (PtrDeviceInfo, &ptrDevice->DeviceInfo, sizeof (SCRUTINY_DEVICE_INFO));

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  	ScrutinyResetDevice ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to reset the device
 *
*/

SCRUTINY_STATUS ScrutinyResetDevice (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiResetDevice (ptrDevice));

}

/**
 *
 * @method  	ScrutinyGetFirmwareCliOutput ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrCommand Holds the CLI command which has to be executed
 *
 * @param	PtrBuffer Pointer to a buffer which holds the output of the CLI command
 *
 * @param	BufferLength	Size of the buffer
 *
 * @param	PtrFolderName	Pointer to a foldername string, if customer provide folder name, then the result will save to the folder.
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to run the CLI command, supports HBA, Expander & Switch
 *
*/

SCRUTINY_STATUS ScrutinyGetFirmwareCliOutput (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PU8 PtrCommand, __OUT__ PVOID PtrBuffer, __IN__ U32 BufferLength, __IN__ char *PtrFolderName)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiGetFirmwareCliOutput (ptrDevice, PtrCommand, PtrBuffer, BufferLength, PtrFolderName));

}

/**
 *
 * @method  	ScrutinyOneTimeCaptureLogs ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags            indicate which logs to be collected
 *
 * @param   PtrFolderName    Folder name to save the logs
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the vairous logs from controller
 *
*/

SCRUTINY_STATUS ScrutinyOneTimeCaptureLogs (
    __IN__ U32                                Flags,
    __IN__ const char*                        PtrFolderName
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;
	U32 				index = 0;
	char				currentDirectoryPath[1028] = { '\0'};
	char				currentTargetPath[1028] = { '\0'};

	if (!gPtrScrutinyDeviceManager)
    {
        return (SCRUTINY_STATUS_LIBRARY_NOT_INITIALIZED);
    }

	if (slibiCreateFolder (ptrDevice, PtrFolderName, currentDirectoryPath) != SCRUTINY_STATUS_SUCCESS)
	{
		return (SCRUTINY_STATUS_FAILED);
	}

	for (index = 0; index < gPtrScrutinyDeviceManager->DeviceCount; index++)
	{
	    if (index >= gPtrScrutinyDeviceManager->DeviceCount)
	    {
	        break;
	    }

	    ptrDevice = gPtrScrutinyDeviceManager->PtrDeviceList[index];

	    if (ptrDevice == NULL)
	    {
	    	//unable to get device details.
	        return (SCRUTINY_STATUS_INVALID_HANDLE);
	    }

		if (slibiCreateTargetFolder (ptrDevice, currentDirectoryPath, currentTargetPath) != SCRUTINY_STATUS_SUCCESS)
		{
			return (SCRUTINY_STATUS_FAILED);
		}

		/* Nothing done here, just we call respective API's to get the details */

		slibiOneTimeCaptureLogs (ptrDevice, Flags, currentTargetPath);

	}

	return (SCRUTINY_STATUS_SUCCESS);
}


/**
 *
 * @method  ScrutinyGetCoreDump()
 *
 * @param   PtrAdapterHandle    Pointer to Adapter Handle
 *
 * @param   PtrBuffer           Buffer pointer for Image data
 *
 * @param   PtrBufferSize       Buffer size
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH - input buffer is not large enough
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 * @brief  Entry point for core dump retrieve function
 *
 */
SCRUTINY_STATUS ScrutinyGetCoreDump (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      EXP_COREDUMP_TYPE               Type
    )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }


    return (slibiGetCoreDump(ptrDevice, PtrBuffer, PtrBufferSize, Type, NULL));
}


/**
 *
 * @method  ScrutinyiGetCoreDumpImage()
 *
 * @param   PtrAdapterHandle    Pointer to Adapter Handle
 *
 * @param   PtrBuffer           Buffer pointer for Image data
 *
 * @param   PtrImageSize        Input buffer size
 *
 * @param   Index               index for core dump data, starts from 0
 *                              if input index is invalid, output the
 *                              index of the last core dump data
 *
 * @param   PtrCoreDumpEntry    information for specific core dump data
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH - input buffer is not large enough
 *                              SCRUTINY_STATUS_INVALID_DEVICE_INDEX - input core dump data index is out of range
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 * @brief  Entry point for core dump retrieve function
 *
 */
SCRUTINY_STATUS ScrutinyGetCoreDumpDataByIndex (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrImageSize,
    __INOUT__   PU32                            PtrIndex,
    __INOUT__   PTR_SCRUTINY_CORE_DUMP_ENTRY    PtrCoreDumpEntry
    )
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }


    return (slibiGetCoreDumpDataByIndex (ptrDevice, PtrBuffer, PtrImageSize, PtrIndex, PtrCoreDumpEntry));

}

/**
 *
 * @method  ScrutinyEraseCoreDump()
 *
 * @param   PtrAdapterHandle    Pointer to Adapter Handle
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.

 *
 * @brief   earse all core dump data from the device
 *
 */

SCRUTINY_STATUS ScrutinyEraseCoreDump (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle
    )
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }


    return (slibiEraseCoreDump (ptrDevice));

}


/**
 *
 * @method  ScrutinyGetTraceBuffer()
 *
 * @param   PtrAdapterHandle    Pointer to Adapter Handle
 *          PtrBuffer           Buffer pointer for trace data
 *          PtrBufferLength     pointer to the buffer length
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *                              SCRUTINY_STATUS_FILE_OPEN_FAILED - unable to create the file to store logs
 *
 * @brief   earse all core dump data from the device
 *
 */
SCRUTINY_STATUS ScrutinyGetTraceBuffer (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }


    return (slibiGetTraceBuffer (ptrDevice, PtrBuffer, PtrBufferLength, NULL));
}



/**
 *
 * @method  ScrutinyGetHealthLogs()
 *
 * @param   PtrAdapterHandle    Pointer to Adapter Handle
 *          PtrBuffer           Buffer pointer for trace data
 *          PtrBufferLength     pointer to the buffer length
 *			Flags               indicates how to retrieve health logs
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *                              SCRUTINY_STATUS_FILE_OPEN_FAILED - unable to create the file to store logs
 *
 * @brief   earse all core dump data from the device
 *
 */
SCRUTINY_STATUS ScrutinyGetHealthLogs (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength,
    __IN__ EXP_HEALTH_LOG_TYPE              Flags
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }


    return (slibiGetHealthLogs (ptrDevice, PtrBuffer, PtrBufferLength, Flags, NULL));
}


/**
 *
 * @method  ScrutinyGetTemperatureValue()
 *
 * @param   PtrProductHandle    Pointer to Device Handle
 *          PtrTemperature      Return Device Temperature Value
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.

 *
 * @brief   Get Device Temperature Value
 *
 */

SCRUTINY_STATUS ScrutinyGetTemperature ( __IN__ PTR_SCRUTINY_PRODUCT_HANDLE  PtrProductHandle,  __OUT__ PU32  PtrTemperature)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiGetTemperatureValue (ptrDevice, PtrTemperature));
}


#if defined (LIB_SUPPORT_CONTROLLER)

/**
 *
 * @method  	ScrutinyControllerMpiPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrMpiRequest Pointer to MPI request
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue MPI call to HBA
 *
*/

SCRUTINY_STATUS ScrutinyControllerMpiPassthrough (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __INOUT__ PTR_SCRUTINY_MPI_PASSTHROUGH PtrMpiRequest)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerMpiPassthrough (ptrDevice, PtrMpiRequest));

}

/**
 *
 * @method  	ScrutinyControllerDcmdPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrDcmdRequest Pointer to D-Command request
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue DCMD call to MR
 *
*/

SCRUTINY_STATUS ScrutinyControllerDcmdPassthrough (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __INOUT__ PTR_SCRUTINY_DCMD_PASSTHROUGH PtrDcmdRequest)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerDcmdPassthrough (ptrDevice, PtrDcmdRequest));

}

/**
 *
 * @method  	ScrutinyControllerGetSnapDumpProperty ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	MR_SNAPDUMP_PROPERTIES  Pointer to SnapDump properties MR_SNAPDUMP_INFO

 * @param	MR_SNAPDUMP_INFO  Pointer to SnapDump info
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to get SnapDump property
 *
*/

SCRUTINY_STATUS ScrutinyControllerGetSnapDumpProperty(__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __INOUT__ MR_SNAPDUMP_PROPERTIES *PtrSnapProp, __INOUT__ PU8  PtrSnapDumpCount)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerGetSnapDumpProperty (ptrDevice, PtrSnapProp, PtrSnapDumpCount));

}
/**
 *
 * @method  	ScrutinyControllerSetSnapDumpProperty ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	MR_SNAPDUMP_PROPERTIES  Pointer to SnapDump properties MR_SNAPDUMP_INFO

 * @param	SnapState  SnapdUmp state indicating on or off
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to set SnapDump property
 *
*/


SCRUTINY_STATUS ScrutinyControllerSetSnapDumpProperty( __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U8  SnapState, __INOUT__ MR_SNAPDUMP_PROPERTIES *PtrSnapProp)

{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerSetSnapDumpProperty (ptrDevice, SnapState, PtrSnapProp));
}

/**
 *
 * @method  	ScrutinyControllerGetSnapDump ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	SnapDumpID id of the snapdump
 *
 * @param	Foldername  Pointer to foldername where user want to create the zip file
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to get actual SnapDump data
 *
*/


SCRUTINY_STATUS ScrutinyControllerGetSnapDump (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U8  SnapDumpID, char *PtrFoldername)
{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerGetSnapDump (ptrDevice, SnapDumpID, PtrFoldername));
}

/**
 *
 * @method  	ScrutinyControllerTriggerSnapDump ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to  trigger the SnapDump
 *
*/


SCRUTINY_STATUS ScrutinyControllerTriggerSnapDump (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle)
{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerTriggerSnapDump (ptrDevice));
}

/**
 *
 * @method  	ScrutinyControllerDeleteSnapDump ()
 *
 * @param	PtrProductHandle Device Handle
 *
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to delete the SnapDump
 *
*/

SCRUTINY_STATUS ScrutinyControllerDeleteSnapDump(__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle)
{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerDeleteSnapDump (ptrDevice));
}


/**
 *
 * @method  	ScrutinyControllerSmpPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrSmpRequest Pointer to SMP request
 *
 * @param	PtrIocStatus Pointer to IOC Status structure, in return it holds IOC Status and log info
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue SMP command
 *
*/

SCRUTINY_STATUS ScrutinyControllerSmpPassthrough (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __INOUT__ PTR_SCRUTINY_SMP_PASSTHROUGH PtrSmpRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    /*
     * Check if we have the correct structure version
     */

    if (PtrSmpRequest->Size != sizeof (SCRUTINY_SMP_PASSTHROUGH))
    {
        return (SCRUTINY_STATUS_INVALID_STRUCT_VERSION);
    }

    return (slibiControllerSmpPassthrough (ptrDevice, PtrSmpRequest, PtrIocStatus));

}

#if defined (OS_UEFI)

/**
 *
 * @method  ScrutinyControllerSendDoorBellMsg ()
 *
 * @param PtrProductHandle - Pointer to Adapter Handle
 *
 * @param PtrRequestMessage - Pointer to a request message for which doorbell has to be sent
 *
 * @param RequestSize - Request message size
 *
 * @param PtrReplyMessage - Pointer to a Reply message for which doorbell has to be sent
 *
 * @param ReplySize - Reply message size
 *
 * @return  scrutiny_status  Indicating Success or Fail
 *
 * @brief  Function for firing basic doorbell functionality
 *
 */

SCRUTINY_STATUS ScrutinyControllerSendDoorBellMsg (
    __IN__ 	PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __IN__	PU32                             PtrRequestMessage,
    __IN__  U32                              RequestSize,
    __OUT__ PU32                             PtrReplyMessage,
    __OUT__ U32                              ReplySize
    )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

	if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
	{
		return (SCRUTINY_STATUS_INVALID_HANDLE);
	}

    return (slibiControllerSendDoorbellMsg (ptrDevice,
                                   			PtrRequestMessage,
                                   			RequestSize,
                                   			PtrReplyMessage,
                                   			ReplySize));



}

#endif

/**
 *
 * @method  	ScrutinyControllerScsiPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	DeviceHandle Handle of the device for which SCSI request has to be issued
 *
 * @param	PtrScsiRequest Pointer to SCSI request
 *
 * @param	PtrIocStatus Pointer to IOC Status structure, in return it holds IOC Status and log info
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue SCSI command
 *
*/

SCRUTINY_STATUS ScrutinyControllerScsiPassthrough (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 DeviceHandle, __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    /*
     * Check if we have the correct structure version
     */

    if (PtrScsiRequest->Size != sizeof (SCRUTINY_SCSI_PASSTHROUGH))
    {
        return (SCRUTINY_STATUS_INVALID_STRUCT_VERSION);
    }

    return (slibiControllerScsiPassthrough (ptrDevice, DeviceHandle, PtrScsiRequest, PtrIocStatus));

}

/**
 *
 * @method  	ScrutinyControllerConfigPagePassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrConfigRequest Pointer to Config request
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue config page passthrough command
 *
*/

SCRUTINY_STATUS ScrutinyControllerConfigPagePassthrough (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE  PtrProductHandle, __INOUT__  PTR_SCRUTINY_CONTROLLER_CFG_PASSTHROUGH  PtrConfigRequest)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    /*
     * Check if we have the correct structure version
     */

    if (PtrConfigRequest->Size != sizeof (SCRUTINY_CONTROLLER_CFG_PASSTHROUGH))
    {
        return (SCRUTINY_STATUS_INVALID_STRUCT_VERSION);
    }

    return (slibiControllerConfigPagePassthrough (ptrDevice, PtrConfigRequest));

}

/**
 *
 * @method  	ScrutinyControllerToolboxClean ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags Region which has to be erased
 *
 * @param	PtrIocStatus Pointer to IOC Status structure, in return it holds IOC Status and log info
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue toolbox clean command
 *
*/

SCRUTINY_STATUS ScrutinyControllerToolboxClean (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__  U32 Flags, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerToolboxClean (ptrDevice, Flags, PtrIocStatus));

}

/**
 *
 * @method  ScrutinyControllerToolboxLaneMargin()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *          PtrIstwiRequest     Buffer pointer for trace data
 *          PtrIocStatus     pointer to the buffer length
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.

 *
 * @brief   earse all core dump data from the device
 *
 */

SCRUTINY_STATUS ScrutinyControllerToolboxLaneMargin (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PTR_SCRUTINY_LANE_MARGIN_REQUEST PtrLaneMarginRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerToolboxLaneMargin (ptrDevice, PtrLaneMarginRequest, PtrIocStatus));

}


SCRUTINY_STATUS ScrutinyControllerMemoryRead (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerToolboxMemoryRead (ptrDevice, PtrMemoryOperationRequest, PtrIocStatus));

}


SCRUTINY_STATUS ScrutinyControllerMemoryWrite (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerToolboxMemoryWrite (ptrDevice, PtrMemoryOperationRequest, PtrIocStatus));

}


/**
 *
 * @method  ScrutinyControllerSpdmOps()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *          PtrSpdmRequest     Buffer pointer for trace data
 *          PtrIocStatus     pointer to the buffer length
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.

 *
 * @brief   earse all core dump data from the device
 *
 */

SCRUTINY_STATUS ScrutinyControllerSpdmOps (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PTR_SCRUTINY_SPDM_REQUEST PtrSpdmRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

	return (slibiControllerSpdmOperations (ptrDevice, PtrSpdmRequest, PtrIocStatus));

}


/**
 *
 * @method  	ScrutinyControllerGetDriveBusTarget ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrDevHandle Pointer to a device handle for which information has to be fetched
 *
 * @param	PtrBus Pointer variable which returns Bus number
 *
 * @param	PtrTarget Pointer variable which returns the target number of the device
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the Bus/Device/Target number.
 *
*/

SCRUTINY_STATUS ScrutinyControllerGetDriveBusTarget (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PU16 PtrDevHandle, __OUT__ PU32 PtrBus, __OUT__ PU32 PtrTarget)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerGetDriveBusTarget (ptrDevice, PtrDevHandle, PtrBus, PtrTarget));

}

/**
 *
 * @method  	ScrutinyControllerGetSasPhyErrorCounters ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PhyId Phy number
 *
 * @param	PtrSasPhyErrCounter Pointer to error counter structure
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch SAS Error counter of the device
 *
*/

SCRUTINY_STATUS ScrutinyControllerGetSasPhyErrorCounters (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE  PtrProductHandle, __IN__ U32 PhyNumber, __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS PtrSasPhyErrCounter)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerGetSasPhyErrorCounters (ptrDevice, PhyNumber, PtrSasPhyErrCounter));

}

/**
 *
 * @method  	ScrutinyControllerGetPciPhyErrorCounters ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PhyId Phy number
 *
 * @param	PtrPciePhyErrCounter Pointer to error counter structure
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch PCI Error counter of the device
 *
*/

SCRUTINY_STATUS ScrutinyControllerGetPciPhyErrorCounters (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 PhyNumber, __OUT__ PTR_SCRUTINY_PCI_LINK_ERROR_COUNTERS PtrPciePhyErrCounter)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerGetPciPhyErrorCounters (ptrDevice, PhyNumber, PtrPciePhyErrCounter));

}

/**
 *
 * @method  	ScrutinyControllerGetHealth ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags Based on the flags health of the HBA will be fetched
 *
 * @param	PtrControllerInventory Pointer to ControllerHealth structure which return the health of the HBA
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the health of the HBA
 *
*/

SCRUTINY_STATUS ScrutinyControllerGetHealth (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__  U32                                     Flags,
    __OUT__ PTR_SCRUTINY_CONTROLLER_INVENTORY       PtrControllerInventory
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (ciiGetControllerInventory (ptrDevice, Flags, PtrControllerInventory));

}


/**
 *
 * @method  	ScrutinyControllerHealthCheck ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrFolderName  Folder name for the ini file
 *
 * @param   DumpToFile     Dump the output to a file
 *
 * @param	PtrErrorCode   Error code indicates which tag failed to compare
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to check the health of the HBA
 *
*/
SCRUTINY_STATUS ScrutinyControllerHealthCheck (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle,
    __IN__ const char*                              PtrFolderName,
    __IN__  BOOLEAN                                 DumpToFile,
    __OUT__ PU32                                    PtrErrorCode
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return slibiControllerHealthCheck (ptrDevice, PtrFolderName, DumpToFile, PtrErrorCode);
}


SCRUTINY_STATUS ScrutinyControllerSetCoffeeFault (
	__IN__  PTR_SCRUTINY_PRODUCT_HANDLE             PtrProductHandle
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;
    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiControllerSetCoffeeFault (ptrDevice));
}

/**
 *
 * @method  ScrutinyControllerToolboxIstwiReadWrite()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *          PtrIstwiRequest     Buffer pointer for trace data
 *          PtrIocStatus     pointer to the buffer length
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.

 *
 * @brief   earse all core dump data from the device
 *
 */
SCRUTINY_STATUS ScrutinyControllerToolboxIstwiReadWrite (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PTR_SCRUTINY_ISTWI_REQUEST PtrIstwiRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerToolboxIstwiReadWrite (ptrDevice, PtrIstwiRequest, PtrIocStatus));

}


#if defined (OS_WINDOWS) || defined (OS_LINUX) || defined (OS_VMWARE)

/*
 * All trigger related operations are as of now supported only in Windows and Linux flavor only
 * Rest of the OS support is not there so limiting this API's only to above said OS flavor's
*/


/**
 *
 * @method  ScrutinyControllerAfdSendTriggerCommand()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *
 * @param   TriggerType         Trigger Type (Master/Event/MPI/SCSI)
 *
 * @param   TriggerAction       Trigger Action (set/get/clear)
 *
 * @param   PtrBuffer           Pointer to the trigger buffer
 *
 * @param   PtrSize             Pointer to the size of the buffer
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief  Entry point for trigger function
 *
 */
SCRUTINY_STATUS ScrutinyControllerAfdSendTriggerCommand (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE         PtrProductHandle,
    __IN__      HBA_AFD_TRIGGER_TYPE                TriggerType,
    __IN__      HBA_AFD_ACTION                      TriggerAction,
    __INOUT__   PVOID                               PtrBuffer,
    __INOUT__   PU32                                PtrSize)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    status = mptiTriggerIoctl (ptrDevice, TriggerType, TriggerAction, PtrBuffer, PtrSize);

    return status;

}

/**
 *
 * @method  ScrutinyControllerAfdDiagnosticBufferRegister()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *
 * @param   BufferType          Buffer Type (Trace/Snapshot)
 *
 * @param   ExtendedType        Extended buffer type
 *
 * @param   Size                Pointer to the size of the buffer
 *
 * @param   DiagnosticFlags     Flag to point when to release the buffer
 *
 * @param   PtrUniqueId         Buffer Id
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief  Entry point for diag buffer register
 *
 */
SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferRegister (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__      U8                              BufferType,
    __IN__      U8                              ExtendedType,
    __IN__      U32                             BufferSize,
    __IN__      U32                             DiagnosticFlags,
    __IN__      U32                             ProductSpecific[23],
    __INOUT__   PU32                            PtrUniqueId)
{


    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (mptiDiagBufferRegister (ptrDevice,
                                    BufferType,
                                    ExtendedType,
                                    BufferSize,
                                    DiagnosticFlags,
                                    ProductSpecific,
                                    PtrUniqueId));

}

/**
 *
 * @method  ScrutinyControllerAfdDiagnosticBufferQuery()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *
 * @param   BufferType          Buffer Type (Trace/Snapshot)
 *
 * @param   Size                Pointer to the size of the buffer
 *
 * @param   PtrFlags            Flag to point when to release the buffer
 *
 * @param   PtrUniqueId         Buffer Id
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief  Entry point for diag buffer query function
 *
 */
SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferQuery  (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__      U8                              BufferType,
    __OUT__     PU16                            PtrFlags,
    __INOUT__   PU32                            PtrBufferSize,
    __INOUT__   PU32                            PtrUniqueId)
{


    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (mptiDiagBufferQuery (ptrDevice,
                                 BufferType,
                                 PtrFlags,
                                 PtrBufferSize,
                                 PtrUniqueId));

}

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferExtendedQuery  (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__      U8                              BufferType,
    __OUT__		PTR_SCRUTINY_EXTENDED_QUERY		PtrExtendedQuery,
    __INOUT__   PU32                            PtrUniqueId)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

	return (mptiDiagBufferExtendedQuery (ptrDevice,
								 		BufferType,
								 		PtrExtendedQuery,
		   							 	PtrUniqueId));


}


/**
 *
 * @method  ScrutinyControllerAfdDiagnosticBufferRead()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *
 * @param   BufferType          Buffer Type (Trace/Snapshot)
 *
 * @param   Size                Pointer to the size of the buffer
 *
 * @param   PtrBuffer           Buffer to hold the data
 *
 * @param   PtrUniqueId         Buffer Id
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief  Entry point for diag buffer read function
 *
 */

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferRead (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__      U8                              BufferType,
    __INOUT__   PU32                            PtrBufferSize,
    __INOUT__   PU32                            PtrDataBuffer,
    __INOUT__   PU32                            PtrUniqueId)
{


    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (mptiDiagBufferRead (ptrDevice,
                                BufferType,
                                PtrBufferSize,
                                PtrDataBuffer,
                                PtrUniqueId));

}

/**
 *
 * @method  ScrutinyControllerAfdDiagnosticBufferRelease()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *
 * @param   BufferType          Buffer Type (Trace/Snapshot)
 *
 * @param   PtrUniqueId         Buffer Id
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief  Entry point for diag buffer release function
 *
 */

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferRelease (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__      U8                              BufferType,
    __IN__      PU32                            PtrUniqueId)
{


    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (mptiDiagBufferRelease (ptrDevice, BufferType, PtrUniqueId));

}

/**
 *
 * @method  ScrutinyControllerAfdDiagnosticBufferUnregister()
 *
 * @param   PtrAdapterHandle    Pointer to Adapter Handle
 *
 * @param   BufferType          Buffer Type (Trace/Snapshot)
 *
 * @param   PtrUniqueId         Buffer Id
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *
 * @brief  Entry point for diag buffer un-register function
 *
 */

SCRUTINY_STATUS ScrutinyControllerAfdDiagnosticBufferUnregister (
    __IN__      PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__      U8                              BufferType,
    __IN__      PU32                            PtrUniqueId
    )
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (mptiDiagBufferUnregister (ptrDevice, BufferType, PtrUniqueId));

}

/**
 *
 * @method  	ScrutinyGetControllerLogs ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags            indicate which logs to be collected
 *
 * @param   PtrFolderName    Folder name to save the logs
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the vairous logs from controller
 *
*/
SCRUTINY_STATUS ScrutinyControllerGetLogs (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __IN__ U32                                  Flags,
    __IN__ const char*                          PtrFolderName
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (PtrFolderName == NULL)
    {
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

	return (slibiControllerGetLogs (ptrDevice, Flags, PtrFolderName));
}


#endif

#if defined (OS_UEFI)

/**
 *
 * @method  	ScrutinyControllerHostboot ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrFirmware Firmware which has to be hostbooted
 *
 * @param	FirmwareLength Length of the firmware passed
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to host boot the IOC
 *
*/

SCRUTINY_STATUS ScrutinyControllerHostboot (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ PU8 PtrFirmware, __IN__ U32 FirmwareLength)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerHostboot (ptrDevice, PtrFirmware, FirmwareLength));

}

/**
 *
 * @method  	ScrutinyControllerGetIocState ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrValue Pointer to a U32 variable which returns the IOC State
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to get the IOC State
 *
*/

SCRUTINY_STATUS ScrutinyControllerGetIocState (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__ PU32 PtrValue)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerGetIocState (ptrDevice, PtrValue));

}

/**
 *
 * @method  	ScrutinyControllerGetIocFaultCode ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrValue Pointer to a U32 variable which returns the IOC State
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to get the IOC State
 *
*/

SCRUTINY_STATUS ScrutinyControllerGetIocFaultCode (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__ PU32 PtrValue, __IN__ U32 SizeInBytes)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiControllerGetIocFaultCode (ptrDevice, PtrValue, SizeInBytes));

}

#endif

#endif

#if defined (LIB_SUPPORT_EXPANDER)
/**
 *
 * @method  	ScrutinyExpanderScsiPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrScsiRequest Pointer to SCSI request structure which holds the SCSI command to be fired
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue SCSI command to Expander
 *
*/

SCRUTINY_STATUS ScrutinyExpanderScsiPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH     PtrScsiRequest
)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderScsiPassthrough (ptrDevice, PtrScsiRequest));

}

/**
 *
 * @method  	ScrutinyExpanderGetConfigPage ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PageNumber	Configuration Page number
 *
 * @param	Region	Region numberm, config page will be fetched from that page
 *
 * @param	PtrPageBuffer Pointer buffer to hold the config page information
 *
 * @param	PtrPageBufferLength Length of the page
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue config command
 *
*/

SCRUTINY_STATUS ScrutinyExpanderGetConfigPage (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__  U16                             PageNumber,
    __IN__  U8                              Region,
    __OUT__ PU8                             PtrPageBuffer,
    __OUT__ PU32                            PtrPageBufferLength
)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderGetConfigPassthrough (ptrDevice, PageNumber, Region, &PtrPageBuffer, PtrPageBufferLength));

}


/**
 *
 * @method  ScrutinyExpanderEnableDisablePhy()
 *
 * @param   PtrProductHandle    Pointer to Expander Handle
 *          PhyIdentifier       Phy index that need be Enabled/Disabled
 *          Enable              A BOOLEAN input value, TRUE means enable phy,FALSE means disable phy
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support core dump feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.

 *
 * @brief   Enable or disable expander phy
 *
 */

SCRUTINY_STATUS ScrutinyExpanderEnableDisablePhy ( __IN__ PTR_SCRUTINY_PRODUCT_HANDLE  PtrProductHandle,  __IN__ U8 PhyIdentifier,  __IN__ BOOLEAN  PhyEnable)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderEnableDisablePhy (ptrDevice, PhyIdentifier, PhyEnable));
}


/**
 *
 *  @method  ScrutinyExpanderResetPhy ()
 *
 *  @param   PtrProductHandle        Pointer to Device Handle
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PhyOperation            Phy control operation,=1 link reset, =2 hard reset =3 clear error counter
 *
 *  @param   DoAllPhys               if the operation for single phy or all phys,=TRUE for all phys, =FALSE for single phy identified by  PhyIdentifier
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do some phy control operation on expander phy
 *
 */


SCRUTINY_STATUS ScrutinyExpanderResetPhy (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U8 PhyIdentifier, __IN__ U8 PhyOperation, __IN__ BOOLEAN DoAllPhys)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderResetPhy (ptrDevice, PhyIdentifier, PhyOperation, DoAllPhys));

}


/**
 *
 *  @method  ScrutinyExpanderGetPhyErrorCounters ()
 *
 *  @param   PtrProductHandle        Pointer to Device Handle
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PtrSasPhyErrCounter     Phy error counter value returned.
 *
   @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get  phy error counters on expander phy ,InvalidDwordCount,RunningDisparityErrorCount,LossDwordSynchCount,PhyResetProblemCount
 *
 */


SCRUTINY_STATUS ScrutinyExpanderGetPhyErrorCounters (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U8 PhyIdentifier, __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS PtrSasPhyErrCounter)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderGetPhyErrorCounters (ptrDevice, PhyIdentifier, PtrSasPhyErrCounter));

}

/**
 *
 *  @method  ScrutinyExpanderHealthCheck ()
 *
 *  @param   PtrProductHandle        Pointer to Device Handle
 *
 *  @param   PtrConfigFilePath       Pointer to the string which contain the config ini file path.
 *
 *  @param   DumpToFile              BOOLEAN variable ,when=TRUE mean the related read back status valve dump to a file.
 *
 *  @param   PtrErrorCode            Pointer to U32 variable, each bit conrespond to a error condition.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do the expander health monitor check according to the config ini file content.
 *
 */

SCRUTINY_STATUS  ScrutinyExpanderHealthCheck (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32  PtrErrorCode)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderHealthCheck (ptrDevice, PtrConfigFilePath, DumpToFile, PtrErrorCode));

}

/**
 *
 * @method  	ScrutinyExpanderMemoryRead ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Address	Address of the flash region from where contents has to be read
 *
 * @param	PtrValue Pointer to a U32 value which return the contents read
 *
 * @param	SizeInBytes Size which has to be read
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to read the expander flash region contents
 *
*/

SCRUTINY_STATUS ScrutinyExpanderMemoryRead (
    __IN__ 	PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__ 	U32 							Address,
    __OUT__ PVOID PtrValue,
    __IN__ 	U32 							SizeInBytes
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiBroadcomScsiMemoryRead (ptrDevice, Address, PtrValue, SizeInBytes));

}

/**
 *
 * @method  	ScrutinyExpanderMemoryWrite ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Address	Address of the flash region from where contents has to be read
 *
 * @param	PtrValue Pointer to a U32 value which return the contents read
 *
 * @param	SizeInBytes Size which has to be read
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to write the expander flash region contents
 *
*/

SCRUTINY_STATUS ScrutinyExpanderMemoryWrite (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE      PtrProductHandle,
    __IN__ U32 								Address,
    __IN__ PVOID PtrValue,
    __IN__ U32 								SizeInBytes
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiBroadcomScsiMemoryWrite (ptrDevice, Address, PtrValue, SizeInBytes));

}


/**
 *
 * @method  	ScrutinyExpanderGetHealth ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrExpHealthInfo Pointer to ExpanderHealth structure which return the health of the Expander
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch all kinds of expander logs
 *
*/

SCRUTINY_STATUS  ScrutinyExpanderGetHealth (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __OUT__  PTR_SCRUTINY_EXPANDER_HEALTH       PtrExpHealthInfo
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderGetHealth (ptrDevice, PtrExpHealthInfo));
}

/**
 *
 * @method  	ScrutinyGetExpanderLogs ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags            indicate which logs to be collected
 *
 * @param   PtrFolderName    Folder name to save the logs
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the health of the Expander
 *
*/
SCRUTINY_STATUS ScrutinyExpanderGetLogs (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __IN__ U32                                  Flags,
    __IN__ const char*                          PtrFolderName
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (PtrFolderName == NULL)
    {
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

	return (slibiExpanderGetLogs (ptrDevice, Flags, PtrFolderName));

}


SCRUTINY_STATUS ScrutinyExpanderFlashMemoryDownload (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE              PtrProductHandle,
    __IN__ U32                                      FlashAddress,
    __IN__ PU8                                      PtrBuffer,
    __IN__ U32                                      BufferSize,
    __IN__ ScrutinyLogCallBack                      CallBack
    )

{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiExpanderFlashMemoryDownload (ptrDevice, FlashAddress, PtrBuffer, BufferSize, CallBack));

}

#endif

#if defined (LIB_SUPPORT_SWITCH)

/**
 *
 * @method  	ScrutinySwitchScsiPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrScsiRequest Pointer to SCSI request structure which holds the SCSI command to be fired
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue SCSI command to Switch
 *
*/

SCRUTINY_STATUS ScrutinySwitchScsiPassthrough (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH     PtrScsiRequest
)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchScsiPassthrough (ptrDevice, PtrScsiRequest));

}


/**
 *
 * @method  	ScrutinySwitchGetConfigPage ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PageNumber	Configuration Page number
 *
 * @param	Region	Region numberm, config page will be fetched from that page
 *
 * @param	PtrPageBuffer Pointer buffer to hold the config page information
 *
 * @param	PtrPageBufferLength Length of the page
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to issue config command
 *
*/

SCRUTINY_STATUS ScrutinySwitchGetConfigPage (
    __IN__  PTR_SCRUTINY_PRODUCT_HANDLE     PtrProductHandle,
    __IN__  U16                             PageNumber,
    __IN__  U8                              Region,
    __OUT__ PU8*                            PtrPageBuffer,
    __OUT__ PU32                            PtrPageBufferLength
)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchGetConfigPassthrough (ptrDevice, PageNumber, Region, PtrPageBuffer, PtrPageBufferLength));

}



/**
 *
 * @method  ScrutinySwitchGetPciPortErrorCounters()
 *
 * @param   PtrProductHandle    Pointer to Adapter Handle
 *          Port                Pcie Port number
 *          PtrPciePortErrStatistic     pointer to the returned port statistic
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success and
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - the input device is not switch device
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.

 *
 * @brief   collect error statistic of specific switch port
 *
 */
SCRUTINY_STATUS ScrutinySwitchGetPciPortErrorStatistics (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
    __IN__ U32 Port,
    __OUT__ PTR_SCRUTINY_SWITCH_ERROR_STATISTICS PtrPciePortErrStatistic)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }


    return (slibiSwitchGetPciPortErrorStatistic (ptrDevice, Port, PtrPciePortErrStatistic));
}


/**
 *
 *  @method  ScrutinySwitchGetPciePortProperties ()
 *
 *  @param   PtrProductHandle        Pointer to Device Handle
 *
 *  @param   PtrPciePortProperties   Pointer to data structure contain the returned port properties.
 *
    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get switch port properties.
 *
 */


SCRUTINY_STATUS ScrutinySwitchGetPciePortProperties (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,  __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchGetPciePortProperties (ptrDevice, PtrPciePortProperties));

}


/**
 *
 *  @method  ScrutinySwitchHealthCheck ()
 *
 *  @param   PtrProductHandle        Pointer to Device Handle
 *
 *  @param   PtrConfigFilePath       Pointer to the string which contain the config ini file path.
 *
 *  @param   DumpToFile              BOOLEAN variable ,when=TRUE mean the related read back status valve dump to a file.
 *
 *  @param   PtrErrorCode            Pointer to U32 variable, each bit conrespond to a error condition.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do the switch health monitor check according to the config ini file content.
 *
 */


SCRUTINY_STATUS  ScrutinySwitchHealthCheck (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32 PtrErrorCode)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchHealthCheck (ptrDevice, PtrConfigFilePath, DumpToFile, PtrErrorCode));

}

/**
 *
 *  @method  ScrutinySwitchRxEqStatus ()
 *
 *  @param   PtrProductHandle        Pointer to Device Handle
 *
 *  @param   StartPort               The start port number.
 *
 *  @param   NumberOfPort            The total number of ports
 *
 *  @param   PtrPortRxEqStatus       Pointer to  data structure containing read back Rx Coeff.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method read switch PCIe port Rx Equalization Coeff, include VGA. AEQ, DFE.
 *
 */

SCRUTINY_STATUS  ScrutinySwitchRxEqStatus (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS PtrPortRxEqStatus)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchRxEqStatus (ptrDevice, StartPort, NumberOfPort, PtrPortRxEqStatus));
}

/**
 *
 *  @method  ScrutinySwitchTxCoeff ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports
 *
 *  @param   PtrPortTxCoeffStatus       Pointer to  data structure containing read back Tx Coeff.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method read switch PCIe port Tx  Coeff, include near end and far end .
 *
 */

SCRUTINY_STATUS  ScrutinySwitchTxCoeff (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF PtrPortTxCoeffStatus)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchTxCoeff (ptrDevice, StartPort, NumberOfPort, PtrPortTxCoeffStatus));

}


/**
 *
 *  @method  ScrutinySwitchHardwareEyeStart ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test.
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again.
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */


SCRUTINY_STATUS ScrutinySwitchHardwareEyeStart (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchHardwareEyeStart (ptrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  ScrutinySwitchHardwareEyePoll ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test.
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again.
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */

SCRUTINY_STATUS ScrutinySwitchHardwareEyePoll (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchHardwareEyePoll (ptrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  ScrutinySwitchHardwareEyeGet ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port . ONLY active lane can do eye test.
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again.
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */

SCRUTINY_STATUS ScrutinySwitchHardwareEyeGet (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchHardwareEyeGet (ptrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  ScrutinySwitchHardwareEyeClean ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .  ONLY active lane can do eye test.
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again.
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */


SCRUTINY_STATUS ScrutinySwitchHardwareEyeClean (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchHardwareEyeClean (ptrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  ScrutinySwitchSoftwareEye ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports
 *
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do software eye test on  switch PCIe port . ONLY active lane can do software eye test
 *                                   The result will save as a file in current directory
 *
 */


SCRUTINY_STATUS ScrutinySwitchSoftwareEye (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ U32  StartPort, __IN__   U32 NumberOfPort )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchSoftwareEye (ptrDevice, StartPort, NumberOfPort));

}


/**
 *
 *  @method  ScrutinySwitchGetPowerOnSense ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   PtrSwPwrOnSense            Pointer to power on sense result.
 *
 *
    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get switch power on sense setting .
 *
 *
 */

SCRUTINY_STATUS ScrutinySwitchGetPowerOnSense (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__   PTR_SCRUTINY_SWITCH_POWER_ON_SENSE PtrSwPwrOnSense )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchGetPowerOnSense (ptrDevice, PtrSwPwrOnSense));

}

/**
 *
 *  @method  ScrutinySwitchGetCcrStatus ()
 *
 *  @param   PtrProductHandle           Pointer to Device Handle
 *
 *  @param   PtrSwCcrStatus            Pointer to CCR status.
 *
 *
    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get switch CCR status .
 *
 *
 */

SCRUTINY_STATUS ScrutinySwitchGetCcrStatus (__IN__   PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__   PTR_SCRUTINY_SWITCH_CCR_STATUS PtrSwCcrStatus )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchGetCcrStatus (ptrDevice, PtrSwCcrStatus));

}

/**
 *
 *  @method  ScrutinySwitchGetPciePortPerformance ()
 *
 *  @param   PtrProductHandle                  Pointer to Device Handle
 *
 *  @param   PtrPciePortPerformance            Pointer to PCIe Performance data output.
 *
 *
    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get switch PCIe port performance data .
 *
 *
 */

SCRUTINY_STATUS ScrutinySwitchGetPciePortPerformance ( __IN__ PTR_SCRUTINY_PRODUCT_HANDLE  PtrProductHandle, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    return (slibiSwitchGetPciePortPerformance (ptrDevice, PtrPciePortPerformance));

}

/**
 *
 * @method  	ScrutinySwitchrGetHealth ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrExpHealthInfo Pointer to Swtich Health structure which return the health of the Swtich
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch all kinds of switch logs
 *
*/

SCRUTINY_STATUS  ScrutinySwitchGetHealth (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __OUT__  PTR_SCRUTINY_SWITCH_HEALTH         PtrSwHealthInfo
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchGetHealth (ptrDevice, PtrSwHealthInfo));
}

/**
 *
 * @method  	ScrutinyGetSwitchLogs ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags            indicate which logs to be collected
 *
 * @param   PtrFolderName    Folder name to save the logs
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the health of the Switch
 *
*/
SCRUTINY_STATUS ScrutinySwitchGetLogs (
    __IN__   PTR_SCRUTINY_PRODUCT_HANDLE        PtrProductHandle,
    __IN__ U32                                  Flags,
    __IN__ const char*                          PtrFolderName
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (PtrFolderName == NULL)
    {
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

	return (slibiSwitchGetLogs (ptrDevice, Flags, PtrFolderName));

}


/**
 *
 * @method  	ScrutinySwitchMemoryRead ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Address	Address of the flash region from where contents has to be read
 *
 * @param	PtrValue Pointer to a U32 value which return the contents read
 *
 * @param	SizeInBytes Size which has to be read
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to write the expander flash region contents
 *
*/


SCRUTINY_STATUS ScrutinySwitchMemoryRead (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __IN__ U32 Address,
    __OUT__ PU32 PtrValue,
    __IN__ U32 SizeInBytes
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiBroadcomScsiMemoryRead (ptrDevice, Address, PtrValue, SizeInBytes));

}

/**
 *
 * @method  	ScrutinySwitchMemoryWrite ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Address	Address of the flash region from where contents has to be read
 *
 * @param	PtrValue Pointer to a U32 value which return the contents read
 *
 * @param	SizeInBytes Size which has to be read
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to write the expander flash region contents
 *
*/


SCRUTINY_STATUS ScrutinySwitchMemoryWrite (
    __IN__ PTR_SCRUTINY_PRODUCT_HANDLE          PtrProductHandle,
    __IN__ U32 Address,
    __IN__ PU32 PtrValue,
    __IN__ U32 SizeInBytes
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiBroadcomScsiMemoryWrite (ptrDevice, Address, PtrValue, SizeInBytes));

}


/**
 *
 * @method ScrutinySwitchGetLaneMarginCapacities()
 *
 * @param    PtrProductHandle                      Pointer to Adapter Handle
 *           SwtichPort                            Selected switch port
 *           Lanes                                 selected lane in the port
 *           PtrMarginControlCapabilities          margin control capabilities supported
 *           PtrNumSteps                           the higher significant 16 bits are NumVoltageSteps and the lower significant 16 bits are NumTimingSteps
 *           PtrMaxOffset                          the higher significant 16 bits are MaxVoltageOffset and the lower significant 16 bits are MaxTimingOffset
 *
 * @return   SCRUTINY_STATUS_SUCCESS                - PCIe Lane Margining was executed successfully
 *           SCRUTINY_STATUS_FAILED                 - on failure.
 *           SCRUTINY_STATUS_INVALID_HANDLE         - invalid input handle
 *           SCRUTINY_STATUS_UNSUPPORTED            - the input device is not switch device
 *           SCRUTINY_STATUS_IOCTL_ERROR            - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 * @brief    retrievethe  lane margining capacities in the selected lane of selected port
 *
 */

SCRUTINY_STATUS ScrutinySwitchGetLaneMarginCapacities (
	__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle,
	__IN__ U8 SwtichPort,
	__IN__ U16 Lane,
	__OUT__ PU32 PtrMarginControlCapabilities,
	__OUT__ PU32 PtrNumSteps,
	__OUT__ PU32 PtrMaxOffset
 )
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchGetLaneMarginCapacities (ptrDevice, SwtichPort, Lane, PtrMarginControlCapabilities, PtrNumSteps, PtrMaxOffset));
}


/**
 *
 * @method   ScrutinySwitchPerformLaneMargining()
 *
 * @param    PtrProductHandle          Pointer to Adapter Handle
 *           PtrSwitchLaneMarginReq    PCIe Lane margin input parameters
 *           PtrSwitchLaneMarginResp   PCIe Lane margin outputs if the margin was executed. One response returns
 *                                   margin result for one lane. The caller need to allocate buffer large enough for
 *                                   the number of input lanes
 *
 * @return   SCRUTINY_STATUS_SUCCESS                - PCIe Lane Margining was executed successfully
 *           SCRUTINY_STATUS_FAILED                 - on failure.
 *           SCRUTINY_STATUS_INVALID_HANDLE         - invalid input handle
 *           SCRUTINY_STATUS_UNSUPPORTED            - the input device is not switch device
 *           SCRUTINY_STATUS_IOCTL_ERROR            - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *           SCRUTINY_STATUS_INVALID_LM_REQUEST     - Input port number is invalid or the port is not enabled, or
 *                                                         the link  is not in L0 state, or the input lane number is invalid
 *
 * @brief    perform the lane margining in the selected lane of selected port
 *
 */

SCRUTINY_STATUS ScrutinySwitchPerformLaneMargining    (
	__IN__ PTR_SCRUTINY_PRODUCT_HANDLE              PtrProductHandle,
	__IN__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_REQUEST  PtrSwitchLaneMarginReq,
    __OUT__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE PtrSwitchLaneMarginResp
)
{
    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchPerformLaneMargining (ptrDevice, PtrSwitchLaneMarginReq, PtrSwitchLaneMarginResp));
}


#if defined(OS_LINUX)


SCRUTINY_STATUS  ScrutinySwitchSetAladinConfiguration (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __IN__ const char* PtrConfigFilePath, __OUT__ PU32	PtrConfigStatus)
{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    return (slibiSwitchSetAladinConfiguration (ptrDevice, PtrConfigFilePath, PtrConfigStatus));
}

SCRUTINY_STATUS ScrutinySwitchGetAladinTracedump (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__ const char* PtrDumpFilePath, __OUT__ PU32 PtrDumpStatus)
{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

	if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
	{
		return (SCRUTINY_STATUS_INVALID_HANDLE);
	}

	return (slibiSwitchGetAladinTracedump (ptrDevice, PtrDumpFilePath, PtrDumpStatus));
}

SCRUTINY_STATUS ScrutinySwitchPollAladinTrigger (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__ PU32 PtrTriggerStatus)
{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

	if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
	{
		return (SCRUTINY_STATUS_INVALID_HANDLE);
	}

	return (slibiSwitchPollAladinTrigger (ptrDevice, PtrTriggerStatus));
}

SCRUTINY_STATUS ScrutinySwitchStopAladinCapture (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__ PU32 PtrStopStatus)
{
	PTR_SCRUTINY_DEVICE ptrDevice = NULL;

	if (slibiProductHandleToScrutinyDevice (PtrProductHandle, &ptrDevice))
	{
		return (SCRUTINY_STATUS_INVALID_HANDLE);
	}

	return (slibiSwitchStopAladinTrace (ptrDevice, PtrStopStatus));
}

#endif
#endif
