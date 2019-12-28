#!/bin/sh

if [ $# -lt 1 ] ; then 
	echo exit
	exit
fi


if [ $1 == 'true' ] ; then


while $1
do
/etc/init.d/nfs-kernel-server restart
echo "restart nfs"
sleep 1000
done

fi
