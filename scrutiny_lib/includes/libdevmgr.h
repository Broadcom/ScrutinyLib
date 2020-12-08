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

#ifndef __LIB_DEVICE_MANAGER__H__
#define __LIB_DEVICE_MANAGER__H__


#ifdef OS_UEFI

    typedef struct _SCRUTINY_PCI_INFO
    {
        EFI_PCI_IO_PROTOCOL             *DeviceIo;
        EFI_HANDLE                      ControllerHandle;
        UINT8                           IoBarIndex;
        UINT8                           MemBarIndex;
    } SCRUTINY_PCI_INFO;

    typedef struct _MEMORY_DESC
    {
        VOID                        *PtrMemory;
        EFI_PHYSICAL_ADDRESS        PhysicalAddress;
        VOID                        *Mapping;
        UINTN                       SizeInBytes;
    }

    MEMORY_DESC;

#endif


typedef struct  _SCRUTINY_DEVICE SCRUTINY_DEVICE, *PTR_SCRUTINY_DEVICE;


typedef struct __SCRUTINY_SCSI_HANDLE
{

    #ifdef OS_WINDOWS

        HANDLE                              SgHbaHandle;
        U32                                 TargetId;
        U32                                 PathId;
        PCSTR                               SourceAdapterDeviceId;
        PSP_DEVICE_INTERFACE_DETAIL_DATA    InterfaceData;

    #elif defined (OS_LINUX) || defined (OS_FREEBSD)

        U32                     Index;

        #if defined (OS_FREEBSD)
            struct cam_device   *PtrSgCamDeviceHandle;
        #else
            int                 SgDeviceHandle;
        #endif

    #endif

    /* We can have this inband support only when the controller is available. */
    PTR_SCRUTINY_DEVICE          PtrMpiAdapterDevice;
    U16                          MpiSasDeviceHandle;
    U64                          MpiSMPEnclosureWWID;

    SCRUTINY_PCI_ADDRESS         AdapterPCIAddress;     //Adapter where this SCSI device is attached into

} SCRUTINY_SCSI_HANDLE, *PTR_SCRUTINY_SCSI_HANDLE;

#ifdef OS_UEFI

typedef struct _SCRUTINY_EFI_FRAME_HEADER
{
    U32             count;
    U32             count1;
    UINT64          tempAddr;
    UINT64          tempAddr1;
    MEMORY_DESC     dataBuffer;
    MEMORY_DESC     dataBuffer1;

}SCRUTINY_EFI_FRAME_HEADER, PTR_SCRUTINY_EFI_FRAME_HEADER;


typedef struct _SCRUTINY_EFI_MPI2_HANDLE
{
    UINT64              OriginalAttributes;
    MEMORY_DESC         RequestBuffer;
    MEMORY_DESC         ReplyBuffer;
    MEMORY_DESC         ReplyFreeQueue;
    MEMORY_DESC         ReplyDescriptorPostQueue;
    MEMORY_DESC         SenseInfoBuffer;
    U16                 ReplyFreeHostIndex;
    U16                 ReplyPostHostIndex;

}SCRUTINY_EFI_MPI2_HANDLE, *PTR_SCRUTINY_EFI_MPI2_HANDLE;

typedef struct _SCRUTINY_EFI_MPI3_HANDLE
{
    UINT64                  OriginalAttributes;


    U8                      AdmRepEphase;

    U16                     NumAdmReqs;
    U16                     NumAdmReps;

    MEMORY_DESC             AdmReqQBase;
    U16                     AdmReqQNumEles;
    U32                     AdmReqQPI;
    U32                     AdmReqQCI;

    MEMORY_DESC             AdmRepQBase;
    U16                     AdmRepQNumEles;
    U32                     AdmRepQCI;

    U32                     ReadyTimeOut;

    MEMORY_DESC             RepFreeQBase;
    U16                     NumOfRepBuffs;
    U16                     ReplyFreeQNumEles;
    U16                     ReplyFreeHostIndex;

    MEMORY_DESC             SenseBufFreeQBase;
    U16                     NumOfSenseBuffs;
    U16                     SenseBuffFreeQNumEles;
    U16                     SenseBuffHostIndex;

    MEMORY_DESC             FreeBufferBase;

} SCRUTINY_EFI_MPI3_HANDLE, *PTR_SCRUTINY_EFI_MPI3_HANDLE;

#endif

typedef struct _SCRUTINY_CONTROLLER_HANDLE
{
    #if defined (OS_LINUX) && !defined (OS_VMWARE)
        U32 IocNumber;
        char IoctlDeviceNode[MAX_PATH_LENGTH];
    #endif

    #if defined (OS_VMWARE)
        U32 IocNumber;
        U32 VmkMajorNumber;
        U32 VmkMinorNumber;
    #endif

    #ifdef OS_WINDOWS
        U32 IocNumber;
    #endif

    #ifdef OS_UEFI

    SCRUTINY_PCI_INFO   ScrutinyPciInfo;

    union {

    SCRUTINY_EFI_MPI2_HANDLE ScrutinyEfiMpi2Handle;
    SCRUTINY_EFI_MPI3_HANDLE ScrutinyEfiMpi3Handle;

    }EfiHandle;

    #endif

} SCRUTINY_CONTROLLER_HANDLE, *PTR_SCRUTINY_CONTROLLER_HANDLE;

#if !defined (OS_BMC) && !defined (OS_VMWARE)
typedef struct __SCRUTINY_PCI_HANDLE
{

    U32                 VendorId;
    U32                 DeviceId;
    U32                 Revision;

    SCRUTINY_PCI_ADDRESS       PciLocation;

    IAL_PCI_BAR_REGIONS     BarRegions[MAX_BAR_REGIONS];
    IAL_PCI_CONFIG_SPACE    ConfigSpace;
} SCRUTINY_PCI_HANDLE, *PTR_SCRUTINY_PCI_HANDLE;
#endif

typedef struct __SCRUTINY_LIB_OOB_Flags
{
    U8 Channeladdress;
    U8 DeviceAddress;
    U8 AppMsgTag;
    U8 EID; // EID of the PCIe endpoint

}SCRUTINY_LIB_OOB_Flags ,*PTR_SCRUTINY_LIB_OOB_Flags;


typedef struct __SCRUTINY_LIB_OOB_HANDLE
{
    U16 	OobDeviceIndex;
	U8 		AppMsgTag;
    U8 		EID;  // EID of the PCIe endpoint	
	U32 	RegisteredID ;  // RegisterID of the driver
	U32     MaxVDMPacketSize;

} SCRUTINY_LIB_OOB_HANDLE , *PTR_SCRUTINY_LIB_OOB_HANDLE;


#if defined (OS_VMWARE)

typedef struct __SCRUTINY_PCI_HANDLE
{

    U32                 VendorId;
    U32                 DeviceId;
    U32                 Revision;

} SCRUTINY_PCI_HANDLE, *PTR_SCRUTINY_PCI_HANDLE;

#endif 

struct  _SCRUTINY_DEVICE
{

    SCRUTINY_HANDLE_TYPE    HandleType;

    union
    {

        SCRUTINY_LIB_OOB_HANDLE         OobHandle;

        #if !defined(OS_BMC)
        SCRUTINY_CONTROLLER_HANDLE      ControllerHandle;

        SCRUTINY_SCSI_HANDLE            ScsiHandle;

        SCRUTINY_IAL_SERIAL_HANDLE      SdbHandle;

        SCRUTINY_PCI_HANDLE             PciHandle;
        #endif

    } Handle;

    SCRUTINY_DEVICE_INFO            DeviceInfo;

    SCRUTINY_PRODUCT_FAMILY         ProductFamily;

};


typedef struct _SCRUTINY_DEVICE_MANAGER
{

    U32                             DeviceCount;
	U32								EId[8];
    PTR_SCRUTINY_DEVICE             *PtrDeviceList;
    SCRUTINY_DISCOVERY_TYPE         DiscoveryFlag;
    SCRUTINY_LIB_OOB_Flags          OobFlag;
    PTR_SCRUTINY_DISCOVERY_PARAMS   PtrDiscoveryParams;

} SCRUTINY_DEVICE_MANAGER, *PTR_SCRUTINY_DEVICE_MANAGER;

#define ITEM_NAME(x)        #x

typedef struct  _ITEMS_LOOKUP_TABLE
{
    char*   ItemName;
    S32     ItemIndex;
} ITEMS_LOOKUP_TABLE, PTR_ITEMS_LOOKUP_TABLE;

SCRUTINY_STATUS ldmiAddScrutinyDevice (__INOUT__ PTR_SCRUTINY_DEVICE_MANAGER PtrScrutinyLibManager, __IN__ PTR_SCRUTINY_DEVICE PtrDeviceEntryStruct);
SCRUTINY_STATUS ldmiFreeDevices (__INOUT__ PTR_SCRUTINY_DEVICE_MANAGER PtrDeviceManager);
SCRUTINY_STATUS ldmiInitializeLibraryDeviceManager (__INOUT__ PTR_SCRUTINY_DEVICE_MANAGER *PPtrDeviceManager);


#endif /* __LIB_DEVICE_MANGER__H__ */


