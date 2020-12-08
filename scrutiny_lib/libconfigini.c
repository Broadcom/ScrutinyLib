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

#include "libincludes.h"

PTR_SCRUTINY_LIBRARY_CONFIG_PARAMS   gPtrLibraryConfigParams = NULL;

SCRUTINY_STATUS lcpParserParseIniContent (__IN__ PU8 PtrBuffer, __IN__ U32 BufferSize, __OUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary);
SCRUTINY_STATUS lcpParserGetNextLine (__IN__ PU8 PtrBuffer, __IN__ U32 BufferLength, __INOUT__ PU32 PtrRunningOffset, __OUT__ char* PtrLineBuffer, __OUT__ PU32 PtrLineLength);
SCRUTINY_STATUS lcpParserProcessLine(__IN__ char *PtrCurrentLine, __IN__ U32 LineLength, __INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary);
SCRUTINY_STATUS lcpParserProcessSegmentHeader (__IN__ char *PtrCurrentLine, __IN__ U32 LineLength, __INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary);
SCRUTINY_STATUS lcpParserProcessSegmentEntry (__IN__ char *PtrCurrentLine, __IN__ U32 LineLength, __INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary);
SCRUTINY_STATUS lcpParserAddEntry(__INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary, __IN__ PTR_CONFIG_INI_ENTRIES PtrEntry);
SCRUTINY_STATUS lcpProcessConfigDictionaryEntry (__IN__ PTR_CONFIG_INI_ENTRIES PtrEntry);
SCRUTINY_STATUS lcpInitializeDefaultConfigurations();
SCRUTINY_STATUS lcpEnumerateConfigDictionaries (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary);

VOID lcpiDestryLibraryConfigurations()
{

    if (gPtrLibraryConfigParams)
    {
        sosiMemFree (gPtrLibraryConfigParams->PtrDebugLogFile);
        gPtrLibraryConfigParams->PtrDebugLogFile = NULL;

        sosiMemFree (gPtrLibraryConfigParams);
    }

    gPtrLibraryConfigParams = NULL;

}

SCRUTINY_STATUS lcpiInitializeLibraryConfigurations()
{

    PTR_CONFIG_INI_DICTIONARY ptrConfigDictionary = NULL;

    /* First initialize the global parameter to NULL */

    gPtrLibraryConfigParams = NULL;

    if (lcpiParserProcessINIFile (SCRUTINY_FILE_CONFIG_INI, &ptrConfigDictionary))
    {
        /* We don't have to fail for this as of now. */
        return (SCRUTINY_STATUS_SUCCESS);
    }

    /* Check if we have any valid entries */

    if (!lcpiHasAnyValidEntries (ptrConfigDictionary))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* We have a valid entries. Thus create the gPtrLibraryConfigParams */

    if (lcpInitializeDefaultConfigurations())
    {
        lcpiDestryLibraryConfigurations();
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Now we can enumerate through the INI contents */

    if (lcpEnumerateConfigDictionaries (ptrConfigDictionary))
    {
        lcpiDestryLibraryConfigurations();
        return (SCRUTINY_STATUS_FAILED);
    }

    /* We have to now free all the buffers. */
    lcpiParserDestroyDictionary (ptrConfigDictionary);

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpInitializeDefaultConfigurations()
{
    gPtrLibraryConfigParams = (PTR_SCRUTINY_LIBRARY_CONFIG_PARAMS) sosiMemAlloc (sizeof (SCRUTINY_LIBRARY_CONFIG_PARAMS));

    gPtrLibraryConfigParams->DebugLogLevelForAll = SCRUTINY_DEBUG_LOG_LEVEL_NONE;
    gPtrLibraryConfigParams->DebugLogLevelForHalController = SCRUTINY_DEBUG_LOG_LEVEL_NONE;
    gPtrLibraryConfigParams->DebugLogLevelForHalExpander = SCRUTINY_DEBUG_LOG_LEVEL_NONE;
    gPtrLibraryConfigParams->DebugLogLevelForHalScsi = SCRUTINY_DEBUG_LOG_LEVEL_NONE;
    gPtrLibraryConfigParams->DebugLogLevelForHalSwitch = SCRUTINY_DEBUG_LOG_LEVEL_NONE;
    gPtrLibraryConfigParams->DebugLogLevelForOthers = SCRUTINY_DEBUG_LOG_LEVEL_NONE;

    gPtrLibraryConfigParams->PtrDebugLogFile = NULL;

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpEnumerateConfigDictionaries (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    PTR_CONFIG_INI_DICTIONARY  ptrTemp = NULL;
    U32 index;

    ptrTemp = PtrDictionary;

    while (ptrTemp)
    {

        if (!ptrTemp->PtrIniEntries)
        {
            /* We don't have any entries. Thus get next list */
            ptrTemp = ptrTemp->PtrNext;
            continue;
        }

        for (index = 0; index < ptrTemp->TotalEntries; index++)
        {
            lcpProcessConfigDictionaryEntry (&ptrTemp->PtrIniEntries[index]);
        }

        ptrTemp = ptrTemp->PtrNext;

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpProcessConfigDictionaryEntry (__IN__ PTR_CONFIG_INI_ENTRIES PtrEntry)
{

    if (sosiStringLength (PtrEntry->Key) < 1)
    {
        /* We have nothing to do for this. */
        return (SCRUTINY_STATUS_FAILED);
    }

    if ((sosiStringCompare (PtrEntry->Key, SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_ALL)) == SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLibraryConfigParams->DebugLogLevelForAll = sosiAtoi (PtrEntry->Value);
    }

    else if ((sosiStringCompare(PtrEntry->Key, SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_GENERIC)) == SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLibraryConfigParams->DebugLogLevelForOthers = sosiAtoi (PtrEntry->Value);
    }

    else if ((sosiStringCompare (PtrEntry->Key, SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_EXPANDER)) == SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLibraryConfigParams->DebugLogLevelForHalExpander = sosiAtoi (PtrEntry->Value);
    }

    else if ((sosiStringCompare (PtrEntry->Key, SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_SWITCH)) == SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLibraryConfigParams->DebugLogLevelForHalSwitch = sosiAtoi (PtrEntry->Value);
    }

    else if ((sosiStringCompare (PtrEntry->Key, SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_SCSI)) == SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLibraryConfigParams->DebugLogLevelForHalScsi = sosiAtoi (PtrEntry->Value);
    }

    else if ((sosiStringCompare (PtrEntry->Key, SCRUTINY_CONFIG_FILE_KEY_LOG_LEVEL_CONTROLLER)) == SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLibraryConfigParams->DebugLogLevelForHalController = sosiAtoi (PtrEntry->Value);
    }

    else if (sosiStringCompare (PtrEntry->Key, SCRUTINY_CONFIG_FILE_KEY_LOG_FILE) == SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLibraryConfigParams->PtrDebugLogFile = (char *) sosiMemAlloc (sosiStringLength (PtrEntry->Value) + 2);
        sosiStringCopy (gPtrLibraryConfigParams->PtrDebugLogFile, PtrEntry->Value);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpiParserDestroyDictionary (__OUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    if (PtrDictionary == NULL)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }

    if (PtrDictionary->PtrNext)
    {
        lcpiParserDestroyDictionary (PtrDictionary->PtrNext);
    }

    /* We have to clear this binary */

    if (PtrDictionary->PtrIniEntries)
    {
        sosiMemFree (PtrDictionary->PtrIniEntries);
    }

    sosiMemFree (PtrDictionary);

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpiParserProcessINIFile (__IN__ const char* PtrFileName, __OUT__ PTR_CONFIG_INI_DICTIONARY *PtrDictionary)
{

    U32 size = 0;
    PU8 ptrBuffer = NULL;

    PTR_CONFIG_INI_DICTIONARY ptrTempDictionary;

    if (PtrFileName == NULL)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (sosiFileBufferRead (PtrFileName, &ptrBuffer, &size))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (size < 3)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    ptrTempDictionary = (PTR_CONFIG_INI_DICTIONARY) sosiMemAlloc (sizeof (CONFIG_INI_DICTIONARY));

    sosiMemSet (ptrTempDictionary, 0, sizeof (CONFIG_INI_DICTIONARY));

    if (lcpParserParseIniContent (ptrBuffer, size, ptrTempDictionary))
    {
        lcpiParserDestroyDictionary (ptrTempDictionary);
        return (SCRUTINY_STATUS_FAILED);
    }

    if (ptrTempDictionary->PtrIniEntries == NULL &&
        ptrTempDictionary->PtrNext != NULL &&
        sosiStringLength (ptrTempDictionary->SegmentName) < 1)
    {
        ptrTempDictionary = ptrTempDictionary->PtrNext;
    }

    *PtrDictionary = ptrTempDictionary;

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpParserParseIniContent (__IN__ PU8 PtrBuffer, __IN__ U32 BufferSize, __OUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    U32 currentOffset = 0;
    U32 lineLength = 0;

    char currentLine[INI_PARSER_MAXIMUM_CHAR_LENGTH * 2];

    SCRUTINY_STATUS status;

    PTR_CONFIG_INI_DICTIONARY ptrCurrentDictionaryPage = PtrDictionary;

    while (TRUE)
    {

        if (currentOffset >= BufferSize)
        {
            break;
        }

        sosiMemSet (currentLine, 0, sizeof (currentLine));

        status = lcpParserGetNextLine (PtrBuffer, BufferSize, &currentOffset, currentLine, &lineLength);

        if (status)
        {
            return (status);
        }

        sosiStringTrim (currentLine);

        if ((currentLine[0] == ';') ||
            (currentLine[0] == '#') ||
            (sosiStringLength (currentLine) < 1))
        {
            /* Ignore this line */
            continue;
        }

        status = lcpParserProcessLine (currentLine, lineLength, ptrCurrentDictionaryPage);

        if (ptrCurrentDictionaryPage->PtrNext)
        {
            /* We need to swap get the next page */
            ptrCurrentDictionaryPage = ptrCurrentDictionaryPage->PtrNext;
        }

    }

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpParserGetNextLine (__IN__ PU8 PtrBuffer, __IN__ U32 BufferLength, __INOUT__ PU32 PtrRunningOffset, __OUT__ char* PtrLineBuffer, __OUT__ PU32 PtrLineLength)
{

    U32 runningOffset = *PtrRunningOffset;
    U32 lineOffset = 0;

    while (TRUE)
    {

        /* We have reached the EOF */
        if (runningOffset >= BufferLength)
        {
            *PtrRunningOffset = runningOffset;
            *PtrLineLength = lineOffset;

            return (SCRUTINY_STATUS_SUCCESS);
        }

        /* Check if we have a line termination */
        if (PtrBuffer[runningOffset] == '\n' || PtrBuffer[runningOffset] == '\r' || PtrBuffer[runningOffset] == '\0')
        {
            runningOffset++;

            *PtrRunningOffset = runningOffset;
            *PtrLineLength = lineOffset;

            return (SCRUTINY_STATUS_SUCCESS);
        }

        /* Check if the character is good */
        if (sosiIsPrint (PtrBuffer[runningOffset]) < 1)
        {
            /* We don't recognize the character. */
            return (SCRUTINY_STATUS_FAILED);
        }

        /* We need to store the value */
        PtrLineBuffer[lineOffset] = PtrBuffer[runningOffset];

        runningOffset++;
        lineOffset++;

        if (lineOffset > INI_PARSER_MAXIMUM_CHAR_LENGTH)
        {
            return (SCRUTINY_STATUS_FAILED);
        }
    }

    return (SCRUTINY_STATUS_FAILED);

}

SCRUTINY_STATUS lcpParserProcessLine(__IN__ char *PtrCurrentLine, __IN__ U32 LineLength, __INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    if (PtrCurrentLine[0] == '[')
    {
        /* We need to process segment header. */
        return (lcpParserProcessSegmentHeader (PtrCurrentLine, LineLength, PtrDictionary));
    }

    else
    {
        /* We need to process segment entry */
        return (lcpParserProcessSegmentEntry (PtrCurrentLine, LineLength, PtrDictionary));
    }

}

SCRUTINY_STATUS lcpParserProcessSegmentHeader (__IN__ char *PtrCurrentLine, __IN__ U32 LineLength, __INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    PTR_CONFIG_INI_DICTIONARY ptrTemp;

    if (PtrCurrentLine[0] != '[' || PtrCurrentLine[LineLength - 1] != ']')
    {
        /* We have an invalid INI header format. */
        return (SCRUTINY_STATUS_FAILED);
    }

    ptrTemp = (PTR_CONFIG_INI_DICTIONARY) sosiMemAlloc (sizeof (CONFIG_INI_DICTIONARY));

    ptrTemp->PtrIniEntries = NULL;
    ptrTemp->PtrNext = NULL;

    /* We have a valid line. */
    if (LineLength > 2)
    {
        sosiStringCopy (ptrTemp->SegmentName, &PtrCurrentLine[1]);
        ptrTemp->SegmentName[LineLength - 2] = '\0';
    }

    PtrDictionary->PtrNext = ptrTemp;

    return (SCRUTINY_STATUS_SUCCESS);

}

SCRUTINY_STATUS lcpParserProcessSegmentEntry (__IN__ char *PtrCurrentLine, __IN__ U32 LineLength, __INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    U32 offset= 0;
    U32 charOffset = 0;
    PTR_CONFIG_INI_ENTRIES ptrEntry;

    ptrEntry = (PTR_CONFIG_INI_ENTRIES) sosiMemAlloc (sizeof (CONFIG_INI_ENTRIES));

    sosiMemSet (ptrEntry->Key, 0, sizeof (ptrEntry->Key));
    sosiMemSet (ptrEntry->Value, 0, sizeof (ptrEntry->Value));

    for (offset = 0; offset < LineLength; offset++)
    {
        if (PtrCurrentLine[offset] == '=')
        {
            break;
        }

        else
        {
            ptrEntry->Key[charOffset] = PtrCurrentLine[offset];
            charOffset++;
        }
    }

    charOffset = 0;

    for (offset = offset + 1; offset < LineLength; offset++)
    {
        if (PtrCurrentLine[offset] == '\0' || PtrCurrentLine[offset] == '#')
        {
            /* break now */
            break;
        }

        else
        {
            ptrEntry->Value[charOffset] = PtrCurrentLine[offset];
            charOffset++;
        }
    }

    sosiStringTrim (ptrEntry->Key);
    sosiStringTrim (ptrEntry->Value);

    return (lcpParserAddEntry (PtrDictionary, ptrEntry));

}

SCRUTINY_STATUS lcpParserAddEntry (__INOUT__ PTR_CONFIG_INI_DICTIONARY PtrDictionary, __IN__ PTR_CONFIG_INI_ENTRIES PtrEntry)
{

    PTR_CONFIG_INI_ENTRIES ptrExisting = 0;

    if (PtrDictionary->PtrIniEntries == NULL)
    {
        /* We have only one. So just add the entry */
        PtrDictionary->PtrIniEntries  = PtrEntry;
        PtrDictionary->TotalEntries = 1;

        return (SCRUTINY_STATUS_SUCCESS);

    }

    ptrExisting = PtrDictionary->PtrIniEntries;
    PtrDictionary->TotalEntries++;

    PtrDictionary->PtrIniEntries = (PTR_CONFIG_INI_ENTRIES) sosiMemAlloc (PtrDictionary->TotalEntries * sizeof (CONFIG_INI_ENTRIES));

    sosiMemCopy (PtrDictionary->PtrIniEntries, ptrExisting, ((PtrDictionary->TotalEntries - 1) * sizeof (CONFIG_INI_ENTRIES)));

    sosiMemCopy (&PtrDictionary->PtrIniEntries[(PtrDictionary->TotalEntries - 1)], PtrEntry, sizeof (CONFIG_INI_ENTRIES));

    sosiMemFree (PtrEntry);
    sosiMemFree (ptrExisting);

    return (SCRUTINY_STATUS_SUCCESS);

}


BOOLEAN lcpiHasAnyValidEntries (__IN__ PTR_CONFIG_INI_DICTIONARY PtrDictionary)
{

    PTR_CONFIG_INI_DICTIONARY  ptrTemp = NULL;

    ptrTemp = PtrDictionary;

    while (ptrTemp)
    {

        if (ptrTemp->PtrIniEntries)
        {
            return (TRUE);
        }

        ptrTemp = ptrTemp->PtrNext;
    }

    return (FALSE);

}

