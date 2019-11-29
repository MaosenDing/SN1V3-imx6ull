#!/bin/sh


function ctrlIO()
{
	if [ ! -d /sys/class/gpio/gpio$1 ] ; then
		echo "init io $1"
		echo $1 > /sys/class/gpio/export
	fi	
	echo out > /sys/class/gpio/gpio$1/direction
	echo $2 > /sys/class/gpio/gpio$1/value
}

if [ $# -lt 2 ] ; then 
	echo "please input 2 para,like $0 jd2 1"
	echo "you can conctrl jd2 jd3 led1 led2"
	exit
fi

if [ $1 == "jd2" ]; then
	ionum=201
fi

if [ $1 == "jd3" ]; then
	ionum=200
fi


if [ $1 == "led1" ]; then
	ionum=199
fi


if [ $1 == "led2" ]; then
	ionum=198
fi

if [ $1 == "ir" ]; then
	ionum=36
fi

if [ ${ionum} != "" ] ; then
	ctrlIO $ionum $2
	echo "set $1 $2"
fi




