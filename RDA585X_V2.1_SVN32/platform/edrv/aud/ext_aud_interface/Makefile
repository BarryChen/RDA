## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/aud/ext_aud_interface

LOCAL_API_DEPENDS := \
					target \
					platform \
					platform/base/common \
					platform/base/sx \
					platform/calib \
					platform/chip/hal \
					platform/chip/defs \
					platform/edrv \
					platform/edrv/aud \
					platform/edrv/pmd \

## Include source files
include src/make.srcs

# C_SRC := ${notdir ${wildcard *.c}}
# S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

