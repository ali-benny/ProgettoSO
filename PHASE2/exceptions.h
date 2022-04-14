#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "pandos_const.h"
#include "pandos_types.h"
#include "listx.h"
#include "main.h"
#include <umps3/umps/libumps.h>
#include "klog/klog.h"
#include "umps_utils/memcpy.h"

void syscall_handler();

void exception_handler();
void interrupt_handler();
void passup_or_die(int type_of_exception);

#endif //EXCEPTIONS_H

