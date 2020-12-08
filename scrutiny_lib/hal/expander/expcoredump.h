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

#ifndef __EXP_CORE_DUMP__H__
#define __EXP_CORE_DUMP__H__

/* Coredump defines and structures */

/** The image signature for a coredump */
#define EXP_COREDUMP_IMAGE_SIGNATURE     (0xDECEA5ED)

/** The current version of the format of a coredump image */
#define EXP_COREDUMP_IMAGE_VERSION       (0x0001)

/** Coredump image header definition */
typedef struct _EXP_COREDUMP_IMAGE_HEADER
{
    U32 Signature;        /**< The image signature */
    U16 HeaderVersion;    /**< The format version of the image header */
    U16 HeaderSize;       /**< The size of the image header in dwords */
    /** flag union for bit and byte access. */
    union _EXP_COREDUMP_IMAGE_FLAGS_U
    {
        /** Structure for bitwise access. */
        struct __EXP_COREDUMP_IMAGE_FLAGS_BITS
        {
            U8 WriteStarted    :1; /**< When ZERO indicates coredump writing started */
            U8 WriteFinished   :1; /**< When ZERO indicates coredump writing finished */
            U8 Reserved        :6; /**< Reserved */
        } Bits; /**< Bit access. */
        U8 Byte; /**< Byte access. */
    } Flags;
    U8  Reserved1;
    U16 Source;            /**< The Source entity requesting the coredump */
    U32 SystemClockHigh;   /**< The high 32-bits of a system clock value */
    U32 SystemClockLow;    /**< The low 32-bits of a system clock value */
    U32 ImageLength;       /**< The length of the image in dwords including header */
    U32 Reserved2[2];
} EXP_COREDUMP_IMAGE_HEADER, *PTR_EXP_COREDUMP_IMAGE_HEADER;

/** A coredump section header */
typedef struct _EXP_COREDUMP_SECTION_HEADER
{
    U16 SectionType;      /**< The type of the section contents (fw data, exec context, etc)*/
    U16 Version;          /**< The format version of the section */
    U8  Flags;            /**< Flags for the section */
    U8  Checksum;         /**< A byte checksum for the section including the header */
    U16 Reserved1;
    U16 SectionLength;    /**< The length of the section in dwords. */
    U16 Reserved2;
    U32 Reserved3;
} EXP_COREDUMP_SECTION_HEADER, *PTR_EXP_COREDUMP_SECTION_HEADER;

/** Coredump definitions for firmware regions. */
#define EXP_COREDUMP_ACTIVE_FWREGION_BOOT    (0)
#define EXP_COREDUMP_ACTIVE_FWREGION_COPY1   (1)
#define EXP_COREDUMP_ACTIVE_FWREGION_COPY2   (2)
#define EXP_COREDUMP_ACTIVE_FWREGION_UNKNOWN (0xDD)
#define EXP_COREDUMP_ACTIVE_FWREGION_RAM     (0xFF)

/** The current section format version.
 * Currently all section types have the same version so
 * that can all share this version value.
 */
#define EXP_COREDUMP_SECTION_VERSION   (0x0001)


/** The section types contained within a coredump image.  Populated in the
 * SectionType field of the section header within the coredump.
 */
typedef enum _EXP_COREDUMP_SECTIONTYPE
{
    EXP_COREDUMP_SECTIONTYPE_REVISION = 0,        /**< Firmware revision data */
    EXP_COREDUMP_SECTIONTYPE_THREADS,             /**< Thread information */
    EXP_COREDUMP_SECTIONTYPE_MEMORY,              /**< Memory pool information */
    EXP_COREDUMP_SECTIONTYPE_TRACELOG,            /**< Tracelog data */
    EXP_COREDUMP_SECTIONTYPE_EXECUTION_CONTEXT,   /**< Execution context data */
    EXP_COREDUMP_SECTIONTYPE_EDFBCOUNTERS,        /**< EDFB counters */
    EXP_COREDUMP_SECTIONTYPE_ISR_PROFILING,       /**< ISR profiling data */
    EXP_COREDUMP_SECTIONTYPE_OEM                  /**< OEM custom data */
} EXP_COREDUMP_SECTIONTYPE;

/** The memory type enum identifies the format for a memory pool within the
 *  memory usage section of a coredump image.  It is populated in the MemoryType
 *  field of a memory pool instance within the memory usage section.
 */
typedef enum _HALI_COREDUMP_MEMORYTYPE
{
    HALI_COREDUMP_MEMORYTYPE_BYTE = 0,     /**< Byte pool */
    HALI_COREDUMP_MEMORYTYPE_NETXPACKET    /**< Netx packet pool */
} HALI_COREDUMP_MEMORYTYPE;


/** The memory type enum identifies the format for a memory pool within the
 *  memory usage section of a coredump image.  It is populated in the MemoryType
 *  field of a memory pool instance within the memory usage section.
 */
typedef enum _EXP_COREDUMP_MEMORYTYPE
{
    EXP_COREDUMP_MEMORYTYPE_BYTE = 0,     /**< Byte pool */
    EXP_COREDUMP_MEMORYTYPE_NETXPACKET    /**< Netx packet pool */
} EXP_COREDUMP_MEMORYTYPE;

/** The source is used to identify the invoker of the coredump generation API.
 *  Is is populated in the image header of the coredump image in the Source field.
 */
typedef enum _EXP_COREDUMP_SOURCE
{
    EXP_COREDUMP_SOURCE_WATCHDOG = 0,      /**< Watchdog handler */
    EXP_COREDUMP_SOURCE_EXCEPTION_HDLR,    /**< ARM exception handler */
    EXP_COREDUMP_SOURCE_FWFAULT,           /**< Firmare fault handler */
    EXP_COREDUMP_SOURCE_CLI                /**< CLI 'coredump' command */
} EXP_COREDUMP_SOURCE;

/** The status from coredump related APIs. */
typedef enum _EXP_COREDUMP_STATUS
{
    EXP_COREDUMP_STATUS_SUCCESS = 0,        /**< Success */
    EXP_COREDUMP_STATUS_STORAGE_NOT_FOUND,  /**< The persistent storage could not be found */
    EXP_COREDUMP_STATUS_FAILURE             /**< A failure occurred */
} EXP_COREDUMP_STATUS;


/** The current section format version.
 * Currently all section types have the same version so
 * that can all share this version value.
 */
#define EXP_COREDUMP_SECTION_VERSION   (0x0001)

/** The definition of a firmware revision section of a coredump image */
typedef struct _EXP_COREDUMP_SECTION_FWREV
{
    U8         ActiveFwRegion;     /**< The region of the currently executing firmware */
    U8         Reserved1;
    U16        Reserved2;
    U8         CompRevLevel;       /**< The revision level of the expander */
    U8         ComponentType;      /**< The compenent type of the expander */
    U16        ComponentID;        /**< The component ID of the expander */
    FW_VERSION FWVersion;          /**< The revision of the firmware */
    FW_VERSION HalLibVersion;      /**< The revision of the HAL library */
    FW_VERSION SesLibVersion;      /**< The revision of the SES library */
    FW_VERSION SceLibVersion;      /**< The revision of the SCE library */
    U32        Reserved3[3];
} EXP_COREDUMP_SECTION_FWREV, *PTR_EXP_COREDUMP_SECTION_FWREV;


/** Enumeration listing different expander revision levels.
 */
typedef enum _HALI_EXPANDER_REV_LEVEL
{
    HALI_EXPANDER_REV_LEVEL_0,      /**< A0 */
    HALI_EXPANDER_REV_LEVEL_1,      /**< B0 */
    HALI_EXPANDER_REV_LEVEL_2,      /**< C0 */
    HALI_EXPANDER_REV_LEVEL_3,      /**< C1 */
    HALI_EXPANDER_REV_LEVEL_UNKNOWN /**< Unknown */
} HALI_EXPANDER_REV_LEVEL, *PTR_HALI_EXPANDER_REV_LEVEL;


/* Cobra Chip Revision Id's
 */
#define HAL_EXP_REVISION_A0     (0x00)  /**< A0 expander chip revision. */
#define HAL_EXP_REVISION_B0     (0x01)  /**< B0 expander chip revision. */
#define HAL_EXP_REVISION_C0     (0x02)  /**< C0 expander chip revision. */
#define HAL_EXP_REVISION_C1     (0x03)  /**< C1 expander chip revision. */


/* Component Type  */

/** Define to identify Standard Cub with 48 phys. */
#define HALI_COMPONENT_TYPE_CUB   (0x0)
/** Define to identify reduced Cobra. */
#define HALI_COMPONENT_TYPE_CUB_R (0x1)


/* Cub Bond options */
#define HAL_CUB_BOND_OPTION_48      (0X0238)
#define HAL_CUB_BOND_OPTION_44      (0X0239)
#define HAL_CUB_BOND_OPTION_40      (0X023a)
#define HAL_CUB_BOND_OPTION_36      (0X023b)
#define HAL_CUB_BOND_OPTION_28      (0X023c)
#define HAL_CUB_BOND_OPTION_24      (0X023d)

/* Cobra Bond options */
#define HAL_CORBA_BOND_OPTION_48      (0X0230)
#define HAL_CORBA_BOND_OPTION_44      (0X0231)
#define HAL_CORBA_BOND_OPTION_40      (0X0232)
#define HAL_CORBA_BOND_OPTION_36      (0X0233)
#define HAL_CORBA_BOND_OPTION_28      (0X0235)
#define HAL_CORBA_BOND_OPTION_24      (0X0236)

/* Margay Bond options */
#define HAL_MARGAY_BOND_OPTION_48      (0X0240)
#define HAL_MARGAY_BOND_OPTION_40      (0X0242)
#define HAL_MARGAY_BOND_OPTION_32      (0X0244)
#define HAL_MARGAY_BOND_OPTION_24      (0X0246)

/** Enumeration listing different Cobra bond options.
 */
typedef enum _HALI_EXP_BOND_OPTION
{
    HALI_BOND_OPTION_0,     /**< Bond Option 48 */
    HALI_BOND_OPTION_1,     /**< Bond Option 44 */
    HALI_BOND_OPTION_2,     /**< Bond Option 40 */
    HALI_BOND_OPTION_3,     /**< Bond Option 36 */
    HALI_BOND_OPTION_4,     /**< Bond Option 32 */
    HALI_BOND_OPTION_5,     /**< Bond Option 28 */
    HALI_BOND_OPTION_6,     /**< Bond Option 24 */
    HALI_BOND_OPTION_7      /**< Unknown Bond Option */
} HALI_EXP_BOND_OPTION, *PTR_HALI_EXP_BOND_OPTION;


/** Coredump definitions for firmware regions. */
#define HALI_COREDUMP_ACTIVE_FWREGION_BOOT    (0)
#define HALI_COREDUMP_ACTIVE_FWREGION_COPY1   (1)
#define HALI_COREDUMP_ACTIVE_FWREGION_COPY2   (2)
#define HALI_COREDUMP_ACTIVE_FWREGION_UNKNOWN (0xDD)
#define HALI_COREDUMP_ACTIVE_FWREGION_RAM     (0xFF)


/**
 *  A structure to hold a set of processor registers for exception handling,
 *  coredump generation, etc.
 *  NOTE: The definition should be in sync with the order in which the
 *  registers are pushed to stack in vectors.s.
 */
typedef struct _REGFILE{
/** Extra reserved dword added so the size of REGFILE struct is
 *  divisible by 8 to maintain stack alignment
 */
   U32  reserved;
   U32  r14_prev;      /**< Previous link register (LR, R14) */
   U32  r13_prev;      /**< R13 Previous. */
   U32  cpsr_prev;     /**< CPSR Previous. */
   U32  r14;           /**< R14. */

   U32  r0;           /**< R0. */
   U32  r1;           /**< R1. */
   U32  r2;           /**< R2. */
   U32  r3;           /**< R3. */
   U32  r4;           /**< R4. */
   U32  r5;           /**< R5. */
   U32  r6;           /**< R6. */
   U32  r7;           /**< R7. */
   U32  r8;           /**< R8. */
   U32  r9;           /**< R9. */
   U32  r10;          /**< R10. */
   U32  r11;          /**< R11. */
   U32  r12;          /**< R12. */
} REGFILE;



/** The number of bytes of thread stack to save.  This
 *  must be divisible by 4 to maintain dword alignment.
 */
#define HALI_COREDUMP_EXEC_CONTEXT_STACK_SIZE  (1024)

/** The definition of an execution context section of a coredump image */
typedef struct _HALI_COREDUMP_SECTION_EXEC_CONTEXT
{
    /** Union for bit and byte access. */
    union _HALI_COREDUMP_EXEC_CONTEXT_FLAGS_U
    {
        /** Structure for bitwise access. */
        struct __HALI_COREDUMP_EXEC_CONTEXT_FLAGS_BITS
        {
            U8 Valid           :1; /**< The execution context data is valid */
            U8 Reserved        :7; /**< Reserved */
        } Bits; /**< Bit access. */
        U8 Byte; /**< Byte access. */
    } Flags;
    U8  Reserved1;
    U16 Reserved2;
    REGFILE Registers;   /**< Processor registers */
    U8 StackData[HALI_COREDUMP_EXEC_CONTEXT_STACK_SIZE];  /**< Captured stack data */
} HALI_COREDUMP_SECTION_EXEC_CONTEXT, *PTR_HALI_COREDUMP_SECTION_EXEC_CONTEXT;


/** The number of characters for storing the thread name.  This value
 *  must be divisible by 4 so dword alignment is maintained.
 */
#define HALI_COREDUMP_THREAD_NAME_LENGTH  (32)

/** The definition of a thread information section of a coredump image */
typedef struct _HALI_COREDUMP_THREAD
{
    U32 ID;                   /**< The ID (handle) of the thread */
    U32 State;                /**< The execution state of the thread */
    U32 NumTimesRan;          /**< The number of times the thread has run */
    U32 Priority;             /**< The thread priority */
    U32 PreemptionThreshold;  /**< The preemption threshold of the thread */
    U32 TimeSlice;            /**< The time slice of the thread */
    U32 NextID;               /**< The next thread handle in the thread list */
    U32 NextSuspendedID;      /**< The handle of the next suspended thread */
    U32 StackSize;            /**< The stack size of the thread */
    U32 MaxStackUsage;        /**< The max amount of stack used */
    char Name[HALI_COREDUMP_THREAD_NAME_LENGTH];
} HALI_COREDUMP_THREAD, *PTR_HALI_COREDUMP_THREAD;

/** The number of characters for storing the memory pool name.  This
 *  value must be divisible by 4 so dword alignment is maintained.
 */
#define HALI_COREDUMP_MEMPOOL_NAME_LENGTH  (32)

/** A definition for the common portion of a memory pool entry in the
 * memory pool section of the coredump image
 */
typedef struct _HALI_COREDUMP_MEMPOOL_COMMON
{
    U8  MemoryType;    /**< The type of memory pool (byte, netx packet, etc) */
    U8  Reserved1;
    U16 Reserved2;
    U32 ID;            /**< The ID (handle) of the memory pool */
} HALI_COREDUMP_MEMPOOL_COMMON, *PTR_HALI_COREDUMP_MEMPOOL_COMMON;

/** Structure for byte memory pools within a coredump image
 *  NOTE:  This structure must be the same size as the NETX pool
 *  structure below
 */
typedef struct _HALI_COREDUMP_BYTEPOOL
{
    HALI_COREDUMP_MEMPOOL_COMMON Common;  /**< The common portion of the memory pool */
    U32 BytesAvailable;                   /**< The number of bytes available in the pool */
    U32 NumFragments;                     /**< The number of fragments in the pool */
    char Name[HALI_COREDUMP_MEMPOOL_NAME_LENGTH];  /**< The name of the pool */
} HALI_COREDUMP_BYTEPOOL, *PTR_HALI_COREDUMP_BYTEPOOL;

/** Structure for netx memory pools within a coredump image
 *  NOTE:  This structure must be the same size as the byte pool
 *  structure above
 */
typedef struct _HALI_COREDUMP_NETXPOOL
{
    HALI_COREDUMP_MEMPOOL_COMMON Common;  /**< The common portion of the memory pool */
    U32 TotalPackets;                     /**< The total number of packets in the pool */
    U32 FreePackets;                      /**< The number of free packets in the pool */
    char Name[HALI_COREDUMP_MEMPOOL_NAME_LENGTH];  /**< The name of the pool */
} HALI_COREDUMP_NETXPOOL, *PTR_HALI_COREDUMP_NETXPOOL;

/** The definition of an EDFB counters section of a coredump image */
typedef struct _HALI_COREDUMP_SECTION_EDFB_COUNTERS
{
    U32 HardResetTimeouts;          /**< Hard reset time out. */
    U32 DriveSignatureWaitTimeouts; /**< Drive signature wait time out. */
    U32 SASErrorsLogged;            /**< Counter for SAS manager errors. */
    U32 SASErrorLogFull;            /**< Counter for SAS manager log filling up. */
    U32 SASErrorUnhandledInts;      /**< Counter for SAS manager unhandled interrupts. */
    U32 SASErrorLogTimeOuts;        /**< Counter for SAS manager log time out. */
    U32 SASErrorTraps;              /**< Counter for SAS manager traps. */
    U32 SATAErrorsLogged;           /**< Counter for SATA errors. */
    U32 SATAErrorLogFull;           /**< Counter for SATA log filling up. */
    U32 EC17HitBT1237;              /**< Counter for BT1237 hits. */
    U32 HitBT1495;                  /**< Count of RxBBCrdTOStat with BT1495 conditions */
    U32 HitBT1467;                  /**< Count of Task log realignment (BT1467) conditions */
    U32 Reserved[2];
} HALI_COREDUMP_SECTION_EDFB_COUNTERS, *PTR_HALI_COREDUMP_SECTION_EDFB_COUNTERS;

/** A value to determine if a counter is invalid */
#define HALI_EDFB_PHY_SETTINGS_INVALID_COUNTER (0xFFFFFFFF)

/** Define the Timing structure for data that we are tracking */
typedef struct _IRQ_PERFORMANCE_TRACKING_VAR
{
    U16     MinTime;     /**< The least amount of time in usecs a given ISR has executed */
    U16     MaxTime;     /**< The most amount of time in usecs a given ISR has executed */
    U32     TotalTime;   /**< The total time in usecs a given ISR has executed across all invocations */
    U32     Count;       /**< The number of times a given ISR has executed */
    char    IrqName[32]; /**< A string identifying a given IRQ/ISR */
} IRQ_PERFORMANCE_TRACKING_VAR, *PTR_IRQ_PERFORMANCE_TRACKING_VAR;


// The size of internal buffer is alway assumed larger enough
#define EXP_COREDUMP_INTERNAL_BUFFER_SIZE       (16 * 1024)

SCRUTINY_STATUS ecdiGetCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice,
    __INOUT__   PU8                             PtrBuffer,
    __INOUT__   PU32                            PtrBufferSize,
    __IN__      EXP_COREDUMP_TYPE               Type,
    __IN__      const char*                     PtrFolderName
);

SCRUTINY_STATUS ecdiEraseCoreDump (
    __IN__      PTR_SCRUTINY_DEVICE             PtrDevice
);

#endif

