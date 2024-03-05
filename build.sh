#!/bin/bash
make
cd ~/zeal/zeal8bit/ZealFS
rm zim_dbg.img
./zealfs --image=zim_dbg.img --size=64 zfs_mnt
cp ~/mediap/zimage/zim-main/lena/lena_128.bmp zfs_mnt/lena.bmp
cp ~/mediap/zimage/zim-main/bin/zimage.bin zfs_mnt/
