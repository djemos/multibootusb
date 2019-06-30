#!/bin/sh
if [ "$1" != "locale" ]; then
	echo "usage: $0 locale"
	exit 1
fi

locale -cva \
| grep -A 2 utf8 | sed '/^-/d' \
| sed '
/^locale:.*directory.*/{
s/^locale: \([^ ]*\) *directory.*$/\1/
N
s/^\(.*\) *\n *title . \(.*\)$/"\1" "\2 " \\/
s/.$//
s/"C.utf8" "C locale "//
/^\s*$/d
s/"//g
s/ /, /'} > locale_file

cat locale_file
