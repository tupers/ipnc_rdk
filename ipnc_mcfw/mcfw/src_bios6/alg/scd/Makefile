
MODULE_NAME = scd

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc ipc syslink hdvpss dvr_rdk fc xdais h264enc h264dec swosd edma3lld
#xdc fc xdais edma3lld sysbios ipc bios xdc syslink hdvpss dvr_rdk

MODULE_SRC_BASE_PATH := $(dvr_rdk_PATH)/mcfw/src_bios6/alg/scd

INCDIR = $(dvr_rdk_PATH)
#$(MUDULE_SRC_BASE_PATH)/../../links_c6xdsp/utils/
INCDIR += . 

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
SRCS_ti816x =
SRCS_ti814x =
CFLAGS_LOCAL_COMMON = $(DVR_RDK_CFLAGS)

SRCDIR = src

include $(MODULE_SRC_BASE_PATH)/SRC_FILES.MK


# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCFW_ROOT_PATH)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
