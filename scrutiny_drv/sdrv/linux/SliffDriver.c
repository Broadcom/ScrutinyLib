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

#define SLIFF_DRIVER

#include <asm/io.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/compat.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/device.h>
#include <asm/dma-mapping.h>


#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,12,1)
#include <asm/uaccess.h>
#else
#include <linux/uaccess.h>
#endif

#include "SliffDriver.h"

/***********************************************
 *               Globals
 **********************************************/
// Global ECAM 64-bit address from ACPI table
//
// Probing is only enabled on i386 or x64 platforms since it requires
// parsing ACPI tables.  This is not supported on non-x86 platforms.
//
#if (defined(__i386__) || defined(__x86_64__))
    static U32 gAcpiAddrECAM[3] = { 0, 0, ACPI_PCIE_NOT_PROBED };
#else
    static U32 gAcpiAddrECAM[3] = { 0, 0, ACPI_PCIE_ALWAYS_USE_OS };
#endif


static int sdiInitialize (void)
{
    int status;

    printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    status = alloc_chrdev_region (&DriverMajorMinor, 0, 1, "SliffDriver");

    if (status)
    {
        printk (KERN_ERR "SliffDriver - %s: alloc_chrdev_region failed\n", __FUNCTION__);

        return status;
    }

    chrdev_class = class_create(THIS_MODULE, "SliffDriverClass");

    if (NULL == chrdev_class)
    {
        printk (KERN_ERR "SliffDriver - %s: class_create failed\n", __FUNCTION__);

        goto unreg_chrdev;
    }

    DriverMajor = MAJOR (DriverMajorMinor);
    DriverMinor = MINOR (DriverMajorMinor);
    DeviceNumber = MKDEV (DriverMajor, DriverMinor);

    printk (KERN_INFO "SliffDriver - %s: Major Number %d Minor Number %d \n", __FUNCTION__, DriverMajor, DriverMinor);

    DeviceControl = cdev_alloc();

    if (NULL == DeviceControl)
    {
        printk (KERN_ERR "SliffDriver - %s:  cdev_alloc failed\n", __FUNCTION__);
        status = -1;
        goto unreg_chrdev;
    }

    DeviceControl->owner = THIS_MODULE;
    DeviceControl->ops   = &SliffDriverOperations;

    status = (cdev_add (DeviceControl, DeviceNumber, 1));

    if (status)
    {
        printk (KERN_ERR "SliffDriver - %s: Failed adding driver Error code '%d'\n", __FUNCTION__, status);
        goto dstry_class;
    }

    if (NULL == device_create (chrdev_class, NULL, DeviceNumber, NULL, "SliffDriver"))
    {
        printk (KERN_ERR "SliffDriver - %s: device_create failed\n", __FUNCTION__);
        status = -1;
        goto del_cdev;
    }

	printk (KERN_INFO "SliffDriver - version %s loaded\n", SLIFF_DRIVER_VERSION);

    /*
     * Return success.
     */
    printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);
    return (0);

del_cdev:
    cdev_del (DeviceControl);

dstry_class:
    class_destroy (chrdev_class);

unreg_chrdev:
    unregister_chrdev_region (DriverMajorMinor, 1);

    printk (KERN_ERR "SliffDriver - %s: <exit>\n", __FUNCTION__);
    return status;
}

static void sdiExit (void)
{
    printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    device_destroy(chrdev_class, DeviceNumber);
    cdev_del(DeviceControl);
    class_destroy(chrdev_class);
    unregister_chrdev_region (DriverMajorMinor, 1);

    printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);
}

int sdiMemoryMap (struct file *file, struct vm_area_struct *vma)
{
    //printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    //vma->flags |= VM_LOCKED;

    if (remap_pfn_range (vma, vma->vm_start, vma->vm_pgoff, vma->vm_end-vma->vm_start, pgprot_noncached(vma->vm_page_prot)))
    {
        return (-EAGAIN);
    }

    //printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);

    return (0);

}

int sdDriverFreeContiguousMemory (PTR_SLIFF_DRIVER_MEMORY PtrMemory)
{

    //printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    if (PtrMemory == NULL)
    {
        printk (KERN_ERR "SliffDriver - %s: Invalid param. Memory param is null.\n", __FUNCTION__);

        return (-1);

    }

    /*
     * Convert the physical address to the virtual address space
     */

    PtrMemory->VirtualAddress = phys_to_virt (PtrMemory->PhysicalAddress);

    /*
     * Free the memory here.
     */

    kfree ((void *) PtrMemory->VirtualAddress);

    //printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);

    return (0);

}

int sdDriverAllocateContiguousMemory (PTR_SLIFF_DRIVER_MEMORY PtrMemory)
{

    //printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    if (PtrMemory == NULL)
    {
        printk (KERN_ERR "SliffDriver - <exit> Invalid param. Memory param is null.\n");

        return (-1);

    }

    if (PtrMemory->MapSize < 1 || PtrMemory->MapSize % 4 != 0)
    {

        printk (KERN_ERR "SliffDriver - <exit> Can't allocate invalid memory params.\n");
        printk (KERN_ERR "Memory size should not be zero and should be dword aligned '%08d'.\n", PtrMemory->MapSize);

        return (-1);

    }

    PtrMemory->VirtualAddress = kmalloc (PtrMemory->MapSize, GFP_ATOMIC);
    PtrMemory->PhysicalAddress = virt_to_phys (PtrMemory->VirtualAddress);

    printk (KERN_INFO "SliffDriver - Virtual Address 0x%p && Physical Address 0x%llx, Size = %08x.\n", PtrMemory->VirtualAddress,
                                                                                                         PtrMemory->PhysicalAddress,
                                                                                                         PtrMemory->MapSize);

    if (PtrMemory->PhysicalAddress != 0 && PtrMemory->VirtualAddress != 0)
    {
        printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);

        return (0);
    }

    //printk (KERN_ERR "SliffDriver - %s: <exit>\n", __FUNCTION__);

    return (-1);

}


void sdDriverProbeForEcam (void)
{
    U8              strID[9];
    U8             *ptrEntry;
    U8             *ptrAddress;
    U8             *ptrVaBiosRom;
    U8             *ptrVaRSDT;
    U8             *ptrVaTable;
    U8             *ptrAcpiAddrRSDP;
    U8             *ptrAcpiAddrRSDT;
    U16             numEntries;
    U32             value;
    BOOLEAN         bFound;

    ACPI_RSDT_v1_0  acpiRsdt;

    //printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    // Do not probe again if previously did
    if (gAcpiAddrECAM[2] != ACPI_PCIE_NOT_PROBED)
    {
       return;
    }

    // Default to ACPI and/or ECAM not detected
    gAcpiAddrECAM[2] = ACPI_PCIE_ALWAYS_USE_OS;

    // Default to ECAM not found
    bFound = FALSE;

    // Initialize virtual addresses
    ptrVaBiosRom = NULL;
    ptrVaRSDT    = NULL;

    // Mark end of string
    strID[8] = '\0';

    // Map BIOS ROM into kernel space
    ptrVaBiosRom = ioremap (BIOS_MEM_START,
                            (BIOS_MEM_END - BIOS_MEM_START)
                            );

    if (ptrVaBiosRom == NULL)
    {
        goto _ExitScrutinyProbeForEcam;
    }

    // Set physical and virtual starting addresses
    ptrAcpiAddrRSDP = (U8*) BIOS_MEM_START;
    ptrAddress  = ptrVaBiosRom;

    // Scan system ROM for ACPI RSDP pointer
    do
    {
        // Read 8-bytes
        *(U32*)strID       = PHYS_MEM_READ_32 ((U32*)ptrAddress );
        *(U32*)(strID + 4) = PHYS_MEM_READ_32 ((U32*)(ptrAddress + 4) );

        // Check for header signature
        if (memcmp ("RSD PTR ",
                    strID,
                    8       // 8 bytes
                    ) == 0)
        {
            bFound = TRUE;
        }
        else
        {
            // Increment to next 16-byte boundary
            ptrAddress        += 16;
            ptrAcpiAddrRSDP += 16;
        }
    }
    while (!bFound && (ptrAcpiAddrRSDP < (U8*) BIOS_MEM_END));

    if (!bFound)
    {
        printk (KERN_ERR "SliffDriver - ACPI not detected\n");

        goto _ExitScrutinyProbeForEcam;
    }

    // Reset flag
    bFound = FALSE;

    // Get ACPI revision
    value = PHYS_MEM_READ_8 ((U8*)(ptrAddress + 15));

    // Store RSDT address
    ptrAcpiAddrRSDT = (U8*) ((VOID*)(unsigned long)(PHYS_MEM_READ_32 ((U32*)(ptrAddress + 16))));

    // Map RSDT table
    ptrVaRSDT = ioremap_prot ((unsigned long)(ptrAcpiAddrRSDT), 1024, 0);

    if (ptrVaRSDT == NULL)
    {
        goto _ExitScrutinyProbeForEcam;
    }

    // Get RSDT size
    acpiRsdt.Length = PHYS_MEM_READ_32( (U32*)(ptrVaRSDT + 4) );

    if (acpiRsdt.Length == 0)
    {
        printk(KERN_ERR "SliffDriver - Unable to read RSDT table length \n");

        goto _ExitScrutinyProbeForEcam;
    }

    // Calculate number of entries
    numEntries = (U16)((acpiRsdt.Length - sizeof (ACPI_RSDT_v1_0)) / sizeof (U32));

    if (numEntries > 100)
    {
        printk (KERN_ERR "SliffDriver - Unable to determine RSDT entry count\n");

        goto _ExitScrutinyProbeForEcam;
    }

    // Get address of first entry
    ptrEntry = ptrVaRSDT + sizeof (ACPI_RSDT_v1_0);

    // Parse entry pointers for MCFG table
    while (numEntries != 0)
    {
        // Get address of entry
        ptrAddress = (U8*)((VOID*)(unsigned long)(PHYS_MEM_READ_32 ((U32*)ptrEntry)));

        // Map table
        ptrVaTable = ioremap_prot ((unsigned long)(ptrAddress), 200, 0);

        if (ptrVaTable == NULL)
        {
            goto _ExitScrutinyProbeForEcam;
        }

        // Get table signature
        value = PHYS_MEM_READ_32 ((U32*) ptrVaTable);

        printk (KERN_INFO "SliffDriver - %c%c%c%c table at %08lX\n",
            (char)(value >>  0),
            (char)(value >>  8),
            (char)(value >> 16),
            (char)(value >> 24),
            (unsigned long)(ptrAddress));

        // Check if MCFG table
        if (memcmp( "MCFG", &value, sizeof (U32) ) == 0)
        {
            // Get 64-bit base address of Enhanced Config Access Mechanism
            gAcpiAddrECAM[0] = PHYS_MEM_READ_32 ((U32*)(ptrVaTable + 44));
            gAcpiAddrECAM[1] = PHYS_MEM_READ_32 ((U32*)(ptrVaTable + 48));

            bFound = TRUE;

            // Flag ok to use ECAM
            gAcpiAddrECAM[2] = ACPI_PCIE_BYPASS_OS_OK;
        }

        // Unmap table
        iounmap (ptrVaTable );

        // Get address of next entry
        ptrEntry += sizeof(U32);

        // Decrement count
        numEntries--;
    }

_ExitScrutinyProbeForEcam:

    // Release the BIOS ROM mapping
    if (ptrVaBiosRom != NULL)
    {
        iounmap (ptrVaBiosRom);
    }

    // Release RSDT mapping
    if (ptrVaRSDT != NULL)
    {
        iounmap (ptrVaRSDT);
    }

    if (bFound)
    {
        printk (KERN_INFO "SliffDriver - PCIe ECAM at %02X_%08X\n",
                (unsigned int)gAcpiAddrECAM[1],
                (unsigned int)gAcpiAddrECAM[0]);

        // For newer Linux kernels, default to using OS
        if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))
        {
            printk (KERN_INFO "SliffDriver - Will default to OS for PCIe reg accesses\n");

            gAcpiAddrECAM[2] = ACPI_PCIE_DEFAULT_TO_OS;
        }
    }
    else
    {
        printk (KERN_INFO "SliffDriver - MCFG entry not found (PCIe ECAM not supported)\n");
    }

    //printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);
}


static int sdDriverDiscoverPciDevices (struct pci_dev **PtrDev, SCRUTINY_DRIVER_PCI_CFG_SPACE Karg)
{
    //printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    *PtrDev = scrutiny_pci_get_domain_bus_and_slot ( Karg.Domain,
                                                    Karg.Bus,
                                                    PCI_DEVFN (Karg.Slot, Karg.Function)
                                                  );


    if (*PtrDev != NULL)
    {
        //printk (KERN_INFO "SliffDriver - <exit> PCI devices discovered.\n");

        return 0;
    }

    //unable to find the right device
    //printk (KERN_ERR "SliffDriver - %s: <exit>\n", __FUNCTION__);

    return (-1);
}

static int sdDriverWriteRegister (U8 Domain, U8 Bus, U8 Slot, U8 Function, U16 Offset, U32 Value)
{
    unsigned long address;
    VOID *ptrKernelValue;

    //printk (KERN_INFO "SliffDriver - %s: <enter>\n", __FUNCTION__);

    if (Domain != 0)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit>, PCI Domain 0 only supported \n", __FUNCTION__);

        return -1;
    }

    if (Offset & 0x3)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit> Offset value set to 3 \n", __FUNCTION__);

        return -1;
    }

    if (gAcpiAddrECAM[2] == ACPI_PCIE_ALWAYS_USE_OS)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit> ACPI always set to use OS\n", __FUNCTION__);

        return -1;
    }


    address = ( (unsigned long) gAcpiAddrECAM[1] << 32) |
                (gAcpiAddrECAM[0]      <<  0) |
                (Bus                   << 20) |
                (Slot                  << 15) |
                (Function              << 12) |
                (Offset                <<  0);


    ptrKernelValue = ioremap ((unsigned long) address, sizeof (unsigned long));

    if (ptrKernelValue == NULL)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit>\n", __FUNCTION__);

        return -1;
    }

    PHYS_MEM_WRITE_32 (ptrKernelValue, Value);

    // Release the mapping
    iounmap (ptrKernelValue);

    //printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);

    return 0;

}

static int sdDriverWriteByLocation (struct pci_dev *PtrDev, U8 Domain, U8 Bus, U8 Slot, U8 Function, U16 Offset, U32 Value)
{
	int rc;
	
    rc = pci_write_config_dword (PtrDev, Offset, Value );

    if (rc != 0)
    {
        return -1;
    }
	
	return 0;
}

static int sdDriverReadRegister (U8 Domain, U8 Bus, U8 Slot, U8 Function, U16 Offset, U32 *PtrValue)
{
    unsigned long address;
    unsigned long value;
    VOID *ptrKernelValue;

    //printk (KERN_INFO "SliffDriver - %s: <entry>\n", __FUNCTION__);

    *PtrValue = (U32) - 1;

    if (Domain != 0)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit>, PCI Domain 0 only supported \n", __FUNCTION__);

        return -1;
    }

    if (Offset & 0x3)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit>\n", __FUNCTION__);

        return -1;
    }

    if (gAcpiAddrECAM[2] == ACPI_PCIE_ALWAYS_USE_OS)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit>\n", __FUNCTION__);

        return -1;
    }


    address = ( (unsigned long) gAcpiAddrECAM[1] << 32) |
                (gAcpiAddrECAM[0]      <<  0) |
                (Bus                   << 20) |
                (Slot                  << 15) |
                (Function              << 12) |
                (Offset                <<  0);

    ptrKernelValue = ioremap ((unsigned long) address, sizeof (unsigned long));

    if (ptrKernelValue == NULL)
    {
        printk (KERN_ERR "SliffDriver - %s: <exit>\n", __FUNCTION__);

        return -1;
    }

    //TO_DO: we need to read U8, U16
    value = PHYS_MEM_READ_32 (ptrKernelValue);

    // Release the mapping
    iounmap (ptrKernelValue);

    *PtrValue = value;

    //printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);

    return 0;
}

static int sdDriverReadByLocation (struct pci_dev *PtrDev, U8 Domain, U8 Bus, U8 Slot, U8 Function, U16 Offset, U32 *PtrValue)
{
    int             rc;

    if (PtrValue == NULL)
    {
        return -1;
    }

	rc = pci_read_config_dword (PtrDev, Offset, (U32*) PtrValue);

	if (rc != 0)
	{
		return -1;
	}

	return 0;

}


static int sdiDriverWrite (struct pci_dev *PtrDev, SCRUTINY_DRIVER_PCI_CFG_SPACE Karg)
{
    if ((Karg.Offset >= 0x100) &&
        (gAcpiAddrECAM[2] != ACPI_PCIE_DEFAULT_TO_OS) &&
        (gAcpiAddrECAM[2] != ACPI_PCIE_ALWAYS_USE_OS))
    {
        return (sdDriverWriteRegister (Karg.Domain,
                                       Karg.Bus,
                                       Karg.Slot,
                                       Karg.Function,
                                       Karg.Offset,
                                       Karg.Data[0]));

    }

    else
    {

        return (sdDriverWriteByLocation (PtrDev,
                                         Karg.Domain,
                                         Karg.Bus,
                                         Karg.Slot,
                                         Karg.Function,
                                         Karg.Offset,
                                         Karg.Data[0]));
    }

}



static int sdiDriverRead (struct pci_dev *PtrDev, SCRUTINY_DRIVER_PCI_CFG_SPACE *Karg)
{

    if ((Karg->Offset >= 0x100) &&
        (gAcpiAddrECAM[2] != ACPI_PCIE_DEFAULT_TO_OS) &&
        (gAcpiAddrECAM[2] != ACPI_PCIE_ALWAYS_USE_OS))
    {
        return (sdDriverReadRegister (Karg->Domain,
                                      Karg->Bus,
                                      Karg->Slot,
                                      Karg->Function,
                                      Karg->Offset,
                                      &Karg->Data[0]));

    }

    else
    {

        return (sdDriverReadByLocation (PtrDev,
                                        Karg->Domain,
                                        Karg->Bus,
                                        Karg->Slot,
                                        Karg->Function,
                                        Karg->Offset,
                                        &Karg->Data[0]));
    }

}


static long sdiDriverIoctl (struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret_val = 0;

    //printk (KERN_INFO "SliffDriver - %s: <enter U32=%d, struct=%d>\n", __FUNCTION__, sizeof (U32), sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE));

    switch (cmd)
    {
        case SCRUTINY_DRIVER_IOCTL_DISCOVER_PCI:
        {
            struct pci_dev *ptrDev = NULL;
            SCRUTINY_DRIVER_PCI_CFG_SPACE karg;
            int ret = -1;

            if (copy_from_user (&karg, (void __user *) arg, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE)))
            {

                printk (KERN_ERR "SliffDriver - failure at %s:%d/%s()!\n", __FILE__, __LINE__, __FUNCTION__);

                return (-EFAULT);

            }

            ret = sdDriverDiscoverPciDevices (&ptrDev, karg);

            if (ret != 0)
            {
				return (-1);
            }

            karg.Bus = ptrDev->bus->number;
            karg.Device = PCI_SLOT (ptrDev->devfn);
            karg.Function = PCI_FUNC (ptrDev->devfn);
            
	    	if (copy_to_user ((void __user *)arg, &karg, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE)))
            {
                printk (KERN_ERR "SliffDriver - failure at %s:%d/%s()!\n", __FILE__, __LINE__, __FUNCTION__);

                return (-EFAULT);
            }

            //printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);

            return (0);

        }

        case SCRUTINY_DRIVER_IOCTL_ENABLE_PCI:
        {
            struct pci_dev *ptrDev = NULL;
            SCRUTINY_DRIVER_PCI_CFG_SPACE karg;
            int ret = -1;

            if (copy_from_user (&karg, (char *) arg, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE)))
            {

                printk (KERN_ERR "SliffDriver - failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__);

                return (-EFAULT);

            }

            ret = sdDriverDiscoverPciDevices (&ptrDev, karg);

            if (ret == -1)
            {
                printk (KERN_ERR "SliffDriver - Unable to discover any PCI device \n");

                return (-1);
            }

            if (ptrDev == NULL)
            {
                printk (KERN_ERR "SliffDriver - Unable to get PCI device information \n");

                return (-1);
            }

            if (ptrDev->bus->number != karg.Bus && PCI_SLOT (ptrDev->devfn) != karg.Device && PCI_FUNC (ptrDev->devfn) != karg.Function)
            {
                //user provided bus/dev/fn number did not match so throwing error

                printk (KERN_ERR "SliffDriver - Unable to match bus/dev/fn \n");

                return (-1);

            }

            pci_set_power_state (ptrDev, PCI_D0);

            pci_set_master (ptrDev);

            ret = 0;

            return (0);
        }

        case SCRUTINY_DRIVER_IOCTL_READ_PCI:
        {
            struct pci_dev *ptrDev = NULL;
            SCRUTINY_DRIVER_PCI_CFG_SPACE karg;
            int ret = -1;

            if (copy_from_user (&karg, (char *) arg, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE)))
            {

                printk (KERN_ERR "SliffDriver - failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__);

                return (-EFAULT);

            }

            ret = sdDriverDiscoverPciDevices (&ptrDev, karg);

            if (ret == -1)
            {
                //printk (KERN_ERR "scrutinyDriver - Unable to identify BRCM device \n");

                return (-1);
            }

            if (ptrDev == NULL)
            {
                printk (KERN_ERR "SliffDriver - Unable to get PCI device information \n");

                return (-1);
            }

            if (ptrDev->bus->number != karg.Bus && PCI_SLOT (ptrDev->devfn) != karg.Device && PCI_FUNC (ptrDev->devfn) != karg.Function)
            {
                //user provided bus/dev/fn number did not match so throwing error

                printk (KERN_ERR "SliffDriver - User provided wrong bus/dev/fn number \n");

                ret = -1;

                return (-1);

            }			

            if (karg.Offset > 0xFF)
            {
                sdDriverProbeForEcam();
            }

            ret = sdiDriverRead (ptrDev, &karg);

            if (ret == -1)
            {
                printk (KERN_ERR "scrutinyDriver - Unable to read \n");

                return (-1);
            }			

	    	ret = copy_to_user ((char *) arg, &karg, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE));

            //copy back the read contents back to pass on to arg structure
            if (ret)
            {
                printk (KERN_ERR "SliffDriver - failure at %s:%d/%s() Copy Back=%d!\n", __FILE__, __LINE__, __func__, ret);

                return (-EFAULT);
            }

            return (0);
        }

        case SCRUTINY_DRIVER_IOCTL_WRITE_PCI:
        {
            struct pci_dev *ptrDev = NULL;
            SCRUTINY_DRIVER_PCI_CFG_SPACE karg;
            int ret = -1;

            if (copy_from_user (&karg, (char *) arg, sizeof (SCRUTINY_DRIVER_PCI_CFG_SPACE)))
            {

                printk (KERN_ERR "SliffDriver - failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__);

                return (-EFAULT);

            }

            ret = sdDriverDiscoverPciDevices (&ptrDev, karg);

            if (ret == -1)
            {
                printk (KERN_ERR "SliffDriver - Unable to identify BRCM device \n");

                return (-1);
            }

            if (ptrDev == NULL)
            {
                printk (KERN_ERR "SliffDriver - Unable to get PCI device information \n");

                return (-1);
            }

            if (ptrDev->bus->number != karg.Bus && PCI_SLOT (ptrDev->devfn) != karg.Device && PCI_FUNC (ptrDev->devfn) != karg.Function)
            {
                //user provided bus/dev/fn number did not match so throwing error

                printk (KERN_ERR "SliffDriver - User provided wrong bus/dev/fn number \n");

                ret = -1;

                return (-1);

            }

            if (karg.Offset > 0xFF)
            {
                sdDriverProbeForEcam();
            }

            ret = sdiDriverWrite (ptrDev, karg);

            if (ret == -1)
            {
                printk (KERN_ERR "SliffDriver - Unable to read \n");

                return (-1);
            }

            return (0);
        }


        case SCRUTINY_DRIVER_IOCTL_ALLOCATE_MEMORY:
        {
            PTR_SLIFF_DRIVER_MEMORY ptrMemory = (PTR_SLIFF_DRIVER_MEMORY) arg;

            return (sdDriverAllocateContiguousMemory (ptrMemory));

        }

        case SCRUTINY_DRIVER_IOCTL_FREE_MEMORY:
        {
            PTR_SLIFF_DRIVER_MEMORY ptrMemory = (PTR_SLIFF_DRIVER_MEMORY) arg;

            return (sdDriverFreeContiguousMemory (ptrMemory));
        }

        default:
        {
            printk (KERN_ERR "SliffDriver - %s: IOCTL (%x) not supported\n", __FUNCTION__, cmd);

            ret_val = -1;
        }
    }

    //printk (KERN_INFO "SliffDriver - %s: <exit>\n", __FUNCTION__);

    return ret_val;
}

/*
 * Register Devices.
 */

module_init(sdiInitialize);
module_exit(sdiExit);

/*
 * Other Declarations
 */

MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
