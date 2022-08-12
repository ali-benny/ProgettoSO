/**
 * @file syscall_supporto.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *   
 * @version 0.1
 * @date 2022-08-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "sysSupport.h"

/** SYSCALL_supp 1
	void SYSCALL(TERMINATE, int pid, 0, 0);
 * Un semplice wrapper per la system call corrispondente del kernel.
*/
void Terminate(int a0, unsigned int a1){

}
/** SYSCALL_supp 2
	void SYSCALL(GET_TOD, 0, 0, 0);
 * Restituisce il numero di microsecondi passati dall’accensione del sistema.
 @return 
*/
void Get_TOD(int a0){

}
/** SYSCALL_supp 3
	void SYSCALL(WRITEPRINTER, char *str, int len, 0);
 * Richede una stampa ininterrotta della stringa richiesta sul 
 terminale associato al processo
 * Fa sostanzialmente il lavoro della funzione print di p2test.c (ma per una stampante)
 * se l’indirizzo e’ fuori dalla memoria virtuale del processo o 
 la lunghezza richiesta e’ zero deve risultare nella sua terminazione
 * Restituisce il numero di caratteri stampati in caso di successo, 
 l’opposto dello stato del dispositivo in caso contrario [= se lo stato di DOIO fallisce, devo restituire questo stato]
*/
void Write_Printer(int a0, unsigned int a1, unsigned int a2){

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