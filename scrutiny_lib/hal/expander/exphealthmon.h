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


#ifndef __EXPANDER_HEALTH_MON__H__
#define __EXPANDER_HEALTH_MON__H__

#if defined (OS_WINDOWS)
#define EXP_HEALTH_CONFIG_INI        "\\exp_health.ini"
#define EXP_HEALTH_CONFIG_LOG        "\\exp_health_out.log"
#elif defined (OS_LINUX) || defined (OS_VMWARE)
#define EXP_HEALTH_CONFIG_INI        "/exp_health.ini"
#define EXP_HEALTH_CONFIG_LOG        "/exp_health_out.log"
#endif


//#define EXP_HEALTH_CONFIG_INI        "exp_health.ini"
//#define EXP_HEALTH_CONFIG_LOG        "exp_health_out.log"



/* The name of the enum should be the same as the INI file */
typedef enum _EXP_GLOBAL_MONITOR_ITEMS 
{
    EXP_TEMPERATURE = 0,
    EXP_ENCL_STATUS,
    EXP_SASADDR_LOW,
    EXP_SASADDR_HIGH,
    EXP_FW_VER,
    EXP_TOTAL_PHYS, 
    EXP_SCE_STATUS_ZERO,
    EXP_SCE_STATUS_ONE,
    EXP_SCE_STATUS_TWO,
    EXP_SCE_STATUS_THREE,
    EXP_SCE_STATUS_FOUR,
    EXP_SCE_STATUS_FIVE,
    EXP_SCE_STATUS_SIX,
    EXP_SCE_STATUS_SEVEN,
    EXP_GLOBAL_MAX_ITEMS
} EXP_GLOBAL_MONITOR_ITEMS;


typedef enum _EXP_PHY_MONITOR_ITEMS 
{
    EXP_LINK_UP = 0,
    EXP_PROGRAMMED_MAX_LINK_RATE,
    EXP_PROGRAMMED_MIN_LINK_RATE,
    EXP_HARDWARE_MAX_LINK_RATE,
    EXP_HARDWARE_MIN_LINK_RATE,
    EXP_NEGOTIATED_LINK_RATE,
    EXP_INVALIDDWORDCOUNT,
    EXP_RUNNINGDISPARITYERRORCOUNT,
    EXP_LOSSDWORDSYNCHCOUNT,
    EXP_PHYRESETPROBLEMCOUNT,
    EXP_ATTACHED_DEVICE,    
    EXP_EDFB_ENABLE,
    EXP_ZONE_GROUP,
    EXP_PHY_MAX_ITEMS
} EXP_PHY_MONITOR_ITEMS;


typedef struct _EXP_HEALTH_MONITOR_PHY_DATA
{
    U32         Valid;
    U32         PhyMonitorItems[EXP_PHY_MAX_ITEMS];
} EXP_HEALTH_MONITOR_PHY_DATA, *PTR_EXP_HEALTH_MONITOR_PHY_DATA;

typedef struct _EXP_HEALTH_MONITOR_DATA 
{
    /* fields applicable for IOC */
    U32         ExpMonitorItems[EXP_GLOBAL_MAX_ITEMS];

    /* Per Phy data */
    EXP_HEALTH_MONITOR_PHY_DATA     PhyMonData[EXP_MAX_PHYS];

} EXP_HEALTH_MONITOR_DATA, *PTR_EXP_HEALTH_MONITOR_DATA;





#define SCRUTINY_CONFIG_SEG_NAME_EXP                        "EXP_Monitor_Gobal"
#define SCRUTINY_CONFIG_SEG_NAME_PHY                        "PHY"


#define EXP_REGISTER_EDFB_ENABLE_LOW    0xC3800044
#define EXP_REGISTER_EDFB_ENABLE_HIGH   0xC3800048


SCRUTINY_STATUS ehmiGetHealthData (
    __IN__ PTR_SCRUTINY_DEVICE PtrDevice, 
    __OUT__  PTR_SCRUTINY_EXPANDER_HEALTH       PtrExpHealthInfo
);


SCRUTINY_STATUS ehmiGetExpEnclosureState (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice,
    __OUT__ PU8                 PtrExpState     
);

SCRUTINY_STATUS  ehmiExpanderHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32  PtrErrorCode);
SCRUTINY_STATUS  ehmiFillExpHealthConfigDataReadBackData (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_EXPANDER_HEALTH PtrExpHealthInfo, __OUT__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthConfigDataReadBack);
SCRUTINY_STATUS  ehmiCompareHealthData ( __IN__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthConfigDataRef, __IN__ PTR_EXP_HEALTH_MONITOR_DATA  PtrExpHealthConfigDataReadBack, __OUT__ PTR_SCRUTINY_EXPANDER_HEALTH_ERROR_INFO  PtrErrorInfo);

#endif

