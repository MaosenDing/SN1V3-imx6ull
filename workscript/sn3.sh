#! /bin/sh

cd /mnt/jaffs/user/
echo sn3 > /tmp/snsta
echo '' > PES.sn

killall daemon.exe
sleep 1
killall aim.exe
sleep 1
killall aim.exe
sleep 1

./ir.sh 1
./aim.exe sn3 SCG.txt



#SN3结束后，系统重启
#cd /mnt/jaffs/user/
#cp CRESN1.sn PES.sn 
#sleep 2
#reboot



