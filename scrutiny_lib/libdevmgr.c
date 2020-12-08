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
 * @method  ldmiInitializeLibraryDeviceManager ()
 *
 * @param   PTR_SCRUTINY_DEVICE_MANAGER - Pointer to Pointer to device Manager structure
 *
 * @return  lsi_status  Indicating Success or Fail
 *
 * @brief   Adapter will be stored in stack data structure
 *
*/

SCRUTINY_STATUS ldmiInitializeLibraryDeviceManager (__INOUT__ PTR_SCRUTINY_DEVICE_MANAGER *PPtrDeviceManager)
{

    if (*PPtrDeviceManager)
    {
        /* We have already initialized the library so just return the same*/
        return (SCRUTINY_STATUS_LIBRARY_ALREADY_INITIALIZED);
    }

    *PPtrDeviceManager = (PTR_SCRUTINY_DEVICE_MANAGER) sosiMemAlloc (sizeof (SCRUTINY_DEVICE_MANAGER));

    if (!*PPtrDeviceManager)
    {
        return (SCRUTINY_STATUS_NO_MEMORY);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  ldmiFreeDevices ()
 *
 * @param   PTR_SCRUTINY_DEVICE_MANAGER - Pointer to device manger structure
 *
 * @return  lsi_status  Indicating Success or Fail
 *
 * @brief   Adapter will be stored in stack data structure
 *
*/
 
SCRUTINY_STATUS ldmiFreeDevices (__INOUT__ PTR_SCRUTINY_DEVICE_MANAGER PtrDeviceManager)
{

    U32 index;

    if (!PtrDeviceManager->DeviceCount)
    {
        /* We don't have any devices in the list. Hence just return the success */
        return (SCRUTINY_STATUS_SUCCESS);
    }

    /* First we need to close all the devices then we will clear the devices. */
    for (index = 0; index < PtrDeviceManager->DeviceCount; index++)
    {

        switch (PtrDeviceManager->PtrDeviceList[index]->ProductFamily)
        {
        	#if defined (LIB_SUPPORT_CONTROLLER)
            case SCRUTINY_PRODUCT_FAMILY_CONTROLLER:
            {
                /* We will first close the device */
                cdmiCloseDevice (PtrDeviceManager->PtrDeviceList[index]);
                break;
            }
			#endif

			#if defined (LIB_SUPPORT_EXPANDER)

            case SCRUTINY_PRODUCT_FAMILY_EXPANDER:
            {
				if (PtrDeviceManager->PtrDeviceList[index]->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC) 
				{
					break;
				}
				
                /* We will first close the device */				
                edmiCloseDevice (PtrDeviceManager->PtrDeviceList[index]);
                break;

            }
			#endif
			#if defined (LIB_SUPPORT_SWITCH)
            case SCRUTINY_PRODUCT_FAMILY_SWITCH:
            {
				if (PtrDeviceManager->PtrDeviceList[index]->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC) 
				{
					break;
				}
				
                /* We will first close the device */
                sdmiCloseDevice (PtrDeviceManager->PtrDeviceList[index]);
                break;
            }
			#endif

            default:
            {
                break;
            }
        }

    }

    sosiMemFree (PtrDeviceManager->PtrDeviceList);
    PtrDeviceManager->PtrDeviceList = NULL;

    sosiMemFree (PtrDeviceManager->PtrDiscoveryParams);
    PtrDeviceManager->PtrDiscoveryParams = NULL;

    PtrDeviceManager->DeviceCount = 0;

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  ldmiAddScrutinyDevice ()
 *
 * @param   PPtrLibraryExtension - Pointer to Pointer to library structure
 *
 * @param   PtrAdapterEntryStruct - Pointer to Pointer to adapter info structure
 *
 * @return  lsi_status  Indicating Success or Fail
 *
 * @brief   Adapter will be stored in stack data structure
 *
 */
#if !defined(OS_BMC)
BOOLEAN ldmHasDuplicateScsiDevice (__IN__ PTR_SCRUTINY_DEVICE_MANAGER PtrScrutinyLibManager, __IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{

    U32 index = 0;

    if (PtrScrutinyLibManager->DeviceCount == 0)
    {
        return (FALSE);
    }

	//This has to be enabled only when we add expander support to BMC
	if (PtrScrutinyLibManager->PtrDeviceList[index]->HandleType & SCRUTINY_HANDLE_TYPE_MASK_BMC) 
	{
		return (FALSE);
	}

    if (!(PtrDevice->HandleType & SCRUTINY_HANDLE_TYPE_MASK_SCSI))
    {
        return (FALSE);
    }

    for (index = 0; index < PtrScrutinyLibManager->DeviceCount; index++)
    {
        if (!(PtrScrutinyLibManager->PtrDeviceList[index]->HandleType & SCRUTINY_HANDLE_TYPE_MASK_SCSI))
        {
            continue;
        }

        if (PtrScrutinyLibManager->PtrDeviceList[index]->ProductFamily != PtrDevice->ProductFamily)
        {
            continue;
        }

        if (PtrScrutinyLibManager->PtrDeviceList[index]->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
        {
            if (PtrScrutinyLibManager->PtrDeviceList[index]->DeviceInfo.u.ExpanderInfo.SASAddress.High != PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.High ||
                PtrScrutinyLibManager->PtrDeviceList[index]->DeviceInfo.u.ExpanderInfo.SASAddress.Low != PtrDevice->DeviceInfo.u.ExpanderInfo.SASAddress.Low)
            {
                continue;
            }

        }

        else if (PtrScrutinyLibManager->PtrDeviceList[index]->ProductFamily == SCRUTINY_PRODUCT_FAMILY_SWITCH)
        {
            if (PtrScrutinyLibManager->PtrDeviceList[index]->DeviceInfo.u.ExpanderInfo.SASAddress.High != PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.High ||
                PtrScrutinyLibManager->PtrDeviceList[index]->DeviceInfo.u.ExpanderInfo.SASAddress.Low != PtrDevice->DeviceInfo.u.SwitchInfo.SASAddress.Low)
            {
                continue;
            }
        }

        if (PtrScrutinyLibManager->PtrDeviceList[index]->Handle.ScsiHandle.AdapterPCIAddress.BusNumber == PtrDevice->Handle.ScsiHandle.AdapterPCIAddress.BusNumber &&
            PtrScrutinyLibManager->PtrDeviceList[index]->Handle.ScsiHandle.AdapterPCIAddress.DeviceNumber == PtrDevice->Handle.ScsiHandle.AdapterPCIAddress.DeviceNumber &&
            PtrScrutinyLibManager->PtrDeviceList[index]->Handle.ScsiHandle.AdapterPCIAddress.FunctionNumber == PtrDevice->Handle.ScsiHandle.AdapterPCIAddress.FunctionNumber)
        {
            return (TRUE);
        }

    }

    return (FALSE);

}
#endif

/**
 *
 * @method  ldmiAddScrutinyDevice ()
 *
 * @param   PPtrLibraryExtension - Pointer to Pointer to library structure
 *
 * @param   PtrAdapterEntryStruct - Pointer to Pointer to adapter info structure
 *
 * @return  lsi_status  Indicating Success or Fail
 *
 * @brief   Adapter will be stored in stack data structure
 *
 */

SCRUTINY_STATUS ldmiAddScrutinyDevice (__INOUT__ PTR_SCRUTINY_DEVICE_MANAGER PtrScrutinyLibManager, __IN__ PTR_SCRUTINY_DEVICE PtrDevice)
{
	#if !defined(OS_BMC)
    if (ldmHasDuplicateScsiDevice (PtrScrutinyLibManager, PtrDevice))
    {
        return (SCRUTINY_STATUS_IGNORE);
    }
	#endif

    if (PtrScrutinyLibManager->DeviceCount == 0)
    {
        PtrScrutinyLibManager->PtrDeviceList = (PTR_SCRUTINY_DEVICE *) sosiMemAlloc (sizeof (PTR_SCRUTINY_DEVICE));
    }

    else
    {
        PtrScrutinyLibManager->PtrDeviceList = (PTR_SCRUTINY_DEVICE *) sosiMemRealloc (PtrScrutinyLibManager->PtrDeviceList,
                                                                                       ((PtrScrutinyLibManager->DeviceCount + 1) * sizeof (PTR_SCRUTINY_DEVICE)),
                                                                                       ((PtrScrutinyLibManager->DeviceCount) * sizeof (PTR_SCRUTINY_DEVICE)));
    }

    if (PtrScrutinyLibManager->PtrDeviceList == NULL)
    {
        return (SCRUTINY_STATUS_NO_MEMORY);
    }

    PtrDevice->DeviceInfo.ProductHandle = PtrScrutinyLibManager->DeviceCount + 1;

    PtrScrutinyLibManager->PtrDeviceList[PtrScrutinyLibManager->DeviceCount] = PtrDevice;

    PtrScrutinyLibManager->DeviceCount++;

    return (SCRUTINY_STATUS_SUCCESS);

}


