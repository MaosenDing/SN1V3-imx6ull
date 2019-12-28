#!/bin/sh
cd /mnt/jaffs/user

while true
do
sleep 1
./io.sh led1 1
sleep 1
./io.sh led1 0
done


