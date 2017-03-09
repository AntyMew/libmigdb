#!/bin/sh
gcc -o version version.c
cd ..
tar jcvf libmigdb-`libmigdb/version`.tar.bz2 `cat libmigdb/files`
