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


#ifndef __LIBRARY_GLOBAL__H__
#define __LIBRARY_GLOBAL__H__

#if defined(OS_UEFI)
extern EFI_BOOT_SERVICES       *gBS;
extern EFI_RUNTIME_SERVICES    *gRT;
extern EFI_HANDLE               gLibUdkImageHandle;
extern EFI_SYSTEM_TABLE         *gLibUdkSystemTable;
#endif

extern PTR_SCRUTINY_DEVICE_MANAGER          gPtrScrutinyDeviceManager;

/*
 * All loggers
 */

extern PTR_SCRUTINY_DEBUG_LOGGER           gPtrLoggerOutput;
extern PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerGeneric;
extern PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerExpanders;
extern PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerController;
extern PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerSwitch;
extern PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerScsi;


/*
 * @breif
 * Note: This variable will be initialized only when the scrutiny.ini file is
 * in the current folder. otherwise this will be set to NULL.
 *
 */

extern PTR_SCRUTINY_LIBRARY_CONFIG_PARAMS   gPtrLibraryConfigParams;


#endif /* __LIBRARY_GLOBAL__H__ */


