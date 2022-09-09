/**
 * @file sysSupport.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *   
 * @version 0.1
 * @date 2022-08-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "sysSupport.h"

support_t* support_exc; // current process support struct
state_t* state_exc;
extern int swap_pool_sem;
extern int mutex_asid;
//se dobbiamo utilizzare più terminali o più stampanti questi potrebbero diventare vettori di dim 8 (= num device installati)
int uproc_sem[UPROCMAX];
extern swap_t swap_pool[POOLSIZE];
extern int master_sem; 

/** 
 * Support Syscall Exception Handler
 * 
 * 
 */
void support_exception_handler(){
	klog_print("exc_h\n");
	//we need the support_struct of the current process, and we get it with NSYS8
	support_exc = (support_t*)SYSCALL(GETSUPPORTPTR,0,0,0); // current process support struct by NSYS8
	state_exc = &support_exc->sup_exceptState[GENERALEXCEPT];
	int cause = CAUSE_GET_EXCCODE(support_exc->sup_exceptState[GENERALEXCEPT].cause);
	unsigned int a0 = state_exc->reg_a0;
	// klog_print("a0: ");klog_print_hex(a0);klog_print("\n");
	//klog_print("cause: ");klog_print_hex(cause);klog_print("\n");
	if (cause == SYSEXCEPTION){
		support_syscall_handler(a0);
	}else{
		support_program_trap();
	}
}

/**
 * Handles support system calls.
 *
 * @param cause a0
 *
 * @returns None
 */
void support_syscall_handler(unsigned int cause){
	klog_print("sys_h\n");
	unsigned int a0 = cause;
	//state_exc inizializzato dal chiamante
	unsigned int a1 = state_exc->reg_a1;
	unsigned int a2 = state_exc->reg_a2;
	switch(a0) {
		case GET_TOD:
			Get_TOD(a0);
			break;
		case TERMINATE:
			Terminate(a0);
			break;
		case WRITEPRINTER:
			Write_Printer(a0, a1, a2);
			break;
		case WRITETERMINAL:
			Write_Terminal(a0, a1, a2);
			break;
		case READTERMINAL:
			Read_Terminal(a0, a1);
			break;
		/*default:
			//passup_or_die(GENERALEXCEPT);
			support_program_trap();*/
	}
	
	state_exc->pc_epc += 4;
	LDST(state_exc);
}

/**
 * Program Trap Exception Handler
 * 
 * pandosplus_phase3.pdf pag 18
 * paragrafo 4.8 The Program Trap Exception Handler
 * 
 * 
 */
void support_program_trap(){
	klog_print("p_trap\n");
	//processor state in the exception is in support_exc->sup_exceptState
	if (support_exc->sup_asid == mutex_asid) {//if my asid is holding mutex
		//siamo in mutua esclusione (quindi l'asid corrente è uguale al asid salvato in mutex_asid
		mutex_asid = -1; //my asid is not longer holding mutex
		SYSCALL(VERHOGEN, (memaddr)&swap_pool_sem, 0, 0);
	}
	// terminate the process with SYS2
	SYSCALL(TERMINATE, 0, 0, 0);
}

/**
 * SYSCALL_supp 1
 * 
 * unsigned int retValue = SYSCALL (GET_TOD, 0, 0, 0);
 * ~ 4.7.1
 * Restituisce il numero di microsecondi passati dall’accensione del sistema.
 * 
 * @return microsecondi di attesa dall'accensione
*/
void Get_TOD(int a0){
	klog_print("sys1");
	int time;
	STCK(time); /* Macro to read the TOD clock */
	state_exc->reg_v0 = time;
}

/**
 * SYSCALL_supp 2
 * 
 * void SYSCALL(TERMINATE, int pid, 0, 0);
 * 
 * Un semplice wrapper per la system call corrispondente del kernel.
 * 
*/
void Terminate(int a0){
	klog_print("sys2");
	//a che serve la terminate della struttura di supporto se devo solo chiamare la terminate del kernel...?
 //   klog_print("asid: ");klog_print_hex(support_exc->sup_asid);klog_print("\n");
	if(a0 == TERMINATE) {
		for (int i = 0 ; i< POOLSIZE; i +=1){
        	if(swap_pool[i].sw_asid == support_exc->sup_asid)
          		swap_pool[i].sw_asid = NOPROC;
        }
        
        SYSCALL(VERHOGEN,(memaddr) &master_sem, 0, 0);
		SYSCALL(TERMPROCESS, 0, 0, 0);

	}
	
}

/**
 * Print Auxiliar function SYS3 & SYS4 of support layer
 * 
 * @param command dove stampare
 * @param semaphore semaforo per la mutua esclusione
 * @param msg puntatore al primo carattere da stampare
 * @param len lunghezza del messaggio da stampare
 *
 * @return il numero di caratteri attualmente trasmessi
 */
int write(devreg_t* command, int* semaphore, char* msg, int len) {
	klog_print("write-");
	//It is an error to write to a ... device from an address outside of the requesting U-proc’s logical address space
	//controlliamo che msg sia dentro kseg e lo sia per tutta la lunghezza della stringa.
	//int is_in_Uproc_address_space = (msg >= UPROCSTARTADDR && (msg + len * Lunghezza_carattere) <= USERSTACKTOP);
	int is_in_Uproc_address_space = ((int)&msg >= UPROCSTARTADDR && ((int)&msg + len) <= USERSTACKTOP);
	//It is an error ... request a SYS3 with a length less than 0, or a length greater than 128.

	char* s = msg;
	int count = len;
	int status;
	int is_ready = 1; // "is all ok in doing next DOIO"
	if (len >= 0 && len <= 128 && is_in_Uproc_address_space) {
		SYSCALL(PASSEREN, (int)semaphore, 0, 0); // P(semaphore)
		mutex_asid = support_exc->sup_asid; //my asid is holding mutex using semaphores in support level
		// controllo se non ho avuto errore al DOIO precedente,
		// se ho ancora caratteri da stampare (len - count)
		// e se non ho già raggiunto la fine della stringa (*)
		while (is_ready && count > 0 && *s != EOS ) {
			int value = PRINTCHR | (((int)*s) << 8);	//! forse non `e int btw
			status         = SYSCALL(DOIO, (int)command, (int)value, 0);
			if (status != READY) {
				state_exc->reg_v0 = status;
				is_ready = 0; //in this case i haven't to do nexts DOIO
			}
			s++;
			count--;
		}
		mutex_asid = -1; //my asid is not longer holding mutex
		SYSCALL(VERHOGEN, (int)semaphore, 0, 0); // V(semaphore)
	} else {
		klog_print("write with wrong parameters");
		if(is_in_Uproc_address_space)
			klog_print("len error");
		else
			klog_print("address error");
		SYSCALL(TERMINATE, 0, 0, 0);
	}
	return len-count;	// num caratteri inviati
	//nota (*) esempio xcvd "server scrivi 'ciao' lunghezza 100".
}

/** 
 * SYSCALL_supp 3
 * 
 * void SYSCALL(WRITEPRINTER, char *str, int len, 0);
 * 
 * Richede una stampa ininterrotta della stringa richiesta sul terminale associato al processo
 * Fa sostanzialmente il lavoro della funzione print di p2test.c (ma per una stampante)
 * se l’indirizzo e’ fuori dalla memoria virtuale del processo o la lunghezza richiesta e’ zero deve risultare nella sua terminazione
 * Restituisce il numero di caratteri stampati in caso di successo, 
 l’opposto dello stato del dispositivo in caso contrario [= se lo stato di DOIO fallisce, devo restituire questo stato]
 *
 * @param a1 stringa da stampare
 * @param a2 lunghezza della stringa
 * 
 * @return numero di caratteri attualmente trasmessi e negative of the devices's status [in write]
*/
void Write_Printer(int a0, unsigned int a1, unsigned int a2){
	klog_print("sys3- ");
	char* str = (char*) a1;
	int len = (int) a2;
	// address of the device's device register
	devregarea_t* devReg = (devregarea_t*) RAMBASEADDR; // device register
	devreg_t* devAddrBase = (devreg_t*) &devReg->devreg[PRNTINT-3][support_exc->sup_asid];
	
	state_exc->reg_v0 = write(devAddrBase + 3, &uproc_sem[support_exc->sup_asid-1], str, len);
}

/**
 *  SYSCALL_supp 4
 *
 * void SYSCALL(WRITETERMINAL, char *str, int len, 0);
 * 
 * Richede una stampa ininterrotta della stringa richiesta sul terminale associato al processo
 * Fa sostanzialmente il lavoro della funzione print di p2test.c
 * se l’indirizzo e’ fuori dalla memoria virtuale del processo o 
 la lunghezza richiesta e’ zero deve risultare nella sua terminazione
 * Restituisce il numero di caratteri stampati in caso di successo, 
 l’opposto dello stato del dispositivo in caso contrario
 * 
 * @param a1 stringa da stampare
 * @param a2 lunghezza della stringa
 * 
*/
void Write_Terminal(int a0, unsigned int a1, unsigned int a2){
	klog_print("sys4- ");
	char *str = (char*)a1;
	int len = (int) a2;
	// address of the device's device register
	devregarea_t* devReg = (devregarea_t*) RAMBASEADDR; // device register
	devreg_t* devAddrBase = (devreg_t*) &devReg->devreg[TERMINT-3][support_exc->sup_asid];
	
	state_exc->reg_v0 = write(devAddrBase + 3, &uproc_sem[support_exc->sup_asid-1], str, len);
}

/**
 * SYSCALL_supp 5
 * 
 *	void SYSCALL(READTERMINAL, char *addr, 0, 0);
 *
 * Legge una riga (fino al newline) dal terminale associato al processo
 * Mentre l’input viene letto il processo e’ sospeso
 * se l’indirizzo e’ fuori dalla memoria virtuale del processo o 
	la lunghezza richiesta e’ zero deve risultare nella sua terminazione
 * Restituisce il numero di caratteri letti in caso di successo, l’opposto dello stato del dispositivo in caso contrario
 * 
 * @param a1 the virtual address of a string buffer where the data read should be placed
 * 
 * @return 
*/
void Read_Terminal(int a0, unsigned int a1){
	klog_print("sys5- ");
/*
// *idea generale* //
	int trasmitted_char = 0;
	//sospendi il processo mentre l'input viene letto
//	SYSCALL(PASSEREN, &sem_terminale_associato_al_processo, 0, 0);
	//devregarea_t* devReg = (devregarea_t*) RAMBASEADDR; // device register
	
	int *commandAddress = ...;
	//devReg->devreg[IntLineNo][DevNo].term.recv_command
	int commandValue = ...;
	char carattere_letto = SYSCALL(DOIO, &commandAddress, commandValue, 0);		//? boh, forse non serve
	while(carattere_letto != EOS) { //finchè la riga da leggere non è finita
		//scrivo carattere_letto sul buffer
		transmitted_char++;
	}
//	SYSCALL(VERHOGEN, &sem_terminale_associato_al_processo, 0, 0);
	state_exc->reg_v0 = transmitted_char;
	
	*/
	// if read was successfull
	// then return in v0 the number of characters actually transmitted (--> to buffer)
	
	// if the operation end with status != 5 [character received]
	// then return in v0 the negative of the device's status value

	/*pandosplus_phase3.pdf
	paragrafo 4.7.5 Read From Terminal (SYS5)
		int SYS5 (READ FROM TERMINAL, char *addr) When requested, this service causes
		the requesting U-proc to be suspended until a line of input (string
		of characters) has been transmitted from the terminal device associated with
		the U-proc.
		The SYS5 service is requested by the calling U-proc by placing the value
		5 in a0, the virtual address of a string buffer where the data read should
		be placed in a1, and then executing a SYSCALL instruction. Once the
		process resumes, the number of characters actually transmitted is returned
		in v0 if the read was successful. If the operation ends with a status other
		than “Character Received” (5), the negative of the device’s status value is
		returned in v0.
		Attempting to read from a terminal device to an address outside of the
		requesting U-proc’s logical address space is an error and should result in the
		U-proc being terminated (SYS2).
		The following C code can be used to request a SYS5:
		int retValue = SYSCALL (READTERMINAL, char *virtAddr, 0, 0);
		Where the mnemonic constant READTERMINAL has the value of 5.
	*/
}
