cd /opt/ipnc

./updateCopy 
sleep 2

./boot_proc &
sleep 1

./EzApp &
sleep 1

./Ezftp &
sleep 1

./system_server&
sleep 7

./EzCamScan &

echo ' inside autorun '
insmod sbull.ko
umount /mnt/ramdisk 2>/dev/null
rmmod sbull.ko 2>/dev/null
insmod sbull.ko
sleep 1
mkdosfs -v /dev/sbulla 2>/dev/null
mkdir /mnt/ramdisk 2>/dev/null
mount -t vfat /dev/sbulla /mnt/ramdisk 2>/dev/null
insmod g_file_storage.ko file=/dev/sbulla stall=0 removable=1

amixer sset 'PGA' 80
sleep 1


