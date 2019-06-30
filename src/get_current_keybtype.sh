keymap=$(grep "^[[:space:]]*\/usr\/bin\/loadkeys.*" /etc/rc.d/rc.keymap | sed -e 's/^.*loadkeys -u *\(.*\)$/\1/')
backtohome=$(pwd)
cd /usr/share/kbd/keymaps/i386
keytab="$(ls */* \
		| egrep "azerty|qwerty|dvorak|qwertz|olpc" \
		| grep "\/$keymap" \
		| sed 's/\/.*//')" 
cd $backtohome		
echo $keytab


