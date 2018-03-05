
ipnc_rdk:
	make -fMAKEFILE.MK libs TARGET=depend
	make -fMAKEFILE.MK libs
	make -fMAKEFILE.MK utils TARGET=exe
	make -C$(MCFW_ROOT_PATH)/mcfw/src_linux/osa_kermod/build install

utils:
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/utils/mem_rdwr/src MODULE=mem_rdwr $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/utils/remote_debug_client/src MODULE=remote_debug_client $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/utils/sys_pri/src MODULE=sys_pri $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/utils/fw_load/src MODULE=fw_load $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/utils/i2c_rdwr/src MODULE=i2c_rdwr $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/utils/vpdma_dump MODULE=vpdma_dump $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/utils/mem_stats MODULE=mem_stats $(TARGET)	

mcfw_api:
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/mcfw_api MODULE=ipnc_rdk_mcfw_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/mcfw_api/usecases MODULE=ipnc_rdk_mcfw_api $(TARGET)
 
link_api:
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/system MODULE=ipnc_rdk_link_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/ipcBitsIn MODULE=ipnc_rdk_link_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/ipcBitsOut MODULE=ipnc_rdk_link_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/ipcFramesIn MODULE=ipnc_rdk_link_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/ipcFramesOut MODULE=ipnc_rdk_link_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/avsync MODULE=ipnc_rdk_link_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/glbceSupport MODULE=ipnc_rdk_link_api $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/links/FrameProcess MODULE=ipnc_rdk_link_api $(TARGET)
osa:
	make -C$(MCFW_ROOT_PATH)/mcfw/src_linux/osa_kermod/build $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/osa/src MODULE=ipnc_rdk_osa $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/src MODULE=ipnc_rdk_osa $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/tvp5158/src MODULE=ipnc_rdk_osa $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/sii9022a/src MODULE=ipnc_rdk_osa $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/mcfw/src_linux/devices/thsfilters/src MODULE=ipnc_rdk_osa $(TARGET)

libs: osa link_api mcfw_api utils

clean:
	make -fMAKEFILE.MK libs TARGET=clean

all: clean ipnc_rdk

.PHONY: mcfw_api link_api osa fbdev libs utils clean all ipnc_rdk
