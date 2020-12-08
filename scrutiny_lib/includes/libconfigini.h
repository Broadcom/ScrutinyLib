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

#ifndef __LIB_CONFIG_INI__H__
#define __LIB_CONFIG_INI__H__

#define SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_ALL          "DEBUG_LOG_LEVEL_ALL"
#define SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_GENERIC      "DEBUG_LOG_LEVEL_OTHERS"
#define SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_EXPANDER     "DEBUG_LOG_LEVEL_HAL_EXPANDER"
#define SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_CONTROLLER   "DEBUG_LOG_LEVEL_HAL_CONTROLLER"
#define SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_SWITCH       "DEBUG_LOG_LEVEL_HAL_SWITCH"
#define SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_SCSI         "DEBUG_LOG_LEVEL_HAL_SCSI"
#define SCRUTINY_CONFIG_FILE_KEY_LOG_FILE               "DEBUG_LOG_FILE"

typedef struct __SCRUTINY_LIBRARY_CONFIG_PARAMS
{

    U32             DebugLogLevelForAll;        /* DebugLevel = value */
    U32             DebugLogLevelForHalExpander;
    U32             DebugLogLevelForHalScsi;
    U32             DebugLogLevelForHalController;
    U32             DebugLogLevelForHalSwitch;
    U32             DebugLogLevelForOthers;

    char*           PtrDebugLogFile;            /* DebugLogFile = file name absolute path */

} SCRUTINY_LIBRARY_CONFIG_PARAMS, *PTR_SCRUTINY_LIBRARY_CONFIG_PARAMS;

typedef struct __CONFIG_INI_ENTRIES
{

    char Key[INI_PARSER_MAXIMUM_CHAR_LENGTH];
    char Value[INI_PARSER_MAXIMUM_CHAR_LENGTH];

} CONFIG_INI_ENTRIES, *PTR_CONFIG_INI_ENTRIES;

typedef struct __CONFIG_INI_DICTIONARY
{

    U32     TotalEntries;

    char    SegmentName[INI_PARSER_MAXIMUM_CHAR_LENGTH];

    PTR_CONFIG_INI_ENTRIES PtrIniEntries;

    struct __CONFIG_INI_DICTIONARY *PtrNext;

} CONFIG_INI_DICTIONARY, *PTR_CONFIG_INI_DICTIONARY;

SCRUTINY_STATUS lcpiInitializeLibraryConfigurations();

SCRUTINY_STATUS lcpiParserDestroyDictionary (__OUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary);
SCRUTINY_STATUS lcpiParserProcessINIFile (__IN__ const char* PtrFileName, __OUT__ PTR_CONFIG_INI_DICTIONARY *PtrDictionary);
BOOLEAN lcpiHasAnyValidEntries (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary);
VOID lcpiDestryLibraryConfigurations();

#endif /* __LIB_CONFIG_INI__H__ */

