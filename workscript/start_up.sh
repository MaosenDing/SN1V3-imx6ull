#!/bin/sh

sleep 5
insmod /home/root/driver/ov5640_camera.ko
insmod /home/root/driver/mx6s_capture.ko
ifconfig eth1 192.168.50.50 up

sleep 5
cd /mnt/jaffs/user/
./ir.sh 0

sleep 1
./daemon.exe -r 115200

