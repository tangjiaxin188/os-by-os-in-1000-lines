# OS in 1000 lines (RISC-V)

[![OS](https://img.shields.io/badge/OS-RISC--V-blue)](https://github.com/your-username/your-repo)
[![Language](https://img.shields.io/badge/Language-C%20%2B%20Assembly-blueviolet)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Build](https://img.shields.io/badge/Build-Clang%20%2B%20LLD-brightgreen)](https://clang.llvm.org/)

本仓库是基于教程 **[OS in 1000 lines](https://1000os.seiya.me/zh/)** 的实践性操作系统内核实现。

**由衷感谢原教程作者 [@nuta](https://github.com/nuta) 提供的优质教学资源！**

## ✨ 仓库关键词
- **OS**: 从零构建的 32 位 RISC-V 操作系统内核。
- **RISC-V**: 使用 `riscv32-unknown-elf` 目标架构。
- **Clang / LLVM**: 全程使用 `clang` + `lld` 进行编译和链接。
- **LinkScript**: 通过自定义链接器脚本 (`kernel.ld`, `user.ld`) 精确控制内存布局。
- **C / Assembly**: 核心代码使用 C 语言，任务切换等关键部分嵌入汇编。
- **Shell**: 将编译，运行封装为Shell脚本。

## 📖 已完成章节（基于原文目录）

| 章节 | 内容描述 | 状态 |
|:---:|:---|:---:|
| 00 | 简介 | ✅ |
| 01 | 入门 | ✅ |
| 02 | RISC-V 101 | ✅ |
| 03 | 总览 | ✅ |
| 04 | 引导 (Boot) | ✅ |
| 05 | Hello World! | ✅ |
| 06 | C 标准库 | ✅ |
| 07 | 内核恐慌 (Kernel Panic) | ✅ |
| 08 | 异常 (Exception) | ✅ |
| 09 | 内存分配 (Memory Allocation) | ✅ |
| 10 | 进程 (Process) | ✅ |
| 11 | 页表 (Page Table) | ✅ |
| 12 | 应用程序 (Application) | ✅ |
| 13 | 用户模式 (User Mode) | ✅ |
| 14 | 系统调用 (System Call) | ✅ |
| 15 | 磁盘 I/O (Disk I/O) | ✅ |
| 16 | 文件系统 (File System) | ✅ |
| 17 | 结语 (Epilogue) | ✅ |
| **扩展** | **网络 I/O (VirtIO Net)** | 🔧 (开发中) |

> **当前进度**: 已完成全书所有章节（00-17）的学习与核心代码实现，从裸机引导、内存管理、进程调度，到用户态程序加载、文件系统均已跑通。

## 🛠️ 构建与运行

### 环境准备
请确保已按照教程安装以下工具链：
- `clang` (>= 13.0) + `lld`
- `llvm-objcopy`
- `qemu-system-riscv32` (支持 riscv32)
- `OpenSBI` 固件 ( `opensbi-riscv32-generic-fw_dynamic.bin` ) [项目地址](https://github.com/riscv-software-src/opensbi)

### 编译
在项目根目录下执行：
```bash
./build.sh