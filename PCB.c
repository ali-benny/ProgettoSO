/**
 * @file PCB.c
 * @author: alice benatti, alberto scuderi, gerald manzan, libera longo
 *  
 * @version 0.1
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 */

#include "PCB.h"
#include <stdio.h>

		//non va *sadge*
void stampaLista(struct list_head *head, char *stampa)
{
	/*struct list_head *tmp=head->next;
	for (int i=0;i<MAXPROC;i++){
		printf("\n %s %d %d", stampa, i, tmp);
		tmp=tmp->next;
	}*/
	
	int i=0;
	struct list_head *iter;
	list_for_each(iter,pcbFree_h) {
        printf("\n%s %d %d ", stampa, i,  &iter);
		i=i+1;
	}
	
}




/*  1-funziona
	Inizializza la lista pcbFree in modo da contenere tutti gli elementi della 
	pcbFree_table. 
	Questo metodo deve essere chiamato una volta sola in fase di 
	inizializzazione della struttura dati
*/
void initPcbs(){
	LIST_HEAD(pcbFree);
	pcbFree_h=&pcbFree;
	for (int i=0; i<MAXPROC; i++){
		//freePcb(&pcbFree_table[i]);
		list_add( &pcbFree_table[i].p_list, pcbFree_h);
	}
	printf("\npcbFree %d", &pcbFree);
	printf("\npcbFree_h %d", pcbFree_h);
	printf("\npcbFree_h->next %d", pcbFree_h->next);
	printf("\npcbFree_h->next->next %d", pcbFree_h->next->next);
}

/*  2-funziona
	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree_h)

	warning!
	! anche se è list_add_tail lo aggiunge al primo elemento BOH
*/
void freePcb(pcb_t * p){
	if(p != NULL){
		
		printf("\nciao");
		struct list_head *new =  &p->p_list;
		struct list_head *prev = pcbFree_h;printf("\nsono in listadd 1");
		struct list_head *next = pcbFree_h->next;printf("\nsono in listadd2");
		
		printf("\nsono in listadd next prev %d\n", &next);
		struct list_head *ppref = next->prev;
		ppref = new;printf("\nsono in listadd3");
		new->next  = next;printf("\nsono in listadd");
		new->prev  = prev;
		prev->next = new;
		//__list_add( &p->p_list, pcbFree_h, pcbFree_h->next);
		//list_add( &p->p_list, pcbFree_h);
		//printf("\nfatto listadd");
	}
	else 
        printf("\nERRORE freePcb! p = NULL!");
}

/*  3-funziona
	Restituisce NULL se la pcbFree_h è vuota. 
	Altrimenti rimuove un elemento dalla 
	pcbFree, inizializza tutti i campi (NULL/0) 
	e restituisce l’elemento rimosso.
*/
pcb_t *allocPcb(){
	pcb_PTR resPcb = NULL;
	if ( !(list_empty(pcbFree_h))) {
		struct list_head *tmp=pcbFree_h->next;
		printf("\nstarted alloc...");
		resPcb = container_of(tmp, pcb_t, p_list); //! warning container_of
		printf("\nresPcb: %d",resPcb );
		// rimuovi elemento resPcb da pcbFree_h
		list_del(pcbFree_h->next);		
		//DOPO AVERLO RIMOSSO possiamo settare i campi
		resPcb->p_list.next = NULL;
		resPcb->p_list.prev = NULL;
		resPcb->p_parent = NULL;
		resPcb->p_child.next = NULL;
		resPcb->p_child.prev = NULL;
		resPcb->p_sib.next = NULL;
		resPcb->p_sib.prev = NULL;
		resPcb->p_semAdd = NULL;
		// per test umps
#ifdef UMPS3
		resPcb->p_s.entry_hi = 0;
		resPcb->p_s.cause = 0;
		resPcb->p_s.status = 0;
		resPcb->p_s.pc_epc = 0;
		resPcb->p_s.gpr[STATE_GPR_LEN] = 0;
		resPcb->p_s.hi = 0;
		resPcb->p_s.lo = 0;

		resPcb->p_time = 0;
#endif
	}
	return resPcb;
}

/*  4
	Crea una lista di PCB, inizializzandola come lista vuota
*/
void mkEmptyProcQ(struct list_head *head){
    if (head != NULL)
	    INIT_LIST_HEAD(head);
    else
        printf("\nERRORE emptyProcQ! head = NULL!");
}

/*  5
	Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti.
*/
int emptyProcQ(struct list_head *head){
    if (head != NULL)
	    return (list_empty(head));
    else{
        printf("\nERRORE emptyProcQ! head = NULL!");
		return 0;
    }
}

/*  6
	Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
*/
void insertProcQ(struct list_head* head, pcb_t* p){
	if(head != NULL && p != NULL)
	    list_add_tail(&p->p_list, head);
	else{
		if(p == NULL)
			printf("\nERRORE insertProcQ(head, p)! p = NULL!");
		if(head == NULL)
	    	printf("\nERRORE insertProcQ(head, p)! head = NULL!");
    }
}

/*  7
	Restituisce l’elemento di testa della coda dei processi da head //!(sentinella), SENZA RIMUOVERLO. 
	Ritorna NULL se la coda non ha elementi.
*/
pcb_t * headProcQ(struct list_head* head){
	if(head != NULL){
		if (!list_empty(head))
			return container_of(list_next(head), pcb_t, p_list);
		else return NULL;
    }
    else{
        printf("\nERRORE headProcQ! head == NULL");
        return NULL;
    }
}

/*  8
	Rimuove il primo elemento dalla coda dei 
	processi puntata da head. Ritorna NULL se la 
	coda è vuota. Altrimenti ritorna il puntatore 
	all’elemento rimosso dalla lista.
*/
pcb_t* removeProcQ(struct list_head* head){
	if(head!=NULL) {

	}
	else
		printf("\nErrore removeProcQ! head = NULL!");

}

/*  9
	Rimuove il PCB puntato da p dalla coda dei 
	processi puntata da head. Se p non è presente 
	nella coda, restituisce NULL. (NOTA: p può 
	trovarsi in una posizione arbitraria della coda).
*/
pcb_t* outProcQ(struct list_head* head, pcb_t *p){
	if(head != NULL && p != NULL) {

	}
	else {
		if(head == NULL)
			printf("\nErrore outProcQ! head = NULL!");
		if(p == NULL)
			printf("\nErrore outProcQ! p = NULL!");
	}
		
}
// ---- Alberi di PCB ----
/*  10
	Restituisce TRUE se il PCB puntato da p 
	non ha figli, FALSE altrimenti.
*/
int emptyChild(pcb_t *p){
	if(p != NULL) {

	} 
	else{
		printf("\nErrore emptyChild! p = NULL!");
	}
}

/*  11
	Inserisce il PCB puntato da p come figlio del PCB puntato da prnt.
*/
void insertChild(pcb_t *prnt,pcb_t *p){
	if(prnt != NULL && p != NULL) {
		
	}
	else{
		if(prnt == NULL)
			printf("\nErrore insertChild! prnt = NULL!");
		if(p == NULL)
			printf("\nErrore insertChild! p = NULL!");
	}
}

/*  12
	Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL.
*/
pcb_t* removeChild(pcb_t *p) {
	if(p != NULL) {
		
	}
	else
		printf("\nErrore removeChild! p = NULL!");
}

/*  13
	Rimuove il PCB puntato da p dalla lista dei figli del padre. 
	Se il PCB puntato da 
	p non ha un padre, restituisce NULL, 
	altrimenti restituisce l’elemento 
	rimosso (cioè p). A differenza della 
	removeChild, p può trovarsi in una 
	posizione arbitraria (ossia non è 
	necessariamente il primo figlio del 
padre).
*/
pcb_t *outChild(pcb_t* p) {
	if(p != NULL) {
		
	} else
		printf("\nErrore outChild! p = NULL!");
}

int main(){
	initPcbs();
	pcb_PTR p = allocPcb();
	
	printf("\np: %d", p);
	printf("\nalloc done!");
	
	LIST_HEAD(list); //usa questo per dichiarare le list_head che ti servono
	///*	
	//stampaLista(pcbFree_h, "prefree");
	struct list_head *tmp=pcbFree_h->next;
	for (int i=0;i<MAXPROC;i++){
		printf("\n prefree %d %d", i, tmp);
		tmp=tmp->next;
	}
	freePcb(p);
	//stampaLista(pcbFree_h, "postfree");
	tmp=pcbFree_h->next;
	for (int i=0;i<MAXPROC;i++){
		printf("\n postfree %d %d", i, tmp);
		tmp=tmp->next;
	}
	//*/
   	mkEmptyProcQ(&list);
  	int empty = emptyProcQ(&list);
    printf("\nlista e' vuota %d", (int) empty);
	insertProcQ(&list, p);
	printf("\ninsertProc done!");
	headProcQ(&list);
	printf("\nheadProc done!");
    printf("\n");
	return 0;
}
