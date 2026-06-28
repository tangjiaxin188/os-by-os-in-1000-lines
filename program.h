#ifndef PROGRAM_H
#define PROGRAM_H

#include "common.h"
#include "panic.h"
#include "page.h"

#define PROCS_MAX 8       // 最大进程数量

#define PROC_UNUSED   0   // 未使用的进程控制结构
#define PROC_RUNNABLE 1   // 可运行的进程

struct process {
    int pid;             // 进程 ID
    int state;           // 进程状态: PROC_UNUSED 或 PROC_RUNNABLE
    vaddr_t sp;          // 栈指针
    uint8_t stack[8192]; // 内核栈
    uint32_t *page_table;// 页表指针
};

__attribute__((naked)) void switch_context(uint32_t *prev_sp, uint32_t *next_sp);
struct process *create_process(const void *image, size_t image_size);
void yield(void);

extern struct process *current_proc; // 当前运行的进程
extern struct process *idle_proc;    // 空闲进程

extern char __kernel_base[];

// 应用程序镜像的基础虚拟地址。这需要与 `user.ld` 中定义的起始地址匹配。
#define USER_BASE 0x1000000
#define SSTATUS_SPIE (1 << 5)

#endif