#!/bin/sh

set -ue

. ./qemu.sh

arm-none-eabi-gcc -o hello.elf hello.c -g -mcpu=cortex-m3 -mthumb -T generic-m-hosted.ld -std=c99

echo
echo "============================================="
echo "Running the open file test"
echo "============================================="
qemu-system-arm  -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel hello.elf
