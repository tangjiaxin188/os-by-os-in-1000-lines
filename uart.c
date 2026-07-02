#include "uart.h"

// 【关键修正】：使用 uint8_t (8位) 进行访问，匹配 1 字节对齐的硬件
#define UART_REG(offset) (*(volatile uint8_t *)(UART0_PADDR + (offset)))

void uart_init(void) {
    // 1. 禁用所有中断
    UART_REG(UART_IER) = 0x00;

    // 2. 设置 DLAB = 1，准备配置波特率 (参考 Zig 代码)
    UART_REG(UART_LCR) = UART_LCR_DLAB; // 0x80

    // 3. 设置波特率除数 (假设时钟频率下，0x0003 对应 115200 或 9600，具体取决于 QEMU/硬件)
    UART_REG(UART_DLL) = 0x03; // 低 8 位
    UART_REG(UART_DLM) = 0x00; // 高 8 位

    // 4. 清除 DLAB，设置 8 数据位, 无奇偶校验, 1 停止位 (8N1)
    UART_REG(UART_LCR) = 0x03;

    // 5. 启用 FIFO，清空收发队列，设置触发级别为 1 字节
    // 【这就是之前崩溃的地方！现在偏移量是 2，在设备范围内，不会触发总线错误了！】
    UART_REG(UART_FCR) = 0x07;

    // 6. 启用 DTR 和 RTS (可选)
    UART_REG(UART_MCR) = 0x03;
}

// 轮询方式发送一个字符 (参考 Zig 代码的 putchar)
void uart_putc(char c) {
    // 等待发送缓冲区为空 (THRE 位为 1)
    while ((UART_REG(UART_LSR) & UART_LSR_THRE) == 0) {
        // Busy-wait
    }
    
    // 写入字符到 THR
    UART_REG(UART_THR) = (uint8_t)c;
}

// 轮询方式接收一个字符
char uart_getc(void) {
    // 等待接收缓冲区有数据 (DR 位为 1)
    while ((UART_REG(UART_LSR) & UART_LSR_DR) == 0) {
        // Busy-wait
    }
    
    // 从 RHR 读取字符
    return (char)UART_REG(UART_RHR);
}