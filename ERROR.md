FIX LEGEND:
- [FIXED1] aggiunto struct nella dichiarazione delle priority q, in tutti i file .c
- [FIXED2] aggiunto struct nella dichiarazione di passupvector, in main e exception
- [FIXED3] fixed tipo in main: LDST ((state_t *) 0x0FFFF000);
- [FIXED4] errore di battitura
NB: scritti a lato dell'errore i vari fix

libera@lenovo:~/Scrivania/ProgettoSO/PHASE2$ make
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o pcb.o pcb.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o asl.o asl.c
asl.c: In function ‘headBlocked’:
asl.c:191:30: warning: returning ‘struct list_head *’ from a function with incompatible return type ‘pcb_t *’ [-Wincompatible-pointer-types]
  191 |      return (current->s_procq.next);
      |             ~~~~~~~~~~~~~~~~~^~~~~~
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o p2test.o p2test.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0 -c -o crtso.o /usr/share/umps3/crtso.S
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0 -c -o libumps.o /usr/share/umps3/libumps.S
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o main.o main.c
main.c:24:1: error: unknown type name ‘list_head’; use ‘struct’ keyword to refer to the type 
   24 | list_head high_priority_q;// alta priorità[FIXED1]
      | ^~~~~~~
      | struct 
main.c:25:1: error: unknown type name ‘list_head’; use ‘struct’ keyword to refer to the type
   25 | list_head low_priority_q;[FIXED1]
      | ^~~~~
      | struct 
main.c: In function ‘uTLB_RefillHandler’:
main.c:30:2: warning: implicit declaration of function ‘setENTRYHI’ [-Wimplicit-function-declaration]
   30 |  setENTRYHI(0x80000000);
      |  ^~~~~~~~
main.c:31:2: warning: implicit declaration of function ‘setENTRYLO’ [-Wimplicit-function-declaration]
   31 |  setENTRYLO(0x00000000);
      |  ^~~~~~~~~~
main.c:32:2: warning: implicit declaration of function ‘TLBWR’ [-Wimplicit-function-declaration]
   32 |  TLBWR();
      |  ^~~
main.c:33:2: warning: implicit declaration of function ‘LDST’; did you mean ‘LDIT’? [-Wimplicit-function-declaration]
   33 |  LDST ((state_PTR) 0x0FFFF000);
      |  ^~
      |  LDIT
main.c:33:9: error: ‘state_PTR’ undeclared (first use in this function); did you mean ‘state_t’?
   33 |  LDST ((state_PTR) 0x0FFFF000);[FIXED3]
      |         ^~~~~~~
      |         state_t
main.c:33:9: note: each undeclared identifier is reported only once for each function it appears in
main.c:33:19: error: expected ‘)’ before numeric constant
   33 |  LDST ((state_PTR) 0x0FFFF000);
      |                   ^~~~~~~
      |                   )
main.c: In function ‘main’:
main.c:42:19: error: ‘passupvector_t’ {aka ‘struct passupvector’} has no member named ‘tlb_refll_handler’; did you mean ‘tlb_refill_handler’?
   42 |     passupvector->tlb_refll_handler = (memaddr) uTLB_RefillHandler;[FIXED2]
      |                   ^~~~~~~~~~~~~
      |                   tlb_refill_handler
main.c:44:41: error: hexadecimal floating constants require an exponent
   44 |     passupvector->tlb_refill_stackPtr = 0x2000.1000;
      |                                         ^~~~~~~
main.c:47:46: warning: implicit declaration of function ‘exceptin_handler’; did you mean ‘exception_handler’? [-Wimplicit-function-declaration]
   47 |  passupvector->exception_handler = (memaddr) exceptin_handler();[FIXED4]
      |                                              ^~~~~~~~~~~~~~
      |                                              exception_handler
main.c:49:37: error: hexadecimal floating constants require an exponent
   49 |  passupvector->exception_stackPtr = 0x2000.1000;
      |                                     ^~~~~~~~~
main.c:53:5: warning: implicit declaration of function ‘initSemd’ [-Wimplicit-function-declaration]
   53 |     initSemd();
      |     ^~~~~~
	  main.c:58:18: warning: passing argument 1 of ‘mkEmptyProcQ’ from incompatible pointer type [-Wincompatible-pointer-types]
   58 |     mkEmptyProcQ(&low_priority_q);
      |                  ^~~~~~~~~~~~~
      |                  |
      |                  int *
In file included from syscall.h:4,
                 from main.h:4,
                 from main.c:13:
pcb.h:11:37: note: expected ‘struct list_head *’ but argument is of type ‘int *’
   11 | void mkEmptyProcQ(struct list_head *head);
      |                   ~~~~~~~~~~~~~~^~
main.c:59:18: warning: passing argument 1 of ‘mkEmptyProcQ’ from incompatible pointer type [-Wincompatible-pointer-types]
   59 |     mkEmptyProcQ(&high_priority_q);
      |                  ^~~~~~~~~~~~~~~~
      |                  |
      |                  int *
In file included from syscall.h:4,
                 from main.h:4,
                 from main.c:13:
pcb.h:11:37: note: expected ‘struct list_head *’ but argument is of type ‘int *’
   11 | void mkEmptyProcQ(struct list_head *head);
      |                   ~~~~~~~~~~~~~~~~^~
In file included from pandos_const.h:10,
                 from pcb.h:4,
                 from syscall.h:4,
                 from main.h:4,
                 from main.c:13:
main.c:82:20: error: incompatible types when assigning to type ‘struct list_head’ from type ‘void *’
   82 |     pcb->p_child = NULL;
      |                    ^~~~
main.c:84:15: error: incompatible types when assigning to type ‘struct list_head’ from type ‘void *’
   84 |  pcb->p_sib = NULL;
      |               ^~~~
main.c:88:17: warning: passing argument 1 of ‘insertProcQ’ from incompatible pointer type [-Wincompatible-pointer-types]
   88 |     insertProcQ(&low_priority_q, pcb);
      |                 ^~~~~~~~~~~~~
      |                 |
      |                 int *
In file included from syscall.h:4,
                 from main.h:4,
                 from main.c:13:
pcb.h:13:36: note: expected ‘struct list_head *’ but argument is of type ‘int *’
   13 | void insertProcQ(struct list_head *head, pcb_t *p);
      |                  ~~~~~~~~~~~~~~^~
make: *** [<incorporato>: main.o] Errore 1