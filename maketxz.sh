#!/bin/sh

cd $(dirname $0)
mkdir -p pkg
export DESTDIR=$PWD/pkg
VER=1.0
ARCH=${ARCH:-x86_64}
RLZ=2dj

	cmake -DCMAKE_INSTALL_PREFIX=/usr .
		
	make || return 1
	make DESTDIR=pkg install || return 1

	cd pkg

	mkdir install

cat <<EOF > install/slack-desc
multibootusb: Multiboot USB (Tool to boot multi iso images)"
multibootusb: It is  written for Slackel / Slackel64 Linux:"
EOF

	/sbin/makepkg -l y -c n ../multibootusb-$VER-$ARCH-$RLZ.txz
	cd ..
rm -rf pkg
md5sum multibootusb-$VER-$ARCH-$RLZ.txz > multibootusb-$VER-$ARCH-$RLZ.md5
echo -e "aaa_elflibs|bzip2,aaa_elflibs|expat,aaa_elflibs|libpng,aaa_elflibs|zlib,atk,cairo,fontconfig,freetype,fribidi,gdk-pixbuf2,glib2,graphite2,gtk+2,harfbuzz,libX11,libXau,libXcomposite,libXcursor,libXdamage,libXdmcp,libXext,libXfixes,libXi,libXinerama,libXrandr,libXrender,libXxf86vm,libdrm,libffi,libxcb,libxshmfence,mesa,pango,pcre,pixman,util-linux" > multibootusb-$VER-$ARCH-$RLZ.dep
