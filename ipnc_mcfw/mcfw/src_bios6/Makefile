
MODULE_NAME = ipnc_rdk_bios6


#INCDIR = .
INCDIR = .  $(jpegenc_PATH)/inc
INCDIR += .  $(jpegdec_PATH)/inc
INCDIR += .  $(mpeg4dec_PATH)/Inc
INCDIR += .  $(mctnf_PATH)/Inc
INCDIR += $(iss_PATH)/packages/ti/psp/iss/alg/evf/inc 
INCDIR += $(fc_PATH)
INCDIR +=  $(TOOLS_INSTALL_DIR)/imglib/
INCDIR +=  $(TOOLS_INSTALL_DIR)/mathlib/packages/
INCDIR += $(TOOLS_INSTALL_DIR)/vlib/packages/
INCDIR += $(TOOLS_INSTALL_DIR)/dsplib/packages
INCDIR += $(MODULE_SRC_BASE_PATH)/links_c6xdsp/imgproc/SDS/include/

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc ipc syslink iss hdvpss ipnc_rdk fc xdais h264enc h264dec swosd edma3lld mpeg4enc

MODULE_SRC_BASE_PATH := $(MCFW_ROOT_PATH)/mcfw/src_bios6

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
SRCS_ti816x =
SRCS_ti814x =
CFLAGS_LOCAL_COMMON = $(IPNC_RDK_CFLAGS)
CFLAGS_LOCAL_COMMON +=-D_4MACS  -D_DUAL_CORE  -D_BIOS -D_DEBUG   -D_SIMCOP 

ifneq ($(CORE),c6xdsp)
SRCDIR = utils/src links_m3video links_m3vpss
SRCDIR += ../demos/chains/common ../demos/chains/vpss_chains ../demos/chains/vpss_video_chains
include $(MODULE_SRC_BASE_PATH)/links_common/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/links_m3vpss/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/links_m3video/SRC_FILES.MK
else

ifneq ($(PLATFORM), ti810x-evm)
CFLAGS_LOCAL_COMMON +=-D_C66X_
CFLAGS_LOCAL_COMMON +=-D_C674DSP_
SRCDIR = utils/src links_c6xdsp
SRCDIR += ../demos/chains/common 
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/swosd
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/scd
INCDIR +=  $(TOOLS_INSTALL_DIR)/imglib/
INCDIR +=  $(TOOLS_INSTALL_DIR)/mathlib/packages/
INCDIR += $(TOOLS_INSTALL_DIR)/vlib/packages/
INCDIR += $(TOOLS_INSTALL_DIR)/dsplib/packages
INCDIR += $(MODULE_SRC_BASE_PATH)/links_c6xdsp/imgproc/SDS/include/
include $(MODULE_SRC_BASE_PATH)/links_common/SRC_FILES_DSP.MK
include $(MODULE_SRC_BASE_PATH)/links_c6xdsp/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/swosd/SRC_FILES.MK
endif
endif

include $(MODULE_SRC_BASE_PATH)/utils/SRC_FILES.MK

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCFW_ROOT_PATH)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
