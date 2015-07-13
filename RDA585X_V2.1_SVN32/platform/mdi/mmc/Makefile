## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := platform/mdi/mmc

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"
LOCAL_MODULE_DEPENDS :=

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
					platform/mdi \
					platform/mdi/alg/common \
					platform/mdi/alg/isp \
					platform/mdi/alg/wma_decoder \
					platform/mdi/alg/amrring_codec_voc \
					platform/mdi/alg/audiojpeg_dec_voc \
					platform/mdi/alg/audiojpegenc \
					platform/mdi/alg/preview\
					platform/mdi/alg/avi_dec \
					platform/mdi/alg/avi_enc \
					platform/mdi/alg/h263_dec \
					platform/mdi/alg/h264_dec \
					platform/mdi/alg/mpeg4_dec \
					platform/mdi/alg/rm_dec \
					platform/mdi/alg/ra8lbr_dec \
					platform/mdi/alg/aac_dec_c \
					platform/mdi/alg/mp3mad_dec_c \
					platform/mdi/alg/amr_dec_c \
					platform/mdi/alg/isomedia_demux \
						platform/mdi/alg/sbc_dec		\
						platform/mdi/alg/sbc_dec_voc \
					platform/mdi/alg/sco_vpp\
					platform/mdi/alg/analogtv \
					platform/mdi/alg/pngdec \
					platform/mdi/alg/aec		\
					platform/mdi/ars      \
					platform/csw \
					platform/edrv/lcdd \
					platform/chip/defs \
					platform/ \
					application/coolmmi/debug \
					application/systeminc/mmi \
					application/coolmmi/interface/config \
					application/coolmmi/mmi/AsyncEvents \
					application/coolmmi/media/common \
					application/coolmmi \
					application/coolmmi/adaptation \
					application/adaptation/custom/audio/PSI \
					application/adaptation/device \
					application/systeminc/interface/config \
					application/systeminc/interface/hwdrv \
					application/systeminc/interface/l1audio \
					application/systeminc/interface/l1interface \
					application/systeminc/interface/media \
					application/systeminc/interface/mmi \
					application/systeminc/interface/os \
					application/systeminc/interface/ps \
					application/coolmmi/mmi \
					application/coolmmi/mmi/Audio \
					application/coolmmi/mmi/DateTime \
					application/coolmmi/mmi/DateTime \
					application/coolmmi/mmi/FileSystem \
					application/coolmmi/mmi/Framework \
					application/coolmmi/mmi/GUI \
					application/coolmmi/mmi/IdleScreen \
					application/coolmmi/mmi/MiscFramework \
					application/coolmmi/mmi/PROFILES \
					application/coolmmi/mmi/gpio \
					application/coolmmi/mmi_csdapp/EngineerMode \
					application/coolmmi/mmi_csdapp/GDI \
					application/coolmmi/mmi_csdapp/MDI \
					application/coolmmi/mmi_csdapp/mmiapi \
					application/systeminc/ps/interfaces \
					application/systeminc/ps/l4 \
					application/coolmmi/mmi/di \
					platform/base/std \
					platform/base/common \
					platform/base/sx \
					platform/calib \
					platform/chip/regs/$(CT_ASIC)\
					platform/chip/defs \
					platform/chip/hal \
					platform/edrv/aud \
					platform/edrv/aud/fm \
					platform/edrv/fmd \
					target \
					target/${CT_TARGET} \
					platform/edrv/camdold \
					platform/edrv/atvd \
					platform/edrv/pmd \
					platform/edrv/cammd \
					platform \
					platform/svc/uctls \
					platform/svc/umss/storage/ram      \
					platform/svc/umss      \
					platform/svc/umss/transport/boscsi \
					platform/svc/utraces	\
					platform/csw \
					platform/svc/ars         \
					platform/vpp/speech \
					platform/mdi/alg/noise_suppress \
					application/coolmmi/mmi/DI \
					application/coolmmi/custom/drv \
					application/coolmmi/interface/hwdrv \
					application/coolmmi/mmi_csdapp/Camera \
					application/coolmmi/mmi_csdapp/filemgr \
					application/coolmmi/mmi_csdapp/fmt \
					application/coolmmi/mmi_csdapp/FileMgr \
					application/coolmmi/mmi/Resource \
					application/coolmmi/mmi_customer \
					application/coolmmi/mmi_csdapp/EbookReader \
					application/coolmmi/media/image \
					application/coolmmi/custom/app \
					application/coolmmi/custom/common \
					application/coolmmi/sst \
					application/coolmmi/interface/adaptation \
					application/coolmmi/mmi_csdapp/Video \
					application/media/common \
					application/media/image \
					application/adaptation/DI \

# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv
IS_TOP_LEVEL := no


# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.

# Video player
ifeq "${VIDEO_PLAYER_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += VIDEO_PLAYER_SUPPORT
endif

# Analog TV
ifeq "${ANALOG_TV_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += ANALOG_TV_SUPPORT
endif

# Sound recorder
ifeq "${SOUND_RECORDER_SUPPORT}" "1"
LOCAL_EXPORT_FLAG  += SOUND_RECORDER_SUPPORT
endif

# Video recorder
ifeq "${VIDEO_RECORDER_SUPPORT}" "1"
LOCAL_EXPORT_FLAG  += VIDEO_RECORDER_SUPPORT
endif

# Camera motion detection
ifeq "${CAM_MD_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += CAM_MD_SUPPORT
endif

# MIDI format
ifeq "${MEDIA_MIDI_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += MEDIA_MIDI_SUPPORT
endif

# RM format
ifeq "${MEDIA_RM_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += MEDIA_RM_SUPPORT
endif

# H264 format
ifeq "${MEDIA_H264_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += MEDIA_H264_SUPPORT
endif

# DRA format
ifeq "${MEDIA_DRA_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += MEDIA_DRA_SUPPORT
endif

# voc vid dec
ifeq "${MEDIA_VOCVID_SUPPORT}" "1"
LOCAL_EXPORT_FLAG += MEDIA_VOCVID_SUPPORT
endif

# FM line-in record
ifeq "${FM_LINEIN_RECORD}" "1"
LOCAL_EXPORT_FLAG += FM_LINEIN_RECORD
endif

LOCAL_EXPORT_FLAG += 

#CFLAGS += -isystem 

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##

include src/make.srcs


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
