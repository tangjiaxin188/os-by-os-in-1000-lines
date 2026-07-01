#ifndef USER_H
#define USER_H

__attribute__((noreturn))
void exit(void);

__attribute__((section(".text.start")))
__attribute__((naked))
void start(void);

void putchar(char ch);
int getchar(void);

int readfile(const char *filename, char *buf, int len);
int writefile(const char *filename, const char *buf, int len);

void poweroff(void);

int random();

int syscall(int sysno, int arg0, int arg1, int arg2);

#include "common.h"

#endif