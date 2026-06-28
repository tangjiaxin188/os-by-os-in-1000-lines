#ifndef PAGE_H
#define PAGE_H

#include "memory.h"

#define SATP_SV32 (1u << 31)
#define PAGE_V    (1 << 0)   // "Valid" 位（表项已启用）
#define PAGE_R    (1 << 1)   // 可读
#define PAGE_W    (1 << 2)   // 可写
#define PAGE_X    (1 << 3)   // 可执行
#define PAGE_U    (1 << 4)   // 用户（用户模式可访问）

void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags);

#endif