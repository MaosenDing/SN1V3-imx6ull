#! /bin/sh

cd /mnt/jaffs/user/

killall daemon.exe
sleep 1
killall daemon.exe
sleep 1
killall aim.exe
sleep 1
killall aim.exe
sleep 1

./daemon.exe -r 115200


