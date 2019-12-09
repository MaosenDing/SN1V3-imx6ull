#!/bin/sh

{
while true
do
	usblist=$(ls /dev/ttyGS*)
	for usbx in ${usblist}
	do
		/root/serial2tun -s ${usbx} -full -ip 192.168.0.10 -gateway 192.168.0.11 
	sleep 1
	done
done
}&

