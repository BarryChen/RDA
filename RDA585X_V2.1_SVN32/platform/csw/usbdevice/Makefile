## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/csw/usbdevice

LOCAL_MODULE_DEPENDS :=     \
        platform/csw/usbdevice/storage   \
        platform/csw/usbdevice/audio   \
        platform/csw/usbdevice/hid   \

         
LOCAL_API_DEPENDS := \
		platform \
         platform/csw                  \
         platform/csw/usbdevice                  \
         platform/csw/usbdevice/storage             \
         platform/csw/usbdevice/audio             \
         platform/chip/hal                  \
         platform/chip/defs                 \
         platform/base/sx               \
		platform/phy \
		platform/chip/defs \
		platform/base/sx		\
    platform/edrv/aud

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG +=

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
S_SRC := 
#setjmp.S
C_SRC := ${notdir ${wildcard src/*.c}}

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
