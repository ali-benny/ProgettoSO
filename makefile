COMPILE = gcc -c
header = pandos_const.h pandos_types.h listx.h utils.h
OBJ = ASL.o PCB.o

all: app
	./app

PCB.o: PCB.c pcb.h $(header)
	$(COMPILE) PCB.c

ASL.o: ASL.c ASL.h pcb.h $(header)
	$(COMPILE) ASL.c

utils.o: utils.c utils.h pcb.h $(header)
	$(COMPILE) utils.c

app: $(OBJ)
	gcc $(OBJ) -o app

clean:
	rm *.o app