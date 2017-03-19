CFLAGS=-lpthread -fopenmp

pa3.x : pa3.c
	gcc -o $@ $^ $(CFLAGS)

run: pa3.x
	./$^ 3 3 6 3 a b c


clean:
	rm pa3.x
