COMPILE = gcc -c
header = pandos_const.h pandos_types.h listx.h
OBJ = PCB.o ASL.o

all: app
	./app

PCB.o: PCB.c pcb.h $(header)
	$(COMPILE) PCB.c

ASL.o: ASL.c ASL.h pcb.h $(header)
	$(COMPILE) ASL.c

app: $(OBJ)
	gcc $(OBJ) -o app

clean:
	rm *.o app