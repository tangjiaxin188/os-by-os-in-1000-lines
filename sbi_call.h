#ifndef SBI_CALL_H
#define SBI_CALL_H
#include "kernel.h"

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid);

#endif
