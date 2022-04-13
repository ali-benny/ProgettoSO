/**
 * @file scheduler.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *   
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "scheduler.h"

//richiami delle variabili globali di main.c
extern list_head high_priority_q;
extern list_head low_priority_q;
extern int process_count;
extern int soft_block_count;
extern pcb_PTR current_process;


//paragraph 3.2 pandos-chapter3.pdf (pag 7-8)
//Scheduler
/**
 * Schedules the next process to run.
 *
 * @returns None
 */
void scheduler(){
    //- if the queue of high priority is not empty
    if (!list_empty(high_priority_q)){
        //1. Remove the pcb from the head of the high priority Ready Queue
        //and store the pointer to the pointer to the pcb in the Current Process field
        current_process = removeProcQ(&high_priority_q);
        setTIMER(5000); //! non c'è sul pdf ma ha senso farlo anche qui
        //2. Perform a Load Processor State (LDST) on the processor state
        //stored in pcb of the Current Process (p_s)
        LDST(&(current_process->p_s));
    //- otherwise
    }else if (!list_empty(low_priority_q)){
        //1. Remove the pcb from the head of the low priority Ready Queue
        //and store the pointer to the pcb in the Current Process field
        current_process = removeProcQ(&low_priority_q);
        //2. Load 5 milliseconds on the PLT (vedi 4.1.4)
        setTIMER(5000); 
        //3. Perform a Load Processor State (LDST) on the processor state
        //stored in pcb of the Current Process (p_s)
        LDST(&(current_process->p_s));
    }
    //if the Process Count is zero
    if (process_count == 0){
        //invoke the HALT BIOS service/instructions (vedi 7.3.7)
        HALT();
    //if the Process Count > 0 and the Soft-block Count > 0
    }else if (process_count>0 && soft_block_count>0){
        //enter a Wait State (vedi 7.2.2)
        setSTATUS(IMON);
        WAIT();
    //Deadlock for Pandos is defined as when
    //the Process Count > 0 and the Soft-block Count is zero
    }else if (process_count>0 && soft_block_count==0){
        //invoke the PANIC BIOS service/instruction. (vedi 7.3.6)
        PANIC();
    }

}