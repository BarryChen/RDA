## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/mmi_compilevars.mk

## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := platform/mdi

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"




ifeq "${CSW_EXTENDED_API}" "1"
# In case of CSW_EXTENDED_API, don't use platform/mdi sources
LOCAL_MODULE_DEPENDS := 

else
# Compile the sub-modules, except when the "platform/mdi" must be used as a library.
ifeq ($(findstring platform/mdi/packed,${GLOBAL_BINARY_LIBS}),)

LOCAL_MODULE_DEPENDS := \
						platform/mdi/alg \
						platform/mdi/mmc \
						platform/mdi/ars

else # GLOBAL_BINARY_LIBS

LOCAL_BINARY_LIBS    += platform/mdi/packed/mdi_${CT_ASIC}
LOCAL_MODULE_DEPENDS := platform/mdi/packed/mdi_${CT_ASIC}

endif # GLOBAL_BINARY_LIBS
endif # CSW_EXTENDED_API

ifeq "${TARGET_IMAGE_DISPLAY_SUPPORT}" "1" 
LOCAL_MODULE_DEPENDS +=	platform/mdi/image 
endif

# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
						platform \
						platform/mdi \
						platform/chip/hal \
						
# Set this to a non-null string to signal a toplevel module, like 
# phy but not like phy/kitchensink. This defines the behavior of 
# make deliv
IS_TOP_LEVEL := yes


# Generates the CoolWatcher headers automatically.
AUTO_XMD2H ?= yes


# Video player (enabled by default)
# VIDEO_PLAYER_SUPPORT ?= 1
# Valid for platform/mdi/alg and platform/mdi/mmc
export VIDEO_PLAYER_SUPPORT


# This can be used to define some preprocessor variables to be used in 
# the current module, but also exported to all dependencies.
# This is especially useful in an ENTRY_POINT modules
# Ex. : LOCAL_EXPORT_FLAGS += OS_USED DEBUG will result in 
# -DOS_USED -DDEBUG being passed on each subsequent compile command.
LOCAL_EXPORT_FLAG += 



## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
# Assembly / C code
S_SRC :=
C_SRC := mdi_map.c 

# Generate the revision (version) file automatically during the make process.
AUTO_GEN_REVISION_HEADER := yes

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk




