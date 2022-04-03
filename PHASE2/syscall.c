/**
 * @file syscall.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *   
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "syscall.h"

#define SYS_DEBUG //per debuggare le sistemcall

//richiami delle variabili globali di main.c
extern list_head high_priority_q;
extern list_head low_priority_q;
extern pcb_PTR current_process;
extern int process_count;
extern int soft_block_count; 
extern pcb_PTR current_process;

// *** Auxiliar Functions ***

void auxiliary_terminate(pcb_PTR current); //terminate
void P_operation(int *semaddr);	//Passeren, Wait for clock

state_t* state_reg;

void syscall_handler(state_t* state){
	state_reg = state;
	state_reg->pc_epc += 4; //incrementiamo il pc
	unsigned int a0 = state_reg->reg_a0;
	unsigned int a1 = state_reg->reg_a1;
	unsigned int a2 = state_reg->reg_a2; 
	unsigned int a3 = state_reg->reg_a3;
	switch(state_reg->reg_a0){
		case -1:
			Create_Process(a0,a1,a2,a3);
			break;
		case: -2:
			Terminate_Process(a0,a1);
			break;
		case: -3:
			Passeren(a0,a1);
			break;
		case: -4:
			Verhogen(a0,a1);
			break;
		case: -5:
			DO_IO(a0,a1,a2);
			break;
		case: -6:
			Get_CPU_Time(a0);
			break;
		case: -7:
			Wait_For_Clock(a0);
			break;
		case: -8:
			Get_Support_Data(a0);
			break;
		case: -9:
			Get_Process_Id(a0,a1);
			break;
		case: -10:
			Yield(a0);
			break;	
	}
}

/*  Syscall -1: Create_Process
    Questa system call crea un nuovo processo come figlio del chiamante.
	Il primo parametro contiene lo stato che deve avere il processo.
	Se la systemcall, ha sucesso il valore di ritorno è l'id del processo creato, altrimenti -1.
	
	- prio: indica se si tratta di un processo ad alta priorità
	- supportp: è un puntatore alla struttura di supporto del processo
	- return: il pid del processo

 *  @pdf pandos-Chapter3 >> 3.5.1
 int SYSCALL(CREATEPROCESS, state_t *statep, int prio, support_t *supportp)
*/
void Create_Process(int a0, unsigned int a1, unsigned int a2, unsigned int a3) {
	if(a0 == CREATEPROCESS) {
#ifdef SYS_DEBUG
		klog_print("Create_Process ...");
#endif

		state_t *statep = (state_t *) a1;
		int prio = (int) a2;
		support_t *supportp = (support_t *) a3;
		
		pcb_PTR nuovo_pcb = allocPcb();// creo un pcb
		if (nuovo_pcb != NULL){
			nuovo_pcb->p_s = *statep;
			nuovo_pcb->p_prio = prio;
			nuovo_pcb->p_supportStruct = supportp;
			nuovo_pcb->p_pid = &nuovo_pcb;
			if (prio == PROCESS_PRIO_HIGH)
				insertProcQ(high_priority_q, nuovo_pcb);
			else if(prio == PROCESS_PRIO_LOW)
				insertProcQ(low_priority_q, nuovo_pcb);
			else klog_print("prio non e' Low o High\n");
			//! un po' troppo: PANIC();
			insertChild(current_process, nuovo_pcb);// aggiungo al current un figlio del nuovo pcb (=chiamante)
			
			nuovo_pcb->p_time = 0;
			nuovo_pcb->p_semAdd = NULL;

			//aggiornare i contatori
			process_count++;
			
			state_reg->reg_v0 = nuovo_pcb->p_pid;
		} else
			state_reg->reg_v0 = -1;	
		
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Create_Process ERROR: a0 != CREATEPROCESS\n");
}

/* Syscall -2: Terminate_Process
	- Quando invocata, la SYS2 termina il processo indicato dal secondo parametro
	insieme a tutta la sua progenie.
	- Se il secondo parametro è 0 il bersaglio è il processo invocante.

	a1: (pid) processo da terminare
 void SYSCALL(TERMPROCESS, int pid, 0, 0)
*/
void Terminate_Process(int a0, unsigned int a1) { //! DA CONTROLLARE
    if(a0 == TERMPROCESS) {
#ifdef SYS_DEBUG
		klog_print("Terminate_Process ...");
#endif
		int pid = (int) a1;
		if (pid == 0){ //devo eliminare current process
			auxiliary_terminate(current_process);
		} else { //elimino il processo con pid indicato
			//cerco pid nella lista di non so cosa
			pcb_PTR trovato = NULL;
			pcb_PTR iter;
			list_for_each(iter, high_priority_q) {
				if(iter->p_pid == pid)
					trovato = iter;		//trovato = processo con pid=pid(=a1)
			}
			if (trovato == NULL){
				list_for_each(iter, low_priority_q) {
					if(iter->p_pid == pid)
						trovato = iter;	//trovato = processo con pid=pid(=a1)
				}
			}
			if(trovato != NULL)
				auxiliary_terminate(trovato);
		}		
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
		scheduler();
	} else klog_print("Terminate_Process ERROR: a0 != TERMPROCESS\n");
}
/*	Auxiliar Function of Terminate_Process	*
	Elimina i current_process eliminando i figli e i figli dei figli.

	current: puntatore processo corrente
*/
void auxiliary_terminate(pcb_PTR current){
	pcb_PTR removed_child;
	if (current != NULL) {
		//se ha figli caso ricorsivo
		while (emptyChild(current)==0) {
			removed_child = removeChild(current);
			auxiliary_terminate(removed_child);
		}
		//caso base
		outProcQ(high_priority_q, current);
		outProcQ(low_priority_q, current);
		freePcb(current);
		//aggiornamento dei contatori
		process_count--;	
	}
}

/* Syscall -3: Passeren
	- Operazione di richiesta di semaforo binario.
	Il valore del semaforo è memorizzato nella variabile di tipo
	intero passata per indirizzo.
	L'indirizzo della variabile agisce da identificatore per il semaforo.	

void SYSCALL(PASSEREN, int* semaddr, 0, 0)
*/
void Passeren(int a0, unsigned int a1) {
	if(a0 == PASSEREN) {
#ifdef SYS_DEBUG
		klog_print("Passeren ...");
#endif

		int *semaddr = (int *) a1;
		P_operation(semaddr);

#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
		scheduler();
	} else klog_print("Passeren ERROR: a0 != PASSEREN\n");
}
/*	Auxiliar Function of Passeren	*
	
	semaddr: a1
*/
void P_operation(int *semaddr) {
	int result = insertBlocked(semaddr, current_process);
	//aggiornare i contatori
	if(result == 0) // insertBlocked avvenuta con successo
		soft_block_count++;
	else {
		klog_print("Passeren ADVICE: inserimento fallito miseramente\n");
		//! un po' troppo: PANIC();
	}
}

/* Syscall -4: Verhogen
	– Operazione di rilascio su un semaforo binario.
	Il valore del semaforo è memorizzato nella variabile
	di tipo intero passata per indirizzo.
	L’indirizzo della variabile agisce da identificatore
	per il semaforo.

 void SYSCALL(VERHOGEN, int *semaddr, 0, 0)
*/
void Verhogen(int a0, unsigned int a1) {
	if(a0 == VERHOGEN) {
#ifdef SYS_DEBUG
		klog_print("Verhogen ...");
#endif

	int * semaddr = (int *) a1;
	pcb_PTR released_proc = removeBlocked(semaddr);
	if(released_proc->p_prio  == PROCESS_PRIO_HIGH) {
		insertProcQ(high_priority_q, released_proc);
		soft_block_count--;		
	}
	else if(released_proc->p_prio  == PROCESS_PRIO_LOW) {
		insertProcQ(low_priority_q, released_proc);
		soft_block_count--;
	} else {
		klog_print("Verhogen ADVICE: released process have no good priority\n");
		//! un po' troppo: PANIC();
	}
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Verhogen ERROR: a0 != VERHOGEN\n");
}

/*	Syscall -5: DO_IO
	– Effettua un’operazione di I/O scrivendo il comando cmdValue nel registro cmdAddr, e mette in 
	pausa il processo chiamante fino a quando non si e’ conclusa.
	– L’operazione è bloccante, quindi il chiamante viene sospeso sino alla conclusione del comando. 
	Il valore ritornato deve essere il contenuto del registro di status
	del dispositivo

int SYSCALL(DOIO, int *cmdAddr, int cmdValue, 0)
*/
void DO_IO(int a0, unsigned int a1, unsigned int a2) {
	if(a0 == DOIO) {
#ifdef SYS_DEBUG
		klog_print("Do_Io ...");
#endif
	
	int *cmdAddr = (int *) a1;
	int cmdValue = (int) a2;
	//TODO
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	scheduler();
	} else klog_print("Do_Io ERROR: a0 != DOIO\n");
	
}

/*	Syscall -6: Get_CPU_Time
	– Quando invocata, la NSYS6 restituisce il tempo di esecuzione (in microsecondi) 
	del processo che l’ha chiamata fino a quel momento.
	– Questa System call implica la registrazione del tempo passato durante l’esecuzione di un
	processo

 int SYSCALL(GETTIME, 0, 0, 0)
*/
void Get_CPU_Time(int a0) {
	if (a0 == GETTIME) {
#ifdef SYS_DEBUG
		klog_print("Get_CPU_Time ...");
#endif

	//TODO: controllare se vuole il tempo in ... e fare eventuali conversioni
	state_reg->reg_v0 = current_process->p_time;
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_CPU_Time ERROR: a0 != GETTIME\n");
	
}

/* Syscall -7: Wait_For_Clock
	– Equivalente a una Passeren sul semaforo dell’Interval Timer. 
	– Blocca il processo invocante fino al prossimo tick del dispositivo.

int SYSCALL(CLOCKWAIT, 0, 0, 0)
*/
void Wait_For_Clock(int a0) {
	if (a0 == CLOCKWAIT) {
#ifdef SYS_DEBUG
		klog_print("Wait_For_Clock ...");
#endif

    P_operation(&device_sem[48]);
	
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	scheduler();
	} else klog_print("Wait_For_Clock ERROR: a0 != CLOCKWAIT\n");
}

/*	Syscall -8: Get_Support_Data
	– Restituisce un puntatore alla struttura di supporto del processo corrente, 
	ovvero il campo p_supportStruct del
	pcb_t

 support_t* SYSCALL(GETSUPPORT, 0, 0, 0) 
*/
void Get_Support_Data(int a0) {
	if (a0 == GETSUPPORTPTR) {
#ifdef SYS_DEBUG
		klog_print("Get_Support_Data ...");
#endif

	state_reg->reg_v0 = current_process->p_supportStruct;	
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_Support_Data ERROR: a0 != GETSUPPORT\n");
	return NULL;
}

/*	Syscall -9: Get_Process_Id
	– Restituisce l’identificatore del processo invocante se parent == 0, 
	quello del genitore del processo invocante altrimenti

int SYSCALL(GETPROCESSID, int parent, 0, 0) 
*/
void Get_Process_Id(int a0, unsigned int a1) {
	if (a0 == GETPROCESSID) {
#ifdef SYS_DEBUG
		klog_print("Get_Process_Id ...");
#endif

	int parent = (int) a1;
	if (parent == 0)
		state_reg->reg_v0 = current_process->p_pid;
	else
		state_reg->reg_v0 = current_process->p_parent->p_pid;
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_Process_Id ERROR: a0 != GETPROCESSID\n");
}

/*	 Syscall -10: Yield
	– Un processo che invoca questa system call viene sospeso 
	e messo in fondo alla coda corrispondente dei processi ready.
	– Il processo che si è autosospeso, anche se rimane “ready”,
	non può ripartire immediatamente

int SYSCALL(YIELD, 0, 0, 0) 
*/
void Yield(int a0) {
	if (a0 == YIELD) {
#ifdef SYS_DEBUG
		klog_print("Yield ...");
#endif
    
	//TODO: come chiamare un low priority se la high priority è vuota?
	//TODO: e se anche la low priority è vuota come chiamare il processo corrente?
	if(current_process->p_prio  == PROCESS_PRIO_HIGH) {
		//se è high priority e la lista è vuota non devo richiamare lui ma un altro
		insertProcQ(high_priority_q, current_process);
	}
	else if(current_proc->p_prio  == PROCESS_PRIO_LOW) {
		insertProcQ(low_priority_q, current_process);
	} else {
		klog_print("Yield ADVICE: current have no good priority\n");
		//! un po' troppo: PANIC();
	}
	//TODO: faccio partire il processo che voglio senza chiamare lo scheduler
	//setTimer(...); //LDST(...)
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Yield ERROR: a0 != YIELD\n");
}
