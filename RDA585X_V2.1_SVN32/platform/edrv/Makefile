## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv

# Compile the sub-modules, except when the "chip" must be used as a library.
ifeq ($(findstring platform/edrv/packed,${GLOBAL_BINARY_LIBS}),)

# When releaselib_all.sh is used to update all libs, no need to compile edrv except rda btd.
ifneq (${NO_EDRV},)
LOCAL_MODULE_DEPENDS := platform/edrv/btd
else
LOCAL_MODULE_DEPENDS := \
                        platform/edrv/aud \
                        platform/edrv/btd \
                        platform/edrv/gpio \
                        platform/edrv/fmd \
                        platform/edrv/nfcd \
                        platform/edrv/lcdd \
                        platform/edrv/mcd \
                        platform/edrv/memd \
                        platform/edrv/pmd \
                        platform/edrv/rfd \
                        platform/edrv/tsd \
			   platform/edrv/i2cgpio \
												
#                        platform/edrv/atvd \
                        platform/edrv/dualsimd \
                        platform/edrv/gsensor \
                        platform/edrv/tsd \
                        platform/edrv/cammd \
                 
endif

ifeq  "${UART2_SUPPORT}" "1"
		LOCAL_MODULE_DEPENDS += platform/edrv/uart2
endif
ifeq "${WIFI_MODEL}" "rdawifi"
LOCAL_MODULE_DEPENDS += platform/edrv/wifi
endif

ifneq "${CSW_EXTENDED_API_CAMERA}" "1"
#LOCAL_MODULE_DEPENDS += platform/edrv/camdold
else
#LOCAL_MODULE_DEPENDS += platform/edrv/camd
endif

else # GLOBAL_BINARY_LIBS

LOCAL_BINARY_LIBS    += platform/edrv/packed/edrv_${CT_TARGET}
LOCAL_MODULE_DEPENDS := platform/edrv/packed/edrv_${CT_TARGET}

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

# This module is a packed module.
BINLIB_PATH      := ${SOFT_WORKDIR}/platform/edrv/packed/edrv_${CT_TARGET}/lib
LOCAL_BINLIBRARY := ${BINLIB_PATH}/libedrv_${CT_TARGET}_${CT_RELEASE}.a

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
