#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "pandos_const.h"
#include "pandos_types.h"
#include "listx.h"
#include "main.h"

void syscall_handler(state_t* state);

void exception_handler();
void interrupt_handler();
void passup_or_die(int type_of_exception);

#endif //EXCEPTIONS_H
