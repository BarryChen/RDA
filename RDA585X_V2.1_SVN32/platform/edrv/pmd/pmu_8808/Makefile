## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/pmd/pmu_8808

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS ?= platform \
                     platform/calib \
                     platform/base/common \
                     platform/base/sx \
                     platform/chip/defs \
                     platform/chip/hal \
                     platform/csw \
                     platform/edrv \
                     platform/edrv/dualsimd \
                     platform/edrv/pmd \
                     platform/edrv/rfd \
                     target \

LOCAL_ADD_INCLUDE := platform/edrv/pmd/src


# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS ?= 

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
# Generate the CoolProfile header automatically during the make process.
AUTO_XCP2H ?= yes

## ------------------------------------- ##
##      List all your sources here       ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk


