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

#ifndef __SCRUTINY_DEF__H__
#define __SCRUTINY_DEF__H__

#define __OUT__     /* Used for Output - When calling a function argument */
#define __IN__      /* Used for Input - When calling a function as an argument */
#define __INOUT__   /* Used for Both Input and Output */


#ifndef TRUE
#define TRUE    (1)
#endif
#ifndef FALSE
#define FALSE   (0)
#endif


#if !defined(VOID_DEFINED)

#define VOID_DEFINED

#ifndef VOID
typedef void VOID;
#endif

typedef VOID *PVOID;

#endif

#if !defined(BOOLEAN_DEFINED)
#define BOOLEAN_DEFINED
typedef U8 BOOLEAN;
#endif

typedef enum __SCRUTINY_CONTROLLER_IOC_STATE
{

    SCRUTINY_CONTROLLER_IOC_STATE_RESET             = 0x00,
    SCRUTINY_CONTROLLER_IOC_STATE_READY             = 0x01,
    SCRUTINY_CONTROLLER_IOC_STATE_OPERATIONAL       = 0x02,
    SCRUTINY_CONTROLLER_IOC_STATE_FAULT             = 0x03,
    SCRUTINY_CONTROLLER_IOC_STATE_BOOT_MSG_PENDING  = 0x04,
    SCRUTINY_CONTROLLER_IOC_STATE_COREDUMP          = 0x05,
    SCRUTINY_CONTROLLER_IOC_STATE_BECOMING_READY    = 0x06,
    SCRUTINY_CONTROLLER_IOC_STATE_RESET_REQUESTED   = 0x07,
    SCRUTINY_CONTROLLER_IOC_STATE_RESET_COMPLETED   = 0x08

} SCRUTINY_CONTROLLER_IOC_STATE;

/**
 * @brief Default allocations
 */

#define SCRUTINY_MAX_CDB_LENGTH           (32)
#define SCRUTINY_MAX_SENSE_LENGTH         (252)
#define SCRUTINY_MAX_SMP_DATA_SIZE        (1028)


/*Buffer Type indicates the direction of the buffer to be filled in*/

#define SCRUTINY_BUFFER_TYPE_UNKNOWN          0
#define SCRUTINY_BUFFER_TYPE_RAIDMGMT_CMD     1
#define SCRUTINY_BUFFER_TYPE_RAIDMGMT_RESP    2
#define SCRUTINY_BUFFER_TYPE_DATA_IN          3
#define SCRUTINY_BUFFER_TYPE_DATA_READ        SCRUTINY_BUFFER_TYPE_DATA_IN
#define SCRUTINY_BUFFER_TYPE_DATA_OUT         4
#define SCRUTINY_BUFFER_TYPE_DATA_WRITE       SCRUTINY_BUFFER_TYPE_DATA_OUT
#define SCRUTINY_BUFFER_TYPE_MPI_REPLY        5
#define SCRUTINY_BUFFER_TYPE_ERR_RESPONSE     6

#define SCRUTINY_HANDLE_TYPE_MASK_OOB       0x00000100
#define SCRUTINY_HANDLE_TYPE_MASK_INBAND    0x00001000
#define SCRUTINY_HANDLE_TYPE_MASK_SCSI      0x00100000
#define SCRUTINY_HANDLE_TYPE_MASK_BMC       0x01000000

typedef enum __SCRUTINY_HANDLE_TYPE
{
    SCRUTINY_HANDLE_TYPE_NONE                       = 0,
    SCRUTINY_HANDLE_TYPE_SDB                        = SCRUTINY_HANDLE_TYPE_MASK_OOB    | 0x01,
    SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC       = SCRUTINY_HANDLE_TYPE_MASK_INBAND | SCRUTINY_HANDLE_TYPE_MASK_SCSI | 0x02,
    SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE      = SCRUTINY_HANDLE_TYPE_MASK_INBAND | SCRUTINY_HANDLE_TYPE_MASK_SCSI | 0x03,
    SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE      = SCRUTINY_HANDLE_TYPE_MASK_INBAND | SCRUTINY_HANDLE_TYPE_MASK_SCSI | 0x04,
    SCRUTINY_HANDLE_TYPE_SCSI_ON_IOF                = SCRUTINY_HANDLE_TYPE_MASK_OOB    | SCRUTINY_HANDLE_TYPE_MASK_SCSI | 0x05,
    SCRUTINY_HANDLE_TYPE_PCI                        = SCRUTINY_HANDLE_TYPE_MASK_INBAND | 0x6,
    SCRUTINY_HANDLE_TYPE_MPT                        = SCRUTINY_HANDLE_TYPE_MASK_INBAND | 0x8,
    SCRUTINY_HANDLE_TYPE_MFI                        = SCRUTINY_HANDLE_TYPE_MASK_INBAND | 0x9,
    SCRUTINY_HANDLE_TYPE_MCTP_I2C                   = SCRUTINY_HANDLE_TYPE_MASK_BMC    | SCRUTINY_HANDLE_TYPE_MASK_SCSI | 0xA,
    SCRUTINY_HANDLE_TYPE_MCTP_PCIE                  = SCRUTINY_HANDLE_TYPE_MASK_BMC    | SCRUTINY_HANDLE_TYPE_MASK_SCSI | 0xB

} SCRUTINY_HANDLE_TYPE;

/**
 *
 * @brief Handle is an abstract reference to the products used
 *        in the Scrutiny Library.
 *
 */

typedef U32 SCRUTINY_PRODUCT_HANDLE;

/**
 *
 * @brief Pointer to the handle
 *
 */

typedef SCRUTINY_PRODUCT_HANDLE* PTR_SCRUTINY_PRODUCT_HANDLE;


/**
 *
 * @brief Enum to define the interface/communication type
 *
 */

typedef enum __SCRUTINY_DISCOVERY_TYPE
{

    SCRUTINY_DISCOVERY_TYPE_INBAND          = 1,  /**< Performs all inband discovery (includes PCIe, MPI and SG) will be happening */
    SCRUTINY_DISCOVERY_TYPE_SERIAL_DEBUG    = 2,   /**< SDB interface/ Serial Debug interface in Expander/Switch products */
    SCRUTINY_DISCOVERY_TYPE_OOB_I2C         = 3,  /** Performs I2C discovery from OOB*/
    SCRUTINY_DISCOVERY_TYPE_OOB_PCI         = 4  /**Perform OOB disocvery of PCIe devices*/

} SCRUTINY_DISCOVERY_TYPE;


/**
 *
 * @brief Following are the list of Product's family supported
 *        by the Scrutiny library
 *
 */

typedef enum __SCRUTINY_PRODUCT_FAMILY
{
    SCRUTINY_PRODUCT_FAMILY_INVALID = 0,    /**< Invalid product family or unknown product type */
    SCRUTINY_PRODUCT_FAMILY_CONTROLLER,     /**< Controller product family includes IT/IR and MR products */
    SCRUTINY_PRODUCT_FAMILY_EXPANDER,       /**< Expander product family including the third party expanders discovered through Controllers*/
    SCRUTINY_PRODUCT_FAMILY_SWITCH,         /**< PCIe Switch products */

} SCRUTINY_PRODUCT_FAMILY;

/**
 *
 * @brief Information about PCI address
 *
 */

typedef struct _SCRUTINY_PCI_ADDRESS
{

    U16  BusNumber;
    U16  DeviceNumber;
    U16  FunctionNumber;
    U16  Reserved;
    U32  SegmentNumber;

} SCRUTINY_PCI_ADDRESS, *PTR_SCRUTINY_PCI_ADDRESS, SCRUTINY_IAL_PCI_CONFIG, *PTR_SCRUTINY_IAL_PCI_CONFIG;

#if defined (LIB_SUPPORT_CONTROLLER)
/**
 *
 * @brief
 *
 * Information about IT controller
 *
 */

typedef struct _SCRUTINY_CONTROLLER_IT_INFO
{
    U32                         AdapterType;
    U32                         MpiPortNumber;
    U32                         MpiVersion;
    MPI2_IOC_FACTS_REPLY        IocFacts;
    U8                          HostScsiId;
    BOOLEAN                     IsNvmeEnabledFirmware;
    BOOLEAN                     IsMPI25Supported;
    BOOLEAN                     IsMPI26Supported;
    BOOLEAN                     IsIRFirmware;

} SCRUTINY_CONTROLLER_IT_INFO, *PTR_SCRUTINY_CONTROLLER_IT_INFO;

typedef union _SCRUTINY_CONTROLLER_GEN3_INFO
{
    SCRUTINY_CONTROLLER_IT_INFO     IT;
    MR_CTRL_INFO                    MR;

} SCRUTINY_CONTROLLER_GEN3_INFO;

#ifdef PRODUCT_SUPPORT_AVENGER
typedef struct _SCRUTINY_CONTROLLER_GEN4_INFO
{
    MR8_CTRL_STATIC_INFO  MRControllerInfo;  //MR information will be obtained from MR header files
    U32                   AdapterPersonality; // Avenger addition : This is a copy of Personality feild in IOC facts. With this we can tell that if it is MR or IT
    U32                   MpiVersion;
    MPI3_IOC_FACTS_DATA   IocFacts;
    U32                   IoctlVersion;   
    U32                   PackageVersion; 
	BOOLEAN               IsMPI3Supported;  // avenger changes

    #if defined(OS_UEFI)
    BOOLEAN               IsHostbootRequired;
    #endif

} SCRUTINY_CONTROLLER_GEN4_INFO;
#else
typedef struct __SCRUTINY_CONTROLLER_GEN4_INFO
{
    U8 Reserved[1024];

}SCRUTINY_CONTROLLER_GEN4_INFO;
#endif

#else
typedef struct __SCRUTINY_CONTROLLER_GEN4_INFO
{
    U8 Reserved[1024];

}SCRUTINY_CONTROLLER_GEN4_INFO;

typedef struct __SCRUTINY_CONTROLLER_GEN3_INFO
{
    U8 Reserved[1024];

}SCRUTINY_CONTROLLER_GEN3_INFO;

#endif


/**
 *
 * @brief
 *
 * Basic structure which will be exposed to customer which contain information
 * about the discovered controller
 *
 */

typedef enum _SCRUTINY_CONTROLLER_TYPE
{
    SCRUTINY_CONTROLLER_TYPE_INVALID = 0 ,
    SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR = 1,
    SCRUTINY_CONTROLLER_TYPE_GEN3_MR,
    SCRUTINY_CONTROLLER_TYPE_GEN4

} SCRUTINY_CONTROLLER_TYPE;

typedef struct _SCRUTINY_CONTROLLER_INFO
{
    SCRUTINY_PCI_ADDRESS    PciAddress; // bus/dev/fun are common between MR and IT
    U8                      Reserved[3];
    U32                     NumPhys;

    U32                     PciVendorId;
    U32                     PciDeviceId;
    U32                     PciDeviceHwRev;
    U32                     PciSSDeviceId;
    U32                     PciSSVendorId;

    char                    DriverVersion[256];

    union
    {
        SCRUTINY_CONTROLLER_GEN4_INFO  Gen4;
        SCRUTINY_CONTROLLER_GEN3_INFO  Gen3;
    } u;

    SCRUTINY_CONTROLLER_TYPE        ControllerType;
    BOOLEAN                         IsTamperedDevice;

} SCRUTINY_CONTROLLER_INFO, *PTR_SCRUTINY_CONTROLLER_INFO;

/**
 *
 * @brief
 *
 * Basic structure which will be exposed to customer which contain information
 * about the discovered expanders
 *
 */

typedef struct _SCRUTINY_EXPANDER_INFO
{
    U64                     SASAddress;
    U64                     EnclosureWwid;
    U32                     FWVersion;
    U32                     ComponentId;
    U32                     RevisionId;
    U32                     NumPhys;
    SCRUTINY_PCI_ADDRESS    HostPciAddress;
    BOOLEAN                 IsBroadcomExpander;

} SCRUTINY_EXPANDER_INFO, *PTR_SCRUTINY_EXPANDER_INFO;

typedef enum __SWITCH_MODE
{

    SWITCH_MODE_UNKNOWN = 0,
    SWITCH_MODE_BASE,               /* This is a Base mode */
    SWITCH_MODE_iSSW,               /* This is a SDK mode */
    SWITCH_MODE_BASE_WITH_iSSW      /* SDK mode running with base mode feature */

} SWITCH_MODE;

/**
 *
 * @brief
 *
 * Basic structure which will be exposed to customer which contain information
 * about the discovered expanders
 *
 */

typedef struct _SCRUTINY_SWITCH_INFO
{

    U64                     SASAddress;
    U64                     EnclosureWwid;
    U32                     FWVersion;
    SWITCH_MODE             SwitchMode;
    U16                     ComponentId;
    U16                     RevisionId;
    U16                     PciDeviceId;
    U16                     PciVendorId;
    U16                     Reserved;
    U32                     NumLanes;
    SCRUTINY_PCI_ADDRESS    PciAddress;

    //TO_DO : Need to know what all information should be passed to upper layer

} SCRUTINY_SWITCH_INFO, *PTR_SCRUTINY_SWITCH_INFO;

/**
 *
 * @brief
 *
 * Basic structure which will be exposed to customer which contain information
 * about the discovered products
 *
 */

typedef struct _SCRUTINY_DEVICE_INFO
{
    SCRUTINY_PRODUCT_HANDLE      ProductHandle;

    U32                          HandleType;

    union
    {
        SCRUTINY_CONTROLLER_INFO    ControllerInfo;
        SCRUTINY_EXPANDER_INFO      ExpanderInfo;
        SCRUTINY_SWITCH_INFO        SwitchInfo;

    } u;

    SCRUTINY_PRODUCT_FAMILY         ProductFamily;  /* Used for identifying the product structure */

} SCRUTINY_DEVICE_INFO, *PTR_SCRUTINY_DEVICE_INFO;


typedef struct _SCRUTINY_SAS_ERROR_COUNTERS
{

    U32     InvalidDwordCount;
    U32     RunningDisparityErrorCount;
    U32     LossDwordSynchCount;
    U32     PhyResetProblemCount;

} SCRUTINY_SAS_ERROR_COUNTERS, *PTR_SCRUTINY_SAS_ERROR_COUNTERS;


typedef struct _SCRUTINY_PCI_LINK_ERROR_COUNTERS
{

    U32     CorrectableErrorCount;
    U16     NonFatalErrorCount;
    U16     FatalErrorCount;

} SCRUTINY_PCI_LINK_ERROR_COUNTERS, *PTR_SCRUTINY_PCI_LINK_ERROR_COUNTERS;



/**
 *
 * @brief
 *
 * Baud Rates used for Com Configuration. This rate is Independant of the
 * Operating systems. User should use appropriate conversion for making it
 * for their own OSs.
 *
 */

typedef enum __SCRUTINY_BAUD_RATE
{

    SCRUTINY_BAUD_RATE_UNKNOWN = 0,  /** Unknown Baud Rate. */

    SCRUTINY_BAUD_RATE_110,          /** Baud Rate with the Speed of 110 */
    SCRUTINY_BAUD_RATE_300,          /** Baud Rate with the Speed of 300 */
    SCRUTINY_BAUD_RATE_600,          /** Baud Rate with the Speed of 600 */
    SCRUTINY_BAUD_RATE_1200,         /** Baud Rate with the Speed of 1200 */
    SCRUTINY_BAUD_RATE_2400,         /** Baud Rate with the Speed of 2400 */
    SCRUTINY_BAUD_RATE_4800,         /** Baud Rate with the Speed of 4800 */
    SCRUTINY_BAUD_RATE_9600,         /** Baud Rate with the Speed of 9600 */
    SCRUTINY_BAUD_RATE_14400,        /** Baud Rate with the Speed of 14400 */
    SCRUTINY_BAUD_RATE_19200,        /** Baud Rate with the Speed of 19200 */
    SCRUTINY_BAUD_RATE_38400,        /** Baud Rate with the Speed of 38400 */
    SCRUTINY_BAUD_RATE_57600,        /** Baud Rate with the Speed of 57600 */
    SCRUTINY_BAUD_RATE_115200,       /** Baud Rate with the Speed of 115200 */
    SCRUTINY_BAUD_RATE_230400        /** Baud Rate with the Speed of 230400 */

} SCRUTINY_BAUD_RATE;

/**
 *
 * @brief
 *
 * Flow Control.
 *
 */

typedef enum __SCRUTINY_FLOW_CONTROL
{
    SCRUTINY_FLOW_CONTROL_UNKNOWN  = -1, /** Unknown flow control */
    SCRUTINY_FLOW_CONTROL_NONE     = 0,  /** Flow Control Disable/None. */
    SCRUTINY_FLOW_CONTROL_SOFTWARE = 1,  /** Software enabled Flow Control */
    SCRUTINY_FLOW_CONTROL_HARDWARE = 2   /** Hardware enabled flow control */

} SCRUTINY_FLOW_CONTROL;

/**
 *
 * @brief
 *
 * Data rates
 *
 */

typedef enum __SCRUTINY_DATA_BITS
{
    SCRUTINY_DATA_BITS_UNKNOWN  = -1,    /** Data Rate Uknown */
    SCRUTINY_DATA_BITS_5        = 5,     /** Data Rate with the value 5 */
    SCRUTINY_DATA_BITS_6        = 6,     /** Data Rate with the value 6 */
    SCRUTINY_DATA_BITS_7        = 7,     /** Data Rate with the value 7 */
    SCRUTINY_DATA_BITS_8        = 8      /** Data Rate with the value 8 */

} SCRUTINY_DATA_BITS;

/**
 *
 * @brief
 *
 * Partiy check
 *
 */

typedef enum __SCRUTINY_PARITY
{
    SCRUTINY_PARITY_UNKNOWN  = -1,       /** Unknown parity */
    SCRUTINY_PARITY_NONE     = 0,        /** Parity Disabled */
    SCRUTINY_PARITY_ODD      = 1,        /** Odd Parity Check */
    SCRUTINY_PARITY_EVEN     = 2         /** Even parity*/

} SCRUTINY_PARITY;

/**
 *
 * @brief
 *
 * Stop Bits
 *
 */

typedef enum __SCRUTINY_STOP_BITS
{
    SCRUTINY_STOP_BITS_1_0   = 0,        /** Stop Bits 1.0 */
    SCRUTINY_STOP_BITS_1_5   = 1,        /** Stop Bits 1.5 */
    SCRUTINY_STOP_BITS_2_0   = 2         /** Stop Bits 2.0 */

} SCRUTINY_STOP_BITS;


/**
 *
 * @brief
 *
 * Com configuration vault. This will hold the configuration of the Com port.
 * OSs dependant module will convert this configuration to the native values.
 *
 */

typedef struct __SCRUTINY_IAL_SERIAL_CONFIG
{

    char                    DeviceName[1024];    /** Name/Path of the device to be openened */
    SCRUTINY_BAUD_RATE      BaudRate;           /** Baud Rates with which SERIAL will connect */
    SCRUTINY_FLOW_CONTROL   FlowControl;        /** Flow Control value */
    SCRUTINY_STOP_BITS      StopBits;           /** Stop bit value */
    SCRUTINY_DATA_BITS      DataBits;           /** Data Bit value */
    SCRUTINY_PARITY         Parity;             /** Parity check */

} SCRUTINY_IAL_SERIAL_CONFIG, *PTR_SCRUTINY_IAL_SERIAL_CONFIG;

typedef struct __SCRUTINY_OOB_CONFIG
{
    U8 ChannelAddress;
    U8 DeviceAddress;

} SCRUTINY_OOB_CONFIG , *PTR_SCRUTINY_OOB_CONFIG;

 typedef struct __SCRUTINY_IAL_OOB_CONFIG
{
    SCRUTINY_OOB_CONFIG                 OobConfig[255];
    U8                                  OobCtrlCount; //Count of the OOB controllers

}SCRUTINY_IAL_OOB_CONFIG , PTR_SCRUTINY_IAL_OOB_CONFIG;

typedef struct __SCRUTINY_DISCOVERY_PARAMS
{
    union
    {
        SCRUTINY_IAL_OOB_CONFIG             OobConfigIAL;
        SCRUTINY_IAL_SERIAL_CONFIG          SerialConfig;
        SCRUTINY_IAL_PCI_CONFIG             PCIConfig;
    } u;

} SCRUTINY_DISCOVERY_PARAMS, *PTR_SCRUTINY_DISCOVERY_PARAMS;

typedef struct __SCRUTINY_DEBUG_LOGGER
{

    VOID (*logiOutput) (const char* PtrLogEntry);

} SCRUTINY_DEBUG_LOGGER, *PTR_SCRUTINY_DEBUG_LOGGER;

typedef struct __SCRUTINY_IOC_STATUS
{
    U16             IocStatus;
    U16             Reserved;
    U32             IocLogInfo;

} SCRUTINY_IOC_STATUS, *PTR_SCRUTINY_IOC_STATUS;

typedef enum _SCRUTINY_DIRECTION
{

    DIRECTION_NONE        = 0,            /* no data transfer */
    DIRECTION_WRITE       = 1,            /* transfer is from host */
    DIRECTION_READ        = 2,            /* transfer is to host  */
    DIRECTION_BOTH        = 3,            /* transfer is both read and write, or unknown */

} SCRUTINY_DIRECTION;



typedef struct _SCRUTINY_BUFFER_ENTRY
{
    U8          BufferType;
    U8          Reserved[3];
    U32         BufferLength;
    PVOID       PtrBuffer;
}SCRUTINY_BUFFER_ENTRY, *PTR_SCRUTINY_BUFFER_ENTRY;


typedef struct _SCRUTINY_DATA_BUFFER
{
    U8                              NumOfEntries;
    U8                              Reserved[3];
    SCRUTINY_BUFFER_ENTRY           BufferEntryList[1];
} SCRUTINY_DATA_BUFFER, *PTR_SCRUTINY_DATA_BUFFER;


typedef struct _SCRUTINY_MPI_PASSTHROUGH
{

    U32                         Size;               /* Size of this structure */
    SCRUTINY_IOC_STATUS         IocStatus;

    U32                         RequestMessageLength;
    PVOID                       PtrRequestMessage;

    U32                         ReplyMessageLength;
    PVOID                       PtrReplyMessage;

    U32                         RequestSenseBufferLength;
    PVOID                       PtrRequestSenseBuffer;

    U32                         MaxIoTimeOut;
    U32                         SglOffset;

    PTR_SCRUTINY_DATA_BUFFER    PtrDataBuffer;

} SCRUTINY_MPI_PASSTHROUGH, *PTR_SCRUTINY_MPI_PASSTHROUGH;


typedef struct _SCRUTINY_SCSI_PASSTHROUGH
{

    U32                     Size;               /* Size of this structure */
    //U8                      BusNumber;
    //U8                      TargetId;

    union
    {
        U8           Lun8;
        U64          Lun64;
    } lun;

    BOOLEAN                 UseLun64;
    U8                      CdbLength;
    U8                      Cdb[SCRUTINY_MAX_CDB_LENGTH];
    U8                      SenseInfoBuffer[SCRUTINY_MAX_SENSE_LENGTH];     /* Sense Buffer length changed to support both fixed and descriptor formats. */
    SCRUTINY_DIRECTION      DataDirection;                  /* Data Direction either Write or Read */

    PVOID                   PtrDataBuffer;
    U32                     DataBufferLength;

    U8                      ScsiStatus;
    U32                     MaxIoTimeout;

    BOOLEAN                 IsLogicalDrive;

} SCRUTINY_SCSI_PASSTHROUGH, *PTR_SCRUTINY_SCSI_PASSTHROUGH;

/**
 *
 * @brief
 *
 * Controller Phy & Link error counter structures.
 *
*/


typedef enum _SCRUTINY_MPT_CONFIG_REGION
{
    MptConfigRegionCurrent = 0,
    MptConfigRegionDefault,
    MptConfigRegionNVRAM

} SCRUTINY_MPT_CONFIG_REGION, *PTR_SCRUTINY_MPT_CONFIG_REGION;

typedef struct _SCRUTINY_CONTROLLER_CFG_PASSTHROUGH
{

    U32                              Size;               /* Size of this structure */
    SCRUTINY_MPT_CONFIG_REGION       ConfigRegion;
    U32                              PageAddress;
    U8                               PageType;
    U8                               PageNumber;
    U8                               ExtPageType;
    PU8                              PtrPageBuffer;
    U16                              PageBufferLength;
    SCRUTINY_IOC_STATUS              IocStatus;
    SCRUTINY_DIRECTION               RequestDirection;

} SCRUTINY_CONTROLLER_CFG_PASSTHROUGH, *PTR_SCRUTINY_CONTROLLER_CFG_PASSTHROUGH;


typedef struct _SCRUTINY_SMP_PASSTHROUGH
{

    U32                     Size;               /* Size of this structure */
    U32                     RequestDataLength;
    U64                     SASAddress;
    U8                      SASStatus;
    U16                     ResponseDataLength;
    SCRUTINY_DIRECTION      RequestDirection;
    U8                      RequestData [SCRUTINY_MAX_SMP_DATA_SIZE];
    U8                      ResponseData [SCRUTINY_MAX_SMP_DATA_SIZE];

} SCRUTINY_SMP_PASSTHROUGH, *PTR_SCRUTINY_SMP_PASSTHROUGH;

typedef struct _SCRUTINY_DCMD_RESPONSE
{

    U32                     CommandStatus;      /* Command for MR 7.x and CmdStatus for MR 8.x */

#if defined(PRODUCT_SUPPORT_AVENGER)

    MR8_MGMT_RESPONSE      *PtrRaidResponse;    /* This can be NULL for getting only the basic information.
                                                   This is for only MR8.x. User must have to handle number of
                                                   data transfer elements in the structure. */

    U32                     RaidResponseSize;

#endif

} SCRUTINY_DCMD_RESPONSE, *PTR_SCRUTINY_DCMD_RESPONSE;



typedef struct _SCRUTINY_OOB_CONTROL_COMMAND
{
    U8  ControlCommand;        // type MR_OOB_CONTROL_CMD
    U8  Reserved;
    U16 PayLoad;            // payload of control command */

}SCRUTINY_OOB_CONTROL_COMMAND, *PTR_SCRUTINY_OOB_CONTROL_COMMAND;

typedef struct _SCRUTINY_DCMD_PASSTHROUGH
{

    U32     Size;                   /* Sizeof this structure */

    U32     DcmdOpCode;             /* DCMD OpCode for all the DCMD commands */

    U32     IoctlControlCode;       /* Use this only for MR7.x for MR8.x this is don't care */

    union
    {
        U8       b[32];               /**< byte reference to mailbox */
        U16      s[16];               /**< short reference to mailbox */
        U32      w[8];                /**< word access to mailbox data */
    } MailBox;

    SCRUTINY_DIRECTION                  DirectionFlags;

    U16                                 Timeout;

    SCRUTINY_OOB_CONTROL_COMMAND        OobControlCommand;
    PTR_SCRUTINY_DATA_BUFFER            PtrDataBuffer;

    PTR_SCRUTINY_DCMD_RESPONSE          PtrDcmdResponse;

} SCRUTINY_DCMD_PASSTHROUGH, *PTR_SCRUTINY_DCMD_PASSTHROUGH;

typedef struct _SCRUTINY_MFI_TOOLBOX_PASSTHROUGH
{
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          reserved;               /* set to 0 */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    U8          reserved1;              /* set to 0 */
    U16         targetId;               /* targetId */
    U8          reserved2;              /* set to 0 */
    U8          numSGE;                 /* number of SG Elements */

    U16         flags;                  /* command flags */
    U16         timeout;                /* command timeout (in seconds) */

    U32         Sge0BufferLength;
    U32         Sge1BufferLength;

    PVOID       PtrSge0Buffer;
    PVOID       PtrSge1Buffer;

}SCRUTINY_MFI_TOOLBOX_PASSTHROUGH, *PTR_SCRUTINY_MFI_TOOLBOX_PASSTHROUGH;



typedef enum
{
    HBA_AFD_ACTION_SET_TRIGGER,
    HBA_AFD_ACTION_GET_TRIGGER,
    HBA_AFD_ACTION_CLEAR_TRIGGER,
    HBA_AFD_ACTION_QUERY_PARAMETERS //to query how many triggers the driver can hold for each trigger
} HBA_AFD_ACTION, *PTR_HBA_AFD_ACTION;


//Supported Trigger Types for Auto FW diag Buff Feature
typedef enum
{
    HBA_AFD_TRIGGER_TYPE_MASTER= 1,
    HBA_AFD_TRIGGER_TYPE_MPI_EVENT,
    HBA_AFD_TRIGGER_TYPE_SCSI_SENSE,
    HBA_AFD_TRIGGER_TYPE_MPI_LOGINFO,
    HBA_AFD_TRIGGER_TYPE_NONE
} HBA_AFD_TRIGGER_TYPE, *PTR_HBA_AFD_TRIGGER_TYPE;

typedef struct _SCRUTINY_EXTENDED_QUERY
{
	U16			BufferReleasedCondition;
	U16 		Reserved0;
	U32			TriggerType;
	U32 		TriggerInfo [2];
	
}SCRUTINY_EXTENDED_QUERY, *PTR_SCRUTINY_EXTENDED_QUERY;


#define SCRUTINY_CONTROLLER_CORE_DUMP_CLI_STR_SIZE   (0x50 * 32 + 32)


typedef struct _SCRUTINY_CONTROLLER_CORE_DUMP_ENTRY
{
    U32             DataSize;
    U64             Time;
    U32             FaultCode;
    BOOLEAN         IsCompressed;
    U8              Cli[SCRUTINY_CONTROLLER_CORE_DUMP_CLI_STR_SIZE];
} SCRUTINY_CONTROLLER_CORE_DUMP_ENTRY, *PTR_SCRUTINY_CONTROLLER_CORE_DUMP_ENTRY;


typedef struct _SCRUTINY_CORE_DUMP_ENTRY
{
    union
    {
        // TODO: Add Expander Support
        SCRUTINY_CONTROLLER_CORE_DUMP_ENTRY  CoreDumpEntry;
    } u;

} SCRUTINY_CORE_DUMP_ENTRY, *PTR_SCRUTINY_CORE_DUMP_ENTRY;


/* bit flags to determine what controller log to be collected*/
#define SCRUTINY_CTRL_LOGS_BIT_CORE_DUMP                    (0x00000001)
#define SCRUTINY_CTRL_LOGS_BIT_HOST_TRACE_BUFFER            (0x00000002)


typedef struct _SCRUTINY_ISTWI_REQUEST
{
    U8          DevIndex;
    U8          Action;
    U8          Reserved[2];
    U16         RxDataLength;
    U16         TxDataLength;
    VOID        *RxDataBuffer;
    VOID        *TxDataBuffer;
    U8          MsgFlags;
    U8          Flags;
    U8          DeviceType;
    U8          IstwiControllerNum;
    U16         DeviceAddr;
    U8          MuxType;
    U8          MuxChannel;
    U16         MuxAddr;

} SCRUTINY_ISTWI_REQUEST, *PTR_SCRUTINY_ISTWI_REQUEST;


typedef struct _SCRUTINY_LANE_MARGIN_REQUEST
{
    U8      ToolboxCommand;
    U8      SwitchPort;
    U8      RegisterOffset;
    U8      Reserved;
    U16     DevHandle;
    U16     DataLength;
    U8      StartLane;  // for MPI3.0 ( Applicable only for Read & Write Requests )
    U8      NumLanes;   // for MPI3.0 ( Applicable only for Read & Write Requests )
    PU32    PtrRegisterValues;

} SCRUTINY_LANE_MARGIN_REQUEST, *PTR_SCRUTINY_LANE_MARGIN_REQUEST;

typedef struct _SCRUTINY_MEMORY_OPERATION_REQUEST
{

    /*Make Register U64*/
    U64     RegisterAddress;
    U32     RegisterData;
    U32     DataLength;
    U32     Direction;

}SCRUTINY_MEMORY_OPERATION_REQUEST, *PTR_SCRUTINY_MEMORY_OPERATION_REQUEST;

#if defined (LIB_SUPPORT_CONTROLLER_MR)
typedef enum {
    ACTION_EXPORT_CSR = 1,
    ACTION_IMPORT_CERT_CHAIN = 2,
    ACTION_SLOT_MANAGEMENT  = 3,
    ACTION_INVALID = 0xFF
} SPDM_ACTIONS;

typedef struct _SCRUTINY_SPDM_REQUEST
{
  SPDM_ACTIONS                      Action;
  union
  {
    MR_CTRL_PSEC_CSR_REQ_RES       CsrRequestResponse;
    MR_CTRL_PSEC_CER_CHAIN_REQ     CertificateChainRequest;
    MR_CTRL_PSEC_SLOT_MGMT_REQ     SlotManagementRequest;
  }Req;
} SCRUTINY_SPDM_REQUEST, *PTR_SCRUTINY_SPDM_REQUEST;
#else
typedef struct _SCRUTINY_SPDM_REQUEST
{
	U8 Reserved[1024];
	
} SCRUTINY_SPDM_REQUEST, *PTR_SCRUTINY_SPDM_REQUEST;

#endif


typedef enum
{
    EXP_HEALTH_LOGS_RAW = 1,
    EXP_HEALTH_LOGS_DECODED,
    EXP_HEALTH_LOGS_RAW_MEMREAD,
} EXP_HEALTH_LOG_TYPE, *PTR_EXP_HEALTH_LOG_TYPE;

#define MAX_PHYS                                24

/* Inventory levels are 4 bit currently*/
#define SCRUTINY_CONTROLLER_IVN_PROPERTIES_FLAG     (0)
#define SCRUTINY_CONTROLLER_IVN_TOPOLOGY_FLAG       (1)
#define SCRUTINY_CONTROLLER_IVN_HEALTH_FLAG         (2)
#define SCRUTINY_CONTROLLER_IVN_ALL                 (4)


typedef struct _SCRUTINY_CONTROLLER_PROPERTIES
{
    U16         VendorId;
    U16         DeviceId;
    U16         SubVendorId;
    U16         SubSystemId;
    U8          PcieWidth;
    U8          PcieSpeed;
    U16         IOCTemperature;
    U8          IOCTemperatureUnit;
    U8          BoardTemperatureUnit;
    U16         BoardTemperature;
    U8          IOCSpeed;
    U8          Reserved1;
    U16         Reserved2;

} SCRUTINY_CONTROLLER_PROPERTIES, *PTR_SCRUTINY_CONTROLLER_PROPERTIES;

typedef struct _SCRUITNY_CONTROLLER_TOPOLOGY
{
    U32         DriveCount;
    U32         ExpanderCount;
    U16         NumPhys;
    U16         Reserved1;

} SCRUITNY_CONTROLLER_TOPOLOGY, *PTR_SCRUITNY_CONTROLLER_TOPOLOGY;

typedef struct _SCRUTINY_CONTROLLER_LINK_STATUS
{
    U8          ProgrammedLinkRate;
    U8          HardwareLinkRate;
    U8          NegotiatedLinkRate;
    U8          Reserved1;

} SCRUTINY_CONTROLLER_LINK_STATUS, *PTR_SCRUTINY_CONTROLLER_LINK_STATUS;

typedef struct _SCRUTINY_CONTROLLER_HEALTH
{
     SCRUTINY_CONTROLLER_LINK_STATUS      LinkStatus [MAX_PHYS];
     SCRUTINY_SAS_ERROR_COUNTERS          ErrorCounters [MAX_PHYS];

} SCRUTINY_CONTROLLER_HEALTH, *PTR_SCRUTINY_CONTROLLER_HEALTH;

typedef struct _SCRUTINY_CONTROLLER_INVENTORY
{
    SCRUTINY_CONTROLLER_PROPERTIES      ControllerProperties;
    SCRUITNY_CONTROLLER_TOPOLOGY        ControllerTopology;
    SCRUTINY_CONTROLLER_HEALTH          ControllerHealth;

} SCRUTINY_CONTROLLER_INVENTORY, *PTR_SCRUTINY_CONTROLLER_INVENTORY;

typedef struct _SCRUTINY_SWITCH_ERROR_COUNTERS
{
    U32 PortReceiverErrors;
    U32 BadTLPErrors;
    U32 BadDLLPErrors;
    U32 RecoveryDiagnosticsErrors;
    U32 LinkDownCount;
    U32 SerdesErrors[16];   // the max link width is 16 per port

    U32 LinkSpeed;   /* for debug purpose, delete in future */

} SCRUTINY_SWITCH_ERROR_COUNTERS, *PTR_SCRUTINY_SWITCH_ERROR_COUNTERS;


typedef struct _SCRUTINY_SWITCH_ADVANCED_ERROR_STATUS
{

    struct
    {
        U8      CorrectableErrorDetected : 1;
        U8      NonFatalErrorDetected : 1;
        U8      FatalErrorDetected  : 1;
        U8      UnsupportedRequestDetected : 1;
        U8      Reserved:  4;
    } ErrorStatus;   //  device control and status register - offset 0x70 bit 16 - 19

    union  __UNCORRECTABLE_ERROR_STATUS_U
    {
        struct  __UNCORRECTABLE_ERROR_STATUS_BITS
        {
            U32      Reserved0 : 4;
            U32      DataLinkProtocolError : 1;
            U32      SurpriseDownError : 1;
            U32      Reserved1 : 6;
            U32      PoisedTLPStatus : 1;
            U32      FlowControlProtocolError : 1;
            U32      CompletionTimeout : 1;
            U32      CompleterAbortStatus : 1;
            U32      UnexpectedCompletionStatus : 1;
            U32      ReceiverOverflowStatus : 1;
            U32      MalformedTLPStatus : 1;
            U32      ECRCError : 1;
            U32      UnsupportedRequestError : 1;
            U32      ACSViolationStatus : 1;
            U32      UncorrectableInternalError : 1;
            U32      Reserved2 : 9 ;
        }  Bits;

        U32     word;
    } Uncorrectable;  // Uncorrectable Error Status register - offset 0xFB8


    union   __CORRECTABLE_ERROR_STATUS_U
    {
        struct
        {
            U32      ReceiverError : 1;
            U32      Reserved0 : 5;
            U32      BadTLP : 1;
            U32      BadDLLP : 1;
            U32      ReplayNumRollOver : 1;
            U32      Reserved2 : 3;
            U32      ReplayTimerTimeout : 1;
            U32      AdvisoryNonFatalError : 1;
            U32      CorrectedInternalError : 1;
            U32      HeaderLogOverflow : 1;
            U32      Reserved : 16;
        } Bits;

        U32     word;
    } Correctable;   // Correctable Error Status register - offset  0xFC4




} SCRUTINY_SWITCH_ADVANCED_ERROR_STATUS, *PTR_SCRUTINY_SWITCH_ADVANCED_ERROR_STATUS;


#define ATLAS_PMG_MAX_PHYS                    (128)

typedef struct _SCRUTINY_SWITCH_ERROR_STATISTIC
{
    SCRUTINY_SWITCH_ERROR_COUNTERS          ErrorCounters;
    SCRUTINY_SWITCH_ADVANCED_ERROR_STATUS   AdvancedErrorStatus;
} SCRUTINY_SWITCH_ERROR_STATISTICS, *PTR_SCRUTINY_SWITCH_ERROR_STATISTICS;


typedef enum
{
    EXP_COREDUMP_RAW = 1,    // Get the RAW coredump data
    EXP_COREDUMP_DECODED,   // Get the decoded, human-readable format for coredump data
} EXP_COREDUMP_TYPE, *PTR_EXP_COREDUMP_TYPE;

// physical phy + virtual phy
#define EXP_MAX_PHYS                          (64)

#define SCRUITNY_EXPANDER_ENCL_STATUS_UNRECO  (0x01)
#define SCRUITNY_EXPANDER_ENCL_STATUS_CRIT    (0x02)
#define SCRUITNY_EXPANDER_ENCL_STATUS_NONCRIT (0x04)

typedef enum _SCRUTINY_EXP_PHY_ATTACHED_DEVICE_TYPE
{
    DEVICE_TYPE_NO_DEVICE  =  0,
    DEVICE_TYPE_SAS_DEVICE,
    DEVICE_TYPE_SATA_DEVICE,
    DEVICE_TYPE_EXPANDER_DEVICE,
}  SCRUTINY_EXP_PHY_ATTACHED_DEVICE_TYPE;


/** Supported link rates */
#define     SCRUTINY_SMPT_LINK_RATE_UNKNOWN           (0x0)
#define     SCRUTINY_SMPT_LINK_RATE_1_5_GBPS          (0x8)
#define     SCRUTINY_SMPT_LINK_RATE_3_0_GBPS          (0x9)
#define     SCRUTINY_SMPT_LINK_RATE_6_0_GBPS          (0xA)
#define     SCRUTINY_SMPT_LINK_RATE_12_0_GBPS         (0xB)


typedef struct _SCRUTINY_EXPANDER_LINK_STATUS
{
    U8          ProgrammedMinLinkRate:      4;
    U8          ProgrammedMaxLinkRate:      4;
    U8          HardwareMinLinkRate:        4;
    U8          HardwareMaxLinkRate:        4;
    U8          NegotiatedLinkRate;
    SCRUTINY_EXP_PHY_ATTACHED_DEVICE_TYPE   AttachedDeviceType;
    U8          ZoneGroup;
    U8          EDFBEnable:                  1;
    U8          VirtualPhy:                  1;
    U8          Reserved0:                   6;
} SCRUTINY_EXPANDER_LINK_STATUS, *PTR_SCRUTINY_EXPANDER_LINK_STATUS;


typedef struct _SCRUTINY_EXPANDER_PHY_INFO
{
    U8                                   TotalPhys;
    SCRUTINY_EXPANDER_LINK_STATUS        LinkStatus [EXP_MAX_PHYS];
    SCRUTINY_SAS_ERROR_COUNTERS          ErrorCounters [EXP_MAX_PHYS];
} SCRUTINY_EXPANDER_PHY_INFO, *PTR_SCRUTINY_EXPANDER_PHY_INFO;


/* The max desriptors supported by expander can be read from SMP REPORT General
     Here, we just supports 128
  */
#define SCRUTINY_MAX_SELF_CONFIG_STATUS_DESCRIPTORS    (128)

/* Self Config Status Descriptor */
typedef struct _SCRUTINY_SMPT_STATUS_DESCRIPTOR
{
    U8             StatusType;          /* Descriptor Status Type */
    U8             Final          :1;   /* 0x01 Final Bit Field */
    U8             Reserved07     :7;   /* 0x01 reserved bits 1 to 7 */
    U8             PhyIdentifier;       /* Phy Identifier */
    U8             Reserved08;
    U64            SASAddress;          /* SAS Address of the expander
                                           reporting the status */
} SCRUTINY_SMPT_STATUS_DESCRIPTOR, *PTR_SCRUTINY_SMPT_STATUS_DESCRIPTOR;

typedef struct _SCRUTINY_EXPANDER_SELF_CONFIG_STATUS
{
    U32                                 TotalSelfConfigStatusDescriptors;
    SCRUTINY_SMPT_STATUS_DESCRIPTOR     StatusDescriptors[SCRUTINY_MAX_SELF_CONFIG_STATUS_DESCRIPTORS];

 } SCRUTINY_EXPANDER_SELF_CONFIG_STATUS, *PTR_SCRUTINY_EXPANDER_SELF_CONFIG_STATUS;

typedef struct _SCRUTINY_EXPANDER_HEALTH
{
    U32                             Temperature;
    U8                              EnclosureStatus;
    SCRUTINY_EXPANDER_PHY_INFO      PhyInfo;
    SCRUTINY_EXPANDER_SELF_CONFIG_STATUS    SelfConfigStatus;
}  SCRUTINY_EXPANDER_HEALTH, *PTR_SCRUTINY_EXPANDER_HEALTH;

typedef enum _SCRUTINY_EXPANDER_HEALTH_ERROR_CODE
{
  SCRUTINY_EXPANDER_HEALTH_ERROR_RESERVED = 0,
  SCRUTINY_EXPANDER_HEALTH_ERROR_NOT_DISCOVERED,
  SCRUTINY_EXPANDER_HEALTH_ERROR_CONFIG_FILE,
  SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_TEMPERATURE,
  SCRUTINY_EXPANDER_HEALTH_ERROR_ENCL_STATUS,
  SCRUTINY_EXPANDER_HEALTH_ERROR_SASADDR_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_FW_VER_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_TOTAL_PHYS_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_SCE_STATUS,
  SCRUTINY_EXPANDER_HEALTH_ERROR_NOT_LINK_UP,
  SCRUTINY_EXPANDER_HEALTH_ERROR_PROGRAMMED_MAX_LINK_RATE_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_PROGRAMMED_MIN_LINK_RATE_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_HARDWARE_MAX_LINK_RATE_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_HARDWARE_MIN_LINK_RATE_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_NEGOTIATED_LINK_RATE_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_INVALIDDWORDCOUNT,
  SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_RUNNINGDISPARITYERRORCOUNT,
  SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_LOSSDWORDSYNCHCOUNT,
  SCRUTINY_EXPANDER_HEALTH_ERROR_OVER_PHYRESETPROBLEMCOUNT,
  SCRUTINY_EXPANDER_HEALTH_ERROR_ATTACHED_DEVICE_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_EDFB_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_ZONE_GROUP_MISMATCH,
  SCRUTINY_EXPANDER_HEALTH_ERROR_MAX_ITEM
} SCRUTINY_EXPANDER_HEALTH_ERROR_CODE;

#define SCRUTINY_MAX_EXP_HEALTH_ERROR_DWORD   (SCRUTINY_EXPANDER_HEALTH_ERROR_MAX_ITEM / 32 + 1)
typedef struct _SCRUTINY_EXPANDER_HEALTH_ERROR_INFO
{
    U32             ErrorDwords[SCRUTINY_MAX_EXP_HEALTH_ERROR_DWORD];
}  SCRUTINY_EXPANDER_HEALTH_ERROR_INFO, *PTR_SCRUTINY_EXPANDER_HEALTH_ERROR_INFO;



/* bit flags to determine what expander log to be collected*/
#define SCRUTINY_EXP_LOGS_BIT_TRACE_BUFFER                  (0x00000001)
#define SCRUTINY_EXP_LOGS_BIT_HEALTH_LOG                    (0x00000002)
#define SCRUTINY_EXP_LOGS_BIT_CORE_DUMP                     (0x00000004)
#define SCRUTINY_EXP_LOGS_BIT_FW_CLI                        (0x00000008)

typedef enum __PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE
{

    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_128_BYTES = 0x0,
    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_256_BYTES,
    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_512_BYTES,
    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_1024_BYTES,
    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_2048_BYTES

} PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE;

typedef enum __PCI_DEVICE_LINK_WIDTH
{

    PCI_DEVICE_LINK_WIDTH_RESERVED  = 0x0,
    PCI_DEVICE_LINK_WIDTH_LINK_DOWN = 0x0,
    PCI_DEVICE_LINK_WIDTH_x1        = 0x1,
    PCI_DEVICE_LINK_WIDTH_x2        = 0x2,
    PCI_DEVICE_LINK_WIDTH_x4        = 0x4,
    PCI_DEVICE_LINK_WIDTH_x8        = 0x8,
    PCI_DEVICE_LINK_WIDTH_x16       = 0x10

} PCI_DEVICE_LINK_WIDTH;

typedef enum __PCI_LINK_MODE_TYPE
{

    PCI_LINK_MODE_TYPE_COMMON_CLOCK     = 0x00,
    PCI_LINK_MODE_TYPE_SRIS             = 0x01,
    PCI_LINK_MODE_TYPE_SSC_ISOLATION    = 0x02,
    PCI_LINK_MODE_TYPE_INVALID          = 0x03

} PCI_LINK_MODE_TYPE;

typedef enum __PCI_DEVICE_LINK_SPEED
{
    PCI_DEVICE_LINK_SPEED_LINK_DOWN = 0x00,
    PCI_DEVICE_LINK_SPEED_GEN_1,
    PCI_DEVICE_LINK_SPEED_GEN_2,
    PCI_DEVICE_LINK_SPEED_GEN_3,
    PCI_DEVICE_LINK_SPEED_GEN_4

} PCI_DEVICE_LINK_SPEED;


typedef enum _SWITCH_PORT_TYPE
{
    SWITCH_PORT_TYPE_DOWNSTREAM = 0x00,
    SWITCH_PORT_TYPE_FABRIC,
    SWITCH_PORT_TYPE_MANAGEMENT,
    SWITCH_PORT_TYPE_UPSTREAM,
    SWITCH_PORT_TYPE_UNKNOWN,
    SWITCH_PORT_TYPE_UPSTREAM_BSW,
    SWITCH_PORT_TYPE_DOWNSTREAM_BSW,
    SWITCH_PORT_TYPE_UNKNOWN_BSW

} SWITCH_PORT_TYPE;

typedef struct _SWITCH_DS_PORT_ATTACHED_DEVICE
{
    U32     DataValid;// indicate if below device data is valid.
    U16     VendorId;
    U16     DeviceId;
    U8      Revision;
    U8      ProgIF;
    U8      SubClassCode;
    U8      ClassCode;
    U16     SubVendorID;
    U16     SubDeviceID;

} SWITCH_DS_PORT_ATTACHED_DEVICE;

/** Downstream port properties */
typedef struct _SWITCH_DS_PORT_PROP
{
    U8  GblBusSec;                         /**< Global DS secondary bus */
    U8  GblBusSub;                         /**< Global DS subordinate bus */
    U8  HostPortNum;                       /**< Host port number assigned to, if any */
    U8  Reserved;
    U16 ChassisPhysSlotNum;                /**< Physical slot number assigned, if any */
    U16 Reserved1;
    SWITCH_DS_PORT_ATTACHED_DEVICE AttachedDevice;/**if DownStream port is Up, then get the attached device info, now data valid only for base mode*/
} SWITCH_DS_PORT_PROP, *PTR_SWITCH_DS_PORT_PROP;

/** Host port properties */
typedef struct _SWITCH_HOST_PORT_PROP
{
    U32  DsPortMask[4];                    /**< Bitmask of DS ports assigned to host */
} SWITCH_HOST_PORT_PROP, *PTR_SWITCH_HOST_PORT_PROP;




typedef struct __SWITCH_PORT_CONFIGURATION
{

    SWITCH_PORT_TYPE                        PortType;
    U32                                     PortNumber;
    U32                                     GID;                    /**< Unique Global ID of port */
    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE      MaxReadRequestSize;
    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE      MaxPayloadSize;
    PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE      MaxPayloadSizeSupport;
    PCI_DEVICE_LINK_SPEED                   NegotiatedLinkSpeed;
    PCI_DEVICE_LINK_SPEED                   MaxLinkSpeed;
    PCI_DEVICE_LINK_WIDTH                   NegotiatedLinkWidth;
    PCI_DEVICE_LINK_WIDTH                   MaxLinkWidth;
    PCI_LINK_MODE_TYPE                      LinkModeType;

    union
    {
        SWITCH_DS_PORT_PROP   Ds;             /**< Downstream port-specific */
        SWITCH_HOST_PORT_PROP Host;           /**< Host-port specific */
    } u;

} SWITCH_PORT_CONFIGURATION, *PTR_SWITCH_PORT_CONFIGURATION;



typedef struct _SCRUTINY_SWITCH_PORT_PROPERTIES
{
    U32                                     ToTalPhyNum;
    U32                                     TotalPortConfigEntry;
    U32                                     TotalHostPort;
    U32                                     TotalDownStreamPort;
    U32                                     TotalFabricPort;
    U32                                     TotalManagementPort;
    SWITCH_PORT_CONFIGURATION               PortConfigurations[ATLAS_PMG_MAX_PHYS];
} SCRUTINY_SWITCH_PORT_PROPERTIES, *PTR_SCRUTINY_SWITCH_PORT_PROPERTIES;


/* data structure definition for switch health monitor */
typedef struct _SCRUTINY_PCI_PORT_STAUTS
{
    BOOLEAN                                 isEnabled;
    PCI_DEVICE_LINK_SPEED                   NegotiatedLinkSpeed;
    PCI_DEVICE_LINK_SPEED                   MaxLinkSpeed;
    PCI_DEVICE_LINK_WIDTH                   NegotiatedLinkWidth;
    PCI_DEVICE_LINK_WIDTH                   MaxLinkWidth;
    SCRUTINY_SWITCH_ADVANCED_ERROR_STATUS   AerState;
    SCRUTINY_SWITCH_ERROR_COUNTERS          ErrorCounter;
}  SCRUTINY_PCI_PORT_STATUS, *PTR_SCRUTINY_PCI_PORT_STATUS;

typedef struct _SCRUTINY_SWITCH_HEALTH
{
    U32             Temperature;
    U8              EnclosureStatus;
    SCRUTINY_PCI_PORT_STATUS    PortStatus[ATLAS_PMG_MAX_PHYS];
} SCRUTINY_SWITCH_HEALTH, *PTR_SCRUTINY_SWITCH_HEALTH;


typedef enum _SCRUTINY_SWITCH_HEALTH_ERROR_CODE
{
  SCRUTINY_SWITCH_HEALTH_ERROR_RESERVED = 0,
  SCRUTINY_SWITCH_HEALTH_ERROR_NOT_DISCOVERED,
  SCRUTINY_SWITCH_HEALTH_ERROR_CONFIG_FILE,
  SCRUTINY_SWITCH_HEALTH_ERROR_OVER_TEMPERATURE,
  SCRUTINY_SWITCH_HEALTH_ERROR_ENCL_STATUS,
  SCRUTINY_SWITCH_HEALTH_ERROR_SASADDR_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_FW_VER_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_TOTAL_PORTS_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_TOTAL_HOST_PORTS_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_NOT_LINK_UP,
  SCRUTINY_SWITCH_HEALTH_ERROR_MAX_LINK_RATE_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_NEGOTIATED_LINK_RATE_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_MAX_LINK_WIDTH_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_NEGOTIATED_LINK_WIDTH_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_OVER_PORTRECEIVERERRORS,
  SCRUTINY_SWITCH_HEALTH_ERROR_OVER_BADTLPERRORS,
  SCRUTINY_SWITCH_HEALTH_ERROR_OVER_BADDLLPERRORS,
  SCRUTINY_SWITCH_HEALTH_ERROR_OVER_RECOVERYDIAGNOSTICSERRORS,
  SCRUTINY_SWITCH_HEALTH_ERROR_OVER_LINKDOWNCOUNT,
  SCRUTINY_SWITCH_HEALTH_ERROR_PORT_TYPE_MISMATCH,
  SCRUTINY_SWITCH_HEALTH_ERROR_UNCORR_ERR,
  SCRUTINY_SWITCH_HEALTH_ERROR_MAX_ITEM
} SCRUTINY_SWITCH_HEALTH_ERROR_CODE;

#define SCRUTINY_MAX_SWITCH_HEALTH_ERROR_DWORD   (SCRUTINY_SWITCH_HEALTH_ERROR_MAX_ITEM / 32 + 1)
typedef struct _SCRUTINY_SWITCH_HEALTH_ERROR_INFO
{
    U32             ErrorDwords[SCRUTINY_MAX_SWITCH_HEALTH_ERROR_DWORD];
}  SCRUTINY_SWITCH_HEALTH_ERROR_INFO, *PTR_SCRUTINY_SWITCH_HEALTH_ERROR_INFO;

#define SCRUTINY_HBA_HEALTH_MON_IOC_CONFIG_MISMATCH                 (0x00000001)
#define SCRUTINY_HBA_HEALTH_MON_IOC_PCIE_LINK_MISMATCH              (0x00000002)
#define SCRUTINY_HBA_HEALTH_MON_IOC_OVER_TEMPERATURE                (0x00000004)
#define SCRUTINY_HBA_HEALTH_MON_IOC_DEVICE_COUNT_MISMATCH           (0x00000008)


#define SCRUTINY_HBA_HEALTH_MON_PHY_LINK_MISMATCH                   (0x00010000)
#define SCRUTINY_HBA_HEALTH_MON_PHY_ERROR_COUNTER_MISMATCH          (0x00020000)


typedef struct _PCIE_ONE_PORT_RX_EQ
{
    U32     VGA;
    U32     AEQ;
    S32     DfeTap1P;
    S32     DfeTap1N;
    S32     DfeTap2;
    S32     DfeTap3;
    S32     DfeTap4;
    S32     DfeTap5;
    S32     DfeTap6;
    S32     DfeTap7;
    S32     DfeTap8;
    S32     DfeTap9;

} PCIE_ONE_PORT_RX_EQ, *PTR_PCIE_ONE_PORT_RX_EQ;

typedef struct _SCRUTINY_SWITCH_PORT_RX_EQ_STATUS
{
    U32                                     ToTalPhyNum;
    U32                                     TotalValidEntry;
    PCIE_ONE_PORT_RX_EQ                     PortRxEqs[ATLAS_PMG_MAX_PHYS];
} SCRUTINY_SWITCH_PORT_RX_EQ_STATUS, *PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS;

typedef struct _PCIE_ONE_PORT_TX_COEFF
{
    U32     PreCursorNear;
    U32     MainCursorNear;
    U32     PostCursorNear;
    U32     PreCursorFar;
    U32     MainCursorFar;
    U32     PostCursorFar;

} PCIE_ONE_PORT_TX_COEFF, *PTR_PCIE_ONE_PORT_TX_COEFF;

typedef struct _SCRUTINY_SWITCH_PORT_TX_COEFF
{
    U32                                     ToTalPhyNum;
    U32                                     TotalValidEntry;
    PCIE_ONE_PORT_TX_COEFF                  PortTxCoeffs[ATLAS_PMG_MAX_PHYS];
} SCRUTINY_SWITCH_PORT_TX_COEFF, *PTR_SCRUTINY_SWITCH_PORT_TX_COEFF;


typedef struct _PCIE_ONE_PORT_HW_EYE_FLAG
{
    U32             PortActive     :1;   /* 0x01 Port Active Bit Field */
    U32             PollingDone    :1;    /*check if the port finish hardware eye test.*/
    U32             Reserved30     :30;   /*  reserved bits  */

} PCIE_ONE_PORT_HW_EYE_FLAG, *PTR_PCIE_ONE_PORT_HW_EYE_FLAG;


typedef struct _PCIE_ONE_PORT_HW_EYE
{
     PCIE_ONE_PORT_HW_EYE_FLAG     Flag;
    double  EyeLeft; 
    double  EyeRight; 
    double  EyeUp; 
    double  EyeDown;

} PCIE_ONE_PORT_HW_EYE, *PTR_PCIE_ONE_PORT_HW_EYE;

typedef struct _SCRUTINY_SWITCH_PORT_HW_EYE
{
    U32                                     ToTalPhyNum;
    U32                                     TotalValidEntry;
    U32                                     GlobalPollingDone; //temperally save polling result.
    PCIE_ONE_PORT_HW_EYE                    PortHwEye[ATLAS_PMG_MAX_PHYS];
} SCRUTINY_SWITCH_PORT_HW_EYE, *PTR_SCRUTINY_SWITCH_PORT_HW_EYE;

typedef enum _SCRUTINY_SWITCH_HW_ENABLE_LANE_NUM
{
    SBL_HW_ENABLE_LANE_NUM_UNKNOWN = 0,
    SBL_HW_ENABLE_LANE_NUM_96,
    SBL_HW_ENABLE_LANE_NUM_80,
    SBL_HW_ENABLE_LANE_NUM_64,
    SBL_HW_ENABLE_LANE_NUM_RESERVED,
    SBL_HW_ENABLE_LANE_NUM_48,
    SBL_HW_ENABLE_LANE_NUM_32,
    SBL_HW_ENABLE_LANE_NUM_24
    

} SCRUTINY_SWITCH_HW_ENABLE_LANE_NUM;


typedef struct _SCRUTINY_SWITCH_POWER_ON_SENSE
{
    U32                                     SBRCsSel;              //Serial Bootstrap ROM Device Select: ,
                                                                   //1'b0: Uses SPI_FLASH_CS_N,
                                                                   //1'b1: Uses SPI_CS_N[0]
    U32                                     SBLDisable;            //Serial Boot Loader Disable:  ,
                                                                   //1'b0: Serial Boot Loader enabled,
                                                                   //1'b1: Serial Boot Loader disabled .
    SCRUTINY_BAUD_RATE                      SdbBaudRate;           //Select power on default SDB UART baud rate.,
                                                                   //1'b0: 115,200,
                                                                   //1'b1: 19,200
    SCRUTINY_SWITCH_HW_ENABLE_LANE_NUM      EnableLaneNumber;      //HW enable Maximun lane number
                                                                   //0 = 96 Lanes ,
                                                                   //1 = 80 Lanes ,
                                                                   //2 = 64 Lanes ,
                                                                   //3 = Reserved ,
                                                                   //4 = 48 Lanes ,
                                                                   //5 = 32 Lanes ,
                                                                   //6 = 24 Lanes ,
    
    PCI_DEVICE_LINK_SPEED                   MaxLinkSpeed;          //Allowed PCIe Maximum Link Speed 
                                                                   //0 - Upto GEN4 PCIE Link speed, 
                                                                   //1 - Upto GEN3 PCIE Link speed
} SCRUTINY_SWITCH_POWER_ON_SENSE, *PTR_SCRUTINY_SWITCH_POWER_ON_SENSE;


/* max SBR supported */
#define SWITCH_MAX_SBR_LOCATIONS                     (5)
#define SWITCH_SPI_MAX_CS_SUPPORTED                  (4)

typedef enum _SCRUTINY_SWITCH_SBL_HW_BOOT_LOC
{
    SBL_HW_BOOT_LOC_UNKNOWN = 0,
    SBL_HW_BOOT_LOC_0000_0400,
    SBL_HW_BOOT_LOC_0004_0400,
    SBL_HW_BOOT_LOC_0008_0400,
    SBL_HW_BOOT_LOC_000C_0400,
    SBL_HW_BOOT_LOC_0010_0400
    

} SCRUTINY_SWITCH_SBL_HW_BOOT_LOC;


typedef enum _SCRUTINY_SWITCH_SBL_HW_STAT
{
    SBL_HW_STAT_UNKNOWN = 0,
    SBL_HW_STAT_CHECKSUM_PASS,
    SBL_HW_STAT_CHECKSUM_FAIL,
    SBL_HW_STAT_SIG_FAIL,
    SBL_HW_STAT_BOOT_LOADING,
    SBL_HW_STAT_BOOT_DISABLED
    

} SCRUTINY_SWITCH_SBL_HW_STAT;


typedef struct _SCRUTINY_SWITCH_CCR_STATUS
{
    U32                                     S0PcePerstN;           //Ccr clock Synchronized version of S0_PCE_PERST_N input PIN. -->Not needed for base mode
    U32                                     S1PcePerstN;           //Ccr clock Synchronized version of S1_PCE_PERST_N input PIN. -->Not needed for base mode
    U32                                     S2PcePerstN;           //Ccr clock Synchronized version of S2_PCE_PERST_N input PIN. -->Not needed for base mode
    U32                                     S3PcePerstN;           //Ccr clock Synchronized version of S3_PCE_PERST_N input PIN. -->Not needed for base mode
    SCRUTINY_SWITCH_SBL_HW_BOOT_LOC         SBLHwBootLoc;          //If SBLHwStat = 0, this field gives the HW Settings SPI Boot location:
                                                                   //0 = Loaded from SPI address 0x0000_0400 ,
                                                                   //1 = Loaded from SPI address 0x0004_0400 ,
                                                                   //2 = Loaded from SPI address 0x0008_0400 ,
                                                                   //3 = Loaded from SPI address 0x000C_0400 ,
                                                                   //4 = Loaded from SPI address 0x0010_0400 ,
    SCRUTINY_SWITCH_SBL_HW_STAT             SBLHWStat;             //HW Settings SPI Bootstrap Loader Status: ,
                                                                   //0 = CHECKSUM_PASS (Good Status) ,
                                                                   //1 = CHECKSUM_FAIL (Error Status) ,
                                                                   //2 = SIG_FAIL (Error Status),
                                                                   //3 = BOOT_LOADING (Error Status. Loading logic still running),
                                                                   //4 = BOOT_DISABLED (Loaded Defaults.  Set when SBL load is disabled),
    U32                                     PCDValid;              //Indicates that the Product Configuration Data (Bond Option Data) has been loaded correctly.
                                                                   //This bit should always be asserted if a proper Efuse read has occurred.
} SCRUTINY_SWITCH_CCR_STATUS, *PTR_SCRUTINY_SWITCH_CCR_STATUS;

/* definitions for report margin control capabilities */
#define  SCRUTINY_PCIE_REG_LM_MRGN_RSP_RMCC_MVOLTAGESUPP     (0x01)
#define  SCRUTINY_PCIE_REG_LM_MRGN_RSP_RMCC_MINDUDVOLTAGE    (0x02)
#define  SCRUTINY_PCIE_REG_LM_MRGN_RSP_RMCC_MINDLRTIMING     (0x04)
#define  SCRUTINY_PCIE_REG_LM_MRGN_RSP_RMCC_MSAMPLERPTMETHOD (0x08)
#define  SCRUTINY_PCIE_REG_LM_MRGN_RSP_RMCC_MINDERRSAMPLER   (0x10)

typedef struct _SCRUTINY_SWITCH_LANE_MARGIN_REQUEST
{
    U8      SwitchPort;    /* selected port number, if UP, receive number is  0x1, if DP, receive number is 0x6*/
    U16     Lanes;             /* bitmap indicates which lane in the port need to perform lane margining*/  
    U32     ErrorCount;   /* User defined error count */
    U32     NumTimeSteps;     /* User defined number of time steps to go, if 0, using the device defined */
    U32     NumVoltageSteps;  /* User defined number of time steps to go, if 0, using the device defined */    
}  SCRUTINY_SWITCH_LANE_MARGIN_REQUEST, *PTR_SCRUTINY_SWITCH_LANE_MARGIN_REQUEST;

typedef struct _SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE
{
    /* if MIndLeftRightTiming is set, indicate the last failure right margin execution status, if MIndLeftRightTiming is not set, 
       it means time Margining status and LeftTimeMarginStatus is ignored. 
    */
    U32     RightTimeMarginStatus;    
    /* The last right time step that succeed */
    U32     RightTimeMarginStep;
    U32     LeftTimeMarginStatus;
     /* The last left time step that succeed */
    U32     LeftTimeMarginStep;
     
    /* if MIndUpDownVoltage is  set, indicate the last failre UP margin execution status, if MIndUpDownVoltage is not set, 
       it means Voltage Margining status and DownVoltageMarginStatus is ignored. 
    */
    U32     UpVoltageMarginStatus;   
    /* The last UP Voltage step that succeed */
    U32     UpVoltageMarginStep;
    U32     DownVoltageMarginStatus;    
    /* The last Down Voltage step that succeed */
    U32     DownVoltageMarginStep;    
}  SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE, *PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE;


/** The beginning portion of a SBR including the HW signature
 *  and the SBR Index.
 */
typedef struct _SCRUTINY_ATLAS_SBR_HEADER
{
    U32 HwSignature;                   /* 00h */
    U32 PsbHwSettingsOffset;           /* 04h */
    U32 PsbHwSettingsSize;             /* 08h */
    U32 Psw0HwSettingsOffset;          /* 0Ch */
    U32 Psw0HwSettingsSize;            /* 10h */
    U32 Psw1HwSettingsOffset;          /* 14h */
    U32 Psw1HwSettingsSize;            /* 18h */
    U32 Psw2HwSettingsOffset;          /* 1Ch */
    U32 Psw2HwSettingsSize;            /* 20h */
    U32 Psw3HwSettingsOffset;          /* 24h */
    U32 Psw3HwSettingsSize;            /* 28h */
    U32 Psw4HwSettingsOffset;          /* 2Ch */
    U32 Psw4HwSettingsSize;            /* 30h */
    U32 Psw5HwSettingsOffset;          /* 34h */
    U32 Psw5HwSettingsSize;            /* 38h */
    U32 Reserved0Offset;               /* 3Ch */
    U32 Reserved0Size;                 /* 40h */
    U32 PswX2HwSettingsOffset;         /* 44h */
    U32 PswX2HwSettingsSize;           /* 48h */
    U32 SerdesHwSettingsOffset;        /* 4Ch */
    U32 SerdesHwSettingsSize;          /* 50h */
    U32 Reserved1Offset;               /* 54h */
    U32 Reserved1Size;                 /* 58h */

} SCRUTINY_ATLAS_SBR_HEADER, *PTR_SCRUTINY_ATLAS_SBR_HEADER;


#endif /* __SCRUTINY_DEF__H__ */

