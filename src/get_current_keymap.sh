grep '^[[:space:]]*\/usr\/bin\/loadkeys.*' /etc/rc.d/rc.keymap | sed -e 's/^.*loadkeys -u *\(.*\)$/\1/'| sed 's/.map//'> currentkeymap
cat currentkeymap
