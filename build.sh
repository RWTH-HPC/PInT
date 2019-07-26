#!/bin/sh

rm -r buildTool
mkdir buildTool
cd buildTool
cmake ..
make -j
