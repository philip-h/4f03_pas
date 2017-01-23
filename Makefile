CFLAGS=-lpthread

pa1.x : pa1.c
	gcc -o pa1.x pa1.c $(CFLAGS)

clean:
	rm pa1.x
