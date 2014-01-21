#!/bin/bash

if [ $# -eq 0 ] ; then
	PARAM="NOT"
else
	PARAM="$1"
fi

echo Czysci projekt

find . -type f -name cmake_install.cmake -print -exec rm -rf {} \;
find . -type f -name CMakeCache.txt -exec rm -f {} \;
find . -type d -name 'CMakeFiles*' -exec rm -rf {} \;
rm -rf ./bin

if [ $PARAM = "ALL" ] ; then
	echo Clean everything ...
	rm -rf CMake_trash
	rm -rf doc/html
	find . -type f -name Makefile -print -exec rm -rf {} \;
	find ./Third_Party/* -type d -name '*' -exec rm -rf {} \;
fi


