COMPILE = gcc -c
header = pandos_const.h pandos_types.h listx.h
OBJ = PCB.o

all: app
	./app

PCB.o: PCB.c PCB.h $(header)
	$(COMPILE) PCB.c

app: $(OBJ)
	gcc $(OBJ) -o app

clean:
	rm *.o app