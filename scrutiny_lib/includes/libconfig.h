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

#ifndef __LIB_CONFIG__H__
#define __LIB_CONFIG__H__

#define SCRUTINY_FILE_CONFIG_INI        "scrutiny.ini"
#define SCRUTINY_FILE_DEFAULT_LOG       "scrutiny.log"


#define INI_PARSER_MAXIMUM_CHAR_LENGTH      (1024 * 4)

#define MAX_PATH_LENGTH         (32)
#define MAX_DEVICE_INFO         (256)

#define MAX_CONTROLLERS         (256)

#define MAX_SCSI_ADAPTERS_TO_ENUMERATE      (0x040)


#define CONTROLLER_MPI_TOOLBOX_CLEAN_TIMEOUT      (512)

#define EXPANDER_SCSI_GENERIC_CHUNK_SIZE          (64 * 1024)

#ifdef OS_WINDOWS
    #define SCRUTINY_DEFAULT_IO_TIMEOUT      (30)
    #define SCRUTINY_IOCTL_MAX_SENSE_LEN     (0xFF)
    #define SCRUTINY_IOCTL_ERROR_BUSY_RETRY_COUNT    (0x0A)
#endif

#define DIAG_READ_BUFFER_SIZE                  (64 * 1024) // Read in chunks of 64KB
#define MIN_DIAG_BUFFER_SIZE_SUPPORTED         (16 * 1024) // Register fails below this.

#if defined(OS_UEFI)

    #define EFIERR_OEM(a)                   (0xc000000000000000 | a)

    //#define APrint                    AsciiPrint //used in Scrutiny code base


    #define EFI_SPECIFICATION_VERSION EFI_SYSTEM_TABLE_REVISION

    #define TIANO_RELEASE_VERSION 0x00080006

#endif

#if !defined(OS_WINDOWS)

    #ifndef __MAX
        #define __MAX
        #define max(x,y)            ((int)(x) > (int)(y) ? (x) : (y))
    #endif

    #ifndef __MIN
        #define __MIN
        #define min(a,b)            (((a) < (b)) ? (a) : (b))
    #endif

#endif

#define SWAP_ENDIANNESS(VAL) (((VAL & 0x000000FF) << 24) + \
                              ((VAL & 0x0000FF00) << 8)  + \
                              ((VAL & 0x00FF0000) >> 8)  + \
                              ((VAL & 0xFF000000) >> 24))


#define BRCM_DRiVER_NAME_UNICODE_STR                (L"AvagoTech MPT35 SAS-PCIE UEFI BSD HII")
#define BRCM_SUPPORTED_LANGUAGE_ASCI_STR            ("en")
#define BRCM_DRIVER_NAME_UNICODE_STR                (L"LSI Corporation")
#define LSI_SUPPORTED_LANGUAGES_ASCII_STR           ("en-US")
#define BRCM_DRIVER_NAME_LENGTH                     (128)
#define EFI_MAX_DRIVERS                             (255)
#define AVAGOTECH_SUPPORTED_LANGUAGES_ASCII_STR     ("en")
#define LSI_DRIVER_NAME_UNICODE_STR                 (L"LSI Corporation")
#define LSI_SUPPORTED_LANGUAGES_ASCII_STR           ("en-US")
#define LSI_DRIVER_NAME_LENGTH                      (128)
#define EFI_MAX_DRIVERS                             (255)
#define AVAGOTECH_DRIVER_NAME_UNICODE_STR           (L"AvagoTech MPT35 SAS-PCIE UEFI BSD HII")

#define LINUX_MPT3SAS_DEV_PATH    "/dev/mpt3ctl"                  //device node for Gen3/3.5 IOC
#define LINUX_MEGARAID_DEV_PATH   "/dev/megaraid_sas_ioctl_node"  //device node for MR Hw RAID IOC.


#define SCRUTINY_LIB_EXTRA_BUFFER_SIZE          (1024)

#endif /* __LIB_CONFIG__H__ */
