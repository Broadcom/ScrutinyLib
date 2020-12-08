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

#ifndef __SCSI_GENERIC_INTERFACE__H__
#define __SCSI_GENERIC_INTERFACE__H__

#define INQ_REPLY_LEN           36
#define VPD_REPLY_LEN           255
#define MAX_CHIPNAME_LEN        64

#define IP_VPD_PAGE_CODE        0x85
#define IP_VPD_PAGE_OFFSET      0x08
#define IP_VPD_PAGE_LEN         0x0F
#define MAC_ADDRESS_LEN         28

/* VPD page offsets */
#define VPD_PAGE_CODE           0x83
#define VPD_PAGE_LEN_OFFSET_1   2
#define VPD_PAGE_LEN_OFFSET_2   3
#define VPD_PROTO_ID_OFFSET     4
#define VPD_DES_TYPE_OFFSET     5
#define VPD_DES_LEN_OFFSET      7

/* VPD page parameters */
#define PROTOCOL_ID             0x6
#define PIV                     0x8
#define ASSOCIATION             0x1
#define DESIGNATOR_TYPE         0x3

#define DESIGN_DESC_BYTE_0      (PROTOCOL_ID << 4)
#define DESIGN_DESC_BYTE_1      (((PIV | ASSOCIATION) << 4) | DESIGNATOR_TYPE)

#define EVPD_ENABLED            (0x01)

/*
 * SCSI Commands
 */

#define SCSI_CMD_INQUIRY        (0x12)


/*
 * SCSI Status
 */

#define SCSISTAT_GOOD                  0x00
#define SCSISTAT_CHECK_CONDITION       0x02
#define SCSISTAT_CONDITION_MET         0x04
#define SCSISTAT_BUSY                  0x08
#define SCSISTAT_INTERMEDIATE          0x10
#define SCSISTAT_INTERMEDIATE_COND_MET 0x14
#define SCSISTAT_RESERVATION_CONFLICT  0x18
#define SCSISTAT_COMMAND_TERMINATED    0x22
#define SCSISTAT_QUEUE_FULL            0x28


typedef struct _SCSI3_SENSE_DATA
{
    U8  ErrorCode;              /* 0x00 */
    U8  SegmentNumber;          /* 0x01 */
    U8  SenseKey;               /* 0x02 */
    U8  InformationMSB;         /* 0x03 - LBA or difference req-actual len or blks */
    U8  InformationByte2;       /* 0x04 */
    U8  InformationByte1;       /* 0x05 */
    U8  InformationLSB;         /* 0x06 */
    U8  AdditionalLength;       /* 0x07 - Sense Length minus 7 */
    U32 CommandSpecificInfo;    /* 0x08 - 0x0b */
    U8  ASC;                    /* 0x0c */
    U8  ASCQ;                   /* 0x0d */
    U8  FieldReplaceableUnitCode;       /* 0x0e */
    U8  SenseKeySpecific;       /* 0x0f */
}SCSI3_SENSE_DATA;

#if 0
STATUS sgiLocateExpanders();

STATUS sgiOpenDevice (PTR_DEVICE_INFO PtrExpander);

STATUS sgiFreeDevice (PTR_DEVICE_INFO PtrExpander);

STATUS sgiCloseDevice (PTR_DEVICE_INFO PtrExpander);

STATUS sgiIsOpened (PTR_DEVICE_INFO PtrExpander);

STATUS sgiWriteCdb (PTR_DEVICE_INFO PtrExpander, U8 *PtrCdb, U32 CdbLen,
                    U8 *PtrBuffer, U32 Size, U32 *PtrSizeWritten);

STATUS sgiReadCdb (PTR_DEVICE_INFO PtrExpander, U8 *PtrCdb, U32 CdbLen,
                   U8 *PtrBuffer, U32 Size, U32 *PtrSizeRead);
#endif


#endif /* __SCSI_GENERIC_INTERFACE__H__ */
