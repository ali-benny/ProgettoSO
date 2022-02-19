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
	//inizializza la lista di semafori liberi
	LIST_HEAD(semdFree);
	semdFree_h=&(semdFree);
	for (int i=0; i<MAXPROC; i++){
	//	semd_table[i].id = i;	//* DEBUG
		list_add(&(semd_table[i].s_link), semdFree_h);
	}   
	//inizializza la lista di semafori attivi
	LIST_HEAD(semd);
	asl_h = &semd;
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
	if ((semAdd != NULL) && (p != NULL)) {
		//cercare il semAdd (key) nella ASL
		struct list_head *iter;
		int found=0;
		list_for_each(iter, asl_h){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				found = 1;
				p->p_semAdd=semAdd;
				list_add(&(p->p_list), &(current->s_procq));
				return 0;
			}
		}
		semd_PTR new = NULL;
		//se non è stato trovato nella ASL e la lista dei liberi non è vuota
		if ((found==0) && !(list_empty(semdFree_h))) {
			//alloca e inserisce
			new = container_of(semdFree_h->next, semd_t, s_link);
			list_del(semdFree_h->next);
			//setta i campi
			p->p_semAdd=semAdd;
			new->s_key = semAdd;
			//LIST_HEAD(sproc);
			//new->s_procq = sproc;
			INIT_LIST_HEAD(&(new->s_procq));
			
			//e poi inserisce in ASL
			list_add(&(new->s_link), asl_h);
			list_add(&(p->p_list), &(new->s_procq));
			return 0;
		} else return 1;
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
	
	return: primo pcb bloccato in asl_h con key = semAdd, NULL altrimenti
	semAdd: chiave associata ad un semaforo di asl_h
*/
pcb_t* removeBlocked(int *semAdd) {
    if (semAdd != NULL) {
		//cercare il semAdd (key) nella ASL
		struct list_head *iter;
		list_for_each(iter, asl_h){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				// se è stato trovato nella ASL
				if (&(current->s_procq)==(current->s_procq.next)){
					// controllo per verificare se la lista dei processi bloccati nel semaforo è vuota
					return NULL;
				}
				struct list_head *res = current->s_procq.next;
				
				//list_del(current->s_procq.next);
				current->s_procq.next->next->prev = current->s_procq.next->prev;
    			current->s_procq.next->prev->next = current->s_procq.next->next;
				
				// reinserimento nella coda dei processi liberi
				list_add(res, &(pcbFree_h));
				if (&(current->s_procq)==(current->s_procq.next)){
					// se la coda dei processi diventa vuota in seguito alla rimozione allora rimuovere semaforo dalla ASL
					list_del(&(current->s_link));
					// reinserimento nella coda dei semafori liberi
					list_add(&(current->s_link),semdFree_h);
				}
				return container_of(res, pcb_t, p_list);
			}
		}
	}
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
	
	return: pcb rimosso, NULL altrimenti
	p: pcb bloccato in un semaforo asl_h
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
				if (&(current->s_procq)==(current->s_procq.next)){
					// controllo per verificare se la lista dei processi bloccati nel semaforo è vuota
					return NULL;
				}
				struct list_head *iterpcb;
				list_for_each(iterpcb, &(current->s_procq)){
					// scorro dentro la lista dei processi bloccati
					if (&(p->p_list)==iterpcb){
						// se ho trovato il pcb con stesso p_list di p allora lo rimuovo
						list_del(iterpcb);
						// reinserimento nella coda dei processi liberi
						list_add(iterpcb, &(pcbFree_h));
						return p;
					}
				}
				if (&(current->s_procq)==(current->s_procq.next)){
					// se la coda dei processi diventa vuota in seguito alla rimozione allora rimuovere semaforo dalla ASL
					list_del(&(current->s_link));
					// reinserimento nella coda dei semafori liberi
					list_add(&(current->s_link),semdFree_h);
				}
				//se p non compare nella lista dei processi  bloccati
				return NULL;
			}
		}
	}
	return NULL;
}

/*	17
	Restituisce (senza rimuovere) il puntatore al PCB che
	si trova in testa alla coda dei processi associata al
	SEMD con chiave semAdd. Ritorna NULL se il SEMD
	non compare nella ASL oppure se compare ma la sua
	coda dei processi è vuota.
	
	return: pcb in testa ad asl_h con key = semAdd, NULL altrimenti
	semAdd: chiave associata ad un semaforo di asl_h
*/
pcb_t* headBlocked(int *semAdd) {
	if (semAdd != NULL) {
		//cercare il semAdd (key) nella ASL
		struct list_head *iter;
		list_for_each(iter, asl_h){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				// se è stato trovato nella ASL
				if (&(current->s_procq)==current->s_procq.next){
					// se la coda dei processi è vuota
					return NULL;
				}else 
					return (current->s_procq.next);
			}
		}
	}
	return NULL;
}

