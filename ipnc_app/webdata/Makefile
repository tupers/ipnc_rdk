SUBDIRS = util gui

.PHONY: $(SUBDIRS) $(DEBUGSUBDIRS) $(RELEASESUBDIRS) $(INSTALLSUBDIRS) \
	$(CLEANSUBDIRS)

all: 
	@echo "do nothing!!!"

install:
	rm -rf $(ROOT_FILE_SYS)/var/www
	cp -aR -f gui/common/. $(ROOT_FILE_SYS)/var/www/
	cp -aR -f util/. $(ROOT_FILE_SYS)/var/www/
	cp -aR -f etc/ $(ROOT_FILE_SYS)/
	cp -aR -f alarm_wav/. $(EXEC_DIR)/
	rm -f $(ROOT_FILE_SYS)/var/www/tvdevicedesc.xml
	ln -s /tmp/tvdevicedesc.xml $(ROOT_FILE_SYS)/var/www/tvdevicedesc.xml

ifeq ($(GUI_ENGINE_MODE), VLABS_VLC_OPENSRC)
	cp -aR -f gui/vLabs/. $(ROOT_FILE_SYS)/var/www/
else
	cp -aR -f gui/GoDB/. $(ROOT_FILE_SYS)/var/www/
endif

clean:
	@echo "do nothing!!!"

