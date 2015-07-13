## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/csw/base/bal

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform/csw/tcpip\
			platform \
			platform/base/common \
			platform/base/sx \
			platform/chip/hal \
			platform/chip/boot \
			platform/chip/defs \
			platform/chip/regs/$(CT_ASIC) \
			platform/csw \
			platform/csw/base \
			platform/csw/dm \
			platform/edrv/aud \
			platform/edrv/pmd \
			platform/stack \
			platform/svc/fstraces \
			platform/svc/ars \
			platform/svc/vois \
			target
			
ifeq "${WIFI_MODEL}" "rdawifi"
LOCAL_API_DEPENDS += platform/edrv/wifi/rdawifi
endif
# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \

# This is a top-level module
IS_TOP_LEVEL := no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
C_SRC := ${notdir ${wildcard src/*.c}}


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
