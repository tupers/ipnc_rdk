restrict default noquery notrust nomodify
restrict 127.0.0.1
restrict 192.168.1.0 mask 255.255.255.0
fudge 127.127.1.0 stratum 3
server 127.127.1.0
driftfile /etc/ntp.drift
logfile /var/log/ntp.log
