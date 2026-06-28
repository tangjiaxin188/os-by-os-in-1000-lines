#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"
#include "stage.h"
#include "panic.h"

#define MEMORY_MAX 2048
paddr_t alloc_pages(uint32_t n);

#endif