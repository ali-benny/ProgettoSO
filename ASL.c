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

//#include "pcb.h"
//#include "PCB.c"

/*	18
	Inizializza la lista dei semdFree in modo da contenere 
	tutti gli elementi della semdTable. 
	Questo metodo viene invocato una volta sola durante
	l’inizializzazione della struttura dati.

	return: void
*/
void initASL(){
	//inizializza la lista di semafori liberi
	LIST_HEAD(semdFree);
	semdFree_h=&semdFree;
	for (int i=0; i<MAXPROC; i++){
		semd_table[i].id = i;	//* DEBUG
		list_add(&semd_table[i].s_link, semdFree_h);
	}
	printf("\nasl_h = %d",asl_h);
	printf("\nasl_h->next = %d",asl_h->next);
	printf("\nsemdFree_h %d", semdFree_h);
    printf("\nsemdfree_h->next %d", semdFree_h->next);                                                                                                                                                                                  
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
				p->p_semAdd=semAdd;
				list_add(&p->p_list, &current->s_procq);
				return 0;
			}
		}
		semd_PTR new = NULL;
		//se non è stato trovato nella ASL e la lista dei liberi non è vuota
		if (found==0 && !list_empty(semdFree_h)) {
			//alloca e inserisce
			new = container_of(semdFree_h->next, semd_t, s_link);
			list_del(semdFree_h->next);
			//setta i campi
			p->p_semAdd=semAdd;
			new->s_key = semAdd;
			//LIST_HEAD(sproc);
			//new->s_procq = sproc;
			INIT_LIST_HEAD(&new->s_procq);
			//new->s_link lo cambierà list_add() dopo.
			printf("\n new->s_link %d", new->s_link);
			printf("\n new->s_link.next %d", new->s_link.next);
			//e poi inserisce in ASL
			list_add(&new->s_link, asl_h);
			list_add(&p->p_list, &new->s_procq);
			return 0;
		} else return 1;
	}
	else {
		if (semAdd == NULL)
			printf("\nERRORE insertBlocked(semAdd, p)! semAdd = NULL!");
		if (p == NULL)
			printf("\nERRORE insertBlocked(semAdd, p)! p = NULL!");
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
    if (semAdd != NULL) {
		//cercare il semAdd (key) nella ASL
		struct list_head *iter;
		list_for_each(iter, asl_h){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				// se è stato trovato nella ASL
				printf("\n !semaforo trovato");
				if (&current->s_procq==current->s_procq.next){
					// controllo per verificare se la lista dei processi bloccati nel semaforo è vuota
					return NULL;
				}
				struct list_head *res = current->s_procq.next;
				printf("\n current->s_procq.next = %d ",current->s_procq.next);
				printf("\n current->s_procq.next->next = %d ",current->s_procq.next->next);
				printf("\n current->s_procq.next->next->next = %d ",current->s_procq.next->next->next);
				//list_del(current->s_procq.next);
				current->s_procq.next->next->prev = current->s_procq.next->prev;
    			current->s_procq.next->prev->next = current->s_procq.next->next;
				printf("\n current->s_procq.next = %d ",current->s_procq.next);
				printf("\n current->s_procq.next->next = %d ",current->s_procq.next->next);
				printf("\n current->s_procq.next->next->next = %d ",current->s_procq.next->next->next);
				// reinserimento nella coda dei processi liberi
				list_add(res, pcbFree_h);
				if (&current->s_procq==current->s_procq.next){
					// se la coda dei processi diventa vuota in seguito alla rimozione allora rimuovere semaforo dalla ASL
					list_del(&current->s_link);printf("\n !semaforo cancellato?");
					// reinserimento nella coda dei semafori liberi
					list_add(&current->s_link,semdFree_h);
				}
				return container_of(res, pcb_t, p_list);
			}
		}
	}
	else 
		printf("\nERRORE removeBlocked(int *semAdd)! semAdd = NULL!");
	// se semAdd = NULL oppure se non è stato trovato il semaforo con quello specifico semAdd
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
    if (p != NULL) {
		//cercare il p->p_semAdd (key) nella ASL
			int *semAdd = p->p_semAdd; 
		struct list_head *iter;
		list_for_each(iter, asl_h){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				// se è stato trovato nella ASL
				if (&current->s_procq==current->s_procq.next){
					// controllo per verificare se la lista dei processi bloccati nel semaforo è vuota
					return NULL;
				}
				struct list_head *iterpcb;
				list_for_each(iterpcb, &current->s_procq){
					// scorro dentro la lista dei processi bloccati
					if (&p->p_list==iterpcb){
						// se ho trovato il pcb con stesso p_list di p allora lo rimuovo
						list_del(iterpcb);
						// reinserimento nella coda dei processi liberi
						list_add(iterpcb, pcbFree_h);
						return p;
					}
				}
				if (&current->s_procq==current->s_procq.next){
					// se la coda dei processi diventa vuota in seguito alla rimozione allora rimuovere semaforo dalla ASL
					list_del(&current->s_link);
					// reinserimento nella coda dei semafori liberi
					list_add(&current->s_link,semdFree_h);
				}
				//se p non compare nella lista dei processi  bloccati
				return NULL;
			}
		}
	}
	else 
		printf("\nERRORE outBlocked(p)! p = NULL!");
	// se p = NULL oppure se non è stato trovato il semaforo con quello specifico semAdd
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
	if (semAdd != NULL) {
		//cercare il semAdd (key) nella ASL
		struct list_head *iter;
		list_for_each(iter, asl_h){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				// se è stato trovato nella ASL
				if (&current->s_procq==current->s_procq.next){
					// se la coda dei processi è vuota
					return NULL;
				}else 
					return (current->s_procq.next);
			}
		}
	}
	else 
		printf("\nERRORE semAddBlocked(int *semAdd)! semAdd = NULL!");
	//se semAdd = NULL oppure se non è stato trovato il semaforo con quello specifico semAdd
    	return NULL;
}

// ****** MAIN per DEBUG ******
///*
int main1() {
	int sem[MAXPROC]; //array di chiavi
	LIST_HEAD(semd);
	asl_h = &semd;
	initPcbs();
	initASL();
	printf("prima di alloc");
	//stampaLista(semdFree_h, "semd");
	pcb_t *p;// = allocPcb();
	int insertBlock = insertBlocked(&sem[0], p);
	printf("\ninsertBlock = %d\n", insertBlock);
	
}
int main2() {
	int sem[MAXPROC];
	//TODO: DA SPOSTARE in initASL()
	//inizializza la lista di semafori attivi
	LIST_HEAD(semd);
	asl_h = &semd;

	printf("\nInitializing ASL and PCBs...");
	initPcbs();
	initASL();
	printf("\n ...done!");
	printf("\nsemdFree_h %d", semdFree_h);
    printf("\nsemdfree_h->next %d", semdFree_h->next);
    printf("\nsemdfree_h->next->next %d", semdFree_h->next->next);
	printf("\ndopo init asl_h = %d",asl_h);
	printf("\ndopo init asl_h->next = %d",asl_h->next);
	stampaLista(semdFree_h, "semdfree");	

	pcb_t *p;
	pcb_t *p2;
	printf("\np = %d",p);
	printf("\npost alloc semdFree_h %d", semdFree_h);
	printf("\npost alloc semdfree_h->next %d", semdFree_h->next);
	printf("\npost alloc semdfree_h->next->next %d", semdFree_h->next->next);
	printf("\npost alloc semdfree_h->next->next->next %d", semdFree_h->next->next->next);

	printf("\n\n **Inserting...**");
	int insBlock = insertBlocked(&sem[0], p);
	printf("\n ...inserting done! %d", insBlock);
	printf("\n\n **Inserting...**");
	int insBlock2 = insertBlocked(&sem[1], p2);
	printf("\n ...inserting done! %d", insBlock);
	printf("\n");///*
	printf("\nsemdFree_h %d", semdFree_h);
    printf("\nsemdfree_h->next %d", semdFree_h->next);
   // printf("\nsemdfree_h->next->next %d", semdFree_h->next->next);
	//*/
	printf("\n\ndopo insert asl_h = %d",asl_h);
	printf("\ndopo insert asl_h->next = %d",asl_h->next);
	printf("\ndopo insert asl_h->next->next = %d",asl_h->next->next);
	printf("\n\n Remove...");
	p=removeBlocked(&sem[0]);
	printf("\n p=%d",p);
	printf("\n\n ...removing done");
	printf("\ndopo insert asl_h = %d",asl_h);
	printf("\ndopo insert asl_h->next = %d",asl_h->next);
	printf("\ndopo insert asl_h->next->next = %d",asl_h->next->next);
	printf("\n");
	return 0;
}
//*/
