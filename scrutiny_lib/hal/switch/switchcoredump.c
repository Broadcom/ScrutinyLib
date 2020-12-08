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

#include "libincludes.h"
#include "atlas.h"
#include "switchcoredump.h"


/* Bond Option strings */
static const char * const sPtrBondOptionString[] = {
                                               "96","80","64","48","32","24",
                                               "Unknown"
                                           };

/* Switch Revision strings */
static const char * const sPtrSwitchRevString[] = {
                                               "A0","B0","C0","C1","Unknown"
                                           };


/**
 *
 * @method  scdRegsAccessGetSwitchRevLevel()
 *
 *
 * @param   ChipRevLevel - Chip revision level.
 *
 * 
 * @return  HALI_SWITCH_REV_LEVEL - The switch revision level
 *
 * @brief   Get the switch revision level.
 *
 *
 */
HALI_SWITCH_REV_LEVEL scdRegsAccessGetSwitchRevLevel (U32 ChipRevLevel)
{
    HALI_SWITCH_REV_LEVEL switchRevLevel;

    /* Chip revision level */
    switch( ChipRevLevel )
    {
        case HAL_SWITCH_REVISION_A0:
            switchRevLevel = HALI_SWITCH_REV_LEVEL_0;
            break;

        case HAL_SWITCH_REVISION_B0:
            switchRevLevel = HALI_SWITCH_REV_LEVEL_1;
            break;

        /* Additional chip revisions should be added here */

        default:
            switchRevLevel = HALI_SWITCH_REV_LEVEL_UNKNOWN;
            break;
    }

    return switchRevLevel;
}


/**
 *
 * @method  scdCoreDumpPrintFwRevSection()
 *
 *
 * @param   PtrSectionData      pointer to original core dump
 *                              section data
 *
 * @param   PtrBuffer           pointer to the output buffer
 * 
 * @param   PtrBufferSize       returned decoded buffer length
 * 
 * @return  void                NIL
 *
 * @brief   decoded FW revision section
 *
 *
 */
void scdCoreDumpPrintFwRevSection (    
    __IN__  PU8                             PtrSectionData,
    __IN__  PU8                             PtrBuffer,
    __OUT__ PU32                            PtrBufferSize
)
{
    PTR_HALI_SWITCH_COREDUMP_SECTION_FWREV ptrFwRevData;
    HALI_SWITCH_REV_LEVEL           switchRevLevel;
    const char *imgStr, *modeStr;


    U32                     tempBufferOffset = 0;
    char *ptrBuffer = (char *) PtrBuffer;

    gPtrLoggerSwitch->logiFunctionEntry ("scdCoreDumpPrintFwRevSection (PtrSectionData=0x%x, PtrBuffer=0x%x, PtrBufferSize=0x%x)", 
                                            PtrSectionData, PtrBuffer, PtrBufferSize);

    ptrFwRevData = (PTR_HALI_SWITCH_COREDUMP_SECTION_FWREV) PtrSectionData;

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "\r\n-------------------------\r\n"
                                    "FIRMWARE REVISION DETAILS\r\n"
                                    "-------------------------\r\n\r\n"
                                    );

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "(Bond Option: %s)\r\n", sPtrBondOptionString[ptrFwRevData->HwBondOption]
                                     );

    switchRevLevel = scdRegsAccessGetSwitchRevLevel ( (ptrFwRevData->ChipRevLevel) );
    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                    "Switch Revision Level:  %u (%s)\r\n", ptrFwRevData->ChipRevLevel,
                                    sPtrSwitchRevString[switchRevLevel]
                                     );



    switch (ptrFwRevData->ActiveFwRegion)
    {
        case HALI_COREDUMP_ACTIVE_FWREGION_BOOT:
        {
            imgStr = "Boot Image";
            break;
        }
        case HALI_COREDUMP_ACTIVE_FWREGION_COPY1:
        {
            imgStr = "Firmware Copy 1";
            break;
        }
        case HALI_COREDUMP_ACTIVE_FWREGION_COPY2:
        {
            imgStr = "Firmware Copy 2";
            break;
        }

        case HALI_COREDUMP_ACTIVE_FWREGION_RAM:
        {
            imgStr = "RAM Image";
            break;
        }

        default:
        {
            imgStr = "Unknown Image";
            break;
        }
    }

    switch ((PMG_SW_MODE)(ptrFwRevData->OperatingMode))
    {
        case PMG_SW_MODE_BASE:
        {
            modeStr = "Base";
            break;
        }
        case PMG_SW_MODE_SMART_SWITCH:
        {
            modeStr = "Smart Switch";
            break;
        }
        default:
        {
            modeStr = "Unknown";
            break;
        }
    }

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                     EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                     "\r\nActive Firmware:  %s (Operating in %s Mode)\r\n", 
                                     imgStr, modeStr
                                    );




    if ( ptrFwRevData->ActiveFwRegion != HALI_COREDUMP_ACTIVE_FWREGION_RAM )
    {
        tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                         EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                         "\r\nFirmware Revision:  %d.%d.%d.%d\r\n",
                                        ptrFwRevData->FWVersion.Struct.Major,
                                        ptrFwRevData->FWVersion.Struct.Minor,
                                        ptrFwRevData->FWVersion.Struct.Unit,
                                        ptrFwRevData->FWVersion.Struct.Dev
                                        );
    } 
    else
    {
        tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                         EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                         "\r\nFirmware Revision:  Not Available\r\n"
                                        );
    }

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                         EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                        "HAL Revision:  %d.%d.%d.%d\r\n",
                                        ptrFwRevData->HalLibVersion.Struct.Major,
                                        ptrFwRevData->HalLibVersion.Struct.Minor,
                                        ptrFwRevData->HalLibVersion.Struct.Unit,
                                        ptrFwRevData->HalLibVersion.Struct.Dev
                                        );

    tempBufferOffset += sosiSprintf (ptrBuffer + tempBufferOffset,
                                         EXP_COREDUMP_INTERNAL_BUFFER_SIZE - tempBufferOffset,
                                        "SES Revision:  %d.%d.%d.%d\r\n",
                                        ptrFwRevData->SesLibVersion.Struct.Major,
                                        ptrFwRevData->SesLibVersion.Struct.Minor,
                                        ptrFwRevData->SesLibVersion.Struct.Unit,
                                        ptrFwRevData->SesLibVersion.Struct.Dev
                                        );

    *PtrBufferSize = tempBufferOffset;

    gPtrLoggerExpanders->logiFunctionExit ("scdCoreDumpPrintFwRevSection ()");
}
