#ifndef MAIN_H
#define MAIN_H

#include "syscall.h"
#include "scheduler.h"
#include "exceptions.h"
#include "asl.h"
#include "pcb.h"
#include "../utils/types.h"
#include <umps3/umps/const.h>

void uTLB_RefillHandler();
void main();

#endif // MAIN_H
