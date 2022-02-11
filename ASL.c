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
#include "pcb.h"
/*
	Funzione stampa lista realizzata da noi

	head: lista da stampare
	stampa: descrizione della stampa
	return: void
*/
void stampaLista(struct list_head *head, char *stampa){
	///*
	struct list_head *tmp = head->next;
	int i = 0;
	//for(int i=0;list_is_last(tmp, head);i++)
	while (tmp != head)
	{	
		if (tmp==NULL)printf("\ntmp NULL");//* DEBUG
		if (tmp->prev==NULL)printf("\ntmp->prev NULL");	//* DEBUG
		printf("\n %s %d %d", stampa, i, tmp);
		if (tmp->next==NULL)printf("\ntmp->next NULL %d", tmp->next);	//* DEBUG
		//printf("\np_list %d", &container_of(tmp, pcb_t, p_list)->p_list);
		printf("\ntmp->prev %d", tmp->prev);
		tmp = tmp->next;
		i++;
	} //*/
	  /*
	int i=0;
	struct list_head *iter;
	list_for_each(iter, head) {
		if(iter != NULL) {
			pcb_PTR current = container_of(iter, pcb_t, p_list);
			printf("\n%s indice= %d PCB id= %d", stampa, i, current->id);
			i=i+1;
		} else printf("head null");
	}*/
}
semd_t semd_table[MAXPROC];
struct list_head* semdFree_h = NULL; //lista di semafori liberi
struct list_head* asl_h = NULL;	 //lista di semafori attivi, utilizzati in questo momento

/*	18
	Inizializza la lista dei semdFree in modo da contenere 
	tutti gli elementi della semdTable. 
	Questo metodo viene invocato una volta sola durante
	l’inizializzazione della struttura dati.
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
	Viene inserito il PCB puntato da p nella coda dei
	processi bloccati associata al SEMD con chiave
	semAdd. Se il semaforo corrispondente non è
	presente nella ASL, alloca un nuovo SEMD dalla
	lista di quelli liberi (semdFree) e lo inserisce nella
	ASL, settando I campi in maniera opportuna (i.e.
	key e s_procQ). Se non è possibile allocare un
	nuovo SEMD perché la lista di quelli liberi è vuota,
	restituisce TRUE=1. In tutti gli altri casi, restituisce
	FALSE=0.

    return: 1 se non è possibile aggiungere, 0 altrimenti
	semAdd: 
	p: 
	
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
			semd_PTR new = container_of(semdFree_h->next, semd_t, s_link);
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
///*
int main() {
	initASL();
	printf("\ninitASL done!");
	printf("\ntestaASL %d", semdFree_h);
    printf("\nsemdfree->next %d", list_next(list_next(semdFree_h)));
	
	stampaLista(semdFree_h, "semdfree");
	printf("\n");
	return 0;
}
//*/