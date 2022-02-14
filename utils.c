/**
 * @file utils.c
 * @authors: alice benatti, alberto scuderi, gerald manzano, libera longo
 * 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "utils.h"

/*
	Funzione stampa lista realizzata da noi

	head: lista da stampare
	stampa: descrizione della stampa
	return: void
*/
void stampaLista(struct list_head *head, char *stampa){
	printf("\ninterno di stampaLista");
	/*
	int i = 0;
	if(head->next != NULL) {
		struct list_head *tmp = head->next;
		//for(int i=0;list_is_last(tmp, head);i++)
		while (tmp != head){	
			if (tmp==NULL)printf("\ntmp NULL");//* DEBUG
			if (tmp->prev==NULL)printf("\ntmp->prev NULL");	//* DEBUG
			printf("\n %s %d %d", stampa, i, tmp);
			if (tmp->next==NULL)printf("\ntmp->next NULL %d", tmp->next);	//* DEBUG
			//printf("\np_list %d", &container_of(tmp, pcb_t, p_list)->p_list);
			printf("\ntmp->prev %d", tmp->prev);
			tmp = tmp->next;printf("\nho fatto tmp = tmp->next");
			i++;
		}
	} else printf("\nhead->next != NULL something get wrong!");
	*/
	 ///*
	int i=0;
	struct list_head *iter;	list_for_each(iter, head) {
		if(iter != NULL) {
			pcb_PTR current = container_of(iter, pcb_t, p_list);
			printf("\n%s indice= %d PCB id= %d", stampa, i, current->id);
			i=i+1;
		} else printf("head null");
	}//*/
}