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
#include "asl.h"

HIDDEN semd_t semd_table[MAXPROC];
HIDDEN struct list_head semdFree_h; //lista di semafori liberi
HIDDEN struct list_head asl_h;	 //lista di semafori attivi, utilizzati in questo momento

/**	Ricerca Semafori dato un certo semAdd in asl_h

 * @return 1 se e` vuota, 0 se ha qualcosa
*/
int BusySem(int *semAdd){
//cercare il semAdd (key) nella ASL
	struct list_head *iter;
	list_for_each(iter, &(asl_h)){
		semd_PTR current = container_of(iter, semd_t, s_link);
		if (current->s_key == semAdd){
			if (emptyProcQ(&current->s_procq)==1) return 1;
			else return 0;
		}
	}
	return 1;
}

#include "klog/klog.h"
/**	Stampa liste di Semafori

	@param head lista da stampare 

 */
void print_list(struct list_head *head){
	klog_print("[");
	struct list_head *iter;
	list_for_each(iter, head){
		semd_PTR current = container_of(iter, semd_t, s_procq);
		klog_print_hex((unsigned int)current);
	}
	klog_print("]\n");
}
void klog_list(){
	print_list(&asl_h);
}

/*	18
	Inizializza la lista dei semdFree in modo da contenere 
	tutti gli elementi della semdTable. 
	Questo metodo viene invocato una volta sola durante
	l’inizializzazione della struttura dati.

	return: void
*/
void initASL(){
	//inizializza la lista di semafori liberi
	INIT_LIST_HEAD(&(semdFree_h));
	for (int i=0; i<MAXPROC; i++){
		list_add_tail(&(semd_table[i].s_link), &(semdFree_h));
	}   
	//inizializza la lista di semafori attivi
	INIT_LIST_HEAD(&(asl_h));
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
		struct list_head *iter;
		int found=0;
		//cercare il semaforo con semAdd (key) nella ASL
		list_for_each(iter, &(asl_h)){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				//semaforo trovato, procedo con l'inserimento di p
				found = 1;
				p->p_semAdd=semAdd;
				list_add_tail(&(p->p_list), &(current->s_procq));
				return 0;
			}
		}
		semd_PTR new = NULL;
		//se non è stato trovato nella ASL e la lista dei liberi non è vuota
		if ((found==0) && !(list_empty(&(semdFree_h)))) {
			//alloca e inserisce
			new = container_of(semdFree_h.next, semd_t, s_link);
			list_del(semdFree_h.next);
			
			//setta i campi
			p->p_semAdd = semAdd;
			new->s_key = semAdd;
			INIT_LIST_HEAD(&(new->s_procq));
			
			//e poi inserisce in ASL
			list_add_tail(&(new->s_link), &(asl_h));
			list_add_tail(&(p->p_list), &(new->s_procq));
			return 0; //ho aggiunto con successo
		}
		//non e` stato trovato e non posso aggiungere semd ad ASL perché semdFree è vuota
		else return 1; 
	}
	// semAdd o p nulli
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
	semd_PTR semd = NULL;
	struct list_head *iter;
	if (semAdd != NULL && !list_empty(&(asl_h))){
		//cercare il semaforo con semAdd (key) nella ASL
		list_for_each(iter, &(asl_h)){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				//se ho trovato il semaforo con semAdd cercata
				semd = current;
			}
		}
	}
	if(semd!= NULL){// nella ASL
		//rimozione del primo processo bloccato dentro a temp
		pcb_PTR pcb = removeProcQ(&semd->s_procq);
		if (list_empty(&semd->s_procq)){
			//se temp non ha più processi bloccati allora lo elimino dalla lista degli attivi
			list_del(&semd->s_link);
			//lo metto nella lista dei semafori liberi
			list_add_tail(&semd->s_link, &(semdFree_h));
		}
		return pcb;
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
	if (p != NULL){
		int *semAdd = p->p_semAdd;
		semd_PTR semd;
		pcb_PTR pcb;
		struct list_head *iter;
		//cercare il semaforo con p->p_semAdd (key) nella ASL
		list_for_each(iter, &(asl_h)){
			semd_PTR current = container_of(iter, semd_t, s_link);			
			if (current->s_key == semAdd){
				//se ho trovato il semaforo
				semd = current;
			}
		}
		//procedo alla rimozione
		pcb = outProcQ(&semd->s_procq, p);
		if (list_empty(&semd->s_procq)){
			//se semaforo non ha più processi bloccati e lo elimino dalla lista degli attivi
			list_del(&semd->s_link);
			//lo metto nella lista dei semafori liberi
			list_add_tail(&semd->s_link, &(semdFree_h));
		}
		return pcb;
	}
	// p e` NULL
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
		list_for_each(iter, &(asl_h)){
			semd_PTR current = container_of(iter, semd_t, s_link);
			if (current->s_key == semAdd){
				// se è stato trovato nella ASL
				if (&(current->s_procq)==current->s_procq.next){
					// se la coda dei processi è vuota
					return NULL;
				}else 
					return (container_of(current->s_procq.next, pcb_t, p_list));
			}
		}
	}
	return NULL;
}

