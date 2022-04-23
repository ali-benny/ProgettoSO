#ifndef H_SYSCALL
#define H_SYSCALL

#include "pcb.h"
#include "scheduler.h"
#include <umps3/umps/libumps.h>
#include "klog/klog.h"
#include <umps3/umps/arch.h>
#include "umps_utils/memcpy.h"

void P_operation(int *semaddr);	//Passeren, Wait for clock
pcb_PTR V_operation(int *semaddr); //Veroghen

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
