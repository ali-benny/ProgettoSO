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
	SYSCALL(TERMPROCESS, 0, 0, 0);
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
 * @return numero di caratteri attualmente trasmessi e negative of the devices's status
*/
void Write_Printer(int a0, unsigned int a1, unsigned int a2){
	char *str = a1;
	int len = a2;
	commandAddr = "una printer...";
	commandValue = "valore del comando";

	//da fase 2 per scrivere al terminal 0 usavamo questa funzione scritta dal prof:
	
/* a procedure to print on terminal 0 */
/*
void print(char *msg) {
    char     *s       = msg;
    devregtr *base    = (devregtr *)(TERM0ADDR);
    devregtr *command = base + 3;
    devregtr  status;

    SYSCALL(PASSEREN, (int)&sem_term_mut, 0, 0); // P(sem_term_mut) 
    while (*s != EOS) {
        devregtr value = PRINTCHR | (((devregtr)*s) << 8);
        status         = SYSCALL(DOIO, (int)command, (int)value, 0);
        if ((status & TERMSTATMASK) != RECVD) {
            PANIC();
        }
        s++;
    }
    SYSCALL(VERHOGEN, (int)&sem_term_mut, 0, 0); // V(sem_term_mut)
}*/
	//
	/*il che significa che dovremo:
	dichiarare un semaforo per la mutua esclusione
	trovare command di una stampante DISPONIBILE (una o 8?)
	trovare value
	chiamare una passeren sul semaforo
	while s != end of file (come prima)
		controllare lo status
		s++
	chiamare una verhoghen sul semaforo
	*/
	DO_IO(DOIO, commandaddr, commandValue)
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
*/
void Write_Terminal(int a0, unsigned int a1, unsigned int a2){

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