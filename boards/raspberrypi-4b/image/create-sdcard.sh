#!/bin/sh
set -eu

output_dir=$1
u_boot=$2
initramfs=$3
kernel=$4
device_tree=$5
config=$6
extlinux=$7
start4=$8
fixup4=$9
license=${10}

rm -rf "$output_dir"
mkdir -p "$output_dir"
install -m 0644 "$u_boot" "$output_dir/u-boot.bin"
install -m 0644 "$initramfs" "$output_dir/initramfs.cpio.gz"
install -m 0644 "$kernel" "$output_dir/zImage"
install -m 0644 "$device_tree" "$output_dir/bcm2711-rpi-4-b.dtb"
install -m 0644 "$config" "$output_dir/config.txt"
install -m 0644 "$extlinux" "$output_dir/extlinux.conf"
install -m 0644 "$start4" "$output_dir/start4.elf"
install -m 0644 "$fixup4" "$output_dir/fixup4.dat"
install -m 0644 "$license" "$output_dir/LICENCE.broadcom"

payload_bytes=0
for file in "$output_dir"/u-boot.bin \
            "$output_dir"/initramfs.cpio.gz \
            "$output_dir"/zImage \
            "$output_dir"/bcm2711-rpi-4-b.dtb \
            "$output_dir"/config.txt \
            "$output_dir"/extlinux.conf \
            "$output_dir"/start4.elf \
            "$output_dir"/fixup4.dat \
            "$output_dir"/LICENCE.broadcom; do
    size=$(stat -c %s "$file")
    payload_bytes=$((payload_bytes + size))
done

fat_size_mib=$(( (payload_bytes * 5 / 4 + 32 * 1024 * 1024 + 1024 * 1024 - 1) / (1024 * 1024) ))
if [ "$fat_size_mib" -lt 128 ]; then
    fat_size_mib=128
fi

bootfs="$output_dir/bootfs.fat"
truncate -s "${fat_size_mib}M" "$bootfs"
mkfs.vfat -F 32 -n boot "$bootfs"
mmd -i "$bootfs" ::/extlinux
mcopy -i "$bootfs" "$output_dir/u-boot.bin" ::/u-boot.bin
mcopy -i "$bootfs" "$output_dir/initramfs.cpio.gz" ::/initramfs.cpio.gz
mcopy -i "$bootfs" "$output_dir/zImage" ::/zImage
mcopy -i "$bootfs" "$output_dir/bcm2711-rpi-4-b.dtb" ::/bcm2711-rpi-4-b.dtb
mcopy -i "$bootfs" "$output_dir/config.txt" ::/config.txt
mcopy -i "$bootfs" "$output_dir/extlinux.conf" ::/extlinux/extlinux.conf
mcopy -i "$bootfs" "$output_dir/start4.elf" ::/start4.elf
mcopy -i "$bootfs" "$output_dir/fixup4.dat" ::/fixup4.dat
mcopy -i "$bootfs" "$output_dir/LICENCE.broadcom" ::/LICENCE.broadcom

fat_sectors=$((fat_size_mib * 2048))
image_sectors=$((2048 + fat_sectors + 2048))
image="$output_dir/sdcard.img"
truncate -s $((image_sectors * 512)) "$image"
printf 'label: dos\nunit: sectors\n\nstart=2048, size=%s, type=c, bootable\n' "$fat_sectors" | \
    sfdisk "$image"
dd if="$bootfs" of="$image" bs=512 seek=2048 conv=notrunc status=none
