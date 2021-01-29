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


#ifndef __LIBRARY_OSAL__H__
#define __LIBRARY_OSAL__H__


#ifdef OS_UEFI

typedef EFI_FILE_HANDLE SOSI_FILE_HANDLE;
#define EFI_EVENT_TIMER                             (0x80000000)
#define RET_VAL_FAILURE                             (0x01)

#define LONG_MAX                                    (0x7FFFFFFFL)
#define CHAR_SIZE                                   ( 256 )



#define stdin                                       (SOSI_FILE_HANDLE)NULL
#define stdout                                      (SOSI_FILE_HANDLE)NULL
#define stderr                                      (SOSI_FILE_HANDLE)NULL
#define IsSpace(c)                                  ( (c == 0x20 ) || (c == 0x09 ) || (c == 0x0A ) || (c == 0x0B ) || (c == 0x0C ) || (c == 0x0D ) )
#define IsDigit(c)                                  (c >= '0' && c <= '9')
#define IsPrint(c)                                  (c >= 0x20 && c <= 0x7F)

#else

typedef FILE*           SOSI_FILE_HANDLE;

#endif


#ifdef OS_UEFI

char* efiiFixStringTypeSpecifier (char* Ptrstr);


#endif

#if defined (OS_UEFI) && !defined (TARGET_ARCH_ARM)

void * memset (void * ptr, int value, size_t num);
#pragma intrinsic (memset)

void * memcpy (void * destination, const void * source, size_t num);
#pragma intrinsic (memcpy)

#endif


VOID* sosiMemAlloc (__IN__ U32 Size);
VOID* sosiMemRealloc (__IN__ VOID *PtrMem, __IN__ U32 Size, __IN__ U32 OldSize);
VOID sosiMemFree (__IN__ VOID *PtrMem);
SCRUTINY_STATUS sosiMemCompare (__IN__ U8 *PtrFirst, __IN__ U8 *PtrSecond, __IN__ U32 Size);
VOID sosiMemCopy (__OUT__ VOID *PtrDest, __IN__ const VOID *PtrSrc, __IN__ U32 Size);
VOID sosiMemSet (__OUT__ VOID *PtrMem, __IN__ U8 FillChar, __IN__ U32 Size);
VOID sosiSleep (U32 Milli);

S32 sosiFprintf (__IN__ SOSI_FILE_HANDLE Stream, __IN__ const char* PtrArguments, ...);

U32 sosiSprintf (__OUT__ char *PtrString, __IN__ U32 StringSize, __IN__ const char* PtrArguments, ...);

U32 sosiIsPrint (const char Character);
BOOLEAN sosiIsSpace (__IN__ const char Character);

SCRUTINY_STATUS sosiStringCompare (__IN__ const char *PtrFirst, __IN__ const char *PtrSecond);
VOID sosiStringCat (__IN__ char *PtrFirst, __IN__ char *PtrSecond);


VOID sosiStringTrim (__INOUT__ char *PtrString);
U32 sosiStringLength (__IN__ const char *PtrStr);
VOID sosiStringCopy (__OUT__ char *PtrDestination, __IN__ const char *PtrSource);

BOOLEAN sosIsNumericChar (__IN__ U8 NumChar);
U32 sosiAtoi (__IN__ const char *PtrStr);
void sosiPrintTimestamp (const char* PtrPrefix);

SCRUTINY_STATUS sosiFileWrite (__IN__ SOSI_FILE_HANDLE FileHandle, __IN__ const U8 *PtrBuffer, __IN__ U32 Size);
SOSI_FILE_HANDLE sosiFileOpen (__IN__ const char *PtrFileName, __IN__ const char *PtrMode);
void sosiFileClose (__IN__ SOSI_FILE_HANDLE FileHandle);
SCRUTINY_STATUS sosiFileLength (__IN__ SOSI_FILE_HANDLE FileHandle, __OUT__ U32 *PtrSize);
SCRUTINY_STATUS sosiFileBufferRead (__IN__ const char *PtrFileName, __OUT__ U8 **PtrBuffer, __OUT__ U32 *PtrLength);
SCRUTINY_STATUS sosiFileBufferWrite (__IN__ const char *PtrFileName, __IN__ U8 *PtrBuffer, __IN__ U32 Length);
SCRUTINY_STATUS sosiFileRead (__IN__ SOSI_FILE_HANDLE FileHandle, __OUT__ U8 *PtrBuffer, __IN__ U32 Length);
SCRUTINY_STATUS sosiDumpHexMemory (PU8 PtrBuffer, U32 Size);
SCRUTINY_STATUS sosiStringCompareIgnoreCase (__IN__ const char *PtrFirst, __IN__ const char *PtrSecond);

SCRUTINY_STATUS sosiMkDir (__IN__ const char *PtrFolderName);
SCRUTINY_STATUS sosiHexToInt (const char *PtrString, PU32 PtrValue);
U32 hexadecimalToDecimal(char hexVal[]) ;
SCRUTINY_STATUS sosiGetSystemTime (char *PtrBuffer);


#endif /* __LIBRARY_OSAL__H__ */
