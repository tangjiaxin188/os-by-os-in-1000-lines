#!/bin/bash
set -xue
#打包disk文件目录
(cd disk && tar cf ../disk.tar --format=ustar *.txt)
# QEMU 文件路径
QEMU=qemu-system-riscv32
# 启动 QEMU
#!!! 定义一个名为 drive0 的磁盘，使用 cipan.txt 作为磁盘镜像。磁盘镜像格式为 raw(将文件内容按原样作为磁盘数据处理)。!!!旧注释
# 添加一个带有 drive0 磁盘的 virtio-blk 设备。bus=virtio-mmio-bus.0 将设备映射到 virtio-mmio 总线(通过内存映射 I/O 的 virtio)。
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
-drive id=drive0,file=disk.tar,format=raw,if=none \
-device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
-kernel kernel.elf