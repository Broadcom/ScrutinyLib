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


#ifndef __SMP_FUNCTIONS__H__
#define __SMP_FUNCTIONS__H__

#ifndef MAX
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#endif

/**SAS world wide ID length */
#define SAS_WWID_LENGTH                                         (8)

/**
 * SAS Address (full 8 byte WWID)
 * This structure is in little endian, so a SAS address of 0x50(MSB) 08 05 EF FF FF 80 00(LSB)
 * would show up here as High = 0xEF050850, Low = 0x0080FFFF
 */
typedef union _SMP_SAS_ADDRESS
{

    U8                      WWID[SAS_WWID_LENGTH];              /* 0x00 */

    struct
    {
        U32                 High;
        U32                 Low;
    } Word;

} SMP_SAS_ADDRESS, *PTR_SMP_SAS_ADDRESS;

/** Bytes to Dwords conversion. */
#define     BYTES_TO_DWORDS( Value )    ((Value) >> 2)
/** Dwords to Bytes conversion. */
#define     DWORDS_TO_BYTES( Value )    ((Value) << 2)


/** Size of Word in Bits. */
#define     WORD_LENGTH                 ( 16 )

/** Size of DWord in Bits. */
#define     DWORD_LENGTH                ( 32 )

#define SMP_DATA_BUFFER_SIZE     	(1028)


/*
 * SMP Request - native big endian format!!!
 */

typedef struct _SMP_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */

    /* Note AdditionalRequestBytes is of variable length based on function */
} SMP_REQUEST, *PTR_SMP_REQUEST;

/* SMP Frame Types */
#define SMP_FRAME_TYPE_SMP_REQUEST                              (0x40)
#define SMP_FRAME_TYPE_SMP_RESPONSE                             (0x41)

/* SMP Functions */
#define SMP_FUNCTION_REPORT_GENERAL                             (0x00)
#define SMP_FUNCTION_REPORT_MANUFACTURER_INFORMATION            (0x01)
#define SMP_FUNCTION_REPORT_SELF_CONFIG_STATUS                  (0X03)
#define SMP_FUNCTION_REPORT_BROADCAST                           (0X06)
#define SMP_FUNCTION_DISCOVER                                   (0x10)
#define SMP_FUNCTION_DISCOVER_LIST                              (0x20)
#define SMP_FUNCTION_REPORT_PHY_ERROR_LOG                       (0x11)
#define SMP_FUNCTION_REPORT_PHY_SATA                            (0x12)
#define SMP_FUNCTION_REPORT_ROUTE_INFORMATION                   (0x13)
#define SMP_FUNCTION_REPORT_PHY_EVENT                           (0x14)
#define SMP_FUNCTION_REPORT_PHY_EVENT_LIST                      (0x21)

#define SMP_FUNCTION_CONFIGURE_ROUTE_INFORMATION                (0x90)
#define SMP_FUNCTION_PHY_CONTROL                                (0x91)
#define SMP_FUNCTION_PHY_TEST_FUNCTION                          (0x92)
#define SMP_FUNCTION_CONFIGURE_PHY_EVENT                        (0x93)

/* These are vendor unique functions for LSI expanders */
#define SMP_FUNCTION_PHY_ACTIVE_CONTROL                         (0xC1)

#define SMP_FUNCTION_RING_WRITE_N_REGISTERS                     (0xC0)
#define SMP_FUNCTION_RING_READ_N_REGISTERS                      (0x40)
#define SMP_FUNCTION_ICL_DISCOVER                               (0x41)

#define SMP_FUNCTION_AHB_WRITE_N_REGISTERS                      (0xC2)
#define SMP_FUNCTION_AHB_READ_N_REGISTERS                       (0x42)
#define SMP_FUNCTION_REPORT_EDFB_SETTINGS                       (0x43)
#define SMP_FUNCTION_REPORT_SECURE_BOOT                         (0x44)

/* These are for SGPIO use */
#define SMP_FUNCTION_READ_GPIO_REGISTERS                        (0x02)
#define SMP_FUNCTION_WRITE_GPIO_REGISTERS                       (0x82)

/* REPORT EXPANDER ROUTE TABLE LIST SMP function */
#define SMP_FUNCTION_REPORT_EXP_ROUTE_TABLE_LIST                (0x22)

/* SMP Functions for Zoning */
#define SMP_FUNCTION_REPORT_ZONE_PERMISSION                     (0x04)
#define SMP_FUNCTION_REPORT_ZONE_MANAGER_PASSWORD               (0x05)
#define SMP_FUNCTION_ENABLE_DISABLE_ZONING                      (0x81)
#define SMP_FUNCTION_ZONED_BROADCAST                            (0x85)
#define SMP_FUNCTION_ZONE_LOCK                                  (0x86)
#define SMP_FUNCTION_ZONE_ACTIVATE                              (0x87)
#define SMP_FUNCTION_ZONE_UNLOCK                                (0x88)
#define SMP_FUNCTION_CONFIGURE_ZONE_MANAGER_PASSWORD            (0x89)
#define SMP_FUNCTION_CONFIGURE_ZONE_PHY                         (0x8a)
#define SMP_FUNCTION_CONFIGURE_ZONE_PERMISSION                  (0x8b)

/*
 * SMP Response - native big endian format!!!
 */

typedef struct _SMP_RESPONSE
{
    U8                      ResponseLength;                         /* 0x03 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      AdditionalResponseBytes[1];         /* 0x04 */
    /* Note AdditionalResponseBytes is of variable length based on function */
} SMP_RESPONSE, *PTR_SMP_RESPONSE;

/* SMP Function Result field */
#define SMP_RESPONSE_FUNCTION_RESULT_ACCEPTED                       (0x00)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_SMP_FUNCTION           (0x01)
#define SMP_RESPONSE_FUNCTION_RESULT_SMP_FUNCTION_FAILED            (0x02)
#define SMP_RESPONSE_FUNCTION_RESULT_INVALID_REQUEST_LENGTH         (0x03)
#define SMP_RESPONSE_FUNCTION_RESULT_PHY_DOES_NOT_EXIST             (0x10)
#define SMP_RESPONSE_FUNCTION_RESULT_INDEX_DOES_NOT_EXIST           (0x11)
#define SMP_RESPONSE_FUNCTION_RESULT_PHY_DOES_NOT_SUPPORT_SATA      (0x12)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_PHY_OPERATION          (0x13)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_PHY_TEST_FUNCTION      (0x14)
#define SMP_RESPONSE_FUNCTION_RESULT_PHY_TEST_FUNCTION_IN_PROGRESS  (0x15)
#define SMP_RESPONSE_FUNCTION_RESULT_PHY_VACANT                     (0x16)

#define SMP_RESPONSE_FUNCTION_FAILED_LENGTH                     (1)

/* SMPT Buffer Size */
#define SMP_TGT_BUFFER_DWORDS   256
#define SMP_BUFFER_SIZE         (SMP_TGT_BUFFER_DWORDS * sizeof(U32))


/*
 * SMP Configure Zone Phy Request
 */

typedef struct _HAL_SMP_CONFIGURE_ZONE_PHY_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      NumberOfZonePhyConfigDescriptors;   /* 0x07 */
    U8                      Save                            :2; /* 0x06 */
    U8                      ZonePhyConfigDescriptorLength   :6; /* 0x06 */
    U16                     ExpectedExpanderChangeCount;        /* 0x04 - 0x05 */

    /* phy zone entry data follows from here... */
} HAL_SMP_CONFIGURE_ZONE_PHY_REQUEST, *PTR_HAL_SMP_CONFIGURE_ZONE_PHY_REQUEST;


#define HAL_SMP_CONFIGURE_ZONE_PHY_SAVE_FIELD_SHADOW         0x00
#define HAL_SMP_CONFIGURE_ZONE_PHY_SAVE_FIELD_SAVED          0x01
#define HAL_SMP_CONFIGURE_ZONE_PHY_SAVE_FIELD_SAVED_SHADOW_2 0x02
#define HAL_SMP_CONFIGURE_ZONE_PHY_SAVE_FIELD_SAVED_SHADOW_3 0x03


typedef struct _HAL_SMP_ZONE_PHY_CONFIG_DESCRIPTOR
{
    U8                      ZoneGroup;                          /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */

    U8                      Reserved01Bit0to1               :2; /* 0x01 */
    U8                      ZoneGroupPersistent             :1;
    U8                      Reserved01Bit3                  :1;
    U8                      RequestedInsideZpsds            :1;
    U8                      InsideZpsdsPersistent           :1;
    U8                      Reserved01Bit6to7               :2;

    U8                      PhyIdentifier;                      /* 0x00 */
} HAL_SMP_ZONE_PHY_CONFIG_DESCRIPTOR, *PTR_HAL_SMP_ZONE_PHY_CONFIG_DESCRIPTOR;

/*
 * SMP Configure Zone Phy Response
 */

typedef struct _HAL_SMP_CONFIGURE_ZONE_PHY_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} HAL_SMP_CONFIGURE_ZONE_PHY_RESPONSE, *PTR_HAL_SMP_CONFIGURE_ZONE_PHY_RESPONSE;

/*
 * SMP Configure Zone Permission request - native big endian format!!!
 */

typedef struct _HAL_SMP_CONFIGURE_ZONE_PERMISSION_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      NumberOfZoneConfigDescriptors;      /* 0x07 */
    U8                      StartingSourceZoneGroup;            /* 0x06 */
    U16                     ExpectedExpanderChangeCount;        /* 0x04 - 0x05 */

    U16                     Reserved0Ato0B;                     /* 0x0A - 0x0B */
    U8                      ZonePermissionConfigDescriptorLength; /* 0x09 */
    U8                      Save               : 2;             /* 0x08 Bit 0 - 1 */
    U8                      Reserved08Bits2to5 : 4;             /* 0x08 Bit 2 - 5 */
    U8                      NumberOfZoneGroups : 2;             /* 0x08 Bit 6 - 7 */

    U32                     Reserved0Cto0F;                     /* 0x0C - 0x0F */

    /* phy zone entry data follows from here... */
} HAL_SMP_CONFIGURE_ZONE_PERMISSION_REQUEST, *PTR_HAL_SMP_CONFIGURE_ZONE_PERMISSION_REQUEST;


#define HAL_SMP_CONFIGURE_PERMISSION_SAVE_FIELD_SHADOW         0x00
#define HAL_SMP_CONFIGURE_PERMISSION_SAVE_FIELD_SAVED          0x01
#define HAL_SMP_CONFIGURE_PERMISSION_SAVE_FIELD_SAVED_SHADOW_2 0x02
#define HAL_SMP_CONFIGURE_PERMISSION_SAVE_FIELD_SAVED_SHADOW_3 0x03


/*
 * Specifies the supported number of zone groups and response descriptor type.
 */
#define HAL_SMP_NUM_ZONE_GROUPS_128    0x00
#define HAL_SMP_NUM_ZONE_GROUPS_256    0x01

typedef union _HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_128
{
    U8                      PermBits[16];                       /* 128 bit mask of zones */
                                                                /* 127-120, 119-112, 111-104, ... */
    U32                     PermWords[4];
} HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_128, *PTR_HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_128;

typedef union _HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_256
{
    U8                      PermBits[32];                       /* 256 bit mask of zones */
                                                                /* 255-248, 247-240, 239-232, ... */
    U32                     PermWords[8];
} HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_256, *PTR_HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_256;

#define HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_SIZE_256 (sizeof(HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_256) / \
                                                     sizeof(U32))
#define HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_SIZE_128 (sizeof(HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_128) / \
                                                     sizeof(U32))

typedef struct _HAL_SMP_CONFIGURE_ZONE_PERMISSION_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} HAL_SMP_CONFIGURE_ZONE_PERMISSION_RESPONSE, *PTR_HAL_SMP_CONFIGURE_ZONE_PERMISSION_RESPONSE;

/*
 * SMP Report Zone Permission request - native big endian format!!!
 */

typedef struct _HAL_SMP_REPORT_ZONE_PERMISSION_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      MaxNumberOfZoneDescriptors;         /* 0x07 */
    U8                      StartingSourceZoneGroup;            /* 0x06 */
    U8                      Reserved05;                         /* 0x05 */
    U8                      ReportType                      :2; /* 0x04 Bits 0 - 1 */
    U8                      Reserved04Bit2to7               :6; /* 0x04 Bits 2 - 7 */

} HAL_SMP_REPORT_ZONE_PERMISSION_REQUEST, *PTR_HAL_SMP_REPORT_ZONE_PERMISSION_REQUEST;


#define HAL_SMP_REPORT_PERMISSION_TYPE_ACTIVE  0x00
#define HAL_SMP_REPORT_PERMISSION_TYPE_SHADOW  0x01
#define HAL_SMP_REPORT_PERMISSION_TYPE_SAVED   0x02
#define HAL_SMP_REPORT_PERMISSION_TYPE_DEFAULT 0x03


typedef struct _HAL_SMP_REPORT_ZONE_PERMISSION_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    U8                      Reserved07Bit0to5               :6; /* 0x07 Bits 0 - 5 */
    U8                      NumberOfZoneGroups              :2; /* 0x07 Bits 6 - 7 */
    U8                      ReportType                      :2; /* 0x06 */
    U8                      Reserved06Bit2to6               :5; /* 0x06 Bits 2 - 6 */
    U8                      ZoneLocked                      :1;
    U16                     ExpanderChangeCount;                /* 0x04 - 0x05 */

    U32                     Reserved08;                         /* 0x08 - 0x0b */

    U8                      NumberZonePermissionDescr;          /* 0x0f */
    U8                      StartingSourceZoneGroup;            /* 0x0e */
    U8                      ZonePermissionDescriptorLength;     /* 0x0d */
    U8                      Reserved0c;                         /* 0x0c */

    /* zone permission descriptors start here... */
} HAL_SMP_REPORT_ZONE_PERMISSION_RESPONSE, *PTR_HAL_SMP_REPORT_ZONE_PERMISSION_RESPONSE;


#define MAX_REPORT_ZONE_PERMISSION_DESCRIPTORS  \
    ( (SMP_BUFFER_SIZE - sizeof(HAL_SMP_REPORT_ZONE_PERMISSION_RESPONSE)) /\
            sizeof(HAL_SMP_ZONE_PERMISSION_DESCRIPTOR_256) )

/*
 * SMP Enable / Disable Zoning request - native big endian format!!!
 */

typedef struct _HAL_SMP_ENABLE_DISABLE_ZONING_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      Reserved07;                         /* 0x07 */
    U8                      Save                            :2; /* 0x06 Bits 0 - 1 */
    U8                      Reserved06Bit2to7               :6; /* 0x06 Bits 2 - 7 */
    U16                     ExpectedExpanderChangeCount;        /* 0x04 - 0x05 */

    U16                     Reserved0a;                         /* 0x0a - 0x0b */
    U8                      Reserved09;                         /* 0x09 */
    U8                      EnableDisableZoning             :2; /* 0x08 Bits 0 - 1 */
    U8                      Reserved08Bit2to7               :6; /* 0x08 Bits 2 - 7 */

} HAL_SMP_ENABLE_DISABLE_ZONING_REQUEST, *PTR_HAL_SMP_ENABLE_DISABLE_ZONING_REQUEST;

/*
 * Definitions for "EnableDisableZoning" field.
 */
#define SMP_ZONING_ENABLE_NO_CHANGE        0x00
#define SMP_ZONING_ENABLE_ENABLE           0x01
#define SMP_ZONING_ENABLE_DISABLE          0x02
#define SMP_ZONING_ENABLE_DISABLE_RESERVED 0x03

/*
 * Definitions for "Save" field.
 */
#define SMP_ZONING_ENABLE_SAVE_FIELD_SHADOW         0x00
#define SMP_ZONING_ENABLE_SAVE_FIELD_SAVED          0x01
#define SMP_ZONING_ENABLE_SAVE_FIELD_SAVED_SHADOW_2 0x02
#define SMP_ZONING_ENABLE_SAVE_FIELD_SAVED_SHADOW_3 0x03

/*
 * SMP Enable / Disable Zoning Response
 */

typedef struct _HAL_SMP_ENABLE_DISABLE_ZONING_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} HAL_SMP_ENABLE_DISABLE_ZONING_RESPONSE, *PTR_HAL_SMP_ENABLE_DISABLE_ZONING_RESPONSE;

/*
 * SMP Zoned Broadcast request - native big endian format!!!
 */

typedef struct _SMP_ZONED_BROADCAST_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      NumberOfSourceZoneGroups;           /* 0x07 */
    U8                      BroadcastType                   :4; /* 0x06 Bits 0 - 3 */
    U8                      Reserved06Bits4to7              :4; /* 0x06 Bits 4 - 7 */
    U16                     ExpectedExpanderChangeCount;        /* 0x04 - 0x05 */

    /* The list of Broadcast Source Zone Groups follow from here... */

} SMP_ZONED_BROADCAST_REQUEST, *PTR_SMP_ZONED_BROADCAST_REQUEST;

/*
 * Zoned Broadcast Types
 */
#define SMP_ZONED_BROADCAST_TYPE_CHANGE              0x00
#define SMP_ZONED_BROADCAST_TYPE_RESERVED_CHANGE_0   0x01
#define SMP_ZONED_BROADCAST_TYPE_RESERVED_CHANGE_1   0x02
#define SMP_ZONED_BROADCAST_TYPE_SES                 0x03
#define SMP_ZONED_BROADCAST_TYPE_EXPANDER            0x04
#define SMP_ZONED_BROADCAST_TYPE_ASYNCHRONOUS_EVENT  0x05
#define SMP_ZONED_BROADCAST_TYPE_RESERVED_3          0x06
#define SMP_ZONED_BROADCAST_TYPE_RESERVED_4          0x07
#define SMP_ZONED_BROADCAST_TYPE_ZONE_ACTIVATE       0x08
#define SMP_ZONED_BROADCAST_TYPE_MAX_TYPES           0x09

/*
 * SMP Zoned Broadcast Response
 */

typedef struct _SMP_ZONED_BROADCAST_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} SMP_ZONED_BROADCAST_RESPONSE, *PTR_SMP_ZONED_BROADCAST_RESPONSE;


/*
 * SMP Zone Activate Request
 */

typedef struct _HAL_SMP_ZONE_ACTIVATE_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U16                     Reserved06to07;                     /* 0x06 - 0x07 */
    U16                     ExpectedExpanderChangeCount;        /* 0x04 - 0x05 */
} HAL_SMP_ZONE_ACTIVATE_REQUEST, *PTR_HAL_SMP_ZONE_ACTIVATE_REQUEST;

/*
 * SMP Zone Activate Response
 */

typedef struct _HAL_SMP_ZONE_ACTIVATE_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} HAL_SMP_ZONE_ACTIVATE_RESPONSE, *PTR_HAL_SMP_ZONE_ACTIVATE_RESPONSE;

#define ZONE_MANAGER_PASSWORD_LENGTH        32

/*
 * SMP Zone Lock Request
 */

typedef struct _HAL_SMP_ZONE_LOCK_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U16                     InactivityTimeLimit;                                    /* 0x06 - 0x07 */
    U16                     ExpectedExpanderChangeCount;                            /* 0x04 - 0x05 */

    U8                      ZoneManagerPassword[ZONE_MANAGER_PASSWORD_LENGTH];      /* 0x08 - 0x27 */

} HAL_SMP_ZONE_LOCK_REQUEST, *PTR_HAL_SMP_ZONE_LOCK_REQUEST;

/*
 * SMP Zone Lock Response
 */

typedef struct _HAL_SMP_ZONE_LOCK_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    U32                     Reserved04;                         /* 0x04 - 0x07 */

    SMP_SAS_ADDRESS         ActiveZoneManager;                  /* 0x08 - 0x0f */

} HAL_SMP_ZONE_LOCK_RESPONSE, *PTR_HAL_SMP_ZONE_LOCK_RESPONSE;

/*
 * SMP Zone Unlock Request
 */

typedef struct _HAL_SMP_ZONE_UNLOCK_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      Reserved07;                         /* 0x07 */

    U8                      ActivateRequired                :1; /* 0x06 */
    U8                      Reserved06                      :7;

    U16                     Restricted;                         /* 0x04 - 0x05 */

} HAL_SMP_ZONE_UNLOCK_REQUEST, *PTR_HAL_SMP_ZONE_UNLOCK_REQUEST;

/*
 * SMP Zone Unlock Response
 */

typedef struct _HAL_SMP_ZONE_UNLOCK_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} HAL_SMP_ZONE_UNLOCK_RESPONSE, *PTR_HAL_SMP_ZONE_UNLOCK_RESPONSE;


/*
 * SMP Report Zone Manager Password Request - native big endian format!!!
 */

typedef struct _HAL_SMP_REPORT_ZONE_MANAGER_PASSWORD_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      Reserved07;                         /* 0x07 */
    U8                      Reserved06;                         /* 0x06 */
    U8                      Reserved05;                         /* 0x05 */
    U8                      ReportType          :2;             /* 0x04 Bits 0 - 1 */
    U8                      Reserved04Bits2to7  :6;             /* 0x04 Bits 2 - 7 */

} HAL_SMP_REPORT_ZONE_MANAGER_PASSWORD_REQUEST, *PTR_HAL_SMP_REPORT_ZONE_MANAGER_PASSWORD_REQUEST;

/*
 * Definitions for "Save" field.
 */
#define SMP_REPORT_ZONE_MANAGER_PASSWORD_CURRENT   0x00
#define SMP_REPORT_ZONE_MANAGER_PASSWORD_RESERVED  0x01
#define SMP_REPORT_ZONE_MANAGER_PASSWORD_SAVED     0x02
#define SMP_REPORT_ZONE_MANAGER_PASSWORD_DEFAULT   0x03

/*
 * SMP Report Zone Manager Password Response - native big endian format!!!
 */

typedef struct _HAL_SMP_REPORT_ZONE_MANAGER_PASSWORD_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    U8                      Reserved07;                                             /* 0x07 */
    U8                      ReportType          :2;                                 /* 0x06 Bits 0 - 1 */
    U8                      Reserved06Bits2to7  :6;                                 /* 0x06 Bits 2 - 7 */
    U16                     ExpanderChangeCount;                                    /* 0x04 - 0x05 */

    U8                      ZoneManagerPassword[ZONE_MANAGER_PASSWORD_LENGTH];      /* 0x08 - 0x27 */

} HAL_SMP_REPORT_ZONE_MANAGER_PASSWORD_RESPONSE, *PTR_HAL_SMP_REPORT_ZONE_MANAGER_PASSWORD_RESPONSE;


/*
 * SMP Configure Zone Manager Password Request - native big endian format!!!
 */

typedef struct _HAL_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      Reserved07;                                             /* 0x07 */
    U8                      Save                :2;                                 /* 0x06 Bit 0 to 1 */
    U8                      Reserved06Bit2to7   :6;                                 /* 0x06 Bit 2 to 7 */
    U16                     ExpectedExpanderChangeCount;                            /* 0x04 - 0x05 */

    U8                      ZoneManagerPassword[ZONE_MANAGER_PASSWORD_LENGTH];      /* 0x08 - 0x27 */

    U8                      NewZoneManagerPassword[ZONE_MANAGER_PASSWORD_LENGTH];   /* 0x28 - 0x47 */

} HAL_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_REQUEST, *PTR_HAL_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_REQUEST;

/*
 * Definitions for "Save" field.
 */
#define SMP_CONFIGURE_PASSWORD_SAVE_FIELD_CURRENT         0x00
#define SMP_CONFIGURE_PASSWORD_SAVE_FIELD_SAVED           0x01
#define SMP_CONFIGURE_PASSWORD_SAVE_FIELD_SAVED_CURRENT_2 0x02
#define SMP_CONFIGURE_PASSWORD_SAVE_FIELD_SAVED_CURRENT_3 0x03

/*
 * SMP Configure Zone Manager Password Response - native big endian format!!!
 */

typedef struct _HAL_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} HAL_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_RESPONSE, *PTR_HAL_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_RESPONSE;

/*
 * SMP Report General Request - native big endian format!!!
 */

typedef struct _SMP_REPORT_GENERAL_REQUEST
{
    U8      RequestLength;      /* 0x03 */
    U8      AllocRespLength;    /* 0x02 */
    U8      Function;           /* 0x01 */
    U8      SMPFrameType;       /* 0x00 */

} SMP_REPORT_GENERAL_REQUEST, *PTR_SMP_REPORT_GENERAL_REQUEST;

/*
 * SMP Report General response - native big endian format!!!
 */
typedef struct _SMP_REPORT_GENERAL_RESPONSE
{
    union
    {
        struct
        {
            U8      ResponseLength;                         /* 0x03 */
            U8      FunctionResult;                         /* 0x02 */
            U8      Function;                               /* 0x01 */
            U8      SMPFrameType;                           /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            U16     ExpanderRouteIndexes;                   /* 0x06 - 0x07 */
            U16     ExpanderChangeCount;                    /* 0x04 - 0x05 */

            U8      InitiatesSspClose               :1;     /* 0x0B */
            U8      ExtendedFairness                :1;
            U8      Reserved0BBits2to7              :6;

            U8     ExtConfigurableRouteTable       :1;     /* 0x0A */
            U8     Configuring                     :1;
            U8     ConfiguresOthers                :1;
            U8     OpenRejectRetrySupported        :1;
            U8     STPContinueAWT                  :1;
            U8     SelfConfiguring                 :1;
            U8     ZoneConfiguring                 :1;
            U8     TableToTableSupported           :1;
            U8      NumberOfPhys;                           /* 0x09 */
            U8     Reserved08Bits0to6              :7;     /* 0x08 */
            U8     LongResponse                    :1;     /* 0x08 */

            U32     EnclosureLogicalIdentifierLow;          /* 0x0C - 0x0F */
            U32     EnclosureLogicalIdentifierHigh;         /* 0x10 - 0x13 */

            U32     Reserved14;                             /* 0x14 - 0x17 */

            U32     Reserved18;                             /* 0x18 - 0x1B */

            U16     StpBusInactivityLimit;                  /* 0x1E - 0x1F */
            U16     SspConnectTimeLimit;                    /* 0x1C - 0x1D */


            U16     StpSmpITNexusLossTime;                  /* 0x22 - 0x23 */
            U16     StpConnectTimeLimit;                    /* 0x20 - 0x21 */

            U16     MaxNumRoutedSasAddress;                 /* 0x26 - 0x27 */

            U8     SaveZoneEnabledSupport:1;               /*0x25*/
            U8     SaveZonePermissionTable:1;
            U8     SaveZonePhyInfoSupport:1;
            U8     SaveZoneManagerPassword:1;
            U8     Saving:1;
            U8     Reserved25Bits5to7:3;


            U8     ZoningEnabled                   :1;     /* 0x24 */
            U8     ZoningSupported                 :1;
            U8     PhyPresenceAsserted             :1;
            U8     PhyPresenceSupported            :1;
            U8     ZoneLocked                      :1;     /* Change in SAS2r07 */
            U8     Reserved24Bit5                  :1;     /* Changed from 4 - 7 to 5 - 6. */
            U8     NoOfZoneGroups                  :2;     /* New Addition */

            SMP_SAS_ADDRESS AZMSasAddress;                      /* 0x28 - 0x2F */ /* Change in SAS2r07 */

            U16     Reserved32;                             /* 0x32 - 0x33 */ /* Change in SAS2r07 */
            U16     ZoneLockInactivityTimeLimit;            /* 0x30 - 0x31 */

            U8      InitialTimeToDelayExpFwdOpnIndication;  /* 0x37 */
            U8      NumberOfEncConnElementIdxs;             /* 0x36 */
            U8      FirstEncConnElementIdx;                 /* 0x35 */
            U8      Reserved34;                             /* 0x34 */

            U8      MaxReducedFuncTime;                     /* 0x3B */
            U8      InitialTimeToReduceFunc;                /* 0x3A */
            U8      TimeToReduceFunc;                       /* 0x39 */
            U8     Reserved38Bits0to6 :7;                  /* 0x38 */
            U8     ReducedFunc        :1;                  /* 0x38 */

            U16     MaxNumOfStoredSelfCfgStatusDescriptors; /* 0x3E - 0x3F */
            U16     LastSelfCfgStatusDescriptorIdx;         /* 0x3C - 0x3D */

            U16     MaxNumOfStoredPhyEventListDescriptors;  /* 0x42 - 0x43 */
            U16     LastPhyEventListDescriptorIdx;          /* 0x40 - 0x41 */

            U16     Reserved46;                             /* 0x46 - 0x47 */
            U16     STPRejectToOpenLimit;                   /* 0x44 - 0x45 */
        } Fields;

        U32         Dword[17];                              /* 0x04 - 0x33 */
    } Response;

} SMP_REPORT_GENERAL_RESPONSE, *PTR_SMP_REPORT_GENERAL_RESPONSE;


/*
 * SMP Report Manufacturer Information request - native big endian format!!!
 */
typedef struct _SMP_REPORT_MANUFACTURER_INFO_REQUEST
{
    U8      RequestLength;      /* 0x03 */
    U8      AllocRespLength;    /* 0x02 */
    U8      Function;           /* 0x01 */
    U8      SMPFrameType;       /* 0x00 */
} SMP_REPORT_MANUFACTURER_INFO_REQUEST, *PTR_SMP_REPORT_MANUFACTURER_INFO_REQUEST;

 /*
 * SMP Report Manufacturer Information response - native big endian format!!!
 */
#define VEND_ID_LEN                                             (8)
#define PROD_ID_LEN                                             (16)
#define COMP_ID_LEN                                             (8)
#define VEND_SPEC_LEN                                           (8)

typedef struct _SMP_REPORT_MANUFACTURER_INFO_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
             U16             Reserved06;                         /* 0x06 - 0x07 */
             U16             ExpanderChangeCount;                /* 0x04 - 0x05 */

             U8              Reserved0B;                         /* 0x0B */
             U8              Reserved0A;                         /* 0x0A */
             U8              Reserved09;                         /* 0x09 */
             U8              SasFormat                       :1; /* 0x08 */
             U8              Reserved08Bits1to7              :7;

             U8              VendorIdent[VEND_ID_LEN];           /* 0x0C - 0x13 */
             U8              ProductIdent[PROD_ID_LEN];          /* 0x14 - 0x23 */
             U32             ProductRevLevel;                    /* 0x24 - 0x27 */

             U8              ComponentVendorIdent[COMP_ID_LEN];  /* 0x28 - 0x2F */

             U8              Reserved33;                         /* 0x33 */
             U8              ComponentRevLevel;                  /* 0x32 */
             U16             ComponentIdent;                     /* 0x30 - 0x31 */

             U8              VendorSpecific[VEND_SPEC_LEN];      /* 0x34 - 0x3B */
        } Fields;
        U32                 Dword[14];                          /* 0x04 - 0x3B */
    } Response;
} SMP_REPORT_MANUFACTURER_INFO_RESPONSE, *PTR_SMP_REPORT_MANUFACTURER_INFO_RESPONSE;


/*
 * SMP Report Self Configuration Status request - native big endian format!!!
 */
typedef struct _SMP_REPORT_SELF_CONFIG_STATUS_REQUEST
{

    U8             RequestLength;                    /* 0x03 */
    U8             AllocRespLength;                  /* 0x02 */
    U8             Function;                         /* 0x01 */
    U8             SMPFrameType;                     /* 0x00 */

    U16            StartSelfConfigStatusDescIndex;   /* 0x06 - 0x07 */
    U16            Reserved04;                       /* 0x04 - 0x05 */

} SMP_REPORT_SELF_CONFIG_STATUS_REQUEST, *PTR_SMP_REPORT_SELF_CONFIG_STATUS_REQUEST;


/* Common part of response needed in
 * SMP Report Self Config Status response
 */
typedef struct _SMP_REPORT_SELF_CONFIG_STATUS_COMMON_RESPONSE
{
    U16      StartSelfConfigStatusDescIndex;     /* 0x06 - 0x07 */
    U16      ExpanderChangeCount;                /* 0x04 - 0x05 */

    U16      LastSelfConfigStatusDescIndex;      /* 0x0A - 0x0B */
    U16      TotalSelfConfigStatusDescriptors;   /* 0x08 - 0x09 */

    U16      Reserved0E;                         /* 0x0E - 0x0F */
    U8       Reserved0D;                         /* 0x0D */
    U8       SelfConfigStatusDescLen;            /* 0x0C */

    U8       NumSelfConfigStatusDescriptors;     /* 0x13 */
    U8       Reserved12;                         /* 0x12 */
    U16      Reserved10;                         /* 0x10 - 0x11 */
} SMP_REPORT_SELF_CONFIG_STATUS_COMMON_RESPONSE,
  *PTR_SMP_REPORT_SELF_CONFIG_STATUS_COMMON_RESPONSE;

/* Self Configuration Status Descriptor needed in
 * SMP Report Self Config Status response
 */
typedef struct _SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR
{
    U8                      PhyIdentifier;              /* 0x03 */
    U8                      Reserved02;                 /* 0x02 */
    U8                      Final               :1;     /* 0x01 bit 0 */
    U8                      Reserved01Bit1to7   :7;     /* 0x01 bit 1 to 7 */
    U8                      StatusType;                 /* 0x00 */

    U32                     Reserved04to07;             /* 0x04 - 0x07 */

    SMP_SAS_ADDRESS             SasAddress;                 /* 0x08 - 0x0F */
} SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR,
  *PTR_SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR;

/* Values needed in Report Self Configuration Status Reposnse */
#define SMP_SCE_STATUS_RESPONSE_HEADER_SIZE     sizeof(U32)

/* max descriptors =
 * (SMP buffer size - header size - size of common response) / size of descriptor
 */
#define MAX_ALLOWED_SCE_STATUS_DESCRIPTORS                             \
            ( (SMP_BUFFER_SIZE - SMP_SCE_STATUS_RESPONSE_HEADER_SIZE -  \
               sizeof(SMP_REPORT_SELF_CONFIG_STATUS_COMMON_RESPONSE)) / \
                   sizeof(SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR) )

#define MAX_ALLOC_RESP_LEN_BASED_SCE_DESCRIPTORS( AllocRespLen )       \
            ( ((AllocRespLen * 4) -                                     \
              sizeof(SMP_REPORT_SELF_CONFIG_STATUS_COMMON_RESPONSE)) /  \
                  sizeof(SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR) )
/*
 * SMP Report Self Configuration Status response - native big endian format!!!
 */
typedef struct _SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE
{
    union
    {
        struct
        {
            U8       ResponseLength;                     /* 0x03 */
            U8       FunctionResult;                     /* 0x02 */
            U8       Function;                           /* 0x01 */
            U8       SMPFrameType;                       /* 0x00 */
        } Fields;
        U32          Dword;
    } Header;

    union
    {
        struct
        {
            SMP_REPORT_SELF_CONFIG_STATUS_COMMON_RESPONSE
                CommonResponse;
            SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR
                SceDescriptor[MAX_ALLOWED_SCE_STATUS_DESCRIPTORS];
        } Fields;

        U32    Dword[252];
    } Response;

} SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE,
*PTR_SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE;


/*
 * SMP Report Broadcast request - native big endian format!!!
 */
typedef struct _SMP_REPORT_BROADCAST_REQUEST
{
    U8      RequestLength;              /* 0x03 */
    U8      AllocRespLength;            /* 0x02 */
    U8      Function;                   /* 0x01 */
    U8      SMPFrameType;               /* 0x00 */

    U16     Reserved07;                 /* 0x07 - 0x06 */
    U8      Reserved05;                 /* 0x05 */
    U8      BroadcastType       :4;     /* 0x04 */
    U8      Reserved04Bits4to7  :4;
} SMP_REPORT_BROADCAST_REQUEST, *PTR_SMP_REPORT_BROADCAST_REQUEST;


/* SMP Report Broadcast response common structure after header!!! */
typedef struct  _SMP_REPORT_BROADCAST_COMMON_RESP
{
    U8      Reserved07;                         /* 0x07 */
    U8      BroadcastType       :4;             /* 0x06 */
    U8      Reserved06Bits4to7  :4;
    U16     ExpanderChangeCount;                /* 0x05 - 0x04 */

    U8      NoOfDescriptors;                    /* 0x0B */
    U8      DescriptorLength;                   /* 0x0A */
    U16     Reserved09;                         /* 0x09 - 0x08 */

} SMP_REPORT_BROADCAST_COMMON_RESP;


/* SMP Report Broadcast Descriptor - native big endian format!!! */
typedef union _SMP_REPORT_BROADCAST_DESCRIPTOR
{
    struct
    {
        U8      Reserved03;                     /* 0x03 */
        U8      BroadcastReason         : 4;    /* 0x02 */
        U8      Restricted02Bit4to7     : 4;
        U8      PhyIdentifier;                  /* 0x01 */
        U8      BroadcastType           : 4;    /* 0x00 */
        U8      Restricted00Bit4to7     : 4;

        U16     Reserved07;                     /* 0x07 - 0x06 */
        U16     BroadcastCount;                 /* 0x05 - 0x04 */
    } Fields;

    U32         Dword[2];
} SMP_REPORT_BROADCAST_DESCRIPTOR, *PTR_SMP_REPORT_BROADCAST_DESCRIPTOR;

/* Macros used by Discover List response and discover list descriptor */
#define SMP_REPORT_BROADCAST_RESPONSE_HEADER_SIZE   sizeof(U32)

#define SMP_REPORT_BROADCAST_COMMON_RESP_SIZE  sizeof(SMP_REPORT_BROADCAST_COMMON_RESP)

/* Since only Broadcast SES count is supported,
 * only 1 descriptor is reported.
 */
#define  NUM_REPORT_BROADCAST_DESCRIPTOR    1


#define SMP_REPORT_BROADCAST_MAX_DESCRIPTOR_SIZE \
    ( sizeof(SMP_REPORT_BROADCAST_DESCRIPTOR) * NUM_REPORT_BROADCAST_DESCRIPTOR )

/*
 * SMP Report Broadcast response - native big endian format!!!
 */
typedef struct _SMP_REPORT_BROADCAST_RESPONSE
{
    union
    {
        struct
        {
            U8      ResponseLength;     /* 0x03 */
            U8      FunctionResult;     /* 0x02 */
            U8      Function;           /* 0x01 */
            U8      SMPFrameType;       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            SMP_REPORT_BROADCAST_COMMON_RESP  CommonResp;
            SMP_REPORT_BROADCAST_DESCRIPTOR
                Descriptors[NUM_REPORT_BROADCAST_DESCRIPTOR];
        } Fields;

        U32 Dword[ BYTES_TO_DWORDS( SMP_REPORT_BROADCAST_COMMON_RESP_SIZE ) +
                   BYTES_TO_DWORDS( SMP_REPORT_BROADCAST_MAX_DESCRIPTOR_SIZE )
                 ];
    } Response;
} SMP_REPORT_BROADCAST_RESPONSE, *PTR_SMP_REPORT_BROADCAST_RESPONSE;


/*
 * SMP Discover request - native big endian format!!!
 */
typedef struct _SMP_DISCOVER_REQUEST
{
    U8      RequestLength;              /* 0x03 */
    U8      AllocRespLength;            /* 0x02 */
    U8      Function;                   /* 0x01 */
    U8      SMPFrameType;               /* 0x00 */

    U32     Reserved04;                 /* 0x04 */

    U8      Reserved0B;                 /* 0x0B */
    U8      Reserved0A;                 /* 0x0A */
    U8      PhyIdentifier;              /* 0x09 */
    U8      IgnoreZoneGroup     :1;     /* 0x08 */
    U8      Reserved08Bits2to7  :7;
} SMP_DISCOVER_REQUEST, *PTR_SMP_DISCOVER_REQUEST;

/*
 * SMP Discover response - native big endian format!!!
 */
typedef struct _SMP_DISCOVER_RESPONSE
{

    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            U16     Reserved06;                         /* 0x06 - 0x07 */
            U16     ExpanderChangeCount;                /* 0x04 - 0x05 */

            U8      Reserved0B;                         /* 0x0B */
            U8      Reserved0A;                         /* 0x0A */
            U8      PhyIdentifier;                      /* 0x09 */
            U8      Reserved08;                         /* 0x08 */

            U8     AttachedSATADevice              :1; /* 0x0F */
            U8     AttachedSMPTarget               :1;
            U8     AttachedSTPTarget               :1;
            U8     AttachedSSPTarget               :1;
            U8     StpBufferTooSmall               :1;
            U8     Reserved0FBits5to6              :2;
            U8     AttachedSATAPortSelector        :1;

            U8     AttachedSATAHost                :1; /* 0x0E */
            U8     AttachedSMPInitiator            :1;
            U8     AttachedSTPInitiator            :1;
            U8     AttachedSSPInitiator            :1;
            U8     Reserved0EBits4to7              :4;

            U8     NegotiatedLogicalLinkRate       :4; /* 0x0D */
            U8     Reserved0DBits4to7              :4;

            U8     AttachedReason                  :4; /* 0x0C */
            U8     AttachedDeviceType              :3;
            U8     Reserved0CBit7                  :1;

            SMP_SAS_ADDRESS     SASAddress;                 /* 0x10 - 0x17 */
            SMP_SAS_ADDRESS     AttachedSASAddress;         /* 0x18 - 0x1F */

            U8      Reserved23;                         /* 0x23 */

            U8      AttachedPwrDisCapable           :1; /* 0x22 */
            U8      AttachedSMPPrioCapable          :1;
            U8      AttachedAPTACapable             :1;
            U8      Reserved22Bits3to7              :5;

            U8     AttachedBreakReplyCapable       :1; /* 0x21 */
            U8     AttachedRequestedInsideZpsds    :1;
            U8     AttachedInsideZpsdsPersistent   :1;
            U8     AttachedPartialCapable          :1;
            U8     AttachedSlumberCapable          :1;
            U8     AttachedPowerCapable            :2;
            U8     Reserved21Bit7                  :1;
            U8      AttachedPhyIdentifier;              /* 0x20 */

            U32     Reserved24;                         /* 0x24 - 0x27 */

            U8     PartialPathTimeoutValue         :4; /* 0x2B */
            U8     Reserved2BBits4to6              :3;
            U8     VirtualPhy                      :1;

            U8      PhyChangeCount;                     /* 0x2A */

            U8     HardwareMaxPhysLinkRate         :4; /* 0x29 */
            U8     ProgMaxPhysLinkRate             :4;

            U8     HardwareMinPhysLinkRate         :4; /* 0x28 */
            U8     ProgMinPhysLinkRate             :4;

            U8      ConnectorPhysicalLink;              /* 0x2F */
            U8      ConnectorElementIndex;              /* 0x2E */

            U8     ConnectorType                   :7; /* 0x2D */
            U8     Reserved2DBit7                  :1;

            U8     RoutingAttribute                :4; /* 0x2C */
            U8     Reserved2CBits4to7              :4;

            U16     VendorSpecific;                     /* 0x32 - 0x33 */

            U8     SataPartialEnabled        :1;       /* 0x31 */
            U8     SataSlumberEnabled        :1;
            U8     SasPartialEnabled         :1;
            U8     SasSlumberEnabled         :1;
            U8      PwrDisControlCapable      :2;
            U8      PwrDisSignal              :2;

            U8     SataPartialCapable        :1;      /* 0x30 */
            U8     SataSlumberCapable        :1;
            U8     SasPartialCapable         :1;
            U8     SasSlumberCapable         :1;
            U8     SasPowerCapable           :2;
            U8     PhyPowerCondition         :2;

            SMP_SAS_ADDRESS     AttachedDeviceName;         /* 0x34 - 3B*/

            U8      ZoneGroup;                          /* 0x3F */
            U8      Reserved3E;                         /* 0x3E */
            U8      Reserved3D;                         /* 0x3D */
            U8     ZoningEnabled                   :1; /* 0x3C */
            U8     InsideZpsds                     :1;
            U8     ZoneGroupPersistent             :1;
            U8     Reserved3CBit3                  :1;
            U8     RequestedInsideZpsds            :1;
            U8     InsideZpsdsPersistent           :1;
            U8     ReqInsideZpsdsChangedByExp      :1;
            U8     Reserved3CBit7                  :1;

            U16     Reserved42;                         /* 0x42 - 0x43 */
            U8      SelfConfigLevelsCompleted;          /* 0x41 */
            U8      SelfConfigurationStatus;            /* 0x40 */

            SMP_SAS_ADDRESS     SelfConfigSasAddress;       /* 0x44 - 0x4B */

            U32     ProgrammedPhyCapabilities;          /* 0x4C - 0x4F */

            U32     CurrentPhyCapabilities;             /* 0x50 - 0x53 */

            U32     AttachedPhyCapabilities;            /* 0x54 - 0x57 */

            U32     Reserved58;                         /* 0x58 - 0x5B */

            U8     HardwareMuxingSupported         :1; /* 0x5F */
            U8     NegotiatedSSC                   :1;
            U8     OpticalModeEnabled              :1;
            U8     Reserved5FBits3to7              :5;
            U8     NegotiatedPhysicalLinkRate      :4; /* 0x5E */
            U8     Reason                          :4;
            U16     Reserved5C;                         /* 0x5C - 0x5D */

            U8      DefaultZoneGroup;                   /* 0x63 */
            U8      Reserved62;                         /* 0x62 */
            U8      Reserved61;                         /* 0x61 */

            U8     DefaultZoningEnabled        :1;     /* 0x60 */
            U8     Reserved60Bit1              :1;
            U8     DefaultZoneGroupPersistent  :1;
            U8     Reserved60Bit3              :1;
            U8     DefaultReqInsideZPSDS       :1;
            U8     DefaultInsideZPSDSPersistent    :1;
            U8     Reserved60Bit6to7               :2;

            U8      SavedZoneGroup;                     /* 0x67 */

            U8      Reserved66;                         /* 0x66 */
            U8      Reserved65;                         /* 0x65 */

            U8     SavedZoningEnabled          :1;     /* 0x64 */
            U8     Reserved64Bit1              :1;
            U8     SavedZoneGroupPersistent    :1;
            U8     Reserved64Bit3              :1;
            U8     SavedReqInsideZPSDS         :1;
            U8     SavedInsideZPSDSPersistent  :1;
            U8     Reserved64Bit6to7           :2;

            U8      ShadowZoneGroup;                    /* 0x6B */

            U8      Reserved6A;                         /* 0x6A */
            U8      Reserved69;                         /* 0x69 */

            U8     Reserved68Bit0to1           :2;     /* 0x68 */
            U8     ShadowZoneGroupPersistent   :1;
            U8     Reserved68Bit3              :1;
            U8     ShadowReqInsideZPSDS        :1;
            U8     ShadowInsideZPSDSPersistent :1;
            U8     Reserved68Bit6to7           :2;

            U8      DevSlotGrpOutputConnectorByte1;     /* 0x6F */
            U8      DevSlotGrpOutputConnectorByte0;     /* 0x6E */
            U8      DevSlotGrpNumber;                   /* 0x6D */
            U8      DevSlotNumber;                      /* 0x6C */

            U8      DevSlotGrpOutputConnectorByte2to5[4]; /* 0x70 - 0x73 */
            U8      Reserved6C;                                  /* 0x77 */
            U8      BufferedPhyBurstSize;                        /* 0x76 */
            U16     StpBufferSize;                               /* 0x74 - 0x75 */


        } Fields;

        U32                 Dword[29];                  /* 0x04 - 0x77*/
    } Response;

} SMP_DISCOVER_RESPONSE, *PTR_SMP_DISCOVER_RESPONSE,
    DISCOVER_LIST_LONG_DESCRIPTOR, *PTR_DISCOVER_LIST_LONG_DESCRIPTOR;


/* SMP Discover List Structure */

/* SMP response common structure after header!!! */
typedef struct  _DISCOVER_LIST_COMMON_RESP
{
    U16             Reserved06;                         /* 0x06 - 0x07 */
    U16             ExpanderChangeCount;                /* 0x04 - 0x05 */

    U8             DescriptorType      : 4;            /* 0x0B Bit 0 to 3*/
    U8             Reserved0BBit4to7   : 4;            /* 0x0B Bit 4 to 7 */
    U8             PhyFilter           : 4;            /* 0x0A Bit 0 to 3 */
    U8             Reserved0ABit4to7   : 4;            /* 0x0A Bit 4 to 7 */
    U8              NumberOfDescriptors;                /* 0x09 */
    U8              StartingPhyIdentifier;              /* 0x08 */

    U16             Reserved0E;                         /* 0x0E - 0x0F */
    U8              Reserved0D;                         /* 0x0D */
    U8              DescriptorLength;                   /* 0x0C */

    U16             LastSelfConfigStatusDescriptorIdx;  /* 0x12 - 0x13 */
    U8              Reserved11;                         /* 0x11 */
    U8             ExtConfigurableRouteTable  : 1;     /* 0x10 Bit 0 */
    U8             Configuring             : 1;        /* 0x10 Bit 1 */
    U8             ZoneConfiguring            : 1;     /* 0x10 Bit 2*/
    U8             SelfConfiguring            : 1;     /* 0x10 Bit 3*/
    U8             Reserved10Bit4to5          : 2;     /* 0x10 Bit 4 to 5 */
    U8             ZoningEnabled           : 1;        /* 0x10 Bit 6 */
    U8             ZonningSupported        : 1;        /* 0x10 Bit 7 */

    U16             Reserved16;                         /* 0x16 - 0x17 */
    U16             LastPhyEventListDescriptorIdx;      /* 0x14 - 0x15 */

    U32             Reserved18;                         /* 0x18 - 0x1B */
    U32             Reserved1C;                         /* 0x1C - 0x1F */

    U8              Vendorspecific[16];                 /* 0x20 - 0x2F */
} DISCOVER_LIST_COMMON_RESP;


/*
 * SMP Short Descovery List Descriptor - native big endian format!!!
 */
typedef union _DISCOVER_LIST_SHORT_DESCRIPTOR
{
    struct
    {
        U8             NegotiatedLogicalLinkRate   : 4;    /* 0x03 Bit 0 to 3*/
        U8             Restricted03Bit4to7         : 4;    /* 0x03 Bit 4 to 7 */

        U8             AttachedReason              : 4;    /* 0x02 Bit 0 to 3 */
        U8             AttachedDeviceType          : 3;    /* 0x02 Bit 4 to 6 */
        U8             Restricted02Bit7            : 1;    /* 0x02 Bit 7 */

        U8              FunctionResult;                     /* 0x01 */
        U8              PhyIdentifier;                      /* 0x00 */

        U8             NegotiatedPhysicalLinkRate  : 4;     /* 0x07 Bit 0 to 3*/
        U8             Reason                      : 4;     /* 0x07 Bit 4 to 7*/

        U8             RoutingAttribute            : 4;    /* 0x06 Byte 0 to 3*/
        U8             Reserved06Bit4to6           : 3;    /* 0x06 Byte 4 to 6*/
        U8             VirtualPhy                  : 1;    /* 0x06 Byte 7*/

        U8             AttachedSATATarget          : 1;    /* 0x05 */
        U8             AttachedSMPTarget           : 1;
        U8             AttachedSTPTarget           : 1;
        U8             AttachedSSPTarget           : 1;
        U8             StpBufferTooSmall           : 1;
        U8             Restricted05Bits5to6        : 2;
        U8             AttachedSATAPortSelector    : 1;

        U8             AttachedSATAHost            : 1;    /* 0x04 */
        U8             AttachedSMPInitiator        : 1;
        U8             AttachedSTPInitiator        : 1;
        U8             AttachedSSPInitiator        : 1;
        U8             Restricted04Bits4to7        : 4;

        U8              PhyChangeCount;                     /* 0x0B */
        U8              AttachedPhyIdentifier;              /* 0x0A */

        U8             Reserved09Bit0             : 1;     /* 0x09 Bit 0 */
        U8             InsideZPSDS                : 1;     /* 0x09 Bit 1 */
        U8             ZoneGroupPersistent        : 1;     /* 0x09 Bit 2 */
        U8             Reserved09Bit3             : 1;     /* 0x09 Bit 3 */
        U8             RequstedInsideZPSDS        : 1;     /* 0x09 Bit 4 */
        U8             InsideZPSDSPersistent      : 1;     /* 0x09 Bit 5 */
        U8             Restricted09Bit6to7        : 2;     /* 0x09 Bit 6 to7 */

        U8              ZoneGroup;                          /* 0x08 */

        SMP_SAS_ADDRESS     AttachedSASAddress;                 /* 0x0C - 0x13 */

        U8              Reserved15[ 3 ];                    /* Offset 0x17 - 0x15 */
        U8              BufferedPhyBurstSize;               /* Offset 0x14 */

    } Fields;

    U32                 Dword[6];                          /* 0x04 - 0x3C */
} DISCOVER_LIST_SHORT_DESCRIPTOR, *PTR_DISCOVER_LIST_SHORT_DESCRIPTOR;


/*
 * SMP Discover List request - native big endian format!!!
 */
typedef struct _SMP_DISCOVER_LIST_REQUEST
{
    U8      RequestLength;              /* 0x03 */
    U8      AllocRespLength;            /* 0x02 */
    U8      Function;                   /* 0x01 */
    U8      SMPFrameType;               /* 0x00 */

    U32     Reserved04to07;             /* 0x04 to 0x07 */

    U8     DiscriptorType      :4;     /* 0x0B */
    U8     Reserved0BBit4to7   :4;
    U8     PhyFilter           :4;     /* 0x0A */
    U8     Reserved0ABit4to6   :3;
    U8     IgnoreZoneGroup     :1;
    U8      MaxNumDiscriptors;          /* 0x09 */
    U8      StartPhyIdentifier;         /* 0x08 */

    U32     Reserved0Cto0F;             /* 0x0C to 0x0F */

    U32     VendorSpecific[3];          /* 0x10 to 0x1B */

} SMP_DISCOVER_LIST_REQUEST, *PTR_SMP_DISCOVER_LIST_REQUEST;


/* Macros used by Discover List response and discover list descriptor */
#define SMP_DISCOVER_LIST_RESPONSE_HEADER_SIZE   sizeof(U32)

#define SMP_DISCOVER_LIST_COMMON_RESP_SIZE  sizeof(DISCOVER_LIST_COMMON_RESP)

#define  MAX_DISCOVER_LIST_LONG_DESCRIPTOR  ((SMP_BUFFER_SIZE -\
    SMP_DISCOVER_LIST_RESPONSE_HEADER_SIZE -\
    SMP_DISCOVER_LIST_COMMON_RESP_SIZE) /\
    sizeof(DISCOVER_LIST_LONG_DESCRIPTOR))

#define  MAX_DISCOVER_LIST_SHORT_DESCRIPTOR ((SMP_BUFFER_SIZE -\
    SMP_DISCOVER_LIST_RESPONSE_HEADER_SIZE -\
    SMP_DISCOVER_LIST_COMMON_RESP_SIZE) /\
    sizeof(DISCOVER_LIST_SHORT_DESCRIPTOR))

#define SMP_DISCOVER_LIST_MAX_DESCRIPTOR_SIZE   MAX(\
    ( sizeof(DISCOVER_LIST_LONG_DESCRIPTOR) * MAX_DISCOVER_LIST_LONG_DESCRIPTOR ),\
    ( sizeof(DISCOVER_LIST_SHORT_DESCRIPTOR) * MAX_DISCOVER_LIST_SHORT_DESCRIPTOR ) )


/*
 * SMP Detailed Discover List response - native big endian format!!!
 */
typedef struct _SMP_DISCOVER_LIST_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;     /* 0x03 */
            U8              FunctionResult;     /* 0x02 */
            U8              Function;           /* 0x01 */
            U8              SMPFrameType;       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            DISCOVER_LIST_COMMON_RESP  CommonResp;

            union
            {
                DISCOVER_LIST_LONG_DESCRIPTOR
                    LongDescriptor[MAX_DISCOVER_LIST_LONG_DESCRIPTOR];
                DISCOVER_LIST_SHORT_DESCRIPTOR
                    ShortDescriptor[MAX_DISCOVER_LIST_SHORT_DESCRIPTOR];
            } Descriptors;

        } Fields;

        U32 Dword[ BYTES_TO_DWORDS( SMP_DISCOVER_LIST_COMMON_RESP_SIZE ) +
                   BYTES_TO_DWORDS( SMP_DISCOVER_LIST_MAX_DESCRIPTOR_SIZE )
                 ];
    } Response;

} SMP_DISCOVER_LIST_RESPONSE, *PTR_SMP_DISCOVER_LIST_RESPONSE;


/* Discover attached device types */
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_NO_DEVICES_ATTACHED   (0x0)
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_END_DEVICE_ONLY       (0x1)
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_EDGE_EXPANDER         (0x2)
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_FANOUT_EXPANDER       (0x3)

/* Discover SAS device attached */
#define SMP_DISCOVER_ATTACHED_SAS_DEVICE_BITS                   (0x00000e0e)

/* Discover negotiated physical link rate */
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_UNKNOWN_RATE         (0x0)
#define SMP_NEG_PHYS_LINK_RATE_PHY_DISABLED                     (0x1)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_SPEED_NEG_FAILED     (0x2)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_IN_SATA_SPINUP_HOLD  (0x3)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_AT_1_5_GBPS          (0x8)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_AT_3_0_GBPS          (0x9)

/* Discover hardware and programmed physical link rates */
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_NOT_PROGRAMMABLE        (0x0)
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_1_5_GBPS                (0x8)
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_3_0_GBPS                (0x9)
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_6_0_GBPS                (0xA)

/* Discover routing attributes */
#define SMP_DISCOVER_ROUTING_ATTRIBUTE_DIRECT_ROUTING           (0x0)
#define SMP_DISCOVER_ROUTING_ATTRIBUTE_SUBTRACTIVE_ROUTING      (0x1)
#define SMP_DISCOVER_ROUTING_ATTRIBUTE_TABLE_ROUTING            (0x2)

/*
 * Default values for Device Slot related fields reported in SMP Discover Response
 */
#define SMP_DISCOVER_NO_DEVICE_SLOT_NUMBER_AVAILABLE               0xFF
#define SMP_DISCOVER_NO_DEVICE_SLOT_GROUP_NUMBER_AVAILABLE         0xFF
#define SMP_DISCOVER_NO_DEVICE_SLOT_GROUP_CONNECTOR_AVAILABLE_BYTE 0x20

/*
 * SMP Report Phy Error Log request - native big endian format!!!
 */
typedef struct _SMP_REPORT_PHY_ERROR_LOG_REQUEST
{

    U8                      RequestLength;                  /* 0x03 */
    U8                      AllocRespLength;                /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U32                     Reserved04;                         /* 0x04 */

    U8                      Reserved0B;                         /* 0x0B */
    U8                      Reserved0A;                         /* 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved08;                         /* 0x08 */

} SMP_REPORT_PHY_ERROR_LOG_REQUEST, *PTR_SMP_REPORT_PHY_ERROR_LOG_REQUEST;


/*
 * SMP Report Phy Error Log response - native big endian format!!!
 */
typedef struct _SMP_REPORT_PHY_ERROR_LOG_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                         /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            U16            Reserved06;                              /* 0x06 - 0x07 */
            U16            ExpanderChangeCount;             /* 0x04 - 0x05 */

            U8              Reserved0B;                         /* 0x0B */
            U8              Reserved0A;                         /* 0x0A */
            U8              PhyIdentifier;                      /* 0x09 */
            U8              Reserved08;                         /* 0x08 */

            U32             InvalidDwordCount;                  /* 0x0C - 0x0F */
            U32             RunningDisparityErrorCount;         /* 0x10 - 0x13 */
            U32             LossOfDwordSynchronizationCount;    /* 0x14 - 0x17 */
            U32             PhyResetProblemCount;               /* 0x18 - 0x1B */

        } Fields;
        U32                 Dword[6];                           /* 0x04 - 0x1B */
    } Response;

} SMP_REPORT_PHY_ERROR_LOG_RESPONSE,
*PTR_SMP_REPORT_PHY_ERROR_LOG_RESPONSE;

/*
 * SMP Report Phy SATA request - native big endian format!!!
 */
typedef struct _SMP_REPORT_PHY_SATA_REQUEST
{

    U8                      RequestLength;                  /* 0x03 */
    U8                      AllocRespLength;                /* 0x02 */

    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U32                     Reserved04;                         /* 0x04 */

    U8                      Reserved0B;                         /* 0x0B */
    U8                      AffiliationContext;                /* 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved08;                         /* 0x08 */

} SMP_REPORT_PHY_SATA_REQUEST, *PTR_SMP_REPORT_PHY_SATA_REQUEST;

/*
 * SMP Report Phy SATA response - native big endian format!!!
 */
typedef struct _SMP_REPORT_PHY_SATA_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                         /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            U16            Reserved06;                              /* 0x06 - 0x07 */
            U16            ExpanderChangeCount;             /* 0x04 - 0x05 */

            U32             AffiliationValid                :1; /* 0x0B */
            U32             AffiliationsSupported           :1;
            U32             STPITNexusLossOccurred    :1;   /* New in SAS 2 */
            U32             Reserved0BBits3to7              :5;

            U8              Reserved0A;                         /* 0x0A */
            U8              PhyIdentifier;                      /* 0x09 */
            U8              Reserved08;                         /* 0x08 */

            U32             Reserved0C;                         /* 0x0C - 0x0F */
            SMP_SAS_ADDRESS     STPSASAddress;                      /* 0x10 - 0x17 */
            U32             RegisterDeviceToHostFIS[5];         /* 0x18 - 0x2B */
            U32             Reserved2C;                         /* 0x2C - 0x2F */
            SMP_SAS_ADDRESS     AffiliatedSTPInitiatorSASAddress;   /* 0x30 - 0x37 */

            SMP_SAS_ADDRESS     STPITNexusLossSASAddress;       /* 0x38 - 0x3F New in SAS 2 */
            U8               MaxAffiliationContexts;              /* 0x43 */
            U8               CurrentAffiliationContexts;        /* 0x42 */
            U8               AffiliationContext;                       /* 0x41 */
            U8               Reserved40;                               /* 0x40 */

        } Fields;
        U32                 Dword[16];                          /* 0x04 - 0x43 */
    } Response;

} SMP_REPORT_PHY_SATA_RESPONSE,
*PTR_SMP_REPORT_PHY_SATA_RESPONSE;
/*
 * SMP Report Route Information request - native big endian format!!!
 */
typedef struct _SMP_REPORT_ROUTE_INFO_REQUEST
{
    U8                      RequestLength;                  /* 0x03 */
    U8                      AllocRespLength;                /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U16                     ExpanderRouteIndex;                 /* 0x06 - 0x07 */
    U16                     Reserved04;                         /* 0x04 - 0x05 */

    U8                      Reserved0B;                         /* 0x0B */
    U8                      Reserved0A;                         /* 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved08;                         /* 0x08 */

} SMP_REPORT_ROUTE_INFO_REQUEST, *PTR_SMP_REPORT_ROUTE_INFO_REQUEST;

/*
 * SMP Report Route Information response - native big endian format!!!
 */
 typedef struct _SMP_REPORT_ROUTE_INFO_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                   /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            U16             ExpanderRouteIndex;                 /* 0x06 - 0x07 */
            U16             ExpanderChangeCount;              /* 0x04 - 0x05 */

            U8              Reserved0B;                         /* 0x0B */
            U8              Reserved0A;                         /* 0x0A */
            U8              PhyIdentifier;                      /* 0x09 */
            U8              Reserved08;                         /* 0x08 */

            U8              Reserved0F;                         /* 0x0F */
            U8              Reserved0E;                         /* 0x0E */
            U8              Reserved0D;                         /* 0x0D */
            U32             Reserved0CBits0to6              :7; /* 0x0C */
            U32             RouteEntryDisabled              :1;

            SMP_SAS_ADDRESS     RoutedSASAddress;                   /* 0x10 - 0x17 */

            U32             Reserved18;                         /* 0x18 - 0x1B */
            U32             Reserved1C;                         /* 0x1C - 0x1F */
            U32             Reserved20;                         /* 0x20 - 0x23 */
            U32             Reserved24;                         /* 0x24 - 0x27 */

        } Fields;
        U32                 Dword[9];                           /* 0x04 - 0x27 */
    } Response;

} SMP_REPORT_ROUTE_INFO_RESPONSE,
*PTR_SMP_REPORT_ROUTE_INFO_RESPONSE;


/*
 * SMP Configure Route Information request - native big endian format!!!
 */
typedef struct _SMP_CONFIGURE_ROUTE_INFO_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */          //NEW_SAS2
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U16                     ExpanderRouteIndex;                 /* 0x06 - 0x07 */
    U16                     ExpanderChangeCount;                /* 0x04 - 0x05 */   //NEW_SAS2

    U8                      Reserved0B;                         /* 0x0B */
    U8                      Reserved0A;                         /* 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved08;                         /* 0x08 */

    U8                      Reserved0F;                         /* 0x0F */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Reserved0D;                         /* 0x0D */
    U32                     Reserved0CBits0to6              :7; /* 0x0C */
    U32                     DisableRouteEntry               :1;

    SMP_SAS_ADDRESS             RoutedSASAddress;                   /* 0x10 - 0x17 */

    U32                     Reserved18;                         /* 0x18 - 0x1B */

    U32                     Reserved1C;                         /* 0x1C - 0x1F */

    U32                     Reserved20;                         /* 0x20 - 0x23 */

    U32                     Reserved24;                         /* 0x24 - 0x27 */
} SMP_CONFIGURE_ROUTE_INFO_REQUEST, *PTR_SMP_CONFIGURE_ROUTE_INFO_REQUEST;

/*
 * SMP Configure Route Information response - native big endian format!!!
 */
typedef struct _SMP_CONFIGURE_ROUTE_INFO_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                         /* 0x03 */  //NEW_SAS2
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;
} SMP_CONFIGURE_ROUTE_INFO_RESPONSE, *PTR_SMP_CONFIGURE_ROUTE_INFO_RESPONSE;


/*
 * SMP Phy Control request - native big endian format!!!
 */
typedef struct _SMP_PHY_CONTROL_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */          //NEW_SAS2
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U16                     Reserved06;                         /* 0x06 - 0x07 */
    U16                     ExpanderChangeCount;                /* 0x04 - 0x05 */   //NEW_SAS2

    U32                     UpdatePartialPathTimeoutValue   :1; /* 0x0B */
    U32                     Reserved0BBits1to7              :7;
    U8                      PhyOperation;                       /* 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved08;                         /* 0x08 */

    U32                     Reserved0C;                         /* 0x0C - 0x0F */

    U32                     Reserved10;                         /* 0x10 - 0x13 */

    U32                     Reserved14;                         /* 0x14 - 0x17 */

    SMP_SAS_ADDRESS             AttachedDeviceName;                 /* 0x18 - 0x1F */   //NEW_SAS2

    U8                      Reserved23Bits0to5       : 6;       /* 0x23 */
    U8                      PwrDisControl            : 2;

    U32                     EnableSataPartial        : 2;       /* 0x22 */
    U32                     EnableSataSlumber        : 2;
    U32                     EnableSasPartial         : 2;
    U32                     EnableSasSlumber         : 2;

    U32                     Reserved21Bits0to3              :4; /* 0x21 */
    U32                     ProgMaxPhysLinkRate             :4;
    U32                     Reserved20Bits0to3              :4; /* 0x20 */
    U32                     ProgMinPhysLinkRate             :4;

    U16                     Reserved26;                         /* 0x26 - 0x27 */
    U8                      Reserved25;                         /* 0x25 */
    U32                     PartialPathTimeoutValue         :4; /* 0x24 */
    U32                     Reserved24Bits4to7              :4;
} SMP_PHY_CONTROL_REQUEST, *PTR_SMP_PHY_CONTROL_REQUEST;

/* Phy Control Phy operation field */
#define SMP_PHY_CONTROL_PHY_OPERATION_NOP                       (0x00)
#define SMP_PHY_CONTROL_PHY_OPERATION_LINK_RESET                (0x01)
#define SMP_PHY_CONTROL_PHY_OPERATION_HARD_RESET                (0x02)
#define SMP_PHY_CONTROL_PHY_OPERATION_DISABLE                   (0x03)
#define SMP_PHY_CONTROL_PHY_OPERATION_CLEAR_ERROR_LOG           (0x05)
#define SMP_PHY_CONTROL_PHY_OPERATION_CLEAR_AFFILIATION         (0x06)
#define SMP_PHY_CONTROL_PHY_OPERATION_TRANSMIT_SATA_PSS         (0x07)
#define SMP_PHY_CONTROL_PHY_OPERATION_CLEAR_STP_I_T_NXS_LOSS    (0x08)
#define SMP_PHY_CONTROL_PHY_OPERATION_SET_ATTACHED_DEV_NAME     (0x09)

/* LSI vendor unique Phy Operation codes for PHY ACTIVE CONTROL SMP */
#define SMP_PHY_CONTROL_PHY_OPERATION_DISABLE_PMMANAGED         (0xF0)
#define SMP_PHY_CONTROL_PHY_OPERATION_ENABLE_PMMANAGED          (0xF1)

/* Phy Control programmed physical link rates */
#define SMP_PHY_CONTROL_PHYSICAL_LINK_RATE_DONT_CHANGE          (0x0)
#define SMP_PHY_CONTROL_PHYSICAL_LINK_RATE_1_5_GBPS             (0x8)
#define SMP_PHY_CONTROL_PHYSICAL_LINK_RATE_3_0_GBPS             (0x9)

#define SMP_PHY_CONTROL_PHY_POWER_CONDITION_ENABLE              (1)
#define SMP_PHY_CONTROL_PHY_POWER_CONDITION_DISABLE             (2)

/*
 * SMP Phy Control response - native big endian format!!!
 */
typedef struct _SMP_PHY_CONTROL_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */  //NEW_SAS2
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} SMP_PHY_CONTROL_RESPONSE, *PTR_SMP_PHY_CONTROL_RESPONSE;



/*
 * SMP Phy Test Function request - native big endian format!!!
 */
typedef struct _SMP_PHY_TEST_FUNCTION_REQUEST
{

    U8                      RequestLength;                      /* 0x03 */
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U16                     Reserved06;                         /* 0x06 - 0x07 */
    U16                     ExpanderChangeCount;                /* 0x04 - 0x05 */

    U8                      PhyTestPattern;                     /* 0x0B */
    U8                      PhyTestFunction;                    /* 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved08;                         /* 0x08 */

    U8                      PhyTestPhysLinkRate             :4; /* 0x0F */
    U8                      PhyTestPatternSSC               :2;
    U8                      PhyTestPatternSATA              :1;
    U8                      Reserved0fBit7                  :1;
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Reserved0D;                         /* 0x0D */
    U8                      Reserved0C;                         /* 0x0C */

    U8                      PhyTestPatternDwordContol;          /* 0x13 */
    U8                      Reserved12;                         /* 0x12 */
    U16                     Reserved10;                         /* 0x10 - 0x11 */

    U32                     PhyTestPatternDword0;                /* 0x14 - 0x1B */
    U32                     PhyTestPatternDword1;                /* 0x14 - 0x1B */

    U32                     Reserved1C;                         /* 0x1C - 0x1F */

    U32                     Reserved20;                         /* 0x20 - 0x23 */

    U32                     Reserved24;                         /* 0x24 - 0x27 */
} SMP_PHY_TEST_FUNCTION_REQUEST, *PTR_SMP_PHY_TEST_FUNCTION_REQUEST;


/* Phy Test Function field */
#define SMP_PHY_TEST_FUNCTION_STOP_PATTERN                      (0x00)
#define SMP_PHY_TEST_FUNCTION_START_PATTERN                     (0x01)
#define SMP_PHY_TEST_FUNCTION_VENDOR_SPECIFIC_F0                (0xF0)
#define SMP_PHY_TEST_FUNCTION_VENDOR_SPECIFIC_F1                (0xF1)
#define SMP_PHY_TEST_FUNCTION_VENDOR_SPECIFIC_F2                (0xF2)
#define SMP_PHY_TEST_FUNCTION_VENDOR_SPECIFIC_F3                (0xF3)

/* Phy Test Pattern field */
#define SMP_PHY_TEST_PATTERN_RESERVED_00                        (0x00)
#define SMP_PHY_TEST_PATTERN_JTPAT                              (0x01)
#define SMP_PHY_TEST_PATTERN_CJTPAT                             (0x02)
#define SMP_PHY_TEST_PATTERN_TRAIN                              (0x10)
#define SMP_PHY_TEST_PATTERN_TRAIN_DONE                         (0x11)
#define SMP_PHY_TEST_PATTERN_IDLE                               (0x12)
#define SMP_PHY_TEST_PATTERN_SCRAMBLED_0                        (0x13)
#define SMP_PHY_TEST_PATTERN_TWO_DWORDS                         (0x40)

/* Phy test pattern dword control masks.*/
#define SMP_PHY_TEST_DWCNT_SEND_FIFTHBYTE_KCHAR   0x80
#define SMP_PHY_TEST_DWCNT_SEND_FIRSTBYTE_KCHAR   0x08

/* Phy Test function ssc field */
#define SMP_PHY_TEST_NO_SSC                       (0)
#define SMP_PHY_TEST_CENTER_SPREADING_SSC         (1)
#define SMP_PHY_TEST_DOWN_SPREADING_SSC           (2)
#define SMP_PHY_TEST_SSC_RSVD                     (3)

/**
 * Phy test pattern control as defined in SPL-3.
 */
typedef enum _SMP_PHY_TEST_PATTERN_CONTROL
{
    /** CONTROL field 0x00: Each byte in the PHY TEST PATTERN
     * DWORDS field shall be sent as a data character (i.e., Dxx.y)
     * without scrambling.
     */
    SMP_PHY_TEST_PATTERN_DWORDS_CONTROL_00H = 0x00,
    /** CONTROL field 0x08: The fifth byte in the PHY TEST PATTERN
     * DWORDS field shall be sent as a control character (i.e., Kxx.y)
     * Each other byte shall be sent as a data character without
     * scrambling.
     */
    SMP_PHY_TEST_PATTERN_DWORDS_CONTROL_08H = 0x08,
    /** CONTROL field 0x80: The first byte in the PHY TEST PATTERN
     * DWORDS field shall be sent as a control character. Each other
     * byte shall be sent as a data character without scrambling.
     */
    SMP_PHY_TEST_PATTERN_DWORDS_CONTROL_80H = 0x80,
    /** CONTROL field 0x88: The first and fifth bytes in the PHY TEST
     * PATTERN DWORDS field shall each be sent as a control character.
     * Each other byte shall be sent as a data character without
     * scrambling.
     */
    SMP_PHY_TEST_PATTERN_DWORDS_CONTROL_88H = 0x88
}SMP_PHY_TEST_PATTERN_CONTROL;


/*
 * SMP Phy Test Function response - native big endian format!!!
 */

typedef struct _SMP_PHY_TEST_FUNCTION_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */  //NEW_SAS2
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} SMP_PHY_TEST_FUNCTION_RESPONSE, *PTR_SMP_PHY_TEST_FUNCTION_RESPONSE;


/*  Number of PHY event configuration decriptors reported in
 *  SMP REPORT PHY EVENT. request.
 *  Cobra hardware monitors 4 events per PHY.
 */
#define SMP_PHY_EVENT_NUM_DESC          4

/* SMP PHY EVENT RESPONSE DESCRIPTOR structure  */
typedef struct _SMP_PHY_EVENT_RESP_DESC
{
    U8                      EventSrc;                           /* 0x03 */

    /* PhyIdentifier is used only by REPORT PHY EVENT LIST. */
    U8                      PhyIdentifier;                      /* 0x02 */
    U8                      Reserved01[2];                      /* 0x01 -0x00 */
    U32                     EventCount;                         /* 0x07 - 0x04 */
    U32                     PvdThreshold;                       /* 0x0B - 0x08 */

} SMP_PHY_EVENT_RESP_DESC,
  *PTR_SMP_PHY_EVENT_RESP_DESC;

/*
 * SMP REPORT PHY EVENT request - native big endian format!!!
 */
typedef struct _SMP_REPORT_PHY_EVENT_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Reserved07[4];                      /* 0x07 - 0x04 */
    U8                      Reserved0B[2];                      /* 0x0B - 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved08;                         /* 0x08 */
} SMP_REPORT_PHY_EVENT_REQUEST, *PTR_SMP_REPORT_PHY_EVENT_REQUEST;


/*
 * SMP REPORT PHY EVENT response - native big endian format!!!
 */
#define  SMP_PHY_EVENT_BASE_RESP_DWORDS   3

#define  SMP_PHY_EVENT_RESP_DWORDS  ( SMP_PHY_EVENT_BASE_RESP_DWORDS +\
    BYTES_TO_DWORDS(sizeof(SMP_PHY_EVENT_RESP_DESC)*SMP_PHY_EVENT_NUM_DESC) )

typedef struct _SMP_REPORT_PHY_EVENT_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            /* Base response fields. */
            U16             Reserved06;                         /* 0x06 - 0x07 */
            U16             ExpanderChangeCount;                /* 0x04 - 0x05 */

            U16             Reserved0B;                         /* 0x0B - 0x0A */
            U8              PhyIdentifier;                      /* 0x09 */
            U8              Reserved08;                         /* 0x08 */

            U8              NumOfDesc;                          /* 0x0F */
            U8              DescLength;                         /* 0x0E */
            U16             Reserved0D;                         /* 0x0D - 0x0C */

            /* Event descriptors. */
            SMP_PHY_EVENT_RESP_DESC RespDesc[SMP_PHY_EVENT_NUM_DESC];
        } Fields;
        U32                 Dword[SMP_PHY_EVENT_RESP_DWORDS];
    } Response;

} SMP_REPORT_PHY_EVENT_RESPONSE,
*PTR_SMP_REPORT_PHY_EVENT_RESPONSE;


/*
 * SMP REPORT PHY EVENT LIST request - native big endian format!!!
 */
typedef struct _SMP_REPORT_PHY_EVENT_LIST_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */
    U16                     DescListStartIndex;                 /* 0x07 - 0x06 */
    U16                     Reserved05;                         /* 0x05 - 0x04 */
} SMP_REPORT_PHY_EVENT_LIST_REQUEST, *PTR_SMP_REPORT_PHY_EVENT_LIST_REQUEST;

/*
 * SMP REPORT PHY EVENT LIST response - native big endian format!!!
 */
#define SMP_PHY_EVENT_LIST_RESP_HEADER_SIZE   sizeof(U32)

#define  SMP_PHY_EVENT_LIST_BASE_RESP_DWORDS   3

#define  SMP_PHY_EVENT_LIST_RESP_MAX_NUM_DESC  ( (SMP_BUFFER_SIZE - \
    SMP_PHY_EVENT_LIST_RESP_HEADER_SIZE - DWORDS_TO_BYTES(SMP_PHY_EVENT_LIST_BASE_RESP_DWORDS))/ \
    sizeof(SMP_PHY_EVENT_RESP_DESC) )

#define  SMP_PHY_EVENT_LIST_RESP_DWORDS  ( SMP_PHY_EVENT_LIST_BASE_RESP_DWORDS +\
    BYTES_TO_DWORDS(sizeof(SMP_PHY_EVENT_RESP_DESC)*SMP_PHY_EVENT_LIST_RESP_MAX_NUM_DESC) )

typedef struct _SMP_REPORT_PHY_EVENT_LIST_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            /* Base response fields. */
            U16             FirstDescIndex;                     /* 0x06 - 0x07 */
            U16             ExpanderChangeCount;                /* 0x04 - 0x05 */

            U8              Reserved0B;                         /* 0x0B */
            U8              DescLength;                         /* 0x0A */
            U16             LastRecordedDescIndex;              /* 0x09 - 0x08*/

            U8              NumOfDesc;                          /* 0x0F */
            U8              Reserved0E[3];                      /* 0x0E -0x0C*/

            /* Event descriptors. */
            SMP_PHY_EVENT_RESP_DESC RespDesc[SMP_PHY_EVENT_LIST_RESP_MAX_NUM_DESC];
        } Fields;
        U32                 Dword[SMP_PHY_EVENT_LIST_RESP_DWORDS];
    } Response;

} SMP_REPORT_PHY_EVENT_LIST_RESPONSE,
  *PTR_SMP_REPORT_PHY_EVENT_LIST_RESPONSE;


/*
 * SMP CONFIGURE PHY EVENT request - native big endian format!!!
 */
#define SMP_PHY_CONFIG_EVENT_DESC_LEN_DWORD ( \
    BYTES_TO_DWORDS(sizeof(SMP_PHY_EVENT_CONFIG_DESC)) )

/* SMP PHY EVENT CONFIG DESCRIPTOR structure  */
typedef struct _SMP_PHY_EVENT_CONFIG_DESC
{
    U8                      EventSrc;                           /* 0x03 */
    U8                      Reserved02[3];                      /* 0x02 -0x00 */
    U32                     PvdThreshold;                       /* 0x07 - 0x04 */

} SMP_PHY_EVENT_CONFIG_DESC,
  *PTR_SMP_PHY_EVENT_CONFIG_DESC;


typedef struct _SMP_CONFIGURE_PHY_EVENT_REQUEST
{
    U8                      RequestLength;                      /* 0x03 */
    U8                      AllocRespLength;                    /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Reserved07;                            /* 0x07 */
    U8                      ClearPeaks    : 1;                  /* 0x06 */
    U8                      Reserved06Bits01to07 : 7;
    U16                     ExpanderChangeCount;                /* 0x04 - 0x05 */
    U8                      NumOfDesc;                          /* 0x0B */
    U8                      DescLength;                         /* 0x0A */
    U8                      PhyIdentifier;                      /* 0x09 */
    SMP_PHY_EVENT_CONFIG_DESC Desc[SMP_PHY_EVENT_NUM_DESC];

} SMP_CONFIGURE_PHY_EVENT_REQUEST, *PTR_SMP_CONFIGURE_PHY_EVENT_REQUEST;


/*
 * SMP CONFIGURE PHY EVENT response - native big endian format!!!
 */

typedef struct _SMP_CONFIGURE_PHY_EVENT_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;                     /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} SMP_CONFIGURE_PHY_EVENT_RESPONSE,
*PTR_SMP_CONFIGURE_PHY_EVENT_RESPONSE;


/*
 * SMP LSI WRITE_N_REGISTERS request - native big endian format!!!
 * note: used for AHB and register-ring mapped addresses
 */

#define MAX_SMP_WRITE_N ((1024/4)-2)    // number of dwords
#define MAX_SMP_READ_N  ((1024/4)-2)    // number of dwords

typedef struct _SMP_LSI_WRITE_N_REGISTERS_REQUEST
{

    U8                      N;                                  /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    union
    {
        struct
        {
            U8              EndByteEnables;                     /* 0x07 */
            U8              StartByteEnables;                   /* 0x06 */
            U16             StartAddr;                          /* 0x04 - 0x05 */
        } Ring;
        struct
        {
            U32             StartAddr;                          /* 0x04 - 0x07 */
        } Ahb;
    } Cmd;

    union
    {
        U32                 Dword[MAX_SMP_WRITE_N];             /* 0x08 - 0x3ff */
        U8                  Byte[4*MAX_SMP_WRITE_N];            /* 0x08 - 0x3ff */
    } WriteData;

} SMP_LSI_WRITE_N_REGISTERS_REQUEST, *PTR_SMP_LSI_WRITE_N_REGISTERS_REQUEST;

#define LSI_WRITE_N_AHB_BYTE_WRITE      (0x08000000)
#define LSI_WRITE_N_AHB_ADDR_MASK       (0xf7ffffff)

/*
 * SMP LSI WRITE_N_REGISTERS response - native big endian format!!!
 */

typedef struct _SMP_LSI_WRITE_N_REGISTERS_RESPONSE
{
    union
    {
        struct
        {
            U8              Reserved03;                         /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} SMP_LSI_WRITE_N_REGISTERS_RESPONSE, *PTR_SMP_LSI_WRITE_N_REGISTERS_RESPONSE;

/*
 * SMP LSI READ_N_REGISTERS request - native big endian format!!!
 * note: only used for register-ring mapped addresses
 */

typedef struct _SMP_LSI_READ_N_REGISTERS_REQUEST
{

    U8                      N;                                  /* 0x03 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    union
    {
        struct
        {
            U16             Reserved06;                         /* 0x06 - 0x07 */
            U16             StartAddr;                          /* 0x04 - 0x05 */
        } Ring;
        struct
        {
            U32             StartAddr;                          /* 0x04 - 0x07 */
        } Ahb;
    } Cmd;

} SMP_LSI_READ_N_REGISTERS_REQUEST, *PTR_SMP_LSI_READ_N_REGISTERS_REQUEST;

/*
 * SMP LSI READ_N_REGISTERS response - native big endian format!!!
 */

typedef struct _SMP_LSI_READ_N_REGISTERS_RESPONSE
{
    union
    {
        struct
        {
            U8              Reserved03;                         /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    struct
    {
        U32                 ReadData[MAX_SMP_READ_N];           /* 0x04 - 0x3ff */
    } Response;

} SMP_LSI_READ_N_REGISTERS_RESPONSE, *PTR_SMP_LSI_READ_N_REGISTERS_RESPONSE;

/*
 * SFF-8485 READ GPIO request - native big endian format !!!
 */

typedef struct _SMP_READ_GPIO_REGISTERS_REQUEST
{

    U8                      RegisterIndex;                      /* 0x03 */
    U8                      RegisterType;                       /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      Reserved05[3];                      /* 0x05 - 0x07 */
    U8                      RegisterCount;                      /* 0x04 */

} SMP_READ_GPIO_REGISTERS_REQUEST, *PTR_SMP_READ_GPIO_REGISTERS_REQUEST;

/*
 * SFF-8485 READ GPIO response - native big endian format !!!
 */

#define MAX_SMP_WRITE_GPIO_N                ((1024/4)-2)    // number of dwords
#define MAX_SMP_READ_GPIO_N                 ((1024/4)-2)    // number of dwords

typedef struct _SMP_READ_GPIO_REGISTERS_RESPONSE
{
    union
    {
        struct
        {
            U8              Reserved03;                         /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    struct
    {
        U32                 ReadData[MAX_SMP_READ_GPIO_N];          /* 0x04 - 0x3ff */
    } Response;

} SMP_READ_GPIO_REGISTERS_RESPONSE, *PTR_SMP_READ_GPIO_REGISTERS_RESPONSE;

/*
 * SFF-8485 WRITE GPIO request - native big endian format !!!
 */

typedef struct _SMP_WRITE_GPIO_REGISTERS_REQUEST
{

    U8                      RegisterIndex;                      /* 0x03 */
    U8                      RegisterType;                       /* 0x02 */
    U8                      Function;                           /* 0x01 */
    U8                      SMPFrameType;                       /* 0x00 */

    U8                      Reserved05[3];                      /* 0x05 - 0x07 */
    U8                      RegisterCount;                      /* 0x04 */

    U32                     WriteData[MAX_SMP_WRITE_GPIO_N];    /* 0x08-... */

} SMP_WRITE_GPIO_REGISTERS_REQUEST, *PTR_SMP_WRITE_GPIO_REGISTERS_REQUEST;

typedef struct _SMP_WRITE_GPIO_REGISTERS_RESPONSE
{
    union
    {
        struct
        {
            U8              Reserved03;                         /* 0x03 */
            U8              FunctionResult;                     /* 0x02 */
            U8              Function;                           /* 0x01 */
            U8              SMPFrameType;                       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

} SMP_WRITE_GPIO_REGISTERS_RESPONSE, *PTR_SMP_WRITE_GPIO_REGISTERS_RESPONSE;


/*
 *  SMP REPORT EXPANDER ROUTE TABLE List request - native big endian format!!!
 */
typedef struct _SMP_REPORT_EXP_ROUTE_TABLE_LIST_REQUEST
{
    U8                      RequestLength;                  /* 0x03 */
    U8                      AllocRespLength;                /* 0x02 */
    U8                      Function;                       /* 0x01 */
    U8                      SMPFrameType;                   /* 0x00 */

    U32                     Reserved04;                     /* 0x04 - 0x07 */

    U16                     StartingRoutedSASAddressIdx;    /* 0x0A - 0x0B */
    U16                     MaxNumDescriptors;              /* 0x08 - 0x09 */

    U32                     Reserved0C;                     /* 0x0C - 0x0F */

    U8                      StartingPhyId;                  /* 0x13 */
    U8                      Reserved12;                     /* 0x12 */
    U16                     Reserved10;                     /* 0x10 */

    U32                     Reserved14;                     /* 0x14 - 0x17 */

    U32                     Reserved18;                     /* 0x18 - 0x0B */
} SMP_REPORT_EXP_ROUTE_TABLE_LIST_REQUEST,
    *PTR_SMP_REPORT_EXP_ROUTE_TABLE_LIST_REQUEST;


/*
 * SMP Report Exp Route Table List response - common structure after header!!!
 */
typedef struct _SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_INFO
{
    U16     ExpanderRouteTableChangeCount;          /* 0x06 - 0x07 */
    U16     ExpanderChangeCount;                    /* 0x04 - 0x05 */

    U8      NumOfExpRouteTableDescriptors;          /* 0x0B */
    U8      ExpRouteTableDescriptorLength;          /* 0x0A */
    U8      Reserved09;                             /* 0x09 */
    U8      ZoningEnabled       : 1;                /* 0x08 Bit 0 */
    U8      Configuring         : 1;                /* 0x08 Bit 1 */
    U8      ZoneConfiguring     : 1;                /* 0x08 Bit 2 */
    U8      SelfConfiguring     : 1;                /* 0x08 Bit 3 */
    U8      Reserved08Bit4to7   : 4;                /* 0x08 Bits 4 to 7 */

    U16     LastRoutedSASAddrIdx;                   /* 0x0E */
    U16     FirstRoutedSASAddrIdx;                  /* 0x0C */

    U8      StartingPhyIdentifier;                  /* 0x13 */
    U8      Reserved12;                             /* 0x12 */
    U16     Reserved10;                             /* 0x10 - 0x11 */

    U32     Reserved14;                             /* 0x14 - 0x17 */
    U32     Reserved18;                             /* 0x18 - 0x1B */
    U32     Reserved1C;                             /* 0x1C - 0x1F */

} SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_INFO,
    *PTR_SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_INFO;


/*
 *  REPORT EXPANDER ROUTE TABLE List response descriptors
 */
typedef struct _SMP_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR
{
    SMP_SAS_ADDRESS     RoutedSASAddress;                   /* 0x00 - 0x07 */

    U32             PhyBitMapBit16to47;                 /* 0x08 - 0x0B */

    U8              ZoneGroup;                          /* 0x0F */
    U8              Reserved0E;                         /* 0x0E */
    U16             PhyBitMapBit0to15;                  /* 0x0C - 0x0D */
} SMP_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR,
    *PTR_SMP_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR;


#define SMP_REPORT_EXP_ROUTE_TABLE_LIST_RESPONSE_HEADER_SIZE    sizeof(U32)
#define MAX_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR  \
    ( (SMP_BUFFER_SIZE - SMP_REPORT_EXP_ROUTE_TABLE_LIST_RESPONSE_HEADER_SIZE - \
        sizeof(SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_INFO)) /\
            sizeof(SMP_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR) )

#define GET_NUM_OF_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR( AllocRespLength )  \
    ( ( ((AllocRespLength) * 4) - \
        sizeof(SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_INFO)) /\
            sizeof(SMP_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR) )

#define SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_RESP_SIZE  \
    sizeof(SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_INFO)

#define SMP_REPORT_EXP_ROUTE_TABLE_LIST_MAX_DESCRIPTOR_SIZE   \
    ( sizeof(SMP_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR) * \
        MAX_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR )


/*
 *  SMP REPORT EXPANDER ROUTE TABLE List response
 */
typedef struct _SMP_REPORT_EXP_ROUTE_TABLE_LIST_RESPONSE
{
    union
    {
        struct
        {
            U8              ResponseLength;     /* 0x03 */
            U8              FunctionResult;     /* 0x02 */
            U8              Function;           /* 0x01 */
            U8              SMPFrameType;       /* 0x00 */
        } Fields;
        U32                 Dword;
    } Header;

    union
    {
        struct
        {
            SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_INFO  CommonResp;

            SMP_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR
                descriptorList[MAX_REPORT_EXP_ROUTE_TABLE_LIST_DESCRIPTOR];

        } Fields;

        U32 Dword[ BYTES_TO_DWORDS(
                        SMP_REPORT_EXP_ROUTE_TABLE_LIST_COMMON_RESP_SIZE ) +
                   BYTES_TO_DWORDS(
                        SMP_REPORT_EXP_ROUTE_TABLE_LIST_MAX_DESCRIPTOR_SIZE )
                 ];
    } Response;

} SMP_REPORT_EXP_ROUTE_TABLE_LIST_RESPONSE,
    *PTR_SMP_REPORT_EXP_ROUTE_TABLE_LIST_RESPONSE;


/* This section contains vendor unique SMP commands for the LSI Sasquatch expander */

#define SMP_SGPIO_REGISTER_TYPE_CFG         (0)
#define SMP_SGPIO_REGISTER_TYPE_RX          (1)
#define SMP_SGPIO_REGISTER_TYPE_RX_GP       (2)
#define SMP_SGPIO_REGISTER_TYPE_TX          (3)
#define SMP_SGPIO_REGISTER_TYPE_TX_GP       (4)


#endif /* __SMP__FUNCTIONS__H__ */

