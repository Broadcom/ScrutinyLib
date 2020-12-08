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

/* If this header file has not been included yet */
#ifndef SCSI_H
#define SCSI_H

/******************************************************************************/
/*                                                                            */
/*      S h a r e d   S C S I   D e f i n i t i o n s                         */
/*                                                                            */
/******************************************************************************/

/*  SCSI_COMMAND values are used to identify SCSI command descriptor block
 *  (CDB) function codes.  Only select commands from all device types and
 *  others from direct access device types are listed from the SCSI-2
 *  document X3T9.2/375R Revision 10L.
 */



#define SCSI_WRITE_MODE_VENDOR_SPECIFIC             0x01
#define SCSI_READ_MODE_VENDOR_SPECIFIC              0x01
#define SCSI_READ_MODE_DESCRIPTOR                   0x03
#define SCSI_READ_MODE_DATA                         0x02
#define SCSI_WRITE_MODE_WRITE_DATA                  0x02




#define SCSI_COMMAND_TEST_UNIT_READY                        (0x00)
#define SCSI_COMMAND_REQUEST_SENSE                          (0x03)
#define SCSI_COMMAND_FORMAT_UNIT                            (0x04)
#define SCSI_COMMAND_REASSIGN_BLOCKS                        (0x07)
#define SCSI_COMMAND_READ_6                                 (0x08)
#define SCSI_COMMAND_WRITE_6                                (0x0A)
#define SCSI_COMMAND_SEEK                                   (0x0B)
#define SCSI_COMMAND_INQUIRY                                (0x12)
#define SCSI_COMMAND_MODE_SELECT                            (0x15)
#define SCSI_COMMAND_RESERVE                                (0x16)
#define SCSI_COMMAND_RELEASE                                (0x17)
#define SCSI_COMMAND_MODE_SENSE                             (0x1A)
#define SCSI_COMMAND_START_STOP_UNIT                        (0x1B)
#define SCSI_COMMAND_RECEIVE_DIAGNOSTIC                     (0x1C)
#define SCSI_COMMAND_SEND_DIAGNOSTIC                        (0x1D)
#define SCSI_COMMAND_PREVENT_ALLOW                          (0x1E)
#define SCSI_COMMAND_READ_CAPACITY                          (0x25)
#define SCSI_COMMAND_READ_10                                (0x28)
#define SCSI_COMMAND_WRITE_10                               (0x2A)
#define SCSI_COMMAND_SEEK_EXTENDED                          (0x2B)
#define SCSI_COMMAND_WRITE_VERIFY_10                        (0x2E)
#define SCSI_COMMAND_VERIFY_10                              (0x2F)
#define SCSI_COMMAND_SYNCHRONIZE_CACHE                      (0x35)
#define SCSI_COMMAND_READ_BUFFER                            (0x3C)
#define SCSI_COMMAND_WRITE_BUFFER                           (0x3B)
#define SCSI_COMMAND_READ_LONG                              (0x3E)
#define SCSI_COMMAND_WRITE_LONG                             (0x3F)
#define SCSI_COMMAND_WRITE_SAME                             (0x41)
#define SCSI_COMMAND_LOG_SELECT                             (0x4C)
#define SCSI_COMMAND_LOG_SENSE                              (0x4D)
#define SCSI_COMMAND_MODE_SELECT_10                         (0x55)
#define SCSI_COMMAND_MODE_SENSE_10                          (0x5A)
#define SCSI_COMMAND_PERS_RESERVE_OUT                       (0x5F)
#define SCSI_COMMAND_ATA_PASSTHROUGH_16                     (0x85)
#define SCSI_COMMAND_READ_16                                (0x88)
#define SCSI_COMMAND_WRITE_16                               (0x8A)
#define SCSI_COMMAND_WRITE_VERIFY_16                        (0x8E)
#define SCSI_COMMAND_VERIFY_16                              (0x8F)
#define SCSI_COMMAND_SYNCHRONIZE_CACHE_16                   (0x91)
#define SCSI_COMMAND_READ_CAPACITY_16                       (0x9E)
#define SCSI_COMMAND_WRITE_LONG_16                          (0x9F)
#define SCSI_COMMAND_REPORT_LUNS                            (0xA0)
#define SCSI_COMMAND_ATA_PASSTHROUGH_12                     (0xA1)
#define SCSI_COMMAND_READ_12                                (0xA8)
#define SCSI_COMMAND_WRITE_12                               (0xAA)
#define SCSI_COMMAND_READ_MEDIA_SERIAL_NUMBER               (0xAB)
#define SCSI_COMMAND_WRITE_VERIFY_12                        (0xAE)
#define SCSI_COMMAND_VERIFY_12                              (0xAF)
#define SCSI_COMMAND_READ_32                                (0x7F)  //TODO: the current draft spec. has not defined the OpCode
#define SCSI_COMMAND_WRITE_32                               (0x7F)  //TODO: the current draft spec. has not defined the OpCode
#define SCSI_COMMAND_REZERO_UNIT                            (0x01)


/*  SCSI_DEVICE values are defined for all SCSI device types known as of the
 *  SCSI-2 document X3T9.2/375R Revision 10L.  The SCSI_DEVICE_MASK is provided
 *  to mask off the device type bits in the first byte of Inquiry data.
 *
 *  NOTE: The first byte in the inquiry data is a combination of one
 *        SCSI_DEVICE and one SCSI_DEVICE_QUALIFIER value.
 */

#define SCSI_DEVICE_MASK           (0x1F)

#define SCSI_DEVICE_DIRECT         (0x00)
#define SCSI_DEVICE_SEQUENTIAL     (0x01)
#define SCSI_DEVICE_PRINTER        (0x02)
#define SCSI_DEVICE_PROCESSOR      (0x03)
#define SCSI_DEVICE_WRITE_ONCE     (0x04)
#define SCSI_DEVICE_CD_ROM         (0x05)
#define SCSI_DEVICE_SCANNER        (0x06)
#define SCSI_DEVICE_OPTICAL        (0x07)
#define SCSI_DEVICE_MEDIUM_CHANGER (0x08)
#define SCSI_DEVICE_COMMUNICATIONS (0x09)
#define SCSI_DEVICE_SES            (0x0D)
#define SCSI_DEVICE_UNKNOWN        (0x1F)

/*  SCSI_DEVICE_QUALIFIER values are defined for all SCSI device qualifiers
 *  known as of the SCSI-2 document X3T9.2/375R Revision 10L.  The
 *  SCSI_DEVICE_QUALIFIER_MASK is provided to mask off the device type bits in
 *  the first byte of Inquiry data.
 *
 *  NOTE: The first byte in the inquiry data is a combination of one
 *        SCSI_DEVICE and one SCSI_DEVICE_QUALIFIER value.
 */

#define SCSI_DEVICE_QUALIFIER_MASK          (0xE0)

#define SCSI_DEVICE_QUALIFIER_AVAILABLE     (0x00)
#define SCSI_DEVICE_QUALIFIER_NOT_CONNECTED (0x20)
#define SCSI_DEVICE_QUALIFIER_NOT_AVAILABLE (0x60)

/* SCSI Protocol Identifier Values */
#define SCSI_PROTOCOL_FIBRE_CHANNEL         (0x00)
#define SCSI_PROTOCOL_PARALLEL_SCSI         (0x01)
#define SCSI_PROTOCOL_SSA                   (0x02)
#define SCSI_PROTOCOL_IEEE_1394             (0x03)
#define SCSI_PROTOCOL_SCSI_RDMA             (0x04)
#define SCSI_PROTOCOL_ISCSI                 (0x05)
#define SCSI_PROTOCOL_SAS                   (0x06)
#define SCSI_PROTOCOL_ADT                   (0x07)
#define SCSI_PROTOCOL_ATA_ATAPI             (0x08)


/******************************************************************************/
/* SCSI STATUS                                                                */
/******************************************************************************/
/*  SCSI_STATUS values are defined below for all SCSI status phase known
 *  conditions as of the SCSI-2 document X3T9.2/375R Revision 10L.  The
 *  SCSI_STATUS_MASK is provided to mask off the status field for the
 *  non-reserved bits.
 */

#define SCSI_STATUS_MASK                       (0x3E)

#define SCSI_STATUS_GOOD                       (0x00)
#define SCSI_STATUS_CHECK_CONDITION            (0x02)
#define SCSI_STATUS_CONDITION_MET              (0x04)
#define SCSI_STATUS_BUSY                       (0x08)
#define SCSI_STATUS_INTERMEDIATE               (0x10)
#define SCSI_STATUS_INTERMEDIATE_CONDITION_MET (0x14)
#define SCSI_STATUS_RESERVATION_CONFLICT       (0x18)
#define SCSI_STATUS_COMMAND_TERMINATED         (0x22)
#define SCSI_STATUS_QUEUE_FULL                 (0x28)


#define MAX_SENSE_DATA_LENGTH                   (252)


/* This is the MPI Control field for a request sense generated internally */
#define REQUEST_SENSE_CONTROL (MPI2_SCSIIO_CONTROL_READ | MPI2_SCSIIO_CONTROL_SIMPLEQ)





/******************************************************************************/
/*                                                                            */
/*      S C S I   C D B   S T R U C T U R E S                                 */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Inquiry Command                                                            */
/******************************************************************************/

/* This is sent to a target to obtain Inquiry information,
 * or "Vital Product Data" if the EVPD bit is set.
 *
 * Internal IO's - we may set the EVPD bit.
 * External IO's - we do not support host IO's setting the EVPD bit, and will
 *                 respond with Check Condition, Sense Key = Illegal Request,
 *                 ASC = Invalid Field in CDB, ASCQ = 0x00.
 */
typedef struct _SCSI_INQUIRY_CDB
{
    U8  Opcode;             /* 0x12                                           */
    U8  LunEvpd;            /* Lun = [7..5], EVPD = bit 0                     */
    U8  VitalPageCode;      /* 0x00, 0x80 = Unit Serial Number Page if EVPD   */
    U8  Reserved;
    U8  AllocationLength;   /*                                                */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_INQUIRY_CDB, *PTR_SCSI_INQUIRY_CDB;

#define SCSI_INQUIRY_EVPD_BIT              (0x01)
#define SCSI_INQUIRY_CMDDT_BIT             (0x02)

/* Vital Product Data Vital Page Codes (VPC) */
#define SCSI_INQUIRY_VPC_SUPPORTED_PAGE    (0x00)
#define SCSI_INQUIRY_VPC_SERIAL_NUMBER     (0x80)
#define SCSI_INQUIRY_VPC_DEVICE_ID         (0x83)
#define SCSI_INQUIRY_VPC_MODE_PAGE_POLICY   (0x87)
#define SCSI_INQUIRY_VPC_ATA_INFO          (0x89)
#define SCSI_INQUIRY_VPC_BLOCK_DEVICE_CHAR (0xB1)


/******************************************************************************/
/* Test Unit Ready (TUR) Command                                              */
/******************************************************************************/

typedef struct _SCSI_TUR_CDB
{
    U8  Opcode;             /* 0x00                                           */
    U8  Lun;                /* Lun = [7..5]                                   */
    U8  Reserved[3];
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_TUR_CDB, *PTR_SCSI_TUR_CDB;


/******************************************************************************/
/* Read Command                                                               */
/******************************************************************************/

typedef struct _SCSI_READ_CDB
{
    U8  Opcode;             /* 0x08                                           */
    U8  LunLba;             /* Lun = [7..5],  LBA bits [20..16] = [4..0]      */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  NumBlocks;          /* 0 == 256 blocks (128 KBytes if 512 per block)  */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_READ_CDB, *PTR_SCSI_READ_CDB;

#define SCSI_READ_LBA_MASK  (0x1F)  /* bits [20..16] of LBA                   */


/******************************************************************************/
/* Read Extended Command                                                      */
/******************************************************************************/

typedef struct _SCSI_READ_10_CDB
{
    U8  Opcode;             /* 0x28                                           */
    U8  LunFlags;           /* Lun = [7..5], DPO, FUA, RA in other bits       */
    U8  Lba3;               /* LBA bits [31..24]                              */
    U8  Lba2;               /* LBA bits [23..16]                              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  Reserved;           /* 0x00                                           */
    U8  NumBlocks1;         /* bits [15..8] (0 == 0 blocks to transfer)       */
    U8  NumBlocks0;         /* bits [7..0]                                    */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_READ_10_CDB, *PTR_SCSI_READ_10_CDB;

#define SCSI_READ_10_DPO    (0x10)  /* Disable Page Out (normally 0)          */
#define SCSI_READ_10_FUA    (0x08)  /* Force Unit Access (normally 0)         */
#define SCSI_READ_10_RA     (0x01)  /* Relative Address,for linked commands   */


/******************************************************************************/
/* Verify (10) Command                                                        */
/******************************************************************************/

typedef struct _SCSI_VERIFY_CDB
{
    U8  Opcode;             /* 0x2F                                           */
    U8  LunFlags;           /* Lun = [7..5], DPO = bit 4, BytChk = bit 1      */
                            /*  RelAdr = bit 0                                */
    U8  Lba3;               /* LBA bits [31..24]                              */
    U8  Lba2;               /* LBA bits [23..16]                              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  Reserved;
    U8  NumBlocks1;         /* bits [15..8] (0 == 0 blocks to verify)         */
    U8  NumBlocks0;         /* bits [7..0]                                    */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_VERIFY_CDB, *PTR_SCSI_VERIFY_CDB;

#define SCSI_VERIFY_DPO       (0x10)  /* Disable Page Out (normally 0)        */
#define SCSI_VERIFY_BC        (0x02)  /* ByteCheck 0 = no data comparison     */
                                      /*           1 = compare data bytes     */
#define SCSI_VERIFY_RA        (0x01)  /* Relative Address,for linked commands */

/******************************************************************************/
/* Verify (12) Command                                                        */
/******************************************************************************/
typedef struct _SCSI_VERIFY_12_CDB
{
    U8  Opcode;              /* 0xAF                                          */
    U8  Flags;               /* Rsvd[7:5;3], DPO = bit4, BlkVfy = bit2,       */
                             /* BytChk = bit1, RelAdr = bit0                  */
    U8  Lba3;                /* LBA bits [31..24]                             */
    U8  Lba2;                /* LBA bits [23..16]                             */
    U8  Lba1;                /* LBA bits [15..8]                              */
    U8  Lba0;                /* LBA bits [7..0]                               */
    U8  NumBlocks3;          /* Verification Length [31:24]                   */
    U8  NumBlocks2;          /* Verification Length [23:16]                   */
    U8  NumBlocks1;          /* Verification Length [15:8]                    */
    U8  NumBlocks0;          /* Verification Length [7:0]                     */
    U8  Reserved;
    U8  Control;
}SCSI_VERIFY_12_CDB, *PTR_SCSI_VERIFY_12_CDB;

/******************************************************************************/
/* Verify (16) Command                                                        */
/******************************************************************************/
typedef struct _SCSI_VERIFY_16_CDB
{
    U8  Opcode;             /* 0x8F                                           */
    U8  Flags;              /* Rsvd[7:5;3], DPO = bit4, BlkVfy = bit2,        */
                            /* BytChk = bit1, RelAdr = bit0                   */
    U8  Lba7;                /* LBA bits [63.56]                              */
    U8  Lba6;                /* LBA bits [55..48]                             */
    U8  Lba5;                /* LBA bits [47..40]                             */
    U8  Lba4;                /* LBA bits [39..32]                             */
    U8  Lba3;                /* LBA bits [31..24]                             */
    U8  Lba2;                /* LBA bits [23..16]                             */
    U8  Lba1;                /* LBA bits [15..8]                              */
    U8  Lba0;                /* LBA bits [7..0]                               */
    U8  NumBlocks3;          /* Verification Length [31:24]                   */
    U8  NumBlocks2;          /* Verification Length [23:16]                   */
    U8  NumBlocks1;          /* Verification Length [15:8]                    */
    U8  NumBlocks0;          /* Verification Length [7:0]                     */
    U8 Reserved;
    U8 Control;
} SCSI_VERIFY_16_CDB, *PTR_SCSI_VERIFY_16_CDB;

/******************************************************************************/
/* Write Command                                                              */
/******************************************************************************/

typedef struct _SCSI_WRITE_CDB
{
    U8  Opcode;             /* 0x0A                                           */
    U8  LunLba;             /* Lun = [7..5], MSB of LBA = [4..0]              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [ 7..0]                               */
    U8  NumBlocks;          /* 0 == 256 blocks (128 KBytes if 512 per block)  */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_WRITE_CDB, *PTR_SCSI_WRITE_CDB;

#define SCSI_WRITE_LBA_MASK (0x1F)      /* bits [20..16] of LBA               */


/******************************************************************************/
/* Extended Write Command                                                     */
/******************************************************************************/

typedef struct _SCSI_WRITE_10_CDB
{
    U8  Opcode;             /* 0x2A                                           */
    U8  LunFlags;           /* Lun = [7..5], DPO, FUA, RA in other bits       */
    U8  Lba3;               /* LBA bits [31..24]                              */
    U8  Lba2;               /* LBA bits [23..16]                              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  Reserved;
    U8  NumBlocks1;         /* bits [15..8] (0 == 0 blocks to transfer)       */
    U8  NumBlocks0;         /* bits [7..0]                                    */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_WRITE_10_CDB, *PTR_SCSI_WRITE_10_CDB;

#define SCSI_WRITE_10_DPO      (0x10)     /* Disable Page Out (normally 0)    */
#define SCSI_WRITE_10_FUA      (0x08)     /* Force Unit Access (normally 0)   */
#define SCSI_WRITE_10_RA       (0x01)     /* Rel. Address, (linked commands)  */


/******************************************************************************/
/* Write Verify Command                                                       */
/******************************************************************************/

typedef struct _SCSI_WRITE_VERIFY_CDB
{
    U8  Opcode;             /* 0x2E                                           */
    U8  LunFlags;           /* Lun = [7..5], DPO, BytChk, RelAdr= flags       */
    U8  Lba3;               /* LBA bits [31..24]                              */
    U8  Lba2;               /* LBA bits [23..16]                              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  Reserved;
    U8  NumBlocks1;         /* bits [15..8] (0 == 0 blocks to transfer)       */
    U8  NumBlocks0;         /* bits [7..0]                                    */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_WRITE_VERIFY_CDB, *PTR_SCSI_WRITE_VERIFY_CDB;

#define SCSI_WRITE_VERIFY_DPO      (0x10) /* Disable Page Out (normally 0)    */
#define SCSI_WRITE_VERIFY_BC       (0x02) /* ByteCheck 0 = no data comparison */
                                          /*           1 = compare data bytes */
#define SCSI_WRITE_VERIFY_RA       (0x01) /* Rel. Address, (linked commands)  */


/******************************************************************************/
/* Write Long Command                                                         */
/******************************************************************************/

#define SCSI_WRITE_LONG_BYTE1_COR_DIS         (0x80)
#define SCSI_WRITE_LONG_BYTE1_WR_UNCOR        (0x40)
#define SCSI_WRITE_LONG_BYTE1_PBLOCK          (0x20)


/******************************************************************************/
/* Reassign Blocks Command                                                    */
/******************************************************************************/

typedef struct _SCSI_REASSIGN_BLOCKS_CDB
{
    U8  Opcode;             /* 0x07                                           */
    U8  Lun;                /* Lun = [7..5], Reserved = [4..0]                */
    U8  Flags;              /* Reserved[7..2], LongLba, LongList              */
    U8  Reserved[2];        /* Reserved                                       */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_REASSIGN_BLOCKS_CDB, *PTR_SCSI_REASSIGN_BLOCKS_CDB;

#define SCSI_REASSIGN_BLOCKS_LONGLBA    (0x02)  /* Long LBA (8 byte) if set */
#define SCSI_REASSIGN_BLOCKS_LONGLIST   (0x01)  /* Long list defect list format if set */

/******************************************************************************/
/* Reassign Blocks Defect List                                                */
/******************************************************************************/

/* Short format */
/* This structure also contains the header returned.  Note that this only             */
/* defines the 1st lba in the defect list.                                            */
/* Depending on LongLBA flag in CDB, LBAs in the list are either 4 or 8 (LongList==1) */
/* if LongList == 0, then ListLength2==ListLength3==0                                 */

typedef struct _SCSI_REASSIGN_BLOCKS_DATA
{
    U8  ListLength3;        /* Defect List Length bits [31..24] (4/8 * #Desc.)  */
    U8  ListLength2;        /* Defect List Length bits [23..16] (4/8 * #Desc.)  */
    U8  ListLength1;        /* Defect List Length bits [15..8]  (4/8 * #Desc.)  */
    U8  ListLength0;        /* Defect List Length bits [7..0]                 */
    U8  DefectLba3;         /* Defect LBA bits [31..24]                       */
    U8  DefectLba2;         /* Defect LBA bits [23..16]                       */
    U8  DefectLba1;         /* Defect LBA bits [15..8]                        */
    U8  DefectLba0;         /* Defect LBA bits [7..0]                         */
} SCSI_REASSIGN_BLOCKS_DATA, *PTR_SCSI_REASSIGN_BLOCKS_DATA;

#define SCSI_REASSIGN_BLOCKS_DEFECT_LIST_HEADER_LEN 4


/******************************************************************************/
/* Read Capacity Command                                                      */
/******************************************************************************/

typedef struct _SCSI_READ_CAPACITY_CDB
{
    U8  Opcode;             /* 0x25                                           */
    U8  LunRelAdr;          /* Lun = [7..5], RelAdr = bit 0                   */
    U8  Lba3;               /* LBA bits [31..24]                              */
    U8  Lba2;               /* LBA bits [23..16]                              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  Reserved[2];
    U8  PMI;                /* Partial Medium Indicator (PMI) == bit 0        */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_READ_CAPACITY_CDB, *PTR_SCSI_READ_CAPACITY_CDB;

#define SCSI_READ_CAPACITY_PMI    (0x01)  /* Partial Medium Indicator         */


/******************************************************************************/
/* Read Capacity 16 Command                                                      */
/******************************************************************************/

typedef struct _SCSI_READ_CAPACITY_16_CDB
{
    U8  Opcode;             /* 0x9E                                               */
    U8  ResServiceAction;   /* 0-4 ServiceAction(10h) 5-7 Reserved*/
    U8  Lba7;               /* LBA bits [63..56]                              */
    U8  Lba6;               /* LBA bits [55..48]                              */
    U8  Lba5;               /* LBA bits [47..40]                               */
    U8  Lba4;               /* LBA bits [39..32]                                */
    U8  Lba3;               /* LBA bits [31..24]                              */
    U8  Lba2;               /* LBA bits [23..16]                              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  AllocationLength3;  /* */
    U8  AllocationLength2;  /* */
    U8  AllocationLength1;  /* */
    U8  AllocationLength0;  /* */
    U8  PMI;                /* Partial Medium Indicator (PMI) == bit 0        */
    U8  Control;            /* 0x00 - only for linked commands.               */
} SCSI_READ_CAPACITY_16_CDB, *PTR_SCSI_READ_CAPACITY_16_CDB;


/******************************************************************************/
/* Start Stop Unit Command                                                    */
/******************************************************************************/

typedef struct _SCSI_START_STOP_UNIT_CDB
{
    U8  Opcode;             /* 0x28                                           */
    U8  LunImmed;           /* Lun = [7..5], Immed = bit 0                    */
    U8  Reserved[2];
    U8  Start;              /* LoEj = bit 1, Start = bit 0                    */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_START_STOP_UNIT_CDB, *PTR_SCSI_START_STOP_UNIT_CDB;

#define SCSI_SS_UNIT_IMMEDIATE      (0x01)  /* 1= return before complete      */
#define SCSI_SS_UNIT_START          (0x01)  /* 1= Start, 0 = Stop             */
#define SCSI_SS_UNIT_EJECT          (0x02)
#define SCSI_SS_UNIT_LOAD           (0x03)
#define SCSI_SS_UNIT_POWER_ACTIVE   (0x10)


/******************************************************************************/
/* Mode Sense Command                                                         */
/******************************************************************************/

typedef struct _SCSI_MODE_SENSE_CDB
{
    U8  Opcode;             /* 0x1A                                           */
    U8  LunDBD;             /* Lun = [7..5], DBD = bit 3                      */
    U8  PageCode;           /* PC = [7..6], PageCode = [5..0]                 */
    U8  Reserved;
    U8  Length;             /*                                                */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_MODE_SENSE_CDB, *PTR_SCSI_MODE_SENSE_CDB;

typedef struct _SCSI_MODE_SENSE_10_CDB
{
    U8  Opcode;             /* 0x5A                                           */
    U8  LLBAA_DBD;          /* Reserved [7:5;2:0]; LLBAA = bit 4; DBD = bit 3 */
    U8  PageCode;           /* PC = [7:6], PageCode = [5:0]                   */
    U8  SubPageCode;
    U8  Reserved[3];
    U16 Length;
    U8  Control;
    U16 Pad;
}SCSI_MODE_SENSE_10_CDB, *PTR_SCSI_MODE_SENSE_10_CDB;

/*  Disable Block Descriptors (DBD) */
#define SCSI_MODE_SENSE_DBD                   (0x08)

/* Long LBA (LLBAA) */
#define SCSI_MODE_SENSE_10_LLBAA              (0x10)

/*  PAGE CONTROL (PC) values */
#define SCSI_MODE_SENSE_PC_MASK               (0xC0)
#define SCSI_MODE_SENSE_PC_CURRENT            (0x00)
#define SCSI_MODE_SENSE_PC_CHANGEABLE         (0x40)
#define SCSI_MODE_SENSE_PC_DEFAULT            (0x80)
#define SCSI_MODE_SENSE_PC_SAVED              (0xC0)

/*  Mode Sense/Select Pages for PageCode field */
#define SCSI_MODE_PAGE_PAGE_CODE_MASK             (0x3F)
#define SCSI_MODE_PAGE_READ_WRITE_ERROR_RECOVERY  (0x01)
#define SCSI_MODE_PAGE_FORMAT_DEVICE              (0x03)
#define SCSI_MODE_PAGE_RIGID_DISK_GEOMETRY        (0x04)
#define SCSI_MODE_PAGE_CACHING                    (0x08)  /* Caching Options  */
#define SCSI_MODE_PAGE_CONTROL_MODE               (0x0A)
#define SCSI_MODE_PAGE_INFORMATIONAL_EXCEPTIONS   (0x1C)  /* SMART            */
#define SCSI_MODE_PAGE_GET_ALL_PAGES              (0x3F)
#define SCSI_MODE_PAGE_INVALID                    (0xFF)

#define SCSI_MODE_PAGE_ALL_SUB_PAGES              (0xFF)


/******************************************************************************/
/* Mode Select Command                                                        */
/******************************************************************************/

typedef struct _SCSI_MODE_SELECT_CDB
{
    U8  Opcode;             /* 0x15                                           */
    U8  LunFlags;           /* Lun = [7..5], PF = bit 4, SP = bit 0           */
    U8  Reserved[2];
    U8  Length;             /* Parameter list length                          */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_MODE_SELECT_CDB, *PTR_SCSI_MODE_SELECT_CDB;

typedef struct _SCSI_MODE_SELECT_CDB_10
{
    U8  Opcode;             /* 0x55                                           */
    U8  LunFlags;           /* Rsvd [7:5]; PF = bit 4; Rsvd[3:1]; SP = bit 0  */
    U8  Reserved[5];
    U16 Lengh;              /* Parameter list length                          */
    U8  Control;
    U16 Pad;                /* Padding for Dword Alignment                    */
}SCSI_MODE_SELECT_10_CDB, *PTR_SCSI_MODE_SELECT_10_CDB;

#define SCSI_MODE_SELECT_PAGE_FORMAT  (0x10)
#define SCSI_MODE_SELECT_SAVE_PAGES   (0x01)  /* 0= don't save, 1= save pages */
#define SCSI_MODE_PARAMETERS_SAVABLE  (0x80)  /* PS bit in each Mode Page */

typedef struct _SCSI_MODE_6_HEADER
{
    U8  ModeDataLength;
    U8  MediumType;
    U8  DeviceSpecificParameter;
    U8  BlockDescriptorLength;
} SCSI_MODE_6_HEADER, *PTR_SCSI_MODE_6_HEADER;

typedef struct _SCSI_MODE_10_HEADER
{
    U16   ModeDataLength;
    U8    MediumType;
    U8    DeviceSpecificParameter;
    U8    Rsvd_LongLba;             /* Rsvd[7:1] LongLBA = bit 0            */
    U8    Rsvd;
    U16   BlockDescriptorLength;

}SCSI_MODE_10_HEADER, *PTR_SCSI_MODE_10_HEADER;

typedef struct _SCSI_MODE_BLOCK_DESCRIPTOR
{
    U8 NumberBlocks[4];
    U8 Reserved;
    U8 BlockLength[3];
} SCSI_MODE_BLOCK_DESCRIPTOR, *PTR_SCSI_MODE_BLOCK_DESCRIPTOR;

typedef struct _SCSI_MODE_CACHING_PAGE
{
    U8  PageCode;
    U8  PageLength;
    U8  Flags;
    U8  RetentionPriority;
    U8  DisablePrefetchTransferLength[2];
    U8  MinimumPrefetch[2];
    U8  MaximumPrefetch[2];
    U8  MaximumPrefetchCeiling[2];
    U8  Flags2;
    U8  CacheSegments;
    U8  CacheSegmentSize[2];
    U8  Reserved;
    U8  NonCacheSegmentSize[3];
} SCSI_MODE_CACHING_PAGE, *PTR_SCSI_MODE_CACHING_PAGE;

#define SCSI_MODE_CACHING_FLAG_RCD      (0x01)   /* Read Cache Disable */
#define SCSI_MODE_CACHING_FLAG_WCE      (0x04)   /* Write Cache Enable */
#define SCSI_MODE_CACHING_FLAG2_DRA     (0x20)

typedef struct _SCSI_MODE_INFO_EXCEPTIONS_PAGE
{
    U8  PageCode;
    U8  PageLength;
    U8  Flags;
    U8  MRIE;
    U8  IntervalTimer[4];
    U8  ReportCount[4];

} SCSI_MODE_INFO_EXCEPTIONS_PAGE, *PTR_SCSI_MODE_INFO_EXCEPTIONS_PAGE;

#define SCSI_MODE_INFO_EXCEPTIONS_FLAG_LOGERR     (0x01)
#define SCSI_MODE_INFO_EXCEPTIONS_FLAG_TEST       (0x04)
#define SCSI_MODE_INFO_EXCEPTIONS_FLAG_DEXCPT     (0x08)
#define SCSI_MODE_INFO_EXCEPTIONS_FLAG_EWASC      (0x10)
#define SCSI_MODE_INFO_EXCEPTIONS_FLAG_EBF        (0x20)
#define SCSI_MODE_INFO_EXCEPTIONS_FLAG_PERF       (0x80)
#define SCSI_MODE_INFO_EXCEPTIONS_FLAG_DEXCPT_SH  (0x03)

#define SCSI_MODE_INFO_MRIE_MASK                  (0x0f)
#define SCSI_MODE_INFO_MRIE_NO_REPORTING          (0)
#define SCSI_MODE_INFO_MRIE_ASYNC                 (1)
#define SCSI_MODE_INFO_MRIE_UNIT_ATN              (2)
#define SCSI_MODE_INFO_MRIE_COND_GENERATE         (3)
#define SCSI_MODE_INFO_MRIE_UNCOND_GENERATE       (4)
#define SCSI_MODE_INFO_MRIE_GEN_NO_SENSE          (5)
#define SCSI_MODE_INFO_MRIE_REQUEST_ONLY          (6)

typedef struct _SCSI_MODE_READ_WRITE_ERROR_RECOVERY_PAGE
{
    U8  PageCode;
    U8  PageLength;
    U8  Flags;
    U8  ReadRetryCount;
    U8  CorrectionSpan;
    U8  HeadOffsetCount;
    U8  DataStrobeOffsetCount;
    U8  Reserved1;
    U8  WriteRetryCount;
    U8  Reserved2;
    U8  RecoveryTimeLimit[2];
} SCSI_MODE_READ_WRITE_ERROR_RECOVERY_PAGE, *PTR_SCSI_MODE_READ_WRITE_ERROR_RECOVERY_PAGE;

/* Read Write Error Recovery Page Flags */
#define SCSI_MODE_RWER_FLAG_DCR      (0x01)
#define SCSI_MODE_RWER_FLAG_DTE      (0x02)
#define SCSI_MODE_RWER_FLAG_PER      (0x04)
#define SCSI_MODE_RWER_FLAG_EER      (0x08)
#define SCSI_MODE_RWER_FLAG_RC       (0x10)
#define SCSI_MODE_RWER_FLAG_TB       (0x20)
#define SCSI_MODE_RWER_FLAG_ARRE     (0x40)
#define SCSI_MODE_RWER_FLAG_AWRE     (0x80)

typedef struct _SCSI_MODE_FORMAT_DEVICE_PAGE
{
    U8  PageCode;
    U8  PageLength;
    U8  TracksPerZone[2];
    U8  AlternateSectorsPerZone[2];
    U8  AlternateTracksPerZone[2];
    U8  AlternateTracksPerLogicalUnit[2];
    U8  SectorsPerTrack[2];
    U8  DataBytePerPhysicalSector[2];
    U8  Interleave[2];
    U8  TrackSkewFactor[2];
    U8  CylinderSkewFactor[2];
    U8  Flags;
    U8  Reserved[3];
} SCSI_MODE_FORMAT_DEVICE_PAGE, *PTR_SCSI_MODE_FORMAT_DEVICE_PAGE;

#define SCSI_MODE_FORMAT_DEVICE_FLAG_SURF        (0x10)
#define SCSI_MODE_FORMAT_DEVICE_FLAG_RMB         (0x20)
#define SCSI_MODE_FORMAT_DEVICE_FLAG_HSEC        (0x40)
#define SCSI_MODE_FORMAT_DEVICE_FLAG_SSEC        (0x80)

typedef struct _SCSI_MODE_RIGID_DISK_GEOMETRY_PAGE
{
    U8  PageCode;
    U8  PageLength;
    U8  NumberCylinders[3];
    U8  NumberHeads;
    U8  StartCylWritePrecomp[3];
    U8  StartCylReducedWriteCurrent[3];
    U8  DeviceStepRate[2];
    U8  LandingZoneCylinder[3];
    U8  Flags;
    U8  RotationalOffset;
    U8  Reserved1;
    U8  MediumRotationRate[2];
    U8  Reserved2[2];
} SCSI_MODE_RIGID_DISK_GEOMETRY_PAGE, *PTR_SCSI_MODE_RIGID_DISK_GEOMETRY_PAGE;

typedef struct _SCSI_MODE_CONTROL_MODE_PAGE
{
    U8  PageCode;
    U8  PageLength;
    U8  Flags;
    U8  QAMQErr;
    U8  Flags2;
    U8  Flags3;
    U8  Obselete[2];
    U8  BusyTimeoutPeriod[2];
    U8  ExtendedSelfTestCompletionTime[2];
} SCSI_MODE_CONTROL_MODE_PAGE, *PTR_SCSI_MODE_CONTROL_MODE_PAGE;

#define SCSI_MODE_CONROL_GLTSD                          (0x02)
#define SCSI_MODE_CONTROL_QAM_RESTRICTED_ORDERING       (0x00)
#define SCSI_MODE_CONTROL_QAM_UNRESTRICTED_ORDERING     (0x10)   /* 0x01 << 4 */
#define SCSI_MODE_CONTROL_QERR_ABORT_TASKS_IN_TASK_SET  (0x02)   /* 0x01 << 1 */

/******************************************************************************/
/* Format Unit Command                                                        */
/******************************************************************************/

typedef struct _SCSI_FORMAT_UNIT_CDB
{
    U8  Opcode;             /* 0x04                                           */
    U8  LunFlags;           /* Lun = [7..5], FmtData = bit 4, CmpLst = bit 3  */
                            /* Defect list format = [2..0]                    */
    U8  VendorSpecific;
    U8  Interleave1;        /*                                                */
    U8  Interleave0;        /*                                                */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_FORMAT_UNIT_CDB, *PTR_SCSI_FORMAT_UNIT_CDB;

#define SCSI_FORMAT_UNIT_FORMAT_DATA              (0x10)
#define SCSI_FORMAT_UNIT_COMPLETE_LIST            (0x08)
#define SCSI_FORMAT_UNIT_DEFECT_LIST_FORMAT_MASK  (0x07)

typedef struct _SCSI_VIRTUAL_MODE_PAGES
{
    SCSI_MODE_READ_WRITE_ERROR_RECOVERY_PAGE ReadWriteErrorRecoveryPage;
    SCSI_MODE_FORMAT_DEVICE_PAGE FormatPage;
    SCSI_MODE_RIGID_DISK_GEOMETRY_PAGE RigidDiskGeometryPage;
    SCSI_MODE_CACHING_PAGE CachingPage;
    SCSI_MODE_CONTROL_MODE_PAGE                 ControlModePage;
    SCSI_MODE_INFO_EXCEPTIONS_PAGE InfoExceptionsPage;
} SCSI_VIRTUAL_MODE_PAGES, *PTR_SCSI_VIRTUAL_MODE_PAGES;

typedef struct _SCSI_MODE_PAGE_W_BD
{
    SCSI_MODE_6_HEADER         ModeParameterHeader;
    SCSI_MODE_BLOCK_DESCRIPTOR BlockDescriptor;
    SCSI_VIRTUAL_MODE_PAGES    VirtualModePages;
} SCSI_MODE_PAGE_W_BD, *PTR_SCSI_MODE_PAGE_W_BD;

typedef struct _SCSI_MODE_PAGE_WO_BD
{
    SCSI_MODE_6_HEADER         ModeParameterHeader;
    SCSI_VIRTUAL_MODE_PAGES    VirtualModePages;
} SCSI_MODE_PAGE_WO_BD, *PTR_SCSI_MODE_PAGE_WO_BD;

typedef struct _SCSI_MODE_PAGE_W_BD_10
{
    SCSI_MODE_10_HEADER       ModeParameterHeader;
    SCSI_MODE_BLOCK_DESCRIPTOR BlockDescriptor;
    SCSI_VIRTUAL_MODE_PAGES    VirtualModePages;
}SCSI_MODE_PAGE_W_BD_10, *PTR_SCSI_MODE_PAGE_W_BD_10;

typedef struct _SCSI_MODE_PAGE_WO_BD_10
{
    SCSI_MODE_10_HEADER       ModeParameterHeader;
    SCSI_VIRTUAL_MODE_PAGES    VirtualModePages;
}SCSI_MODE_PAGE_WO_BD_10, *PTR_SCSI_MODE_PAGE_WO_BD_10;

typedef struct _SCSI_MODE_PAGE
{
    union
    {
        SCSI_MODE_PAGE_W_BD      ModePageWithBD;       /* Mode page with Block Desscriptor */
        SCSI_MODE_PAGE_WO_BD     ModePageWithoutBD;    /* Mode page without Block Descriptor */

        SCSI_MODE_PAGE_W_BD_10   ModePageWithBD10;     /* Mode page without Blk Des; has 10b CDB Parameter Header version */
        SCSI_MODE_PAGE_WO_BD_10  ModePageWithoutBD10;  /* Mode page with Blk Descpp; has 10b CDB Parameter Header version */
    } u;
} SCSI_MODE_PAGE, *PTR_SCSI_MODE_PAGE;

/******************************************************************************/
/* Reserve Command                                                            */
/******************************************************************************/

typedef struct _SCSI_RESERVE_CDB
{
    U8  Opcode;             /* 0x16                                           */
    U8  LunFlags;           /* Lun = [7..5], 3rdPty = bit 4,                  */
                            /* 3rd Party Device Id = [3..1], Extent = bit 0   */
    U8  ReservationId;      /* Reservation Identification                     */
    U8  Length1;            /* bits [15..8] Extent List Length                */
    U8  Length0;            /* bits [7..0] Extent List Length                 */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_RESERVE_CDB, *PTR_SCSI_RESERVE_CDB;

#define SCSI_RESERVE_3RD_PARTY                    (0x10)
#define SCSI_RESERVE_EXTENT                       (0x01)
#define SCSI_RESERVE_3RD_PARTY_DEVICE_ID_SHIFT    (0x01)
#define SCSI_RESERVE_3RD_PARTY_DEVICE_ID_MASK     (0x07)


/******************************************************************************/
/* Release Command                                                            */
/******************************************************************************/

typedef struct _SCSI_RELEASE_CDB
{
    U8  Opcode;             /* 0x17                                           */
    U8  LunFlags;           /* Lun = [7..5], 3rdPty = bit 4,                  */
                            /* 3rd Party Device Id = [3..1], Extent = bit 0   */
    U8  ReservationId;      /* Reservation Identification                     */
    U8  Reserved[2];
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_RELEASE_CDB, *PTR_SCSI_RELEASE_CDB;

#define SCSI_RELEASE_3RD_PARTY                    (0x10)
#define SCSI_RELEASE_EXTENT                       (0x01)
#define SCSI_RELEASE_3RD_PARTY_DEVICE_ID_SHIFT    (0x01)
#define SCSI_RELEASE_3RD_PARTY_DEVICE_ID_MASK     (0x07)


/******************************************************************************/
/* Send Diagnostic Command                                                    */
/******************************************************************************/

typedef struct _SCSI_SEND_DIAGNOSTIC_CDB
{
    U8  Opcode;             /* 0x1D                                           */
    U8  LunFlags;           /* Lun = [7..5], PF = bit 4, ST = bit 2           */
                            /* DevOfL = bit 1, UnitOfL = bit 0                */
    U8  Reserved;
    U8  Length1;            /* bits [15..8] Parameter List Length             */
    U8  Length0;            /* bits [7..0] Parameter List Length              */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_SEND_DIAGNOSTIC_CDB, *PTR_SCSI_SEND_DIAGNOSTIC_CDB;

#define SCSI_SEND_DIAGNOSTIC_SELF_TEST_CODE_MASK            (0xE0)
#define SCSI_SEND_DIAGNOSTIC_SELF_TEST_CODE_SHIFT           (0x05)
#define SCSI_SEND_DIAGNOSTIC_DEFAULT_SELF_TEST              (0x00)
#define SCSI_SEND_DIAGNOSTIC_BACKGROUND_SHORT_SELF_TEST     (0x01)
#define SCSI_SEND_DIAGNOSTIC_BACKGROUND_EXTENDED_SELF_TEST  (0x02)
#define SCSI_SEND_DIAGNOSTIC_RESERVED3                      (0x03)
#define SCSI_SEND_DIAGNOSTIC_ABORT_BACKGROUND_SELF_TEST     (0x04)
#define SCSI_SEND_DIAGNOSTIC_FOREGROUND_SHORT_SELF_TEST     (0x05)
#define SCSI_SEND_DIAGNOSTIC_FOREGROUND_EXTENDED_SELF_TEST  (0x06)
#define SCSI_SEND_DIAGNOSTIC_RESERVED7                      (0x07)
#define SCSI_SEND_DIAGNOSTIC_PAGE_FORMAT      (0x10)
#define SCSI_SEND_DIAGNOSTIC_SELF_TEST        (0x04)
#define SCSI_SEND_DIAGNOSTIC_DEV_OFF_LINE     (0x02)
#define SCSI_SEND_DIAGNOSTIC_UNIT_OFF_LINE    (0x01)


/******************************************************************************/
/* Synchronize Cache Command                                                  */
/******************************************************************************/

typedef struct _SCSI_SYNCHRONIZE_CACHE_CDB
{
    U8  Opcode;             /* 0x35                                           */
    U8  LunFlags;           /* Lun = [7..5], Immed= bit 1, RelAdr= bit 0      */
    U8  Lba3;               /* LBA bits [31..24]                              */
    U8  Lba2;               /* LBA bits [23..16]                              */
    U8  Lba1;               /* LBA bits [15..8]                               */
    U8  Lba0;               /* LBA bits [7..0]                                */
    U8  Reserved;
    U8  NumBlocks1;         /* bits [15..8] (0 == all blocks)                 */
    U8  NumBlocks0;         /* bits [7..0] (0 == all blocks)                  */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_SYNCHRONIZE_CACHE_CDB, *PTR_SCSI_SYNCHRONIZE_CACHE_CDB;

#define SCSI_SYNCHRONIZE_CACHE_IMMEDIATE      (0x02)
#define SCSI_SYNCHRONIZE_CACHE_REL_ADDR       (0x01)

/******************************************************************************/
/* Read Buffer Command                                                        */
/******************************************************************************/
typedef struct _SCSI_READ_BUFFER_CDB
{
    U8  Opcode;             /* 0x3C                                           */
    U8  LunFlags;           /* Lun = [7..5], Mode = [2..0]                    */
    U8  BufferID;           /* Buffer ID                                      */
    U8  Reserved[4];
    U8  TransferLength[2];  /* TransferLength MSB/LSB                         */
    U8  Control;            /* 0x00 - only for linked commands.               */
} SCSI_READ_BUFFER_CDB, *PTR_SCSI_READ_BUFFER_CDB;

#define SCSI_READ_BUFFER_MODE_MASK                  (0x1F)
#define SCSI_READ_BUFFER_MODE_DATA                  (0x02)
#define SCSI_READ_BUFFER_MODE_BUFFER_DESCRIPTOR     (0x03)

/******************************************************************************/
/* Read/Write Buffer descriptor format                                        */
/******************************************************************************/
typedef struct _SCSI_READ_BUFFER_DESCRIPTOR
{
    U8  OffsetBoundary;             /* A power of two                         */
    U8  BufferCapacity[3];          /* Big Endian                             */
} SCSI_READ_BUFFER_DESCRIPTOR, *PTR_SCSI_READ_BUFFER_DESCRIPTOR;

/******************************************************************************/
/* Write Buffer Command                                                       */
/******************************************************************************/
typedef struct _SCSI_WRITE_BUFFER_CDB
{
    U8  Opcode;             /* 0   - 0x3B                                     */
    U8  LunFlags;           /* 1   - Lun = [7..5], Mode = [2..0]              */
    U8  BufferID;           /* 2   - Buffer ID                                */
    U8  BufferOffset[3];    /* 3-5 - Buffer Offset                            */
    U8  TransferLength[3];  /* 6-8 - TransferLength MSB/LSB                   */
    U8  Control;            /* 9   - 0x00 - only for linked commands.         */
} SCSI_WRITE_BUFFER_CDB, *PTR_SCSI_WRITE_BUFFER_CDB;

#define SCSI_WRITE_BUFFER_MODE_MASK                     (0x1F)
#define SCSI_WRITE_BUFFER_DNLD_MICROCODE                (0x04)
#define SCSI_WRITE_BUFFER_DNLD_MICROCODE_SAVE           (0x05)
#define SCSI_WRITE_BUFFER_DNLD_MICROCODE_OFFSETS        (0x06)
#define SCSI_WRITE_BUFFER_DNLD_MICROCODE_OFFSETS_SAVE   (0x07)
/******************************************************************************/
/* Report LUNS Command                                                        */
/******************************************************************************/
typedef struct _SCSI_REPORT_LUNS_CDB
{
    U8  Opcode;                 /* 0xA0                                       */
    U8  Reserved[5];
    U8  AllocationLength[4];    /* AllocationLength MSB...LSB                 */
    U8  Reserved2;
    U8  Control;                /* 0x00 - only for linked commands.           */
} SCSI_REPORT_LUNS_CDB, *PTR_SCSI_REPORT_LUNS_CDB;

#define SCSI_REPORT_LUNS_SELECT_REPORT_ALL_LOG_UNITS            0x02
/******************************************************************************/
/* Log Sense Command                                                         */
/******************************************************************************/
typedef struct _SCSI_LOG_SENSE_CDB
{
    U8  Opcode;             /* 0x4D                                           */
    U8  LunDBD;             /* Reserved = [7..2], PPC = bit 1, SP = bit 0     */
    U8  PageCode;           /* PC = [7..6], PageCode = [5..0]                 */
    U8  Reserved[2];        /* Reserved                                       */
    U8  ParameterPtr[2];    /* Not used by LSI implementation                 */
    U8  AllocationLength[2];/* Page length                                    */
    U8  Control;            /* 0x00 - only for linked commands.               */
    U16 Pad;                /* Padding for 32 bit alignment                   */
} SCSI_LOG_SENSE_CDB, *PTR_SCSI_LOG_SENSE_CDB;

#define SCSI_LOG_SENSE_SAVE_PARAMETERS                          0x01
#define SCSI_LOG_SENSE_PPC                                      0x02
#define SCSI_LOG_SENSE_PAGE_CODE_MASK                           0x3F
#define SCSI_LOG_SENSE_PAGE_CODE_SHIFT                          0x06
#define SCSI_LOG_SENSE_CURRENT_VALUES                           0x00
#define SCSI_LOG_SENSE_CUMULATIVE_VALUES                        0x01
#define SCSI_LOG_SENSE_PC_SHIFT                                 0x06
#define SCSI_LOG_SENSE_PC_MASK                                  0xC0
#define SCSI_LOG_SENSE_PC_CURRENT                               0x01

#define SCSI_LOG_SENSE_SMART_PAGE                               0x31
#define SCSI_LOG_SENSE_INFORMATIONAL_EXCEPTIONS_PAGE            0x2F
#define SCSI_LOG_SENSE_SELF_TEST_LOG_PAGE                       0x10
#define SCSI_LOG_SENSE_SUPPORTED_PAGES_PAGE                     0x00
/******************************************************************************/
/* Persistent Reserve Out Command                                             */
/******************************************************************************/
typedef struct _SCSI_PERSISTENT_RESERVE_OUT_CDB
{
    U8  OpCode;
    U8  ServiceAction;
    U8  Scope_Type;
    U8  Reserved[2];
    U8  ParameterListLength[4];
    U8  Control;
}SCSI_PERSISTENT_RESERVE_OUT_CDB, *PTR_SCSI_PERSISTENT_RESERVE_OUT_CDB;

#define SCSI_PRO_SERVICE_CODE_REGISTER                          0x00
#define SCSI_PRO_SERVICE_CODE_RESERVE                           0x01
#define SCSI_PRO_SERVICE_CODE_RELEASE                           0x02
#define SCSI_PRO_SERVICE_CODE_CLEAR                             0x03
#define SCSI_PRO_SERVICE_CODE_PREMPT                            0x04
#define SCSI_PRO_SERVICE_CODE_PRESCRUTINY_AND_ABORT                  0x05
#define SCSI_PRO_SERVICE_CODE_REGISTER_AND_IGNOR_EXISTING_KEY   0x06
#define SCSI_PRO_SERVICE_CODE_REGISTER_AND_MOVE                 0x07

typedef struct _SCSI_PERS_RESERVE_OUT_PARAM_LIST
{
    U8  ReservationKey[8];
    U8  ServiceActionReservationKey[8];
    U8  Obsolete16to19[4];
    U8  Flags;
    U8  Reserved;
    U8  Obsolete22to23[2];
}SCSI_PERS_RESERVE_OUT_PARAM_LIST, *PTR_SCSI_PERS_RESERVE_OUT_PARAM_LIST;

#define SCSI_PRO_PARAM_LIST_SPEC_I_PT                           0x08
#define SCSI_PRO_PARAM_LIST_ALL_TG_PT                           0x04
#define SCSI_PRO_PARAM_LIST_APTPL                               0x01

/******************************************************************************/
/* ATA Passthrough                                                            */
/******************************************************************************/

typedef struct _SCSI_ATA_PASSTHROUGH_12
{
    U8      OpCode;
    U8      MulCnt_Protocol;
    U8      Flags;
    U8      Features;
    U8      SectorCount;
    U8      LBALow;
    U8      LBAMid;
    U8      LBAHigh;
    U8      Device;
    U8      Command;
    U8      Reserved;
    U8      Conrol;

}SCSI_ATA_PASSTHROUGH_12, *PTR_SCSI_ATA_PASSTHROUGH_12;

typedef struct _SCSI_ATA_PASSTHROUGH_16
{
    U8      OpCode;
    U8      MulCnt_Protocol_Ext;
    U8      Flags;
    U8      FeaturesExt;
    U8      Features;
    U8      SectorCountExt;
    U8      SectorCount;
    U8      LBALowExt;
    U8      LBALow;
    U8      LBAMidExt;
    U8      LBAMid;
    U8      LBAHighExt;
    U8      LBAHigh;
    U8      Device;
    U8      Command;
    U8      Control;

}SCSI_ATA_PASSTHROUGH_16, *PTR_SCSI_ATA_PASSTHROUGH_16;

#define     ATA_PASSTHRU_MULTIPLE_COUNT         (0xE0)
#define     ATA_PASSTHRU_PROTOCOL               (0x1E)
#define     ATA_PASSTHRU_EXT                    (0x01)
#define     ATA_PASSTHRU_MULTIPLE_COUNT_SHIFT   (0x05)
#define     ATA_PASSTHRU_PROTOCOL_SHIFT         (0x01)


#define     ATA_PASSTHRU_OFFLINE                (0xC0)
#define     ATA_PASSTHRU_CK_COND                (0x20)
#define     ATA_PASSTHRU_T_DIR                  (0x08)
#define     ATA_PASSTHRU_BYTE_BLOCK             (0x04)
#define     ATA_PASSTHRU_T_LENGTH               (0x03)
#define     ATA_PASSTHRU_CK_COND_SHIFT          (0x05)

#define     ATA_PASSTHRU_PROTOCOL_HARD_RESET    (0x00)
#define     ATA_PASSTHRU_PROTOCOL_SRST          (0x01)
#define     ATA_PASSTHRU_PROTOCOL_RESERVED_2    (0x02)
#define     ATA_PASSTHRU_PROTOCOL_NON_DATA      (0x03)
#define     ATA_PASSTHRU_PROTOCOL_PIO_IN        (0x04)
#define     ATA_PASSTHRU_PROTOCOL_PIO_OUT       (0x05)
#define     ATA_PASSTHRU_PROTOCOL_DMA           (0x06)
#define     ATA_PASSTHRU_PROTOCOL_DMA_QUEUED    (0x07)
#define     ATA_PASSTHRU_PROTOCOL_DEV_DIAG      (0x08)
#define     ATA_PASSTHRU_PROTOCOL_DEV_RESET     (0x09)
#define     ATA_PASSTHRU_PROTOCOL_UDMA_IN       (0x0A)
#define     ATA_PASSTHRU_PROTOCOL_UDMA_OUT      (0x0B)
#define     ATA_PASSTHRU_PROTOCOL_FPDMA         (0x0C)
#define     ATA_PASSTHRU_PROTOCOL_RESERVED_13   (0x0D)
#define     ATA_PASSTHRU_PROTOCOL_RESERVED_14   (0x0E)
#define     ATA_PASSTHRU_PROTOCOL_RET_RESP      (0x0F)

#define     ATA_PASSHTRU_TLEN_NO_DATA           (0x00)
#define     ATA_PASSTHRU_TLEN_IN_FEATURES       (0x01)
#define     ATA_PASSHTRU_TLEN_IN_SECTOR_CNT     (0x02)
#define     ATA_PASSHTRU_TLEN_IN_STPSIU         (0x03)


/******************************************************************************/
/*                                                                            */
/*      S C S I   D A T A   S t r u c t u r e s                               */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Inquiry DATA                                                               */
/******************************************************************************/

typedef struct _MPI_SCSI_INQUIRY_DATA
{
    U8   DeviceType;               /* 0x00 */
    U8   DeviceTypeModifier;       /* 0x01 */
    U8   Versions;                 /* 0x02 */
    U8   DataFormat;               /* 0x03 */
    U8   AdditionalLength;         /* 0x04 */
    U8   Reserved[ 2 ];            /* 0x05-0x06 */
    U8   CapabilityBits;           /* 0x07 */
    char VendorId[ 8 ];            /* 0x08-0x0f */
    char ProductId[ 16 ];          /* 0x10-0x1f */
    char ProductRevision[ 4 ];     /* 0x20-0x23 */
    char VendorUnique[ 20 ];       /* 0x24-0x37 */
    U8   Clk_Qas_IUS;              /* 0x38 */
    U8   Reserved_39;              /* 0x39 */
    U16  VersionDescriptor[8];     /* 0x3A - 0x49 */
} MPI_SCSI_INQUIRY_DATA, *PTR_MPI_SCSI_INQUIRY_DATA;

#define SCSI_INQUIRY_DEV_TYPE_DIRECT_ACCESS             (0)
#define SCSI_INQUIRY_DEV_TYPE_PROCESSOR                 (0x03)
#define SCSI_INQUIRY_DEV_TYPE_UNKNOWN                   (0x1f)

#define SCSI_INQUIRY_DEV_TYPE_MASK                      (0x1f)
#define SCSI_INQUIRY_PERIPHERAL_QUALIFIER_MASK          (0xe0)
#define SCSI_INQUIRY_PERIPHERAL_QUALIFIER_SHIFT         (5)
#define SCSI_INQUIRY_PERIPHERAL_QUALIFIER_CONNECTED     (0)
#define SCSI_INQUIRY_PERIPHERAL_QUALIFIER_NOT_SUPPORTED (0x60)


/*  The SCSI_INQUIRY_DTM_REMOVABLE bit is defined for the DeviceTypeModifier
 *  field of the INQUIRY_DATA structure.  When this bit is set, the device is
 *  a removable media device.
 */

#define SCSI_INQUIRY_DTM_REMOVABLE (0x80)

/*  The SCSI_INQUIRY_VERSION bits are defined for the Versions
 *  field of the INQUIRY_DATA structure.  The mask maintains compatibility
 *  with pre-SCSI-3 versions.
 */
#define SCSI_INQUIRY_VERSION_MASK               (0x07)
#define SCSI_INQUIRY_VERSION_UNDEFINED          (0x00)
#define SCSI_INQUIRY_VERSION_SCSI_1             (0x01)
#define SCSI_INQUIRY_VERSION_SCSI_2             (0x02)
#define SCSI_INQUIRY_VERSION_SCSI_3_SPC         (0x03)
#define SCSI_INQUIRY_VERSION_SCSI_3_SPC2        (0x04)
#define SCSI_INQUIRY_VERSION_SCSI_3_SPC3        (0x05)

/*  The SCSI_INQUIRY_CB bits are defined for the CapabilityBits field of the
 *  SCSI_INQUIRY_DATA structure.  For each of these bits, the condition is
 *  TRUE or ALLOWED when the bit is set.
 */


#define SCSI_INQUIRY_HISUP                  (0x10)

#define SCSI_INQUIRY_CB_RELATIVE_ADDRESSING (0x80)
#define SCSI_INQUIRY_CB_WIDE                (0x60)
#define SCSI_INQUIRY_CB_WIDE_32             (0x40)
#define SCSI_INQUIRY_CB_WIDE_16             (0x20)

#define SCSI_INQUIRY_CB_SYNCHRONOUS         (0x10)
#define SCSI_INQUIRY_CB_LINKED_COMMAND      (0x08)
#define SCSI_INQUIRY_CB_COMMAND_QUEUING     (0x02)
#define SCSI_INQUIRY_CB_SOFT_RESET          (0x01)

/* SCSI Inquiry data - Version Descriptor Values */
#define SCSI_INQUIRY_VD_SAM_NO_VER                  (0x0020)
#define SCSI_INQUIRY_VD_SAM_ANSI_X3_270             (0x003C)
#define SCSI_INQUIRY_VD_SAM_T10_0994_D_REV18        (0x003B)
#define SCSI_INQUIRY_VD_SAM_2_NO_VERSION            (0x0040)
#define SCSI_INQUIRY_VD_SAM_2_ANSI_INCITS_366       (0x005C)
#define SCSI_INQUIRY_VD_SAM_2_T10_1157D_REV24       (0x0055)
#define SCSI_INQUIRY_VD_SAM_2_T10_1157D_REV23       (0x0054)
#define SCSI_INQUIRY_VD_SAM_3_NO_VER                (0x0060)
#define SCSI_INQUIRY_VD_SAM_3_T10_1561D_REV14       (0x0076)
#define SCSI_INQUIRY_VD_SAM_3_ANSI_INCITS_402_2005  (0x0077)
#define SCSI_INQUIRY_VD_SAM_3_T10_1561D_REV7        (0x0062)
#define SCSI_INQUIRY_VD_SAM_3_T10_1561D_REV13       (0x0075)
#define SCSI_INQUIRY_VD_SAM_4_NO_VERSION            (0x0080)

#define SCSI_INQUIRY_VD_SAT_NO_VERSION              (0x1EA0)

#define SCSI_INQUIRY_VD_SPC_NO_VER                  (0x0120)
#define SCSI_INQUIRY_VD_SPC_ANSI_X3_301             (0x013C)
#define SCSI_INQUIRY_VD_SPC_T10_0995D_REV11A        (0x013B)
#define SCSI_INQUIRY_VD_SPC_2_NO_VER                (0x0260)
#define SCSI_INQUIRY_VD_SPC__2_ANSI_NCITS_351       (0x0277)
#define SCSI_INQUIRY_VD_SPC_2_T10_1236D_REV20       (0x0276)
#define SCSI_INQUIRY_VD_SPC_2_T10_1236D_REV12       (0x0267)
#define SCSI_INQUIRY_VD_SPC_2_T10_1236D_REV18       (0x0269)
#define SCSI_INQUIRY_VD_SPC_2_T10_1236D_REV19       (0x0275)
#define SCSI_INQUIRY_VD_SPC_3_T10_NO_VER            (0x0300)
#define SCSI_INQUIRY_VD_SPC_3_T10_1416D_REV7        (0x0301)
#define SCSI_INQUIRY_VD_SPC_3_T10_1416D_REV21       (0x0307)
#define SCSI_INQUIRY_VD_SPC_4_NO_VER                (0x0460)

#define SCSI_INQUIRY_VD_SBC_NO_VER              (0x0180)
#define SCSI_INQUIRY_VD_SBC_T10_0996D_REV08C    (0x019B)
#define SCSI_INQUIRY_VD_SBC_2_NO_VER            (0x0320)
#define SCSI_INQUIRY_VD_SBC_2_T10_1417D_REV5A   (0x0322)
#define SCSI_INQUIRY_VD_SBC_2_T10_1417D_REV15   (0x0324)

#define SCSI_INQUIRY_VD_SAS_NO_VER              (0x0BE0)
#define SCSI_INQUIRY_VD_SAS_ANSI_INCITS_376     (0x0BFD)
#define SCSI_INQUIRY_VD_SAS_1_1_NO_VER          (0x0C00)

#define SCSI_INQUIRY_VD_ATA_PI_7_NO_VER         (0x1600)
#define SCSI_INQUIRY_VD_ATA_PI_7_T13_1532D_REV3 (0x1602)

/******************************************************************************/
/* Read Capacity DATA returned from target                                    */
/******************************************************************************/

typedef struct _SCSI_READ_CAPACITY_DATA
{
    U32  LastLBA;      /* Big-endian */
    U32  SectorSize;   /* Big-endian */
} SCSI_READ_CAPACITY_DATA, * PTR_SCSI_READ_CAPACITY_DATA;

typedef struct _SCSI_READ_CAPACITY_DATA_16
{
    U32  LastLBAHigh;  /* Big-endian 0-3 */
    U32  LastLBALow;   /* Big-endian 4-7 */
    U32  SectorSize;   /* Big-endian 8-11*/
    U8   RsvdRtoProt;  /* reserved + RTO_EN + PROT_EN == 0 (for now) 12*/
    U8   Rsvd[19];     /* 13 - 31 */
} SCSI_READ_CAPACITY_DATA_16, * PTR_SCSI_READ_CAPACITY_DATA_16;

/******************************************************************************/
/* Report LUNS DATA returned from target                                    */
/******************************************************************************/
#define SCSI_MAX_LUNS    1

typedef struct _LUN_LEVELS
{
    U16  LunLevel3;
    U16  LunLevel2;
    U16  LunLevel1;
    U16  LunLevel0;
}LUN_LEVELS, *PTR_LUN_LEVELS;

typedef struct _SCSI_REPORT_LUNS_DATA
{
    U32  LUNListLength;             /* Big-endian */
    U32  Reserved;
    //U8   LUN[SCSI_MAX_LUNS][8];     /* Big-endian */
    LUN_LEVELS  LUN[SCSI_MAX_LUNS];
} SCSI_REPORT_LUNS_DATA, * PTR_SCSI_REPORT_LUNS_DATA;

/******************************************************************************/
/* Read Media Serial Number parameter data                                    */
/******************************************************************************/
typedef struct _READ_MEDIA_SERIAL_NUMBER_DATA
{
    U32 MediaSerialNumberLength;    /* Big-endian */
    U8  MediaSerialNumber[60];      /* Vendor Specific */
}READ_MEDIA_SERIAL_NUMBER_DATA, *PTR_READ_MEDIA_SERIAL_NUMBER_DATA;



/******************************************************************************/
/* Unit Serial Number Page DATA                                               */
/******************************************************************************/

typedef struct _SCSI_UNIT_SERIAL_NUMBER_PAGE
{
    U8  PQualDevType;       /* 0x00 Peripheral qualifier and device type      */
    U8  PageCode;           /* 0x80 = Unit Serial Number Page                 */
    U8  Reserved;           /* 0x00                                           */
    U8  PageLength;         /* (n-3, number of bytes in serial number) == 20  */
    U8  SerialNumber[20];   /* Serial Number in ASCII - any length
                             * If the serial number is not available, then this
                             * field may be filled with spaces. Note ATA has
                             * a 20 byte serial number. If we use this structure
                             * elsewhere (like IR), we may want to change this
                             */
} SCSI_UNIT_SERIAL_NUMBER_PAGE, *PTR_SCSI_UNIT_SERIAL_NUMBER_PAGE;

/******************************************************************************/
/* Supported Vital Product Data Pages Page DATA                                               */
/******************************************************************************/

typedef struct _SCSI_SUPPORTED_VPD_PAGES
{
    U8  PQualDevType;       /* 0x00 Peripheral qualifier and device type      */
    U8  PageCode;           /* 0x00 = Unit Serial Number Page                 */
    U8  Reserved;           /* 0x00                                           */
    U8  PageLength;         /* (n-3, number of supported pages) <= 16  */
    U8  SupportedPages[6];  /* Serial Number in ASCII - any length
                             * If the serial number is not available, then this
                             * field may be filled with spaces. Note the ATA
                             * emulate code only supports two pages. Change this
                             * if ever used elsewhere
                             */
} SCSI_SUPPORTED_VPD_PAGES, *PTR_SCSI_SUPPORTED_VPD_PAGES;

/******************************************************************************/
/* Supported Vital Product Data Pages - Mode Page Policy Page                 */
/******************************************************************************/
typedef struct _MODE_PAGE_POLICY_DESCRIPTOR
{
    U8  PolicyPageCode;
    U8  PolicySubPageCode;
    U8  Policy;
    U8  Reserved;

}MODE_PAGE_POLICY_DESCRIPTOR, *PTR_MODE_PAGE_POLICY_DESCRIPTOR;

#define MODE_PAGE_POLICY_SHARED             (0x00)
#define MODE_PAGE_POLICY_PER_TARGET_PORT    (0x01)
#define MODE_PAGE_POLICY_PER_INITIATOR_PORT (0x02)
#define MODE_PAGE_POLICY_PER_I_T_NEXUS      (0x03)

typedef struct _SCSI_MODE_PAGE_POLICY
{
    U8                              PQualDevType;
    U8                              PageCode;
    U16                             PageLength;
    MODE_PAGE_POLICY_DESCRIPTOR     PolicyDescriptor;

}SCSI_MODE_PAGE_POLICY, *PTR_SCSI_MODE_PAGE_POLICY;

/******************************************************************************/
/* Supported Vital Product Data Pages - ATA Information Page                  */
/******************************************************************************/
typedef struct _SCSI_ATA_INFO_PAGE
{
    U8  PQualDevType;        /* 0x00 Peripheral qualifier and Device type           */
    U8  PageCode;            /* 0x89 = ATA Information(Identify) Page               */
    U16 PageLength;          /* The ATA info structure is 512 bytes + 56 add. bytes */
    U32 Reserved4to7;
    U8  SATVendorId[8];     /* SAT Vendor Identification   = "LSI"                  */
    U8  SATProductId[16];   /* SAT Product Identification                           */
    U8  SATRevision[4];
    U8  Signature[20];
    U8  CommandCode;
    U8  Reserved57to59[3];
    U16 ATAIdentifyData[256];/* The ATA info Structure                              */

}SCSI_ATA_INFO_PAGE, *PTR_SCSI_ATA_INFO_PAGE;

/******************************************************************************/
/* Supported Vital Product Data Pages - Device Identification                 */
/******************************************************************************/
typedef struct _SCSI_VPD_PAGE_HEADER
{
    U8  PQualDevType;
    U8  PageCode;
    U16 PageLength;

}SCSI_VPD_PAGE_HEADER, *PTR_SCSI_VPD_PAGE_HEADER;

/******************************************************************************/
/* Supported Vital Product Data Pages - Block Device Characteristics          */
/******************************************************************************/

typedef struct _SCSI_BLOCK_DEVICE_CHAR_PAGE
{
    U8  PQualDevType;       /* 0x00 Peripheral qualifier and device type      */
    U8  PageCode;           /* 0xB1 = Block Device Characteristics page       */
    U8  Reserved2;          /* 0x00                                           */
    U8  PageLength;         /* 0x3C                                           */
    U16 MediumRotationRate; /* ATA IDENTIFY DEVICE data word 217.
                             * Dword 217 value            Description
                             * ---------------            -----------
                             * 0x0000                     Rate not reported
                             * 0x0001                     Non-rotating Media
                             * 0x0002 - 0x0040            Reserved
                             * 0x0401 - 0xFFFE            Nominal media rotation rate in RPMs
                             * 0xFFFF                     Reserved
                             */
    U8 Reserved6;           /* 0x00 */
    U8 NominalFormFactor;   /* ATA IDENTIFY DEVICE data word 168 bits 3:0 */
    U8 Reserved8to63[56];
} SCSI_BLOCK_DEVICE_CHAR_PAGE, *PTR_SCSI_BLOCK_DEVICE_CHAR_PAGE;

typedef struct _SCSI_DEV_ID_DESCRIPTOR_HEADER
{
    U8  ProtID_CodeSet;
    U8  PIV_ASSC_IDType;
    U8  Reserved;
    U8  IDLength;
}SCSI_DEV_ID_DESCRIPTOR_HEADER, *PTR_SCSI_DEV_ID_DESCRIPTOR_HEADER;

#define SCSI_DEV_ID_CODESET_BINARY              (0x01)
#define SCSI_DEV_ID_CODESET_ASCII               (0x02)
#define SCSI_DEV_ID_CODESET_UTF8                (0x03)
#define SCSI_DEV_ID_PID_SHIFT                   (0x04)

#define SCSI_DEV_ID_ASSC_LOGICAL_UNIT           (0x00)
#define SCSI_DEV_ID_ASSC_ADDRESSED_TARG_PORT    (0x01)
#define SCSI_DEV_ID_ASSC_SCSI_DEVICE            (0x02)
#define SCSI_DEV_ID_ASSC_SHIFT                  (0x04)

#define SCSI_DEV_ID_PROT_ID_VALID               (0x80)


#define SCSI_DEV_ID_IDTYPE_VENDOR_SPECIFIC      (0x00)
#define SCSI_DEV_ID_IDTYPE_T10_VENDOR_ID        (0x01)
#define SCSI_DEV_ID_IDTYPE_EUI_64               (0x02)
#define SCSI_DEV_ID_IDTYPE_NAA                  (0x03)
#define SCSI_DEV_ID_IDTYPE_REL_TARG_PORT_ID     (0x04)
#define SCSI_DEV_ID_IDTYPE_TARGET_PORT_GROUP    (0x05)
#define SCSI_DEV_ID_IDTYPE_LOGICAL_UNIT_GROUP   (0x06)
#define SCSI_DEV_ID_IDTYPE_MD5_LOGICAL_UNIT_ID  (0x07)
#define SCSI_DEV_ID_IDTYPE_SCSI_NAME_STRING     (0x08)

typedef struct _SCSI_DEVICE_ID_VENDOR_IDENTIFIER
{
    SCSI_DEV_ID_DESCRIPTOR_HEADER       DescriptorHeader;
    U8                                  VendorIdentification[8];
    U8                                  VenSpecId[60];           /* Length is actually arbitrary */
}SCSI_DEVICE_ID_VENDOR_IDENTIFIER, *PTR_SCSI_DEVICE_ID_VENDOR_IDENTIFIER;

typedef struct _SCSI_DEVICE_ID_NAA_IDENTIFIER_EXTENDED
{
    SCSI_DEV_ID_DESCRIPTOR_HEADER       DescriptorHeader;
    U8                                  NAA_VendorSpecificId_A_3_0;
    U8                                  VendorSpecificIdentifierA;
    U8                                  IEEE_CompanyId[3];
    U8                                  VendorSpecificIdentifier_B[3];
}SCSI_DEVICE_ID_NAA_IDENTIFIER_EXTENDED, *PTR_SCSI_DEVICE_ID_NAA_IDENTIFIER_EXTENDED;

typedef struct _SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_IDENTIFIER
{
    SCSI_DEV_ID_DESCRIPTOR_HEADER       DescriptorHeader;
    U8                                  NAA_IEEE_CompanyId_3_0;     /* NAA [7:4], IEEE Company ID [3:0]                 */
    U8                                  IEEE_CompanyId[2];
    U8                                  IEEE_CId_7_4_VSI_3_0;       /* IEEE Company ID [7:4], Vendor Specific Id [3:0]  */
    U8                                  VenSpecId[4];
}SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_IDENTIFIER,*PTR_SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_IDENTIFIER;

typedef struct _SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_EXTENDED_IDENTIFIER
{
    SCSI_DEV_ID_DESCRIPTOR_HEADER       DescriptorHeader;
    U8                                  NAA_IEEE_CompanyId_3_0;     /* NAA [7:4], IEEE Company ID [3:0]                 */
    U8                                  IEEE_CompanyId[2];
    U8                                  IEEE_CId_7_4_VSI_3_0;       /* IEEE Company ID [7:4], Vendor Specific Id [3:0]  */
    U8                                  VenSpecId[4];
    U8                                  VenSpecIdExtension[8];
}SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_EXTENDED_IDENTIFIER,
 *PTR_SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_EXTENDED_IDENTIFIER;


#define NAA_IEEE_EXTENDED                   (0x02)
#define NAA_IEEE_REGISTERED                 (0x05)
#define NAA_IEEE_REGISTERED_EXTENDED        (0x06)

#define NAA_SHIFT                           (0x04)

typedef struct _SCSI_DEVICE_ID_REL_TARGET_PORT_IDENTIFIER
{
    SCSI_DEV_ID_DESCRIPTOR_HEADER       DescriptorHeader;
    U16                                 Reserved;           /* This field is obsolete                   */
    U16                                 RelTargetPortId;
}SCSI_DEVICE_ID_REL_TARGET_PORT_IDENTIFIER, *PTR_SCSI_DEVICE_ID_REL_TARGET_PORT_IDENTIFIER;


typedef struct _SCSI_DEVICE_IDENTIFICATION_PAGE
{
    SCSI_VPD_PAGE_HEADER             PageHeader;

    union
    {
        SCSI_DEVICE_ID_VENDOR_IDENTIFIER                        VendorSpecificIdentifier;
        SCSI_DEVICE_ID_NAA_IDENTIFIER_EXTENDED                  NAA_ID_Extended;
        SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_IDENTIFIER           NAA_ID_Registered;
        SCSI_DEVICE_ID_NAA_IEEE_REGISTERED_EXTENDED_IDENTIFIER  NAA_ID_Reg_Extended;
        SCSI_DEVICE_ID_REL_TARGET_PORT_IDENTIFIER               RelTargPortId;
    }Identifier_1_Type;

}SCSI_DEVICE_IDENTIFICATION_PAGE,*PTR_SCSI_DEVICE_IDENTIFICATION_PAGE;

/******************************************************************************/
/* SAF-TE DATA                                                                */
/******************************************************************************/

typedef struct _SAFTE_READ_ENC_CONFIG_DATA
{
    U8   NumberFans;
    U8   NumberPowerSupplies;
    U8   NumberDeviceSlots;
    U8   DoorLockInstalled;
    U8   NumberTempSensors;
    U8   AudibleAlarmInstalled;
    U8   NumberThermostats;
    U8   Reserved[62-7+1];
    U8   NumberVendorSpecificBytes;
} SAFTE_READ_ENC_CONFIG_DATA, *PTR_SAFTE_READ_ENC_CONFIG_DATA;

#define SCSI_READ_WRITE_BUFFER_MODE_SAFTE     (0x01)

/* The following are values that are put into the BufferID field for SAF-TE */
#define SAFTE_READ_ENCLOSURE_CONFIGURATION     (0x0)
#define SAFTE_READ_ENCLOSURE_STATUS            (0x1)
#define SAFTE_WRITE_DEVICE_SLOT_STATUS         (0x10)
#define SAFTE_PERFORM_SLOT_OPERATION           (0x12)

#define SAFTE_PERFORM_SLOT_OPERATION_NONE                             (0x00)
#define SAFTE_PERFORM_SLOT_OPERATION_PREPARE_FOR_OPERATION            (0x01)
#define SAFTE_PERFORM_SLOT_OPERATION_PREPARE_FOR_INSERTION_OR_REMOVAL (0x02)
#define SAFTE_PERFORM_SLOT_OPERATION_IDENTIFY                         (0x04)

/******************************************************************************/
/* Sense DATA                                                                 */
/******************************************************************************/

/* Fixed Format Sense Data */
typedef struct _SCSI_SENSE_DATA
{
    U8  ErrorCode;                          /* 0x00 */
    U8  SegmentNumber;                      /* 0x01 */
    U8  SenseKey;                           /* 0x02 */
    U8  Information[4];                     /* 0x03 - 0x06 */
    U8  AdditionalLength;                   /* 0x07 */
    U8  CommandSpecific[4];                 /* 0x08 - 0x0B */
    U8  ASC;                                /* 0x0C */
    U8  ASCQ;                               /* 0x0D */
    U8  FieldReplaceableUnitCode;           /* 0x0E */
    U8  SenseKeySpecific[3];                /* 0x0F - 0x11 */
    U8  AdditionalSenseBytes[18];           /* 0x12 */ /* Defined to pad out to 36 bytes total */
} SCSI_SENSE_DATA, *PTR_SCSI_SENSE_DATA;

#define REQUEST_SENSE_BYTE_DESCRIPTOR_FORMAT_MASK                       (0x00000001)

/* SENSE_DATA keys */
#define SCSI_SENSE_KEY_NO_SENSE                 (0x00)
#define SCSI_SENSE_KEY_RECOVERED_ERROR          (0x01)
#define SCSI_SENSE_KEY_NOT_READY                (0x02)
#define SCSI_SENSE_KEY_MEDIUM_ERROR             (0x03)
#define SCSI_SENSE_KEY_HARDWARE_ERROR           (0x04)
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST          (0x05)
#define SCSI_SENSE_KEY_UNIT_ATTENTION           (0x06)
#define SCSI_SENSE_KEY_DATA_PROTECT             (0x07)
#define SCSI_SENSE_KEY_BLANK_CHECK              (0x08)
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC          (0x09)
#define SCSI_SENSE_KEY_COPY_ABORTED             (0x0A)
#define SCSI_SENSE_KEY_ABORTED_COMMAND          (0x0B)
#define SCSI_SENSE_KEY_RESERVED_C               (0x0C)
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW          (0x0D)
#define SCSI_SENSE_KEY_MISCOMPARE               (0x0E)
#define SCSI_SENSE_KEY_RESERVED_F               (0x0F)
#define SCSI_SENSE_KEY_MASK                     (0x0F)

#define SCSI_SENSE_ILI                          (0x20)

/* SENSE_DATA ASC/ASCQ codes */
#define SCSI_NO_ADDITIONAL_SENSE_INFORMATION                (0x0000)
#define SCSI_ATA_PASSTHROUGH_INFORMATION_AVAILABLE            (0x001D)
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY                     (0x0400)
#define SCSI_ASC_LOGICAL_UNIT_IN_PROCESS_BECOMING_READY     (0x0401)
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY_INIT_REQUIRED       (0x0402)
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY_FORMAT_IN_PROGRESS  (0x0404)
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY_SELFTEST_IN_PROGRSS   (0x0409)
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY_SPINUP_REQUIRED     (0x0411)
#define SCSI_ASC_LOGICAL_UNIT_DOES_NOT_RESPOND_TO_SELECTION   (0x0500)
#define SCSI_ASC_UNRECOVERED_READ_ERROR                     (0x1100)
#define SCSI_ASC_UNRECOVERED_READ_ERROR_AUTO_REALLOCATE_FAIL  (0x1104)
#define SCSI_ASC_RECORD_NOT_FOUND                           (0x1401)
#define SCSI_ASC_PARAMETER_LIST_LENGTH_ERROR                  (0x1A00)
#define SCSI_ASC_INVALID_COMMAND_OP_CODE                    (0x2000)
#define SCSI_ASC_LOGICAL_BLOCK_OUT_OF_RANGE                 (0x2100)
#define SCSI_ASC_INVALID_FIELD_IN_CDB                       (0x2400)
#define SCSI_ASC_LOGICAL_UNIT_NOT_SUPPORTED                 (0x2500)
#define SCSI_ASC_INVALID_FIELD_IN_PARAM_LIST                (0x2600)
#define SCSI_ASC_WRITE_PROTECTED                            (0x2700)
#define SCSI_ASC_MEDIUM_MAY_HAVE_CHANGED                    (0x2800)
#define SCSI_ASC_POWER_ON_RESET_OR_BDR                      (0x2900)
#define SCSI_ASC_COMMAND_SEQUENCE_ERROR                     (0x2C00)
#define SCSI_ASC_FORMAT_COMMAND_FAILED                        (0x3101)
#define SCSI_ASC_MEDIUM_NOT_PRESENT                         (0x3A00)
#define SCSI_ASC_LOGICAL_UNIT_FAILURE                         (0x3E01)
#define SCSI_ASC_LOGICAL_UNIT_FAILED_SELFTEST                 (0x3E03)
#define SCSI_ASC_LOGICAL_UNUIT_UNABLE_TO_UPDATE_SELF_TEST_LOG (0x3E04)
#define SCSI_ASC_DIAGNOSTIC_FAILURE_ON_COMPONENT_NN           (0x4000)
#define SCSI_ASC_INTERNAL_TARGET_FAILURE                    (0x4400)
#define SCSI_ASC_INFORMATION_UNIT_IU_CRC_ERROR_DETECTED     (0x4703)
#define SCSI_ASC_PROTOCOL_SERVICE_CRC_ERROR                 (0x4705)
#define SCSI_ASC_LOGICAL_UNIT_FAILED_SELF_CONFIGURATION       (0x4C00)
#define SCSI_ASC_OPERATOR_MEDIUM_REMOVAL                    (0x5A01)
#define SCSI_ASC_FAIL_PRED_THRESH_EXCEEDED                  (0x5D10)
#define SCSI_ASC_LOW_POWER_CONDITION_ON                     (0x5E00)
#define SCSI_ASC_MEDIA_LOAD_OR_EJECT_FAILED                 (0x5300)
#define SCSI_ASC_ATA_DEVICE_FEATURE_NOT_ENABLED               (0x670B)

//adding it for PMR drives - need to check whether firmware latest scsi.h has to be ported or not
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY_ADDITIONAL_POWER_USE_NOT_YET_GRANTED            (0x041C)



/* Sense Key Specific Valid bit in byte 15 of sense data */
#define SCSI_SENSE_KEY_SPECIFIC_VALID                       (0x80)

/* Sense data error type for fixed format sense buffer*/
#define SCSI_SENSE_DATA_RESPONSE_CODE_CURRENT               (0x70)
#define SCSI_SENSE_DATA_RESPONSE_CODE_DEFERRED              (0x71)
/*CQ#98932, LUN ER:SCSI sense data's byte 0(Error Code) contains
Valid bit (7th bit) + Response Code (6th-0th bit)
where
Valid(7th bit)
0 -Invalid sense data
1 -valid sense data
Response Code (6th-0th bit)
70h- Current Errors
71h- Deferred Errors

Thus valid current error is 0xF0*/

/* Sense data error type for descriptor format sense buffer*/
#define SCSI_SENSE_RESPONSE_CODE_MASK                            (0x7F)
#define SCSI_SENSE_ERROR_VALID_BIT                               (0x80)
#define SCSI_SENSE_DESC_DATA_RESPONSE_CODE_CURRENT               (0x72)
#define SCSI_SENSE_DESC_DATA_RESPONSE_CODE_DEFERRED              (0x73)


#define SCSI_SENSE_DATA_RESPONSE_CODE_VALID_CURRENT_ERROR   (0xF0)//CQ#98932, LUN ER

/* Descriptor Format Sense Data */
typedef struct _SCSI_DESC_FMT_SENSE_DATA
{
    U8  ResponseCode;
    U8  SenseKey;
    U8  ASC;
    U8  ASCQ;
    U8  Reserved[3];
    U8  AdditionalLength;
    U8  Descriptor[1];
}SCSI_DESC_FMT_SENSE_DATA, *PTR_SCSI_DESC_FMT_SENSE_DATA;


/* ATA Status Return Descriptor */
typedef struct _ATA_STATUS_RETURN_DESCRIPTOR
{
    U8  DescriptorCode;
    U8  AdditionalLength;
    U8  Extend;
    U8  Error;
    U8  SectorCountExt;
    U8  SectorCount;
    U8  LbaLowExt;
    U8  LbaLow;
    U8  LbaMidExt;
    U8  LbaMid;
    U8  LbaHighExt;
    U8  LbaHigh;
    U8  Device;
    U8  Status;
}ATA_STATUS_RETURN_DESCRIPTOR, *PTR_ATA_STATUS_RETURN_DESCRIPTOR;

/* Sense data error type */
#define SCSI_SENSE_DATA_RESPONSE_CODE_DESCRIPTOR_CURRENT               (0x72)
#define SCSI_SENSE_DATA_RESPONSE_CODE_DESCRIPTOR_DEFERRED              (0x73)

/* Sense Data Descriptor types */
#define SCS_SENSE_DATA_DESCRIPTOR_TYPE_INFORMATION                      (0x00)
#define SCS_SENSE_DATA_DESCRIPTOR_TYPE_CMD_SPECIFIC_INFORMATION         (0x01)
#define SCS_SENSE_DATA_DESCRIPTOR_TYPE_SENSEKEY_SPECIFIC                (0x02)
#define SCS_SENSE_DATA_DESCRIPTOR_TYPE_BLOCK_CMD                        (0x05)
#define SCS_SENSE_DATA_DESCRIPTOR_TYPE_ATA_RETURN                       (0x09)


/******************************************************************************/
/* Format Unit Defect List Header                                             */
/******************************************************************************/
typedef struct _FORMAT_UNIT_DEFECT_LIST_HEADER
{
    U8       Reserved;
    U8       Flags;
    U8       DefectListLengthMSB;
    U8       DefectListLengthLSB;
} FORMAT_UNIT_DEFECT_LIST_HEADER, *PTR_FORMAT_UNIT_DEFECT_LIST_HEADER;

#define SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_IMMED            (0x02)
#define SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_FOV              (0x80)
#define SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_DPRY             (0x40)
#define SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_DCRT             (0x20)
#define SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_STPF             (0x10)
#define SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_IP               (0x08)
#define SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_FMT_OPTS         (SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_DPRY  | \
                                                             SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_DCRT  | \
                                                             SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_STPF  | \
                                                             SCSI_FORMAT_UNIT_DEFECT_LIST_FLAGS_IP        )


/******************************************************************************/
/* Log Pages                                                                  */
/******************************************************************************/

/* Log Parameter Flags */
typedef struct _LOG_PARAM_FLAGS
{
    union
    {
        struct
        {
            U8  LP          : 1;            /* List Parameter                      */
            U8  LBIN        : 1;            /*                                     */
            U8  TMC         : 2;            /* Threshold Met Criteria              */
            U8  ETC         : 1;            /* Enable Threshold Comparison         */
            U8  TSD         : 1;            /* Target Save Disable                 */
            U8  Ds          : 1;            /* Disable Save                        */
            U8  DU          : 1;            /* Disable Update                      */
        }bits;
        U8 Byte;
    }u;
} LOG_PARAM_FLAGS, *PTR_LOG_PARAM_FLAGS;

/* Log Parameter Header */
typedef struct _LOG_PARAM_HEADER
{
    U16                     ParameterCode;
    LOG_PARAM_FLAGS         Flags;
    U8                      ParameterLength;
}LOG_PARAM_HEADER, *PTR_LOG_PARAM_HEADER;

/* Log Page Header */
typedef struct _SCSI_LOG_PAGE_HEADER
{
    U8                      PageCode;       /*                                      */  /* 0x00 */
    U8                      Reserved;       /*                                      */  /* 0x01 */
    U16                     PageLength;     /*                                      */  /* 0x02 */
}SCSI_LOG_PAGE_HEADER, *PTR_SCSI_LOG_PAGE_HEADER;


/* Informational Exceptions Log Page */
typedef struct _SCSI_LOG_PAGE_INFORMATIONAL_EXCEPTIONS_GEN_PARAM_DATA
{
    LOG_PARAM_HEADER        Header ;        /*                                      */  /* 0x04 */
    U8                      ASC;            /*                                      */  /* 0x08 */
    U8                      ASCQ;           /*                                      */  /* 0x09 */
    U8                      TempReading;    /*                                      */  /* 0x0A */
    U8                      VendorSpec[5];  /*                                      */  /* 0x0B */
} SCSI_LOG_PAGE_INFORMATIONAL_EXCEPTIONS_GEN_PARAM_DATA,
  *PTR_SCSI_LOG_PAGE_INFORMATIONAL_EXCEPTIONS_GEN_PARAM_DATA;

typedef struct  _SCSI_LOG_PAGE_INFORMATIONAL_EXCEPTIONS
{
   SCSI_LOG_PAGE_HEADER                                     Header;
   SCSI_LOG_PAGE_INFORMATIONAL_EXCEPTIONS_GEN_PARAM_DATA    Param;
}SCSI_LOG_PAGE_INFORMATIONAL_EXCEPTIONS, *PTR_SCSI_LOG_PAGE_INFORMATIONAL_EXCEPTIONS;

typedef struct _SELF_TEST_LOG_PARAMETER
{
    LOG_PARAM_HEADER        Header;
    U8                      SelfTest_Code_Results;
    U8                      SelfTestNumber;
    U16                     TimeStamp;
    U8                      AddressOfFirstFailure[8];
    U8                      SenseKey;
    U8                      ASC;
    U8                      ASCQ;
    U8                      VendorSpecific;

}SELF_TEST_LOG_PARAMETER, *PTR_SELF_TEST_LOG_PARAMETER;

typedef struct _SCSI_SELF_TEST_RESULTS
{
    SCSI_LOG_PAGE_HEADER    Header;
    SELF_TEST_LOG_PARAMETER Param[20];

}SCSI_SELF_TEST_RESULTS, *PTR_SCSI_SELF_TEST_RESULTS;

#define SELFTEST_RESULTS_NO_ERROR                           (0x00)
#define SELFTEST_RESULTS_SEND_DIAG_ABORTED_BG_SELFTEST      (0x01)
#define SELFTEST_RESULTS_NON_SEND_DIAG_ABORTED_SELFTEST     (0x02)
#define SELFTEST_RESULTS_UNKNOWN_ERROR                      (0x03)
#define SELFTEST_RESULTS_IN_PROGRESS                        (0x0F)

#define NUMBER_OF_SUPPORTED_LOG_PAGES   (3)
typedef struct _SCSI_LOG_PAGE_SUUPPORTED
{
    SCSI_LOG_PAGE_HEADER    Header;
    U8                      SupportedList[3];
    U8                      Reserved[2];
}SCSI_LOG_PAGE_SUPPORTED, *PTR_SCSI_LOG_PAGE_SUPPORTED;


#endif /* End of if SCSI.H */

