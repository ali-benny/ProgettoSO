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

/* ****************************************************************************** */

/*  1-funziona
	Inizializza la lista pcbFree in modo da contenere tutti gli elementi della 
	pcbFree_table. 
	Questo metodo deve essere chiamato una volta sola in fase di 
	inizializzazione della struttura dati
*/
void initPcbs(){
	LIST_HEAD(pcbFree);
	pcbFree_h = &pcbFree;
	for (int i = 0; i < MAXPROC; i++){
		pcbFree_table[i].id = i; //* DEBUG
		//freePcb(&pcbFree_table[i]);
		list_add(&pcbFree_table[i].p_list, pcbFree_h);
	}
	/*printf("\npcbFree %d", &pcbFree);
	printf("\npcbFree_h %d", pcbFree_h);
	printf("\npcbFree_h->next %d", pcbFree_h->next);
	printf("\npcbFree_h->next->next %d", pcbFree_h->next->next);*/
}

/*  2-funziona
	Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree_h)

	warning!
	! anche se è list_add_tail lo aggiunge al primo elemento BOH
*/
void freePcb(pcb_t *p){
	if (p != NULL)
	{
		/*
		pcbFree_h->next->prev = &p->p_list;
		p->p_list.next  = pcbFree_h->next;
		p->p_list.prev  = pcbFree_h;		
		pcbFree_h->next = &(p->p_list);
		*/
		list_add(&p->p_list, pcbFree_h);
	}
	else
		printf("\nERRORE freePcb! p = NULL!");
}

/*  3-funziona!! su linux ma non su visual studio perchè WINDOWS FA SCHIFO! NOOO aLice si dissocia
	Restituisce NULL se la pcbFree_h è vuota. 
	Altrimenti rimuove un elemento dalla 
	pcbFree, inizializza tutti i campi (NULL/0) 
	e restituisce l’elemento rimosso.
*/
pcb_t *allocPcb(){
	pcb_PTR resPcb = NULL;
	if (pcbFree_h!=pcbFree_h->next){
		//	printf("\nstarted alloc...");	//* DEBUG
		resPcb = container_of(pcbFree_h->next, pcb_t, p_list); //! warning container_of
		//	printf("\nresPcb: %d",resPcb );	//* DEBUG

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
void mkEmptyProcQ(struct list_head *head)
{
	if (head != NULL)
		INIT_LIST_HEAD(head);
	else
		printf("\nERRORE emptyProcQ! head = NULL!");
}

/*  5
	Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti.
*/
int emptyProcQ(struct list_head *head)
{
	if (head != NULL)
		return (list_empty(head));
	else
	{
		printf("\nERRORE emptyProcQ! head = NULL!");
		return 0;
	}
}

/*  6
	Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
*/
void insertProcQ(struct list_head *head, pcb_t *p)
{
	if (head != NULL && p != NULL)
		list_add_tail(&p->p_list, head);
	else
	{
		if (p == NULL)
			printf("\nERRORE insertProcQ(head, p)! p = NULL!");
		if (head == NULL)
			printf("\nERRORE insertProcQ(head, p)! head = NULL!");
	}
}

/*  7
	Restituisce l’elemento di testa della coda dei processi da head //!(sentinella), SENZA RIMUOVERLO. 
	Ritorna NULL se la coda non ha elementi.
*/
pcb_t *headProcQ(struct list_head *head)
{
	if (head != NULL)
	{
		if (!list_empty(head))
			return container_of(list_next(head), pcb_t, p_list);
		else
			return NULL;
	}
	else
	{
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
pcb_t *removeProcQ(struct list_head *head)
{
	if (head != NULL)
	{
		pcb_PTR res = NULL;
		if (!list_empty(head))
		{
			res = container_of(head->next, pcb_t, p_list);
			list_del(&res->p_list);
		}
		return res;
	}
	else
	{
		printf("\nErrore removeProcQ! head = NULL!");
		return NULL;
	}
}

/*  9
	Rimuove il PCB puntato da p dalla coda dei 
	processi puntata da head. Se p non è presente 
	nella coda, restituisce NULL. (NOTA: p può 
	trovarsi in una posizione arbitraria della coda).
*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p)
{
	if (head != NULL && p != NULL)
	{
		if (!list_empty(head))
		{
			struct list_head *iter;
			pcb_PTR current;
			list_for_each(iter, head)
			{
				current = container_of(iter, pcb_t, p_list);
				if (current == p)
				{
					list_del(&current->p_list);
					return current;
				}
			}
		}
		else
			return NULL;
	}
	else
	{
		if (head == NULL)
			printf("\nErrore outProcQ! head = NULL!");
		if (p == NULL)
			printf("\nErrore outProcQ! p = NULL!");
		return NULL;
	}
}

// ---- Alberi di PCB ----
/*  10
	Restituisce TRUE se il PCB puntato da p 
	non ha figli, FALSE altrimenti.
*/
int emptyChild(pcb_t *p)
{
	if (p != NULL)
	{
		if (p->p_child.next == NULL)
			return 0;
		else
			return 1;
	}
	else
	{
		printf("\nErrore emptyChild! p = NULL!");
		return 0;
	}
}

/*  11
	Inserisce in coda il PCB puntato da p come figlio del PCB puntato da prnt.
	Quindi: inserisce p->p_sib in prnt->p_child
	
	return: void
	prnt: daddy
	p: child

*/
void insertChild(pcb_t *prnt, pcb_t *p)
{
	if (prnt != NULL && p != NULL)
	{
		p->p_parent = prnt;
		if (prnt->p_child.next == NULL || prnt->p_child.prev == NULL)
		{
			INIT_LIST_HEAD(&prnt->p_child); // mette una sentinella a p_child
			/*//* DEBUG
			printf("\n &p->p_sib doprimapo %d",&p->p_sib);	
			printf("\n &prnt->p_child prima %d",&prnt->p_child);
			printf("\n prnt->p_child.next prima %d",prnt->p_child.next);*/
		}
		//* DEBUG
		/*(prnt->p_child.next)->prev = &p->p_sib;
		p->p_sib.next  = &prnt->p_child.next;
		p->p_sib.prev  = &prnt->p_child;
		prnt->p_child.next = &p->p_sib;*/

		list_add_tail(&p->p_sib, &prnt->p_child);
		//* DEBUG
		/*printf("\n\n &p->p_sib dopo %d",&p->p_sib);
		printf("\n &prnt->p_child dopo %d",&prnt->p_child);
		printf("\n prnt->p_child.next dopo %d",prnt->p_child.next);*/
	}
	else
	{
		if (prnt == NULL)
			printf("\nErrore insertChild! prnt = NULL!");
		if (p == NULL)
			printf("\nErrore insertChild! p = NULL!");
	}
}

/*  12
	Rimuove il primo figlio del PCB puntato da p. 
	Se p non ha figli, restituisce NULL.

	return: puntatore all'elemanto rimosso
*/
pcb_t *removeChild(pcb_t *p)
{
	if (p != NULL)
	{
		if (list_empty(&p->p_child))
			return NULL;
		else
		{
			pcb_t *res = container_of(p->p_child.next, pcb_t, p_sib);
			list_del(p->p_child.next);
			printf("\nres.id = %d", res->id);
			return res;
		}
	}
	else
	{
		printf("\nErrore removeChild! p = NULL!");
		return NULL;
	}
}

/*  13
	Rimuove il PCB puntato da p dalla lista dei figli del padre. 
	Se il PCB puntato da p non ha un padre, restituisce NULL, 
	altrimenti restituisce l’elemento rimosso (cioè p). 

	A differenza della removeChild, p può trovarsi in una posizione 
	arbitraria (ossia non è necessariamente il primo figlio del padre).
	
	return: 
	p: 
*/
pcb_t *outChild(pcb_t *p)
{
	if (p != NULL){
		pcb_PTR daddy = p->p_parent;
		if (daddy != NULL)
		{
			//cercare p nella lista dei figli di daddy (daddy->p_child)
			struct list_head *child = &p->p_sib;
			struct list_head *iteratore;
			/*
			struct list_head *tmp=daddy->p_child.next;
			while (tmp!=&daddy->p_child){
				if (child==tmp){
					pcb_t *res = container_of(tmp, pcb_t, p_sib);
					list_del(tmp);
					return res;
				}
				tmp=tmp->next;
			}
			*/
			//scorro la lista dei figli
			list_for_each(iteratore, &daddy->p_child)
			{
				if (iteratore == child)
				{ //se ho trovato p
					pcb_t *res = container_of(iteratore, pcb_t, p_sib);
					list_del(iteratore); //elimino p dalla lista
					printf("\n res.id = %d", res->id);
					return res; //e ritorno
				}
			}
			printf("\n p non trovato");
			return NULL;
		}
		else
		{
			printf("\nErrore daddy = NULL");
			return NULL;
		}
	}
	else
	{
		printf("\nErrore outChild! p = NULL!");
		return NULL;
	}
}


// ****** MAIN per DEBUG ******
/*
int mainListe(){
	//stampaLista(pcbFree_h, "prima di alloc pcbFree_h =");
	pcb_PTR p = allocPcb();
	pcb_PTR p2 = allocPcb();
	pcb_PTR p3 = allocPcb();
	pcb_PTR p4 = allocPcb();
	printf("\n");
	printf("\npcbFree_h %d", pcbFree_h);
	printf("\npcbFree_h->next %d",list_next(pcbFree_h));
	printf("\npcbFree_h->next->next %d",list_next(list_next(pcbFree_h)));
	//	pcb_PTR p5 = allocPcb();
	printf("\np: %d", p);
	printf("\np2: %d", p2);
	printf("\np3: %d", p3);
	printf("\np4: %d", p4);
	//  printf("\np5: %d", p5);
	printf("\nalloc done!");

	//stampaLista(pcbFree_h, "dopo alloc pcbFree_h =");

	LIST_HEAD(list); //usa questo per dichiarare le list_head che ti servono
	stampaLista(pcbFree_h, "prefree");
	//freePcb(p);
	stampaLista(pcbFree_h, "postfree");

	mkEmptyProcQ(&list);
	int empty = emptyProcQ(&list);
	printf("\nlista e' vuota %d", (int)empty);
	stampaLista(&list, "lista appena creata vuota");

	insertProcQ(&list, p);
	insertProcQ(&list, p2);
	insertProcQ(&list, p3);
	insertProcQ(&list, p4);
	printf("\ninsertProc done!");
	stampaLista(&list, "lista con un elemento in piu'");

	//removeProcQ(&list);
	//printf("\nremoveProc done!");

	outProcQ(&list, p2);
	printf("\ndopo out proc q");
	stampaLista(&list, "lista con un elemento in meno");

	printf("\nheadlist %d", &headProcQ(&list)->p_list);
	printf("\nheadProc done!");
	printf("\n");
	return 0;
}
int mainAlberi(){
	stampaLista(pcbFree_h, "prima di alloc pcbFree_h =>");
	pcb_PTR p = allocPcb();	 // child
	stampaLista(pcbFree_h, "dopo di alloc PRIMO pcbFree_h =>");
	pcb_PTR p2 = allocPcb(); // parent
	stampaLista(pcbFree_h, "dopo di alloc SECONDO pcbFree_h =>");
	pcb_PTR p3 = allocPcb(); // second child
	stampaLista(pcbFree_h, "dopo di alloc TERZO pcbFree_h =>");
	pcb_PTR p4 = allocPcb(); // third child
	stampaLista(pcbFree_h, "dopo di alloc QUARTO pcbFree_h =>");

	printf("\np (child1) = %d p.id = %d", p, p->id);
	printf("\np2 (parent) = %d p2.id = %d", p2, p2->id);
	printf("\np3 (child2) = %d p3.id = %d", p3, p3->id);
	printf("\np4 (child3) = %d p4.id = %d", p4, p4->id);

	int empty = emptyChild(p);
	printf("\nempty child done! empty = %d", empty);

	insertChild(p2, p);
	insertChild(p2, p3);
	insertChild(p2, p4);

	printf("\np (child)= %d", p);
	printf("\np3 (child2)= %d", p3);
	printf("\np2 (parent) = %d", p2);
	printf("\ninsert child done!\n");
	printf("\np2->p_child = %d", &p2->p_child);
	printf("\np->p_sib = %d", &p->p_sib);
	printf("\np3->p_sib = %d", &p3->p_sib);
	printf("\np2->p_child.next = %d", p2->p_child.next);
	printf("\np2->p_child.next->next = %d", p2->p_child.next->next);

	int empty2 = emptyChild(p2);

	printf("\ninsert child done! emptyChild = %d", empty2);

	//printf("\nremoveChild done! %d", removeChild(p2));
	printf("\noutChild done! %d", outChild(p));
	printf("\n");
	return 0;
}
int main(){
	initPcbs();	
	printf("\npcbFree_h %d", pcbFree_h);
	printf("\npcbFree_h->next %d", pcbFree_h->next);
	printf("\npcbFree_h->next->next %d", pcbFree_h->next->next);
	//mainListe();
	mainAlberi();
	return 0;
}
*/