#ifndef SBI_CALL_H
#define SBI_CALL_H
#include "kernel.h"

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid);


#define SBI_EXT_SRST               0x53525354
#define SBI_SRST_RESET             0
#define SBI_SRST_RESET_TYPE_SHUTDOWN   0
#define SBI_SRST_RESET_REASON_NONE      0

#endif
