## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/chip/boot

# List all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
                     platform \
                     platform/calib \
                     platform/base/common \
                     platform/base/std \
                     platform/base/sx \
                     platform/chip/boot \
                     platform/chip/defs \
                     platform/chip/hal \
                     platform/chip/regs/$(CT_ASIC) \
                     platform/chip/boot/$(CT_ASIC) \
                     platform/edrv/memd \
                     target \
                     target/${CT_TARGET}

# List all the modules that need to be compiled and added to this module library
LOCAL_MODULE_DEPENDS ?= 

ifneq "${RAM_PHY_SIZE}" ""
LOCAL_EXPORT_FLAG += RAM_SIZE=${RAM_PHY_SIZE}
else
LOCAL_EXPORT_FLAG += RAM_SIZE=${RAM_SIZE}
endif

# List other APIs that are neccessary to compile this module
LOCAL_ADD_INCLUDE =  \
                     platform/chip/boot/$(CT_ASIC)/src \

# This is a top-level module
IS_TOP_LEVEL := yes

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

# Generate the revision (version) file automatically during the make process.
# Used by the USB boot and boot ROM.
AUTO_GEN_REVISION_HEADER := yes

# The profiling is always enabled in the boot (except for Granite).
# But the configuration of the IOMux must be off.
ifneq (,$(findstring BOOT_WITH_IOMUX_PROFILE, $(ALL_EXPORT_FLAG)))
    $(warning "!!!! IOMux configured for PXTS in the boot code !!!!")
endif

# We don't want any warning in the boot code.
CFLAGS += -Wall -Werror

# We require a code as small as possible
CT_OPT := dbg_size

# This is only valid as there is no module under in the compilation
# tree.
ifneq "${CT_OPT}" "dbg_size"
$(warning "!!! Compiling boot without CT_OPT=dbg_size wil take extra size !!!")
endif

# Add here things you would want to be defined (the same as -D in gcc)
LOCAL_EXPORT_FLAG  += ROM_LINKED_AGAINST_VERSION=${ROM_LINKED_AGAINST_VERSION}

# To set the ROM version
ifeq "${ROM_VERSION}" ""
ROM_VERSION_MAJOR ?= 1
ROM_VERSION_MINOR ?= 1

ROM_BUILD_DATE = $(shell echo $(BUILD_DATE) | sed 's/^20//')
ROM_VERSION ?= 0x$(ROM_VERSION_MAJOR)$(ROM_BUILD_DATE)$(ROM_VERSION_MINOR)
endif
LOCAL_EXPORT_FLAG += ROM_VERSION_NUMBER=${ROM_VERSION}

# To set the CT_ASIC_CAPD
LOCAL_EXPORT_FLAG += CT_ASIC_CAPD=${CT_ASIC_CAPD}

# To set the NAND flash boot code size
LOCAL_EXPORT_FLAG += NAND_FLASH_BOOT_CODE_SIZE=${NAND_FLASH_BOOT_CODE_SIZE}

LOCAL_EXPORT_FLAG  +=

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# This vpath is used to get the sources from the chip directory.
vpath %.c . $(CT_ASIC)
vpath %.S . $(CT_ASIC)
# Include the sources ROMed. They depend on the chip.
include $(CT_ASIC)/src/make.srcs
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
