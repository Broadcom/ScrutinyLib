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
#include "switchportcounters.h"



/**
 *
 * @method  spcGetPexChipId()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   PtrChipId      returned chip id
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read the PEX device chip ID
 *
 *
 */
SCRUTINY_STATUS spcGetPexChipId (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __OUT__ PU32 PtrChipId
)
{

    /* Offset of the Configuration Register */
    U32 offset = 0xB7C;
    U32 value = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("spcGetPexChipId (PtrDevice=%x, PtrChipId=%x)", PtrDevice != NULL, PtrChipId != NULL);

    if (atlasPCIeConfigurationSpaceRead (PtrDevice, 0, offset, &value))
    {
	gPtrLoggerSwitch->logiFunctionExit ("spcGetPexChipId (status=%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrChipId = (value >> 16) & 0xFFFF;

    gPtrLoggerSwitch->logiDebug ("ChipId=0x%x", *PtrChipId);
    gPtrLoggerSwitch->logiFunctionExit ("spcGetPexChipId (status=%x)", SCRUTINY_STATUS_SUCCESS);
	
    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  spcGetErrorCounters()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrErrorCounters    read error counters
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read error counters for the specific port
 *
 *
 */
SCRUTINY_STATUS spcGetErrorCounters (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__  U32 PortNum, 
    __OUT__ PTR_SCRUTINY_SWITCH_ERROR_COUNTERS PtrErrorCounters
)
{
    U32 dword;
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32	port0OfStation;

    gPtrLoggerSwitch->logiFunctionEntry ("spcGetErrorCounters (PtrDevice=%x, PortNum=0x%x, PtrErrorCounters=%x)", 
	    PtrDevice != NULL, PortNum, PtrErrorCounters != NULL);

    port0OfStation = (PortNum / ATLAS_PMG_MAX_STNPORT) * ATLAS_PMG_MAX_STNPORT;

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, 0xFAC, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit("spcGetErrorCounters (status=0x%x)", status);

        return (status);
    }
	
    PtrErrorCounters->BadTLPErrors = dword;

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, 0xFB0, &dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit("spcGetErrorCounters (status=0x%x)", status);

        return (status);
    }
    
    PtrErrorCounters->BadDLLPErrors = dword;

	// PMG_OFF_ERR_PORT_RECEIVER, station level register
    dword = ((U32)(PortNum % 16) << 24);
    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, port0OfStation, 0xBF4, dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit("spcGetErrorCounters (status=0x%x)", status);

        return (status);
    }
	
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, port0OfStation, 0xBF4, &dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit("spcGetErrorCounters (status=0x%x)", status);

        return (status);
    }
	
    PtrErrorCounters->PortReceiverErrors = (dword & 0xFF);


    // PMG_OFF_ERR_RECOVERY_DIAG, station level register
    dword = ((U32)(PortNum % 16) << 24);
    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, port0OfStation, 0xBC4, dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit("spcGetErrorCounters (status=0x%x)", status);

        return (status);
    }
	
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, port0OfStation, 0xBC4, &dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit("spcGetErrorCounters (status=0x%x)", status);

        return (status);
    }
	
    PtrErrorCounters->RecoveryDiagnosticsErrors = (dword & 0xFF);

    dword = ((U32)(PortNum % 16) << 12);
    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, port0OfStation, 0x254, dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
	{
		gPtrLoggerSwitch->logiFunctionExit ("spcGetErrorCounters (status=0x%x)", status);
		
	    return (status);
	}
	
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, port0OfStation, 0x254, &dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
	{
		gPtrLoggerSwitch->logiFunctionExit ("spcGetErrorCounters (status=0x%x)", status);
		
	    return (status);
	}
    PtrErrorCounters->LinkDownCount = ((dword & 0xF8) >> 3);
    PtrErrorCounters->LinkSpeed = ((dword & 0x300) >> 8);

    gPtrLoggerSwitch->logiFunctionExit ("spcGetErrorCounters (status=0x%x)", status);
		
    return (status);
}


/**
 *
 * @method  spcIsPortEnabled()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 *
 * @return  BOOLEAN             enabled or disable          
 *
 * @brief   check whether specific port is enabled or not
 *
 *
 */
BOOLEAN spcIsPortEnabled (
	__IN__ PTR_SCRUTINY_DEVICE  	PtrDevice,
	__IN__ U32						PortNum
	)
{
    U32 offset = 0;
    U32 regValue = 0;
    SCRUTINY_STATUS   status;

    gPtrLoggerSwitch->logiFunctionEntry ("spcIsPortEnabled (PtrDevice=%x, Port=0x%x)", PtrDevice != NULL, PortNum);
 
    // Verify port is enabled (ports 116 & 117 are special case)
    if (PortNum < ATLAS_PMG_MAX_PORT_NO_2X1)
    {
        // Check if port is disabled
        offset = ATLAS_REGISTER_PMG_REG_PORT_CLOCK_EN_0 + ((PortNum / 32) * sizeof(U32));
        status = bsdiMemoryRead32 (PtrDevice, ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE + offset, 
        	&regValue, sizeof (U32));
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiDebug ("read PSB PORT SPACE fail, status=0x%x", status);
            gPtrLoggerSwitch->logiFunctionExit ("spcIsPortEnabled (FALSE)");
            return (FALSE);
        }

        if ( (regValue & ((U32)1 << (PortNum % 32))) == 0 )
        {
            // Port is not enabled
            gPtrLoggerSwitch->logiDebug ("Port disabled");
            gPtrLoggerSwitch->logiFunctionExit ("spcIsPortEnabled (FALSE)");
            return (FALSE);
        }
    }
    else if ( (PortNum == ATLAS_PMG_PORT_NUM_X1_1) || (PortNum == ATLAS_PMG_PORT_NUM_X1_2) )
    {
        // Clock enable for ports 116 & 117 (Port 0 318h[1:0])
        offset = ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE + ATLAS_REGISTER_PMG_REG_PORT_CLOCK_EN_3;
        status = bsdiMemoryRead32 (PtrDevice, offset, &regValue, sizeof (U32));
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiDebug ("read PSB PORT SPACE fail, status=0x%x", status);
            gPtrLoggerSwitch->logiFunctionExit ("spcIsPortEnabled (FALSE)");
        	return (FALSE);
        }

        if ( (PortNum == ATLAS_PMG_PORT_NUM_X1_1) && ((regValue & (1 << 0)) == 0) )
        {
            gPtrLoggerSwitch->logiDebug ("Port disabled");
            gPtrLoggerSwitch->logiFunctionExit ("spcIsPortEnabled (FALSE)");
            // Port 116 not enabled
            return (FALSE);
        }
        else if ( (PortNum == ATLAS_PMG_PORT_NUM_X1_2) && ((regValue & (1 << 1)) == 0) )
        {
            gPtrLoggerSwitch->logiDebug ("Port disabled");
            gPtrLoggerSwitch->logiFunctionExit ("spcIsPortEnabled (FALSE)");
            // Port 117 not enabled
            return (FALSE);;
        }
    }
    else
    {
        gPtrLoggerSwitch->logiDebug ("Port disabled");
        gPtrLoggerSwitch->logiFunctionExit ("spcIsPortEnabled (FALSE)");
        // Port not enabled
        return (FALSE);
    }

    gPtrLoggerSwitch->logiFunctionExit ("spcIsPortEnabled (TRUE)");
    return (TRUE);
}

/**
 *
 * @method  spcGetPciPortMaxLinkWidth()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrMaxLinkWidth     returned max link width
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read max supported link width for the specific port
 *
 *
 */
SCRUTINY_STATUS spcGetPciPortMaxLinkWidth (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32					PtrMaxLinkWidth
	)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;
    gPtrLoggerSwitch->logiFunctionEntry ("spcGetPciPortMaxLinkWidth (PtrDevice=%x, PortNum=0x%x, PtrMaxLinkWidth=%x)", 
            PtrDevice != NULL, PortNum, PtrMaxLinkWidth != NULL);
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_CAP, &dword);

    *PtrMaxLinkWidth = ((dword & 0x3F0) >> 4);
    gPtrLoggerSwitch->logiFunctionExit ("spcGetPciPortMaxLinkWidth (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  spcGetPciPortNegLinkWidth()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrMaxLinkWidth     returned negotiated link width
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read negotiatedlink width for the specific port
 *
 *
 */
SCRUTINY_STATUS spcGetPciPortNegLinkWidth (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32					PtrNegLinkWidth
	)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("spcGetPciPortNegLinkWidth (PtrDevice=%x, PortNum=0x%x, PtrNegLinkWidth=%x)", 
            PtrDevice != NULL, PortNum, PtrNegLinkWidth != NULL);
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, &dword);

    *PtrNegLinkWidth = ((dword & 0x3F00000) >> 20);

    gPtrLoggerSwitch->logiFunctionExit ("spcGetPciPortNegLinkWidth (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  spcGetAdvancedErrorStatus()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrErrorStatus      data structure for advance status
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read advanced status for the specific port
 *
 *
 */
SCRUTINY_STATUS spcGetAdvancedErrorStatus  (
    __IN__ PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__ U32 PortNum, 
    __OUT__ PTR_SCRUTINY_SWITCH_ADVANCED_ERROR_STATUS PtrErrorStatus 
    )
{
    U32 dword;
    SCRUTINY_STATUS  status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerSwitch->logiFunctionEntry ("spcGetAdvancedErrorStatus (PtrDevice=%x, PortNum=0x%x, PtrErrorStatus=%x)", 
            PtrDevice != NULL, PortNum, PtrErrorStatus != NULL);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, 0x70, &dword);

	if (status != SCRUTINY_STATUS_SUCCESS)
	{
		gPtrLoggerSwitch->logiFunctionExit ("spcGetAdvancedErrorStatus (status=0x%x)", status);
		
	    return (status);
	}

    PtrErrorStatus->ErrorStatus.CorrectableErrorDetected    = (U8)((dword >> 16) & 1);
    PtrErrorStatus->ErrorStatus.NonFatalErrorDetected       = (U8)(dword >> 17) & 1;
    PtrErrorStatus->ErrorStatus.FatalErrorDetected          = (U8)(dword >> 18) & 1;
    PtrErrorStatus->ErrorStatus.UnsupportedRequestDetected  = (U8)(dword >> 19) & 1;

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, 0xFB8, &dword);
	
	if (status != SCRUTINY_STATUS_SUCCESS)
	{
		gPtrLoggerSwitch->logiFunctionExit ("spcGetAdvancedErrorStatus (status=0x%x)", status);
		
	    return (status);
	}
	
    PtrErrorStatus->Uncorrectable.word = dword;

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, 0xFC4, &dword);
    
	if (status != SCRUTINY_STATUS_SUCCESS)
	{
		gPtrLoggerSwitch->logiFunctionExit ("spcGetAdvancedErrorStatus (status=0x%x)", status);
		
	    return (status);
	}
    
    PtrErrorStatus->Correctable.word = dword;

    gPtrLoggerSwitch->logiFunctionExit ("spcGetAdvancedErrorStatus (status=0x%x)", status);
	
    return (status);

}

/**
 *
 * @method  spciGetPciPortErrorStatistic()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   Port                specific port 
 *
 * @param   PtrPciePortErrStatistic    pointer to pci statistics outputs
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   API to get all counters/statistics for specific port
 *
 *
 */
SCRUTINY_STATUS spciGetPciPortErrorStatistic(
    __IN__ PTR_SCRUTINY_DEVICE	 	PtrDevice, 
    __IN__ U32 						Port, 
    __OUT__ PTR_SCRUTINY_SWITCH_ERROR_STATISTICS PtrPciePortErrStatistic)
{
	U32 				chipId = 0;
	SCRUTINY_STATUS   	status;

    gPtrLoggerSwitch->logiFunctionEntry ("spciGetPciPortErrorStatistic (PtrDevice=%x, Port=0x%x, PtrPciePortErrStatistic=%x)", 
                                         PtrDevice != NULL, Port, PtrPciePortErrStatistic != NULL);

    if (PtrPciePortErrStatistic == NULL)
    {
        gPtrLoggerSwitch->logiFunctionExit ("spciGetPciPortErrorStatistic (status=0x%x)", SCRUTINY_STATUS_INVALID_PARAMETER);
        return (SCRUTINY_STATUS_INVALID_PARAMETER);
    }

    status = spcGetPexChipId (PtrDevice, &chipId);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("spciGetPciPortErrorStatistic (status=0x%x)", status);
    	return (status);
    }

    if (Port > (chipId & 0xFF) || !spcIsPortEnabled (PtrDevice, Port))
    {
        gPtrLoggerSwitch->logiFunctionExit ("spciGetPciPortErrorStatistic (status=0x%x)", SCRUTINY_STATUS_INVALID_PORT);
    	return (SCRUTINY_STATUS_INVALID_PORT);
    }

    status = spcGetErrorCounters (PtrDevice, Port, &PtrPciePortErrStatistic->ErrorCounters);

	if (status != SCRUTINY_STATUS_SUCCESS)
	{		
		gPtrLoggerSwitch->logiFunctionExit ("spciGetPciPortErrorStatistic (status=0x%x)", status);
		
		return (status);
	}

    status = spcGetAdvancedErrorStatus (PtrDevice, Port, &PtrPciePortErrStatistic->AdvancedErrorStatus);
	
	if (status != SCRUTINY_STATUS_SUCCESS)
	{		
		gPtrLoggerSwitch->logiFunctionExit ("spciGetPciPortErrorStatistic (status=0x%x)", status);
		
		return (status);
	}
    
	gPtrLoggerSwitch->logiFunctionExit ("spciGetPciPortErrorStatistic (status=0x%x)", status);
		
	return (status);
}
    
