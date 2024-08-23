#!/usr/bin/env bash
# Bash "strict mode"
set -o errexit
set -o nounset
set -o pipefail

mkdir -p /opt/gcc-arm-13_2_1
curl -L -o /opt/gcc-arm-13_2_1/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz https://downloads.myriota.com/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
tar -xvf /opt/gcc-arm-13_2_1/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz -C /opt/gcc-arm-13_2_1 --strip-components=1
rm /opt/gcc-arm-13_2_1/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
