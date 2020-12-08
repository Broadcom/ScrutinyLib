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

#ifndef __SWITCH_HEALTH_MON__H__
#define __SWITCH_HEALTH_MON__H__


#define SWH_HEALTH_CONFIG_INI        "switch_health.ini"
#define SWH_HEALTH_CONFIG_LOG        "switch_health_out.log"



//#define ITEM_NAME(x)        #x

//typedef struct  _ITEMS_LOOKUP_TABLE
//{
//    char*   ItemName;
//    S32     ItemIndex;
//} ITEMS_LOOKUP_TABLE, PTR_ITEMS_LOOKUP_TABLE;


/* The name of the enum should be the same as the INI file */
typedef enum _SWH_GLOBAL_MONITOR_ITEMS 
{
    SWH_TEMPERATURE = 0,
    SWH_ENCL_STATUS,
    SWH_SASADDR_LOW,
    SWH_SASADDR_HIGH,
    SWH_FW_VER,
    SWH_TOTAL_PORTS, 
    SWH_TOTAL_HOST_PORTS,
    SWH_GLOBAL_MAX_ITEMS
} SWH_GLOBAL_MONITOR_ITEMS;


typedef enum _SWH_PORT_MONITOR_ITEMS 
{
    SWH_LINK_UP = 0,
    SWH_MAX_LINK_RATE,
    SWH_NEGOTIATED_LINK_RATE,
    SWH_MAX_LINK_WIDTH,
    SWH_NEGOTIATED_LINK_WIDTH,
    SWH_PORTRECEIVERERRORS,
    SWH_BADTLPERRORS,
    SWH_BADDLLPERRORS,
    SWH_RECOVERYDIAGNOSTICSERRORS,
    SWH_LINKDOWNCOUNT,    
    SWH_PORT_TYPE,
    SWH_PORT_MAX_ITEMS,
} SWH_PORT_MONITOR_ITEMS;


typedef struct _SWH_HEALTH_MONITOR_PORT_DATA
{
    U32         Valid;
    U32         PortMonitorItems[SWH_PORT_MAX_ITEMS];
} SWH_HEALTH_MONITOR_PORT_DATA, *PTR_SWH_HEALTH_MONITOR_PORT_DATA;

typedef struct _SWH_HEALTH_MONITOR_DATA 
{
    /* fields applicable for IOC */
    U32         SwhMonitorItems[SWH_GLOBAL_MAX_ITEMS];

    /* Per Phy data */
    SWH_HEALTH_MONITOR_PORT_DATA     PortMonData[ATLAS_PMG_MAX_PHYS];

} SWH_HEALTH_MONITOR_DATA, *PTR_SWH_HEALTH_MONITOR_DATA;





#define SCRUTINY_CONFIG_SEG_NAME_SWH                         "SWH_Monitor_Gobal"
#define SCRUTINY_CONFIG_SEG_NAME_PORT                        "PORT"











SCRUTINY_STATUS shmiGetHealthData (
    __IN__   PTR_SCRUTINY_DEVICE PtrDevice, 
    __OUT__  PTR_SCRUTINY_SWITCH_HEALTH       PtrSwHealthInfo
);

SCRUTINY_STATUS  shmiSwitchHealthCheck (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __IN__ BOOLEAN DumpToFile, __OUT__ PU32 PtrErrorCode);
SCRUTINY_STATUS shmiFillSwhHealthConfigDataReadBackData (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ PTR_SCRUTINY_SWITCH_HEALTH PtrSwhHealthInfo, __IN__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties, __OUT__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthConfigDataReadBack);
SCRUTINY_STATUS shmiCompareHealthData (__IN__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthConfigDataRef, __IN__ PTR_SWH_HEALTH_MONITOR_DATA  PtrSwhHealthConfigDataReadBack, __OUT__ PTR_SCRUTINY_SWITCH_HEALTH_ERROR_INFO PtrErrorInfo);

#endif


