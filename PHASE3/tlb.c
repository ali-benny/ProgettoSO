/**
 * @file syscall_supporto.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *   
 * @version 0.1
 * @date 2022-08-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "tlb.h"

extern pcb_PTR current_process;

void uTLB_RefillHandler(){
    //pandosplus_phase3.pdf pag 6,7
    //4.3 The TLB-Refill event handler

    //it need to:
    //* Locate the correct Page Table entry in the Current Process’s Page Table;
    //a component of p supportStruct [Section 3.7]
    //* Write the entry into the TLB using the TLBWR instruction. [Section 6.4-pops]).
    //* Return control (LDST) to the Current Process to restart the address translation process.

    //1. Determine the page number (denoted as p) of the missing TLB entry
    //  by inspecting EntryHi in the saved exception state located at the start
    //  of the BIOS Data Page. [Section 3.4]
    state_t* state = (state_t*)BIOSDATAPAGE;

    //2. Get the Page Table entry for page number p for the Current Process.
    //  This will be located in the Current Process’s Page Table,
    //  which is part of its Support Structure.
    int page_number_P = state->entry_hi >> 12;

    //point entry del numero di pagina page_number_P per il processo corrente
    pteEntry_t pteEntryP = current_process->p_supportStruct->sup_privatePgTbl[page_number_P];
    
    //3. Write this Page Table entry into the TLB. This is a three-set process:
    //(a) setENTRYHI
    setENTRYHI( pteEntryP.pte_entryHI );
    //(b) setENTRYLO
    setENTRYLO( pteEntryP.pte_entryLO );
    //(c) TLBWR
    TLBWR();

    //4. Return control to the Current Process to retry the instruction that
    //  caused the TLB-Refill event: LDST on the saved exception state
    //  located at the start of the BIOS Data Page.
    LDST(state);
}