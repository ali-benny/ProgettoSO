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
extern struct list_head high_priority_q;
extern struct list_head low_priority_q;
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
	if (emptyProcQ(&high_priority_q)==0){
		current_process = removeProcQ(&high_priority_q);
		if (current_process == NULL){
			auxiliar_scheduler();
		}else{
			setTIMER(TIMESLICE);
			LDST(&(current_process->p_s));	
		}

	}else if (emptyProcQ(&low_priority_q)==0){
	
		current_process = removeProcQ(&low_priority_q);
		if (current_process == NULL){
			auxiliar_scheduler();
		}else{
			
			setTIMER(TIMESLICE);
			LDST(&(current_process->p_s));	
		}

	}else{
		auxiliar_scheduler();
	}
	
}

void auxiliar_scheduler(){
	if (process_count == 0){
		//invoke the HALT BIOS service/instructions (vedi 7.3.7)
		HALT();
		//if the Process Count > 0 and the Soft-block Count > 0
	} else if (process_count>0 && soft_block_count>0) {
		//enter a Wait State (vedi 7.2.2-pop)
		setSTATUS(IECON| IMON); // ovvero:	setSTATUS(0b00000000000000001111111100000001);
		WAIT();
		//Deadlock for Pandos is defined as when
		//the Process Count > 0 and the Soft-block Count is zero
	} else if (process_count>0 && soft_block_count==0)
		//invoke the PANIC BIOS service/instruction. (vedi 7.3.6)
		PANIC();
}

