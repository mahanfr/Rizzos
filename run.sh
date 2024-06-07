#!/bin/sh
set -xe

qemu-system-i386 -fda ./build/osmium.img
