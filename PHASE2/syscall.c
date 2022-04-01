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

/*  Syscall -1: Create_Process
 *  Questa system call crea un nuovo processo come figlio del chiamante.
	Il primo parametro contiene lo stato che deve avere il processo.
	Se la systemcall, ha sucesso il valore di ritorno è l'id del processo creato, altrimenti -1.
	
	- prio: indica se si tratta di un processo ad alta priorità
	- supportp: è un puntatore alla struttura di supporto del processo
	- return: il pid del processo
*/
// int SYSCALL(CREATEPROCESS, state_t *statep, int prio, support_t *supportp)
int Create_Process(int a0, unsigned int a1, unsigned int a2, unsigned int a3) {
	if(a0 == CREATEPROCESS) {
#ifdef SYS_DEBUG
		klog_print("Create_Process ...");
#endif

		state_t *statep = (state_t *) a1;
		int prio = (int) a2;
		support_t *supportp = (support_t *) a3;
		//TODO
		pcb_PTR nuovo_pcb = allocPcb();
		insertChild(current_process, nuovo_pcb);
		nuovo_pcb->p_prio = prio;
		nuovo_pcb->p_supportStruct = supportp;
		nuovo_pcb->p_s = *statep;
		
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Create_Process ERROR: a0 != CREATEPROCESS\n");
}

/* Syscall -2: Terminate_Process
- Quando invocata, la SYS3 termina il processo indicato dal secondo parametro
insieme a tutta la sua progenie.
- Se il secondo parametro è 0 il bersaglio è il processo invocante.
*/
//void SYSCALL(TERMPROCESS, int pid, 0, 0) {
void Terminate_Process(int a0, unsigned int a1) {
    if(a0 == TERMPROCESS) {
#ifdef SYS_DEBUG
		klog_print("Terminate_Process ...");
#endif

		int pid = (int) a1;
		//TODO
		
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Terminate_Process ERROR: a0 != TERMPROCESS\n");
	
}

/* Syscall -3: Passeren
- Operazione di richiesta di semaforo binario.
Il valore del semaforo è memorizzato nella variabile di tipo
intero passata per indirizzo.
L'indirizzo della variabile agisce da identificatore per il semaforo.
*/
//void SYSCALL(PASSEREN, int* semaddr, 0, 0) {
void Passeren(int a0, unsigned int a1) {
	    if(a0 == PASSEREN) {
#ifdef SYS_DEBUG
		klog_print("Passeren ...");
#endif

		int *semaddr = (int *) a1;
    	//TODO
		
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Passeren ERROR: a0 != PASSEREN\n");
	
}
/* Syscall -4: Verhogen
– Operazione di rilascio su un semaforo binario.
Il valore del semaforo è memorizzato nella variabile
di tipo intero passata per indirizzo.
L’indirizzo della variabile agisce da identificatore
per il semaforo.
*/
//void SYSCALL(VERHOGEN, int *semaddr, 0, 0)
void Verhogen(int a0, unsigned int a1) {
	if(a0 == VERHOGEN) {
#ifdef SYS_DEBUG
		klog_print("Verhogen ...");
#endif

	int * semaddr = (int *) a1;
	//TODO
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Verhogen ERROR: a0 != VERHOGEN\n");
	
}
/* Syscall -5: DO_IO
– Effettua un’operazione di I/O scrivendo il comando cmdValue nel registro cmdAddr, e mette in 
pausa il processo chiamante fino a quando non si e’ conclusa.
– L’operazione è bloccante, quindi il chiamante viene sospeso sino alla conclusione del comando. 
Il valore ritornato deve essere il contenuto del registro di status
del dispositivo
*/
//int SYSCALL(DOIO, int *cmdAddr, int cmdValue, 0)
int DO_IO(int a0, unsigned int a1, unsigned int a2) {
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
	} else klog_print("Do_Io ERROR: a0 != DOIO\n");
	
}

/* Syscall -6: Get_CPU_Time
– Quando invocata, la NSYS6 restituisce il tempo di esecuzione (in microsecondi) 
del processo che l’ha chiamata fino a quel momento.
– Questa System call implica la registrazione del tempo passato durante l’esecuzione di un
processo
*/
//int SYSCALL(GETTIME, 0, 0, 0) {
int Get_CPU_Time(int a0) {
	if (a0 == GETTIME) {
#ifdef SYS_DEBUG
		klog_print("Get_CPU_Time ...");
#endif

	//TODO
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_CPU_Time ERROR: a0 != GETTIME\n");
	
}

/* Syscall -7: Wait_For_Clock
	– Equivalente a una Passeren sul semaforo dell’Interval Timer. 
	– Blocca il processo invocante fino al prossimo tick del
dispositivo.
*/
//int SYSCALL(CLOCKWAIT, 0, 0, 0)
int Wait_For_Clock(int a0) {
	if (a0 == CLOCKWAIT) {
#ifdef SYS_DEBUG
		klog_print("Wait_For_Clock ...");
#endif

    //TODO
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Wait_For_Clock ERROR: a0 != CLOCKWAIT\n");
}

/*syscall -8: Get_Support_Data
– Restituisce un puntatore alla struttura di supporto del processo corrente, 
ovvero il campo p_supportStruct del
pcb_t
*/
//support_t* SYSCALL(GETSUPPORT, 0, 0, 0) 
support_t* Get_Support_Data(int a0) {
	if (a0 == GETSUPPORT) {
#ifdef SYS_DEBUG
		klog_print("Get_Support_Data ...");
#endif

    //TODO
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_Support_Data ERROR: a0 != GETSUPPORT\n");
}
/*syscall -9: Get_Process_Id
– Restituisce l’identificatore del processo invocante se parent == 0, 
quello del genitore del processo invocante altrimenti
*/
//int SYSCALL(GETPROCESSID, int parent, 0, 0) 
int Get_Process_Id(int a0, unsigned int a1) {
	if (a0 == GETPROCESSID) {
#ifdef SYS_DEBUG
		klog_print("Get_Process_Id ...");
#endif

	int parent = (int) a1;
    //TODO
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Get_Process_Id ERROR: a0 != GETPROCESSID\n");
}
/*syscall -10: Yield
– Un processo che invoca questa system call viene sospeso e messo in fondo alla coda 
corrispondente dei processi ready.
– Il processo che si e’ autosospeso, anche se rimane “ready”,
non puo’ ripartire immediatamente
*/
//int SYSCALL(YIELD, 0, 0, 0) 
int Yield(int a0) {
	if (a0 == YIELD) {
#ifdef SYS_DEBUG
		klog_print("Yield ...");
#endif

    //TODO
	
#ifdef SYS_DEBUG
		klog_print(" done!\n");
#endif
	} else klog_print("Yield ERROR: a0 != YIELD\n");
}
