## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/csw/dsm


# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := platform \
                     platform/mdi \
                     platform/base/sx \
                     platform/chip/defs \
                     platform/chip/hal \
                     platform/csw \
				   					 platform/csw/dsm \
										 platform/csw/base \
                     platform/edrv/lcdd \
                     platform/svc/cams \
                     platform/svc/imgs \
                     platform/svc/mps \
                     platform/svc/mrs \
                     platform/svc/uctls \
                     platform/svc/umss \
				  					 target \



# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS :=  

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

LOCAL_EXPORT_FLAG +=

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
