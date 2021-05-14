echo "user bash 111"
#mount /mnt/jaffs/dropbear/ /etc/dropbear/
#ifconfig lo 127.0.0.1 up
#ifconfig eth0 192.168.50.5 up
#route add default gw 192.168.50.250
#cd /opt/topsee/ko
#./load3516ev100 -i -sensor ar0237 -osmem 32 -total 64
#telnetd 
#mount /usr/lib /usr/local/lib
#mount -t nfs -o nolock 192.168.50.160:/home/ririyeye/Desktop/nfs /home &
#dropbear
export LD_LIBRARY_PATH="/usr/local/lib:/usr/lib"
{
	sleep 10
	cd /mnt/jaffs/user/
	./daemon.exe -r 115200 &
}
