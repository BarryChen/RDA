## --------------Don't change the line below------------------ ##
# For more information on the different variables, and others you
# may define, see ${SOFT_DIR}/Makefile.skel and/or read the doc
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

LOCAL_NAME := platform/edrv/mcd/mcd_usb

LOCAL_API_DEPENDS := \
                     platform \
                     platform/chip/hal \
                     platform/chip/defs \
                     platform/edrv/mcd \
                     platform/edrv \
                     target \
                     platform/base/sx \
                     platform/csw


C_SRC := mcd_usb.c \
				 usb_hcd.c
S_SRC :=


## --------------Don't change the line below------------------ ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
