cflags = -c
headers = listx.h pandos_const.h pandos_types.h
objects = PCB.o

PCB.o: PCB.c $(headers)
	gcc $(cflags) PCB.c

p1test.o: p1test.c $(headers)
	gcc $(cflags) p1test.c

all: PCB.o
	./PCB.o

clean:
	rm *.o 
