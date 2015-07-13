## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/csw

# list all modules APIs that are necessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					platform/chip/hal \
					platform/base/common \
					platform/chip/defs \
					platform/stack \


# Compile the sub-modules, except when the "csw" must be used as a library.
ifeq ($(findstring platform/csw/packed,${GLOBAL_BINARY_LIBS}),)

# list all the modules that need to be compiled prior to using this module

LOCAL_MODULE_DEPENDS := \
						platform/csw/driver \
						platform/csw/pm \
						platform/csw/dm \
						platform/csw/dsm \
						platform/csw/usbdevice \
						platform/csw/tools \

#						platform/csw/base \


ifneq "${CT_USER}" "CUSTOM"
LOCAL_MODULE_DEPENDS += 			platform/csw/base 
endif
						
ifeq "${WIFI_MODEL}" "rdawifi"
LOCAL_MODULE_DEPENDS += platform/csw/wifi
endif
# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

C_SRC += csw_map.c

else # GLOBAL_BINARY_LIBS

ifeq "${NUMBER_OF_SIM}" "2"

ifeq "${GPRS_SUPPORT}" "1"
LOCAL_BINARY_LIBS    += platform/csw/packed/csw_${CT_ASIC}/csw_dualsim_gprs
LOCAL_MODULE_DEPENDS := platform/csw/packed/csw_${CT_ASIC}/csw_dualsim_gprs
else # !GPRS_SUPPORT
LOCAL_BINARY_LIBS    += platform/csw/packed/csw_${CT_ASIC}/csw_dualsim
LOCAL_MODULE_DEPENDS := platform/csw/packed/csw_${CT_ASIC}/csw_dualsim
endif # GPRS_SUPPORT

else # DUAL_SIM_SUPPORT

ifeq "${GPRS_SUPPORT}" "1"
LOCAL_BINARY_LIBS    += platform/csw/packed/csw_${CT_ASIC}/csw_singlesim_gprs
LOCAL_MODULE_DEPENDS := platform/csw/packed/csw_${CT_ASIC}/csw_singlesim_gprs
else # !GPRS_SUPPORT
LOCAL_BINARY_LIBS    += platform/csw/packed/csw_${CT_ASIC}/csw_singlesim
LOCAL_MODULE_DEPENDS := platform/csw/packed/csw_${CT_ASIC}/csw_singlesim
endif # GPRS_SUPPORT

endif # DUAL_SIM_SUPPORT

endif # GLOBAL_BINARY_LIBS

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
LOCAL_EXPORT_FLAG += \
	HAVE_CFW_CONFIG \
	MF32_BASE_REG_DISABLE \
	MF32_NO_RFM_RH \
	MF32_NO_RFM \
	MF32_CFW_MMI_TWO_TASKS \
	CPP_KEYWORD_SUPPORT
	
ifneq "${AMR_SUPPORT}" "NO"	
LOCAL_EXPORT_FLAG += CFW_AMR_SUPPORT
endif

	
ifeq "${DUAL_SIM_SUPPORT}" "1"	
LOCAL_EXPORT_FLAG += CFW_DUAL_SIM STK_DUAL_SIM
endif

ifeq "${GPRS_SUPPORT}" "1"	
LOCAL_EXPORT_FLAG += CFW_GPRS_SUPPORT CFW_TCPIP_SUPPORT 
endif

# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes

ifeq "${CT_ASIC}" "jade"
export WITHOUT_WERROR := 1
endif


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
