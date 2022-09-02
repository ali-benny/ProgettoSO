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
extern int mutex_asid;

int sup_dev_sem[48];
int master_sem;     // master semaphore

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
void initialize_uproc() {
    klog_print("init_uproc\n");
    //stato
    static state_t uproc_state[UPROCMAX];
    //struttura di supporto
    static support_t uproc_support[UPROCMAX];
    for (int i = 0; i < UPROCMAX; i++){        
        //STATO

        //EntryHi.ASID set to the process’s unique ID; an integer from [1..8]
        // - il registro entry_hi deve contenere l’ASID
        uproc_state[i].entry_hi = (i + 1) << ASIDSHIFT;

        //PC (and s_t9) set to 0x8000.00B0; the address of the start of the .text section
        uproc_state[i].pc_epc = UPROCSTARTADDR;
        uproc_state[i].reg_t9 = UPROCSTARTADDR;

        //SP set to 0xC000.0000 [Section 4.2]
        uproc_state[i].reg_sp = USERSTACKTOP;

        //Status set for user-mode with all interrupts and the processor Local Timer enabled.
        // - User mode e interrupt abilitati
        uproc_state[i].status =  IMON | IEPON | TEBITON | USERPON;

        //STRUTTURA DI SUPPORTO

        //sup_asid: The process’s ASID.
        uproc_support[i].sup_asid = i + 1;
        
        //sup_exceptContext[2]: The two processor context (context t) sets.
        //Each context is a PC/SP/Status combination. These are the two
        //processor contexts which the Nucleus uses for passing up exception
        //handling to the Support Level.
        uproc_support[i].sup_exceptContext[PGFAULTEXCEPT].pc = (memaddr) pager;
        uproc_support[i].sup_exceptContext[GENERALEXCEPT].pc = (memaddr) support_exception_handler;
        uproc_support[i].sup_exceptContext[PGFAULTEXCEPT].status = IMON | IEPON | TEBITON;
        uproc_support[i].sup_exceptContext[GENERALEXCEPT].status = IMON | IEPON | TEBITON;
        //      Set the two SP fields to utilize the two stack spaces allocated in the
        //      Support Structure. Stacks grow “down” so set the SP fields to the
        //      address of the end of these areas. e.g. . . . = &(. . .sup stackGen[499])
        uproc_support[i].sup_exceptContext[PGFAULTEXCEPT].stackPtr = (memaddr) &(uproc_support[i].sup_stackTLB[499]);
        uproc_support[i].sup_exceptContext[GENERALEXCEPT].stackPtr = (memaddr) &(uproc_support[i].sup_stackGen[499]);

        //sup_privatePgTbl[32]: The process’s Page Table.
        for(int j = 0; j < USERPGTBLSIZE - 1; j++){
            uproc_support[i].sup_privatePgTbl[j].pte_entryHI = (( i + 1) << ASIDSHIFT) + (PRESENTFLAG) + (i << VPNSHIFT);
            //all 0 except bit "N non used", "D, V e G devono avere valori fissi 1, 0 e 0".
            uproc_support[i].sup_privatePgTbl[j].pte_entryLO = 0b00000000000000000000010000000000;
        }
        uproc_support[i].sup_privatePgTbl[USERPGTBLSIZE -1].pte_entryHI = 0xBFFFF000 + ((i + 1) << ASIDSHIFT);
        uproc_support[i].sup_privatePgTbl[USERPGTBLSIZE -1].pte_entryLO = 0b00000000000000000000010000000000;
    }
    //Initialize and launch (NSYS1) between 1 and 8 U-procs.
    for (int i = 0; i < UPROCMAX; i++){
        SYSCALL(CREATEPROCESS, (memaddr) &uproc_state[i], PROCESS_PRIO_LOW, (memaddr) &uproc_support[i]);   //! prio cosa mettiano?
        //PROCESS_PRIO_LOW scelta a caso giusto per.
    }
    //TODO
}

/**
 * 
 */
void initSwapStructs(){
    klog_print("initSwapStructs\n");
    //inizializzo il valore del semaforo binario per la swap pool
    //pandosplus_phase3.pdf
    //par 4.4.1 The Swap Pool
    //Since this semaphore is used for mutual exclusion, it should be initialized to one.
    swap_pool_sem = 1;
    mutex_asid = -1;
    //per ogni pagina della swap_pool
    for(int i = 0; i < POOLSIZE; i++){
        //la inizialmente non è di nessun processo (è -1)
        swap_pool[i].sw_asid = NOPROC;
    }
    //* done *
}

/**
 * Test function
 * 
 * @brief 
 * pandosplus_phase3.pdf
 * par 4.9 Process Initialization and test
 */
void test(){
    klog_print("test\n");
    master_sem = 0;
    //Initialize the Level 4/Phase 3 data structures. These are:
    //– The Swap Pool table and Swap Pool semaphore
	initSwapStructs();

    // semaphore of peripherical I/O device
    for (int i = 0; i <= 48; i++){
        sup_dev_sem[i] = 1;
    }

    //for each asid we have to do
    initialize_uproc();

    for (int i = 0; i < UPROCMAX; i++){
        SYSCALL(PASSEREN, (memaddr) &master_sem, 0, 0);
    }
}
