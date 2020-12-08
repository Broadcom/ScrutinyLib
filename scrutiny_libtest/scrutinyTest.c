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

#ifndef OS_UEFI 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#ifdef OS_WINDOWS
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include<sys/time.h>
#endif
#endif

#include "types.h"

#if defined (LIB_SUPPORT_CONTROLLER)
//IT - MPI Headers
#include "mpi2_type.h"
#include "mpi2.h"
#include "mpi2_ioc.h"
#include "mpi2_cnfg.h"
#include "mpi2_init.h"
#include "mpi2_sas.h"
#include "mpi2_tool.h"
#include "mpi2_pci.h"
#include "mpi2_image.h"
#include "mpi2_hbd.h"
#include "mpi2_ra.h"
#include "mpi2_targ.h"

//MR Headers
#include "mr.h"
#include "mr_drv.h"
#include "mfistat.h"
#include "mfi_ioctl.h"
#include "mfi.h"
#endif

//Scrutiny Library Headers
#include "scrutinylib.h"

#include "scrutinytest.h"
STATUS scrutinyTest (const char *devNode);
U32   gSelectDeviceIndex = 0;
SCRUTINY_PRODUCT_HANDLE   gSelectDeviceHandle = 0xFF;

S32 scrtnyLibOsiStrNCmp (const char *PtrStr1, const char *PtrStr2, size_t Num)
{
    #ifdef OS_UEFI
        return (efiLibStrNCmp (PtrStr1, PtrStr2, Num));
    #else
        return (strncmp (PtrStr1, PtrStr2, Num) );
    #endif
}

VOID scrtnyLibOsiMemSet (__OUT__ VOID *PtrMem, __IN__ U8 FillChar, __IN__ U32 Size)
{
    #ifdef OS_UEFI
        SetMem (PtrMem, Size, FillChar);
    #else
        memset (PtrMem, FillChar, Size);
    #endif
}

VOID scrtnyLibOsiMemFree (__IN__ VOID *PtrMem)
{

    if (PtrMem == NULL)
    {
        return;
    }

    #ifdef OS_UEFI

        /*
         * We are not checking for the return staus, hope free does
         * not return any error in uefi
         */

        FreePool (PtrMem);

    #else

        free (PtrMem);

    #endif

}


VOID* scrtnyLibOsiMemAlloc (__IN__ U32 Size)
{

    #ifdef OS_UEFI

        return ((VOID *) AllocateZeroPool (Size));

    #else

        VOID *ptrMemory;

        ptrMemory = malloc (Size);

        if (ptrMemory != NULL)
        {
            scrtnyLibOsiMemSet (ptrMemory, 0 , Size);
        }

        return (ptrMemory);

    #endif

}

VOID scrtnyLibOsiMemCopy (__OUT__ VOID *PtrDest, __IN__ const VOID *PtrSrc, __IN__ U32 Size)
{

    #ifdef OS_UEFI
        CopyMem (PtrDest, PtrSrc, Size);
    #else
        memcpy (PtrDest, PtrSrc, Size);
    #endif

}

void dumpBufferToConsole ( PU8 PtrBuffer,  U32 Size)
{
    U32  index;
    for (index = 0; index < Size; index++)
    {
        putc(PtrBuffer[index], stdout);
    }
}

int HexDump(FILE *fp, U8 *dump, int count)
{
    int i = 0;
    for(i = 0; i < count; i++)
    {
        if(i % 16 == 0)
            fprintf(fp, "\n");
        fprintf(fp, "%02X  ", dump[i]);
    }
    fprintf(fp, "\n");
    return i;
}

int main (int argc, char **argv)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    char   *ptrDevNode = NULL;
    
    /* Initialize the global variables */
    gSelectDeviceHandle = 0xFF;
    gSelectDeviceIndex = 0xFF;

    scrtnyPrintBanner();
        
    libStatus = ScrutinyInitialize ();

    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("Initialization of Scrutiny Library interface failed.");
        
        return (1);
    }

    if (argc > 1)
    {
        if (strcmp (argv[1], "-sdb"))
        {
            printf("\nUsage: scrutinyLibTest -sdb <uart_intf>\n");
            goto out;
        }
        else 
        {
            ptrDevNode = argv[2];
        }
    }
    
    scrutinyTest (ptrDevNode);

out:
    libStatus = ScrutinyExit ();
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("EXIT of Scrutiny Library interface failed.");
        
        return (1);
    }
    
    return (0);
}


void scrtnyPrintBanner()
{
    printf ("------------------------------------------------------------- \n");

    if (SCRUTINY_VERSION_DEV)
    {
        printf ("*** Development version *** \n");
    }

    printf ("%s v%d.%d.%d.%d - Broadcom Inc. (c) 2020 \n",
                                  SCRUTINY_UTILITY_NAME,
                                  SCRUTINY_VERSION_MAJOR,
                                  SCRUTINY_VERSION_MINOR,
                                  SCRUTINY_VERSION_REVISION,
                                  SCRUTINY_VERSION_DEV);

    printf ("------------------------------------------------------------- \n");
}


STATUS selectDevice (U32  numOfDev)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    SCRUTINY_DEVICE_INFO        deviceInfo;
    U8                      quit = 0;
    
    do 
    {
        do
        {
            printf("Please select a device index(0 based) to test with: ");
            fflush(stdin);
            scanf("%d", &gSelectDeviceIndex);
        } while(gSelectDeviceIndex >= numOfDev);
        
        libStatus = ScrutinyGetDeviceInfo (gSelectDeviceIndex, sizeof (SCRUTINY_DEVICE_INFO), &deviceInfo);
        if (libStatus != SCRUTINY_STATUS_SUCCESS)
        {
            gSelectDeviceHandle = 0xFF;
            gSelectDeviceIndex = 0xFF;
            printf ("ScrutinyGetDeviceInfo ().. Failed Status -  %x \n", libStatus);
            return (STATUS_FAILED);
        }
        
        if (deviceInfo.ProductFamily != SCRUTINY_PRODUCT_FAMILY_SWITCH)
        {
            printf ("Only PCIe Switch product family is Supported!\n");
        }
        else 
        {
            gSelectDeviceHandle = deviceInfo.ProductHandle;
            quit = 1;
        }
        
    } while (quit == 0);
    
    return STATUS_SUCCESS;
}


STATUS scrutinyTest (const char *devNode)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;

    U32 numDevices = 0;
    U32 deviceIndex = 0;

    SCRUTINY_DEVICE_INFO        deviceInfo;
    SCRUTINY_DISCOVERY_PARAMS   discoveryParam;
  
    if (devNode == NULL)
    {
        libStatus = ScrutinyDiscoverDevices (SCRUTINY_DISCOVERY_TYPE_INBAND, NULL);
    }
    else 
    {
        scrtnyLibOsiMemSet (&discoveryParam.u.SerialConfig.DeviceName[0], '\0', sizeof(discoveryParam.u.SerialConfig.DeviceName));
        scrtnyLibOsiMemCopy (&discoveryParam.u.SerialConfig.DeviceName[0], devNode, strlen (devNode));
		
        libStatus = ScrutinyDiscoverDevices (SCRUTINY_DISCOVERY_TYPE_SERIAL_DEBUG, &discoveryParam);
    }

    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("ScrutinyDiscoverDevices ().. Failed Status -  %x \n", libStatus);
		printf ("Unable to discover any devices \n");
        
        return (STATUS_FAILED);
    }
        
    libStatus = ScrutinyGetDeviceCount (&numDevices);
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("ScrutinyiGetProductCount ().. Failed Status -  %x \n", libStatus);
		printf ("Unable to get product count \n");
		
        return (STATUS_FAILED);
    }

    printf("\n\n");
        
    printf ("  %-15s %-18s %-16s\n", "Index", "FwVersion", "Type");
    
    for (deviceIndex = 0; deviceIndex < numDevices; deviceIndex++)
    {   
        libStatus = ScrutinyGetDeviceInfo (deviceIndex, sizeof (SCRUTINY_DEVICE_INFO), &deviceInfo);
        
        if (libStatus != SCRUTINY_STATUS_SUCCESS)
        {
            printf ("ScrutinyGetDeviceInfo ().. Failed Status -  %x \n", libStatus);
        
            return (STATUS_FAILED);
        }
		
		#if defined (LIB_SUPPORT_CONTROLLER)
        if (deviceInfo.ProductFamily == SCRUTINY_PRODUCT_FAMILY_CONTROLLER)
        {
            //call IOC function to print the device details

            scrtnyControllerDetails (deviceIndex, &deviceInfo);

        }
		#endif

		#if defined (LIB_SUPPORT_EXPANDER)
		if (deviceInfo.ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
        {
            scrtnyExpanderDetails (deviceIndex, &deviceInfo);

        }
		#endif

		#if defined (LIB_SUPPORT_SWITCH)
		if (deviceInfo.ProductFamily == SCRUTINY_PRODUCT_FAMILY_SWITCH)
        {
            scrtnySwitchDetails (deviceIndex, &deviceInfo);
        }
		#endif

		continue;

    }

    /* test section */
    selectDevice (numDevices);
    libStatus = ScrutinyGetDeviceInfo (gSelectDeviceIndex, sizeof (SCRUTINY_DEVICE_INFO), &deviceInfo);
            
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("ScrutinyGetDeviceInfo ().. Failed Status -  %x \n", libStatus);
    
        return (STATUS_FAILED);
    }

    if (deviceInfo.ProductFamily == SCRUTINY_PRODUCT_FAMILY_SWITCH)
    {
        showSwitchMenu (&deviceInfo);
    } 
    
    return (STATUS_SUCCESS);
}



#if defined (LIB_SUPPORT_CONTROLLER)
STATUS scrtnyControllerDetails (__IN__ U8 Index, __IN__ PTR_SCRUTINY_DEVICE_INFO PtrDeviceInfo)
{
    char buffer[50];

    if (PtrDeviceInfo->u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_IT_IR)
    {
        scrtnyLibOsiMemSet (&buffer, 0, sizeof (buffer));

        snprintf(buffer, sizeof(buffer), "%02d.%02d.%02d.%02d", PtrDeviceInfo->u.ControllerInfo.u.Gen3.IT.IocFacts.FWVersion.Struct.Major,
                                                                PtrDeviceInfo->u.ControllerInfo.u.Gen3.IT.IocFacts.FWVersion.Struct.Minor,
                                                                PtrDeviceInfo->u.ControllerInfo.u.Gen3.IT.IocFacts.FWVersion.Struct.Unit,
                                                                PtrDeviceInfo->u.ControllerInfo.u.Gen3.IT.IocFacts.FWVersion.Struct.Dev);
                                                                  
        printf ("%3d)               %-18s %-16s\n",
                                      Index,
                                      buffer,
                                      PtrDeviceInfo->u.ControllerInfo.u.Gen3.IT.IsIRFirmware ? "IR" : "IT");  
    }
    else if (PtrDeviceInfo->u.ControllerInfo.ControllerType == SCRUTINY_CONTROLLER_TYPE_GEN3_MR)
    {
        printf ("%3d)               %-18s %-16s\n",
                                      Index,
                                      PtrDeviceInfo->u.ControllerInfo.u.Gen3.MR.packageVersion,
                                      "MR");  
    }       

    return (STATUS_SUCCESS);
}
#endif

STATUS scrtnyExpanderDetails (__IN__ U32 Index, __IN__ PTR_SCRUTINY_DEVICE_INFO  PtrDeviceInfo)
{
    char buffer[26];

    if (PtrDeviceInfo->HandleType & SCRUTINY_HANDLE_TYPE_MASK_INBAND)
    {
        scrtnyLibOsiMemSet (&buffer, 0, sizeof (buffer));
        snprintf (buffer, sizeof(buffer), "%02d.%02d.%02d.%02d", (PtrDeviceInfo->u.ExpanderInfo.FWVersion >> 24) & 0xFF,
                                                                   (PtrDeviceInfo->u.ExpanderInfo.FWVersion >> 16) & 0xFF,
                                                                   (PtrDeviceInfo->u.ExpanderInfo.FWVersion >> 8) & 0xFF,
                                                                   (PtrDeviceInfo->u.ExpanderInfo.FWVersion) & 0xFF);
                                                                   
        printf (  "%3d)               %-18s Expander (%02x:%02x:%02x:%02x)\n",
                                  Index,
                                  buffer,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.SegmentNumber,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.BusNumber,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.DeviceNumber,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.FunctionNumber);
    }

    if (PtrDeviceInfo->HandleType == SCRUTINY_HANDLE_TYPE_SDB)
    {
        scrtnyLibOsiMemSet (&buffer, 0, sizeof (buffer));
        snprintf (buffer, sizeof(buffer), "%02d.%02d.%02d.%02d", (PtrDeviceInfo->u.ExpanderInfo.FWVersion >> 24) & 0xFF,
                                                                   (PtrDeviceInfo->u.ExpanderInfo.FWVersion >> 16) & 0xFF,
                                                                   (PtrDeviceInfo->u.ExpanderInfo.FWVersion >> 8) & 0xFF,
                                                                   (PtrDeviceInfo->u.ExpanderInfo.FWVersion) & 0xFF);

        printf ("%3d)               %-18s Expander (SDB)\n", Index, buffer);
    }
    
    return (STATUS_SUCCESS);
}


STATUS scrtnySwitchGetTrace ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    
    U32     bufferSize = 1 * 1024;
    PU8     ptrBuffer = NULL;
  
    ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
    if (ptrBuffer == NULL)
    {
        printf("Unable to allocate buffer to get Trace!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (ptrBuffer, 0xFF, bufferSize);

    libStatus = ScrutinyGetTraceBuffer (&gSelectDeviceHandle, ptrBuffer, &bufferSize);

    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        if (libStatus == SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH) 
        {
            scrtnyLibOsiMemFree (ptrBuffer);

            ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
            if (ptrBuffer == NULL)
            {
                printf("Unable to allocate buffer to get Trace!!!\n\n");
                return (STATUS_FAILED);
            }

            scrtnyLibOsiMemSet (ptrBuffer, 0xFF, bufferSize);

            libStatus = ScrutinyGetTraceBuffer (&gSelectDeviceHandle, ptrBuffer, &bufferSize);
            if (libStatus != SCRUTINY_STATUS_SUCCESS)
            {
                printf("Get Trace logs failed with libStatus %x\n", libStatus);
            }
            else 
            {
                printf("\n\n\nTrace Buffer: \n");
                dumpBufferToConsole (ptrBuffer, bufferSize);
            }
        } 
        else 
        {
            printf("Get Trace logs failed with libStatus %x\n", libStatus);
        }
    }
    else 
    {
        printf("\n\n\nTrace Buffer: \n");
        dumpBufferToConsole (ptrBuffer, bufferSize);
    }
    
    scrtnyLibOsiMemFree (ptrBuffer);

    return (STATUS_SUCCESS);
}

STATUS scrtnyCoreDump (int InputOperation)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;    
    U32     bufferSize = 1024 * 1;
    PU8     ptrBuffer = NULL;
    int     operation;
    
    if (InputOperation == 0)
    {
        printf("\n\n");
        printf("%30s\n", "...CORE DUMP MENU...");
        printf("%30s\n", "--------------------");
        printf("\t%s\n", "(1) Get Decoded core dump");
        printf("\t%s\n", "(2) Erase core dump region");

        fflush(stdin);
        scanf("%d",&operation);

        if (operation != 1 && operation != 2)
        {
            printf("\nWrong operation selected, quit....\n");
            return (STATUS_FAILED);
        }
    }
    else 
    {
        operation = InputOperation;
        if (operation != 1 && operation != 2)
        {
            printf("\nWrong Input operation, quit....\n");
            return (STATUS_FAILED);
        }
    }

    if (operation == 2)
    {
        libStatus = ScrutinyEraseCoreDump (&gSelectDeviceHandle);
        if (libStatus != SCRUTINY_STATUS_SUCCESS)
        {
            printf ("Failed to erase core dump buffer, libStatus %x", libStatus);
            return (STATUS_FAILED);
        }
         
        return (STATUS_SUCCESS);
    }

    bufferSize = 10;
    ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
    if (ptrBuffer == NULL)
    {
        printf("Unable to allocate buffer to get core dump!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (ptrBuffer, 0xFF, bufferSize);
    
    /* the library decodes the core dump buffer and outputs human-readable format */
    libStatus = ScrutinyGetCoreDump (&gSelectDeviceHandle, ptrBuffer, &bufferSize, EXP_COREDUMP_DECODED);
    if (libStatus == SCRUTINY_STATUS_SUCCESS)
    {
        printf("Dump Core dump data:\n");
        dumpBufferToConsole (ptrBuffer, bufferSize);
    }
    else
    {
        if (libStatus == SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH)
        {
            scrtnyLibOsiMemFree (ptrBuffer);
            ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize + 10);
            if (ptrBuffer == NULL)
            {
                printf("Unable to allocate buffer to get core dump!!!\n\n");
                return (STATUS_FAILED);
            }

            libStatus = ScrutinyGetCoreDump (&gSelectDeviceHandle, ptrBuffer, &bufferSize, EXP_COREDUMP_DECODED);
            if (libStatus == SCRUTINY_STATUS_SUCCESS)
            {
                printf("Dump Core dump data:\n");
                dumpBufferToConsole (ptrBuffer, bufferSize);
            }
            else 
            {
                printf("%s - Failed to get core dump data, status %x\n", __func__, libStatus);
            }
        }
        else 
        {
            printf("%s - Failed to get core dump data, status %x\n", __func__, libStatus);
        }
    }

    scrtnyLibOsiMemFree (ptrBuffer);
    return (STATUS_SUCCESS);
}

STATUS scrtnySwitchHealthLogs (int LogType)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    int  logType;
    
    U32     bufferSize = 16;    /* use a small buffer to test whether the library returns needed buffer size */
    PU8     ptrBuffer = NULL;
    
    if (LogType == 0)
    {
		printf("\n\n");
        printf("%30s\n", "...HEALTH LOGS MENU...");
        printf("%30s\n", "--------------------");
        printf("\t%s\n", "(1) Get raw health log");
        printf("\t%s\n", "(2) Get decoded health log");
		printf("\t%s\n", "(3) Get health log via memory read");

        fflush(stdin);
        scanf("%d",&logType);

        if (logType != 3 && logType != 1 && logType != 2)
        {
            printf("\nUnknown selected log type, quit....\n");
            return (STATUS_FAILED);
        }
    }
    else 
    {
        logType = LogType;
    }
        
    ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
    if (ptrBuffer == NULL)
    {
        printf("Unable to allocate buffer to get Health log!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (ptrBuffer, 0xFF, bufferSize);

    libStatus = ScrutinyGetHealthLogs (&gSelectDeviceHandle, ptrBuffer, &bufferSize, (EXP_HEALTH_LOG_TYPE)logType);

    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        if (libStatus == SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH) 
        {
            scrtnyLibOsiMemFree (ptrBuffer);

            ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
            if (ptrBuffer == NULL)
            {
                printf("Unable to allocate buffer to get Healty Log!!!\n\n");
                return (STATUS_FAILED);
            }

            scrtnyLibOsiMemSet (ptrBuffer, 0xFF, bufferSize);

            libStatus = ScrutinyGetHealthLogs (&gSelectDeviceHandle, ptrBuffer, &bufferSize, (EXP_HEALTH_LOG_TYPE)logType);
            if (libStatus != SCRUTINY_STATUS_SUCCESS)
            {
                if (libStatus == SCRUTINY_STATUS_UNSUPPORTED) 
                {
                    printf("current interface does not support EXP_HEALTH_LOGS_DECODED\n\n");
                }
                else 
                {
                    printf("Get Health logs failed with libStatus %x\n", libStatus);
                }
            }
            else 
            {
                printf("\n\n\nHealth Logs: \n");
                dumpBufferToConsole (ptrBuffer, bufferSize);
            }
        } 
        else 
        {
            if (libStatus == SCRUTINY_STATUS_UNSUPPORTED) 
            {
                printf("current interface does not support EXP_HEALTH_LOGS_DECODED\n\n");
            }
            else 
            {
                printf("Get Health logs failed with libStatus %x\n", libStatus);
            }
            
        }
    }
    else 
    {
        printf("\n\n\nHealth Logs: \n");
        dumpBufferToConsole (ptrBuffer, bufferSize);
    }
    
    scrtnyLibOsiMemFree (ptrBuffer);

    return (STATUS_SUCCESS);

}


STATUS scrtnySwitchEraseSbr ()
{
#if 0
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32   bufferSize = 128 * 1024;
    PU8   ptrBuffer;
    
    ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
    if (ptrBuffer == NULL)
    {
        printf("Unable to allocate memory, scrtnySwitchUploadSbr test failed 1\n");
        return STATUS_FAILED;
    }
    
    /*
    libStatus = ScrutinySwitchUploadSbr(&gSelectDeviceHandle, 0, 1, ptrBuffer, &bufferSize, NULL);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("\n\nscrtnySwitchUploadSbr test failed with status 1 %x\n\n", libStatus);
    }
            
    printf("Dumping the buffer before erasing:\n");
    HexDump(stdout, (U8 *) ptrBuffer, bufferSize);
    */
    
    libStatus = ScrutinySwitchEraseSbr (&gSelectDeviceHandle, 0, 1);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("\n\nscrtnySwitchEraseSbr test failed with status %x\n\n", libStatus);
        return (STATUS_FAILED);
    }
/*
    libStatus = ScrutinySwitchUploadSbr(&gSelectDeviceHandle, 0, 1, ptrBuffer, &bufferSize, NULL);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("\n\nscrtnySwitchUploadSbr test failed with status %x\n\n", libStatus);
    }
            
    printf("Dumping the buffer after erasing:\n");
    HexDump(stdout, (U8 *) ptrBuffer, bufferSize);
            
    scrtnyLibOsiMemFree (ptrBuffer); 
*/
    printf("\n\nscrtnySwitchUploadSbr test succeed\n\n");
    return STATUS_SUCCESS;
#endif

	return STATUS_FAILED;
}

STATUS scrtnySwitchUploadSbr ()
{
#if 0
    U32   bufferSize = 0;
    PU8   ptrBuffer;
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    
#if defined(OS_LINUX)
    char *folderName = "switchsbr/";
#elif defined(OS_WINDOWS)
    char *folderName = "switchsbr\\";
#endif 

    char currentDirectoryPath[512] = { '\0' };

    if (scrtnyOtcGetCurrentDirectory(currentDirectoryPath) != STATUS_SUCCESS)
    {
        printf("Unable to get current working directory");

        return (STATUS_FAILED);
    }
    strcat(currentDirectoryPath, folderName);

    libStatus = ScrutinySwitchUploadSbr(&gSelectDeviceHandle, 0, 1, NULL, &bufferSize, currentDirectoryPath);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        if (libStatus == SCRUTINY_STATUS_INSUFFICIENT_BUFFER)
        {
            printf("Need buffer Size %d\n", bufferSize);
            ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
            if (ptrBuffer == NULL)
            {
                printf("Unable to allocate memory, scrtnySwitchUploadSbr test failed\n");
                return STATUS_FAILED;
            }
            libStatus = ScrutinySwitchUploadSbr(&gSelectDeviceHandle, 0, 1, ptrBuffer, &bufferSize, NULL);
            if (libStatus != SCRUTINY_STATUS_SUCCESS)
            {
                printf("\n\nscrtnySwitchUploadSbr test failed with status %x\n\n", libStatus);
            }
            
            printf("Dumping the buffer\n");
            HexDump(stdout, (U8 *) ptrBuffer, bufferSize);
            
            scrtnyLibOsiMemFree (ptrBuffer); 
        }
        else 
        {
            printf("\n\nscrtnySwitchUploadSbr test failed with status %x\n\n", libStatus);
        }
    }

    return STATUS_SUCCESS;
#endif

	return STATUS_FAILED;
}

SOSI_Test_FILE_HANDLE scrtnyLibFileOpen (__IN__ const char *PtrFileName, __IN__ const char *PtrMode)
{

    SOSI_Test_FILE_HANDLE ptrHandle;

    ptrHandle = fopen (PtrFileName, PtrMode);
    

    return (ptrHandle);

}



/**
 *
 * @method  sosiFileClose()
 *
 * @param   FileHandle     File handle which needs to be closed
 *
 * @brief   Will close the handle that was created upon opening a file.
 *
 */

void scrtnyLibFileClose (__IN__ SOSI_Test_FILE_HANDLE FileHandle)
{

        fclose (FileHandle);
    

}

SCRUTINY_STATUS scrtnyLibFileWrite (__IN__ SOSI_Test_FILE_HANDLE FileHandle, __IN__ const U8 *PtrBuffer, __IN__ U32 Size)
{
 

        fwrite (PtrBuffer, 1, Size, FileHandle);
        
        return (SCRUTINY_STATUS_SUCCESS);

}

STATUS scrtnyLibFileLength (__IN__ SOSI_Test_FILE_HANDLE FileHandle, __OUT__ U32 *PtrSize)
{
    U32 current = ftell(FileHandle);

    fseek (FileHandle, 0, SEEK_END);    
    *PtrSize = ftell (FileHandle);
    rewind (FileHandle);
    fseek (FileHandle, current, SEEK_SET);

    return (STATUS_SUCCESS);
}

STATUS scrtnyLibFileRead (__IN__ SOSI_Test_FILE_HANDLE FileHandle, __OUT__ U8 *PtrBuffer, __IN__ U32 Length)
{
    U32 bytesRead;
    
    if (FileHandle == NULL)
    {
        return (STATUS_FAILED);
    }

    bytesRead = fread (PtrBuffer, 1, Length, FileHandle);

    if (bytesRead != Length)
    {
        return (STATUS_FAILED);
    }
        
    return (STATUS_SUCCESS);
}

STATUS scrtnySwitchDownloadSbr ()
{
#if 0
    U32   bufferSize = 0, fileSize = 0;
    PU8   ptrBuffer, verBuffer;
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    SOSI_Test_FILE_HANDLE fPtr;
    U32   i;
    STATUS    status;

    fPtr = scrtnyLibFileOpen ("sbr_dl.bin", "rb");
    
    if (!fPtr)
    {
        printf("Unable to Open SBR binary file!\n");
        return (STATUS_FAILED);
    }
    
    bufferSize = 128 * 1024;
    
    ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
    if (ptrBuffer == NULL)
    {
        printf("Unable to allocate memory, scrtnySwitchUploadSbr test failed\n");
        return STATUS_FAILED;
    }
    
    scrtnyLibOsiMemSet (ptrBuffer, 0, bufferSize);
    
    status = scrtnyLibFileLength (fPtr, &fileSize);
    if (status != STATUS_SUCCESS)
    {
        printf("Unable to get the SBR file length!\n");
        scrtnyLibOsiMemFree (ptrBuffer);
        return STATUS_FAILED;
    }
    
    status = scrtnyLibFileRead (fPtr, ptrBuffer, fileSize);
    if (status != STATUS_SUCCESS)
    {
        printf("Unable to Read the SBR Image file!\n");
        scrtnyLibOsiMemFree (ptrBuffer);
        return STATUS_FAILED;
    }
    
    libStatus = ScrutinySwitchDownloadSbr ( &gSelectDeviceHandle, 0, 1, 0, ptrBuffer, fileSize);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("\n\nScrutinySwitchDownloadSbr test failed with status %x\n\n", libStatus);
        scrtnyLibOsiMemFree (ptrBuffer); 
        return STATUS_FAILED;
    }
    
    
    /* Read the burned image for verification */
    verBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
    if (verBuffer == NULL)
    {
        printf("Unable to allocate memory for verify buffer, scrtnySwitchUploadSbr test failed\n");
        return STATUS_FAILED;
    }
    libStatus = ScrutinySwitchUploadSbr(&gSelectDeviceHandle, 0, 1, verBuffer, &bufferSize, NULL);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("\n\nscrtnySwitchUploadSbr test failed with status %x\n\n", libStatus);
    }
    
    if (bufferSize != fileSize)
    {
        printf("Size mismatch! bufferSize = %d vs. fileSize = %d\n", bufferSize, fileSize);
    }
    
    status = STATUS_SUCCESS;
    for (i=0; i<fileSize; i++)
    {
        if (verBuffer[i] != ptrBuffer[i])
        {
            printf("Data Mismatch @ location %d, verBuffer=%x, ptrBuffer=%x\n", i, verBuffer[i], ptrBuffer[i]);
            status = STATUS_FAILED;
        }
    }
        
    scrtnyLibOsiMemFree (verBuffer); 
    scrtnyLibOsiMemFree (ptrBuffer); 
    
    if (status == STATUS_SUCCESS)
    {
        printf("Successfully downloading the SBR image!\n");
    }
    
    return STATUS_SUCCESS;
#endif
	return STATUS_FAILED;
}

#if 0
static U32 gSBR_OFFSET_TABLE[] = {
    0x000400, 0x040400, 0x080400, 0x0C0400, 0x100400
};
#endif

STATUS scrtnyPrintSBRIndexTable (__IN__ PTR_SCRUTINY_ATLAS_SBR_HEADER PtrHeader)
{
#if 0
    BOOLEAN present;

    /* By default we will print the SoC settins as True. */
    printf ("%5s %20s : %s\n", "", "SoC HW Settings", "TRUE");

    present = (BOOLEAN) (PtrHeader->PsbHwSettingsOffset != 0 && PtrHeader->PsbHwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSB PCIe Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->Psw0HwSettingsOffset != 0 && PtrHeader->Psw0HwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSW0 HW Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->Psw1HwSettingsOffset != 0 && PtrHeader->Psw1HwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSW1 HW Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->Psw2HwSettingsOffset != 0 && PtrHeader->Psw2HwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSW2 HW Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->Psw3HwSettingsOffset != 0 && PtrHeader->Psw3HwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSW3 HW Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->Psw4HwSettingsOffset != 0 && PtrHeader->Psw4HwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSW4 HW Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->Psw5HwSettingsOffset != 0 && PtrHeader->Psw5HwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSW5 HW Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->PswX2HwSettingsOffset != 0 && PtrHeader->PswX2HwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "PSWX2 HW Settings", present ? "TRUE" : "FALSE");

    present = (BOOLEAN) (PtrHeader->SerdesHwSettingsOffset != 0 && PtrHeader->SerdesHwSettingsSize != 0);
    printf ("%5s %20s : %s\n", "", "Serdes HW Settings", present ? "TRUE" : "FALSE");

    return (STATUS_SUCCESS);
#endif
	return (STATUS_FAILED);
}

static char *sHwStatusString[] = { "CHECKSUM_PASS", "CHECKSUM_FAIL", "SIG_FAIL", "BOOT_LOADING", 
                                "BOOT_DISABLED", "UNKNOWN" };

const char *ssGetHwStatusString (__IN__ U8 HwStatus)
{
    return (sHwStatusString[HwStatus]);
}

STATUS scrtnyPrintSBRInfo ( U8 Region, U32 ChipSelect, BOOLEAN IsActive, U8 HwStatus)
{
#if 0
    U32 sbrOffset = 0;
    U32 sbrSize = 1024 * 128;
    PU8 ptrSBR = NULL;
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;

    sbrOffset = gSBR_OFFSET_TABLE[Region];

    printf ("%20s : %d\n", "SBR Region Id", Region);

    if (IsActive)
    {
        printf ("%5s %20s : %s\n", "", "Image Type", "Active SBR");
        printf ("%5s %20s : %s\n", "", "CCR HW Status", ssGetHwStatusString (HwStatus));
    }

    printf ("%5s %20s : %x\n", "", "Image Offset", sbrOffset);
    
    ptrSBR = (PU8) scrtnyLibOsiMemAlloc (sbrSize);
    if (ptrSBR == NULL)
    {
        printf("Unable to allocate memory, scrtnyPrintSBRInfo test failed\n");
        return STATUS_FAILED;
    }
    
    scrtnyLibOsiMemSet (ptrSBR, 0, sbrSize);
    
    libStatus = ScrutinySwitchUploadSbr(&gSelectDeviceHandle, ChipSelect, Region, ptrSBR, &sbrSize, NULL);
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("%5s %20s : %s\n", "", "Validation", "Invalid/Unknown Image");
        printf ("%5s %20s : %s\n", "", "Image Size", "Unknown");
    }
    else 
    {
        printf ("%5s %20s : %d bytes\n", "", "Image Size", sbrSize);
        scrtnyPrintSBRIndexTable ((PTR_SCRUTINY_ATLAS_SBR_HEADER) ptrSBR);
    }

    scrtnyLibOsiMemFree (ptrSBR);

    return (STATUS_SUCCESS);
#endif
	return (STATUS_FAILED);
}

STATUS scrtnySwitchSbrInfo ()
{
#if 0
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32   temp;
    U8 hwLocation = 0, hwStatus = 0;
    
    // Only display CS = 0
    libStatus = ScrutinySwitchMemoryRead (&gSelectDeviceHandle, 0xFFF00000 + 0x00A0, (PU32) &temp, sizeof (U32));
    if (libStatus == SCRUTINY_STATUS_SUCCESS)
    {
        /* HW Status is 3 bits from 28th bit */
        hwStatus = (U8) ((temp >> 28) & 0x7);

        /* Bootlocation is 3 bits from 19th bit */
        hwLocation = (U8) ((temp >> 19) & 0x7);

        /* We need to check if the Boot is disabled, we will not have the correct SBR location */
        if (hwStatus == 0x4)
        {
            hwLocation = 0xFF;
        }
    }    
    else
    {
        hwLocation = 0xFF;
        hwStatus = 0xFF;
    }
    
    printf("Dumping all SBR regions.....\n");

    for (temp = 0; temp < SWITCH_MAX_SBR_LOCATIONS; temp++)
    {
        /* print all regions */
        scrtnyPrintSBRInfo ((U8) temp, (U8)0, (BOOLEAN) (hwLocation == temp), hwStatus);
    }
    
    return STATUS_SUCCESS;
#endif
	return STATUS_FAILED;
}


STATUS scrtnySwitchFwCli (PU8 PtrCmdLine)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U8      cmdLine[58];
    U32     bufferSize = 1024 * 1024;
    PU8     ptrBuffer = NULL;
    PU8     ptrCmd;
    
    if (PtrCmdLine == NULL) 
    {
        printf("\n\nInput the FW UART cmd: ");
    
    #if defined(OS_LINUX)
        setbuf(stdin, NULL);
    #elif defined(OS_WINDOWS)
        rewind(stdin);
    #endif
        scanf("%[^\n]", cmdLine);
        fflush(stdin);
        
        ptrCmd = &cmdLine[0];
    }
    else
    {
        ptrCmd = PtrCmdLine;
    }
    
    ptrBuffer = (PU8) scrtnyLibOsiMemAlloc (bufferSize);
    if (ptrBuffer == NULL)
    {
        printf("Unable to allocate buffer to perform FW CLI\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (ptrBuffer, 0xFF, bufferSize);
    

    libStatus = ScrutinyGetFirmwareCliOutput (&gSelectDeviceHandle, ptrCmd, ptrBuffer, bufferSize, NULL);



    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("Cli command \"%s\": failed with libStatus %x\n", ptrCmd, libStatus);
    }
    else 
    {
        printf("\n\n\nCLI outputs: %s\n", ptrBuffer);
    }

    scrtnyLibOsiMemFree (ptrBuffer);

    return (STATUS_SUCCESS);
    
}


STATUS scrtnySwitchGetTemperature ()
{
    U32 Temperature;
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    libStatus= ScrutinyGetTemperature (&gSelectDeviceHandle, &Temperature);
    
    if (libStatus == SCRUTINY_STATUS_SUCCESS)
    {
        printf("Switch Temperature = %d C degree \n", Temperature);
    }
    else 
    {
        printf ("ScrutinySwitchGetTemperatureValue ().. return Status -  %x \n", libStatus);
    }
                
    return STATUS_SUCCESS;
}

STATUS appPrintHeaders()
{
    printf ("\n");
    printf ("Type:  Port type Down-Downstream, Up-Upstream, Fab-Fabric Mode, Mgmt-Management\n");
    printf ("MRR:   Max Read Request Size                 MPSS: Max Payload Size Supported\n");
    printf ("MPS:   Max Payload Size                      Link: Link Width by Negotiated/Maximum\n");
    printf ("Speed: Link Speed by Negotiated/Maximum   ClkMode: Clock Mode");

    printf ("\n");
    printf ("%-5s %-10s %-5s %-5s %-5s %-5s %-13s %-10s %-7s  %-5s", "Port", "GID", "Type", "MRR", "MPS", "MPSS", "LinkSpeed", "LinkWidth", "ClkMode", "Up?");
    printf ("\n");
    printf ("----- ----- ----- ----- ----- ------------- ---------- -------- -----");
    printf ("\n");
    return (STATUS_SUCCESS);

}

STATUS appPrintPortProperties (U32 PortIndex, PTR_SWITCH_PORT_CONFIGURATION PtrConfiguration)
{

    /* Port Index */
    printf ("%-5d ", PortIndex);

    /* Port GID */
    printf ("%-10x ", PtrConfiguration->GID);

    /* Print Port type */

    switch (PtrConfiguration->PortType)
    {
        case SWITCH_PORT_TYPE_DOWNSTREAM:
            printf ("%-5s ", "Down");
            break;

        case SWITCH_PORT_TYPE_FABRIC:
            printf ("%-5s ", "Fab");
            break;

        case SWITCH_PORT_TYPE_MANAGEMENT:
            printf ("%-5s ", "Mgmt");
            break;

        case SWITCH_PORT_TYPE_UPSTREAM:
            printf ("%-5s ", "Host");
            break;

        case SWITCH_PORT_TYPE_UPSTREAM_BSW:
            printf ("%-5s ", "Host");
            break;

        case SWITCH_PORT_TYPE_DOWNSTREAM_BSW:
            printf ("%-5s ", "Down");
            break;


        default:
            printf ("%-5s ", "-");
            break;
    }

    /* MRR */

    switch (PtrConfiguration->MaxReadRequestSize)
    {
        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_128_BYTES:
            printf ("%-5s ", "128");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_256_BYTES:
            printf ("%-5s ", "256");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_512_BYTES:
            printf ("%-5s ", "512");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_1024_BYTES:
            printf ("%-5s ", "1024");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_2048_BYTES:
            printf ("%-5s ", "2048");
            break;

        default:
            printf ("%-5s ", "-");
            break;

    }

    /* MPS/MPSS */

    switch (PtrConfiguration->MaxPayloadSize)
    {
        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_128_BYTES:
            printf ("%-5s ", "128");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_256_BYTES:
            printf ("%-5s ", "256");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_512_BYTES:
            printf ("%-5s ", "512");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_1024_BYTES:
            printf ("%-5s ", "1024");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_2048_BYTES:
            printf ("%-5s ", "2048");
            break;

        default:
            printf ("%-5s ", "-");
            break;

    }

    switch (PtrConfiguration->MaxPayloadSizeSupport)
    {
        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_128_BYTES:
            printf ("%-5s ", "128");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_256_BYTES:
            printf ("%-5s ", "256");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_512_BYTES:
            printf ("%-5s ", "512");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_1024_BYTES:
            printf ("%-5s ", "1024");
            break;

        case PCI_DEVICE_CAPABILITY_PAYLOAD_SIZE_2048_BYTES:
            printf ("%-5s ", "2048");
            break;

        default:
            printf ("%-5s ", "-");
            break;

    }

    if (PtrConfiguration->NegotiatedLinkWidth)
    {
        /* Speed */
        switch (PtrConfiguration->NegotiatedLinkSpeed)
        {
            case PCI_DEVICE_LINK_SPEED_GEN_1:
                printf ("%-5s/ ", "Gen1");
                break;
            case PCI_DEVICE_LINK_SPEED_GEN_2:
                printf ("%-5s/ ", "Gen2");
                break;
            case PCI_DEVICE_LINK_SPEED_GEN_3:
                printf ("%-5s/ ", "Gen3");
                break;
            case PCI_DEVICE_LINK_SPEED_GEN_4:
                printf ("%-5s/ ", "Gen4");
                break;
            default:
                printf ("%-5s/ ", "-");
                break;
        }

    }

    else
    {
        printf ("%-5s/ ", "-");
    }

    switch (PtrConfiguration->MaxLinkSpeed)
    {
        case PCI_DEVICE_LINK_SPEED_GEN_1:
            printf ("%-5s  ", "Gen1");
            break;
        case PCI_DEVICE_LINK_SPEED_GEN_2:
            printf ("%-5s  ", "Gen2");
            break;
        case PCI_DEVICE_LINK_SPEED_GEN_3:
            printf ("%-5s  ", "Gen3");
            break;
        case PCI_DEVICE_LINK_SPEED_GEN_4:
            printf ("%-5s  ", "Gen4");
            break;
        default:
            printf ("%-5s  ", "-");
            break;
    }

    /* Link */
    switch (PtrConfiguration->NegotiatedLinkWidth)
    {

        case PCI_DEVICE_LINK_WIDTH_x1:
            printf ("%-4s/ ", "x1");
            break;

        case PCI_DEVICE_LINK_WIDTH_x2:
            printf ("%-4s/ ", "x2");
            break;

        case PCI_DEVICE_LINK_WIDTH_x4:
            printf ("%-4s/ ", "x4");
            break;

        case PCI_DEVICE_LINK_WIDTH_x8:
            printf ("%-4s/ ", "x8");
            break;

        case PCI_DEVICE_LINK_WIDTH_x16:
            printf ("%-4s/ ", "x16");
            break;

        default:
            printf ("%-4s/ ", "-");
            break;

    }

    switch (PtrConfiguration->MaxLinkWidth)
    {

        case PCI_DEVICE_LINK_WIDTH_x1:
            printf ("%-4s ", "x1");
            break;

        case PCI_DEVICE_LINK_WIDTH_x2:
            printf ("%-4s ", "x2");
            break;

        case PCI_DEVICE_LINK_WIDTH_x4:
            printf ("%-4s ", "x4");
            break;

        case PCI_DEVICE_LINK_WIDTH_x8:
            printf ("%-4s ", "x8");
            break;

        case PCI_DEVICE_LINK_WIDTH_x16:
            printf ("%-4s ", "x16");
            break;

        default:
            printf ("%-4s ", "-");
            break;
    }

    /* Mode */
    switch (PtrConfiguration->LinkModeType)
    {

        case PCI_LINK_MODE_TYPE_COMMON_CLOCK:
            printf ("%-7s  ", "Common");
            break;

        case PCI_LINK_MODE_TYPE_SRIS:
            printf ("%-7s  ", "SRIS");
            break;

        case PCI_LINK_MODE_TYPE_SSC_ISOLATION:
            printf ("%-7s  ", "SSC");
            break;

        default:
            printf ("%-7s  ", "-");
            break;

    }

    /* Link Status */

    if (PtrConfiguration->NegotiatedLinkWidth)
    {
        printf ("%-5s", "Up");
    }

    else
    {
        printf ("%-5s", "Down");
    }

    printf ("\n");
    printf ("\n");


    return (STATUS_SUCCESS);

}

STATUS appPrintDownStreamPortHeaders()
{
    printf ("\n");
    printf ("Downstream Ports \n");
    printf ("\n");
    printf ("%-5s %-12s %-17s  %-19s  %57s", "Port", "DS Buses", "Phys Slot Number",  "Assigned Host Port", "Attached Device VID DID SVD SID ClassCode SubClassCode" );
    printf ("\n");
    printf ("----- ----- ----- ----- ----- ------------- ---------- -------- ----- ------------------ -----------");
    printf ("\n");
    return (STATUS_SUCCESS);

}

STATUS appPrintDownStreamPortProperties (U32 PortIndex, PTR_SWITCH_PORT_CONFIGURATION PtrConfiguration)
{


    /* Port Index */
    printf ("%-5d", PortIndex);// default is right align, '-' means left align.

    printf ("%5Xh", PtrConfiguration->u.Ds.GblBusSec);
    printf ("->");
    printf ("%Xh      ", PtrConfiguration->u.Ds.GblBusSub);
    
    printf ("%-17d", PtrConfiguration->u.Ds.ChassisPhysSlotNum);

    printf ("%-19d", PtrConfiguration->u.Ds.HostPortNum);

    if (PtrConfiguration->u.Ds.AttachedDevice.DataValid)
    {
        printf ("%25Xh", PtrConfiguration->u.Ds.AttachedDevice.VendorId);
        printf ("%5Xh", PtrConfiguration->u.Ds.AttachedDevice.DeviceId);
        printf ("%5Xh", PtrConfiguration->u.Ds.AttachedDevice.SubVendorID);
        printf ("%5Xh", PtrConfiguration->u.Ds.AttachedDevice.SubDeviceID);
        printf ("%5Xh", PtrConfiguration->u.Ds.AttachedDevice.ClassCode);
        printf ("%5Xh", PtrConfiguration->u.Ds.AttachedDevice.SubClassCode);
    }
    printf("\n");
    return (STATUS_SUCCESS);

}


STATUS appPrintHostPortHeaders()
{
    printf ("\n");
    printf ("Host Ports \n");
    printf ("\n");
    printf ("%-5s %-17s", "Port",  "Assigned DS Ports");
    printf ("\n");
    printf ("----- ----- ----- ----- ----- ------------- ---------- -------- -----");
    printf ("\n");
    return (STATUS_SUCCESS);

}

STATUS appPrintHostPortProperties (U32 PortIndex, PTR_SWITCH_PORT_CONFIGURATION PtrConfiguration)
{
    U32 index;

    /* Port Index */
    printf ("%-5d\n", PortIndex);// default is right align, '-' means left align.

    for (index = 0; index < 128; index++)
    {
        if (((PtrConfiguration->u.Host.DsPortMask[index/32] >> (index % 32)) &0x01) == 1)
        {
            printf("       %d,\n ", index);
        }
    }
    
    printf("\n");
    return (STATUS_SUCCESS);

}

STATUS ScrutinySwitchDecodePciePortProperties (PTR_SCRUTINY_SWITCH_PORT_PROPERTIES  PtrPciePortProperties)
{
    U32                               entryIndex;
    U32                               portIndex;
    PTR_SWITCH_PORT_CONFIGURATION     ptrConfiguration;

    //common
    appPrintHeaders();
    for (entryIndex = 0; entryIndex < PtrPciePortProperties->TotalPortConfigEntry; entryIndex++)
    {
        ptrConfiguration = &(PtrPciePortProperties->PortConfigurations[entryIndex]);
        portIndex = PtrPciePortProperties->PortConfigurations[entryIndex].PortNumber; 
        appPrintPortProperties (portIndex,  ptrConfiguration);
    }

    //downstream port
    appPrintDownStreamPortHeaders();
    for (entryIndex = 0; entryIndex < PtrPciePortProperties->TotalPortConfigEntry; entryIndex++)
    {
        ptrConfiguration = &(PtrPciePortProperties->PortConfigurations[entryIndex]);
        portIndex = PtrPciePortProperties->PortConfigurations[entryIndex].PortNumber;
        if ((ptrConfiguration->PortType == SWITCH_PORT_TYPE_DOWNSTREAM) || (ptrConfiguration->PortType == SWITCH_PORT_TYPE_DOWNSTREAM_BSW))
        { 
            appPrintDownStreamPortProperties (portIndex,  ptrConfiguration);
        }
    }

    
    //Host port
    appPrintHostPortHeaders();
    for (entryIndex = 0; entryIndex < PtrPciePortProperties->TotalPortConfigEntry; entryIndex++)
    {
        ptrConfiguration = &(PtrPciePortProperties->PortConfigurations[entryIndex]);
        portIndex = PtrPciePortProperties->PortConfigurations[entryIndex].PortNumber;
        if ((ptrConfiguration->PortType == SWITCH_PORT_TYPE_UPSTREAM) || (ptrConfiguration->PortType == SWITCH_PORT_TYPE_UPSTREAM_BSW))
        { 
            appPrintHostPortProperties (portIndex,  ptrConfiguration);
        }
    }

    
    
    return (STATUS_SUCCESS);
}

STATUS scrtnySwitchPortProperties ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    SCRUTINY_SWITCH_PORT_PROPERTIES  PciePortProperties;
    
    libStatus = ScrutinySwitchGetPciePortProperties (&gSelectDeviceHandle, &PciePortProperties);
    if (libStatus == SCRUTINY_STATUS_SUCCESS)
    {
        ScrutinySwitchDecodePciePortProperties (&PciePortProperties);
    }
    else 
    {
        printf ("ScrutinySwitchGetPciePortProperties ().. return Status -  %x \n", libStatus);
    }
    
    return STATUS_SUCCESS;
}


STATUS scrtnySwitchPerformLaneMargin ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32   portNum = 0, lanes = 0, errorCount =0, index;
    U32   numTimeSteps, numVoltageSteps;  
    SCRUTINY_SWITCH_LANE_MARGIN_REQUEST   request;
    PTR_SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE    ptrResponse, ptrResponse2;

    printf("Input the Port Number: ");
    fflush(stdin);
    scanf("%d",&portNum);

    printf("Input the the number of lanes: ");
    fflush(stdin);
    scanf("%d",&lanes);

    printf("Input the number of Time Steps: ");
    fflush(stdin);
    scanf("%d",&numTimeSteps);

    printf("Input the number of Voltage Steps: ");
    fflush(stdin);
    scanf("%d",&numVoltageSteps);

    printf("Input the Error Count Limit: ");
    fflush(stdin);
    scanf("%d",&errorCount);

    if (lanes > 16)
    {
        printf("more than 16 lanes in one port is wrong setting, %d\n", lanes);
        return (STATUS_SUCCESS); 
    }
    scrtnyLibOsiMemSet (&request, 0, sizeof(SCRUTINY_SWITCH_LANE_MARGIN_REQUEST));
    ptrResponse = scrtnyLibOsiMemAlloc (sizeof (SCRUTINY_SWITCH_LANE_MARGIN_RESPONSE) * lanes);

    for (index =0; index < lanes; index++)
    {
        request.Lanes |= (1 << index);
    }

    request.SwitchPort = portNum;
    request.ErrorCount = errorCount;
    request.NumTimeSteps = numTimeSteps;
    request.NumVoltageSteps = numVoltageSteps;
    
    printf ("Measuring the lane margining, it's going to take a while...\n");
    
    libStatus = ScrutinySwitchPerformLaneMargining (&gSelectDeviceHandle, &request, ptrResponse);
    if (libStatus != SCRUTINY_STATUS_SUCCESS) 
    {
        printf ("Failed to perform lane margin port %d - status %x\n", portNum, libStatus);
        return (STATUS_SUCCESS); 
    }

    ptrResponse2 = ptrResponse;
    for (index =0; index < lanes; index++)
    {
        printf("Result for lane %d at port %d\n", index, portNum);
        printf("    RightTimeMarginStatus %x RightTimeMarginStep %x\n", ptrResponse->RightTimeMarginStatus, ptrResponse->RightTimeMarginStep);
        printf("    LeftTimeMarginStatus %x LeftTimeMarginStep %x\n", ptrResponse->LeftTimeMarginStatus, ptrResponse->LeftTimeMarginStep);
        printf("    UpVoltageMarginStatus %x UpVoltageMarginStep %x\n", ptrResponse->UpVoltageMarginStatus, ptrResponse->UpVoltageMarginStep);
        printf("    DownVoltageMarginStatus %x DownVoltageMarginStep %x\n", ptrResponse->DownVoltageMarginStatus, ptrResponse->DownVoltageMarginStep);
        ptrResponse++;
    }

    scrtnyLibOsiMemFree (ptrResponse2);
    return (STATUS_SUCCESS); 
}

STATUS scrtnySwitchGetLaneMarginCapacities()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32   portNum = 0, laneNum = 0;
    U32   marginControllCapabilities;
    U32   numSteps, maxOffset;  

    printf("Input the Port Number: ");
    fflush(stdin);
    scanf("%d",&portNum);

    printf("Input the Lane Number: ");
    fflush(stdin);
    scanf("%d",&laneNum);

    libStatus = ScrutinySwitchGetLaneMarginCapacities (&gSelectDeviceHandle, portNum, laneNum, &marginControllCapabilities, 
        &numSteps, &maxOffset);

    if (libStatus != SCRUTINY_STATUS_SUCCESS) 
    {
        printf ("Failed to get lane margin Capacities for the port %d lane %d - status %x\n", portNum, laneNum, libStatus);
    }
    else 
    {
        printf("marginControllCapabilities = %x\n", marginControllCapabilities);
        printf("Voltage Steps: %d, Timing Steps: %d\n", (numSteps >> 16), (numSteps & 0xFFFF));
        printf("Voltage Offset: %d, Timing Offset: %d\n", (maxOffset >> 16), (maxOffset & 0xFFFF));
    }

    return (STATUS_SUCCESS); 
    
}

STATUS scrtnySwitchCCRStatus ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_CCR_STATUS            PtrSwCcrStatus;
    

    //start the specific code
    PtrSwCcrStatus = (PTR_SCRUTINY_SWITCH_CCR_STATUS) scrtnyLibOsiMemAlloc (sizeof(SCRUTINY_SWITCH_CCR_STATUS));
    if (PtrSwCcrStatus == NULL)
    {
        printf("Unable to allocate buffer for switch!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (PtrSwCcrStatus, 0x00, sizeof(SCRUTINY_SWITCH_CCR_STATUS));

    libStatus = ScrutinySwitchGetCcrStatus ( &gSelectDeviceHandle, PtrSwCcrStatus );      
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("scrtnySwitchCCRStatus test failed with status %x\n", libStatus);
        scrtnyLibOsiMemFree (PtrSwCcrStatus);
        return STATUS_FAILED;
    }
        
    
    printf ("S0PcePerstN = %d\n",  PtrSwCcrStatus->S0PcePerstN);
    printf ("S1PcePerstN = %d\n",  PtrSwCcrStatus->S1PcePerstN);
    printf ("S2PcePerstN = %d\n",  PtrSwCcrStatus->S2PcePerstN);
    printf ("S3PcePerstN = %d\n",  PtrSwCcrStatus->S3PcePerstN);
    switch (PtrSwCcrStatus->SBLHwBootLoc)
    {
        case SBL_HW_BOOT_LOC_0000_0400:
        {
            printf ("SBLHwBootLoc = %s\n", "SBL_HW_BOOT_LOC_0000_0400");
            break;
        }
        case SBL_HW_BOOT_LOC_0004_0400:
        {
            printf ("SBLHwBootLoc = %s\n", "SBL_HW_BOOT_LOC_0004_0400");
            break;
        }
        case SBL_HW_BOOT_LOC_0008_0400:
        {
            printf ("SBLHwBootLoc = %s\n", "SBL_HW_BOOT_LOC_0008_0400");
            break;
        }
        case SBL_HW_BOOT_LOC_000C_0400:
        {
            printf ("SBLHwBootLoc = %s\n", "SBL_HW_BOOT_LOC_000C_0400");
            break;
        }
        case SBL_HW_BOOT_LOC_0010_0400:
        {
            printf ("SBLHwBootLoc = %s\n", "SBL_HW_BOOT_LOC_0010_0400");
            break;
        }
        
        default:
        {
            printf ("SBLHwBootLoc = %s\n", "ERROR!");
            break;
        }

    }


    switch (PtrSwCcrStatus->SBLHWStat)
    {
        case SBL_HW_STAT_CHECKSUM_PASS:
        {
            printf ("SBLHWStat = %s\n", "SBL_HW_STAT_CHECKSUM_PASS");
            break;
        }
        case SBL_HW_STAT_CHECKSUM_FAIL:
        {
            printf ("SBLHWStat = %s\n", "SBL_HW_STAT_CHECKSUM_FAIL");
            break;
        }
        case SBL_HW_STAT_SIG_FAIL:
        {
            printf ("SBLHWStat = %s\n", "SBL_HW_STAT_SIG_FAIL");
            break;
        }
        case SBL_HW_STAT_BOOT_LOADING:
        {
            printf ("SBLHWStat = %s\n", "SBL_HW_STAT_BOOT_LOADING");
            break;
        }
        case SBL_HW_STAT_BOOT_DISABLED:
        {
            printf ("SBLHWStat = %s\n", "SBL_HW_STAT_BOOT_DISABLED");
            break;
        }
        
        default:
        {
            printf ("SBLHWStat = %s\n", "ERROR!");
            break;
        }

    }

    printf ("PCDValid = %d\n",     PtrSwCcrStatus->PCDValid);
    
    scrtnyLibOsiMemFree (PtrSwCcrStatus);
    
    return STATUS_SUCCESS;
    
}

STATUS scrtnySwitchPowerOnSense ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_POWER_ON_SENSE        PtrSwPwrOnSense;
    

    //start the specific code
    PtrSwPwrOnSense = (PTR_SCRUTINY_SWITCH_POWER_ON_SENSE) scrtnyLibOsiMemAlloc (sizeof(SCRUTINY_SWITCH_POWER_ON_SENSE));
    if (PtrSwPwrOnSense == NULL)
    {
        printf("Unable to allocate buffer for switch!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (PtrSwPwrOnSense, 0x00, sizeof(SCRUTINY_SWITCH_POWER_ON_SENSE));

    libStatus = ScrutinySwitchGetPowerOnSense ( &gSelectDeviceHandle, PtrSwPwrOnSense );
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("scrtnySwitchPowerOnSense test failed with status %x\n", libStatus);
        scrtnyLibOsiMemFree (PtrSwPwrOnSense);
        return STATUS_FAILED;
    }
    
    switch (PtrSwPwrOnSense->SBRCsSel)
    {
        case 0:
        {
            printf ("SBRCsSel = %s\n", "SPI_FLASH_CS_N");
            break;        
        }
        case 1:
        {
            printf ("SBRCsSel = %s\n", "SPI_CS_N[0]");
            break;        
        }
        default:
        {
            printf ("SBRCsSel = %s\n", "ERROR!");
            break;
        }

    }
    
    printf ("SBLDisable = %d\n", PtrSwPwrOnSense->SBLDisable);
    
    switch (PtrSwPwrOnSense->SdbBaudRate)
    {
        case SCRUTINY_BAUD_RATE_115200:
        {
            printf ("SdbBaudRate = %s\n", "SCRUTINY_BAUD_RATE_115200");
            break;
        }
        case SCRUTINY_BAUD_RATE_19200:
        {
            printf ("SdbBaudRate = %s\n", "SCRUTINY_BAUD_RATE_19200");
            break;
        }
        default:
        {
            printf ("SdbBaudRate = %s\n", "ERROR!");
            break;
        }

    }
    
    switch (PtrSwPwrOnSense->EnableLaneNumber)
    {
        case SBL_HW_ENABLE_LANE_NUM_96:
        {
            printf ("EnableLaneNumber = %s\n", "SBL_HW_ENABLE_LANE_NUM_96");
            break;
        }
        case SBL_HW_ENABLE_LANE_NUM_80:
        {
            printf ("EnableLaneNumber = %s\n", "SBL_HW_ENABLE_LANE_NUM_80");
            break;
        }
        case SBL_HW_ENABLE_LANE_NUM_64:
        {
            printf ("EnableLaneNumber = %s\n", "SBL_HW_ENABLE_LANE_NUM_64");
            break;
        }
        case SBL_HW_ENABLE_LANE_NUM_48:
        {
            printf ("EnableLaneNumber = %s\n", "SBL_HW_ENABLE_LANE_NUM_48");
            break;
        }
        case SBL_HW_ENABLE_LANE_NUM_32:
        {
            printf ("EnableLaneNumber = %s\n", "SBL_HW_ENABLE_LANE_NUM_32");
            break;
        }
        case SBL_HW_ENABLE_LANE_NUM_24:
        {
            printf ("EnableLaneNumber = %s\n", "SBL_HW_ENABLE_LANE_NUM_24");
            break;
        }
                
        default:
        {
            printf ("EnableLaneNumber = %s\n", "ERROR!");
            break;
        }

    }
    
    switch (PtrSwPwrOnSense->MaxLinkSpeed)
    {
        case PCI_DEVICE_LINK_SPEED_GEN_4:
        {
            printf ("MaxLinkSpeed = %s\n", "PCI_DEVICE_LINK_SPEED_GEN_4");
            break;
        }
        case PCI_DEVICE_LINK_SPEED_GEN_3:
        {
            printf ("MaxLinkSpeed = %s\n", "PCI_DEVICE_LINK_SPEED_GEN_3");
            break;
        }
        default:
        {
            printf ("MaxLinkSpeed = %s\n", "ERROR!");
            break;
        }

    }

    
    free(PtrSwPwrOnSense);
    
    return  STATUS_SUCCESS;
    
} 

STATUS scrtnySwitchRxEqStatus ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS ptrPortRxEqStatus;
    int startPort;
    int numberOfPort;
    int index;
    
    ptrPortRxEqStatus = (PTR_SCRUTINY_SWITCH_PORT_RX_EQ_STATUS) scrtnyLibOsiMemAlloc (sizeof(SCRUTINY_SWITCH_PORT_RX_EQ_STATUS));
    if (ptrPortRxEqStatus == NULL)
    {
        printf("Unable to allocate buffer for switch!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (ptrPortRxEqStatus, 0x00, sizeof(SCRUTINY_SWITCH_PORT_RX_EQ_STATUS));
    printf("input the start port:");
    //fflush(stdin);
    scanf("%d",&startPort);
    
    printf("input the number of port:");                    
    //fflush(stdin);
    scanf("%d",&numberOfPort);
    printf("StartPort = %x, NumberOfPort = %x\n",startPort, numberOfPort);
    
    libStatus = ScrutinySwitchRxEqStatus ( &gSelectDeviceHandle, startPort, numberOfPort, ptrPortRxEqStatus);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("scrtnySwitchRxEqStatus test failed with status %x\n", libStatus);
        scrtnyLibOsiMemFree(ptrPortRxEqStatus);
        return (STATUS_FAILED);
    }
    
    for (index = startPort; index <(startPort + numberOfPort); index++)
    {
        printf("Lane %d, VGA = %d, AEQ = %d, DfeTap1P = %d, DfeTap1N = %d, DfeTap2 = %d, DfeTap3 = %d, DfeTap4 = %d, DfeTap5 = %d, DfeTap6 = %d, DfeTap7 = %d, DfeTap8 = %d, DfeTap9 = %d\n", \
        index, \
        ptrPortRxEqStatus->PortRxEqs[index].VGA, \
        ptrPortRxEqStatus->PortRxEqs[index].AEQ, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap1P, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap1N, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap2, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap3, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap4, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap5, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap6, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap7, \
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap8,\
        ptrPortRxEqStatus->PortRxEqs[index].DfeTap9);
    }
    scrtnyLibOsiMemFree(ptrPortRxEqStatus);
    
    return (STATUS_SUCCESS);
    
}


STATUS scrtnySwitchTxCoeff ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_PORT_TX_COEFF     ptrPortTxCoeff;
    int startPort;
    int numberOfPort;
    int index;

    ptrPortTxCoeff = (PTR_SCRUTINY_SWITCH_PORT_TX_COEFF) scrtnyLibOsiMemAlloc (sizeof(SCRUTINY_SWITCH_PORT_TX_COEFF));
    if (ptrPortTxCoeff == NULL)
    {
        printf("Unable to allocate buffer for switch!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (ptrPortTxCoeff, 0x00, sizeof(SCRUTINY_SWITCH_PORT_TX_COEFF));
    printf("input the start port:");
    //fflush(stdin);
    scanf("%d",&startPort);
    
    printf("input the number of port:");                    
    //fflush(stdin);
    scanf("%d",&numberOfPort);
    printf("StartPort = %x, NumberOfPort = %x\n",startPort, numberOfPort);
    
    libStatus = ScrutinySwitchTxCoeff ( &gSelectDeviceHandle, startPort, numberOfPort, ptrPortTxCoeff);
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("scrtnySwitchTxCoeff test failed with status %x\n", libStatus);
        scrtnyLibOsiMemFree(ptrPortTxCoeff);
        return (STATUS_FAILED);
    }
    
    for (index = startPort; index <(startPort + numberOfPort); index++)
    {
        printf("Lane %d, PreCursorNear = %d, MainCursorNear = %d, PostCursorNear = %d, PreCursorFar = %d, MainCursorFar =%d PostCursorFar = %d, \n", \
        index, \
        ptrPortTxCoeff->PortTxCoeffs[index].PreCursorNear, \
        ptrPortTxCoeff->PortTxCoeffs[index].MainCursorNear, \
        ptrPortTxCoeff->PortTxCoeffs[index].PostCursorNear, \
        ptrPortTxCoeff->PortTxCoeffs[index].PreCursorFar, \
        ptrPortTxCoeff->PortTxCoeffs[index].MainCursorFar, \
        ptrPortTxCoeff->PortTxCoeffs[index].PostCursorFar);


    }
    scrtnyLibOsiMemFree(ptrPortTxCoeff);
    return (STATUS_SUCCESS);

}


STATUS scrtnySwitchHwEye ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_PORT_HW_EYE       ptrPortHwEyeStatus;
    int                                   startPort;
    int                                   numberOfPort;
    int                                   index;    
    U32                                   timeout;
    /* removing compiler warning */
	libStatus = libStatus;

    ptrPortHwEyeStatus = (PTR_SCRUTINY_SWITCH_PORT_HW_EYE) scrtnyLibOsiMemAlloc (sizeof(SCRUTINY_SWITCH_PORT_HW_EYE));
    if (ptrPortHwEyeStatus == NULL)
    {
        printf("Unable to allocate buffer for switch!!!\n\n");
        return (STATUS_FAILED);
    }

    scrtnyLibOsiMemSet (ptrPortHwEyeStatus, 0x00, sizeof(SCRUTINY_SWITCH_PORT_HW_EYE));
    printf("input the start port:");
    //fflush(stdin);
    scanf("%d",&startPort);
    
    printf("input the number of port:");                    
    //fflush(stdin);
    scanf("%d",&numberOfPort);
    
    printf("input the timeout:");                    
    //fflush(stdin);
    scanf("%d",&timeout);
    printf("StartPort = %x, NumberOfPort = %x\n",startPort, numberOfPort);
    
    libStatus = ScrutinySwitchHardwareEyeStart ( &gSelectDeviceHandle, startPort, numberOfPort, ptrPortHwEyeStatus);

    for (index = 0; index < timeout; index++)
    {                        
        libStatus = ScrutinySwitchHardwareEyePoll ( &gSelectDeviceHandle, startPort, numberOfPort, ptrPortHwEyeStatus);

        if(ptrPortHwEyeStatus->GlobalPollingDone)
        {
            break;
        }
        #if defined(OS_LINUX) || defined(OS_FREEBSD) || defined(OS_VMWARE) || defined (OS_BMC)
        usleep (1000 * 1000);
        #elif OS_WINDOWS
        Sleep (1000);
        #elif defined(OS_UEFI)
        lefiDelay (1000 * 1000);
        #endif

    }

    if (ptrPortHwEyeStatus->GlobalPollingDone)
    {
        libStatus = ScrutinySwitchHardwareEyeGet ( &gSelectDeviceHandle, startPort, numberOfPort, ptrPortHwEyeStatus);
        for (index = startPort; index <(startPort + numberOfPort); index++)
        {
            printf("Lane %d, EyeLeft = %.1f%%UI, EyeRight = %.1f%%UI, EyeUp = %.1fmV, EyeDown = %.1fmV \n", \
            index, \
            ptrPortHwEyeStatus->PortHwEye[index].EyeLeft, \
            ptrPortHwEyeStatus->PortHwEye[index].EyeRight, \
            ptrPortHwEyeStatus->PortHwEye[index].EyeUp, \
            ptrPortHwEyeStatus->PortHwEye[index].EyeDown);


        }
    }
	
    libStatus = ScrutinySwitchHardwareEyeClean ( &gSelectDeviceHandle, startPort, numberOfPort, ptrPortHwEyeStatus);
                        
    free(ptrPortHwEyeStatus);
    return (STATUS_SUCCESS);
    
}

STATUS scrtnySwitchSoftEye ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    int                                   startPort;
    int                                   numberOfPort;

    printf("input the start port:");
    //fflush(stdin);
    scanf("%d",&startPort);
    
    printf("input the number of port:");                    
    //fflush(stdin);
    scanf("%d",&numberOfPort);
    

    printf("StartPort = %x, NumberOfPort = %x\n",startPort, numberOfPort);
    
    libStatus = ScrutinySwitchSoftwareEye ( &gSelectDeviceHandle, startPort, numberOfPort);

	if (libStatus != SCRUTINY_STATUS_SUCCESS)
	{
		return (STATUS_FAILED);
	}
	
    return (STATUS_SUCCESS);

}

STATUS scrtnySwitchMemoryRead (U32 Address, U32 SizeInBytes)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32    memAddress;
    U32    readBytes;
    PU8    ptrBuffer;
    
    if (Address == 0xFFFFFFFF)
    {
        /* interactive mode */
        printf("Input Memory Address: 0x");
        fflush(stdin);
        scanf("%x",&memAddress);

        printf("Input how many bytes to read: ");
        fflush(stdin);
        scanf("%d",&readBytes);
    }
    else
    {
        memAddress = Address;
        readBytes = SizeInBytes;
    }
    
    ptrBuffer = scrtnyLibOsiMemAlloc (readBytes);
    if (ptrBuffer == NULL)
    {
        printf("%s - unable to allocate buffer\n", __func__);
        return (STATUS_FAILED);
    }
    libStatus = ScrutinySwitchMemoryRead (&gSelectDeviceHandle, memAddress, (PU32)ptrBuffer, readBytes);
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("%s failed with status %x\n", __func__, libStatus);
        scrtnyLibOsiMemFree (ptrBuffer);
        return (STATUS_FAILED);
    }

    HexDump(stdout, (U8 *) ptrBuffer, readBytes);
    scrtnyLibOsiMemFree (ptrBuffer);
    return (STATUS_SUCCESS);
}

STATUS scrtnySwitchMemoryWrite (U32 Address, PU8 PtrValue, U32 SizeInBytes)
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32    memAddress;
    U32    writeBytes;
    PU8    ptrBuffer;
    U32    index;
    
    if (Address == 0xFFFFFFFF)
    {
        /* interactive mode */
        printf("Input Memory Address: 0x");
        fflush(stdin);
        scanf("%x",&memAddress);

        printf("Input how many bytes to be written: ");
        fflush(stdin);
        scanf("%d",&writeBytes);
        
        ptrBuffer = scrtnyLibOsiMemAlloc (writeBytes);
        if (ptrBuffer == NULL)
        {
            printf("%s - unable to allocate buffer\n", __func__);
            return (STATUS_FAILED);
        }

        for (index = 0; index < writeBytes; index++)
        {
            printf ("input the %d byte to be written: 0x", index);
            fflush(stdin);
            scanf("%hhx", &ptrBuffer[index]);
        }
    }
    else
    {
        memAddress = Address;
        writeBytes = SizeInBytes;
        ptrBuffer = PtrValue;
    }
    
    HexDump(stdout, (U8 *) ptrBuffer, writeBytes);
	
    libStatus = ScrutinySwitchMemoryWrite (&gSelectDeviceHandle, memAddress, (PU32)ptrBuffer, writeBytes);
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("%s failed with status %x\n", __func__, libStatus);
        if (Address == 0xFFFFFFFF)
        {
            scrtnyLibOsiMemFree (ptrBuffer);
        }
        return (STATUS_FAILED);
    }
    
    HexDump(stdout, (U8 *) ptrBuffer, writeBytes);
    
    if (Address == 0xFFFFFFFF)
    {
        scrtnyLibOsiMemFree (ptrBuffer);
    }
    return (STATUS_SUCCESS);
}

/* read page 0xE002 - Vital Product Data Unit Serial Number page as example */
STATUS scrtnySwitchGetConfigPage ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    PU8 ptrPageBuffer = NULL;
    U32 pageLength = 0;
    
    libStatus = ScrutinySwitchGetConfigPage (&gSelectDeviceHandle, 0xE002, 0, &ptrPageBuffer, &pageLength);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("%s failed with status %x\n", __func__, libStatus);
        return (STATUS_FAILED);
    }
    
    HexDump(stdout, (U8 *) ptrPageBuffer, pageLength);
    
    scrtnyLibOsiMemFree (ptrPageBuffer);
    return (STATUS_SUCCESS);
}

STATUS scrtnySwitchResetDevice ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    libStatus = ScrutinyResetDevice (&gSelectDeviceHandle);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("%s failed with status %x\n", __func__, libStatus);
        return (STATUS_FAILED);
    }
    
    return (STATUS_SUCCESS);
}

STATUS scrtnySwitchAladin ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32     ConfigStatus=0;
    U32     TriggerStatus=0;
    U32     StopStatus=0;
    U32     DumpStatus=0;
    //U32     parameterNumber;
    U32     choice;
    //U32     provideFolderName;
    char    FolderName[256];
    //char*   ptrFolderName = NULL;
    int bExit = 0;
	
	/* removing compiler warning */
	libStatus = libStatus;
	
    for(;;)
    {
        printf("%30s\n", "...ALADIN MENU...");
        printf("%30s\n", "-------------------");
        printf("(1) Aladin Configuration \n");
        printf("(2) Aladin Poll Trigger  \n");
        printf("(3) Aladin Manual Stop Trace    \n");
        printf("(4) Aladin Dump Trace    \n");
        printf("Please input the chioce or 0 to exit Aladin Menu:");

        fflush(stdin);
        scanf("%d",&choice);

        switch(choice)
        {
            case 1:
                {
                    printf("\n\nInput folder name for Aladin.ini file: ");
                    fflush(stdin);
                    scanf("%s", FolderName);
                    libStatus = ScrutinySwitchSetAladinConfiguration (&gSelectDeviceHandle, FolderName, &ConfigStatus);
					if (libStatus == SCRUTINY_STATUS_SUCCESS)
					{
						if (ConfigStatus !=1)
							printf("Aladin not configured %d\n", ConfigStatus);  
					}
					else 
					{
						printf("%s - failed with status %x\n", __func__, libStatus); 
					}
                    break;
                }
            case 2:
                {
                    libStatus = ScrutinySwitchPollAladinTrigger (&gSelectDeviceHandle, &TriggerStatus);
                    if (TriggerStatus == 1)
                        printf("Trigger happened %d \n", TriggerStatus);
                    else if (TriggerStatus == 2)                    
                        printf("Capture in progress %d \n", TriggerStatus);
                    else if (TriggerStatus == 3)
                        printf("Trigger happened ... capture in progress %d\n", TriggerStatus);
                    else
                        printf("Trigger not happened No capture in progress%d\n", TriggerStatus);
                                            
                    break;
                }
            case 3:
                {
                    libStatus = ScrutinySwitchStopAladinCapture (&gSelectDeviceHandle, &StopStatus);
                    if (StopStatus !=1)
                        printf("Trace not stopped %d \n", StopStatus);                  
                    break;
                }
            case 4:
                {
                    printf("\n\nInput the Folder Name for Trace dump: ");
                    fflush(stdin);
                    scanf("%s", FolderName);
                    libStatus = ScrutinySwitchGetAladinTracedump (&gSelectDeviceHandle, FolderName, &DumpStatus);
                    if (DumpStatus !=1)
                        printf("Aladin dump failed %d \n", DumpStatus);                 
                    break;
                }
            case 0:
            
            default:
                bExit=1;
                break;

        }
        if (bExit==1)
            break;
        printf("\n");
    }   
    return STATUS_SUCCESS;
    
}

STATUS scrtnySwitchScsiPassthrough ()
{

    SCRUTINY_SCSI_PASSTHROUGH        scsiPassthrough;
    SCRUTINY_STATUS                  status = SCRUTINY_STATUS_SUCCESS;
	SCRUTINY_SCSI_INQUIRY_DATA       inquiryData;
    
	scrtnyLibOsiMemSet (&inquiryData, 0, sizeof (SCRUTINY_SCSI_INQUIRY_DATA));
    scrtnyLibOsiMemSet (&scsiPassthrough, 0, sizeof (SCRUTINY_SCSI_PASSTHROUGH));

    scsiPassthrough.Size = sizeof (SCRUTINY_SCSI_PASSTHROUGH);
    scsiPassthrough.Cdb[0] = 0x12;
    scsiPassthrough.Cdb[4] = 0x56;
    scsiPassthrough.CdbLength = 6;
    scsiPassthrough.DataDirection = DIRECTION_READ;
    scsiPassthrough.DataBufferLength = sizeof (SCRUTINY_SCSI_INQUIRY_DATA);
    scsiPassthrough.PtrDataBuffer = &inquiryData;
    scsiPassthrough.DataDirection = DIRECTION_READ;

    status = ScrutinySwitchScsiPassthrough (&gSelectDeviceHandle, &scsiPassthrough);

    if (status || scsiPassthrough.ScsiStatus)
    {
        printf ("SCSI passthrough failed. Unable to perform the inquiry operation. status  (%x, %x).", status, scsiPassthrough.ScsiStatus);

        return (STATUS_FAILED);
    }
	
	printf("Inquiry Response Data:\n");
	HexDump(stdout, (U8 *)&inquiryData, sizeof (SCRUTINY_SCSI_INQUIRY_DATA));
    return (STATUS_SUCCESS);
}


STATUS showSwitchMenu (PTR_SCRUTINY_DEVICE_INFO PtrDeviceInfo)
{
    int choice;
    int bExit = 0;

    for(;;)
    {
        printf("%30s\n", "...BRCM Atlas Switch MENU...");
        printf("%30s\n", "----------------------------");
        printf("(1) Cli                 (2) Get Temperature\n");
        printf("(3) Switch Trace        (4) Health Logs\n");
        printf("(5) Core Dump           (6) GetSwitchLog\n");
        printf("(7) Health Data         (8) Health Check\n");
        printf("(9) Error Counters      (10) Get Switch Port Properties\n");
        printf("(11) Upload SBR         (12) Erase SBR\n");
        printf("(13) Download SBR       (14) SBR Info\n");
        printf("(15) Get LM Capacities  (16) Lane Margining\n");
        printf("(17) Power On Sense     (18) Switch CCR Status Data\n");
        printf("(19) Switch Rx Eq       (20) Switch Tx Coeff\n");
        printf("(21) Hardware Eye       (22) Software Eye\n");
        printf("(23) Memory Read        (24) Memory Write\n");
        printf("(25) Cfg Page Read      (26) Reset Device\n");
		printf("(27) Aladin Capture     (28) SCSI Passthrough\n");

        fflush(stdin);
        scanf("%d",&choice);

        switch(choice)
        {
            case 1:
                scrtnySwitchFwCli (NULL);
                break;
                
            case 2:  
                scrtnySwitchGetTemperature();
                break;
    
            case 3:
                scrtnySwitchGetTrace ();
                break;
                
            case 4:
                scrtnySwitchHealthLogs (0);
                break;
                
            case 5:
                scrtnyCoreDump (0);
                break;
                
            case 6:
                scrtnyGetSwitchLogs();
                break;
            
            case 7:
                scrtnySwitchGetHealth();
                break;
                
            case 8:
                scrtnySwitchHealthCheck ();
                break;  
                
            case 9:
                scrtnySwitchCounters();
                break;
                
            case 10:
                scrtnySwitchPortProperties ();
                break;
            
            case 11:
                scrtnySwitchUploadSbr ();
                break;
                
            case 12:
                scrtnySwitchEraseSbr ();
                break;
            
            case 13:
                scrtnySwitchDownloadSbr ();
                break;
                
            case 14:
                scrtnySwitchSbrInfo ();
                break; 
            
            case 15:
                scrtnySwitchGetLaneMarginCapacities ();  // TODO: add parameters to support command line mode
                break;
            
            case 16:
                scrtnySwitchPerformLaneMargin ();   //TODO: add parameters to support command line mode
                break;
            
            case 17:
                scrtnySwitchPowerOnSense ();
                break;
                
            case 18:
                scrtnySwitchCCRStatus ();
                break;
            
            case 19: 
                scrtnySwitchRxEqStatus ();
                break;
                
            case 20:
                scrtnySwitchTxCoeff ();
                break;
                
            case 21:
                scrtnySwitchHwEye ();
                break;
                
            case 22:
                scrtnySwitchSoftEye ();
                break;
                
            case 23:
                scrtnySwitchMemoryRead (0xFFFFFFFF, 0);
                break;
                
            case 24:
                scrtnySwitchMemoryWrite(0xFFFFFFFF, NULL, 0);
                break;
                
            case 25:
                scrtnySwitchGetConfigPage ();
                break;
                
            case 26:
                scrtnySwitchResetDevice ();
                break;
			
			case 27:
				scrtnySwitchAladin ();
				break;
				
			case 28:
				scrtnySwitchScsiPassthrough ();
				break;
                
            case 0:
            default:
                bExit=1;
                break;
        }

        if(bExit)
            break;

        printf("\n");

    }
    return STATUS_SUCCESS;
}

STATUS scrtnySwitchDetails (__IN__ U32 Index, __IN__ PTR_SCRUTINY_DEVICE_INFO PtrDeviceInfo)
{
    char buffer[30];

    scrtnyLibOsiMemSet (&buffer, 0, sizeof (buffer));
    snprintf (buffer, sizeof(buffer), "%02d.%02d.%02d.%02d",    (PtrDeviceInfo->u.SwitchInfo.FWVersion >> 24) & 0xFF, 
                                                            (PtrDeviceInfo->u.SwitchInfo.FWVersion >> 16) & 0xFF,
                                                            (PtrDeviceInfo->u.SwitchInfo.FWVersion >> 8) & 0xFF,
                                                            (PtrDeviceInfo->u.SwitchInfo.FWVersion) & 0xFF);
    
    if (PtrDeviceInfo->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_SCSI_GENERIC)
    {
        printf (  "%3d)               %-18s Switch (%02x:%02x:%02x:%02x-SG)\n",
                                  Index,
                                  buffer,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.SegmentNumber,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.BusNumber,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.DeviceNumber,
                                  PtrDeviceInfo->u.SwitchInfo.PciAddress.FunctionNumber);
    }
    else if (PtrDeviceInfo->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MFI_INTERFACE)
    {
        printf ("%3d)               %-18s Switch (MR)\n", Index, buffer);
    } 
    else if (PtrDeviceInfo->HandleType == SCRUTINY_HANDLE_TYPE_SCSI_ON_MPT_INTERFACE)
    {
        printf ("%3d)               %-18s Switch (IT)\n", Index, buffer);
    }
    else if (PtrDeviceInfo->HandleType == SCRUTINY_HANDLE_TYPE_PCI)
    {

        printf ("%3d)               %-18s Switch (%x %x:%x:%x-PCI)\n",
                                      Index,
                                      "Unknown",    
                                      PtrDeviceInfo->u.SwitchInfo.PciAddress.SegmentNumber,
                                      PtrDeviceInfo->u.SwitchInfo.PciAddress.BusNumber,
                                      PtrDeviceInfo->u.SwitchInfo.PciAddress.DeviceNumber,
                                      PtrDeviceInfo->u.SwitchInfo.PciAddress.FunctionNumber);

    }


    return (STATUS_SUCCESS);    
}


STATUS scrtnyGetSwitchLogs()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
#if defined(OS_LINUX)
        char *folderName = "switchlogs/";
#elif defined(OS_WINDOWS)
        char *folderName = "switchlogs\\";
#endif 
    char currentDirectoryPath[512] = { '\0' };

    if (scrtnyOtcGetCurrentDirectory(currentDirectoryPath) != STATUS_SUCCESS)
    {
        printf("Unable to get current working directory");

        return (STATUS_FAILED);
    }
    strcat(currentDirectoryPath, folderName);

    libStatus = ScrutinySwitchGetLogs (&gSelectDeviceHandle, SCRUTINY_EXP_LOGS_BIT_TRACE_BUFFER | SCRUTINY_EXP_LOGS_BIT_HEALTH_LOG | SCRUTINY_EXP_LOGS_BIT_CORE_DUMP, currentDirectoryPath);
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("\n\nscrtnyGetSwitchLogs test failed with status %x\n\n", libStatus);
    }
    else 
    {
        printf("Health logs are saved under %s\n", currentDirectoryPath);
    }

    return STATUS_SUCCESS;
}

STATUS scrtnySwitchCounters ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    SCRUTINY_SWITCH_ERROR_STATISTICS    errorStatistic;
    U32     portNum;
    
    for (portNum = 0; portNum < 128; portNum++)
    {
        libStatus = ScrutinySwitchGetPciPortErrorStatistics (&gSelectDeviceHandle, portNum, &errorStatistic);
		
		/* SCRUTINY_STATUS_INVALID_PORT means the portNum is invalid or the port is not enabled */
        if ((libStatus != SCRUTINY_STATUS_SUCCESS) && (libStatus != SCRUTINY_STATUS_INVALID_PORT))
        {
            printf("ScrutinySwitchGetPciPortErrorStatistics test failed with status %x on Port %d!!!\n", libStatus, portNum);
            continue;
        }
        
        printf("Port %d error statistic:\n", portNum);
        printf("    BadTLPErrors:               %d\n", errorStatistic.ErrorCounters.BadTLPErrors);
        printf("    BadDLLPErrors:              %d\n", errorStatistic.ErrorCounters.BadDLLPErrors);
        printf("    PortReceiverErrors:         %d\n", errorStatistic.ErrorCounters.PortReceiverErrors);
        printf("    RecoveryDiagnosticsErrors:  %d\n", errorStatistic.ErrorCounters.RecoveryDiagnosticsErrors);
        printf("    LinkDownCount:              %d\n", errorStatistic.ErrorCounters.LinkDownCount);
        printf("    LinkSpeed:                  %d\n", errorStatistic.ErrorCounters.LinkSpeed);
        
        printf("\nPort %d Advanced Error Status:\n", portNum);
        printf("  Error Status:\n");
        printf("    CorrectableErrorDetected: %c\n", (errorStatistic.AdvancedErrorStatus.ErrorStatus.CorrectableErrorDetected) ? 'Y' : 'N');
        printf("    NonFatalErrorDetected: %c\n", (errorStatistic.AdvancedErrorStatus.ErrorStatus.NonFatalErrorDetected) ? 'Y' : 'N');
        printf("    FatalErrorDetected: %c\n", (errorStatistic.AdvancedErrorStatus.ErrorStatus.FatalErrorDetected) ? 'Y' : 'N');
        printf("    UnsupportedRequestDetected: %c\n", (errorStatistic.AdvancedErrorStatus.ErrorStatus.UnsupportedRequestDetected) ? 'Y' : 'N');
        
        printf("\n\n  Uncorrectable:\n");
        printf("    DataLinkProtocolError: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.DataLinkProtocolError) ? 'Y' : 'N');
        printf("    SurpriseDownError: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.SurpriseDownError) ? 'Y' : 'N');
        printf("    PoisedTLPStatus: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.PoisedTLPStatus) ? 'Y' : 'N');
        printf("    FlowControlProtocolError: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.FlowControlProtocolError) ? 'Y' : 'N');
        printf("    CompletionTimeout: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.CompletionTimeout) ? 'Y' : 'N');
        printf("    CompleterAbortStatus: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.CompleterAbortStatus) ? 'Y' : 'N');
        printf("    UnexpectedCompletionStatus: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.UnexpectedCompletionStatus) ? 'Y' : 'N');
        printf("    ReceiverOverflowStatus: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.ReceiverOverflowStatus) ? 'Y' : 'N');
        printf("    MalformedTLPStatus: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.MalformedTLPStatus) ? 'Y' : 'N');
        printf("    ECRCError: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.ECRCError) ? 'Y' : 'N');
        printf("    UnsupportedRequestError: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.UnsupportedRequestError) ? 'Y' : 'N');
        printf("    ACSViolationStatus: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.ACSViolationStatus) ? 'Y' : 'N');
        printf("    UncorrectableInternalError: %c\n", (errorStatistic.AdvancedErrorStatus.Uncorrectable.Bits.UncorrectableInternalError) ? 'Y' : 'N');
        
        
        printf("\n\n  Correctable:\n");
        printf("    ReceiverError: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.ReceiverError) ? 'Y' : 'N');
        printf("    BadTLP: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.BadTLP) ? 'Y' : 'N');
        printf("    BadDLLP: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.BadDLLP) ? 'Y' : 'N');
        printf("    ReplayNumRollOver: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.ReplayNumRollOver) ? 'Y' : 'N');
        printf("    ReplayTimerTimeout: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.ReplayTimerTimeout) ? 'Y' : 'N');
        printf("    AdvisoryNonFatalError: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.AdvisoryNonFatalError) ? 'Y' : 'N');
        printf("    CorrectedInternalError: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.CorrectedInternalError) ? 'Y' : 'N');
        printf("    HeaderLogOverflow: %c\n", (errorStatistic.AdvancedErrorStatus.Correctable.Bits.HeaderLogOverflow) ? 'Y' : 'N');
        
        printf("\n\n");
    }
    
    return (STATUS_SUCCESS);
}


STATUS scrtnyOtcGetCurrentDirectory (char *PtrDirPath)
{
    char    tempPath[256] = { '\0' };

    #if defined(OS_LINUX)
        if (getcwd (tempPath, sizeof(tempPath)) == NULL)
        {
            //unable to get the current directory

            printf ("getcwd() failed");

            return (STATUS_FAILED);
        }
    #elif defined(OS_WINDOWS)
        if (!GetCurrentDirectory (512, tempPath))
        {
            printf ("GetCurrentDirectory() failed");

            return (STATUS_FAILED);
        }       
    #endif

    strcpy (PtrDirPath, tempPath);

#if defined(OS_LINUX)
    strcat(PtrDirPath, "/");
#elif defined(OS_WINDOWS)
    strcat(PtrDirPath, "\\");
#endif

    return (STATUS_SUCCESS);
    
}


STATUS scrtnyOtc ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;

    U32     flag = 0xf; 
    char    currentDirectoryPath[512] = { '\0'};

    if (scrtnyOtcGetCurrentDirectory (currentDirectoryPath) != STATUS_SUCCESS)
    {
        printf ("Unable to get current working directory");

        return (STATUS_FAILED);
    }

    libStatus = ScrutinyOneTimeCaptureLogs (flag, currentDirectoryPath);
     
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        return (STATUS_FAILED);
    }
    
    return (STATUS_SUCCESS);
}




STATUS scrtnySwitchGetHealth()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    PTR_SCRUTINY_SWITCH_HEALTH  ptrSwHealth;
    U32   index;

    ptrSwHealth = (PTR_SCRUTINY_SWITCH_HEALTH) scrtnyLibOsiMemAlloc (sizeof (SCRUTINY_SWITCH_HEALTH));
    if (ptrSwHealth == NULL)
    {
        return STATUS_FAILED;
    }

    scrtnyLibOsiMemSet (ptrSwHealth, 0, sizeof (SCRUTINY_SWITCH_HEALTH));
    libStatus = ScrutinySwitchGetHealth(&gSelectDeviceHandle, ptrSwHealth);

    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf("\n\nFailed to GET Switch health data, status %x\n", libStatus);
        return STATUS_FAILED;
    }

    printf("------- Switch Health Data----------\n");
    printf("Chip Temperature:   %d\n", ptrSwHealth->Temperature);
    printf("Enclosure Status:   %x\n", ptrSwHealth->EnclosureStatus);
    for (index=0; index<ATLAS_PMG_MAX_PHYS; index++)
    {
        if (ptrSwHealth->PortStatus[index].isEnabled == TRUE)
        {
            printf("Port %d error statistic:\n", index);
            printf("    BadTLPErrors:               %d\n", ptrSwHealth->PortStatus[index].ErrorCounter.BadTLPErrors);
            printf("    BadDLLPErrors:              %d\n", ptrSwHealth->PortStatus[index].ErrorCounter.BadDLLPErrors);
            printf("    PortReceiverErrors:         %d\n", ptrSwHealth->PortStatus[index].ErrorCounter.PortReceiverErrors);
            printf("    RecoveryDiagnosticsErrors:  %d\n", ptrSwHealth->PortStatus[index].ErrorCounter.RecoveryDiagnosticsErrors);
            printf("    LinkDownCount:              %d\n", ptrSwHealth->PortStatus[index].ErrorCounter.LinkDownCount);
            printf("    LinkSpeed:                  %d\n", ptrSwHealth->PortStatus[index].ErrorCounter.LinkSpeed);
            
            printf("\nPort %d Advanced Error Status:\n", index);
            printf("  Error Status:\n");
            printf("    CorrectableErrorDetected: %c\n", (ptrSwHealth->PortStatus[index].AerState.ErrorStatus.CorrectableErrorDetected) ? 'Y' : 'N');
            printf("    NonFatalErrorDetected: %c\n", (ptrSwHealth->PortStatus[index].AerState.ErrorStatus.NonFatalErrorDetected) ? 'Y' : 'N');
            printf("    FatalErrorDetected: %c\n", (ptrSwHealth->PortStatus[index].AerState.ErrorStatus.FatalErrorDetected) ? 'Y' : 'N');
            printf("    UnsupportedRequestDetected: %c\n", (ptrSwHealth->PortStatus[index].AerState.ErrorStatus.UnsupportedRequestDetected) ? 'Y' : 'N');
            
            printf("\n\n  Uncorrectable:\n");
            printf("    DataLinkProtocolError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.DataLinkProtocolError) ? 'Y' : 'N');
            printf("    SurpriseDownError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.SurpriseDownError) ? 'Y' : 'N');
            printf("    PoisedTLPStatus: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.PoisedTLPStatus) ? 'Y' : 'N');
            printf("    FlowControlProtocolError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.FlowControlProtocolError) ? 'Y' : 'N');
            printf("    CompletionTimeout: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.CompletionTimeout) ? 'Y' : 'N');
            printf("    CompleterAbortStatus: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.CompleterAbortStatus) ? 'Y' : 'N');
            printf("    UnexpectedCompletionStatus: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.UnexpectedCompletionStatus) ? 'Y' : 'N');
            printf("    ReceiverOverflowStatus: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.ReceiverOverflowStatus) ? 'Y' : 'N');
            printf("    MalformedTLPStatus: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.MalformedTLPStatus) ? 'Y' : 'N');
            printf("    ECRCError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.ECRCError) ? 'Y' : 'N');
            printf("    UnsupportedRequestError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.UnsupportedRequestError) ? 'Y' : 'N');
            printf("    ACSViolationStatus: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.ACSViolationStatus) ? 'Y' : 'N');
            printf("    UncorrectableInternalError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Uncorrectable.Bits.UncorrectableInternalError) ? 'Y' : 'N');
            
            
            printf("\n\n  Correctable:\n");
            printf("    ReceiverError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.ReceiverError) ? 'Y' : 'N');
            printf("    BadTLP: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.BadTLP) ? 'Y' : 'N');
            printf("    BadDLLP: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.BadDLLP) ? 'Y' : 'N');
            printf("    ReplayNumRollOver: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.ReplayNumRollOver) ? 'Y' : 'N');
            printf("    ReplayTimerTimeout: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.ReplayTimerTimeout) ? 'Y' : 'N');
            printf("    AdvisoryNonFatalError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.AdvisoryNonFatalError) ? 'Y' : 'N');
            printf("    CorrectedInternalError: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.CorrectedInternalError) ? 'Y' : 'N');
            printf("    HeaderLogOverflow: %c\n", (ptrSwHealth->PortStatus[index].AerState.Correctable.Bits.HeaderLogOverflow) ? 'Y' : 'N');
            
            printf("\n\n");
        }
    }

    return (STATUS_SUCCESS);
}


STATUS scrtnySwitchHealthCheck ()
{
    SCRUTINY_STATUS libStatus = SCRUTINY_STATUS_SUCCESS;
    U32             errCode[SCRUTINY_MAX_SWITCH_HEALTH_ERROR_DWORD];
    char            currentDirectoryPath[512] = { '\0'};
    U32             i;

    if (scrtnyOtcGetCurrentDirectory (currentDirectoryPath) != STATUS_SUCCESS)
    {
        printf ("Unable to get current working directory");

        return (STATUS_FAILED);
    }

    printf("Directory Path %s \n", currentDirectoryPath);

    libStatus = ScrutinySwitchHealthCheck (&gSelectDeviceHandle, currentDirectoryPath, TRUE, &errCode[0]);
    
    if (libStatus != SCRUTINY_STATUS_SUCCESS)
    {
        printf ("Health Monitoring Failed %x \n", libStatus);
        
        return (STATUS_FAILED);
    }

    for (i = 0; i < SCRUTINY_MAX_SWITCH_HEALTH_ERROR_DWORD; i++)
    {
        if (errCode[i] != 0)
        {
            printf ("Error Code [%d] = 0x%x \n", i, errCode[i]);        
        }
    }

    return (STATUS_SUCCESS);
}
