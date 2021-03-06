## Makefile automatically generated by NewTest.pl ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/download

LOCAL_API_DEPENDS :=    platform						\
                        platform/base/common			\
                        platform/chip/defs				\
                        platform/edrv/aud				\
                        platform/edrv/pmd


LOCAL_MODULE_DEPENDS := target \
                        platform/chip/hal \
                        platform/edrv/memd \
                        platform/base/std \
                        platform/edrv/pmd \
                        platform/edrv/rfd  \
                        platform/chip/boot


IS_ENTRY_POINT := yes

ifneq "$(CT_RELEASE)" "release"
$(error You can only compile the flash programmer with CT_RELEASE defined as release)
endif

export NO_BCPU_IMAGE := 1

LOCAL_EXPORT_FLAG += _FLASH_PROGRAMMER

# Discard charger code and depencies to SX.
LOCAL_EXPORT_FLAG += PMD_NO_CHARGER

# Disable trace as there is no OS in flash programmer
LOCAL_EXPORT_FLAG += SXS_NO_PRINTF SXS_NO_DUMP HAL_NO_PRINTF EDRV_NO_TRACE

# Disable profiling and SPI register debugging
ifneq "${CT_ASIC}" "greenstone"
export NO_PROFILE := 1
PROFILE_FLAGS := HAL_PROFILE_ON_BUFFER HAL_PROFILE_NOT_ON_ROMULATOR \
                 HAL_PROFILING PAL_PROFILING SX_PROFILING
TARGET_EXPORT_FLAG := $(filter-out $(PROFILE_FLAGS),$(TARGET_EXPORT_FLAG))
GLOBAL_EXPORT_FLAG := $(filter-out $(PROFILE_FLAGS),$(GLOBAL_EXPORT_FLAG))

export NO_SPI_REG_DEBUG := 1
TARGET_EXPORT_FLAG := $(filter-out SPI_REG_DEBUG,$(TARGET_EXPORT_FLAG))
GLOBAL_EXPORT_FLAG := $(filter-out SPI_REG_DEBUG,$(GLOBAL_EXPORT_FLAG))
LDPPFLAGS := $(filter-out -DSPI_REG_DEBUG,$(LDPPFLAGS))
endif # CT_ASIC != greenstone

# Check if it is a USB flash programmer
ifeq "${FASTPF_USE_USB}" "1"
LOCAL_EXPORT_FLAG += FASTPF_USE_USB
LOCAL_BATCH_NAME = usb
else
LOCAL_BATCH_NAME = host
endif

# Check if forcing to setup all GPIO pins as input mode and set all GPO pins to low level
FORCE_GPIO_INPUT_GPO_LOW ?= 1
ifeq "${FORCE_GPIO_INPUT_GPO_LOW}" "1"
LOCAL_EXPORT_FLAG += FORCE_GPIO_INPUT_GPO_LOW
endif


#####################################################
# FIXME: DEPRECATED???
LOCAL_EXPORT_FLAG += NO_RF 
#####################################################


# Avoids TGT to load the UCTLS services (mass storage, traces, etc) in
# the target configuration structure. This will avoid the linker to include
# the useless UCTLS function in the lodfile. Flash prog manages the Host
# over USB itself. 
export UCTLS_SERVICE := no

# We always want to compile the flash programmer as a Ramrun.
export CT_RAMRUN    := 1

# Generates the CoolWatcher headers automatically.
# This is for the flash programmer XMD.
#AUTO_XMD2H ?= yes

# Special linker to get everything in internal sram.
LINKER_MASTER := flash_download.ld

C_SRC := ${wildcard *.c}
S_SRC := ${wildcard *.S}
%/flash_programmer.o: CT_MIPS16_CFLAGS=

## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

## installing from inside make is not working because of some variables that are kept from first make, so everything has been moved to the bash script install.sh
install:
	@echo This was brocken, use "./install.rb" instead

FLSHPROG_INSTALL_DIR := ${SOFT_WORKDIR}/toolpool/plugins/fastpf/flash_programmers

flshprog_install_ff:
	@${ECHO} "INSTALL           ${LOCAL_BATCH_NAME}_${CT_TARGET}_ramrun.lod"
	cp ${LODBASE}ramrun.lod ${FLSHPROG_INSTALL_DIR}/${LOCAL_BATCH_NAME}_${CT_ASIC}_${FLSH_MODEL}_ramrun.lod

flshprog_install_eng:
	@${ECHO} "INSTALL           ${LOCAL_BATCH_NAME}_${CT_TARGET}_${FLSH_MODEL}_ramrun.lod"
	cp ${LODBASE}ramrun.lod ${FLSHPROG_INSTALL_DIR}/${LOCAL_BATCH_NAME}_${CT_TARGET}_${FLSH_MODEL}_ramrun.lod

flshprog_clean:
	rm -rf ${BINARY_PATH}
