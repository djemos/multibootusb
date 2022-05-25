#!/bin/bash
# Gettext internationalization
#
# Copyright 2016, 2017  Dimitris Tzemos, GR
# All rights reserved.
#
# Redistribution and use of this script, with or without modification, is
# permitted provided that the following conditions are met:
#
# 1. Redistributions of this script must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
#  EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# "BASED ON CODE OF BUILD_SLACKWARE-LIVE.SH FROM linux-nomad"
# Eric Hameleers <alien@slackware.com> (encryption support)

AUTHOR='Dimitris Tzemos - dijemos@gmail.com'
LICENCE='GPL v3+'
SCRIPT=$(basename "$0")
SCRIPT=$(readlink -f "$SCRIPT")
VER=2.0

version() {
  echo "Slackel USB installer and persistent creator for 32 and EFI/UEFI 64 v$VER"
  echo " by $AUTHOR"
  echo "Licence: $LICENCE"
}
usage() {
  echo 'multiple_iso_on_usb.sh [-h/--help] [-v/--version]'
  echo ' -h, --help: this usage message'
  echo ' -v, --version: the version author and licence'
  echo ''
  echo "`basename $0` --usb isoimagesdir device"
  echo ''
  echo '-> --usb option installs syslinux on a USB key using an ISO (specify path to image and device)'
  echo ''
  echo '-> isoimagesdir is the dir containing iso images files'
  exit 1
}

if [ "$1" = "--version" ] || [ "$1" = "-v" ]; then
version
  exit 0
fi
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
usage
fi
#if [ $(id -ru) -ne 0 ]; then
#echo "Error : you must run this script as root" >&2
#  exit 2
#fi

LIVELABEL="LIVE" #edit in init script too
LIVEFS="vfat"
CWD=`pwd`
CMDERROR=1
PARTITIONERROR=2
FORMATERROR=3
BOOTERROR=4
INSUFFICIENTSPACE=5
persistent_file=""
ENCRYPT=""
SCRIPT_NAME="$(basename $0)"
NAME="persistent"
ENCRYPT=""
SVER=15.0

function check_root(){
# make sure I am root
if [ `id -u` != "0" ]; then
	echo "ERROR: You need to be root to run $SCRIPT_NAME"
	echo "The install_on_usb_uefi.sh script needs direct access to your boot device."
	echo "Use sudo or kdesudo or similar wrapper to execute this."
	exit 1
fi
}

function find_syslinux(){
# we need the path to syslinux files to be able to use the script 
# from other distro's e.g. ubuntu
if [ -d /usr/share/syslinux ]; then
  PATH_TO_SYSLINUX="/usr/share/syslinux"
elif [ -d /usr/lib/SYSLINUX ]; then
  PATH_TO_SYSLINUX="/usr/lib/SYSLINUX"
elif [ -d /usr/lib/syslinux ]; then
    if [ -f /usr/lib/syslinux/mbr.bin ]; then
		PATH_TO_SYSLINUX="/usr/lib/syslinux"
	elif [ -f /usr/lib/syslinux/mbr/mbr.bin ]; then
		PATH_TO_SYSLINUX="/usr/lib/syslinux/mbr"
	fi	
fi
}

function check_device(){
installmedia=$1
msgdev=$1
#Be sure do not format hard disk
disk=`echo $installmedia | cut -c6-8`  
 if [ "$disk" == "sda" ] || [ "$disk" == "hda" ]; then
	echo "You cannot install to /dev/$disk hard disk drive"
	exit
 fi
 
 # check if usb device pluged in
 disks=`cat /proc/partitions | sed 's/  */:/g' | cut -f5 -d: | sed -e /^$/d -e /[1-9]/d -e /^sr0/d -e /loop/d -e /ram/d -e /^sda/d`
 disk=`echo $installmedia | cut -c6-8` 
 
 installmedia="" ## clear the variable
	for usb in $disks; do
		if [ "$usb" == "$disk" ]; then
			installmedia="/dev/$usb"
		fi	
		done
if [ "$installmedia" == "" ] || [ "$installmedia" == NULL ]; then
	echo "There is no removable usb device $msgdev attached to your system"
	exit
else
echo "========================================="
echo "										   "
echo "Removable device is $installmedia        "
echo "										   "
echo "========================================="
flag=*
for n in $installmedia$flag ; do umount $n > /dev/null 2>&1; done
fi
}

function prepare_usb() {
	device=`echo $installmedia | cut -c6-8`
	sectorscount=`cat /sys/block/$device/size`
	sectorsize=`cat /sys/block/$device/queue/hw_sector_size`
	let mediasize=$sectorscount*$sectorsize/1048576 #in MB
	installdevice="/dev/$device"
#if [ "$installdevice" == "$installmedia" ]; then #install on whole disk: partition and format media
		#if [ "$iso_arch" == "64" ]; then #EFI/GPT
			dd if=/dev/zero of=$installdevice bs=512 count=34 >/dev/null 2>&1
			if [ "$LIVEFS" == "vfat" ]; then
				partitionnumber=1
				installmedia="$installdevice$partitionnumber"
				echo -e "2\nn\n\n\n\n0700\nr\nh\n1 2\nn\n\ny\n\nn\n\nn\nwq\ny\n" | gdisk $installdevice || return $PARTITIONERROR
				partprobe $installdevice >/dev/null 2>&1; sleep 3
				fat32option="-F 32"
				mkfs.vfat $fat32option -n "$LIVELABEL" $installmedia || return $FORMATERROR
				sleep 3
			fi
		#else #BIOS/MBR
		#	partitionnumber=1
		#	installmedia="$installdevice$partitionnumber"
		#	if (( $mediasize < 2048 ))
		#	then heads=128; sectors=32
		#	else heads=255; sectors=63
		#	fi
		#	mkdiskimage $installdevice 1 $heads $sectors || return $PARTITIONERROR
		#	dd if=/dev/zero of=$installdevice bs=1 seek=446 count=64 >/dev/null 2>&1
		#	if [ "$LIVEFS" = "vfat" ]; then
		#		echo -e ',,b,*' | sfdisk $installdevice || return $PARTITIONERROR
		#		partprobe $installdevice; sleep 3
		#		fat32option="-F 32"
		#		mkfs.vfat $fat32option -n "$LIVELABEL" $installmedia || return $FORMATERROR
		#	fi
		#	sleep 3
		#fi
#fi

syslinux -maf $installmedia             # installation of syslinux and MBR
mkdir -p /media/mb                         # create point of mount
mount $installmedia /media/mb           # mount of the USB drive
mkdir -p /media/mb/syslinux                # create the directory for the syslinux configuration
cp /usr/share/syslinux/{vesamenu.c32,reboot.c32,poweroff.com} /media/mb/syslinux  # copy same file necessary
mkdir -p /media/mb/syslinux/iso           # creation of the directory where to store every Linux distribution we want
}

function change_init_to_multiboot()
{
mkdir cpiowork
cd cpiowork
gzip -dc /media/mb/syslinux/iso/$filename/boot/initrd.gz | cpio -i -d -H newc --no-absolute-filenames
cp /usr/share/multibootusb/init init
sed -i.bak '3i\
path_to_modules="syslinux/iso/fake"\
' init
sed -i "s/fake/$filename/" init

sed -i.bak '4i\
DISTRONAME="fake"\
' init
sed -i "s/fake/$filename/" init
if [[ $filename == *"salix"* ]]; then
	sed -i.bak '/$READWRITE = yes/d' init
	sed -i.bak '/# Disable the root filesystem check altogether:/d' init
fi
rm init.bak

chmod +x init
find . -print | cpio -o -H newc | gzip -9 > /media/mb/syslinux/iso/$filename/boot/initrd.gz
cd ../
rm -rf cpiowork
}

function copy_iso_images()
{
echo ""
echo ""
echo "Copying isoimages on $installmedia"
echo ""
echo ""
for isoimage in $isoimagesdir/* 
do
	filename=$(basename $isoimage) 
	extension="${filename##*.}"
	filename="${filename%.*}"
	mkdir -p /media/mb/syslinux/iso/$filename
	
	ISODIR=$(mktemp -d)
	LODEVISO=$(losetup -f)
	losetup $LODEVISO $isoimage
	mount $LODEVISO $ISODIR > /dev/null 2>&1
	cp -r $ISODIR/* /media/mb/syslinux/iso/$filename/
	change_init_to_multiboot
	umount $ISODIR
	losetup -d $LODEVISO || true
	rm -rf $ISODIR
 done
}

function make_usb_bootable()
{ 
usbdevice=`echo $installmedia | cut -c6-8`
echo ""
echo "Make $usbdevice bootable"
echo ""
	if fdisk -l $installdevice 2>/dev/null | grep -q "^$installmedia "; then #legacy / CSM (Compatibility Support Module) boot, if $installmedia present in MBR (or hybrid MBR)
		sfdisk --force $installdevice -A $partitionnumber 2>/dev/null
		if mount | grep -q "^$installmedia .* vfat "; then #FAT32
			umount /media/mb
			# Use syslinux to make the USB device bootable:
			echo "--- Making the USB drive '$installdevice' bootable using syslinux..."
			#syslinux -d /media/mb/syslinux/iso/$filename/boot/syslinux $installmedia || return $BOOTERROR
			cat ${PATH_TO_SYSLINUX}/mbr.bin > $installdevice
		fi
	#else
		#umount /media/mb
	fi
	#umount /media/mb 2>/dev/null
	sleep 2
	#rm -rf /media/mb
	return 0
}


function create_boot_menu()
{
	cat > /media/mb/syslinux/syslinux.cfg << EOF
DEFAULT vesamenu.c32
MENU BACKGROUND slackel.png
PROMPT 0
TIMEOUT 300
MENU color border	30;44      #00000000 #00000000 none
MENU TABMSGROW 2
MENU CMDLINEROW 26
MENU TABMSG Press [Tab] to edit bootparameters

LABEL -
	MENU LABEL
	MENU DISABLE

EOF
flag="true"
for isoimage in $isoimagesdir/* 
do
	filename=$(basename $isoimage) 
	extension="${filename##*.}"
	filename="${filename%.*}"

echo "label $filename
MENU LABEL $filename
kernel /syslinux/iso/$filename/boot/vmlinuz
append initrd=/syslinux/iso/$filename/boot/initrd.gz max_loop=255 vga=791 locale=$language keymap=$keyboard useswap=yes copy2ram=no tz=$timezone hwc=localtime runlevel=4
" >> /media/mb/syslinux/syslinux.cfg
done

echo "MENU SEPARATOR

LABEL Reboot
MENU LABEL Reboot
COM32 reboot.c32

LABEL Power Off
MENU LABEL Power Off
COMBOOT poweroff.com"	>> /media/mb/syslinux/syslinux.cfg
}

function copy_background_images()
{
cp /usr/share/multibootusb/*.png /media/mb/syslinux
}

function create_boot_menu-bak()
{
	cat > /media/mb/syslinux/syslinux.cfg << EOF
DEFAULT vesamenu.c32
PROMPT 0
allowoptions 0
TIMEOUT 100
UI vesamenu.c32
MENU TITLE USB Multiboot Key
MENU BACKGROUND splash02.png
MENU COLOR border 30;44 #40ffffff #a0000000 std
MENU COLOR title 1;36;44 #9033ccff #a0000000 std
MENU COLOR sel 7;37;40 #e0ffffff #20ffffff all
MENU COLOR unsel 37;44 #50ffffff #a0000000 std
MENU COLOR help 37;40 #c0ffffff #a0000000 std
MENU COLOR timeout_msg 37;40 #80ffffff #00000000 std
MENU COLOR timeout 1;37;40 #c0ffffff #00000000 std
MENU COLOR msg07 37;40 #90ffffff #a0000000 std
MENU COLOR tabmsg 31;40 #30ffffff #00000000 std
MENU WIDTH 80
MENU MARGIN 10
MENU ROWS 15        # number of rows in the menu
MENU TABMSGROW 18
MENU CMDLINEROW 18
MENU ENDROW -1
MENU PASSWORDROW 11
MENU TIMEOUTROW 20
MENU HELPMSGROW 22
MENU HELPMSGENDROW -1
MENU HIDDENROW -2
MENU HSHIFT 0
MENU VSHIFT 4   # vertical position of the menu

EOF
flag="true"
for isoimage in $isoimagesdir/* 
do
	filename=$(basename $isoimage) 
	extension="${filename##*.}"
	filename="${filename%.*}"

echo "label $filename
MENU LABEL $filename
kernel /syslinux/iso/$filename/boot/vmlinuz
append initrd=/syslinux/iso/$filename/boot/initrd.gz max_loop=255 vga=791 locale=$language keymap=$keyboard useswap=yes copy2ram=no tz=$timezone hwc=localtime runlevel=4
" >> /media/mb/syslinux/syslinux.cfg
done

echo "MENU SEPARATOR

LABEL Reboot
MENU LABEL Reboot
COM32 reboot.c32

LABEL Power Off
MENU LABEL Power Off
COMBOOT poweroff.com"	>> /media/mb/syslinux/syslinux.cfg
}

action=$1
case $action in
"--usb")
    isoimagesdir=$2
    installmedia=$3
	timezone=$4
	language=$5
	keyboard=$6
    if [ "$installmedia" == "" ] || [ "$installmedia" == NULL ]; then
    		echo "`basename $0` --usb isoimagesdir device"
		exit $CMDERROR
	fi
    check_root
	flag=*
	for n in $installmedia$flag ; do umount $n > /dev/null 2>&1; done
	check_device $installmedia
	find_syslinux
	
    if [ -d "$isoimagesdir" ] && [ -b "$installmedia" ]; then
			prepare_usb $installmedia
			copy_iso_images 
			create_boot_menu
			copy_background_images
			make_usb_bootable
			umount /media/mb 2>/dev/null
			sleep 2
			rm -rf /media/mb
			exit $!
	else
		echo "`basename $0` --usb isoimagesdir device"
		exit $CMDERROR
	fi
		cd ~/
		sleep 5
		umount  /tmp/tmp.* > /dev/null 2>&1
		rm -rf /tmp/tmp.*
;;
"--version")
	version
    exit 0
	;;
"--help")
usage
exit 0
;;	
	
*)	echo "`basename $0` --usb --usb isoimagesdir device"
	exit $CMDERROR
	;;
esac
