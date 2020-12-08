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

static char sTempString[1024];
static char sTempFmtString[1024];

/**
 *
 * @method  etFreeTraceStructure()
 *
 * @param   PtrTraceStructure      pointer to the internal data structure allocated 
 *
 * 
 * @return  STATUS         Indication Success or Fail
 *
 * @brief   free all resources used for get/decode trace buffer
 *
 *
 */
SCRUTINY_STATUS etFreeTraceStructure (PTR_SCRUTINY_EXP_TRACE_BUFFER PtrTraceStructure)
{
    U32 index;

    gPtrLoggerExpanders->logiFunctionEntry ("etFreeTraceStructure (PtrTraceStructure=%x)", PtrTraceStructure != NULL);

    for (index =0; index < 32; index++)
    {
        if (!PtrTraceStructure->PtrEmbeddedStrings[index])
        {
            continue;
        }

        if (PtrTraceStructure->PtrEmbeddedStrings[index]->PtrEmbeddedStrings)
        {
            sosiMemFree (PtrTraceStructure->PtrEmbeddedStrings[index]->PtrEmbeddedStrings);
            PtrTraceStructure->PtrEmbeddedStrings[index]->PtrEmbeddedStrings = NULL;
        }

        sosiMemFree (PtrTraceStructure->PtrEmbeddedStrings[index]);

        PtrTraceStructure->PtrEmbeddedStrings[index]= NULL;
    }

    gPtrLoggerExpanders->logiFunctionExit ("etFreeTraceStructure (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);
}

/**
 *
 * @method  etParseTraceHeader()
 *
 *
 * @param   PtrTraceBuffer     pointer to the original trace buffer
 *
 * @param   TraceBufferSize    original trace buffer size
 * 
 * @param   PtrDecodeTrace     pointer to decoded trace
 *
 * 
 * @return  STATUS         SUCCESS if it's current region, otherwise, fail
 *
 * @brief   parse trace header and check whether it's valid
 *
 *
 */
SCRUTINY_STATUS etParseTraceHeader (
    __IN__  PU8                           PtrTraceBuffer,
    __IN__  U32                           TraceBufferSize,
    __IN__  PTR_SCRUTINY_EXP_TRACE_BUFFER PtrDecodeTrace
)
{
    gPtrLoggerExpanders->logiFunctionEntry ("etParseTraceHeader (PtrTraceBuffer=%x, TraceBufferSize=0x%x, PtrDecodeTrace=%x)", 
            PtrTraceBuffer != NULL, TraceBufferSize, PtrDecodeTrace != NULL);

    PtrDecodeTrace->TraceVersion = ((PU32) PtrTraceBuffer)[0x00];
    PtrDecodeTrace->TraceLength = ((PU32) PtrTraceBuffer)[0x01];

    PtrDecodeTrace->StringsIncluded = ((((PU32) PtrTraceBuffer)[0x02]) & 0x1) ? TRUE : FALSE;

    PtrDecodeTrace->EmbeddedStringsLengthInBytes = ((PU32) PtrTraceBuffer)[0x03];

    gPtrLoggerExpanders->logiDebug("TraceVersion=0x%x TraceLength=0x%x EmbeddedStringsLengthInBytes=0x%x "
                                   "StringsIncluded=%c", PtrDecodeTrace->TraceVersion,
                                   PtrDecodeTrace->TraceLength, PtrDecodeTrace->EmbeddedStringsLengthInBytes,
                                   (PtrDecodeTrace->StringsIncluded) ? 'Y' : 'N');

    /*
     * Make sure header is valid.
     */

    if (PtrDecodeTrace->TraceVersion != 1) 
    {
        gPtrLoggerExpanders->logiFunctionExit ("etFreeTraceStructure (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    if (PtrDecodeTrace->EmbeddedStringsLengthInBytes > (TraceBufferSize - TRACE_EMBEDDED_STRING_HEADER_OFFSET)) 
    {
        gPtrLoggerExpanders->logiFunctionExit ("etFreeTraceStructure (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    gPtrLoggerExpanders->logiFunctionExit ("etFreeTraceStructure (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  etParseEmbeddedStrings()
 *
 *
 * @param   PtrTraceBuffer     pointer to the original trace buffer
 *
 * @param   TraceBufferSize    original trace buffer size
 * 
 * @param   PtrDecodeTrace     pointer to decoded trace
 *
 * 
 * @return  STATUS         SUCCESS if it's current region, otherwise, fail
 *
 * @brief   retrieve the embedded strings
 *
 *
 */
SCRUTINY_STATUS etParseSubsystemStrings (
    __IN__  PU8 PtrEmbeddedBuffer,
    __IN__  U32 BufferSize,
    __IN__  PTR_SCRUTINY_EXP_TRACE_STRING PtrTraceString
)
{
    gPtrLoggerExpanders->logiFunctionEntry ("etParseSubsystemStrings (PtrEmbeddedBuffer=0x%x, BufferSize=0x%x, PtrTraceString=0x%x)", 
                                            PtrEmbeddedBuffer != NULL, BufferSize, PtrTraceString != NULL );

    PtrTraceString->Subsystem = PtrEmbeddedBuffer [0x00];

    PtrTraceString->TotalStrings = ((PU16) PtrEmbeddedBuffer)[0x1];

    PtrTraceString->StringsLength = ((PU32) PtrEmbeddedBuffer)[0x1];

    PtrTraceString->TraceMask = ((PU32) PtrEmbeddedBuffer)[0x02];

    /*
     * Check whether the system is valid.
     */

    if (!PtrTraceString->StringsLength || !PtrTraceString->TotalStrings ||
        PtrTraceString->StringsLength > BufferSize)
    {
        gPtrLoggerExpanders->logiFunctionExit ("etFreeTraceStructure (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    /*
     * Copy the trace string into the buffer.
     */

    PtrTraceString->PtrEmbeddedStrings = (PU8) sosiMemAlloc (PtrTraceString->StringsLength);

    if (PtrTraceString->PtrEmbeddedStrings == NULL)
    {
        gPtrLoggerExpanders->logiFunctionExit ("etFreeTraceStructure (status=0x%x)", SCRUTINY_STATUS_FAILED);
        return (SCRUTINY_STATUS_FAILED);
    }

    sosiMemCopy (PtrTraceString->PtrEmbeddedStrings,
                PtrEmbeddedBuffer + TRACE_EMBEDDED_STRING_HEADER_LENGTH,
                PtrTraceString->StringsLength);


    gPtrLoggerExpanders->logiFunctionExit ("etFreeTraceStructure (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  etParseEmbeddedStrings()
 *
 *
 * @param   PtrTraceBuffer     pointer to the original trace buffer
 *
 * @param   TraceBufferSize    original trace buffer size
 * 
 * @param   PtrDecodeTrace     pointer to decoded trace
 *
 * 
 * @return  STATUS         SUCCESS if it's current region, otherwise, fail
 *
 * @brief   retrieve the embedded strings
 *
 *
 */
SCRUTINY_STATUS etParseEmbeddedStrings (
    __IN__  PU8                             PtrTraceBuffer,
    __IN__  U32                             TraceBufferSize,
    __IN__  PTR_SCRUTINY_EXP_TRACE_BUFFER   PtrDecodeTrace
)
{
    const char          			*ptrUnkownString = "Unknown";
    PU8                 			ptrTemp;
    U32                 			offset;
    SCRUTINY_STATUS     			status;
    U32                 			dword0, dword1;
    PTR_SCRUTINY_EXP_TRACE_STRING   ptrTraceString;

    gPtrLoggerExpanders->logiFunctionEntry ("etParseEmbeddedStrings (PtrTraceBuffer=0x%x, TraceBufferSize=0x%x, PtrDecodeTrace=0x%x)", 
                                            PtrTraceBuffer != NULL, TraceBufferSize, PtrDecodeTrace != NULL );

    offset = TRACE_EMBEDDED_STRING_HEADER_OFFSET;

    while (offset < TRACE_EMBEDDED_STRING_HEADER_LENGTH + PtrDecodeTrace->EmbeddedStringsLengthInBytes)
	{

        ptrTemp = &PtrTraceBuffer[offset];

        dword0 = ((PU32) ptrTemp)[0];
        dword1 = ((PU32) ptrTemp)[1];

        /* To avoid compiler warnings */
        dword0 = dword0;
		
        if (dword1 < 1)
        {
            break;
        }

        /*
         * Check the length.
         */

        if ( (dword1 + TRACE_EMBEDDED_STRING_HEADER_OFFSET + offset) > TraceBufferSize) {

            break;
        }

        ptrTraceString = (PTR_SCRUTINY_EXP_TRACE_STRING) sosiMemAlloc (sizeof (SCRUTINY_EXP_TRACE_STRING));

        if (ptrTraceString == NULL)
        {
            gPtrLoggerExpanders->logiFunctionExit ("etParseEmbeddedStrings  (status=0x%x)", SCRUTINY_STATUS_FAILED);
            return (SCRUTINY_STATUS_FAILED);
        }

        sosiMemSet (ptrTraceString, 0, sizeof (SCRUTINY_EXP_TRACE_STRING));

        status = etParseSubsystemStrings (PtrTraceBuffer + offset,
                                           (dword1 + TRACE_EMBEDDED_STRING_HEADER_LENGTH),
                                           ptrTraceString);

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            sosiMemCopy(ptrTraceString, ptrUnkownString, sosiStringLength(ptrUnkownString));
        }

        PtrDecodeTrace->PtrEmbeddedStrings[PtrDecodeTrace->SubsytemCounts] = ptrTraceString;
        PtrDecodeTrace->SubsytemCounts++;

        offset += (dword1 + TRACE_EMBEDDED_STRING_HEADER_LENGTH);

    }

    gPtrLoggerExpanders->logiFunctionExit ("etParseEmbeddedStrings  (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);
}


/**
 *
 * @method  etGetStatementSimple()
 *
 *
 * @param   StringIndex    statement string index
 *
 * @param   Subsystem      sub system string index
 * 
 * @param   PtrTraceStructure pointer to the internal data structure used for decode
 *
 * 
 * @return  char*          human-readable statement string 
 *
 * @brief   output the index statement string
 *
 *
 */
char* etGetStatementSimple (
    __IN__  U32 StringIndex,
    __IN__  U32 Subsystem, 
    __IN__  PTR_SCRUTINY_EXP_TRACE_BUFFER PtrTraceStructure
)
{
    U32 index = 0;
    U32 strCount = 0, chrIndex = 0, chrStart = 0;

    gPtrLoggerExpanders->logiFunctionEntry ("etGetStatementSimple (StringIndex=0x%x, Subsystem=0x%x, PtrTraceStructure=%x)",
            StringIndex, Subsystem, PtrTraceStructure != NULL);

    for (index = 0; index < 32; index++)
    {

        if (!PtrTraceStructure->PtrEmbeddedStrings[index]) 
        {
            continue;
        }

        if (PtrTraceStructure->PtrEmbeddedStrings[index]->Subsystem != Subsystem) 
        {
            continue;
        }

        if (PtrTraceStructure->PtrEmbeddedStrings[index]->TotalStrings < StringIndex)
        {
            gPtrLoggerExpanders->logiFunctionExit ("etGetStatementSimple ()");
            return (NULL);
        }

        chrStart = 0;
        strCount = 0;

        sosiMemSet ((char *) sTempString, '\0', 1024);

        for (chrIndex = 2; chrIndex < PtrTraceStructure->PtrEmbeddedStrings[index]->StringsLength; chrIndex++)
        {

            if (PtrTraceStructure->PtrEmbeddedStrings[index]->PtrEmbeddedStrings[chrIndex] == '\0')
            {

                sTempString[chrStart] = '\0';

                if (strCount == StringIndex)
                    return ((char *) sTempString);

                strCount++;

                sosiMemSet ((char *) sTempString, '\0', 1023);

                chrStart = 0;

            }
            else
            {
                sTempString[chrStart] = PtrTraceStructure->PtrEmbeddedStrings[index]->PtrEmbeddedStrings[chrIndex];
                chrStart++;
            }

        }

    }

    gPtrLoggerExpanders->logiFunctionExit ("etGetStatementSimple ()");
    return (NULL);
}

/**
 *
 * @method  etGetStatement()
 *
 *
 * @param   StringIndex    statement string index
 *
 * @param   Subsystem      sub system string index
 * 
 * @param   PtrTraceStructure pointer to the internal data structure used for decode
 *
 * 
 * @return  char*          human-readable statement string 
 *
 * @brief   get the indexed statement
 *
 *
 */
const char* etGetStatement (
    __IN__  U32 StringIndex, 
    __IN__  U32 Subsystem, 
    __IN__  PTR_SCRUTINY_EXP_TRACE_BUFFER PtrTraceStructure
)
{

    char *ptrTemp = NULL;
    U32 index;

    gPtrLoggerExpanders->logiFunctionEntry ("etGetStatement (StringIndex=0x%x, Subsystem=0x%x, PtrTraceStructure=%x)",
            StringIndex, Subsystem, PtrTraceStructure != NULL);
    /*
     * We have to convert the strings into %p.
     */

    ptrTemp = etGetStatementSimple (StringIndex, Subsystem, PtrTraceStructure);

    if (!ptrTemp)
    {
        gPtrLoggerExpanders->logiFunctionExit ("etGetStatement (ptrTemp=0x%x)", ptrTemp);
        return (NULL);
    }

    index = 0;

    while (ptrTemp[index] != '\0')
    {

        if (ptrTemp[index] == '%' && ptrTemp[index + 1] == 's')
            ptrTemp[index + 1] = 'p';

        index++;

    }

    gPtrLoggerExpanders->logiFunctionExit ("etGetStatement (ptrTemp=0x%x)", ptrTemp);
    return (ptrTemp);

}

/**
 *
 * @method  etProcessTimeStampIfRequired()
 *
 *
 * @param   PtrStmt    pointer to the time stamp 
 *
 * @param   Arg1       argument 1 depends on the PtrStmt
 *
 * @param   Arg2       argument 2 depends on the PtrStmt
 * 
 * @return  U32        returned number of char for the output timestamp
 *
 * @brief   format the time stamp in the trace buffer
 *
 */
U32 etProcessTimeStampIfRequired (
    __IN__ const char* PtrStmt, 
    __IN__ U32 Arg1, 
    __IN__ U32 Arg2
)
{

    U32        numOfChar = 0;

    gPtrLoggerExpanders->logiFunctionEntry ("etProcessTimeStampIfRequired (PtrStmt=%x, Arg1=0x%x, Arg2=0x%x)",
            PtrStmt != NULL, Arg1, Arg2);

    if (!PtrStmt)
    {
        numOfChar = sosiSprintf (sTempFmtString, sizeof(sTempFmtString), "0x%X 0x%X", Arg1, Arg2);
    }

    else if (PtrStmt[0] == 'T' && PtrStmt[1] == 'S' && PtrStmt[2] == 'T' &&
             PtrStmt[3] == 'A' && PtrStmt[4] == 'M' && PtrStmt[5] == 'P')
    {

        numOfChar = sosiSprintf (sTempFmtString,
								sizeof (sTempFmtString),
								"<%02d:%02d:%02d:%02d.%03d> ",
								(Arg1 / (24*60*60*1000)),
								((Arg2 / (60*60*1000)) % 24),
								((Arg2 / (60*1000)) % 60),
								((Arg2 / 1000) % 60),
								((Arg2 % 1000)));

    }

    else
    {
        numOfChar = sosiSprintf (sTempFmtString, sizeof(sTempFmtString), PtrStmt, Arg1, Arg2);
    }

    gPtrLoggerExpanders->logiFunctionExit ("etProcessTimeStampIfRequired (numOfChar=0x%x)", numOfChar);

    return (numOfChar);

}

/**
 *
 * @method  etGetNextStatement()
 *
 *
 * @param   PtrTraceBuffer    pointer to the original trace buffer
 *
 * @param   TraceBufferSize   size of the trace buffer
 *
 * @param   PtrCounterOffset  offset of the trace buffer
 * 
 * @param   PtrTraceStructure pointer to the internal data structure used for decode
 *
 * @param   PtrStatementSize  pointer to the output statement length
 * 
 * @return  STATUS         SUCCESS if it's current region, otherwise, fail
 *
 * @brief   get the next human-readable statement from the trace buffer
 *
 *
 */
SCRUTINY_STATUS etGetNextStatement (
    __IN__ PU32                PtrTraceBuffer,
    __IN__ U32                 TraceBufferSize,
    __IN__ PU32                PtrCounterOffset,
    __IN__ PTR_SCRUTINY_EXP_TRACE_BUFFER PtrTraceStructure,
    __OUT__ PU32               PtrStatementSize
    )
{
    U32 val, idx, sub, arg;
    U32 arg1, arg2, arg3, arg4;
    U32 numOfChar = 0;

    const char* ptrStatement = NULL;

    gPtrLoggerExpanders->logiFunctionEntry ("etGetNextStatement (PtrTraceBuffer=%x, TraceBufferSize=0x%x,"
                                            "PtrCounterOffset=%x, PtrTraceStructure=%x, PtrStatementSize=%x)", 
                                            PtrTraceBuffer != NULL, TraceBufferSize, PtrCounterOffset != NULL,
                                            PtrTraceStructure != NULL, PtrStatementSize != NULL);

    *PtrStatementSize = 0;

    val = PtrTraceBuffer[(*PtrCounterOffset) / 4];
    (*PtrCounterOffset) += 4;

    idx = (val & 0xFFFF);
    sub = ((val >> 16) & 0xFF);
    arg = ((val >> 24) & 0xFF);

    if (arg > 4 || (arg * 4 + ((*PtrCounterOffset) - 4)) >= TraceBufferSize)
    {
        gPtrLoggerExpanders->logiFunctionExit ("etGetNextStatement (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
        return (SCRUTINY_STATUS_SUCCESS);
    }

    ptrStatement = etGetStatement (idx - 1, sub, PtrTraceStructure);

    if (arg == 0 && ptrStatement != NULL)
    {
        numOfChar = sosiSprintf (sTempFmtString, sizeof (sTempFmtString), "%s\n", ptrStatement);
    }
    else if (arg == 1)
    {
        arg1 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        if (!ptrStatement)
        {
            numOfChar = sosiSprintf (sTempFmtString, sizeof (sTempFmtString), "0x%X", arg1);
        }
        else
        {
            numOfChar = sosiSprintf (sTempFmtString, sizeof (sTempFmtString), ptrStatement, arg1);
        }

    }
    else if (arg == 2)
    {
        arg1 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        arg2 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        numOfChar = etProcessTimeStampIfRequired (ptrStatement, arg1, arg2);

    }

    else if (arg == 3)
    {
        arg1 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        arg2 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        arg3 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        if (!ptrStatement)
        {
            numOfChar = sosiSprintf (sTempFmtString, sizeof (sTempFmtString), "0x%X 0x%X 0x%X", arg1, arg2, arg3);
        }
        else
        {
            numOfChar = sosiSprintf (sTempFmtString, sizeof (sTempFmtString), ptrStatement, arg1, arg2, arg3);
        }
		
    }
	else if (arg == 4)
	{

        arg1 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        arg2 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        arg3 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;

        arg4 = PtrTraceBuffer[(*PtrCounterOffset) / 4];
        (*PtrCounterOffset) += 4;


        if (!ptrStatement)
        {
            numOfChar = sosiSprintf (sTempFmtString, sizeof (sTempFmtString), "0x%X 0x%X 0x%X 0x%X", arg1, arg2, arg3, arg4);
        }
        else
        {
            numOfChar = sosiSprintf (sTempFmtString, sizeof (sTempFmtString), ptrStatement, arg1, arg2, arg3, arg4);
        }

    }

    *PtrStatementSize = numOfChar;

    gPtrLoggerExpanders->logiFunctionExit ("etGetNextStatement (status=0x%x)", SCRUTINY_STATUS_SUCCESS);
    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  etDecodeTraceBuffer()
 *
 *
 * @param   PtrTraceBuffer    pointer to the original trace buffer
 *
 * @param   TraceBufferSize   size of the trace buffer
 * 
 * @param   PtrBuffer         pointer to output buffer
 *
 * @param   PtrBufferLength   pointer to the output buffer length
 * 
 * @return  STATUS         SUCCESS if it's current region, otherwise, fail
 *
 * @brief   decode the original trace buffer to human-readable outputs
 *
 *
 */
SCRUTINY_STATUS etDecodeTraceBuffer (
    __IN__ PU8                       PtrTraceBuffer,
    __IN__ U32                       TraceBufferSize,
    __OUT__ PVOID                    PtrBuffer,
    __IN__ PU32                      PtrBufferLength,
    __IN__ SOSI_FILE_HANDLE          PtrFile)
{
    U32                         runningOffset;
    U32                         totalNeededBufferSize = 0;
    U32                         statementSize;
    U32                         bufferOffset, count, remainBufferSize = 0;
    SCRUTINY_EXP_TRACE_BUFFER   traceBufferStruct = { 0 };
    SCRUTINY_STATUS             status = SCRUTINY_STATUS_SUCCESS;

    gPtrLoggerExpanders->logiFunctionEntry ("etDecodeTraceBuffer (PtrTraceBuffer=%x, TraceBufferSize=0x%x, PtrBuffer=%x, PtrBufferLength=%x, PtrFile=%x)", 
                                           PtrTraceBuffer != NULL, TraceBufferSize, PtrBuffer != NULL, PtrBufferLength != NULL, PtrFile != NULL);

    status = etParseTraceHeader (PtrTraceBuffer, TraceBufferSize, &traceBufferStruct);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("etDecodeTraceBuffer (status=0x%x)", status);
        return (status);
    }

    status = etParseEmbeddedStrings(PtrTraceBuffer, TraceBufferSize, &traceBufferStruct);
	
    if (status != SCRUTINY_STATUS_SUCCESS)
    {
		etFreeTraceStructure (&traceBufferStruct);
        gPtrLoggerExpanders->logiFunctionExit ("etDecodeTraceBuffer (status=0x%x)", status);
	    return (status);        
    }

    runningOffset = (TRACE_EMBEDDED_STRING_HEADER_LENGTH + traceBufferStruct.EmbeddedStringsLengthInBytes);
    bufferOffset = 0;

    if (PtrFile == NULL)
    {
        remainBufferSize = *PtrBufferLength;
    }

    while (runningOffset < TraceBufferSize)
    {
        statementSize = 0;

        etGetNextStatement ( (PU32)PtrTraceBuffer,
                                 TraceBufferSize,
                                 (PU32) &runningOffset,
                                 &traceBufferStruct,
                                 &statementSize);

        if (PtrFile != NULL)
        {
            status = sosiFileWrite (PtrFile,(PU8)sTempFmtString, statementSize);
					
        	if (status != SCRUTINY_STATUS_SUCCESS)
        	{
            	gPtrLoggerExpanders->logiFunctionExit ("etDecodeTraceBuffer (status=0x%x)", status);
                return (status);
        	}

            /* if the trace buffer is written to a file, the input buffer is invalid, skip */
            continue;
        }

        totalNeededBufferSize += statementSize;

        //if (totalNeededBufferSize < *PtrBufferLength)
        if (remainBufferSize && statementSize !=0)
        {
            count = (statementSize < remainBufferSize) ? statementSize : remainBufferSize;
            sosiMemCopy ((PU8)PtrBuffer + bufferOffset, sTempFmtString, count);
            bufferOffset += count;
            remainBufferSize -= count;
        }
    }

    if (PtrFile == NULL)
    {
        if (totalNeededBufferSize > *PtrBufferLength)
        {
            /* User might call the API with the returned buffer size, add more size in case 2 calls, the trace buffer
               increased 
             */
            totalNeededBufferSize += SCRUTINY_LIB_EXTRA_BUFFER_SIZE;
            status = SCRUTINY_STATUS_BUFFER_NOT_LARGE_ENOUGH;
        }

        *PtrBufferLength = totalNeededBufferSize;
    }

    etFreeTraceStructure (&traceBufferStruct);

    gPtrLoggerExpanders->logiFunctionExit ("etDecodeTraceBuffer (status=0x%x)", status);
    return (status);
}

/**
 *
 * @method  etUploadTraceBuffer()
 *
 *
 * @param   PtrExpander    pointer to the device
 *
 * @param   PtrTraceBuffer pointer to the output buffer
 * 
 * @param   PtrBufferSize  pointer to output buffer size
 * 
 * @return  STATUS         SUCCESS if it's current region, otherwise, fail
 *
 * @brief   upload the FW trace buffer
 *
 *
 */
SCRUTINY_STATUS etUploadTraceBuffer (
    __IN__   PTR_SCRUTINY_DEVICE       PtrExpander,
    __OUT__  PU8                       *PtrTraceBuffer,
    __OUT__  PU32                      PtrBufferSize
    )
{
    SCRUTINY_STATUS             status;
    PU8                         ptrBuffer;
    U32                         index, tempSize = 0;
    SCRUTINY_SCSI_PASSTHROUGH   scsiRequest = { 0 };

    gPtrLoggerExpanders->logiFunctionEntry ("etUploadTraceBuffer (PtrExpander=%x, PtrTraceBuffer=%x, PtrBufferSize=%x)", 
            PtrExpander != NULL, PtrTraceBuffer != NULL, PtrBufferSize != NULL);

    *PtrTraceBuffer = NULL;
    
    status = bsdiGetRegionSize (PtrExpander, BRCM_SCSI_BUFFER_ID_ACTIVE_TRACES, PtrBufferSize);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        gPtrLoggerExpanders->logiFunctionExit ("etUploadTraceBuffer (status=0x%x)", status);
        return (status);
    }

    ptrBuffer = (PU8) sosiMemAlloc (*PtrBufferSize);
	
    if (ptrBuffer == NULL)
    {
        gPtrLoggerExpanders->logiFunctionExit ("etUploadTraceBuffer (status=0x%x)", SCRUTINY_STATUS_NO_MEMORY);
        return (SCRUTINY_STATUS_NO_MEMORY);
    }

    sosiMemSet (ptrBuffer, 0, *PtrBufferSize);

    scsiRequest.CdbLength = 10;

    scsiRequest.Cdb[0x00]  = SCSI_COMMAND_READ_BUFFER;
    scsiRequest.Cdb[0x01]  = SCSI_READ_MODE_DATA;
    scsiRequest.Cdb[0x02]  = BRCM_SCSI_BUFFER_ID_ACTIVE_TRACES;
    scsiRequest.Cdb[0x09]  = 0x00;

    for (index = 0; index < *PtrBufferSize; index += 0x80000)
    {
        tempSize = (0x80000 < ((*PtrBufferSize) - index)) ? 0x80000 : ((*PtrBufferSize) - index);

        scsiRequest.Cdb[0x03]  = (U8) ((index >> 16) & 0xFF);
        scsiRequest.Cdb[0x04]  = (U8) ((index >> 8) & 0xFF);
        scsiRequest.Cdb[0x05]  = (U8) (index & 0xFF);
        scsiRequest.Cdb[0x06]  = (U8) ((tempSize >> 16) & 0xFF);
        scsiRequest.Cdb[0x07]  = (U8) ((tempSize >> 8) & 0xFF);
        scsiRequest.Cdb[0x08]  = (U8) (tempSize & 0xFF);

        scsiRequest.DataDirection = DIRECTION_READ;
        scsiRequest.PtrDataBuffer = (PVOID) (ptrBuffer + index);
        scsiRequest.DataBufferLength = tempSize;

        status = bsdiPerformScsiPassthrough (PtrExpander, &scsiRequest);

        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            sosiMemFree (ptrBuffer);
            gPtrLoggerExpanders->logiFunctionExit ("etUploadTraceBuffer (status=0x%x)", status);
            return (status);
        }

    }

    *PtrTraceBuffer = ptrBuffer;

    gPtrLoggerExpanders->logiFunctionExit ("etUploadTraceBuffer (status=0x%x)", status);
    return (SCRUTINY_STATUS_SUCCESS);

}


/**
 *
 * @method  etiGetTraceBuffer()
 *
 * @param   PtrDevice      pointer to the device
 *
 * @param   PtrBuffer      pointer to output buffer 
 * 
 * @param   PtrBufferLength  pointer to size of input buffer
 *
 * @param   PtrFolderName   pointer to the folder name incase we need to save trace 
 *                          buffer into a file
 * 
 * @return  STATUS         Indication Success or Fail
 *
 * @brief   API for the FW trace buffer, if PtrFolderName is NULL, dump the trace buffer into 
 *          the input buffer, otherwise, the input buffer is ignored
 *
 *
 */
SCRUTINY_STATUS etiGetTraceBuffer (
    __IN__ PTR_SCRUTINY_DEVICE       PtrDevice,
    __OUT__ PVOID                    PtrBuffer,
    __IN__ PU32                      PtrBufferLength,
    __IN__ const char*               PtrFolderName
    )
{
    SCRUTINY_STATUS        status;
    PU8                    ptrTraceBuffer;
    U32                    bufferSize;
    SOSI_FILE_HANDLE       ptrFile = NULL;
    char                   traceFileName[512];
    char                   tempName[128];

    gPtrLoggerExpanders->logiFunctionEntry ("etiGetTraceBuffer (PtrDevice=%x, PtrBuffer=%x, PtrBufferLength=%x, PtrFolderName=%x)", 
        PtrDevice != NULL, PtrBuffer != NULL, PtrBufferLength != NULL, PtrFolderName != NULL );
    
    if (PtrFolderName == NULL)
    {
        if (PtrBufferLength == NULL)
        {
            gPtrLoggerExpanders->logiFunctionExit ("etiGetTraceBuffer() - status 0x%x", SCRUTINY_STATUS_FAILED);
			
            return (SCRUTINY_STATUS_FAILED);
        }
    }
    else 
    {
        sosiMemSet (traceFileName, '\0', sizeof (traceFileName));
        sosiMemSet (tempName, '\0', sizeof (tempName));
        
        status = sosiMkDir (PtrFolderName);
		
        if (status != SCRUTINY_STATUS_SUCCESS)
        {
            gPtrLoggerExpanders->logiFunctionExit ("etiGetTraceBuffer() - status 0x%x", status);
            return (status);
        }


        sosiStringCopy (traceFileName, PtrFolderName);

        if (PtrDevice->ProductFamily == SCRUTINY_PRODUCT_FAMILY_EXPANDER)
        {
            sosiSprintf (tempName, sizeof (tempName), "tracebuffer.txt");
        }
        else
        {
            sosiSprintf (tempName, sizeof (tempName), "tracebuffer.txt");
        }

        sosiStringCat (traceFileName, tempName);
        ptrFile = sosiFileOpen (traceFileName, "wb");

        if (!ptrFile)
		{	
			gPtrLoggerExpanders->logiFunctionExit ("etiGetTraceBuffer() - status 0x%x", SCRUTINY_STATUS_FILE_OPEN_FAILED);
			return (SCRUTINY_STATUS_FILE_OPEN_FAILED);	
		}	
        
    }


    status = etUploadTraceBuffer (PtrDevice, &ptrTraceBuffer, &bufferSize);

    if (status != SCRUTINY_STATUS_SUCCESS)
    {
        if (PtrFolderName != NULL && ptrFile != NULL)
        {
            sosiFileClose (ptrFile); 
        }
        gPtrLoggerExpanders->logiFunctionExit ("etiGetTraceBuffer() - status 0x%x", status);
        return (status);
    }

    status = etDecodeTraceBuffer (ptrTraceBuffer, bufferSize, PtrBuffer, PtrBufferLength, ptrFile);

    if (PtrFolderName != NULL && ptrFile != NULL)
    {
        sosiFileClose (ptrFile); 
    }
    
    sosiMemFree (ptrTraceBuffer);
	
    gPtrLoggerExpanders->logiFunctionExit ("etiGetTraceBuffer() - status 0x%x", status);
    return (status);
}


