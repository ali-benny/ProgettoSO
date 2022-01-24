#include "PCB.h"
#include <stdio.h>


/*  1-funziona
	Inizializza la lista pcbFree in modo da contenere tutti gli elementi della 
	pcbFree_table. 
	Questo metodo deve essere chiamato una volta sola in fase di 
	inizializzazione della struttura dati
*/
void initPcbs(){
	INIT_LIST_HEAD(&pcbFree_h);
	for (int i=0; i<MAXPROC; i++){
		list_add( &pcbFree_table[i].p_list, &pcbFree_h);
	}
}

/*  2-funziona
	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree_h)
*/
void freePcb(pcb_t * p){
	if(p != NULL)
		list_add( &p->p_list, &pcbFree_h);
	else 
        printf("ERRORE freePcb! p = NULL!");
}

/*  3
	Restituisce NULL se la pcbFree_h è vuota. 
	Altrimenti rimuove un elemento dalla 
	pcbFree, inizializza tutti i campi (NULL/0) 
	e restituisce l’elemento rimosso.
*/
pcb_t *allocPcb(){
	pcb_PTR resPcb = NULL;
	if ( !(list_empty(&pcbFree_h))) {
		pcb_PTR resPcb = container_of(list_next(&pcbFree_h), pcb_t, p_list); //! warning container_of
		resPcb->p_list.next = NULL;
		resPcb->p_list.prev = NULL;
		resPcb->p_parent = NULL;
		resPcb->p_child.next = NULL;
		resPcb->p_child.prev = NULL;
		resPcb->p_sib.next = NULL;
		resPcb->p_sib.prev = NULL;
		resPcb->p_semAdd = NULL;
		// da decommentare per test umps
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
pcb_t* removeProcQ(struct list_head* head);

/*  9
	Rimuove il PCB puntato da p dalla coda dei 
	processi puntata da head. Se p non è presente 
	nella coda, restituisce NULL. (NOTA: p può 
	trovarsi in una posizione arbitraria della coda).
*/
pcb_t* outProcQ(struct list_head* head, pcb_t *p);

// ---- Alberi di PCB ----
/*  10
	Restituisce TRUE se il PCB puntato da p 
	non ha figli, FALSE altrimenti.
*/
int emptyChild(pcb_t *p); 

/*  11
	Inserisce il PCB puntato da p come figlio del PCB puntato da prnt.
*/
void insertChild(pcb_t *prnt,pcb_t *p);

/*  12
	Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL.
*/
pcb_t* removeChild(pcb_t *p);

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
pcb_t *outChild(pcb_t* p);

int main(){
	initPcbs();
    pcb_PTR p = allocPcb();
    //pcb_PTR p = &pcbFree_table[0];
	printf("\np: %d", p);
	for (int i = 0; i<MAXPROC; i++ )
		printf("\n pcbFree_table [%d]: %d",i,pcbFree_table[i]);	
	printf("\nalloc");
	//freePcb(p);
	LIST_HEAD(list); //usa questo per dichiarare le list_head che ti servono
	struct list_head *head = &list;
	printf("\nhead = %d, \nlist = %d", head, &list);
   // mkEmptyProcQ(head);
  //  int empty = emptyProcQ(head);
  //  printf("\nlista e' vuota %d", (int) empty);
	insertProcQ(&head, p);
	printf("\ninsertProc");
	headProcQ(&head);
	printf("\nheadProc");
	return 0;
}