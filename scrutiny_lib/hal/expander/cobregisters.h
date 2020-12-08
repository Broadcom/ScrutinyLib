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


#ifndef __EXPANDER_REGISTERS__H__
#define __EXPANDER_REGISTERS__H__

/*
 * Cobra/Cub Register addresses
 */

#define REGISTER_ADDRESS_COBRA_FLASH_START              (0x10000000)
#define REGISTER_ADDRESS_COBRA_DEVICE_SIGNATURE         (0xC380002C)
#define REGISTER_ADDRESS_COBRA_RESET_EXPANDER           (0xC3800200)

#define SIGNATURE_VALUE_COBRA_FLASH_SIG0_MASK           (0xFFFFFF00)
#define SIGNATURE_VALUE_COBRA_FLASH_SIG0_DATA           (0xEA000000)
#define COBRA_MAX_SLEEP_FOR_RESET_IN_MILLISECONDS       (500)

#define REGISTER_ADDRESS_COBRA_EXT_PORT_REGS            (0xC0000000)
#define REGISTER_SIZE_COBRA_LINK_REGS                   (0x80000)
#define REGISTER_SIZE_COBRA_EDFB_REGS                   (0x80000)

/*
 * Macro to get Expander LINK Regs address for given Physical Phy Id.
 */

#define REGISTER_ADDRESS_COBRA_EXP_LINK_REGS(PhysicalPhyId)  \
    ( REGISTER_ADDRESS_COBRA_EXT_PORT_REGS                   \
      + ( PhysicalPhyId * ( REGISTER_SIZE_COBRA_LINK_REGS    \
                          + REGISTER_SIZE_COBRA_EDFB_REGS ) ) )

/*
 * Margay register addresses
 */

#define REGISTER_ADDRESS_MARGAY_CONFIGURATION_STATUS    (0x43400000)
#define REGISTER_ADDRESS_MARGAY_COMPONENT_ID_SIGNATURE  (REGISTER_ADDRESS_MARGAY_CONFIGURATION_STATUS + 0x2C)

#define REGISTER_ADDRESS_MARGAY_SAS_ADDRESS_HIGH        (0x40000048)
#define REGISTER_ADDRESS_MARGAY_SAS_ADDRESS_LOW         (0x4000004C)

#define REGISTER_ADDRESS_MARGAY_XMEM_FLASH_START        (0x50000000)
#define REGISTER_ADDRESS_MARGAY_XMEM_FLASH_END          (0x57FFFFFF)

#define REGISTER_ADDRESS_MARGAY_SPI_FLASH_CONFIG        (0x44100000)

#define REGISTER_ADDRESS_MARGAY_SPI_FLASH_START         (0x10000000)
#define REGISTER_ADDRESS_MARGAY_SPI_FLASH_END           (0x17FFFFFF)

#define REGISTER_ADDRESS_MARGAY_XMEM_CONTROL            (0x44200000)
#define REGISTER_ADDRESS_MARGAY_POWER_ON_SENSE          (REGISTER_ADDRESS_MARGAY_XMEM_CONTROL + 0x0000)
#define REGISTER_ADDRESS_MARGAY_XMEM_NOR_FLASH_CONFIG   (REGISTER_ADDRESS_MARGAY_XMEM_CONTROL + 0x0004)

#endif /* __EXPANDER_REGISTERS__H__ */

