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


#ifndef _ATLAS_FW_HEADER_H
#define _ATLAS_FW_HEADER_H

/** @addtogroup fwHeader Firmware Header
 *  @{ */


/** Maximum number of flash region supported */
#define ATLAS_FW_MAX_NUM_OF_FLASH_REGIONS 11

/** Structure for firmware version.*/
typedef struct _ATLAS_FW_VERSION_STRUCT
{
    /** Developement version. */
    U8                      Dev;                        /* 00h */
    /** Unit version. */
    U8                      Unit;                       /* 01h */
    /** Minor version. */
    U8                      Minor;                      /* 02h */
    /** Major version. */
    U8                      Major;                      /* 03h */
} ATLAS_FW_VERSION_STRUCT;

/** Dword or structured access to firmware version.*/
typedef union _ATLAS_FW_VERSION
{
    /** Structured access to firmware version. */
    ATLAS_FW_VERSION_STRUCT   Struct;
    /** Word access to firmware version. */
    U32                 Word;
} ATLAS_FW_VERSION, * PTR_ATLAS_FW_VERSION;

/** Structure for firmware header*/
typedef struct _ATLAS_FW_HEADER
{
    /** ARM branch instruction. */
    U32                     ArmBranchInstruction0;               /* 00h */
    /** Firmware signature : #MPI_FW_HEADER_SIGNATURE_0. */
    U32                     Signature0;                          /* 04h */
    /** Firmware signature : #MPI_FW_HEADER_SIGNATURE_1. */
    U32                     Signature1;                          /* 08h */
    /** Firmware signature : #MPI_FW_HEADER_SIGNATURE_2. */
    U32                     Signature2;                          /* 0Ch */
    /** Firmware image identifier. */
    U32                     FwIdentifier;                        /* 10h */
    /** Additional header length -- number of bytes remaining in header,
     *  including this field. */
    U32                     AdditionalHeaderLength;              /* 14h */
    /** Firmware image checksum. */
    U32                     Checksum;                            /* 18h */
    /** Firmware version. */
    ATLAS_FW_VERSION       FWVersion;                           /* 1Ch */
    /** Firmware family. */
    U16                     FwFamily;                            /* 20h */
    /** Reserved. */
    U16                     Reserved22;                          /* 22h */
    /** OEM family. */
    U16                     OEMFamily;                           /* 24h */
    /** Reserved. */
    U16                     Reserved26;                          /* 26h */
    /** Header flags. */
    U32                     HeaderFlags;                         /* 28h */
    /** Additional firmware information offset. */
    U32                     AdditionalFWInfoOffset;              /* 2Ch */
    /** Firmware image size. */
    U32                     FwImageSize;                         /* 30h */
    /** Offset of execution entry point. */
    U32                     RunOffset;                           /* 34h */
    /** Offset of next image. */
    U32                     NextImageOffset;                     /* 38h */

} ATLAS_FW_HEADER, * PTR_ATLAS_FW_HEADER;

/** Hardware defined flash signature */
#define MPI_ATLAS_FW_HEADER_SIGNATURE_0       ( 0x5AEAA503 )  /**< Firmware header signature 0. */
#define MPI_ATLAS_FW_HEADER_SIGNATURE_1       ( 0xA55AEAA5 )  /**< Firmware header signature 1. */
#define MPI_ATLAS_FW_HEADER_SIGNATURE_2       ( 0x5AA55AEA )  /**< Firmware header signature 2. */
#define MPI_ATLAS_FW_HEADER_FW_ID             ( 0x41744677 )  /**< Atlas firmware ID ("AtFw").  */


/*
 * Signature value that can be used by the "what" utility to locate string
 * information in the image.
 */
#define ATLAS_FW_HEADER_WHAT_SIGNATURE        ( 0x29232840 )

/** Length of firmware name. */
#define ATLAS_FW_NAME_LENGTH  32

/** Structure for firmware additional name information.*/
typedef struct _ATLAS_FW_ADDITIONAL_INFO_NAME
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
    U8  Name[ATLAS_FW_NAME_LENGTH];
} ATLAS_FW_ADDITIONAL_INFO_NAME, * PTR_ATLAS_FW_ADDITIONAL_INFO_NAME;

/** Structure for firmware additional information.*/
typedef struct _ATLAS_FW_ADDITIONAL_INFO
{
    /** Additional information's version. */
    U32                     AdditionalInfoVersion;
    /** Version name. */
    ATLAS_FW_ADDITIONAL_INFO_NAME VersionName;
    /** Vendor name. */
    ATLAS_FW_ADDITIONAL_INFO_NAME VendorName;
} ATLAS_FW_ADDITIONAL_INFO, * PTR_ATLAS_FW_ADDITIONAL_INFO;

/*
 * Chained header related declarations.
 */

/**
 * First DWORD of chained header. Identifies header type and revision.
 */
typedef struct _ATLAS_CHAINED_HEADER_IDENTIFIER
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

} ATLAS_CHAINED_HEADER_IDENTIFIER, *PTR_ATLAS_CHAINED_HEADER_IDENTIFIER;

/**
 * Second DWORD of chained header. Identifies image type.
 */
typedef struct _ATLAS_CHAINED_HDR_IMAGE_IDENTIFIER
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

} ATLAS_CHAINED_HDR_IMAGE_IDENTIFIER, *PTR_ATLAS_CHAINED_HDR_IMAGE_IDENTIFIER;

/**
 * Chained header to be used for concatenation of images.
 * LSI might redefine 'LSI reserved' fields at a later stage if needed.
 */
typedef struct _ATLAS_CHAINED_HEADER
{
    /** This field identifies chained header revision & type.*/
    ATLAS_CHAINED_HEADER_IDENTIFIER    ChainedHeaderId;               /* 00h */
    /** This field identifies chained image type. */
    ATLAS_CHAINED_HDR_IMAGE_IDENTIFIER ImageIdentifier;               /* 04h */
    /** This field stores data specific to an image type. For example: signature. */
    U32                          IdSpecificData;                /* 08h */
    /** LSI reserved field. */
    U32                          LSIReserved0C;                 /* 0Ch */
    /** LSI reserved field. */
    U32                          LSIReserved10;                 /* 10h */
    /** Additional header length -- number of bytes remaining in header,
     *  including this field. */
    U32                          AdditionalHeaderLength;        /* 14h */
    /** Image checksum. */
    U32                          Checksum;                      /* 18h */
    /** Firmware version. */
    ATLAS_FW_VERSION              ContentsVersion;               /* 1Ch */
    /** Family to which firmware belongs to. */
    U16                          FwFamily;                      /* 20h */
    /** Reserved field. */
    U16                          Reserved2A;                    /* 22h */
    /** OEM family. */
    U16                          OEMFamily;                     /* 24h */
    /** Reserved field. */
    U16                          Reserved2E;                    /* 26h */
    /** Header flags. */
    U32                          HeaderFlags;                   /* 28h */
    /** LSI reserved field. */
    U32                          LSIReserved34;                 /* 2Ch */
    /** Image size in dwords. */
    U32                          ImageSize;                     /* 30h */
    /** Offset to content. */
    U32                          OffsetToContent;               /* 34h */
    /** Next image offset. */
    U32                          NextImageOffset;               /* 38h */

} ATLAS_CHAINED_HEADER, *PTR_ATLAS_CHAINED_HEADER;


#define ATLAS_FW_HEADER_END_OF_NEXT_OFFSET  (0xFFFFFFFF)

/*
 * Concatenated image 'header type'.
 * This field can be used for differentiating firmware images, concatenated LSI
 * images, and concatenated OEM images.
 */

/** Chained Header Identifier type value - Firmware Image.
 * Identifies firmware header. Hex 0xEA identifies the ARM branch instruction
 * at the beginning of firmware header.
 */
#define ATLAS_HEADER_TYPE_FIRMWARE   0xEA

/** Chained Header Identifier type value - OEM image
 * The header is OEM specific concatented image header.
 */
#define ATLAS_HEADER_TYPE_OEM        0x7F

/** Chained Header Identifier type value - LSI reserved image.
 * The header is LSI specific concatented image header. For example there could
 * be LSI style MFG image concatenated using this header.
 */
#define ATLAS_HEADER_TYPE_LSI        0xFF


/*
 * Concatenated image 'image types'.
 * Signature for a concatenated image corresponds to Image Type.
 * In case more LSI Image Types are added, those could be added here
 */

/** Chained Header Image Identifier value for Avago format
 * Manufacturing Configuration Image.
 */
#define ATLAS_CONCAT_IMAGE_TYPE_MFG  0x01

/** Chained Header Image Identifier value for Avago format Flash
 *  Table / BootLoader Image
 */
#define ATLAS_CONCAT_IMAGE_TYPE_FT_BL  0x02

/** Chained Header Image Identifier value for Avago format
 * Signature Image.
 */
#define ATLAS_CONCAT_IMAGE_TYPE_SIGNATURE 0x03

/** Chained Header Image Identifier value for Avago format
 * Status Image (to distinguish new vs old images).
 */
#define ATLAS_CONCAT_IMAGE_TYPE_FWSTATUS 0x04

/** Chained Header Image Identifier value for Avago format
 * SBR Image .
 */
#define ATLAS_CONCAT_IMAGE_TYPE_SBR 0x05

/** Chained Header Image Identifier value for Avago format
 * Key update signature image .
 */
#define ATLAS_CONCAT_IMAGE_TYPE_KEY_UPDATE 0x06

/**
 * Signature DWORD 2 value (IdSpecificData field) for SBR
 * Image.
 */
#define ATLAS_CONCAT_IMAGE_SBR_SIGNATURE ( 0x152A354A )

/**
 * Signature DWORD 2 value (IdSpecificData field) for FW Status
 * Image.
 */
#define CONCAT_IMAGE_FWSTATUS_SIGNATURE ( 0xFEEDFACE )

/**
 * Signature DWORD 2 value (IdSpecificData field) for chained MFG image.
 */
#define ATLAS_CONCAT_IMAGE_MFG_SIGNATURE ( 0x03FCDA7A )

/**
 * Signature DWORD 2 value (IdSpecificData field) for chained
 * flash table/BootLoader Image
 */
#define ATLAS_CONCAT_IMAGE_FT_SBR_BL_SIGNATURE ( 0xBFAE289B )

/**
 * Chained image containing fields used to to distinguish new vs
 * old images.  The fields will be pre-populated with all Fs to
 * match erased flash.
 */
typedef struct _ATLAS_FWSTATUS_IMAGE
{
    U32                         CurrentState;           /* 00h */
    U32                         ChecksumAdjustment;     /* 04h */

} ATLAS_FWSTATUS_IMAGE, *PTR_ATLAS_FWSTATUS_IMAGE;

/**
 * The header for a flashtable/bootloader image.  This comes
 * after the chained image header.
 */
typedef struct _ATLAS_FLASHTBL_SBR_BOOTLDR_HEADER
{
    /** A CRC-32 value covering the contents of the image
     * (does not include the header) */
    U32     CRC;                       /* 00h */
    /** A value used to indicate the format of this header and image contents. */
    U32     ImageFormatVersion;        /* 04h */
    /** The offset in bytes from the beginning of this header
     *  to the start of the flash table */
    U32     FlashTableOffset;          /* 08h */
    /** The length in bytes of the flash table */
    U32     FlashTableLength;          /* 0Ch */
    /** The offset in bytes from the beginning of this header
     *  to the start of the boot loader */
    U32     BootLoaderOffset;          /* 10h */
    /** The length in bytes of the boot loader */
    U32     BootLoaderLength;          /* 14h */
    U32     Reserved;                  /* 18h */
} ATLAS_FLASHTBL_BOOTLDR_HEADER, *PTR_ATLAS_FLASHTBL_BOOTLDR_HEADER;

/** Hardware signature to identify the start of an SBR */
#define ATLAS_SBR_HW_SIGNATURE    ( 0xC0103DC4 )

/** The SoC HW settings section of the SBR is a required section
 *  as well as a fixed size number of bytes.  The rest of the
 *  sections are variable and specified in the SBR header.
 */

#define ATLAS_SBR_SOC_HW_SETTINGS_SIZE    (416)         /* Size in bytes */

#define ATLAS_SBR_MAXIMUM_SIZE            (128 * 1024)  /* Size in bytes and 128k */

/** The HW Settings checksum in SBR is defined as:
 *    Byte by Byte Sum of (0xA5 + HW Signature + [...] + HW Settings Checksum) = 0x00
 *
 *  HW_SETTINGS_CHECKSUM_SEED defines the 0xA5 portion of the formula
 */
#define ATLAS_SBR_CHECKSUM_SEED           (0xA5)

/** The SPICO checksum in SBR is defined as:
 *    Dword Sum of (0x5AA5A55A + SPICO Signature + SPICO code +
 *    SPICO Checksum) = 0x00
 *
 *  SPICO_CHECKSUM_SEED defines the 0xA5 portion of the formula
 */
#define SPICO_CHECKSUM_SEED                 (0x5AA5A55A)


/** The beginning portion of a SBR including the HW signature
 *  and the SBR Index.
 */
typedef struct _ATLAS_SBR_HEADER
{
    U32 HwSignature;                   /* 00h */
    U32 PsbHwSettingsOffset;           /* 04h */
    U32 PsbHwSettingsSize;             /* 08h */
    U32 Psw0HwSettingsOffset;          /* 0Ch */
    U32 Psw0HwSettingsSize;            /* 10h */
    U32 Psw1HwSettingsOffset;          /* 14h */
    U32 Psw1HwSettingsSize;            /* 18h */
    U32 Psw2HwSettingsOffset;          /* 1Ch */
    U32 Psw2HwSettingsSize;            /* 20h */
    U32 Psw3HwSettingsOffset;          /* 24h */
    U32 Psw3HwSettingsSize;            /* 28h */
    U32 Psw4HwSettingsOffset;          /* 2Ch */
    U32 Psw4HwSettingsSize;            /* 30h */
    U32 Psw5HwSettingsOffset;          /* 34h */
    U32 Psw5HwSettingsSize;            /* 38h */
    U32 Reserved0Offset;               /* 3Ch */
    U32 Reserved0Size;                 /* 40h */
    U32 PswX2HwSettingsOffset;         /* 44h */
    U32 PswX2HwSettingsSize;           /* 48h */
    U32 SerdesHwSettingsOffset;        /* 4Ch */
    U32 SerdesHwSettingsSize;          /* 50h */
    U32 Reserved1Offset;               /* 54h */
    U32 Reserved1Size;                 /* 58h */

} ATLAS_SBR_HEADER, *PTR_ATLAS_SBR_HEADER;

#define ATLAS_SBR_INDEX_SIZE            (88)
#define ATLAS_SBR_HW_SIGNATURE_SIZE     (4)

#define ATLAS_SBR_TOTAL_HEADER_SIZE     (ATLAS_SBR_SOC_HW_SETTINGS_SIZE + ATLAS_SBR_INDEX_SIZE + ATLAS_SBR_HW_SIGNATURE_SIZE)



/** Maximum number of entries in flash table. */
#define ATLAS_MAX_FLASH_TABLE_ENTRIES 12

/**
 * Structure for Flash Table.
 * This table describes all the different flash regions. The regions
 * can occur in any order within the table.
 */
typedef struct _ATLAS_FLASH_TABLE
{
    /** Flash table version.*/
    U16 FlashTableVersion;
    /** Number of flash table entries.*/
    U8  NumFlashTableEntries;
    /** reserved.*/
    U8  Reserved;
    /** Structure for flash region entry.*/
    struct _ATLAS_FLASH_REGION_ENTRY
    {
        /** Flash region type.*/
        U8  RegionType;
        /** Reserved.*/
        U8  Reserved[3];
        /** Region size.*/
        U32 RegionSize;
        /** Region offset.*/
        U32 RegionOffset;
    } FlashRegionEntries[ATLAS_MAX_FLASH_TABLE_ENTRIES];

} ATLAS_FLASH_TABLE, * PTR_ATLAS_FLASH_TABLE;

/** Physical Flash Regions */
#define     ATLAS_FLASH_SBR_BOOTLOADER_1 (0x00)  /**< Flash Region Id for SBR/Boot Loader Image 1. */
#define     ATLAS_FLASH_SBR_BOOTLOADER_2 (0x01)  /**< Flash Region Id for SBR/Boot Loader Image 2. */
#define     ATLAS_FLASH_FIRMWARE_COPY_1  (0x02)  /**< Flash Region Id for Firmware Image 1 */
#define     ATLAS_FLASH_FIRMWARE_COPY_2  (0x03)  /**< Flash Region Id for Firmware Image 2 */
#define     ATLAS_FLASH_CONFIG_1         (0x04)  /**< Flash Region Id for Config Pages, At a time One
                                                     Config Region is Active */
#define     ATLAS_FLASH_CONFIG_2         (0x05)  /**< Flash Region Id for Config Pages */
#define     ATLAS_FLASH_LOG_1            (0x06)  /**< Flash Region Id for Logs, At a time one Log
                                                     Region is Active */
#define     ATLAS_FLASH_LOG_2            (0x07)  /**< Flash Region Id for Logs */
#define     ATLAS_FLASH_OEM_1            (0x08)  /**< Flash Region Id for OEM related Data */
#define     ATLAS_FLASH_OEM_2            (0x09)  /**< Flash Region Id for OEM related Data */

#define     ATLAS_FLASH_COREDUMP         (0x0A)  /**< Flash Region Id for coredump data */

/** Virtual Flash Regions */
#define     ATLAS_FLASH_MANUFACTURING    (0xF0)  /**< Virtual Flash Region for default NVDATA */

/** Per the EDS the SBR locations start 1K bytes into a 256K area.  Our flash regions containing
 *  SBR images start on those 256K bounaries so this value accounts for the 1K offset
 */
#define ATLAS_HALI_FLASH_SBR_START_OFFSET  (0x400)

/** This is the offset within an SBR region to the flash table */
#define ATLAS_HALI_FLASH_FLASHTABLE_OFFSET  (4)

#define  ATLAS_HAS_NEXT_CHAIN_IMAGE(Offset) ((Offset != 0) && (Offset != ATLAS_FW_HEADER_END_OF_NEXT_OFFSET))

/**@}*/
#endif


