
include $(MCFW_ROOT_PATH)/makerules/common_header_a8.mk

INCLUDE += $(MCFW_INC) $(OSA_INC) $(STREAM_INC) $(IPNC_APP_INC) $(XDAIS_INC) $(SYSLINK_INC) $(CODEC_INC) $(CMEM_INC) $(DEVICES_INC)

LIBS= $(LIB_DIR)/ipnc_rdk_mcfw.a $(MCFW_LIBS) $(AUDIO_LIBS) $(SHMEM_LIB) $(CMEM_LIB)

include $(MCFW_ROOT_PATH)/makerules/common_footer_a8.mk


