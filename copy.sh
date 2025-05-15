#!/bin/sh
cd `dirname $0`
for wdir in `ls -d /usr/local/app/tars/tarsnode/data/*RoomServer/bin/`
do
	# echo $wdir
	rm -vf ${wdir}/RoomServer
	cp -vf ./RoomServer ${wdir}
done