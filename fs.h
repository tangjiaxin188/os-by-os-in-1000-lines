#ifndef FS_H
#define FS_H

#define FILES_MAX      2
#define DISK_MAX_SIZE  align_up(sizeof(struct file) * FILES_MAX, SECTOR_SIZE)

#include "common.h"
#include "panic.h"
#include "virtio.h"

struct tar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char type;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
    char data[];      // 指向头部后面数据区域的数组
                      // (flexible array member)
} __attribute__((packed));

struct file {
    bool in_use;      // 表示此文件条目是否在使用中
    char name[100];   // 文件名
    char data[1024];  // 文件内容
    size_t size;      // 文件大小
};

void fs_init(void);
void fs_flush(void);

extern struct file files[FILES_MAX];
extern uint8_t disk[DISK_MAX_SIZE];

#endif