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


#ifndef __PCI_LINUX__H__
#define __PCI_LINUX__H__

typedef struct __SCRUTINY_DRIVER_MEMORY
{

    unsigned long long      PhysicalAddress;

    unsigned long           VirtualAddress;

    unsigned int            MapSize;

} SCRUTINY_DRIVER_MEMORY, *PTR_SCRUTINY_DRIVER_MEMORY;


typedef struct __SCRUTINY_DRIVER_PCI_CFG_SPACE
{
    U8       Domain;
    U8       Bus;
    U8       Device;
    U8       Function;
    U8       Slot;
    U8       Reserved;
    U16      Reserved1;

    U32      Offset;
    U32      Data [3];

} SCRUTINY_DRIVER_PCI_CFG_SPACE, *PTR_SCRUTINY_DRIVER_PCI_CFG_SPACE;

#define SCRUTINY_DRIVER_IOCTL_DISCOVER_PCI              0x1000
#define SCRUTINY_DRIVER_IOCTL_READ_PCI                  0x1001
#define SCRUTINY_DRIVER_IOCTL_WRITE_PCI                 0x1002
#define SCRUTINY_DRIVER_IOCTL_ENABLE_PCI                0x1003
#define SCRUTINY_DRIVER_IOCTL_ALLOCATE_MEMORY           0x1004
#define SCRUTINY_DRIVER_IOCTL_FREE_MEMORY               0x1005


#endif /* __PCI_LINUX__H__ */

