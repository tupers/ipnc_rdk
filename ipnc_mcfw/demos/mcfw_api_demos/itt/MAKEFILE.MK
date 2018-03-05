
include $(MCFW_ROOT_PATH)/makerules/common_header_a8.mk

IPC_INC=-I$(ipc_PATH)/packages
SYSLINK_INC=-I$(syslink_PATH)/packages

INCLUDE+= $(MCFW_INC) $(IPNC_APP_INC) $(OSA_INC) $(SYSLINK_INC) $(IPC_INC) $(XDAIS_INC)

include $(MCFW_ROOT_PATH)/makerules/common_footer_a8.mk


