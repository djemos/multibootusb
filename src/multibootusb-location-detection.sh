#!/bin/sh
if [ "$1" != "copy" ]; then
	echo "usage: $0 copy"
	exit 1
fi

usbdevice=`cat /proc/partitions | sed 's/  */:/g' | cut -f5 -d: | sed -e /^$/d -e /[1-9]/d -e /^sr0/d -e /loop/d -e/ram/d -e /^sda/d`
#	livesize=`du -s /live/media/boot | sed 's/\t.*//'`
#else bootdevice="notlive" #to list all partitions
#	livesize=0


case "$1" in
"copy") requiredsize=$livesize ;;
esac

for disk in $usbdevice; do
	echo "/dev/$disk"
done
