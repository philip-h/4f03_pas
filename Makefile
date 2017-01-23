CFLAGS=-lpthread -std=c++11

pa1.x : pa1.c
	g++ -o pa1.x pa1.c $(CFLAGS)

clean:
	rm pa1.x
