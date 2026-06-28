#include "memory.h"

//static uint8_t mem_table[MEMORY_MAX];

paddr_t alloc_pages(uint32_t n) {
    static paddr_t next_paddr = (paddr_t) __free_ram;
    paddr_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr > (paddr_t) __free_ram_end)
        PANIC("out of memory");

    memset((void *) paddr, 0, n * PAGE_SIZE);
    return paddr;
}
// {

// }

// void free_pages(paddr_t addr)   {

// } 待修改： 改为插空分配，增加释放功能