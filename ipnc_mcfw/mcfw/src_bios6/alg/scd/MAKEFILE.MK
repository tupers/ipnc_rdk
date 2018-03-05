ISA  ?=c674
LIBEXT ?=ae674

LIB_OUT_PATH=$(dvr_rdk_PATH)/mcfw/src_bios6/alg/scd/lib
LIB_IN_PATH=$(DEST_ROOT)/lib/$(ISA)/$(PROFILE_c6xdsp)/scd.$(LIBEXT)

.PHONY: all libs clean

libs:
	$(MAKE) -C $(dvr_rdk_PATH)/mcfw/src_bios6/alg/scd CORE=c6xdsp
	-mkdir -p $(LIB_OUT_PATH)
	cp $(LIB_IN_PATH) $(LIB_OUT_PATH)/scd.$(LIBEXT)

clean:
	$(MAKE) -C $(dvr_rdk_PATH)/mcfw/src_bios6 clean CORE=c6xdsp

all: clean libs


