## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/csw/base

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
	platform \
	platform/chip/hal \
	platform/base/common \
	platform/chip/defs \
	platform/stack \
	platform/csw \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \
	platform/csw/base/cos \
	platform/csw/base/ffs \
	platform/csw/base/sul \
	platform/csw/base/tm \
	platform/csw/base/ml \
	platform/csw/base/ver \
	platform/csw/base/vds \

#	platform/csw/base/reg \
	platform/csw/base/bal \
	platform/csw/base/db \
	platform/csw/base/nvram \
	

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG += 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
