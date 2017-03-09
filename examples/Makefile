#!/usr/bin/make

all: test_target x11_test remote_test linux_test target_frames x11_fr_test \
	x11_wp_test x11_cpp_test pty_test

CFLAGS=-O0 -Wall -gstabs+3 -I../src
CXXFLAGS=-O0 -Wall -gstabs+3 -I../src
LDLIBS=

# fpgacores/PIC16C84/soft/icepic/icepic
ticepic: ticepic.c ../src/libmigdb.a

x11_test: x11_test.c ../src/libmigdb.a

x11_cpp_test: x11_cpp_test.cc ../src/libmigdb.a

x11_fr_test: x11_fr_test.c ../src/libmigdb.a

x11_wp_test: x11_wp_test.c ../src/libmigdb.a

remote_test: remote_test.c ../src/libmigdb.a

linux_test: linux_test.c ../src/libmigdb.a

pty_test: pty_test.c ../src/libmigdb.a

clean:
	-@rm *.o *.a .*~ test_target x11_test remote_test linux_test 2> /dev/null
	-@rm x11_wp_test x11_cpp_test target_frames x11_fr_test 2> /dev/null
	-@rm pty_test ticepic 2> /dev/null


