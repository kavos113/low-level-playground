#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: $0 <mnt name> <mnt drive> <.efi file> [<file to copy to root of disk image>]"
    exit 1
fi

MNT_DIR=/mnt/$1
MNT_DRIVE=$2
EFI_FILE=$3
MNT_FILE=$4

sudo mkdir -p $MNT_DIR
sudo mount -t drvfs $MNT_DRIVE $MNT_DIR
sudo mkdir -p $MNT_DIR/EFI/BOOT
sudo cp $EFI_FILE $MNT_DIR/EFI/BOOT/BOOTX64.EFI
if [ "$MNT_FILE" != "" ]; then
    sudo cp $MNT_FILE $MNT_DIR
fi
sudo umount $MNT_DIR
