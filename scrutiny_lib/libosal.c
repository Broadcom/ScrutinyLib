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

#if defined (OS_UEFI)

    EFI_HANDLE               gLibUdkImageHandle = NULL;
    EFI_SYSTEM_TABLE         *gLibUdkSystemTable = NULL;
	
#endif

#if defined (OS_UEFI) && !defined (TARGET_ARCH_ARM)


    /**
     *
     *  For UEFI, memset and memcpy functions are to be rewritten for native
     *  methods.
     *
     *  Using the allocation i.e. u8 temp[5] = { 0 }; will automatically generate
     *  memset or memcpy functions internally by the compiler. To redirect the same,
     *  we will have to initiate the pragma for these compiler generated calls.
     *
     */

    #pragma function (memset)

    #pragma function (memcpy)
	
#endif

#if defined (OS_UEFI)
/**
 *
 * @method  lefiStrCopyUtoA(CHAR8 *PtrDest, CHAR16 *PtrSrc)
 *
 * @param   PtrSrc - Source string
 *
 * @param   PtrDest - Destination string
 *f
 * @return    TRUE
 *
 * @brief      This function is to convert the unicode string to ascii string
 *
*/
BOOLEAN lefiStrCopyUtoA (CHAR8 *PtrDest, CHAR16 *PtrSrc)
{
    UINTN index;

    for (index=0; PtrSrc[index]; index++)
    {
        PtrDest[index] = (CHAR8) PtrSrc[index];
    }

    PtrDest[index] = 0;

    return (TRUE);
}




/**
 *
 * @method  BSDWaitForSingleEvent ()
 *
 * @return    EFI STATUS indicating pass or fail
 *
 * @brief     This module is to create a event and wait for particular event to happen
 *
 *
*/

EFI_STATUS lefiBSDWaitForSingleEvent (IN EFI_EVENT Event, IN UINT64 Timeout OPTIONAL)
{
    EFI_STATUS          status = EFI_SUCCESS;
    UINTN               index = 0;
    EFI_EVENT           timerEvent;
    EFI_EVENT           waitList[2];

    if (Timeout)
    {
        //
        // Create a timer event
        //

        status = gBS->CreateEvent (EFI_EVENT_TIMER, 0, NULL, NULL, &timerEvent);
        if (!EFI_ERROR (status))
        {

            //
            // Set the timer event
            //

            gBS->SetTimer (timerEvent, TimerRelative, Timeout);

            //
            // Wait for the original event or the timer
            //

            waitList[0] = Event;
            waitList[1] = timerEvent;
            status = gBS->WaitForEvent (2, waitList, &index);
            gBS->CloseEvent (timerEvent);

            //
            // If the timer expired, change the return to timed out
            //

            if (!EFI_ERROR (status)  &&  index == 1)
            {
                status = EFI_TIMEOUT;
            }
        }

    }
    else
    {
        //
        // No timeout... just wait on the event
        //

        status = gBS->WaitForEvent (1, &Event, &index);

        if (EFI_ERROR (status) || (index != 0))
        {
            return (status);
        }
    }

    return (status);
}

/**
 *
 * @method  lefiLsiInput ()
 *
 * @brief   To get the input from keyboard event Input a string
 *          at the current cursor location, for StrLen
 *
 */

VOID lefiLsiInput (IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut, IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn,
                  IN CHAR16 *Prompt OPTIONAL, OUT CHAR16 *PtrInStr, IN UINTN StrLen)
{

    EFI_INPUT_KEY                   key;
    EFI_STATUS                      status;
    UINTN                           len;

    if (Prompt)
    {
        ConOut->OutputString (ConOut, Prompt);
    }

    len = 0;

    for ( ; ; )
    {
        lefiBSDWaitForSingleEvent (ConIn->WaitForKey, 0);

        status = ConIn->ReadKeyStroke (ConIn, &key);

        if (EFI_ERROR(status))
        {
            break;
        }

        if (key.UnicodeChar == '\n' || key.UnicodeChar == '\r')
        {
            break;
        }

        if (key.UnicodeChar == '\b')
        {
            if (len)
            {
                ConOut->OutputString (ConOut, L"\b \b");
                len -= 1;
            }

            continue;
        }

       if (len < StrLen-1)
       {
           PtrInStr[len] = key.UnicodeChar;

           PtrInStr[len+1] = 0;
           ConOut->OutputString (ConOut, &PtrInStr[len]);

           len += 1;
       }

       continue;

    }

    PtrInStr[len] = 0;
}

/**
 *
 * @method  lefiInput ()
 *
 * @brief     To get the input from keyboard event
 *
 *
*/

VOID
lefiInput (
    IN CHAR16    *Prompt OPTIONAL,
    OUT CHAR16   *PtrInStr,
    IN UINTN     StrLen
    )
// Input a string at the current cursor location, for StrLen
{
    lefiLsiInput (
        gLibUdkSystemTable->ConOut,
        gLibUdkSystemTable->ConIn,
        Prompt,
        PtrInStr,
        StrLen
        );
}

/**
 *
 * @method  lefiiGetInputString()
 *
 * @brief     To get the input from keyboard event
 *
 *
 */

char * lefiiGetInputString (char *PtrInput, U32 sizeOfInput)
{
    CHAR16  inputString[1024];

    lefiInput (NULL, inputString, 1024);

    lefiStrCopyUtoA (PtrInput, inputString);

    AsciiPrint ("\n");

    return (PtrInput);
}

/**
 *
 * @method  lefiStrCopyUtoA(CHAR8 *PtrDest, CHAR16 *PtrSrc)
 *
 * @param   PtrSrc - Source string
 *
 * @param   PtrDest - Destination string
 *
 * @return    TRUE/FALSE
 *
 * @brief      This function is to convert the ascii string to unicode string
 *
 */

BOOLEAN lefiStrCopyAtoU (CHAR16 *PtrDest, const char *PtrSrc, UINTN Size)
{
    UINTN index;
    UINTN len;

    len = AsciiStrLen (PtrSrc);

    for (index = 0; index < (Size-1) && index < len; index++)
    {
        PtrDest[index] = (CHAR16) PtrSrc[index];
    }

    PtrDest[index] = 0;

    if (index < (Size - 1) || len == (Size - 1))
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}


/**
 *
 * @method  lefiFileOpen(EFI_HANDLE Udk_ImageHandle,     CHAR16 *pFileName, EFI_FILE_HANDLE *pFile, UINT64 Mode, UINT64 Attributes)
 *
 * @param    pFileName File name
 *
 * @param    Mode    In which mode the file has to be opened
 *
 * @return   STATUS EFI File status
 *
 * @brief     function to open a file
 *
 */

EFI_STATUS lefiFileOpen (EFI_HANDLE gUdk_ImageHandle, CHAR16 *PtrFileName, EFI_FILE_HANDLE *PtrFile,
                        UINT64 Mode, UINT64 Attributes)
{

    EFI_STATUS  status = RET_VAL_FAILURE;

    status = ShellOpenFileByName (PtrFileName, (SHELL_FILE_HANDLE *)PtrFile, Mode, 0);

    if (EFI_ERROR (status))
    {
        if (PtrFile != NULL)
        {
            PtrFile = NULL;
        }
    }

    return status;

}

/**
 *
 * @method  lefiiFileOpen(const char *PtrFileName, char *mode)
 *
 * @param    PtrFileName File to open
 *
 * @param    PtrMode    In which mode the file has to be opened
 *
 * @return    File handle
 *
 * @brief     function to open a file
 *
 */

SOSI_FILE_HANDLE lefiiFileOpen (const char *PtrFileName, const char *PtrMode)
{

    SOSI_FILE_HANDLE fileHandle = NULL;
    U32 index = 0;
    UINT64 mode=0;
    BOOLEAN appendMode = FALSE;
    EFI_STATUS status = RET_VAL_FAILURE;
    CHAR16 uniFileName[256];

    for (index = 0; index < AsciiStrLen (PtrMode); index++)
    {
        switch (PtrMode[index])
        {
            case 'w':
            case 'W':
                    mode |= (EFI_FILE_MODE_WRITE  |
                             EFI_FILE_MODE_CREATE |
                             EFI_FILE_MODE_READ);
                        break;
            case 'r':
            case 'R':
                    mode |=  EFI_FILE_MODE_READ;
                        break;
            case 'a':
            case 'A':
                    mode |= (EFI_FILE_MODE_WRITE  |
                             EFI_FILE_MODE_CREATE |
                             EFI_FILE_MODE_READ);
                        appendMode=TRUE;
                        break;
            case 'b':
            case 'B':
                    break;
            case '+':
                    mode |= (EFI_FILE_MODE_READ |
                             EFI_FILE_MODE_WRITE);
                    appendMode=TRUE;
                    break;
            default:
                    break;
        }

    }

    if (lefiStrCopyAtoU (uniFileName, PtrFileName, 256) != TRUE)
    {
        return (NULL);
    }

    status = lefiFileOpen (gUdkImageHandle, uniFileName, &fileHandle, mode, 0);

    if (status != EFI_SUCCESS)
    {
        /* Error openning the file */
        return (NULL);
    }

    if (fileHandle == NULL )
    {
        /* Error openning the file */
        return (NULL);
    }

    if (appendMode == TRUE)
    {
        status = fileHandle->SetPosition (fileHandle, 0xFFFFFFFFFFFFFFFF);    // Set to end of file

        if (EFI_ERROR (status))
        {
            fileHandle->Close (fileHandle);
            fileHandle = NULL;

            return (NULL);
        }

    }

    return (fileHandle);

}

/**
 *
 * @method  lefiiFileLength(U32 FileHandle, U32 *PtrSize)
 *
 * @param    FileHandle
 *
 * @param    PtrSize length of a file
 *
 * @return    STATUS indicating success or fail
 *
 * @brief     function to get the length of the file contents
 *
 */

SCRUTINY_STATUS lefiiFileLength (SOSI_FILE_HANDLE FileHandle, U32 *PtrSize)
{

    UINT64 current,size;
    EFI_STATUS status;
    *PtrSize = 0;

    status = FileHandle->GetPosition (FileHandle, &current);

    if (EFI_ERROR (status))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    status = FileHandle->SetPosition (FileHandle, 0xFFFFFFFFFFFFFFFF);

    if (EFI_ERROR (status))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    status = FileHandle->GetPosition (FileHandle, &size);

    if (EFI_ERROR (status))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    status = FileHandle->SetPosition (FileHandle, current);

    if (EFI_ERROR (status))
    {
        return SCRUTINY_STATUS_FAILED;
    }

    *PtrSize = (U32) size;

    return (SCRUTINY_STATUS_SUCCESS);
}

/**
 *
 * @method  lefiiFileClose(U32 FileHandle)
 *
 * @param    FileHandle handle to which a file has to be opened
 *
 * @brief     function to read the contents of the file
 *
 */

VOID lefiiFileClose (SOSI_FILE_HANDLE FileHandle)
{
    FileHandle->Close (FileHandle);
}

/**
 *
 * @method  lefiiFileRead(U8 *PtrBuffer, U32 *Length, U32 FileHandle)
 *
 * @param    FileHandle handle to which a file has to be opened
 *
 * @param    PtrLength length of a file
 *
 * @return    STATUS indicating success or fail
 *
 * @brief     function to read the contents of the file
 *
 */

SCRUTINY_STATUS lefiiFileRead (SOSI_FILE_HANDLE FileHandle, U8 *PtrBuffer, U32 ReadLength)
{

    UINTN bytesRead;

    if (FileHandle == NULL )
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    bytesRead = ReadLength;

    FileHandle->Read (FileHandle, &bytesRead, PtrBuffer);

    //Commented this section as it is a bug for fread. The current position of read call should not be set to beginning.
    //FileHandle->SetPosition (FileHandle, 0);

    if (bytesRead != ReadLength )
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  lefiiFileWrite(U8 *PtrBuffer, U32 *Length, U32 FileHandle)
 *
 * @param    FileHandle handle to which a file has to be opened
 *
 * @param    PtrLength length of a file
 *
 * @return    STATUS indicating success or fail
 *
 * @brief     function to read the contents of the file
 *
*/

SCRUTINY_STATUS lefiiFileWrite (SOSI_FILE_HANDLE FileHandle, const U8 *PtrBuffer, U32 Length)
{

    UINTN bytesWritten = 0;
    U8* ptrBuffer = NULL;

    if (FileHandle == NULL )
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    bytesWritten = Length;

    ptrBuffer = sosiMemAlloc (Length);

    if (!ptrBuffer)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    sosiMemCopy (ptrBuffer, PtrBuffer, Length);

    FileHandle->Write (FileHandle, &bytesWritten, ptrBuffer);

    sosiMemFree(ptrBuffer);

    if (bytesWritten != Length )
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    return (SCRUTINY_STATUS_SUCCESS);

}


/* Microsecond delay timer */
/* Current resolution == 10000 us */
void lefiDelay (U32 uSec)
{
    gBS->Stall ((UINTN) uSec);
}


UINT32 lefiiIsFormatSpecifier (CHAR8 Temp)
{

    CHAR8 *ptrspecifiers = (CHAR8*) "acdeEfgGnopisuxXl%";

    UINT32 i = 0;

    for (i=0; ptrspecifiers[i]; i++)

        if (ptrspecifiers[i] == Temp)
            return 1;

    return 0;

}


CHAR8* lefiiFixStringTypeSpecifier (CHAR8* Ptrstr)
{

    CHAR8 *ptrTemp1 = Ptrstr;

    while (*ptrTemp1)
    {
        if (*ptrTemp1 == '%')
        {
            ptrTemp1++ ;

            while (*ptrTemp1 && !lefiiIsFormatSpecifier (*ptrTemp1))
            {
                ptrTemp1++;
            }

            if (!*ptrTemp1)
            {
                return (Ptrstr);
            }

            if (*ptrTemp1 == 's')
            {
                *ptrTemp1 = 'a';
            }

            if (*ptrTemp1 == 'u')
            {
                *ptrTemp1 = 'd';
            }

        }

        ptrTemp1++;

    }

    return (Ptrstr);

}



/**
 *
 * @method  lefiAsciiStrCpy (IN CHAR8 *PtrDestination, IN CHAR8 *PtrSource)
 *
 * @param   PtrDestination is the pointer to the destination array where the content is to be copied.
 *
 * @param   PtrSource is the string to be copied.
 *
 * @return  Returns a pointer to the destination string PtrDestination.
 *
 * @brief   Copies the string pointed to, by PtrSource to PtrDestination.
 *
*/

CHAR8 *lefiAsciiStrCpy (IN CHAR8 *PtrDestination, IN CHAR8 *PtrSource)
{
    CHAR8 *Ret = PtrDestination;

    while (*PtrSource)
        *(PtrDestination++) = *(PtrSource++);
    *PtrDestination = 0;

    return Ret;
}


/**
 *
 * @method  lefiAsciiStrCpy (IN CHAR8 *PtrDestination, IN CHAR8 *PtrSource)
 *
 * @param   PtrDestination is the pointer to the destination array where the content is to be copied.
 *
 * @param   PtrSource is the string to be copied.
 *
 * @param   Length to be copied from destination string.
 *
 * @return  Void.
 *
 * @brief   Copies the string pointed to, by PtrSource to PtrDestination.
 *
*/

VOID lefiAsciiStrNCpy (OUT CHAR8 *PtrDst, IN  CHAR8  *PtrSrc, IN  UINTN  Length)
{
    UINTN Index;
    UINTN SrcLen;

    SrcLen = sosiStringLength (PtrSrc);

    Index = 0;
    while (Index < Length && Index < SrcLen)
    {
        PtrDst[Index] = PtrSrc[Index];
        Index++;
    }
    for (Index = SrcLen; Index < Length; Index++)
    {
        PtrDst[Index] = 0;
    }
}


/**
 *
 * @method  lefiStrToUInteger ( IN  CONST CHAR16  *PtrStr,  OUT EFI_STATUS  *Status  )
 *
 * @param   PtrStr is pointer to the string.
 *
 * @param   Status is operation status.
 *
 * @return  The integer value of converted PstStr.
 *
 * @brief   This function converts string to integer.
 *
*/

UINT64 lefiStrToUInteger ( IN  CONST CHAR16  *PtrStr,  OUT EFI_STATUS  *Status  )
{
  UINT64    Value;
  UINT64    NewValue;
  CHAR16    *StrTail;
  CHAR16    Char;
  UINTN     Base;
  UINTN     Len;

  Base    = 10;
  Value   = 0;
  *Status = EFI_ABORTED;


  // Skip leading white space.
  while ((*PtrStr != 0) && (*PtrStr == ' '))
  {
    PtrStr++;
  }

  // For NULL PtrStr, just return.
  if (*PtrStr == 0)
  {
    return 0;
  }

  // Skip white space in tail.
  Len     = StrLen (PtrStr);
  StrTail = (CHAR16 *) (PtrStr + Len - 1);
  while (*StrTail == ' ')
  {
    *StrTail = 0;
    StrTail--;
  }

  Len = StrTail - PtrStr + 1;

  // Check hex prefix '0x'.
  if ((Len >= 2) && (*PtrStr == '0') && ((*(PtrStr + 1) == 'x') || (*(PtrStr + 1) == 'X')))
  {
    PtrStr += 2;
    Len -= 2;
    Base = 16;
  }

  if (Len == 0)
  {
    return 0;
  }

  // Convert the string to value.
  for (; PtrStr <= StrTail; PtrStr++)
  {

    Char = *PtrStr;

    if (Base == 16)
    {
      if (RShiftU64 (Value, 60) != 0)
      {
        // Overflow here x16.
        return 0;
      }

      NewValue = LShiftU64 (Value, 4);
    }
    else
    {
      if (RShiftU64 (Value, 61) != 0)
      {
        // Overflow here x8.
        return 0;
      }

      NewValue  = LShiftU64 (Value, 3);
      Value     = LShiftU64 (Value, 1);
      NewValue += Value;
      if (NewValue < Value)
      {
        // Overflow here.
        return 0;
      }
    }

    Value = NewValue;

    if ((Base == 16) && (Char >= 'a') && (Char <= 'f'))
    {
      Char = (CHAR16) (Char - 'a' + 'A');
    }

    if ((Base == 16) && (Char >= 'A') && (Char <= 'F'))
    {
      Value += (Char - 'A') + 10;
    }
    else if ((Char >= '0') && (Char <= '9'))
    {
      Value += (Char - '0');
    }
    else
    {
      // Unexpected Char encountered.
      return 0;
    }
  }

  *Status = EFI_SUCCESS;
  return Value;
}


/**
 *
 * @method  lefiFileSeek (SOSI_FILE_HANDLE FileHandle, long int Offset, U32 Whence)
 *
 * @param    FileHandle is the pointer to a FILE object that identifies the stream where characters are read from.
 *
 * @param    Offset is the number of bytes to offset from whence.
 *
 * @param    Whence is the position from where offset is added.
 *
 * @return   Returns zero if successful, or else it returns a non-zero value.
 *
 * @brief    Function sets the file position of the stream to the given offset.
 *
 */

U32 lefiFileSeek (SOSI_FILE_HANDLE FileHandle, long int Offset, U32 Whence)
{
    EFI_STATUS  status;
    status = FileHandle->SetPosition (FileHandle, Whence + Offset);

    if ( EFI_SUCCESS != status )
    {
        return (U32)(-1);
    }

    return 0;
}


/**
 *
 * @method   lefiFileTell (SOSI_FILE_HANDLE FileHandle)
 *
 * @param    FileHandle is the pointer to a FILE object that identifies the stream where characters are read from.
 *
 * @return   Returns the current value of the position indicator.
 *
 * @brief    Function returns the current file position of the given stream.
 *
 */

long lefiFileTell (SOSI_FILE_HANDLE FileHandle)
{
    EFI_STATUS  status;
    UINT64      curPos;


    status = FileHandle->GetPosition( FileHandle, &curPos );

    if ( EFI_SUCCESS != status )
    {
        return ( (long)-1 );
    }

    return ( (long)curPos );
}


/**
 *
 * @method  lefiiFileGets (char* PtrBuffer, U32 ReadLength,  SOSI_FILE_HANDLE FileHandle)
 *
 * @param    PtrBuffer is the pointer to an array of chars where the string read is stored.
 *
 * @param    ReadLength is the maximum number of characters to be read.
 *
 * @param    FileHandle is the pointer to a FILE object that identifies the stream where characters are read from.
 *
 * @return   Returns the character buffer on success. If an error occurs, a null pointer is returned.
 *
 * @brief    Function to reads a line from the specified stream and stores it into the string pointed to by PtrBuffer.
 *
 */


char* lefiiFileGets (char* PtrBuffer, U32 ReadLength,  SOSI_FILE_HANDLE FileHandle)
{

    U32 count = 0, done = 0;
    U8 letter = 0;

    if (FileHandle == NULL )
    {
        return NULL;
    }


    if( PtrBuffer == 0 || ReadLength <= 0 || FileHandle == 0 )
    {
        return NULL;
    }
    for ( count = 0; !done && count < ReadLength - 1; count++)
    {
        //to read the file
        if ( lefiiFileRead (FileHandle, &letter, 1 ) != SCRUTINY_STATUS_SUCCESS)
        {
            done = 1;
            count--;
        }
        else
        {
            PtrBuffer[count] = letter;
            if (letter == '\n' || letter == '\r')
            {
                done = 1;
                if( count != 0 )
                {
                    count--;
                }
            }
        }
    }
    PtrBuffer[count] = '\0';

    if ( count == 0 )
    {
        return NULL;
    }
    else
    {
        return PtrBuffer;
    }
}



/**
 *
 * @method  lefiToUpper(S8 Character)
 *
 * @param   Character is the letter to be converted to uppercase.
 *
 * @return  uppercase equivalent to Character.
 *
 * @brief   This function converts lowercase letter to uppercase.
 *
*/

char lefiCharacterToUpper (char Character)
{
    if ('a' <= Character && Character <= 'z')
    {
        //return (CHAR16) (Character - 0x20);
        return (Character - 0x20);
    }
    else
    {
        return Character;
    }
}


/**
 *
 * @method  lefiStrToL (const char *PtrStr, char **PtrEnd, U32 Base)
 *
 * @param   PtrStr - Source string of integral number.
 *
 * @param   PtrEnd - Reference to an object of type char*, whose value is set by the function to
 *                   the next character in PtrStr after the numerical value.
 *
 * @param   Base -   Numerical base that determines the valid characters and
 *                   their interpretation.
 *
 * @return  Returns the converted integral number as a long int value, else zero value is returned.
 *
 * @brief   This function converts the initial part of the string in PtrStr to a long int value according to the given Base.
 *
*/


UINT64 lefiStrToL (const char *PtrStr, char **PtrEnd, U32 Base)
{
    EFI_STATUS   Status;
    UINT64       Value64;
    CHAR16  *ValueStr;

    ValueStr = (CHAR16*)AllocateZeroPool ( CHAR_SIZE );

    if (lefiStrCopyAtoU (ValueStr, (CHAR8 *)PtrStr, CHAR_SIZE ) != TRUE)
    {
    }

    Value64 = lefiStrToUInteger( ValueStr,  &Status ) ; //here we have to use strtouinteger from EFI


    if ( EFI_SUCCESS != Status )
    {
        return (UINT64)(-1);
    }
    if ( Value64 > LONG_MAX -1)
    {
        return LONG_MAX;
    }
    return Value64;
}


/**
 *
 * @method  lefiAtoi(const char* PtrStr )
 *
 * @param   PtrStr - Source string of integral number.
 *
 * @return  U32
 *
 * @brief   This function implement atoi.
 *
*/

U32 lefiAtoi (const char* PtrStr )
{
    int i, sign = 1;
    char c;

    //
    // skip preceeding white space
    //
    while (*PtrStr && (*PtrStr == ' ' || *PtrStr == '\t') )
    {
        PtrStr++;
    }
    if ( *PtrStr == '-' )
    {
        sign = -1;
        PtrStr++;
    }
    else if ( *PtrStr == '+' )
    {
        sign = 1;
        PtrStr++;
    }

    i = 0;
    c = *PtrStr;
    while (c)
    {
        if (c >= '0' && c <= '9')
            i = (i * 10) + c - '0';
        else
            break;
        PtrStr++;
        c = *PtrStr;
    }

    return i * sign;
}


/**
 *
 * @method  lefiAtol ( const char* PtrStr )
 *
 * @param   PtrStr - Source string of integral number.
 *
 * @return  long
 *
 * @brief   This function implement atol.
 *
*/

long lefiAtol  ( const char* PtrStr )
{
    return (long)lefiAtoi(PtrStr);
}


/**
 *
 * @method  lefiStrNCpy (char *PtrDest, const char *PtrSrc, U32 Len)
 *
 * @param   PtrDest is the pointer to the destination array where the content is to be copied.
 *
 * @param   PtrSrc is the string to be copied.
 *
 * @param   Len is Length to be copied from destination string.
 *
 * @return  Returns the destination string.
 *
 * @brief   Copies the string pointed to, by PtrSrc to PtrDest.
 *
*/

char* lefiStrNCpy (char *PtrDest, const char *PtrSrc, U32 Len)
{
    lefiAsciiStrNCpy ( (CHAR8*)PtrDest, (CHAR8*)PtrSrc, (UINTN)Len);
    return PtrDest;
}



/**
 *
 * @method  lefiStrCpy (char *PtrDest, const char *PtrSrc)
 *
 * @param   PtrDest is the pointer to the destination array where the content is to be copied.
 *
 * @param   PtrSrc is the string to be copied.
 *
 * @return  Returns a pointer to the destination string PtrDest.
 *
 * @brief   Copies the string pointed to, by PtrSrc to PtrDest.
 *
*/

char* lefiStrCpy (char *PtrDest, const char *PtrSrc)
{
    return (CHAR8*)lefiAsciiStrCpy ( (CHAR8*)PtrDest, (CHAR8*)PtrSrc );
}


/**
 *
 * @method  lefiStrCat (char *PtrDest, const char *PtrSrc)
 *
 * @param   PtrDest is the pointer to the destination array where the content is to be copied.
 *
 * @param   PtrSrc is the string to be copied.
 *
 * @return  Returns the final copy of the copied string.
 *
 * @brief   Copies string pointed to, by PtrSrc to PtrDest.
 *
*/

char* lefiStrCat (char *PtrDest, const char *PtrSrc)
{
    return (CHAR8*)lefiAsciiStrCpy ( (CHAR8*)PtrDest + sosiStringLength((CHAR8*)PtrDest ), (CHAR8*)PtrSrc);
}


/**
 *
 * @method  lefiStrNCat (char *PtrDest, const char *PtrSrc, U32 Len)
 *
 * @param   PtrDest is the pointer to the destination array where the content is to be copied.
 *
 * @param   PtrSrc is the string to be copied.
 *
 * @param   Len is the number of characters to be copied from source.
 *
 * @return  Returns the final copy of the copied string.
 *
 * @brief   Copies upto Len character from string pointed to, by PtrSrc to PtrDest.
 *
*/

char* lefiStrNCat (char *PtrDest, const char *PtrSrc, U32 Len)
{
    char cpbuff[50];
    char* cpstr=&cpbuff[0];

    Len = Len > 49 ? 49:Len;
    sosiMemCopy(cpstr,(VOID *) PtrSrc, (U32)Len);
    cpbuff[Len]='\0';
    return (CHAR8*)lefiAsciiStrCpy ( ((CHAR8*)PtrDest + sosiStringLength ((CHAR8*)PtrDest)), (CHAR8*)cpstr);
}


/**
 *
 * @method  lefiAsciiHexToInt(S8 Character)
 *
 * @param   Character as input
 *
 * @return  Returns the hex equivalent of Character.
 *
 * @brief   Character to hexadecimal number.
 *
*/

S32 lefiAsciiHexToInt(S8 Character)
{
    switch(Character)
    {
        case 'a':
        case 'A':
                return 10;
        case 'b':
        case 'B':
                return 11;
        case 'c':
        case 'C':
                return 12;
        case 'd':
        case 'D':
                return 13;
        case 'e':
        case 'E':
                return 14;
        case 'f':
        case 'F':
                return 15;
        default:
                return -1;
    }
}

/**
 *
 * @method  lefiIsDigit(const char Character)
 *
 * @param   Character is the character to be checked.
 *
 * @return  TRUE or FALSE
 *
 * @brief   This function returns non-zero value if c is a digit, else it returns 0.
 *
*/

BOOLEAN lefiIsDigit(const char Character)
{
    return IsDigit(Character);
}


/**
 *
 * @method  lefiScanByteHexStr(const char* HexChar)
 *
 * @param   HexChar input hex string.
 *
 * @return  Returns the hex equivalent of Character.
 *
 * @brief   Convert hex string to decimal value.
 *
*/

S32 lefiScanByteHexStr (const char* HexChar)
{
    int dVal = 0;
    char digit[2]={'0','\0'};

    if(sosiStringLength(HexChar) != 4)
    {
        return -1;
    }

    if(lefiIsDigit(HexChar[2]))
    {
        digit[0] = HexChar[2];

        dVal = sosiAtoi(&digit[0])*16;

    }
    else
    {
        dVal = lefiAsciiHexToInt(HexChar[2])*16;
    }

    if(lefiIsDigit(HexChar[3]))
    {
        digit[0] = HexChar[3];

        dVal += sosiAtoi(&digit[0]);
    }
    else
    {
        dVal += lefiAsciiHexToInt(HexChar[3]);
    }
    return dVal;
}


/**
 *
 * @method  lefiStrNCmp (const char *PtrStr1, const char *PtrStr2, U32 Num)
 *
 * @param   PtrStr1 is the first string to be compared.
 *
 * @param   PtrStr2 is the second string to be compared.
 *
 * @param   Len is the maximum number of characters to be compared.
 *
 * @return  Returns interger value.
 *
 * @brief   Compares at most the first Num bytes of PtrStr1 and PtrStr2.
 *
*/

S32 lefiStrNCmp (const char *PtrStr1, const char *PtrStr2, U32 Num)
{
    while ( Num > 0 && *PtrStr1 && *PtrStr2 && *PtrStr1 == *PtrStr2 ) PtrStr1++, PtrStr2++, Num--;

    return !Num ? 0 : (*PtrStr1 - *PtrStr2);
}


/**
 *
 * @method  lefiStrStr (const char *PtrStr, const char *PtrFindStr)
 *
 * @param   PtrStr the main string to be scanned.
 *
 * @param   PtrFindStr is the small string to be searched with-in PtrStr string.
 *
 * @return  Returns a pointer to the first occurrence in PtrStr of any of the entire sequence
 *          of characters specified in PtrFindStr, or a null pointer.
 *
 * @brief   Finds the first occurrence of the substring PtrFindStr in the string PtrStr.
 *
*/

const char *lefiStrStr (const char *PtrStr, const char *PtrFindStr)
{
    int x = 0;
    U32 l = sosiStringLength( PtrFindStr ) ;

    x = lefiStrNCmp ( PtrStr++, PtrFindStr, l ) ;

    while( *PtrStr && x )
    {
         x = lefiStrNCmp ( PtrStr++, PtrFindStr, l ) ;
    }
    //while ( *PtrStr && ( x = lefiStrNCmp ( PtrStr++, PtrFindStr, l ) ) ) ;

    return x==0 ? PtrStr-1 : 0;
}


/**
 *
 * @method  lefiRand()
 *
 * @param   None
 *
 * @return  Returns an integer value
 *
 * @brief   This function returns an random integer value.
 *
*/

UINT32 lefiRand (VOID)
{
    UINT32 b;
    UINT32 t1 = 0, t2 = 0;
    b = t1 ^ (t1 >> 2) ^ (t1 >> 6) ^ (t1 >> 7);
    t1 = (t1 >> 1) | (~b << 31);

    b = (t2 << 1) ^ (t2 << 2) ^ (t1 << 3) ^ (t2 << 4);
    t2 = (t2 << 1) | (~b >> 31);

    return t1 ^ t2;
}


/**
 *
 * @method  lefiStrChr (const char *PtrStr, S32 Character)
 *
 * @param   PtrStr the main string to be scanned.
 *
 * @param   Character is the character to be searched in str.
 *
 * @return  Returns a pointer to the first occurrence of the character c in the string PtrStr, or NULL if the character is not found.
 *
 * @brief   Searches for the first occurrence of the Character in the string pointed to by the argument PtrStr.
 *
*/

const char * lefiStrChr (const char *PtrStr, char Character)
{
    while ( *PtrStr && (int)*PtrStr != Character )
    {
        PtrStr++;
    }

    return *PtrStr ? PtrStr : (char*)NULL;
}


/**
 *
 * @method  lefiStrCmp (const char *PtrStr1, const char *PtrStr2)
 *
 * @param   PtrStr1 is the first string to be compared.
 *
 * @param   PtrStr2 is the second string to be compared.
 *
 * @return  Returns interger value.
 *
 * @brief   Compares PtrStr1 and PtrStr2.
 *
*/

S32 lefiStrCmp (const char *PtrStr1, const char *PtrStr2)
{
    while ( *PtrStr1 && *PtrStr2 && *PtrStr1 == *PtrStr2 ) PtrStr1++, PtrStr2++;

    return (S32) (*PtrStr1 - *PtrStr2);
}


/**
 *
 * @method  osiStrToK()
 *
 * @param   PtrStr contents are modified and broken into smaller strings (tokens).
 *
 * @param   Delimiter is the string containing the delimiters.
 *
 * @return  Returns a pointer to the last token found in the string.
 *
 * @brief   Breaks string PtrStr into a series of tokens using the Delimiter.
 *
*/

char* lefiStrToK (char *PtrStr, const char *Delimiter)
{
    static int xpectNull = 0;
    static char * start;
    char *token;

    if ( PtrStr )
        start = PtrStr;
    else if ( !xpectNull )
        return (char*)NULL;

    while ( lefiStrChr ( Delimiter, *start) ) start++;

    token = start;

    while ( *start && !lefiStrChr (Delimiter, *start ) ) start++;

    xpectNull = *start ? 1 : 0;

    *start++ = 0;

    return token;
}


/**
 *
 * @method  lefiIsPrint(const char Character)
 *
 * @param   Character is the character to be checked.
 *
 * @return  uppercase equivalent to Character.
 *
 * @brief   This function returns a non-zero value(true) if c is a printable character else, zero (false).
 *
*/

BOOLEAN lefiIsPrint (const char Character)
{
    return IsPrint (Character);
}



/**
 *
 * @method  lefiIsSpace(const char Character)
 *
 * @param    Character is is the character to be checked
 *
 * @return  TRUE or FALSE
 *
 * @brief    This function checks whether the passed character is white-space.
 *
 */

BOOLEAN lefiIsSpace(const char Character)
{
    return IsSpace(Character);
}


#endif



/**
 *
 * @method  sosiMemSet()
 *
 * @param   PtrMem      Memory to which the fill char has to be written
 *
 * @param   FillChar    Character to be filled into memory
 *
 * @param   Size        Size of the buffer
 *
 * @brief   Operating system independent Memory set method
 *
 */

VOID sosiMemSet (__OUT__ VOID *PtrMem, __IN__ U8 FillChar, __IN__ U32 Size)
{
    #ifdef OS_UEFI
        SetMem (PtrMem, Size, FillChar);
    #else
        memset (PtrMem, FillChar, Size);
    #endif
}

/**
 *
 * @method  sosiMemCopy()
 *
 * @param   PtrDest     Destination where the memory to be copied into
 *
 * @param   PtrSrc      Source memory from which the memory will be copied from.
 *
 * @param   Size        Size of the Source or number of bytes that needs to be
 *                      copied from the source.
 *
 * @brief   Operating system independent method to perform the memory copy
 *          operation.
 *
 */

VOID sosiMemCopy (__OUT__ VOID *PtrDest, __IN__ const VOID *PtrSrc, __IN__ U32 Size)
{

    #ifdef OS_UEFI
        CopyMem (PtrDest, PtrSrc, Size);
    #else
        memcpy (PtrDest, PtrSrc, Size);
    #endif

}

/**
 *
 * @method  sosiMemCompare()
 *
 * @param   PtrrFirst   Source pointer memory that needs to be compered.
 *
 * @param   PtrSecond   Destination pointer memory to be compared with.
 *
 * @param   Size        Size of the memory that needs to be compared.
 *
 * @return  Compares two memory pointers and returns STATUS_SUCCESS if the
 *          memory is identical otherwise returns STATUS_FAILED.
 *
 * @brief   Compares two memory pointers and returns SUCCESS or FAILED.
 *
 */

SCRUTINY_STATUS sosiMemCompare (__IN__ U8 *PtrFirst, __IN__ U8 *PtrSecond, __IN__ U32 Size)
{

    #ifdef OS_UEFI

        if (CompareMem (PtrFirst, PtrSecond, Size) )
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        return (SCRUTINY_STATUS_SUCCESS);

    #else

        if (memcmp (PtrFirst, PtrSecond, Size))
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        return (SCRUTINY_STATUS_SUCCESS);

    #endif

}

/**
 *
 * @method  sosiMemFree()
 *
 * @param   PtrMem      Memory to be freed
 *
 * @brief   Memory function that needs to be freed.
 *
 */

VOID sosiMemFree (__IN__ VOID *PtrMem)
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

/**
 *
 * @method  sosiMemAlloc()
 *
 * @param   Size    Number of bytes that needs to be allocated.
 *
 * @return  Returns the allocated memory in void pointer.
 *
 * @brief   Operating system independent memory allocation method.
 *
 */

VOID* sosiMemAlloc (__IN__ U32 Size)
{

    VOID *ptrMemory = NULL;

    #ifdef OS_UEFI

        ptrMemory = (VOID *) AllocateZeroPool (Size);

    #else

        ptrMemory = malloc (Size);

        if (ptrMemory != NULL)
        {
            sosiMemSet (ptrMemory, 0, Size);
        }

    #endif

    return (ptrMemory);

}

/**
 *
 * @method  sosiMemReAlloc()
 *
 * @param   PtrMem      Memory that has to be resized
 *
 * @param   Size        Size of the memory which is to be allocated
 *
 * @param   OldSize     Memory size that was originally allocated.
 *
 * @return  Returns the re-allocated Pointer memory or NULL will be returned.
 *
 * @brief   For the previously allocated memory re-allocate the complete memory.
 *
 */

VOID* sosiMemRealloc (__IN__ VOID *PtrMem, __IN__ U32 Size, __IN__ U32 OldSize)
{

    #ifdef OS_UEFI

        /*
         * Here we need to give old size of the buffer, so it callling function responsibility
         * to send the old size
         */

        return ((VOID *) ReallocatePool (OldSize, Size, PtrMem));

    #else

        PtrMem = realloc (PtrMem, Size);

        return (PtrMem);

    #endif

}

/**
 *
 * @method  sosiStringLength()
 *
 * @param   PtrStr      String pointer for which the length should be
 *                      calculated
 *
 * @return  Length of the string
 *
 * @brief   Function to get the string length
 *
 */

U32 sosiStringLength (__IN__ const char *PtrStr)
{
    #ifdef OS_UEFI
        return ((U32) (AsciiStrLen (PtrStr)));
    #else
        return ((U32) strlen (PtrStr));
    #endif
}


/**
 *
 * @method  sosiStringCompare()
 *
 * @param   PtrFirst        First String pointer to be compared
 *
 * @param   PtrSecond       Second String pointer to be compared
 *
 * @return  Status Indicating success or failure when there is a difference
 *
 * @brief   Function to compare two string
 *
 */

SCRUTINY_STATUS sosiStringCompare (__IN__ const char *PtrFirst, __IN__ const char *PtrSecond)
{
    #ifdef OS_UEFI

        if((AsciiStrCmp (PtrFirst,PtrSecond)) != 0)
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        return (SCRUTINY_STATUS_SUCCESS);

    #else

        if (strcmp (PtrFirst, PtrSecond))
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        return (SCRUTINY_STATUS_SUCCESS);

    #endif

}


/**
 *
 * @method  sosiStringCat()
 *
 * @param   PtrFirst        First String pointer to be compared
 *
 * @param   PtrSecond       Second String pointer to be compared
 *
 * @return  Status Indicating success or failure when there is a difference
 *
 * @brief   Appends a copy of the source string to the destination string
 *
 */

VOID sosiStringCat (__IN__ char *PtrFirst, __IN__ char *PtrSecond)
{
	char *ptrTemp = PtrFirst + (sosiStringLength (PtrFirst));

	while (*PtrSecond != '\0')
	{
		*ptrTemp++ = *PtrSecond++;
	}

	*ptrTemp = '\0';

}



/**
 *
 * @method  sosiStringTrim()
 *
 * @param   PtrString       String pointer which needs to be trimmed from
 *                          Whitespaces
 *
 * @brief   Trims the white spaces
 *
 */

VOID sosiStringTrim (__INOUT__ char *PtrString)
{

    char *Ptr1 = PtrString, *Ptr2 = PtrString;

    if (PtrString == NULL)
    {
        return;
    }

    for (; ((*Ptr1 != 0) && (sosiIsSpace (*Ptr1))); Ptr1++);

    if (Ptr2 == Ptr1)
    {
        while (*Ptr1 != 0)
        {
            Ptr2++;
            Ptr1++;
        }
    }

    else
    {
        while (*Ptr1 != 0)
        {
            *Ptr2 = *Ptr1;
            Ptr1++;
            Ptr2++;
        }

        *Ptr2 = 0;

    }

    for (Ptr1 = (PtrString + sosiStringLength (PtrString) - 1); ((Ptr1 >= PtrString) && (sosiIsSpace (*Ptr1))); Ptr1--);

    if  (Ptr1 !=  (PtrString + sosiStringLength (PtrString) - 1))
    {
        *(Ptr1 + 1) = 0;
    }

}

/**
 *
 * @method  osiIsSpace()
 *
 * @param   Character       Character to be checked for whitespace
 *
 * @return  Returns TRUE if the space is white, otherwise FALSE
 *
 * @brief   This function checks whether the passed character is white-space.
 *
*/

BOOLEAN sosiIsSpace (__IN__ const char Character)
{

    #ifdef OS_UEFI
        return ((BOOLEAN) IsSpace (Character));
    #else
        return ((BOOLEAN) (isspace (Character) != 0));
    #endif

}

/**
 *
 * @method  sosiIsPrint()
 *
 * @param   Character is the character to be checked.
 *
 * @return  uppercase equivalent to Character.
 *
 * @brief   This function returns a non-zero value(true) if c is a printable character else, zero (false).
 *
*/

U32 sosiIsPrint (const char Character)
{
    #ifdef OS_UEFI
        return (lefiIsPrint (Character));
    #else
        return (isprint (Character));
    #endif
}

/**
 *
 * @method  osiStringCopy()
 *
 * @param   PtrDestination      Destination string pointer where the string will
 *                              be copied into.
 *
 * @return  PtrSource           Source string pointer from which the string will
 *                              be copied from.
 *
 * @brief   String will be copied from source to destination and length of the
 *          string that will be copied is calculated from PtrSource.
 *
 */

VOID sosiStringCopy (__OUT__ char *PtrDestination, __IN__ const char *PtrSource)
{
    #if defined(OS_UEFI)
        lefiStrCpy( PtrDestination, PtrSource);
    #else
        strcpy (PtrDestination, PtrSource);
    #endif
}

/**
 *
 * @method  sosiStringNCopy()
 *
 * @param   PtrDestination  Destination where string needs to be copied into
 *
 * @param   PtrSource       Source where String needs to be copied into
 *
 * @param   Length          Number of chars that needs to be copied into
 *
 * @brief   String will be copied from source to destination for the given
 *          length of characters.
 *
 */

VOID sosiStringNCopy (__OUT__ char *PtrDestination, __IN__ const char* PtrSource, __IN__ U32 Length)
{

    #if defined(OS_UEFI)
        lefiStrNCpy (PtrDestination, PtrSource, Length);
    #else
        strncpy (PtrDestination, PtrSource, Length);
    #endif

}

/**
 *
 *  @method     sosiSleep()
 *
 *  @param      Milli       Milli-seconds to sleep
 *
 *  @return     VOID        No return value.
 *
 *  @brief      This method is for init sleep
 *
 */

VOID sosiSleep (U32 Milli)
{

    #if defined(OS_LINUX) || defined(OS_FREEBSD) || defined(OS_VMWARE) || defined (OS_BMC)
    usleep (Milli * 1000);
    #elif OS_WINDOWS
    Sleep (Milli);
    #elif defined(OS_UEFI)
    lefiDelay (Milli * 1000);
    #endif

}

/**
 *
 *
 * @method  sosiFprintf()
 *
 * @param   PtrArguments    Variable arguments to print
 *
 * @return  count            the total number of characters written is returned
 *
 * @brief   Method can be used to write outputs to the file
 *
 */

S32 sosiFprintf (__IN__ SOSI_FILE_HANDLE Stream, __IN__ const char* PtrArguments, ...)
{
    va_list vaargs;

    S32  count = 0;

    #ifdef OS_UEFI

        return (-1);

    #elif defined (OS_WINDOWS)
        va_start (vaargs, PtrArguments);
 
        count = vfprintf_s (Stream, PtrArguments, vaargs);
    #else
        va_start (vaargs, PtrArguments);
    
        count = vfprintf (Stream, PtrArguments, vaargs);
    #endif

    va_end (vaargs);

    return (count);

}

/**
 *
 *
 * @method  sosiSprintf()
 *
 * @param   PtrArguments    Variable arguments to print
 *
 * @return  count            the total number of characters written is returned
 *
 * @brief   Method can be used for printing the debug messages.
 *
 */

U32 sosiSprintf (__OUT__ char *PtrString, __IN__ U32 StringSize, __IN__ const char* PtrArguments, ...)
{
    va_list vaargs;

    U32  count = 0;

    #ifdef OS_UEFI

        CHAR8 *ptrTemp = (CHAR8*) PtrArguments;
        va_start (vaargs, PtrArguments);

        ptrTemp = lefiiFixStringTypeSpecifier (ptrTemp);

        count = (U32) AsciiVSPrint (PtrString, StringSize, (const CHAR8 *) PtrArguments, vaargs);

    #else
        va_start (vaargs, PtrArguments);

        count = vsprintf (PtrString, PtrArguments, vaargs);

    #endif

    va_end (vaargs);

    return ((U32) count);

}

/**
 *
 * @method  osiStringCompareIgnoreCase()
 *
 * @param   PtrFirst    String one that needs to be compared
 *
 * @param   PtrSecond   String that needs to be compared with
 *
 * @return  Non-zero value, if the string fails to match
 *          zero value, if the string matches
 *
 * @brief   Compares two string with case insenstive.
 *
 */

SCRUTINY_STATUS sosiStringCompareIgnoreCase (__IN__ const char *PtrFirst, __IN__ const char *PtrSecond)
{

    U32 temp = 0, index = 0;
    char a, b;

    temp = sosiStringLength ((const char*) PtrFirst);

    if (PtrFirst == NULL || PtrSecond == NULL)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (sosiStringLength ((const char*) PtrFirst) != sosiStringLength (PtrSecond))
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    for (index = 0; index < temp; index++)
    {
        a = PtrFirst[index];
        b = PtrSecond[index];

        /* Convert all cases into lower and compare the string */
        if (PtrFirst[index] >= 'a' && PtrFirst[index] <= 'z')
            a = PtrFirst[index] - 32;

        if (PtrSecond[index] >= 'a' && PtrSecond[index] <= 'z')
            b = PtrSecond[index] - 32;

        if (a != b) { return (SCRUTINY_STATUS_FAILED); }

    }

    /* We have full matching string */
    return (SCRUTINY_STATUS_SUCCESS);

}

/**
 *
 * @method  sosiFileWrite()
 *
 * @param   FileHandle  File handle to which the buffer has to be written.
 *
 * @param   PtrBuffer   Buffer that has to be written into the file.
 *
 * @param   Size        Size of the buffer which needs to be written into the
 *                      file.
 *
 * @return  STATUS_SUCCESS if the file has been written to a buffer successfully
 *          or STATUS_FAILED will be returned.
 *
 * @brief   Function to write the contents from buffer to the file
 *
 */

SCRUTINY_STATUS sosiFileWrite (__IN__ SOSI_FILE_HANDLE FileHandle, __IN__ const U8 *PtrBuffer, __IN__ U32 Size)
{
    #ifdef OS_UEFI

        return (lefiiFileWrite (FileHandle, PtrBuffer, Size));

    #else

        fwrite (PtrBuffer, 1, Size, FileHandle);

        return (SCRUTINY_STATUS_SUCCESS);

    #endif


}



/**
 *
 * @method  osiFileBufferWrite()
 *
 * @param   PtrFileName     File name in string which will be used opening the
 *                          file.
 *
 * @param   PtrBuffer       Binary buffer that has to be written to the file
 *
 * @param   Length          Length of the given buffer
 *
 * @return  If the file has been written successfully STATUS_SUCCESS will be
 *          returned otherwise STATUS_FAILED will be returned.
 *
 * @brief   Will write the given binary buffer into the file name specified.
 *
 */

SCRUTINY_STATUS sosiFileBufferWrite (__IN__ const char *PtrFileName, __IN__ U8 *PtrBuffer, __IN__ U32 Length)
{

    SOSI_FILE_HANDLE fileHandle;

    fileHandle = sosiFileOpen (PtrFileName, "wb+");

    if (!fileHandle)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    if (sosiFileWrite (fileHandle, PtrBuffer, Length) != SCRUTINY_STATUS_SUCCESS)
    {
        sosiFileClose (fileHandle);
        return (SCRUTINY_STATUS_FAILED);
    }

    sosiFileClose (fileHandle);

    return (SCRUTINY_STATUS_SUCCESS);
}

/**
 *
 * @method  sosiFileLength()
 *
 * @param   FileHandle      File handle for which the length in bytes to be
 *                          calculated.
 *
 * @param   PtrSize         Length of the file returned in bytes.
 *
 * @return  STATUS_SUCCESS upon successfully retrieving the file size in bytes.
 *
 * @brief   Calculates the length of the given file.
 *
 */

SCRUTINY_STATUS sosiFileLength (__IN__ SOSI_FILE_HANDLE FileHandle, __OUT__ U32 *PtrSize)
{

    #ifdef OS_UEFI

    return (lefiiFileLength (FileHandle, PtrSize));

    #else

    U32 current = ftell(FileHandle);

    fseek (FileHandle, 0, SEEK_END);

    *PtrSize = ftell (FileHandle);

    rewind (FileHandle);

    fseek (FileHandle, current, SEEK_SET);

    return (SCRUTINY_STATUS_SUCCESS);

    #endif

}


/**
 *
 * @method  osiFileRead()
 *
 * @param   FileHandle  File handle from which the file has to be read.
 *
 * @param   PtrBuffer   Buffer where the read file contents to be stored.
 *
 * @param   Length      Length of the bytes that needs to be read from given
 *                      file
 *
 * @return  STATUS_SUCCESS if the file has been read to a buffer successfully or
 *          STATUS_FAILED will be returned.
 *
 * @brief   Reads the content of the file from the given Handle.
 *
 */

SCRUTINY_STATUS sosiFileRead (__IN__ SOSI_FILE_HANDLE FileHandle, __OUT__ U8 *PtrBuffer, __IN__ U32 Length)
{

    #ifndef OS_UEFI
    U32 bytesRead;
    #endif

    if (FileHandle == NULL)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    #ifdef OS_UEFI

        return (lefiiFileRead (FileHandle, PtrBuffer, Length));

    #else

        bytesRead = (U32) fread (PtrBuffer, 1, Length, FileHandle);

        if (bytesRead != Length)
        {
            return (SCRUTINY_STATUS_FAILED);
        }

        return (SCRUTINY_STATUS_SUCCESS);

    #endif

}

/**
 *
 * @method  sosiFileBufferRead()
 *
 *
 * @param   PtrFileName     File name in string which will be used opening the
 *                          file.
 *
 * @param   PtrBuffer       Binary buffer that will be filled from the file
 *
 * @param   PtrLength       Length of the file or buffer in bytes.
 *
 * @return  If the file has been read successfully STATUS_SUCCESS will be
 *          returned otherwise STATUS_FAILED will be returned.
 *
 * @brief   Reads the contents from file to the method allocate buffer. It is
 *          user's responsibitlity to free the buffer which is allocated upon
 *          successful read.
 *
 */

SCRUTINY_STATUS sosiFileBufferRead (__IN__ const char *PtrFileName, __OUT__ U8 **PtrBuffer, __OUT__ U32 *PtrLength)
{

    SOSI_FILE_HANDLE fileHandle = NULL;

    fileHandle = sosiFileOpen (PtrFileName, "rb");

    if (!fileHandle)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    /* Get the length of the file and check if we have enough buffer to read & allocate */

    if (sosiFileLength (fileHandle, PtrLength) != SCRUTINY_STATUS_SUCCESS)
    {
        sosiFileClose (fileHandle);
        return (SCRUTINY_STATUS_FAILED);
    }

    if (*PtrLength < 1)
    {
        sosiFileClose (fileHandle);
        return (SCRUTINY_STATUS_FAILED);
    }

    *PtrBuffer = (U8*) sosiMemAlloc (*PtrLength);

    sosiMemSet (*PtrBuffer , 0 , *PtrLength);

    /* Read the buffer from the opened handle and allocated memory. */

    if (sosiFileRead (fileHandle, *PtrBuffer, *PtrLength) != SCRUTINY_STATUS_SUCCESS)
    {
        sosiMemFree (*PtrBuffer);
        sosiFileClose (fileHandle);

        return (SCRUTINY_STATUS_FAILED);
    }

    sosiFileClose (fileHandle);

    return (SCRUTINY_STATUS_SUCCESS);


}

/**
 *
 * @method  sosiFileOpen()
 *
 * @param   PtrFileName     File name that needs to be opened
 *
 * @param   PtrMode         In which mode the file has to be opened
 *
 * @return  File handle upon successful opening of a given file
 *
 * @brief   Method will open the file irrespective of the operating system
 *
 */

SOSI_FILE_HANDLE sosiFileOpen (__IN__ const char *PtrFileName, __IN__ const char *PtrMode)
{

    SOSI_FILE_HANDLE ptrHandle;

    #ifdef OS_UEFI
        ptrHandle = lefiiFileOpen (PtrFileName, PtrMode);
    #else
        ptrHandle = fopen (PtrFileName, PtrMode);
    #endif

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

void sosiFileClose (__IN__ SOSI_FILE_HANDLE FileHandle)
{

    #ifdef OS_UEFI
        lefiiFileClose (FileHandle);
    #else
        fclose (FileHandle);
    #endif

}

/**
 *
 * @method  sosIsNumericChar()
 *
 * @param   NumChar     Character that needs to be checked
 *
 * @return  Returns TRUE if the given character is numeric value (0 to 9)
 *
 * @brief   Checks the specified character is a numeric value.
 *
 */

BOOLEAN sosIsNumericChar (__IN__ U8 NumChar)
{
    return (NumChar >= '0' && NumChar <= '9') ? TRUE: FALSE;
}


/**
 *
 * @method  sosiAtoi()
 *
 * @param   PtrStr      String pointer for which the integer to be returned
 *
 * @return  Returns the converted integer value from the string given
 *
 * @brief   atoi() function implementation. Any invalid character, then this
 *          function returns 0
 *
 */

U32 sosiAtoi (__IN__ const char *PtrStr)
{

    int res = 0, i = 0;

    if ((PtrStr == NULL)) //|| (*PtrStr == NULL))
    {
        return (0);
    }

    while (PtrStr[i] == ' ')
    {
        i++;
    }

    for ( ; PtrStr[i] != '\0'; ++i)
    {

        if (sosIsNumericChar (PtrStr[i]) == FALSE)
        {
            return (0);
        }

        res = res * 10 + PtrStr[i] - '0';

    }

    return (res);

}


/**
 *
 * @method  sosiMkDir()
 *
 * @param   PtrFolderName      pointer to the folder name
 *
 * @return  return failure if the folder is not created or cannot accessed
 *
 * @brief   create or check whether a folder is able to be created or access
 *
 */

SCRUTINY_STATUS sosiMkDir (__IN__ const char *PtrFolderName)
{

#if defined (OS_UEFI)

    // Need to check how to do it under uEFI
    return (SCRUTINY_STATUS_UNSUPPORTED);
    
#elif defined (OS_LINUX) || defined (OS_VMWARE)
    
    U32    check;
        
    check = mkdir (PtrFolderName, 0777);

    //Return the error if we are unable to create the folder , not able to access the folder etc...
                                    
    if (check > 0 && errno == EACCES && errno == EIO )
    {
        return (SCRUTINY_STATUS_FOLDER_CREATION_FAILED);
    }

    return (SCRUTINY_STATUS_SUCCESS);


#else 
    U32         check;

    check = _mkdir (PtrFolderName);

    if (check == 0 || errno == EEXIST)
    {
        return (SCRUTINY_STATUS_SUCCESS);
    }
    
    return (SCRUTINY_STATUS_FOLDER_CREATION_FAILED);

#endif

    

}

void sosiPrintTimestamp (const char* PtrPrefix)
{

    #ifdef OS_WINDOWS

    SYSTEMTIME st;

    GetLocalTime (&st);

    printf ("%s >> %02d/%02d/%04d %02d:%02d:%02d.%03d\n", PtrPrefix,
                                  st.wMonth, st.wDay, st.wYear,
                                  st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    #elif defined (OS_UEFI)

    AsciiPrint ("%a %a\n", __DATE__, __TIME__);

    #else

    char buffer[2048];
    time_t timer;
    struct tm* tm_info;

    time (&timer);
    tm_info = localtime (&timer);

    sosiMemSet (buffer, '\0', sizeof (buffer));
    strftime (buffer, 64, "%m/%d/%Y %H:%M:%S", tm_info);

    printf ("%s >> %s\n", PtrPrefix, buffer);

    #endif

}

SCRUTINY_STATUS sosiGetSystemTime (char *PtrBuffer)
{

#if	defined (OS_WINDOWS)

    SYSTEMTIME          st;
    GetLocalTime        (&st);

    sosiMemSet (PtrBuffer, 0, sizeof (PtrBuffer));

    sosiSprintf (PtrBuffer, sizeof (PtrBuffer), "%02d-%02d-%02d_%02d-%02d-%02d", st.wMonth, st.wDay, st.wYear,
                st.wHour, st.wMinute, st.wSecond );

#elif defined (OS_LINUX)


    time_t now;
    struct tm *ptrTm;

    now = time(NULL);

    ptrTm = localtime (&now);

    sosiMemSet (PtrBuffer, 0, sizeof (PtrBuffer) );

    sosiSprintf (PtrBuffer, sizeof (PtrBuffer), "%02d-%02d-%02d_%02d-%02d-%02d", (ptrTm->tm_mon + 1 ),ptrTm->tm_mday,(ptrTm->tm_year + 1900),ptrTm->tm_hour,ptrTm->tm_min,ptrTm->tm_sec);

#endif

    return (SCRUTINY_STATUS_SUCCESS);

}



/**
 *
 *  @method     sosiHexToInt()
 *
 *  @param      PtrString           Pointer to the string which has to be converted.
 *
 *  @param      PtrValue            Pointer to value where the result has to be stored.
 *
 *  @return     STATUS              '0' for success and non-zero for failure
 *
 *  @brief                          The method is used to convert the hex value in string
 *                                  to a integer value.
 *
 */

SCRUTINY_STATUS sosiHexToInt (const char *PtrString, PU32 PtrValue)
{

    U32 i = 0;

    int j = sosiStringLength (PtrString);
    char ch;
    int num = 0;

    if (PtrString[0] == '0' && (PtrString[1] == 'x' || PtrString[1] == 'X'))
    {
        i += 2; j -= 2;
    }

    if (j > 8)
    {
        return (SCRUTINY_STATUS_FAILED);
    }

    while (j--)
    {

        ch = PtrString[i];

        if (('0' <= ch && ch <= '9'))
        {
            num = num  * 16;
            num = num + (ch - '0');
        }

        else if(('A' <= ch && ch <= 'F'))
        {
            num = num * 16;
            num = num + (ch - '7');
        }

        else if(('a' <= ch && ch<= 'f'))
        {
            num = num * 16;
            num = num + (ch - 'W');
        }

        else
        {
            /*
             * This is an invalid number.
             */

            return (SCRUTINY_STATUS_FAILED);

        }

        i++;

    }

    *PtrValue = num;


    return (SCRUTINY_STATUS_SUCCESS);

}

/* Pravin - TO_DO: This function will be removed and will be replaced by sosiHexToInt() */

U32 hexadecimalToDecimal(char hexVal[]) 
{    
    U32 len = sosiStringLength(hexVal); 
      
    // Initializing base value to 1, i.e 16^0 
    U32 base = 1; 
      
    U32 dec_val = 0;
    U32 i;
    U32 isHex=0;
    
//    gPtrLoggerExpanders->logiDebug("hexVal = %s", hexVal);
    //for Hex, length must >2, because the prefix is 0x or 0X, and the string maybe have space.
    if(len > 2)
    { 
        for (i=1; i<len; i++)
        {
           if((hexVal[i]=='x' || hexVal[i]=='X')&&(hexVal[i-1]=='0'))
           {
               isHex=1;
           } 
        }
    }
//    gPtrLoggerExpanders->logiDebug("isHex = %d", isHex);
    if(!isHex)
    {
        return (sosiAtoi (hexVal)); 
    }
      
    // Extracting characters as digits from last character 
    for (i=len-1; i>=0; i--) 
    {    
        // if character lies in '0'-'9', converting  
        // it to integral 0-9 by subtracting 48 from 
        // ASCII value. 
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
                  
            // incrementing base by power 
            base = base * 16; 
        } 
  
        // if character lies in 'A'-'F' , converting  
        // it to integral 10 - 15 by subtracting 55  
        // from ASCII value 
        else if (hexVal[i]>='A' && hexVal[i]<='F') 
        { 
            dec_val += (hexVal[i] - 55)*base; 
          
            // incrementing base by power 
            base = base*16; 
        }
        // if character lies in 'a'-'f' , converting  
        // it to integral 10 - 15 by subtracting 87  
        // from ASCII value 
        else if (hexVal[i]>='a' && hexVal[i]<='f') 
        { 
            dec_val += (hexVal[i] - 87)*base; 
          
            // incrementing base by power 
            base = base*16; 
        }
        else if (hexVal[i]=='x' || hexVal[i]=='X')
        {
            break;
        }
         
    } 
      
    return dec_val; 
} 



