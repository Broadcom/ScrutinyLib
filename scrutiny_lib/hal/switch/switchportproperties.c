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
#include "switchportproperties.h"

SCRUTINY_STATUS sppGetPciePortProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties)
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32                                   ccrAddress;
    U32                                   regVal;
    U32                                   switchMode;

    gPtrLoggerSwitch->logiFunctionEntry ("sppGetPciePortProperties (PtrDevice=%x,  PtrPciePortProperties=%x)", PtrDevice != NULL,  PtrPciePortProperties != NULL);

    
    ccrAddress = ATLAS_REGISTER_PMG_REG_SWITCH_MODE;
    
    status = bsdiMemoryRead32 (PtrDevice, ccrAddress, &regVal, sizeof (U32));
    gPtrLoggerSwitch->logiDebug ("Switch Mode register = %x", regVal);
    switchMode = regVal&0x03;

    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortProperties  (Status = %x) ",status);
        return (status);    
    }


    switch (switchMode)
    {
        case 0x0 :
        {                
            return (sppGetPciePortPropertiesBsw (PtrDevice, PtrPciePortProperties));                   
        }
        
        case 0x1 :
        {
            return (sppGetPciePortPropertiesSsw (PtrDevice, PtrPciePortProperties));            
        }
                    
        default:
        {
            gPtrLoggerSwitch->logiDebug ("Switch Mode Unknown");
            status = SCRUTINY_STATUS_FAILED;
            gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortProperties  (Status = %x) ",status);
            return (status);              
        }

    }


    
    
}


SCRUTINY_STATUS sppGetPciePortPropertiesSsw (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties)
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32                                   index, index2, index3, index4;
    U32                                   chipId;
    U32                                   maxPort;
    U32                                   pageSize = 0;
    PU8                                   ptrPage = NULL;
    U32                                   cfgAssignedHostPorts;
    U8                                    cfgHostPortNum;
    U32                                   cfgAssignedTotalPorts;
    SWITCH_PORT_CONFIGURATION             portConfiguration = { 0 };
    PTR_SWITCH_CFG_PAGE_E00F              ptrCfgPageE00F;
    PTR_SWITCH_CFG_PAGE_E00E_PORT         ptrCfgPageE00EPort;
    SWITCH_ID                             switchID;
    U32                                   regVal;
    U32                                   gepAddress;
    
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppGetPciePortPropertiesSsw (PtrDevice=%x,  PtrPciePortProperties=%x)", PtrDevice != NULL,  PtrPciePortProperties != NULL);
    //initialze TotalPortConfigEntry
    PtrPciePortProperties->TotalPortConfigEntry = 0;
    PtrPciePortProperties->TotalHostPort = 0;
    PtrPciePortProperties->TotalDownStreamPort = 0;
    PtrPciePortProperties->TotalFabricPort = 0;
    PtrPciePortProperties->TotalManagementPort = 0;
    
    //get regular port properties
    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesSsw  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);
    PtrPciePortProperties->ToTalPhyNum = maxPort;
    
    for (index = 0; index < maxPort; /* Don't increment here */ )
    {
        portConfiguration.MaxLinkWidth = 0;
        /* Get the first port information */
        status = sppGetPcieOnePortProperties (PtrDevice, index, &portConfiguration);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            continue;
        }

        //appPrintPortProperties (index, &portConfiguration);

        //index++;
        index += portConfiguration.MaxLinkWidth;
        if (portConfiguration.MaxLinkWidth != 0)
        {
            sosiMemCopy (&(PtrPciePortProperties->PortConfigurations[PtrPciePortProperties->TotalPortConfigEntry]), &portConfiguration, sizeof (SWITCH_PORT_CONFIGURATION));
            PtrPciePortProperties->TotalPortConfigEntry++;
        }

    }
    //get port 116 and 117 properties
    //I use a trick,MaxLinkWidth is always 1. 
    for (index = ATLAS_PMG_PORT_NUM_X1_1; index <= ATLAS_PMG_PORT_NUM_X1_2; /* Don't increment here */ )
    {

        portConfiguration.MaxLinkWidth = 0;
        /* Get the first port information */
        status = sppGetPcieOnePortProperties (PtrDevice, index, &portConfiguration);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            continue;
        }

        //appPrintPortProperties (index, &portConfiguration);

        //index++;
        index += portConfiguration.MaxLinkWidth; //it always is 1.
        if (portConfiguration.MaxLinkWidth != 0)
        {
            sosiMemCopy (&(PtrPciePortProperties->PortConfigurations[PtrPciePortProperties->TotalPortConfigEntry]), &portConfiguration, sizeof (SWITCH_PORT_CONFIGURATION));
            PtrPciePortProperties->TotalPortConfigEntry++;
        }


    }
    
    //setup GID for every port
            
     //get switch ID register
    gepAddress = ATLAS_REGISTER_PMG_REG_SWITCH_ID;
    
    status = bsdiMemoryRead32 (PtrDevice, gepAddress, &regVal, sizeof (U32));
    gPtrLoggerSwitch->logiDebug ("Switch ID register = %x", regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesSsw  (Status = %x) ",status);
        return (status);    
    }
    switchID.u.DN.Domain = (U8) ((regVal >> 0) & 0xFF);
    switchID.u.DN.Number = (U8) ((regVal >> 16) & 0x3F);
    
    for (index = 0; index < PtrPciePortProperties->TotalPortConfigEntry; index++)
    {
        // fill GID
        PtrPciePortProperties->PortConfigurations[index].GID = (((U32)(switchID.u.ID)) << 8) | PtrPciePortProperties->PortConfigurations[index].PortNumber;
        //do some summary
        switch (PtrPciePortProperties->PortConfigurations[index].PortType)
        {
            case SWITCH_PORT_TYPE_DOWNSTREAM :
            {                
                PtrPciePortProperties->TotalDownStreamPort++;
                break;
            }
            
            case SWITCH_PORT_TYPE_FABRIC :
            {
                PtrPciePortProperties->TotalFabricPort++;
                break;
            }
            
            case SWITCH_PORT_TYPE_MANAGEMENT :
            {
                PtrPciePortProperties->TotalManagementPort++;
                break;
            }
            
            case SWITCH_PORT_TYPE_UPSTREAM :
            {
                PtrPciePortProperties->TotalHostPort++;
                break;
            }
            
            default:
            {
                break;
            }

        }

    }
    //finish fill GID
            
    // I want to get DsToHostMask from configuration page 0xE00F
       
    status  = bsdiGetConfigPage (PtrDevice, 0xE00F, BRCM_SCSI_DEVICE_CONFIG_PAGE_REGION_ANY, &ptrPage, &pageSize);   
    gPtrLoggerSwitch->logiVerbose ("Cfg E00F Read Address=%x, SizeInBytes=%x", ptrPage, pageSize);  
    gPtrLoggerSwitch->logiDumpMemoryInVerbose ((PU8) ptrPage, pageSize, gPtrLoggerSwitch);          

    if (status)
    {
        gPtrLoggerSwitch->logiDebug ("sppGetPciePortPropertiesSsw read config page (Status=%x)", status);
        if (ptrPage != NULL)
        {
            sosiMemFree (ptrPage);//please remeber release the buffer after getting configuration page.
        }

        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortProperties  (Status = %x) ",status);
        return (status); 
    }

    cfgAssignedHostPorts = pageSize / sizeof (SWITCH_CFG_PAGE_E00F);
    gPtrLoggerSwitch->logiDebug ("cfgAssignedHostPorts = %x", cfgAssignedHostPorts);
    if (cfgAssignedHostPorts != PtrPciePortProperties->TotalHostPort)
    {
        gPtrLoggerSwitch->logiDebug ("sppGetPciePortPropertiesSsw host port number not match");
        
        if (ptrPage != NULL)
        {
            sosiMemFree (ptrPage);//please remeber release the buffer after getting configuration page.
        }

        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesSsw  (Status = %x) ",status);
        return (status);
    }
    
    ptrCfgPageE00F = (PTR_SWITCH_CFG_PAGE_E00F) (ptrPage + 4);// the first 4 bytes is the header, ptrPage is PU8 type
    for (index = 0; index < cfgAssignedHostPorts; index++)
    {
        cfgHostPortNum = ptrCfgPageE00F->PortNum;
        gPtrLoggerSwitch->logiDebug ("cfgHostPortNum = %x", cfgHostPortNum);
        for (index2 = 0; index2 < PtrPciePortProperties->TotalPortConfigEntry; index2++)
        {
            if ((PtrPciePortProperties->PortConfigurations[index2].PortType == SWITCH_PORT_TYPE_UPSTREAM) && (PtrPciePortProperties->PortConfigurations[index2].PortNumber == cfgHostPortNum))
            {  
                //total 4 Dwords
                for (index3 = 0; index3 < 4; index3++)
                {
                    PtrPciePortProperties->PortConfigurations[index2].u.Host.DsPortMask[index3] = ptrCfgPageE00F->DsToHostMask[index3];
                }

                //handle DsToHostMask,total 128 bits
                for (index3 = 0; index3 < 128; index3++)
                {
                    if (((ptrCfgPageE00F->DsToHostMask[index3 / 32] >> (index3 % 32)) & 0x01) == 1)
                    {
                       for (index4 = 0; index4 < PtrPciePortProperties->TotalPortConfigEntry; index4++)
                       {
                           if (PtrPciePortProperties->PortConfigurations[index4].PortNumber == index3)
                           {
                                PtrPciePortProperties->PortConfigurations[index4].u.Ds.HostPortNum = cfgHostPortNum;
                           }
                       }
                    }
                }


                  
            }
        }
        ptrCfgPageE00F++;
    } 
      

    
    if (ptrPage != NULL)
    {
        sosiMemFree (ptrPage);//please remeber release the buffer after getting configuration page.
    }
    //finish getting DsToHostMask from configuration page 0xE00F

    // start getting physical slot number from configuation page 0xE00E
    status  = bsdiGetConfigPage (PtrDevice, 0xE00E, BRCM_SCSI_DEVICE_CONFIG_PAGE_REGION_ANY, &ptrPage, &pageSize);   
    gPtrLoggerSwitch->logiVerbose ("Cfg E00E Read Address=%x, SizeInBytes=%x", ptrPage, pageSize);  
    gPtrLoggerSwitch->logiDumpMemoryInVerbose ((PU8) ptrPage, pageSize, gPtrLoggerSwitch);
    if (status)
    {
        gPtrLoggerSwitch->logiDebug ("sppGetPciePortPropertiesSsw read config page (Status=%x)", status);
        if (ptrPage != NULL)
        {
            sosiMemFree (ptrPage);//please remeber release the buffer after getting configuration page.
        }

        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesSsw  (Status = %x) ",status);
        return (status); 
    }
    //there are total 52 bytes before port data
    if (pageSize < 52)
    {
        gPtrLoggerSwitch->logiDebug ("sppGetPciePortPropertiesSsw no port cfg data (Status=%x)", status);
        if (ptrPage != NULL)
        {
            sosiMemFree (ptrPage);//please remeber release the buffer after getting configuration page.
        }

        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesSsw  (Status = %x) ",status);
        return (status);
    }
    cfgAssignedTotalPorts = (pageSize - 52) / sizeof (SWITCH_CFG_PAGE_E00E_PORT);
    gPtrLoggerSwitch->logiDebug ("cfgAssignedTotalPorts = %x", cfgAssignedTotalPorts);
    
    // I decide not check cfgAssignedTotalPorts, because I am not sure what port will be included ,host,downstream,fabric,management port?
    // currently remove below code
    //if (cfgAssignedTotalPorts != PtrPciePortProperties->TotalPortConfigEntry)
    //{
    //    gPtrLoggerSwitch->logiDebug ("sppGetPciePortProperties total port number not match");
    //    
    //    if (ptrPage != NULL)
    //    {
    //        sosiMemFree (ptrPage);//please remeber release the buffer after getting configuration page.
    //    }
    //
    //    status = SCRUTINY_STATUS_FAILED;
    //    gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortProperties  (Status = %x) ",status);
    //    return (status);
    //}
    ptrCfgPageE00EPort = (PTR_SWITCH_CFG_PAGE_E00E_PORT) (ptrPage + 52);// the first 52 bytes is skipped, ptrPage is PU8 type
    
    for (index = 0; index < cfgAssignedTotalPorts; index++)
    {
        if (ptrCfgPageE00EPort->PortType == SWITCH_PORT_TYPE_DOWNSTREAM)
        {
           for (index2 = 0; index2 < PtrPciePortProperties->TotalPortConfigEntry; index2++)
           {
               if ((PtrPciePortProperties->PortConfigurations[index2].PortNumber == ptrCfgPageE00EPort->PortNum) && (PtrPciePortProperties->PortConfigurations[index2].PortType == SWITCH_PORT_TYPE_DOWNSTREAM))
               {
                   PtrPciePortProperties->PortConfigurations[index2].u.Ds.ChassisPhysSlotNum = ptrCfgPageE00EPort->ChassisPhysSlotNum;
               }
           }
        }
        ptrCfgPageE00EPort++;
    }
    
    if (ptrPage != NULL)
    {
        sosiMemFree (ptrPage);//please remeber release the buffer after getting configuration page.
    }
    // finish getting physical slot number from configuation page 0xE00E
     
    gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesSsw  (Status = %x) ",status);

    return (status);


}

/**
 *
 *  @method  sppGetPcieOnePortProperties ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PortIndex               port index 
 * 
 *  @param   PtrPortConfiguration   Pointer to data structure contain the returned port properties.
 *
    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get switch one port's common properties. 
 *
 */

SCRUTINY_STATUS sppGetPcieOnePortProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrPortConfiguration)
{
    SCRUTINY_STATUS 			status = SCRUTINY_STATUS_FAILED;
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppGetPcieOnePortProperties (PtrDevice=%x, PortIndex=%x, PtrPortConfiguration=%x)", PtrDevice != NULL, PortIndex, PtrPortConfiguration != NULL);
    
    status = sppFillPortConfigurationFromConfigSpace (PtrDevice, PortIndex, PtrPortConfiguration);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPcieOnePortProperties  (Status = %x) ",status);
        return (status);    
    }

    status = sppFillPortConfigurationFromCcrSpace (PtrDevice, PortIndex, PtrPortConfiguration);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPcieOnePortProperties  (Status = %x) ",status);
        return (status);    
    }
    
    //do some special for different port type
    switch (PtrPortConfiguration->PortType)
    {
        case SWITCH_PORT_TYPE_DOWNSTREAM :
        {                
            sppGetDownStreamPortAdditionalProperties (PtrDevice, PortIndex, PtrPortConfiguration);
            break;
        }
            
        case SWITCH_PORT_TYPE_FABRIC :
        {
            //For future use
            break;
        }
            
        case SWITCH_PORT_TYPE_MANAGEMENT :
        {
            //For future use
            break;
        }
            
        case SWITCH_PORT_TYPE_UPSTREAM :
        {
            //For future use
            break;
        }
        
        case SWITCH_PORT_TYPE_DOWNSTREAM_BSW :
        {                
            sppGetDownStreamPortAdditionalProperties (PtrDevice, PortIndex, PtrPortConfiguration);
            break;
        }

        case SWITCH_PORT_TYPE_UPSTREAM_BSW :
        {
            //For future use
            break;
        }
            
        default:
        {
            break;
        }
    
    }
    
    gPtrLoggerSwitch->logiFunctionExit ("sppGetPcieOnePortProperties  (Status = %x) ",status);
    return (status);   
    
}


/**
 *
 *  @method  sppFillPortConfigurationFromConfigSpace ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PortIndex               port index 
 * 
 *  @param   PtrPortConfiguration   Pointer to data structure contain the returned port properties.
 *
    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method get switch one port's common properties from pcie configuration space. 
 *
 */


SCRUTINY_STATUS sppFillPortConfigurationFromConfigSpace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrConfiguration)
{

    U32 capabilityOffset = 0;
    U32 regVal;

    SCRUTINY_STATUS 			status = SCRUTINY_STATUS_FAILED;
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppFillPortConfigurationFromConfigSpace (PtrDevice=%x, PortIndex=%x, PtrConfiguration=%x)", PtrDevice != NULL, PortIndex, PtrConfiguration != NULL);
    


    // Get the offset of the PCI Express capability
    status = sppGetCapabilityOffset (PtrDevice, PortIndex, PCI_CAP_ID_PCI_EXPRESS, FALSE, &capabilityOffset);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, capabilityOffset = %x", PortIndex, capabilityOffset);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
        return (status);    
    }


    if (capabilityOffset == 0)
    {
        PtrConfiguration->PortType               = SWITCH_PORT_TYPE_UNKNOWN;
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
        return (status);
    }

    // Get PCIe Capability
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, capabilityOffset, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
        return (status);    
    }

    // Get port type for Base mode, BASE mode port type is get from configuration space register.
    //In synthetic mode , port type is get from CCR register, I will overwrite it later. 
     PtrConfiguration->PortType = (SWITCH_PORT_TYPE) ((regVal >> 20) & 0xF);
     gPtrLoggerSwitch->logiDebug("PortIndex = %x, only for Base mode PortType = %x", PortIndex, PtrConfiguration->PortType);
    
    // Get PCIe Device Capabilities
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, capabilityOffset + 0x04, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
        return (status);    
    }

    // Get max payload size supported field
    // Set max payload size (=128 * (2 ^ MaxPaySizeField))
    PtrConfiguration->MaxPayloadSizeSupport = (PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE) ((regVal >> 0) & 0x7);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, MaxPayloadSizeSupport = %x", PortIndex, PtrConfiguration->MaxPayloadSizeSupport);
    
    // Get PCIe Device Control
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, capabilityOffset + 0x08, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
        return (status);    
    }

    // Get max payload size field

    PtrConfiguration->MaxPayloadSize = (PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE) ((regVal >> 5) & 0x7);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, MaxPayloadSize = %x", PortIndex, PtrConfiguration->MaxPayloadSize);

    // Set max read request size (=128 * (2 ^ MaxReadReqSizeField))
    PtrConfiguration->MaxReadRequestSize = (PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE) ((regVal >> 12) & 0x7);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, MaxReadRequestSize = %x", PortIndex, PtrConfiguration->MaxReadRequestSize);
    
    // Get PCIe Link Capabilities
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, capabilityOffset + 0x0C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
        return (status);    
    }
    // Get port number
    PtrConfiguration->PortNumber = (U8) ((regVal >> 24) & 0xFF);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, PortNumber = %x", PortIndex, PtrConfiguration->PortNumber);

    // Get max link width
    PtrConfiguration->MaxLinkWidth = (PCI_DEVICE_LINK_WIDTH) ((regVal >> 4) & 0x3F);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, MaxLinkWidth = %x", PortIndex, PtrConfiguration->MaxLinkWidth);
    
    // Get max link speed
    PtrConfiguration->MaxLinkSpeed = (PCI_DEVICE_LINK_SPEED) ((regVal >> 0) & 0xF);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, MaxLinkSpeed = %x", PortIndex, PtrConfiguration->MaxLinkSpeed);
    
    // Get PCIe Link Status/Control
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, capabilityOffset + 0x10, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
        return (status);    
    }

    // Get link width
    PtrConfiguration->NegotiatedLinkWidth = (PCI_DEVICE_LINK_WIDTH) ((regVal >> 20) & 0x3F);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, NegotiatedLinkWidth = %x", PortIndex, PtrConfiguration->NegotiatedLinkWidth);
    
    // Get link speed
    PtrConfiguration->NegotiatedLinkSpeed = (PCI_DEVICE_LINK_SPEED) ((regVal >> 16) & 0xF);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, NegotiatedLinkSpeed = %x", PortIndex, PtrConfiguration->NegotiatedLinkSpeed);


    gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromConfigSpace  (Status = %x) ",status);
    return (status);
    

}


SCRUTINY_STATUS sppGetCapabilityOffset (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __IN__ U32 CapabilityId, __IN__ BOOLEAN IsExtendedPcieCapability, __IN__ PU32 PtrOffset)
{
    return (sppGetCapabilityOffsetInstance (PtrDevice, PortIndex, CapabilityId, 0, IsExtendedPcieCapability, PtrOffset));
}



SCRUTINY_STATUS sppGetCapabilityOffsetInstance (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __IN__ U32 CapabilityId, __IN__ U32 InstanceNumber, __IN__ BOOLEAN IsExtendedPcieCapability, __IN__ PU32 PtrOffset)
{

    U32          pcieRegisterStatus = 0x0;
    U32          instanceCount= 0x0;
    U32          dword = 0x0;
    U32          capabilityOffset = 0x0;
    U16          currID = 0x0;
//    U8           bytes[1024 * 4];
//    PU32         ptrDwords;
//    U32          index;

    SCRUTINY_STATUS 			status = SCRUTINY_STATUS_FAILED;
    
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppGetCapabilityOffsetInstance (PtrDevice=%x, PortIndex=%x, CapabilityId=%x, InstanceNumber=%x, IsExtendedPcieCapability=%x, PtrOffset=%x)", PtrDevice != NULL, PortIndex, CapabilityId, InstanceNumber, IsExtendedPcieCapability, PtrOffset != NULL);
    


    /* Check if the device, does support Capability or not */
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, PCI_REG_CMD_STAT, &pcieRegisterStatus);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetCapabilityOffsetInstance  (Status = %x) ",status);
        return (status);    
    }
    gPtrLoggerSwitch->logiDebug ("sppGetCapabilityOffsetInstance pcieRegisterStatus = %x", pcieRegisterStatus);
    if (pcieRegisterStatus == 0xFFFFFFFF)
    {
        gPtrLoggerSwitch->logiDebug ("sppGetCapabilityOffsetInstance pcieRegisterStatus = 0xFFFFFFFF");
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("sppGetCapabilityOffsetInstance  (Status = %x) ",status);
        return (status);
    }

    /* We have got the register, check if the capability pointer list is available */

    if ((pcieRegisterStatus & 0x100000) != 0x100000)
    {
        gPtrLoggerSwitch->logiDebug ("sppGetCapabilityOffsetInstance pcieRegisterStatus capability pointer list is not available");
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("sppGetCapabilityOffsetInstance  (Status = %x) ",status);
        return (status);
    }

    instanceCount = 0;

    // Set capability pointer capabilityOffset
    if (IsExtendedPcieCapability)
    {
        // PCIe capabilities must start at 100h
        capabilityOffset = 0x100;
    }
    else
    {
        // Get capabilityOffset of first capability from capability pointer (34h[7:0])
        status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, PCI_REG_CAP_PTR, &dword);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("sppGetCapabilityOffsetInstance  (Status = %x) ",status);
            return (status);    
        }

        //printf("I'm here %x = %x\n", PCI_REG_CAP_PTR, dword);

        // Set first capability capabilityOffset
        capabilityOffset = dword & 0xFF;

    }

    // Ignore instance number for non-VSEC capabilities
    if (CapabilityId != PCI_CAP_ID_VENDOR_SPECIFIC)
    {
        InstanceNumber = 0;
    }

    // Start with 1st match
    instanceCount = 0;

    //printf("%x = %x\n", capabilityOffset, dword);

    //ptrDwords = (PU32) &bytes[0];

    //for (index = 0; index < 1024; index++)
    //{
    //    if (atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, index * 4, &ptrDwords[index]))
    //    {
    //        continue;
    //    }
    //}

    //cmniDumpHexMemory (bytes, sizeof (bytes));
    // Traverse capability list searching for desired ID
    while ((capabilityOffset != 0) && (dword != 0xFFFFFFFF))
    {

        //printf("%x = %x\n", capabilityOffset, dword);


        if ((capabilityOffset == 0xB0) //&&
            /* (pDevice->Key.ApiMode != PLX_API_MODE_PCI) &&
            ((pDevice->Key.PlxPort == PLX_FLAG_PORT_MPT1) ||
            (pDevice->Key.PlxPort == PLX_FLAG_PORT_MPT2) ||
            (pDevice->Key.PlxPort == PLX_FLAG_PORT_MPT3)) */)
        {
            // MPT1-3 break the capability list at B0h (VPD), so override
            dword = 0x0004803;
        }
        else
        {
            // Get next capability
            atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, capabilityOffset, &dword);
        }

        // Verify capability is valid
        if ((dword == 0) || (dword == 0xFFFFFFFF))
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        // Extract the capability ID
        if (IsExtendedPcieCapability)
        {
            // PCIe ID in [15:0]
            currID = (U16)((dword >> 0) & 0xFFFF);
        }
        else
        {
            // PCI ID in [7:0]
            currID = (U16)((dword >> 0) & 0xFF);
        }

        // Compare with desired capability
        if (currID == CapabilityId)
        {
            // Verify correct instance
            if (InstanceNumber == instanceCount)
            {
                // Capability found, return base capabilityOffset
                *PtrOffset = capabilityOffset;
                return (SCRUTINY_STATUS_SUCCESS);
            }

            // Increment count of matches
            instanceCount++;
        }

        // Jump to next capability
        if (IsExtendedPcieCapability)
        {
            // PCIe next cap capabilityOffset in [31:20]
            capabilityOffset = (U16)((dword >> 20) & 0xFFF);
        }
        else
        {
            // PCI next cap capabilityOffset in [15:8]
            capabilityOffset = (U8)((dword >> 8) & 0xFF);
        }
    }

    gPtrLoggerSwitch->logiFunctionExit ("sppGetCapabilityOffsetInstance  (Status = %x) ",status);
    return (status);


}


SCRUTINY_STATUS sppFillPortConfigurationFromCcrSpace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrConfiguration)
{
    U32 regVal;
    U32 ccrAddress;
    U32 shiftLocation;
    U32 switchMode;

    SCRUTINY_STATUS 			status = SCRUTINY_STATUS_FAILED;
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppFillPortConfigurationFromCcrSpace (PtrDevice=%x, PortIndex=%x, PtrConfiguration=%x)", PtrDevice != NULL, PortIndex, PtrConfiguration != NULL);
    
    //get the switch mode
    ccrAddress = ATLAS_REGISTER_PMG_REG_SWITCH_MODE;
    
    status = bsdiMemoryRead32 (PtrDevice, ccrAddress, &regVal, sizeof (U32));
    gPtrLoggerSwitch->logiDebug ("Switch Mode register = %x", regVal);
    switchMode = regVal&0x03;
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromCcrSpace  (Status = %x) ",status);
        return (status);    
    }

    if (switchMode == 0x01)//this is synthetic mode, so need overwrite port type value
    {
        //every 2 bit for a port, operation based on 32-bit Dword
        //Get port type register for synthetic mode   
        ccrAddress = 0xFFF00120 + (PortIndex / 16) * 4;    
        status = bsdiMemoryRead32 (PtrDevice, ccrAddress, &regVal, sizeof (U32));
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromCcrSpace  (Status = %x) ",status);
            return (status);    
        }
        shiftLocation = (PortIndex % 16)*2;
        // Get port type for synthetic mode
        PtrConfiguration->PortType = (SWITCH_PORT_TYPE) ((regVal >> shiftLocation) & 0x3);
        gPtrLoggerSwitch->logiDebug("PortIndex = %x, PortType = %x", PortIndex, PtrConfiguration->PortType);
    }

    

    //get port clock mode register
    ccrAddress = 0xFFF00140 + (PortIndex / 16) * 4;
    
    status = bsdiMemoryRead32 (PtrDevice, ccrAddress, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromCcrSpace  (Status = %x) ",status);
        return (status);    
    }
    shiftLocation = (PortIndex % 16)*2;
    // Get port clock mode
    PtrConfiguration->LinkModeType = (PCI_LINK_MODE_TYPE) ((regVal >> shiftLocation) & 0x3);
    gPtrLoggerSwitch->logiDebug("PortIndex = %x, LinkModeType = %x", PortIndex, PtrConfiguration->LinkModeType);

    gPtrLoggerSwitch->logiFunctionExit ("sppFillPortConfigurationFromCcrSpace  (Status = %x) ",status);
    return (status);
    


}



SCRUTINY_STATUS sppGetDownStreamPortAdditionalProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrPortConfiguration)
{
    SCRUTINY_STATUS 			status = SCRUTINY_STATUS_FAILED;
    U32                         pcieRegisterBusNumber;
    U32                         pcieRegisterPhysicalSlotNumber;
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppGetDownStreamPortAdditionalProperties (PtrDevice=%x, PortIndex=%x, PtrPortConfiguration=%x)", PtrDevice != NULL, PortIndex, PtrPortConfiguration != NULL);

    atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, PCI_REG_T1_PRIM_SEC_BUS, &pcieRegisterBusNumber);
    PtrPortConfiguration->u.Ds.GblBusSec = (U8)((pcieRegisterBusNumber >> 8) & 0xFF);
    PtrPortConfiguration->u.Ds.GblBusSub = (U8)((pcieRegisterBusNumber >> 16) & 0xFF);

    //just get physical slot number from register for base mode, if it is in synthetic mode, the value will be overwriten by configuration page.
    atlasPCIeConfigurationSpaceRead (PtrDevice, PortIndex, PCI_REG_T1_PHY_SLOT, &pcieRegisterPhysicalSlotNumber);
    gPtrLoggerSwitch->logiDebug ("pcieRegisterPhysicalSlotNumber = %x", pcieRegisterPhysicalSlotNumber);
    PtrPortConfiguration->u.Ds.ChassisPhysSlotNum = (U8)((pcieRegisterPhysicalSlotNumber >> 19) & 0x1FFF);
    
    gPtrLoggerSwitch->logiFunctionExit ("sppGetDownStreamPortAdditionalProperties  (Status = %x) ",status);
    return (status);

}



SCRUTINY_STATUS sppGetPciePortPropertiesBsw (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties)
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32                                   index;
    U32                                   chipId;
    U32                                   maxPort;
    SWITCH_PORT_CONFIGURATION             portConfiguration = { 0 };

    SWITCH_ID                             switchID;
    U32                                   regVal;
    U32                                   gepAddress;
    U32                                   tempDsPortMask[4];
    U32                                   savedHostPortIndex = 0;


    SCRUTINY_PCI_ADDRESS busIndex;
    U32 dword = 0xFFFFFFFF;
    
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppGetPciePortPropertiesBsw (PtrDevice=%x,  PtrPciePortProperties=%x)", PtrDevice != NULL,  PtrPciePortProperties != NULL);
    //initialze TotalPortConfigEntry
    PtrPciePortProperties->TotalPortConfigEntry = 0;
    PtrPciePortProperties->TotalHostPort = 0;
    PtrPciePortProperties->TotalDownStreamPort = 0;
    PtrPciePortProperties->TotalFabricPort = 0;
    PtrPciePortProperties->TotalManagementPort = 0;
    
    //get regular port properties
    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesBsw  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);
    
    for (index = 0; index < maxPort; /* Don't increment here */ )
    {
        portConfiguration.MaxLinkWidth = 0;
        /* Get the first port information */
        status = sppGetPcieOnePortProperties (PtrDevice, index, &portConfiguration);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            continue;
        }

        //appPrintPortProperties (index, &portConfiguration);

        //index++;
        index += portConfiguration.MaxLinkWidth;
        if (portConfiguration.MaxLinkWidth != 0)
        {
            sosiMemCopy (&(PtrPciePortProperties->PortConfigurations[PtrPciePortProperties->TotalPortConfigEntry]), &portConfiguration, sizeof (SWITCH_PORT_CONFIGURATION));
            PtrPciePortProperties->TotalPortConfigEntry++;
        }

    }
    //get port 116 and 117 properties
    //I use a trick,MaxLinkWidth is always 1. 
    for (index = ATLAS_PMG_PORT_NUM_X1_1; index <= ATLAS_PMG_PORT_NUM_X1_2; /* Don't increment here */ )
    {

        portConfiguration.MaxLinkWidth = 0;
        /* Get the first port information */
        status = sppGetPcieOnePortProperties (PtrDevice, index, &portConfiguration);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            continue;
        }

        //appPrintPortProperties (index, &portConfiguration);

        //index++;
        index += portConfiguration.MaxLinkWidth; //it always is 1.
        if (portConfiguration.MaxLinkWidth != 0)
        {
            sosiMemCopy (&(PtrPciePortProperties->PortConfigurations[PtrPciePortProperties->TotalPortConfigEntry]), &portConfiguration, sizeof (SWITCH_PORT_CONFIGURATION));
            PtrPciePortProperties->TotalPortConfigEntry++;
        }


    }
    
    //setup GID for every port
            
     //get switch ID register
    gepAddress = ATLAS_REGISTER_PMG_REG_SWITCH_ID;
    
    status = bsdiMemoryRead32 (PtrDevice, gepAddress, &regVal, sizeof (U32));
    gPtrLoggerSwitch->logiDebug ("Switch ID register = %x", regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesBsw  (Status = %x) ",status);
        return (status);    
    }
    switchID.u.DN.Domain = (U8) ((regVal >> 0) & 0xFF);
    switchID.u.DN.Number = (U8) ((regVal >> 16) & 0x3F);
    
    //initialize tempDsPortMask, this is for host port, in base mode there is only one host port.  
    for (index = 0; index < 4; index++)
    {
        tempDsPortMask[index] = 0;
    }
    
    for (index = 0; index < PtrPciePortProperties->TotalPortConfigEntry; index++)
    {
        // fill GID
        PtrPciePortProperties->PortConfigurations[index].GID = (((U32)(switchID.u.ID)) << 8) | PtrPciePortProperties->PortConfigurations[index].PortNumber;
        //do some summary
        switch (PtrPciePortProperties->PortConfigurations[index].PortType)
        {
            case SWITCH_PORT_TYPE_DOWNSTREAM_BSW :
            {                
                PtrPciePortProperties->TotalDownStreamPort++;
                //record Downstream port in tempDsPortMask
                gPtrLoggerSwitch->logiDebug("Before calculate");
                tempDsPortMask[(PtrPciePortProperties->PortConfigurations[index].PortNumber / 32)] |= (((U32)0x01) << (PtrPciePortProperties->PortConfigurations[index].PortNumber % 32));
                gPtrLoggerSwitch->logiDebug("PortNumber = %x,tempDsPortMask: %8x,%8x,%8x,%8x",PtrPciePortProperties->PortConfigurations[index].PortNumber,tempDsPortMask[0],tempDsPortMask[1],tempDsPortMask[2],tempDsPortMask[3]);
                break;
            }
            
            
            case SWITCH_PORT_TYPE_UPSTREAM_BSW :
            {
                PtrPciePortProperties->TotalHostPort++;//base mode expect it is one
                //record the host port index, base mode only has one host port
                savedHostPortIndex = index;
                gPtrLoggerSwitch->logiDebug("Host Port is %x", PtrPciePortProperties->PortConfigurations[index].PortNumber);
                break;
            }
            
            default:
            {
                break;
            }

        }

    }    
    //finish fill GID
    
    gPtrLoggerSwitch->logiDebug("Total Host Port = %x", PtrPciePortProperties->TotalHostPort);
    if (PtrPciePortProperties->TotalHostPort != 0x01)
    {
        status = SCRUTINY_STATUS_FAILED; 
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesBsw  Host Port Number Mismatch (Status = %x) ",status);
        return (status);
    
    }
   //fill host port DsPortMask, this is for host port, in base mode there is only one host port.  
    for (index = 0; index < 4; index++)
    {        
        PtrPciePortProperties->PortConfigurations[savedHostPortIndex].u.Host.DsPortMask[index] = tempDsPortMask[index];
    }
    //fill host port number for Downstream port
    for (index = 0; index < PtrPciePortProperties->TotalPortConfigEntry; index++)
    {
        if (PtrPciePortProperties->PortConfigurations[index].PortType == SWITCH_PORT_TYPE_DOWNSTREAM_BSW)
        {
            PtrPciePortProperties->PortConfigurations[index].u.Ds.HostPortNum =  (U8)(PtrPciePortProperties->PortConfigurations[savedHostPortIndex].PortNumber);
            //find the attached device
            if (PtrPciePortProperties->PortConfigurations[index].NegotiatedLinkWidth)// that means link is up, some device connected to the port
            {

                PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.DataValid = 0;

                
                busIndex.BusNumber = PtrPciePortProperties->PortConfigurations[index].u.Ds.GblBusSec;
                busIndex.DeviceNumber = 0x0;
                busIndex.FunctionNumber = 0x0;

                /*
                 * Read 4 bytes from config space and get the device id information
                 */
                dword = 0xFFFFFFFF;
                if (pcsiReadDword (busIndex, PCI_REG_DEV_VEN_ID, &dword) != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiDebug ("Unable to read the attached device id.");
                    continue;
                }

                if (dword != 0xFFFFFFFF) 
                {
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.VendorId = (U16)(dword & 0xFFFF);
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.DeviceId = (U16)((dword >> 16) & 0xFFFF);                    
                }
                
                dword = 0xFFFFFFFF;
                if (pcsiReadDword (busIndex, PCI_REG_CLASS_REV, &dword) != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiDebug ("Unable to read the attached device class code.");
                    continue;
                }

                if (dword != 0xFFFFFFFF) 
                {
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.Revision = (U8)(dword & 0xFF);
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.ProgIF = (U8)((dword >> 8) & 0xFF);
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.SubClassCode = (U8)((dword >> 16) & 0xFF);
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.ClassCode = (U8)((dword >> 24) & 0xFF);                    
                }

                dword = 0xFFFFFFFF;
                if (pcsiReadDword (busIndex, PCI_REG_DEV_SUB_VEN_ID, &dword) != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiDebug ("Unable to read the attached subdevice id.");
                    continue;
                }

                if (dword != 0xFFFFFFFF) 
                {
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.SubVendorID = (U16)(dword & 0xFFFF);
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.SubDeviceID = (U16)((dword >> 16) & 0xFFFF);                    
                }
                // after finish all the information collection, set data valid
                if (dword != 0xFFFFFFFF)
                {
                    PtrPciePortProperties->PortConfigurations[index].u.Ds.AttachedDevice.DataValid = 1;
                }


            }

        }
    }
            


     
    gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPropertiesBsw  (Status = %x) ",status);

    return (status);


}

