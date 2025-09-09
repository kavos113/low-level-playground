#!/bin/bash

EDK_DIR="$HOME/edk2"
SCRIPT_DIR=$(cd $(dirname $0) && pwd)

cd "$EDK_DIR"
source edksetup.sh
if build; then
    echo "---------------"
    echo "Build succeeded."
    echo "---------------"
  "$SCRIPT_DIR"/qemu.sh Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi "$SCRIPT_DIR"/../kernel/kernel.elf
else
    echo "---------------"
    echo "Build failed."
    echo "---------------"
    exit 1
fi

