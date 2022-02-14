/**
 * @file ASL.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *  
 * @version 1.0
 * @date 2022-02-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ASL.h"

/*	18
	Inizializza la lista dei semdFree in modo da contenere 
	tutti gli elementi della semdTable. 
	Questo metodo viene invocato una volta sola durante
	l’inizializzazione della struttura dati.

	return: void
*/
void initASL(){
	//inizializza la lista di semafori attivi
	LIST_HEAD(semd);
	asl_h = &semd;
	//inizializza la lista di semafori liberi
	LIST_HEAD(semdFree);
	semdFree_h=&semdFree;
	for (int i=0; i<MAXPROC; i++){
		semd_table[i].id = i;	//* DEBUG
		list_add( &semd_table[i].s_link, semdFree_h);
	}
}

/*	14
	Viene inserito il PCB puntato da p nella coda dei processi bloccati 
	associata al SEMD con chiave semAdd. 
	Se il semaforo corrispondente non è presente nella ASL, alloca un nuovo SEMD dalla
	lista di quelli liberi (semdFree) e lo inserisce nella ASL, settando I campi in 
	maniera opportuna (i.e. key e s_procQ). 
	Se non è possibile allocare un nuovo SEMD perché la lista di quelli liberi è vuota, 
	restituisce TRUE=1. 
	In tutti gli altri casi, restituisce FALSE=0.

    return: 1 se non è possibile aggiungere, 0 altrimenti
	semAdd: chiave dei processi associati al SEMD
	p: puntatore al PCB che voglio inserire
	
	nota: ASL = Active Semaphore List => (semd_h) lista dei semafori attivi
*/
int insertBlocked(int *semAdd, pcb_t *p) {
	if (semAdd != NULL && p != NULL) {
		//cercare il semAdd (key) nella ASL
		struct list_head *iter;
		int found=0;
		list_for_each(iter, asl_h){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				found = 1;
				list_add(&p->p_list, &current->s_procq);
				return 0;
			}
		}
		//se non è stato trovato nella ASL e la lista dei liberi non è vuota
		if (found==0 && !list_empty(semdFree_h)) {
			//alloca e inserisce
			semd_PTR new = container_of(list_next(semdFree_h), semd_t, s_link);
			list_del(semdFree_h->next);

			//setta i campi in maniera opportuna (cosa significa?)
			new->s_key = semAdd;
			LIST_HEAD(sproc);
			new->s_procq = sproc;
			//new->s_link lo cambierà list_add() dopo.

			//e poi inserisce in ASL
			list_add(&new->s_link,asl_h);
			list_add(&p->p_list, &new->s_procq);
			return 0;
		} else return 1;
	}
	else {
		if (semAdd == NULL)
			printf("\nERRORE insertProcQ(semAdd, p)! semAdd = NULL!");
		if (p == NULL)
			printf("\nERRORE insertProcQ(semAdd, p)! p = NULL!");
	}
    return 0;
}

/*	15
	Ritorna il primo PCB dalla coda dei processi
	bloccati (s_procq) associata al SEMD della
	ASL con chiave semAdd. Se tale descrittore
	non esiste nella ASL, restituisce NULL.
	Altrimenti, restituisce l’elemento rimosso. Se
	la coda dei processi bloccati per il semaforo
	diventa vuota, rimuove il descrittore
	corrispondente dalla ASL e lo inserisce nella
	coda dei descrittori liberi (semdFree_h).
*/
pcb_t* removeBlocked(int *semAdd) {
    return NULL;
}

/*	16
	Rimuove il PCB puntato da p dalla coda del semaforo
	su cui è bloccato (indicato da p->p_semAdd). Se il PCB
	non compare in tale coda, allora restituisce NULL
	(condizione di errore). Altrimenti, restituisce p. Se la
	coda dei processi bloccati per il semaforo diventa
	vuota, rimuove il descrittore corrispondente dalla ASL
	e lo inserisce nella coda dei descrittori liberi
*/
pcb_t* outBlocked(pcb_t *p) {
    return NULL;
}

/*	17
	Restituisce (senza rimuovere) il puntatore al PCB che
	si trova in testa alla coda dei processi associata al
	SEMD con chiave semAdd. Ritorna NULL se il SEMD
	non compare nella ASL oppure se compare ma la sua
	coda dei processi è vuota.
*/
pcb_t* semAddBlocked(int *semAdd) {
	return NULL;
}

// ****** MAIN per DEBUG ******
/*
int main() {
	printf("\nInitializing ASL...");
	initASL();
	printf("\ninitASL done!");
	printf("\nsemdFree_h %d", semdFree_h);
    printf("\nsemdfree_h->next %d", list_next(list_next(semdFree_h)));
	stampaLista(semdFree_h, "semdfree");	//! Segmentation Fault: cerco di usare il semdFree_h->next anche se è NULL
	
	//int insBlock = insertBlocked();
	//printf("\ninsertBlocked done! %d", insBlock);
	printf("\n");
	return 0;
}
*/