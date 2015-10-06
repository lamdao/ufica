#!/bin/sh
# Cross compile M$ Windows DLL using MinGW64 on Linux
CXX=x86_64-w64-mingw32-g++ WRC=x86_64-w64-mingw32-windres make $@

