#!/bin/sh

intltool-extract --type="gettext/ini" src/multibootusb.desktop.in
intltool-extract --type="gettext/ini" src/multibootusb-kde.desktop.in

xgettext --from-code=utf-8 \
	-L Glade \
	-o po/multibootusb.pot \
	src/multibootusb.ui

xgettext --from-code=utf-8 \
	-j \
	-L Python \
	-o po/multibootusb.pot \
	src/multibootusb.c
xgettext --from-code=utf-8 -j -L C -kN_ -o po/multibootusb.pot src/multibootusb.desktop.in.h
xgettext --from-code=utf-8 -j -L C -kN_ -o po/multibootusb.pot src/multibootusb-kde.desktop.in.h

rm src/multibootusb.desktop.in.h src/multibootusb-kde.desktop.in.h

cd po
for i in `ls *.po`; do
	msgmerge -U $i multibootusb.pot
done
rm -f ./*~
