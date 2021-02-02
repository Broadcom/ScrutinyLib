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


#ifndef __LIB_INCLUDES__H__
#define __LIB_INCLUDES__H__

/*
 * Including the basic configurations here.
 */


#if defined (OS_LINUX)

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdarg.h>
#include <unistd.h>
#include <error.h>
#include <sys/uio.h>
#include <stdint.h>
#include <signal.h>
#include <getopt.h>
#include <linux/ioctl.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
#include <sys/utsname.h>
#include <sys/uio.h>
#include <dlfcn.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <unistd.h>

#endif


#ifdef OS_WINDOWS
#define _NTDDSTOR_H_
#include <stdio.h>
#include <windows.h>
#undef _NTDDSTOR_H_
#include <devguid.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <ntddscsi.h>
#include <tchar.h>
#include <initguid.h>
#include <winioctl.h>
#include <time.h>
#include <stddef.h>
#include <windows.h>
#include <fcntl.h>
#include <errno.h>
#include "devioctl.h"
#include "ntddscsi.h"
#include <crtdefs.h>
#include <process.h>
#include <direct.h>
#include <io.h>

#define INLINE
#endif


#if defined (LIB_SUPPORT_CONTROLLER_IT)

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
#include "mpi2_raid.h"
#include "mpi2_targ.h"

#if defined(OS_UEFI) && !defined (TARGET_ARCH_ARM)
#pragma warning(disable : 4201)
#endif


#ifdef PRODUCT_SUPPORT_AVENGER

#include "mpi30_type.h"
#include "mpi30_transport.h"
#include "mpi30_cnfg.h"
#include "mpi30_image.h"
#include "mpi30_init.h"
#include "mpi30_ioc.h"
#include "mpi30_pci.h"
#include "mpi30_raid.h"
#include "mpi30_sas.h"
#include "mpi30_targ.h"
#include "mpi30_tool.h"
#include "mr8.h"
#include "mr8event.h"
//#include "mr8event_dev.h"
#include "mr8stat.h"
#include "mpi_passthru.h"
//#include "mr8_dev.h"
#endif
#endif

#if defined (LIB_SUPPORT_CONTROLLER_MR)

#include "mr.h"
#include "mr_drv.h"
#include "mfistat.h"
#include "mfi_ioctl.h"
#include "mfi.h"

#if defined (PRODUCT_SUPPORT_AVENGER)

#include "mr8.h"
#include "mr8event.h"
#include "mr8stat.h"
#include "mpi_passthru.h"
#endif
#endif

#ifdef OS_UEFI
    #define va_start VA_START
    #define va_arg VA_ARG
    #define va_end VA_END
    #define va_list VA_LIST
#else
    #include <stdarg.h>
#endif

#include "types.h"
#include "scrutinylib.h"

#if defined(OS_VMWARE)
#include "vmwareserial.h"
#elif defined(OS_LINUX)
#include "linuxserial.h"
#elif defined(OS_WINDOWS)
#include "winserial.h"
#elif defined(OS_UEFI)
#include "efiserial.h"
#endif

#include "pcicommon.h"
#include "libconfig.h"
#include "libconfigini.h"
#include "libdevmgr.h"
#include "pciscan.h"
#include "libinternal.h"
#include "libosal.h"
#include "libdebug.h"
#include "scsi.h"
#include "smp.h"
#include "libglobal.h"
#include "endianppc.h"


#if defined (LIB_SUPPORT_CONTROLLER)
#include "ctrldevmgr.h"
#include "ctrlinventory.h"
#endif

#if defined (LIB_SUPPORT_EXPANDER)
#include "cobfwheader.h"
#include "cobregisters.h"
#include "expdevmgr.h"
#include "expcli.h"
#include "exptrace.h"
#include "exphealthlogs.h"
#include "expcoredump.h"
#include "exphealthmon.h"
#include "expphyenable.h"
#include "expphy.h"
//#include "cobbsmgmt.h"
#endif

#if defined (LIB_SUPPORT_SWITCH)
#include "switchdevmgr.h"
#include "atlasregisters.h"
#include "atlasfwheader.h"

#include "switchportcounters.h"
#include "switchcoredump.h"
#include "switchportproperties.h"
#include "switchhealthmon.h"
#include "switchsignalintegrity.h"
#include "switchregbla.h"
#include "switchlanemargin.h"
#include "switchportperformance.h"

#ifdef OS_LINUX
#include "pcicommon.h"
#include "pcilinux.h"
#include "switchaladin.h"
#include "switchltssm.h"
#endif

#include "pciscan.h"

#endif


#ifdef OS_WINDOWS
#include "sgwindows.h"
#endif

#ifdef OS_LINUX
#include "sglinux.h"
#endif

#include "sdbconsole.h"

#include "brcmscsisdk.h"
#include "brcmscsigettemperature.h"
#include "osalserial.h"

#endif


