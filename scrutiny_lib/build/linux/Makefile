#****************************************************************************
#  									    *
#	Copyright (C) 2019 - 2020 Broadcom. All Rights Reserved.            *
# 	Broadcom Confidential. The term "Broadcom" refers to Broadcom Inc.  *
#   and/or its subsidiaries.                                                *
#                                                                           *
#****************************************************************************
#
#            Name:  Makefile
#           Title:  Makefile for Linux version of scrutiny lib
#      Programmer:  Pravin Ranganathan
#   Creation Date:  April 05, 2019
#
#   Version History
#   ---------------
#
#     Date    Who?      			Description
#   --------  ------------------	--------------------------------------
#   04/05/19  Pravin Ranganathan      Initial build#
#
#   This makefile builds the Linux version of scrutiny library.
#
#*****************************************************************************
#

YES = 1
NO = 0

DEBUG = $(YES)

ifeq ($(ARM),1)
ARCH = arm64
else
ARCH = $(shell uname -m)
endif

ifeq ($(PRODUCT_SUPPORT_AVENGER), )
PRODUCT_SUPPORT_AVENGER=0
endif

ifeq ($(LIB_SUPPORT_CONTROLLER), )
LIB_SUPPORT_CONTROLLER=0
LIB_SUPPORT_CONTROLLER_IT=0
LIB_SUPPORT_CONTROLLER_MR=0
endif

ifeq ($(LIB_SUPPORT_EXPANDER), )
LIB_SUPPORT_EXPANDER=0
endif

ifeq ($(LIB_SUPPORT_SWITCH), )
LIB_SUPPORT_SWITCH=1
endif

LIBRARY_NAME = libscrutiny
ECHO := $(shell echo)
#
# Clean
#
ERASE	= rm

.SUFFIXES: .o .c

ifeq ($(PRODUCT_SUPPORT_AVENGER),1)
	$(ECHO) Building for Avenger Product
else
	$(ECHO) Building for Pre-Avenger Product
endif

#-------------------------------------------------------------------------------------------
# Paths
#-------------------------------------------------------------------------------------------

CORE_DIR 		= ../..

ifeq ($(LIB_SUPPORT_CONTROLLER),1)
DIR_MPI 		= $(CORE_DIR)/../../fwapi/mpi2/

ifeq ($(PRODUCT_SUPPORT_AVENGER),1)
DIR_MPI_3 		= $(CORE_DIR)/../../fwapi/mpi3/
endif

DIR_MR8HDR 		= $(CORE_DIR)/../../fwapi/fwsasmegaapi/
DIR_MR7HDR		= $(CORE_DIR)/includes/fwsas35megaapi/

DIR_IAL_MPT 	= $(DIR_IAL)/mpi/
DIR_IAL_MFI 	= $(DIR_IAL)/mfi

DIR_HAL_IT 		= $(DIR_HAL)/it/
DIR_HAL_MR 		= $(DIR_HAL)/mr/

endif

DIR_IAL 		= $(CORE_DIR)/ial/

DIR_IAL_SG 		= $(DIR_IAL)/sg
DIR_IAL_PCI 	= $(DIR_IAL)/pci
DIR_IAL_COM 	= $(DIR_IAL)/com
DIR_IAL_SDB 	= $(DIR_IAL)/sdb

DIR_IAL_BOOTSTRAP  = $(DIR_IAL)/bootstrap

DIR_HAL 		= $(CORE_DIR)/hal/
DIR_HAL_SWITCH 	= $(DIR_HAL)/switch/
DIR_HAL_SCSI 	= $(DIR_HAL)/scsi/

ifeq ($(LIB_SUPPORT_EXPANDER),1)
DIR_HAL_EXPANDER= $(DIR_HAL)/expander/
endif

#DIR_SPI      	= $(CORE_DIR)/spi
DIR_BUILD 		= $(CORE_DIR)/build/linux

ifeq ($(ARCH),x86_64)
OUT_PATH = $(DIR_BUILD)/bin/x86_64
OBJ_PATH = $(DIR_BUILD)/obj/x86_64
else ifeq ($(ARCH),arm64)
OUT_PATH = $(DIR_BUILD)/bin/arm64
OBJ_PATH = $(DIR_BUILD)/obj/arm64
else ifeq ($(ARCH),ppc64)
OUT_PATH = $(DIR_BUILD)/bin/ppc64
OBJ_PATH = $(DIR_BUILD)/obj/ppc64
else ifeq ($(ARCH),ppc64le)
OUT_PATH = $(DIR_BUILD)/bin/ppc64le
OBJ_PATH = $(DIR_BUILD)/obj/ppc64le
else 	
OUT_PATH = $(DIR_BUILD)/bin/x86
OBJ_PATH = $(DIR_BUILD)/obj/x86
endif

#------------------------------------------------------------------------------------------------
# Compiler Options
#------------------------------------------------------------------------------------------------
ifeq ($(ARCH),arm64)                                                                                                                            
	CC 	= aarch64-linux-gnu-gcc
	STRIP   = aarch64-linux-gnu-strip
	ARCHIVE	= aarch64-linux-gnu-ar
else ifeq ($(ARCH),ppc64)                                                                                                                            
	CC 	= powerpc64-linux-gnu-gcc
	STRIP = powerpc64-linux-gnu-strip
	ARCHIVE	= powerpc64-linux-gnu-ar
else ifeq ($(ARCH),ppc64le)   
	CC 	= gcc
	STRIP = strip 
	ARCHIVE	= ar
else
	CC  = gcc
	STRIP = strip
	ARCHIVE	= ar
endif                                                                                                                      
  
COMPILERFLAGS	:= -DOS_LINUX -fPIC -Wall -pthread -Wno-missing-braces

ifeq ($(DEBUG), $(YES))
	COMPILERFLAGS	+= -g -DOS_LINUX -fPIC -Wall
endif

ifeq ($(PRODUCT_SUPPORT_AVENGER),1)
	COMPILERFLAGS 	+= -DPRODUCT_SUPPORT_AVENGER
endif

ifeq ($(ARCH),ppc64le)
	COMPILERFLAGS += -mlittle-endian -mcpu=powerpc64le
endif

ifeq ($(ARCH),x86_64)
	COMPILERFLAGS   += -DTARGET_ARCH_64BIT
else ifeq ($(ARCH),arm64)
	COMPILERFLAGS   += -DTARGET_ARCH_64BIT
else ifeq ($(ARCH),ppc64le)
	COMPILERFLAGS   += -DTARGET_ARCH_64BIT
else ifeq ($(ARCH),ppc64)
	COMPILERFLAGS   += -DTARGET_ARCH_64BIT
else
	COMPILERFLAGS   += -DTARGET_ARCH_32BIT
endif

ifeq ($(LIB_SUPPORT_CONTROLLER),1)
	COMPILERFLAGS 	+= -DLIB_SUPPORT_CONTROLLER
	
	ifeq ($(LIB_SUPPORT_CONTROLLER_IT),1)
		COMPILERFLAGS 	+= -DLIB_SUPPORT_CONTROLLER_IT
	endif
	
	ifeq ($(LIB_SUPPORT_CONTROLLER_MR),1)
		COMPILERFLAGS 	+= -DLIB_SUPPORT_CONTROLLER_MR
	endif	
endif

ifeq ($(LIB_SUPPORT_EXPANDER),1)
	COMPILERFLAGS 	+= -DLIB_SUPPORT_EXPANDER
endif

ifeq ($(LIB_SUPPORT_SWITCH),1)
	COMPILERFLAGS 	+= -DLIB_SUPPORT_SWITCH
endif

LINKERFLAGS			= -shared -pthread 

ifeq ($(LIB_SUPPORT_CONTROLLER),1)
ifeq ($(PRODUCT_SUPPORT_AVENGER),1)
	INCLUDE_PATH := -I/usr/local/include/sysfs -I$(DIR_MPI) -I$(DIR_MR8HDR)  -I$(DIR_MR7HDR) -I$(DIR_MPI_3)
else
	INCLUDE_PATH := -I/usr/local/include/sysfs -I$(DIR_MPI) -I$(DIR_MR7HDR)
endif
INCLUDE_PATH += -I$(DIR_IAL_MPT) -I$(DIR_IAL_MFI)
INCLUDE_PATH += -I$(DIR_HAL_IT) -I$(DIR_HAL_MR)
endif

INCLUDE_PATH += -I$(CORE_DIR) -I$(CORE_DIR)/includes/  -I$(CORE_DIR)/api

ifeq ($(LIB_SUPPORT_SWITCH),1)
INCLUDE_PATH += -I$(DIR_IAL_SG) -I$(DIR_IAL_PCI) -I$(DIR_IAL_COM)  -I$(DIR_IAL_SDB)
INCLUDE_PATH += -I$(DIR_HAL_SWITCH) -I$(DIR_HAL_SCSI)
endif

ifeq ($(LIB_SUPPORT_EXPANDER),1)
INCLUDE_PATH += -I$(DIR_IAL_SG) -I$(DIR_IAL_COM)  -I$(DIR_IAL_SDB) -I$(DIR_IAL_BOOTSTRAP)
INCLUDE_PATH += -I$(DIR_HAL_SCSI) -I$(DIR_HAL_EXPANDER)
endif

#-------------------------------------------------------------------------------------------------
# Sources PAL module
#-------------------------------------------------------------------------------------------------

CORE_OBJ := $(CORE_DIR)/scrutinylib.o
CORE_OBJ += $(CORE_DIR)/libdevmgr.o
CORE_OBJ += $(CORE_DIR)/libosal.o
CORE_OBJ += $(CORE_DIR)/libinternal.o
CORE_OBJ += $(CORE_DIR)/libdebug.o
CORE_OBJ += $(CORE_DIR)/libconfigini.o

#-------------------------------------------------------------------------------------------------
# Sources IAL - MPT module
#-------------------------------------------------------------------------------------------------

ifeq ($(LIB_SUPPORT_CONTROLLER),1)
	#MPT
	IAL_OBJ := $(DIR_IAL_MPT)/mpilinux.o
	IAL_OBJ += $(DIR_IAL_MPT)/mpicommon.o
	IAL_OBJ += $(DIR_IAL_MPT)/mpi2common.o

	ifeq ($(PRODUCT_SUPPORT_AVENGER),1)
		IAL_OBJ += $(DIR_IAL_MPT)/mpi3common.o
		IAL_OBJ += $(DIR_IAL_MPT)/mpi3os.o
	else
		IAL_OBJ += $(DIR_IAL_MPT)/mpi3stub.o
	endif

	# MR
	IAL_OBJ += $(DIR_IAL_MFI)/mrlinux.o
	IAL_OBJ += $(DIR_IAL_MFI)/mrcommon.o
endif

#ifeq ($(LIB_SUPPORT_SWITCH),1) || ($(LIB_SUPPORT_EXPANDER),1)
ifeq ($(LIB_SUPPORT_SWITCH),1)
	# COM
	IAL_OBJ += $(DIR_IAL_COM)/linuxserial.o
	IAL_OBJ += $(DIR_IAL_COM)/osalserial.o

	# SDB
	IAL_OBJ += $(DIR_IAL_SDB)/sdbconsole.o

	# SG
	IAL_OBJ += $(DIR_IAL_SG)/sglinux.o

	ifeq ($(LIB_SUPPORT_SWITCH),1)
		# PCI
		IAL_OBJ += $(DIR_IAL_PCI)/pciscan.o
		IAL_OBJ += $(DIR_IAL_PCI)/pcilinux.o
	endif

	ifeq ($(LIB_SUPPORT_EXPANDER),1)
		# Bootstrap
		#IAL_OBJ += $(DIR_IAL_BOOTSTRAP)/cobbsmgmt.o
	endif
endif


#-------------------------------------------------------------------------------------------------
# HAL Modules
#-------------------------------------------------------------------------------------------------

ifeq ($(LIB_SUPPORT_CONTROLLER),1)
	# IT
	HAL_OBJ := $(DIR_HAL_IT)/ctrldevmgr.o
	HAL_OBJ += $(DIR_HAL_IT)/ctrlcoredump.o
	HAL_OBJ += $(DIR_HAL_IT)/ctrlscsidiscovery.o
	HAL_OBJ += $(DIR_HAL_IT)/ctrlinventory.o
	HAL_OBJ += $(DIR_HAL_IT)/ctrltemperature.o
	HAL_OBJ += $(DIR_HAL_IT)/ctrlfault.o
	HAL_OBJ += $(DIR_HAL_IT)/ctrlhealthmon.o

	# MR
	HAL_OBJ += $(DIR_HAL_MR)/mrsnapdump.o
endif

#ifeq ($(LIB_SUPPORT_SWITCH), 1) || ($(LIB_SUPPORT_EXPANDER), 1)
ifneq ($(filter 1,$(LIB_SUPPORT_SWITCH) $(LIB_SUPPORT_EXPANDER)),)
	# SCSI
	HAL_OBJ += $(DIR_HAL_SCSI)/brcmscsisdk.o
	HAL_OBJ += $(DIR_HAL_SCSI)/brcmscsigettemperature.o

	ifeq ($(LIB_SUPPORT_EXPANDER),1)
		# EXPANDER
		HAL_OBJ += $(DIR_HAL_EXPANDER)/expdevmgr.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/cobra.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/margay.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/expcli.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/exptrace.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/exphealthlogs.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/expphyenable.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/expcoredump.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/expphy.o
		HAL_OBJ += $(DIR_HAL_EXPANDER)/exphealthmon.o
	endif
	
	ifeq ($(LIB_SUPPORT_SWITCH),1)
		# SWITCH
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchdevmgr.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/atlas.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/atlaspci.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchportcounters.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchcoredump.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchportproperties.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchhealthmon.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchsignalintegrity.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchregbla.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchlanemargin.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchaladin.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchportperformance.o
		HAL_OBJ += $(DIR_HAL_SWITCH)/switchltssm.o
		#HAL_OBJ += $(DIR_HAL_SWITCH)/switchsbr.o
	endif
 
endif


#--------------------------------------------------------------------------------------------------
# Combine all objects
#---------------------------------------------------------------------------------------------------

LIB_OBJECTS  := $(CORE_OBJ)
LIB_OBJECTS  += $(IAL_OBJ)
LIB_OBJECTS  += $(HAL_OBJ)
#LIB_OBJECTS  += $(SPILIB_OBJ)

#
# makefile default target
#

All : dirs $(LIBRARY_NAME)

dirs:
	mkdir -p $(OBJ_PATH)
	mkdir -p $(OUT_PATH)
	rm -rf $(OBJ_PATH)/*

#
# Implementation
#                                                                                                                            
.c.o :
	$(CC) $(COMPILERFLAGS) $(INCLUDE_PATH) -c $< -o $(OBJ_PATH)/$(@F)

#
# Targets
#
$(LIBRARY_NAME): $(LIB_OBJECTS)
	$(CC) -pthread  -o $(OUT_PATH)/$(LIBRARY_NAME).so $(INCLUDE_PATH) $(wildcard $(OBJ_PATH)/*.o) $(COMPILERFLAGS) $(LINKERFLAGS)
ifeq ($(DEBUG), $(NO))
	strip --strip-unneeded $(OUT_PATH)/$(LIBRARY_NAME).so
endif
	$(ARCHIVE) -r $(OUT_PATH)/$(LIBRARY_NAME).a $(wildcard $(OBJ_PATH)/*.o)

clean:
	$(ERASE) -f $(OUT_PATH)/$(LIBRARY_NAME).so $(OUT_PATH)/$(LIBRARY_NAME).a $(wildcard $(OBJ_PATH)/*.o)

cleanobj:
	$(ERASE) -f $(wildcard $(OBJ_PATH)/*.o)
