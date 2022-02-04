#ifndef ASL_H
#define ASL_H

#include "listx.h"
//#include "pandos_const.h"
#include "pandos_types.h"
#include "pcb.h"

semd_t semd_table[MAXPROC];
struct list_head semdFree_h;
struct list_head semd_h;

int insertBlocked(int *semAdd, pcb_t *p);
pcb_t* removeBlocked(int *semAdd);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *semAdd);
void initASL();
#endif