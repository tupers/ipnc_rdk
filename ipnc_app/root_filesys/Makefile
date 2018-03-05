SUBDIRS = root-filesys

all: 
	@echo "do nothing!!!"

install:
	cp -aR -f --remove-destination etc/ $(ROOT_FILE_SYS)/
	cp -aR -f --remove-destination opt/ $(ROOT_FILE_SYS)/
	cp -aR -f --remove-destination home/root/ $(ROOT_FILE_SYS)/home/
	cp -aR -f --remove-destination usr/ $(ROOT_FILE_SYS)/
	rm -f $(ROOT_FILE_SYS)/etc/resolv.conf
	rm -f $(ROOT_FILE_SYS)/usr/local/etc/esmtprc
	rm -f $(ROOT_FILE_SYS)/home/root/.esmtprc
	rm -f $(ROOT_FILE_SYS)/etc/dhcpcdhcpcd-eth0.cache
	rm -f $(ROOT_FILE_SYS)/etc/dhcpcdhcpcd-eth0.info
	rm -f $(ROOT_FILE_SYS)/etc/dhcpcdhcpcd-eth0.info.old
	rm -f $(ROOT_FILE_SYS)/etc/rcS.d/S99finish.sh
	ln -s /tmp/resolv.conf $(ROOT_FILE_SYS)/etc/resolv.conf
	ln -s /tmp/.esmtprc $(ROOT_FILE_SYS)/usr/local/etc/esmtprc
	ln -s /tmp/.esmtprc $(ROOT_FILE_SYS)/home/root/.esmtprc
	ln -s /tmp/dhcpcdhcpcd-eth0.cache $(ROOT_FILE_SYS)/etc/dhcpcdhcpcd-eth0.cache
	ln -s /tmp/dhcpcdhcpcd-eth0.info $(ROOT_FILE_SYS)/etc/dhcpcdhcpcd-eth0.info
	ln -s /tmp/dhcpcdhcpcd-eth0.info.old $(ROOT_FILE_SYS)/etc/dhcpcdhcpcd-eth0.info.old
	ln -s ../init.d/finish_ubifs.sh $(ROOT_FILE_SYS)/etc/rcS.d/S99finish.sh

	# Make /mnt/ramdisk to be a directory.
	if [ ! -d $(ROOT_FILE_SYS)/mnt/ramdisk ]; then \
		if [ -e $(ROOT_FILE_SYS)/mnt/ramdisk ]; then \
			rm -f $(ROOT_FILE_SYS)/mnt/ramdisk; \
		fi; \
		mkdir $(ROOT_FILE_SYS)/mnt/ramdisk; \
	fi
	echo you are $(UID)....
clean:
	@echo "do nothing!!!"

