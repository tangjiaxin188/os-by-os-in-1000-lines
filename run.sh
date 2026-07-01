#!/bin/bash
set -xue
#打包disk文件目录
(cd disk && tar cf ../disk.tar --format=ustar *.txt)
# QEMU 文件路径
QEMU=qemu-system-riscv32
# 启动 QEMU
#!!! 定义一个名为 drive0 的磁盘，使用 cipan.txt 作为磁盘镜像。磁盘镜像格式为 raw(将文件内容按原样作为磁盘数据处理)。!!!旧注释
# 添加一个带有 drive0 磁盘的 virtio-blk 设备。bus=virtio-mmio-bus.0 将设备映射到 virtio-mmio 总线(通过内存映射 I/O 的 virtio)。
# 添加一个熵源，并给新设备virtio-rng使用，并且将设备映射到mmio中
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
-object rng-random,filename=/dev/urandom,id=rng0 \
-drive id=drive0,file=disk.tar,format=raw,if=none \
-device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
-device virtio-rng-device,rng=rng0,bus=virtio-mmio-bus.1 \
-kernel kernel.elf