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
typedef unsigned int devregtr;
//se dobbiamo utilizzare più terminali o più stampanti questi potrebbero diventare vettori di dim 8 (= num device installati)
int semaphore_terminal = 1;
int semaphore_printer = 1;

/** 
 * Support Syscall Exception Handler
 * 
 * 
 */
void support_exception_handler(){
	//we need the support_struct of the current process, and we get it with NSYS8
	support_exc = (support_t*)SYSCALL(GETSUPPORTPTR,0,0,0); // current process support struct by NSYS8
	state_exc = support_exc->sup_exceptState[GENERALEXCEPT];
	unsigned int a0 = state_exc->reg_a0;
	if (a0 >=1 && a0 <= 5){
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
			passup_or_die(GENERALEXCEPT);
	}
	
	state_exc->pc_epc += 4;
	LDST(state_exc);
	
}

/**
 * Support Program Traps
 * ~ 4.8 pandosplus-phase3-Chapter4.pdf
 */
void support_program_trap(){

}
/**
 * SYSCALL_supp 1
 * unsigned int retValue = SYSCALL (GET_TOD, 0, 0, 0);
 * ~ 4.7.1
 * Restituisce il numero di microsecondi passati dall’accensione del sistema.
 * 
 * @return in v0 microsecondi di attesa dall'accensione
*/
void Get_TOD(int a0){
	int time;
	STCK(time); /* Macro to read the TOD clock */
	state_exc->reg_v0 = time;
}

/**
 * SYSCALL_supp 2
 * void SYSCALL(TERMINATE, int pid, 0, 0);
 * 
 * Un semplice wrapper per la system call corrispondente del kernel.
 * 
*/
void Terminate(int a0){
	//a che serve la terminate della struttura di supporto se devo solo chiamare la terminate del kernel...?
	if(a0 == TERMINATE) SYSCALL(TERMPROCESS, 0, 0, 0);
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
int write(devregtr *command, semd_PTR semaphore, char *msg, int len) {
	//It is an error to write to a ... device from an address outside of the requesting U-proc’s logical address space

	//It is an error ... request a SYS3 with a length less than 0, or a length greater than 128.
	if(len >= 0 && len <= 128) {
		char *s = msg;
		int count = len;
		devregtr status;
		int is_ready = 1; // "is all ok in doing next DOIO"
		SYSCALL(PASSEREN, (int)&semaphore, 0, 0) // P(semaphore)
		//controllo se non ho avuto errore al DOIOprecedente,
		// se ho ancora caratteri da stampare (len - count)
		// e se non ho già raggiunto la fine della stringa (*)
		while (is_ready && count > 0 && s != EOS ) {
			devregtr value = PRINTCHR | (((devregtr)*s) << 8);
			status         = SYSCALL(DOIO, (int)command, (int)value, 0);
			if (status != READY) {
				state_exc->reg_v0 = status;
				is_ready = 0; //in this case i haven't to do nexts DOIO
			}
			s++;
			count--;
		}
		SYSCALL(VERHOGEN, (int)&semaphore, 0, 0) // V(semaphore)
	} else Terminate(TERMINATE);
	return len-count;	// num caratteri inviati
	//nota (*) esempio xcvd "server scrivi 'ciao' lunghezza 100".
}

/** 
 * SYSCALL_supp 3
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
	
	char *str = a1;
	int len = a2;
    devregtr *base    = (devregtr *)(....); //? in quale printer?
	devregtr *command = base + 3;
	
	state_exc->reg_v0 = write(command, &semaphore_printer, str, len);
}

/** SYSCALL_supp 4
	void SYSCALL(WRITETERMINAL, char *str, int len, 0);
 * Richede una stampa ininterrotta della stringa richiesta sul 
 terminale associato al processo
 * Fa sostanzialmente il lavoro della funzione print di p2test.c
 * se l’indirizzo e’ fuori dalla memoria virtuale del processo o 
 la lunghezza richiesta e’ zero deve risultare nella sua terminazione
 * Restituisce il numero di caratteri stampati in caso di successo, 
 l’opposto dello stato del dispositivo in caso contrario
 * 
 * @param a1 stringa da stampare
 * @param a2 lunghezza della stringa
 * 
 * @return
*/
void Write_Terminal(int a0, unsigned int a1, unsigned int a2){
	
	char *str = a1;
	int len = a2;
    devregtr *base    = (devregtr *)(....); //? in quale terminale?
	//se terminal 0 allora è TERM0ADDR
    devregtr *command = base + 3;
	state_ext->reg_v0 = write(command, &semaphore_terminal, str, len);
}
/** SYSCALL_supp 5
	void SYSCALL(READTERMINAL, char *str, int len, 0);

 * Legge una riga (fino al newline) dal terminale associato al processo
 * Mentre l’input viene letto il processo e’ sospeso
 * se l’indirizzo e’ fuori dalla memoria virtuale del processo o 
	la lunghezza richiesta e’ zero deve risultare nella sua terminazione
 * Restituisce il numero di caratteri letti in caso di successo, l’opposto dello stato del dispositivo in caso contrario
*/
void Read_Terminal(int a0, unsigned int a1, unsigned int a2){

}