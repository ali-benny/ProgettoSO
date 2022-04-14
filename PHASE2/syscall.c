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

#define SYS_DEBUG //per debuggare le systemcall

//richiami delle variabili globali di main.c
extern struct list_head high_priority_q;
extern struct list_head low_priority_q;
extern int process_count;
extern int soft_block_count; 
extern pcb_PTR current_process;
extern int device_sem[49]; //Device Semaphores: 

extern state_t* state_reg;


// *** Auxiliar Functions *** //

void auxiliary_terminate(pcb_PTR current); //terminate
//void P_operation(int *semaddr);	//Passeren, Wait for clock
//pcb_PTR V_operation(int *semaddr); //Veroghen

void Blocking_Syscall(){
	//As described above [Section 3.5.12] the value of the PC must be incremented by 4 
	//   to avoid an infinite loop of SYSCALLs.
	//a. The saved processor state (located at the start of the BIOS Data Page [state_reg] [Section 3.4]) 
	//	must be copied into the Current Process’s pcb (p_s).
	current_process->p_s = *state_reg;

	//b. Update the accumulated CPU time for the Current Process. [Section 3.8]
	int time = STCK(time);
	current_process->p_time += time;
	
	//c. The Current Process is blocked on the ASL (insertBlocked), transitioning 
	//	the process from the “running” state, to the “blocked” state.
	// già fatto nelle relative syscall con P_operation
	//d. Call the Scheduler
	scheduler();
}



/**  Syscall -1: Create_Process
    Questa system call crea un nuovo processo come figlio del chiamante.
	Il primo parametro contiene lo stato che deve avere il processo.
	Se la systemcall, ha sucesso il valore di ritorno è l'id del processo creato, altrimenti -1.

 * @param statep puntatore dello stato del processo
 * @param prio priorità del processo
 * @param supportp è un puntatore alla struttura di supporto del processo
 *
 * @returns The PID of the new process.
 * int SYSCALL(CREATEPROCESS, state_t *statep, int prio, support_t *supportp)
 *  *  @pdf pandos-Chapter3 >> 3.5.1
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
			nuovo_pcb->p_pid = (memaddr) nuovo_pcb;
			//scelgo in che coda metterlo in base alla sua priority
			if (prio == PROCESS_PRIO_HIGH)
				insertProcQ(&high_priority_q, nuovo_pcb);
			else if(prio == PROCESS_PRIO_LOW)
				insertProcQ(&low_priority_q, nuovo_pcb);
			else klog_print("prio non e' Low o High\n");
			//! un po' troppo: PANIC();
			
			// aggiungo al current un figlio del nuovo pcb (=chiamante)
			insertChild(current_process, nuovo_pcb);
			
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
	
	LDST(state_reg);
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
			//cerco pid nelle varie liste
			pcb_PTR trovato = NULL;
			struct list_head *iter;
			list_for_each(iter, &high_priority_q) { //lo cerco nella high priority coda
				pcb_PTR iter_pcb = container_of(iter, pcb_t, p_list);
				if(iter_pcb->p_pid == pid)
					trovato = iter_pcb;		//trovato = processo con pid=pid(=a1)
			}
			if (trovato == NULL){
				list_for_each(iter, &low_priority_q) { //lo cerco nella low priority
					pcb_PTR iter_pcb = container_of(iter, pcb_t, p_list);
					if(iter_pcb->p_pid == pid)
						trovato = iter_pcb;	//trovato = processo con pid=pid(=a1)
				}
			}
			if(trovato != NULL) //se l'ho trovato
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
		outProcQ(&high_priority_q, current);
		outProcQ(&low_priority_q, current);
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
		Blocking_Syscall();
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

	int *semaddr = (int *) a1;
	V_operation(semaddr);
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Verhogen ERROR: a0 != VERHOGEN\n");
	
	LDST(state_reg);
}
/**	Auxiliar Function of Veroghen	*
	
	semAddr
 *	@return released_proc 
 */
pcb_PTR V_operation(int *semaddr){
	pcb_PTR released_proc = removeBlocked(semaddr);
	if(released_proc->p_prio  == PROCESS_PRIO_HIGH) {
		insertProcQ(&high_priority_q, released_proc);
		soft_block_count--;	
		return	released_proc;
	}
	else if(released_proc->p_prio  == PROCESS_PRIO_LOW) {
		insertProcQ(&low_priority_q, released_proc);
		soft_block_count--;
		return released_proc;
	}
	return NULL;
}

/**	Syscall -5: DO_IO
	– Effettua un’operazione di I/O scrivendo il comando cmdValue nel registro cmdAddr, e mette in 
	pausa il processo chiamante fino a quando non si e’ conclusa.
	– L’operazione è bloccante, quindi il chiamante viene sospeso sino alla conclusione del comando. 
	Il valore ritornato deve essere il contenuto del registro di status	del dispositivo

	@param commandAddr è il puntatore al registro di comando del dispositivo corrispondente, 
	@param commandValue è il valore da scriverci. 
	Potete capire linea e istanza cercando commandAddr tra i registri di tutti i device noti

int SYSCALL(DOIO, int *cmdAddr, int cmdValue, 0)
*/
void DO_IO(int a0, unsigned int a1, unsigned int a2) {
	if(a0 == DOIO) {
#ifdef SYS_DEBUG
		klog_print("Do_Io ...");
#endif
	
	unsigned int *cmdAddr = (unsigned int *) a1;
	int cmdValue = (int) a2;
	//initializes found boolean, intLineNumber and Device Number to 0 for the whiles.
	int found = 0, IntLine = 0, DevNo = 0;
	
	while (IntLine < 5 && !found){
		while (DevNo < 8 && !found){
			//calculate the devAddrBase
			devreg_t* devAddrBase = (devreg_t*)(0x10000054 + ((IntLine - 3) * 0x80) + (DevNo * 0x10));
			int device_position = (IntLine - 3) + DevNo;
			//check if we find it
			if (&(devAddrBase->dtp.command) == cmdAddr){ //if we find it
				//if them are not terminals
				if (device_position < 31) {
					devAddrBase->dtp.command = cmdValue;
					state_reg->reg_v0 = devAddrBase->dtp.status;
				}					
				else if (device_position < 38 ) { //if them are terminals and writing terminals
					devAddrBase->term.recv_command = cmdValue;
					state_reg->reg_v0 = devAddrBase->term.recv_status;	
				}				
				else if (device_position < 47 ) { //if them are terminals and reading terminals 
					devAddrBase->term.transm_command = cmdValue;
					state_reg->reg_v0 = devAddrBase->term.transm_status;
				}
				else {//if i am not supposed to be hear (device position = 48 or 49 or other stuff)				
					klog_print("DO_IO ERROR: out of bounds\n");
				}
				//if we are supposed to be hear (non da 48 in su)
				if (device_position < 47) {
					P_operation(&device_sem[device_position]);
				}
				found = 1;
			}else{ //if we not found it yet
				DevNo++;
			}
		}
		IntLine++;
	}
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
		Blocking_Syscall();
	} else
		klog_print("Do_Io ERROR: a0 != DOIO\n");	
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

	int time = STCK(time);
	state_reg->reg_v0 = current_process->p_time + time;

#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_CPU_Time ERROR: a0 != GETTIME\n");
	
	LDST(state_reg);
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
		Blocking_Syscall();
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

	state_reg->reg_v0 = (unsigned int) current_process->p_supportStruct; //! modificato per fix error con cast

#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_Support_Data ERROR: a0 != GETSUPPORT\n");
	
	LDST(state_reg);
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
	
	LDST(state_reg);
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
	pcb_PTR next_process = NULL; //next process to be started
	
	//controllo quale altro processo potrebbe andare avanti
	if(emptyProcQ(&high_priority_q)==0)//se la lista ad alta priorità non è vuota		
		next_process = removeProcQ(&high_priority_q);
	else if(emptyProcQ(&low_priority_q)==0) //se la alta è vuota ma la bassa no
		next_process = removeProcQ(&low_priority_q);
	else //se sono entrambe vuote faccio partire il processo corrente
		next_process = current_process;
	
	//e successivamente inserisco il processo corrente nella sua coda
	if(current_process->p_prio  == PROCESS_PRIO_HIGH) {
		//se è high priority e la lista è vuota non devo richiamare lui ma un altro
		insertProcQ(&high_priority_q, current_process);
	}
	else if(current_process->p_prio  == PROCESS_PRIO_LOW) {
		insertProcQ(&low_priority_q, current_process);
	} else {
		klog_print("Yield ADVICE: current have no good priority\n");
		//! un po' troppo: PANIC();
	}
	//poi avvio il next_process
	// faccio partire il processo che voglio senza chiamare lo scheduler
	current_process = next_process;
	setTIMER(5000);
	LDST(state_reg);

#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Yield ERROR: a0 != YIELD\n");
}
