/**
 * @file exceptions.c
 * @author alice benatti, alberto scuderi, gerald manzano, libera longo
 *  
 * @version 0.1
 * @date 2022-03-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "exceptions.h"

//paragraph 3.4 pandos-chapter3.pdf (pag 8-9)
//Exception Handling

// Global Variables/Functions
extern void syscall_handler();

void exception_handler() {
	state_t *state_reg = (state_t*)BIOSDATAPAGE; // register value: che tipo di syscall è?
	//The cause of the exception is encoded in the .ExcCode field of the Cause
	//registrer (Cause.ExcCode) in the saved exception state (vedi 3.3)
	unsigned int cause = getCAUSE();
	unsigned int excCode = cause & GETEXECCODE;
	switch (excCode) { //for exception code ... :processing should be pased logn to your ...
		case 0: //Interrupts
			//Nucleus's device intterrupt handler (vedi 3.6)
			interrupt_handler();
			break;
		case 1 ... 3: //TLB exceptions
			//Nucleus's TLB exception handler (vedi 3.7.3)
			passup_or_die(PGFAULTEXCEPT);
			break;
		case 8: //SYSCALL
			//Nucleus's SYSCALL exception handler (vedi 3.5)
            syscall_handler(state_reg);
			break;
		case 4 ... 7: //Program Traps
        case 9 ... 12: //Program Traps
			//Nucleus's Program Traps exception handler (vedi 3.7.2)
            passup_or_die(GENERALEXCEPT);
			break;
		default: 
			//TODO: termprint?
			klog_print("Unknown exception! You're in default case.\n");
			break;
	}
}
