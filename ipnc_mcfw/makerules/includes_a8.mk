# (c) Texas Instruments

ifndef $(INCLUDES_MK)
INCLUDES_MK = 1

ISS_INC=-I$(iss_PATH)

ALG_INC=-I$(MCFW_ROOT_PATH)/mcfw/src_linux/links/alg/

GLBCE_INC=-I$(MCFW_ROOT_PATH)/mcfw/src_linux/links/alg/glbce/inc

GLBCE_LIBS=$(MCFW_ROOT_PATH)/mcfw/src_linux/links/alg/glbce/lib/libGlbceSupport_ti.a

OSA_INC=-I$(MCFW_ROOT_PATH)/mcfw/src_linux/osa/inc

DEVICES_INC=-I$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/inc -I$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/tvp5158/inc -I$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/sii9022a/inc  -I$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/thsfilters/inc

OSA_KERMOD_INC=-I$(MCFW_ROOT_PATH)/mcfw/src_linux/osa_kermod/inc

SYSLINK_INC=-I$(syslink_PATH)/packages -I$(ipc_PATH)/packages

CODEC_INC=-I$(xdais_PATH)/packages -I$(h264dec_PATH)/Inc -I$(h264enc_PATH)/Inc

LINUXDEVKIT_INC=-I$(linuxdevkit_PATH)/usr/include 

MCFW_INC=-I$(MCFW_ROOT_PATH)/mcfw/interfaces -I$(MCFW_ROOT_PATH)/mcfw/interfaces/common_def 

IPNC_APP_INC=-I$(PUBLIC_INCLUDE_DIR)

STREAM_INC=-I$(MCFW_ROOT_PATH)/demos/mcfw_api_demos/stream

USECASE_INC=-I$(MCFW_ROOT_PATH)/demos/mcfw_api_demos/multich_usecase

XDAIS_INC=-I$(xdais_PATH)/packages

COMMON_INC=-I../inc -I../priv -I. -I$(MCFW_ROOT_PATH) 

CMEM_INC=-I$(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/include

SYSLINK_LIBS=$(syslink_PATH)/packages/ti/syslink/lib/syslink.a_release

AUDIO_LIBS=$(linuxdevkit_PATH)/usr/lib/libasound.so.2

MCFW_LIBS=$(LIB_DIR)/ipnc_rdk_mcfw_api.a $(LIB_DIR)/ipnc_rdk_link_api.a $(LIB_DIR)/ipnc_rdk_osa.a $(SYSLINK_LIBS)   $(GLBCE_LIBS)

SHMEM_LIB=$(IPNC_DIR)/interface/lib/Appro_interface.a $(IPNC_DIR)/interface/lib/alarm_msg_drv.a $(IPNC_DIR)/interface/lib/share_mem.a

CMEM_LIB=$(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV



endif # ifndef $(INCLUDES_MK)

