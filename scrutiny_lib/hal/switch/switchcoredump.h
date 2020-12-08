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

#ifndef __SWITCH_CORE_DUMP__H__
#define __SWITCH_CORE_DUMP__H__

/** Switch mode types */
typedef enum _PMG_SW_MODE
{
    PMG_SW_MODE_BASE             = 0,       /**< Standard PCIe fan-out */
    PMG_SW_MODE_SMART_SWITCH     = 1,       /**< Smart switch */
    PMG_SW_MODE_USE_DEFAULT      = 3        /**< Config page request to use default mode */
} PMG_SW_MODE;

/* Chip Revision IDs
 */
#define HAL_SWITCH_REVISION_A0     (0xA0)  /**< A0 switch chip revision. */
#define HAL_SWITCH_REVISION_B0     (0xB0)  /**< B0 switch chip revision. */

/** Enumeration listing different switch revision levels.
 */
typedef enum _HALI_SWITCH_REV_LEVEL
{
    HALI_SWITCH_REV_LEVEL_0,      /**< A0 */
    HALI_SWITCH_REV_LEVEL_1,      /**< B0 */
    HALI_SWITCH_REV_LEVEL_UNKNOWN /**< Unknown */
} HALI_SWITCH_REV_LEVEL, *PTR_HALI_SWITCH_REV_LEVEL;

/** The definition of a firmware revision section of a coredump image */
typedef struct _HALI_SWITCH_COREDUMP_SECTION_FWREV
{
    U8         ActiveFwRegion;     /**< The region of the currently executing firmware */
    U8         OperatingMode;      /**< Indicates Smart Switch or Base Mode */
    U8         HwBondOption;       /**< Indicates the bond option */
    U8         Reserved1;
    U32        ChipRevLevel;       /**< The revision level of the hardware*/
    FW_VERSION FWVersion;          /**< The revision of the firmware */
    FW_VERSION HalLibVersion;      /**< The revision of the HAL library */
    FW_VERSION SesLibVersion;      /**< The revision of the SES library */
    FW_VERSION SceLibVersion;      /**< The revision of the SCE library */
    U32        Reserved2[3];
} HALI_SWITCH_COREDUMP_SECTION_FWREV, *PTR_HALI_SWITCH_COREDUMP_SECTION_FWREV;


void scdCoreDumpPrintFwRevSection (    
    __IN__  PU8                             PtrSectionData,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
);

#endif
