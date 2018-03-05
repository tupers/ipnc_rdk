#Disable XDC package build by default
ENABLE_XDC_PKG_BUILD ?= false
XDC_BUILD_PKG_LIST_COMMON += $(fc_PATH)/packages/ti.sdo.fc.dskt2
XDC_BUILD_PKG_LIST_M3      = $(fc_PATH)/packages/ti.sdo.fc.ires.hdvicp
XDC_BUILD_PKG_LIST_M3      += $(XDC_BUILD_PKG_LIST_COMMON)
XDC_BUILD_PKG_LIST_C6XDSP  = 
XDC_BUILD_PKG_LIST_C6XDSP += $(XDC_BUILD_PKG_LIST_COMMON)

FIRMWARE_OUT_PATH=$(TARGET_MCFW_DIR)/firmware/
XEM3_IN_PATH=$(DEST_ROOT)/ipnc_rdk/bin/$(PLATFORM)/

apps:
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 CORE=c6xdsp
endif
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 CORE=m3vpss
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 CORE=m3video
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app xdc_configuro CORE=c6xdsp
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app all CORE=c6xdsp
endif
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app xdc_configuro CORE=m3vpss
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app CORE=m3vpss
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app xdc_configuro CORE=m3video
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app all CORE=m3video
	-mkdir -p $(FIRMWARE_OUT_PATH)
	cp $(XEM3_IN_PATH)/ipnc_rdk_m3video_$(PROFILE_m3video).xem3 $(FIRMWARE_OUT_PATH)/ipnc_rdk_fw_m3video.xem3
	cp $(XEM3_IN_PATH)/ipnc_rdk_m3vpss_$(PROFILE_m3vpss).xem3 $(FIRMWARE_OUT_PATH)/ipnc_rdk_fw_m3vpss.xem3
ifneq ($(POWER_OPT_DSP_OFF), YES)
	cp $(XEM3_IN_PATH)/ipnc_rdk_c6xdsp_$(PROFILE_c6xdsp).xe674 $(FIRMWARE_OUT_PATH)/ipnc_rdk_fw_c6xdsp.xe674
endif
	cp $(XEM3_IN_PATH)/ipnc_rdk_m3video_$(PROFILE_m3video).xem3 $(TARGET_FS_DIR)/opt/ipnc/firmware/ipnc_rdk_fw_m3video.xem3
	cp $(XEM3_IN_PATH)/ipnc_rdk_m3vpss_$(PROFILE_m3vpss).xem3 $(TARGET_FS_DIR)/opt/ipnc/firmware/ipnc_rdk_fw_m3vpss.xem3
ifneq ($(POWER_OPT_DSP_OFF), YES)
	cp $(XEM3_IN_PATH)/ipnc_rdk_c6xdsp_$(PROFILE_c6xdsp).xe674 $(TARGET_FS_DIR)/opt/ipnc/firmware/ipnc_rdk_fw_c6xdsp.xe674	
endif

clean:
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 clean CORE=c6xdsp
endif
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 clean CORE=m3vpss
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 clean CORE=m3video
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app clean CORE=c6xdsp	
endif
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app clean CORE=m3vpss
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app clean CORE=m3video

all: clean apps


