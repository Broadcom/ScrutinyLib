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



#ifndef __SWITCH_PORT_PROPERTIES__H__
#define __SWITCH_PORT_PROPERTIES__H__

#if !defined (OS_UEFI)
struct __PCI_STATUS_BITS
{
    U16     DetectedParityError: 1;  /* BIT: 15 */
    U16     SignaledSystemError: 1;  /* BIT: 13 */
    U16     ReceivedMasterAbort: 1;
    U16     ReceivedTargetAbort: 1;
    U16     SignaledTargetAbort: 1;
    U16     DeviceSelectTiming:  2;
    U16     Others;
    U16     CapabilityList: 1;
    U16     InterruptStatus: 1;
} PCI_STATUS_BITS ;
#endif


// Standard PCI registers
#define PCI_REG_DEV_VEN_ID                      0x00
#define PCI_REG_CMD_STAT                        0x04
#define PCI_REG_CLASS_REV                       0x08
#define PCI_REG_HDR_CACHE_LN                    0x0C
#define PCI_REG_BAR_0                           0x10
#define PCI_REG_BAR_1                           0x14
#define PCI_REG_DEV_SUB_VEN_ID                  0x2C
#define PCI_REG_CAP_PTR                         0x34
#define PCI_REG_INT_PIN_LN                      0x3C

// Type 0 specific standard registers
#define PCI_REG_T0_BAR_2                        0x18
#define PCI_REG_T0_BAR_3                        0x1C
#define PCI_REG_T0_BAR_4                        0x20
#define PCI_REG_T0_BAR_5                        0x24
#define PCI_REG_TO_CARDBUS_PTR                  0x28
#define PCI_REG_TO_SUBSYS_ID                    0x2C
#define PCI_REG_TO_EXP_ROM                      0x30
#define PCI_REG_TO_RSVD_38H                     0x38

// Type 1 specific standard registers
#define PCI_REG_T1_PRIM_SEC_BUS                 0x18
#define PCI_REG_T1_IO_BASE_LIM                  0x1C
#define PCI_REG_T1_MEM_BASE_LIM                 0x20
#define PCI_REG_T1_PF_MEM_BASE_LIM              0x24
#define PCI_REG_T1_PF_MEM_BASE_HIGH             0x28
#define PCI_REG_T1_PF_MEM_LIM_HIGH              0x2C
#define PCI_REG_T1_IO_BASE_LIM_HIGH             0x30
#define PCI_REG_T1_EXP_ROM                      0x38
#define PCI_REG_T1_PHY_SLOT                     0x7C


// PCIe 1st capability pointer
#define PCIE_REG_CAP_PTR                        0x100


// PCI Extended Capability IDs
#define PCI_CAP_ID_NULL                         0x00
#define PCI_CAP_ID_POWER_MAN                    0x01
#define PCI_CAP_ID_AGP                          0x02
#define PCI_CAP_ID_VPD                          0x03
#define PCI_CAP_ID_SLOT_ID                      0x04
#define PCI_CAP_ID_MSI                          0x05
#define PCI_CAP_ID_HOT_SWAP                     0x06
#define PCI_CAP_ID_PCIX                         0x07
#define PCI_CAP_ID_HYPER_TRANSPORT              0x08
#define PCI_CAP_ID_VENDOR_SPECIFIC              0x09
#define PCI_CAP_ID_DEBUG_PORT                   0x0A
#define PCI_CAP_ID_RESOURCE_CTRL                0x0B
#define PCI_CAP_ID_HOT_PLUG                     0x0C
#define PCI_CAP_ID_BRIDGE_SUB_ID                0x0D
#define PCI_CAP_ID_AGP_8X                       0x0E
#define PCI_CAP_ID_SECURE_DEVICE                0x0F
#define PCI_CAP_ID_PCI_EXPRESS                  0x10
#define PCI_CAP_ID_MSI_X                        0x11
#define PCI_CAP_ID_SATA                         0x12
#define PCI_CAP_ID_ADV_FEATURES                 0x13
#define PCI_CAP_ID_ENHANCED_ALLOCATION          0x14


// PCI Express Extended Capability IDs
#define PCIE_CAP_ID_NULL                        0x000       // Empty capability
#define PCIE_CAP_ID_ADV_ERROR_REPORTING         0x001       // Advanced Error Reporting (AER)
#define PCIE_CAP_ID_VIRTUAL_CHANNEL             0x002       // Virtual Channel (VC)
#define PCIE_CAP_ID_DEV_SERIAL_NUMBER           0x003       // Device Serial Number
#define PCIE_CAP_ID_POWER_BUDGETING             0x004       // Power Budgeting
#define PCIE_CAP_ID_RC_LINK_DECLARATION         0x005       // Root Complex Link Declaration
#define PCIE_CAP_ID_RC_INT_LINK_CONTROL         0x006       // Root Complex Internal Link Control
#define PCIE_CAP_ID_RC_EVENT_COLLECTOR          0x007       // Root Complex Event Collector Endpoint Association
#define PCIE_CAP_ID_MF_VIRTUAL_CHANNEL          0x008       // Multi-Function Virtual Channel (MFVC)
#define PCIE_CAP_ID_VC_WITH_MULTI_FN            0x009       // Virtual Channel with Multi-Function
#define PCIE_CAP_ID_RC_REG_BLOCK                0x00A       // Root Complex Register Block (RCRB)
#define PCIE_CAP_ID_VENDOR_SPECIFIC             0x00B       // Vendor-specific (VSEC)
#define PCIE_CAP_ID_CONFIG_ACCESS_CORR          0x00C       // Configuration Access Correlation
#define PCIE_CAP_ID_ACCESS_CTRL_SERVICES        0x00D       // Access Control Services (ACS)
#define PCIE_CAP_ID_ALT_ROUTE_ID_INTERPRET      0x00E       // Alternate Routing-ID Interpretation (ARI)
#define PCIE_CAP_ID_ADDR_TRANS_SERVICES         0x00F       // Address Translation Services (ATS)
#define PCIE_CAP_ID_SR_IOV                      0x010       // SR-IOV
#define PCIE_CAP_ID_MR_IOV                      0x011       // MR-IOV
#define PCIE_CAP_ID_MULTICAST                   0x012       // Multicast
#define PCIE_CAP_ID_PAGE_REQUEST                0x013       // Page Request Interface (PRI)
#define PCIE_CAP_ID_AMD_RESERVED                0x014       // Reserved for AMD
#define PCIE_CAP_ID_RESIZABLE_BAR               0x015       // Resizable BAR
#define PCIE_CAP_ID_DYNAMIC_POWER_ALLOC         0x016       // Dynamic Power Allocation (DPA)
#define PCIE_CAP_ID_TLP_PROCESSING_HINT         0x017       // TLP Processing Hints (TPH)
#define PCIE_CAP_ID_LATENCY_TOLERANCE_REPORT    0x018       // Latency Tolerance Reporting (LTR)
#define PCIE_CAP_ID_SECONDARY_PCI_EXPRESS       0x019       // Secondary PCI Express
#define PCIE_CAP_ID_PROTOCOL_MULTIPLEX          0x01A       // Protocol Multiplexing (PMUX)
#define PCIE_CAP_ID_PROCESS_ADDR_SPACE_ID       0x01B       // Process Address Space ID (PASID)
#define PCIE_CAP_ID_LTWT_NOTIF_REQUESTER        0x01C       // Lightweight Notification Requester (LNR)
#define PCIE_CAP_ID_DS_PORT_CONTAINMENT         0x01D       // Downstream Port Containment (DPC)
#define PCIE_CAP_ID_L1_PM_SUBSTRATES            0x01E       // L1 Power Management Substrates (L1PM)
#define PCIE_CAP_ID_PRECISION_TIME_MEAS         0x01F       // Precision Time Measurement (PTM)
#define PCIE_CAP_ID_PCIE_OVER_M_PHY             0x020       // PCIe over M-PHY (M-PCIe)
#define PCIE_CAP_ID_FRS_QUEUEING                0x021       // FRS Queueing
#define PCIE_CAP_ID_READINESS_TIME_REPORTING    0x022       // Readiness Time Reporting
#define PCIE_CAP_ID_DESIGNATED_VEND_SPECIFIC    0x023       // Designated vendor-specific
#define PCIE_CAP_ID_VF_RESIZABLE_BAR            0x024       // VF resizable BAR
#define PCIE_CAP_ID_DATA_LINK_FEATURE           0x025       // Data Link Feature
#define PCIE_CAP_ID_PHYS_LAYER_16GT             0x026       // Physical Layer 16 GT/s
#define PCIE_CAP_ID_PHYS_LAYER_16GT_MARGINING   0x027       // Physical Layer 16 GT/s Margining
#define PCIE_CAP_ID_HIERARCHY_ID                0x028       // Hierarchy ID
#define PCIE_CAP_ID_NATIVE_PCIE_ENCL_MGMT       0x029       // Native PCIe Enclosure Management (NPEM)


#define ATLAS_REGISTER_PMG_REG_SWITCH_ID        0x609F0000
#define ATLAS_REGISTER_PMG_REG_SWITCH_MODE      0xFFF000B0


typedef struct _SWITCH_CFG_PAGE_E00F
{
    U8   PortNum;                   /**< Port number */
    U8   DsSlotCount;               /**< Number of synthetic DS ports to assign to host */
    U16  Flags;                     /**< Flags for configuration & features */
    U16  Reserved00;                /**< Reserved 16-bit value for future use */
    U16  SynthDevSerialNum;         /**< User configurable portion of synthetic PCIe serial num */
    U8   SynthDsResNonPFMem;        /**< Synthetic DS Non-Prefetchable memory to reserve (2^x) */
    U8   SynthDsResPFMem;           /**< Synthetic DS Prefetchable memory to reserve (2^x) */
    U8   SynthDsResIoPort;          /**< Synthetic DS I/O port space to reserve (2^x) */
    U8   TwcBarSize;                /**< TWC BAR 2/3 size (2^x) */
    U32  TwcPeerGID;                /**< TWC Peer GID - Used in Pairing */
    U32  DsToHostMask[4];           /**< Physical DS --> Host port assignment mask */
    U32  Reserved[4];
} SWITCH_CFG_PAGE_E00F, *PTR_SWITCH_CFG_PAGE_E00F;


/** Config page per-port options */
typedef struct _SWITCH_CFG_PAGE_E00E_PORT
{
    U8  PortNum;                    /**< Port number */
    U8  PortType;                   /**< Port type (FFh=Disabled, 0=DS, 1=Fabric, 3=Host) */
    U8  Reserved00;                 /**< Reserved 8-bit value for future use */
    U8  Reserved01;                 /**< Reserved 8-bit value for future use */
    U16 ChassisPhysSlotNum;         /**< If DS, chassis physical slot number assigned */
    U16 Reserved02;                 /**< Reserved 16-bit value for future use */
    U32 Reserved03;                 /**< Reserved 32-bit value for future use */
    U32 Reserved04;                 /**< Reserved 32-bit value for future use */
} SWITCH_CFG_PAGE_E00E_PORT, *PTR_SWITCH_CFG_PAGE_E00E_PORT;

/** Switch ID unique within fabric */
typedef struct _SWITCH_ID
{
    union
    {
        U16 ID;                             /**< Switch ID */
        struct
        {
            U8 Number;                      /**< Chip number within domain (0-based) */
            U8 Domain;                      /**< Fabric domain number */
        } DN;
    } u;
} SWITCH_ID, *PTR_SWITCH_ID;





SCRUTINY_STATUS sppGetPciePortProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties);
SCRUTINY_STATUS sppGetPciePortPropertiesSsw (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties);
SCRUTINY_STATUS sppGetPcieOnePortProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrPortConfiguration);
SCRUTINY_STATUS sppFillPortConfigurationFromConfigSpace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrPortConfiguration);
SCRUTINY_STATUS sppGetCapabilityOffset (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __IN__ U32 CapabilityId, __IN__ BOOLEAN IsExtendedPcieCapability, __IN__ PU32 PtrOffset);
SCRUTINY_STATUS sppGetCapabilityOffsetInstance (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __IN__ U32 CapabilityId, __IN__ U32 InstanceNumber, __IN__ BOOLEAN IsExtendedPcieCapability, __IN__ PU32 PtrOffset);
SCRUTINY_STATUS sppFillPortConfigurationFromCcrSpace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrConfiguration);
SCRUTINY_STATUS sppGetDownStreamPortAdditionalProperties (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 PortIndex, __OUT__ PTR_SWITCH_PORT_CONFIGURATION PtrPortConfiguration);


SCRUTINY_STATUS sppGetPciePortPropertiesBsw (__IN__ PTR_SCRUTINY_DEVICE PtrDevice,  __OUT__ PTR_SCRUTINY_SWITCH_PORT_PROPERTIES PtrPciePortProperties);
#endif /* __SWITCH_PORT_PROPERTIES__H__ */
