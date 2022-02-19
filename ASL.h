#ifndef ASL_H
#define ASL_H

//#include "listx.h"
//#include "pandos_const.h"
//#include "pandos_types.h"

#include "pcb.h"

//extern pcb_t pcbFree_table[MAXPROC];
//extern struct list_head *pcbFree_h;

HIDDEN semd_t semd_table[MAXPROC];
HIDDEN struct list_head* semdFree_h; //lista di semafori liberi
HIDDEN struct list_head* asl_h;	 //lista di semafori attivi, utilizzati in questo momento

int insertBlocked(int *semAdd, pcb_t *p);
pcb_t* removeBlocked(int *semAdd);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *semAdd);
void initASL();
//void mainSemafori();


#endif //ASL_H
