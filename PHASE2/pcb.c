/**
 * @file PCB.c
 * @authors: alice benatti, alberto scuderi, gerald manzano, libera longo
 *  
 * @version 1.0
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 */

#include "pcb.h"

HIDDEN pcb_t pcbFree_table[MAXPROC];
HIDDEN struct list_head pcbFree_h;

/* ****************************************************************************** */

/*  1
	Inizializza la lista pcbFree in modo da contenere tutti gli elementi della 
	pcbFree_table. 
	Questo metodo deve essere chiamato una volta sola in fase di 
	inizializzazione della struttura dati
*/
void initPcbs(){
	// inizializzo pcbFree_h
	INIT_LIST_HEAD(&(pcbFree_h));
	// inserisco i pcb di pcbFree_table
	for (int i = 0; i < MAXPROC; i++){
		freePcb(&pcbFree_table[i]);
	}
}

/*  2
	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree_h)
	
	p: pcb da inserire in pcbFree_h
*/
void freePcb(pcb_t *p){
	list_add_tail(&(p->p_list), &(pcbFree_h));
}

/*  3-funziona davvero!!
	Restituisce NULL se la pcbFree_h è vuota. 
	Altrimenti rimuove un elemento dalla 
	pcbFree, inizializza tutti i campi (NULL/0) 
	e restituisce l’elemento rimosso.
	
	return: pcb rimosso da pcbFree_h, NULL altrimenti
*/
pcb_t *allocPcb(){
	if (list_empty(&(pcbFree_h)))return NULL;
	else{
		// salvo il pcb in resPcb
		pcb_t *resPcb = container_of(pcbFree_h.next, pcb_t, p_list);
		
		// rimuovi elemento appena salvato da pcbFree_h
		list_del(pcbFree_h.next);
		
		//DOPO AVERLO RIMOSSO possiamo settare i campi
		resPcb->p_parent = NULL;
		resPcb->p_semAdd = NULL;
		INIT_LIST_HEAD(&(resPcb->p_list));
		INIT_LIST_HEAD(&(resPcb->p_child));
		INIT_LIST_HEAD(&(resPcb->p_sib));
		
		// UMPS3
		resPcb->p_s.entry_hi = 0;
		resPcb->p_s.cause = 0;
		resPcb->p_s.status = 0;
		resPcb->p_s.pc_epc = 0;
		for(int i = 0; i < STATE_GPR_LEN; i++)
			resPcb->p_s.gpr[i] = 0;
		resPcb->p_s.hi = 0;
		resPcb->p_s.lo = 0;

		resPcb->p_time = 0;
		return resPcb;
	}
}

/*  4
	Crea una lista di PCB, inizializzandola come lista vuota
	
	head: lista da inizializzare
*/
void mkEmptyProcQ(struct list_head *head){
	INIT_LIST_HEAD(head);
}

/*  5
	Restituisce TRUE (=1) se la lista puntata da head è vuota, FALSE altrimenti.
	
	return: 1 se head NULL o vuota, 0 altrimenti
	head: lista da controllare
*/
int emptyProcQ(struct list_head *head){
	return list_empty(head);
}

/*  6
	Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
	
	head: lista in cui inserire p
	p: pcb da inserire in head
*/
void insertProcQ(struct list_head *head, pcb_t *p){
	list_add_tail(&(p->p_list), head);
}

/*  7
	Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO. 
	Ritorna NULL se la coda non ha elementi.
	
	return: primo pcb dopo la sentinella, NULL altrimenti
	head: lista dei pcb
*/
pcb_t *headProcQ(struct list_head *head){
	if (list_empty(head))return NULL;
	return container_of(list_next(head), pcb_t, p_list);
}

/*  8
	Rimuove il primo elemento dalla coda dei 
	processi puntata da head. Ritorna NULL se la 
	coda è vuota. Altrimenti ritorna il puntatore 
	all’elemento rimosso dalla lista.
	
	return: puntatore dell'elemento rimosso, NULL altrimenti
	head: lista da cui rimuovere il pcb dalla testa (escludendo la sentinella)
*/
pcb_t *removeProcQ(struct list_head *head){
	if (head != NULL){
		pcb_PTR res = NULL;
		if (!list_empty(head)){
			//rimozione del primo processo della lista puntata da head (quello successivo alla sentinella)
			res = container_of(head->next, pcb_t, p_list);
			list_del(&(res->p_list));
		}
		return res; // ritorno il pcb rimosso oppure NULL se head è una lista vuota
	}
	return NULL; // head è NULL
}

/*  9
	Rimuove il PCB puntato da p dalla coda dei 
	processi puntata da head. Se p non è presente 
	nella coda, restituisce NULL. (NOTA: p può 
	trovarsi in una posizione arbitraria della coda).
	
	return: elemento rimosso (=p), NULL altrimenti
	head: lista in cui cercare p
	p: pcb da rimuovere
*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p){
	if (head != NULL && p != NULL){
		if (!list_empty(head)){
			struct list_head *iter;
			pcb_PTR current;
			// cerco p nella lista head
			list_for_each(iter, head){
				current = container_of(iter, pcb_t, p_list);
				if (current == p){
					// ho trovato p e lo rimuovo
					list_del(&(current->p_list));
					return current; // ritorno il pcb rimoosso
				}
			}
		}
	} return NULL;
}

// ---- Alberi di PCB ----
/*  10
	Restituisce TRUE se il PCB puntato da p 
	non ha figli, FALSE altrimenti.
	
	return: 1 se p non ha figli, 0 altrimenti
	p: pcb da controllare
*/
int emptyChild(pcb_t *p){
	return list_empty(&(p->p_child));
}

/*  11
	Inserisce in coda il PCB puntato da p come figlio del PCB puntato da prnt.
	Quindi: inserisce p->p_sib in prnt->p_child
	
	prnt: pcb padre
	p: pcb figlio, da inserire in prnt

*/
void insertChild(pcb_t *prnt, pcb_t *p){
	if (prnt != NULL && p != NULL){
		p->p_parent = prnt;
		// se prnt child non e` inizializzata (dovrebbe gia` essere inizializzato in allocPcb())
		if (prnt->p_child.next == NULL || prnt->p_child.prev == NULL){
			INIT_LIST_HEAD(&(prnt->p_child)); // mette una sentinella a p_child
		}
		// aggiungo child nella coda dei figli (con i fratelli)
		list_add_tail(&(p->p_sib), &(prnt->p_child));
	}
}

/*  12
	Rimuove il primo figlio del PCB puntato da p. 
	Se p non ha figli, restituisce NULL.

	return: puntatore all'elemanto rimosso, NULL altrimenti
	p: pcb da cui rimuovere il primo figlio
*/
pcb_t *removeChild(pcb_t *p){
	if (p != NULL){
		// controllo se p ha figli
		if (!list_empty(&(p->p_child))){
			pcb_t *res = container_of(p->p_child.next, pcb_t, p_sib);
			list_del(p->p_child.next);
			return res;
		}
	}
	return NULL;
}

/*  13
	Rimuove il PCB puntato da p dalla lista dei figli del padre. 
	Se il PCB puntato da p non ha un padre, restituisce NULL, 
	altrimenti restituisce l’elemento rimosso (cioè p). 

	A differenza della removeChild, p può trovarsi in una posizione 
	arbitraria (ossia non è necessariamente il primo figlio del padre).
	
	return: pcb rimosso, NULL altrimenti
	p: pcb figlio di un ulteriore pcb
*/
pcb_t *outChild(pcb_t *p){
	if (p != NULL){
		pcb_PTR daddy = p->p_parent; // padre di p
		if (daddy != NULL){
			//cercare p nella lista dei figli di daddy (daddy->p_child)
			struct list_head *child = &(p->p_sib);
			struct list_head *iteratore;
			
			//scorro la lista dei figli
			list_for_each(iteratore, &(daddy->p_child)){
				if (iteratore == child){ //se ho trovato p
					pcb_t *res = container_of(iteratore, pcb_t, p_sib);
					list_del(iteratore); //elimino p dalla lista
					return res; //e ritorno
				}
			}
		}
	}
	return NULL;
}

