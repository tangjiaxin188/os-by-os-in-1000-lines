#include "virtio.h"

// 1. 重构寄存器访问函数，支持传入 base 地址
uint32_t virtio_reg_read32(paddr_t base, unsigned offset) {
    return *((volatile uint32_t *) (base + offset));
}

uint64_t virtio_reg_read64(paddr_t base, unsigned offset) {
    return *((volatile uint64_t *) (base + offset));
}

void virtio_reg_write32(paddr_t base, unsigned offset, uint32_t value) {
    *((volatile uint32_t *) (base + offset)) = value;
}

void virtio_reg_fetch_and_or32(paddr_t base, unsigned offset, uint32_t value) {
    virtio_reg_write32(base, offset, virtio_reg_read32(base, offset) | value);
}

struct virtio_virtq *blk_request_vq;
struct virtio_blk_req *blk_req;
paddr_t blk_req_paddr;
uint64_t blk_capacity;

struct virtio_virtq *virtq_init(paddr_t base,unsigned index) {
    paddr_t virtq_paddr = alloc_pages(align_up(sizeof(struct virtio_virtq), PAGE_SIZE) / PAGE_SIZE);
    struct virtio_virtq *vq = (struct virtio_virtq *) virtq_paddr;
    vq->base = base;       // <--- 保存基地址
    vq->queue_index = index;
    vq->used_index = (volatile uint16_t *) &vq->used.index;
    // 选择队列：写入 virtqueue 索引（第一个队列为 0）
    virtio_reg_write32(base,VIRTIO_REG_QUEUE_SEL, index);
    // 指定队列大小：写入要使用的描述符数量
    virtio_reg_write32(base,VIRTIO_REG_QUEUE_NUM, VIRTQ_ENTRY_NUM);
    // 写入队列的页框编号（不是物理地址！）
    virtio_reg_write32(base,VIRTIO_REG_QUEUE_PFN, virtq_paddr / PAGE_SIZE);
    return vq;
}

void virtio_blk_init(void) {
    paddr_t base = VIRTIO_BLK_PADDR;
    if (virtio_reg_read32(base,VIRTIO_REG_MAGIC) != 0x74726976)
        PANIC("virtio-blk: invalid magic value");
    if (virtio_reg_read32(base,VIRTIO_REG_VERSION) != 1)
        PANIC("virtio-blk: invalid version");
    if (virtio_reg_read32(base,VIRTIO_REG_DEVICE_ID) != VIRTIO_DEVICE_BLK)
        PANIC("virtio-blk: invalid device id");

    // 1. 重置设备
    virtio_reg_write32(base,VIRTIO_REG_DEVICE_STATUS, 0);
    // 2. 设置 ACKNOWLEDGE 状态位：已发现设备
    virtio_reg_fetch_and_or32(base,VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_ACK);
    // 3. 设置 DRIVER 状态位：知道如何使用此设备
    virtio_reg_fetch_and_or32(base,VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER);
    // 设置页面大小：使用 4KB 页面。这用于 PFN（页框编号）的计算
    virtio_reg_write32(base,VIRTIO_REG_PAGE_SIZE, PAGE_SIZE);
    // 初始化磁盘读写请求用的队列
    blk_request_vq = virtq_init(base,0);
    // 6. 设置 DRIVER_OK 状态位：现在可以使用设备了
    virtio_reg_write32(base,VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER_OK);

    // 获取磁盘容量。
    blk_capacity = virtio_reg_read64(base,VIRTIO_REG_DEVICE_CONFIG + 0) * SECTOR_SIZE;
    printf("virtio-blk: capacity is %d bytes\n", (int)blk_capacity);

    // 分配一个区域来存储对设备的请求。
    blk_req_paddr = alloc_pages(align_up(sizeof(*blk_req), PAGE_SIZE) / PAGE_SIZE);
    blk_req = (struct virtio_blk_req *) blk_req_paddr;
}

// 通知设备有新的请求。`desc_index` 是新请求头描述符的索引。
void virtq_kick(struct virtio_virtq *vq, int desc_index) {
    vq->avail.ring[vq->avail.index % VIRTQ_ENTRY_NUM] = desc_index;
    vq->avail.index++;
    __sync_synchronize();
    virtio_reg_write32(vq->base,VIRTIO_REG_QUEUE_NOTIFY, vq->queue_index);
    vq->last_used_index++;
}

// 返回是否有请求正在被设备处理。
int virtq_is_busy(struct virtio_virtq *vq) {
    return vq->last_used_index != *vq->used_index;
}

struct virtio_virtq *rng_request_vq;

void virtio_rng_init(void) {
    paddr_t base = VIRTIO_RNG_PADDR;
    
    if (virtio_reg_read32(base, VIRTIO_REG_MAGIC) != 0x74726976)
        PANIC("virtio-rng: invalid magic value");
    if (virtio_reg_read32(base, VIRTIO_REG_VERSION) != 1)
        PANIC("virtio-rng: invalid version");
    if (virtio_reg_read32(base, VIRTIO_REG_DEVICE_ID) != VIRTIO_DEVICE_RNG)
        PANIC("virtio-rng: invalid device id");

    // 设备初始化流程与 blk 完全一致
    virtio_reg_write32(base, VIRTIO_REG_DEVICE_STATUS, 0);
    virtio_reg_fetch_and_or32(base, VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_ACK);
    virtio_reg_fetch_and_or32(base, VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER);
    virtio_reg_write32(base, VIRTIO_REG_PAGE_SIZE, PAGE_SIZE);
    
    // RNG 设备只有一个队列 (queue 0)
    rng_request_vq = virtq_init(base, 0);
    
    virtio_reg_write32(base, VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER_OK);
    printf("virtio-rng: initialized successfully\n");
}

// 获取随机数
void virtio_rng_get_random(void *buf, size_t len) {
    struct virtio_virtq *vq = rng_request_vq;
    
    // 如果后续引入了用户空间和非线性映射，这里需要改为 paddr_t buf_paddr = vaddr_to_paddr(buf);
    paddr_t buf_paddr = (paddr_t)buf; 

    // 【核心区别】：RNG 请求不需要 header 和 status，只需要一个描述符。
    // 标志位必须是 VIRTQ_DESC_F_WRITE，表示“设备向此内存写入数据”。
    vq->descs[0].addr = buf_paddr;
    vq->descs[0].len = len;
    vq->descs[0].flags = VIRTQ_DESC_F_WRITE; 
    vq->descs[0].next = 0; // 不需要 NEXT，因为只有一个描述符

    // 提交请求并等待完成
    virtq_kick(vq, 0);
    while (virtq_is_busy(vq))
        ;
}

// 从 virtio-blk 设备读取/写入。
void read_write_disk(void *buf, unsigned sector, int is_write) {
    if (sector >= blk_capacity / SECTOR_SIZE) {
        printf("virtio: tried to read/write sector=%d, but capacity is %d\n",
              sector, blk_capacity / SECTOR_SIZE);
        return;
    }

    // 根据 virtio-blk 规范构造请求。
    blk_req->sector = sector;
    blk_req->type = is_write ? VIRTIO_BLK_T_OUT : VIRTIO_BLK_T_IN;
    if (is_write)
        memcpy(blk_req->data, buf, SECTOR_SIZE);

    // 构造 virtqueue 描述符(使用 3 个描述符)。
    struct virtio_virtq *vq = blk_request_vq;
    vq->descs[0].addr = blk_req_paddr;
    vq->descs[0].len = sizeof(uint32_t) * 2 + sizeof(uint64_t);
    vq->descs[0].flags = VIRTQ_DESC_F_NEXT;
    vq->descs[0].next = 1;

    vq->descs[1].addr = blk_req_paddr + offsetof(struct virtio_blk_req, data);
    vq->descs[1].len = SECTOR_SIZE;
    vq->descs[1].flags = VIRTQ_DESC_F_NEXT | (is_write ? 0 : VIRTQ_DESC_F_WRITE);
    vq->descs[1].next = 2;

    vq->descs[2].addr = blk_req_paddr + offsetof(struct virtio_blk_req, status);
    vq->descs[2].len = sizeof(uint8_t);
    vq->descs[2].flags = VIRTQ_DESC_F_WRITE;

    // 通知设备有新的请求。
    virtq_kick(vq, 0);

    // 等待设备完成处理。
    while (virtq_is_busy(vq))
        ;

    // virtio-blk：如果返回非零值，则表示错误。
    if (blk_req->status != 0) {
        printf("virtio: warn: failed to read/write sector=%d status=%d\n",
               sector, blk_req->status);
        return;
    }

    // 对于读操作，将数据复制到缓冲区。
    if (!is_write)
        memcpy(buf, blk_req->data, SECTOR_SIZE);
}