#ifndef UART_H
#define UART_H

#include "common.h"

// UART 物理基地址 (请确认你的 constants.UART_BASE 是多少，通常是 0x10000000)
#define UART0_PADDR 0x10000000 

// 【关键修正】：使用 1 字节对齐的偏移量 (Reg-Shift = 0)
#define UART_RHR 0  // Receive Holding Register (Read)
#define UART_THR 0  // Transmit Holding Register (Write)
#define UART_IER 1  // Interrupt Enable Register
#define UART_FCR 2  // FIFO Control Register
#define UART_LCR 3  // Line Control Register
#define UART_MCR 4  // Modem Control Register
#define UART_LSR 5  // Line Status Register
#define UART_MSR 6  // Modem Status Register
#define UART_SPR 7  // Scratch Pad Register

// 波特率除数锁存器 (当 LCR 的 DLAB 位为 1 时)
#define UART_DLL 0  // Divisor Latch Low
#define UART_DLM 1  // Divisor Latch High

// LSR 标志位
#define UART_LSR_DR   0x01  // Data Ready
#define UART_LSR_THRE 0x20  // Transmit Holding Register Empty

// LCR 标志位
#define UART_LCR_DLAB 0x80  // Divisor Latch Access Bit

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char *s);

#endif