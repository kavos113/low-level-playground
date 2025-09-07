#!/bin/bash

EDK_DIR="$HOME/edk2"
SCRIPT_DIR=$(cd $(dirname $0) && pwd)

cd "$EDK_DIR"
source edksetup.sh
if build; then
    echo "Build succeeded."
  "$SCRIPT_DIR"/qemu.sh Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi "$SCRIPT_DIR"/../kernel/kernel.elf
else
    echo "Build failed."
    exit 1
fi

