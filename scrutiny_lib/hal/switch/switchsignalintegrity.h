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


#ifndef __SWITCH_SIGNAL_INTEGRITY__H__
#define __SWITCH_SIGNAL_INTEGRITY__H__

// move below definition from scrutinydef.h to here
//because the detail is not suitable to expose to customer.

/********************
Scratch[0]<-->old_e74;
Scratch[1]<-->old_344;
Scratch[2]<-->old_398;
Scratch[3]<-->old_e70;
Scratch[4]<-->old_c8;
Scratch[5]<-->current_phase;
Scratch[6]<-->sample_count_lo
Scratch[7]<-->sample_count_hi
Scratch[8]<-->sample_count
Scratch[9]<-->last_sample_count
Scratch[10]<-->error_bit_count
Scratch[11]<-->value[i],<-->TempRegE00
Scratch[12]<-->bit_count

ScratchFloat[0]<-->BER
********************/

typedef struct _PCIE_ONE_PORT_SW_EYE_FLAG
{
    U32             PortActive       :1;   /* 0x01 Port Active Bit Field */
    U32             PollingDone      :1;    /*check if the port finish hardware eye test.*/
    U32             CommandReceived  :1;
    U32             Granted          :1;
    U32             Reserved28       :28;   /*  reserved bits   */

} PCIE_ONE_PORT_SW_EYE_FLAG, *PTR_PCIE_ONE_PORT_SW_EYE_FLAG;


typedef struct _PCIE_ONE_PORT_SW_EYE
{
	 PCIE_ONE_PORT_SW_EYE_FLAG  Flag;
    
    U32 OldRegE74;
    U32 OldReg344;
    U32 OldReg398;
    U32 OldRegE70;
    U32 OldRegC8;
    S32 CurrentPhase;
    U32 SampleCountLo;
    U32 SampleCountHi;
    U32 SampleCount;
    U32 LastSampleCount;
    U32 ErrorBitCount;
    U32 TempRegE00;//used in ssiSoftEyeGetBer, as value[16]
    U32 BitCount;
    double BER;
    double                     TripleArrayBuffer[128][64][3]; // provide the buffer for internal calculation use.

} PCIE_ONE_PORT_SW_EYE, *PTR_PCIE_ONE_PORT_SW_EYE;

typedef struct _SCRUTINY_SWITCH_PORT_SW_EYE
{
    U32                                     ToTalPhyNum;
    U32                                     TotalValidEntry;
    U32                                     GlobalPollingDone; //temperally save polling result.
    PCIE_ONE_PORT_SW_EYE                    PortSwEye[ATLAS_PMG_MAX_PHYS];
} SCRUTINY_SWITCH_PORT_SW_EYE, *PTR_SCRUTINY_SWITCH_PORT_SW_EYE;



SCRUTINY_STATUS ssiRxEqStatusOnePort( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS PtrPortRxEqStatus );
SCRUTINY_STATUS ssiRxEqStatus(__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS PtrPortRxEqStatus);
SCRUTINY_STATUS ssiTxCoeff( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF PtrPortTxCoeffStatus );
SCRUTINY_STATUS ssiTxCoeffOnePort( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_TX_COEFF PtrPortTxCoeffStatus );
SCRUTINY_STATUS ssiHardwareEyeOnePortStart( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus );
SCRUTINY_STATUS ssiHardwareEyeOnePortPoll( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus );
SCRUTINY_STATUS ssiHardwareEyeOnePortGet( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus );
SCRUTINY_STATUS ssiHardwareEyeOnePortClean ( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex );
SCRUTINY_STATUS ssiHardwareEyeStart( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus );
SCRUTINY_STATUS ssiHardwareEyePoll( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus);
SCRUTINY_STATUS ssiHardwareEyeGet( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus);
SCRUTINY_STATUS ssiHardwareEyeClean( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_HW_EYE PtrPortHwEyeStatus);

SCRUTINY_STATUS ssiSoftwareEye( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort );
SCRUTINY_STATUS ssiSoftwareEyeWithBuf( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort, __OUT__  PTR_SCRUTINY_SWITCH_PORT_SW_EYE PtrPortSwEyeStatus );
SCRUTINY_STATUS ssiSoftEyeSerdesInit(__IN__ PTR_SCRUTINY_DEVICE PtrDevice, U32 PortIndex, __OUT__  PTR_SCRUTINY_SWITCH_PORT_SW_EYE PtrPortSwEyeStatus );
SCRUTINY_STATUS ssiSoftEyeVertAdjust( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, U32 PortIndex, U32 Voffset);
SCRUTINY_STATUS ssiSoftEyePhaseAdjust( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, U32 PortIndex, S32 CurrentPhase, S32 NewPhase);
SCRUTINY_STATUS ssiSoftEyeGetBer ( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 StartPort, __IN__ U32 NumberOfPort,  __IN__ U32 RequestBitCount, __OUT__  PTR_SCRUTINY_SWITCH_PORT_SW_EYE PtrPortSwEyeStatus );
SCRUTINY_STATUS ssiGetSerdesPortRegBaseOffset (__IN__ U32 PortIndex, __OUT__ PU32 PtrOffset,  __OUT__ PU32 PtrStation,  __OUT__ PU32 PtrLaneInStation );
SCRUTINY_STATUS ssiGetCfgSpaceStationLevelRegBaseOffset (__IN__ U32 PortIndex, __OUT__ PU32 PtrOffset,  __OUT__ PU32 PtrStation,  __OUT__ PU32 PtrLaneInStation );


#endif /* __SWITCH_SIGNAL_INTEGRITY__H__ */