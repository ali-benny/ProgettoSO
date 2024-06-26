#ifndef H_SYSCALL
#define H_SYSCALL

#include "pcb.h"
#include "asl.h"
#include "scheduler.h"
#include <umps3/umps/libumps.h>
#include "klog/klog.h"
#include <umps3/umps/arch.h>
#include "umps_utils/memcpy.h"

 //Syscall per la fase 2. Quelle per la fase 3 vengono aggiunte nel file syscall_supporto.c e .h

pcb_PTR P_operation(int *semaddr, int isDevSem);	//Passeren, Wait for clock
pcb_PTR V_operation(int *semaddr, int isDevSem); //Veroghen

void Create_Process(int a0, unsigned int a1, unsigned int a2, unsigned int a3);
void Terminate_Process(int a0, unsigned int a1);
void Passeren(int a0, unsigned int a1);
void Verhogen(int a0, unsigned int a1);
void DO_IO(int a0, unsigned int a1, unsigned int a2);
void Get_CPU_Time(int a0);
void Wait_For_Clock(int a0);
void Get_Support_Data(int a0);
void Get_Process_Id(int a0, unsigned int a1);
void Yield(int a0);

#endif // H_SYSCALL
