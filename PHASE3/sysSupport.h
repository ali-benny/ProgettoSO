#ifndef SYSCALL_SUPP
#define SYSCALL_SUPP

//syscall per la fase di supporto, a0 positivo, quelle di fase 2 sono nel file syscall.c e .h

// void SYSCALL(TERMINATE, int pid, 0, 0);
// void SYSCALL(GET_TOD, 0, 0, 0);
// void SYSCALL(WRITEPRINTER, char *str, int len, 0);
// void SYSCALL(WRITETERMINAL, char *str, int len, 0);
// void SYSCALL(READTERMINAL, char *str, int len, 0);

void Terminate(int a0, unsigned int a1);
void Get_TOD(int a0);
void Write_Printer(int a0, unsigned int a1, unsigned int a2);
void Write_Terminal(int a0, unsigned int a1, unsigned int a2);
void Read_Terminal(int a0, unsigned int a1, unsigned int a2);

#endif //SYSCALL_SUPP