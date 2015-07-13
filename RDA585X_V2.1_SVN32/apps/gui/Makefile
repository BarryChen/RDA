## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := apps/gui

# Compile the sub-modules, except when the "base" must be used as a library.
# list all modules APIs that are neccessary to compile this module
# some extra dependencies are added for Jade
LOCAL_API_DEPENDS := \
				platform \
				platform/csw \
				platform/edrv/lcdd \
			    apps/gui \
			    apps/system \
			    

# List of submodules which contain code we need to include in the final lib
LOCAL_MODULE_DEPENDS := \
			    apps/gui/gui_lcd \
			    apps/gui/gui_rgblcd \
			    apps/gui/gui_led8s \

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
C_SRC := ${notdir ${wildcard src/*.c}} # uncomment to take all .c files

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk
