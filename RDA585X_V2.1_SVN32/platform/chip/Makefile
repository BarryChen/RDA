## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/chip

# Compile the sub-modules, except when the "chip" must be used as a library.
ifeq ($(findstring platform/chip/packed,${GLOBAL_BINARY_LIBS}),)

LOCAL_API_DEPENDS    := 
LOCAL_MODULE_DEPENDS := \
						platform/chip/boot \
						platform/chip/hal \


#						platform/chip/bcpu \
						platform/chip/patch \
						platform/chip/pal \

ifeq ($(CT_ASICSIMU_DRIVERS),1)
LOCAL_MODULE_DEPENDS += platform/chip/asicsimu
endif # CT_ASICSIMU_DRIVERS=1

# To set the REWRITE_ROM_AT_BOOT flag
ifeq "${REWRITE_ROM_AT_BOOT}" "1"
LOCAL_EXPORT_FLAG  += REWRITE_ROM_AT_BOOT
endif

else # GLOBAL_BINARY_LIBS

LOCAL_API_DEPENDS    := 
LOCAL_BINARY_LIBS    += platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}
LOCAL_MODULE_DEPENDS := platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}

ifeq "${NUMBER_OF_SIM}" "2"

ifeq "${GPRS_SUPPORT}" "1"
LOCAL_BINARY_LIBS    += platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_dualsim_gprs
LOCAL_MODULE_DEPENDS := platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_dualsim_gprs
else # !GPRS_SUPPORT
LOCAL_BINARY_LIBS    += platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_dualsim
LOCAL_MODULE_DEPENDS := platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_dualsim
endif # GPRS_SUPPORT

else # !DUAL_SIM_SUPPORT

ifeq "${GPRS_SUPPORT}" "1"
LOCAL_BINARY_LIBS    += platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_singlesim_gprs
LOCAL_MODULE_DEPENDS := platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_singlesim_gprs
else # !GPRS_SUPPORT
LOCAL_BINARY_LIBS    += platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_singlesim
LOCAL_MODULE_DEPENDS := platform/chip/packed/chip_${CT_ASIC}_${CT_ASIC_CFG}/chip_singlesim
endif # GPRS_SUPPORT

endif # DUAL_SIM_SUPPORT

endif # GLOBAL_BINARY_LIBS

# This is a top-level module.
IS_TOP_LEVEL := yes
## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
