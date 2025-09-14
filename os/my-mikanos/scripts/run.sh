#!/bin/bash

EDK_DIR="$HOME/edk2"
SCRIPT_DIR=$(cd $(dirname $0) && pwd)
OPTION=$1

set --

cd "$EDK_DIR"
source edksetup.sh
if build; then
    echo "---------------"
    echo "Build succeeded."
    echo "---------------"
else
    echo "---------------"
    echo "Build failed."
    echo "---------------"
    exit 1
fi

if [ "$OPTION" == "qemu" ]; then
    "$SCRIPT_DIR"/qemu.sh Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi "$SCRIPT_DIR"/../kernel/kernel.elf
elif [ "$OPTION" != "" ]; then
    "$SCRIPT_DIR"/usb.sh usbm $OPTION Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi "$SCRIPT_DIR"/../kernel/kernel.elf
fi
