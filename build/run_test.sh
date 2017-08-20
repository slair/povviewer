#!/bin/sh
#-*- coding: utf-8 -*-

# Exit the script on errors:
set -e
trap 'echo "$0 FAILED at line ${LINENO}"' ERR
# Catch unitialized variables:
set -u

rm scene.dump test_obj.png test.png || true

time povdump +itest.pov +w160 +h120 +nr +dfscene.dump
time povdump +itest.pov +w160 +h120
./pov_scene.py
