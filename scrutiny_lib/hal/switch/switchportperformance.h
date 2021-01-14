/*****************************************************************************
*                                                                           *
* Copyright (C) 2019 - 2020 Broadcom. All Rights Reserved.                  *
* Broadcom Confidential. The term "Broadcom" refers to Broadcom Inc.        *
* and/or its subsidiaries.                                                  *
*                                                                           *
*****************************************************************************
*
*           Name:  switchportperformance.h          
 *     Programmer:  Yuanshan
*  Creation Date:  Apr 10, 2020 - 05:12 PM
*
*  Description
*  ------------
*            Switch Port Performance module.
*
*****************************************************************************
*
*/





#ifndef __SWITCH_PORT_PERFORMANCE__H__
#define __SWITCH_PORT_PERFORMANCE__H__


/** Performance monitor control */
typedef enum _PCIE_PORT_PERF_MON_CMD
{
    PCIE_PORT_PERF_MON_CMD_START,                    /**< Command to start Performance Monitor */
    PCIE_PORT_PERF_MON_CMD_STOP                      /**< Command to stop Performance Monitor */
} PCIE_PORT_PERF_MON_CMD;

// Perf Monitor Control (3E0h)
#define PCIE_PERF_MONITOR_START                   0x08000000
#define PCIE_PERF_MONITOR_INFINITE_SAMPLE_ENABLE  0x10000000
#define PCIE_PERF_MONITOR_RESET                   0x40000000
#define PCIE_PERF_MONITOR_ENABLE                  0x80000000

// Perf Monitor InOut Probe RAM Control (3F0h)
#define PCIE_PERF_PROBE_RAM_CTL_RAM_BUF_ENABLE    0x00000001
#define PCIE_PERF_PROBE_RAM_CTL_RAM_RESET         0x00000002
#define PCIE_PERF_PROBE_RAM_CTL_READ_PTR_RESET    0x00000004
#define PCIE_PERF_PROBE_RAM_CTL_CAP_EACH_TRIGGER  0x00000020
#define PCIE_PERF_PROBE_RAM_CTL_NO_COUNT_INCR     0x00000300

// Perf Monitor Datapath BIST Control (768h)
#define PCIE_PERF_DP_BIST_CTL_TIC_ENABLE          0x20000000

// Perf Monitor Egress Control Status (F30h)
#define PCIE_PERF_EGRESS_CTL_STAT_EGRESS_ENABLE   0x00000040

#define PCIE_PERF_COUNTERS_PER_PORT               (14)

// 250 MBps (2.5 Gbps * 80%)
#define PCIE_PERF_MAX_BPS_GEN_1_0             ((unsigned long long)250000000)

// 500 MBps (5 Gbps * 80%)
#define PCIE_PERF_MAX_BPS_GEN_2_0             ((unsigned long long)500000000)

// 1 GBps (8 Gbps)
#define PCIE_PERF_MAX_BPS_GEN_3_0             ((unsigned long long)1000000000)

// 2 GBps (16 Gbps)
#define PCIE_PERF_MAX_BPS_GEN_4_0             ((unsigned long long)2000000000)

//
// Used for performance counter calculations
//

// Overhead DW per TLP
#define PCIE_PERF_TLP_OH_DW                   2

// DW per TLP
#define PCIE_PERF_TLP_DW                      (3 + PCIE_PERF_TLP_OH_DW)

// TLP header bytes with overhead
#define PCIE_PERF_TLP_SIZE                    (PCIE_PERF_TLP_DW * sizeof(U32))

// TLP header bytes w/o overhead
#define PCIE_PERF_TLP_SIZE_NO_OH              (3 * sizeof(U32))

// Bytes per DLLP
#define PCIE_PERF_DLLP_SIZE                   (2 * sizeof(U32))









    
// Initial block of Ingress performance counters
typedef struct _PCIE_PERF_PORT_INGRESS_TLP_COUNTERS
{
    U32 IngPH;
    U32 IngPDW;
    U32 IngNPH;
    U32 IngNPDW;
    U32 IngCplH;
    U32 IngCplDW;
} PCIE_PERF_PORT_INGRESS_TLP_COUNTERS, *PTR_PCIE_PERF_PORT_INGRESS_TLP_COUNTERS;


// Initial block of Egress performance counters
typedef struct _PCIE_PERF_PORT_EGRESS_TLP_COUNTERS
{
    U32 EgPH;
    U32 EgPDW;
    U32 EgNPH;
    U32 EgNPDW;
    U32 EgCplH;
    U32 EgCplDW;
} PCIE_PERF_PORT_EGRESS_TLP_COUNTERS, *PTR_PCIE_PERF_PORT_EGRESS_TLP_COUNTERS;


// Performance Counter Arrangement for a station
typedef struct _PCIE_PERF_MONITOR_READ_FIFO_ONE_STN
{
    PCIE_PERF_PORT_INGRESS_TLP_COUNTERS IngTLPCounters[ATLAS_MAX_PORT_PER_STN];
    PCIE_PERF_PORT_EGRESS_TLP_COUNTERS  EgTLPCounters[ATLAS_MAX_PORT_PER_STN];
    U32                                 IngDLLPCounter[ATLAS_MAX_PORT_PER_STN];
    U32                                 EgDLLPCounter[ATLAS_MAX_PORT_PER_STN];
} PCIE_PERF_MONITOR_READ_FIFO_ONE_STN, *PTR_PCIE_PERF_MONITOR_READ_FIFO_ONE_STN;


// Performance Counter Arrangement for all stations
typedef struct _PCIE_PERF_MONITOR_READ_FIFO
{
    PCIE_PERF_MONITOR_READ_FIFO_ONE_STN PciePerfCounters[ATLAS_MAX_STN];
} PCIE_PERF_MONITOR_READ_FIFO, *PTR_PCIE_PERF_MONITOR_READ_FIFO;




SCRUTINY_STATUS sppGetPciePortPerformance (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance);
SCRUTINY_STATUS sppGetPciePortPerformanceInit (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance );
SCRUTINY_STATUS sppPerfMonControl( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance, __IN__ PCIE_PORT_PERF_MON_CMD Command );
SCRUTINY_STATUS sppPerfGetCounters( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance );
SCRUTINY_STATUS sppPerfGetCountersBuf( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance, __IN__ PU32  PtrBuf, __IN__ U32 BufSize);
SCRUTINY_STATUS sppPerfCalcStatisticsOnePort(    __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance,  __IN__ U32 PortIndex);

#endif /* __SWITCH_PORT_PERFORMANCE__H__ */
