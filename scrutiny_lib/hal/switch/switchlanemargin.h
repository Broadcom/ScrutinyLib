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

#ifndef __SWITCH_LANE_MARGIN__H__
#define __SWITCH_LANE_MARGIN__H__


/* atlas lane margin registers */
#define ATLAS_REGISTER_PMG_REG_PCIE_CAPABILITY          (0x68)
#define ATLAS_REGISTER_PMG_REG_LINK_STS_CTRL_2          (0x98)



#define ATLAS_REGISTER_PMG_OFF_LANE_MARGIN              (0xD40)             // Lane Margining capacity offset
#define ATLAS_REGISTER_PMG_REG_LM_EXT_CAP_HEADER        (0xD40)             // Lane Margining Extended Capability Header
#define ATLAS_REGISTER_PMG_REG_LM_CAP_STATUS            (0xD44)             // Margining Capabilities and Port status reg
#define ATLAS_REGISTER_PMG_REG_LM_LANE_0_CTRL_STATUS    (0xD48)             // Margin control and status reg

/* Step margining Direction either right or left*/
#define STEP_TO_RIGHT   0x1
#define STEP_TO_LEFT    0x2

#define STEP_TO_UP    0x3
#define STEP_TO_DOWN  0x4



/* Margin Type values */
#define PCIE_REG_LM_MARGIN_TYPE_ACCESS_RPT              (0x1)
#define PCIE_REG_LM_MARGIN_TYPE_SET_PARAMS              (0x2)
#define PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_TIMING      (0x3)
#define PCIE_REG_LM_MARGIN_TYPE_STEP_MARGIN_VOLTAGE     (0x4)
#define PCIE_REG_LM_MARGIN_TYPE_VENDOR_DEF              (0x5)
#define PCIE_REG_LM_MARGIN_TYPE_NO_CMD                  (0x7)

/* Receiver Number (Command) values for upstream ports */
#define PCIE_REG_LM_RX_NUM_BROADCAST_UPSTREAM           (0x0)
#define PCIE_REG_LM_RX_NUM_RX_F                         (0x6)


/* Receiver Number (Command) values for downstream ports */
#define PCIE_REG_LM_RX_NUM_BROADCAST_DOWNSTREAM         (0x0)
#define PCIE_REG_LM_RX_NUM_RX_A                         (0x1)
#define PCIE_REG_LM_RX_NUM_RX_B                         (0x2)
#define PCIE_REG_LM_RX_NUM_RX_C                         (0x3)
#define PCIE_REG_LM_RX_NUM_RX_D                         (0x4)
#define PCIE_REG_LM_RX_NUM_RX_E                         (0x5)


/* Receiver Number (Command) value common to both US and DS ports */
#define PCIE_REG_LM_RX_NUM_RESERVED7                    (0x7)

/* Margin Payload (Command) values*/
#define PCIE_REG_LM_MRGN_CMD_NO_CMD                     (0x9C)

#define PCIE_REG_LM_MRGN_CMD_RPT_MARGIN_CTRL_CAP        (0x88)
#define PCIE_REG_LM_MRGN_CMD_RPT_MNUMVOLTAGESTEPS       (0x89)
#define PCIE_REG_LM_MRGN_CMD_RPT_MNUMTIMINGSTEPS        (0x8A)
#define PCIE_REG_LM_MRGN_CMD_RPT_MMAXTIMINGOFFSET       (0x8B)
#define PCIE_REG_LM_MRGN_CMD_RPT_MMAXVOLTAGEOFFSET      (0x8C)
#define PCIE_REG_LM_MRGN_CMD_RPT_MSAMPLERATEVOLTAGE     (0x8D)
#define PCIE_REG_LM_MRGN_CMD_RPT_MSAMPLERATETIMING      (0x8E)
#define PCIE_REG_LM_MRGN_CMD_RPT_MSAMPLECOUNT           (0x8F)
#define PCIE_REG_LM_MRGN_CMD_RPT_MMAXLANES              (0x90)

#define PCIE_REG_LM_MRGN_CMD_SET_ERR_CNT_LIMIT(ECL)     (0xC0 | ECL)
#define PCIE_REG_LM_MRGN_CMD_GO_TO_NORMAL_SETTINGS      (0x0F)
#define PCIE_REG_LM_MRGN_CMD_CLEAR_ERROR_LOG            (0x55)

/* Margin Payload (Response) values */
#define PCIE_REG_LM_MRGN_RSP_RMCC_MVOLTAGESUPP_MASK     (0x01)
#define PCIE_REG_LM_MRGN_RSP_RMCC_MINDUDVOLTAGE_MASK    (0x02)
#define PCIE_REG_LM_MRGN_RSP_RMCC_MINDLRTIMING_MASK     (0x04)
#define PCIE_REG_LM_MRGN_RSP_RMCC_MSAMPLERPTMETHOD_MASK (0x08)
#define PCIE_REG_LM_MRGN_RSP_RMCC_MINDERRSAMPLER_MASK   (0x10)

/* Offsets into the Lane Margining at the Receiver capability structure */
#define PCIE_EXTCAP_MARGIN_PORT_CAP_STATUS_OFFSET   (0x4)

/* A PCIe link consists of up to 32 lanes */
#define PCIE_REG_LANE_MARGIN_EXTCAP_MAX_LANES       (32)

/* PCIE Lane Margining Operations*/
#define PCIE_LM_COMMANDS(MT, RX, MP)        (((MP) << 0x8 ) | ((MT) << 0x3) | (RX))    

/*Register Offset Calculatoin*/
#define PCIE_LANE_REGISTER_OFFSET(LANE)     (4 * (LANE))

#define PCIE_LANE_MARGIN_DWELL_TIME			(100)


SCRUTINY_STATUS slmGetLaneMarginCapacities (
    __IN__ PTR_SCRUTINY_DEVICE		PtrDevice,
    __IN__ U32						PortNumber,
	__IN__ U32                      LaneNumber,
	__OUT__ PU32                    PtrMarginControlCapabilities,
	__OUT__ PU32                    PtrNumSteps,
	__OUT__ PU32                    PtrMaxOffset
);

SCRUTINY_STATUS slmPerformLaneMargining    (
	__IN__ PTR_SCRUTINY_DEVICE                      PtrDevice, 
	__IN__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_REQUEST  PtrSwitchLaneMarginReq,
    __OUT__ PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE PtrSwitchLaneMarginResp 
);




#endif

