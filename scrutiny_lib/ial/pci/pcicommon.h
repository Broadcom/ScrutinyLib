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


#ifndef __PCI_COMMON__H__
#define __PCI_COMMON__H__

#define  MAX_BAR_REGIONS            (6)

#define PCI_CONFIG_SPACE_OFFSET_VENDOR_ID       (0x00)

#define PCI_CONFIG_SPACE_OFFSET_DEVICE_ID       (0x02)

#define PCI_CONFIG_SPACE_OFFSET_COMMAND         (0x04)

#define PCI_CONFIG_SPACE_OFFSET_STATUS          (0x06)

#define PCI_CONFIG_SPACE_OFFSET_REVISION        (0x08)

#define PCI_CONFIG_SPACE_OFFSET_BAR_0           (0x10)

#define PCI_CONFIG_SPACE_OFFSET_BAR_5           (0x24)

#define PCI_CONFIG_SPACE_OFFSET_MEM_BAR_MASK           (0xFFFFFFF0)
#define PCI_CONFIG_SPACE_OFFSET_IO_BAR_MASK            (0xFFFFFFFC)

typedef struct __IAL_PCI_CONFIG_SPACE_P2P
{

    U16     VendorId;
    U16     DeviceId;

    U16     Command;
    U16     Status;

    U8      Revision;
    U8      ClassCode;
    U16     SubClassCode;

    U8      CacheLineSize;
    U8      PrimaryLatencyTimer;
    U8      HeaderType;
    U8      BIST;
    U32     BAR0;
    U32     BAR1;
    U8      PrimaryBusNumber;
    U8      SecondaryBusNumber;
    U8      SubordinateBusNumber;
    U8      SecondaryLatencyTimer;
    U8      IOBase;
    U8      IOLimit;
    U16     SecondaryStatus;
    U16     MemoryBase;
    U16     MemoryLimit;
    U16     PrefetchableBase;
    U16     PrefetchableLimit;
    U32     PrefetchableBaseUpper;
    U32     PrefetchableLimitUpper;
    U16     IOBaseUpper;
    U16     IOLimitUpper;
    U8      CapabilitiesPointer;
    U32     Reserved:24;
    U32     ExpansionROMBaseAddress;
    U8      InterruptLine;
    U8      InterruptPin;
    U16     BridgeControl;

} IAL_PCI_CONFIG_SPACE_P2P, *PTR_IAL_PCI_CONFIG_SPACE_P2P;

typedef union __IAL_PCI_CONFIG_SPACE
{
    U32                                     Dwords[64];
    U8                                      Bytes[256];
    IAL_PCI_CONFIG_SPACE_P2P        P2PConfig;

} IAL_PCI_CONFIG_SPACE, *PTR_IAL_PCI_CONFIG_SPACE;

typedef struct __IAL_PCI_LOCATION
{

    U16      Bus;
    U16      Device;
    U16      Function;
    U16      Reserved;
} IAL_PCI_LOCATION, *PTR_IAL_PCI_LOCATION;
typedef struct __IAL_PCI_BAR_REGISTERS
{
    volatile void*  PtrIoBAR;
    U32    IoBarMapSize;
    volatile void *  PtrBAR1;
    U32    Bar1MapSize;
    volatile void*  PtrBAR2;
    U32    Bar2MapSize;
} IAL_PCI_BAR_REGISTERS;
#if defined(OS_UEFI)
typedef UINT64 __U64;
#else
typedef unsigned long __U64;
#endif

typedef union __ADDRESS_64
{
    __U64 Quad;
    struct __parts
    {
        U32 Low;
        U32 High;
    } u;
    U8 Buffer[0x08];
} SLIFF_UINT64, *PTR_SLIFF_UINT64, SAS_ADDRESS, *PTR_SAS_ADDRESS;
typedef struct __IAL_PCI_BAR_REGIONS
{
    BOOLEAN         IsMemBar;       //TODO: Archaa: We need to chage the to bool , as linux does not have BOOLEAN
    U32             BarSize;
    BOOLEAN         Is64BitAddress;
    SLIFF_UINT64    PhysicalAddress;
    U32             ConfigSpaceOffset;
    PVOID           PtrVirtualAddress;
} IAL_PCI_BAR_REGIONS, *PTR_IAL_PCI_BAR_REGIONS;
#endif
