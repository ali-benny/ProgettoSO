alibi@alibi-VirtualBox:~/Desktop/ProgettoSO/PHASE3$ make
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I -I -Wall -O0   -c -o sysSupport.o sysSupport.c
sysSupport.c: In function ‘support_exception_handler’:
sysSupport.c:28:28: warning: implicit declaration of function ‘SYSCALL’ [-Wimplicit-function-declaration]
   28 |  support_exc = (support_t*)SYSCALL(GETSUPPORTPTR,0,0,0); // current process support struct by NSYS8
      |                            ^~~~~~~
sysSupport.c:29:14: error: incompatible types when assigning to type ‘state_t *’ {aka ‘struct state *’} from type ‘state_t’ {aka ‘struct state’}
   29 |  state_exc = support_exc->sup_exceptState[GENERALEXCEPT];
      |              ^~~~~~~~~~~
sysSupport.c: In function ‘support_syscall_handler’:
sysSupport.c:56:4: error: too few arguments to function ‘Terminate’
   56 |    Terminate(a0);
      |    ^~~~~~~~~
In file included from sysSupport.c:11:
sysSupport.h:19:6: note: declared here
   19 | void Terminate(int a0, unsigned int a1);
      |      ^~~~~~~~~
sysSupport.c:68:4: warning: implicit declaration of function ‘passup_or_die’ [-Wimplicit-function-declaration]
   68 |    passup_or_die(GENERALEXCEPT);
      |    ^~~~~~~~~~~~~
sysSupport.c:72:2: warning: implicit declaration of function ‘LDST’; did you mean ‘LDIT’? [-Wimplicit-function-declaration]
   72 |  LDST(state_exc);
      |  ^~~~
      |  LDIT
sysSupport.c: At top level:
sysSupport.c:119:6: error: conflicting types for ‘Terminate’
  119 | void Terminate(int a0){
      |      ^~~~~~~~~
In file included from sysSupport.c:11:
sysSupport.h:19:6: note: previous declaration of ‘Terminate’ was here
   19 | void Terminate(int a0, unsigned int a1);
      |      ^~~~~~~~~
sysSupport.c: In function ‘write’:
sysSupport.c:140:39: warning: comparison between pointer and integer
  140 |  int is_in_Uproc_address_space = (msg >= UPROCSTARTADDR && (msg + len) <= USERSTACKTOP);
      |                                       ^~
sysSupport.c:140:72: warning: comparison between pointer and integer
  140 |  int is_in_Uproc_address_space = (msg >= UPROCSTARTADDR && (msg + len) <= USERSTACKTOP);
      |                                                                        ^~
sysSupport.c:147:42: error: expected ‘;’ before ‘mutex_asid’
  147 |   SYSCALL(PASSEREN, (int)semaphore, 0, 0) // P(semaphore)
      |                                          ^
      |                                          ;
  148 |   mutex_asid = support_exc->sup_asid; //my asid is holding mutex using semaphores in support level
      |   ~~~~~~~~~~                              
sysSupport.c:152:37: warning: comparison between pointer and zero character constant [-Wpointer-compare]
  152 |   while (is_ready && count > 0 && s != EOS ) {
      |                                     ^~
sysSupport.c:152:35: note: did you mean to dereference the pointer?
  152 |   while (is_ready && count > 0 && s != EOS ) {
      |                                   ^
sysSupport.c:153:34: error: cast to union type from type not present in union
  153 |    devreg_t value = PRINTCHR | (((devreg_t)*s) << 8);
      |                                  ^
sysSupport.c:154:4: error: aggregate value used where an integer was expected
  154 |    status         = SYSCALL(DOIO, (int)command, (int)value, 0);
      |    ^~~~~~
sysSupport.c:154:21: error: incompatible types when assigning to type ‘devreg_t’ {aka ‘union devreg’} from type ‘int’
  154 |    status         = SYSCALL(DOIO, (int)command, (int)value, 0);
      |                     ^~~~~~~
sysSupport.c:155:15: error: invalid operands to binary != (have ‘devreg_t’ {aka ‘union devreg’} and ‘int’)
  155 |    if (status != READY) {
      |               ^~
sysSupport.c:156:25: error: incompatible types when assigning to type ‘unsigned int’ from type ‘devreg_t’ {aka ‘union devreg’}
  156 |     state_exc->reg_v0 = status;
      |                         ^~~~~~
sysSupport.c:163:42: error: expected ‘;’ before ‘}’ token
  163 |   SYSCALL(VERHOGEN, (int)semaphore, 0, 0) // V(semaphore)
      |                                          ^
      |                                          ;
  164 |  } else {
      |  ~                                        
sysSupport.c:169:31: error: expected ‘;’ before ‘SYSCALL’
  169 |    klog_print("address error")
      |                               ^
      |                               ;
  170 |   SYSCALL(TERMINATE, 0, 0, 0);
      |   ~~~~~~~                      
sysSupport.c:172:13: error: ‘count’ undeclared (first use in this function)
  172 |  return len-count; // num caratteri inviati
      |             ^~~~~
sysSupport.c:172:13: note: each undeclared identifier is reported only once for each function it appears in
sysSupport.c: In function ‘Write_Printer’:
sysSupport.c:194:14: warning: initialization of ‘char *’ from ‘unsigned int’ makes pointer from integer without a cast [-Wint-conversion]
  194 |  char* str = a1;
      |              ^~
sysSupport.c: In function ‘Write_Terminal’:
sysSupport.c:221:14: warning: initialization of ‘char *’ from ‘unsigned int’ makes pointer from integer without a cast [-Wint-conversion]
  221 |  char *str = a1;
      |              ^~
sysSupport.c: In function ‘write’:
sysSupport.c:174:1: warning: control reaches end of non-void function [-Wreturn-type]
  174 | }
      | ^
make: *** [<builtin>: sysSupport.o] Error 1

