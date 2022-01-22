cflags = -c
headers = listx.h pandos_const.h pandos_types.h
OBJ = PCB.o

all: app
	./app

PCB.o: PCB.c $(headers)
	gcc $(cflags) PCB.c

p1test.o: p1test.c $(headers)
	gcc $(cflags) p1test.c

app: $(OBJ)
	gcc $(OBJ) -o app

clean:
	rm *.o 
