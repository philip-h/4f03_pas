CFLAGS=-lpthread -std=c++11

pa1.x : pa1.c
	g++ -o pa1.x pa1.c $(CFLAGS)

run: pa1.x
	./pa1.x 0 4 6 3 a b c

clean:
	rm pa1.x
