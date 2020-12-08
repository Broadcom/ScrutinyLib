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
#include "switchsignalintegrity.h"


/**
 *
 *  @method  ssiRxEqStatus ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   StartPort               The start port number.
 *
 *  @param   NumberOfPort            The total number of ports 
 *
 *  @param   PtrPortRxEqStatus       Pointer to  data structure containing read back Rx Coeff.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method read switch PCIe port Rx Equalization Coeff, include VGA. AEQ, DFE. 
 *
 */

SCRUTINY_STATUS ssiRxEqStatus (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS PtrPortRxEqStatus)
{
	SCRUTINY_STATUS   	status = SCRUTINY_STATUS_SUCCESS;
    U32     			chipId;
    U32     			maxPort;
    U32     			regVal;
    U32     			portIndex;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiRxEqStatus (PtrDevice=%x, StartPort=%x, NumberOfPort=%x, PtrPortRxEqStatus=%x)", PtrDevice != NULL, StartPort, NumberOfPort, PtrPortRxEqStatus != NULL);

    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatus  (Status = %x) ",status);
		
        return (status);    
    }
	
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);
	
    if (StartPort > (maxPort-1))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatus  (Status = %x) ",status);
		
        return (status);
    }

    if ((StartPort + NumberOfPort) > maxPort)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatus  (Status = %x) ",status);
		
        return (status);
    }

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        status = ssiRxEqStatusOnePort(  PtrDevice,  portIndex, PtrPortRxEqStatus );

		if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatus  (Status = %x) ",status);
            return (status);    
        }
        
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatus  (Status = %x) ",status);
    return (status);
}

/**
 *
 *  @method  ssiRxEqStatusOnePort ()
 *
 *  @param   PtrDevice               pointer to  device
 *
 *  @param   PortIndex               The port number .
 *
 *  @param   PtrPortRxEqStatus       Pointer to  data structure containing read back Rx Coeff.
 *
 *  @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method read switch PCIe port Rx Equalization Coeff, include VGA. AEQ, DFE. 
 *
 */


SCRUTINY_STATUS ssiRxEqStatusOnePort (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS PtrPortRxEqStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;
	U32		lane;
	U32		stn;
	U32		vga, aeq;
	U32		offset;
	S32		dfeTap1P, dfeTap1N, dfeTap2, dfeTap3, dfeTap4, dfeTap5, dfeTap6, dfeTap7, dfeTap8, dfeTap9;
	U32		mask = 0xFFFFFF00;//this will used to extend the signed bit
	U32		qskew, rawQskew;


    gPtrLoggerSwitch->logiFunctionEntry ("ssiRxEqStatusOnePort (PtrDevice=%x, PortIndex=%x, PtrPortRxEq=%x)", PtrDevice != NULL, PortIndex, PtrPortRxEqStatus != NULL);
    	
    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );

    dword = 0x30;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset +  0x1180, &regVal, sizeof (U32));
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

	vga = regVal & 0x1FF;
    dword = 0x31;
	
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset +  0x1180, &regVal, sizeof (U32));
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }
	
    aeq = regVal & 0x1FF;
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0x170, &regVal, sizeof (U32));
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

	rawQskew = regVal & 0x3F;
    
	if((rawQskew>>5) == 1)
	{
		qskew = (~rawQskew & 0x3F) + 1;
	}
	else
    { 
        qskew = rawQskew;
    }

    dword = 0x22;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap2 = regVal & 0xFF;
    
	if (dfeTap2 >> 5) 
    {
        dfeTap2 = dfeTap2 | mask | 0xE0;
    }

    dword = 0x23;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap3 = regVal & 0xFF;
    
	if (dfeTap3 >> 4) 
    {
        dfeTap3 = dfeTap3 | mask | 0xF0;
    }

    dword = 0x24;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap4 = regVal & 0xFF;
    
	if (dfeTap4 >> 4) 
    {
        dfeTap4 = dfeTap4 | mask | 0xF0;
    }

    dword = 0x25;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap5 = regVal & 0xFF;

	if (dfeTap5 >> 3) 
    {
        dfeTap5 = dfeTap5 | mask | 0xF8;
    }

    dword = 0x26;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));
	
	if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

	dfeTap6 = regVal & 0xFF;
	if (dfeTap6 >> 3) 
    {
        dfeTap6 = dfeTap6 | mask | 0xF8;
    }

    dword = 0x27;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap7 = regVal & 0xFF;
	if (dfeTap7 >> 3) 
    {
        dfeTap7 = dfeTap7 | mask | 0xF8;
    }

    dword = 0x28;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap8 = regVal & 0xFF;
    
	if (dfeTap8 >> 2) 
    {
        dfeTap8 = dfeTap8 | mask | 0xFC;
    }

    dword = 0x29;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap9 = regVal & 0xFF;
    
	if (dfeTap9 >> 2) 
    {
        dfeTap9 = dfeTap9 | mask | 0xFC;
    }

    dword = 0x32;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap1P = regVal & 0xFF;

	if (dfeTap1P >> 5) 
    {
        dfeTap1P = dfeTap1P | mask | 0xE0;
    }

    dword = 0x33;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x1180, &regVal, sizeof (U32));

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

    dfeTap1N = regVal & 0xFF;
    
	if(dfeTap1N >> 5) 
    {
        dfeTap1N = dfeTap1N | mask | 0xE0;
    }

    dword = 0x0;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x117C, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
        return (status);    
    }

	gPtrLoggerSwitch->logiDebug("Lane %d, VGA = %d, AEQ = %d, ", (stn * 0x10) + lane, vga, aeq);

	if((rawQskew >> 5) == 1)
	{
		gPtrLoggerSwitch->logiDebug("QSKEW = -%d\n", qskew);
	}
	else
	{
		gPtrLoggerSwitch->logiDebug("QSKEW = +%d\n", qskew);
	}

	gPtrLoggerSwitch->logiDebug("Lane %d, dfeTap1P = %d, dfeTap1N = %d, dfeTap2 = %d, dfeTap3 = %d, dfeTap4 = %d, dfeTap5 = %d, dfeTap6 = %d, dfeTap7 = %d, dfeTap8 = %d, dfeTap9 = %d\n", (stn*0x10)+lane, dfeTap1P, dfeTap1N, dfeTap2, dfeTap3, dfeTap4, dfeTap5, dfeTap6, dfeTap7, dfeTap8,dfeTap9);

    PtrPortRxEqStatus->PortRxEqs[PortIndex].VGA = vga;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].AEQ = aeq;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap1P = dfeTap1P;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap1N = dfeTap1N;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap2  = dfeTap2;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap3  = dfeTap3;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap4  = dfeTap4;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap5  = dfeTap5;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap6  = dfeTap6;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap7  = dfeTap7;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap8  = dfeTap8;
    PtrPortRxEqStatus->PortRxEqs[PortIndex].DfeTap9  = dfeTap9;

    gPtrLoggerSwitch->logiFunctionExit ("ssiRxEqStatusOnePort  (Status = %x) ",status);
    return (status); 

}

/**
 *
 *  @method  ssiTxCoeff ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *  @param   PtrPortTxCoeffStatus       Pointer to  data structure containing read back Tx Coeff.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method read switch PCIe port Tx  Coeff, include near end and far end . 
 *
 */

SCRUTINY_STATUS ssiTxCoeff( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF PtrPortTxCoeffStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;

    U32     chipId;
    U32     maxPort;
    U32     regVal;
    U32     portIndex;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiTxCoeff (PtrDevice=%x, StartPort=%x, NumberOfPort=%x, PtrPortTxCoeffStatus=%x)", PtrDevice != NULL, StartPort, NumberOfPort, PtrPortTxCoeffStatus != NULL);

    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeff  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);
    
    if (StartPort > (maxPort-1))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeff  (Status = %x) ",status);
        return (status);
    }

    if ((StartPort + NumberOfPort) > maxPort)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeff  (Status = %x) ",status);
        return (status);
    }

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        status = ssiTxCoeffOnePort(  PtrDevice,  portIndex, PtrPortTxCoeffStatus );
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeff  (Status = %x) ",status);
            return (status);    
        }
        
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeff  (Status = %x) ",status);
    return (status);

}

/**
 *
 *  @method  ssiTxCoeffOnePort ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The port number.
 *
 *  @param   PtrPortTxCoeffStatus       Pointer to  data structure containing read back Tx Coeff.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method read switch PCIe port Tx  Coeff, include near end and far end . 
 *
 */


SCRUTINY_STATUS ssiTxCoeffOnePort( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF PtrPortTxCoeffStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;
	U32		lane;
	U32		stn;
    U32     offset;
    U32		preCursorNear;
    U32     mainCursorNear;
    U32     postCursorNear;
    U32		preCursorFar;
    U32     mainCursorFar;
    U32     postCursorFar;
    U32     regBBCSaved;
    
    gPtrLoggerSwitch->logiFunctionEntry ("ssiTxCoeffOnePort (PtrDevice=%x, PortIndex=%x, PtrPortTxCoeffStatus=%x)", PtrDevice != NULL, PortIndex, PtrPortTxCoeffStatus != NULL);

	//the register is 0x60800000|(stn<<16)|0xBBC ,it is a station level register, only the bit field in the PORT 0 of a station has the control function
    ssiGetCfgSpaceStationLevelRegBaseOffset (PortIndex, &offset,  &stn,  &lane );

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xBBC, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeffOnePort  (Status = %x) ",status);
        return (status);    
    }
    
    regBBCSaved = regVal;
    //read near end Tx coeff
    dword = ((lane & 0xF) << 28) |  (regBBCSaved & 0x0EFFFFFF);
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xBBC, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeffOnePort  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xBBC, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeffOnePort  (Status = %x) ",status);
        return (status);    
    }

    gPtrLoggerSwitch->logiDebug("port = %d near end Tx = %8x", PortIndex,regVal);
    preCursorNear = regVal & 0x3F;
    mainCursorNear = (regVal >> 8) & 0x3F;
    postCursorNear = (regVal >> 16) & 0x3F; 

    //read far end Tx coeff
    dword = ((lane & 0xF) << 28) |  (regBBCSaved & 0x0EFFFFFF) | (0x1 << 24);
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xBBC, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeffOnePort  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xBBC, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeffOnePort  (Status = %x) ",status);
        return (status);    
    }

    gPtrLoggerSwitch->logiDebug("port = %d far end Tx = %8x", PortIndex, regVal);
    preCursorFar = regVal & 0x3F;
    mainCursorFar = (regVal >> 8) & 0x3F;
    postCursorFar = (regVal >> 16) & 0x3F;
        
    //restore the register setting
    dword = regBBCSaved;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xBBC, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeffOnePort  (Status = %x) ",status);
        return (status);    
    }

    PtrPortTxCoeffStatus->PortTxCoeffs[PortIndex].PreCursorNear  = preCursorNear;
    PtrPortTxCoeffStatus->PortTxCoeffs[PortIndex].MainCursorNear = mainCursorNear;
    PtrPortTxCoeffStatus->PortTxCoeffs[PortIndex].PostCursorNear = postCursorNear;
    PtrPortTxCoeffStatus->PortTxCoeffs[PortIndex].PreCursorFar   = preCursorFar;
    PtrPortTxCoeffStatus->PortTxCoeffs[PortIndex].MainCursorFar  = mainCursorFar;
    PtrPortTxCoeffStatus->PortTxCoeffs[PortIndex].PostCursorFar  = postCursorFar;
    
    gPtrLoggerSwitch->logiFunctionExit ("ssiTxCoeffOnePort  (Status = %x) ",status);
    return (status); 

}

/**
 *
 *  @method  ssiHardwareEyeStart ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */


SCRUTINY_STATUS ssiHardwareEyeStart( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     chipId;
    U32     maxPort;
    U32     portIndex;    
    U32     regVal;
	U32		lane;
	U32		stn;
    U32     offset;
    U32     activeLane;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyeStart (PtrDevice=%x, StartPort=%x, NumberOfPort=%x, PtrPortHwEyeStatus=%x)", PtrDevice != NULL, StartPort, NumberOfPort, PtrPortHwEyeStatus != NULL );

    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeStart  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);

    if (StartPort > (maxPort-1))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeStart  (Status = %x) ",status);
        return (status);
    }

    if ((StartPort + NumberOfPort) > maxPort)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeStart  (Status = %x) ",status);
        return (status);
    }

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        ssiGetCfgSpaceStationLevelRegBaseOffset (portIndex, &offset,  &stn,  &lane );

	    // first check if the lane is active
        status = bsdiMemoryRead32 (PtrDevice, offset + 0x330, &regVal, sizeof (U32));
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeStart  (Status = %x) ",status);
            return (status);    
        }
        
        activeLane = regVal & 0xFFFF;

	
	    if(((activeLane >> lane) & 0x1) != 1)
	    {
		    PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PortActive = 0;
            gPtrLoggerSwitch->logiDebug("Target lane %d is not active\n", portIndex);
            
	    }
        else
        {
            PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PortActive = 1;
            gPtrLoggerSwitch->logiDebug("Target lane %d is active\n", portIndex);
            
        }

    
    }

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PortActive)
        {
            status = ssiHardwareEyeOnePortStart(  PtrDevice,  portIndex, PtrPortHwEyeStatus );
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeStart  (Status = %x) ",status);
                return (status);    
            }
            
        }
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeStart  (Status = %x) ",status);
    return (status);

}

/**
 *
 *  @method  ssiHardwareEyeOnePortStart ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The port number.
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */

SCRUTINY_STATUS ssiHardwareEyeOnePortStart( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;    
	U32		lane;
	U32		stn;
	U32		offset;
	U32		tmp;
	U32		targetBER = 100000000;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyeOnePortStart (PtrDevice=%x, PortIndex=%x)", PtrDevice != NULL, PortIndex);
	
    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE74, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }
    
    tmp = regVal & 0xFFFFFFFE;
    
    dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE74, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }
    	
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    tmp = regVal & 0xFFFFFFFE;
    
    dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }
	
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    tmp = regVal  | 0x1;
    
    dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }
	
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    tmp = regVal  | 0x8;
    
    dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }
    	
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    tmp = regVal  & 0xFFFFFEFF;
        	
    dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }
	
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE04, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    tmp = (regVal  & 0xFFFFFF1F) | 0x60;
    	
    dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE04, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }
	
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE10, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    tmp = regVal  & 0xFFFFFFE0;
	
	dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE10, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

	dword = targetBER & 0xFFFF;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE1C, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

	dword = (targetBER >> 16) & 0xFFFF;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE20, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

	dword = 0xFE80;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE10, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

    tmp = regVal  | 0x4;
    	
	dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
        return (status);    
    }

	gPtrLoggerSwitch->logiDebug ("Start Measuring the lane %d hardware eye\n", (stn*0x10) + lane);
    
    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortStart  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiHardwareEyePoll ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */

SCRUTINY_STATUS ssiHardwareEyePoll( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;

    U32     chipId;
    U32     maxPort;
    U32     regVal;
    U32     portIndex;
    U32     totalDonePort;
    U32     totalActivePort;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyePoll (PtrDevice=%x, StartPort=%x, NumberOfPort=%x, PtrPortHwEyeStatus=%x)", PtrDevice != NULL, StartPort, NumberOfPort, PtrPortHwEyeStatus != NULL );

    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyePoll  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);

    if (StartPort > (maxPort-1))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyePoll  (Status = %x) ",status);
        return (status);
    }

    if ((StartPort + NumberOfPort) > maxPort)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyePoll  (Status = %x) ",status);
        return (status);
    }
    
    totalActivePort = 0;
    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
       if (PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PortActive)
       {
           totalActivePort++;
       }
    
    }
    
    totalDonePort = 0;
    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
       if (PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PortActive)
        {
            status = ssiHardwareEyeOnePortPoll (  PtrDevice,  portIndex, PtrPortHwEyeStatus);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyePoll  (Status = %x) ",status);
                return (status);    
            }
            
            gPtrLoggerSwitch->logiDebug ("port %d hardware eye polling done = %d", portIndex, PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PollingDone);

            if (PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PollingDone)
            {
                 totalDonePort++;
            }
        }

    }
    if (totalDonePort == totalActivePort)
    {
        PtrPortHwEyeStatus->GlobalPollingDone = 1;    
    }
    else if (totalDonePort < totalActivePort)
    {
        PtrPortHwEyeStatus->GlobalPollingDone = 0;
    }
    else
    {
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyePoll  (Status = %x) ",status);
        return (status);
        
    }
    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyePoll  (Status = %x) ",status);
    return (status);

}

/**
 *
 *  @method  ssiHardwareEyeOnePortPoll ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The port number.
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */


SCRUTINY_STATUS ssiHardwareEyeOnePortPoll( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;    
	U32		lane;
	U32		stn;
	U32		offset;
	U32		tmp;
	U8		grant, pass, done;


    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyeOnePortPoll (PtrDevice=%x, PortIndex=%x, PtrPortHwEyeStatus=%x)", PtrDevice != NULL, PortIndex, PtrPortHwEyeStatus != NULL);
	
    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );
	
	pass = 0;	
	grant = 0;
	done = 0;

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xEBC, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortPoll  (Status = %x) ",status);
        return (status);    
    }
    
    tmp = regVal  & 0x1F;

	gPtrLoggerSwitch->logiDebug("Current status=%x\n", tmp);
	grant	= ((U8)tmp >> 2) & 0x1;
	done	= ((U8)tmp >> 3) & 0x1;
	pass = ((U8)tmp >> 4) & 0x1;
    gPtrLoggerSwitch->logiDebug("grant=%x, done=%x, pass=%x\n", grant, done, pass);
    PtrPortHwEyeStatus->PortHwEye[PortIndex].Flag.PollingDone = done;

    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortPoll  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiHardwareEyeGet ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */
 

SCRUTINY_STATUS ssiHardwareEyeGet( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;

    U32     chipId;
    U32     maxPort;
    U32     regVal;
    U32     portIndex;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyeGet (PtrDevice=%x, StartPort=%x, NumberOfPort=%x, PtrPortHwEyeStatus=%x)", PtrDevice != NULL, StartPort, NumberOfPort, PtrPortHwEyeStatus != NULL );

    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeGet  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);

    if (StartPort > (maxPort-1))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeGet  (Status = %x) ",status);
        return (status);
    }

    if ((StartPort + NumberOfPort) > maxPort)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeGet  (Status = %x) ",status);
        return (status);
    }

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PortActive)
        {
            status = ssiHardwareEyeOnePortGet (  PtrDevice,  portIndex, PtrPortHwEyeStatus);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeGet  (Status = %x) ",status);
                return (status);    
            }
            
        }
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeGet  (Status = %x) ",status);
    return (status);

}

/**
 *
 *  @method  ssiHardwareEyeOnePortGet ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The port number.
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */

SCRUTINY_STATUS ssiHardwareEyeOnePortGet( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;    
	U32		lane;
	U32		stn;
	U32		offset;
	U32		regE88, regE8c;
	U32		eyeLeft, eyeRight, eyeUp, eyeDn;
	double	eyeL, eyeR, eyeU, eyeD;


    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyeOnePort (PtrDevice=%x, PortIndex=%x, PtrPortHwEyeStatus=%x)", PtrDevice != NULL, PortIndex, PtrPortHwEyeStatus != NULL);
	
    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE88, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortGet  (Status = %x) ",status);
        return (status);    
    }
    
    regE88 = regVal  & 0xFFFF;
            
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE8C, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortGet  (Status = %x) ",status);
        return (status);    
    }
    
    regE8c = regVal  & 0xFFFF;
            
	eyeLeft = regE88 & 0x3F;
	eyeRight = (regE88 >> 8) & 0x3F;
	eyeL = (64 - (double)eyeLeft) * 100 / 32;
	eyeR = (double)eyeRight / 32 * 100;
	eyeUp = regE8c & 0x7F;
	eyeDn = (regE8c >> 8) & 0x7F;
	eyeU = (double)eyeUp * 3.52;
	eyeD = ((double)eyeDn - 64) * 3.52;
	gPtrLoggerSwitch->logiDebug ("Eye Left = %.1f%%UI, Eye Right = %.1f%%UI, Eye Height = %.1fmV, Eye Bottom = %.1fmV\n", eyeL, eyeR, eyeU, eyeD);

    PtrPortHwEyeStatus->PortHwEye[PortIndex].EyeLeft  = eyeL;
    PtrPortHwEyeStatus->PortHwEye[PortIndex].EyeRight = eyeR;
    PtrPortHwEyeStatus->PortHwEye[PortIndex].EyeUp    = eyeU;
    PtrPortHwEyeStatus->PortHwEye[PortIndex].EyeDown  = eyeD;
    
    
    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePort  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiHardwareEyeClean ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *  @param   PtrPortHwEyeStatus         Pointer to  data structure containing read back Hardware eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */

SCRUTINY_STATUS ssiHardwareEyeClean( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;

    U32     chipId;
    U32     maxPort;
    U32     regVal;
    U32     portIndex;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyeClean (PtrDevice=%x, StartPort=%x, NumberOfPort=%x, PtrPortHwEyeStatus=%x)", PtrDevice != NULL, StartPort, NumberOfPort, PtrPortHwEyeStatus != NULL );

    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeClean  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);
    if (StartPort > (maxPort-1))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeClean  (Status = %x) ",status);
        return (status);
    }

    if ((StartPort + NumberOfPort) > maxPort)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeClean  (Status = %x) ",status);
        return (status);
    }

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortHwEyeStatus->PortHwEye[portIndex].Flag.PortActive)
        {
            status = ssiHardwareEyeOnePortClean (  PtrDevice,  portIndex);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeClean  (Status = %x) ",status);
                return (status);    
            }
            
        }
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeClean  (Status = %x) ",status);
    return (status);

}

/**
 *
 *  @method  ssiHardwareEyeOnePortClean ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The port number.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do hardware eye test on  switch PCIe port .ONLY active lane can do eye test. 
 *                                   four API should use together:
 *                                   Step 1: call ScrutinySwitchHardwareEyeStart, which will start the hardware eye test
 *                                   Step 2: call ScrutinySwitchHardwareEyePoll,  which will check if hardware eye is finished,
 *                                           customer can call it at any convenient time, it will return immediately.
 *                                           if polling result say hardware finished ,then go to step 3, otherwise need polling again. 
 *                                   Step 3: if Step 2 polling result say hardware eye finished, then call ScrutinySwitchHardwareEyeGet
 *                                   Step 4: call ScrutinySwitchHardwareEyeClean to do some clean job.
 *
 *
 */

SCRUTINY_STATUS ssiHardwareEyeOnePortClean ( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;    
	U32		lane;
	U32		stn;
	U32		offset;
	U32		tmp;


    gPtrLoggerSwitch->logiFunctionEntry ("ssiHardwareEyeOnePortClean (PtrDevice=%x, PortIndex=%x)", PtrDevice != NULL, PortIndex);

    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeClean  (Status = %x) ",status);
        return (status);    
    }
    
    tmp = regVal & 0xFFFFFFFB;
    	
	dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortClean  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE74, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortClean  (Status = %x) ",status);
        return (status);    
    }

    tmp = regVal | 0x1;
        	
	dword = tmp;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE74, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortClean  (Status = %x) ",status);
        return (status);    
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiHardwareEyeOnePortClean  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiSoftwareEye ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do software eye test on  switch PCIe port . ONLY active lane can do software eye test
 *                                   The result will save as a file in current directory
 *
 */

SCRUTINY_STATUS ssiSoftwareEye( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_PORT_SW_EYE PtrPortSwEyeStatus = NULL;
    
    gPtrLoggerSwitch->logiFunctionEntry ("ssiSoftwareEye (PtrDevice=%x, StartPort=%x, NumberOfPort=%x)", PtrDevice != NULL, StartPort, NumberOfPort);

    PtrPortSwEyeStatus = (PTR_SCRUTINY_SWITCH_PORT_SW_EYE) sosiMemAlloc (sizeof (SCRUTINY_SWITCH_PORT_SW_EYE));
	
    if (PtrPortSwEyeStatus == NULL)
    {
        status = SCRUTINY_STATUS_NO_MEMORY;
		
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEye  (Status = %x) ",status);
        return (status);

    }   
    sosiMemSet (PtrPortSwEyeStatus, 0, sizeof (SCRUTINY_SWITCH_PORT_SW_EYE));

    status = ssiSoftwareEyeWithBuf( PtrDevice, StartPort,  NumberOfPort,  PtrPortSwEyeStatus );

    if (PtrPortSwEyeStatus)
    {
        sosiMemFree (PtrPortSwEyeStatus);
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEye  (Status = %x) ",status);
    return (status); 

}

/**
 *
 *  @method  ssiSoftwareEyeWithBuf ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *
 *  @param   PtrPortSwEyeStatus         Pointer to  data structure containing read back Soft eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do software eye test on  switch PCIe port . ONLY active lane can do software eye test
 *                                   The result will save as a file in current directory
 *
 */


SCRUTINY_STATUS ssiSoftwareEyeWithBuf( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_SW_EYE PtrPortSwEyeStatus )
{
	SCRUTINY_STATUS        status = SCRUTINY_STATUS_SUCCESS;
    SOSI_FILE_HANDLE       ptrFile = NULL;
    U32                    portIndex;
    U32                    chipId;
    U32                    maxPort;
    U32                    dword;
    U32                    verticalIndex;
    U32                    horizonIndex;
    U32         regVal;
    U32         bufSize;
    char        tempBuffer[512];
	U32			value;
    U32         stn;
	U32         lane;
    U32         offset;	
	U32			bitCount;
	U32			voffset = 0;
    S32         voffsetInt = 0;
	S32			phase;
	U32			LoHi;
	U32			timeOut = 1000;
	U32			wait = 0;
	char		pFileName[100];    
	char		strLane[16];
	U16			activeLane;
	U32			horiStep;
	U32			vertValue;


    gPtrLoggerSwitch->logiFunctionEntry ("ssiSoftwareEyeWithBuf (PtrDevice=%x, StartPort=%x, NumberOfPort=%x, PtrPortSwEyeStatus=%x)", PtrDevice != NULL, StartPort, NumberOfPort, PtrPortSwEyeStatus != NULL);

    bitCount = 100000000;		//default 1e8
    
    //read the chip ID ,extract the max port number, the offset is 0xB7C, just read it from port 0 cfg space
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, 0, 0xB7C, &regVal);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
        return (status);    
    }
    chipId = (regVal >> 16) & 0xFFFF;
    gPtrLoggerSwitch->logiDebug ("Switch chipID = %x", chipId);
    maxPort = ((chipId >> 4) & 0xF)*10 + (chipId & 0xF);
    if (StartPort > (maxPort-1))
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
        return (status);
    }

    if ((StartPort + NumberOfPort) > maxPort)
    {
        status = SCRUTINY_STATUS_INVALID_PARAMETER;
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
        return (status);
    }
    
    //check the active port
    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {

        //The register 0x330 is station based ,so no need port index when calculating offset
        ssiGetCfgSpaceStationLevelRegBaseOffset (portIndex, &offset,  &stn,  &lane );
        
        status = bsdiMemoryRead32 (PtrDevice, offset + 0x330, &regVal, sizeof (U32));
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
            return (status);    
        }
        
        activeLane = regVal & 0xFFFF;

	
	    if(((activeLane >> lane) & 0x1) != 1)
	    {
		    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive = 0;
            gPtrLoggerSwitch->logiDebug("Target lane %d is not active\n", portIndex);
	    }
        else
        {
            PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive = 1;
            gPtrLoggerSwitch->logiDebug("Target lane %d is  active\n", portIndex);
        }

    }
    //end check the active port

    //Step1
    //initialize the port
    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
            
            ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );
            
	        status = ssiSoftEyeSerdesInit(PtrDevice, portIndex, PtrPortSwEyeStatus);   
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            //initialize the voltage offset to 0
	        status = ssiSoftEyeVertAdjust(PtrDevice, portIndex, 0);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            //initialize the phase offset to 0, at the start point, we assume it is 0
            PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase = 0; //current_phase = 0;
	        status = ssiSoftEyePhaseAdjust( PtrDevice, portIndex, PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase, 0);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            //current_phase = 0;//save the current phase
            PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase = 0; 

		    /***********************************************************************************************
		    * Start the soft eye scope
		    ************************************************************************************************/
            status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            value = regVal;
                
		    value |= (1<<1);
            dword = value;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

        }
    }
    //end initialize the port

    //Step2     
    //Check logic is granted or not .how much time it will take? do we need optimize it?
    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
		                
            ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );
            
            /************************************************************************************************
		    *	Check logic is granted or not
		    *************************************************************************************************/
            status = bsdiMemoryRead32 (PtrDevice, offset + 0xEBC, &regVal, sizeof (U32));
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            value = regVal;
                
		    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.CommandReceived = (value>>1) & 0x1;

		    gPtrLoggerSwitch->logiDebug("Wait for command received");
		    wait = 0;

		    while(PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.CommandReceived == 0)
		    {
                status = bsdiMemoryRead32 (PtrDevice, offset + 0xEBC, &regVal, sizeof (U32));
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                    return (status);    
                }

                value = regVal;
                        
			    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.CommandReceived = (value>>1) & 0x1;
			    wait++;
                sosiSleep (1);
			    if(wait > timeOut)
			    {
				    gPtrLoggerSwitch->logiDebug("Plot eye command can't received on lane %d", portIndex);
				    goto _restoreExit;
			    }
		    }

		    gPtrLoggerSwitch->logiDebug("Start command is received");

		    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.Granted = (value>>2)&0x1;

		    gPtrLoggerSwitch->logiDebug("Waiting for granted");
		    wait = 0;

		    while(PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.Granted == 0)
		    {
                status = bsdiMemoryRead32 (PtrDevice, offset + 0xEBC, &regVal, sizeof (U32));
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                    return (status);    
                }

                value = regVal;
            
			    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.Granted = (value>>2)&0x1;
			    wait++;
                sosiSleep (1);
			    if(wait > timeOut)
			    {
				    gPtrLoggerSwitch->logiDebug("Plot eye logic does not grant on lane %d", portIndex);
				    goto _restoreExit;
			    }		
		    }
        }
    }
    //end Check logic is granted or not
	gPtrLoggerSwitch->logiDebug("Soft eye logic is granted");

    //Step3
	//start the measurement		
	gPtrLoggerSwitch->logiDebug("Start eye measurement");
    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
            ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );

            status = bsdiMemoryRead32 (PtrDevice, offset + 0xE70, &regVal, sizeof (U32));
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            value = regVal;
        
		    value |= (3<<7);
            dword = value;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE70, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }
        }
    }

    gPtrLoggerSwitch->logiDebug("Start  Vertical Scan");

	for(voffsetInt = 63; voffsetInt > -64; voffsetInt--)
	{
        verticalIndex = (U32)(63 - voffsetInt);
        
        if (voffsetInt > 0)//for the positive voltage
        {
            voffset = (U32)voffsetInt;
        }
        else //for the negative voltage.
        {
            vertValue = (U32)(0 - voffsetInt);
            voffset = vertValue | (1<<6);
        }
        
        //Set voltage for all test ports
        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {
		        gPtrLoggerSwitch->logiDebug("\nShift vertical offset = 0x%x ", voffset);
			
		        status = ssiSoftEyeVertAdjust( PtrDevice, portIndex, voffset);
		        if(status)
		        {
			        gPtrLoggerSwitch->logiDebug("Vertical adjust has error");
			        goto _restoreExit;
		        }

            }
        }

		for(LoHi = 0; LoHi < 2; LoHi++)
		{
            //the phase go back to center
            for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
            {
                if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
                {
			        status = ssiSoftEyePhaseAdjust(PtrDevice, portIndex, PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase, 0);		//go back to center
			        if(status)
			        {
				        gPtrLoggerSwitch->logiDebug("Phase adjust has error");
				        goto _restoreExit;
			        }
			        else
                    { 
                        PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase = 0; 
                         
                    }

                }
            }


            for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
            {
                if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
                {
                    ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );


                    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE70, &regVal, sizeof (U32));
                    if (status != SCRUTINY_STATUS_SUCCESS)
                    {
                        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                        return (status);    
                    }

                    value = regVal;
                
				    value &= ~(3<<7);
				    value |= ((LoHi*3) << 7);
                    dword = value;
                    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE70, &dword);
                    if (status != SCRUTINY_STATUS_SUCCESS)
                    {
                        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                        return (status);    
                    }

                }
            }


			phase = -20;
			for(horiStep = 0; horiStep < 42; horiStep++)
			{


                //change the phase
                for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
                {
                    if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
                    {
				        status = ssiSoftEyePhaseAdjust(PtrDevice, portIndex, PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase, phase);
				        if(status)
				        {
					        gPtrLoggerSwitch->logiDebug("Phase adjust has error");
					        goto _restoreExit;
				        }
				        else
                        { 
                            PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase = phase; 
                            
                        }
			            gPtrLoggerSwitch->logiDebug(" voltage scan start, voffset = %x,LoHi = %x, current_phase=%d", voffset, LoHi,PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase);

                    }
                }

				if(LoHi == 0)
				{	
                    status = ssiSoftEyeGetBer ( PtrDevice,  StartPort,   NumberOfPort,  bitCount,  PtrPortSwEyeStatus );                    
					
					if(status)
					{
						goto _restoreExit;
					}
                    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
                    {
                        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
                        {
                            PtrPortSwEyeStatus->PortSwEye[portIndex].TripleArrayBuffer[verticalIndex][horiStep][0] = PtrPortSwEyeStatus->PortSwEye[portIndex].BER;
                            
                        }                    
                    }
				}
				else
				{
                    status = ssiSoftEyeGetBer ( PtrDevice,  StartPort,   NumberOfPort,  bitCount,  PtrPortSwEyeStatus );                    					
                    if(status)
					{
						goto _restoreExit;
					}
                    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
                    {
                        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
                        {
                            PtrPortSwEyeStatus->PortSwEye[portIndex].TripleArrayBuffer[verticalIndex][horiStep][1] = PtrPortSwEyeStatus->PortSwEye[portIndex].BER;
                            
                        }
                    
                    }
                    
				}
    
                for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
                {
                    if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
                    {
				        PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase = phase;
                    }
                }
                
				phase++;
			}
		}//End of phase shift


        //the phase go back to center
        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {
			    status = ssiSoftEyePhaseAdjust(PtrDevice, portIndex, PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase, 0);		//go back to center
			    if(status)
			    {
				    gPtrLoggerSwitch->logiDebug("Phase adjust has error");
				    goto _restoreExit;
			    }
			    else
                { 
                    PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase = 0; 
                     
                }

            }
        }

	}
    
    gPtrLoggerSwitch->logiDebug("finish  vertical Scan");

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
            for (verticalIndex = 0; verticalIndex < 127; verticalIndex++)
            {
                for (horizonIndex = 0; horizonIndex <42; horizonIndex++)
                {
                    PtrPortSwEyeStatus->PortSwEye[portIndex].TripleArrayBuffer[verticalIndex][horizonIndex][2] = ( PtrPortSwEyeStatus->PortSwEye[portIndex].TripleArrayBuffer[verticalIndex][horizonIndex][0] + PtrPortSwEyeStatus->PortSwEye[portIndex].TripleArrayBuffer[verticalIndex][horizonIndex][1] ) / 2;
                }
            }

        }
    }

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
	        sosiStringCopy(pFileName, "Eye_Lane_");
            sosiSprintf (strLane, sizeof (strLane), "%d", portIndex);
	        sosiStringCat(pFileName, strLane);
	        sosiStringCat(pFileName, ".dat");
	
	        ptrFile = sosiFileOpen(pFileName, "w");
	
            bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s\n", "min_pos_ui: -0.625\n");
            sosiFileWrite(ptrFile, (U8 *)tempBuffer, bufSize);
            bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s\n", "max_pos_ui: 0.625\n");
            sosiFileWrite(ptrFile, (U8 *)tempBuffer, bufSize);
            bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s\n", "min_mv: -233.7890625\n");
            sosiFileWrite(ptrFile, (U8 *)tempBuffer, bufSize);
            bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), " %s\n", "max_mv: 233.7890625\n");
            sosiFileWrite(ptrFile, (U8 *)tempBuffer, bufSize);    
	        sosiFileWrite(ptrFile, (U8 *)"\n", 1);

            for (verticalIndex = 0; verticalIndex < 127; verticalIndex++)
            {
                for (horizonIndex = 0; horizonIndex <42; horizonIndex++)
                {
                    bufSize = sosiSprintf (tempBuffer, sizeof (tempBuffer), "%e ", PtrPortSwEyeStatus->PortSwEye[portIndex].TripleArrayBuffer[verticalIndex][horizonIndex][2]);
                    sosiFileWrite(ptrFile, (U8 *)tempBuffer, bufSize);    

                }
                sosiFileWrite(ptrFile, (U8 *)"\n", 1);
                
            }
            sosiFileClose (ptrFile);
        }
    }



_restoreExit:

    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {            
            ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );
                        
	        status = ssiSoftEyeVertAdjust( PtrDevice, portIndex, 0);						//move vert back to 0mV
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

	        status = ssiSoftEyePhaseAdjust( PtrDevice, portIndex, PtrPortSwEyeStatus->PortSwEye[portIndex].CurrentPhase, 0);		//move phase back to center
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            dword = 0x2C09;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].OldRegE74; //   old_e74[i];
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE74, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }
    
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].OldReg344; //  old_344[i];
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0x344, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }
    
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].OldReg398; //   old_398[i];
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0x398, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }
    
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].OldRegC8; //  old_c8[i];
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xC8, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }
        	
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].OldRegE70; //  old_e70[i];
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE70, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
                return (status);    
            }

        }
    }
    
    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftwareEyeWithBuf  (Status = %x) ",status);
    return (status); 

}

/**
 *
 *  @method  ssiSoftEyeSerdesInit ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The  port number.
 *
 *
 *  @param   PtrPortSwEyeStatus         Pointer to  data structure containing read back Soft eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do serdes init on  switch PCIe port . 
 *                                   
 *
 */


SCRUTINY_STATUS ssiSoftEyeSerdesInit(__IN__ PTR_SCRUTINY_DEVICE PtrDevice, U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_SW_EYE PtrPortSwEyeStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;
    U32     stn;
	U32     lane;
    U32     offset;


    gPtrLoggerSwitch->logiFunctionEntry ("ssiSoftEyeSerdesInit (PtrDevice=%x,  PortIndex=%x, PtrPortSwEyeStatus=%x)",\
     PtrDevice != NULL, PortIndex, PtrPortSwEyeStatus != NULL);

    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );
		
    dword = 0x0;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE68, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
    
    dword = 0x2C08;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
    
    dword = 0x2C09;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    dword = 0x4040;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE04, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    dword = 0;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE14, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
    
    dword = 0;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE18, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
    
    dword = 0xFFFF;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE1C, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    dword = 0xFFFF;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE20, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    dword = 0xF;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE6C, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
    
	dword = 0xC180;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE70, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE74, &regVal, sizeof (U32));
    PtrPortSwEyeStatus->PortSwEye[PortIndex].OldRegE74 = regVal;
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
     
    dword = (PtrPortSwEyeStatus->PortSwEye[PortIndex].OldRegE74) & (~(1<<0));
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE74, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0x344, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    PtrPortSwEyeStatus->PortSwEye[PortIndex].OldReg344 = regVal;
    
    dword = (PtrPortSwEyeStatus->PortSwEye[PortIndex].OldReg344)|(1<<1);
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x344, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0x398, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    PtrPortSwEyeStatus->PortSwEye[PortIndex].OldReg398 = regVal;
            
    dword = (PtrPortSwEyeStatus->PortSwEye[PortIndex].OldReg398)|(3<<8);
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0x398, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE70, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    PtrPortSwEyeStatus->PortSwEye[PortIndex].OldRegE70 = regVal;
            
    dword = ((PtrPortSwEyeStatus->PortSwEye[PortIndex].OldRegE70)|(1<<10)) & 0xFF80;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE70, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xC8, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }

    PtrPortSwEyeStatus->PortSwEye[PortIndex].OldRegC8 = regVal;
    
    dword = (PtrPortSwEyeStatus->PortSwEye[PortIndex].OldRegC8)|(0x1<<8);
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xC8, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
        return (status);    
    }
    
    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeSerdesInit  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiSoftEyeVertAdjust ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The  port number.
 *
 *
 *  @param   Voffset                    vertical offset.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do vertical offset adjust on  switch PCIe port . 
 *                                   
 *
 */
SCRUTINY_STATUS	ssiSoftEyeVertAdjust( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, U32 PortIndex, U32 Voffset)	//do vertical adjust on one lane
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;
	U32     value;
    U32     stn;
	U32     lane;
    U32     offset;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiSoftEyeVertAdjust (PtrDevice=%x PortIndex=%x, Voffset=%x)", PtrDevice != NULL, PortIndex, Voffset);

    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );

	if(Voffset > 0x7F)
	{
		gPtrLoggerSwitch->logiDebug("Vertical offset range is 0 to 0x7F, the input vertical offset is out of the range. voffset = 0x%x", Voffset);
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeVertAdjust  (Status = %x) ",status);
        return (status); 
	}

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE70, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeVertAdjust  (Status = %x) ",status);
        return (status);    
    }

    value = regVal;
    
	value &= ~0x7F;
	value |= (Voffset & 0x7F);
    dword = value;
    status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE70, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeVertAdjust  (Status = %x) ",status);
        return (status);    
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeVertAdjust  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiSoftEyePhaseAdjust ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   PortIndex                  The  port number.
 *
 *
 *  @param   CurrentPhase               Current phase offset.
 *
 *  @param   NewPhase                   New phase offset.
 *
    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do phase offset adjust on  switch PCIe port . 
 *                                   
 *
 */

SCRUTINY_STATUS ssiSoftEyePhaseAdjust( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, U32 PortIndex, S32 CurrentPhase, S32 NewPhase)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;

    U32 stn;
	U32 lane;
    U32 offset;
    
	U32 regValE00, regValE68;
	U32 step;
	U32 i;
	U32 quadrant;
	S32	phase;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiSoftEyePhaseAdjust (PtrDevice=%x )", PtrDevice != NULL );

    ssiGetSerdesPortRegBaseOffset (PortIndex, &offset,  &stn,  &lane );


	if((CurrentPhase < -20) || (CurrentPhase > 21))
	{
		gPtrLoggerSwitch->logiDebug("Phase must be within range -20 to 21. Input current phase is out of the range");
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
        return (status); 
	}

	if((NewPhase < -20) || (NewPhase > 21))
	{
		gPtrLoggerSwitch->logiDebug("Phase must be within range -20 to 21. Input new phase is out of the range");
        status = SCRUTINY_STATUS_FAILED;
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
        return (status); 
	}

	phase = CurrentPhase;

	

    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
        return (status);    
    }

    regValE00 = regVal;
    
    status = bsdiMemoryRead32 (PtrDevice, offset + 0xE68, &regVal, sizeof (U32));
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
        return (status);    
    }

    regValE68 = regVal;

	if(NewPhase > phase) 
	{
		step = NewPhase - phase;
		if(step > 41)		//full range of the phase is 41 steps
		{
			gPtrLoggerSwitch->logiDebug("Full range of phase is from -20 to 21, delta between current phase and new phase is out of the range");
			gPtrLoggerSwitch->logiDebug("Message 1 New phase = %d, Current phase = %d\n", NewPhase, phase);
            status = SCRUTINY_STATUS_FAILED;
            gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
            return (status); 
		}

		for(i = 0; i < step; i++)
		{
			if(((phase>>5)&0x1) != (((phase+1)>>5)&0x1))
			{
				quadrant = 0;
				regValE68 &= ~(3<<6);
				regValE68 |= (quadrant<<6);				
                dword = regValE68;
				
                status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE68, &dword);

				if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                    return (status);    
                }

			}
			phase++;
			regValE68 &= ~(0x3F);
			regValE68 |= (phase & 0x3F);

            dword = regValE68;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE68, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                return (status);    
            }

            dword = regValE00|(1<<12);
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                return (status);    
            }
            
            dword = regValE00;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                return (status);    
            }

		}
	}
	else if (NewPhase < phase)
	{
		step = phase - NewPhase;
		if(step > 41)		//full range of the phase is 41 steps
		{
			gPtrLoggerSwitch->logiDebug("Full range of phase is from -20 to 21, delta between current phase and new phase is out of the range");
			gPtrLoggerSwitch->logiDebug("Message 2 New phase = %d, Current phase = %d", NewPhase, phase);
            status = SCRUTINY_STATUS_FAILED;
            gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
            return (status); 
		}

		for(i = 0; i < step; i++)
		{
			if(((phase>>5)&0x1) != ((phase-1)>>5&0x1))
			{
				quadrant = 3;
				regValE68 &= ~(3<<6);
				regValE68 |= (quadrant<<6);
                dword = regValE68;
                status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE68, &dword);
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                    return (status);    
                }

			}

			phase--;
			regValE68 &= ~(0x3F);
			regValE68 |= (phase & 0x3F);

            dword = regValE68;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE68, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                return (status);    
            }

            dword = regValE00|(1<<12);
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);                
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                return (status);    
            }

            dword = regValE00;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword); 
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
                return (status);    
            }

		}
	}
    
    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyePhaseAdjust  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiSoftEyeGetBer ()
 *
 *  @param   PtrDevice                  pointer to  device
 *
 *  @param   StartPort                  The start port number.
 *
 *  @param   NumberOfPort               The total number of ports 
 *
 *  @param   RequestBitCount            The total number of bit count 
 *
 *
 *  @param   PtrPortSwEyeStatus         Pointer to  data structure containing read back Soft eye data.
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *                              SCRUTINY_STATUS_FAILED on failure.
 *                              SCRUTINY_STATUS_INVALID_HANDLE - invalid input handle
 *                              SCRUTINY_STATUS_UNSUPPORTED - device not support this feature
 *                              SCRUTINY_STATUS_NO_MEMORY - interal memory failure
 *                              SCRUTINY_STATUS_IOCTL_ERROR - An IOCTL error occurred when an IOCTL call was issued to the device driver.
 *
 *  @brief                           This method do software eye BER test on  switch PCIe port . 
 *                                   
 *
 */


SCRUTINY_STATUS ssiSoftEyeGetBer ( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __IN__ U32 RequestBitCount, __OUT__  PTR_SCRUTINY_SWITCH_PORT_SW_EYE PtrPortSwEyeStatus )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     regVal;
    U32     dword;
    U32     portIndex;
    U32     stn;
	U32     lane;
    U32     offset;

	U32 requestSampleCount;
	U32 minSampleCount = 1000000 / 16;


    gPtrLoggerSwitch->logiFunctionEntry ("ssiSoftEyeGetBer (PtrDevice=%x )", PtrDevice != NULL );


    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
       
            PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCountLo = 0;
            PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCountHi = 0;
            PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount = 0;
            PtrPortSwEyeStatus->PortSwEye[portIndex].LastSampleCount = 0;        
            PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PollingDone = 0;
        }
        
    }
	
	requestSampleCount = RequestBitCount / 32 * 3;		//each sample takes 32 bits. But only 16 bits are sampled



    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
            ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );

            status = bsdiMemoryRead32 (PtrDevice, offset + 0xE00, &regVal, sizeof (U32));
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                return (status);    
            }

            PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 = regVal;

		    PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 = PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 & (~(1<<4)) & (~(1<<0));														//Clear the eye scope start bit
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                return (status);    
            }

		    PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 |= (1<<0);
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                return (status);    
            }

		    PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 |= (1<<4);	//start the eye scope
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                return (status);    
            }

        }
    }

    sosiSleep(1);
    
	do
	{

        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {
                ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );

			    PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 |= (1<<6);
                dword = PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00;
                status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                    return (status);    
                }

            }
        }

        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {
                ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );

                status = bsdiMemoryRead32 (PtrDevice, offset + 0xE7C, &regVal, sizeof (U32));
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                    return (status);    
                }

                PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCountLo = regVal; 
            
                status = bsdiMemoryRead32 (PtrDevice, offset + 0xE80, &regVal, sizeof (U32));
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                    return (status);    
                }

                PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCountHi = regVal; 
            
                status = bsdiMemoryRead32 (PtrDevice, offset + 0xE84, &regVal, sizeof (U32));
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                    return (status);    
                }

                PtrPortSwEyeStatus->PortSwEye[portIndex].ErrorBitCount = regVal; 

            }
        }

        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {
                ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );

			    PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 &= ~(1<<6);
                dword = PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00;
                status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
                if (status != SCRUTINY_STATUS_SUCCESS)
                {
                    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                    return (status);    
                }

            }
        }



        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {
                PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount = (PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCountHi << 16) | PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCountLo;
            }
        }

        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {

			    if(PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount == PtrPortSwEyeStatus->PortSwEye[portIndex].LastSampleCount)
			    {
                    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PollingDone = 1;
			    }
			    else if((PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount > minSampleCount) && (PtrPortSwEyeStatus->PortSwEye[portIndex].ErrorBitCount >= 33))
			    {
                    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PollingDone = 1;
			    }
			    else if(PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount == 0)
			    {
				    gPtrLoggerSwitch->logiDebug("Sample counter one lane %d does not run", portIndex);
                    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PollingDone = 1;
			    }
			    else if(PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount > requestSampleCount)
			    {
                    PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PollingDone = 1;
			    }
			    else
                {
                    PtrPortSwEyeStatus->PortSwEye[portIndex].LastSampleCount = PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount;
                }

            }
        }

        PtrPortSwEyeStatus->GlobalPollingDone = 1;
        for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
        {
            if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
            {
                if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PollingDone == 0)
                {
                    PtrPortSwEyeStatus->GlobalPollingDone = 0;
                    break;
                }
        
            }
        }
        if (PtrPortSwEyeStatus->GlobalPollingDone == 1)
        {
            break;
        }
        
        sosiSleep(1);
	}while(1);


    for (portIndex = StartPort; portIndex < (StartPort + NumberOfPort); portIndex++)
    {
        if (PtrPortSwEyeStatus->PortSwEye[portIndex].Flag.PortActive)
        {
            ssiGetSerdesPortRegBaseOffset (portIndex, &offset,  &stn,  &lane );
		    PtrPortSwEyeStatus->PortSwEye[portIndex].BitCount = PtrPortSwEyeStatus->PortSwEye[portIndex].SampleCount * 16;
	
		    if(PtrPortSwEyeStatus->PortSwEye[portIndex].BitCount != 0)
		    {
			    PtrPortSwEyeStatus->PortSwEye[portIndex].BER = (float)(PtrPortSwEyeStatus->PortSwEye[portIndex].ErrorBitCount) / (float)(PtrPortSwEyeStatus->PortSwEye[portIndex].BitCount);
		    }
		    else 
		    {
			    gPtrLoggerSwitch->logiDebug("Sample counter for lane %d = 0\n", portIndex);
                status = SCRUTINY_STATUS_FAILED;
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                return (status); 
		    }

		    PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00 &= ~(1<<4);	//disable the counter
            dword = PtrPortSwEyeStatus->PortSwEye[portIndex].TempRegE00;
            status= bsdiMemoryWrite32 (PtrDevice, offset + 0xE00, &dword);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
                return (status);    
            }

        }
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiSoftEyeGetBer  (Status = %x) ",status);
    return (status); 
}

/**
 *
 *  @method  ssiGetSerdesPortRegBaseOffset ()
 *
 *  @param   PortIndex                  port index
 *
 *  @param   PtrOffset                  output register offset address.
 *
 *  @param   PtrStation                 station number 
 *
 *  @param   PtrLaneInStation           lane number in the station 
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *
 *  @brief                           This method get the serdes register offset address by port index . 
 *                                   
 *
 */


SCRUTINY_STATUS ssiGetSerdesPortRegBaseOffset (__IN__ U32 PortIndex, __OUT__ PU32 PtrOffset,  __OUT__ PU32 PtrStation,  __OUT__ PU32 PtrLaneInStation )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     stn;
	U32     lane;
    U32     offset;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiGetSerdesPortRegBaseOffset (PortIndex =%x, PtrOffset=%x, PtrStation=%x, PtrLaneInStation=%x)", PortIndex, PtrOffset != NULL, PtrStation != NULL, PtrLaneInStation != NULL );

	stn = PortIndex / 16;
    lane = PortIndex % 16; 	
	offset = 0x70000000 + stn * 0x100000 + lane * 0x4000;

    if (PtrOffset)
    {
        (*PtrOffset) = offset;
    }
    if (PtrStation)
    {
        (*PtrStation) = stn;
    }
    if (PtrLaneInStation)
    {
        (*PtrLaneInStation) = lane;
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiGetSerdesPortRegBaseOffset  (Status = %x) ",status);
    return (status); 

}

/**
 *
 *  @method  ssiGetCfgSpaceStationLevelRegBaseOffset ()
 *
 *  @param   PortIndex                  port index
 *
 *  @param   PtrOffset                  output register offset address.
 *
 *  @param   PtrStation                 station number 
 *
 *  @param   PtrLaneInStation           lane number in the station 
 *

    @return  SCRUTINY_STATUS    SCRUTINY_STATUS_SUCCESS when success 
 *
 *  @brief                           This method get the config space register station level offset address by port index . 
 *                                   
 *
 */

SCRUTINY_STATUS ssiGetCfgSpaceStationLevelRegBaseOffset (__IN__ U32 PortIndex, __OUT__ PU32 PtrOffset,  __OUT__ PU32 PtrStation,  __OUT__ PU32 PtrLaneInStation )
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32     stn;
	U32     lane;
    U32     offset;

    gPtrLoggerSwitch->logiFunctionEntry ("ssiGetCfgSpaceStationLevelRegBaseOffset (PortIndex =%x, PtrOffset=%x, PtrStation=%x, PtrLaneInStation=%x)", PortIndex, PtrOffset != NULL, PtrStation != NULL, PtrLaneInStation != NULL );

	stn = PortIndex / 16;
    lane = PortIndex % 16; 	
    //The register is station level based ,so no need port index when calculating offset
	offset = 0x60800000 + stn * 0x10000 ;

    if (PtrOffset)
    {
        (*PtrOffset) = offset;
    }
    if (PtrStation)
    {
        (*PtrStation) = stn;
    }
    if (PtrLaneInStation)
    {
        (*PtrLaneInStation) = lane;
    }

    gPtrLoggerSwitch->logiFunctionExit ("ssiGetCfgSpaceStationLevelRegBaseOffset  (Status = %x) ",status);
    return (status); 

}
