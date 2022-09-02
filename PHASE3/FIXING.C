alibi@alibi-VirtualBox:~/Desktop/ProgettoSO/PHASE3$ make
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I -I -Wall -O0   -c -o sysSupport.o sysSupport.c
sysSupport.c: In function ‘support_exception_handler’:
sysSupport.c:28:28: warning: implicit declaration of function ‘SYSCALL’ [-Wimplicit-function-declaration]
   28 |  support_exc = (support_t*)SYSCALL(GETSUPPORTPTR,0,0,0); // current process support struct by NSYS8
      |                            ^~~~~~~

sysSupport.c:72:2: warning: implicit declaration of function ‘LDST’; did you mean ‘LDIT’? [-Wimplicit-function-declaration]
   72 |  LDST(state_exc);
      |  ^~~~
      |  LDIT
            
			
sysSupport.c:153:34: error: cast to union type from type not present in union
  153 |    devreg_t value = PRINTCHR | (((devreg_t)*s) << 8);
      |                        
	  
make: *** [<builtin>: sysSupport.o] Error 1

