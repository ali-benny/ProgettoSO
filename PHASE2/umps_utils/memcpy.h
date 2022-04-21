#ifndef MEMCPY_H
#define MEMCPY_H

#include "../pandos_types.h"
#include "../pandos_const.h"

void memcpy(state_t *to, state_t *from);

//void memcpy(void* to, void* from, size_tt n);
//void *memcpy(void* dst, void* src, unsigned int cnt);
//void memcpy(void *dest, void *src, unsigned long n);

//void memcpy(pcb_t* current_process, state_t *state_reg); // nostra proposta


#endif //MEMCPY_H

