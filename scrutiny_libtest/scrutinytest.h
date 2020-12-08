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
#ifndef SCRUTINYTEST_H
#define SCRUTINYTEST_H

#define SCRUTINY_UTILITY_NAME   "ScrutinyLib-Test Utility"

#define SCRUTINY_VERSION_MAJOR          0
#define SCRUTINY_VERSION_MINOR          1
#define SCRUTINY_VERSION_REVISION       0
#define SCRUTINY_VERSION_DEV            0

/*
 * In scsi passthru reply we recieves bit fields as a output that is in the big endian format,
 * so the structure's big endianess has been changed to recieve correct data.
 */
typedef struct _SCRUTINY_SCSI_INQUIRY_DATA
{
#if defined(_sparc_) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
    U8      DeviceTypeQualifier: 3;
    U8      DeviceType: 5;
    U8      RemoveableMedia: 1;
    U8      DeviceTypeModifier: 7;
#else
    U8      DeviceType: 5;
    U8      DeviceTypeQualifier: 3;
    U8      DeviceTypeModifier: 7;
    U8      RemoveableMedia: 1;
#endif
    U8      Versions: 8;

#if defined(_sparc_) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
    U8      AERC: 1;
    U8      Reserved1: 1;
    U8      NormalACA: 1;
    U8      HiSupport: 1;
    U8      ResponseDataFormat: 4;
#else
    U8      ResponseDataFormat: 4;
    U8      HiSupport: 1;
    U8      NormalACA: 1;
    U8      Reserved1: 1;
    U8      AERC: 1;
#endif

    U8      AdditionalLength: 8;
    U8      Reserved2[2];
#if defined(_sparc_) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
    U8      RelativeAddressing: 1;
    U8      Wide32Bit: 1;
    U8      Wide16Bit: 1;
    U8      Synchronous: 1;
    U8      LinkedCommands: 1;
    U8      Reserved3: 1;
    U8      CommandQueue: 1;
    U8      SoftReset: 1;
#else
    U8      SoftReset: 1;
    U8      CommandQueue: 1;
    U8      Reserved3: 1;
    U8      LinkedCommands: 1;
    U8      Synchronous: 1;
    U8      Wide16Bit: 1;
    U8      Wide32Bit: 1;
    U8      RelativeAddressing: 1;
#endif
    U8      VendorId[8];
    U8      ProductId[16];
    U8      RevisionLevel[4];
    U8      VendorSpecific[20];
    U8      Reserved4[40];
} SCRUTINY_SCSI_INQUIRY_DATA, *PTR_SCRUTINY_SCSI_INQUIRY_DATA;


#define BORDER "\n-------------------------------------------"

#define NUM_VALID_ENTRIES  20


typedef FILE*           SOSI_Test_FILE_HANDLE;

SOSI_Test_FILE_HANDLE scrtnyLibFileOpen (__IN__ const char *PtrFileName, __IN__ const char *PtrMode);


typedef enum __STATUS
{

    STATUS_SUCCESS = 0,     /** Return Status Success */
    STATUS_FAILED = 1,      /** Return Status         */
    STATUS_RETRY,
    STATUS_IGNORE,
    STATUS_ABORT,
    STATUS_MEM_FAILED,
    STATUS_INVALID_PORT

} STATUS;

typedef struct _EVENT_TRIGGER_ENTRY
{
    U16		EventValue;
    U16     LogEntryQualifier;
	
} EVENT_TRIGGER_ENTRY, *PTR_EVENT_TRIGGER_ENTRY;

typedef struct _EVENT_TRIGGERS
{

    U32                   ValidEntries;
    EVENT_TRIGGER_ENTRY	  EventTriggerEntry [NUM_VALID_ENTRIES];
	
} EVENT_TRIGGERS, *PTR_EVENT_TRIGGERS;

#define EVENT_TRIGGERS_SIZE sizeof (EVENT_TRIGGERS)



typedef struct _SCSI_TRIGGER_ENTRY
{

    U8 	ASCQ;
    U8 	ASC;
    U8 	SenseKey;
    U8 	Reserved;
	
} SCSI_TRIGGER_ENTRY, *PTR_SCSI_TRIGGER_ENTRY;


typedef struct _SCSI_TRIGGERS
{
    U32                   	ValidEntries;
    SCSI_TRIGGER_ENTRY  	SCSITriggerEntry [NUM_VALID_ENTRIES];
	
} SCSI_TRIGGERS, *PTR_SCSI_TRIGGERS;

#define SCSI_TRIGGERS_SIZE sizeof (SCSI_TRIGGERS)

/*
 * MPI Triggers
 */

typedef struct _MPI_TRIGGER_ENTRY
{

    U16 	IOCStatus;
    U16 	Reserved;
    U32 	IOCLogInfo;
	
} MPI_TRIGGER_ENTRY, *PTR_MPI_TRIGGER_ENTRY;

typedef struct _MPI_TRIGGERS
{

    U32                   	ValidEntries;
    MPI_TRIGGER_ENTRY   	MPITriggerEntry [NUM_VALID_ENTRIES];
	
} MPI_TRIGGERS, *PTR_MPI_TRIGGERS;

#define MPI_TRIGGERS_SIZE sizeof (MPI_TRIGGERS)

typedef struct _MASTER_TRIGGERS
{	
	U32			MasterTriggerEntry;
	
} MASTER_TRIGGERS, *PTR_MASTER_TRIGGERS;


typedef struct _DIAG_BUFFER_OPERATION
{
	U8		BufferType;
	U32		DiagFlags;
	U32		BufferSize;
	U32		ProductSpecific [23];
	const char* 		FileName;
	U32		UniqId;

/* Flags for verification */	
	BOOLEAN	FlagBufferEnabled;	
	BOOLEAN FlagSize;
	BOOLEAN FlagTraceBuffer;
	BOOLEAN FlagSnapShotBuffer;
	BOOLEAN	FlagFileName;
	BOOLEAN FlagIopMask;
	BOOLEAN FlagPlMask;
	BOOLEAN FlagPoptMask;
	BOOLEAN FlagIrMask;
	
}DIAG_BUFFER_OPERATION, *PTR_DIAG_BUFFER_OPERATION;

typedef struct _TRIGGER_OPERATION
{
	MASTER_TRIGGERS		MasterTrigger;
	MPI_TRIGGERS		MpiTrigger;
	SCSI_TRIGGERS		ScsiTrigger;
	EVENT_TRIGGERS		EventTrigger;

	/* Flags for command verfication */
	BOOLEAN				FlagSet;
	BOOLEAN				FlagGet;
	BOOLEAN				FlagClear;
	BOOLEAN				FlagMaster;
	BOOLEAN				FlagEvent;
	BOOLEAN				FlagMPI;
	BOOLEAN				FlagSCSI;

}TRIGGER_OPERATION, *PTR_TRIGGER_OPERATION;

typedef enum _INVADER_DB_OPCODE
{
    INVADER_DB_OPCODE_NONE 			= 0,
	INVADER_DB_OPCODE_REGISTER		= 1,
	INVADER_DB_OPCODE_READ			= 2,
	INVADER_DB_OPCODE_QUERY			= 3,
	INVADER_DB_OPCODE_RELEASE		= 4,
	INVADER_DB_OPCODE_UNREGISTER	= 5,
	INVADER_DB_OPCODE_TRIGGER		= 6
} INVADER_DB_OPCODE;


typedef struct _INVADER_DIAG_BUFFER_PARAMS
{
	TRIGGER_OPERATION			TriggerParams;
	DIAG_BUFFER_OPERATION		DiagBufferParams;	
	INVADER_DB_OPCODE 			OpCode;

}INVADER_DIAG_BUFFER_PARAMS, *PTR_INVADER_DIAG_BUFFER_PARAMS;


void scrtnyPrintBanner();
STATUS scrutinyTest();
STATUS showSwitchMenu (PTR_SCRUTINY_DEVICE_INFO PtrDeviceInfo);

STATUS scrtnyControllerDetails (__IN__ U8 Index, __IN__ PTR_SCRUTINY_DEVICE_INFO PtrDeviceInfo);
STATUS scrtnyExpanderDetails (__IN__ U32 Index, __IN__ PTR_SCRUTINY_DEVICE_INFO  PtrDeviceInfo);
STATUS scrtnySwitchDetails (__IN__ U32 Index, __IN__ PTR_SCRUTINY_DEVICE_INFO PtrDeviceInfo);

STATUS scrtnySwitchFwCli (PU8 PtrCmdLine);
STATUS scrtnySwitchGetTemperature ();
STATUS scrtnySwitchGetTrace ();
STATUS scrtnySwitchHealthLogs (int LogType);
STATUS scrtnyGetCoreDump (int InputOperation);
STATUS scrtnyGetSwitchLogs();
STATUS scrtnySwitchGetHealth ();
STATUS scrtnySwitchHealthCheck ();
STATUS scrtnySwitchCounters ();
STATUS scrtnySwitchPortProperties ();
STATUS scrtnySwitchUploadSbr ();  
STATUS scrtnySwitchEraseSbr ();
STATUS scrtnySwitchDownloadSbr ();
STATUS scrtnySwitchSbrInfo ();

STATUS scrtnySwitchPortProperties ();
STATUS scrtnySwitchPerformLaneMargin ();
STATUS scrtnySwitchPowerOnSense ();
STATUS scrtnySwitchCCRStatus ();

STATUS scrtnySwitchRxEqStatus ();
STATUS scrtnySwitchTxCoeff ();
STATUS scrtnySwitchHwEye ();
STATUS scrtnySwitchSoftEye ();


STATUS scrtnySwitchMemoryRead (U32 Address, U32 SizeInBytes);
STATUS scrtnySwitchMemoryWrite (U32 Address, PU8 PtrValue, U32 SizeInBytes);
STATUS scrtnySwitchGetConfigPage ();





STATUS scrtnyOtc ();


STATUS scrtnyOtcGetCurrentDirectory(char* PtrDirPath);

long long timeInMilliseconds(void) ;

#endif