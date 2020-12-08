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


#ifndef __SLIFF_DRIVER__H__
#define __SLIFF_DRIVER__H__


#ifdef SLIFF_DRIVER

/*****************************************************************************
*
*               Basic Types
*
*****************************************************************************/

typedef signed   char   S8;
typedef unsigned char   U8;
typedef signed   short  S16;
typedef unsigned short  U16;


#if defined(unix) || defined(__arm) || defined(ALPHA) || defined(__PPC__) || defined(__ppc)

    typedef signed   int   S32;
    typedef unsigned int   U32;
    typedef unsigned long long U64;

#else

    typedef signed   int  S32;
    typedef unsigned int   U32;

#endif

#if 0
typedef struct _S64
{
    U32          Low;
    S32          High;
} S64;

typedef struct _U64
{
    U32          Low;
    U32          High;
} U64;
#endif

/*****************************************************************************
*
*               Pointer Types
*
*****************************************************************************/

typedef S8      *PS8;
typedef U8      *PU8;
typedef S16     *PS16;
typedef U16     *PU16;
typedef S32     *PS32;
typedef U32     *PU32;
//typedef S64     *PS64;
typedef U64     *PU64;


#ifndef TRUE
#define TRUE    (1)
#endif
#ifndef FALSE
#define FALSE   (0)
#endif

#if !defined(OS_VMWARE)
    #ifndef VOID
    typedef void VOID;
    #endif
#endif

#ifndef PVOID
typedef VOID * PVOID;
#endif

#ifndef BOOLEAN
typedef U8 BOOLEAN;
#endif

#define SLIFF_DRIVER_VERSION		"00.00.01.02"

#define SLIFF_DRIVER_VERSION_MAJOR   00
#define SLIFF_DRIVER_VERSION_MINOR   00
#define SLIFF_DRIVER_VERSION_BUILD   01
#define SLIFF_DRIVER_VERSION_DEV     02


/*
 * Prototype initialization
 */

int sdiMemoryMap (struct file *file, struct vm_area_struct *vma);
void sdDriverProbeForEcam (void);

static long sdiDriverIoctl (struct file *file, unsigned int cmd, unsigned long arg);

/*
 * Driver operations for Sliff Driver
 */

static struct file_operations SliffDriverOperations = {

    .owner          = THIS_MODULE,
    .mmap           = sdiMemoryMap,
    .unlocked_ioctl = sdiDriverIoctl,
};

dev_t           DriverMajorMinor;
unsigned int    DriverMajor;
unsigned int    DriverMinor;
unsigned int    DeviceNumber;
struct cdev     *DeviceControl;
struct class    *chrdev_class;

#endif

typedef struct __SLIFF_DRIVER_MEMORY
{
    phys_addr_t     PhysicalAddress;
    void            *VirtualAddress;
    unsigned int    MapSize;

} SLIFF_DRIVER_MEMORY, *PTR_SLIFF_DRIVER_MEMORY;

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

#define SLIFF_MAGIC_NUMBER  'S'

/**
 * IOCTL opcodes
 */
#define SLIFF_DRIVER_IOCTL_TEST             _IO(SLIFF_MAGIC_NUMBER, 1)
#define SLIFF_DRIVER_IOCTL_DISCOVER_PCI     _IOWR(SLIFF_MAGIC_NUMBER, 10, struct __SCRUTINY_DRIVER_PCI_CFG_SPACE)
#define SLIFF_DRIVER_IOCTL_ALLOCATE_MEMORY  _IOWR(SLIFF_MAGIC_NUMBER, 11, struct __SLIFF_DRIVER_MEMORY)
#define SLIFF_DRIVER_IOCTL_FREE_MEMORY      _IOWR(SLIFF_MAGIC_NUMBER, 12, struct __SLIFF_DRIVER_MEMORY)
#define SLIFF_DRIVER_IOCTL_READ_PCI         _IOWR(SLIFF_MAGIC_NUMBER, 13, struct __SCRUTINY_DRIVER_PCI_CFG_SPACE)
#define SLIFF_DRIVER_IOCTL_WRITE_PCI        _IOWR(SLIFF_MAGIC_NUMBER, 14, struct __SCRUTINY_DRIVER_PCI_CFG_SPACE)
#define SLIFF_DRIVER_IOCTL_ENABLE_PCI       _IOWR(SLIFF_MAGIC_NUMBER, 15, struct __SCRUTINY_DRIVER_PCI_CFG_SPACE)


/***********************************************************
 * ioremap_prot
 *
 * This function is supported after 2.6.27 only one some
 * architectures, like x86 & PowerPC. Other architectures
 * added support for it in later kernels. In newer kernels, calls
 * to ioremap() for ACPI locations may report errors if the
 * default flags conflict with kernel mappings.
 **********************************************************/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
    #define ioremap_prot(addr,size,flags)     ioremap((addr), (size))
#endif


// Macros for I/O port access
#define IO_PORT_READ_8(port)                        inb((port))
#define IO_PORT_READ_16(port)                       inw((port))
#define IO_PORT_READ_32(port)                       inl((port))
#define IO_PORT_WRITE_8(port, val)                  outb((val), (port))
#define IO_PORT_WRITE_16(port, val)                 outw((val), (port))
#define IO_PORT_WRITE_32(port, val)                 outl((val), (port))


/***********************************************************
 * Macros for device memory access
 *
 * ioreadX() and iowriteX() functions were added to kernel 2.6,
 * but do not seem to be present in all kernel distributions.
 **********************************************************/
#if defined(ioread8)
    #define PHYS_MEM_READ_8                         ioread8
    #define PHYS_MEM_READ_16                        ioread16
    #define PHYS_MEM_READ_32                        ioread32
    #define PHYS_MEM_WRITE_8(addr, data)            iowrite8 ( (data), (addr) )
    #define PHYS_MEM_WRITE_16(addr, data)           iowrite16( (data), (addr) )
    #define PHYS_MEM_WRITE_32(addr, data)           iowrite32( (data), (addr) )
#else
    #define PHYS_MEM_READ_8                         readb
    #define PHYS_MEM_READ_16                        readw
    #define PHYS_MEM_READ_32                        readl
    #define PHYS_MEM_WRITE_8(addr, data)            writeb( (data), (addr) )
    #define PHYS_MEM_WRITE_16(addr, data)           writew( (data), (addr) )
    #define PHYS_MEM_WRITE_32(addr, data)           writel( (data), (addr) )
#endif


/*******************************************
 *    Definitions used for ACPI probe
 ******************************************/
// Used to scan ROM for services
#define BIOS_MEM_START                  0x000E0000
#define BIOS_MEM_END                    0x00100000


// ACPI RSDT v1.0 structure
typedef struct _ACPI_RSDT_v1_0
{
    U32 Signature;
    U32 Length;
    U8  Revision;
    U8  Oem_Id[6];
    U8  Oem_Table_Id[8];
    U32 Oem_Revision;
    U32 Creator_Id;
    U32 Creator_Revision;
} ACPI_RSDT_v1_0;

// ACPI probe states
#define ACPI_PCIE_NOT_PROBED            0
#define ACPI_PCIE_BYPASS_OS_OK          1
#define ACPI_PCIE_DEFAULT_TO_OS         2
#define ACPI_PCIE_ALWAYS_USE_OS         3

//ACPI end



#define SCRUTINY_DRIVER_IOCTL_DISCOVER_PCI              0x1000
#define SCRUTINY_DRIVER_IOCTL_READ_PCI                  0x1001
#define SCRUTINY_DRIVER_IOCTL_WRITE_PCI                 0x1002
#define SCRUTINY_DRIVER_IOCTL_ENABLE_PCI                0x1003
#define SCRUTINY_DRIVER_IOCTL_ALLOCATE_MEMORY           0x1004
#define SCRUTINY_DRIVER_IOCTL_FREE_MEMORY               0x1005


/***********************************************************
 * pci_get_domain_bus_and_slot not added until 2.6.33
 **********************************************************/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
    #define scrutiny_pci_get_domain_bus_and_slot(d,b,df)    pci_get_bus_and_slot( b, df )
#else
    #define scrutiny_pci_get_domain_bus_and_slot            pci_get_domain_bus_and_slot
#endif

// Function number passed to driver in upper 3-bits of slot
//#define PCI_DEVFN(slot, fn)       ((char)((((char)(fn)) << 5) | ((char)(slot) & 0x1f)))

// Endian swap macros
#define EndianSwap32(value)              ( ((((value) >>  0) & 0xff) << 24) | \
                                           ((((value) >>  8) & 0xff) << 16) | \
                                           ((((value) >> 16) & 0xff) <<  8) | \
                                           ((((value) >> 24) & 0xff) <<  0) )

#define EndianSwap16(value)              ( ((((value) >>  0) & 0xffff) << 16) | \
                                           ((((value) >> 16) & 0xffff) <<  0) )


// Convert pointer to an integer
#define SCRUTINY_PTR_TO_INT( ptr )                     ((unsigned int)(ptr))

// Convert integer to a pointer
#define SCRUTINY_INT_TO_PTR( intval )                  ((VOID*)(unsigned long)(intval))

// Macros that guarantee correct endian format regardless of CPU platform
#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
    #define SCRUTINY_LE_DATA_32(value)                 EndianSwap32( (value) )
    #define SCRUTINY_BE_DATA_32(value)                 (value)
#else
    #define SCRUTINY_LE_DATA_32(value)                 (value)
    #define SCRUTINY_BE_DATA_32(value)                 EndianSwap32( (value) )
#endif

// Macros to support portable type casting on BE/LE platforms
#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))

    #define SCRUTINY_64_HIGH_32(value)             ((U32)((U64)value))
    #define SCRUTINY64_LOW_32(value)              ((U32)(((U64)value) >> 32))

    #define SCRUTINY_CAST_64_TO_8_PTR( ptr64 )     (U8*) ((U8*)SCRUTINY_INT_TO_PTR(ptr64) + (7*sizeof(U8)))
    #define SCRUTINY_CAST_64_TO_16_PTR( ptr64 )    (U16*)((U8*)SCRUTINY_INT_TO_PTR(ptr64) + (6*sizeof(U8)))
    #define SCRUTINY_CAST_64_TO_32_PTR( ptr64 )    (U32*)((U8*)SCRUTINY_INT_TO_PTR(ptr64) + sizeof(U32))
    #define SCRUTINY_LE_U32_BIT( pos )             ((U32)(1 << (31 - (pos))))

#else
    #define SCRUTINY_64_HIGH_32(value)             ((U32)(((U64)value) >> 32))
    #define SCRUTINY_64_LOW_32(value)              ((U32)((U64)value))

    #define SCRUTINY_CAST_64_TO_8_PTR( ptr64 )     (U8*) SCRUTINY_INT_TO_PTR(ptr64)
    #define SCRUTINY_CAST_64_TO_16_PTR( ptr64 )    (U16*)SCRUTINY_INT_TO_PTR(ptr64)
    #define SCRUTINY_CAST_64_TO_32_PTR( ptr64 )    (U32*)SCRUTINY_INT_TO_PTR(ptr64)

#endif

/***********************************************************
 * IORESOURCE_MEM_64
 *
 * This flag specifies whether a PCI BAR space is 64-bit. The
 * definition wasn't added until 2.6.31.
 **********************************************************/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
    #define IORESOURCE_MEM_64           0x00100000
#endif




#endif

