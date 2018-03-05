
include $(MCFW_ROOT_PATH)/makerules/common_header_a8.mk

LIBS=$(LIB_DIR)/fw_load.a $(SYSLINK_LIBS)

INCLUDE+= $(OSA_INC) $(SYSLINK_INC)

include $(MCFW_ROOT_PATH)/makerules/common_footer_a8.mk


