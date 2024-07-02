#!/usr/bin/env bash
# Bash "strict mode"
set -o errexit
set -o nounset
set -o pipefail

TOOLCHAIN_FILE=gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2

mkdir -p /opt/gcc-arm
curl -o /opt/gcc-arm/"$TOOLCHAIN_FILE" https://downloads.myriota.com/"$TOOLCHAIN_FILE"
tar -xjvf /opt/gcc-arm/"$TOOLCHAIN_FILE" -C /opt/gcc-arm --strip-components=1
rm /opt/gcc-arm/"$TOOLCHAIN_FILE"
