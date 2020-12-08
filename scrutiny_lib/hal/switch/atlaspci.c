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

#if defined(OS_WINDOWS)
    #include "pciwin.h"
#endif

#if defined(OS_UEFI)
    #include "pciefi.h"
#endif

#if !defined (OS_VMWARE)
    #include "pcilinux.h"
#endif

#if defined (OS_VMWARE)
    #include "pcivmware.h"
#endif

#include "atlas.h"



SCRUTINY_STATUS sdAddDeviceIntoGlobalArray (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch);

SCRUTINY_STATUS atlasAddDevice (PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation, PTR_IAL_PCI_CONFIG_SPACE PtrConfigSpace);

SCRUTINY_STATUS atlasIsDeviceExist (PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation);

SCRUTINY_STATUS atlasIsParentFound (PTR_SCRUTINY_PCI_ADDRESS PtrRootDevice, PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation);

SCRUTINY_STATUS pdiGetConfigurationSpace (__IN__ PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation, __OUT__ PTR_IAL_PCI_CONFIG_SPACE PtrConfigSpace);

SCRUTINY_STATUS atlasiQualifyPCIDevice (PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation)
{

    IAL_PCI_CONFIG_SPACE configSpace;

    if (pdiGetConfigurationSpace (PtrDeviceLocation, &configSpace))
    {
        return (SCRUTINY_STATUS_IGNORE);
    }

    if (configSpace.P2PConfig.DeviceId != 0xC010 && configSpace.P2PConfig.DeviceId != 0xC012)
    {
        return (SCRUTINY_STATUS_IGNORE);
    }

    /* We will check if the device can be supported */
    if (configSpace.P2PConfig.HeaderType != 0x01)
    {
        /* Device is not P2P hence it cant be supported */
        return (SCRUTINY_STATUS_FAILED);
    }

    /* We have got the device now, we can now add the device into the tree. */
    return (atlasAddDevice (PtrDeviceLocation, &configSpace));

}

#if defined(OS_UEFI)
SCRUTINY_STATUS pcsiOpenDevice (__IN__ SCRUTINY_PCI_ADDRESS PciLocation);
#endif

SCRUTINY_STATUS atlasAddDevice (PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation, PTR_IAL_PCI_CONFIG_SPACE PtrConfigSpace)
{

    PTR_SCRUTINY_DEVICE ptrDevice = NULL;

    U32 dword = 0;

    SCRUTINY_STATUS status;

    gPtrLoggerGeneric->logiDebug ("atlasAddDevice (%x:%x:%x)", PtrDeviceLocation->BusNumber, PtrDeviceLocation->DeviceNumber, PtrDeviceLocation->FunctionNumber);

    ptrDevice = (PTR_SCRUTINY_DEVICE) sosiMemAlloc (sizeof (SCRUTINY_DEVICE));

    sosiMemSet (ptrDevice, 0, sizeof (SCRUTINY_DEVICE));

    ptrDevice->HandleType = SCRUTINY_HANDLE_TYPE_PCI;
    ptrDevice->DeviceInfo.HandleType = ptrDevice->HandleType;
    ptrDevice->ProductFamily = SCRUTINY_PRODUCT_FAMILY_SWITCH;

    ptrDevice->DeviceInfo.ProductFamily = SCRUTINY_PRODUCT_FAMILY_SWITCH;

    ptrDevice->DeviceInfo.u.SwitchInfo.PciDeviceId = PtrConfigSpace->P2PConfig.DeviceId;
    ptrDevice->DeviceInfo.u.SwitchInfo.PciVendorId = PtrConfigSpace->P2PConfig.VendorId;
    ptrDevice->DeviceInfo.u.SwitchInfo.RevisionId  = PtrConfigSpace->P2PConfig.Revision;

    ptrDevice->DeviceInfo.u.SwitchInfo.ComponentId = PtrConfigSpace->P2PConfig.DeviceId;

    ptrDevice->Handle.PciHandle.PciLocation.BusNumber = PtrDeviceLocation->BusNumber;
    ptrDevice->Handle.PciHandle.PciLocation.DeviceNumber = PtrDeviceLocation->DeviceNumber;
    ptrDevice->Handle.PciHandle.PciLocation.FunctionNumber = PtrDeviceLocation->FunctionNumber;

    ptrDevice->DeviceInfo.u.SwitchInfo.PciAddress.BusNumber = PtrDeviceLocation->BusNumber;
    ptrDevice->DeviceInfo.u.SwitchInfo.PciAddress.DeviceNumber = PtrDeviceLocation->DeviceNumber;
    ptrDevice->DeviceInfo.u.SwitchInfo.PciAddress.FunctionNumber = PtrDeviceLocation->FunctionNumber;

    sosiMemCopy (&ptrDevice->Handle.PciHandle.ConfigSpace, PtrConfigSpace, sizeof(IAL_PCI_CONFIG_SPACE));

    /* We will check if the device can be supported */

    if (PtrConfigSpace->P2PConfig.HeaderType != 0x01)
    {
        /* Device is not P2P hence it cant be supported */
        return (SCRUTINY_STATUS_IGNORE);
    }

    if (ptrDevice->Handle.PciHandle.Revision == 0xA0)
    {
        /* We don't support A0 in this mode. */
        return (SCRUTINY_STATUS_IGNORE);
    }

    //atlasiAssignProductString (ptrDevice, ptrPciHandle->ConfigSpace.Dwords[0], ptrPciHandle->Revision);

    #if defined(OS_UEFI)
    pcsiOpenDevice (*PtrDeviceLocation);
    #endif

    status = peiInitializeBARRegions (PtrDeviceLocation, &ptrDevice->Handle.PciHandle);

    if (!ptrDevice->Handle.PciHandle.BarRegions[0].PtrVirtualAddress)
    {
        gPtrLoggerGeneric->logiDebug ("Atlas PCI Device '%x:%x:%x' not added as there is no BAR0 region available.",
                                       PtrDeviceLocation->BusNumber, PtrDeviceLocation->DeviceNumber, PtrDeviceLocation->FunctionNumber);

        sosiMemFree (ptrDevice);

        return (SCRUTINY_STATUS_IGNORE);

    }

    if (atlasIsDeviceExist (PtrDeviceLocation) == SCRUTINY_STATUS_SUCCESS)
    {

        sosiMemFree (ptrDevice);

        /* We don't need to add again. */
        return (SCRUTINY_STATUS_IGNORE);
    }


    if (!status)
    {
        status = ldmiAddScrutinyDevice (gPtrScrutinyDeviceManager, ptrDevice);
    }

    /*
     * If the status is good, then return otherwise destroy all the references.
     */

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sosiMemFree (ptrDevice);
        return (status);
    }

    /* If everything is good, we will be able to perform the lane specific Product string. */

    atlasiPciChimeToAxiReadRegister (ptrDevice,
                                     ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE + ATLAS_REGISTER_OFFSET_HARDWARE_CHIP_ID_REVISION,
                                     &dword);

    /*
     *  If incase problem reading the chip register we are safe to assign the
     *  default sring with the proper revision value, so not using the Chip ID register.
     */

    //atlasiAssignLaneSpecificProductString (ptrDevice, dword, ptrPciHandle->Revision);

    //cmniDumpHexMemory (PtrConfigSpace->Bytes, 256);

    atlasGetSwitchMode (ptrDevice);

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS atlasIsDeviceExist (PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation)
{

    U32 index = 0;

    for (index = 0; index < gPtrScrutinyDeviceManager->DeviceCount; index++)
    {
        if (gPtrScrutinyDeviceManager->PtrDeviceList[index] == NULL)
        {
            break;
        }

        if (gPtrScrutinyDeviceManager->PtrDeviceList[index]->ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
        {
            continue;
        }

        if (gPtrScrutinyDeviceManager->PtrDeviceList[index]->HandleType != SCRUTINY_HANDLE_TYPE_PCI)
        {
            continue;
        }

        gPtrLoggerGeneric->logiDebug ("Checking %x.%x.%x in %x.%x.%x",
                                       PtrDeviceLocation->BusNumber,
                                       PtrDeviceLocation->DeviceNumber,
                                       PtrDeviceLocation->FunctionNumber,
                                       gPtrScrutinyDeviceManager->PtrDeviceList[index]->Handle.PciHandle.PciLocation.BusNumber,
                                       gPtrScrutinyDeviceManager->PtrDeviceList[index]->Handle.PciHandle.PciLocation.DeviceNumber,
                                       gPtrScrutinyDeviceManager->PtrDeviceList[index]->Handle.PciHandle.PciLocation.FunctionNumber);

        if (atlasIsParentFound (&gPtrScrutinyDeviceManager->PtrDeviceList[index]->Handle.PciHandle.PciLocation, PtrDeviceLocation))
        {
            continue;
        }

        else
        {
            return (SCRUTINY_STATUS_SUCCESS);
        }

    }

    return (SCRUTINY_STATUS_FAILED);

}

SCRUTINY_STATUS atlasIsParentFound (PTR_SCRUTINY_PCI_ADDRESS PtrRootDevice, PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation)
{

    IAL_PCI_CONFIG_SPACE configSpace, rootConfigSpace;
    SCRUTINY_PCI_ADDRESS child;

    if (pdiGetConfigurationSpace (PtrDeviceLocation, &configSpace))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (pdiGetConfigurationSpace (PtrRootDevice, &rootConfigSpace))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* We will check if the device can be supported */

    if (configSpace.P2PConfig.HeaderType != 0x01)
    {
        /* Device is not P2P hence it cant be supported */
        return (SCRUTINY_STATUS_FAILED);
    }

    if (rootConfigSpace.P2PConfig.SecondaryBusNumber == PtrDeviceLocation->BusNumber)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    child.BusNumber = rootConfigSpace.P2PConfig.SecondaryBusNumber;

    if (child.BusNumber == 0)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    return (atlasIsParentFound (&child, PtrDeviceLocation));

}

SCRUTINY_STATUS pdiGetConfigurationSpace (__IN__ PTR_SCRUTINY_PCI_ADDRESS PtrDeviceLocation, __OUT__ PTR_IAL_PCI_CONFIG_SPACE PtrConfigSpace)
{

    U32 dword;
    U32 index;

    sosiMemSet (PtrConfigSpace, 0, sizeof (IAL_PCI_CONFIG_SPACE));

    for (index = 0; index < 256; index += 4)
    {
        pcsiReadDword (*PtrDeviceLocation, index, &dword);

        if (dword == 0xFFFFFFFF && index == 0)
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        PtrConfigSpace->Dwords[index / 4] = dword;
    }

    return (SCRUTINY_STATUS_SUCCESS);

}


SCRUTINY_STATUS atlasiGetPciMappedBAR0Address (__IN__ U32 Address, __OUT__ U32 *PtrMappedAddress)
{

    *PtrMappedAddress = 0;

    /* Check if we are between the flash address range */

    if ((Address >= ATLAS_REGISTER_SPI_FLASH_BASE_ADDRESS) &&
        (Address <= (ATLAS_REGISTER_SPI_FLASH_BASE_ADDRESS + (4 * 1024 * 1024))))
    {
        *PtrMappedAddress = (Address - ATLAS_REGISTER_SPI_FLASH_BASE_ADDRESS) + ATLAS_REGISTER_BAR0_SPI_FLASH_BASE_ADDRESS;
    }

    else if ((Address >= 0x2A000000) && (Address <= (0x2A000000 + (1 * 1024 * 1024))))
    {
        *PtrMappedAddress = (Address - 0x2A000000) + 0x100000;
    }

    else if ( (Address >= ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE) &&
         (Address <= ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE + ATLAS_REGISTER_SIZE_PORT_CONFIG_SPACE_SIZE))
    {
        *PtrMappedAddress = (Address - ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE) + (8 * 1024 * 1024);
    }

    /** Ignore following registers for the BAR0 ids, less bit of hack to avoid
     *  any problems */
    else if (Address == ATLAS_REGISTER_CPU_WD_CTL || Address == ATLAS_REGISTER_FMU_HOST_DIAGNOSTIC)
    {
        *PtrMappedAddress = 0;
        return (SCRUTINY_STATUS_IGNORE);
    }

    else
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 * @method  atlasPciDiagRead32()
 *
 * @param   Address     Value of the memory address
 *
 * @param   PtrData     Data where the memory read value stored.
 *
 * @return  SCRUTINY_STATUS      SCRUTINY_STATUS_SUCCESS for the success and non-zero if failed.
 *
 * @brief   Perform the quick Read/write operation and this is to by-pass the
 *          global Atlas.c memory read/write operations because they will be
 *          having additional checks and params. We need only a single size
 *          Byte/Word/Dword read/write.
 *
 */

SCRUTINY_STATUS atlasPciDiagRead32 (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __OUT__ PU32 PtrData)
{

    if (PtrSwitch->Handle.PciHandle.BarRegions[0].PtrVirtualAddress)
    {

        #if !defined(OS_UEFI_)


        *PtrData = ((PU32) PtrSwitch->Handle.PciHandle.BarRegions[0].PtrVirtualAddress)[Address / 4];

        #else

        pcsiReadBARRegisterDword (PtrSwitch->Handle.PciHandle.PciLocation,
                                  PtrSwitch->Handle.PciHandle.BarRegions[0].ConfigSpaceOffset,
                                  Address,
                                  PtrData);
        #endif
    }

    else
    {
        gPtrLoggerGeneric->logiDebug ("[PCIDiag] Internal Error. Mapped Address is NULL. MR32 %08x", Address);
        return (SCRUTINY_STATUS_FAILED);
    }

    //gPtrLoggerGeneric->logiDebug ("[PCIDiag] MR32 %08x = %08x", Address, *PtrData);

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  atlasPciDiagWrite32()
 *
 * @param   Address     Value of the memory address
 *
 * @param   PtrData     Data where the memory read value stored.
 *
 * @return  SCRUTINY_STATUS      SCRUTINY_STATUS_SUCCESS for the success and non-zero if failed.
 *
 * @brief   Perform the quick Read/write operation and this is to by-pass the
 *          global Atlas.c memory read/write operations because they will be
 *          having additional checks and params. We need only a single size
 *          Byte/Word/Dword read/write.
 *
 */

SCRUTINY_STATUS atlasPciDiagWrite32 (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __IN__ U32 Data)
{

    if (PtrSwitch->Handle.PciHandle.BarRegions[0].PtrVirtualAddress)
    {
        #if !defined(OS_UEFI_)
        ((PU32) PtrSwitch->Handle.PciHandle.BarRegions[0].PtrVirtualAddress)[Address / 4] = Data;
        #else

        pcsiWriteBARRegisterDword (PtrSwitch->Handle.PciHandle.PciLocation,
                                   PtrSwitch->Handle.PciHandle.BarRegions[0].ConfigSpaceOffset,
                                   Address,
                                   Data);
        #endif
    }

    else
    {
        gPtrLoggerGeneric->logiDebug ("[PCIDiag] Internal Error. Mapped Address is NULL. MW32 %08x = %08x", Address, Data);
        return (SCRUTINY_STATUS_FAILED);
    }

    //gPtrLoggerGeneric->logiDebug ("[PCIDiag] MW32 %08x = %08x", Address, Data);

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS atlasFSMBusyClearCheck (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch)
{

    U32 registerAddress;
    U32 data;
    U32 retryCount = 10;

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_CONTROL_STATUS_OFFSET;

    while (retryCount--)
    {
        atlasPciDiagRead32 (PtrSwitch, registerAddress, &data);

        if ((data >> 2) & 1)                //BIT[2] is set
        {
            sosiSleep (1);
            continue;
        }

        else
        {
            return (SCRUTINY_STATUS_SUCCESS);
        }

    }

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_ADDRESS_OFFSET;

    atlasPciDiagRead32 (PtrSwitch, registerAddress, &data);

    gPtrLoggerGeneric->logiDebug ("ChimeToAxi FSM bit is not cleared for the outstanding address in Chime is '%x'",
                                   data);

    return (SCRUTINY_STATUS_FAILED);

}

SCRUTINY_STATUS atlasiFSMFallBackRegisterRead (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __OUT__ U32 *PtrData)
{

    U32 mappedAddress = 0;
    SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;

    status = atlasiGetPciMappedBAR0Address (Address, &mappedAddress);

    if (status)
    {
        gPtrLoggerGeneric->logiDebug ("MemMap BAR0: Unmapped register '%x' for BAR 0 (MR)",  Address);
    }

    if (status == SCRUTINY_STATUS_IGNORE)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED_BAR0_ADDRESS_NOT_MAPPED);
    }

    gPtrLoggerGeneric->logiDebug ("[BAR0-AXI] MR32 %08x, Mapped=%08x", Address, mappedAddress);

    return (atlasPciDiagRead32 (PtrSwitch, mappedAddress, PtrData));

}

SCRUTINY_STATUS atlasiFSMFallBackRegisterWrite (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __OUT__ U32 Data)
{

    U32 mappedAddress;
    SCRUTINY_STATUS status;

    status = atlasiGetPciMappedBAR0Address (Address, &mappedAddress);

    if (status)
    {
        gPtrLoggerGeneric->logiDebug ("MemMap BAR0: Unmapped register '%x' for BAR 0 (MW)",  Address);
    }

    if (status == SCRUTINY_STATUS_IGNORE)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    if (status)
    {
        return (SCRUTINY_STATUS_FAILED_BAR0_ADDRESS_NOT_MAPPED);
    }

    gPtrLoggerGeneric->logiDebug ("[BAR0-AXI] MW32 %08x, Mapped=%08x, Data=%08x", Address, mappedAddress, Data);

    return (atlasPciDiagWrite32 (PtrSwitch, mappedAddress, Data));

}

SCRUTINY_STATUS atlasiPciChimeToAxiReadRegister (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __OUT__ U32 *PtrData)
{

    U32 registerAddress;
    U32 data;
    U32 retryCount = 100;

    if (atlasFSMBusyClearCheck (PtrSwitch))
    {
        gPtrLoggerGeneric->logiDebug ("FSM Generator bit is not cleared and trying with BAR0 mapped mode.");
        return (atlasiFSMFallBackRegisterRead (PtrSwitch, Address, PtrData));
    }

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_CONTROL_STATUS_OFFSET;

    /* First we will clear the data in the Control status */

    atlasPciDiagWrite32 (PtrSwitch, registerAddress, 0x00);

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_ADDRESS_OFFSET;

    /* Now we will write the address */

    atlasPciDiagWrite32 (PtrSwitch, registerAddress, Address);

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_CONTROL_STATUS_OFFSET;

    /* Now we will have to perform the cycle read register command which is offset 1 */

    atlasPciDiagWrite32 (PtrSwitch, registerAddress, 0x02);

    while (retryCount--)
    {


        /* Read the control status and see if we have the data read successfully done */

        atlasPciDiagRead32 (PtrSwitch, registerAddress, &data);

        if ((data >> 3) & 0x1)
        {

            /* We have a data which is good */

            registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                              ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                              ATLAS_REGISTER_CHIME_TO_AXI_DATA_OFFSET;

            atlasPciDiagRead32 (PtrSwitch, registerAddress, &data);

            gPtrLoggerGeneric->logiDebug ("[CHIME-AXI] MR32 %08x, Data=%08x", Address, data);

            *PtrData = data;

            return (SCRUTINY_STATUS_SUCCESS);
        }

    }

    *PtrData = 0;

    return (SCRUTINY_STATUS_FAILED);


}

SCRUTINY_STATUS atlasiPciChimeToAxiWriteRegister (__IN__ PTR_SCRUTINY_DEVICE PtrSwitch, __IN__ U32 Address, __IN__ U32 Data)
{

    U32 registerAddress;

    if (atlasFSMBusyClearCheck (PtrSwitch))
    {
        gPtrLoggerGeneric->logiDebug ("FSM Generator bit is not cleared and trying with BAR0 mapped mode.");
        return (atlasiFSMFallBackRegisterWrite (PtrSwitch, Address, Data));
    }

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_CONTROL_STATUS_OFFSET;

    /* First we will clear the data in the Control status */

    atlasPciDiagWrite32 (PtrSwitch, registerAddress, 0x00);

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_ADDRESS_OFFSET;

    /* Now we will write the address */

    atlasPciDiagWrite32 (PtrSwitch, registerAddress, Address);

    /* Write the data */

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_DATA_OFFSET;

    atlasPciDiagWrite32 (PtrSwitch, registerAddress, Data);

    /* Now we will have to perform the cycle write register command which is offset 0 */

    registerAddress = ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS +
                      ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET +
                      ATLAS_REGISTER_CHIME_TO_AXI_CONTROL_STATUS_OFFSET;


    atlasPciDiagWrite32 (PtrSwitch, registerAddress, 0x01);

    gPtrLoggerGeneric->logiDebug ("[CHIME-AXI] MW32 %08x, Data=%08x", Address, Data);

    return (SCRUTINY_STATUS_SUCCESS);

}

