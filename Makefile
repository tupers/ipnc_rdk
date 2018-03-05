# ============================================================================
# Copyright (c) Texas Instruments Inc 2013
#
# Use of this software is controlled by the terms and conditions found in the
# license agreement under which this software has been supplied or provided.
# ============================================================================

DIR=default
FILE=default

include Rules.make
include $(ROOTDIR)/makerules/build_config.mk
include $(ROOTDIR)/makerules/env.mk
include $(ROOTDIR)/makerules/platform.mk
include $(ROOTDIR)/component.mk

ipncapp: app hdvpss iss mcfw fsupdate 

clean: appclean hdvpssclean issclean mcfwclean  

all: clean ipncapp

##### Code Checkers #####

sc_indent:
ifeq ($(DIR), default)
ifeq ($(FILE), default)
	$(SC_SCRIPTS_BASE_DIR)/SCIndent_RDK.pl $(MCFW_ROOT_PATH)	
else
	$(SC_SCRIPTS_BASE_DIR)/SCIndent_RDK.pl -file $(FILE)	
endif
else
	$(SC_SCRIPTS_BASE_DIR)/SCIndent_RDK.pl -dir $(DIR)
endif

sc_check:
ifeq ($(DIR), default)
ifeq ($(FILE), default)
	$(SC_SCRIPTS_BASE_DIR)/SCCheckers_RDK.pl $(MCFW_ROOT_PATH)
else
	$(SC_SCRIPTS_BASE_DIR)/SCCheckers_RDK.pl -file $(FILE)
endif
else
	$(SC_SCRIPTS_BASE_DIR)/SCCheckers_RDK.pl -dir $(DIR)
endif

sc_insert:
ifeq ($(DIR), default)
ifeq ($(FILE), default)
	$(SC_SCRIPTS_BASE_DIR)/SCInsert_RDK.pl $(MCFW_ROOT_PATH)
else
	$(SC_SCRIPTS_BASE_DIR)/SCInsert_RDK.pl -file $(FILE)
endif
else
	$(SC_SCRIPTS_BASE_DIR)/SCInsert_RDK.pl -dir $(DIR)
endif

sc_code2comment:
	$(SC_SCRIPTS_BASE_DIR)/SCCode2Comment_RDK.pl -dir $(MCFW_ROOT_PATH)
	$(SC_SCRIPTS_BASE_DIR)/cloc-1.55.pl $(MCFW_ROOT_PATH)	

sc_cpp:
	$(SC_SCRIPTS_BASE_DIR)/SCInsertCPP_Only_RDK.pl -dir $(DIR)


#### McFW BUILD #####

mcfw_linux:
	make -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/mcfw/src_linux
	make -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/demos
	cp $(TARGET_MCFW_DIR)/bin/* $(TARGET_FS_DIR)/opt/ipnc/bin/

mcfw_linux_clean:
	make -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/mcfw/src_linux clean
	make -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/demos clean

mcfw_linux_all: mcfw_linux_clean mcfw_linux

mcfw_bios6: 
	$(MAKE) -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 $(TARGET)

mcfw_vpssm3: 
	$(MAKE) -fVPSSM3MAKEFILE.MK -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 $(TARGET)

mcfw_ezdsp: 
	$(MAKE) -fDSPMAKEFILE.MK -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 $(TARGET)

mcfw_ezdsp_clean:
	$(MAKE) -fDSPMAKEFILE.MK -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 clean

mcfw_bios6_clean:
	$(MAKE) -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/mcfw/src_bios6 clean

mcfw_bios6_all: mcfw_bios6_clean mcfw_bios6

mcfw: mcfw_linux mcfw_bios6 

mcfwclean: mcfw_linux_clean mcfw_bios6_clean 

mcfwall: mcfwclean mcfw 

##### IPNCAPP BUILD #####

applibs:
ifneq ($(MAKE_TARGET), depend)
	$(MAKE) -C$(IPNC_DIR) ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) $(MAKE_TARGET)
endif

appclean:
	$(MAKE) applibs MAKE_TARGET=clean

appdepend:
	$(MAKE) applibs MAKE_TARGET=depend  

appinstall:
	$(MAKE) applibs MAKE_TARGET=install

app: appdepend applibs appinstall

appall: appclean app

app_server:
	$(MAKE) -C$(IPNC_DIR)/sys_server ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX)
	$(MAKE) -C$(IPNC_DIR)/sys_server ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) install
app_util:
	$(MAKE) -C$(IPNC_DIR)/utils ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX)
	$(MAKE) -C$(IPNC_DIR)/utils ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) install

##### XDC BUILD #####

XDC_BUILD_PKG_LIST  = $(fc_PATH)/packages/ti.sdo.fc.ires.hdvicp

xdcpkgs:
	$(xdc_PATH)/xdc XDCBUILDCFG=$(CONFIG_BLD_XDC_$(ISA)) -PD $(subst .,/,$(XDC_BUILD_PKG_LIST))

xdcpkgs_clean:
	$(xdc_PATH)/xdc XDCBUILDCFG=$(CONFIG_BLD_XDC_$(ISA)) clean -PD $(subst .,/,$(XDC_BUILD_PKG_LIST))

##### ISS BUILD #####

iss:
	$(MAKE) -C $(iss_PATH)/packages/ti/psp/iss $(TARGET) CORE=m3vpss

issclean:
	$(MAKE) iss TARGET=clean

issall: issclean iss

##### HDVPSS BUILD #####

hdvpss:
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/vps $(TARGET) CORE=m3vpss
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/i2c $(TARGET) CORE=m3vpss
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/devices $(TARGET) CORE=m3vpss
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/platforms $(TARGET) CORE=m3vpss
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/proxyServer $(TARGET) CORE=m3vpss

hdvpssclean:
	$(MAKE) hdvpss TARGET=clean

hdvpssall: hdvpssclean hdvpss

itt:
	#$(MAKE) -C $(MCFW_ROOT_PATH)/itt/itt_bios6/dcc $(TARGET)
	#cp $(DEST_ROOT)/lib/$(ISA)/$(PROFILE_m3video)/dcc.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/dcc/lib/release/iss_dcc.aem3
	#$(MAKE) -C $(MCFW_ROOT_PATH)/itt/itt_bios6/vsp_awb $(TARGET)
	#cp $(DEST_ROOT)/lib/$(ISA)/$(PROFILE_m3video)/vsp_awb.aem3 $(MCFW_ROOT_PATH)/itt/itt_bios6/vsp_awb/lib/vsp_awb.aem3
	$(MAKE) -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/itt/itt_bios6/itt_app $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/itt/itt_linux
	cp $(MCFW_ROOT_PATH)/itt/itt_linux/autorun.sh $(EXEC_DIR)/.
	make fsupdate
	
itt_clean:
	#$(MAKE) -C $(MCFW_ROOT_PATH)/itt/itt_bios6/dcc clean
	#$(MAKE) -C $(MCFW_ROOT_PATH)/itt/itt_bios6/vsp_awb clean
	$(MAKE) -fMAKEFILE.MK -C $(MCFW_ROOT_PATH)/itt/itt_bios6/itt_app clean
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/itt/itt_linux clean
dcc:
	$(MAKE) -C $(MCFW_ROOT_PATH)/itt/itt_bios6/dcc $(TARGET) CORE=m3vpss
	cp $(DEST_ROOT)/lib/$(ISA)/$(PROFILE_m3video)/dcc.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/dcc/lib/release/iss_dcc.aem3
	
dcc_clean:
	$(MAKE) -C $(MCFW_ROOT_PATH)/itt/itt_bios6/dcc clean
	
##### U-BOOT BUILD #####

ubootbuild:
	$(MAKE) -C$(UBOOTDIR) ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) $(MAKE_TARGET)

ubootmin:
	$(MAKE) ubootclean
	$(MAKE) ubootbuild MAKE_TARGET=$(PLATFORM_CFG)_$(SYSTEM_CFG)_min_$(BINARY_MODE)
	$(MAKE) ubootbuild MAKE_TARGET=u-boot.ti
ifeq ($(BINARY_MODE),sd)
	cp $(UBOOTDIR)/u-boot.min.$(BINARY_MODE) $(TFTP_HOME)/MLO
else
	cp $(UBOOTDIR)/u-boot.min.$(BINARY_MODE) $(TFTP_HOME)/u-boot.min.$(BINARY_MODE)
endif

ubootbin:
	$(MAKE) ubootclean
	$(MAKE) ubootbuild MAKE_TARGET=$(PLATFORM_CFG)_$(SYSTEM_CFG)_config_$(BINARY_MODE)
	$(MAKE) ubootbuild MAKE_TARGET=u-boot.ti
	cp $(UBOOTDIR)/u-boot.bin $(TFTP_HOME)/u-boot.bin

ubootopti:
	$(MAKE) ubootclean
	$(MAKE) ubootbuild MAKE_TARGET=$(PLATFORM_CFG)_ipnc_opti_nand
	$(MAKE) ubootbuild MAKE_TARGET=u-boot.ti
	cp $(UBOOTDIR)/u-boot.opti.nand $(TFTP_HOME)/u-boot.opti.nand 
	
ubootclean:
	$(MAKE) ubootbuild MAKE_TARGET=distclean
	
uboot:
	$(MAKE) ubootmin
	$(MAKE) ubootbin

ubootall:
	$(MAKE) ubootclean
	$(MAKE) uboot

##### LSP BUILD #####
lspbuild:
	make -C$(KERNELDIR) ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) $(MAKE_TARGET)
	
lspmenu:
	make lspbuild MAKE_TARGET=menuconfig

lspcfg:
	make lspbuild MAKE_TARGET=$(PLATFORM_CFG)_$(SYSTEM_CFG)_$(FS_CFG)_defconfig
	
lsp:
	cp $(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/src/module/cmemk.o $(KERNELDIR)/drivers/char/
	make lspcfg
	make lspbuild MAKE_TARGET=uImage
	make lspbuild MAKE_TARGET=modules
	cp $(KERNELDIR)/arch/arm/boot/uImage $(TFTP_HOME)/uImage
	cp $(KERNELDIR)/arch/arm/boot/uImage /tftpboot/
	-mkdir -p $(TARGET_MCFW_DIR)/kermod
	cp $(KERNELDIR)/drivers/video/ti81xx/vpss/vpss.ko $(TARGET_MCFW_DIR)/kermod/.
	cp $(KERNELDIR)/drivers/video/ti81xx/ti81xxfb/ti81xxfb.ko $(TARGET_MCFW_DIR)/kermod/.
	cp $(KERNELDIR)/drivers/video/ti81xx/ti81xxhdmi/ti81xxhdmi.ko $(TARGET_MCFW_DIR)/kermod/.
	cp $(KERNELDIR)/drivers/usb/gadget/g_file_storage.ko $(EXEC_DIR)
	cp $(KERNELDIR)/block/sbull/sbull.ko $(EXEC_DIR)
	cp $(KERNELDIR)/drivers/mtd/devices/m25p80.ko $(EXEC_DIR)/kermod/.
lspall:
	make lspclean
	make lsp
	
lspclean:	
	make lspbuild MAKE_TARGET=distclean

lspcfgopti:
	make lspbuild MAKE_TARGET=$(PLATFORM_CFG)_ipnc_$(FS_CFG)_opt_defconfig

lspopti:
	cp $(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/src/module/cmemk.o $(KERNELDIR)/drivers/char/
	make lspcfgopti
	make lspbuild MAKE_TARGET=uImage 
	make lspbuild MAKE_TARGET=modules
	cp $(KERNELDIR)/arch/arm/boot/uImage $(TFTP_HOME)/uImage

	
##### SYSLINK BUILD #####

syslinkbuild:
	cp $(MCFW_ROOT_PATH)/makerules/syslink_products.mak $(syslink_PATH)/products.mak
	make -C$(syslink_PATH) $(TARGET)

syslink:
	make syslinkbuild DEVICE=$(syslink_DEVICE) TARGET=syslink
	-mkdir -p $(TARGET_MCFW_DIR)/kermod
	cp $(syslink_OUT_DIR)/syslink.ko $(TARGET_MCFW_DIR)/kermod/.

syslinkclean:
	make syslinkbuild DEVICE=$(syslink_DEVICE) TARGET=clean

syslinkall: syslinkclean syslink

##### CMEM MAKE #####

cmem:
	make -C$(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/src/interface \
	LINUXKERNEL_INSTALL_DIR=$(KERNELDIR) MVTOOL_PREFIX=$(BUILD_TOOL_PREFIX)
	make -C$(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/src/module \
	LINUXKERNEL_INSTALL_DIR=$(KERNELDIR) MVTOOL_PREFIX=$(BUILD_TOOL_PREFIX)
	
cmemclean:
	make -C$(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/src/module clean
	
cmemall: cmemclean cmem	

##### FILESYS MAKE #####

ubifs:
	mkdir -p $(IPNC_INSTALL_DIR)/tmp
	$(MTD_UTILS)/mkfs.ubifs -r $(TARGET_FS) -F -o ./tmp/ubifs.img -m 2048 -e 126976 -c 827
	$(MTD_UTILS)/ubinize -o $(TFTP_HOME)/ubifs_$(SYSTEM_CFG).bin -m 2048 -p 128KiB -O 2048 $(IPNC_INSTALL_DIR)/ubinize.cfg
	rm -rf $(IPNC_INSTALL_DIR)/tmp
	
fsupdate:
	cp -R $(TARGET_MCFW_DIR)/* $(EXEC_DIR)/.
	chmod 755 $(EXEC_DIR)/*.sh
ifeq ($(IMGS_ID), IMGS_MICRON_AR0331)
	@echo "dcc_default_ar0331.bin selected"
	cp  $(EXEC_DIR)/dcc/dcc_default_ar0331.bin $(EXEC_DIR)/dcc/dcc_default.bin
endif
ifeq ($(IMGS_ID), IMGS_MICRON_AR0330)
	@echo "dcc_default_ar0331.bin selected"
	cp  $(EXEC_DIR)/dcc/dcc_default_ar0331.bin $(EXEC_DIR)/dcc/dcc_default.bin
endif
ifeq ($(IMGS_ID), IMGS_PANASONIC_MN34041)
	@echo "dcc_default_mn34041.bin selected"
	cp  $(EXEC_DIR)/dcc/dcc_default_mn34041.bin $(EXEC_DIR)/dcc/dcc_default.bin
endif
ifeq ($(IMGS_ID), IMGS_SONY_IMX104)
	@echo "dcc_default_imx104.bin selected"
	cp  $(EXEC_DIR)/dcc/dcc_default_imx104.bin $(EXEC_DIR)/dcc/dcc_default.bin
endif
ifeq ($(IMGS_ID), IMGS_SONY_IMX136)
	@echo "dcc_default_imx136.bin selected"
	cp  $(EXEC_DIR)/dcc/dcc_default_imx136.bin $(EXEC_DIR)/dcc/dcc_default.bin
endif
ifeq ($(IMGS_ID), IMGS_SONY_IMX140)
	@echo "dcc_default_imx136.bin selected"
	cp  $(EXEC_DIR)/dcc/dcc_default_imx136.bin $(EXEC_DIR)/dcc/dcc_default.bin
endif
ifeq ($(IMGS_ID), IMGS_MICRON_MT9M034)
	@echo "dcc_default_9m034.bin selected"
	cp  $(EXEC_DIR)/dcc/dcc_default_9m034.bin $(EXEC_DIR)/dcc/dcc_default.bin
endif


ifeq ($(APP_BUILD_CFG),release)
	$(STRIP470) $(TARGET_FS)/opt/ipnc/firmware/ipnc_rdk_fw_m3video.xem3
	$(STRIP470) $(TARGET_FS)/opt/ipnc/firmware/ipnc_rdk_fw_m3vpss.xem3
ifeq ($(PLATFORM),ti814x-evm)
	$(STRIP6x)  $(TARGET_FS)/opt/ipnc/firmware/ipnc_rdk_fw_c6xdsp.xe674
endif
	chmod 755   $(TARGET_FS)/opt/ipnc/firmware/*.*
endif

	@echo 
	@echo "--------------------------------------------------------------"
	@echo "******** Build completed for following configuration *********"
	@echo "--------------------------------------------------------------"
	@echo "Hardware Platform  : $(IPNC_DEVICE) $(SYSTEM_PLATFORM)"
	@echo "Build Mode         : $(APP_BUILD_CFG)"
	@echo "Hardware Revision  : $(HARDWARE_REVISION)"
	@echo "Sensor Used        : $(IMGS_ID)"
	@echo "Sensor Vendor      : $(SENSOR_BOARD_VENDOR)"
	@echo "Configuration      : $(IPNC_CONFIG)"
	@echo "Filesystem Mode    : $(FS_CFG)"	
	@echo "Memory Config      : $(MEMORY_CONFIG)"	
	@echo "Build Date         : $(shell date)"
	@echo "--------------------------------------------------------------"
	@echo 
	@echo "------------------------------------------------------------------------------------------------------------"
	@echo "Please run < make lsp > if you have changed anything in kernel modules and use the new built image"
	@echo "------------------------------------------------------------------------------------------------------------"

nfsreset:
	/usr/sbin/exportfs -av
	/etc/init.d/nfs-kernel-server restart

##### SYSTEM BUILD #####

sysclean: binariesclean syslinkclean lspclean ubootclean cmemclean clean

sys: 
	make uboot 
	make lspcfg 
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) prepare
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) modules_prepare
	make cmem 
	make lsp 
	make syslink 
	make ipncapp 

sysopti: 
	make ubootopti 
	make lspcfgopti 
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) prepare
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(BUILD_TOOL_PREFIX) modules_prepare
	make cmem 
	make lspopti
	make syslink 
	make ipncapp 
	
sysall: sysclean sys

sysoptiall: sysclean sysopti

##### Release Binaries #####

binaries:
	make sysall
	make
	make $(FS_CFG)
	
binariesopti:
	make sysoptiall
	make
	make $(FS_CFG)

binariesclean:
	rm -rf $(TARGET_MCFW_DIR)/bin 
	rm -rf $(TARGET_MCFW_DIR)/firmware 
	rm -rf $(TARGET_MCFW_DIR)/kermod
	rm -rf $(MCFW_ROOT_PATH)/build
	

