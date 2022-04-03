/**
 * @file main.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *  
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "main.h"

//paragraph 3.1 pandos-chapter3.pdf (pag 5-6)
//Nucleus Initialization

//1. dichiara queste variabili globali.
int process_count; //Process Count: numero di processi partiti ma non terminati
int soft_block_count; //Soft-block Count: numero di processi partiti e bloccati ma non terminati
//list_head ready_q; //Ready Queue: puntatore alla coda dei pcb che sono nello stato "ready"
pcb_PTR current_process; //Current Process: processo corrente in stato "ready"
int device_sem[49]; //Device Semaphores: 
list_head high_priority_q;// alta priorità
list_head low_priority_q;
passupvector_t *passupvector;

//paragraph 3.3 pando-chapter3.pdf
void uTLB_RefillHandler() { 
	setENTRYHI(0x80000000); 
	setENTRYLO(0x00000000); 
	TLBWR(); 
	LDST ((state_PTR) 0x0FFFF000);
}

extern void test(); 

void main(){
    //paragraph 3.1 pandos-chapter3.pdf (pag 5-6)
	//2. popolare il passupvector.
    //- set the Nucleus TLB_Refill event andler address
    passupvector->tlb_refll_handler = (memaddr) uTLB_RefillHandler;
    //- set the Stack Pointer for the nucleus TLB_Refill event handler
    passupvector->tlb_refill_stackPtr = 0x2000.1000;
    //- set the Nucleus exception handler address
    //all'indirizzo della vostra funzine del nucleo
	passupvector->exception_handler = (memaddr) exceptin_handler();
    //- set the Stack Pointer for the Nucleus exception handler
	passupvector->exception_stackPtr = 0x2000.1000;
	
    //3. inizializza le strutture di fase 1
    initPcbs();
    initSemd(); 

    //4. Inizializza tutte le variabili mantenute a 0
    process_count = 0;
    soft_block_count = 0;
    mkEmptyProcQ(&low_priority_q);
    mkEmptyProcQ(&high_priority_q);
    current_process = NULL;

    for (int i = 0; i<=49; i++){
        device_sem[i] = 0;
	}
    
	//5. Load Interval Timer with 100 milliseconds (vedi 3.6.3)
    LDIT(100000);

    //6. Instantiate a single low priority process
    pcb_PTR pcb = allocPcb(); //pcb
    pcb->p_prio = 0;
    process_count++;
	//this process needs to have:
    // - interrupts enabled
    // - processor Local Timer enabled
    // - kernel mode on 
    //(vedi paragraph 2.3 virtualsquare (pag 9))
    pcb->p_s.status = IMON | IEPON | TEBITON; //? 0b00001000000000001111111100000100;
	// - the SP (stack pointer) set to RAMTOP
	RAMTOP(pcb->p_s.gpr[26]);
    // - and its PC set to the address of test.
    pcb->p_s.pc_epc = (memaddr) test;
    //set the remaining pcb fields as follow:
	//- set all the Process Tree fields to NULL
    pcb->p_child = NULL;
    pcb->p_parent = NULL;
	pcb->p_sib = NULL;
	pcb->p_time = 0; //- accumulated time field to 0
	pcb->p_semAdd = NULL; //- semaforo bloccante a NULL
	pcb->p_supportStruct = NULL; //- Struttura di supporto a NULL
    insertProcQ(&low_priority_q, pcb);

    //7. Call the Scheduler
    scheduler();
}