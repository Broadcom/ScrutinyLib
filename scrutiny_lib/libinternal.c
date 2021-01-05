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
 * @method  slibiInitializeLibrary ()
 *
 * @return  lib_status  Indicating Success or Fail
 *
 * @brief  Initialization of library calls.
 *
*/
SCRUTINY_STATUS slibiInitializeLibrary()
{

    SCRUTINY_STATUS status;

    status = ldmiInitializeLibraryDeviceManager (&gPtrScrutinyDeviceManager);

    if (status)
    {
        return (status);
    }

    lcpiInitializeLibraryConfigurations();

    #if defined(OS_UEFI)

        /*
         * Disconnect the existing EFI controllers first
         */

        if (cdmiDisconnectEfiControllerDrivers() != SCRUTINY_STATUS_SUCCESS)
        {
            return (SCRUTINY_STATUS_EFI_DISCONNECT_CTRL_FAILED);
        }

    #endif

    /* We need to initialize the default logging. */

    ldliInitializeDefaultLogging();

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  slibiInitializeLogging ()
 *
 * @param   PTR_SCRUTINY_DEBUG_LOGGER Pointer to a logger function
 *
 * @return   LIB_status  Indicating Success or Fail
 *
 * @brief   Basic entry point for logger module, user can use this API to use their own logger method
 *
*/

SCRUTINY_STATUS slibiInitializeLogging (__IN__ PTR_SCRUTINY_DEBUG_LOGGER PtrLogger)
{
    ldliInitializeLogging (PtrLogger);

    return (SCRUTINY_STATUS_SUCCESS);
}

/**
 *
 * @method  slibiExitLibrary ()
 *
 * @return  lib_status  Indicating Success or Fail
 *
 * @brief  Exit point for all library calls.
 *
 */

SCRUTINY_STATUS slibiExitLibrary()
{

    ldmiFreeDevices (gPtrScrutinyDeviceManager);

    #ifdef OS_UEFI

        /*
         * Connect the EFI controllers back
         */

        if (cdmiConnectEfiControllerDrivers() != SCRUTINY_STATUS_SUCCESS)
        {
            return (SCRUTINY_STATUS_FAILED);
        }

    #endif

	#ifdef OS_BMC
		oobExit ();
	#endif	

    //sosiMemFree (gPtrLoggerController);
    sosiMemFree (gPtrScrutinyDeviceManager);

    lcpiDestryLibraryConfigurations();

    /* Restore the global variables to NULL */
    gPtrLoggerGeneric = NULL;
    gPtrScrutinyDeviceManager = NULL;
    gPtrLibraryConfigParams = NULL;

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
 * @brief   	Discovers all the supported devices connected
 *
*/

SCRUTINY_STATUS slibiDiscoverDevices (__IN__ U32 DiscoveryFlag, __IN__ PTR_SCRUTINY_DISCOVERY_PARAMS PtrDiscoveryFilter)
{

    SCRUTINY_STATUS status;

    /* Every time when discovery is called, we will have to reinitialize the device. So, we always get the fresh entries
       added. Hence call the device manager and cleanup the device */

    /* NOTE: ldmiFreeDevice will not or should not free the gPtrScrutinyDeviceManager */
    gPtrLoggerGeneric->logiFunctionEntry ("slibiDiscoverDevices (%X, %X)", DiscoveryFlag, (PtrDiscoveryFilter != NULL));

    ldmiFreeDevices (gPtrScrutinyDeviceManager);

    gPtrScrutinyDeviceManager->DiscoveryFlag = DiscoveryFlag;

    if (PtrDiscoveryFilter != NULL)
    {
        gPtrScrutinyDeviceManager->PtrDiscoveryParams = (PTR_SCRUTINY_DISCOVERY_PARAMS) sosiMemAlloc (sizeof (SCRUTINY_DISCOVERY_PARAMS));

        sosiMemCopy (gPtrScrutinyDeviceManager->PtrDiscoveryParams, PtrDiscoveryFilter, sizeof (SCRUTINY_DISCOVERY_PARAMS));
    }

    switch (DiscoveryFlag)
    {
        case SCRUTINY_DISCOVERY_TYPE_SERIAL_DEBUG:
        {
            /* We don't support Serial debug for controller. Hence just break */
            break;
        }

        default:
        {
         
        #if !defined (OS_VMWARE)
            hliFindLSIDevices();
        #endif

            break;
        }
    }


    if ((DiscoveryFlag != SCRUTINY_DISCOVERY_TYPE_OOB_I2C) &&
        (DiscoveryFlag != SCRUTINY_DISCOVERY_TYPE_OOB_PCI))
    {
        bsdiDiscoverDevices();
    }

    gPtrLoggerGeneric->logiDebug ("Number of valid device found %x", gPtrScrutinyDeviceManager->DeviceCount);

    if (gPtrScrutinyDeviceManager->DeviceCount)
    {
        status = SCRUTINY_STATUS_SUCCESS;
    }

    else
    {
        status = SCRUTINY_STATUS_DISCOVERY_FAILURE;
    }

    gPtrLoggerGeneric->logiFunctionExit ("slibiDiscoverDevices (%X)", status);

    return (status);

}

/**
 *
 * @method  	slibiProductHandleToScrutinyDevice ()
 *
 * @param	PtrProductHandle Index of the device for which informaiton has to be fetched
 *
 * @param	PtrDeviceInfo Pointer to device info structure
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Returns the device information
 *
*/

SCRUTINY_STATUS slibiProductHandleToScrutinyDevice (__IN__ PTR_SCRUTINY_PRODUCT_HANDLE PtrProductHandle, __OUT__ PTR_SCRUTINY_DEVICE *PPtrDevice)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;
    U32 index = (*PtrProductHandle - 1);

    /* Check if the library is initialized or not. */

    if (!gPtrScrutinyDeviceManager)
    {
        return (SCRUTINY_STATUS_LIBRARY_NOT_INITIALIZED);
    }

    if (index >= gPtrScrutinyDeviceManager->DeviceCount)
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    ptrDevice = gPtrScrutinyDeviceManager->PtrDeviceList[index];

    if (ptrDevice == NULL)
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }

    *PPtrDevice = ptrDevice;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  	slibiResetDevice ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Reset the device
 *
*/

SCRUTINY_STATUS slibiResetDevice (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    /* Based on the product type, we will have to return the device appropriately. */
    switch (PtrDevice->ProductFamily)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER_IT) || defined (LIB_SUPPORT_CONTROLLER_MR)
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
            return (cdmiResetDevice (PtrDevice));
		#endif

		#if defined (LIB_SUPPORT_CONTROLLER_EXPANDER)
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
            return (edmiResetDevice (PtrDevice));
		#endif
		
		#if defined (LIB_SUPPORT_SWITCH)
        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
            return (sdmiResetDevice (PtrDevice));
		#endif
		
        default:
            return (SCRUTINY_STATUS_UNKNOWN_DEVICE_TYPE);

    }

}


/**
 *
 * @method  	slibiGetFirmwareCliOutput ()
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
 * @brief   	Fetch the CLI command output, supports HBA, Expander & Switch
 *
*/

SCRUTINY_STATUS slibiGetFirmwareCliOutput (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrCommand, __OUT__ PVOID PtrBuffer, __IN__ U32 BufferLength, __IN__ char *PtrFolderName)
{

    switch (PtrDevice->ProductFamily)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER)
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
        {
            if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR ||
                PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN4)
            {
                return (mptiGetFirmwareCliOutput (PtrDevice, PtrCommand, PtrBuffer, BufferLength, PtrFolderName));
            }

            break;
        }
		#endif
		#if defined (LIB_SUPPORT_EXPANDER)
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
        {
            if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF)
            {
                return (eciGetFirmwareCliOutput (PtrDevice, PtrCommand, PtrBuffer, BufferLength, PtrFolderName));
            }

            break;
        }
		#endif
		
		#if defined (LIB_SUPPORT_SWITCH)
        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
        {
            if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF)
            {
                return (eciGetFirmwareCliOutput (PtrDevice, PtrCommand, PtrBuffer, BufferLength, PtrFolderName));
            }

            break;
        }
		#endif

        case SCRUTINY_PRODUCT_FAMILY_INVALID:
        {
            break;
        }

        default:
        {
            break;
        }

    }

    return (SCRUTINY_STATUS_UNSUPPORTED);

}




SCRUTINY_STATUS slibiGetCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      EXP_COREDUMP_TYPE               Type,
    __IN__      const char*                     PtrFolderName
    )
{
    SCRUTINY_STATUS         status;

    switch (PtrDevice->ProductFamily)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER)
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
        {
            if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
            {
                status = ccdiGetCoreDumpImage (PtrDevice, PtrBuffer, PtrBufferSize, PtrFolderName);
                return status;
            }

            break;
        }
		#endif
		#if defined (LIB_SUPPORT_EXPANDER)
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
        {
            status = ecdiGetCoreDump (PtrDevice, PtrBuffer, PtrBufferSize, Type, PtrFolderName);
            return (status);
        }
		#endif
		#if defined (LIB_SUPPORT_SWITCH)
        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
        {
            status = ecdiGetCoreDump (PtrDevice, PtrBuffer, PtrBufferSize, Type, PtrFolderName);
            return (status);
        }
		#endif

        case SCRUTINY_PRODUCT_FAMILY_INVALID:
        {
            break;
        }

        default:
        {
            break;
        }

    }

    return (SCRUTINY_STATUS_UNSUPPORTED);
}

/**
 *
 * @method  slibiGetCoreDumpDataByIndex()
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

SCRUTINY_STATUS slibiGetCoreDumpDataByIndex (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrImageSize,
    __INOUT__   PU32                            PtrIndex,
    __INOUT__   PTR_SCRUTINY_CORE_DUMP_ENTRY    PtrCoreDumpEntry
)
{
    SCRUTINY_STATUS         status = SCRUTINY_STATUS_UNSUPPORTED;
	
	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

    switch (PtrDevice->ProductFamily)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER)
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
        {
            if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
            {
                status = ccdiGetCoreDumpDataByIndex (PtrDevice, PtrBuffer, PtrImageSize, PtrIndex, PtrCoreDumpEntry);

                return status;
            }

            break;
        }
		#endif

        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
        {
            //TODO: Expander coredump support
            break;
        }

        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
        {
            //TODO: Switch coredump support
            break;
        }

        case SCRUTINY_PRODUCT_FAMILY_INVALID:
        {
            break;
        }

        default:
        {
            break;
        }

    }

    return (status);

}

/**
 *
 * @method  slibiEraseCoreDumpImage()
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

SCRUTINY_STATUS slibiEraseCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice
    )
{
    SCRUTINY_STATUS         status;
	
	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

    switch (PtrDevice->ProductFamily)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER)
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
        {
            if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
            {
                status = ccdiEraseCoreDumpData (PtrDevice);
                return (status);
            }

            break;
        }
		#endif
		
		#if defined (LIB_SUPPORT_EXPANDER)
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
        {
            status = ecdiEraseCoreDump (PtrDevice);
            return (status);
        }
		#endif
		
		#if defined (LIB_SUPPORT_SWITCH)
        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
        {
            status = ecdiEraseCoreDump (PtrDevice);
            return (status);
        }
		#endif
        case SCRUTINY_PRODUCT_FAMILY_INVALID:
        {
            break;
        }

        default:
        {
            break;
        }

    }

    return (SCRUTINY_STATUS_UNSUPPORTED);
}


/**
 *
 * @method  slibiGetTraceBuffer()
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

 *
 * @brief   module to get expander, switch trace buffer
 *
 */

SCRUTINY_STATUS slibiGetTraceBuffer (
    __IN__ PTR_SCRUTINY_DEVICE              PtrDevice,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength,
    __IN__ const char*                      PtrFolderName
)
{

    switch (PtrDevice->ProductFamily)
    {
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
        {
            break;
        }
		#if defined (LIB_SUPPORT_EXPANDER)
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
        {
            if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF)
            {

                return (etiGetTraceBuffer (PtrDevice, PtrBuffer, PtrBufferLength, PtrFolderName));
            }
            break;
        }
		#endif
		
		#if defined (LIB_SUPPORT_SWITCH)
        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
        {
            if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF)
            {
                return (etiGetTraceBuffer (PtrDevice, PtrBuffer, PtrBufferLength, PtrFolderName));
            }
			
            break;
        }
		#endif

        case SCRUTINY_PRODUCT_FAMILY_INVALID:
        {
            break;
        }

        default:
        {
            break;
        }

    }

    return (SCRUTINY_STATUS_UNSUPPORTED);
}

/**
 *
 * @method  slibiGetHealthLogs()
 *
 * @param   PtrAdapterHandle    Pointer to Adapter Handle
 *          PtrBuffer           Buffer pointer for trace data
 *          PtrBufferLength     pointer to the buffer length
 *			Flags               indicates how to retrieve health logs
 *          PtrFolderName       pointer to the folder which log file to save
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

SCRUTINY_STATUS slibiGetHealthLogs (
    __IN__ PTR_SCRUTINY_DEVICE              PtrDevice,
    __OUT__ PVOID                           PtrBuffer,
    __IN__ PU32                             PtrBufferLength,
    __IN__  EXP_HEALTH_LOG_TYPE             Flags,
    __IN__ const char*                      PtrFolderName
)
{
    switch (PtrDevice->ProductFamily)
    {
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
        {
            break;
        }

		#if defined (LIB_SUPPORT_EXPANDER)
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
        {
            //TODO: Expander Get Trace buffer Support

            if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
            {

                return (ehliGetHealthLogs (PtrDevice, PtrBuffer, PtrBufferLength, Flags, PtrFolderName));
            }
            break;
        }
		#endif
		
		#if defined (LIB_SUPPORT_SWITCH)

        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
        {
            if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
            {
            
                return (ehliGetHealthLogs (PtrDevice, PtrBuffer, PtrBufferLength, Flags, PtrFolderName));
            }
            break;
        }
		#endif

        case SCRUTINY_PRODUCT_FAMILY_INVALID:
        {
            break;
        }

        default:
        {
            break;
        }

    }

    return (SCRUTINY_STATUS_UNSUPPORTED);
}


SCRUTINY_STATUS slibiGetTemperatureValue (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PU32  PtrTemperature)
{    
    
    /* Based on the product type, we will have to return the device appropriately. */

    switch (PtrDevice->ProductFamily)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER)
        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
    	{
            return (ctrlGetTemperatureValue (PtrDevice, PtrTemperature));
        }
		#endif

		#if defined (LIB_SUPPORT_EXPANDER)
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:            
		{
            return (bsdScsiGetTemperatureValue (PtrDevice, PtrTemperature));
        }
		#endif
		
		#if defined (LIB_SUPPORT_SWITCH)	
        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
		{
            return (bsdScsiGetTemperatureValue (PtrDevice, PtrTemperature));
        }
		#endif
        default:
		{
            return (SCRUTINY_STATUS_UNKNOWN_DEVICE_TYPE);
        }

    }

}

/**
 *
 * @method  	slibiCreateFolder ()
 *
 * @param	PtrDevice 	Pointer to device info structure
 *
 * @param	PtrFolderName  Pointer to current folder
 *
 * @param   PtrDirPath    Folder name to save the logs
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Function to create the directory
 *
*/

SCRUTINY_STATUS slibiCreateFolder (
	__IN__  PTR_SCRUTINY_DEVICE		PtrDevice,    
    __IN__  const char*             PtrFolderName,
    __OUT__ char* 					PtrDirPath    
    )
{
	SCRUTINY_STATUS 	status = SCRUTINY_STATUS_SUCCESS;
	char         		tempName[256];
	char 				timeBuffer[64] = { '\0' };
	char 				folderName[1028] = { '\0' };

    // saving to file
    sosiMemSet (tempName, '\0', sizeof (tempName));

	sosiGetSystemTime (timeBuffer);

	#if defined (OS_LINUX)
	sosiSprintf (tempName,
            	 sizeof (tempName),
	        	 "/scrutiny-logs-%s/",
				 timeBuffer);
	
	#elif defined (OS_WINDOWS)
	sosiSprintf (tempName,
            	sizeof (tempName),
	        	"\\scrutiny-logs-%s\\",
				timeBuffer);
	#endif
	
	sosiStringCopy (folderName, PtrFolderName);

	sosiStringCat (folderName, tempName);

	status = sosiMkDir (folderName);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        return (status);
    }

	sosiStringCopy (PtrDirPath, folderName);
	
	return (status);

}

/**
 *
 * @method  	slibiCreateTargetFolder ()
 *
 * @param	PtrDevice 	Pointer to device info structure
 *
 * @param	PtrFolderName  Pointer to current folder
 *
 * @param   PtrDirPath    Folder name to save the logs
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Function to create the directory
 *
*/

SCRUTINY_STATUS slibiCreateTargetFolder (
	__IN__  PTR_SCRUTINY_DEVICE		PtrDevice,    
    __IN__  const char*             PtrFolderName,
    __OUT__ char* 					PtrDirPath
    )
{
	SCRUTINY_STATUS 	status = SCRUTINY_STATUS_SUCCESS;
	char         		tempName[256];
	char 				folderName[1028] = { '\0' };
	//U64					sasAddress;

    // saving to file
    sosiMemSet (tempName, '\0', sizeof (tempName));
	sosiStringCopy (folderName, PtrFolderName);

    if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
    	#if defined (LIB_SUPPORT_EXPANDER)
	    	#if defined (OS_LINUX)
	        sosiSprintf (tempName, sizeof (tempName), "expander_%08X_%08X/", PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High,
	            		 PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low);
			#elif defined (OS_WINDOWS)
	        sosiSprintf (tempName, sizeof (tempName), "expander_%08X_%08X\\", PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High,
	            		 PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low);
			#endif
		#endif
    }
    else if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
    	#if defined (LIB_SUPPORT_SWITCH)
	        #if defined (OS_LINUX)
	        sosiSprintf (tempName, sizeof (tempName), "switch_%08X_%08X/", PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.High,
	            		 PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.Low);
			#elif defined (OS_WINDOWS)
	        sosiSprintf (tempName, sizeof (tempName), "switch_%08X_%08X\\", PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.High,
	            		 PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.Low);
			#endif
		#endif

    }
	else if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER)
	    	if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
	    	{
	    		//No direct way to get controller SAS address
				mptiGetManPage5 (PtrDevice, &sasAddress);
				
		        #if defined (OS_LINUX)
		        sosiSprintf (tempName, sizeof (tempName), "IOC_%08X_%08X/", sasAddress.High, sasAddress.Low);
				#elif defined (OS_WINDOWS)
		        sosiSprintf (tempName, sizeof (tempName), "IOC_%08X_%08X\\", sasAddress.High, sasAddress.Low);
				#endif
	    	}
			else if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_MR)
			{
				//TODO
			}
		#endif

    }

    sosiStringCat (folderName, tempName);

	status = sosiMkDir (folderName);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {                  
        return (status);
    }

	sosiStringCopy (PtrDirPath, folderName);

	return (status);

}


/**
 *
 * @method  	slibiOneTimeCaptureLogs ()
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
SCRUTINY_STATUS slibiOneTimeCaptureLogs (
    __IN__ PTR_SCRUTINY_DEVICE                PtrDevice,
    __IN__ U32                                Flags, 
    __IN__ const char*                        PtrFolderName
)
{
	if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
    	#if defined (LIB_SUPPORT_CONTROLLER)
		if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
	    {			
			slibiControllerGetLogs (PtrDevice, Flags, PtrFolderName);
	    }
		else if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_MR)
	    {
			//TO_DO: Implement snapdump
	    }
		#endif
	}
	else if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
    	#if defined (LIB_SUPPORT_EXPANDER)
		slibiExpanderGetLogs (PtrDevice, Flags, PtrFolderName);
		#endif
    }
	else if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
    	#if defined (LIB_SUPPORT_SWITCH)
    	slibiSwitchGetLogs (PtrDevice, Flags, PtrFolderName);
		#endif
    }
	else 
    {
        return (SCRUTINY_STATUS_FAILED);
    }

	return (SCRUTINY_STATUS_SUCCESS);
}

#if defined (LIB_SUPPORT_CONTROLLER)

#if defined(OS_UEFI)
/**
 *
 * @method  	slibiControllerHostboot ()
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

SCRUTINY_STATUS slibiControllerHostboot (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU8 PtrFirmware, __IN__ U32 FirmwareLength)
{
    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if ( (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR) ||
         (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_MR))
    {
        return (mpi2hbiHostboot (PtrDevice, PtrFirmware, FirmwareLength));
    }

    else if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN4)
    {
        return (mpi3hbiHostboot (PtrDevice, PtrFirmware, FirmwareLength));
    }

    else
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }
}


/**
 *
 * @method  	slibiControllerGetIocFaultCode ()
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

SCRUTINY_STATUS slibiControllerGetIocFaultCode (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrValue, __IN__ U32 SizeInBytes)
{

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
    {
        if (SizeInBytes != 4)
        {
            return (SCRUTINY_STATUS_INVALID_PARAMETER);
        }

        return (mpi2iEfiSysIfGetIocFaultCode (PtrDevice, PtrValue));
    }

    else if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_MR)
    {
        if (SizeInBytes != 4)
        {
            return (SCRUTINY_STATUS_INVALID_PARAMETER);
        }

        return (mfiiEfiGetIocFaultCode (PtrDevice, PtrValue));
    }

    else if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN4)
    {
        if (SizeInBytes <= 16)
        {
            return (SCRUTINY_STATUS_INVALID_PARAMETER);
        }

        return (mpi3iEfiGetIocFaultCode (PtrDevice, PtrValue));
    }

    else
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

}

/**
 *
 * @method  	slibiControllerGetIocState ()
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

SCRUTINY_STATUS slibiControllerGetIocState (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrValue)
{

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
    {
        return (mpi2iEfiSysIfGetIocState (PtrDevice, PtrValue));
    }

    else if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_MR)
    {
        return (mfiiEfiGetActualMfiState (PtrDevice, PtrValue));
    }

    else if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN4)
    {
        return (mpi3iEfiGetIocState (PtrDevice, PtrValue));
    }

    else
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

}

/**
 *
 * @method  	slibiControllerSendDoorbellMsg ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrMpiRequest Pointer to MPI request
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues MPI call to HBA
 *
*/

SCRUTINY_STATUS slibiControllerSendDoorbellMsg (
	__IN__  PTR_SCRUTINY_DEVICE 	PtrDevice, 
	__IN__	PU32                    PtrRequestMessage,
    __IN__  U32                     RequestSize,
    __OUT__ PU32                    PtrReplyMessage,
    __OUT__ U32                     ReplySize
    )

{
	SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType != SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR &&
        PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType != SCRUTINY_CONTROLLER_TYPE_GEN3_MR)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    status = mpi2iEfiDoorbellSendMessage (PtrDevice,
  	                                      PtrRequestMessage,
 	                                      RequestSize,
 	                                      PtrReplyMessage,
 	                                      ReplySize,
	                                      SCRUTINY_MPI2_EFI_DEFAULT_IO_TIMEOUT);

    return (status);

}

#endif

/**
 *
 * @method  	slibiControllerMpiPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrMpiRequest Pointer to MPI request
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues MPI call to HBA
 *
*/

SCRUTINY_STATUS slibiControllerMpiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_MPI_PASSTHROUGH PtrMpiRequest)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType != SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR &&
        PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType != SCRUTINY_CONTROLLER_TYPE_GEN4)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    return (mptiPerformMpiPassthrough (PtrDevice, PtrMpiRequest));

}


/**
 *
 * @method  	slibiControllerDcmdPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrDcmdRequest Pointer to D-Command request
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues DCMD call to MR
 *
*/

SCRUTINY_STATUS slibiControllerDcmdPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_DCMD_PASSTHROUGH PtrDcmdRequest)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
            return (mfiPerformDcmdPassthrough (PtrDevice, PtrDcmdRequest));

        #if defined(PRODUCT_SUPPORT_AVENGER)
        case SCRUTINY_CONTROLLER_TYPE_GEN4:
            return (mpi3iRaidManagementPassthrough (PtrDevice, PtrDcmdRequest));
        #endif

        default:
            return (SCRUTINY_STATUS_UNSUPPORTED);
    }

}

/**
 *
 * @method  	slibiControllerGetSnapDumpProperty ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	MR_SNAPDUMP_PROPERTIES  Pointer to SnapDump properties MR_SNAPDUMP_INFO
 
 * @param	MR_SNAPDUMP_INFO  Pointer to SnapDump info
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues DCMD call to MR
 *
*/

SCRUTINY_STATUS slibiControllerGetSnapDumpProperty (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ MR_SNAPDUMP_PROPERTIES *PtrSnapProp, __INOUT__ PU8 PtrSnapDumpCount)
	
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

	
    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
            return (mrGetSnapDumpProperty (PtrDevice, PtrSnapProp, PtrSnapDumpCount));

        case SCRUTINY_CONTROLLER_TYPE_GEN4:
            return (SCRUTINY_STATUS_UNSUPPORTED); // Need to add support for Avenger

        default:
            return (SCRUTINY_STATUS_UNSUPPORTED);
    }

}

/**
 *
 * @method  	slibiControllerSetSnapDumpProperty ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	MR_SNAPDUMP_PROPERTIES  Pointer to SnapDump properties MR_SNAPDUMP_INFO
 
 * @param	SnapState  SnapdUmp state indicating on or off
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issue DCMD call to MR
 *
*/

SCRUTINY_STATUS slibiControllerSetSnapDumpProperty (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8  SnapState, __INOUT__ MR_SNAPDUMP_PROPERTIES *PtrSnapProp)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

	
    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
            return (mrSetSnapDumpProperty (PtrDevice, SnapState, PtrSnapProp));

        case SCRUTINY_CONTROLLER_TYPE_GEN4:
            return (SCRUTINY_STATUS_UNSUPPORTED); // Need to add support for Avenger

        default:
            return (SCRUTINY_STATUS_UNSUPPORTED);
    }

}
/**
 *
 * @method  	slibiControllerGetSnapDump ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	SnapDumpID id of the snapdump
 *
 * @param	Foldername  Pointer to foldername where user want to create the zip file 
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issue DCMD call to MR
 *
*/

SCRUTINY_STATUS slibiControllerGetSnapDump(__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8  SnapDumpId, char *PtrFoldername)
{
	   /* Check if we have controller otherwise, we will have to show invalid request */
	 
    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

	
    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
            return (mrGetSnapDump (PtrDevice, SnapDumpId, PtrFoldername));

        case SCRUTINY_CONTROLLER_TYPE_GEN4:
            return (SCRUTINY_STATUS_UNSUPPORTED); // Need to add support for Avenger

        default:
            return (SCRUTINY_STATUS_UNSUPPORTED);
    }
}


/**
 *
 * @method  	slibiControllerTriggerSnapDump ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issue DCMD call to MR
 *
*/

SCRUTINY_STATUS slibiControllerTriggerSnapDump(__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{
	   /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

	
    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
            return (mrTriggerSnapDump (PtrDevice));

        case SCRUTINY_CONTROLLER_TYPE_GEN4:
            return (SCRUTINY_STATUS_UNSUPPORTED); // Need to add support for Avenger

        default:
            return (SCRUTINY_STATUS_UNSUPPORTED);
    }
}


/**
 *
 * @method  	slibiControllerDeleteSnapDump ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issue DCMD call to MR
 *
*/

SCRUTINY_STATUS slibiControllerDeleteSnapDump (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

	
    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
            return (mrDeleteSnapDump (PtrDevice));

        case SCRUTINY_CONTROLLER_TYPE_GEN4:
            return (SCRUTINY_STATUS_UNSUPPORTED); // Need to add support for Avenger

        default:
            return (SCRUTINY_STATUS_UNSUPPORTED);
    }
}

/**
 *
 * @method  	slibiControllerSmpPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrSmpRequest Pointer to SMP request
 *
 * @param	PtrIocStatus Pointer to IOC Status structure, in return it holds IOC Status and log info
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues SMP command
 *
*/

SCRUTINY_STATUS slibiControllerSmpPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_SMP_PASSTHROUGH PtrSmpRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS  PtrIocStatus)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4:
        {
            return (mptiPerformSmpPassthrough (PtrDevice, PtrSmpRequest, PtrIocStatus));
        }

        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
        {
            return (mfiPerformSmpPassthrough (PtrDevice, PtrSmpRequest, PtrIocStatus));
        }

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

}

/**
 *
 * @method  	slibiControllerScsiPassthrough ()
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
 * @brief   	Issues SCSI command
 *
*/

SCRUTINY_STATUS slibiControllerScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 DeviceHandle, __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS  PtrIocStatus)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    /* Just check if we have a proper request */

    switch (PtrScsiRequest->DataDirection)
    {

        case DIRECTION_WRITE:
        case DIRECTION_READ:
        {
            if (PtrScsiRequest->PtrDataBuffer && !PtrScsiRequest->DataBufferLength)
            {
                return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
            }

            break;
        }

        default:
        {
            return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
        }
    }

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4:
        {
            return (mptiPerformScsiPassthrough (PtrDevice, DeviceHandle, PtrScsiRequest, PtrIocStatus));
        }

        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
        {
            return (mfiPerformScsiPassthrough (PtrDevice, DeviceHandle, PtrScsiRequest, PtrIocStatus));
        }

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

}


/**
 *
 * @method  	slibiControllerConfigPagePassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrConfigRequest Pointer to Config request
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues config page passthrough command
 *
*/

SCRUTINY_STATUS slibiControllerConfigPagePassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_CONTROLLER_CFG_PASSTHROUGH PtrCfgRequest)
{

    /* Check if we have controller otherwise, we will have to show invalid request */


    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    switch (PtrCfgRequest->ConfigRegion)
    {
        case MptConfigRegionCurrent:
        case MptConfigRegionDefault:
        case MptConfigRegionNVRAM:
            break;
        default:

            return (SCRUTINY_STATUS_ILLEGAL_REQUEST);

    }

    switch (PtrCfgRequest->RequestDirection)
    {
        case DIRECTION_WRITE:
        case DIRECTION_READ:
            break;
        default:

            return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrCfgRequest->RequestDirection == DIRECTION_WRITE)
    {
        /* For write we should have the data otherwise we will not be able to execute the command. */

        if (!PtrCfgRequest->PtrPageBuffer || !PtrCfgRequest->PageBufferLength)
        {
            return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
        }
    }


    else if (PtrCfgRequest->RequestDirection == DIRECTION_READ)
    {

        if (PtrCfgRequest->PtrPageBuffer == NULL && PtrCfgRequest->PageBufferLength)
        {
            return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
        }

    }

    // Check is to ensure if user has sent EXT Page and not specifying the page type
    if (PtrCfgRequest->PageType == MPI2_CONFIG_PAGETYPE_EXTENDED && !PtrCfgRequest->ExtPageType)
    {

        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }


    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4:
        {
            return (mptiPerformConfigPagePassthrough (PtrDevice, PtrCfgRequest));
        }

        case SCRUTINY_CONTROLLER_TYPE_GEN3_MR:
        {
            return (mfiPerformConfigPagePassthrough (PtrDevice, PtrCfgRequest));
        }

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

}

/**
 *
 * @method  	slibiControllerToolboxClean ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags Region which has to be erased
 *
 * @param	PtrIocStatus Pointer to IOC Status structure, in return it holds IOC Status and log info
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues toolbox clean command
 *
*/

SCRUTINY_STATUS slibiControllerToolboxClean (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Flags, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4 :
        {
            return (mptiSendToolboxClean (PtrDevice, Flags, PtrIocStatus));
        }

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

}

/**
 *
 * @method  	slibiControllerGetDriveBusTarget ()
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
 * @brief   	Module to fetch Bus/Device/Target number. 
 *
*/

SCRUTINY_STATUS slibiControllerGetDriveBusTarget (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PU16 PtrDevHandle, __OUT__ PU32 PtrBus, __OUT__ PU32 PtrTarget)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{		
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

	#if !defined (OS_UEFI)

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        {
            return (mptiGetDriveBusTarget (PtrDevice, PtrDevHandle, PtrBus, PtrTarget));
        }

        case SCRUTINY_CONTROLLER_TYPE_GEN4:
        {
            //TODO: For Gen 4 Adapters, We have PersistenceID maintained in FW for a Target device
            //      We need to add one more parameter for PersistenceID as well. From the
            //      application, user can give persistence ID to get the bus, target and dev handle infor
            return (mpi3iGetDriveBusTarget (PtrDevice, PtrDevHandle, PtrBus, PtrTarget));
        }


        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

    #else
        return (SCRUTINY_STATUS_UNSUPPORTED);

    #endif

}

/**
 *
 * @method  	slibiControllerHealth ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	Flags Based on the flags health of the HBA will be fetched
 *
 * @param	PtrControllerInventory Pointer to ControllerHealth structure which return the health of the HBA
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Module to fetch health of the HBA
 *
*/

SCRUTINY_STATUS slibiControllerHealth (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Flags, __OUT__ PTR_SCRUTINY_CONTROLLER_INVENTORY PtrScrutinyInventory)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

        case SCRUTINY_CONTROLLER_TYPE_GEN4:
        {
             return (SCRUTINY_STATUS_SUCCESS);
        }


        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

}


/**
 *
 * @method  	slibiControllerHealthCheck ()
 *
 * @param	PtrDevice Device Handle
 *
 * @param	PtrFolderName  Folder for the input ini file and output file
  *
 * @param   DumpToFile     whether dump to output file
 *
 * @param	PtrErrorCode   Error code indicates which tag failed to compare
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	internal API to check the health of the HBA
 *
*/
SCRUTINY_STATUS slibiControllerHealthCheck (
    __IN__ PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__ const char*         PtrFolderName,
    __IN__  BOOLEAN            DumpToFile,
    __OUT__ PU32               PtrErrorCode
)
{
	SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;
    
    if ( (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER) || 
        (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType !=SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR))
    {
        return (SCRUTINY_STATUS_UNSUPPORTED);
    }

#if !defined (OS_UEFI)

	status = chmControllerHealthCheck (PtrDevice, PtrFolderName, DumpToFile, PtrErrorCode);

#endif

    return (status);
    
}


/**
 *
 * @method  	slibiControllerGetSasPhyErrorCounters ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PhyId Phy number
 *
 * @param	PtrSasPhyErrCounter Pointer to error counter structure
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Module to fetch SAS Error counter of the device
 *
*/

SCRUTINY_STATUS slibiControllerGetSasPhyErrorCounters (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PhyId, __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS PtrSasPhyErrCounter)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        {
            return (mptiGetSasPhyErrorCounters (PtrDevice, PhyId, PtrSasPhyErrCounter));
        }

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

}

/**
 *
 * @method  	slibiControllerGetPciPhyErrorCounters ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PhyId Phy number
 *
 * @param	PtrPciePhyErrCounter Pointer to error counter structure
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Module to fetch PCI Error counter of the device
 *
*/

SCRUTINY_STATUS slibiControllerGetPciPhyErrorCounters (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PhyId, __OUT__ PTR_SCRUTINY_PCI_LINK_ERROR_COUNTERS PtrPciPhyErrCounter)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        {
            /* Check if we have NVMe Enabled firmware */
            if (!PtrDevice->DeviceInfo.u.ControllerInfo.u.Gen3.IT.IsNvmeEnabledFirmware)
            {
                return (SCRUTINY_STATUS_UNSUPPORTED);
            }

            return (mptiGetPciPhyErrorCounters (PtrDevice, PhyId, PtrPciPhyErrCounter));
        }

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }

    }

}

/**
 *
 * @method  	slibiControllerSetCoffeeFault ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Module to issue coffee fault
 *
*/

SCRUTINY_STATUS slibiControllerSetCoffeeFault (__IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

	switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
	{
		case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
		{
			return (mptiSetCoffeeFault (PtrDevice));
		}

		default:
		{
			return (SCRUTINY_STATUS_UNSUPPORTED);
		}

	}

}


/**
 *
 * @method  slibiControllerToolboxIstwiReadWrite()
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

SCRUTINY_STATUS slibiControllerToolboxIstwiReadWrite (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_ISTWI_REQUEST PtrIstwiRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4 :
        {
            return (mptiSendToolboxIstwiReadWrite (PtrDevice, PtrIstwiRequest, PtrIocStatus));
        }

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }
    }

}

/**
 *
 * @method  slibiControllerToolboxIstwiReadWrite()
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

SCRUTINY_STATUS slibiControllerToolboxLaneMargin (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_LANE_MARGIN_REQUEST PtrLaneMarginRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4 :
        {
            return (mptiSendToolboxLaneMargin (PtrDevice, PtrLaneMarginRequest, PtrIocStatus));
        }

		case SCRUTINY_CONTROLLER_TYPE_GEN3_MR :
            return (mfiPerformToolboxLaneMargin (PtrDevice, PtrLaneMarginRequest, PtrIocStatus));

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }
    }

}


SCRUTINY_STATUS slibiControllerToolboxMemoryRead (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4 :
        {
            return (mptiSendToolBoxMemoryRead (PtrDevice, PtrMemoryOperationRequest, PtrIocStatus));
        }

		case SCRUTINY_CONTROLLER_TYPE_GEN3_MR :
            return (mfiPerformToolBoxMemoryRead (PtrDevice, PtrMemoryOperationRequest, PtrIocStatus));

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }
    }

}


SCRUTINY_STATUS slibiControllerToolboxMemoryWrite (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_MEMORY_OPERATION_REQUEST PtrMemoryOperationRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4 :
        {
            return (mptiSendToolBoxMemoryWrite (PtrDevice, PtrMemoryOperationRequest, PtrIocStatus));
        }

		case SCRUTINY_CONTROLLER_TYPE_GEN3_MR :
            return (mfiPerformToolBoxMemoryWrite (PtrDevice, PtrMemoryOperationRequest, PtrIocStatus));

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }
    }

}



/**
 *
 * @method  slibiControllerToolboxIstwiReadWrite()
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

SCRUTINY_STATUS slibiControllerSpdmOperations (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SPDM_REQUEST PtrSpdmRequest, __OUT__ PTR_SCRUTINY_IOC_STATUS PtrIocStatus)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
	if (PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC)
	{
		return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
	}

    switch (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType)
    {
        case SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR:
        case SCRUTINY_CONTROLLER_TYPE_GEN4 :
        {
            return (mptiSendToolboxSpdmRequest (PtrDevice, PtrSpdmRequest, PtrIocStatus));
        }

		case SCRUTINY_CONTROLLER_TYPE_GEN3_MR :
            //return (mfiPerformToolboxLaneMargin (PtrDevice, PtrLaneMarginRequest, PtrIocStatus));

        default:
        {
            return (SCRUTINY_STATUS_UNSUPPORTED);
        }
    }

}

/**
 *
 * @method  	slibiGetControllerLogs ()
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
SCRUTINY_STATUS slibiControllerGetLogs (
    __IN__ PTR_SCRUTINY_DEVICE                PtrDevice,
    __IN__ U32                                Flags, 
    __IN__ const char*                        PtrFolderName
)
{
    SCRUTINY_STATUS     status;
#if !defined (OS_UEFI)
    U16                 flags;
    U32                 bufferSize;
    U32                 uniqueId;
    PU32                ptrBuffer;
    SOSI_FILE_HANDLE    fileHandle;
    char                logFileName[512];
#endif

    if (PtrFolderName == NULL)
    {
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    if (PtrDevice->DeviceInfo.u.ControllerInfo.ControllerType != SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
    {
        return (SCRUTINY_STATUS_INVALID_HANDLE);
    }
    

    if (Flags & SCRUTINY_CTRL_LOGS_BIT_CORE_DUMP)
    {
        status = (slibiGetCoreDump (PtrDevice, NULL, NULL, 0, PtrFolderName));
		
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            //return (status);
        }
    }

#if !defined (OS_UEFI)
    if (Flags & SCRUTINY_CTRL_LOGS_BIT_HOST_TRACE_BUFFER)
    {
        status = mptiDiagBufferQuery (PtrDevice, MPI2_DIAG_BUF_TYPE_TRACE, &flags, &bufferSize, &uniqueId);
		
        if ((status != SCRUTINY_STATUS_SUCCESS) || (uniqueId == 0))
        {
        	/* No buffer registered, so returning here itself */
			
            return (status);
        } 

        ptrBuffer = (PU32) sosiMemAlloc (bufferSize);
		
        if (ptrBuffer == NULL)
        {
            return (SCRUTINY_STATUS_NO_MEMORY);
        }

        sosiMemSet (logFileName, '\0', sizeof (logFileName));
        
        status = sosiMkDir (PtrFolderName);
        
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            sosiMemFree (ptrBuffer);
            return (status);
        }

        sosiStringCopy (logFileName, PtrFolderName);
        sosiStringCat ((char *)logFileName, "htb.bin");
        fileHandle = sosiFileOpen (logFileName, "wb");

        status = mptiDiagBufferRead (PtrDevice, MPI2_DIAG_BUF_TYPE_TRACE, &bufferSize, ptrBuffer, &uniqueId);

        if (status == SCRUTINY_STATUS_SUCCESS)
        {
            sosiFileWrite (fileHandle, (PU8)ptrBuffer, bufferSize);
        }

        sosiMemFree (ptrBuffer);
        sosiFileClose (fileHandle);
    }
#endif
 
    return (SCRUTINY_STATUS_SUCCESS);
}

#endif

#if defined (LIB_SUPPORT_EXPANDER)

/**
 *
 * @method  	slibiExpanderScsiPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrScsiRequest Pointer to SCSI request structure which holds the SCSI command to be fired
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues SCSI command to Expander
 *
*/

SCRUTINY_STATUS slibiExpanderScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if ((PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER) && (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH))
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        return (bsdiPerformScsiPassthrough (PtrDevice, PtrScsiRequest));
    }

    return (SCRUTINY_STATUS_FAILED);

}


/**
 *
 * @method  	slibiExpanderGetConfigPassthrough ()
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
 * @brief   	Issues config command
 *
*/

SCRUTINY_STATUS slibiExpanderGetConfigPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U16 Page, __IN__ U8 Region, __OUT__ PU8 *PtrValue, __OUT__ PU32 PtrSize)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if ((PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER) && (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH))
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        return (bsdiGetConfigPage (PtrDevice, Page, Region, PtrValue, PtrSize));
    }

    return (SCRUTINY_STATUS_FAILED);

}


SCRUTINY_STATUS  slibiExpanderEnableDisablePhy (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier,  __IN__ BOOLEAN  PhyEnable)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

	/*
      Use SMP_PHY_ CONTROL to implement this feature.
      According to SMP spec, if a phy is disabled ,it need use link reset or hard reset to enable it.
      Let firmware check if PhyIdentifier out of range?    
     */

	if (PhyEnable)
    {
        return (expiEnableDisablePhy (PtrDevice, PhyIdentifier, SMP_PHY_CONTROL_PHY_OPERATION_LINK_RESET));
    }
    else
    {
        return (expiEnableDisablePhy ( PtrDevice, PhyIdentifier, SMP_PHY_CONTROL_PHY_OPERATION_DISABLE));
    }

//	return (SCRUTINY_STATUS_UNSUPPORTED);
}


/**
 *
 * @method  	slibiExpanderGetHealth ()
 *
 * @param	PtrDevice            Device Handle
 *
 * @param	PtrExpHealthInfo     Pointer to ExpanderHealth structure which return the health of the Expander
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the health of the Expander
 *
*/

SCRUTINY_STATUS  slibiExpanderGetHealth ( 
    __IN__   PTR_SCRUTINY_DEVICE                PtrDevice, 
    __OUT__  PTR_SCRUTINY_EXPANDER_HEALTH       PtrExpHealthInfo
)
{
	SCRUTINY_STATUS		status = SCRUTINY_STATUS_SUCCESS;
	
    /* Check if we have Expander, otherwise, we will have to show invalid request */
    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

#if !defined (OS_UEFI)

	status = ehmiGetHealthData (PtrDevice, PtrExpHealthInfo);

#endif
	
    return (status);
}



/**
 * @method  	slibiGetExpanderLogs ()
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
SCRUTINY_STATUS slibiExpanderGetLogs (
    __IN__ PTR_SCRUTINY_DEVICE                  PtrDevice,
    __IN__ U32                                  Flags, 
    __IN__ const char*                          PtrFolderName
)
{
    SCRUTINY_STATUS     status, finalStatus = SCRUTINY_STATUS_SUCCESS;    
    char *              ptrCommand = "debuginfo";

    if (PtrFolderName == NULL)
    {
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_TRACE_BUFFER) 
    {
        status = slibiGetTraceBuffer (PtrDevice, NULL, NULL, PtrFolderName);		
        finalStatus |= status;
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_HEALTH_LOG) 
    {
        /* Alway get RAW health log because it's supported via both inband and SDB */
        status = slibiGetHealthLogs (PtrDevice, NULL, NULL, EXP_HEALTH_LOGS_DECODED, PtrFolderName);		
        finalStatus |= status; 
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_CORE_DUMP)
    {
        status = slibiGetCoreDump (PtrDevice, NULL, NULL, EXP_COREDUMP_DECODED, PtrFolderName);
        if (status == SCRUTINY_STATUS_NO_COREDUMP)
        {
            status = SCRUTINY_STATUS_SUCCESS;
        }
        
        finalStatus |= status;
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_FW_CLI)
    {
        /* Now the cli command is hard coded, in future, consider put in the configuration file */
        status = slibiGetFirmwareCliOutput (PtrDevice, (PU8)ptrCommand, NULL, 0, (char *)PtrFolderName);
        finalStatus |= status;
    }

    if (finalStatus != SCRUTINY_STATUS_SUCCESS)
    {
        finalStatus = SCRUTINY_STATUS_FAILED;
    }
    return (finalStatus);

}


SCRUTINY_STATUS slibiExpanderFlashMemoryDownload (
    __IN__ PTR_SCRUTINY_DEVICE    PtrDevice,
    __IN__ U32                    FlashAddress,
    __IN__ PU8                    PtrBuffer,
    __IN__ U32                    BufferSize,
    __IN__ ScrutinyLogCallBack                      CallBack)
{

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrDevice->HandleType != SCRUTINY_HANDLE_TYPE_SDB)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }


    //return (ediPerformBoostrapDownload (PtrDevice, FlashAddress, PtrBuffer, BufferSize, CallBack));
	return (SCRUTINY_STATUS_UNSUPPORTED);
}





/**
 *
 *  @method  slibiExpanderResetPhy ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PhyOperation            Phy control operation,=1 link reset, =2 hard reset =3 clear error counter 
 *
 *  @param   DoAllPhys               if the operation for single phy or all phys,=TRUE for all phys, =FALSE for single phy identified by  PhyIdentifier
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *
 *  @brief                           This method do some phy control operation on expander phy by SMP 
 *
 */


SCRUTINY_STATUS slibiExpanderResetPhy (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __IN__ U8 PhyOperation, __IN__ BOOLEAN DoAllPhys)
{
    /* Check if we have expander otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (expiResetPhy ( PtrDevice, PhyIdentifier, PhyOperation, DoAllPhys));

}

/**
 *
 *  @method  slibiExpanderGetPhyErrorCounters ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PhyIdentifier           Phy index
 *
 *  @param   PtrSasPhyErrCounter     Phy error counter value returned. 
 *
 * @return  SCRUTINY_STATUS     SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get  phy error counters on expander phy ,InvalidDwordCount,RunningDisparityErrorCount,LossDwordSynchCount,PhyResetProblemCount
 *
 */

SCRUTINY_STATUS slibiExpanderGetPhyErrorCounters (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U8 PhyIdentifier, __OUT__ PTR_SCRUTINY_SAS_ERROR_COUNTERS PtrSasPhyErrCounter)
{
    /* Check if we have expander otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (expiGetPhyErrCounters ( PtrDevice, PhyIdentifier, PtrSasPhyErrCounter));

}



/**
 *
 *  @method  slibiExpanderHealthCheck ()
 *
 *  @param   PtrDevice               pointer to  device
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

SCRUTINY_STATUS  slibiExpanderHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32  PtrErrorCode)
{
	SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;
	
    /* Check if we have expander otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

#if !defined (OS_UEFI)

	status = ehmiExpanderHealthCheck ( PtrDevice, PtrConfigFilePath, DumpToFile, PtrErrorCode);

#endif
	
    return (status);

}

#endif

#if defined (LIB_SUPPORT_SWITCH) || defined (LIB_SUPPORT_EXPANDER)

/**
 *
 * @method  	slibiBroadcomScsiMemoryWrite ()
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

SCRUTINY_STATUS slibiBroadcomScsiMemoryWrite (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,
                                              __IN__ U32 Address,
                                              __IN__ PVOID PtrValue,
                                              __IN__ U32 SizeInBytes)
{

    U32 index, offset;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    /* Check if we have controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER &&
        PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (SizeInBytes % 4 == 0)
    {

        for (index = 0, offset = 0; index < (SizeInBytes / 4); index++, offset += 4)
        {
            status = bsdiMemoryWrite32 (PtrDevice, (Address + offset), &((PU32) PtrValue)[index]);

            if (status)
            {
                break;
            }
        }

    }

    else if (SizeInBytes % 2 == 0)
    {

        for (index = 0, offset = 0; index < (SizeInBytes / 2); index++, offset += 2)
        {
            status = bsdiMemoryWrite16 (PtrDevice, (Address + offset), &((PU16) PtrValue)[index]);

            if (status)
            {
                break;
            }
        }

    }

    else
    {

        for (index = 0, offset = 0; index < SizeInBytes; index++, offset++)
        {
            status = bsdiMemoryWrite8 (PtrDevice, (Address + offset), &((PU8) PtrValue)[index]);

            if (status)
            {
                break;
            }
        }

    }

    return (status);

}


/**
 *
 * @method  	slibiBroadcomScsiMemoryRead ()
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
SCRUTINY_STATUS slibiBroadcomScsiMemoryRead  (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,
                                              __IN__ U32 Address,
                                              __OUT__ PVOID PtrValue,
                                              __IN__ U32 SizeInBytes)
{

    /* Check if we have Expander/Controller otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER &&
        PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (SizeInBytes % 4 == 0)
    {
        return (bsdiMemoryRead32 (PtrDevice, Address, (PU32) PtrValue, SizeInBytes));
    }

    else if (SizeInBytes % 2 == 0)
    {
        return (bsdiMemoryRead16 (PtrDevice, Address, (PU16) PtrValue, SizeInBytes));
    }

    else
    {
        return (bsdiMemoryRead8 (PtrDevice, Address, (PU8) PtrValue, SizeInBytes));
    }

}

#endif

#if defined (LIB_SUPPORT_SWITCH)

/**
 *
 * @method  	slibiSwitchScsiPassthrough ()
 *
 * @param	PtrProductHandle Device Handle
 *
 * @param	PtrScsiRequest Pointer to SCSI request structure which holds the SCSI command to be fired
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	Issues SCSI command to Switch
 *
*/

SCRUTINY_STATUS slibiSwitchScsiPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __INOUT__ PTR_SCRUTINY_SCSI_PASSTHROUGH PtrScsiRequest)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if ((PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER) && (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH))
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        return (bsdiPerformScsiPassthrough (PtrDevice, PtrScsiRequest));
    }

    return (SCRUTINY_STATUS_FAILED);

}


/**
 *
 * @method  	slibiSwitchGetConfigPassthrough ()
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
 * @brief   	Issues config command
 *
*/

SCRUTINY_STATUS slibiSwitchGetConfigPassthrough (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U16 Page, __IN__ U8 Region, __OUT__ PU8 *PtrValue, __OUT__ PU32 PtrSize)
{
    /* Check if we have controller otherwise, we will have to show invalid request */

    if ((PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_EXPANDER) && (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH))
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

    if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF ||
        PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        return (bsdiGetConfigPage (PtrDevice, Page, Region, PtrValue, PtrSize));
    }

    return (SCRUTINY_STATUS_FAILED);

}



/**
 *
 * @method  slibiSwitchGetPciPortErrorStatistic()
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

SCRUTINY_STATUS slibiSwitchGetPciPortErrorStatistic (
    __IN__ PTR_SCRUTINY_DEVICE  PtrDevice, 
    __IN__ U32                  Port, 
    __OUT__ PTR_SCRUTINY_SWITCH_ERROR_STATISTICS PtrPciePortErrStatistic)
{
    switch (PtrDevice->ProductFamily)
    {
        case SCRUTINY_PRODUCT_FAMILY_SWITCH:
        {
            if (PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF  ||
                PtrDevice->HandleType == SCRUTINY_HANDLE_TYPE_PCI)
            {
            
                return (spciGetPciPortErrorStatistic (PtrDevice, Port, PtrPciePortErrStatistic));
            }
			
            break;
        }

        case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
        case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
        case SCRUTINY_PRODUCT_FAMILY_INVALID:
        {
            break;
        }

        default:
        {
            break;
        }

    }

    return (SCRUTINY_STATUS_UNSUPPORTED);
}



/**
 *
 *  @method  ScrutinySwitchGetPciePortProperties ()
 *
 *  @param   PtrDevice               pointer to  device
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


SCRUTINY_STATUS slibiSwitchGetPciePortProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties)
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (sppGetPciePortProperties (PtrDevice, PtrPciePortProperties));

}



/**
 *
 *  @method  slibiSwitchHealthCheck ()
 *
 *  @param   PtrDevice               pointer to  device
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


SCRUTINY_STATUS  slibiSwitchHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32 PtrErrorCode)
{

	SCRUTINY_STATUS		status = SCRUTINY_STATUS_SUCCESS;
	
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

#if !defined (OS_UEFI)

	status = shmiSwitchHealthCheck ( PtrDevice, PtrConfigFilePath, DumpToFile, PtrErrorCode);

#endif
	
    return (status);

}

#if !defined (OS_UEFI)

/**
 *
 *  @method  slibiSwitchRxEqStatus ()
 *
 *  @param   PtrDevice               pointer to  device
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

SCRUTINY_STATUS slibiSwitchRxEqStatus (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS PtrPortRxEqStatus)
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (ssiRxEqStatus ( PtrDevice, StartPort, NumberOfPort, PtrPortRxEqStatus));

}

/**
 *
 *  @method  slibiSwitchTxCoeff ()
 *
 *  @param   PtrDevice                  pointer to  device
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

SCRUTINY_STATUS slibiSwitchTxCoeff (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF PtrPortTxCoeffStatus)
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (ssiTxCoeff ( PtrDevice, StartPort, NumberOfPort, PtrPortTxCoeffStatus));

}

/**
 *
 *  @method  slibiSwitchHardwareEyeStart ()
 *
 *  @param   PtrDevice                  pointer to  device
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


SCRUTINY_STATUS slibiSwitchHardwareEyeStart( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (ssiHardwareEyeStart ( PtrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  slibiSwitchHardwareEyePoll ()
 *
 *  @param   PtrDevice                  pointer to  device
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

SCRUTINY_STATUS slibiSwitchHardwareEyePoll( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (ssiHardwareEyePoll ( PtrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  slibiSwitchHardwareEyeGet ()
 *
 *  @param   PtrDevice                  pointer to  device
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

SCRUTINY_STATUS slibiSwitchHardwareEyeGet( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (ssiHardwareEyeGet ( PtrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  slibiSwitchHardwareEyeClean ()
 *
 *  @param   PtrDevice                  pointer to  device
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

SCRUTINY_STATUS slibiSwitchHardwareEyeClean( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (ssiHardwareEyeClean ( PtrDevice, StartPort, NumberOfPort, PtrPortHwEyeStatus));

}

/**
 *
 *  @method  slibiSwitchSoftwareEye ()
 *
 *  @param   PtrDevice                  pointer to  device
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

SCRUTINY_STATUS slibiSwitchSoftwareEye( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort)
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (ssiSoftwareEye ( PtrDevice, StartPort, NumberOfPort));

}

#endif

/**
 *
 *  @method  slibiSwitchGetPowerOnSense ()
 *
 *  @param   PtrDevice                  pointer to  device
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

SCRUTINY_STATUS slibiSwitchGetPowerOnSense (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__   PTR_SCRUTINY_SWITCH_POWER_ON_SENSE PtrSwPwrOnSense )
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (srbiGetPowerOnSense ( PtrDevice, PtrSwPwrOnSense));

}

/**
 *
 *  @method  slibiSwitchGetCcrStatus ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PtrSwCcrStatus             Pointer to CCR status.
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

SCRUTINY_STATUS slibiSwitchGetCcrStatus (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__   PTR_SCRUTINY_SWITCH_CCR_STATUS PtrSwCcrStatus )
{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    return (srbiGetCcrStatus ( PtrDevice, PtrSwCcrStatus));

}


 /**
 *
 * @method  	slibiSwitchGetHealth ()
 *
 * @param	PtrDevice            Device Handle
 *
 * @param	PtrExpHealthInfo     Pointer to Switch Health structure which return the health of the Switch
 *
 * @return   	LIB_status  Indicating Success or Fail
 *
 * @brief   	API to fetch the health of the Switch
 *
*/

SCRUTINY_STATUS  slibiSwitchGetHealth ( 
    __IN__   PTR_SCRUTINY_DEVICE                PtrDevice, 
    __OUT__  PTR_SCRUTINY_SWITCH_HEALTH         PtrSwHealthInfo
)
{
	 SCRUTINY_STATUS	 status = SCRUTINY_STATUS_SUCCESS;

    /* Check if we have Expander, otherwise, we will have to show invalid request */
    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

#if !defined (OS_UEFI)

	status = shmiGetHealthData (PtrDevice, PtrSwHealthInfo);

#endif
	
    return (status);
}


/**
 *
 * @method  	slibiGetSwitchLogs ()
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
SCRUTINY_STATUS slibiSwitchGetLogs (
    __IN__ PTR_SCRUTINY_DEVICE                PtrDevice,
    __IN__ U32                                Flags, 
    __IN__ const char*                        PtrFolderName
)
{
    SCRUTINY_STATUS     status, finalStatus = SCRUTINY_STATUS_SUCCESS;
    
    if (PtrFolderName == NULL)
    {
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_TRACE_BUFFER) 
    {
        status = slibiGetTraceBuffer (PtrDevice, NULL, NULL, PtrFolderName);
		
        finalStatus |= status; 
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_CORE_DUMP)
    {
        status = slibiGetCoreDump (PtrDevice, NULL, NULL, EXP_COREDUMP_DECODED, PtrFolderName);
        if (status == SCRUTINY_STATUS_NO_COREDUMP)
        {
            status = SCRUTINY_STATUS_SUCCESS;
        }
        finalStatus |= status;
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_HEALTH_LOG) 
    {
        /* Alway get RAW health log because it's supported via both inband and SDB */
        status = slibiGetHealthLogs (PtrDevice, NULL, NULL, EXP_HEALTH_LOGS_RAW, PtrFolderName);
		
        finalStatus |= status;
    }

    if (Flags & SCRUTINY_EXP_LOGS_BIT_FW_CLI)
    {
        //TODO: Not sure what commands to be implemented.
        
    }
    
    if (finalStatus != SCRUTINY_STATUS_SUCCESS)
    {
        finalStatus = SCRUTINY_STATUS_FAILED;
    }
    
    return (finalStatus);
}


/**
 *
 * @method slibiSwitchGetLaneMarginCapacities()
 *
 * @param    PtrDevice 						pointer to  device
 *           SwtichPort						Selected switch port
 *			 Lanes           				selected lane in the port
 *           PtrMarginControlCapabilities   margin control capabilities supported 
 *           PtrNumSteps                    the higher significant 16 bits are NumVoltageSteps and the lower significant 16 bits are NumTimingSteps
 *           PtrMaxOffset                   the higher significant 16 bits are MaxVoltageOffset and the lower significant 16 bits are MaxTimingOffset
 *
 * @return   SCRUTINY_STATUS_SUCCESS                - PCIe Lane Margining was executed successfully
 *           SCRUTINY_STATUS_FAILED                    - on failure.
 *           SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *           SCRUTINY_STATUS_UNSUPPORTED     - the input device is not switch device
 *           SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 * @brief     internal retrievethe lane margining capacities in the selected lane of selected port
 *
 */

SCRUTINY_STATUS slibiSwitchGetLaneMarginCapacities (
    __IN__ PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__ U8 SwtichPort,  
    __IN__ U16 Lane,
    __OUT__ PU32 PtrMarginControlCapabilities,
    __OUT__ PU32 PtrNumSteps,
    __OUT__ PU32 PtrMaxOffset
 )
{
    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }

#if !defined (OS_BMC)
    return (slmGetLaneMarginCapacities (PtrDevice, SwtichPort, Lane, PtrMarginControlCapabilities, PtrNumSteps, PtrMaxOffset));
#else
    return (SCRUTINY_STATUS_UNSUPPORTED);
#endif
}

/**
 *
 * @method slibiSwitchPerformLaneMargining()
 *
 * @param	PtrProductHandle		  Pointer to Adapter Handle
 *			PtrSwitchLaneMarginReqÂ   PCIe Lane margin input parameters
 *			PtrSwitchLaneMarginRespÂ  PCIe Lane margin input if the margin was executed.
 *
 * @return	SCRUTINY_STATUS_SUCCESS                - PCIe Lane Margining was executed successfully
 *			SCRUTINY_STATUS_FAILED                    - on failure.
 *			SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *			SCRUTINY_STATUS_UNSUPPORTED     - the input device is not switch device
 *			SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *			SCRUTINY_STATUS_INVALID_LM_REQUEST - Input port number is invalid or the port is not enabled, or the link  
 *												is not in L0 state, or the input lane number is invalid
 *
 * @brief	internal function to perform the lane margining in the selected lane of selected port
 *
 */


SCRUTINY_STATUS slibiSwitchPerformLaneMargining    (
    __IN__ PTR_SCRUTINY_DEVICE                      PtrDevice, 
    __IN__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_REQUEST  PtrSwitchLaneMarginReq,
    __OUT__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE PtrSwitchLaneMarginResp 
    )
{
    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
    
#if !defined (OS_BMC)
    return (slmPerformLaneMargining(PtrDevice, PtrSwitchLaneMarginReq, PtrSwitchLaneMarginResp));
#else
    return (SCRUTINY_STATUS_UNSUPPORTED);
#endif
}
 

#if defined(OS_LINUX)


/**
 *
 *  @method  ScrutinySwitchSetAladinConfiguration ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrConfigStatus   Pointer to Aladin configuration status.
 *	
 *  @param   PtrConfigFilePath  Pointer to configuration ini file path
 *
 *  @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method configures Aladin Capture based on the Aladin.ini file provided. 
 *
 */


SCRUTINY_STATUS  slibiSwitchSetAladinConfiguration (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __OUT__ PU32 PtrConfigStatus)

{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
    return (satcSetAladinConfiguration (PtrDevice, PtrConfigFilePath, PtrConfigStatus));
}

/**
 *
 *  @method  ScrutinySwitchGetAladinTracedump ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrDumpStatus   Pointer to Aladin trace dump status.
 *	
 *  @param   PtrConfigFilePath  Pointer to configuration ini file path
 *
 *  @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief       This method dumps Aladin trace captured to output file. 
 *
 */


SCRUTINY_STATUS  slibiSwitchGetAladinTracedump (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrDumpFilePath, __OUT__ PU32 PtrDumpStatus)

{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
    return (satcGetAladinTracedump (PtrDevice, PtrDumpFilePath, PtrDumpStatus));
}

/**
 *
 *  @method  ScrutinySwitchPollAladinTrigger ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrTriggerStatus   Pointer to Aladin trigger status.
 *	
 *  @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief       This method polls for the tirgger. 
 *
 */


SCRUTINY_STATUS  slibiSwitchPollAladinTrigger (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrTriggerStatus)

{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
    return (satcPollAladinTrigger (PtrDevice, PtrTriggerStatus));
}

/**
 *
 *  @method  ScrutinySwitchStopAladinCapture ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PtrStopStatus   Pointer to Aladin trace manual stop  status.
 *	
 *  @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief       This method manually stops the trace capture. 
 *
 */


SCRUTINY_STATUS  slibiSwitchStopAladinTrace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrStopStatus)

{
    /* Check if we have switch otherwise, we will have to show invalid request */

    if (PtrDevice->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        return (SCRUTINY_STATUS_ILLEGAL_REQUEST);
    }
	
    return (satcStopAladinTrace (PtrDevice, PtrStopStatus));
}


#endif


#endif

