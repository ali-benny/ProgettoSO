COMPILE = gcc -c
header = pandos_const.h pandos_types.h listx.h
OBJ = PCB.o

PCB.o: PCB.h $(header)
	$(COMPILE) PCB.c

app: $(OBJ)
	gcc $(OBJ) -o app

all: app
	./app

clean:
	rm *.o app