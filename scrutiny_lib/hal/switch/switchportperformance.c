/*****************************************************************************
*                                                                           *
* Copyright (C) 2019 - 2020 Broadcom. All Rights Reserved.                  *
* Broadcom Confidential. The term "Broadcom" refers to Broadcom Inc.        *
* and/or its subsidiaries.                                                  *
*                                                                           *
*****************************************************************************
*
*           Name:  switchportperformance.c          
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


#include "libincludes.h"


SCRUTINY_STATUS sppGetPciePortPerformance (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance )
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32                                   portIndex;
    
    gPtrLoggerSwitch->logiFunctionEntry ("sppGetPciePortPerformance (PtrDevice=%x,  PtrPciePortPerformance=%x)", PtrDevice != NULL,  PtrPciePortPerformance != NULL);

    switch (PtrPciePortPerformance->Operation )
    {
        case SWITCH_PCIE_PERF_OP_CMD_START : 
        {
            status = sppGetPciePortPerformanceInit ( PtrDevice, PtrPciePortPerformance );
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
                return (status);    
            }

            status = sppPerfMonControl( PtrDevice, PtrPciePortPerformance, PCIE_PORT_PERF_MON_CMD_START );
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
                return (status);    
            }
            
            break;
        }
        case SWITCH_PCIE_PERF_OP_CMD_READ :
        {
            status = sppPerfGetCounters( PtrDevice, PtrPciePortPerformance );
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
                return (status);    
            }
            
            if ( PtrPciePortPerformance->Flag.DoStatistic  )            
            {  
                if (PtrPciePortPerformance->StatisticElapsedTimeMs == 0)
                {
                    status = SCRUTINY_STATUS_INVALID_PARAMETER;
                    gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
                    return (status);              
                
                }
                
                //copy current counter to pre-counter
                for (portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; portIndex++)
                {
                    if (PtrPciePortPerformance->PortPerfData[portIndex].Valid)
                    {
                        sosiMemCopy ( &(PtrPciePortPerformance->PortPerfData[portIndex].PrePortPerfCounter), &(PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter), sizeof (SWITCH_PCIE_PERF_ONE_PORT_COUNTERS));
                    }
                }
                //delay user-defined time
                sosiSleep (PtrPciePortPerformance->StatisticElapsedTimeMs);

                status = sppPerfGetCounters( PtrDevice, PtrPciePortPerformance );
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
                    return (status);    
                }
                // calculate the statistic
                for (portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; portIndex++)
                {
                    if (PtrPciePortPerformance->PortPerfData[portIndex].Valid)
                    {              
                        status = sppPerfCalcStatisticsOnePort( PtrDevice, PtrPciePortPerformance, portIndex );
                        if (status != SCRUTINY_STATUS_SUCCESS)
                        {
                            gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
                            return (status);    
                        }
                        
                    }
                }
                
                
            }
            break;
        }
        case SWITCH_PCIE_PERF_OP_CMD_STOP :
        {
            status = sppPerfMonControl( PtrDevice, PtrPciePortPerformance, PCIE_PORT_PERF_MON_CMD_STOP );
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
                return (status);    
            }

            break;
        }
        default :
        {
            status = SCRUTINY_STATUS_UNSUPPORTED;
            gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
            return (status);              
        }
    }
    gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformance  (Status = %x) ",status);
    return (status);              

}

SCRUTINY_STATUS sppGetPciePortPerformanceInit (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance )
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32                                   chipId;
    U32                                   maxPort;
    U32                                   portIndex;
    U32                                   maxLinkWidth;
    U32                                   regVal = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("sppGetPciePortPerformanceInit (PtrDevice=%x,  PtrPciePortPerformance=%x,)", PtrDevice != NULL,  PtrPciePortPerformance != NULL);

    //get regular port properties
    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    regVal = 0;
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformanceInit  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);
    PtrPciePortPerformance->ToTalUserPhyNum = maxPort;

    for( portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; portIndex++ )
    {
        if (!(( portIndex < maxPort ) || (portIndex == ATLAS_PMG_PORT_NUM_X1_1) || (portIndex == ATLAS_PMG_PORT_NUM_X1_2)))
        {
            continue;
        }
        // Get PCIe Link Capabilities        
        regVal = 0;
        status = atlasPCIeConfigurationSpaceRead (PtrDevice, portIndex, 0x74, &regVal);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformanceInit  (Status = %x) ",status);
            return (status);    
        }

        // Get max link width
        PtrPciePortPerformance->PortPerfData[portIndex].MaxLinkWidth = (PCI_DEVICE_LINK_WIDTH) ((regVal >> 4) & 0x3F);
        gPtrLoggerSwitch->logiDebug("PortIndex = %x, MaxLinkWidth = %x", portIndex, PtrPciePortPerformance->PortPerfData[portIndex].MaxLinkWidth);
    
        // Get max link speed
        PtrPciePortPerformance->PortPerfData[portIndex].MaxLinkSpeed = (PCI_DEVICE_LINK_SPEED) ((regVal >> 0) & 0xF);
        gPtrLoggerSwitch->logiDebug("PortIndex = %x, MaxLinkSpeed = %x", portIndex,  PtrPciePortPerformance->PortPerfData[portIndex].MaxLinkSpeed);
    
        // Get PCIe Link Status/Control
        regVal = 0;
        status = atlasPCIeConfigurationSpaceRead (PtrDevice, portIndex, 0x78, &regVal);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformanceInit  (Status = %x) ",status);
            return (status);    
        }

        // Get Negotiated link width
        PtrPciePortPerformance->PortPerfData[portIndex].NegotiatedLinkWidth = (PCI_DEVICE_LINK_WIDTH) ((regVal >> 20) & 0x3F);
        gPtrLoggerSwitch->logiDebug("PortIndex = %x, NegotiatedLinkWidth = %x", portIndex, PtrPciePortPerformance->PortPerfData[portIndex].NegotiatedLinkWidth);
    
        // Get Negotiated link speed
        PtrPciePortPerformance->PortPerfData[portIndex].NegotiatedLinkSpeed = (PCI_DEVICE_LINK_SPEED) ((regVal >> 16) & 0xF);
        gPtrLoggerSwitch->logiDebug("PortIndex = %x, NegotiatedLinkSpeed = %x", portIndex, PtrPciePortPerformance->PortPerfData[portIndex].NegotiatedLinkSpeed);


        maxLinkWidth =  (U32) PtrPciePortPerformance->PortPerfData[portIndex].MaxLinkWidth;
        gPtrLoggerSwitch->logiDebug("sppGetPciePortPerformanceInit Port = %d, maxLinkWidth = %x", portIndex , maxLinkWidth);

        if ( maxLinkWidth )
        {
            PtrPciePortPerformance->PortPerfData[portIndex].Valid = 1;
            
            portIndex = portIndex + maxLinkWidth - 1;
        }
        
    }
    gPtrLoggerSwitch->logiFunctionExit ("sppGetPciePortPerformanceInit  (Status = %x) ",status);
    return (status);              

}


SCRUTINY_STATUS sppPerfMonControl( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance, __IN__ PCIE_PORT_PERF_MON_CMD Command )
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;

    U32 portIndex = 0;
    U32 regVal = 0;
    U32 dWord = 0;
    U32 regCommand = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("sppPerfMonControl (PtrDevice=%x,  PtrPciePortPerformance=%x, Command=%x)", PtrDevice != NULL,  PtrPciePortPerformance != NULL, Command);

    //3E0h	 	 	 	 	 	 	 	Monitor Control
    //[25:0]		Sample Count
    //[26]		Reserved
    //[27]		Monitor start
    //[28]		Infinite Sample enable
    //[29]		Probe Mode Trigger to start Sample
    //[30]		Monitor Reset
    //[31]		Monitor Enable


    /* Process command for performance monitor */
    switch (Command)
    {
        case PCIE_PORT_PERF_MON_CMD_START:
        {
            gPtrLoggerSwitch->logiDebug ("Reset & enable monitor with infinite sampling");
            regCommand = PCIE_PERF_MONITOR_ENABLE | PCIE_PERF_MONITOR_RESET |
                         PCIE_PERF_MONITOR_INFINITE_SAMPLE_ENABLE | PCIE_PERF_MONITOR_START;
            break;
        }
        case PCIE_PORT_PERF_MON_CMD_STOP:
        {
            gPtrLoggerSwitch->logiDebug ("Reset & disable monitor");
            regCommand = PCIE_PERF_MONITOR_RESET;
            break;
        }
        default:
        {
            status = SCRUTINY_STATUS_UNSUPPORTED;
            gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
            return (status);              
        }
    }

    // Enable/Disable Performance Counter in each station (or ports if applicable)
    for( portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; portIndex++ )
    {
        // Ingress init. In station port 0 only
        if( (portIndex % 16) == 0 )
        {
            // Set control offset & enable/disable counters in stations
            // Set device configuration
            // Set 3F0[9:8] to disable probe mode interval timer
            // & avoid RAM pointer corruption
            if( Command == PCIE_PORT_PERF_MON_CMD_START )
            {
                //0x3F0 InOut Probe RAM Control Register
                //[0]		RAM Enable
                //[1]	    Reset RAM
                //[2]		Reset Read Pointer
                //[3]       Capture Loop
                //[5:4] 	Capture Type
                //[7:6]  	Trigger Location
                //[9:8]  	Count Increment
                //[10]      State Change Switch
                //[20:11] 	Reserved
                //[30:21]	Last RAM Write Addr
                //[31]	    RAM Full Status
                
                regVal = 0;
                status = atlasPCIeConfigurationSpaceRead (PtrDevice, portIndex, 0x3F0, &regVal);
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
                    return (status);    
                }
                
                dWord =            regVal                                   |
                                   PCIE_PERF_PROBE_RAM_CTL_RAM_BUF_ENABLE   |
                                   PCIE_PERF_PROBE_RAM_CTL_RAM_RESET        |
                                   PCIE_PERF_PROBE_RAM_CTL_READ_PTR_RESET   |
                                   PCIE_PERF_PROBE_RAM_CTL_CAP_EACH_TRIGGER |
                                   PCIE_PERF_PROBE_RAM_CTL_NO_COUNT_INCR ;
                
                status = atlasPCIeConfigurationSpaceWrite (PtrDevice, portIndex, 0x3F0, dWord);
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
                    return (status);    
                }

            }
            //0x768 DP BIST Control Register 
            //[0]	Datapath BIST enable
            //[1]	Datapath BIST ECRC enable
            //[2]	Datapath BIST TX done
            //[3]	Datapath BIST RX done
            //[19:4]	Datapath BIST delay count
            //[27:20]	Datapath BIST extra mode bits
            //[28]	Datapath BIST tlp only appears on accumulator bus 0
            //[29]	Performance monitor enable
            //[30]	DP BIST infinite loop mode
            //[31]	Datapath BIST Pass or fail
            
            regVal = 0;
            status = atlasPCIeConfigurationSpaceRead (PtrDevice, portIndex, 0x768, &regVal);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
                return (status);    
            }

            if( Command == PCIE_PORT_PERF_MON_CMD_START )
            {
                dWord = regVal | PCIE_PERF_DP_BIST_CTL_TIC_ENABLE;
            }
            else
            {
                dWord = regVal & ~PCIE_PERF_DP_BIST_CTL_TIC_ENABLE;
            }
            status = atlasPCIeConfigurationSpaceWrite (PtrDevice, portIndex, 0x768, dWord);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
                return (status);    
            }
        
        
        }

        // Egress init for all ports

        //F30h	 	 	 	 	 	 	 	TEC Control and Status Register
        //[0]  Disable reader 2nd beat start of TLPs
        //[1]  Disable CSR-redirection FIFO full logic
        //[4:2]		Chicken Bits Gr0
        //[5]	Snoop Enable as Requester Id
        //[6]		Performance Mon Mode
        //[7]		Chicken Bit Gr1
        //[8]		Rdr Hdr Prefetch Disable
        //[9]		Vendor Defined Type0 UR
        //[10]		Egress Credit Timeout Enable
        //[13:11]		Egress Credit Timeout Value
        //[14]		Enable TWC tlp start on reader second beat
        //[15]		Reserved
        //[16]		Egress Credit Timeout Status
        //[18:17]		Egress Credit Timeout VCnT
        //[19]		Reserved
        //[20]		Unused saftey bits 0
        //[21]		Use Egress Credit
        //[22]		Unused saftey bits 2
        //[23]		Unused saftey bits 3
        //[24]		Clear TC
        //[30:25]		Reserved
        //[31]		Port Activity
        
        regVal = 0;
        status = atlasPCIeConfigurationSpaceRead (PtrDevice, portIndex, 0xF30, &regVal);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
            return (status);    
        }

        // F30h[21] is egress credit enable but always reads 0, so ensure remains set
        dWord = regVal | ((U32)1 << 21);

        if( Command == PCIE_PORT_PERF_MON_CMD_START )
        {
            dWord = dWord | PCIE_PERF_EGRESS_CTL_STAT_EGRESS_ENABLE;
        }
        else
        {
            dWord = dWord & ~PCIE_PERF_EGRESS_CTL_STAT_EGRESS_ENABLE;
        }

        status = atlasPCIeConfigurationSpaceWrite (PtrDevice, portIndex, 0xF30, dWord);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
            return (status);    
        }


    }


    //3E0h	 	 	 	 	 	 	 	Monitor Control
    //[25:0]		Sample Count
    //[26]		Reserved
    //[27]		Monitor start
    //[28]		Infinite Sample enable
    //[29]		Probe Mode Trigger to start Sample
    //[30]		Monitor Reset
    //[31]		Monitor Enable

    for( portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; portIndex++ )
    {
        // station level
        if( (portIndex % 16) == 0 )
        {
            status = atlasPCIeConfigurationSpaceWrite (PtrDevice, portIndex, 0x3E0, regCommand);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
                return (status);    
            }
        
        }
    }

    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("sppPerfMonControl  (Status = %x) ",status);
    return (status);              
}

SCRUTINY_STATUS sppPerfGetCounters( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance )
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    PU32                                  ptrTempBuf = NULL;
    U32                                   tempBufSize = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("sppPerfGetCounters (PtrDevice=%x,  PtrPciePortPerformance=%x)", PtrDevice != NULL,  PtrPciePortPerformance != NULL);


    tempBufSize = PCIE_PERF_COUNTERS_PER_PORT * ATLAS_PMG_MAX_PHYS * sizeof( U32 );
    ptrTempBuf = (PU32) sosiMemAlloc ( tempBufSize );
    if (ptrTempBuf == NULL)
    {
        status = SCRUTINY_STATUS_NO_MEMORY; 
        gPtrLoggerSwitch->logiFunctionExit ("sppPerfGetCounters  (Status = %x) ",status);
        return (status);              

    }
    sosiMemSet (ptrTempBuf, 0, tempBufSize);    
    status = sppPerfGetCountersBuf ( PtrDevice, PtrPciePortPerformance, ptrTempBuf, tempBufSize);


    sosiMemFree ( ptrTempBuf );    
    gPtrLoggerSwitch->logiFunctionExit ("sppPerfGetCounters  (Status = %x) ",status);
    return (status);              

}

SCRUTINY_STATUS sppPerfGetCountersBuf( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance, __IN__ PU32  PtrBuf, __IN__ U32 BufSize)
{
    SCRUTINY_STATUS 			          status = SCRUTINY_STATUS_FAILED;
    U32          dWord;
    U32          portIndex;
    U32          regVal;
    
    U32          currStation;
    U32          station;
    U32          stnPort;
    U32          idx;
    PU32         ptrCounter = NULL;
    PTR_PCIE_PERF_MONITOR_READ_FIFO          ptrCounterFifo = NULL;
    PTR_PCIE_PERF_PORT_INGRESS_TLP_COUNTERS  ptrIngCounters = NULL;
    PTR_PCIE_PERF_PORT_EGRESS_TLP_COUNTERS   ptrEgCounters = NULL;

    gPtrLoggerSwitch->logiFunctionEntry ("sppPerfGetCountersBuf (PtrDevice=%x,  PtrPciePortPerformance=%x, PtrBuf=%x, BufSize=%x)", PtrDevice != NULL,  PtrPciePortPerformance != NULL, PtrBuf != NULL, BufSize);


/*
 * Notes      :  The counters are retrieved from the Atlas chip as a preset structure.
 *               Each register read returns the next value from the sequence.
 *               Below is a diagram of the structure matching PMG_MONITOR_READ_FIFO.
 *
 *   IN    = Ingress port
 *   EG    = Egress port
 *   PH    = Number of Posted Headers (Write TLPs)
 *   PDW   = Number of Posted DWords
 *   NPH   = Number of Non-Posted Headers
 *   NPDW  = Non-Posted DWords (Read TLP Dwords)
 *   CPLH  = Number of Completion Headers (CPL TLPs)
 *   CPLDW = Number of Completion DWords
 *   DLLP  = Number of DLLPs
 *
 *           Atlas
 *    -----------------------
 *       14 counters/port
 *       16 pts/stn
 *        6 stn (96 ports)
 *
 *      224 counters/station
 *     1334 counters (14 * 16 * 6)
 *
 *          Counters for first Station. Remaining stations are identical,
 *          spaced 0x380 apart.
 *    Offset -----------------
 *         0| Port 0 IN PH    |
 *         4| Port 0 IN PDW   |
 *         8| Port 0 IN NPH   |
 *         C| Port 0 IN NPDW  |
 *        10| Port 0 IN CPLH  |
 *        14| Port 0 IN CPLDW |
 *          |-----------------|
 *        18| Port 1 IN PH    |
 *          |       :         |
 *          |       :         |
 *        2C| Port 1 IN CPLDW |
 *          |/\/\/\/\/\/\/\/\/|
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *          |/\/\/\/\/\/\/\/\/|
 *       168| Port 15 IN PH   |
 *          |       :         |
 *          |       :         |
 *       17C| Port 15 IN CPLDW|
 *          |-----------------|
 *       180| Port 0 EG PH    |
 *       184| Port 0 EG PDW   |
 *       188| Port 0 EG NPH   |
 *       18C| Port 0 EG NPDW  |
 *       190| Port 0 EG CPLH  |
 *       194| Port 0 EG CPLDW |
 *          |-----------------|
 *       198| Port 1 EG PH    |
 *          |       :         |
 *          |       :         |
 *       1B0| Port 1 EG CPLDW |
 *          |/\/\/\/\/\/\/\/\/|
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *          |/\/\/\/\/\/\/\/\/|
 *          |-----------------|
 *       2E8| Port 15 EG PH   |
 *          |       :         |
 *          |       :         |
 *       2FC| Port 15 EG CPLDW|
 *          |-----------------|
 *       300| Port 0 IN DLLP  |
 *       304| Port 1 IN DLLP  |
 *       308| Port 2 IN DLLP  |
 *       30C| Port 3 IN DLLP  |
 *       310| Port 4 IN DLLP  |
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *       33C| Port 15 IN DLLP |
 *          |-----------------|
 *       340| Port 0 EG DLLP  |
 *       344| Port 1 EG DLLP  |
 *       348| Port 2 EG DLLP  |
 *       34C| Port 3 EG DLLP  |
 *          |       :         |
 *          |       :         |
 *          |       :         |
 *       37C| Port 15 EG DLLP |
 *           -----------------
 */

    /* Get memory to store Performance counters. Need 5,376 bytes if storing
     * counters for all ports. This is the same as sizeof(PMG_MONITOR_READ_FIFO).
     */
    if( PtrBuf == NULL )
    {
        status = SCRUTINY_STATUS_NO_MEMORY;
        gPtrLoggerSwitch->logiFunctionExit ("sppPerfGetCountersBuf  (Status = %x) ",status);
        return (status);              
    }
    ptrCounter = PtrBuf;
    ptrCounterFifo = ( PTR_PCIE_PERF_MONITOR_READ_FIFO )ptrCounter;



    //0x3F0 InOut Probe RAM Control Register
    //[0]		RAM Enable
    //[1]	    Reset RAM
    //[2]		Reset Read Pointer
    //[3]       Capture Loop
    //[5:4] 	Capture Type
    //[7:6]  	Trigger Location
    //[9:8]  	Count Increment
    //[10]      State Change Switch
    //[20:11] 	Reserved
    //[30:21]	Last RAM Write Addr
    //[31]	    RAM Full Status

    // RAM control
    dWord = (2 << 4) |   // Capture type ([5:4])
               (1 << 2) |   // Reset read pointer
               (1 << 0);    // Enable RAM


    // Reset RAM read pointers
    for( portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; portIndex++ )
    {
        // station level
        if( (portIndex % ATLAS_MAX_PORT_PER_STN) == 0 )
        {
            status = atlasPCIeConfigurationSpaceWrite (PtrDevice, portIndex, 0x3F0, dWord);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("sppPerfGetCountersBuf  (Status = %x) ",status);
                return (status);    
            }
        
        }
    }


    // Read in all counters
    
    currStation = 0;
    for ( idx = 0; idx < (ATLAS_PMG_MAX_PHYS * PCIE_PERF_COUNTERS_PER_PORT); idx++)
    {
        // Check if reached station boundary
        if( (idx % (PCIE_PERF_COUNTERS_PER_PORT * ATLAS_MAX_PORT_PER_STN)) == 0 )
        {
            // Increment to next station
            currStation = idx / (PCIE_PERF_COUNTERS_PER_PORT * ATLAS_MAX_PORT_PER_STN);

        }

        // Get next counter. To get counters use port 0 of station
        //0x3E4h	 	 	 	 	 	 	 	Monitor Read  FIFO Register
        //[31:0]		Monitor Read  FIFO Register
        status = atlasPCIeConfigurationSpaceRead (PtrDevice, currStation * ATLAS_MAX_PORT_PER_STN, 0x3E4, &regVal);
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("sppPerfGetCountersBuf  (Status = %x) ",status);
            return (status);    
        }
        *ptrCounter = regVal;
        gPtrLoggerSwitch->logiDebug("currStation = %d ptrCounter idx = %d, value = %x", currStation, idx, *ptrCounter);
        // Jump to next counter
        ptrCounter++;
    }

    /* Populate PERF_PROP structs with counters. Each struct is associated with a port.
     * Counters were read for all ports above in a predetermined format (see comment at
     * beginning of this routine). Extract counters for port associated with PERF_PROP
     * struct and populate struct. Then repeat until all structs have been populated.
     */
    for (portIndex = 0; portIndex < ATLAS_PMG_MAX_PHYS; portIndex++)
    {
        if( !(PtrPciePortPerformance->PortPerfData[portIndex].Valid) )
        {
            continue;
        }


        // Calculate starting index for counters based on station
        station = portIndex / ATLAS_MAX_PORT_PER_STN;
        stnPort = portIndex % ATLAS_MAX_PORT_PER_STN;

       // Get pointer to start of Ingress counters. Adjust for port here.
        ptrIngCounters = &ptrCounterFifo->PciePerfCounters[station].IngTLPCounters[stnPort];

        // Get Ingress counters (6 DW/port)
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.IngressPHCounter = ptrIngCounters->IngPH;
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.IngressPDWCounter = ptrIngCounters->IngPDW;
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.IngressNPHCounter = ptrIngCounters->IngNPH;
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.IngressNPDWCounter = ptrIngCounters->IngNPDW;
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.IngressCplHCounter = ptrIngCounters->IngCplH;
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.IngressCplDWCounter = ptrIngCounters->IngCplDW;
        
        // Egress counters start after ingress. Also adjust for port.
        ptrEgCounters = &ptrCounterFifo->PciePerfCounters[station].EgTLPCounters[stnPort];

        /**********************************************************************/
        /*  FIRMWARE WORKAROUND here is implemented due to HWBug CQ 1051251   */
        /**********************************************************************/
        /*  Ingress DW counters count 2 dword overhead per TLP but Egress     */
        /*  DW counters do not. So firmware workaround is following. Since    */
        /*  there is a header counter and one header/TLP then code will add   */
        /*  (2 * Egress header counter) to actual Egress DW counter. There    */
        /*  are 3 Egress counters which are modified: Posted DW, Nonposted DW,*/
        /*  and Completion DW.                                                */
        /**********************************************************************/
        // Get Egress counters (6 DW/port)
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.EgressPHCounter = ptrEgCounters->EgPH;
        /* Adjust for the 2 DW overhead/Header that Egress doesn't account for */            
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.EgressPDWCounter = ptrEgCounters->EgPDW + ( ptrEgCounters->EgPH * PCIE_PERF_TLP_OH_DW );
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.EgressNPHCounter = ptrEgCounters->EgNPH;
        /* Adjust for the 2 DW overhead/Header that Egress doesn't account for */            
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.EgressNPDWCounter = ptrEgCounters->EgNPDW + ( ptrEgCounters->EgNPH * PCIE_PERF_TLP_OH_DW );
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.EgressCplHCounter = ptrEgCounters->EgCplH;        
        /* Adjust for the 2 DW overhead/Header that Egress doesn't account for */
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.EgressCplDWCounter = ptrEgCounters->EgCplDW +  ( ptrEgCounters->EgCplH * PCIE_PERF_TLP_OH_DW );

        // Get DLLP Ingress counters (1 DW/port)
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.IngressDLLPCounter = ptrCounterFifo->PciePerfCounters[station].IngDLLPCounter[stnPort];

        // Get DLLP Egress counters (1 DW/port)
        PtrPciePortPerformance->PortPerfData[portIndex].PortPerfCounter.EgressDLLPCounter = ptrCounterFifo->PciePerfCounters[station].EgDLLPCounter[stnPort];

    }

    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("sppPerfGetCountersBuf  (Status = %x) ",status);
    return (status);              
}




SCRUTINY_STATUS sppPerfCalcStatisticsOnePort(    __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PCIE_PORT_PERFORMANCE   PtrPciePortPerformance,  __IN__ U32 PortIndex)
{
    SCRUTINY_STATUS 			                 status = SCRUTINY_STATUS_FAILED;

    PTR_SWITCH_PCIE_PERF_ONE_PORT_COUNTERS       ptrCounters;
    PTR_SWITCH_PCIE_PERF_ONE_PORT_COUNTERS       ptrPrevCounters;
    PTR_SWITCH_PCIE_PERF_ONE_PORT_STATISTIC      ptrStats;
    unsigned long long                           totalBytes;
    unsigned long long                           maxLinkBandwidth;
    unsigned long long                           payloadAvg;
    unsigned long long                           counterPostedHeader;
    unsigned long long                           counterPostedDW;
    unsigned long long                           counterNonpostedHeader;
    unsigned long long                           counterNonpostedDW;
    unsigned long long                           counterCplHeader;
    unsigned long long                           counterCplDW;
    unsigned long long                           counterDllp;


    gPtrLoggerSwitch->logiFunctionEntry ("sppPerfCalcStatisticsOnePort (PtrDevice=%x,  PtrPciePortPerformance=%x, PortIndex=%d)", PtrDevice != NULL,  PtrPciePortPerformance != NULL, PortIndex);

    
    // Determine theoretical max link rate for 1 second
    switch ( PtrPciePortPerformance->PortPerfData[PortIndex].NegotiatedLinkSpeed )
    {
        case PCI_DEVICE_LINK_SPEED_GEN_1 :
        {   
            maxLinkBandwidth = (unsigned long long)(PCIE_PERF_MAX_BPS_GEN_1_0 * PtrPciePortPerformance->PortPerfData[PortIndex].NegotiatedLinkWidth );
            break;
        }
        case PCI_DEVICE_LINK_SPEED_GEN_2 :
        {   
            maxLinkBandwidth = (unsigned long long)(PCIE_PERF_MAX_BPS_GEN_2_0 * PtrPciePortPerformance->PortPerfData[PortIndex].NegotiatedLinkWidth );
            break;
        }
        case PCI_DEVICE_LINK_SPEED_GEN_3 :
        {   
            maxLinkBandwidth = (unsigned long long)(PCIE_PERF_MAX_BPS_GEN_3_0 * PtrPciePortPerformance->PortPerfData[PortIndex].NegotiatedLinkWidth );
            break;
        }
        case PCI_DEVICE_LINK_SPEED_GEN_4 :
        {   
            maxLinkBandwidth = (unsigned long long)(PCIE_PERF_MAX_BPS_GEN_4_0 * PtrPciePortPerformance->PortPerfData[PortIndex].NegotiatedLinkWidth );
            break;
        }
        default :
        {
            status = SCRUTINY_STATUS_FAILED;
            gPtrLoggerSwitch->logiFunctionExit ("sppPerfCalcStatisticsOnePort  (Status = %x) ",status);
            return (status);              
        }

    }
    gPtrLoggerSwitch->logiDebug (" First maxLinkBandwidth = %llu", maxLinkBandwidth);
    // Adjust rate for specified elapsed period (ms)
    maxLinkBandwidth = (maxLinkBandwidth * PtrPciePortPerformance->StatisticElapsedTimeMs) / 1000;

    gPtrLoggerSwitch->logiDebug ("Second maxLinkBandwidth = %llu", maxLinkBandwidth);
    
    ptrCounters = (PTR_SWITCH_PCIE_PERF_ONE_PORT_COUNTERS) &(PtrPciePortPerformance->PortPerfData[PortIndex].PortPerfCounter);
    ptrPrevCounters = (PTR_SWITCH_PCIE_PERF_ONE_PORT_COUNTERS) &(PtrPciePortPerformance->PortPerfData[PortIndex].PrePortPerfCounter);
    ptrStats = (PTR_SWITCH_PCIE_PERF_ONE_PORT_STATISTIC) &(PtrPciePortPerformance->PortPerfData[PortIndex].PortPerfStats);


    /* Start the Ingress Counter Statistic*/
    //
    // Calculate Ingress actual counters, adjusting for counter wrapping
    //
    counterPostedHeader    = ptrCounters->IngressPHCounter;
    counterPostedDW        = ptrCounters->IngressPDWCounter;
    counterNonpostedHeader = ptrCounters->IngressNPHCounter;
    counterNonpostedDW     = ptrCounters->IngressNPDWCounter;
    counterCplHeader       = ptrCounters->IngressCplHCounter;
    counterCplDW           = ptrCounters->IngressCplDWCounter;
    counterDllp            = ptrCounters->IngressDLLPCounter;

    gPtrLoggerSwitch->logiDebug ("Get current Ingress counter for statistic");
    gPtrLoggerSwitch->logiDebug ("counterPostedHeader = %llu ", counterPostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterPostedDW = %llu ", counterPostedDW);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedHeader = %llu ", counterNonpostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedDW = %llu ", counterNonpostedDW);
    gPtrLoggerSwitch->logiDebug ("counterCplHeader = %llu ", counterCplHeader);
    gPtrLoggerSwitch->logiDebug ("counterCplDW = %llu ", counterCplDW);
    gPtrLoggerSwitch->logiDebug ("counterDllp = %llu ", counterDllp);

    // Add 4GB in case counter wrapped
    if( ptrCounters->IngressPHCounter < ptrPrevCounters->IngressPHCounter )
    {
        counterPostedHeader += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->IngressPDWCounter < ptrPrevCounters->IngressPDWCounter )
    {
        counterPostedDW += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->IngressNPHCounter < ptrPrevCounters->IngressNPHCounter )
    {
        counterNonpostedHeader += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->IngressNPDWCounter < ptrPrevCounters->IngressNPDWCounter )
    {
        counterNonpostedDW += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->IngressCplHCounter < ptrPrevCounters->IngressCplHCounter )
    {
        counterCplHeader += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->IngressCplDWCounter < ptrPrevCounters->IngressCplDWCounter )
    {
        counterCplDW += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->IngressDLLPCounter < ptrPrevCounters->IngressDLLPCounter )
    {
        counterDllp += ((unsigned long long)1 << 32);
    }

    gPtrLoggerSwitch->logiDebug ("Adjust 4GB wrapper for Ingress statistic");
    gPtrLoggerSwitch->logiDebug ("counterPostedHeader = %llu ", counterPostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterPostedDW = %llu ", counterPostedDW);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedHeader = %llu ", counterNonpostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedDW = %llu ", counterNonpostedDW);
    gPtrLoggerSwitch->logiDebug ("counterCplHeader = %llu ", counterCplHeader);
    gPtrLoggerSwitch->logiDebug ("counterCplDW = %llu ", counterCplDW);
    gPtrLoggerSwitch->logiDebug ("counterDllp = %llu ", counterDllp);


    // Determine counter differences
    counterPostedHeader    -= ptrPrevCounters->IngressPHCounter;
    counterPostedDW        -= ptrPrevCounters->IngressPDWCounter;
    counterNonpostedHeader -= ptrPrevCounters->IngressNPHCounter;
    counterNonpostedDW     -= ptrPrevCounters->IngressNPDWCounter;
    counterCplHeader       -= ptrPrevCounters->IngressCplHCounter;
    counterCplDW           -= ptrPrevCounters->IngressCplDWCounter;
    counterDllp            -= ptrPrevCounters->IngressDLLPCounter;


    gPtrLoggerSwitch->logiDebug ("Minus Ingress pre-counter for statistic");
    gPtrLoggerSwitch->logiDebug ("counterPostedHeader = %llu ", counterPostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterPostedDW = %llu ", counterPostedDW);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedHeader = %llu ", counterNonpostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedDW = %llu ", counterNonpostedDW);
    gPtrLoggerSwitch->logiDebug ("counterCplHeader = %llu ", counterCplHeader);
    gPtrLoggerSwitch->logiDebug ("counterCplDW = %llu ", counterCplDW);
    gPtrLoggerSwitch->logiDebug ("counterDllp = %llu ", counterDllp);

    //
    // Calculate statistics
    //

    /*************************************************************************
     * Periodically, the chip counters report less TLP Posted DW than expected
     * in comparison to the total number of TLP Posted Headers.  We need an
     * error check for this, otherwise the Posted Payload becomes incorrect.
     * The fix involves changing the posted header count based on the number of
     * posted DW, assuming a 4 byte Payload.
     ************************************************************************/
    if( (counterPostedHeader * PCIE_PERF_TLP_DW) > counterPostedDW )
    {
        counterPostedHeader = counterPostedDW / (PCIE_PERF_TLP_DW + 1);
    }

    // Posted Payload bytes ((P_DW * size(DW) - (P_TLP * size(P_TLP))
    if( counterPostedDW * sizeof(U32) < counterPostedHeader * PCIE_PERF_TLP_SIZE )
    {
        ptrStats->IngressPayloadWriteBytes = 0;
    }
    else
    {
        ptrStats->IngressPayloadWriteBytes =
            (counterPostedDW * sizeof(U32)) -
            (counterPostedHeader * PCIE_PERF_TLP_SIZE);
    }

    // Completion Payload ((CPL_DW * size(DW) - (CPL_TLP * size(TLP))
    if( counterCplDW * sizeof(U32) < counterCplHeader * PCIE_PERF_TLP_SIZE )
    {
        ptrStats->IngressPayloadReadBytes = 0;
    }
    else
    {
        ptrStats->IngressPayloadReadBytes = (counterCplDW * sizeof(U32)) -
                                     (counterCplHeader * PCIE_PERF_TLP_SIZE);
    }

    // Total payload
    ptrStats->IngressPayloadTotalBytes =
        ptrStats->IngressPayloadWriteBytes + ptrStats->IngressPayloadReadBytes;

    // Average payload size (Payload / (P_TLP + CPL_TLP))
    payloadAvg = counterPostedHeader + counterCplHeader;

    if( payloadAvg != 0 )
    {
        ptrStats->IngressPayloadAvgPerTlp = (unsigned long long)ptrStats->IngressPayloadTotalBytes / payloadAvg;
    }
    else
    {
        ptrStats->IngressPayloadAvgPerTlp = 0;
    }

    // Total number of TLP data ((P_DW + NP_DW + CPL_DW) * size(DW))
    totalBytes = (counterPostedDW    +
                  counterNonpostedDW +
                  counterCplDW) * sizeof(U32);

    // Add DLLPs to total bytes
    totalBytes += (counterDllp * PCIE_PERF_DLLP_SIZE);

    // Total bytes
    ptrStats->IngressTotalBytes = totalBytes;

    // Total byte rate
    ptrStats->IngressTotalByteRate = totalBytes * 1000 / PtrPciePortPerformance->StatisticElapsedTimeMs;

    // Payload rate
    ptrStats->IngressPayloadByteRate = ptrStats->IngressPayloadTotalBytes * 1000 / PtrPciePortPerformance->StatisticElapsedTimeMs;

    // Link Utilization
    if( maxLinkBandwidth == 0 )
    {
        ptrStats->IngressLinkUtilization = 0;
    }
    else
    {
        /* Multiply Link Utilization by extra 100 to capture two digits to the
         * right of decimal point since Atlas does not have floating point
         * support.
         */
        ptrStats->IngressLinkUtilization = (totalBytes * 100 * 100) / maxLinkBandwidth;

        // Account for error margin
        if( ptrStats->IngressLinkUtilization > 10000 )
        {
            ptrStats->IngressLinkUtilization = 10000;
        }
    }


    gPtrLoggerSwitch->logiDebug ("IngressTotalBytes = %llu ", ptrStats->IngressTotalBytes);
    gPtrLoggerSwitch->logiDebug ("IngressTotalByteRate = %llu ", ptrStats->IngressTotalByteRate);
    gPtrLoggerSwitch->logiDebug ("IngressPayloadReadBytes = %llu ", ptrStats->IngressPayloadReadBytes);
    gPtrLoggerSwitch->logiDebug ("IngressPayloadWriteBytes = %llu ", ptrStats->IngressPayloadWriteBytes);
    gPtrLoggerSwitch->logiDebug ("IngressPayloadTotalBytes = %llu ", ptrStats->IngressPayloadTotalBytes);
    gPtrLoggerSwitch->logiDebug ("IngressPayloadAvgPerTlp = %llu ", ptrStats->IngressPayloadAvgPerTlp);
    gPtrLoggerSwitch->logiDebug ("IngressPayloadByteRate = %llu ", ptrStats->IngressPayloadByteRate);
    gPtrLoggerSwitch->logiDebug ("IngressLinkUtilization = %llu ", ptrStats->IngressLinkUtilization);

        /* End the Ingress Counter Statistic*/


////////////////////////////////////////////////
    /* Start the Egress Counter Statistic*/
    //
    // Calculate Egress actual counters, adjusting for counter wrapping
    //
    counterPostedHeader    = ptrCounters->EgressPHCounter;
    counterPostedDW        = ptrCounters->EgressPDWCounter;
    counterNonpostedHeader = ptrCounters->EgressNPHCounter;
    counterNonpostedDW     = ptrCounters->EgressNPDWCounter;
    counterCplHeader       = ptrCounters->EgressCplHCounter;
    counterCplDW           = ptrCounters->EgressCplDWCounter;
    counterDllp            = ptrCounters->EgressDLLPCounter;


    gPtrLoggerSwitch->logiDebug ("Get current Egress counter for statistic");
    gPtrLoggerSwitch->logiDebug ("counterPostedHeader = %llu ", counterPostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterPostedDW = %llu ", counterPostedDW);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedHeader = %llu ", counterNonpostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedDW = %llu ", counterNonpostedDW);
    gPtrLoggerSwitch->logiDebug ("counterCplHeader = %llu ", counterCplHeader);
    gPtrLoggerSwitch->logiDebug ("counterCplDW = %llu ", counterCplDW);
    gPtrLoggerSwitch->logiDebug ("counterDllp = %llu ", counterDllp);


    // Add 4GB in case counter wrapped
    if( ptrCounters->EgressPHCounter < ptrPrevCounters->EgressPHCounter )
    {
        counterPostedHeader += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->EgressPDWCounter < ptrPrevCounters->EgressPDWCounter )
    {
        counterPostedDW += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->EgressNPHCounter < ptrPrevCounters->EgressNPHCounter )
    {
        counterNonpostedHeader += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->EgressNPDWCounter < ptrPrevCounters->EgressNPDWCounter )
    {
        counterNonpostedDW += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->EgressCplHCounter < ptrPrevCounters->EgressCplHCounter )
    {
        counterCplHeader += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->EgressCplDWCounter < ptrPrevCounters->EgressCplDWCounter )
    {
        counterCplDW += ((unsigned long long)1 << 32);
    }

    if( ptrCounters->EgressDLLPCounter < ptrPrevCounters->EgressDLLPCounter )
    {
        counterDllp += ((unsigned long long)1 << 32);
    }

    gPtrLoggerSwitch->logiDebug ("Adjust 4GB wrapper for Egress statistic");
    gPtrLoggerSwitch->logiDebug ("counterPostedHeader = %llu ", counterPostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterPostedDW = %llu ", counterPostedDW);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedHeader = %llu ", counterNonpostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedDW = %llu ", counterNonpostedDW);
    gPtrLoggerSwitch->logiDebug ("counterCplHeader = %llu ", counterCplHeader);
    gPtrLoggerSwitch->logiDebug ("counterCplDW = %llu ", counterCplDW);
    gPtrLoggerSwitch->logiDebug ("counterDllp = %llu ", counterDllp);

    // Determine counter differences
    counterPostedHeader    -= ptrPrevCounters->EgressPHCounter;
    counterPostedDW        -= ptrPrevCounters->EgressPDWCounter;
    counterNonpostedHeader -= ptrPrevCounters->EgressNPHCounter;
    counterNonpostedDW     -= ptrPrevCounters->EgressNPDWCounter;
    counterCplHeader       -= ptrPrevCounters->EgressCplHCounter;
    counterCplDW           -= ptrPrevCounters->EgressCplDWCounter;
    counterDllp            -= ptrPrevCounters->EgressDLLPCounter;

    gPtrLoggerSwitch->logiDebug ("Minus Egress pre-counter for statistic");
    gPtrLoggerSwitch->logiDebug ("counterPostedHeader = %llu ", counterPostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterPostedDW = %llu ", counterPostedDW);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedHeader = %llu ", counterNonpostedHeader);
    gPtrLoggerSwitch->logiDebug ("counterNonpostedDW = %llu ", counterNonpostedDW);
    gPtrLoggerSwitch->logiDebug ("counterCplHeader = %llu ", counterCplHeader);
    gPtrLoggerSwitch->logiDebug ("counterCplDW = %llu ", counterCplDW);
    gPtrLoggerSwitch->logiDebug ("counterDllp = %llu ", counterDllp);


    //
    // Calculate statistics
    //

    /*************************************************************************
     * Periodically, the chip counters report less TLP Posted DW than expected
     * in comparison to the total number of TLP Posted Headers.  We need an
     * error check for this, otherwise the Posted Payload becomes incorrect.
     * The fix involves changing the posted header count based on the number of
     * posted DW, assuming a 4 byte Payload.
     ************************************************************************/
    if( (counterPostedHeader * PCIE_PERF_TLP_DW) > counterPostedDW )
    {
        counterPostedHeader = counterPostedDW / (PCIE_PERF_TLP_DW + 1);
    }

    // Posted Payload bytes ((P_DW * size(DW) - (P_TLP * size(P_TLP))
    if( counterPostedDW * sizeof(U32) < counterPostedHeader * PCIE_PERF_TLP_SIZE )
    {
        ptrStats->EgressPayloadWriteBytes = 0;
    }
    else
    {
        ptrStats->EgressPayloadWriteBytes =
            (counterPostedDW * sizeof(U32)) -
            (counterPostedHeader * PCIE_PERF_TLP_SIZE);
    }

    // Completion Payload ((CPL_DW * size(DW) - (CPL_TLP * size(TLP))
    if( counterCplDW * sizeof(U32) < counterCplHeader * PCIE_PERF_TLP_SIZE )
    {
        ptrStats->EgressPayloadReadBytes = 0;
    }
    else
    {
        ptrStats->EgressPayloadReadBytes = (counterCplDW * sizeof(U32)) -
                                     (counterCplHeader * PCIE_PERF_TLP_SIZE);
    }

    // Total payload
    ptrStats->EgressPayloadTotalBytes =
        ptrStats->EgressPayloadWriteBytes + ptrStats->EgressPayloadReadBytes;

    // Average payload size (Payload / (P_TLP + CPL_TLP))
    payloadAvg = counterPostedHeader + counterCplHeader;

    if( payloadAvg != 0 )
    {
        ptrStats->EgressPayloadAvgPerTlp = (unsigned long long)ptrStats->EgressPayloadTotalBytes / payloadAvg;
    }
    else
    {
        ptrStats->EgressPayloadAvgPerTlp = 0;
    }

    // Total number of TLP data ((P_DW + NP_DW + CPL_DW) * size(DW))
    totalBytes = (counterPostedDW    +
                  counterNonpostedDW +
                  counterCplDW) * sizeof(U32);

    // Add DLLPs to total bytes
    totalBytes += (counterDllp * PCIE_PERF_DLLP_SIZE);

    // Total bytes
    ptrStats->EgressTotalBytes = totalBytes;

    // Total byte rate
    ptrStats->EgressTotalByteRate = totalBytes * 1000 / PtrPciePortPerformance->StatisticElapsedTimeMs;

    // Payload rate
    ptrStats->EgressPayloadByteRate = ptrStats->EgressPayloadTotalBytes * 1000 / PtrPciePortPerformance->StatisticElapsedTimeMs;

    // Link Utilization
    if( maxLinkBandwidth == 0 )
    {
        ptrStats->EgressLinkUtilization = 0;
    }
    else
    {
        /* Multiply Link Utilization by extra 100 to capture two digits to the
         * right of decimal point since Atlas does not have floating point
         * support.
         */
        ptrStats->EgressLinkUtilization = (totalBytes * 100 * 100) / maxLinkBandwidth;

        // Account for error margin
        if( ptrStats->EgressLinkUtilization > 10000 )
        {
            ptrStats->EgressLinkUtilization = 10000;
        }
    }


    gPtrLoggerSwitch->logiDebug ("EgressTotalBytes = %llu ", ptrStats->EgressTotalBytes);
    gPtrLoggerSwitch->logiDebug ("EgressTotalByteRate = %llu ", ptrStats->EgressTotalByteRate);
    gPtrLoggerSwitch->logiDebug ("EgressPayloadReadBytes = %llu ", ptrStats->EgressPayloadReadBytes);
    gPtrLoggerSwitch->logiDebug ("EgressPayloadWriteBytes = %llu ", ptrStats->EgressPayloadWriteBytes);
    gPtrLoggerSwitch->logiDebug ("EgressPayloadTotalBytes = %llu ", ptrStats->EgressPayloadTotalBytes);
    gPtrLoggerSwitch->logiDebug ("EgressPayloadAvgPerTlp = %llu ", ptrStats->EgressPayloadAvgPerTlp);
    gPtrLoggerSwitch->logiDebug ("EgressPayloadByteRate = %llu ", ptrStats->EgressPayloadByteRate);
    gPtrLoggerSwitch->logiDebug ("EgressLinkUtilization = %llu ", ptrStats->EgressLinkUtilization);

    /* End the Egress Counter Statistic*/




    status = SCRUTINY_STATUS_SUCCESS;
    gPtrLoggerSwitch->logiFunctionExit ("sppPerfCalcStatisticsOnePort  (Status = %x) ",status);
    return (status);              
    
}

