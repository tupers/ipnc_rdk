# File: env.mk. This file contains all the paths and other ENV variables

#
# Module paths
#

# Destination root directory.
#   - specify the directory where you want to place the object, archive/library,
#     binary and other generated files in a different location than source tree
#   - or leave it blank to place then in the same tree as the source
DEST_ROOT = $(MCFW_ROOT_PATH)/build

# Utilities directory. This is required only if the build machine is Windows.
#   - specify the installation directory of utility which supports POSIX commands
#     (eg: Cygwin installation or MSYS installation).
UTILS_INSTALL_DIR := $(xdc_PATH)

# Set path separator, etc based on the OS
ifeq ($(OS),Windows_NT)
  PATH_SEPARATOR = ;
  UTILSPATH = $(UTILS_INSTALL_DIR)/bin/
else
  # else, assume it is linux
  PATH_SEPARATOR = :
  UTILSPATH = /bin/
endif

# BIOS
bios_INCLUDE = $(bios_PATH)/packages

# IPC
ipc_INCLUDE = $(ipc_PATH)/packages

# SYSLINK
syslink_INCLUDE = $(syslink_PATH)/packages

# XDC
xdc_INCLUDE = $(xdc_PATH)/packages

# ISS drivers
iss_INCLUDE = $(iss_PATH)/packages
include $(iss_PATH)/component.mk
export iss_PATH

# HDVPSS drivers
hdvpss_INCLUDE = $(hdvpss_PATH)/packages
include $(hdvpss_PATH)/component.mk
export hdvpss_PATH

ipnc_rdk_INCLUDE = $(MCFW_ROOT_PATH)
include $(MCFW_ROOT_PATH)/component.mk
export MCFW_ROOT_PATH

#FC include
fc_INCLUDE = $(fc_PATH)/packages

#XDAIS include
xdais_INCLUDE = $(xdais_PATH)/packages

#H264ENC include
h264enc_INCLUDE = $(h264enc_PATH)/Inc

#H264DEC include
h264dec_INCLUDE = $(h264dec_PATH)/Inc

#MPEG4ENC include
mpeg4enc_INCLUDE = $(mpeg4enc_PATH)/Inc

#JPEGENC include
jpegenc_INCLUDE = $(jpegenc_PATH)/Inc

#HDVICP API
hdvicpapi_INCLUDE = $(hdvicplib_PATH)/packages
#
# Tools paths
#

# EDMA3LLD 
edma3lld_INCLUDE = $(edma3lld_PATH)/packages


# Commands commonly used within the make files
RM = $(UTILSPATH)rm
RMDIR = $(UTILSPATH)rm -rf
MKDIR = $(UTILSPATH)mkdir
ECHO = @$(UTILSPATH)echo

#emake provides detailed info for build analysis.
EMAKE = emake --emake-emulation=gmake --emake-gen-subbuild-db=1 --emake-annodetail=waiting

ifeq ($(OS),Windows_NT)
MAKE = $(UTILSPATH)make
#  MAKE =  $(EMAKE)
else
  MAKE = make
endif
EGREP = $(UTILSPATH)egrep
CP = $(UTILSPATH)cp
ifeq ($(OS),Windows_NT)
  CHMOD = $(UTILSPATH)echo
else
  CHMOD = $(UTILSPATH)chmod
endif

#
# XDC specific ENV variables
#
# XDC Config.bld file (required for configuro); Derives from top-level hdvpss_PATH
ifeq ($(PLATFORM),ti814x-evm)
ifeq ($(MEMORY_256MB),YES)
  CONFIG_BLD_XDC_m3     = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_256M.bld
  CONFIG_BLD_XDC_c674   = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_256M.bld
else  
  CONFIG_BLD_XDC_m3     = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_512M.bld
  CONFIG_BLD_XDC_c674   = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_512M.bld
endif
ifeq ($(IPNC_TILER_ENABLE),YES)
  CONFIG_BLD_XDC_m3     = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_512M_tiler.bld
  CONFIG_BLD_XDC_c674   = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_512M_tiler.bld
endif
endif
ifeq ($(PLATFORM),ti814x-sim)
  CONFIG_BLD_XDC_m3     = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_512M.bld
  CONFIG_BLD_XDC_c674   = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti814x/config_512M.bld
endif
ifeq ($(PLATFORM),ti810x-evm)
ifeq ($(MEMORY_256MB),YES)
  CONFIG_BLD_XDC_m3     = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti810x/config_256M.bld
else
  CONFIG_BLD_XDC_m3     = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti810x/config_512M.bld
endif
ifeq ($(IPNC_TILER_ENABLE),YES)
  CONFIG_BLD_XDC_m3     = $(MCFW_ROOT_PATH)/mcfw/src_bios6/cfg/ti810x/config_512M_tiler.bld
endif
endif

XDCROOT = $(xdc_PATH)
XDCTOOLS = $(xdc_PATH)
export XDCROOT
export XDCTOOLS

CGTOOLS = $(CODEGEN_PATH_M3)
export CGTOOLS

CGTOOLS_DSP = $(CODEGEN_PATH_DSP)
export CGTOOLS_DSP

CODESOURCERYCGTOOLS = $(CODEGEN_PATH_A8)
export CODESOURCERYCGTOOLS

STRIP470 = $(CODEGEN_PATH_M3)/bin/armstrip -p
STRIP6x = $(CODEGEN_PATH_DSP)/bin/strip6x -p

PATH += $(PATH_SEPARATOR)$(xdc_PATH)$(PATH_SEPARATOR)$(CODEGEN_PATH_M3)/bin $(PATH_SEPARATOR)$(CODEGEN_PATH_DSP)/bin
PATH += $(PATH_SEPARATOR)$(UBOOTDIR)/tools
export PATH

# Nothing beyond this point
