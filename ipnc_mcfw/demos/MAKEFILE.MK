
libs:
	make -fMAKEFILE.MK -C$(MCFW_ROOT_PATH)/demos/mcfw_api_demos $(TARGET)

demo:
	make -fMAKEFILE.MK libs

clean:
	make -fMAKEFILE.MK libs TARGET=clean

all: clean demo

