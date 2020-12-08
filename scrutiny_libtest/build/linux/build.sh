#!/bin/sh

rm -f scrutinyLibTest
ARCH=`uname -m`
GCCFLAGS=-m64
#####For 64bit systems 'uname -m' returns x86_64, ia64 & ppc64. Defaulting to x86 if we don't get this value.
#####This is being done since for 32 bit systems the value can be i386, i586, i686 etc.

if [ "${ARCH}" != "x86_64" ] && [ "${ARCH}" != "ia64" ] && [ "${ARCH}" != "ppc64" ]; then
    ARCH="x86"
    GCCFLAGS=-m32
fi

echo Building Scrutiny Library Test Application for ${ARCH}
gcc  -pthread -g3 -Wall ${GCCFLAGS} ../../scrutinyTest.c -I../../../scrutiny_lib/api/ -DOS_LINUX -D_LINUX -DCLI_SUPPORT_SWITCH -DLIB_SUPPORT_SWITCH -DLIB_SUPPORT_EXPANDER -static -L../../../scrutiny_lib/build/linux/bin/x86_64/ -lscrutiny -o scrutinyLibTest
