#!/bin/sh
ORIGIN=$PWD
cd ../../MinGW-Release
make clean
make all -j4
cd $ORIGIN
makensis -DVERSION_STRING=$1 raceCapturePro.nsi
