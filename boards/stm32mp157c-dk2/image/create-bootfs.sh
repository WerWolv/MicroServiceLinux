#!/bin/sh
set -eu

staging=$1
image=$2
initramfs=$3
kernel=$4
device_tree=$5
extlinux=$6

rm -rf "$staging"
mkdir -p "$staging/boot/extlinux"
install -m 0644 "$initramfs" "$staging/boot/initramfs.cpio.gz"
install -m 0644 "$kernel" "$staging/boot/zImage"
install -m 0644 "$device_tree" "$staging/boot/stm32mp157c-dk2.dtb"
install -m 0644 "$extlinux" "$staging/boot/extlinux/extlinux.conf"
chown -R 0:0 "$staging"

rm -f "$image"
set -- $(du -sk "$staging")
payload_kib=$1
image_size_mib=$(( (payload_kib * 5 / 4 + 32768 + 1023) / 1024 ))
if [ "$image_size_mib" -lt 128 ]; then
    image_size_mib=128
fi
mke2fs -q -F -t ext4 -L boot -U 00000000-0000-0000-0000-000000000001 \
    -d "$staging" -E root_owner=0:0,lazy_itable_init=0,lazy_journal_init=0 \
    "$image" "${image_size_mib}M"
