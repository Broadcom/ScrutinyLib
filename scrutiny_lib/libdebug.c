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

PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerGeneric = NULL;
PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerExpanders = NULL;
PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerController = NULL;
PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerSwitch = NULL;
PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL  gPtrLoggerScsi = NULL;
PTR_SCRUTINY_DEBUG_LOGGER           gPtrLoggerOutput = NULL;

static U32 sIndent = 0;

VOID ldlInitializeLoggerFromConfigParams (__IN__ PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule, __IN__ U32 LogLevel);
VOID ldlInitializeLoggingForModule (__IN__ PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule);
VOID ldlInitializeLoggingForConfigLevels();
VOID ldlInitializeLoggingLevelForAllModule (__IN__ U32 LogLevel);
VOID ldlInitializeDefaultLoggingForAllModule();
VOID ldlLogBuffer (__IN__ const char* PtrArguments, __IN__ const char *PtrPrefix, __IN__ va_list VariableArguments, __IN__ BOOLEAN LineFeed);
VOID ldliLogFileBufferWrite (__IN__ const char *PtrLineEntry);
VOID ldliConsoleBufferWrite (__IN__ const char* PtrLineEntry);
VOID ldliEmptyOutput (__IN__ const char* PtrLineEntry);
/**
 *
 * @method  ldliInitializeDefaultLogging()
 *
 * @brief   This function initialize default logging. 
 *
 */

VOID ldliInitializeDefaultLogging()
{

    if (!gPtrLoggerOutput)
    {
    	gPtrLoggerOutput = (PTR_SCRUTINY_DEBUG_LOGGER) sosiMemAlloc (sizeof (SCRUTINY_DEBUG_LOGGER));
        gPtrLoggerOutput->logiOutput = &ldliEmptyOutput;
    }

    gPtrLoggerExpanders = (PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL) sosiMemAlloc (sizeof (SCRUTINY_DEBUG_LOGGER_INTERNAL));
    gPtrLoggerController = (PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL) sosiMemAlloc (sizeof (SCRUTINY_DEBUG_LOGGER_INTERNAL));
    gPtrLoggerSwitch = (PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL) sosiMemAlloc (sizeof (SCRUTINY_DEBUG_LOGGER_INTERNAL));
    gPtrLoggerScsi = (PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL) sosiMemAlloc (sizeof (SCRUTINY_DEBUG_LOGGER_INTERNAL));
    gPtrLoggerGeneric = (PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL) sosiMemAlloc (sizeof (SCRUTINY_DEBUG_LOGGER_INTERNAL));

    ldlInitializeLoggingForModule (gPtrLoggerExpanders);
    ldlInitializeLoggingForModule (gPtrLoggerController);
    ldlInitializeLoggingForModule (gPtrLoggerSwitch);
    ldlInitializeLoggingForModule (gPtrLoggerScsi);
    ldlInitializeLoggingForModule (gPtrLoggerGeneric);

    ldlInitializeLoggingForConfigLevels();

}

/**
 *
 * @method  ldliInitializeLogging()
 *
 * @param   PtrLogger Logger function pointer
 *
 * @brief   This function assign the logger provided by top layers
 *
 */

VOID ldliInitializeLogging (__IN__ PTR_SCRUTINY_DEBUG_LOGGER PtrLogger)
{

    /*
     * Check, if we already have debug logs initialized by the app otherwise,
     * initialize the library once again.
     */

    if (!gPtrLoggerGeneric)
    {
        ldliInitializeDefaultLogging();
    }

    /*
     * We need to reinitialize only the function pointers. No need to worry about the
     * global library logger memory. This can be preserved as it is.
     */

    if (PtrLogger->logiOutput)
    {
        gPtrLoggerOutput->logiOutput = PtrLogger->logiOutput;
    }

}

VOID ldliEmptyOutput (__IN__ const char* PtrLineEntry)
{

}

/**
 *
 * @method  ldliConsoleDebug()
 *
 * @param   PtrArguments        Test to be written to buffer
 *
 * @brief   This function will print the message to screen with line feed and
 *          debug tag will be added in front of the message
 *
 */

VOID ldliLogDebug (const char* PtrArguments, ...)
{

    va_list vaargs;

    va_start (vaargs, PtrArguments);

    ldlLogBuffer (PtrArguments, "[DEBUG  ]: ", vaargs, TRUE);

    va_end (vaargs);

}

/**
 *
 * @method  ldliConsoleVerbose()
 *
 * @param   PtrArguments        Test to be written to buffer
 *
 * @brief   This function will print the message to screen with line feed and
 *          verbose tag will be added in front of the message
 *
 */

VOID ldliLogVerbose (const char* PtrArguments, ...)
{
    va_list vaargs;

    va_start (vaargs, PtrArguments);

    ldlLogBuffer (PtrArguments, "[VERBOSE]: ", vaargs, TRUE);

    va_end (vaargs);

}

/**
 *
 * @method  ldliConsoleFunctionEntry()
 *
 * @param   PtrArguments    Test to be written to buffer
 *
 * @brief   This function will print the message to screen with line feed and
 *            printes as Warning message
 *
 */

VOID ldliLogFunctionEntry (const char* PtrArguments, ...)
{

    va_list vaargs;

    va_start (vaargs, PtrArguments);

    ldlLogBuffer (PtrArguments, "[FENTER ]: ", vaargs, TRUE);

    sIndent++;

    va_end (vaargs);

}

/**
 *
 * @method  ldliConsoleFunctionExit()
 *
 * @param   PtrArguments    Test to be written to buffer
 *
 * @brief   This function will print the message to screen with line feed and
 *          printes as ERROR tag
 *
 */

VOID ldliLogFunctionExit (const char* PtrArguments, ...)
{

    va_list vaargs;

    va_start (vaargs, PtrArguments);

    sIndent--;

    ldlLogBuffer (PtrArguments, "[FEXIT  ]: ", vaargs, TRUE);

    va_end (vaargs);

}

/**
 *
 * @method  ldliEmptyHearse()
 *
 * @param   PtrArguments    Arguments that wants to handled
 *
 * @brief   This method does not handle any of the logging actions, calling to
 *          this function just returns.
 *
 */

VOID ldliEmptyHearse (const char *PtrArguments, ...)
{
    /* DO NOTHING */
}


/**
 *
 * @method  ldliEmptyHearseDumpHexMemory()
 *
 * @param   PtrArguments    Arguments that wants to handled
 *
 * @brief   This method does not handle any of the logging actions, calling to
 *          this function just returns.
 *
 */

VOID ldliEmptyHearseDumpHexMemory (PU8 PtrBuffer, U32 SizeInBytes, PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule)
{
    /* DO NOTHING */
}

/**
 *
 * @method  ldliDumpHexMemory()
 *
 * @param   PtrArguments    Arguments that wants to handled
 *
 * @brief   This method will dump the memory in hexa decimal format
 *
 */

VOID ldliDumpHexMemory (PU8 PtrBuffer, U32 SizeInBytes, PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule)
{

    U32 index = 0, col = 0, temp;
    U32 address = 0;

    char buffer[1024] = { 0 };

    if (PtrBuffer == NULL || SizeInBytes < 1)
    {
        PtrModule->logiVerbose ("Invalid memory for dump. PtrBuffer=%x, SizeInBytes=%x", PtrBuffer, SizeInBytes);
        return;
    }

    if (SizeInBytes % 4 == 0)
    {

        /*
         * We need to print in 32bit format.
         */

        PtrModule->logiVerbose ("%-8s  %-8s %-8s %-8s %-8s", " ", "0 ", "4 ", "8 ", "C ");

        for (index = 0; index < (SizeInBytes / 4); index += 4)
        {

            sosiSprintf (buffer, sizeof (buffer), "%08x: ", address);

            temp = ((SizeInBytes / 4) - (index + 4) > (SizeInBytes / 4)) ? (SizeInBytes / 4) - index: 4;

            for (col = 0; col < temp; col++)
                sosiSprintf (buffer, sizeof (buffer), "%s%08x ", buffer, ((PU32) PtrBuffer)[index + col]);

            if (col < 4)
            {
                for (col = temp; col < 4; col++)
                   sosiSprintf (buffer, sizeof (buffer), "%s%8s ", buffer, " ");

            }

            sosiSprintf (buffer, sizeof (buffer), "%s  ", buffer);

            for (col = 0; col < temp * 4; col++)
            {

                if (col != 0 && col % 4 == 0)
                    sosiSprintf (buffer, sizeof (buffer), "%s ", buffer);

                if ((((PU8) PtrBuffer)[index * 4 + col] >= 0x20) && (((PU8) PtrBuffer)[index * 4 + col] <= 0x7E))
                    sosiSprintf (buffer, sizeof (buffer), "%s%c", buffer, ((PU8) PtrBuffer)[index * 4 + col]);
                else
                    sosiSprintf (buffer, sizeof (buffer), "%s.", buffer);

            }

            address += 16;

            PtrModule->logiVerbose ("%s", buffer);

        }

    }

    /*
     * We need to print in 16bit format.
     */

    else
    {

        /*
         * We need to print in 8bit format.
         */

        PtrModule->logiVerbose ("%-8s  %-2s %-2s %-2s %-2s %-2s %-2s %-2s %-2s",
                                      " ", "0 ", "1 ", "2 ", "3 ", "4 ", "5 ", "6 ", "7 ");

        for (index = 0; index < SizeInBytes; index += 8)
        {

            sosiSprintf (buffer, sizeof (buffer), "%08x ", address);

            temp = (SizeInBytes - index + 8) < SizeInBytes ? SizeInBytes - index: 8;

            for (col = 0; col < temp; col++)
                sosiSprintf (buffer, sizeof (buffer), "%s%02x ", buffer, PtrBuffer[index + col]);

            address += 8;

            PtrModule->logiVerbose ("%s", buffer);

        }

    }

}


/**
 *
 * @method  ldliFileBufferWrite()
 *
 * @param   PtrLine     Message that needs to be written to file. This line is
 *                      already processed with variable arguments
 *
 * @param   LineFeed    Do we need a line feed after the message?
 *
 * @param   PtrPrefix   Prefix to the log printed can be Verbose, warning, error
 *                      etc. It can also be empty.
 *
 * @return  VOID        No return
 *
 * @brief   Message opens the file and writes the line into the file closes it.
 *
 */

VOID ldliLogFileBufferWrite (__IN__ const char *PtrLineEntry)
{

    SOSI_FILE_HANDLE ptrFile;

    if (gPtrLibraryConfigParams->PtrDebugLogFile)
    {
        ptrFile = sosiFileOpen (gPtrLibraryConfigParams->PtrDebugLogFile, "a+");
    }

    else
    {
        ptrFile = sosiFileOpen (SCRUTINY_FILE_DEFAULT_LOG, "a+");
    }


    if (!ptrFile)
    {
        return;
    }

    sosiFileWrite (ptrFile, (PU8) PtrLineEntry, sosiStringLength (PtrLineEntry));

    sosiFileClose (ptrFile);

}

/**
 *
 * @method  ldliConsoleBufferWrite()
 *
 * @param   PtrLineEntry Pointer to the data which has to be printed
 *
 * @brief   This function will print the message to screen
 *
 */

VOID ldliConsoleBufferWrite (__IN__ const char* PtrLineEntry)
{

    #ifdef OS_UEFI
    AsciiPrint (PtrLineEntry);
    #else
    printf("%s", PtrLineEntry);
    #endif

}

/**
 *
 * @method  ldlConsoleDirect()
 *
 * @param   PtrArguments         Test to be written to buffer
 *
 * @param   VariableArguments    Arguments to be printed on screen
 *
 * @param   LineFeed             Bool value flag for a new line
 *
 * @brief   This function will print the message to screen
 *
 */

VOID ldlLogBuffer (__IN__ const char* PtrArguments, __IN__ const char *PtrPrefix, __IN__ va_list VariableArguments, __IN__ BOOLEAN LineFeed)
{

    char buffer[2048] = { 0 };
    char logentry[2048] = { 0 };
    char prefix[256] = { 0 };
    U32 index;

    #ifdef OS_UEFI

        CHAR8 *ptrTemp = (CHAR8*) PtrArguments;

        ptrTemp = efiiFixStringTypeSpecifier (ptrTemp);

        AsciiVSPrint (buffer, sizeof (buffer), (const CHAR8 *) ptrTemp, VariableArguments);

    #else

        vsprintf (buffer, PtrArguments, VariableArguments);

    #endif

    for (index = 0; index < sIndent * 2; index++)
    {
        if (index > 32)
        {
            break;
        }

        prefix[index] = ' ';

    }

    if (PtrPrefix)
    {
        sosiSprintf (prefix, sizeof (prefix), "%s%s", prefix, PtrPrefix);
    }

    if (LineFeed)
    {
        sosiSprintf (logentry, sizeof (logentry), "%s%s\n", prefix, buffer);
    }

    else
    {
        sosiSprintf (logentry, sizeof (logentry), "%s%s", prefix, buffer);
    }

    gPtrLoggerOutput->logiOutput (logentry);

}

/**
 *
 * @method  ldlInitializeLoggingLevelForAllModule()
 *
 * @param   LogLevel 	Logger level, bit 1 - debug, bit 2 - verbose, bit 3- function entry/exit
 *
 * @brief   This function will set the log level
 *
 */

VOID ldlInitializeLoggingLevelForAllModule (__IN__ U32 LogLevel)
{

    U32 index = 0;

    PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL loggerLists[6] = { NULL };

    loggerLists[0] = gPtrLoggerExpanders;
    loggerLists[1] = gPtrLoggerController;
    loggerLists[2] = gPtrLoggerSwitch;
    loggerLists[3] = gPtrLoggerScsi;
    loggerLists[4] = gPtrLoggerGeneric;
    loggerLists[5] = NULL;

    do
    {
        if (loggerLists[index])
        {
            ldlInitializeLoggerFromConfigParams (loggerLists[index], LogLevel);
        }

        else
        {
            break;
        }

        index++;

    } while (TRUE);

}

/**
 *
 * @method  ldlInitializeLoggingForConfigLevels()
 *
 * @brief   This function will set the module based loggers
 *
*/

VOID ldlInitializeLoggingForConfigLevels()
{
    if (!gPtrLibraryConfigParams)
    {
        return;
    }

    if (gPtrLibraryConfigParams->DebugLogLevelForAll)
    {
        ldlInitializeLoggingLevelForAllModule (gPtrLibraryConfigParams->DebugLogLevelForAll);
        return;
    }

    /* We will have to initialize module based logging */
    if (gPtrLibraryConfigParams->DebugLogLevelForHalController)
    {
        ldlInitializeLoggerFromConfigParams (gPtrLoggerController, gPtrLibraryConfigParams->DebugLogLevelForHalController);
    }

    if (gPtrLibraryConfigParams->DebugLogLevelForHalExpander)
    {
        ldlInitializeLoggerFromConfigParams (gPtrLoggerExpanders, gPtrLibraryConfigParams->DebugLogLevelForHalExpander);
    }

    if (gPtrLibraryConfigParams->DebugLogLevelForHalScsi)
    {
        ldlInitializeLoggerFromConfigParams (gPtrLoggerScsi, gPtrLibraryConfigParams->DebugLogLevelForHalScsi);
    }

    if (gPtrLibraryConfigParams->DebugLogLevelForHalSwitch)
    {
        ldlInitializeLoggerFromConfigParams (gPtrLoggerSwitch, gPtrLibraryConfigParams->DebugLogLevelForHalSwitch);
    }

    if (gPtrLibraryConfigParams->DebugLogLevelForOthers)
    {
        ldlInitializeLoggerFromConfigParams (gPtrLoggerGeneric, gPtrLibraryConfigParams->DebugLogLevelForOthers);
    }


}

/**
 *
 * @method  ldlInitializeLoggingForConfigLevels()
 *
 * @brief   This function will set the module based loggers
 *
*/

VOID ldlInitializeLoggingForModule (__IN__ PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule)
{

    PtrModule->logiDebug = &ldliEmptyHearse;
    PtrModule->logiVerbose = &ldliEmptyHearse;
    PtrModule->logiFunctionEntry = &ldliEmptyHearse;
    PtrModule->logiFunctionExit = &ldliEmptyHearse;
    PtrModule->logiDumpMemoryInVerbose = &ldliEmptyHearseDumpHexMemory;

}

/**
 *
 * @method  ldlInitializeLoggingForConfigLevels()
 *
 * @brief   This function will set the module based loggers
 *
*/

VOID ldlInitializeLoggerFromConfigParams (__IN__ PTR_SCRUTINY_DEBUG_LOGGER_INTERNAL PtrModule, __IN__ U32 LogLevel)
{

    if (LogLevel & SCRUTINY_DEBUG_LOG_LEVEL_DEBUG)
    {
        PtrModule->logiDebug = &ldliLogDebug;
    }

    if (LogLevel & SCRUTINY_DEBUG_LOG_LEVEL_VERBOSE)
    {
        PtrModule->logiVerbose = &ldliLogVerbose;
        PtrModule->logiDumpMemoryInVerbose = &ldliDumpHexMemory;
    }

    if (LogLevel & SCRUTINY_DEBUG_LOG_LEVEL_FUNCTIONS)
    {
        PtrModule->logiFunctionEntry = &ldliLogFunctionEntry;
        PtrModule->logiFunctionExit = &ldliLogFunctionExit;
    }

    if (gPtrLibraryConfigParams->PtrDebugLogFile &&
        sosiStringCompare (gPtrLibraryConfigParams->PtrDebugLogFile, "NULL") == SCRUTINY_STATUS_SUCCESS)
    {

        gPtrLoggerOutput->logiOutput = &ldliConsoleBufferWrite;
    }

    else if (gPtrLibraryConfigParams->PtrDebugLogFile)
    {
        gPtrLoggerOutput->logiOutput = &ldliLogFileBufferWrite;
    }

}

