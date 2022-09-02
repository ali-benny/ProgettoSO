#ifndef TLB
#define TLB

#include "syscall.h"
#include "sysSupport.h"
#include "klog/klog.h"

void uTLB_RefillHandler();
void pager();

#endif //TLB
