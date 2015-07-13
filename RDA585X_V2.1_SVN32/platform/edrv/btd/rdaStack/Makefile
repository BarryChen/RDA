## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/edrv/btd/rdaStack

# Compile the sub-modules, except when the "base" must be used as a library.

# list all modules APIs that are neccessary to compile this module
# some extra dependencies are added for Jade
LOCAL_API_DEPENDS := \
				target \
				platform \
				platform/base/sx \
				platform/csw \
				platform/chip/hal \
				platform/chip/defs \
				platform/edrv/btd/rdadriver \
				platform/edrv/btd/rdaStack/stack \
				platform/edrv \
				platform/edrv/pmd \
                    		platform/edrv/i2cgpio \
			    apps/gui \
			    apps/system \

# List of submodules which contain code we need to include in the final lib
LOCAL_MODULE_DEPENDS := \


## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
include src/make.srcs

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
