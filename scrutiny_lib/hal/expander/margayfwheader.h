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

#ifndef __MARGAY_FW_HEADER__H__
#define __MARGAY_FW_HEADER__H__

/** @addtogroup fwHeader Firmware Header
 *  @{ */


/** Maximum number of flash region supported */
#define FW_MAX_NUM_OF_FLASH_REGIONS 11

/** Structure for firmware version.*/
typedef struct _MARGAY_FW_VERSION_STRUCT
{
    /** Developement version. */
    U8                      Dev;                        /* 00h */
    /** Unit version. */
    U8                      Unit;                       /* 01h */
    /** Minor version. */
    U8                      Minor;                      /* 02h */
    /** Major version. */
    U8                      Major;                      /* 03h */
} MARGAY_FW_VERSION_STRUCT;

/** Dword or structured access to firmware version.*/
typedef union _MARGAY_FW_VERSION
{
    /** Structured access to firmware version. */
    MARGAY_FW_VERSION_STRUCT   Struct;
    /** Word access to firmware version. */
    U32                 Word;
} MARGAY_FW_VERSION, * PTR_MARGAY_FW_VERSION;

/** Structure for firmware header*/
typedef struct _MARGAY_FW_HEADER
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
    U32                     Reserved1C;                          /* 1Ch */
    /** New image signature 2. */
    U32                     Reserved20;                          /* 20h */
    /** Firmware version. */
    MARGAY_FW_VERSION       FWVersion;                           /* 24h */
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

} MARGAY_FW_HEADER, * PTR_MARGAY_FW_HEADER;


#define MARGAY_MPI_FW_HEADER_SIGNATURE_0       ( 0xC0EABAA0 )  /**< Firmware header signature 0. */
#define MARGAY_MPI_FW_HEADER_SIGNATURE_1       ( 0xC0BAEAA0 )  /**< Firmware header signature 1. */
#define MARGAY_MPI_FW_HEADER_SIGNATURE_2       ( 0xC0BAA0EA )  /**< Firmware header signature 2. */


/*
 * Signature value that can be used by the "what" utility to locate string
 * information in the image.
 */
#define MARGAY_FW_HEADER_WHAT_SIGNATURE        ( 0x29232840 )

/** Length of firmware name. */
#define MARGAY_FW_NAME_LENGTH  32

/** Structure for firmware additional name information.*/
typedef struct _MARGAY_FW_ADDITIONAL_INFO_NAME
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
    U8  Name[MARGAY_FW_NAME_LENGTH];
} MARGAY_FW_ADDITIONAL_INFO_NAME, * PTR_MARGAY_FW_ADDITIONAL_INFO_NAME;

/** Structure for firmware additional information.*/
typedef struct _MARGAY_FW_ADDITIONAL_INFO
{
    /** Additional information's version. */
    U32                     AdditionalInfoVersion;
    /** Version name. */
    MARGAY_FW_ADDITIONAL_INFO_NAME VersionName;
    /** Vendor name. */
    MARGAY_FW_ADDITIONAL_INFO_NAME VendorName;
    /** Platform external memory devices information table offset. */
    U32                     ExtMemInfoOffset;
} MARGAY_FW_ADDITIONAL_INFO, * PTR_MARGAY_FW_ADDITIONAL_INFO;

/*
 * Chained header related declarations.
 */

/**
 * First DWORD of chained header. Identifies header type and revision.
 */
typedef struct _MARGAY_CHAINED_HEADER_IDENTIFIER
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

} MARGAY_CHAINED_HEADER_IDENTIFIER, *PTR_MARGAY_CHAINED_HEADER_IDENTIFIER;

/**
 * Second DWORD of chained header. Identifies image type.
 */
typedef struct _MARGAY_CHAINED_HDR_IMAGE_IDENTIFIER
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

} MARGAY_CHAINED_HDR_IMAGE_IDENTIFIER, *PTR_MARGAY_CHAINED_HDR_IMAGE_IDENTIFIER;

/**
 * Chained header to be used for concatenation of images.
 * LSI might redefine 'LSI reserved' fields at a later stage if needed.
 * OEM reserved fields will be never redefined by LSI and will not be validated by LSI
 * SDK/LSI tools. OEMs can define these safely.
 */
typedef struct _MARGAY_CHAINED_HEADER
{
    /** This field identifies chained header revision & type.*/
    MARGAY_CHAINED_HEADER_IDENTIFIER    ChainedHeaderId;               /* 00h */
    /** This field identifies chained image type. */
    MARGAY_CHAINED_HDR_IMAGE_IDENTIFIER ImageIdentifier;               /* 04h */
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
    MARGAY_FW_VERSION            ContentsVersion;               /* 24h */
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

} MARGAY_CHAINED_HEADER, *PTR_MARGAY_CHAINED_HEADER;


#define  MARGAY_FW_HEADER_END_OF_NEXT_OFFSET (0xFFFFFFFF)
#define  MARGAY_HAS_NEXT_CHAIN_IMAGE(Offset) ((Offset != 0) && (Offset != MARGAY_FW_HEADER_END_OF_NEXT_OFFSET))

/*
 * Concatenated image 'header type'.
 * This field can be used for differentiating a firmware image from concatenated LSI
 * image from concatenated OEM image.
 */

/** Chained Header Identifier type value - Firmware Image
 * Identifies firmware header. Hex 0xEA identifies the ARM branch instruction
 * at the beginning of firmware header.
 */
#define MARGAY_HEADER_TYPE_FIRMWARE   0xEA

/** Chained Header Identifier type value - OEM image
 * The header is OEM specific concatented image header.
 */
#define MARGAY_HEADER_TYPE_OEM        0x7F

/** Chained Header Identifier type value - LSI reserved image
 * The header is LSI specific concatented image header. For example there could
 * be LSI style MFG image concatenated using this header.
 */
#define MARGAY_HEADER_TYPE_LSI        0xFF

/*
 * Concatenated image 'image types'.
 * Signature for a concatenated image corresponds to Image Type.
 * In case more LSI Image Types are added, those could be added here
 */

/** Chained Header Image Identifier value for Avago format
 * Manufacturing Configuration Image.
 */
#define MARGAY_CONCAT_IMAGE_TYPE_MFG  0x01

/** Chained Header Image Identifier value for Avago format Flash
 *  Table / Serial Bootstrap ROM (SBR) / BootLoader Image
 */
#define MARGAY_CONCAT_IMAGE_TYPE_FT_SBR_BL  0x02

/** Chained Header Image Identifier value for Avago format
 * Signature Image.
 */
#define MARGAY_CONCAT_IMAGE_TYPE_SIGNATURE 0x03

/** Chained Header Image Identifier value for Avago format
 * Status Image (to distinguish new vs old images).
 */
#define MARGAY_CONCAT_IMAGE_TYPE_FWSTATUS 0x04

/** Chained Header Image Identifier value for Avago format
 *  Key Update Image.
 */
#define MARGAY_CONCAT_IMAGE_TYPE_KEYUPDATE 0x05

/**
 * Signature DWORD 2 value (IdSpecificData field) for FW Status
 * Image.
 */
#define MARGAY_CONCAT_IMAGE_FWSTATUS_SIGNATURE (0xFEEDFACE)

/**
 * Signature DWORD 2 value (IdSpecificData field) for concatenated MFG image.
 */
#define MARGAY_CONCAT_IMAGE_MFG_SIGNATURE (0x03FCDA7A)

/**
 * Chained image containing fields used to to distinguish new vs
 * old images.  The fields will be pre-populated with all Fs to
 * match erased flash.
 */
typedef struct _MARGAY_FWSTATUS_IMAGE
{
    U32                         CurrentState;           /* 00h */
    U32                         ChecksumAdjustment;     /* 04h */

} MARGAY_FWSTATUS_IMAGE, *PTR_MARGAY_FWSTATUS_IMAGE;

/* Number of flash regions supported.
 * Note: Here inside HAL we define only one flash region entry inside
 * FLASH_TABLE as a place holder. The NUM_OF_FLASH_REGIONS value needs to be
 * overrided inside oemPlatform.h based on OEM platform and oemPlatform.h
 * shall always be included before fwHeader.h in any source/header file.
 */
#ifndef MARGAY_NUM_OF_FLASH_REGIONS
#define MARGAY_NUM_OF_FLASH_REGIONS 1
#endif

/** External memory interface types */
typedef enum _MARGAY_HALI_MEM_INTF_TYPE
{
    /** External memory over SPI */
    MARGAY_HALI_MEM_INTF_TYPE_SPI,
    /** External memory over XMEM */
    MARGAY_HALI_MEM_INTF_TYPE_XMEM,
    MARGAY_HALI_MEM_INTF_TYPE_INVALID
} MARGAY_HALI_MEM_INTF_TYPE;

/** Structure for flash region entry.*/
typedef struct _MARGAY_FLASH_REGION_ENTRY
{
    /** Flash region type.*/
    U8  RegionType;
    struct _MARGAY_FLASH_REGION_ENTRY_FLAGS
    {
        /** Flash device where the region is mapped. HALI_MEM_INTF_TYPE_SPI, HALI_MEM_INTF_TYPE_XMEM.*/
        U8  DevType       : 1;
        /** Flash device chip select where the region is mapped.*/
        U8  ChipSel       : 3;
        U8  ReservedBits  : 4;
    } Flags;
    /** Reserved.*/
    U8  Reserved[2];
    /** Region size.*/
    U32 RegionSize;
    /** Region offset is an offset from base address of Flash Device selected
      * by DevType-ChipSel pair.
      */
    U32 RegionOffset;
} MARGAY_FLASH_REGION_ENTRY, *PTR_MARGAY_FLASH_REGION_ENTRY;


/**
 * Structure for Flash Table.
 * This table describes all the different flash regions. The regions
 * can occur in any order within the table.
 */
typedef struct _MARGAY_FLASH_TABLE
{
    /** Flash table version.*/
    U16 FlashTableVersion;
    /** Number of flash table entries.*/
    U8  NumFlashTableEntries;
    /** Reserved.*/
    U8  Reserved;
    /** Flash Region Entry */
    MARGAY_FLASH_REGION_ENTRY FlashRegionEntries[MARGAY_NUM_OF_FLASH_REGIONS];
} MARGAY_FLASH_TABLE, * PTR_MARGAY_FLASH_TABLE;


/** Number of memory devices described by EXTMEM_CS_TABLE. */
#define MARGAY_EXTMEM_CS_ENTRIES        (5)

/** Structure for SPI external memory description entry.*/
typedef struct _MARGAY_SPI_CS_ENTRY
{
    /** Allocated address space for SPI chip select.
     *  Start/End offset from base address of SPI address space.
     *  The address space allocated for different devices
     *  shall not overlap.
     */
    U32 StartOffset;
    U32 EndOffset;

    /** Physical memory size of the device connected to chip select.
     */
    U32 MemorySize;

    struct _MARGAY_SPI_CS_ENTRY_FLAGS
    {
        /** Memory Type. #HALI_MEM_DEV_TYPES */
        U8  MemType       : 3;

        /* Read ECC enable. */
        U8  EccEn         : 1;
        U8  ReservedBits  : 4;
    } Flags;
    /** Reserved.*/
    U8  Reserved[3];
} MARGAY_SPI_CS_ENTRY, *PTR_MARGAY_SPI_CS_ENTRY;


/** Structure for XMEM external memory description entry.*/
typedef struct _MARGAY_XMEM_CS_ENTRY
{
    /** Allocated address space for XMEM chip select.
     *  Start/End offset from base address of SPI address space.
     *  The address space allocated for different devices
     *  shall not overlap.
     */
    U32 StartOffset;
    U32 EndOffset;

    /** Physical memory size of the device connected to chip select.
     */
    U32 MemorySize;

    /** XMEM configuration register.
     *  The default XMEM configuration register values in gExtMemCsTable table
     *  shall hold good for most platforms. Bit 8 of the register value defines
     *  data bus width, 1 is 16 bit and 0 is 8 bit.
     *  For platform specific changes for the register values, consult
     *  Broadcom systems engineering team.
     */
    U32 ConfigReg;

    struct _MARGAY_XMEM_CS_ENTRY_FLAGS
    {
        /** Memory Type. #HALI_MEM_DEV_TYPES */
        U8  MemType       : 3;

        /* Read ECC enable. */
        U8  EccEn         : 1;
        U8  ReservedBits  : 4;
    } Flags;
    /** Reserved.*/
    U8  Reserved[3];
} MARGAY_XMEM_CS_ENTRY, *PTR_MARGAY_XMEM_CS_ENTRY;


/**
 * Structure for external memory description.
 * This table describes all the external memory devices connected
 * using either XMEM or SPI external memory interface.
 */
typedef const struct _MARGAY_EXTMEM_CS_TABLE
{
    /** SPI External memory description entries.
     *  5 entries correspond to SPI chip selects HALI_MEM_NOR_CS
     *  to HALI_MEM_GP_CS3 in the same order.
     *  For unused chip selects, MemorySize must be set to 0.
     */
    MARGAY_SPI_CS_ENTRY ExtMemEntriesSpi[MARGAY_EXTMEM_CS_ENTRIES];

    /** XMEM External memory description entries.
     *  5 entries correspond to XMEM chip selects HALI_MEM_NOR_CS
     *  to HALI_MEM_GP_CS3 in the same order.
     *  For unused chip selects, MemorySize must be set to 0.
     */
    MARGAY_XMEM_CS_ENTRY ExtMemEntriesXmem[MARGAY_EXTMEM_CS_ENTRIES];

} MARGAY_EXTMEM_CS_TABLE, * PTR_MARGAY_EXTMEM_CS_TABLE;




/* This symbol indicates the signature of Manufacturing Image.*/
#define MARGAY_HAL_CFG_MFG_REGION                         0x10

////signature for firmware validation.
//#define MPI_FW_HEADER_SIGNATURE_0                       (0xC0EABAA0)
//#define MPI_FW_HEADER_SIGNATURE_1                       (0xC0BAEAA0)
//#define MPI_FW_HEADER_SIGNATURE_2                       (0xC0BAA0EA)


typedef U16     MARGAY_HALI_CFG_PAGE_ID;

/** Part of the Config page header which is set by the user */
typedef struct _MARGAY_HALI_CFG_PAGE_VISIBLE_HEADER
{
    U16               PageLength; /**< 16bit length (multiple of 4) of page */
    MARGAY_HALI_CFG_PAGE_ID  PageId;     /**< Unique ID to identify the page */
} MARGAY_HALI_CFG_PAGE_VISIBLE_HEADER, *PTR_MARGAY_HALI_CFG_PAGE_VISIBLE_HEADER;

/** Config page header flags */
typedef union _MARGAY_HAL_CFG_PAGE_HEADER_FLAGS
{
    U8 Byte;

    struct
    {
        U8 PageValid     : 1; /**< Bit clear when page is NOT valid */
        U8 PageWriting   : 1; /**< Bit clear when page is being written */
        U8 PageWriteDone : 1; /**< Bit clear when page writing is done*/
        U8 Reserved      : 5;
    } Bits;

} MARGAY_HAL_CFG_PAGE_HEADER_FLAGS, *PTR_MARGAY_HAL_CFG_PAGE_HEADER_FLAGS;

/** Part of the Config page header which is not visible to the user */
typedef struct _MARGAY_HAL_CFG_PAGE_HIDDEN_HEADER
{
    MARGAY_HAL_CFG_PAGE_HEADER_FLAGS    PageFlags;
    U8                           PageChecksum;
    U16                          Reserved;
} MARGAY_HAL_CFG_PAGE_HIDDEN_HEADER, *PTR_MARGAY_HAL_CFG_PAGE_HIDDEN_HEADER;

/** Config page header is made of HIDDEN & VISIBLE headers */
typedef struct _MARGAY_HAL_CFG_PAGE_HEADER
{
    MARGAY_HAL_CFG_PAGE_HIDDEN_HEADER    HiddenHdr;
    MARGAY_HALI_CFG_PAGE_VISIBLE_HEADER  VisibleHdr;
} MARGAY_HAL_CFG_PAGE_HEADER, *PTR_MARGAY_HAL_CFG_PAGE_HEADER;


#define MARGAY_HAL_CFG_INVALID_PAGE_ID         0xFFFF
#define MARGAY_HAL_CFG_EMPTY_PAGE_ID           0x0000
#define MARGAY_HAL_CFG_INVALID_PAGE_LENGTH     0xFFFF
#define MARGAY_HAL_CFG_EMPTY_PAGE_LENGTH       0x0000
#define MARGAY_HAL_CFG_REGION_VERSION    (0x04)
#define MARGAY_HAL_CFG_MAX_RETRIES       (0x03)
#define MARGAY_HAL_CFG_REGION_HEADER_FLAG_REGIONVALID     (0x01)
#define MARGAY_HAL_CFG_REGION_HEADER_FLAG_CREATING        (0x02)
#define MARGAY_HAL_CFG_REGION_HEADER_FLAG_CREATEDONE      (0x04)
#define MARGAY_HAL_CFG_REGION_HEADER_FLAG_PACKING         (0x08)
#define MARGAY_HAL_CFG_REGION_HEADER_FLAG_PACKDONE        (0x10)
#define MARGAY_HAL_CFG_PAGE_HEADER_FLAG_PAGEVALID    (0x01)
#define MARGAY_HAL_CFG_PAGE_HEADER_FLAG_WRITING      (0x02)
#define MARGAY_HAL_CFG_PAGE_HEADER_FLAG_WRITEDONE    (0x04)
#define MARGAY_HAL_CFG_DATA_EQUIV_HIDDEN_HEADER_SIZE      (0xFFFFFFFF)
#define MARGAY_HAL_CFG_REGION_HEADER_FLAGS_ON_CREATE  \
    ((U8)~MARGAY_HAL_CFG_REGION_HEADER_FLAG_CREATING)
#define MARGAY_HAL_CFG_PAGE_HEADER_FLAGS_ON_WRITE     \
    ((U8)~MARGAY_HAL_CFG_PAGE_HEADER_FLAG_WRITING)
typedef enum _MARGAY_HALI_CFG_REGION_TYPE
{
    MARGAY_HALI_CFG_ANY_REGION          = (0x00),
    MARGAY_HALI_CFG_MFG_REGION          = (0x10),
    MARGAY_HALI_CFG_PERSISTENT_REGION   = (0x20),
    MARGAY_HALI_CFG_INVALID_REGION      = (0xff)
} MARGAY_HALI_CFG_REGION_TYPE;
typedef union _MARGAY_HAL_CFG_REGION_HEADER_FLAGS
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
} MARGAY_HAL_CFG_REGION_HEADER_FLAGS, *PTR_MARGAY_HAL_CFG_REGION_HEADER_FLAGS;
typedef struct _MARGAY_HAL_CFG_REGION_HEADER
{
    MARGAY_HAL_CFG_REGION_HEADER_FLAGS RegionHdrFlags; /* Current status flags */
    U8                          RegionVersion;  /* Current version */
    U8                          RegionType;     /* PERSISTENT/MFG/Both regions */
    U8                          FirstPageOffset;/* Offset to first page in region*/
    U16                         Reserved;
    U16                         RegionSize;     /* Total size of the region */
} MARGAY_HAL_CFG_REGION_HEADER, *PTR_MARGAY_HAL_CFG_REGION_HEADER;


#define     MARGAY_HALI_FLASH_BOOT_LOADER      (0x00)  /**< Flash Region Id for Boot Loader Image. */
#define     MARGAY_HALI_FLASH_FIRMWARE_COPY_1  (0x01)  /**< Flash Region Id for Firmware Image 1 */
#define     MARGAY_HALI_FLASH_FIRMWARE_COPY_2  (0x02)  /**< Flash Region Id for Firmware Image 2 */
#define     MARGAY_HALI_FLASH_FIRMWARE_ACTIVE  (0x01)  /**< Flash Region Id for Active Image of Firmware */
#define     MARGAY_HALI_FLASH_FIRMWARE_BACKUP  (0x02)  /**< Flash Region Id for Backup Image of Firmware */
#define     MARGAY_HALI_FLASH_MANUFACTURING    (0x03)  /**< Flash Region Id for Manufacturing Page Image */
#define     MARGAY_HALI_FLASH_CONFIG_1         (0x04)  /**< Flash Region Id for Config Pages, At a time One
                                                     Config Region is Active */
#define     MARGAY_HALI_FLASH_CONFIG_2         (0x05)  /**< Flash Region Id for Config Pages */
#define     MARGAY_HALI_FLASH_LOG_1            (0x06)  /**< Flash Region Id for Logs, At a time one Log
                                                     Region is Active */
#define     MARGAY_HALI_FLASH_LOG_2            (0x07)  /**< Flash Region Id for Logs */
#define     MARGAY_HALI_FLASH_OEM_1            (0x08)  /**< Flash Region Id for OEM related Data */
#define     MARGAY_HALI_FLASH_OEM_2            (0x09)  /**< Flash Region Id for OEM related Data */
#define     MARGAY_HALI_FLASH_COREDUMP         (0x0A)  /**< Flash Region Id for coredump data */
#define     MARGAY_HALI_FLASH_INVALID          (0xFF)  /**< Invalid Region */

#define MARGAY_RW_BUFFER_NETWORK_IPV6_RESERVE     (0x24)

typedef struct _MARGAY_RW_BUFFER_NETWORK_INFO
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
    U8  Reserved2[MARGAY_RW_BUFFER_NETWORK_IPV6_RESERVE];
} MARGAY_RW_BUFFER_NETWORK_INFO, *PTR_MARGAY_RW_BUFFER_NETWORK_INFO;


/**@}*/
#endif



