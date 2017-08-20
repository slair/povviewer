#!/bin/sh

. ./config.sh

rm -rf $org_dir
./make_patch.sh
rm -rf $org_dir
cd build
./build_povdump.sh
#./run_test.sh
cd ..
cd povviewer37
make -s -j2
