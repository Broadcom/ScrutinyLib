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
#include "switchlanemargin.h"


extern SCRUTINY_STATUS spcGetPciPortNegLinkWidth (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32					PtrNegLinkWidth
);

/**
 *
 * @method  slmRegRead()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   Address        register Address 
 * 
 * @param   PtrValue       pointer to the register value
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   read the internal switch registers
 *
 *
 */
SCRUTINY_STATUS slmRegRead (
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__  U32  Address, 
    __OUT__ PU32 PtrValue
)
{
    return (bsdiMemoryRead32 (PtrDevice, Address, PtrValue, sizeof (U32)));
}

/**
 *
 * @method  slmRegWrite()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   Address        register Address 
 * 
 * @param   PtrValue       written value
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   write the internal switch registers
 *
 *
 */
SCRUTINY_STATUS slmRegWrite ( 
    __IN__  PTR_SCRUTINY_DEVICE PtrDevice, 
    __IN__  U32 Address, 
    __IN__  U32 Value
)
{
    return (bsdiMemoryWrite32 (PtrDevice, Address, &Value));
}

/**
 *
 * @method  slmGetReceiveNumber()
 *
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   PortNum        specific port 
 * 
 * @param   PtrReceiveNumber    returned receive number
 * 
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   return LM receive number based on the port type (USP or DSP)
 *
 *
 */
SCRUTINY_STATUS slmGetLmReceiveNumber (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32                    PtrReceiveNumber
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;
    U32 portType = 0xFF;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetLmReceiveNumber (PtrDevice=%x, PortNum=0x%x, PtrReceiveNumber=%x)", 
            PtrDevice != NULL, PortNum, PtrReceiveNumber != NULL);
    
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PCIE_CAPABILITY, &dword);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        *PtrReceiveNumber = 0xFF;
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLmReceiveNumber (status=0x%x)", status);
        return (status);
    }

    portType = ((dword & 0xF00000) >> 20);
    if (portType == 5)
    {
        /* upstream port */
        *PtrReceiveNumber = PCIE_REG_LM_RX_NUM_RX_F;
    }
    else if (portType == 6)
    {
        *PtrReceiveNumber = PCIE_REG_LM_RX_NUM_RX_A;
    }

    gPtrLoggerSwitch->logiFunctionExit ("slmGetLmReceiveNumber (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  slmGetPciPortLinkState()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrLinkState        returned data link layer link active state, link speed and link width
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   get the link active state, link speed and link width
 *
 *
 */
SCRUTINY_STATUS slmGetPciPortLinkState (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32					PtrLinkActive,
	__OUT__ PU32                    PtrLinkWidth,
	__OUT__ PU32                    PtrLinkSpeed
	)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetPciPortLinkState (PtrDevice=%x, PortNum=0x%x, PtrLinkActive=%x, PtrLinkWidth=%x, PtrLinkSpeed=%x,)", 
            PtrDevice != NULL, PortNum, PtrLinkActive != NULL, PtrLinkWidth != NULL, PtrLinkSpeed != NULL);


    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetPciPortLinkState (status=0x%x)", status);
        return (status);
    }

    *PtrLinkActive = ((dword & 0x20000000) >> 29);
    *PtrLinkSpeed = ((dword & 0xF0000) >> 16);
    *PtrLinkWidth = ((dword & 0x1F00000) >> 20);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmGetPciPortLinkState (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmSetLinkAspm()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   LinkPmCtrl          Link ASPM setting
 *
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   set Link ASPM and return the old value
 *
 *
 */
SCRUTINY_STATUS slmSetLinkAspm (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__IN__ U32                      LinkPmCtrl
)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("slmSetLinkAspm (PtrDevice=%x, PortNum=0x%x, LinkPmCtrl=%x)", 
            PtrDevice != NULL, PortNum, LinkPmCtrl);
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmSetLinkAspm (status=0x%x)", status);
        return (status);
    }

    dword = ((dword & ~0x3) | (LinkPmCtrl & 0x3));
    
    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, dword);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmSetLinkAspm (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmGetLinkAspm ()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrBakLinkPmCtrl    returned Original Link ASPM Setting
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   get Link ASPM and return the old value
 *
 *
 */
SCRUTINY_STATUS slmGetLinkAspm (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32					PtrBakLinkPmCtrl
)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetLinkAspm (PtrDevice=%x, PortNum=0x%x, PtrBakLinkPmCtrl=%x)", 
            PtrDevice != NULL, PortNum, PtrBakLinkPmCtrl != NULL );
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLinkAspm (status=0x%x)", status);
        return (status);
    }
    
    *PtrBakLinkPmCtrl = (dword & 0x3);    
    gPtrLoggerSwitch->logiFunctionExit ("slmGetLinkAspm (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  slmGetHwAutoSpeed()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrBakHwAutoSpeed   returned Original HW Auto Speed setting
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   get HW Auto Speed and return the old value
 *
 *
 */
SCRUTINY_STATUS slmGetHwAutoSpeed (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32					PtrBakHwAutoSpeed
)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetHwAutoSpeed (PtrDevice=%x, PortNum=0x%x, PtrBakHwAutoSpeed=%x)", 
            PtrDevice != NULL, PortNum, PtrBakHwAutoSpeed != NULL );
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_LINK_STS_CTRL_2, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetHwAutoSpeed (status=0x%x)", status);
        return (status);
    }
    
    *PtrBakHwAutoSpeed = ((dword & 0x20) >> 5);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmGetHwAutoSpeed (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmSetHwAutoSpeed()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   HwAutoSpeed         HW Auto Speed setting
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   set HW Auto Speed and return the old value
 *
 *
 */
SCRUTINY_STATUS slmSetHwAutoSpeed (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__IN__ U32                      HwAutoSpeed
)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("slmSetHwAutoSpeed (PtrDevice=%x, PortNum=0x%x, HwAutoSpeed=%x)", 
            PtrDevice != NULL, PortNum, HwAutoSpeed);
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_LINK_STS_CTRL_2, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmSetHwAutoSpeed (status=0x%x)", status);
        return (status);
    }

    dword = ((dword & ~0x20) | ((HwAutoSpeed & 0x1) << 5));
    
    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_LINK_STS_CTRL_2, dword);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmSetHwAutoSpeed (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  slmGetHwAutoWidth()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   PtrBakHwAutoWidth   returned Original HW Auto width setting
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   get HW Auto width and return the old value
 *
 *
 */
SCRUTINY_STATUS slmGetHwAutoWidth (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__OUT__ PU32					PtrBakHwAutoWidth
)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetHwAutoWidth (PtrDevice=%x, PortNum=0x%x, PtrBakHwAutoWidth=%x)", 
            PtrDevice != NULL, PortNum, PtrBakHwAutoWidth != NULL );
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetHwAutoWidth (status=0x%x)", status);
        return (status);
    }
    
    *PtrBakHwAutoWidth = ((dword & 0x200) >> 9);
    
    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, dword);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmGetHwAutoWidth (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmSetHwAutoWidth()
 *
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNum             specific port 
 *
 * @param   HwAutoWidth         HW Auto width setting
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   set HW Auto width and return the old value
 *
 *
 */
SCRUTINY_STATUS slmSetHwAutoWidth (
	__IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
	__IN__ U32						PortNum,
	__IN__ U32                      HwAutoWidth
)
{
	SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
	U32 dword = 0;

    gPtrLoggerSwitch->logiFunctionEntry ("slmSetHwAutoWidth (PtrDevice=%x, PortNum=0x%x, HwAutoWidth=%x )", 
            PtrDevice != NULL, PortNum, HwAutoWidth );
    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, &dword);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmSetHwAutoWidth (status=0x%x)", status);
        return (status);
    }

    dword = ((dword & ~0x200) | ((HwAutoWidth & 0x1) << 9));
    
    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNum, ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS, dword);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmSetHwAutoWidth (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  invLmNoCommand()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'No Command' margin command to the target
 *
 */

SCRUTINY_STATUS slmNoCommandPerLane( 
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber
)
{
    SCRUTINY_STATUS status = SCRUTINY_STATUS_SUCCESS;
    U32 valueWrite, valueRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmNoCommandPerLane (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber);

    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_NO_CMD,
                              0,
                              PCIE_REG_LM_MRGN_CMD_NO_CMD);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);
    
    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmNoCommandPerLane (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);

    valueRead = ( valueRead & 0xFFFF0000) >> 16 ;

    if ( valueRead != ( valueWrite & 0xFFFF) )
    {
        gPtrLoggerSwitch->logiDebug ("slmNoCommandPerLane command mismatch: valueRead=%x valueWrite=%x", valueRead, valueWrite);
        gPtrLoggerSwitch->logiFunctionExit ("slmNoCommandPerLane (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    gPtrLoggerSwitch->logiFunctionExit ("slmNoCommandPerLane (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmGetReportCapabilities()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   PtrLaneMarginCtrlCap  returned margin control capabilities   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Report Report Margin Control Capabilities' margin command to the target
 *
 */

SCRUTINY_STATUS slmGetReportCapabilities (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      ReceiverNumber,
    __OUT__ PU32                    PtrLaneMarginCtrlCap 
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 capabilities = 0;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetReportCapabilities (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x, PtrLaneMarginCtrlCap=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber, (PtrLaneMarginCtrlCap == NULL));

    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_RPT_MARGIN_CTRL_CAP);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportCapabilities (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = ( valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    
    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT) |
        (receiverNumberRead != ReceiverNumber))
    {
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x", 
            status, marginTypeRead, receiverNumberRead);
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportCapabilities (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
    capabilities = ((valueRead >> 8) & 0x1F);
    *PtrLaneMarginCtrlCap = capabilities;

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmGetReportCapabilities (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmGetReportNumTimingSteps()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   PtrNumVoltageSteps  returned Number of Timing Steps for the lane   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Report Num Timing Steps' margin command to the target
 *
 */

SCRUTINY_STATUS slmGetReportNumTimingSteps (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      ReceiverNumber,
    __OUT__ PU32                    PtrNumTimingSteps 
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 numTimingSteps = 0;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetReportNumTimingSteps (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x, PtrNumTimingSteps=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber, (PtrNumTimingSteps == NULL));

    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_RPT_MNUMTIMINGSTEPS);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportNumTimingSteps (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = ( valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    
    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT) |
        (receiverNumberRead != ReceiverNumber))
    {
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x", 
            status, marginTypeRead, receiverNumberRead);
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportNumTimingSteps (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
    numTimingSteps = ((valueRead >> 8) & 0x3F);
    *PtrNumTimingSteps = numTimingSteps;

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmGetReportNumTimingSteps (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmGetReportNumVoltageSteps()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   PtrNumVoltageSteps  returned Number of Voltage Steps for the lane   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Report Num Voltage Steps' margin command to the target
 *
 */

SCRUTINY_STATUS slmGetReportNumVoltageSteps (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      ReceiverNumber,
    __OUT__ PU32                    PtrNumVoltageSteps 
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 numVoltageSteps = 0;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetReportNumVoltageSteps (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x, PtrNumVoltageSteps=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber, (PtrNumVoltageSteps == NULL));

    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_RPT_MNUMVOLTAGESTEPS);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportNumVoltageSteps (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = (valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    
    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT) |
        (receiverNumberRead != ReceiverNumber))
    {
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x", 
            status, marginTypeRead, receiverNumberRead);
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportNumVoltageSteps (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
    numVoltageSteps = ((valueRead >> 8) & 0x7F);
    *PtrNumVoltageSteps = numVoltageSteps;

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmGetReportNumVoltageSteps (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmGetReportMaxTimingOffset()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   PtrMaxTimingOffset  returned Number of Max Timing Offset for the lane   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Report Max Timing Offset' margin command to the target
 *
 */

SCRUTINY_STATUS slmGetReportMaxTimingOffset (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      ReceiverNumber,
    __OUT__ PU32                    PtrMaxTimingOffset 
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 maxTimingOffset = 0;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetReportMaxTimingOffset (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x, PtrMaxTimingOffset=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber, (PtrMaxTimingOffset == NULL));

    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_RPT_MMAXTIMINGOFFSET);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportMaxTimingOffset (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = (valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    
    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT) |
        (receiverNumberRead != ReceiverNumber))
    {
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x", 
            status, marginTypeRead, receiverNumberRead);
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportMaxTimingOffset (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
    maxTimingOffset = ((valueRead >> 8) & 0x7F);
    *PtrMaxTimingOffset = maxTimingOffset;

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmGetReportMaxTimingOffset (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmGetReportMaxVoltageOffset()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   PtrMaxTimingOffset  returned Number of Max Voltage Offset for the lane   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Report Max Voltage Offset' margin command to the target
 *
 */

SCRUTINY_STATUS slmGetReportMaxVoltageOffset (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      ReceiverNumber,
    __OUT__ PU32                    PtrMaxVoltageOffset 
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 maxVoltageOffset = 0;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetReportMaxVoltageOffset (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x, PtrMaxVoltageOffset=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber, (PtrMaxVoltageOffset == NULL));

    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_RPT_MMAXVOLTAGEOFFSET);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportMaxVoltageOffset (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = (valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    
    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT) |
        (receiverNumberRead != ReceiverNumber))
    {
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x", 
            status, marginTypeRead, receiverNumberRead);
        gPtrLoggerSwitch->logiFunctionExit ("slmGetReportMaxVoltageOffset (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
    maxVoltageOffset = ((valueRead >> 8) & 0x7F);
    *PtrMaxVoltageOffset = maxVoltageOffset;

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmGetReportMaxVoltageOffset (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  slmGoToNormalSettings()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Go to Normal Settings' margin command to the target
 *
 */

SCRUTINY_STATUS slmGoToNormalSettings (
    __IN__ PTR_SCRUTINY_DEVICE      PtrDevice,
    __IN__ U32                      PortNumber,
    __IN__ U32                      LaneNumber,
    __IN__ U32                      ReceiverNumber
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead, response;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGoToNormalSettings (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber);


    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_SET_PARAMS,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_GO_TO_NORMAL_SETTINGS);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGoToNormalSettings (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = (valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    response = (U8)((valueRead >> 8) & 0xFF);

    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_SET_PARAMS) |
        (receiverNumberRead != ReceiverNumber) |
        (response != PCIE_REG_LM_MRGN_CMD_GO_TO_NORMAL_SETTINGS))
    {
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x, response=%x", 
            status, marginTypeRead, receiverNumberRead, response);
        gPtrLoggerSwitch->logiFunctionExit ("slmClearErrorLog (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmClearErrorLog (status=0x%x)", status);
    return (status);

}

/**
 *
 * @method  slmSetErrorCountLimit()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   ErrorCountLimit     Error count limit to be set to the target   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Set Error Count Limit' margin command to the target
 *
 */

SCRUTINY_STATUS slmSetErrorCountLimit (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      ReceiverNumber,
	__IN__ U32                      ErrorCountLimit   
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead, errorCountLimitRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmSetErrorCountLimit (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x, ErrorCountLimit=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber, ErrorCountLimit);


    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_SET_PARAMS,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_SET_ERR_CNT_LIMIT(ErrorCountLimit));

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmSetErrorCountLimit (status=0x%x)", status);
        return (status);
    }

    sosiSleep (100);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = (valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    errorCountLimitRead = (U8)((valueRead >> 8) & 0x3F);
    
    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_SET_PARAMS) |
        (receiverNumberRead != ReceiverNumber) |
        (errorCountLimitRead != ErrorCountLimit))
    {
    	        // ???????????????????
        slmGoToNormalSettings (PtrDevice, PortNumber, LaneNumber, 6);
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x, errorCountLimitRead=%x", 
            status, marginTypeRead, receiverNumberRead, errorCountLimitRead);
        gPtrLoggerSwitch->logiFunctionExit ("slmSetErrorCountLimit (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmSetErrorCountLimit (status=0x%x)", status);
    return (status);
}


/**
 *
 * @method  slmSetErrorCountLimit()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   ErrorCountLimit     Error count limit to be set to the target   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Set Error Count Limit' margin command to the target
 *
 */

SCRUTINY_STATUS slmClearErrorLog(
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      ReceiverNumber
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 valueWrite, valueRead;
    U8  marginTypeRead, receiverNumberRead, response;

    gPtrLoggerSwitch->logiFunctionEntry ("slmClearErrorLog (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, ReceiverNumber=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, ReceiverNumber);


    valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_SET_PARAMS,
                              ReceiverNumber,
                              PCIE_REG_LM_MRGN_CMD_CLEAR_ERROR_LOG);

    status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

    if( status != SCRUTINY_STATUS_SUCCESS )
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmClearErrorLog (status=0x%x)", status);
        return (status);
    }

    sosiSleep (10);

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);
    
    valueRead = (valueRead & 0xFFFF0000) >> 16 ;
    marginTypeRead = (U8)((valueRead >> 3) & 0x7);
    receiverNumberRead = (U8)(valueRead & 0x7);
    response = (U8)((valueRead >> 8) & 0xFF);
    
    if ((status != SCRUTINY_STATUS_SUCCESS) |
        (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_SET_PARAMS) |
        (receiverNumberRead != ReceiverNumber) |
        (response != PCIE_REG_LM_MRGN_CMD_CLEAR_ERROR_LOG))
    {
        gPtrLoggerSwitch->logiDebug ("status=%x, marginTypeRead=%x, receiverNumberRead=%x, response=%x", 
            status, marginTypeRead, receiverNumberRead, response);
        gPtrLoggerSwitch->logiFunctionExit ("slmClearErrorLog (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);

    gPtrLoggerSwitch->logiFunctionExit ("slmClearErrorLog (status=0x%x)", status);
    return (status);
}



/**
 *
 * @method  slmVoltageMargining()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 * 
 * @param   NumSteps            Number of steps to run
 *
 * @param   PtrSteps            number of steps has run before stops 
 *
 * @param   PtrVoltageMarginStatus Status returned from the target for the lane margin command
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Step Margin to Voltage offset' margin command to the target
 *
 */

SCRUTINY_STATUS slmVoltageMargining (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      StepDirection,
	__IN__ U32                      NumSteps,
	__OUT__ PU32                    PtrSteps,
	__OUT__ PU32                    PtrVoltageMarginStatus
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 numSteps = 0, steps = 0;
    U32 valueWrite = 0, valueRead;
    U32 stepMargingExecStatus = 0;
    U32 receiverNumber;
    U8  marginTypeRead, receiverNumberRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmVoltageMargining (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, StepDirection=%x, NumSteps=%x, PtrSteps=%x, PtrVoltageMarginStatus=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, StepDirection, NumSteps, (PtrSteps == NULL), (PtrVoltageMarginStatus == NULL));

    status = slmGetLmReceiveNumber (PtrDevice, PortNumber, &receiverNumber);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmVoltageMargining (status=0x%x)", status);
        return (status);
    }

    status = slmGetReportNumVoltageSteps (PtrDevice, PortNumber, LaneNumber, receiverNumber, &valueRead);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmVoltageMargining (status=0x%x)", status);
        return (status);
    }
   
    numSteps = (NumSteps > valueRead) ? valueRead : NumSteps;

    *PtrSteps = 0;
    *PtrVoltageMarginStatus = 0;

    while (steps < numSteps) 
    {
        if (StepDirection == STEP_TO_UP)
        {
            valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_VOLTAGE,
                                      receiverNumber, 127 & steps );
        }
        else if (StepDirection == STEP_TO_DOWN) 
        {
            valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_VOLTAGE,
                                      receiverNumber, (128 | (127 & steps)));
        }

        status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

        sosiSleep (10);

        if( status != SCRUTINY_STATUS_SUCCESS )
        {
            gPtrLoggerSwitch->logiFunctionExit ("slmVoltageMargining (status=0x%x)", status);
            return (status);
        }

        sosiSleep (PCIE_LANE_MARGIN_DWELL_TIME);

        status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);

        marginTypeRead = (U8)((valueRead >> 3) & 0x7);
        receiverNumberRead = (U8)(valueRead & 0x7);

        if( (status != SCRUTINY_STATUS_SUCCESS) |
            (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_VOLTAGE) |
            (receiverNumberRead != receiverNumber))
        {
            gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
            return (status);
        }
        
        //printf("Lane %x Voltage Margining - Steps %x, valueRead %x\n", LaneNumber, steps, valueRead);
        //Read the Lane Margining status Register
        stepMargingExecStatus =  ( valueRead & 0xFFFF0000 ) >> 16 ;
        stepMargingExecStatus = stepMargingExecStatus >> 8;      
        *PtrVoltageMarginStatus = stepMargingExecStatus;
        
        if ( (stepMargingExecStatus & 0xC0) == 0xC0) // TODO: Move it to Macro for Readability
        {
            gPtrLoggerSwitch->logiDebug ("Step Margin Execution Status is NAK Received. Unsupported Lane Margining Command was issued");
            break;
        }
        else if  ((stepMargingExecStatus & 0xC0) == 0x80)
        {
            status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
                return (status);
            }

            status = slmClearErrorLog (PtrDevice, PortNumber, LaneNumber, receiverNumber);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
                return (status);
            }

            steps = steps + 1;
            continue;
        } 
        else if ((stepMargingExecStatus & 0xC0) == 0x40) 
        {
            gPtrLoggerSwitch->logiDebug ("Step Margin Execution Status is Set up for a Margin in Progress. ");
            break;
        }

        else if ((stepMargingExecStatus & 0xC0) == 0x00)//00b
        {
            gPtrLoggerSwitch->logiDebug ("Step Margin Execution Status is Too Many errors. ");
            gPtrLoggerSwitch->logiDebug ("Error Count %d is more than the User Set Error Count", (stepMargingExecStatus & 0x1F));
            break;
        }
        
    }
            
    *PtrSteps = ((steps == 0) ? 0xFF : (steps - 1));

    slmClearErrorLog (PtrDevice, PortNumber, LaneNumber, receiverNumber);
    slmGoToNormalSettings (PtrDevice, PortNumber, LaneNumber, receiverNumber);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
    return (status);

}

/**
 *
 * @method  slmTimeMargining()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 * 
 * @param   NumSteps            Number of steps to run
 *
 * @param   PtrSteps            number of steps has run before stops 
 *
 * @param   PtrTimeMarginStatus Status returned from the target for the lane margin command
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   issue 'Step Margin to timing offset' margin command to the target
 *
 */

SCRUTINY_STATUS slmTimeMargining (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__IN__ U32                      StepDirection,
	__IN__ U32                      NumSteps,
	__OUT__ PU32                    PtrSteps,
	__OUT__ PU32                    PtrTimeMarginStatus
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32 numSteps = 0, steps = 0;
    U32 valueWrite = 0, valueRead;
    U32 stepMargingExecStatus = 0;
    U32 receiverNumber;
    U8  marginTypeRead, receiverNumberRead;

    gPtrLoggerSwitch->logiFunctionEntry ("slmTimeMargining (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, StepDirection=%x, NumSteps=%x, PtrSteps=%x, PtrTimeMarginStatus=%x)", 
            PtrDevice != NULL, PortNumber, LaneNumber, StepDirection, NumSteps, (PtrSteps == NULL), (PtrTimeMarginStatus == NULL));

    status = slmGetLmReceiveNumber (PtrDevice, PortNumber, &receiverNumber);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
        return (status);
    }

    status = slmGetReportNumTimingSteps (PtrDevice, PortNumber, LaneNumber, receiverNumber, &valueRead);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
        return (status);
    }
   
    numSteps = (NumSteps > valueRead) ? valueRead : NumSteps;

    *PtrSteps = 0;
    *PtrTimeMarginStatus = 0;

    while (steps < numSteps) 
    {
        if (StepDirection == STEP_TO_RIGHT)
        {
            valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_TIMING,
                                      receiverNumber, 63 & steps );
        }
        else if (StepDirection == STEP_TO_LEFT) 
        {
            valueWrite = PCIE_LM_COMMANDS( PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_TIMING,
                                      receiverNumber, (64 | (63 & steps)));
        }

        status = atlasPCIeConfigurationSpaceWrite (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), valueWrite);

        sosiSleep (10);

        if( status != SCRUTINY_STATUS_SUCCESS )
        {
        	gPtrLoggerSwitch->logiDebug ("Step Margin - atlasPCIeConfigurationSpaceWrite");
            gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
            return (status);
        }

        sosiSleep (PCIE_LANE_MARGIN_DWELL_TIME);

        status = atlasPCIeConfigurationSpaceRead (PtrDevice, PortNumber, 
                ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS + PCIE_LANE_REGISTER_OFFSET(LaneNumber), &valueRead);

        marginTypeRead = (U8)((valueRead >> 3) & 0x7);
        receiverNumberRead = (U8)(valueRead & 0x7);

        if( (status != SCRUTINY_STATUS_SUCCESS) |
            (marginTypeRead != PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_TIMING) |
            (receiverNumberRead != receiverNumber))
        {
        	gPtrLoggerSwitch->logiDebug ("Step Margin - atlasPCIeConfigurationSpaceRead");
            gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
            return (status);
        }
        
        //printf("Lane %x Timing Margining - Steps %x, valueRead %x\n", LaneNumber, steps, valueRead);
        //Read the Lane Margining status Register
        stepMargingExecStatus =  ( valueRead & 0xFFFF0000 ) >> 16 ;
        stepMargingExecStatus = stepMargingExecStatus >> 8;      
        *PtrTimeMarginStatus = stepMargingExecStatus;
        
        if ( (stepMargingExecStatus & 0xC0) == 0xC0) 
        {
            gPtrLoggerSwitch->logiDebug ("Step Margin Execution Status is NAK Received. Unsupported Lane Margining Command was issued");
            break;
        }
        else if  ((stepMargingExecStatus & 0xC0) == 0x80)
        {
            status = slmNoCommandPerLane (PtrDevice, PortNumber, LaneNumber);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
            	gPtrLoggerSwitch->logiDebug ("Step Margin failed at slmNoCommandPerLane");
                gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
                return (status);
            }

            status = slmClearErrorLog (PtrDevice, PortNumber, LaneNumber, receiverNumber);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
            	gPtrLoggerSwitch->logiDebug ("Step Margin failed at slmClearErrorLog ");
                gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
                return (status);
            }

            steps = steps + 1;
            continue;
        } 
        else if ((stepMargingExecStatus & 0xC0) == 0x40) 
        {
            gPtrLoggerSwitch->logiDebug ("Step Margin Execution Status is Set up for a Margin in Progress. ");
            break;
        }

        else if ((stepMargingExecStatus & 0xC0) == 0x00)//00b
        {
            gPtrLoggerSwitch->logiDebug ("Step Margin Execution Status is Too Many errors. ");
            gPtrLoggerSwitch->logiDebug ("Error Count %d is more than the User Set Error Count", (stepMargingExecStatus & 0x1F));
            break;
        }
        
    }
            
    *PtrSteps = ((steps == 0) ? 0xFF : (steps - 1));

    slmClearErrorLog (PtrDevice, PortNumber, LaneNumber, receiverNumber);
    slmGoToNormalSettings (PtrDevice, PortNumber, LaneNumber, receiverNumber);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmTimeMargining (status=0x%x)", status);
    return (status);
}




/**
 *
 * @method  slmGetLaneMarginCapacities()
 *
 * @param   PtrDevice           pointer to the device
 *
 * @param   PortNumber          specific port 
 *
 * @param   LaneNumber          which lane in the specific port
 *
 * @param   ReceiverNumber      Lane receiver number
 *
 * @param   PtrMaxTimingOffset  returned Number of Max Voltage Offset for the lane   
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   interface to Get the Lane capacities
 *
 */

SCRUTINY_STATUS slmGetLaneMarginCapacities (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__OUT__ PU32                    PtrMarginControlCapabilities,
	__OUT__ PU32                    PtrNumSteps,
	__OUT__ PU32                    PtrMaxOffset
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32  receiverNumber;
    U32  linkWidth, linkState, linkSpeed;
    U32  readValue;

    gPtrLoggerSwitch->logiFunctionEntry ("slmGetLaneMarginCapacities (PtrDevice=%x, PortNumber=0x%x, LaneNumber=%x, PtrMarginControlCapabilities=%x, PtrNumSteps=%x,  PtrMaxOffset=%x)", 
                PtrDevice != NULL, PortNumber, LaneNumber, (PtrMarginControlCapabilities == NULL), (PtrNumSteps == NULL), (PtrMaxOffset == NULL));

    status = slmGetPciPortLinkState (PtrDevice, PortNumber, &linkState, &linkWidth, &linkSpeed);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
        return (status);
    }

    /* if Link is not at Gen 4, or the lane is not in the port */
    if ((linkSpeed != 4) || (linkWidth <= LaneNumber))
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", SCRUTINY_STATUS_INVALID_LM_REQUEST);
        return (SCRUTINY_STATUS_INVALID_LM_REQUEST);
    }

    status = slmGetLmReceiveNumber (PtrDevice, PortNumber, &receiverNumber);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
        return (status);
    }

    status = slmGetReportCapabilities (PtrDevice, PortNumber, LaneNumber, receiverNumber, &readValue);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
        return (status);
    }
    *PtrMarginControlCapabilities = readValue;

    status = slmGetReportNumVoltageSteps (PtrDevice, PortNumber, LaneNumber, receiverNumber, &readValue);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
        return (status);
    }
    *PtrNumSteps = ((readValue & 0xFFFF) << 16);

    status = slmGetReportNumTimingSteps (PtrDevice, PortNumber, LaneNumber, receiverNumber, &readValue);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
        return (status);
    }
    *PtrNumSteps |= (readValue & 0xFFFF);

    status = slmGetReportMaxVoltageOffset (PtrDevice, PortNumber, LaneNumber, receiverNumber, &readValue);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
        return (status);
    }
    *PtrMaxOffset = ((readValue & 0xFFFF) << 16);

    status = slmGetReportMaxTimingOffset (PtrDevice, PortNumber, LaneNumber, receiverNumber, &readValue);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
        return (status);
    }
    *PtrMaxOffset |= (readValue & 0xFFFF);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmGetLaneMarginCapacities (status=0x%x)", status);
    
    return (status);
}


/**
 *
 * @method  slmPerformLaneMargining()
 *
 * @param   PtrDevice                pointer to the device
 * @param   PtrProductHandle         Pointer to Adapter Handle
 * @param   PtrSwitchLaneMarginReq   PCIe Lane margin input parameters
 * @param   PtrSwitchLaneMarginResp  PCIe Lane margin outputs if the margin was executed. One response returns 
 *                                   margin result for one lane. The caller need to allocate buffer large enough for 
 *                                   the number of input lanes
 *
 * @return  SCRUTINY_STATUS     Indication Success or Fail          
 *
 * @brief   interface to perform lane margin
 *
 */

SCRUTINY_STATUS slmPerformLaneMargining    (
	__IN__ PTR_SCRUTINY_DEVICE                      PtrDevice, 
	__IN__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_REQUEST  PtrSwitchLaneMarginReq,
    __OUT__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE PtrSwitchLaneMarginResp 
)
{
    SCRUTINY_STATUS   status = SCRUTINY_STATUS_SUCCESS;
    U32  receiverNumber;
    U32  LaneNumber, lanes;
    U32  laneCapabilities;
    U32  marginExecStatus = 0, stepsPerf = 0;
    U32  bakLinkAspm, bakHwAutoSpeed, bakHwAutoWidth, marginPortStatus;
    U32  linkWidth, linkState, linkSpeed;
    PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE   ptrMarginResp;

    gPtrLoggerSwitch->logiFunctionEntry ("slmPerformLaneMargining (PtrDevice=%x, PtrSwitchLaneMarginReq=0x%x, PtrSwitchLaneMarginResp=%x)", 
                PtrDevice != NULL, (PtrSwitchLaneMarginReq == NULL), (PtrSwitchLaneMarginResp == NULL));

    status = slmGetPciPortLinkState (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, &linkState, &linkWidth, &linkSpeed);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    /* if Link is not at Gen 4 */
    if (linkSpeed != 4)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", SCRUTINY_STATUS_INVALID_LM_REQUEST);
        return (SCRUTINY_STATUS_INVALID_LM_REQUEST);
    }      

    status = atlasPCIeConfigurationSpaceRead (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, ATLAS_REGISTER_PMG_REG_LM_CAP_STATUS, &marginPortStatus);
    /* is the bit 'Margin Ready' set ? */
    if ((marginPortStatus & 0x10000) == 0)
    {
    	gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }
    
    status = slmGetLmReceiveNumber (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, &receiverNumber);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    status = slmGetLinkAspm (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, &bakLinkAspm);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    status = slmGetHwAutoSpeed (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, &bakHwAutoSpeed);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        slmSetLinkAspm (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakLinkAspm);
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    status = slmGetHwAutoWidth  (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, &bakHwAutoWidth);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        slmSetLinkAspm (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakLinkAspm);
        slmSetHwAutoSpeed (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakHwAutoSpeed);
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    status = slmSetLinkAspm (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, 0);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    status = slmSetHwAutoSpeed (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, 1);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        slmSetLinkAspm (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakLinkAspm);
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    status = slmSetHwAutoWidth (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, 1);
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        slmSetLinkAspm (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakLinkAspm);
        slmSetHwAutoSpeed (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakHwAutoSpeed);
        gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
        return (status);
    }

    lanes = PtrSwitchLaneMarginReq->Lanes;
    LaneNumber = 0;
    ptrMarginResp = PtrSwitchLaneMarginResp;
    
    while (lanes != 0)
    {
        if (lanes & (1 << LaneNumber))
        {
            if (linkWidth <= LaneNumber)
            {
                status = SCRUTINY_STATUS_INVALID_LM_REQUEST;
                break;
                //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", SCRUTINY_STATUS_INVALID_LM_REQUEST);
                //return (SCRUTINY_STATUS_INVALID_LM_REQUEST);
            }
            
            sosiMemSet (ptrMarginResp, 0xFF, sizeof (SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE));
            
            gPtrLoggerSwitch->logiDebug ("Performing Lane Margining on port %x lane %x", PtrSwitchLaneMarginReq->SwitchPort, LaneNumber);
           	
            lanes = (lanes & ~(1 << LaneNumber));
            status = slmSetErrorCountLimit (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, 
                                            receiverNumber, PtrSwitchLaneMarginReq->ErrorCount);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiDebug ("Failed to set up Error Count Limit at lane %", LaneNumber);
                //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                //return (status);
                break;
            } 
            
            status = slmGetReportCapabilities (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, 
                                            receiverNumber, &laneCapabilities);
            if (status != SCRUTINY_STATUS_SUCCESS)
            {
                gPtrLoggerSwitch->logiDebug ("Failed to get Report Capabilities at lane %", LaneNumber);
                //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                //return (status);
                break;
            } 

            /*************** Timing Margining *********************************/
            if (laneCapabilities & PCIE_REG_LM_MRGN_RSP_RMCC_MINDLRTIMING_MASK)
            {

                gPtrLoggerSwitch->logiDebug ("Starting Time Margining at Right at Lane %d .....", LaneNumber);
                status = slmTimeMargining (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, STEP_TO_RIGHT, 
                    PtrSwitchLaneMarginReq->NumTimeSteps, &stepsPerf, &marginExecStatus);
                if (status != SCRUTINY_STATUS_SUCCESS) 
                {
                    //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                    //return (status);
                    break;
                }
                ptrMarginResp->RightTimeMarginStep = stepsPerf;
                ptrMarginResp->RightTimeMarginStatus = marginExecStatus;


                gPtrLoggerSwitch->logiDebug ("Starting Time Margining at Left at Lane %d .....", LaneNumber);
                status = slmTimeMargining (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, STEP_TO_LEFT, 
                    PtrSwitchLaneMarginReq->NumTimeSteps, &stepsPerf, &marginExecStatus);
                if (status != SCRUTINY_STATUS_SUCCESS) 
                {
                    //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                    //return (status);
                    break;
                }
                ptrMarginResp->LeftTimeMarginStep = stepsPerf;
                ptrMarginResp->LeftTimeMarginStatus = marginExecStatus;

            }
            else
            {
                gPtrLoggerSwitch->logiDebug ("Starting Time Margining at Lane %d .....", LaneNumber);
                status = slmTimeMargining (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, STEP_TO_RIGHT, 
                    PtrSwitchLaneMarginReq->NumTimeSteps, &stepsPerf, &marginExecStatus);
                if (status != SCRUTINY_STATUS_SUCCESS) 
                {
                    break;
                    //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                    //return (status);
                }
                ptrMarginResp->RightTimeMarginStep = stepsPerf;
                ptrMarginResp->RightTimeMarginStatus = marginExecStatus;
            }

            /************ Voltage Step Margining *************************************/
            if (laneCapabilities & SCRUTINY_PCIE_REG_LM_MRGN_RSP_RMCC_MVOLTAGESUPP)
            {
                if (laneCapabilities & PCIE_REG_LM_MRGN_RSP_RMCC_MINDUDVOLTAGE_MASK)
                {

                    gPtrLoggerSwitch->logiDebug ("Starting Voltage Margining at Up at Lane %d .....", LaneNumber);
                    status = slmVoltageMargining (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, STEP_TO_UP, 
                        PtrSwitchLaneMarginReq->NumVoltageSteps, &stepsPerf, &marginExecStatus);
                    if (status != SCRUTINY_STATUS_SUCCESS) 
                    {
                        break;
                        //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                        //return (status);
                    }
                    ptrMarginResp->UpVoltageMarginStep = stepsPerf;
                    ptrMarginResp->UpVoltageMarginStatus = marginExecStatus;


                    gPtrLoggerSwitch->logiDebug ("Starting Voltage Margining at Down at Lane %d .....", LaneNumber);
                    status = slmVoltageMargining (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, STEP_TO_DOWN, 
                        PtrSwitchLaneMarginReq->NumVoltageSteps, &stepsPerf, &marginExecStatus);
                    if (status != SCRUTINY_STATUS_SUCCESS) 
                    {
                        break;
                        //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                        //return (status);
                    }
                    ptrMarginResp->DownVoltageMarginStep = stepsPerf;
                    ptrMarginResp->DownVoltageMarginStatus = marginExecStatus;

                }
                else
                {
                    gPtrLoggerSwitch->logiDebug ("Starting Voltage Margining at Lane %d .....", LaneNumber);
                    status = slmVoltageMargining (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, LaneNumber, STEP_TO_UP, 
                        PtrSwitchLaneMarginReq->NumVoltageSteps, &stepsPerf, &marginExecStatus);
                    if (status != SCRUTINY_STATUS_SUCCESS) 
                    {
                        break;
                        //gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);
                        //return (status);
                    }
                    ptrMarginResp->UpVoltageMarginStep = stepsPerf;
                    ptrMarginResp->UpVoltageMarginStatus = marginExecStatus;
                }
            }

            ptrMarginResp++;
            LaneNumber++;
        }
    }

    slmSetLinkAspm (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakLinkAspm);
    slmSetHwAutoSpeed (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakHwAutoSpeed);
    slmSetHwAutoWidth (PtrDevice, PtrSwitchLaneMarginReq->SwitchPort, bakHwAutoWidth);
    
    gPtrLoggerSwitch->logiFunctionExit ("slmPerformLaneMargining (status=0x%x)", status);            
    return (status);            
}


