/**
 * @file exceptions.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *  
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "exceptions.h"

//paragraph 3.4 pandos-chapter3.pdf (pag 8-9)
//Exception Handling

// Global Variables/Functions
extern void syscall_handler();
extern int process_count; //Process Count: numero di processi partiti ma non terminati
extern int soft_block_count; //Soft-block Count: numero di processi partiti e bloccati ma non terminati
//list_head ready_q; //Ready Queue: puntatore alla coda dei pcb che sono nello stato "ready"
extern pcb_PTR current_process; //Current Process: processo corrente in stato "ready"
extern int device_sem[49]; //Device Semaphores: 
extern list_head high_priority_q;// alta priorità
extern list_head low_priority_q;
extern passupvector_t *passupvector;

// * Auxiliar Functions * //
void PLT_Interrupt();
void Interval_Timer_Interrupt();
void Device_interrupt();


void exception_handler() {
	state_t *state_reg = (state_t*)BIOSDATAPAGE; // register value: che tipo di syscall è?
	//The cause of the exception is encoded in the .ExcCode field of the Cause
	//registrer (Cause.ExcCode) in the saved exception state (vedi 3.3)
	unsigned int cause = getCAUSE();
	unsigned int excCode = cause & GETEXECCODE;
	switch (excCode) { //for exception code ... :processing should be pased logn to your ...
		case 0: //Interrupts
			//Nucleus's device intterrupt handler (vedi 3.6)
			interrupt_handler();
			break;
		case 1 ... 3: //TLB exceptions
			//Nucleus's TLB exception handler (vedi 3.7.3)
			passup_or_die(PGFAULTEXCEPT);
			break;
		case 8: //SYSCALL
			//Nucleus's SYSCALL exception handler (vedi 3.5)
            syscall_handler(state_reg);
			break;
		case 4 ... 7: //Program Traps
        case 9 ... 12: //Program Traps
			//Nucleus's Program Traps exception handler (vedi 3.7.2)
            passup_or_die(GENERALEXCEPT);
			break;
		default: 
			klog_print("Unknown exception! You're in default case.\n");
			break;
	}
}

//paragrafo 3.6 pandos-chapter 3 (pag 17-18)
void interrupt_handler(){
	unsigned int cause = getCAUSE();
	unsigned int ip = (cause & 0x0000FF00) >> 8;  // tutti a 0 tranne 8-15 (IP) e >> (= saltiamo i primi 8 bit)
	
	ip >> 1; // tranne 1
	if (ip == 0) return; // non ci sono interrupt 
	else{
		if (ip % 2 != 0){ //PLT
			PLT_Interrupt();
		} 

		ip >> 1;
		if (ip % 2 != 0){ //Interval Timer
			Interval_Timer_Interrupt();
		}

		ip >> 1;
		if (ip % 2 != 0){ //Devices, Non-Timer Interrupts
			Device_Interrupt();
		}
	}
}

void passup_or_die(int type_of_exception){
	
}

//paragrafo 3.6.2 pandos-chapter3.pdf (pag 19)
void PLT_Interrupt(){ //(PLT = Processor Local Timer)
	//Aknowledge the PLT interrupt by loading the timer with a new value
	setTIMER(5000);
	//Copy the processor state at the time of the exception (..) into the Current process's pcb
	state_t *state_reg = (state_t *) BIOSDATAPAGE;
	current_process->p_s = *state_reg; // copy process state at the time of the exception
	//Place the Current Process on the Ready Queue; transitioning the Current Process
	//from the "running" state to the "ready" state.
	if (current_process->p_prio == 1) insertProcQ(&high_priority_q, current_process);
	else insertProcQ(&low_priority_q, current_process);
	//Call the Scheduler.
	scheduler();
}

//paragrafo 3.6.3 pandos-chapter3.pdf (pag 20)
//The System-wide Interval Timer and the Pseud-clock
void Interval_Timer_Interrupt(){
	//1. Aknnowledge the interrupt by loading the Interval Timer with a new value (100 milliseconds)
	LDIT(100000);
	//2. Unblock ALL pcbs blocked on the Pseudo-clock semaphore.
	//Hence the semantics of this semaphore are a bit different than traditional synchronization semaphores.
	while(headBlocked(&device_sem[48]) != NULL){
		pcb_PTR pcb = removeBlocked(&device_sem[48]);
		pcb->p_semAdd = NULL;
		if (pcb->p_prio == 1) insertProcQ(&high_priority_q, pcb);
		else insertProcQ(&low_priority_q, pcb);
	}
	//3. Reset the Pseudo-clock semaphore to zero.
	//   This insures that all NSYS7 calls block and that the Pseudo-clock semaphore does not grow positive-
	device_sem[48] = 0;
	//4. Return control to the Current Process: Perform a LDST on the saved exception state(...)
	if (current_process == NULL) scheduler();
	else LDST((state_t*)BIOSDATAPAGE);
}


//paragrafo 3.6.1 pandos-chapter3.pdf (pag 18) Non-Timer Interrups
void Device_interrupt() {
	//paragrafo 3.6.1 pandos-chapter3.pdf(pag 18) Non-Timer-Interrupts			
	//1. Calculate the address for this device's device register [5.1 pops].
	//   [from 5.1 pops] devAddrBase = 0x1000.0054 + ((IntlineNo - 3) * 0x80) + (DevNo * 0x10) // num di linea 3-7
	
	int IntLineNo = 3; // numero di linea
	while (ip % 2 == 0) { // trova il primo bit che è a 1
		ip >> 1;
		IntLineNo++;
	}	
	int DevNo = 0; // numero device
	int found = 0;
	while (DevNo <= 7 && !found) {
		unsigned int base = 0x1000.0040 , mask = 1;
		switch(IntLineNo) {
			//Table 5.4 pop
			case 3: break; //base rimane base e mi va bene
			case 4: base += 0x04; break;
			case 5: base += 0x08; break;
			case 6: base += 0x0C; break;
			case 7: base += 0x10; break;
			default: break; //messaggio di errore?
		}
		for (int i = 0; i < DevNo; i++) mask = mask*2;
		if (((base & mask) >> DevNo) > 0) found = 1;
		else DevNo++;
	}

	devreg_t* devAddrBase = (devreg_t*)(0x1000.0054 + ((IntlineNo - 3) * 0x80) + (DevNo * 0x10));

	//2. Save off the status code from the device's device register.
	
	//3. Aknlowledge the outstanding interrup.
	//   This is accomplished by writing the Aknowledge command code in the interrupting device's device register
	//   Alternatively, writing a new command in the interrupting device's device register will also aknowledge the interrupt
	//4. Perform a V operation on the Nucleus maintained semaphore associated with this (sub) device.
	//   Thid operation should unblock the process (pcb) wich initiates this I/O operation
	//   and then requested to wait its completion via a NSYS5 operation
	//5. Place the stored off status code in the newly unblocked pcb's v0 register.
	//6. Insert the newly unblocked pcb on the Ready Queue, transitioning this process
	//   from the "blocked" state to the "ready" state
	//7. Return controll to the Current Process: Perform a LDST on the saved exception state
	//   (located at the start of the BIOS Data Page [section 3.4])
	
	
	
}