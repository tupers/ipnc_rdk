
include $(MCFW_ROOT_PATH)/makerules/common_header_a8.mk

IPC_INC=-I$(ipc_PATH)/packages $(CMEM_INC)

INCLUDE+= $(MCFW_INC) $(STREAM_INC) $(IPNC_APP_INC) $(OSA_INC) $(IPC_INC) $(XDAIS_INC) $(USECASE_INC) $(CMEM_LIB)

include $(MCFW_ROOT_PATH)/makerules/common_footer_a8.mk


