#Disable XDC package build by default
ENABLE_XDC_PKG_BUILD ?= false
XDC_BUILD_PKG_LIST_COMMON += $(fc_PATH)/packages/ti.sdo.fc.dskt2
XDC_BUILD_PKG_LIST_C6XDSP  = 
XDC_BUILD_PKG_LIST_C6XDSP += $(XDC_BUILD_PKG_LIST_COMMON)

FIRMWARE_OUT_PATH=$(TARGET_MCFW_DIR)/firmware/
XEM3_IN_PATH=$(DEST_ROOT)/ipnc_rdk/bin/$(PLATFORM)/

apps:
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 CORE=c6xdsp
endif
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app xdc_configuro CORE=c6xdsp
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app all CORE=c6xdsp
endif
	-mkdir -p $(FIRMWARE_OUT_PATH)
ifneq ($(POWER_OPT_DSP_OFF), YES)
	cp $(XEM3_IN_PATH)/ipnc_rdk_c6xdsp_$(PROFILE_c6xdsp).xe674 $(FIRMWARE_OUT_PATH)/ipnc_rdk_fw_c6xdsp.xe674
endif
	cp $(XEM3_IN_PATH)/ipnc_rdk_c6xdsp_$(PROFILE_c6xdsp).xe674 $(TARGET_FS)/opt/ipnc/firmware/ipnc_rdk_fw_c6xdsp.xe674
clean:
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 clean CORE=c6xdsp
endif
ifneq ($(POWER_OPT_DSP_OFF), YES)
	$(MAKE) -C $(MCFW_ROOT_PATH)/mcfw/src_bios6/main_app clean CORE=c6xdsp	
endif

all: clean apps


