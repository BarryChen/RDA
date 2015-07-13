# MIPS Makefile
# (c) Copyright RDA Inc. 2013


# Unix Style

ifeq "${CT_RELEASE}" "debug"
DEBUG_FLAGS ?= -DDEBUG
endif
DEFINES    = -D_REENTRANT -DREMAPPING
ARCH_FLAGS = $(DEBUG_FLAGS) -G0 -Wall -march=xcpu -mtune=xcpu -Wa,-march=xcpu,-mtune=xcpu -EL -mexplicit-relocs -fweb -frename-registers -mmemcpy -mmips-tfile -nostartfiles -nostdlib -msoft-float -nodefaultlibs -c -pipe 
ARCH_FLAGS +=  -ffixed-t3 -ffixed-t4 -ffixed-t5 -ffixed-t6 -ffixed-t7 -ffixed-s2 -ffixed-s3 -ffixed-s4 -ffixed-s5 -ffixed-s6 -ffixed-s7 -ffixed-fp -minterlink-mips16 -mips16 -ffunction-sections -fdata-sections
CFLAGS     =  -Os -g -fwide-exec-charset=UTF-16LE -fshort-wchar $(ARCH_FLAGS) $(INCLUDE) $(DEFINES) 
LDFLAGS    = 

CC         = mips-elf-gcc 
AR         = mips-elf-ar
RANLIB     = 

TOP_DIR = ..
TARGET_DIR   = $(TOP_DIR)/arch/mips-mp3/stack
OUT_DIR      = $(TARGET_DIR)/objects
TARGET_RULES = $(TARGET_DIR)/Make.rules

INCLUDE = -Iinclude/ \
					-I$(TOP_DIR)/arch/common/include \
					-I$(TOP_DIR)/include \
					-I$(TOP_DIR)/hci/include \
					-I$(TOP_DIR)/l2cap/include \
					-I$(TOP_DIR)/manager/include \
					-I$(TOP_DIR)/sdp/include \
					-I$(TOP_DIR)/rfcomm/include \
					-I$(TARGET_DIR)/../include

layer_dirs = hci \
							generic \
							manager \
							sdp \
							rfcomm \
							l2cap \
							hfp \
							a2dp  \
							spp \
							obex \
							hid \
							

export OUT_DIR TARGET_RULES CC CFLAGS CPPFLAGS TOP_DIR TARGET_DIR AR RANLIB

all: subdirs library

subdirs:
	mkdir -p objects
	set -e ; for i in $(layer_dirs) ; do $(MAKE) -C ../../../$$i ; done

library: 
	$(AR) cru ../librdabthost_${CT_RELEASE}.a ../../$(OUT_DIR)/*.o
	cp -rf ../librdabthost_${CT_RELEASE}.a ../../../../../platform/edrv/btd/rdaStack
clean:
	rm -rf ../../$(OUT_DIR)/*.o
