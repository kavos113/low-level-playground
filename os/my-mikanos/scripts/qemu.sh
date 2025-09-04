#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: $0 <.efi file>"
    exit 1
fi

DISK_IMG="disk.img"

rm -f "$DISK_IMG"
qemu-img create -f raw "$DISK_IMG" 200M
mkfs.fat -n 'MY-OS' -s 2 -f 2 -R 32 -F 32 "$DISK_IMG"

mkdir -p mnt
sudo mount -o loop "$DISK_IMG" mnt
sudo mkdir -p mnt/EFI/BOOT
sudo cp "$1" mnt/EFI/BOOT/BOOTX64.EFI
sudo umount mnt

qemu-system-x86_64 \
    -drive if=pflash,file=$HOME/osbook/devenv/OVMF_CODE.fd \
    -drive if=pflash,file=$HOME/osbook/devenv/OVMF_VARS.fd \
    -monitor stdio \
    -hda "$DISK_IMG"