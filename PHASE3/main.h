#ifndef MAIN_H
#define MAIN_H

#include "syscall.h"
#include "scheduler.h"
#include "exceptions.h"
#include "asl.h"
#include "pcb.h"
#include <umps3/umps/types.h>
#include <umps3/umps/const.h>
#include <umps3/umps/libumps.h>
#include "klog/klog.h"

//void uTLB_RefillHandler();
int main();

#endif // MAIN_H

