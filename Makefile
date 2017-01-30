CFLAGS=-lpthread -std=c++11

pa1.x : pa1.c
	g++ -o pa1.x pa1.c $(CFLAGS)

run: pa1.x
	./pa1.x 3 3 6 3 a b c
	# ./pa1.x 0 3 4 3 a b c

clean:
	rm pa1.x
