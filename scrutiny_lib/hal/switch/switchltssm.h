/*****************************************************************************
 *                                                                           *
 * Copyright (C) 2019 - 2020 Broadcom. All Rights Reserved.                  *
 * Broadcom Confidential. The term "Broadcom" refers to Broadcom Inc.        *
 * and/or its subsidiaries.                                                  *
 *                                                                           *
 *****************************************************************************
 *
 *        Name : switchaladin.h
 *        Programmer : 
 *        Creation Date : Jan 20th, 2021  
 *
 *  Description
 *  ------------
 *
 *        Pcie LTSSM APIs header file
 *
 *
 *******************************************************************************
 *
 */


#ifndef __SWITCH_LTSSM__H__
#define __SWITCH_LTSSM__H__


#define LTSSM_FILE_DEFAULT_DUMP       	"ltssm.log"

#define ALADIN_LTSSM_TRIGGER			(0x103)      // Polling.Active
 
#define SWITCH_ALADIN_REG_OFFSET_PER_STATION(station,offset) (ATLAS_REGISTER_BASE_ADDRESS_PSB_STATION_BASE + ((station)<<16) + ALADIN_BASE + (offset))


SCRUTINY_STATUS sltLtssmSetup ( __IN__ PTR_SCRUTINY_DEVICE PtrDevice, __IN__ U32 Port);

SCRUTINY_STATUS sltLtssmIsTriggerred ( 
	__IN__ PTR_SCRUTINY_DEVICE  PtrDevice,
	__IN__ U32                  Port, 
	__IN__ PU32                 PtrCaptureStatus
);

SCRUTINY_STATUS sltStopAndGetLtssm (
	__IN__ PTR_SCRUTINY_DEVICE              PtrDevice,
	__IN__ U32                              Port,
	__IN__ const char*                      PtrDumpFilePath
);


#endif

