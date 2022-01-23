# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld

# uMPS3-related paths

# Simplistic search for the umps2 install. prefix. If you have umps2
# installed on some weird location, set UMPS2_DIR_PREFIX by hand.
ifneq ($(wildcard /usr/bin/umps3),)
    UMPS3_DIR_PREFIX = /usr
else
    UMPS3_DIR_PREFIX = /usr/local
endif

UMPS3_DATA_DIR = $(UMPS3_DIR_PREFIX)/share/umps3
UMPS3_INCLUDE_DIR = $(UMPS3_DIR_PREFIX)/include/umps3

# Compiler options
CFLAGS_LANG = -ffreestanding
CFLAGS_MIPS = -c -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32 -std=gnu11 -nostdlib
# rimosso -I/usr/local/include/umps2
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS3_INCLUDE_DIR) -Wall -O0

# Linker options
LDFLAGS = -G 0 -nostdlib -T $(UMPS3_DATA_DIR)/umpscore.ldscript

# Add the location of crt*.S to the search path
VPATH = $(UMPS3_DATA_DIR)

.PHONY : all clean

all : kernel.core.umps

kernel.core.umps : kernel
	umps3-elf2umps -k $<

# gli header non vanno compilati vanno sono inclusi!
#inserisco tutti i flag nella "macro" CFLAGS_MIPS" e chiamo la compilazione cono solo
# mipsel-linux-gnu-gcc $(GFLAGS_MIPS) nomefile.c

#per compilare
COMPILE = $(CC) $(CFLAGS)

headers = listx.h pandos_const.h pandos_types.h
OBJ = PCB.o

PCB.o: PCB.c $(headers)
    (COMPILE) PCB.c

p1test.o: p1test.c $(headers)
	$(COMPILE) p1test.c

kernel : PCB.o p1test.o
#kernel : main.o p1.5test_rikaya_v0.o crtso.o libumps.o
    $(LD) -o $@ $^ $(LDFLAGS)

# linking command:
#  mipsel-linux-gnu-ld -o
#  kernel LISTEPCB.o
#  /usr/local/lib/umps2/crtso.o
#  /usr/local/lib/umps2/libumps.o -nostdlib -T /
#  usr/local/share/umps2/umpscore.ldscript

clean :
    #su rikaia due anni fa era -rm e non rm e basta
    rm -f *.o kernel kernel.*.umps

# Pattern rule for assembly modules
%.o : %.S
    $(CC) $(CFLAGS) -c -o $@ $<