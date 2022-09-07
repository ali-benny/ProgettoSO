#$Id: Makefile,v 1.2 2004/05/01 14:53:48 morsiani Exp morsiani $
# Makefile for mipsel-linux
#
ifneq ($(wildcard /usr/bin/umps3),)
	UMPS3_DIR_PREFIX = /usr
else
	UMPS3_DIR_PREFIX = /usr/local
endif

TEST = testers/

LIBDIR = $(UMPS3_DIR_PREFIX)/lib/umps3
INCDIR = $(UMPS3_DIR_PREFIX)/include/umps3
INCDIR_1 = $(INCDIR)/umps
SUPDIR = $(UMPS3_DIR_PREFIX)/share/umps3

TDEFS = $(TEST)/h/print.h $(TEST)/h/tconst.h $(INCDIR_1)/libumps.e $(TEST)/Makefile

# CFLAGS = -ffreestanding -ansi -c -mips1 -mabi=32 -mfp32 -mno-gpopt -G 0 -fno-pic -mno-abicalls -Wall
CFLAGS_LANG = -ffreestanding
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(INCDIR) -I$(INCDIR_1) -Wall -O0

#LDAOUTFLAGS = -G 0 -nostdlib -T $(SUPDIR)/umpsaout.ldscript
#LDCOREFLAGS =  -G 0 -nostdlib -T $(SUPDIR)/umpscore.ldscript

LDFLAGS = -G 0 -nostdlib -T $(SUPDIR)/umpscore.ldscript
VPATH = $(SUPDIR)

CC = mipsel-linux-gnu-gcc
LD = mipsel-linux-gnu-ld
#AS = mipsel-linux-gnu-as -KPIC

EF = umps3-elf2umps
UDEV = umps3-mkdev


PHASE2 = main.o syscall.o exceptions.o scheduler.o pcb.o asl.o klog/klog.o umps_utils/memcpy.o

PHASE3 = sysSupport.o vmSupport.o initProc.o

OBJ = $(PHASE2) $(PHASE3)

#main target
all: $(TEST)printerTest.umps $(TEST)strConcat.umps \
	$(TEST)fibEight.umps $(TEST)fibEleven.umps \
	$(TEST)terminalTest2.umps $(TEST)terminalTest3.umps $(TEST)terminalTest4.umps \
	$(TEST)terminalTest5.umps  \
	$(OBJ) kernel.core.umps
	echo "all done"

kernel.core.umps : kernel
	umps3-elf2umps -k $<
	echo "kernel.core.umps done"
 
kernel : $(OBJ) crtso.o libumps.o 
	$(LD) -o $@ $^ $(LDFLAGS)
	echo "kernel done"

%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.t *.umps

