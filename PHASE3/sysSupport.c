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

//#define SUP_SYS_DEBUG //per debuggare le system call del livello di supporto

support_t* support_exc; // current process support struct
state_t* state_exc;
extern int swap_pool_sem;
extern int mutex_asid;
//se dobbiamo utilizzare più terminali o più stampanti questi potrebbero diventare vettori di dim 8 (= num device installati)
extern int sup_dev_sem[48];
extern swap_t swap_pool[POOLSIZE];
extern int master_sem; 

//auxiliar function to determine if we are in Uproc_address_space
int uproc_space(char* str) {
	//It is an error to write to a ... device from an address
	//outside of the requesting U-proc’s logical address space
	//controlliamo che msg sia dentro kseg e lo sia per tutta la lunghezza della stringa.
	return (memaddr)str >= UPROCSTARTADDR && ((memaddr)str) <= USERSTACKTOP;
}

//auxiliar function to determine "WHO IS IN MUTEX RIGHT NOW ON THE SUPPORT STRUCTURE?"
void passeren_on_sem(int* semaphore, int asid) {
	SYSCALL(PASSEREN, (memaddr)semaphore, 0, 0); // P(semaphore)
	mutex_asid = support_exc->sup_asid; //my asid is holding mutex using semaphores in support level
}
void verhogen_on_sem(int* semaphore) {
	mutex_asid = -1; //my asid is not longer holding mutex
	SYSCALL(VERHOGEN, (memaddr)semaphore, 0, 0); // V(semaphore)
}


/** 
 * Support Syscall Exception Handler
 * 
 * 
 */
void support_exception_handler() {
#ifdef SUP_SYS_DEBUG
	klog_print(" exc_h");
#endif
	//we need the support_struct of the current process, and we get it with NSYS8
	support_exc = (support_t*)SYSCALL(GETSUPPORTPTR,0,0,0); // current process support struct by NSYS8
	state_exc = &support_exc->sup_exceptState[GENERALEXCEPT];
	int cause = CAUSE_GET_EXCCODE(support_exc->sup_exceptState[GENERALEXCEPT].cause);
	unsigned int a0 = state_exc->reg_a0;

#ifdef SUP_SYS_DEBUG
	klog_print(" a0: ");klog_print_hex(a0);
	klog_print(" cause: ");klog_print_hex(cause);
#endif
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
void support_syscall_handler(unsigned int cause) {
#ifdef SUP_SYS_DEBUG
	klog_print(" sys_h");
#endif
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
		default:
			//passup_or_die(GENERALEXCEPT);
			support_program_trap();
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
void support_program_trap() {
#ifdef SUP_SYS_DEBUG
	klog_print(" p_trap");
#endif
	//processor state in the exception is in support_exc->sup_exceptState
	if (support_exc->sup_asid == mutex_asid) {//if my asid is holding mutex
		//siamo in mutua esclusione (quindi l'asid corrente è uguale al asid salvato in mutex_asid

		verhogen_on_sem(&swap_pool_sem);
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

#ifdef SUP_SYS_DEBUG
	klog_print(" get tod");
#endif
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
#ifdef SUP_SYS_DEBUG
	klog_print(" sup_terminate asid: ");klog_print_hex(support_exc->sup_asid);
#endif
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
void write(memaddr command, int* semaphore, char* msg, int len) {

#ifdef SUP_SYS_DEBUG
	klog_print("-write");
#endif

	char* s = msg;
	int count = len;
	int status;
	int is_ready = 1; // "is all ok in doing next DOIO"

	//It is an error ... request a SYS3 with a length less than 0, or a length greater than 128.
	//It is an error to write to a ... device from an address
	//outside of the requesting U-proc’s logical address space
	if (len > 0 && len <= 128 && uproc_space(msg)) {

		passeren_on_sem(semaphore, support_exc->sup_asid);

		// controllo se non ho avuto errore al DOIO precedente,
		// se ho ancora caratteri da stampare (len - count)
		// e se non ho già raggiunto la fine della stringa (*)
		while (is_ready && count > 0 && *s != EOS ) {
			int value = PRINTCHR | (((int)*s) << 8);	//! forse non `e int btw
			status = SYSCALL(DOIO, (int)command, (int)value, 0);
			if ((status & 0xFF) != RECVD) {
				is_ready = 0;
			}
			s++;
			count--;
		}

		verhogen_on_sem(semaphore);
		if (status == OKCHARTRANS)
			state_exc->reg_v0 = len-count;	// num caratteri inviati
		else
			state_exc->reg_v0 = -status;  //errore
	}else {
#ifdef SUP_SYS_DEBUG
		klog_print(" WRITE parameter error, or out of user space memory!");
#endif
		support_program_trap();
	}
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
void Write_Printer(int a0, unsigned int a1, unsigned int a2) {

#ifdef SUP_SYS_DEBUG
	klog_print(" write_printer");
#endif
	char* str = (char*) a1;
	int len = (int) a2;
	// address of the device's device register
	devregarea_t* devReg = (devregarea_t*) RAMBASEADDR; // device register
	devreg_t* devAddrBase = (devreg_t*) &devReg->devreg[PRNTINT-3][support_exc->sup_asid -1 ];
	
	write((memaddr)&devAddrBase->dtp.command,
					&sup_dev_sem[(PRNTINT-3)*8 + support_exc->sup_asid -1], str, len);
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
#ifdef SYS_SUP_DEBUG
	klog_print(" write_terminal");
#endif
	char *str = (char*)a1;
	int len = (int) a2;
	// address of the device's device register
	devregarea_t* devReg = (devregarea_t*) RAMBASEADDR; // device register
	devreg_t* devAddrBase = (devreg_t*) &devReg->devreg[TERMINT-3][support_exc->sup_asid - 1];
	write((memaddr)&devAddrBase->term.transm_command,
					&sup_dev_sem[(TERMINT-3)*8 + support_exc->sup_asid -1 + 8], str, len);
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

#ifdef SUP_SYS_DEBUG
	klog_print(" read_terminal");
#endif
	char *str = (char*)a1;
	// address of the device's device register
	devregarea_t* devReg = (devregarea_t*) RAMBASEADDR; // device register
	devreg_t* devAddrBase = (devreg_t*) &devReg->devreg[TERMINT-3][support_exc->sup_asid - 1];
	
	int* semaphore = &sup_dev_sem[(TERMINT-3)*8 + support_exc->sup_asid -1];

	char* s = str;
	int count = 0;
	int status;
	int is_ready = 1; // "is all ok in doing next DOIO"

	//It is an error to write to a ... device from an address
	//outside of the requesting U-proc’s logical address space
	if (uproc_space(str)) {

		passeren_on_sem(semaphore, support_exc->sup_asid);

		// controllo se non ho avuto errore al DOIO precedente,
		// se ho ancora caratteri da stampare (len - count)
		// e se non ho già raggiunto la fine della stringa (*)
		while (is_ready && count > 0 && *s != EOS ) {
			int value = 2 | (((int)*s) << 8);	//! 2 = RECVCHAR
			status         = SYSCALL(DOIO, (int)&devAddrBase->term.recv_command, (int)value, 0);
			if (status != READY) {
				is_ready = 0; //in this case i haven't to do nexts DOIO
			}
			s++;
			count++;
		}
		
		verhogen_on_sem(semaphore);
		if (status == OKCHARTRANS)
			state_exc->reg_v0 = count;
		else
			state_exc->reg_v0 = -status;

	} else {
#ifdef SUP_SYS_DEBUG
		klog_print(" READ TERMINAL error: attempt to read not in u-proc address space!");
#endif
		support_program_trap();
	}
	

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
