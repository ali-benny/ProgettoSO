/**
 * @file initProc.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 * @brief 
 * @version 0.1
 * @date 2022-08-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "initProc.h"

/* global variables */
extern int swap_pool_sem;      //swap pool semaphore
extern swap_t swap_pool[POOLSIZE];

/* le slide phase 3 dicono pag 37, 38, 39, 40
 * Inizializzazione dei processi *
Stato del processo utente:
- il registro entry_hi deve contenere l’ASID
- stack pointer = 0xC000.0000
- program counter = 0x0000.00B0
- User mode e interrupt abilitati

Struttura di supporto:
- Kernel mode e interrupt abilitati
- sup_exceptContext[0].pc = gestore dei
trap TLB (page fault)
- sup_exceptContext[1].pc = gestore delle
eccezioni generiche
- sup_exceptContext[0].stackPtr = ramTop -
(ASID * 4K * 2) + 4K
- sup_exceptContext[1].stackPtr = ramTop -
(ASID * 4K * 2)

Page table:
31 entry con Virtual Page Number a
partire da 0x80000 crescendo di un frame
alla volta + uno dedicato allo stack con
valore 0xBFFFF

* Note generali *
- Siccome il livello di supporto lavora con
interrupt abilitati non e’ garantito l’accesso
esclusivo alle strutture dati condivise:
serve un semaforo dedicato.

 */

/**
 * @brief Initialization of support structure
 * 
 */
void initialize_support_struct() {
    
}

/**
 * @brief Initialization of Swap Pool
 * 
 */
void initSwapStructs(){
    //inizializzo il valore del semaforo binario per la swap pool
    //pandosplus_phase3.pdf
    //par 4.4.1 The Swap Pool
    //Since this semaphore is used for mutual exclusion, it should be initialized to one.
    swap_pool_sem = 1;

    //per ogni pagina della swap_pool
    for(int i = 0; < POOLSIZE; i++){
        //la inizialmente non è di nessun processo (è -1)
        swap_pool[i].sw_asid = NOPROC;
    }
    //* done *
}

/**
 * @brief 
 * pandosplus_phase3.pdf
 * par 4.9 Process Initialization and test
 */
void test(){
	initSwapStructs();
	
}