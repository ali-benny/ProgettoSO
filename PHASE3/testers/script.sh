mipsel-linux-gnu-gcc -ffreestanding -ansi -c -mips1 -mabi=32 -mfp32 -mno-gpopt -G 0 -fno-pic -mno-abicalls printerTest.c
mipsel-linux-gnu-gcc -ffreestanding -ansi -c -mips1 -mabi=32 -mfp32 -mno-gpopt -G 0 -fno-pic -mno-abicalls print.c
mipsel-linux-gnu-ld -G 0 -nostdlib -T /usr/share/umps3/umpsaout.ldscript /usr/lib/umps3/crti.o printerTest.o print.o /usr/lib/umps3/libumps.o -o printerTest.t
umps3-elf2umps -a printerTest.t
umps3-mkdev -f printerTest.umps printerTest.t.aout.umps
