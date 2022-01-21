#include "PCB.h"

/* 
    Inizializza la lista pcbFree in modo da contenere tutti gli elementi della 
    pcbFree_table. 
    Questo metodo deve essere chiamato una volta sola in fase di 
    inizializzazione della struttura dati
*/
void initPcbs(){
    LIST_HEAD(pcbFree_h);
    for (int i=0; i<MAXPROC; i++){
        list_add( &pcbFree_table[i].p_list, &pcbFree_h);
    }
}

/*  2
    Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree_h)
*/
void freePcb(pcb_t * p){
    list_add( &p->p_list, &pcbFree_h);
}

/*  3
    Restituisce NULL se la pcbFree_h è vuota. 
    Altrimenti rimuove un elemento dalla 
    pcbFree, inizializza tutti i campi (NULL/0) 
    e restituisce l’elemento rimosso.
*/
pcb_t *allocPcb(){
    pcb_PTR resPcb = NULL;
    if ( !( list_empty( &pcbFree_h ))) {
        struct list_head * res_head = list_next(&pcbFree_h);
        if(res_head != NULL) {
            pcb_PTR resPcb = container_of(res_head, pcb_t, p_list);
        }
    }
    return resPcb;
}

/*  4
    Crea una lista di PCB, inizializzandola come lista vuota
*/
void mkEmptyProcQ(struct list_head *head){}

/*  5
    Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti.
*/
int emptyProcQ(struct list_head *head);

/*  6
    Inserisce l’elemento puntato da p nella 
    coda dei processi puntata da head.
*/
void insertProcQ(struct list_head* head, pcb_t* p);

/*  7
    Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO. 
    Ritorna NULL se la coda non ha elementi.
*/
pcb_t headProcQ(struct list_head* head);

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
    pcb_t * p;
    initPcbs();
    freePcb(p);
} 