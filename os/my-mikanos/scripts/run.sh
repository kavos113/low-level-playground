#!/bin/bash

EDK_DIR="$HOME/edk2"
SCRIPT_DIR=$(cd $(dirname $0) && pwd)

cd "$EDK_DIR"
source edksetup.sh
build

"$SCRIPT_DIR"/qemu.sh Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi