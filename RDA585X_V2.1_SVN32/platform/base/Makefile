## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/base

# Compile the sub-modules, except when the "base" must be used as a library.
ifeq ($(findstring platform/base/packed,${GLOBAL_BINARY_LIBS}),)

# List of submodules which contain code we need to include in the final lib
LOCAL_MODULE_DEPENDS := \
					    platform/base/dbg \
					    platform/base/sap \
					    platform/base/std \
					    platform/base/sx \

else # GLOBAL_BINARY_LIBS

ifeq "${NUMBER_OF_SIM}" "2"

ifeq "${GPRS_SUPPORT}" "1"
LOCAL_BINARY_LIBS    += platform/base/packed/base_dualsim_gprs
LOCAL_MODULE_DEPENDS := platform/base/packed/base_dualsim_gprs
else # !GPRS_SUPPORT
LOCAL_BINARY_LIBS    += platform/base/packed/base_dualsim
LOCAL_MODULE_DEPENDS := platform/base/packed/base_dualsim
endif # GPRS_SUPPORT

else # !DUAL_SIM_SUPPORT

ifeq "${GPRS_SUPPORT}" "1"
LOCAL_BINARY_LIBS    += platform/base/packed/base_singlesim_gprs
LOCAL_MODULE_DEPENDS := platform/base/packed/base_singlesim_gprs
else # !GPRS_SUPPORT
LOCAL_BINARY_LIBS    += platform/base/packed/base_singlesim
LOCAL_MODULE_DEPENDS := platform/base/packed/base_singlesim
endif # GPRS_SUPPORT

endif # DUAL_SIM_SUPPORT

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
