# File: component.mk
#       This file is component include make file of IPNC_RDK.

# List of variables set in this file and their purpose:
# <mod>_RELPATH        - This is the relative path of the module, typically from
#                        top-level directory of the package
# <mod>_PATH           - This is the absolute path of the module. It derives from
#                        absolute path of the top-level directory (set in env.mk)
#                        and relative path set above
# <mod>_INCLUDE        - This is the path that has interface header files of the
#                        module. This can be multiple directories (space separated)
# <mod>_PKG_LIST       - Names of the modules (and sub-modules) that are a part
#                        part of this module, including itself.
# <mod>_PLATFORM_DEPENDENCY - "yes": means the code for this module depends on
#                             platform and the compiled obj/lib has to be kept
#                             under <platform> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no platform dependent code and hence
#                             the obj/libs are not kept under <platform> dir.
# <mod>_CORE_DEPENDENCY     - "yes": means the code for this module depends on
#                             core and the compiled obj/lib has to be kept
#                             under <core> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no core dependent code and hence
#                             the obj/libs are not kept under <core> dir.
# <mod>_APP_STAGE_FILES     - List of source files that belongs to the module
#                             <mod>, but that needs to be compiled at application
#                             build stage (in the context of the app). This is
#                             primarily for link time configurations or if the
#                             source file is dependent on options/defines that are
#                             application dependent. This can be left blank or
#                             not defined at all, in which case, it means there
#                             no source files in the module <mod> that are required
#                             to be compiled in the application build stage.
#

ipnc_rdk_PKG_LIST = ipnc_rdk_bios6

#
# IPNC_RDK Modules
#

# VSI SDK BIOS6 libraries
ipnc_rdk_bios6_RELPATH = mcfw/src_bios6
ipnc_rdk_bios6_PATH = $(MCFW_ROOT_PATH)/$(ipnc_rdk_bios6_RELPATH)
ipnc_rdk_bios6_INCLUDE = $(MCFW_ROOT_PATH)
ipnc_rdk_bios6_PKG_LIST = ipnc_rdk_bios6
ipnc_rdk_bios6_PLATFORM_DEPENDENCY = yes
ipnc_rdk_bios6_CORE_DEPENDENCY = yes

# Component specific CFLAGS
CFLAGS_COMP_COMMON +=

# CFLAGS based on platform selected
ifeq ($(PLATFORM),ti814x-evm)
    IPNC_RDK_CFLAGS = -DTI_814X_BUILD -DPLATFORM_EVM_SI
endif    
ifeq ($(PLATFORM),ti810x-evm)
    IPNC_RDK_CFLAGS = -DTI_8107_BUILD -DPLATFORM_EVM_SI -DIPNC_DM385
endif    


IPNC_RDK_CFLAGS += -DUTILS_ASSERT_ENABLE
ifeq ($(SYSTEM_CFG),ipnc)
IPNC_RDK_CFLAGS += -D_IPNC_HW_PLATFORM_
endif
ifeq ($(SYSTEM_CFG),evm)
IPNC_RDK_CFLAGS += -D_IPNC_HW_PLATFORM_EVM_
endif
#
#  IPNC_RDK_BOARD_TYPE definition
#
ifeq ($(DVR_RDK_BOARD_TYPE),DM814X_IPNC)
    IPNC_RDK_CFLAGS += -DTI814X_EVM 
endif	
ifeq ($(DVR_RDK_BOARD_TYPE),DM8107_IPNC)
    IPNC_RDK_CFLAGS += -DTI8107_EVM 
endif

ifneq ($(POWER_OPT_DSS_OFF), YES)
IPNC_RDK_CFLAGS += -DIPNC_DSS_ON
endif
ifneq ($(POWER_OPT_DSP_OFF), YES)
IPNC_RDK_CFLAGS += -DIPNC_DSP_ON
endif

ifeq ($(CAPTURE_DISPLAY_MODE_ON), YES)
IPNC_RDK_CFLAGS += -DCAPTURE_DISPLAY_MODE
endif

ifeq ($(CAPTURE_MCTNF_DISP_MODE_ON), YES)
IPNC_RDK_CFLAGS += -DCAPTURE_MCTNF_DISP_MODE
endif

ifeq ($(FRAMES_TO_A8), YUV)
IPNC_RDK_CFLAGS += -DYUV_FRAMES_TO_A8
endif
ifeq ($(FRAMES_TO_A8), RAW)
IPNC_RDK_CFLAGS += -DRAW_FRAMES_TO_A8
endif

ifeq ($(CAPTURE_MODE), MIPI_CSI2)
IPNC_RDK_CFLAGS += -DUSE_MIPI_MODE
endif

#ifeq ($(OSD_MODE), IMX_BASED)
IPNC_RDK_CFLAGS += -DIMX_BASED_SWOSD_ENABLED
#endif

ifeq ($(USE_A8_FOR_TC_CALC), YES)
IPNC_RDK_CFLAGS += -DUSE_A8_FOR_TC_CALC
endif

ifeq ($(USE_ISS_BOXCAR), YES)
IPNC_RDK_CFLAGS += -DUSE_ISS_BOXCAR
endif

ifeq ($(WDR_ON), YES)
IPNC_RDK_CFLAGS += -DWDR_ON
endif

ifeq ($(IPNC_DEVICE), DM388)
ifneq ($(QUAD_STREAM_NF_USE_CASE), YES)
IPNC_RDK_CFLAGS += -DUSE_MCTNF
#IPNC_RDK_CFLAGS += -DVNF_BEFORE_MCTNF
endif
endif

ifeq ($(IPNC_DEVICE), DMVA3)
ifeq ($(DMVA3_VNF_MODE), MCTNF)
IPNC_RDK_CFLAGS += -DUSE_MCTNF
#IPNC_RDK_CFLAGS += -DVNF_BEFORE_MCTNF
endif
endif

IPNC_RDK_CFLAGS += -DUSE_TNF3_FILTER

ifneq ($(IPNC_DEVICE), DM8127)
IPNC_RDK_CFLAGS += -DIPNC_DM385
endif

ifeq ($(IPNC_TILER_ENABLE), YES)
IPNC_RDK_CFLAGS += -DENABLE_TILER
endif

ifeq ($(CAMERA_TILER_ENABLE), YES)
IPNC_RDK_CFLAGS += -DENABLE_TILER_CAMERA
endif

#ifeq ($(SYSTEM_SIMCOP_ENABLE), YES)
#IPNC_RDK_CFLAGS += -DSYSTEM_SIMCOP_ENABLE
#endif

ifeq ($(MEMORY_256MB), YES)
IPNC_RDK_CFLAGS += -DMEMORY_256MB
endif

ifeq ($(TEARDOWN_LOAD_UNLOAD), YES)
IPNC_RDK_CFLAGS += -DTEARDOWN_LOAD_UNLOAD
endif

ifeq ($(ENC_A8_DEC_USE_CASE), YES)
IPNC_RDK_CFLAGS += -DENC_A8_DEC_USE_CASE
endif

ifeq ($(QUAD_STREAM_NF_USE_CASE), YES)
IPNC_RDK_CFLAGS += -DQUAD_STREAM_NF_USE_CASE
endif

ifeq ($(RVM_DEMO_MODE), YES)
IPNC_RDK_CFLAGS += -DRVM_DEMO_MODE
endif

ifeq ($(SENSOR_BOARD_VENDOR), LIMG)
IPNC_RDK_CFLAGS += -DLIMG_SENSOR_VENDOR
else
IPNC_RDK_CFLAGS += -DAPPRO_SENSOR_VENDOR
endif

IPNC_RDK_CFLAGS += -D$(IMGS_ID)
IPNC_RDK_CFLAGS += -D$(SENSOR_DATA_OP)

IPNC_RDK_CFLAGS += -D$(MJPEG_RC_MODE)

export IPNC_RDK_LIBS
export IPNC_RDK_EXAMPLE_LIBS
export IPNC_RDK_CFLAGS

# Nothing beyond this point
