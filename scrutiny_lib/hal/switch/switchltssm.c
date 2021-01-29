/*****************************************************************************
 *                                                                           *
 * Copyright (C) 2019 - 2020 Broadcom. All Rights Reserved.                  *
 * Broadcom Confidential. The term "Broadcom" refers to Broadcom Inc.        *
 * and/or its subsidiaries.                                                  *
 *                                                                           *
 *****************************************************************************
 *
 *        Name : switchltssm.c
 *        Programmer : 
 *        Creation Date : Jan 20th, 2021 
 *
 *  Description
 *  ------------
 *
 *     PCIe LTSSM related APIs
 *
 *
 *******************************************************************************
 *
 */

#include "libincludes.h"
#include "atlas.h"


extern SCRUTINY_STATUS spcGetPexChipId (PTR_SCRUTINY_DEVICE PtrDevice, PU32 PtrChipId);
extern void satcprobeDataDecode(U8 module, U8 bus, U8 port, U8 signal, U32 data, char *output);

SCRUTINY_STATUS sltStopAladinTrace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Station)
{
	U32 regValue;
	SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerSwitch->logiFunctionEntry ("sltStopAladinTrace (PtrDevice=%x, Station=0x%x)", PtrDevice != NULL, Station);

    regValue = (BIT_ALADIN_CAP_CTL_REG_COMPRESSION | BIT_ALADIN_CAP_CTL_REG_ALADIN_ENABLE | BIT_ALADIN_CAP_CTL_REG_CAPTURE_STOP);
    status = bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(Station, ALADIN_CAP_CTL_REG), &regValue);  

    gPtrLoggerSwitch->logiFunctionExit ("sltStopAladinTrace (status=0x%x)", status);
	return status;
}


SCRUTINY_STATUS sltLtssmIsTriggerred ( 
	__IN__ PTR_SCRUTINY_DEVICE  PtrDevice,
	__IN__ U32                  Port, 
	__IN__ PU32                 PtrCaptureStatus
)
{
    SCRUTINY_STATUS  status = SCRUTINY_STATUS_SUCCESS;
    U32              regValue;
    U32              station = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("sltLtssmIsTriggerred (PtrDevice=%x, Port=0x%x, PtrCaptureStatus=%x)", 
        PtrDevice != NULL, Port, PtrCaptureStatus != NULL);
    
    station = Port / ATLAS_PMG_MAX_STNPORT;

    status = bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_STS_REG), &regValue, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("Unable to read register 0x%x", SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_STS_REG));
        gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", status);
        return (status);
    } 

    gPtrLoggerSwitch->logiDebug ("Aladin Cap StS reg: 0x%x", regValue);

    *PtrCaptureStatus = 0;

    if (regValue &  BIT_ALADIN_CAP_STS_REG_TRIGGERED)
    {
        *PtrCaptureStatus = 1;  // triggered

        if (regValue & BIT_ALADIN_CAP_STS_REG_CAP_IN_PROGRESS)
        {
            // triggered but not stop yet, stop it
            sltStopAladinTrace (PtrDevice, station);
        }
    }

    gPtrLoggerSwitch->logiFunctionExit ("sltLtssmIsTriggerred (status=0x%x)", status);
    return (status);    
}

SCRUTINY_STATUS sltLtssmSetup (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Port)
{
    SCRUTINY_STATUS  status;
    U32 		     chipId = 0;
    U32              regValue;
    U32              station = 0, portInStation = 0;
    
    gPtrLoggerSwitch->logiFunctionEntry ("sltLtssmSetup (PtrDevice=%x, Port=0x%x)", PtrDevice != NULL, Port);

    status = spcGetPexChipId (PtrDevice, &chipId);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", status);
    	return (status);
    }

    if (Port > (chipId & 0xFF) || !spcIsPortEnabled (PtrDevice, Port))
    {
        gPtrLoggerSwitch->logiDebug ("Supported Port Number %d ", (chipId & 0xFF));
        gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", SCRUTINY_STATUS_INVALID_PORT);
    	return (SCRUTINY_STATUS_INVALID_PORT);
    }

    station = Port / ATLAS_PMG_MAX_STNPORT;
    portInStation = Port & (ATLAS_PMG_MAX_STNPORT - 1);
    
    // Check whether the Aladin capture is in progress
    status = bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_STS_REG), &regValue, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("Unable to read register 0x%x", SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_STS_REG));
        gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", status);
        return (status);
    }

    if (regValue & BIT_ALADIN_CAP_STS_REG_CAP_IN_PROGRESS)
    {
        gPtrLoggerSwitch->logiDebug ("Aladin Capture is in progress, quit...");
        gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", SCRUTINY_STATUS_RETRY);
        return (SCRUTINY_STATUS_RETRY);
    }


    status = bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_STS_REG), &regValue, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("Unable to read register 0x%x", SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_STS_REG));
        gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", status);
        return (status);
    }

    // Station TestMux Out. Fix value 0x11111111
    regValue = 0x11111111;
    status = bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWTICH_ALADIN_PEX_FINAL_SEL_REG), &regValue);
    // 0 - PHY, 1 - ACCM, 2 - TIC, 3 - INCH, 4 - TIC_HDR_LOG
    regValue = 0;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_TMUX_TIC_SEL_REG), &regValue);
    // 1 - PHY, 2 - DLL, 3 - SOPS
    regValue = 0x11111111;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_TMUX_STN_PHYDLL_SEL_REG), &regValue);
    //  0 - PHY, 1 - SQM, 2 - RDR, 3 - SCH, 4 - Reset, 5 - DMA
    regValue = 0;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_TMUX_STN_TEC_SEL_REG), &regValue);

    //0x80003050 [31]-PCIe Core Aladin En, [19:8]-SQM Port Sel, [7:0]-SQM_A/B
    regValue = 0 | (0 << 4) | (portInStation << 8) | (0 << 12);
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_SQM_SEL_REG), &regValue);

    //0x05000050        [27:20]-TIC/INCH_A/B, [19:16]-ACCM, [15:12]-DLL, [7:0]-PHY_A/B
    regValue = 0;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_SEL0_REG), &regValue);
    
    // 0x00005050        #[15:8]-RCLK, [7:0]-SOPS
    regValue = 0;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_SEL1_REG), &regValue);

    // 0x00300030        #[23:16]-TIC/INCH Port Sel, [15:12]-ACCM Port Sel, [11:8]-DLL Port Sel, [7:0]-PHY_A/B Port Selre
    regValue = (portInStation | (portInStation << 8) | (portInStation << 12) | (portInStation << 16));
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_PROBE_PORT_SEL0_REG), &regValue);

    // [7:0]-SOPS Port Sel
    regValue = (portInStation & 0xF);   
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_PROBE_PORT_SEL1_REG), &regValue);

    // [15:8]-SCH_A/B, [7:0]-RDR_A/B
    regValue = 0;   
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_TEC_SELECT_REG), &regValue);    

    // [15:8]-SCH_A/B Port Sel, [7:0]-RDR_A/B Port Sel
    regValue = (portInStation | (portInStation << 8));   
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_TEC_PORT_SELECT_REG), &regValue);    

    // [15:8]-SHPC_A/B, [7:0]-CHIME_A/B]
    regValue = 0;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_VCORE_PROBE_SEL_REG), &regValue);   

    // [31]-PostCaptureMode, [30:0]-PostCaptureCount 
    regValue = (1024 | (1 << 31));
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_POST_CAP_CFG_REG), &regValue);

    // Trigger Mask 
    regValue = 0xFFFFF000;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DATA_MASK0_LOW_REG), &regValue);    
    regValue = 0xFFFFFFFF;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DATA_MASK0_HIGH_REG), &regValue);    
    regValue = 0xFFFFFFFF;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DATA_MASK1_LOW_REG), &regValue);    
    regValue = 0xFFFFFFFF;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DATA_MASK1_HIGH_REG), &regValue);    

    // Data Mask
    regValue = 0xFFFF0000;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_COMP_MASK_LOW_REG), &regValue);
    regValue = 0xFFFFFFFF;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_COMP_MASK_HIGH_REG), &regValue);

    // Trigger 
    regValue = ALADIN_LTSSM_TRIGGER;
    //regValue = 0x00000301;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_TRIG_DATA_PAT0_LOW_REG), &regValue);    
    regValue = 0x11111111;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_TRIG_DATA_PAT0_HIGH_REG), &regValue);    
    regValue = 0x00000418;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_TRIG_DATA_PAT1_LOW_REG), &regValue);    
    regValue = 0x11111111;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_TRIG_DATA_PAT1_HIGH_REG), &regValue);    

    // trigSrc = Cond 0 , Cond0notEqTo = 0, Cond1notEqTo = 0
    regValue = 0;
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_TRIG_CFG_REG), &regValue);    

    // Enable Aladin and Compression 
    regValue = (BIT_ALADIN_CAP_CTL_REG_COMPRESSION | BIT_ALADIN_CAP_CTL_REG_ALADIN_ENABLE);
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_CTL_REG), &regValue);  

    // Clear trigger status 
    regValue = (BIT_ALADIN_CAP_CTL_REG_COMPRESSION | BIT_ALADIN_CAP_CTL_REG_ALADIN_ENABLE | BIT_ALADIN_CAP_CTL_REG_CLEAR_TRIGGER);
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_CTL_REG), &regValue);  

    regValue = (BIT_ALADIN_CAP_CTL_REG_COMPRESSION | BIT_ALADIN_CAP_CTL_REG_ALADIN_ENABLE);
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_CTL_REG), &regValue);  

    status |= bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_SQM_SEL_REG), &regValue, sizeof (U32));
    regValue |= (1 << 31);
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, SWITCH_ALADIN_STN_SQM_SEL_REG), &regValue);  

    // Capture Start 
    regValue = (BIT_ALADIN_CAP_CTL_REG_COMPRESSION | BIT_ALADIN_CAP_CTL_REG_ALADIN_ENABLE | BIT_ALADIN_CAP_CTL_REG_CAPTURE_START);
    status |= bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_CTL_REG), &regValue);  

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        // if any failure detect, just quit...
        gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", status);
        return (SCRUTINY_STATUS_FAILED);
    }

    gPtrLoggerSwitch->logiDebug ("switch Aladin running");	
    gPtrLoggerSwitch->logiFunctionExit ("sltLtssmSetup (status=0x%x)", status);
    return (status);
}


SCRUTINY_STATUS sltStopAndGetLtssm (
	__IN__ PTR_SCRUTINY_DEVICE              PtrDevice,
	__IN__ U32                              Port,
	__IN__ const char*                      PtrDumpFilePath
)
{
    SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
    char	dumpFileName[512];
    char    tempName[128];
    U32 	tmp, depth, wrap=FALSE;
	U32 	capRamPtr, curPtr;
	SOSI_FILE_HANDLE 	dumpFileHdl;
	U32 	i, fieldIdx;
	U32 	ramDatalow, ramDatahigh, ramDatafmt;
	U32 	capFmt;
    char	probeStr[1024];
    U32 	busA[2048], busB[2048];
	U32		time[2048];
    U32     station = 0, portInStation = 0, regVal, tirggered = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("sltStopAndGetLtssm (PtrDevice=%x, Port=0x%x, PtrDumpFilePath=%x)", PtrDevice != NULL, Port, PtrDumpFilePath != NULL);

    station = Port / ATLAS_PMG_MAX_STNPORT;
    portInStation = Port & (ATLAS_PMG_MAX_STNPORT - 1);

    status = sltLtssmIsTriggerred (PtrDevice, Port, &tmp); 
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiDebug ("Unable to get trigger status");
        gPtrLoggerSwitch->logiFunctionExit ("sltStopAndGetLtssm (status=0x%x)", status);
        return (status);
    }  

    if (tmp == 0) 
    {
        gPtrLoggerSwitch->logiDebug ("LTSSM not triggered");
        sltStopAladinTrace (PtrDevice, station);
    }

    // open the output file
    sosiMemSet (dumpFileName, '\0', sizeof (dumpFileName));
    sosiMemSet (tempName, '\0', sizeof (tempName));
    status = sosiMkDir (PtrDumpFilePath);	
    sosiStringCopy (dumpFileName, PtrDumpFilePath);
    sosiStringCat (dumpFileName, LTSSM_FILE_DEFAULT_DUMP);
    dumpFileHdl = sosiFileOpen(dumpFileName, "wb");
	if(dumpFileHdl == NULL)
	{
        status = SCRUTINY_STATUS_FILE_OPEN_FAILED;
		gPtrLoggerSwitch->logiDebug ("Output file not opened");	
        gPtrLoggerSwitch->logiFunctionExit ("sltStopAndGetLtssm (status=0x%x)", status);
        return (status);
	}

    /* whether the buffer wrapped */
    status = bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_WRAP_COUNT_REG), &tmp, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sosiFileClose(dumpFileHdl);
        gPtrLoggerSwitch->logiDebug ("Unable to read register 0x%x", SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_WRAP_COUNT_REG));
        gPtrLoggerSwitch->logiFunctionExit ("sltStopAndGetLtssm (status=0x%x)", status);
        return (status);
    }    

    if(tmp != 0)
    {
		wrap = TRUE;
    }
    
    status = bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CUR_CAP_RAM_WR_ADDR_REG), &capRamPtr, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        sosiFileClose(dumpFileHdl);
        gPtrLoggerSwitch->logiDebug ("Unable to read register 0x%x", SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_CAP_WRAP_COUNT_REG));
        gPtrLoggerSwitch->logiFunctionExit ("sltStopAndGetLtssm (status=0x%x)", status);
        return (status);
    }
    
	gPtrLoggerSwitch->logiDebug ("satcGetAladinTracedump CapturePointer = %x", capRamPtr);

    if((capRamPtr == 0) && (wrap == FALSE))
	{
		gPtrLoggerSwitch->logiDebug ("No Data in RAM");	        
		sosiFileClose(dumpFileHdl);    
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("sltStopAndGetLtssm (status=0x%x)", status);
		return (status);
	}
    else 
    {
        sosiMemSet (busA, 0, sizeof (busA));
        sosiMemSet (busB, 0, sizeof (busB));
        sosiMemSet (time, 0, sizeof (time));

        regVal = 1;
        bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DCR_DRAM_ACCESS_CTL_REG), &regVal);

        sosiFprintf(dumpFileHdl, "%-30s\tOutput A\tOutput B\tTime(ns)\n", "Decode");

        if(wrap == 0)
		{
			depth = capRamPtr;
			curPtr = 0;
		}
		else
		{
			curPtr = capRamPtr;
			depth = 2048;
		}

        fieldIdx = 0;

        for (i = 0; i < depth; i++)
		{
			if(wrap == 0)
			{
                regVal = curPtr;
                bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DCR_RAM_ADDR_REG), &regVal);
			}
			else
			{
				if(curPtr > 2047)
				{
                    regVal = curPtr-2048;
					bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DCR_RAM_ADDR_REG), &regVal);
				}
				else
				{
                    regVal = curPtr;
					bsdiMemoryWrite32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_DCR_RAM_ADDR_REG), &regVal);
				}
			}
			
            bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_RAM_READ_DATA_LOW_REG), &ramDatalow, sizeof (U32));
            bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_RAM_READ_DATA_HIGH_REG), &ramDatahigh, sizeof (U32));
            bsdiMemoryRead32 (PtrDevice, SWITCH_ALADIN_REG_OFFSET_PER_STATION(station, ALADIN_RAM_READ_FMT_REG), &ramDatafmt, sizeof (U32));

            curPtr++;

			capFmt = !((ramDatafmt>>7) & 0x1);
			if(capFmt == 1)
			{
				busA[fieldIdx] = ramDatalow;
				busB[fieldIdx] = ramDatahigh;

				if(ramDatafmt == 0)
				{
					time[fieldIdx] = 128;
				}
				else
				{
					time[fieldIdx] = ramDatafmt;
				}
				fieldIdx++;
			}
			else
			{
				if(fieldIdx == 0)
				{
                    sosiFileClose(dumpFileHdl);
				    gPtrLoggerSwitch->logiDebug ("No Data in RAM");	
    				return status;
				}

                if(ramDatalow == 0)
				{
					time[fieldIdx -1] = 0xFFFFFFFF;
				}
				else
				{
					time[fieldIdx -1] += ramDatalow;
				}
			}
		}        
    }
    

    for(i = 0; i < depth; i++)
	{
		switch((busA[i]&0xFFF))
		{
			case 0x0:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.Quiet");
				break;
			case 0x1:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.Active");
				break;
			case 0x2:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.Rate");
				break;
			case 0x3:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.Wait12ms");
				break;
			case 0x4:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.P1_Req");
				break;
			case 0x5:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.Donest");
				break;
			case 0x6:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.P0_Req");
				break;
			case 0x7:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.RateOk");
				break;
			case 0x9:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.SpcOp");
				break;
			case 0xA:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.DET_WAIT_P1_ST");
				break;
			case 0xB:
				sosiFprintf(dumpFileHdl, "%-30s", "Detect.DET_WAIT_RATE_ST");
				break;
			case 0x103:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.Active");
				break;
			case 0x101:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.P0_Req");
				break;
			case 0x10B:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.Compliance");
				break;
			case 0x10A:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.TxEIOS");
				break;
			case 0x10E:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.EIdle");
				break;
			case 0x10F:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.Speed");
				break;
			case 0x107:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.Config");
				break;
			case 0x106:
				sosiFprintf(dumpFileHdl, "%-30s", "Polling.Done");
				break;
			case 0x200:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.Idle");
				break;
			case 0x201:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LW_Start_Dn");
				break;
			case 0x210:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LW_Start_Up");
				break;
			case 0x211:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.XLinkArb_Won");
				break;
			case 0x203:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LW_Accept_Dn");
				break;
			case 0x218:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LW_Accept_Up");
				break;
			case 0x202:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LN_Wait_Dn");
				break;
			case 0x208:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LN_Wait_Up");
				break;
			case 0x206:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LN_Accept_Dn");
				break;
			case 0x20C:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.LN_Accept_Up");
				break;
			case 0x207:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.Complete_Dn");
				break;
			case 0x21C:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.Complete_Up");
				break;
			case 0x214:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.TxSDSM");
				break;
			case 0x204:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.TxIdle");
				break;
			case 0x20E:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.Done_St");
				break;
			case 0x20F:
				sosiFprintf(dumpFileHdl, "%-30s", "CFG.TxCtlSkip");
				break;
			case 0x400:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.Idle");
				break;
			case 0x401:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.RcvrLock");
				break;
			case 0x403:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.RcvrCfg");
				break;
			case 0x402:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.TxEIOS");
				break;
			case 0x407:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.Speed");
				break;
			case 0x40B:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.TxIdle");
				break;
			case 0x40A:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.Disable");
				break;
			case 0x40F:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.Loopback");
				break;
			case 0x409:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.Reset");
				break;
			case 0x408:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.TxEIEOS");
				break;
			case 0x40D:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.TxSDSM");
				break;
			case 0x40C:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.ErrCfg");
				break;
			case 0x40E:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.ErrDet");
				break;
			case 0x411:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.EqPh0_Up");
				break;
			case 0x419:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.EqPh1_Up");
				break;
			case 0x418:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.EqPh2_Up");
				break;
			case 0x410:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.EqPh3_Up");
				break;
			case 0x405:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.EqPh1_Dn");
				break;
			case 0x415:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.EqPh2_Dn");
				break;
			case 0x417:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.EqPh3_Dn");
				break;
			case 0x41F:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.MyEqPhase");
				break;
			case 0x41E:
				sosiFprintf(dumpFileHdl, "%-30s", "Recovery.TxCtlSKP");
				break;
			case 0x300:
				sosiFprintf(dumpFileHdl, "%-30s", "L0_Idle");
				break;
			case 0x301:
				sosiFprintf(dumpFileHdl, "%-30s", "L0");
				break;
			case 0x303:
				sosiFprintf(dumpFileHdl, "%-30s", "L0_TxEIOS");
				break;
			case 0x302:
				sosiFprintf(dumpFileHdl, "%-30s", "L0s");
				break;
			case 0x307:
				sosiFprintf(dumpFileHdl, "%-30s", "L1");
				break;
			case 0x30B:
				sosiFprintf(dumpFileHdl, "%-30s", "L2");
				break;
			case 0x30D:
				sosiFprintf(dumpFileHdl, "%-30s", "ActRec");		//Internal state from L0 to Recovery
				break;
			case 0x309:
				sosiFprintf(dumpFileHdl, "%-30s", "L0_Recovery");
				break;
			case 0x305:
				sosiFprintf(dumpFileHdl, "%-30s", "LDWait");
				break;
			case 0x304:
				sosiFprintf(dumpFileHdl, "%-30s", "LinkDown");
				break;
			case 0x600:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_Idle");
				break;
			case 0x605:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_TxEIEOS");
				break;
			case 0x601:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_Dir");
				break;
			case 0x603:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_Tx");
				break;
			case 0x602:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_Rcv");
				break;
			case 0x606:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_RcvWait");
				break;
			case 0x607:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_TxEIOS");
				break;
			case 0x604:
				sosiFprintf(dumpFileHdl, "%-30s", "HR_TxEIOS_1");
				break;
			case 0x500:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_Idle");
				break;
			case 0x501:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_Entry");
				break;
			case 0x504:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_SlvEntry");
				break;
			case 0x506:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_TxEIOS");
				break;
			case 0x50C:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_Speed");
				break;
			case 0x509:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_ActMst");
				break;
			case 0x50A:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_ActSlv");
				break;
			case 0x50B:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_ExitSt");
				break;
			case 0x50F:
				sosiFprintf(dumpFileHdl, "%-30s", "LB_Eidle");
				break;
			case 0x801:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L0sEntry");
				break;
			case 0x802:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L0sIdle");
				break;
			case 0x80A:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L0sTxEIE");
				break;
			case 0x803:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L0sTxFTS");
				break;
			case 0x804:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L0sTxSKP");
				break;
			case 0x80C:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L0sTxSDSM");
				break;
			case 0x905:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L1RxEIOS");
				break;
			case 0x906:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L1Idle");
				break;
			case 0xA08:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L2RxEIOS");
				break;
			case 0xA0F:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L2Idle");
				break;
			case 0xA09:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L2Rate");
				break;
			case 0xA0D:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_L2RateOk");
				break;
			case 0x800:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_Idle");
				break;
			case 0x900:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_Idle");
				break;
			case 0xA00:
				sosiFprintf(dumpFileHdl, "%-30s", "LP_Idle");
				break;
			case 0x700:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_Idle");
				break;
			case 0x709:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_TxEIEOS");
				break;
			case 0x701:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_TxTS1");
				break;
			case 0x703:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_TxEIOS");
				break;
			case 0x707:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_WaitEIOS");
				break;
			case 0x70F:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_Rate");
				break;
			case 0x70E:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_RateOk");
				break;
			case 0x706:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_P1_Req");
				break;
			case 0x704:
				sosiFprintf(dumpFileHdl, "%-30s", "DISABLE_Done");
				break;
			default:
				sosiFprintf(dumpFileHdl, "%-30s", "Unknown State");
				break;
		}
		sosiFprintf(dumpFileHdl, "\t");

        sosiFprintf(dumpFileHdl, "0x%08x\t0x%08x\t%10u\t", busA[i], busB[i], time[i]);

        /******Trigger decode ****/
    	satcprobeDataDecode((U8)0, 0, (U8)portInStation, (U8)0, busA[i], &probeStr[0]);
    	sosiFprintf(dumpFileHdl, probeStr);

        /* By default, L0 state is the trigger point */
        if (((busA[i]&0xFFF) == ALADIN_LTSSM_TRIGGER) &&(tirggered == 0))
        {
            sosiFprintf(dumpFileHdl, "\t<====");
            tirggered = 1;
        }

        sosiFprintf(dumpFileHdl, "\n");

	}
    
    sosiFileClose(dumpFileHdl);
    
    gPtrLoggerSwitch->logiFunctionExit ("sltStopAndGetLtssm (status=0x%x)", status);
    return (status);
}


