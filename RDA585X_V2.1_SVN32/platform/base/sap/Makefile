## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/base/sap

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/base/common \
	platform/base/sx \
	platform/base/std \
	platform/csw \
	platform/stack \
	platform/chip/hal \
	platform/chip/defs \
	platform/chip/pal \
	platform/phy

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := 

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

ifeq "${DUAL_SIM_SUPPORT}" "1"
MYCPPFLAGS += -DSTK_DUAL_SIM
endif

ifeq "${GPRS_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += __GPRS__
endif

LOCAL_EXPORT_FLAG += 


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

# Include the flags of the stack so that sap matches the stack configuration
#include ${SOFT_WORKDIR}/platform/stack/stack.mk

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
