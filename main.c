#include "utils.h"
#include "ASL.c"
#include "PCB.c"

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
	list_for_each(iter,head) {
		pcb_PTR current = container_of(iter, pcb_t, p_list);
		printf("\n%s indice= %d PCB id= %d", stampa, i, current->id);
		i=i+1;
	}*/
}

int main(){
	mainSemafori();
	printf("\nfine main\n");
	return 0;
}

void mainSemafori() {
	initASL();
	printf("\ninitASL done!");
	printf("\nasl_h %d", asl_h);
    printf("\nasl_h->next %d", asl_h->next);
	stampaLista(semdFree_h, "semdfree");
	printf("\n");
}

int mainTestPCB()
{
	initPcbs();
	printf("\npcbFree_h %d", pcbFree_h);
	printf("\npcbFree_h->next %d", pcbFree_h->next);
	printf("\npcbFree_h->next->next %d", pcbFree_h->next->next);
	mainListe();
	//mainAlberi();
	return 0;
}
int mainListe()
{
	stampaLista(pcbFree_h, "prima di alloc pcbFree_h =");
	pcb_PTR p = allocPcb();
	pcb_PTR p2 = allocPcb();
	pcb_PTR p3 = allocPcb();
	pcb_PTR p4 = allocPcb();
	printf("\n");
	printf("\npcbFree_h %d", pcbFree_h);
	printf("\npcbFree_h->next %d", pcbFree_h->next);
	printf("\npcbFree_h->next->next %d", pcbFree_h->next->next);
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

int mainAlberi()
{

	pcb_PTR p = allocPcb();	 // child
	pcb_PTR p2 = allocPcb(); // parent
	pcb_PTR p3 = allocPcb(); // second child
	pcb_PTR p4 = allocPcb(); // third child

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
	printf("\np2->p_child.next->next = %d", p2->p_child.next->next); //*/

	int empty2 = emptyChild(p2);

	printf("\ninsert child done! emptyChild = %d", empty2);

	//printf("\nremoveChild done! %d", removeChild(p2));
	printf("\noutChild done! %d", outChild(p));
	printf("\n");
	return 0;
}

