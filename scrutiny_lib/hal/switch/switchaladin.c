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
#include "atlas.h"
#include "switchaladin.h"


/*
* Text Parsing of .conf file
*/




ITEMS_LOOKUP_TABLE  gSwitchAladinCfgMonTable[] =
{
    { ITEM_NAME (MODULEA), MODULEA },
    { ITEM_NAME (MODULEB), MODULEB },
    { ITEM_NAME (STATION), STATION },
    { ITEM_NAME (PORTA), PORTA },
    { ITEM_NAME (PORTB), PORTB },
    { ITEM_NAME (SIGNALA), SIGNALA },
    { ITEM_NAME (SIGNALB), SIGNALB },
    { ITEM_NAME (TRIGMASKA0), TRIGMASKA0 },
    { ITEM_NAME (TRIGMASKB0), TRIGMASKB0 },
    { ITEM_NAME (TRIGMASKA1), TRIGMASKA1 },
    { ITEM_NAME (TRIGMASKB1), TRIGMASKB1 },
    { ITEM_NAME (DATAMASKA), DATAMASKA },
    { ITEM_NAME (DATAMASKB), DATAMASKB },
    { ITEM_NAME (TRIGGERA0), TRIGGERA0 },
    { ITEM_NAME (TRIGGERB0), TRIGGERB0 },
    { ITEM_NAME (TRIGGERA1), TRIGGERA1 },
    { ITEM_NAME (TRIGGERB1), TRIGGERB1 },
    { ITEM_NAME (COND0NOTEQTO), COND0NOTEQTO },
    { ITEM_NAME (COND1NOTEQTO), COND1NOTEQTO },
    { ITEM_NAME (COMPRESSION), COMPRESSION },
    { ITEM_NAME (POSTCAPTUREMODE), POSTCAPTUREMODE },
    { ITEM_NAME (CAPTURELOC), CAPTURELOC },
    { ITEM_NAME (READTIME), READTIME },
    { ITEM_NAME (TRIGSRC), TRIGSRC },
    { ITEM_NAME (EVENTSRC0), EVENTSRC0 },
    { ITEM_NAME (EVENTSRC1), EVENTSRC1 },
    { ITEM_NAME (EVENTCOUNT0), EVENTCOUNT0 },
    { ITEM_NAME (EVENTCOUNT1), EVENTCOUNT1 },
};


static SWITCH_ALADIN_CONFIG_DATA      gSwitchAladinConfigData;

/**
 *
 * @method  satcInitializeAtlasConfiguration 
 *
 * @param   PtrFolderName - input folder name for the ini file
 *
 *
 * @return  Status   '0' for success and  non-zero for failure
 *
 * @brief   parse INI file for parameters used to configure Aladin core
 *
*/


SCRUTINY_STATUS satcInitializeAtlasConfiguration(
	__IN__ PTR_SCRUTINY_DEVICE 		  PtrDevice,
    __IN__ const char*                PtrFolderName
)
{
    char                   		logFileName[512];
    char                   		tempName[128];
    SCRUTINY_STATUS        		status = SCRUTINY_STATUS_SUCCESS;
    PTR_CONFIG_INI_DICTIONARY 	ptrConfigDictionary = NULL;
    U32 						index = 0, tableIndex = 0;
	U32 						tmp = 0;
    PTR_CONFIG_INI_ENTRIES   	ptrEntry;

    gPtrLoggerSwitch->logiFunctionEntry ("satcInitializeAtlasConfiguration (PtrFolderName=%x)", PtrFolderName != NULL);    

    if (PtrFolderName == NULL)
    {
        gPtrLoggerSwitch->logiFunctionExit ("satcInitializeAtlasConfiguration (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
		
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    sosiMemSet (logFileName, '\0', sizeof (logFileName));
    sosiMemSet (tempName, '\0', sizeof (tempName));
    sosiMemSet (&gSwitchAladinConfigData, 0xFF, sizeof (SWITCH_ALADIN_CONFIG_DATA));
    gSwitchAladinConfigData.parsedone = 0;
    
    status = sosiMkDir (PtrFolderName);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("satcInitializeAtlasConfiguration-0(status=0x%x)", status);
		
        return (status);
    }

    sosiStringCopy (logFileName, PtrFolderName);

    sosiStringCat (logFileName, ALADIN_FILE_CONFIG_INI);
	   
    if (lcpiParserProcessINIFile (logFileName, &ptrConfigDictionary))
    {
        /* We don't have to fail for this as of now. */

		gPtrLoggerSwitch->logiFunctionExit ("satcInitializeAtlasConfiguration-1(status=0x%x)", SCRUTINY_STATUS_FAILED);
		
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Check if we have any valid entries */
    if (!lcpiHasAnyValidEntries (ptrConfigDictionary))
    {
        gPtrLoggerSwitch->logiFunctionExit ("satcInitializeAtlasConfiguration-2(status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
	 
    for (index = 0; index < ptrConfigDictionary->TotalEntries; index++)
    {
        ptrEntry = &ptrConfigDictionary->PtrIniEntries[index];

        for (tableIndex = 0; tableIndex < ALADIN_CFG_MAX_ITEMS; tableIndex++)
        {
            if ((sosiStringCompare (ptrEntry->Key, gSwitchAladinCfgMonTable[tableIndex].ItemName)) == SCRUTINY_STATUS_SUCCESS)
            {
				sscanf (ptrEntry->Value, "%8x", &gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[tableIndex].ItemIndex]);
				
			    gPtrLoggerSwitch->logiDebug ("switch aladin config value [%d]=0x%8x string1: %s)", tableIndex,  gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[tableIndex].ItemIndex], ptrEntry->Value);
                break;
            }
        }

    }
	
	gPtrLoggerSwitch->logiDebug ("switch aladin Capture location=0x%8x)", gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[CAPTURELOC].ItemIndex]);
        
    lcpiParserDestroyDictionary(ptrConfigDictionary);

    gSwitchAladinConfigData.parsedone = 1; 

    gPtrLoggerSwitch->logiDebug ("switch Tmux settings");

    status = satcPrepTmuxsettings();

	//Aladin Register Programming
	gPtrLoggerSwitch->logiDebug ("switch Mux configure");
	tmp = 0;
	satcAladinRegWrite (PtrDevice,SWTICH_ALADIN_PEX_FINAL_SEL_REG, (tmp | 0x11111111));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_TMUX_TIC_SEL_REG, gSwitchAladinConfigData.tmuxTicSel);
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_TMUX_STN_PHYDLL_SEL_REG, gSwitchAladinConfigData.tmuxPhydllSel);
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_TMUX_STN_TEC_SEL_REG, gSwitchAladinConfigData.tmuxTecSel);

	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_STN_SQM_SEL_REG, gSwitchAladinConfigData.signalA | (gSwitchAladinConfigData.signalB << 4) | (gSwitchAladinConfigData.portA << 8) | (gSwitchAladinConfigData.portB << 12));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_STN_SEL0_REG, gSwitchAladinConfigData.signalA | (gSwitchAladinConfigData.signalB << 4) | (gSwitchAladinConfigData.signalA << 12) | (gSwitchAladinConfigData.signalA << 16) | (gSwitchAladinConfigData.signalA << 20) | (gSwitchAladinConfigData.signalB << 24));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_STN_SEL1_REG, gSwitchAladinConfigData.signalA | (gSwitchAladinConfigData.signalB << 4) | (gSwitchAladinConfigData.signalA << 8) | (gSwitchAladinConfigData.signalB << 12));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_PROBE_PORT_SEL0_REG, gSwitchAladinConfigData.portA | (gSwitchAladinConfigData.portB << 4) | (gSwitchAladinConfigData.portA << 8) | (gSwitchAladinConfigData.portA << 12) | (gSwitchAladinConfigData.portA << 16) | (gSwitchAladinConfigData.portB << 20));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_PROBE_PORT_SEL1_REG, gSwitchAladinConfigData.portA | (gSwitchAladinConfigData.portB << 4));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_STN_TEC_SELECT_REG, gSwitchAladinConfigData.signalA | (gSwitchAladinConfigData.signalB << 4) | (gSwitchAladinConfigData.signalA << 8) | (gSwitchAladinConfigData.signalB << 12));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_STN_TEC_PORT_SELECT_REG, gSwitchAladinConfigData.portA | (gSwitchAladinConfigData.portB << 4) | (gSwitchAladinConfigData.portA << 8) | (gSwitchAladinConfigData.portB << 12));
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_STN_VCORE_PROBE_SEL_REG, gSwitchAladinConfigData.signalA | (gSwitchAladinConfigData.signalB << 4) | (gSwitchAladinConfigData.signalA << 8) | (gSwitchAladinConfigData.signalB << 12));

	
	//Atlas  Programming
	gPtrLoggerSwitch->logiDebug ("switch Aladin core configure");

	satcAladinRegWrite (PtrDevice,ALADIN_POST_CAP_CFG_REG, (gSwitchAladinConfigData.PostCaptureMode<<31));
	satcAladinRegRead (PtrDevice, ALADIN_POST_CAP_CFG_REG, &tmp);
	satcAladinRegWrite (PtrDevice,ALADIN_POST_CAP_CFG_REG, (tmp | gSwitchAladinConfigData.captureLoc));
	satcAladinRegWrite (PtrDevice,ALADIN_DATA_MASK0_LOW_REG, (gSwitchAladinConfigData.trigMaskA0));
	satcAladinRegWrite (PtrDevice,ALADIN_DATA_MASK0_HIGH_REG, (gSwitchAladinConfigData.trigMaskB0));
	satcAladinRegWrite (PtrDevice,ALADIN_DATA_MASK1_LOW_REG, (gSwitchAladinConfigData.trigMaskA1));
	satcAladinRegWrite (PtrDevice,ALADIN_DATA_MASK1_HIGH_REG, (gSwitchAladinConfigData.trigMaskB1));
	satcAladinRegWrite (PtrDevice,ALADIN_COMP_MASK_LOW_REG, (gSwitchAladinConfigData.dataMaskA));
	satcAladinRegWrite (PtrDevice,ALADIN_COMP_MASK_HIGH_REG, (gSwitchAladinConfigData.dataMaskB));

	satcAladinRegWrite (PtrDevice,ALADIN_TRIG_DATA_PAT0_LOW_REG, (gSwitchAladinConfigData.triggerA0));
	satcAladinRegWrite (PtrDevice,ALADIN_TRIG_DATA_PAT0_HIGH_REG, (gSwitchAladinConfigData.triggerB0));
	satcAladinRegWrite (PtrDevice,ALADIN_TRIG_DATA_PAT1_LOW_REG, (gSwitchAladinConfigData.triggerA1));
	satcAladinRegWrite (PtrDevice,ALADIN_TRIG_DATA_PAT1_HIGH_REG, (gSwitchAladinConfigData.triggerB1));

	satcAladinRegWrite (PtrDevice,ALADIN_TRIG_CFG_REG, (gSwitchAladinConfigData.trigSrc | gSwitchAladinConfigData.Cond0notEqTo <<30 | gSwitchAladinConfigData.Cond1notEqTo<<31));

	satcAladinRegWrite (PtrDevice,ALADIN_EVENT0_COUNT_THRESH_REG, (gSwitchAladinConfigData.eventCnt0));
	satcAladinRegWrite (PtrDevice,ALADIN_EVENT1_COUNT_THRESH_REG, (gSwitchAladinConfigData.eventCnt1));
	
	satcAladinRegWrite (PtrDevice,ALADIN_CAP_CTL_REG, ((1<<16) | 1)); //Enable Aladin and Compression
	satcAladinRegWrite (PtrDevice,ALADIN_CAP_CTL_REG, ((1<<30) | (1<<16) | 1)); //Clear trigger status
	satcAladinRegWrite (PtrDevice,ALADIN_CAP_CTL_REG, ((1<<16) | 1));
	

	satcAladinRegRead (PtrDevice,SWITCH_ALADIN_STN_SQM_SEL_REG, &tmp);
	satcAladinRegWrite (PtrDevice,SWITCH_ALADIN_STN_SQM_SEL_REG, (tmp | 1<<31));

	if (gSwitchAladinConfigData.trigSrc == 7)
	{
		satcAladinRegWrite (PtrDevice, ALADIN_EVENT_COUNT0_REG, (gSwitchAladinConfigData.eventSrc0<<26 | 1<<31));
		satcAladinRegWrite (PtrDevice, ALADIN_EVENT_COUNT1_REG, (gSwitchAladinConfigData.eventSrc1<<26 | 1<<31));
	}
	
	satcAladinRegWrite (PtrDevice, ALADIN_CAP_CTL_REG, (0x00000101 | gSwitchAladinConfigData.Compression<<16)); //Capture start

	gPtrLoggerSwitch->logiDebug ("switch Aladin running");	
	

	gPtrLoggerSwitch->logiFunctionExit ("satcInitializeAtlasConfiguration(status=0x%x)", status);

    return (status);
}

SCRUTINY_STATUS satcSetAladinConfiguration (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __OUT__ PU32 PtrConfigStatus)
{
	SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
	
	status = satcInitializeAtlasConfiguration(PtrDevice, PtrConfigFilePath);

	*PtrConfigStatus = TRUE;	

	gPtrLoggerSwitch->logiDebug ("satcSetAladinConfiguration status: %d", PtrConfigStatus);
	return status;
}

void satcrecoveryReason(U32 reason, char *output)
{
	char str[100]= "";

	if (reason == 0)
	{
		sosiStringCat (str, "None ");
	}
	
	else
	{
		if (reason & 0x1)	sosiStringCat (str, "EI ");

		if (reason>>1&0x1)	sosiStringCat (str, "autoSpCh ");

		if (reason>>2&0x1)	sosiStringCat (str, "nftsTimeout ");

		if (reason>>3&0x1)	sosiStringCat (str, "rcvdTxEios ");

		if (reason>>4&0x1)	sosiStringCat (str, "rxErr ");

		if (reason>>5&0x1)	sosiStringCat (str, "retrain ");

		if (reason>>6&0x1)	sosiStringCat (str, "progHR ");

		if (reason>>7&0x1)	sosiStringCat (str, "lpCmd ");

		if (reason>>8&0x1)	sosiStringCat (str, "disableCmd ");

		if (reason>>9&0x1)	sosiStringCat (str, "frameErr ");
	}

	sosiStringCopy (output, str);
}

void satcprobeDataDecode(U8 module, U8 bus, U8 port, U8 signal, U32 data, char *output)
{
	char str[1024] = "";
	char substr1[100] = "";
	U8	 i;

	/**************************************************************************************
	*	Bus A
	***************************************************************************************/
	if (bus==0)
	{
		switch (module)
		{
		case 0:			//PHY A
			switch (port)
			{
			case 0:			//Port 0
				switch (signal)
				{
				case 0:			//Signal 0
					//PHY A, Port 0, Signal 0
					sprintf (str, "P0_linkTimeout=%d P0_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					//PHY A, Port 0, Signal 1
					sprintf (str, "P0_linkSp=%d P0_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					//PHY A, Port 0, Signal 2
					sprintf (str, "P0_LnReversal=%d ", (data>>3)&0x1);
					break;
				case 3:			//Signal 3
					//PHY A, Port 0, Signal 3
					break;
				case 4:			//Signal 4
					//PHY A, Port 0, Siganl 4
					break;
				case 5:			//Signal 5
					//PHY A, Port 0, Siganl 5
					break;
				case 6:			//Signal 6
					//PHY A, Port 0, Siganl 6
					break;
				case 7:			//Signal 7
					//PHY A, Port 0, Siganl 7
					break;
				case 8:			//Signal 8
					//PHY A, Port 0, Siganl 8
					sprintf (str, "P0 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					//PHY A, Port 0, Siganl 9
					break;
				case 10:		//Signal 10
					//PHY A, Port 0, Siganl 10
					break;
				case 11:		//Signal 11
					//PHY A, Port 0, Siganl 11
					break;
				case 12:		//Signal 12
					//PHY A, Port 0, Siganl 12
					break;
				case 13:		//Signal 13
					//PHY A, Port 0, Siganl 13
					break;
				case 14:		//Signal 14
					//PHY A, Port 0, Siganl 14
					break;
				case 15:		//Signal 15
					//PHY A, Port 0, Siganl 15
					break;
				}
				break;
			case 1:			//Port 1
				switch (signal)
				{
				case 0:			//Signal 0
					//PHY A, Port 1, Siganl 0
					sprintf (str, "P1_linkTimeout=%d P1_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					//PHY A, Port 1, Siganl 1
					sprintf (str, "P1_linkSp=%d P1_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					//PHY A, Port 1, Siganl 2
					break;
				case 3:			//Signal 3
					//PHY A, Port 1, Siganl 3
					break;
				case 4:			//Signal 4
					//PHY A, Port 1, Siganl 4
					break;
				case 5:			//Signal 5
					//PHY A, Port 1, Siganl 5
					break;
				case 6:			//Signal 6
					//PHY A, Port 1, Siganl 6
					break;
				case 7:			//Signal 7
					//PHY A, Port 1, Siganl 7
					break;
				case 8:			//Signal 8
					//PHY A, Port 1, Siganl 8
					sprintf (str, "P1 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					//PHY A, Port 1, Siganl 9
					break;
				case 10:		//Signal 10
					//PHY A, Port 1, Siganl 10
					break;
				case 11:		//Signal 11
					//PHY A, Port 1, Siganl 11
					break;
				case 12:		//Signal 12
					//PHY A, Port 1, Siganl 12
					break;
				case 13:		//Signal 13
					//PHY A, Port 1, Siganl 13
					break;
				case 14:		//Signal 14
					//PHY A, Port 1, Siganl 14
					break;
				case 15:		//Signal 15
					//PHY A, Port 1, Siganl 15
					break;
				}
				break;
			case 2:			//Port 2
				switch (signal)
				{
				case 0:			//Signal 0
					//PHY A, Port 2, Siganl 0
					sprintf (str, "P2_linkTimeout=%d P2_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					//PHY A, Port 2, Siganl 1
					sprintf (str, "P2_linkSp=%d P2_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					//PHY A, Port 2, Siganl 2
					break;
				case 3:			//Signal 3
					//PHY A, Port 2, Siganl 3
					break;
				case 4:			//Signal 4
					//PHY A, Port 2, Siganl 4
					break;
				case 5:			//Signal 5
					//PHY A, Port 2, Siganl 5
					break;
				case 6:			//Signal 6
					//PHY A, Port 2, Siganl 6
					break;
				case 7:			//Signal 7
					//PHY A, Port 2, Siganl 7
					break;
				case 8:			//Signal 8
					//PHY A, Port 2, Siganl 8
					sprintf (str, "P2 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					//PHY A, Port 2, Siganl 9
					break;
				case 10:		//Signal 10
					//PHY A, Port 2, Siganl 10
					break;
				case 11:		//Signal 11
					//PHY A, Port 2, Siganl 11
					break;
				case 12:		//Signal 12
					//PHY A, Port 2, Siganl 12
					break;
				case 13:		//Signal 13
					//PHY A, Port 2, Siganl 13
					break;
				case 14:		//Signal 14
					//PHY A, Port 2, Siganl 14
					break;
				case 15:		//Signal 15
					//PHY A, Port 2, Siganl 15
					break;
				}
				break;
			case 3:			//Port 3
				switch (signal)
				{
				case 0:			//Signal 0
					//PHY A, Port 3, Siganl 0
					sprintf (str, "P3_linkTimeout=%d P3_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					//PHY A, Port 3, Siganl 1
					sprintf (str, "P3_linkSp=%d P3_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					//PHY A, Port 3, Siganl 2
					break;
				case 3:			//Signal 3
					//PHY A, Port 3, Siganl 3
					break;
				case 4:			//Signal 4
					//PHY A, Port 3, Siganl 4
					break;
				case 5:			//Signal 5
					//PHY A, Port 3, Siganl 5
					break;
				case 6:			//Signal 6
					//PHY A, Port 3, Siganl 6
					break;
				case 7:			//Signal 7
					//PHY A, Port 3, Siganl 7
					break;
				case 8:			//Signal 8
					//PHY A, Port 3, Siganl 8
					sprintf (str, "P3 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					//PHY A, Port 3, Siganl 9
					break;
				case 10:		//Signal 10
					//PHY A, Port 3, Siganl 10
					break;
				case 11:		//Signal 11
					//PHY A, Port 3, Siganl 11
					break;
				case 12:		//Signal 12
					//PHY A, Port 3, Siganl 12
					break;
				case 13:		//Signal 13
					//PHY A, Port 3, Siganl 13
					break;
				case 14:		//Signal 14
					//PHY A, Port 3, Siganl 14
					break;
				case 15:		//Signal 15
					//PHY A, Port 3, Siganl 15
					break;
				}
				break;
			case 4:			//Port 4
				switch (signal)
				{
				case 0:			//Signal 0
					//PHY A, Port 4, Siganl 0
					sprintf (str, "P4_linkTimeout=%d P4_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					//PHY A, Port 4, Siganl 1
					sprintf (str, "P4_linkSp=%d P4_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					//PHY A, Port 4, Siganl 2
					break;
				case 3:			//Signal 3
					//PHY A, Port 4, Siganl 3
					break;
				case 4:			//Signal 4
					//PHY A, Port 4, Siganl 4
					break;
				case 5:			//Signal 5
					//PHY A, Port 4, Siganl 5
					break;
				case 6:			//Signal 6
					//PHY A, Port 4, Siganl 6
					break;
				case 7:			//Signal 7
					//PHY A, Port 4, Siganl 7
					break;
				case 8:			//Signal 8
					//PHY A, Port 4, Siganl 8
					sprintf (str, "P4 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					//PHY A, Port 4, Siganl 9
					break;
				case 10:		//Signal 10
					//PHY A, Port 4, Siganl 10
					break;
				case 11:		//Signal 11
					//PHY A, Port 4, Siganl 11
					break;
				case 12:		//Signal 12
					//PHY A, Port 4, Siganl 12
					break;
				case 13:		//Signal 13
					//PHY A, Port 4, Siganl 13
					break;
				case 14:		//Signal 14
					//PHY A, Port 4, Siganl 14
					break;
				case 15:		//Signal 15
					//PHY A, Port 4, Siganl 15
					break;
				}
				break;
			case 5:			//Port 5
				switch (signal)
				{
				case 0:			//Signal 0
					//PHY A, Port 5, Signal 0
					sprintf (str, "P5_linkTimeout=%d P5_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					//PHY A, Port 5, Signal 1
					sprintf (str, "P5_linkSp=%d P5_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					//PHY A, Port 5, Signal 2
					break;
				case 3:			//Signal 3
					//PHY A, Port 5, Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					//PHY A, Port 5, Signal 8
					sprintf (str, "P5 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					//PHY A, Port 5, Signal 13
					sprintf (str, "P5_SkpDet=%d ", data&0x1);
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 6:			//Port 6
				switch (signal)
				{
				case 0:			//Signal 0
					//PHY A, Port 6, Signal 0
					sprintf (str, "P6_linkTimeout=%d P6_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P6_linkSp=%d P6_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:		 	//Signal 8
					sprintf (str, "P6 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					//PHY A, Port 6, Signal 13
					sprintf (str, "P6_SkpDet=%d ", data&0x1);
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 7:			//Port 7
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P7_linkTimeout=%d P7_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P7_linkSp=%d P7_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P7 Recovery Reason=");
					satcrecoveryReason(data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					//PHY A, Port 7, Signal 10
					sprintf (str, "P7_LkTmeout=%d P7_XLkTimeout=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 8:			//Port 8
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P8_linkTimeout=%d P8_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P8_linkSp=%d P8_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P8 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 9:			//Port 9
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P9_linkTimeout=%d P9_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P9_linkSp=%d P9_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P9 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 10:			//Port 10
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P10_linkTimeout=%d P10_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P10_linkSp=%d P10_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P10 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 11:			//Port 10
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P11_linkTimeout=%d P11_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P11_linkSp=%d P11_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P11 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 12:			//Port 12
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P12_linkTimeout=%d P12_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P12_linkSp=%d P12_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P12 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 13:			//Port 13
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P13_linkTimeout=%d P13_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P13_linkSp=%d P13_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P13 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 14:			//Port 10
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P14_linkTimeout=%d P14_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P14_linkSp=%d P14_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P14 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			case 15:			//Port 10
				switch (signal)
				{
				case 0:			//Signal 0
					sprintf (str, "P15_linkTimeout=%d P15_widthDrop=%d ", (data>>17)&0x1, (data>>16)&0x1);
					break;
				case 1:			//Signal 1
					sprintf (str, "P15_linkSp=%d P15_UpCfg=%d ", (data&0x3)+1, data>>2&0x1);
					break;
				case 2:			//Signal 2
					break;
				case 3:			//Signal 3
					break;
				case 4:			//Signal 4
					break;
				case 5:			//Signal 5
					break;
				case 6:			//Signal 6
					break;
				case 7:			//Signal 7
					break;
				case 8:			//Signal 8
					sprintf (str, "P15 Recovery Reason=");
					satcrecoveryReason (data&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 9:			//Signal 9
					break;
				case 10:		//Signal 10
					break;
				case 11:		//Signal 11
					break;
				case 12:		//Signal 12
					break;
				case 13:		//Signal 13
					break;
				case 14:		//Signal 14
					break;
				case 15:		//Signal 15
					break;
				}
				break;
			}
			break;
		case 1:	//DLL A
			switch (port)
			{
			case 0:		//Port 0
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 0, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 0, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 0, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 0, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 0, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 0, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 0, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 0, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 0, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 0, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 0, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 0, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 0, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 0, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 0, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 0, Signal 15
					break;
				}
				break;
			case 1:		//Port 1
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 1, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 1, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 1, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 1, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 1, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 1, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 1, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 1, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 1, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 1, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 1, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 1, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 1, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 1, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 1, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 1, Signal 15
					break;
				}
				break;
			case 2:		//Port 2
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 2, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 2, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 2, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 2, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 2, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 2, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 2, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 2, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 2, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 2, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 2, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 2, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 2, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 2, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 2, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 2, Signal 15
					break;
				}
				break;
			case 3:		//Port 3
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 3, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 3, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 3, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 3, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 3, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 3, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 3, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 3, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 3, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 3, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 3, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 3, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 3, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 3, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 3, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 3, Signal 15
					break;
				}
				break;
			case 4:		//Port 4
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 4, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 4, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 4, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 4, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 4, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 4, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 4, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 4, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 4, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 4, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 4, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 4, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 4, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 4, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 4, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 4, Signal 15
					break;
				}
				break;
			case 5:		//Port 5
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 5, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 5, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 5, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 5, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 5, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 5, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 5, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 5, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 5, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 5, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 5, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 5, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 5, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 5, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 5, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 5, Signal 15
					break;
				}
				break;
			case 6:		//Port 6
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 6, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 6, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 6, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 6, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 6, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 6, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 6, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 6, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 6, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 6, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 6, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 6, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 6, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 6, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 6, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 6, Signal 15
					break;
				}
				break;
			case 7:		//Port 7
				switch (signal)
				{
				case 0:		//Signal 0
					//DLL A, Port 7, Signal 0
					break;
				case 1:		//Signal 1
					//DLL A, Port 7, Signal 1
					break;
				case 2:		//Signal 2
					//DLL A, Port 7, Signal 2
					break;
				case 3:		//Signal 3
					//DLL A, Port 7, Signal 3
					break;
				case 4:		//Signal 4
					//DLL A, Port 7, Signal 4
					break;
				case 5:		//Signal 5
					//DLL A, Port 7, Signal 5
					break;
				case 6:		//Signal 6
					//DLL A, Port 7, Signal 6
					break;
				case 7:		//Signal 7
					//DLL A, Port 7, Signal 7
					break;
				case 8:		//Signal 8
					//DLL A, Port 7, Signal 8
					break;
				case 9:		//Signal 9
					//DLL A, Port 7, Signal 9
					break;
				case 10:	//Signal 10
					//DLL A, Port 7, Signal 10
					break;
				case 11:	//Signal 11
					//DLL A, Port 7, Signal 11
					break;
				case 12:	//Signal 12
					//DLL A, Port 7, Signal 12
					break;
				case 13:	//Signal 13
					//DLL A, Port 7, Signal 13
					break;
				case 14:	//Signal 14
					//DLL A, Port 7, Signal 14
					break;
				case 15:	//Signal 15
					//DLL A, Port 7, Signal 15
					break;
				}
				break;
			}
		case 2:	//SOPS A
			break;
		case 3:	//ACCM A
			break;
		case 4:	//TIC A
			break;
		case 5:	//SQM A
			break;
		case 6:	//RDR A
			break;
		case 7:	//SCH A
			break;
		case 8:	//RESET
			break;
		case 9:	//DMA
			break;
		}
	}
	/**************************************************************************************
	*	Bus B
	***************************************************************************************/
	else if (bus==1)
	{
		switch (module)
		{
		case 0:			//PHY B
			switch (port)
			{
			case 0:			//Port 0
				//PHY B, Port 0, Signal 0
				switch (signal)
				{
				case 0:			//Signal 0
					for(i = 0; i < 4; i++)
					{
						sprintf (str, "L%d=", i+(signal*4));
						switch ((data>>(i*4))&0x7)
						{
						case 0:
							sprintf (str, "Ok ");
							break;
						case 1:
							sprintf (str, "SkpAdded ");
							break;
						case 2:
							sprintf (str, "SkpDel ");
							break;
						case 3:
							sprintf (str, "RvcrDet ");
							break;
						case 4:
							sprintf (str, "8/10Err ");
							break;
						case 5:
							sprintf (str, "OverFlow ");
							break;
						case 6:
							sprintf (str, "UnderFlow ");
							break;
						case 7:
							sprintf (str, "RvcrErr ");
							break;
						}
					}
					break;
				case 1:			//Signal 1
					//PHY B, Port 0, Signal 1
					for (i = 0; i < 4; i++)
					{
						sprintf (str, "L%d=", i+(signal*4));
						switch ((data>>(i*4))&0x7)
						{
						case 0:
							sprintf (str, "Ok ");
							break;
						case 1:
							sprintf (str, "SkpAdded ");
							break;
						case 2:
							sprintf (str, "SkpDel ");
							break;
						case 3:
							sprintf (str, "RvcrDet ");
							break;
						case 4:
							sprintf (str, "8/10Err ");
							break;
						case 5:
							sprintf (str, "OverFlow ");
							break;
						case 6:
							sprintf (str, "UnderFlow ");
							break;
						case 7:
							sprintf (str, "RvcrErr ");
							break;
						}
					}
					break;
				case 2:
					//PHY B, Port 0, Signal 2
					for(i = 0; i < 4; i++)
					{
						sprintf (str, "L%d=", i+(signal*4));
						switch ((data>>(i*4))&0x7)
						{
						case 0:
							sprintf (str, "Ok ");
							break;
						case 1:
							sprintf (str, "SkpAdded ");
							break;
						case 2:
							sprintf (str, "SkpDel ");
							break;
						case 3:
							sprintf (str, "RvcrDet ");
							break;
						case 4:
							sprintf (str, "8/10Err ");
							break;
						case 5:
							sprintf (str, "OverFlow ");
							break;
						case 6:
							sprintf (str, "UnderFlow ");
							break;
						case 7:
							sprintf (str, "RvcrErr ");
							break;
						}
					}
					break;
				case 3:
					//PHY B, Port 0, Signal 3
					for (i = 0; i < 4; i++)
					{
						sprintf (str, "L%d=", i+(signal*4));
						switch ((data>>(i*4))&0x7)
						{
						case 0:
							sprintf (str, "Ok ");
							break;
						case 1:
							sprintf (str, "SkpAdded ");
							break;
						case 2:
							sprintf (str, "SkpDel ");
							break;
						case 3:
							sprintf (str, "RvcrDet ");
							break;
						case 4:
							sprintf (str, "8/10Err ");
							break;
						case 5:
							sprintf (str, "OverFlow ");
							break;
						case 6:
							sprintf (str, "UnderFlow ");
							break;
						case 7:
							sprintf (str, "RvcrErr ");
							break;
						}
					}
					break;
				case 4:
					//PHY B, Port 0, Signal 4
					sprintf (str, "RxValid=0x%04x ", data&0xFFFF);
					break;
				case 5:
					//PHY B, Port 0, Signal 5
					sprintf (str, "RxEI=0x%04x ", data&0xFFFF);
					break;
				case 6:
					//PHY B, Port 0, Signal 6
					sprintf (str, "LkRcvd=0x%04x ", data&0xFFFF);
					break;
				case 7:
					//PHY B, Port 0, Signal 7
					sprintf (str, "LnRcvd=0x%04x ", data&0xFFFF);
					break;
				case 8:
					//PHY B, Port 0, Signal 8
					sprintf (str, "LnZeroRcvd=0x%04x ", data&0xFFFF);
					break;
				case 9:
					//PHY B, Port 0, Signal 9
					sprintf (str, "TsRcvd=0x%04x ", data&0xFFFF);
					break;
				case 10:
					//PHY B, Port 0, Signal 10
					sprintf (str, "Ts8Rcvd=0x%04x ", data&0xFFFF);
					break;
				case 11:
					//PHY B, Port 0, Signal 11
					sprintf (str, "2Ts1Rcvd=0x%04x ", data&0xFFFF);
					break;
				case 12:
					//PHY B, Port 0, Signal 12
					sprintf (str, "8Ts1Rcvd=0x%04x ", data&0xFFFF);
					break;
				case 13:
					//PHY B, Port 0, Signal 13
					sprintf (str, "2Ts2Rcvd=0x%04x ", data&0xFFFF);
					break;
				case 14:
					//PHY B, Port 0, Signal 14
					sprintf (str, "P0 Recovery Reason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcpy(str, substr1);
					break;
				case 15:
					//PHY B, Port 0, Signal 15
					break;
				}
				break;
			case 1:	//Port 1
				switch(signal)
				{
				case 0:		//Signal 0
					//PHY B, Port 1, Signal 0
					sprintf (str, "8Ts2Rcvd=0x%04x ", data&0xFFFF);
					break;
				case 1:
					//PHY B, Port 1, Signal 1
					sprintf (str, "Ts1Rcvd=0x%04x ", data&0xFFFF);
					break;
				case 2:
					//PHY B, Port 1, Signal 2
					sprintf (str, "Ts2Rcvd=0x%04x ", data&0xFFFF);
					break;
				case 3:
					//PHY B, Port 1, Signal 3
					sprintf (str, "P1 LkTimeout=%d 8IdleRcvd=0x%04x ", (data>>17)&0x1, data&0xFFFF);
					break;
				case 4:
					//PHY B, Port 1, Signal 4
					sprintf (str, "Ts1CfgMatch=0x%04x ", data&0xFFFF);
					break;
				case 5:
					//PHY B, Port 1, Signal 5
					sprintf (str, "Ts2CfgMatch=0x%04x ", data&0xFFFF);
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					//PHY B, Port 1, Signal 9
					sprintf (str, "SkpAdd=0x%04x ", data&0xFFFF);
					break;
				case 10:
					//PHY B, Port 1, Signal 10
					sprintf (str, "SkpDel=0x%04x ", data&0xFFFF);
					break;
				case 11:
					break;
				case 12:
					//PHY B, Port 1, Signal 12
					sprintf (str, "LaneUp=0x%04x ", data&0xFFFF);
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
				}
				break;
			case 2:	//Port 2
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					//PHY B, Port 2, Signal 3
					sprintf (str, "FTSDet=0x%04x ", data&0xFFFF);
					break;
				case 4:
					//PHY B, Port 2, Signal 3
					sprintf (str, "P2 Recovery Reason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcpy(str, substr1);
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
				}
				break;
			case 3:	//Port 3
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					//PHY B, Port 3, Signal 1
					sprintf (str, "P3_LkTimeout=%d P3 Recovery Reason=", (data>>17)&0x1);
					satcrecoveryReason(data&0x3FF, substr1);
					strcpy(str, substr1);
					break;
				case 2:
					//PHY B, Port 3, Signal 2
					sprintf (str, "RxCmplRcvd=0x%04x ", data&0xFFFF);
					break;
				case 3:
					//PHY B, Port 3, Signal 3
					sprintf (str, "RxScrambleDisable=0x%04x ", data&0xFFFF);
					break;
				case 4:
					//PHY B, Port 3, Signal 4
					sprintf (str, "8Ts1Mismatch=0x%04x ", data&0xFFFF);
					break;
				case 5:
					//PHY B, Port 3, Signal 5
					sprintf (str, "P0_RxRateCap=%d P1_RxRateCap=%d P2_RxRateCap=%d P3_RxRateCap=%d P4_RxRateCap=%d P5_RxRateCap=%d P6_RxRateCap=%d P7_RxRateCap=%d P8_RxRateCap=%d ",
						((data>>0)&03)+1, ((data>>2)&0x3)+1, ((data>>4)&0x3)+1, ((data>>6)&0x3)+1, ((data>>8)&0x3)+1, ((data>>10)&0x3)+1, ((data>>12)&0x3)+1, ((data>>14)&0x3)+1, ((data>>16)&0x3)+1);
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 3, Signal 14
					sprintf (str, "P9_RxRateCap=%d P10_RxRateCap=%d P11_RxRateCap=%d P12_RxRateCap=%d P13_RxRateCap=%d P14_RxRateCap=%d P15_RxRateCap=%d ",
						((data>>4)&0x3)+1, ((data>>6)&0x3)+1, ((data>>8)&0x3)+1, ((data>>10)&0x3)+1, ((data>>12)&0x3)+1, ((data>>14)&0x3)+1, ((data>>16)&0x3)+1);
					break;
				case 15:
					break;
				}
				break;
			case 4:	//Port 4
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					//PHY B, Port 4, Signal 6
					sprintf (str, "LkTimeout=0x%04x ", data&0xFFFF);
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
				}
				break;
			case 5:	//Port 5
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					//PHY B, Port 5, Signal 6
					sprintf (str, "xLkTimeout=0x%04x ", data&0xFFFF);
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
				}
				break;
			case 6:	//Port 6
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					//PHY B, Port 6, Signal 6
					sprintf (str, "P6_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
				}
				break;
			case 7:	//Port 7
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					//PHY B, Port 7, Signal 1
					sprintf (str, "P5_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					//PHY B, Port 7, Signal 8
					sprintf (str, "P7_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
				}
				break;
			case 8:	//Port 8
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					//PHY B, Port 8, Signal 1
					sprintf (str, "P8_satcrecoveryReason=");
					satcrecoveryReason((data>>4)&0x3FF, substr1);
					sosiStringCat (str, substr1);
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					break;
				case 15:
					break;
				}
				break;
			case 9:	//Port 9
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 9, Signal 14
					sprintf (str, "P9_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 15:
					break;
				}
				break;
			case 10:	//Port 10
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 10, Signal 14
					sprintf (str, "P10_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 15:
					break;
				}
				break;
			case 11:	//Port 11
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 11, Signal 14
					sprintf (str, "P11_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 15:
					break;
				}
				break;
			case 12:	//Port 12
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 12, Signal 14
					sprintf (str, "P12_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 15:
					break;
				}
				break;
			case 13:	//Port 13
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 13, Signal 14
					sprintf (str, "P13_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 15:
					break;
				}
				break;
			case 14:	//Port 14
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 14, Signal 14
					sprintf (str, "P14_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 15:
					break;
				}
				break;
			case 15:	//Port 15
				switch(signal)
				{
				case 0:		//Signal 0
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
				case 13:
					break;
				case 14:
					//PHY B, Port 15, Signal 14
					sprintf (str, "P15_satcrecoveryReason=");
					satcrecoveryReason(data&0x3FF, substr1);
					strcat(str, substr1);
					break;
				case 15:
					break;
				}
				break;
			}
			break;
		case 1:	//DLL
			break;
		case 2:	//SOPS
			break;
		case 3:	//ACCM
			break;
		case 4:	//TIC
			break;
		case 5:	//SQM
			break;
		case 6:	//RDR
			break;
		case 7:	//SCH
			break;
		case 8:	//RESET
			break;
		case 9:	//DMA
			break;
		}
	}
	else
	{
		sprintf (str, "Undefined");
	}

	strcpy(output, str);
}


SCRUTINY_STATUS satcGetAladinTracedump (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrDumpFilePath, __OUT__ PU32 PtrDumpStatus)
{
	SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
	char	dumpFileName[512];
    char    tempName[128];
	U32 	tmp, depth, wrap=FALSE;
	U32 	capRamPtr, curPtr;
	SOSI_FILE_HANDLE 	dumpFileHdl;
	int 	i, fieldIdx;
	U32 	ramDatalow, ramDatahigh, ramDatafmt;
	U32 	capFmt;
	U32 	busA[2048], busB[2048];
	U32		time[2048];
	U32 	moduleA, signalA;
	U32 	moduleB, signalB;
	U32 	trigSrc;
	U32 	portA, portB;
	BOOLEAN		bHdr2 = FALSE;
	U8		cmd;
	char	probeStr[1024];
	U32 	notMaskA0, notMaskA1, notMaskB0, notMaskB1;
	U32 	triggerA0, triggerB0, triggerA1, triggerB1;
	U32 	trigMaskA0, trigMaskB0, trigMaskA1,trigMaskB1;
	U32		messageCode;

	moduleA = gSwitchAladinConfigData.moduleA;
	moduleB = gSwitchAladinConfigData.moduleB;
	signalA = gSwitchAladinConfigData.signalA;
	signalB = gSwitchAladinConfigData.signalB;
	trigSrc = gSwitchAladinConfigData.trigSrc;
	portA   = gSwitchAladinConfigData.portA;
	portB   = gSwitchAladinConfigData.portB;
	triggerA0 = gSwitchAladinConfigData.triggerA0;
	triggerA1 = gSwitchAladinConfigData.triggerA1;
	triggerB0 = gSwitchAladinConfigData.triggerB0;
	triggerB1 = gSwitchAladinConfigData.triggerB1;
	
	trigMaskA0 = gSwitchAladinConfigData.trigMaskA0;
	trigMaskA1 = gSwitchAladinConfigData.trigMaskA1;
	trigMaskB0 = gSwitchAladinConfigData.trigMaskB0;
	trigMaskB1 = gSwitchAladinConfigData.trigMaskB1;

	
	notMaskA0 = ~trigMaskA0;
	notMaskB0 = ~trigMaskB0;
	notMaskA1 = ~trigMaskA1;
	notMaskB1 = ~trigMaskB1;
	

	sosiMemSet (dumpFileName, '\0', sizeof (dumpFileName));
    sosiMemSet (tempName, '\0', sizeof (tempName));
       
    status = sosiMkDir (PtrDumpFilePath);	
    
    sosiStringCopy (dumpFileName, PtrDumpFilePath);

    sosiStringCat (dumpFileName, ALADIN_FILE_DEFAULT_DUMP);

	dumpFileHdl = sosiFileOpen(dumpFileName, "wb");
	if (dumpFileHdl == NULL)
		gPtrLoggerSwitch->logiDebug ("Output file not opened");	
	else
		gPtrLoggerSwitch->logiDebug ("Output file opened success");

	satcAladinRegRead(PtrDevice,ALADIN_CAP_WRAP_COUNT_REG, &tmp);
	if (tmp != 0)
		wrap = TRUE;

	satcAladinRegRead(PtrDevice,ALADIN_CUR_CAP_RAM_WR_ADDR_REG, &capRamPtr);

	gPtrLoggerSwitch->logiDebug ("satcGetAladinTracedump CapturePointer = %x", capRamPtr);

	if ((capRamPtr == 0) && (wrap == FALSE))
	{
		gPtrLoggerSwitch->logiDebug ("No Data in RAM");	
		satcdumpreg(PtrDevice);
		sosiFileClose(dumpFileHdl);
		*PtrDumpStatus = 0;
		return status;
	}
	else
	{
		satcAladinRegWrite(PtrDevice, ALADIN_DCR_DRAM_ACCESS_CTL_REG, 0x1);

		if ((moduleA == 0 && signalA == 0)||((moduleB == 0) && ((signalB <= 4 && portB == 6) ||(signalB <= 4 && portB == 10) || (signalB >=8 && signalB <= 13 && portB == 10))))
		{
			fprintf (dumpFileHdl, "%-30s\tOutput A\tOutput B\tTime(ns)\n", "Decode");
		}
		else
		{
			fprintf (dumpFileHdl, "Output A\tOutput B\tTime(ns)\n");
		}

		if (wrap == 0)
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

		for(i = 0; i < depth; i++)
		{
			if (wrap == 0)
			{
				satcAladinRegWrite(PtrDevice, ALADIN_DCR_RAM_ADDR_REG, curPtr);
			}
			else
			{
				if (curPtr > 2047)
				{
					//PlxPci_PlxMappedRegisterWrite(pDevice, 0x900000+(station<<16)+0x2000+0xC, currentPointer - 2048);
					satcAladinRegWrite(PtrDevice, ALADIN_DCR_RAM_ADDR_REG, curPtr-2048);
				}
				else
				{
					//PlxPci_PlxMappedRegisterWrite(pDevice, 0x900000+(station<<16)+0x2000+0xC, currentPointer);
					satcAladinRegWrite(PtrDevice, ALADIN_DCR_RAM_ADDR_REG, curPtr);
				}
			}
			curPtr++;

			satcAladinRegRead(PtrDevice,ALADIN_RAM_READ_DATA_LOW_REG, &ramDatalow);
			satcAladinRegRead(PtrDevice,ALADIN_RAM_READ_DATA_HIGH_REG, &ramDatahigh);
			satcAladinRegRead(PtrDevice,ALADIN_RAM_READ_FMT_REG, &ramDatafmt);

			capFmt = !((ramDatafmt>>7) & 0x1);
			if (capFmt == 1)
			{
				busA[fieldIdx] = ramDatalow;
				busB[fieldIdx] = ramDatahigh;

				if (ramDatafmt == 0)
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
				if (fieldIdx == 0)
				{
				gPtrLoggerSwitch->logiDebug ("No Data in RAM");	
				satcdumpreg(PtrDevice);
				*PtrDumpStatus = 1;
				return status;
				
				}
				if (ramDatalow == 0)
				{
					time[fieldIdx -1] = 0xFFFFFFFF;
				}
				else
				{
					time[fieldIdx -1] += ramDatalow;
				}
			}
		}
			/*-------------------------------------------*/
	for(i = 0; i < depth; i++)
		{
		if ((moduleA == 0) && (signalA == 0))
		{
			switch((busA[i]&0xFFF))
			{
				case 0x0:
					fprintf (dumpFileHdl, "%-30s", "Detect.Quiet");
					break;
				case 0x1:
					fprintf (dumpFileHdl, "%-30s", "Detect.Active");
					break;
				case 0x2:
					fprintf (dumpFileHdl, "%-30s", "Detect.Rate");
					break;
				case 0x3:
					fprintf (dumpFileHdl, "%-30s", "Detect.Wait12ms");
					break;
				case 0x4:
					fprintf (dumpFileHdl, "%-30s", "Detect.P1_Req");
					break;
				case 0x5:
					fprintf (dumpFileHdl, "%-30s", "Detect.Donest");
					break;
				case 0x6:
					fprintf (dumpFileHdl, "%-30s", "Detect.P0_Req");
					break;
				case 0x7:
					fprintf (dumpFileHdl, "%-30s", "Detect.RateOk");
					break;
				case 0x9:
					fprintf (dumpFileHdl, "%-30s", "Detect.SpcOp");
					break;
				case 0xA:
					fprintf (dumpFileHdl, "%-30s", "Detect.DET_WAIT_P1_ST");
					break;
				case 0xB:
					fprintf (dumpFileHdl, "%-30s", "Detect.DET_WAIT_RATE_ST");
					break;
				case 0x103:
					fprintf (dumpFileHdl, "%-30s", "Polling.Active");
					break;
				case 0x101:
					fprintf (dumpFileHdl, "%-30s", "Polling.P0_Req");
					break;
				case 0x10B:
					fprintf (dumpFileHdl, "%-30s", "Polling.Compliance");
					break;
				case 0x10A:
					fprintf (dumpFileHdl, "%-30s", "Polling.TxEIOS");
					break;
				case 0x10E:
					fprintf (dumpFileHdl, "%-30s", "Polling.EIdle");
					break;
				case 0x10F:
					fprintf (dumpFileHdl, "%-30s", "Polling.Speed");
					break;
				case 0x107:
					fprintf (dumpFileHdl, "%-30s", "Polling.Config");
					break;
				case 0x106:
					fprintf (dumpFileHdl, "%-30s", "Polling.Done");
					break;
				case 0x200:
					fprintf (dumpFileHdl, "%-30s", "CFG.Idle");
					break;
				case 0x201:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Start_Dn");
					break;
				case 0x210:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Start_Up");
					break;
				case 0x211:
					fprintf (dumpFileHdl, "%-30s", "CFG.XLinkArb_Won");
					break;
				case 0x203:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Accept_Dn");
					break;
				case 0x218:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Accept_Up");
					break;
				case 0x202:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Wait_Dn");
					break;
				case 0x208:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Wait_Up");
					break;
				case 0x206:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Accept_Dn");
					break;
				case 0x20C:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Accept_Up");
					break;
				case 0x207:
					fprintf (dumpFileHdl, "%-30s", "CFG.Complete_Dn");
					break;
				case 0x21C:
					fprintf (dumpFileHdl, "%-30s", "CFG.Complete_Up");
					break;
				case 0x214:
					fprintf (dumpFileHdl, "%-30s", "CFG.TxSDSM");
					break;
				case 0x204:
					fprintf (dumpFileHdl, "%-30s", "CFG.TxIdle");
					break;
				case 0x20E:
					fprintf (dumpFileHdl, "%-30s", "CFG.Done_St");
					break;
				case 0x20F:
					fprintf (dumpFileHdl, "%-30s", "CFG.TxCtlSkip");
					break;
				case 0x400:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Idle");
					break;
				case 0x401:
					fprintf (dumpFileHdl, "%-30s", "Recovery.RcvrLock");
					break;
				case 0x403:
					fprintf (dumpFileHdl, "%-30s", "Recovery.RcvrCfg");
					break;
				case 0x402:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxEIOS");
					break;
				case 0x407:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Speed");
					break;
				case 0x40B:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxIdle");
					break;
				case 0x40A:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Disable");
					break;
				case 0x40F:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Loopback");
					break;
				case 0x409:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Reset");
					break;
				case 0x408:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxEIEOS");
					break;
				case 0x40D:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxSDSM");
					break;
				case 0x40C:
					fprintf (dumpFileHdl, "%-30s", "Recovery.ErrCfg");
					break;
				case 0x40E:
					fprintf (dumpFileHdl, "%-30s", "Recovery.ErrDet");
					break;
				case 0x411:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh0_Up");
					break;
				case 0x419:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh1_Up");
					break;
				case 0x418:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh2_Up");
					break;
				case 0x410:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh3_Up");
					break;
				case 0x405:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh1_Dn");
					break;
				case 0x415:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh2_Dn");
					break;
				case 0x417:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh3_Dn");
					break;
				case 0x41F:
					fprintf (dumpFileHdl, "%-30s", "Recovery.MyEqPhase");
					break;
				case 0x41E:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxCtlSKP");
					break;
				case 0x300:
					fprintf (dumpFileHdl, "%-30s", "L0_Idle");
					break;
				case 0x301:
					fprintf (dumpFileHdl, "%-30s", "L0");
					break;
				case 0x303:
					fprintf (dumpFileHdl, "%-30s", "L0_TxEIOS");
					break;
				case 0x302:
					fprintf (dumpFileHdl, "%-30s", "L0s");
					break;
				case 0x307:
					fprintf (dumpFileHdl, "%-30s", "L1");
					break;
				case 0x30B:
					fprintf (dumpFileHdl, "%-30s", "L2");
					break;
				case 0x30D:
					fprintf (dumpFileHdl, "%-30s", "ActRec");		//Internal state from L0 to Recovery
					break;
				case 0x309:
					fprintf (dumpFileHdl, "%-30s", "L0_Recovery");
					break;
				case 0x305:
					fprintf (dumpFileHdl, "%-30s", "LDWait");
					break;
				case 0x304:
					fprintf (dumpFileHdl, "%-30s", "LinkDown");
					break;
				case 0x600:
					fprintf (dumpFileHdl, "%-30s", "HR_Idle");
					break;
				case 0x605:
					fprintf (dumpFileHdl, "%-30s", "HR_TxEIEOS");
					break;
				case 0x601:
					fprintf (dumpFileHdl, "%-30s", "HR_Dir");
					break;
				case 0x603:
					fprintf (dumpFileHdl, "%-30s", "HR_Tx");
					break;
				case 0x602:
					fprintf (dumpFileHdl, "%-30s", "HR_Rcv");
					break;
				case 0x606:
					fprintf (dumpFileHdl, "%-30s", "HR_RcvWait");
					break;
				case 0x607:
					fprintf (dumpFileHdl, "%-30s", "HR_TxEIOS");
					break;
				case 0x604:
					fprintf (dumpFileHdl, "%-30s", "HR_TxEIOS_1");
					break;
				case 0x500:
					fprintf (dumpFileHdl, "%-30s", "LB_Idle");
					break;
				case 0x501:
					fprintf (dumpFileHdl, "%-30s", "LB_Entry");
					break;
				case 0x504:
					fprintf (dumpFileHdl, "%-30s", "LB_SlvEntry");
					break;
				case 0x506:
					fprintf (dumpFileHdl, "%-30s", "LB_TxEIOS");
					break;
				case 0x50C:
					fprintf (dumpFileHdl, "%-30s", "LB_Speed");
					break;
				case 0x509:
					fprintf (dumpFileHdl, "%-30s", "LB_ActMst");
					break;
				case 0x50A:
					fprintf (dumpFileHdl, "%-30s", "LB_ActSlv");
					break;
				case 0x50B:
					fprintf (dumpFileHdl, "%-30s", "LB_ExitSt");
					break;
				case 0x50F:
					fprintf (dumpFileHdl, "%-30s", "LB_Eidle");
					break;
				case 0x801:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sEntry");
					break;
				case 0x802:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sIdle");
					break;
				case 0x80A:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxEIE");
					break;
				case 0x803:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxFTS");
					break;
				case 0x804:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxSKP");
					break;
				case 0x80C:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxSDSM");
					break;
				case 0x905:
					fprintf (dumpFileHdl, "%-30s", "LP_L1RxEIOS");
					break;
				case 0x906:
					fprintf (dumpFileHdl, "%-30s", "LP_L1Idle");
					break;
				case 0xA08:
					fprintf (dumpFileHdl, "%-30s", "LP_L2RxEIOS");
					break;
				case 0xA0F:
					fprintf (dumpFileHdl, "%-30s", "LP_L2Idle");
					break;
				case 0xA09:
					fprintf (dumpFileHdl, "%-30s", "LP_L2Rate");
					break;
				case 0xA0D:
					fprintf (dumpFileHdl, "%-30s", "LP_L2RateOk");
					break;
				case 0x800:
					fprintf (dumpFileHdl, "%-30s", "LP_Idle");
					break;
				case 0x900:
					fprintf (dumpFileHdl, "%-30s", "LP_Idle");
					break;
				case 0xA00:
					fprintf (dumpFileHdl, "%-30s", "LP_Idle");
					break;
				case 0x700:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_Idle");
					break;
				case 0x709:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_TxEIEOS");
					break;
				case 0x701:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_TxTS1");
					break;
				case 0x703:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_TxEIOS");
					break;
				case 0x707:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_WaitEIOS");
					break;
				case 0x70F:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_Rate");
					break;
				case 0x70E:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_RateOk");
					break;
				case 0x706:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_P1_Req");
					break;
				case 0x704:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_Done");
					break;
				default:
					fprintf (dumpFileHdl, "%-30s", "Unknown State");
					break;
			}
			fprintf (dumpFileHdl, "\t");
		}

		if ((moduleB == 0) && ((signalB <= 4 && portB == 6) ||(signalB <= 4 && portB == 10) || (signalB >=8 && signalB <= 13 && portB == 10)))
		{
			switch((busB[i]&0xFFF))
			{
				case 0x0:
					fprintf (dumpFileHdl, "%-30s", "Detect.Quiet");
					break;
				case 0x1:
					fprintf (dumpFileHdl, "%-30s", "Detect.Active");
					break;
				case 0x2:
					fprintf (dumpFileHdl, "%-30s", "Detect.Rate");
					break;
				case 0x3:
					fprintf (dumpFileHdl, "%-30s", "Detect.Wait12ms");
					break;
				case 0x4:
					fprintf (dumpFileHdl, "%-30s", "Detect.P1_Req");
					break;
				case 0x5:
					fprintf (dumpFileHdl, "%-30s", "Detect.Donest");
					break;
				case 0x6:
					fprintf (dumpFileHdl, "%-30s", "Detect.P0_Req");
					break;
				case 0x7:
					fprintf (dumpFileHdl, "%-30s", "Detect.RateOk");
					break;
				case 0x9:
					fprintf (dumpFileHdl, "%-30s", "Detect.SpcOp");
					break;
				case 0xA:
					fprintf (dumpFileHdl, "%-30s", "Detect.DET_WAIT_P1_ST");
					break;
				case 0xB:
					fprintf (dumpFileHdl, "%-30s", "Detect.DET_WAIT_RATE_ST");
					break;
				case 0x103:
					fprintf (dumpFileHdl, "%-30s", "Polling.Active");
					break;
				case 0x101:
					fprintf (dumpFileHdl, "%-30s", "Polling.P0_Req");
					break;
				case 0x10B:
					fprintf (dumpFileHdl, "%-30s", "Polling.Compliance");
					break;
				case 0x10A:
					fprintf (dumpFileHdl, "%-30s", "Polling.TxEIOS");
					break;
				case 0x10E:
					fprintf (dumpFileHdl, "%-30s", "Polling.EIdle");
					break;
				case 0x10F:
					fprintf (dumpFileHdl, "%-30s", "Polling.Speed");
					break;
				case 0x107:
					fprintf (dumpFileHdl, "%-30s", "Polling.Config");
					break;
				case 0x106:
					fprintf (dumpFileHdl, "%-30s", "Polling.Done");
					break;
				case 0x200:
					fprintf (dumpFileHdl, "%-30s", "CFG.Idle");
					break;
				case 0x201:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Start_Dn");
					break;
				case 0x210:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Start_Up");
					break;
				case 0x211:
					fprintf (dumpFileHdl, "%-30s", "CFG.XLinkArb_Won");
					break;
				case 0x203:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Accept_Dn");
					break;
				case 0x218:
					fprintf (dumpFileHdl, "%-30s", "CFG.LW_Accept_Up");
					break;
				case 0x202:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Wait_Dn");
					break;
				case 0x208:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Wait_Up");
					break;
				case 0x206:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Accept_Dn");
					break;
				case 0x20C:
					fprintf (dumpFileHdl, "%-30s", "CFG.LN_Accept_Up");
					break;
				case 0x207:
					fprintf (dumpFileHdl, "%-30s", "CFG.Complete_Dn");
					break;
				case 0x21C:
					fprintf (dumpFileHdl, "%-30s", "CFG.Complete_Up");
					break;
				case 0x214:
					fprintf (dumpFileHdl, "%-30s", "CFG.TxSDSM");
					break;
				case 0x204:
					fprintf (dumpFileHdl, "%-30s", "CFG.TxIdle");
					break;
				case 0x20E:
					fprintf (dumpFileHdl, "%-30s", "CFG.Done_St");
					break;
				case 0x20F:
					fprintf (dumpFileHdl, "%-30s", "CFG.TxCtlSkip");
					break;
				case 0x400:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Idle");
					break;
				case 0x401:
					fprintf (dumpFileHdl, "%-30s", "Recovery.RcvrLock");
					break;
				case 0x403:
					fprintf (dumpFileHdl, "%-30s", "Recovery.RcvrCfg");
					break;
				case 0x402:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxEIOS");
					break;
				case 0x407:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Speed");
					break;
				case 0x40B:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxIdle");
					break;
				case 0x40A:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Disable");
					break;
				case 0x40F:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Loopback");
					break;
				case 0x409:
					fprintf (dumpFileHdl, "%-30s", "Recovery.Reset");
					break;
				case 0x408:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxEIEOS");
					break;
				case 0x40D:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxSDSM");
					break;
				case 0x40C:
					fprintf (dumpFileHdl, "%-30s", "Recovery.ErrCfg");
					break;
				case 0x40E:
					fprintf (dumpFileHdl, "%-30s", "Recovery.ErrDet");
					break;
				case 0x411:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh0_Up");
					break;
				case 0x419:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh1_Up");
					break;
				case 0x418:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh2_Up");
					break;
				case 0x410:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh3_Up");
					break;
				case 0x405:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh1_Dn");
					break;
				case 0x415:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh2_Dn");
					break;
				case 0x417:
					fprintf (dumpFileHdl, "%-30s", "Recovery.EqPh3_Dn");
					break;
				case 0x41F:
					fprintf (dumpFileHdl, "%-30s", "Recovery.MyEqPhase");
					break;
				case 0x41E:
					fprintf (dumpFileHdl, "%-30s", "Recovery.TxCtlSKP");
					break;
				case 0x300:
					fprintf (dumpFileHdl, "%-30s", "L0_Idle");
					break;
				case 0x301:
					fprintf (dumpFileHdl, "%-30s", "L0");
					break;
				case 0x303:
					fprintf (dumpFileHdl, "%-30s", "L0_TxEIOS");
					break;
				case 0x302:
					fprintf (dumpFileHdl, "%-30s", "L0s");
					break;
				case 0x307:
					fprintf (dumpFileHdl, "%-30s", "L1");
					break;
				case 0x30B:
					fprintf (dumpFileHdl, "%-30s", "L2");
					break;
				case 0x30D:
					fprintf (dumpFileHdl, "%-30s", "ActRec");		//Internal state from L0 to Recovery
					break;
				case 0x309:
					fprintf (dumpFileHdl, "%-30s", "L0_Recovery");
					break;
				case 0x305:
					fprintf (dumpFileHdl, "%-30s", "LDWait");
					break;
				case 0x304:
					fprintf (dumpFileHdl, "%-30s", "LinkDown");
					break;
				case 0x600:
					fprintf (dumpFileHdl, "%-30s", "HR_Idle");
					break;
				case 0x605:
					fprintf (dumpFileHdl, "%-30s", "HR_TxEIEOS");
					break;
				case 0x601:
					fprintf (dumpFileHdl, "%-30s", "HR_Dir");
					break;
				case 0x603:
					fprintf (dumpFileHdl, "%-30s", "HR_Tx");
					break;
				case 0x602:
					fprintf (dumpFileHdl, "%-30s", "HR_Rcv");
					break;
				case 0x606:
					fprintf (dumpFileHdl, "%-30s", "HR_RcvWait");
					break;
				case 0x607:
					fprintf (dumpFileHdl, "%-30s", "HR_TxEIOS");
					break;
				case 0x604:
					fprintf (dumpFileHdl, "%-30s", "HR_TxEIOS_1");
					break;
				case 0x500:
					fprintf (dumpFileHdl, "%-30s", "LB_Idle");
					break;
				case 0x501:
					fprintf (dumpFileHdl, "%-30s", "LB_Entry");
					break;
				case 0x504:
					fprintf (dumpFileHdl, "%-30s", "LB_SlvEntry");
					break;
				case 0x506:
					fprintf (dumpFileHdl, "%-30s", "LB_TxEIOS");
					break;
				case 0x50C:
					fprintf (dumpFileHdl, "%-30s", "LB_Speed");
					break;
				case 0x509:
					fprintf (dumpFileHdl, "%-30s", "LB_ActMst");
					break;
				case 0x50A:
					fprintf (dumpFileHdl, "%-30s", "LB_ActSlv");
					break;
				case 0x50B:
					fprintf (dumpFileHdl, "%-30s", "LB_ExitSt");
					break;
				case 0x50F:
					fprintf (dumpFileHdl, "%-30s", "LB_Eidle");
					break;
				case 0x801:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sEntry");
					break;
				case 0x802:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sIdle");
					break;
				case 0x80A:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxEIE");
					break;
				case 0x803:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxFTS");
					break;
				case 0x804:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxSKP");
					break;
				case 0x80C:
					fprintf (dumpFileHdl, "%-30s", "LP_L0sTxSDSM");
					break;
				case 0x905:
					fprintf (dumpFileHdl, "%-30s", "LP_L1RxEIOS");
					break;
				case 0x906:
					fprintf (dumpFileHdl, "%-30s", "LP_L1Idle");
					break;
				case 0xA08:
					fprintf (dumpFileHdl, "%-30s", "LP_L2RxEIOS");
					break;
				case 0xA0F:
					fprintf (dumpFileHdl, "%-30s", "LP_L2Idle");
					break;
				case 0xA09:
					fprintf (dumpFileHdl, "%-30s", "LP_L2Rate");
					break;
				case 0xA0D:
					fprintf (dumpFileHdl, "%-30s", "LP_L2RateOk");
					break;
				case 0x800:
					fprintf (dumpFileHdl, "%-30s", "LP_Idle");
					break;
				case 0x900:
					fprintf (dumpFileHdl, "%-30s", "LP_Idle");
					break;
				case 0xA00:
					fprintf (dumpFileHdl, "%-30s", "LP_Idle");
					break;
				case 0x700:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_Idle");
					break;
				case 0x709:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_TxEIEOS");
					break;
				case 0x701:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_TxTS1");
					break;
				case 0x703:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_TxEIOS");
					break;
				case 0x707:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_WaitEIOS");
					break;
				case 0x70F:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_Rate");
					break;
				case 0x70E:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_RateOk");
					break;
				case 0x706:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_P1_Req");
					break;
				case 0x704:
					fprintf (dumpFileHdl, "%-30s", "DISABLE_Done");
					break;
				default:
					fprintf (dumpFileHdl, "%-30s", "Unknown State");
					break;
			}
			fprintf (dumpFileHdl, "\t");
		}

		//fprintf (dumpFileHdl, "0x%08x\t0x%08x\t%10u\t", busA[i], busB[i], time[i]);  /***********printing trace data  into file *******/
		fprintf (dumpFileHdl, "0x%08x\t0x%08x\t%10u\t", busA[i], busB[i], time[i]);
		

		if ((moduleA == 11) || (moduleA == 10))		//TIC and TEC
		{
			if ((time[i] == 1) && (bHdr2 == FALSE))
			{
				switch(busA[i] & 0xFF)
				{
					case 0x0:
						fprintf (dumpFileHdl, "\tMemRd32 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x | ", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0;
						break;
					case 0x20:
						fprintf (dumpFileHdl, "\tMemRd64 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x20;
						break;
					case 0x01:
						fprintf (dumpFileHdl, "\tMemRdLk32 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 1;
						break;
					case 0x21:
						fprintf (dumpFileHdl, "\tMemRdLk64 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x21;
						break;
					case 0x40:
						fprintf (dumpFileHdl, "\tMemWr32 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));;
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x40;
						break;
					case 0x60:
						fprintf (dumpFileHdl, "\tMemWr64 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x60;
						break;
					case 0x2:
						fprintf (dumpFileHdl, "\tIoRd | ");
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x2;
						break;
					case 0x42:
						fprintf (dumpFileHdl, "\tIoWr | ");
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x42;
						break;
					case 0x4:
						fprintf (dumpFileHdl, "\tCfgRd0 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x4;
						break;
					case 0x44:
						fprintf (dumpFileHdl, "\tCfgWr0 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x44;
						break;
					case 0x5:
						fprintf (dumpFileHdl, "\tCfgRd1 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x,:x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x5;
						break;
					case 0x25:
						fprintf (dumpFileHdl, "\tCfgWr1 | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)&0xFF);
						fprintf (dumpFileHdl, "LastBE=0x%x, 1stBE=0x%x", (busB[i]>>28)&0xF, (busB[i]>>24)&0xF);
						bHdr2 = TRUE;
						cmd = 0x25;
						break;
					case 0x30:
					case 0x31:
					case 0x32:
					case 0x33:
					case 0x34:
					case 0x35:
					case 0x36:
					case 0x37:
						fprintf (dumpFileHdl, "\tMsg | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16)& 0xFF);
						messageCode = (busB[i]>>24)& 0xFF;
						switch (messageCode){
							case 0x10:
								fprintf (dumpFileHdl, "LTR");
								break;
							case 0x12:
								fprintf (dumpFileHdl, "OBFF");
								break;
							case 0x30:
								fprintf (dumpFileHdl, "ERR_COR");
								break;
							case 0x31:
								fprintf (dumpFileHdl, "ERR_NONFATAL");
								break;
							case 0x33:
								fprintf (dumpFileHdl, "ERR_FATAL");
								break;
							case 0x14:
								fprintf (dumpFileHdl, "PM_Active_State_Nak");
								break;
							case 0x18:
								fprintf (dumpFileHdl, "PM_PME");
								break;
							case 0x19:
								fprintf (dumpFileHdl, "PME_Turn_Off");
								break;
							case 0x1B:
								fprintf (dumpFileHdl, "PME_TO_Ack");
								break;
							case 0x20:
								fprintf (dumpFileHdl, "Assert_INTA");
								break;
							case 0x21:
								fprintf (dumpFileHdl, "Assert_INTB");
								break;
							case 0x22:
								fprintf (dumpFileHdl, "Assert_INTC");
								break;
							case 0x23:
								fprintf (dumpFileHdl, "Assert_INTD");
								break;
							case 0x24:
								fprintf (dumpFileHdl, "Deassert_INTA");
								break;
							case 0x25:
								fprintf (dumpFileHdl, "Deassert_INTB");
								break;
							case 0x26:
								fprintf (dumpFileHdl, "Deassert_INTC");
								break;
							case 0x27:
								fprintf (dumpFileHdl, "Deassert_INTD");
								break;
							case 0x50:
								fprintf (dumpFileHdl, "Set_Slot_Power_Limit");
								break;
							case 0x7E:
								fprintf (dumpFileHdl, "Vendor_Defined Type 0");
								break;
							case 0x7F:
								fprintf (dumpFileHdl, "Vendor_Defined Type 1");
								break;
							default:
								fprintf (dumpFileHdl, "Unknown Msg Code");
								break;
						}
						bHdr2 = TRUE;
						cmd = 0x30;
						break;
					case 0x70:
					case 0x71:
					case 0x72:
					case 0x73:
					case 0x74:
					case 0x75:
					case 0x76:
					case 0x77:
						fprintf (dumpFileHdl, "\tMsgD | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "ReqID(BDF)=0x%x:0x%x.0x%x | ", busB[i] & 0xFF, (busB[i]>>11) & 0x1F, (busB[i]>>8) & 0x7);
						fprintf (dumpFileHdl, "Tag=%d | ", (busB[i]>>16) & 0xFF);
						fprintf (dumpFileHdl, "Msg Code=0x%x", ((busB[i]>>24) & 0xFF));
						bHdr2 = TRUE;
						cmd = 0x70;
						break;
					case 0x0A:
						fprintf (dumpFileHdl, "\tCpl | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "CplID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Status=");
						switch((busB[i]>>21)&0x7)
						{
							case 0:
								fprintf (dumpFileHdl, "SC | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "UR | ");
								break;
							case 2:
								fprintf (dumpFileHdl, "CRS | ");
								break;
							case 3:
								fprintf (dumpFileHdl, "CA | ");
								break;
							default:
								fprintf (dumpFileHdl, "Unkown | ");
								break;
						}
						fprintf (dumpFileHdl, "ByteCnt=0x%x", ((busB[i]>>8)&0xF00)|((busB[i]>>24)&0xFF));
						bHdr2 = TRUE;
						cmd = 0xA;
						break;
					case 0x4A:
						fprintf (dumpFileHdl, "\tCplD | Len(DW)=%d | ", ((busA[i]>>8)&0x300)|((busA[i]>>24)&0xFF));
						fprintf (dumpFileHdl, "TC=%d | ", (busA[i]>>12)&0x7);
						switch((busA[i]>>23)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "TD- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "TD+ | ");
								break;
						}
						switch((busA[i]>>22)&0x1)
						{
							case 0:
								fprintf (dumpFileHdl, "EP- | ");
								break;
							case 1:
								fprintf (dumpFileHdl, "EP+ | ");
								break;
						}
						fprintf (dumpFileHdl, "CplID(BDF)=0x%x:0x%x.0x%x | ", busB[i]&0xFF, (busB[i]>>11)&0x1F, (busB[i]>>8)&0x7);
						fprintf (dumpFileHdl, "Status=%d | ", (busB[i]>>20)&0x7);
						fprintf (dumpFileHdl, "ByteCnt=0x%x", ((busB[i]>>8)&0xF00)|((busB[i]>>24)&0xFF));
						bHdr2 = TRUE;
						cmd = 0x4A;
						break;
					default:
						fprintf (dumpFileHdl, "\tUnknown Cmd");
						bHdr2 = TRUE;
						cmd = 0xFF;
						break;
				}
			}
			else if (bHdr2)
			{
				if ((cmd == 0x0) || (cmd == 0x1))		//32-bit memory read and read lock
				{
					fprintf (dumpFileHdl, "\tAddr=0x%08x", endian32swap (busA[i]));
					bHdr2 = FALSE;
				}
				else if ((cmd == 0x20) || (cmd == 0x21))	//64-bit memory read and read lock
				{
					fprintf (dumpFileHdl, "\tHiAddr=0x%08x | LoAddr=0x%08x", endian32swap (busA[i]), endian32swap (busB[i]));
					bHdr2 = FALSE;
				}
				else if (cmd == 0x40)	//32-bit memory write
				{
					fprintf (dumpFileHdl, "\tAddr=0x%08x | ", endian32swap(busA[i]));
					fprintf (dumpFileHdl, "Data=0x%08x", busB[i]);
					bHdr2 = FALSE;
				}
				else if (cmd == 0x60)	//64-bit memory write
				{
					fprintf (dumpFileHdl, "\tHiAddr=0x%08x | LoAddr=0x%08x", endian32swap(busA[i]), endian32swap(busB[i]));
					bHdr2 = FALSE;
				}
				else if (cmd == 0x2)	//IO read
				{
					fprintf (dumpFileHdl, "\tAddr=0x%08x", endian32swap(busA[i]));
					bHdr2 = FALSE;
				}
				else if (cmd == 0x42)	//IO write
				{
					fprintf (dumpFileHdl, "\tAddr=0x%08x | ", endian32swap(busA[i]));
					fprintf (dumpFileHdl, "Data=0x%08x", busB[i]);
					bHdr2 = FALSE;
				}
				else if (cmd == 0x4)	//CfgRd0
				{
					fprintf (dumpFileHdl, "\tTargetID(BDF)=0x%x,0x%x,0x%x | Reg=0x%x", busA[i]&0xFF, (busA[i]>>11)&0x1F, (busA[i]>>8)&0x7, (((busA[i]>>8)&0xF00)|((busA[i]>>24)&0xFF))&0xFFF);
					bHdr2 = FALSE;
				}
				else if (cmd == 0x24)	//CfgWr0
				{
					fprintf (dumpFileHdl, "\tTargetID(BDF)=0x%x,0x%x,0x%x | Reg=0x%x | ", busA[i]&0xFF, (busA[i]>>11)&0x1F, (busA[i]>>8)&0x7, (((busA[i]>>8)&0xF00)|((busA[i]>>24)&0xFF))&0xFFF);
					fprintf (dumpFileHdl, "Data=0x%08x", busB[i]);
					bHdr2 = FALSE;
				}
				else if (cmd == 0x5)	//CfgRd1
				{
					fprintf (dumpFileHdl, "\tTargetID(BDF)=0x%x,0x%x,0x%x | Reg=0x%x", busA[i]&0xFF, (busA[i]>>11)&0x1F, (busA[i]>>8)&0x7, (((busA[i]>>8)&0xF00)|((busA[i]>>24)&0xFF))&0xFFF);
					bHdr2 = FALSE;
				}
				else if (cmd == 0x45)	//CfgWr1
				{
					fprintf (dumpFileHdl, "\tTargetID(BDF)=0x%x,0x%x,0x%x | Reg=0x%x", busA[i]&0xFF, (busA[i]>>11)&0x1F, (busA[i]>>8)&0x7, (((busA[i]>>8)&0xF00)|((busA[i]>>24)&0xFF))&0xFFF);
					fprintf (dumpFileHdl, "Data=0x%08x", busB[i]);
					bHdr2 = FALSE;
				}
				else if (cmd == 0xA)		//Cpl
				{
					fprintf (dumpFileHdl, "\tReqID(BDF)=0x%x,0x%x,0x%x | Tag=0x%x | LowerAddr=0x%x", busA[i]&0xFF, (busA[i]>>11)&0x1F, (busA[i]>>8)&0x7, (busA[i]>>16)&0xFF, ((busA[i]>>24)&0xFF));
					bHdr2 = FALSE;
				}
				else if (cmd == 0x4A)	//CplD
				{
					fprintf (dumpFileHdl, "\tReqID(BDF)=0x%x,0x%x,0x%x | Tag=0x%x | LowerAddr=0x%x | ", busA[i]&0xFF, (busA[i]>>11)&0x1F, (busA[i]>>8)&0x7, (busA[i]>>16)&0xFF, ((busA[i]>>24)&0xFF));
					fprintf (dumpFileHdl, "Data=0x%08x", busB[i]);
					bHdr2 = FALSE;
				}
				else
				{
					bHdr2 = FALSE;
				}
			}
		}

		if (moduleA == 12)
		{
			if (signalA == 0)
			{
				fprintf (dumpFileHdl, "\tneP_PldLink[8:0]=0x%x | neP_HdrCredit[8:0]=0x%x | ", (busA[i]>>9)&0x1FF, busA[i]&0x1FF);
			}
			else if (signalA == 1)
			{
				fprintf (dumpFileHdl, "\tneP_PldLink[9]=%d | neNP_PldCredit[7:0]=0x%x | neNP_HdrCredit[8:0]=0x%x | ", (busA[i]>>17)&0x1, (busA[i]>>9)&0xFF, busA[i]&0x1FF);
			}
			else if (signalA == 2)
			{
				fprintf (dumpFileHdl, "\tneCpl_PldLink[8:0]=0x%x | neCpl_HdrCredit[8:0]=0x%x | ", (busA[i]>>9)&0x1FF, busA[i]&0x1FF);
			}
		}

		if (moduleB == 12)
		{
			if (signalB == 0)
			{
				fprintf (dumpFileHdl, "neP_ShortHdr=%d | neP_ShortHpHdr=%d | neP_ShortZerHdr=%d | neP_ShortPld=%d | neP_ShortHpPld=%d | neP_ShortZerPld=%d | neP_PldCredit[11:0]=0x%x",
					(busB[i]>>17)&0x1, (busB[i]>>16)&0x1, (busB[i]>>15)&0x1, (busB[i]>>14)&0x1, (busB[i]>>13)&0x1, (busB[i]>>12)&0x1, busB[i]&0xFFF);
			}
			else if (signalB == 1)
			{
				fprintf (dumpFileHdl, "neNP_ShortHdr=%d | neNP_ShortHpHdr=%d | neNP_ShortZerHdr=%d | neCpl_PldLink[9]=%d",
					(busB[i]>>17)&0x1, (busB[i]>>16)&0x1, (busB[i]>>15)&0x1, (busB[i]>>14)&0x1);
			}
			else if (signalB == 2)
			{
				fprintf (dumpFileHdl, "neCpl_ShortHdr=%d | neCpl_ShortHpHdr=%d | neCpl_ShortZerHdr=%d | neCpl_ShortPld=%d | neCpl_ShortHpPld=%d | neCpl_ShortZerPld=%d | neCpl_PldCredit[11:0]=0x%x",
					(busB[i]>>17)&0x1, (busB[i]>>16)&0x1, (busB[i]>>15)&0x1, (busB[i]>>14)&0x1, (busB[i]>>13)&0x1, (busB[i]>>12)&0x1, busB[i]&0xFFF);
			}
			else if (signalB == 3)
			{
				fprintf (dumpFileHdl, "neP_PldCons[11:0]=0x%x", busB[i]&0xFFF);
			}
			else if (signalB == 4)
			{
				fprintf (dumpFileHdl, "neNP_PldCons[11:0]=0x%x", busB[i]&0xFFF);
			}
			else if (signalB == 5)
			{
				fprintf (dumpFileHdl, "neCpl_PldCons[11:0]=0x%x", busB[i]&0xFFF);
			}
		}

		if (moduleA == 7)
		{
			if (signalA == 0)
			{
				fprintf (dumpFileHdl, "\tfeCpl_PldCredit[6:0]=0x%x | feCpl_HdrAvail=%d | ",
					(busA[i]>>1)&0x7F, busA[i]&0x1);
			}
			else if (signalA == 1)
			{
				fprintf (dumpFileHdl, "\tfeNP_PldCredit[6:0]=0x%x | feNP_HdrAvail=%d | feP_PldCredit[6:0]=0x%x | feP_HdrAvail=%d | ",
					(busA[i]>>10)&0x7F, (busA[i]>>9)&0x1, (busA[i]>>1)&0x7F, busA[i]&0x1);
			}
			else if (signalA == 3)
			{
				fprintf (dumpFileHdl, "\tfeP_PldLimit[11:0]=0x%x | ", busA[i] & 0xFFF);
			}
			else if (signalA == 4)
			{
				fprintf (dumpFileHdl, "\tfeNP_PldLimit[11:0]=0x%x | ", busA[i] & 0xFFF);
			}
			else if (signalA == 5)
			{
				fprintf (dumpFileHdl, "\tfeCpl_PldLimit[11:0]=0x%x | ", busA[i] & 0xFFF);
			}
		}

		if (moduleB == 7)
		{
			if (signalB == 0)
			{
				fprintf (dumpFileHdl, "feCpl_PldInfi=%d | feCpl_HdrInfi=%d | feCpl_HdrCons[7:0]=0x%x | feCpl_HdrLimit[7:0]=0x%x",
					(busB[i]>>17)&0x1, (busB[i]>>16)&0x1, (busB[i]>>8)&0xFF, busB[i]&0xFF);
			}
			if (signalB == 1)
			{
				fprintf (dumpFileHdl, "feNP_PldInfi=%d | feNP_HdrInfi=%d | feNP_HdrCons[7:0]=0x%x | feNP_HdrLimit[7:0]=0x%x",
					(busB[i]>>17)&0x1, (busB[i]>>16)&0x1, (busB[i]>>8)&0xFF, busB[i]&0xFF);
			}
			if (signalB == 2)
			{
				fprintf (dumpFileHdl, "feP_PldInfi=%d | feP_HdrInfi=%d |feP_HdrCons[7:0]=0x%x | feP_HdrLimit[7:0]=0x%x",
					(busB[i]>>17)&0x1, (busB[i]>>16)&0x1, (busB[i]>>8)&0xFF, busB[i]&0xFF);
			}
		}

		/******Trigger decode ****/
		satcprobeDataDecode((U8)moduleA, 0, (U8)portA, (U8)signalA, busA[i], &probeStr[0]);
		fprintf (dumpFileHdl, "%s" ,probeStr);

		satcprobeDataDecode((U8)moduleB, 1, (U8)portB, (U8)signalB, busB[i], &probeStr[0]);
		fprintf (dumpFileHdl, "%s", probeStr);

		if (trigSrc == 0)
		{
			if ((((busA[i] & notMaskA0) == (triggerA0 & notMaskA0)) && ((busB[i] & notMaskB0) == (triggerB0 & notMaskB0))))
			{
				fprintf (dumpFileHdl, "\t<====");
			}
		}
		else if (trigSrc == 1)
		{
			if ((((busA[i] & notMaskA1) == (triggerA1 & notMaskA1)) && ((busB[i] & notMaskB1) == (triggerB1 & notMaskB1))))
			{
				fprintf (dumpFileHdl, "\t<====");
			}
		}
		else if (trigSrc == 2)
		{
			if ((((busA[i] & notMaskA0) == (triggerA0 & notMaskA0)) && ((busB[i] & notMaskB0) == (triggerB0 & notMaskB0))) &
				(((busA[i] & notMaskA1) == (triggerA1 & notMaskA1)) && ((busB[i] & notMaskB1) == (triggerB1 & notMaskB1))))
			{
				fprintf (dumpFileHdl, "\t<====");
			}
		}
		else if (trigSrc == 3)
		{
			if ((((busA[i] & notMaskA0) == (triggerA0 & notMaskA0)) && ((busB[i] & notMaskB0) == (triggerB0 & notMaskB0))) |
				(((busA[i] & notMaskA1) == (triggerA1 & notMaskA1)) && ((busB[i] & notMaskB1) == (triggerB1 & notMaskB1))))
			{
				fprintf (dumpFileHdl, "\t<====");
			}
		}
		else
		{
			if ((((busA[i] & notMaskA0) == (triggerA0 & notMaskA0)) && ((busB[i] & notMaskB0) == (triggerB0 & notMaskB0))) |
				(((busA[i] & notMaskA1) == (triggerA1 & notMaskA1)) && ((busB[i] & notMaskB1) == (triggerB1 & notMaskB1))))
			{
				fprintf (dumpFileHdl, "\t<====");
			}
		}
		fprintf (dumpFileHdl, "\n");
	}
		}


			/*-------------------------------------------*/

				//fprintf (dumpFileHdl, "0x%08x\t0x%08x\t%10u\n", dataA[i], dataB[i], time[i]);
								
				//}
			//}					
	*PtrDumpStatus = 1;	
	gPtrLoggerSwitch->logiDebug ("satcGetAladinTracedump status: %d", *PtrDumpStatus);
	sosiFileClose(dumpFileHdl);
	//satcdumpreg(PtrDevice);
	return status;
}
SCRUTINY_STATUS satcPollAladinTrigger (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrTrigStatus)
{
	SCRUTINY_STATUS status = SCRUTINY_STATUS_FAILED;
	//U32 u32regval;
	//U32 regAddr;
	U32 triggered=0;
	U32 tmp;
	U32 stopstatus=0;
	
	satcAladinRegRead(PtrDevice,ALADIN_CAP_STS_REG, &tmp);
	
	if ((tmp & 0x1)& (tmp >> 8 & 0x1)){
		triggered = 0x3;
		satcStopAladinTrace(PtrDevice, &stopstatus);
	}
	if (tmp & 0x1)
		triggered = 0x2;
	satcAladinRegRead(PtrDevice,ALADIN_CAP_STS_REG, &tmp);
	if ((tmp >> 8)& 0x1)
		triggered = 0x1;
		
	*PtrTrigStatus = triggered;
	gPtrLoggerSwitch->logiDebug ("satcPollAladinTrigger trigger status: %d", triggered);
	return status;
}
SCRUTINY_STATUS satcStopAladinTrace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrStopStatus)
{
	SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;

	satcAladinRegWrite (PtrDevice, ALADIN_CAP_CTL_REG, (1 | (1<<9)| gSwitchAladinConfigData.Compression));

	*PtrStopStatus = TRUE;
	
	gPtrLoggerSwitch->logiDebug ("satcStopAladinTrace stop status: %d", *PtrStopStatus);
	return status;
}

SCRUTINY_STATUS satcStopAladinReadlivedata (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrStopStatus)
{
	SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
	return status;
}


SCRUTINY_STATUS satcPrepTmuxsettings()
{

	gPtrLoggerSwitch->logiDebug ("switch aladin ModuleA=0x%8x)", gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[MODULEA].ItemIndex]);
	gPtrLoggerSwitch->logiDebug ("switch aladin ModuleB=0x%8x)", gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[MODULEB].ItemIndex]);
	gSwitchAladinConfigData.moduleA=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[MODULEA].ItemIndex];
	gSwitchAladinConfigData.moduleB=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[MODULEB].ItemIndex];

	switch (gSwitchAladinConfigData.moduleA)
	{
		case 0:	//PHY
			gSwitchAladinConfigData.tmuxTecSel = 0x0;
			gSwitchAladinConfigData.tmuxTicSel = 0x0;
			gSwitchAladinConfigData.tmuxPhydllSel = 0x1111;
			break;
		case 1:	//DLL
			gSwitchAladinConfigData.tmuxTecSel = 0x0;
			gSwitchAladinConfigData.tmuxTicSel = 0x0;
			gSwitchAladinConfigData.tmuxPhydllSel = 0x2222;
			break;
		case 2:	//SOPS
			gSwitchAladinConfigData.tmuxTecSel = 0x0;
			gSwitchAladinConfigData.tmuxTicSel = 0x0;
			gSwitchAladinConfigData.tmuxPhydllSel = 0x3333;
			break;
		case 3:	//ACCM
			gSwitchAladinConfigData.tmuxTecSel = 0x0;
			gSwitchAladinConfigData.tmuxTicSel = 0x1111;
			break;
		case 4:	//TIC
			gSwitchAladinConfigData.tmuxTecSel = 0x0;
			gSwitchAladinConfigData.tmuxTicSel = 0x2222;
			break;
		case 5: //SQM
			gSwitchAladinConfigData.tmuxTecSel = 0x1111;
			break;
		case 6: //RDR
			gSwitchAladinConfigData.tmuxTecSel = 0x2222;
			break;
		case 7: //SCH
			gSwitchAladinConfigData.tmuxTecSel = 0x3333;
			break;
		case 8: //RESET
			gSwitchAladinConfigData.tmuxTecSel = 0x4444;
			break;
		case 9: //DMA
			gSwitchAladinConfigData.tmuxTecSel = 0x5555;
			break;
		case 10: //TIC HDR
			gSwitchAladinConfigData.tmuxTecSel = 0x0;
			gSwitchAladinConfigData.tmuxTicSel = 0x4444;
			break;
		case 11: //TEC HDR
			gSwitchAladinConfigData.tmuxTecSel = 0x6666;
			break;
		case 12: //INCH
			gSwitchAladinConfigData.tmuxTecSel = 0x0;
			gSwitchAladinConfigData.tmuxTicSel = 0x3333;
			break;
		default:
			break;
	}

	switch (gSwitchAladinConfigData.moduleB)
	{
		case 0:	//PHY
			gSwitchAladinConfigData.tmuxTecSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxTicSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxPhydllSel = (gSwitchAladinConfigData.tmuxPhydllSel & 0xFFFF) | (0x1111 << 16);
			break;
		case 1:	//DLL
			gSwitchAladinConfigData.tmuxTecSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxTicSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxPhydllSel = (gSwitchAladinConfigData.tmuxPhydllSel & 0xFFFF) | (0x2222 << 16);
			break;
		case 2:	//SOPS
			gSwitchAladinConfigData.tmuxTecSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxTicSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxPhydllSel = (gSwitchAladinConfigData.tmuxPhydllSel & 0xFFFF) | (0x3333 << 16);
			break;
		case 3:	//ACCM
			gSwitchAladinConfigData.tmuxTecSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxTicSel = (gSwitchAladinConfigData.tmuxTicSel & 0xFFFF) | (0x1111 << 16);
			break;
		case 4:	//TIC
			gSwitchAladinConfigData.tmuxTecSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxTicSel = (gSwitchAladinConfigData.tmuxTicSel & 0xFFFF) | (0x2222 << 16);
			break;
		case 5: //SQM
			gSwitchAladinConfigData.tmuxTecSel = (gSwitchAladinConfigData.tmuxTecSel & 0xFFFF) | (0x1111 << 16);
			break;
		case 6: //RDR
			gSwitchAladinConfigData.tmuxTecSel = (gSwitchAladinConfigData.tmuxTecSel & 0xFFFF) | (0x2222 << 16);
			break;
		case 7: //SCH
			gSwitchAladinConfigData.tmuxTecSel = (gSwitchAladinConfigData.tmuxTecSel & 0xFFFF) | (0x3333 << 16);
			break;
		case 8: //RESET
			gSwitchAladinConfigData.tmuxTecSel = (gSwitchAladinConfigData.tmuxTecSel & 0xFFFF) | (0x4444 << 16);
			break;
		case 9: //DMA
			gSwitchAladinConfigData.tmuxTecSel = (gSwitchAladinConfigData.tmuxTecSel & 0xFFFF) | (0x5555 << 16);
			break;
		case 10: //TIC HDR
			gSwitchAladinConfigData.tmuxTecSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxTicSel = (gSwitchAladinConfigData.tmuxTicSel & 0xFFFF) | (0x4444 << 16);
			break;
		case 11: //TEC HDR
			gSwitchAladinConfigData.tmuxTecSel = (gSwitchAladinConfigData.tmuxTecSel & 0xFFFF) | (0x6666 << 16);
			break;
		case 12: //INCH
			gSwitchAladinConfigData.tmuxTecSel &= 0xFFFF;
			gSwitchAladinConfigData.tmuxTicSel = (gSwitchAladinConfigData.tmuxTicSel & 0xFFFF) | (0x3333 << 16);
			break;
		default:
			break;
	}

	gPtrLoggerSwitch->logiDebug ("switch aladin TMUX_TIC_SEL=0x%8x)", gSwitchAladinConfigData.tmuxTicSel);
	gPtrLoggerSwitch->logiDebug ("switch aladin TMUX_PHYDLL_SEL=0x%8x)", gSwitchAladinConfigData.tmuxPhydllSel);
	gPtrLoggerSwitch->logiDebug ("switch aladin TMUX_TEC_SEL=0x%8x)", gSwitchAladinConfigData.tmuxTecSel);

	//gSwitchAladinConfigData.moduleA=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[MODULEA].ItemIndex];
	//gSwitchAladinConfigData.moduleA=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[MODULEB].ItemIndex];
	gSwitchAladinConfigData.station=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[STATION].ItemIndex];
	gSwitchAladinConfigData.portA=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[PORTA].ItemIndex];
	gSwitchAladinConfigData.portB=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[PORTB].ItemIndex];
	gSwitchAladinConfigData.signalA=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[SIGNALA].ItemIndex];
	gSwitchAladinConfigData.signalB=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[SIGNALB].ItemIndex];
	gSwitchAladinConfigData.trigMaskA0=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGMASKA0].ItemIndex];
	gSwitchAladinConfigData.trigMaskB0=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGMASKB0].ItemIndex];
	gSwitchAladinConfigData.trigMaskA1=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGMASKA1].ItemIndex];
	gSwitchAladinConfigData.trigMaskB1=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGMASKB1].ItemIndex];
	gSwitchAladinConfigData.dataMaskA=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[DATAMASKA].ItemIndex];
	gSwitchAladinConfigData.dataMaskB=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[DATAMASKB].ItemIndex];
	gSwitchAladinConfigData.triggerA0=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGGERA0].ItemIndex];
	gSwitchAladinConfigData.triggerB0=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGGERB0].ItemIndex];
	gSwitchAladinConfigData.triggerA1=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGGERA1].ItemIndex];
	gSwitchAladinConfigData.triggerB1=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGGERB1].ItemIndex];
	gSwitchAladinConfigData.Cond0notEqTo=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[COND0NOTEQTO].ItemIndex];
	gSwitchAladinConfigData.Cond1notEqTo=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[COND1NOTEQTO].ItemIndex];
	gSwitchAladinConfigData.Compression=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[COMPRESSION].ItemIndex];
	gSwitchAladinConfigData.PostCaptureMode=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[POSTCAPTUREMODE].ItemIndex];
	gSwitchAladinConfigData.captureLoc=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[CAPTURELOC].ItemIndex];
	gSwitchAladinConfigData.readTime=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[READTIME].ItemIndex];
	gSwitchAladinConfigData.trigSrc=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[TRIGSRC].ItemIndex];
	gSwitchAladinConfigData.eventSrc0=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[EVENTSRC0].ItemIndex];
	gSwitchAladinConfigData.eventSrc1=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[EVENTSRC1].ItemIndex];
	gSwitchAladinConfigData.eventCnt0=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[EVENTCOUNT0].ItemIndex];
	gSwitchAladinConfigData.eventCnt1=gSwitchAladinConfigData.AladinCfgItems[gSwitchAladinCfgMonTable[EVENTCOUNT1].ItemIndex];

	return SCRUTINY_STATUS_SUCCESS;

}

/**
 *
 * @method  satcAladinRegRead()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   Offset         register offset to be retrieved
 * 
 * @param   PtrValue       pointer to the register value
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read the configration register value of specific port
 *
 *
 */
SCRUTINY_STATUS satcAladinRegRead (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    //__IN__  U32 Port, 
    __IN__  U32 Offset, 
    __OUT__ PU32 PtrValue
)
{

    U32 regAddr = ATLAS_REGISTER_BASE_ADDRESS_PSB_STATION_BASE + (gSwitchAladinConfigData.station<<16) + ALADIN_BASE + Offset;

    return (bsdiMemoryRead32 (PtrDevice, regAddr, PtrValue, sizeof (U32)));

}

/**
 *
 * @method  satcAladinRegWrite()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   Offset         register offset to be retrieved
 * 
 * @param   PtrValue       value to be written
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   write the configration register value of specific port
 *
 *
 */
SCRUTINY_STATUS satcAladinRegWrite ( 
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
   // __IN__  U32 station, 
    __IN__  U32 Offset, 
    __IN__  U32 Value
)
{

    U32 regAddr = regAddr = ATLAS_REGISTER_BASE_ADDRESS_PSB_STATION_BASE + (gSwitchAladinConfigData.station<<16) + ALADIN_BASE + Offset;

    //baseAddress += Offset;

    return (bsdiMemoryWrite32 (PtrDevice, regAddr, &Value));
}

SCRUTINY_STATUS satcdumpreg(__IN__  PTR_SCRUTINY_DEVICE PtrDevice)
{
	U32 tmp = 0;
	gPtrLoggerSwitch->logiDebug ("switch Aladin Register Dump...");
	
	satcAladinRegRead(PtrDevice, ALADIN_CAP_CTL_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_CAP_CTL_REG: @%x- %x", ALADIN_CAP_CTL_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_CAP_STS_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_CAP_STS_REG: @%x- %x", ALADIN_CAP_STS_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_DCR_DRAM_ACCESS_CTL_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_DCR_DRAM_ACCESS_CTL_REG: @%x- %x", ALADIN_DCR_DRAM_ACCESS_CTL_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_DCR_RAM_ADDR_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_DCR_RAM_ADDR_REG: @%x- %x", ALADIN_DCR_RAM_ADDR_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_POST_CAP_CFG_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_POST_CAP_CFG_REG: @%x- %x", ALADIN_POST_CAP_CFG_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_MANUAL_TRIG_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_MANUAL_TRIG_REG: @%x- %x", ALADIN_MANUAL_TRIG_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_CUR_CAP_RAM_WR_ADDR_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_CUR_CAP_RAM_WR_ADDR_REG: @%x- %x", ALADIN_CUR_CAP_RAM_WR_ADDR_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_CAP_WRAP_COUNT_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_CAP_WRAP_COUNT_REG: @%x- %x", ALADIN_DCR_RAM_ADDR_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_COMP_MASK_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_COMP_MASK_LOW_REG: @%x- %x", ALADIN_COMP_MASK_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_COMP_MASK_HIGH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_COMP_MASK_HIGH_REG: @%x- %x", ALADIN_COMP_MASK_HIGH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_LIVE_DATAIN_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_LIVE_DATAIN_LOW_REG: @%x- %x", ALADIN_LIVE_DATAIN_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_LIVE_DATAIN_HIGH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_LIVE_DATAIN_HIGH_REG: @%x- %x", ALADIN_LIVE_DATAIN_HIGH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_TRIG_CFG_REG	, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_TRIG_CFG_REG	: @%x- %x", ALADIN_TRIG_CFG_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_COND0_AUTOCLEAR_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_COND0_AUTOCLEAR_REG: @%x- %x", ALADIN_COND0_AUTOCLEAR_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_TRIG_DATA_PAT0_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_TRIG_DATA_PAT0_LOW_REG: @%x- %x", ALADIN_TRIG_DATA_PAT0_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_TRIG_DATA_PAT0_HIGH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_TRIG_DATA_PAT0_HIGH_REG: @%x- %x", ALADIN_TRIG_DATA_PAT0_HIGH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_TRIG_DATA_RPT_COUNT0_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_TRIG_DATA_RPT_COUNT0_REG: @%x- %x", ALADIN_TRIG_DATA_RPT_COUNT0_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_TRIG_DATA_PAT1_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_TRIG_DATA_PAT1_LOW_REG: @%x- %x", ALADIN_TRIG_DATA_PAT1_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_TRIG_DATA_PAT1_HIGH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_TRIG_DATA_PAT1_HIGH_REG: @%x- %x", ALADIN_TRIG_DATA_PAT1_HIGH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_TRIG_DATA_RPT_COUNT1_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_TRIG_DATA_RPT_COUNT1_REG: @%x- %x", ALADIN_TRIG_DATA_RPT_COUNT1_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_DATA_MASK0_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_DATA_MASK0_LOW_REG: @%x- %x", ALADIN_DATA_MASK0_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_DATA_MASK0_HIGH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_DATA_MASK0_HIGH_REG: @%x- %x", ALADIN_DATA_MASK0_HIGH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_DATA_MASK1_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_DATA_MASK1_LOW_REG: @%x- %x", ALADIN_DATA_MASK1_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_EVENT_COUNT0_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_EVENT_COUNT0_REG: @%x- %x", ALADIN_EVENT_COUNT0_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_EVENT_COUNT1_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_EVENT_COUNT1_REG: @%x- %x", ALADIN_EVENT_COUNT1_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_RAM_READ_DATA_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_RAM_READ_DATA_LOW_REG: @%x- %x", ALADIN_RAM_READ_DATA_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_RAM_READ_DATA_HIGH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_RAM_READ_DATA_HIGH_REG: @%x- %x", ALADIN_RAM_READ_DATA_HIGH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_RAM_READ_FMT_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_RAM_READ_FMT_REG: @%x- %x", ALADIN_RAM_READ_FMT_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_RAM_WRITE_DATA_LOW_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_RAM_WRITE_DATA_LOW_REG: @%x- %x", ALADIN_RAM_WRITE_DATA_LOW_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_RAM_WRITE_DATA_HIGH_REG	, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_RAM_WRITE_DATA_HIGH_REG	: @%x- %x", ALADIN_RAM_WRITE_DATA_HIGH_REG	,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_RAM_WRITE_FMT_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_RAM_WRITE_FMT_REG: @%x- %x", ALADIN_RAM_WRITE_FMT_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_EVENT0_COUNT_THRESH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_EVENT0_COUNT_THRESH_REG: @%x- %x", ALADIN_EVENT0_COUNT_THRESH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_EVENT1_COUNT_THRESH_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_EVENT1_COUNT_THRESH_REG: @%x- %x", ALADIN_EVENT1_COUNT_THRESH_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_RAM_POWER_MODES_REG	, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_RAM_POWER_MODES_REG	: @%x- %x", ALADIN_RAM_POWER_MODES_REG	,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_INTERRUPT_STATUS_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_INTERRUPT_STATUS_REG: @%x- %x", ALADIN_INTERRUPT_STATUS_REG,tmp);
	satcAladinRegRead(PtrDevice, ALADIN_CLK_CFG_REG, &tmp);
	gPtrLoggerSwitch->logiDebug ("ALADIN_CLK_CFG_REG: @%x- %x", ALADIN_CLK_CFG_REG,tmp);
	
	return SCRUTINY_STATUS_SUCCESS;
}



