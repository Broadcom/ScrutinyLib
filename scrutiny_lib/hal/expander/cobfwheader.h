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


#ifndef _FW_HEADER_H
#define _FW_HEADER_H

/** @addtogroup fwHeader Firmware Header
 *  @{ */


/** Maximum number of flash region supported */
#define FW_MAX_NUM_OF_FLASH_REGIONS 11

/** Structure for firmware version.*/
typedef struct _FW_VERSION_STRUCT
{
    /** Developement version. */
    U8                      Dev;                        /* 00h */
    /** Unit version. */
    U8                      Unit;                       /* 01h */
    /** Minor version. */
    U8                      Minor;                      /* 02h */
    /** Major version. */
    U8                      Major;                      /* 03h */
} FW_VERSION_STRUCT;

/** Dword or structured access to firmware version.*/
typedef union _FW_VERSION
{
    /** Structured access to firmware version. */
    FW_VERSION_STRUCT   Struct;
    /** Word access to firmware version. */
    U32                 Word;
} FW_VERSION, * PTR_FW_VERSION;

/** Structure for firmware header*/
typedef struct _FW_HEADER
{
    /** ARM branch instruction. */
    U32                     ArmBranchInstruction0;               /* 00h */
    /** Firmware signature : #MPI_FW_HEADER_SIGNATURE_0. */
    U32                     Signature0;                          /* 04h */
    /** Firmware signature : #MPI_FW_HEADER_SIGNATURE_1. */
    U32                     Signature1;                          /* 08h */
    /** Firmware signature : #MPI_FW_HEADER_SIGNATURE_2. */
    U32                     Signature2;                          /* 0Ch */
    /** Reserved. */
    U32                     Reserved10;                          /* 10h */
    /** Additional header length. */
    U32                     AdditionalHeaderLength;              /* 14h */
    /** Firmware image checksum. */
    U32                     Checksum;                            /* 18h */
    /** New image signature 1. */
    U32                     NewImageSignature1;                  /* 1Ch */
    /** New image signature 2. */
    U32                     NewImageSignature2;                  /* 20h */
    /** Firmware version. */
    FW_VERSION              FWVersion;                           /* 24h */
    /** Firmware family. */
    U16                     FwFamily;                            /* 28h */
    /** Reserved. */
    U16                     Reserved2A;                          /* 2Ah */
    /** OEM family. */
    U16                     OEMFamily;                           /* 2Ch */
    /** Reserved. */
    U16                     Reserved2E;                          /* 2Eh */
    /** Header flags. */
    U32                     HeaderFlags;                         /* 30h */
    /** Additional firmware information offset. */
    U32                     AdditionalFWInfoOffset;              /* 34h */
    /** Firmware image size. */
    U32                     FwImageSize;                         /* 38h */
    /** Base address of first entry point. */
    U32                     FirstRunBase;                        /* 3Ch */
    /** Offset of first entry point. */
    U32                     FirstRunOffset;                      /* 40h */
    /** Base address of second entry point. */
    U32                     SecondRunBase;                       /* 44h */
    /** Offset of second entry point. */
    U32                     SecondRunOffset;                     /* 48h */
    /** Offset of next image. */
    U32                     NextImageOffset;                     /* 4Ch */
    /** Offset of flash table. */
    U32                     FlashTableOffset;                    /* 50h */

} FW_HEADER, * PTR_FW_HEADER;

#define COBRA_FW_HEADER_END_OF_NEXT_OFFSET  (0xFFFFFFFF)

#define  COBRA_HAS_NEXT_CHAIN_IMAGE(Offset) ((Offset != 0) && (Offset != COBRA_FW_HEADER_END_OF_NEXT_OFFSET))


#define MPI_FW_HEADER_SIGNATURE_0       ( 0xC0EABAA0 )  /**< Firmware header signature 0. */
#define MPI_FW_HEADER_SIGNATURE_1       ( 0xC0BAEAA0 )  /**< Firmware header signature 1. */
#define MPI_FW_HEADER_SIGNATURE_2       ( 0xC0BAA0EA )  /**< Firmware header signature 2. */


/*
 * Signature value that can be used by the "what" utility to locate string
 * information in the image.
 */
#define FW_HEADER_WHAT_SIGNATURE        ( 0x29232840 )

/** Length of firmware name. */
#define FW_NAME_LENGTH  32

/** Structure for firmware additional name information.*/
typedef struct _FW_ADDITIONAL_INFO_NAME
{
    /** Length in bytes. */
    U8  Length;
    /** Reserved. */
    U8  Reserved1;
    /** Reserved. */
    U16 Reserved2;
    /** Signature. */
    U32 Signature;
    /** Character string. */
    U8  Name[FW_NAME_LENGTH];
} FW_ADDITIONAL_INFO_NAME, * PTR_FW_ADDITIONAL_INFO_NAME;

/** Structure for firmware additional information.*/
typedef struct _FW_ADDITIONAL_INFO
{
    /** Additional information's version. */
    U32                     AdditionalInfoVersion;
    /** Version name. */
    FW_ADDITIONAL_INFO_NAME VersionName;
    /** Vendor name. */
    FW_ADDITIONAL_INFO_NAME VendorName;
} FW_ADDITIONAL_INFO, * PTR_FW_ADDITIONAL_INFO;

/*
 * Chained header related declarations.
 */

/**
 * First DWORD of chained header. Identifies header type and revision.
 */
typedef struct _CHAINED_HEADER_IDENTIFIER
{
    /**
     * Identifies chained header revision.
     * In case the header format gets updated, this revision should be incremented.
     */
     U8                      ChainedHeaderRevision;            /* 00h */

    /** LSI reserved field. Set to zero. */
     U8                      Reserved01;                       /* 01h */
    /** LSI reserved field. Set to zero. */
    U8                      Reserved02;                       /* 02h */

    /**
     * Identifies header type. Could be one of
     * - #HEADER_TYPE_FIRMWARE
     * - #HEADER_TYPE_OEM
     * - #HEADER_TYPE_LSI
     */
     U8                      HeaderType;                       /* 03h */

} CHAINED_HEADER_IDENTIFIER, *PTR_CHAINED_HEADER_IDENTIFIER;

/**
 * Second DWORD of chained header. Identifies image type.
 */
typedef struct _CHAINED_HDR_IMAGE_IDENTIFIER
{
    /**
     * Preferred buffer ID for downloading this child image. Provides guidance on
     * how write buffer mechanism should treat this child image. Provides flexibility
     * of change of write buffer behaviour by modifying concatenated header without
     * needing modification to the running firmware.
     * Note that use of this buffer ID is 'preferred' and not 'mandatory'.
     * Platform specific policies may be applied for specific behaviour depending
     * on requirements of platform.
     */
    U8                          PreferredDownloadBufferId;        /* 00h */

    /**
     * Concatenated image 'image type'.
     * Signature check for concatenated image happens based on this image type.
     * - #CONCAT_IMAGE_TYPE_MFG
     */
    U8                          ImageType;                        /* 01h */

    /** LSI reserved field. Set to 0xFF. */
    U8                          Reserved02;                       /* 02h */
    /** LSI reserved field. Set to 0xFF. */
    U8                          Reserved03;                       /* 03h */

} CHAINED_HDR_IMAGE_IDENTIFIER, *PTR_CHAINED_HDR_IMAGE_IDENTIFIER;

/**
 * Chained header to be used for concatenation of images.
 * LSI might redefine 'LSI reserved' fields at a later stage if needed.
 * OEM reserved fields will be never redefined by LSI and will not be validated by LSI
 * SDK/LSI tools. OEMs can define these safely.
 */
typedef struct _CHAINED_HEADER
{
    /** This field identifies chained header revision & type.*/
    CHAINED_HEADER_IDENTIFIER    ChainedHeaderId;               /* 00h */
    /** This field identifies chained image type. */
    CHAINED_HDR_IMAGE_IDENTIFIER ImageIdentifier;               /* 04h */
    /** This field stores data specific to an image type. For example: signature. */
    U32                          IdSpecificData;                /* 08h */
    /** LSI reserved field. */
    U32                          LSIReserved0C;                 /* 0Ch */
    /** LSI reserved field. */
    U32                          LSIReserved10;                 /* 10h */
    /** Additional header length. */
    U32                          AdditionalHeaderLength;        /* 14h */
    /** Image checksum. */
    U32                          Checksum;                      /* 18h */
    /** LSI reserved field. */
    U32                          LSIReserved1C;                 /* 1Ch */
    /** LSI reserved field. */
    U32                          LSIReserved20;                 /* 20h */
    /** Firmware version. */
    FW_VERSION                   ContentsVersion;               /* 24h */
    /** Family to which firmware belongs to. */
    U16                          FwFamily;                      /* 28h */
    /** Reserved field. */
    U16                          Reserved2A;                    /* 2Ah */
    /** OEM family. */
    U16                          OEMFamily;                     /* 2Ch */
    /** Reserved field. */
    U16                          Reserved2E;                    /* 2Eh */
    /** Header flags. */
    U32                          HeaderFlags;                   /* 30h */
    /** LSI reserved field. */
    U32                          LSIReserved34;                 /* 34h */
    /** Image size in dwords. */
    U32                          ImageSize;                     /* 38h */
    /** Offset to content. */
    U32                          OffsetToContent;               /* 3Ch */
    /** LSI reserved field. */
    U32                          LSIReserved40;                 /* 40h */
    /** OEM reserved field. */
    U32                          OemReserved44;                 /* 44h */
    /** OEM reserved field. */
    U32                          OemReserved48;                 /* 48h */
    /** Next image offset. */
    U32                          NextImageOffset;               /* 4Ch */
    /** OEM reserved field. */
    U32                          OemReserved50;                 /* 50h */

} CHAINED_HEADER, *PTR_CHAINED_HEADER;


/*
 * Concatenated image 'header type'.
 * This field can be used for differentiating a firmware image from concatenated LSI
 * image from concatenated OEM image.
 */

/** Chained Header Identifier type value - Firmware Image
 * Identifies firmware header. Hex 0xEA identifies the ARM branch instruction
 * at the beginning of firmware header.
 */
#define HEADER_TYPE_FIRMWARE   0xEA

/** Chained Header Identifier type value - OEM image
 * The header is OEM specific concatented image header.
 */
#define HEADER_TYPE_OEM        0x7F

/** Chained Header Identifier type value - LSI reserved image
 * The header is LSI specific concatented image header. For example there could
 * be LSI style MFG image concatenated using this header.
 */
#define HEADER_TYPE_LSI        0xFF


/*
 * Concatenated image 'image types'.
 * Signature for a concatenated image corresponds to Image Type.
 * In case more LSI Image Types are added, those could be added here
 */

/** Chained Header Image Identifier value for LSI format
 * Manufacturing Configuration Image.
 */
#define CONCAT_IMAGE_TYPE_MFG  0x01

/**
 * Signature DWORD 2 value (IdSpecificData field) for concatenated MFG image.
 */
#define CONCAT_IMAGE_MFG_SIGNATURE ( 0x03FCDA7A )

/** Maximum number of entries in flash table. */
#define MAX_FLASH_TABLE_ENTRIES 11

/**
 * Structure for Flash Table.
 * This table describes all the different flash regions. The regions
 * can occur in any order within the table.
 */
typedef struct _FLASH_TABLE
{
    /** Flash table version.*/
    U16 FlashTableVersion;
    /** Number of flash table entries.*/
    U8  NumFlashTableEntries;
    /** reserved.*/
    U8  Reserved;
    /** Structure for flash region entry.*/
    struct _FLASH_REGION_ENTRY
    {
        /** Flash region type.*/
        U8  RegionType;
        /** Reserved.*/
        U8  Reserved[3];
        /** Region size.*/
        U32 RegionSize;
        /** Region offset.*/
        U32 RegionOffset;
    } FlashRegionEntries[MAX_FLASH_TABLE_ENTRIES];

} FLASH_TABLE, * PTR_FLASH_TABLE;


/* This symbol indicates the signature of Manufacturing Image.*/
#define HAL_CFG_MFG_REGION                         0x10

////signature for firmware validation.
//#define MPI_FW_HEADER_SIGNATURE_0                       (0xC0EABAA0)
//#define MPI_FW_HEADER_SIGNATURE_1                       (0xC0BAEAA0)
//#define MPI_FW_HEADER_SIGNATURE_2                       (0xC0BAA0EA)


typedef U16     HALI_CFG_PAGE_ID;

/** Part of the Config page header which is set by the user */
typedef struct _HALI_CFG_PAGE_VISIBLE_HEADER
{
    U16               PageLength; /**< 16bit length (multiple of 4) of page */
    HALI_CFG_PAGE_ID  PageId;     /**< Unique ID to identify the page */
} HALI_CFG_PAGE_VISIBLE_HEADER, *PTR_HALI_CFG_PAGE_VISIBLE_HEADER;

/** Config page header flags */
typedef union _HAL_CFG_PAGE_HEADER_FLAGS
{
    U8 Byte;

    struct
    {
        U8 PageValid     : 1; /**< Bit clear when page is NOT valid */
        U8 PageWriting   : 1; /**< Bit clear when page is being written */
        U8 PageWriteDone : 1; /**< Bit clear when page writing is done*/
        U8 Reserved      : 5;
    } Bits;

} HAL_CFG_PAGE_HEADER_FLAGS, *PTR_HAL_CFG_PAGE_HEADER_FLAGS;

/** Part of the Config page header which is not visible to the user */
typedef struct _HAL_CFG_PAGE_HIDDEN_HEADER
{
    HAL_CFG_PAGE_HEADER_FLAGS    PageFlags;
    U8                           PageChecksum;
    U16                          Reserved;
} HAL_CFG_PAGE_HIDDEN_HEADER, *PTR_HAL_CFG_PAGE_HIDDEN_HEADER;

/** Config page header is made of HIDDEN & VISIBLE headers */
typedef struct _HAL_CFG_PAGE_HEADER
{
    HAL_CFG_PAGE_HIDDEN_HEADER    HiddenHdr;
    HALI_CFG_PAGE_VISIBLE_HEADER  VisibleHdr;
} HAL_CFG_PAGE_HEADER, *PTR_HAL_CFG_PAGE_HEADER;


#define HAL_CFG_INVALID_PAGE_ID         0xFFFF
#define HAL_CFG_EMPTY_PAGE_ID           0x0000
#define HAL_CFG_INVALID_PAGE_LENGTH     0xFFFF
#define HAL_CFG_EMPTY_PAGE_LENGTH       0x0000
#define HAL_CFG_REGION_VERSION    (0x04)
#define HAL_CFG_MAX_RETRIES       (0x03)
#define HAL_CFG_REGION_HEADER_FLAG_REGIONVALID     (0x01)
#define HAL_CFG_REGION_HEADER_FLAG_CREATING        (0x02)
#define HAL_CFG_REGION_HEADER_FLAG_CREATEDONE      (0x04)
#define HAL_CFG_REGION_HEADER_FLAG_PACKING         (0x08)
#define HAL_CFG_REGION_HEADER_FLAG_PACKDONE        (0x10)
#define HAL_CFG_PAGE_HEADER_FLAG_PAGEVALID    (0x01)
#define HAL_CFG_PAGE_HEADER_FLAG_WRITING      (0x02)
#define HAL_CFG_PAGE_HEADER_FLAG_WRITEDONE    (0x04)
#define HAL_CFG_DATA_EQUIV_HIDDEN_HEADER_SIZE      (0xFFFFFFFF)
#define HAL_CFG_REGION_HEADER_FLAGS_ON_CREATE  \
    ((U8)~HAL_CFG_REGION_HEADER_FLAG_CREATING)
#define HAL_CFG_PAGE_HEADER_FLAGS_ON_WRITE     \
    ((U8)~HAL_CFG_PAGE_HEADER_FLAG_WRITING)
typedef enum _HALI_CFG_REGION_TYPE
{
    HALI_CFG_ANY_REGION          = (0x00),
    HALI_CFG_MFG_REGION          = (0x10),
    HALI_CFG_PERSISTENT_REGION   = (0x20),
    HALI_CFG_INVALID_REGION      = (0xff)
} HALI_CFG_REGION_TYPE;
typedef union _HAL_CFG_REGION_HEADER_FLAGS
{
    U8 Byte;
    struct
    {
        U8  RegionValid    : 1; /**< Bit clear when region is NOT valid */
        U8  Creating       : 1; /**< Bit clear indicates creating started */
        U8  CreateDone     : 1; /**< Bit clear indicates creation done */
        U8  Packing        : 1; /**< Bit clear indicates migration started */
        U8  PackDone       : 1; /**< Bit clear indicates migration done */
        U8  Reserved       : 3;
    } Bits;
} HAL_CFG_REGION_HEADER_FLAGS, *PTR_HAL_CFG_REGION_HEADER_FLAGS;
typedef struct _HAL_CFG_REGION_HEADER
{
    HAL_CFG_REGION_HEADER_FLAGS RegionHdrFlags; /* Current status flags */
    U8                          RegionVersion;  /* Current version */
    U8                          RegionType;     /* PERSISTENT/MFG/Both regions */
    U8                          FirstPageOffset;/* Offset to first page in region*/
    U16                         Reserved;
    U16                         RegionSize;     /* Total size of the region */
} HAL_CFG_REGION_HEADER, *PTR_HAL_CFG_REGION_HEADER;


#define     HALI_FLASH_BOOT_LOADER      (0x00)  /**< Flash Region Id for Boot Loader Image. */
#define     HALI_FLASH_FIRMWARE_COPY_1  (0x01)  /**< Flash Region Id for Firmware Image 1 */
#define     HALI_FLASH_FIRMWARE_COPY_2  (0x02)  /**< Flash Region Id for Firmware Image 2 */
#define     HALI_FLASH_FIRMWARE_ACTIVE  (0x01)  /**< Flash Region Id for Active Image of Firmware */
#define     HALI_FLASH_FIRMWARE_BACKUP  (0x02)  /**< Flash Region Id for Backup Image of Firmware */
#define     HALI_FLASH_MANUFACTURING    (0x03)  /**< Flash Region Id for Manufacturing Page Image */
#define     HALI_FLASH_CONFIG_1         (0x04)  /**< Flash Region Id for Config Pages, At a time One
                                                     Config Region is Active */
#define     HALI_FLASH_CONFIG_2         (0x05)  /**< Flash Region Id for Config Pages */
#define     HALI_FLASH_LOG_1            (0x06)  /**< Flash Region Id for Logs, At a time one Log
                                                     Region is Active */
#define     HALI_FLASH_LOG_2            (0x07)  /**< Flash Region Id for Logs */
#define     HALI_FLASH_OEM_1            (0x08)  /**< Flash Region Id for OEM related Data */
#define     HALI_FLASH_OEM_2            (0x09)  /**< Flash Region Id for OEM related Data */
#define     HALI_FLASH_COREDUMP         (0x0A)  /**< Flash Region Id for coredump data */
#define     HALI_FLASH_INVALID          (0xFF)  /**< Invalid Region */

#define RW_BUFFER_NETWORK_IPV6_RESERVE     (0x24)

typedef struct _RW_BUFFER_NETWORK_INFO
{
    /** Mac address */
    U8  MacAddress[6];

    /** Whether DHCP is currently enabled */
    U8  DhcpEnabled     :1;
    /** Whether AutoIp is currently enabled */
    U8  AutoIpEnabled   :1;
    U8  Reserved0       :6;

    /** Whether the link is up, down, or negotiating */
    U8  LinkState       :2;
    /** The ethernet link speed */
    U8  Speed           :2;
    /** Whether the link is full or half duplex */
    U8  Duplex          :1;
    U8  Reserved1       :3;

    /** Ip address */
    U32  Ipv4Address;
    /** Subnet mask */
    U32  Ipv4SubNetMask;
    /** Default gateway */
    U32  Ipv4GatewayAddress;

    /** Reserved for IPv6 information */
    U8  Reserved2[RW_BUFFER_NETWORK_IPV6_RESERVE];
} RW_BUFFER_NETWORK_INFO, *PTR_RW_BUFFER_NETWORK_INFO;


/**@}*/
#endif



