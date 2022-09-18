/**
 * @file vmSupport.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *   
 * @version 0.1
 * @date 2022-08-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "vmSupport.h"

#define VSHIFT        9     /* shift for bit v (valid), added for phase 3 */
#define BLKSHIFT	  8		/* blocknumber shift, added for phase 3 */
#define PFNSHIFT	  12	/* pfn shift */

//#define VM_DEBUG //per debuggare pager e utlb

extern pcb_PTR current_process;
int swap_pool_sem;      //swap pool semaphore
swap_t swap_pool[POOLSIZE]; //swap pool table 
int mutex_asid;     //which asid is holding the mutual exclusion for support semaphores (this case swap_pool_sem)

/**
 *  pandosplus_phase3.pdf pag 6,7
 * paragrafo 4.3 The TLB-Refill event handler
 *
 * it need to:
 * * Locate the correct Page Table entry in the Current Process’s Page Table;
 *   a component of p supportStruct [Section 3.7]
 * * Write the entry into the TLB using the TLBWR instruction. [Section 6.4-pops]).
 * * Return control (LDST) to the Current Process to restart the address translation process.
 * 
 */
void uTLB_RefillHandler() {
#ifdef VM_DEBUG
	klog_print("U"); klog_print_hex(current_process->p_supportStruct->sw_asid);
#endif
    //1. Determine the page number (denoted as p) of the missing TLB entry
    //  by inspecting EntryHi in the saved exception state located at the start
    //  of the BIOS Data Page. [Section 3.4]
    state_t* state = (state_t*)BIOSDATAPAGE;
    int page_number_P;
    if (state->entry_hi >= 0xBFFFF000 && state->entry_hi <= USERSTACKTOP)
    	page_number_P = 31;
    else 
    	page_number_P = (state->entry_hi - PRESENTFLAG) >> VPNSHIFT;

    //2. Get the Page Table entry for page number p for the Current Process.
    //  This will be located in the Current Process’s Page Table,
    //  which is part of its Support Structure.
    pteEntry_t entry = current_process->p_supportStruct->sup_privatePgTbl[page_number_P];
    //3. Write this Page Table entry into the TLB. This is a three-set process:
    //	(a) setENTRYHI    
    setENTRYHI((unsigned int) entry.pte_entryHI );
    //	(b) setENTRYLO
    setENTRYLO((unsigned int) entry.pte_entryLO );
    //	(c) TLBWR
    TLBWR();
    //4. Return control to the Current Process to retry the instruction that
    //  caused the TLB-Refill event: LDST on the saved exception state
    //  located at the start of the BIOS Data Page.
    LDST(state);
}

/**
 * First in First Out
 *
 * section 4.5.4 The Pandos Page Replacement Algorithm
    
    When a page fault occurs, the page replacement algorithm picks one of the
    frames from the Swap Pool. The recommended Pandos page replacement
    algorithm is First in First out.
    Though inefficient, this “round robin” algorithm is easily implemented
    via a static variable. Whenever a frame is needed to support a page fault,
    simply increment this variable mod the size of the Swap Pool.

 * @return 
 */
int FIFO() {
    int static page_index = 0;      // prima pagina, static così è inizializzata una e una sola volta.
    return (page_index++)%POOLSIZE;
}

/** Disable TLB Atomically
 * 
 */
void disable_interrupts(){
    
    setSTATUS(getSTATUS() & (~IMON));
}

/** Enable TLB Atomically
 * 
 */
void enable_interrupts() {
    
    setSTATUS(getSTATUS() | IMON); //esempio: 00FF0000 | 0000FF00 = 00FFFF00
}

//DA MODIFICARE: vedi 4.5.2
void update_TLB() {
    //Probe the TLB (TLBP) to see if the newly updated TLB entry is
    //indeed cached in the TLB. If so (Index.P is 0), rewrite (update) that
    //entry (TLBWI) to match the entry in the Page Table.
    //   * or *
    //Erase ALL the entries in the TLB (TLBCLR)
    TLBCLR();
        //! 4.10 pandosplus_phase3: update TLB using TLBP and TLBWI instead of TLBCLR
    //TLBP();
    //TLBWI();
}

/** 4.4.2 The pager
 * 
 * utilizzo di paragrafo 3.3 pop "The Cause Register"
 * ~
 * pandosplus_phase3.pdf pag 10
 * paragrafo 4.4.2 The pager

    Nota: chiamiamo il pager quando c'è page-fault quindi di sicuro la pagina cercata non c'è in cache.
 */
void pager(){
#ifdef VM_DEBUG
	klog_print(" pager");
#endif
	//1. Obtain the pointer to the Current Process’s Support Structure: NSYS8 (fase 2)
	support_t* current_support = (support_t *)SYSCALL(GETSUPPORTPTR, 0, 0, 0);

	//2. Determine the cause of the TLB exception.
	//  The saved exception state responsible for this TLB exception
	//  should be found in the Current Process’s Support Structure for TLB exceptions.
	//  (sup exceptState[0]’s Cause register)
	unsigned int exccode =  CAUSE_GET_EXCCODE(current_support->sup_exceptState[PGFAULTEXCEPT].cause);

	//3. If the Cause is a TLB-Modification exception, treat this exception as
	//  a program trap [Section 4.8], otherwise continue.
	if (exccode == 1){      // see pop Table 1.1 [pag 163 e 35]
#ifdef VM_DEBUG
		klog_print(" TLB-MODification error!");
#endif
		support_program_trap();

	} else {
		//4.  Gain mutual exclusion over the Swap Pool table.
		//  (NSYS3 – P operation on the Swap Pool semaphore)
		SYSCALL(PASSEREN, (int)&swap_pool_sem, 0, 0);
		mutex_asid = current_support->sup_asid;

		//5. Determine the missing page number (denoted as p):
		//  found in the saved exception state’s EntryHi.
		int pteEntryP;
		if (current_support->sup_exceptState[PGFAULTEXCEPT].entry_hi  >= 0xBFFFF000
					&& current_support->sup_exceptState[PGFAULTEXCEPT].entry_hi <= USERSTACKTOP)
			pteEntryP = 31;
		else 
			pteEntryP = (current_support->sup_exceptState[PGFAULTEXCEPT].entry_hi - PRESENTFLAG) >> VPNSHIFT;
#ifdef VM_DEBUG
		klog_print(" entry_p:");klog_print_hex(pteEntryP);
#endif

		//6. Pick a frame, i, from the Swap Pool.
		//  Which frame is selected is determined by the Pandos page replacement algorithm. [Section 4.5.4]
		int frame_i = FIFO();
		// --> for point 8.c and 9. 9 is outside the if.
		memaddr starting_address = SWAPPOOLSTART + PAGESIZE * frame_i;  //this is for 8 & 9
		devregarea_t* devReg = (devregarea_t*) RAMBASEADDR; // device register
		devreg_t* devAddrBase;

		// 7. Determine if frame i is occupied; examine entry i in the Swap Pool table.
		if (swap_pool[frame_i].sw_pte->pte_entryLO & VALIDON) // check the v bit is valid[1]
		{
			// v is valid

			//8. If frame i is currently occupied, assume it is occupied by logical page
			//  number k belonging to process x (ASID) and that it is “dirty” (i.e. been modified):
			disable_interrupts();   // * start TLB atomically *

			//      (a) Update process x’s Page Table: mark Page Table entry k as not valid.
			//      This entry is easily accessible, since the Swap Pool table’s
			//      entry i contains a pointer to this Page Table entry.
			swap_pool[frame_i].sw_pte->pte_entryLO &= ~VALIDON;  // v is not valid now

			//      (b) Update the TLB, if needed. The TLB is a cache of the most
			//      recently executed process’s Page Table entries. If process x’s page
			//      k’s Page Table entry is currently cached in the TLB it is clearly
			//      out of date; it was just updated in the previous step.  [Section 4.5.2]                          
			//   Important Point: This step and the previous step must be accomplished atomically. [Section 4.5.3]
			update_TLB();
			enable_interrupts();    // * end TLB atomically *
			//      (c) Update process x’s backing store. Write the contents of frame i
			//      to the correct location on process x’s backing store/flash device. [Section 4.5.1]
			
			//			SECTION 4.5.1		Reading and Writing from/to a Flash Device
			//	   			(1.) write the flash device's DATA0 field with the appropriate starting physical address
			//	      			of the 4k block to be read (or written);
			//	      			the particularframe’s starting address
			devAddrBase = (devreg_t*) &devReg->devreg[ 1 ][ swap_pool[frame_i].sw_asid - 1 ];  //flash device line
			devAddrBase->dtp.data0 = starting_address;
			//	   			(2.) Use the NSYS5 [doio] system call to write the flash device’s COMMAND field 
			//	      			with the device block number (high order three bytes) and the
			//	      			command to read (or write) in the lower order byte
			int return_doio = SYSCALL(DOIO, (memaddr)&devAddrBase->dtp.command,
								FLASHWRITE | (swap_pool[frame_i].sw_pageNo << BLKSHIFT), 0);
			//      Treat any error status from the write operation as a program trap.[Section 4.8]
			if (return_doio != READY) {
#ifdef VM_DEBUG
				klog_print(" WRITE not ready!");
#endif
				support_program_trap();
			}

		}

		// 9.Read the contents of the Current Process’s backing store/flash device
		// logical page p into frame i. [Section 4.5.1]
		// Treat any error status from the read operation as a program trap. [Section 4.8]
		//		SECTION 4.5.1		Reading and Writing from/to a Flash Device
		//			(1.) write the flash device's DATA0 field with the appropriate starting physical address
		//				of the 4k block to be read (or written);
		//				the particularframe’s starting address
		devAddrBase = (devreg_t*) &devReg->devreg[ 1 ][current_support->sup_asid-1];  //flash device line
		devAddrBase->dtp.data0 =(memaddr) starting_address;
		//	   		(2.) Use the NSYS5 [doio] system call to write the flash device’s COMMAND field 
		//	      		with the device block number (high order three bytes) and the
		//	      		command to read (or write) in the lower order byte
		int return_doio = SYSCALL(DOIO, (memaddr)&devAddrBase->dtp.command,
								FLASHREAD | (pteEntryP << BLKSHIFT)  , 0); 
		if (return_doio != READY) {
		//il prof nel p2test mette come controllo "if ((status & TERMSTATMASK) != RECVD)"
#ifdef VM_DEBUG
			klog_print(" READ not ready!");
#endif
			support_program_trap();
		}

		// 10. Update the Swap Pool table’s entry i [frame] to reflect frame i’s new contents:
		// page p[=pteEntryP] belonging to the Current Process’s ASID, and a pointer to the
		// Current Process’s Page Table entry for page p.
		swap_pool[frame_i].sw_asid = current_support->sup_asid;
		swap_pool[frame_i].sw_pageNo = pteEntryP;
		swap_pool[frame_i].sw_pte = &current_support->sup_privatePgTbl[pteEntryP];

		// 11. Update the Current Process’s Page Table entry for page p to indicate
		// it is now present (V bit) and occupying frame i (PFN field).
		disable_interrupts(); //* start TLB atomically *
		current_support->sup_privatePgTbl[pteEntryP].pte_entryLO = (starting_address) | VALIDON | DIRTYON;

		// 12. Update the TLB. The cached entry in the TLB for the Current Process’s page p
		// is clearly out of date; it was just updated in the previous step.
		// Important Point: This step and the previous step must be accomplished atomically. [Section 4.5.3]
		update_TLB();
		enable_interrupts(); //* end TLB atomically *

		// 13. Release mutual exclusion over the Swap Pool table. (NSYS4 – V operation on the Swap Pool semaphore)
		mutex_asid = -1;    //my asid is not longer holding mutex
		SYSCALL(VERHOGEN, (int)&swap_pool_sem, 0, 0);

		// 14. Return control to the Current Process to retry the instruction that
		// caused the page fault: LDST on the saved exception state
		LDST((state_t*)&current_support->sup_exceptState[PGFAULTEXCEPT]);
	}
}
