alibi@alibi-VirtualBox:~/Desktop/ProgettoSO/PHASE2$ make
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o main.o main.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o syscall.o syscall.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o exceptions.o exceptions.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o scheduler.o scheduler.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o pcb.o pcb.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o asl.o asl.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0   -c -o p2test.o p2test.c
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0 -c -o crtso.o /usr/share/umps3/crtso.S
mipsel-linux-gnu-gcc -ffreestanding -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 -I/usr/include/umps3/umps -Wall -O0 -c -o libumps.o /usr/share/umps3/libumps.S
mipsel-linux-gnu-ld -o kernel main.o syscall.o exceptions.o scheduler.o pcb.o asl.o p2test.o crtso.o libumps.o klog/klog.o umps_utils/memcpy.o -G 0 -nostdlib -T /usr/share/umps3/umpscore.ldscript
umps3-elf2umps -k kernel

