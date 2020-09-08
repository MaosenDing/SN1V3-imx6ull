!/bin/sh
ssh -n -f root@192.168.50.164 "source /etc/profile;cd /mnt/bin;gdbserver 0.0.0.0:2345 ./aim.exe crctest"

