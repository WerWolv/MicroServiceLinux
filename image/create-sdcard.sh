#!/bin/sh
set -eu

output_dir=$1
spl=$2
u_boot=$3
bootfs=$4
initramfs=$5
kernel=$6
device_tree=$7

mkdir -p "$output_dir"
install -m 0644 "$spl" "$output_dir/u-boot-spl.stm32"
install -m 0644 "$u_boot" "$output_dir/u-boot.img"
install -m 0644 "$bootfs" "$output_dir/bootfs.ext4"
install -m 0644 "$initramfs" "$output_dir/initramfs.cpio"
install -m 0644 "$kernel" "$output_dir/zImage"
install -m 0644 "$device_tree" "$output_dir/stm32mp157c-dk2.dtb"

bootfs_sectors=$((128 * 1024 * 1024 / 512))
image_sectors=$((5154 + bootfs_sectors + 33))
image="$output_dir/sdcard.img"
rm -f "$image"
truncate -s $((image_sectors * 512)) "$image"

sgdisk --clear --resize-table=128 -a 1 \
    -n 1:34:545 -c 1:fsbl1 \
    -n 2:546:1057 -c 2:fsbl2 \
    -n 3:1058:5153 -c 3:ssbl \
    -n 4:5154:-34 -c 4:rootfs -A 4:set:2 \
    "$image"

dd if="$spl" of="$image" bs=512 seek=34 conv=notrunc status=none
dd if="$spl" of="$image" bs=512 seek=546 conv=notrunc status=none
dd if="$u_boot" of="$image" bs=512 seek=1058 conv=notrunc status=none
dd if="$bootfs" of="$image" bs=512 seek=5154 conv=notrunc status=none
