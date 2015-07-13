## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/chip/hal

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform                       \
                    platform/base/common            \
                    platform/base/sx                \
                    platform/base/std               \
                    platform/calib                  \
                    platform/chip/bcpu              \
                    platform/chip/boot              \
                    platform/chip/boot/$(CT_ASIC)   \
                    platform/chip/defs              \
                    platform/chip/pal               \
                    platform/chip/patch             \
                    platform/chip/regs/$(CT_ASIC)   \
                    platform/edrv/memd              \
                    platform/edrv/pmd               \
                    platform/edrv/pmd/${PM_MODEL}   \
                    platform/edrv/rfd               \
                    target                          \
                    platform/csw/                   \
                    platform/mdi/alg/sco_vpp	    \

# list all the modules that need to be compiled and added to this module library
LOCAL_MODULE_DEPENDS := platform/chip/regs/$(CT_ASIC)    \

# This is a top-level module
IS_TOP_LEVEL := yes

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

# Forces the trace to be sent over USB Host.
#LOCAL_EXPORT_FLAG += HAL_TRACE_USB

# Enable some drivers to be more or less verbose.
#LOCAL_EXPORT_FLAG += HAL_USB_PRINTF
#LOCAL_EXPORT_FLAG += HAL_CAMERA_PRINTF
#LOCAL_EXPORT_FLAG += HAL_NO_PRINTF
LOCAL_EXPORT_FLAG += PAL_NO_DBGTRC

# Compilation mode of HAL used, for the version string.
LOCAL_EXPORT_FLAG += HAL_RELEASE_MODE=${CT_RELEASE}
LOCAL_EXPORT_FLAG += HAL_ASIC_CAPD=${CT_ASIC_CAPD}_${CT_ASIC_CFG}

ifeq "${CT_RELEASE}" "cool_profile"
LOCAL_EXPORT_FLAG += ENABLE_PXTS
LOCAL_EXPORT_FLAG += PXTS_MAX_SUPPORTED_FREQUENCY=52000000
# FIXME: This is a bit permissive. Should only be used in SX...
# Necessary here as a part of the jobs are profiled in HAL.
LOCAL_EXPORT_FLAG += SX_PROFILING
endif

# Configurable exception behaviour:
# User can choose whether to restart system or enter GDB when exception occurs.
# Only check this flag when CT_RELEASE is "release"
HAL_GDB_CONFIG ?= 1
ifeq "${CT_RELEASE}" "release"
ifeq "${HAL_GDB_CONFIG}" "1"
LOCAL_EXPORT_FLAG += HAL_GDB_CONFIG
endif
endif

# Whether to skip checking BCPU error info
ifeq "${NO_BCPU_IMAGE}" "1"
LOCAL_EXPORT_FLAG += NO_BCPU_IMAGE
endif

# Gouda timing updating workaround
ifeq "${CT_CHIP_DIE}" "8805"
LOCAL_EXPORT_FLAG += GOUDA_UPDATE_TIMINGS_WORKAROUND
endif

ifeq "${CT_ASIC}" "8808"
#LOCAL_EXPORT_FLAG += USE_DIGITAL_CLK
ifeq "${USE_SPI_LCD}" "1"
LOCAL_EXPORT_FLAG += USE_SPI_LCD
LOCAL_EXPORT_FLAG += GOUDA_UPDATE_TIMINGS_WORKAROUND
endif
endif

# MIC capless mode
ifeq "${MIC_CAPLESS_MODE}" "1"
LOCAL_EXPORT_FLAG += MIC_CAPLESS_MODE
endif

# FM line-in record
ifeq "${FM_LINEIN_RECORD}" "1"
LOCAL_EXPORT_FLAG += FM_LINEIN_RECORD
endif

ifeq ($(CT_ASICSIMU_DRIVERS),1)
LOCAL_API_DEPENDS += platform/chip/asicsimu
endif # CT_ASICSIMU_DRIVERS=1

## ------------------------------------- ##
##  List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
