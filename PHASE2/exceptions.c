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
//struct list_head ready_q; //Ready Queue: puntatore alla coda dei pcb che sono nello stato "ready"
extern pcb_PTR current_process; //Current Process: processo corrente in stato "ready"
extern int device_sem[49]; //Device Semaphores: 
extern struct list_head high_priority_q;// alta priorità
extern struct list_head low_priority_q;
extern struct passupvector_t *passupvector;

// * Auxiliar Functions * //
void PLT_Interrupt();
void Interval_Timer_Interrupt();
void Device_Interrupt(unsigned int ip);

// global var
state_t* state_reg;

/**
 * Handles system calls.
 *
 * @param state The state of the current process.
 *
 * @returns None
 */
void syscall_handler(state_t* state){
	state_reg = state;
	state_reg->pc_epc += 4; //incrementiamo il pc
	unsigned int a0 = state_reg->reg_a0;
	unsigned int a1 = state_reg->reg_a1;
	unsigned int a2 = state_reg->reg_a2; 
	unsigned int a3 = state_reg->reg_a3;
	switch(state_reg->reg_a0){
		case CREATEPROCESS: //-1
			Create_Process(a0,a1,a2,a3);
			break;
		case TERMPROCESS: //-2
			Terminate_Process(a0,a1);
			break;
		case PASSEREN:
			Passeren(a0,a1);
			break;
		case VERHOGEN:
			Verhogen(a0,a1);
			break;
		case DOIO:
			DO_IO(a0,a1,a2);
			break;
		case GETTIME:
			Get_CPU_Time(a0);
			break;
		case CLOCKWAIT:
			Wait_For_Clock(a0);
			break;
		case GETSUPPORTPTR:
			Get_Support_Data(a0);
			break;
		case GETPROCESSID:
			Get_Process_Id(a0,a1);
			break;
		case YIELD:
			Yield(a0);
			break;
		default: 
			passup_or_die(GENERALEXCEPT);
	}
}

/**
 * Handles an exception.
 *
 * @returns None
 */
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
/**
 * Handles the interrupt handler.
 *
 * @returns None
 */
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
			Device_Interrupt(ip);
		}
	}
}


/**
 * Passes up an exception to the parent process.
 *
 * @param type_of_exception The type of exception to pass up.
 *
 * @returns None
 */
void passup_or_die(int type_of_exception){
	//a. if the Current Process's p_supportScruct is NULL,
	//then the exception should be handled as a NSYS2:
	//The Current Process and all its progeny are terminated.
	//this is the "DIE" portion of PassUp or Die.
	
	if(current_process->p_supportStruct == NULL){
		Terminate_Process(TERMPROCESS,(unsigned int) current_process); //! modificato con casting per prova fix error
	} else {
		//- If the Current Process's p_supportStruct is non-NULL
		//then handling of the exception is "PASSED UP"		
		//- the location, in this case, is fixed; 
		// a given location in the BIOS data page (For Processor 0, this is 0x0FFF.F000) [Section 3.2.2-pop]
		//- the address (and stack pointer) for the handler to pass control to was seed by the Nucleus,
		//during Nucleus initialization, in the appriopriate location of the Pass Up Vector. (Section 3.1)
		//- TLB exceptions (i.e. page faults): The Support Level TLB exception handler
		//- all other exceptions: The Support Level general exception handler

		//b. Copy the saved exception state from the Bios Data Page to the correct
		//sup_exceptState field of the Current Process.
		//The Curren Process's pcb should point to a non-null support_t.
		current_process->p_supportStruct->sup_exceptState[type_of_exception] = *((state_t*) BIOSDATAPAGE);
		context_t context = current_process->p_supportStruct->sup_exceptContext[type_of_exception]; //! modificato con context_t invece di unsigned int per prova fix error
		
		//c. Perform a LDCXT using the fields from the correct sup exceptContext
		//field of the Current Process. [Section 7.3.4-pops]
		LDCXT(context.stackPtr, context.status, context.pc);
	}
}

//paragrafo 3.6.2 pandos-chapter3.pdf (pag 19)
/**
 * This function is called when the user presses the interrupt button.
 * It is responsible for setting the state of the current process to
 * the state of the process that was interrupted.
 *
 * @returns None
 */
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
/**
 * Interrupt handler for the interval timer.
 *
 * @returns None
 */
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
	else LDST((state_t*) BIOSDATAPAGE);
}


//paragrafo 3.6.1 pandos-chapter3.pdf (pag 18) Non-Timer Interrups
/**
 * Interrupts the device.
 *
 * @param ip The interrupt number.
 *
 * @returns None
 */
void Device_Interrupt(unsigned int ip) {
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
		unsigned int base = 0x10000040 , mask = 1;
		switch(IntLineNo) {
			//Table 5.4 pop
			case 3: break; //base rimane base e mi va bene
			case 4: base += 0x04; break;
			case 5: base += 0x08; break;
			case 6: base += 0x0C; break;
			case 7: base += 0x10; break;
			default: break;
		}
		for (int i = 0; i < DevNo; i++) mask = mask*2;
		if (((base & mask) >> DevNo) > 0) found = 1;
		else DevNo++;
	}
	
	// address of the device's device register
	// NOTA: con devreg abbiamo -> dtp e term 
	devreg_t* devAddrBase = (devreg_t*)(0x10000054 + ((IntLineNo - 3) * 0x80) + (DevNo * 0x10));
	int device_position = (IntLineNo - 3) + DevNo;
	unsigned int statusCode;
	int *semAddr;
	pcb_PTR pcb; 
	if (IntLineNo != 7){ //it is a terminal?
		//it is NOT a terminal
		statusCode = devAddrBase->dtp.status;
		devAddrBase->dtp.command = ACK;
	} else { //it is a terminal
		//scrittura:
		if (devAddrBase->term.recv_status != READY && devAddrBase->term.recv_status != BUSY){
			//2. Save off the status code from the device's device register.
			statusCode = devAddrBase->term.recv_status;
			//3. Aknlowledge the outstanding interrupt.
			devAddrBase->term.recv_command = ACK; //? term.recv_command se è un terminale?
			//4. Perform a V operation on the Nucleus maintained semaphore associated with this (sub) device.
			//semAddr = dev_sem[device_position];
			//pcb = V_operation(semAddr);
			//5. Place the stored off status code in the newly unblocked pcb's v0 register.
			//pcb->p_s.reg_v0 = statusCode;
			//6. Insert the newly unblocked pcb on the Ready Queue, transitioning this process
			//   from the "blocked" state to the "ready" state
			//?domanda: non lo fa già la V_operation? dobbiamo togliere questa parte
		}
		//lettura:
		if (devAddrBase->term.transm_status != READY && devAddrBase->term.transm_status != BUSY){
			//2. Save off the status code from the device's device register.
			statusCode = devAddrBase->term.transm_status;
			//3. Aknlowledge the outstanding interrupt.
			devAddrBase->term.transm_command = ACK;
			//4. Perform a V operation on the Nucleus maintained semaphore associated with this (sub) device.
			device_position += 8;
		//	semAddr = dev_sem[device_position];
		//	//pcb = V_operation(semAddr);
			//5. Place the stored off status code in the newly unblocked pcb's v0 register.
			//pcb->p_s.reg_v0 = statusCode;
			//6. Insert the newly unblocked pcb on the Ready Queue, transitioning this process
			//   from the "blocked" state to the "ready" state
			//?domanda: non lo fa già la V_operation? dobbiamo togliere questa parte
		}
	}
	//4. Perform a V operation on the Nucleus maintained semaphore associated with this (sub) device.
	//   This operation should unblock the process (pcb) wich initiates this I/O operation
	//   and then requested to wait its completion via a NSYS5 operation
	semAddr = (int *) device_sem[device_position];
	pcb = V_operation(semAddr);
	//5. Place the stored off status code in the newly unblocked pcb's v0 register.
	pcb->p_s.reg_v0 = statusCode;	
	//6. Insert the newly unblocked pcb on the Ready Queue, transitioning this process
	//   from the "blocked" state to the "ready" state
	//?domanda: non lo fa già la V_operation? dobbiamo togliere questa parte
	//7. Return controll to the Current Process: Perform a LDST on the saved exception state
	//   (located at the start of the BIOS Data Page [section 3.4])
	LDST((state_t*)BIOSDATAPAGE);
	
	// *********************************************************************** //
	//NOTA: (cosa dice il pdf, il codice e' sopra (diviso tra lettura e scrittura ))
	//2. Save off the status code from the device's device register.
	
	//3. Aknlowledge the outstanding interrupt.
	//   This is accomplished by writing the Aknowledge command code in the interrupting device's device register
	//   Alternatively, writing a new command in the interrupting device's device register will also aknowledge the interrupt

}
