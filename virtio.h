#ifndef VIRTIO_H
#define VIRTIO_H

#include "common.h"
#include "memory.h"

/*

Virtio 设备具有一个称为 virtqueue 的结构。顾名思义，它是驱动程序和设备之间共享的队列。简而言之：

Virtqueue 由以下三个区域组成：
名称	            写入者	    内容	                        具体内容
Descriptor Table	驱动程序	描述符表：请求的地址和大小	    内存地址、长度、下一个描述符的索引
Available Ring	    驱动程序	向设备发送处理请求	            描述符链的头索引
Used Ring	        设备	    设备处理完成的请求	            描述符链的头索引

*/

/* 在写入磁盘时，virtqueue 将按以下方式使用：
        驱动程序在 Descriptor Table 写入读/写请求。
        驱动程序将头描述符的索引添加到 Available Ring。
        驱动程序通知设备有新的请求。
        设备从 Available Ring 读取请求并处理它。
        设备将描述符索引写入 Used Ring，并通知驱动程序完成。
*/

#define SECTOR_SIZE       512
#define VIRTQ_ENTRY_NUM   16
#define VIRTIO_DEVICE_BLK 2
#define VIRTIO_BLK_PADDR  0x10001000
#define VIRTIO_REG_MAGIC         0x00
#define VIRTIO_REG_VERSION       0x04
#define VIRTIO_REG_DEVICE_ID     0x08
#define VIRTIO_REG_PAGE_SIZE     0x28
#define VIRTIO_REG_QUEUE_SEL     0x30
#define VIRTIO_REG_QUEUE_NUM_MAX 0x34
#define VIRTIO_REG_QUEUE_NUM     0x38
#define VIRTIO_REG_QUEUE_PFN     0x40
#define VIRTIO_REG_QUEUE_READY   0x44
#define VIRTIO_REG_QUEUE_NOTIFY  0x50
#define VIRTIO_REG_DEVICE_STATUS 0x70
#define VIRTIO_REG_DEVICE_CONFIG 0x100
#define VIRTIO_STATUS_ACK       1
#define VIRTIO_STATUS_DRIVER    2
#define VIRTIO_STATUS_DRIVER_OK 4
#define VIRTQ_DESC_F_NEXT          1
#define VIRTQ_DESC_F_WRITE         2
#define VIRTQ_AVAIL_F_NO_INTERRUPT 1
#define VIRTIO_BLK_T_IN  0
#define VIRTIO_BLK_T_OUT 1
#define DISK_READ 0
#define DISK_WRITE 1

// Virtqueue Descriptor Table entry.
struct virtq_desc {
    uint64_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
} __attribute__((packed));

// Virtqueue Available Ring。
struct virtq_avail {
    uint16_t flags;
    uint16_t index;
    uint16_t ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));

// Virtqueue Used Ring 条目。
struct virtq_used_elem {
    uint32_t id;
    uint32_t len;
} __attribute__((packed));

// Virtqueue Used Ring。
struct virtq_used {
    uint16_t flags;
    uint16_t index;
    struct virtq_used_elem ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));

// Virtqueue。
struct virtio_virtq {
    struct virtq_desc descs[VIRTQ_ENTRY_NUM];
    struct virtq_avail avail;
    struct virtq_used used __attribute__((aligned(PAGE_SIZE)));
    int queue_index;
    volatile uint16_t *used_index;
    uint16_t last_used_index;
} __attribute__((packed));

// Virtio-blk 请求。
struct virtio_blk_req {
    uint32_t type;
    uint32_t reserved;
    uint64_t sector;
    uint8_t data[512];
    uint8_t status;
} __attribute__((packed));


// uint32_t virtio_reg_read32(unsigned offset);
// uint64_t virtio_reg_read64(unsigned offset);
// void virtio_reg_write32(unsigned offset, uint32_t value);
// void virtio_reg_fetch_and_or32(unsigned offset, uint32_t value);

void virtio_blk_init(void);
void read_write_disk(void *buf, unsigned sector, int is_write);

#endif