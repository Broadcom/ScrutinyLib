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

#ifndef __ATLAS_REGISTERS__H__
#define __ATLAS_REGISTERS__H__


/*
 * All registers must have these conventions
 *
 * For Base Address or register address
 *      REGISTER_ADDRESS_<product>_<region>
 *
 * For Register offset
 *      REGISTER_OFFSET_<product>_<region offset>
 *
 * For REGISTER size
 *
 *      REGISTER_SIZE_<product>_<region>
 *
 * For Register - Operation/Value/Data
 *
 *      REGISTER_DATA_<product>_<region operation/value/mask>
 *
 * For Signatures
 *
 *      SIGNATURE_VALUE_<product>_<name of singature>
 *
 */


#define REGISTER_ADDRESS_ATLAS_CCR                      (0xFFF00000)
#define REGISTER_ADDRESS_ATLAS_CCR_CHIP_DEVICE_ID       (REGISTER_ADDRESS_ATLAS_CCR + 0x0000)
#define REGISTER_ADDRESS_ATLAS_CCR_CHIP_REVISION        (REGISTER_ADDRESS_ATLAS_CCR + 0x0004)
#define REGISTER_ADDRESS_ATLAS_CCR_RESET_STATUS         (REGISTER_ADDRESS_ATLAS_CCR + 0x00A0)
#define REGISTER_ADDRESS_ATLAS_CCR_SECURE_BOOT_STATUS   (REGISTER_ADDRESS_ATLAS_CCR + 0x00A4)
#define REGISTER_ADDRESS_ATLAS_CCR_ATLAS_MODE           (REGISTER_ADDRESS_ATLAS_CCR + 0x00B0)
#define REGISTER_ADDRESS_ATLAS_CCR_MISC_CONFIG_1        (REGISTER_ADDRESS_ATLAS_CCR + 0x00B4)
#define REGISTER_ADDRESS_ATLAS_CCR_MISC_CONFIG_2        (REGISTER_ADDRESS_ATLAS_CCR + 0x00B8)
#define REGISTER_ADDRESS_ATLAS_CCR_BOOT_MODE            (REGISTER_ADDRESS_ATLAS_CCR + 0x00D0)
#define REGISTER_ADDRESS_ATLAS_CCR_BOOT_ADDRESS         (REGISTER_ADDRESS_ATLAS_CCR + 0x00D4)
#define REGISTER_ADDRESS_ATLAS_CCR_SYSTEM_RESET_ENABLE  (REGISTER_ADDRESS_ATLAS_CCR + 0x00F0)
#define REGISTER_ADDRESS_ATLAS_CCR_SYSTEM_RESET_GEN     (REGISTER_ADDRESS_ATLAS_CCR + 0x00F4)
#define REGISTER_ADDRESS_ATLAS_CCR_PCIE_MISC_CONFIG     (REGISTER_ADDRESS_ATLAS_CCR + 0x0174)

#define REGISTER_ADDRESS_ATLAS_CPU_WD_RESET_THRESHOLD   (0xFFE80004)
#define REGISTER_ADDRESS_ATLAS_CPU_WD_TIMER_COUNT       (0xFFE80010)
#define REGISTER_ADDRESS_ATLAS_CPU_WD_CTL               (0xFFE80014)

#define REGISTER_ADDRESS_ATLAS_FLASH                    (0x10000000)
//#define REGISTER_ADDRESS_ATLAS_FLASH_START              (ATLAS_BASE_ADDRESS_FLASH)
#define REGISTER_ADDRESS_ATLAS_FLASH_END                (0x17FFFFFF)
//#define ATLAS_MAX_ADDRESSABLE_FLASH                     (ATLAS_BASE_ADDRESS_FLASH - ATLAS_BASE_ADDRESS_FLASH_END)

#define REGISTER_ADDRESS_ATLAS_FMU_HOST_DIAGNOSTIC      (0x60000008)


#define REGISTER_DATA_MASK_ATLAS_CHIP_ID                (0xFF00)
#define REGISTER_DATA_ATLAS_CHIP_ID_HIGH                (0xC000)

#define REGISTER_DATA_ATLAS_VENDOR_ID                   (0x1000)


#if 1
/*static U32 sSBR_OFFSET_TABLE[] = {
    0x000400, 0x040400, 0x080400, 0x0C0400, 0x100400
};*/

#define ATLAS_MAX_SBR_LOCATIONS                     (5)
#define ATLAS_MAX_SLEEP_FOR_RESET_IN_MILLISECONDS   (500)


/* BAR Registers Maps are like this */
/* 0 to 1 MB --> OCM, 1MB to 2MB --> PBAM Peripheral, 3MB - 4MB -->SPI Flash CS[1], 4MB onwards SPI Flash */

#define ATLAS_REGISTER_BAR0_SPI_FLASH_BASE_ADDRESS          (0x300000)
#define ATLAS_REGISTER_BAR0_SPI_FLASH_CONFIG_BASE_ADDRESS   (0x100000 + 0xC0000)        //1 MB + SPI Config offset

#define ATLAS_REGISTER_SPI_FLASH_BASE_ADDRESS             (0x10000000) /* We have to use the alias */
#define ATLAS_REGISTER_SPI_FLASH_CONFIG_BASE_ADDRESS      (0x2A0C0000)
#define ATLAS_REGISTER_SPI_FLASH_CS0_CONFIG_OFFSET        (0x0)
#define ATLAS_REGISTER_SPI_FLASH_CS0_CONFIG_OFFSET_SIZE   (0x18)
#define ATLAS_REGISTER_SPI_CHIPSELECT_COUNT               (1)
#define ATLAS_REGISTER_SPI_PBAM_MASK                      (0xFFFFFFFFl)
#define ATLAS_SPI_FLASH_MAX_SRAM_BUFFER_SIZE              (512)
#define ATLAS_SPI_FLASH_TIMEOUT                           (0x100)

#define ATLAS_RECOVERY_OFFSET_FOR_FIRMWARE                (0x80000)
#define ATLAS_RECOVERY_OFFSET_FOR_SBR                     (0x400)

#define ATLAS_BASE_ADDRESS_SRK                            (0x2A028000)

#define ATLAS_BASE_ADDRESS_BOOTSTRAP            (0x00000000)
#define ATLAS_BASE_ADDRESS_FLASH                (0x10000000)
#define ATLAS_BASE_ADDRESS_FLASH_START          (ATLAS_BASE_ADDRESS_FLASH)
#define ATLAS_BASE_ADDRESS_FLASH_END            (0x17FFFFFF)
#define ATLAS_MAX_ADDRESSABLE_FLASH             (ATLAS_BASE_ADDRESS_FLASH - ATLAS_BASE_ADDRESS_FLASH_END)
#define ATLAS_BASE_ADDRESS_FLASH_CS1            (0x18000000)

#define ATLAS_REGISTER_CPU_WD_RESET_THRESHOLD   (0xFFE80004)
#define ATLAS_REGISTER_CPU_WD_TIMER_COUNT       (0xFFE80010)
#define ATLAS_REGISTER_CPU_WD_CTL               (0xFFE80014)

#define ATLAS_REGISTER_FMU_HOST_DIAGNOSTIC      (0x60000008)

#define ATLAS_REGISTER_BAR0_SPI_FLASH_BASE_ADDRESS          (0x300000)
#define ATLAS_REGISTER_BAR0_SPI_FLASH_CONFIG_BASE_ADDRESS   (0x100000 + 0xC0000)        //1 MB + SPI Config offset


#define ATLAS_REGISTER_GEP_BAR0_PSB_CHIME_BASE_ADDRESS    (0x800000)        /* 8th MB is the PSB Chime Offset */
#define ATLAS_REGISTER_CHIME_GLOBAL_CHIP_REGISTER_OFFSET  (0x1F0000)        /* Global Chip Register Offset */
#define ATLAS_REGISTER_CHIME_TO_AXI_ADDRESS_OFFSET        (0x000100)        /* AXI Address register */
#define ATLAS_REGISTER_CHIME_TO_AXI_DATA_OFFSET           (0x000104)        /* AXI Data register */
#define ATLAS_REGISTER_CHIME_TO_AXI_CONTROL_STATUS_OFFSET (0x000108)        /* AXI Control Status register */

#define ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE       (0x60800000)
#define ATLAS_REGISTER_SIZE_PORT_CONFIG_SPACE_SIZE              (0x1000)
#define ATLAS_REGISTER_OFFSET_HARDWARE_CHIP_ID_REVISION         (0x0B7C)

#define ATLAS_BASE_ADDRESS_CCR                  (0xFFF00000)
#define ATLAS_REGISTER_CCR_CHIP_DEVICE_ID       (ATLAS_BASE_ADDRESS_CCR + 0x0000)
#define ATLAS_REGISTER_CCR_CHIP_REVISION        (ATLAS_BASE_ADDRESS_CCR + 0x0004)
#define ATLAS_REGISTER_CCR_POR_SENSE            (ATLAS_BASE_ADDRESS_CCR + 0x000C)
#define ATLAS_REGISTER_CCR_RESET_STATUS         (ATLAS_BASE_ADDRESS_CCR + 0x00A0)
#define ATLAS_REGISTER_CCR_SECURE_BOOT_STATUS   (ATLAS_BASE_ADDRESS_CCR + 0x00A4)
#define ATLAS_REGISTER_CCR_ATLAS_MODE           (ATLAS_BASE_ADDRESS_CCR + 0x00B0)
#define ATLAS_REGISTER_CCR_MISC_CONFIG_1        (ATLAS_BASE_ADDRESS_CCR + 0x00B4)
#define ATLAS_REGISTER_CCR_MISC_CONFIG_2        (ATLAS_BASE_ADDRESS_CCR + 0x00B8)
#define ATLAS_REGISTER_CCR_BOOT_MODE            (ATLAS_BASE_ADDRESS_CCR + 0x00D0)
#define ATLAS_REGISTER_CCR_BOOT_ADDRESS         (ATLAS_BASE_ADDRESS_CCR + 0x00D4)
#define ATLAS_REGISTER_CCR_SYSTEM_RESET_ENABLE  (ATLAS_BASE_ADDRESS_CCR + 0x00F0)
#define ATLAS_REGISTER_CCR_SYSTEM_RESET_GEN     (ATLAS_BASE_ADDRESS_CCR + 0x00F4)
#define ATLAS_REGISTER_CCR_MODULE_RESET_0       (ATLAS_BASE_ADDRESS_CCR + 0x0410)

#define ATLAS_REGISTER_BASE_ADDRESS_PSB_PORT_CONFIG_SPACE       (0x60800000)
#define ATLAS_REGISTER_SIZE_PORT_CONFIG_SPACE_SIZE              (0x1000)


#define ATLAS_REGISTER_PMG_REG_PORT_LINK_CAP             		0x074
#define ATLAS_REGISTER_PMG_REG_PORT_LINK_STATUS            		0x078

#define ATLAS_REGISTER_PMG_REG_PORT_CLOCK_EN_0             		0x30C                // Port clock enable for 0-31
#define ATLAS_REGISTER_PMG_REG_PORT_CLOCK_EN_1             		0x310                // Port clock enable for 32-63
#define ATLAS_REGISTER_PMG_REG_PORT_CLOCK_EN_2             		0x314                // Port clock enable for 64-95
#define ATLAS_REGISTER_PMG_REG_PORT_CLOCK_EN_3             		0x318                // Port clock enable for 116 & 117

#define ATLAS_PMG_MAX_STN                		8     /**< Max stations in chip */
#define ATLAS_PMG_MAX_STNPORT                 	16    /**< Max ports per station in chip */

/** Max total ports in chip */
#define ATLAS_PMG_MAX_PORT                    (128)               /**< Max port number, must match HALI_PCIE_MAX_PORT_COUNT */
#define ATLAS_PMG_MAX_PORT_NO_2X1             (96)                /**< Max port number, without 2 x1 ports */
#define ATLAS_PMG_MAX_PORT_MGMT               (ATLAS_PMG_MAX_PORT + 2)  /**< Max total ports inc Mgmt & parent DS */

#define ATLAS_PMG_PORT_NUM_X1_1                   116     // x1 DS ports
#define ATLAS_PMG_PORT_NUM_X1_2                   117


#endif

#endif /* __ATLAS_REGISTERS__H__ */

