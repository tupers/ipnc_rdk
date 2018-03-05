
demo:
	make -fMAKEFILE.MK libs TARGET=depend
	make -fMAKEFILE.MK libs
	make -fMAKEFILE.MK apps TARGET=exe

multich_mcfw:
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/demos/audio_sample MODULE=ipnc_rdk_mcfw $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/demos/mcfw_api_demos/multich_usecase MODULE=ipnc_rdk_mcfw $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/demos/mcfw_api_demos/stream MODULE=ipnc_rdk_mcfw $(TARGET)
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/demos/mcfw_api_demos/itt MODULE=ipnc_rdk_mcfw $(TARGET)

libs: multich_mcfw 

apps:
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/demos/mcfw_api_demos/multich_usecase MODULE=ipnc_rdk_mcfw $(TARGET)
clean:
	make -fMAKEFILE.MK libs TARGET=clean

all: clean demo 

.PHONY: ipnc_rdk_mcfw libs apps clean demo all
