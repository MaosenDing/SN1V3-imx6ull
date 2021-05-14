#!/bin/sh 

if [ $1 -eq 1 ]; then

	echo 9 > /sys/class/gpio/export

	echo out > /sys/class/gpio/gpio9/direction

	echo 1 > /sys/class/gpio/gpio9/value
	
elif [ $1 -eq 0 ]; then

	echo 9 > /sys/class/gpio/export

	echo out > /sys/class/gpio/gpio9/direction

	echo 0 > /sys/class/gpio/gpio9/value

else
	echo 'usage error'
fi
echo 9 > /sys/class/gpio/unexport









