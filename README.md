Scrutiny Library – Readme
Folder Structure:
-	../ScrutinyLib/scrutiny_drv 
o	Driver code, only for Atlas Base mode switches
-	../ScrutinyLib/scrutiny_lib
o	Scrutiny Library source code
-	../ScrutinyLib/scrutiny_libtest
o	Library test application source

Compilation Steps for Library and Test application:
1.	Make sure you have installed Linux development package
2.	Navigate to “../ScrutinyLib/”
3.	Change the permission by running “chmod +x compile.sh”
4.	To compile, execute “./compile.sh”
a.	will compile library first and later test application

If library has to be compiled separately, then follow below steps
1.	Navigate to “../ScrutinyLib/scrutiny_lib/build/linux”
2.	Execute
a.	make clean
b.	make LIB_SUPPORT_SWITCH=1 LIB_SUPPORT_EXPANDER=1
3.	Binary will be available in “../linux/bin/<arch>/”


Compilation Steps for SLIFF driver:
Driver compilation is needed only for Base mode switches. If synthetic mode then below driver compilation steps are not needed.
1.	Make sure you have installed Linux kernel development package
2.	Navigate to the driver directory
a.	“../ScrutinyLib/scrutiny_drv/sdrv/linux”
3.	Execute "make"
4.	Upon compilation, execute "insmod SliffDriver.ko"
5.	Make sure you have "/dev/SliffDriver" created
