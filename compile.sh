#!/bin/sh

ARCH=`uname -m`

#Compiling the library source
echo "-----------------------------------------------"
echo Compiling ScrutinyLib source for ${ARCH}
echo "-----------------------------------------------"

cd scrutiny_lib/build/linux/
make clean
make LIB_SUPPORT_SWITCH=1 LIB_SUPPORT_EXPANDER=1

if [ "$?" -ne 0 ]; then
	echo "-----------------------------------------------"
	echo "ScrutinyLib compilation failed"
	echo "-----------------------------------------------"
	exit   
fi
echo
echo "-----------------------------------------------"
echo "ScrutinyLib compilation Successful"
echo "-----------------------------------------------"

cd ../../../scrutiny_libtest/build/linux/
rm -f scrutinyLibTest

echo
echo "-------------------------------------------------------"
echo Compiling Scrutiny Library Test Application for ${ARCH}
echo "-------------------------------------------------------"

chmod +x build.sh

./build.sh

if [ "$?" -ne 0 ]; then
	echo "------------------------------------------------------"
	echo "Scrutiny Library Test Application compilation failed"
	echo "------------------------------------------------------"
	exit   
fi
echo "----------------------------------------------------------"
echo "Scrutiny Library Test Application compilation Successful"
echo "----------------------------------------------------------"
