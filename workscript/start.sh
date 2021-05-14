#! /bin/sh

rm -rf /tmp/SN1V2
mkdir -p /tmp/SN1V2


if [ -f 'reset.txt' ] ; then
	ln -s $(pwd)/reset.txt  /tmp/SN1V2/reset.txt
fi


