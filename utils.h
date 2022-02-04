#ifndef UTILS_H
#define UTILS_H

#include "pandos_const.h"
#include "pandos_types.h"
#include "listx.h"

#include <stdio.h>

pcb_t pcbFree_table[MAXPROC];
struct list_head *pcbFree_h = NULL;
semd_t semd_table[MAXPROC];
struct list_head* semdFree_h = NULL; //lista di semafori liberi
struct list_head* asl_h = NULL;	 //lista di semafori attivi, utilizzati in questo momento

#endif