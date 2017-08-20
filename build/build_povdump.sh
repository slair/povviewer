#!/bin/sh
#-*- coding: utf-8 -*-

# Exit the script on errors:
set -e
trap 'echo "$0 FAILED at line ${LINENO}"' ERR
# Catch unitialized variables:
set -u

. ../config.sh

msg "Building povdump..."

if ! [ -f ${src_bundle} ]; then
	msg "Downloading ${POVRAY_URL} to ${src_bundle}"
	wget -c -nv -T 20 -O ${src_bundle} ${POVRAY_URL}
fi

msg "Removing ${povdump_dir}"
rm -rf ${povdump_dir}

VERSION=${VERSION:-3.7.0.0}
set +u
if [ -z "$ARCH" ]; then
  case "$( uname -m )" in
    i?86) ARCH=i486 ;;
    arm*) ARCH=arm ;;
       *) ARCH=$( uname -m ) ;;
  esac
fi
set -u
# povray prefers -O3 to build, so we do not use the -O2 flag
if [ "$ARCH" = "i486" ]; then
  SLKCFLAGS="-march=i486 -mtune=i686"
  LIBDIRSUFFIX=""
elif [ "$ARCH" = "i686" ]; then
  SLKCFLAGS="-march=i686 -mtune=i686"
  LIBDIRSUFFIX=""
elif [ "$ARCH" = "x86_64" ]; then
  SLKCFLAGS="-fPIC"
  LIBDIRSUFFIX="64"
else
  SLKCFLAGS="-O2"
  LIBDIRSUFFIX=""
fi

if ! [ -d ${povdump_dir} ]; then
	msg "Unpacking ${src_bundle}"
	tar xf ${src_bundle}

	msg "Renaming ${org_dir} to ${povdump_dir}"
	mv ${org_dir} ${povdump_dir}

	msg "Patching with povdump.patch"
	patch -p0 < povdump.patch

	cd $povdump_dir
	set +e
	(
		cd unix
		sed 's/automake --w/automake --add-missing --w/g' -i prebuild.sh
		sh ./prebuild.sh
	)
	msg "Configuring..."
	CXXFLAGS="$SLKCFLAGS" \
	  ./configure \
	  --prefix=/usr \
	  --libdir=/usr/lib${LIBDIRSUFFIX} \
	  --sysconfdir=/etc \
	  --localstatedir=/var \
	  --mandir=/usr/man \
	  --build=$ARCH-slackware-linux \
	  LIBS="-lboost_system -lboost_thread" \
	  COMPILED_BY="${POVRAY_COMPILED_BY}"

	msg "Making..."
	grc make -s -j2
	cd unix
	mv povray povdump
fi

if [ $? != 0 ]; then
	DISPLAY=:0 notify-send -u critical -t 10000 "PovDump" "Build failed!!!"
	say "Сборка сломалась!"
else
	DISPLAY=:0 notify-send -u critical -t 10000 "PovDump" "Build complete!!!"
	say "Сборка успешно завершена"
fi
