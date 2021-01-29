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

 /* Test MUX and ALADIN core registers definition to parse the ALADIN conf file */

#ifndef __SWITCH_ALADIN_REG__H__
#define __SWITCH_ALADIN_REG__H__

#define ALADIN_FILE_CONFIG_INI        	"Aladin.ini"
#define ALADIN_FILE_DEFAULT_DUMP       	"Aladin_trace.bin"


#define ALADIN_CFG_MAX_ITEMS 28

/* 
*  The definitions are meant for conf file interpretation 
*    Conventional register definition style is not followed 
*/

/*Internal function and structure definitions only */

/*
typedef struct _SWITCH_ALADIN_CONFIG_DATA 
{
U32		moduleA;
U32		moduleB;
U32		station;
U32		portA;
U32		portB;
U32		signalA;
U32		signalB;
U32		trigMaskA0;
U32		trigMaskB0;
U32		trigMaskA1;
U32		trigMaskB1;
U32		dataMaskA;
U32		dataMaskB;
U32		triggerA0;
U32		triggerB0;
U32		triggerA1;
U32		triggerB1;
U32		Cond0notEqTo;
U32		Cond1notEqTo;
U32		Compression;
U32		PostCaptureMode;
U32		captureLoc;
U32		readTime;
U32		trigSrc;
U32		eventSrc0;
U32		eventSrc1;
U32		eventCnt0;
U32		eventCnt1;
U32     AladinCfgItems[ALADIN_CFG_MAX_ITEMS];

}SWITCH_ALADIN_CONFIG_DATA, *PTR_SWITCH_ALADIN_CONFIG_DATA;
*/

typedef enum _ALADIN_CFG_MONITOR_ITEMS 
{
    MODULEA = 0,
    MODULEB,
    STATION,
    PORTA,
    PORTB,
    SIGNALA, 
    SIGNALB,
    TRIGMASKA0,
    TRIGMASKB0,    
    TRIGMASKA1,
    TRIGMASKB1,
    DATAMASKA,
    DATAMASKB,
    TRIGGERA0,
    TRIGGERB0,
    TRIGGERA1,
    TRIGGERB1,
    COND0NOTEQTO,
    COND1NOTEQTO,
    COMPRESSION,
    POSTCAPTUREMODE,
    CAPTURELOC,
    READTIME,
    TRIGSRC,
    EVENTSRC0,
    EVENTSRC1,
    EVENTCOUNT0,
    EVENTCOUNT1    
} ALADIN_CFG_MONITOR_ITEMS;

typedef struct _SWITCH_ALADIN_CONFIG_DATA 
{
U32 	tmuxTicSel;
U32		tmuxPhydllSel;
U32		tmuxTecSel;
U32     AladinCfgItems[ALADIN_CFG_MAX_ITEMS];
U32		parsedone;
U32   	Configured;
U32		moduleA;
U32		moduleB;
U32		station;
U32		portA;
U32		portB;
U32		signalA;
U32		signalB;
U32		trigMaskA0;
U32		trigMaskB0;
U32		trigMaskA1;
U32		trigMaskB1;
U32		dataMaskA;
U32		dataMaskB;
U32		triggerA0;
U32		triggerB0;
U32		triggerA1;
U32		triggerB1;
U32		Cond0notEqTo;
U32		Cond1notEqTo;
U32		Compression;
U32		PostCaptureMode;
U32		captureLoc;
U32		readTime;
U32		trigSrc;
U32		eventSrc0;
U32		eventSrc1;
U32		eventCnt0;
U32		eventCnt1;
}SWITCH_ALADIN_CONFIG_DATA, *PTR_SWITCH_ALADIN_CONFIG_DATA;

SCRUTINY_STATUS satcSetAladinConfiguration (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __OUT__ PU32 PtrConfigStatus);
SCRUTINY_STATUS satcPollAladinTrigger (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrTrigStatus);
SCRUTINY_STATUS satcStopAladinTrace (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __OUT__ PU32 PtrStopStatus);
SCRUTINY_STATUS satcGetAladinTracedump (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrDumpFilePath, __OUT__ PU32 PtrDumpStatus);


SCRUTINY_STATUS satcInitializeAtlasConfiguration(PTR_SCRUTINY_DEVICE PtrDevice,__IN__ const char* PtrFolderName);
SCRUTINY_STATUS satcSetAladinConfiguration (__IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ const char* PtrConfigFilePath, __OUT__ PU32 PtrConfigStatus);
SCRUTINY_STATUS satcPrepTmuxsettings();
SCRUTINY_STATUS satcAladinRegRead (__IN__  PTR_SCRUTINY_DEVICE PtrDevice, __IN__  U32 Offset, __OUT__ PU32 PtrValue);
SCRUTINY_STATUS satcAladinRegWrite(__IN__ PTR_SCRUTINY_DEVICE PtrDevice,__IN__ U32 Offset,__IN__ U32 Value);
SCRUTINY_STATUS satcdumpreg(__IN__  PTR_SCRUTINY_DEVICE PtrDevice);
void satcrecoveryReason(U32 reason, char *output);
void satcprobeDataDecode(U8 module, U8 bus, U8 port, U8 signal, U32 data, char *output);





#endif



