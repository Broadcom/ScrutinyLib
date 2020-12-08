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
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES ; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "libincludes.h"
#include "switchregbla.h"


SCRUTINY_STATUS srbiGetPowerOnSense (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PTR_SCRUTINY_SWITCH_POWER_ON_SENSE PtrSwPwrOnSense)
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32                                   ccrAddress;
    U32                                   regVal;

    gPtrLoggerSwitch->logiFunctionEntry ("srbiGetPowerOnSense (PtrDevice=%x,  PtrSwPwrOnSense=%x)", PtrDevice != NULL,  PtrSwPwrOnSense != NULL);

    
    ccrAddress = ATLAS_REGISTER_PMG_REG_POWER_ON_SENSE;
    
    status = bsdiMemoryRead32 (PtrDevice, ccrAddress, &regVal, sizeof (U32));
    gPtrLoggerSwitch->logiDebug ("Power On sense register = %x", regVal);

    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("srbiGetPowerOnSense  (Status = %x) ",status);
        return (status);    
    }

    PtrSwPwrOnSense->SBRCsSel        = regVal & 0x01;                     //[0] PCFG HwInit No PWRONRST SBRCsSel Serial Bootstrap ROM Device Select:
    PtrSwPwrOnSense->SBLDisable      = (regVal >> 1) & 0x01;              //[1] PCFG HwInit No PWRONRST SBLDisable Serial Boot Loader Disable: 
                                                                          //[2] PCFG HwInit No PWRONRST SdbBaudRate Select power on default SDB UART baud rate.
    switch ((regVal >> 2) & 0x01)
    {
        case 0x0 :
        {                
            PtrSwPwrOnSense->SdbBaudRate     = SCRUTINY_BAUD_RATE_115200;
            break;                   
        }
        case 0x1 :
        {                
            PtrSwPwrOnSense->SdbBaudRate     = SCRUTINY_BAUD_RATE_19200;
            break;                   
        }
        default:
        {
            PtrSwPwrOnSense->SdbBaudRate     = SCRUTINY_BAUD_RATE_UNKNOWN;
            break;                           
        }
    }
                                                                         //[26:24] PCFG HwInit No PWRONRST Efuse Lane Enable Lane enable from PCD
    switch ((regVal >> 24) & 0x07)
    {
        case 0x0 :
        {                
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_96;
            break;                   
        }
        case 0x1 :
        {                
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_80;
            break;                   
        }
        case 0x2 :
        {                
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_64;
            break;                   
        }
        case 0x3 :
        {                
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_RESERVED; 
            break;                  
        }
        case 0x4 :
        {                
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_48;
            break;                   
        }
        case 0x5 :
        {                
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_32; 
            break;                  
        }
        case 0x6 :
        {                
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_24;
            break;                   
        }
        default:
        {
            PtrSwPwrOnSense->EnableLaneNumber       = SBL_HW_ENABLE_LANE_NUM_UNKNOWN;
            break;                           
        }

    }
    
                                                                          //[27] PCFG HwInit No PWRONRST Link Speed 0 - Upto GEN4 PCIE Link speed, 1 - Upto GEN3 PCIE Link speed
    switch ((regVal >> 27) & 0x01)
    {
        case 0x0 :
        {                
            PtrSwPwrOnSense->MaxLinkSpeed       = PCI_DEVICE_LINK_SPEED_GEN_4;
            break;                   
        }
        case 0x1 :
        {                
            PtrSwPwrOnSense->MaxLinkSpeed       = PCI_DEVICE_LINK_SPEED_GEN_3;
            break;                   
        }
        default:
        {
            PtrSwPwrOnSense->MaxLinkSpeed       = PCI_DEVICE_LINK_SPEED_LINK_DOWN;
            break;                           
        }
    }


    
    gPtrLoggerSwitch->logiFunctionExit ("srbiGetPowerOnSense  (Status = %x) ",status);
    return (status);    

        
}



SCRUTINY_STATUS srbiGetCcrStatus (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PTR_SCRUTINY_SWITCH_CCR_STATUS PtrSwCcrStatus)
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32                                   ccrAddress;
    U32                                   regVal;

    gPtrLoggerSwitch->logiFunctionEntry ("srbiGetCcrStatus (PtrDevice=%x,  PtrSwPwrOnSense=%x)", PtrDevice != NULL,  PtrSwCcrStatus != NULL);

    
    ccrAddress = ATLAS_REGISTER_PMG_REG_CCR_STATUS;
    
    status = bsdiMemoryRead32 (PtrDevice, ccrAddress, &regVal, sizeof (U32));
    gPtrLoggerSwitch->logiDebug ("CCR Status register = %x", regVal);

    
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("srbiGetCcrStatus  (Status = %x) ",status);
        return (status);    
    }

    PtrSwCcrStatus->S0PcePerstN   = regVal & 0x01;                     //[0] PCFG RO No PWRONRST S0_PCE_PERST_N
    PtrSwCcrStatus->S1PcePerstN   = (regVal >> 1) & 0x01;              //[1] PCFG RO No PWRONRST S1_PCE_PERST_N  
    PtrSwCcrStatus->S2PcePerstN   = (regVal >> 2) & 0x01;              //[2] PCFG RO No PWRONRST S2_PCE_PERST_N 
    PtrSwCcrStatus->S3PcePerstN   = (regVal >> 3) & 0x01;              //[3] PCFG RO No PWRONRST S3_PCE_PERST_N
                                                                       //[21:19] 0 RO No PWRONRST SBLHwBootLoc

    switch ((regVal >> 19) & 0x07)                                    
    {
        case 0x0 :
        {                
            PtrSwCcrStatus->SBLHwBootLoc  = SBL_HW_BOOT_LOC_0000_0400;
            break;                   
        }
        case 0x1 :
        {                
            PtrSwCcrStatus->SBLHwBootLoc  = SBL_HW_BOOT_LOC_0004_0400;
            break;                   
        }
        case 0x2 :
        {                
            PtrSwCcrStatus->SBLHwBootLoc  = SBL_HW_BOOT_LOC_0008_0400; 
            break;                  
        }
        case 0x3 :
        {                
            PtrSwCcrStatus->SBLHwBootLoc  = SBL_HW_BOOT_LOC_000C_0400;
            break;                   
        }
        case 0x4 :
        {                
            PtrSwCcrStatus->SBLHwBootLoc  = SBL_HW_BOOT_LOC_0010_0400;
            break;                   
        }
        
        default:
        {
            PtrSwCcrStatus->SBLHwBootLoc  = SBL_HW_BOOT_LOC_UNKNOWN;
            break;                   
        
        }
    
    }
     
                                                                      //[30:28] PCFG RO No PWRONRST SBLHWStat
    switch ((regVal >> 28) & 0x07)
    {
        case 0x0 :
        {
            PtrSwCcrStatus->SBLHWStat     = SBL_HW_STAT_CHECKSUM_PASS;
            break;
        }
        case 0x1 :
        {
            PtrSwCcrStatus->SBLHWStat     = SBL_HW_STAT_CHECKSUM_FAIL;
            break;
        }
        case 0x2 :
        {
            PtrSwCcrStatus->SBLHWStat     = SBL_HW_STAT_SIG_FAIL;
            break;
        }
        case 0x3 :
        {
            PtrSwCcrStatus->SBLHWStat     = SBL_HW_STAT_BOOT_LOADING;
            break;
        }
        case 0x4 :
        {
            PtrSwCcrStatus->SBLHWStat     = SBL_HW_STAT_BOOT_DISABLED;
            break;
        }
        default :
        {
            PtrSwCcrStatus->SBLHWStat     = SBL_HW_STAT_UNKNOWN;
            break;
        }
        
    }
    PtrSwCcrStatus->PCDValid      = (regVal >> 31) & 0x01;          //[31] PCFG RO No PWRONRST PCD_VALID 

    
    gPtrLoggerSwitch->logiFunctionExit ("srbiGetCcrStatus  (Status = %x) ",status);
    return (status);    

        
}



