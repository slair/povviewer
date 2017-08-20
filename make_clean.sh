#!/bin/sh

. ./config.sh

echo -e "Removing '${org_dir}'"
rm -rf ${org_dir}

echo -e "Removing 'build/${povdump_dir}'"
rm -rf build/${povdump_dir}

cd povviewer37
make clean
